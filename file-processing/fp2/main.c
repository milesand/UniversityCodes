#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>



///////////////////////////////////////////////////////////////////////////////

// Hashing. 32-bit FNV-1a.

typedef struct Hasher {
    uint32_t state;
} Hasher;

void Hasher_init(Hasher *self);
void Hasher_consume_byte(Hasher *self, char byte);
void Hasher_consume_bytes(Hasher *self, const char *bytes, size_t length);
uint8_t Hasher_finish(Hasher *self);
uint8_t hash(const char *bytes, size_t length);
uint8_t get_low_bits(uint8_t byte, uint8_t number_of_bits);

void Hasher_init(Hasher *self) {
    self->state = 2166136261;
}

void Hasher_consume_byte(Hasher *self, char byte) {
    self->state ^= (uint32_t) byte;
    self->state *= 16777619; // Wrap-around is intentional.
}

void Hasher_consume_bytes(Hasher *self, const char *bytes, size_t length) {
    for (size_t i = 0; i < length; i += 1) {
        Hasher_consume_byte(self, bytes[i]);
    }
}

uint8_t Hasher_finish(Hasher *self) {
    return (uint8_t) (self->state & 255);
}

uint8_t hash(const char *bytes, size_t length) {
    Hasher h;
    Hasher_init(&h);
    Hasher_consume_bytes(&h, bytes, length);
    return Hasher_finish(&h);
}

uint8_t get_low_bits(uint8_t byte, uint8_t number_of_bits) {
    uint8_t ret = 0;
    for (uint8_t i = 0; i < number_of_bits; i += 1) {
        ret |= byte & (1 << i);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////////////

// Extendible hashing.

const uint8_t MAXIMUM_DEPTH = 8;

// Following length includes null terminator.
#define NAME_LEN 16
#define ADDR_LEN 52
#define MAJOR_LEN 28
#define PHONE_LEN 12

typedef uint8_t BucketIndex;

typedef struct Record {
    char name[NAME_LEN];
    char addr[ADDR_LEN];
    char major[MAJOR_LEN];
    char phone[PHONE_LEN];
} Record;

typedef struct Bucket {
    uint8_t depth;
    uint8_t count;
    Record records[3];
    // since we aren't building anything too serious, no padding.
} Bucket;

typedef struct Directory {
    uint8_t depth;
    BucketIndex buckets[256];
} Directory;

typedef struct ExtendibleHashFile {
    FILE *file;
    Directory *dir;
} ExtendibleHashFile;

uint8_t ExtendibleHashFile_create(ExtendibleHashFile *self, const char *pathname, uint8_t depth);
uint8_t ExtendibleHashFile_insert(ExtendibleHashFile *self, const Record *record);
uint8_t ExtendibleHashFile_close(ExtendibleHashFile *self);

uint8_t ExtendibleHashFile_create(ExtendibleHashFile *self, const char *pathname, uint8_t depth) {
    Directory *dir = malloc(sizeof(Directory));
    if (dir == NULL) {
        return 1;
    }
    dir->depth = depth;

    FILE *file = fopen(pathname, "w+");
    if (file == NULL) {
        free(dir);
        return 2;
    }
    
    Bucket bucket;
    memset(&bucket, 0, sizeof(Bucket));
    bucket.depth = depth;
    for (uint8_t i = 0; i < (1 << depth); i += 1) {
        dir->buckets[i] = (BucketIndex) i;
        if (fwrite(&bucket, sizeof(Bucket), 1, file) != 1) {
            fclose(file);
            free(dir);
            return 3;
        }
    }
    if (fflush(file) != 0) {
        return 4;
    }

    self->dir = dir;
    self->file = file;
    return 0;
}

uint8_t ExtendibleHashFile_insert(ExtendibleHashFile *self, const Record *record) {
    size_t len = strlen(&record->name[0]);
    uint8_t hashed = hash(&record->name[0], len);
    for (;;) {
        uint8_t pseudo_key = get_low_bits(hashed, self->dir->depth);
        BucketIndex index = self->dir->buckets[pseudo_key];
        Bucket bucket;

        if (fseek(self->file, index * sizeof(Bucket), SEEK_SET) != 0) {
            return 1;
        }
        if (fread(&bucket, sizeof(Bucket), 1, self->file) != 1) {
            return 1;
        }

        if (bucket.count < 3) {
            bucket.records[bucket.count] = *record;
            bucket.count += 1;
            if (fseek(self->file, index * sizeof(Bucket), SEEK_SET) != 0) {
                return 1;
            }
            if (fwrite(&bucket, sizeof(Bucket), 1, self->file) != 1) {
                return 1;
            }
            if (fflush(self->file) != 0) {
                return 1;
            }
            return 0;
        }

        // Target bucket is full, attempt resize
        if (bucket.depth == MAXIMUM_DEPTH) {
            return 2;
        }

        // Resize directory if necessary
        // since bucket_depth != MAX_DEPTH && bucket_depth == dir_depth,
        // dir_depth != MAX_DEPTH; thus resizable.
        if (bucket.depth == self->dir->depth) {
            uint8_t old_count = 1 << self->dir->depth;
            printf("Resizing directory from %d buckets to %d buckets\n", old_count, old_count << 1);
            for (uint8_t i = 0; i < old_count; i += 1) {
                self->dir->buckets[i + old_count] = self->dir->buckets[i];
            }
            self->dir->depth += 1;
        }
        uint8_t new_pseudo_key = get_low_bits(pseudo_key, bucket.depth) | (1 << bucket.depth);
        printf("Splitting bucket %d\n", get_low_bits(pseudo_key, bucket.depth));

        bucket.depth += 1;
        Bucket new_bucket;
        new_bucket.depth = bucket.depth;
        new_bucket.count = 0;
        uint8_t base = 0;
        for (uint8_t i = 0; i < 3; i += 1) {
            size_t len = strlen(&bucket.records[i].name[0]);
            uint8_t hashed = hash(&bucket.records[i].name[0], len);
            if (get_low_bits(hashed, bucket.depth) == new_pseudo_key) {
                // Move this record to new bucket
                new_bucket.records[new_bucket.count] = bucket.records[i];
                new_bucket.count += 1;
            } else {
                if (base != i) {
                    bucket.records[base] = bucket.records[i];
                }
                base += 1;
            }
        }
        bucket.count = base;
        memset(&bucket.records[base], 0, (3 - base) * sizeof(Record));
        memset(&new_bucket.records[new_bucket.count], 0, (3 - new_bucket.count) * sizeof(Record));

        if (fseek(self->file, index * sizeof(Bucket), SEEK_SET) != 0) {
            return 1;
        }
        if (fwrite(&bucket, sizeof(Bucket), 1, self->file) != 1) {
            return 1;
        }
        if (fflush(self->file) != 0) {
            return 1;
        }

        if (fseek(self->file, 0, SEEK_END) != 0) {
            return 1;
        }
        BucketIndex new_bucket_idx = ftell(self->file) / sizeof(Bucket);
        if (fwrite(&new_bucket, sizeof(Bucket), 1, self->file) != 1) {
            return 1;
        }
        if (fflush(self->file) != 0) {
            return 1;
        }
        for (uint8_t i = 0; i < (1 << (self->dir->depth - bucket.depth)); i += 1) {
            self->dir->buckets[(i << bucket.depth) | new_pseudo_key] = new_bucket_idx;
        }
    }
}

uint8_t ExtendibleHashFile_close(ExtendibleHashFile *self) {
    if (fseek(self->file, 0, SEEK_END) != 0) {
        return 1;
    }

    uint8_t count = 1 << self->dir->depth;
    if (fwrite(&self->dir->buckets, sizeof(BucketIndex), count, self->file) != count) {
        return 1;
    }
    if (fwrite(&self->dir->depth, sizeof(uint8_t), 1, self->file) != 1) {
        return 1;
    }
    if (fflush(self->file) != 0) {
        return 1;
    }
    if (fclose(self->file) != 0) {
        return 1;
    }
    free(self->dir);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

uint8_t read_field_until_comma(FILE *input, int *ch, char *arr, size_t len, const char *name) {
    for (size_t i = 0; i < len; i += 1) {
        if (*ch == EOF) {
            fputs("Unexpected EOF", stderr);
            return 1;
        }
        if (*ch == '\n') {
            fputs("Unexpected newline", stderr);
            *ch = fgetc(input);
            return 2;
        }
        if (*ch == ',') {
            return 0;
        }
        arr[i] = (char) *ch;
        *ch = fgetc(input);
    }
    fprintf(stderr, "%s too long\n", name);
    while (*ch != '\n' && *ch != EOF) {
        *ch = fgetc(input);
    }
    return 2;
}

int main() {
    FILE *input = fopen("input.csv", "r");
    if (input == NULL) {
        fputs("Error while opening input file", stderr);
        return 1;
    }
    Record record;
    memset(&record, 0, sizeof(Record));
    ExtendibleHashFile ehf;
    uint8_t res = ExtendibleHashFile_create(&ehf, "hash", 2);
    if (res != 0) {
        fprintf(stderr, "Error while creating hash file: %d\n", res);
        return 1;
    };

    int ch;

    for (;;) {
        memset(&record, 0, sizeof(Record));
        
        // Read first char in the line. If line is empty (EOF), break.
        ch = fgetc(input);
        if (ch == EOF) {
            break;
        }

        uint8_t res;

        res = read_field_until_comma(input, &ch, record.name, NAME_LEN - 1, "Name");
        if (res == 1) {
            ExtendibleHashFile_close(&ehf);
            fclose(input);
            return 1;
        }
        if (res == 2) {
            continue;
        }

        ch = fgetc(input);
        res = read_field_until_comma(input, &ch, record.addr, ADDR_LEN - 1, "Address");
        if (res == 1) {
            ExtendibleHashFile_close(&ehf);
            fclose(input);
            return 1;
        }
        if (res == 2) {
            continue;
        }

        ch = fgetc(input);
        res = read_field_until_comma(input, &ch, record.major, MAJOR_LEN, "Major");
        if (res == 1) {
            ExtendibleHashFile_close(&ehf);
            fclose(input);
            return 1;
        }
        if (res == 2) {
            continue;
        }

        ch = fgetc(input);
        size_t i = 0;
        for (;;) {
            if (ch == EOF || ch == '\n') {
                res = 0;
                break;
            }
            if (ch == ',') {
                fputs("Unexpected comma", stderr);
                do {
                    ch = fgetc(input);
                } while (ch != EOF && ch != '\n');
                res = 2;
                break;
            }
            if (i == PHONE_LEN - 1) {
                fputs("Phone number too long", stderr);
                do {
                    ch = fgetc(input);
                } while (ch != EOF && ch != '\n');
                res = 2;
                break;
            }
            record.phone[i] = (char) ch;
            ch = fgetc(input);
            i += 1;
        }
        if (res == 2) {
            continue;
        }

        if (ExtendibleHashFile_insert(&ehf, &record) != 0) {
            fputs("Error while inserting to file", stderr);
            ExtendibleHashFile_close(&ehf); // ehf is probably now corrupt and bad.
            fclose(input);
            return 1;
        }
    }

    ExtendibleHashFile_close(&ehf);
    fclose(input);
    return 0;
}
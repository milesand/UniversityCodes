#define _GNU_SOURCE

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "common/common.h"
#include "sort/sort.h"

typedef enum {ok, sort_fail, invalid_algorithm} error;

double solve(ulong *arr, ulong len, error *e);

bool empty_str(char *str);
void init_shuffle(ulong *arr, ulong len);
int is_sorted(ulong *arr, ulong len);

ulong random_ul();
ulong random_ul_range(ulong max);

// Usage: (bin name) [-n <Length>] [-s <Seed>] [-i <Iterations>]

int main(int argc, char** argv) {
    ulong len = 131072; // length of array to be sorted
    unsigned int seed = time(NULL); // seed for random number to be used
    ulong iterations = 16;

    // Parse arguments.
    // Skip argp = 0, since arguments start at position 1.
    for (int argp = 1; argp < argc; argp += 1) {
        if (strcmp(argv[argp], "-n") == 0) {
            argp += 1;
            if (argp == argc) {
                puts("Error: Missing value after -n");
                return 1;
            }
            char *endptr;
            len = strtoul(argv[argp], &endptr, 0);
            // when *endptr is not 0, then there was some invalid character
            // in the input (now pointed by endptr). If *endptr is 0 and the
            // returned value is 0, input might have been empty which is invalid.
            if (*endptr != 0 || (len == 0 && empty_str(argv[argp]))) {
                puts("Error: Invalid value for -n");
                return 1;
            }
            // or maybe the input was too big.
            if (errno == ERANGE) {
                puts("Error: Out-of-range value for -n");
                return 1;
            }
        } else if (strcmp(argv[argp], "-s") == 0) {
            argp += 1;
            if (argp == argc) {
                puts("Error: Missing value after -s");
                return 1;
            }
            char *endptr;
            // Since seed should be uint and not ulong but all standard uint
            // parsing functions give ulong, we store the result and check
            // if it fits.
            ulong seed_long = strtoul(argv[argp], &endptr, 0);

            // Covered in -s case, see there
            if (*endptr != 0 || (seed_long == 0 && empty_str(argv[argp]))) {
                puts("Error: Invalid value for -s");
                return 1;
            }
            if (errno == ERANGE || seed_long > UINT_MAX) {
                puts("Error: Out-of-range value for -s");
                return 1;
            }

            seed = seed_long;
        } else if (strcmp(argv[argp], "-i") == 0) {
            argp += 1;
            if (argp == argc) {
                puts("Error: Missing value after -i");
                return 1;
            }
            char *endptr;
            iterations = strtoul(argv[argp], &endptr, 0);

            // Covered in -s case, see there
            if (*endptr != 0 || (iterations == 0 && empty_str(argv[argp]))) {
                puts("Error: Invalid value for -i");
                return 1;
            }
            if (errno == ERANGE) {
                puts("Error: Out-of-range value for -i");
                return 1;
            }
        } else {
            printf("Error: Unknown flag: %s\n", argv[argp]);
            return 1;
        }
    }

    srand(seed);

    // Initialize our array and shuffle it.
    ulong *arr = malloc(sizeof(ulong) * len);
    if (arr == NULL) {
        puts("Error: Memory allocation failed");
        return 1;
    }

    printf("Running with len = %lu, seed = %u, iterations = %lu\n", len, seed, iterations);

    double time_total = 0.0;

    // Get number of digits in iterations, for formatting.
    int len_of_iter = 1;
    ulong iterations_temp = iterations;
    while (iterations_temp >= 10) {
        len_of_iter += 1;
        iterations_temp /= 10;
    }
    
    for (ulong i = 1; i <= iterations; i += 1) {
        error e = ok;
        double time_elapsed = solve(arr, len, &e);
        if (!isnan(time_elapsed)) {
            printf("#%*lu: Sort complete in %lf seconds\n", len_of_iter, i, time_elapsed);
            time_total += time_elapsed;
        } else {
            // print message for invalid_algorithm case only.
            // sort() is expected to print its own output message on failure.
            if (e == invalid_algorithm) {
                puts("Error: Invalid sorting algorithm");
            }
            return 1;
        }
    }
    printf("Average time: %lf seconds\n", time_total / (double)iterations);

    return 0;
}

// shuffle and sort the given array, then return elapsed time for the sort.
// If error occurs, NaN is returned and e is set.
double solve(ulong *arr, ulong len, error *e) {
    init_shuffle(arr, len);

    // Run and time sort.
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    if (sort(arr, len) == 0) {
        *e = sort_fail;
        return nan("");
    }
    clock_gettime(CLOCK_REALTIME, &end);

    // Check if the array is actually sorted now.
    if (!is_sorted(arr, len)) {
        *e = invalid_algorithm;
        return nan("");
    }

    double time_elapsed = (end.tv_nsec / 1000000000.0 + end.tv_sec) - (start.tv_nsec / 1000000000.0 + start.tv_sec);
    return time_elapsed;
}

// Check if given string is composed entirely of whitespace characters.
// Return true for empty string.
bool empty_str(char *str) {
    while (*str != 0) {
        if (!isspace((int)(*str))) {
            return false;
        }
        str += 1;
    }
    return true;
}

// Initialize the given possibly uninitialized array with a shuffled sequence
// of [0..len) using the "inside-out" variant of Fisher-Yates described in the
// Wikipedia.
void init_shuffle(ulong *arr, ulong len) {
    for (ulong i = 0; i < len; i += 1) {
        // arr[0] to arr[i - 1] is initialized, arr[i] is not
        ulong j = random_ul_range(i);
        if (i != j) {
            // Since 0 <= j < i, arr[j] must be initialized thus safe to read.
            // if j != i, arr[i] is initialized here
            arr[i] = arr[j];
        }
        // if j == i, arr[i] is initialized here
        arr[j] = i;
    }
}

// Generate random ulong.
// This implementation relies on RAND_MAX = 2147483647 and ULONG_MAX = 18446744073709551615,
// or, more properly, RAND_MAX = 2^31 - 1 and ULONG_MAX = 2^64 - 1.
// In other words, this is probably not portable. Fuck portability.
ulong random_ul() {
    ulong result = 0;
    result |= (ulong)rand() << 33;
    result |= (ulong)rand() << 2;
    result |= (ulong)rand() & 3;
    return result;
}

// Generate random ulong within [0..max].
ulong random_ul_range(ulong max) {
    if (max == ULONG_MAX) {
        return random_ul();
    }
    ulong result;
    do {
        result = random_ul();
    } while (result >= ULONG_MAX - (ULONG_MAX % (max + 1)));
    return result % (max + 1); 
}

// Check whether the given array is sorted.
int is_sorted(ulong *arr, ulong len) {
    for (unsigned int i = 0; i < len - 1; i += 1) {
        if (arr[i] > arr[i + 1]) {
            return 0;
        }
    }
    return 1;
}
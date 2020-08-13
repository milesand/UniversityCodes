#include <stdio.h>
#include <string.h>

void swap(int *arr, unsigned int i, unsigned int j);
void print_arr(int *arr, unsigned int len);

void selection_sort(int *arr, unsigned int len);
void insertion_sort(int *arr, unsigned int len);
void bubble_sort(int *arr, unsigned int len);

void quicksort(int *arr, unsigned int len);
void mergesort(int *arr, unsigned int len);
void heapsort(int *arr, unsigned int len);

int main() {
    int arr1[8] = {7, 4, 9, 6, 3, 8, 7, 5};
    printf("Before sorting: ");
    print_arr(arr1, 8);
    putchar('\n');

    selection_sort(arr1, 8);
    insertion_sort(arr1, 8);
    bubble_sort(arr1, 8);

    putchar('\n');

    int arr2[8] = {11, 49, 92, 55, 38, 82, 72, 53};
    printf("Before sorting: ");
    print_arr(arr2, 8);
    putchar('\n');

    quicksort(arr2, 8);
    mergesort(arr2, 8);
    // heapsort(arr2, 8);
}

void swap(int *arr, unsigned int i, unsigned int j) {
    if (i == j) {
        return;
    }
    arr[i] ^= arr[j];
    arr[j] ^= arr[i];
    arr[i] ^= arr[j];
}

void print_arr(int *arr, unsigned int len) {
    putchar('{');
    char first = 1;
    for (int i = 0; i < len; i += 1) {
        if (first) {
            first = 0;
        } else {
            printf(", ");
        }
        printf("%d", arr[i]);
    }
    putchar('}');
}

void selection_sort(int *arr, unsigned int len) {
    int array[len];
    memcpy(array, arr, len * sizeof(int));
    puts("Selection Sort:");
    for (int i = 0; i < len - 1; i += 1) {
        int min_i = i;
        for (int j = i + 1; j < len; j += 1) {
            if (array[j] < array[min_i]) {
                min_i = j;
            }
        }
        swap(array, i, min_i);
        printf("    %d: ", i + 1);
        print_arr(array, 8);
        putchar('\n');
    }
}

void insertion_sort(int *arr, unsigned int len) {
    int array[len];
    memcpy(array, arr, len * sizeof(int));
    puts("Insertion Sort:");
    for (int i = 1; i < len; i += 1) {
        int v = array[i];
        int j = i;
        while (j > 0 && array[j - 1] > v) {
            array[j] = array[j - 1];
            j -= 1;
        }
        array[j] = v;
        printf("    %d: ", i);
        print_arr(array, 8);
        putchar('\n');
    }
}

void bubble_sort(int *arr, unsigned int len) {
    int array[len];
    int swapped;
    memcpy(array, arr, len * sizeof(int));
    puts("Bubble Sort:");
    for (int i = len; i > 1; i -= 1) {
        swapped = 0;
        for (int j = 1; j < i; j += 1) {
            if (array[j-1] > array[j]) {
                swap(array, j-1, j);
                swapped = 1;
            }
        }
        printf("    %d: ", len - i + 1);
        print_arr(array, 8);
        if (!swapped) {
            puts(" (no swap occured)");
            break;
        }
        putchar('\n');
    }
}


void _quicksort(int *array, unsigned int start, unsigned int end, unsigned int len) {
    for (;;) {
        if (end - start < 2) {
            printf("    [%d, %d) is trivially sorted\n", start, end);
            return;
        }
        int pivot = array[start];
        int lo = start + 1;
        int hi = end - 1;
        for (;;) {
            while (lo < end && array[lo] < pivot) {
                lo += 1;
            }
            while (hi > start && array[hi] > pivot) {
                hi -= 1;
            }
            if (lo < hi) {
                swap(array, lo, hi);
            } else {
                break;
            }
        }
        swap(array, start, hi);

        printf("    [%d, %d) sorted with pivot %d; Array: ", start, end, pivot);
        print_arr(array, len);
        putchar('\n');

        if (hi - start < end - hi - 1) {
            _quicksort(array, start, hi, len);
            start = hi + 1;
        } else {
            _quicksort(array, hi + 1, end, len);
            end = hi;
        }
    }
}

void quicksort(int *arr, unsigned int len) {
    int array[len];
    memcpy(array, arr, len * sizeof(int));
    puts("Quicksort:");
    _quicksort(array, 0, len, len);
}


void _merge(int *src, int *dst, unsigned int start, unsigned  int mid, unsigned int end) {
    int left = start;
    int right = mid;
    for (int i = start; i < end; i += 1) {
        if (left >= mid || (right < end && src[right] < src[left])) {
            dst[i] = src[right];
            right += 1;
        } else {
            dst[i] = src[left];
            left += 1;
        }
    }
}

void _mergesort(int *src, int *dst, unsigned int start, unsigned int end) {
    if (end - start < 2) {
        return;
    }
    int mid = start + (end - start) / 2;

    printf("    Splitting [%d, %d) to [%d, %d): ", start, end, start, mid);
    print_arr(src + start, mid - start);
    printf(" and [%d, %d): ", mid, end);
    print_arr(src + mid, end - mid);
    putchar('\n');

    _mergesort(dst, src, start, mid);
    _mergesort(dst, src, mid, end);
    _merge(src, dst, start, mid, end);

    printf("    Merged [%d, %d) and [%d, %d): ", start, mid, mid, end);
    print_arr(dst + start, end - start);
    putchar('\n');
}

void mergesort(int* arr, unsigned int len) {
    int array[len];
    int buffer[len];
    memcpy(array, arr, len * sizeof(int));
    memcpy(buffer, arr, len * sizeof(int));
    puts("Mergesort:");
    _mergesort(buffer, array, 0, len);
}


void sink(int *array, unsigned int idx, unsigned int len) {
    unsigned int hole = idx;
    int value = array[idx];
    for (;;) {
        unsigned int left_idx = 2 * hole + 1;
        unsigned int right_idx = 2 * hole + 2;
        if (right_idx < len) {
            int left = array[left_idx];
            int right = array[right_idx];
            if (left > right && left > value) {
                array[hole] = left;
                hole = left_idx;
            } else if (right > value) {
                array[hole] = right;
                hole = right_idx;
            } else {
                break;
            }
        } else if (left_idx < len) {
            int left = array[left_idx];
            if (left > value) {
                array[hole] = left;
                hole = left_idx;
            } else {
                break;
            }
        } else {
            break;
        }
    }
    array[hole] = value;
}

void heapsort(int *arr, unsigned int len) {
    int array[len];
    memcpy(array, arr, len * sizeof(int));
    puts("Heapsort:");

    unsigned int i = (len - 1) / 2;
    for (;;) {
        sink(array, i, len);
        if (i == 0) {
            break;
        }
        i -= 1;
    }
    printf("    Heapified: ");
    print_arr(array, len);
    putchar('\n');

    for (i = len - 1; i != 0; i -= 1) {
        swap(array, 0, i);
        sink(array, 0, i);
        printf("    %d: ", len - i);
        print_arr(array, len);
        putchar('\n');
    }
}
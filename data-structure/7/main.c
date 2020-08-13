#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>

void print_arr(int *arr, unsigned int len);
void quicksort(int *arr, unsigned int len);

int main() {
    int arr[50];
    srand(time(NULL));
    for (unsigned int i = 0; i < 50; i += 1) {
        arr[i] = rand() % 1000 + 1;
    }
    printf("Before sorting:\n    ");
    print_arr(arr, 50);
    putchar('\n');

    quicksort(arr, 50);

    printf("After sorting:\n    ");
    print_arr(arr, 50);
    putchar('\n');
}

void print_arr(int *arr, unsigned int len) {
    putchar('{');
    for (unsigned int i = 0; i < len; i += 1) {
        if (i != 0) {
            printf(", %d", arr[i]);
        } else {
            printf("%d", arr[i]);
        }
    }
    putchar('}');
}

void swap(int *arr, unsigned int i, unsigned int j) {
    if (i == j) {
        return;
    }
    arr[i] ^= arr[j];
    arr[j] ^= arr[i];
    arr[i] ^= arr[j];
}

void _quicksort(int *arr, unsigned int start, unsigned int end) {
    while (start != UINT_MAX && end > start + 1) {
        int pivot = arr[start];
        int lo = start;
        int hi = end;
        for (;;) {
            do {
                lo += 1;
            } while (lo < end && arr[lo] < pivot);
            do {
                hi -= 1;
            } while (hi > start && arr[hi] > pivot);
            if (lo > hi) {
                break;
            }
            swap(arr, lo, hi);
        }
        swap(arr, start, hi);
        if (hi - start > end - hi - 1) {
            _quicksort(arr, hi + 1, end);
            end = hi;
        } else {
            _quicksort(arr, start, hi);
            start = hi + 1;
        }
    }
}

void quicksort(int *arr, unsigned int len) {
    _quicksort(arr, 0, len);
}
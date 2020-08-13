#include "sort.h"

#include <stdbool.h>
#include "../common/common.h"

ulong pick_pivot(ulong *arr, ulong len);
ulong partition(ulong *arr, ulong len, ulong pivot);

bool sort(ulong *arr, ulong len) {
    while (len > 1) {
        ulong pivot = pick_pivot(arr, len);
        ulong mid = partition(arr, len, pivot);
        if (mid + 1 < len / 2) {
            sort(arr, mid + 1);
            arr = &arr[mid + 1];
            len = len - mid - 1;
        } else {
            sort(&arr[mid + 1], len - mid - 1);
            len = mid + 1;
        }
    }
    return true;
}

ulong pick_pivot(ulong *arr, ulong len) {
    return arr[0];
}

ulong partition(ulong *arr, ulong len, ulong pivot) {
    ulong i = 0;
    ulong j = len - 1;
    for (;;) {
        while (arr[i] < pivot) {
            i += 1;
        }
        while (arr[j] > pivot) {
            j -= 1;
        }
        if (i >= j) {
            return j;
        }
        swap(&arr[i], &arr[j]);
        i += 1;
        j -= 1;
    }
}
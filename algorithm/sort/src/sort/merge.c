#include "sort.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "../common/common.h"

void sort_into(ulong *src, ulong len, ulong *dst);
void merge(ulong *arr, ulong len, ulong mid, ulong *buf);

bool sort(ulong *arr, ulong len) {
    if (len < 2) {
        return true;
    }
    ulong half_ceil = (len / 2) + (len & 1);
    ulong *buf = malloc(sizeof(ulong) * half_ceil);
    if (buf == NULL) {
        return false;
    }
    sort_into(&arr[len / 2], half_ceil, buf);
    sort_into(arr, len / 2, &arr[half_ceil]);
    merge(arr, len, half_ceil, buf);
    return true;
}

void sort_into(ulong *src, ulong len, ulong *dst) {
    if (len == 1) {
        *dst = *src;
        return;
    }
    ulong half_ceil = (len / 2) + (len & 1);
    sort_into(&src[len / 2], half_ceil, &dst[len / 2]);
    sort_into(src, len / 2, &src[half_ceil]);
    merge(dst, len, len / 2, &src[half_ceil]);
}

void merge(ulong *arr, ulong len, ulong mid, ulong *buf) {
    ulong i = 0;
    ulong j = 0;
    ulong buf_len = mid;
    while (mid != len && j != buf_len) {
        if (arr[mid] < buf[j]) {
            arr[i] = arr[mid];
            mid += 1;
        } else {
            arr[i] = buf[j];
            j += 1;
        }
        i += 1;
    }
    if (mid == len) {
        memcpy(&arr[i], &buf[j], sizeof(ulong) * (len - i));
    }
}
#include "sort.h"

#include <stdbool.h>
#include "../common/common.h"

void build_max_heap(ulong *arr, ulong len);
void sift_down(ulong *arr, ulong idx, ulong len);

bool sort(ulong *arr, ulong len) {
    if (len < 2) {
        return true;
    }
    build_max_heap(arr, len);
    for (ulong top = len - 1; top != 0; top -= 1) {
        swap(&arr[0], &arr[top]);
        sift_down(arr, 0, top);
    }
    return true;
}

void build_max_heap(ulong *arr, ulong len) {
    if (len < 2) {
        return;
    }
    ulong idx = (len / 2) - 1;
    for (;;) {
        sift_down(arr, idx, len);
        if (idx == 0) {
            break;
        }
        idx -= 1;
    }
    return;
}

void sift_down(ulong *arr, ulong idx, ulong len) {
    for (;;) {
        ulong largest = idx;
        ulong left = 2 * idx + 1;
        ulong right = 2 * idx + 2;
        if (left < len && arr[largest] < arr[left]) {
            largest = left;
        }
        if (right < len && arr[largest] < arr[right]) {
            largest = right;
        }
        if (largest == idx) {
            break;
        }
        swap(&arr[idx], &arr[largest]);
        idx = largest;
    }
}
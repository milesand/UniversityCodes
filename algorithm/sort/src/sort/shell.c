#include "sort.h"

#include <stdbool.h>
#include "../common/common.h"

bool sort(ulong *arr, ulong len) {
    for (ulong gap = len / 2; gap != 0; gap /= 2) {
        for (ulong base = 0; base < gap; base += 1) {
            for (ulong i = base; i < len; i += gap) {
                for (ulong j = i; j != base; j -= gap) {
                    if (arr[j - gap] <= arr[j]) {
                        break;
                    }
                    swap(&arr[j - gap], &arr[j]);
                }
            }
        }
    }
    return true;
}
#include "sort.h"

#include <stdbool.h>
#include "../common/common.h"

bool sort(ulong *arr, ulong len) {
    for (ulong i = 1; i < len; i += 1) {
        for (ulong j = i; j != 0; j -= 1) {
            if (arr[j - 1] <= arr[j]) {
                break;
            }
            swap(&arr[j - 1], &arr[j]);
        }
    }
    return true;
}
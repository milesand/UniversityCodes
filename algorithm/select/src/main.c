#include "./qselect.h"
#include "./common/common.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

void init_shuffle(ulong *arr, ulong len);
ulong random_ul();
ulong random_ul_range(ulong max);

int main() {
    ulong arr[1024];
    init_shuffle(arr, 1024);

    for (ulong i = 0; i < 1024; i += 1) {
        if (*qselect(arr, 1024, i) != i) {
            printf("Error: i = %ld\n", i);
        }
    }

    printf("Done\n");

    return 0;
}

void init_shuffle(ulong *arr, ulong len) {
    for (ulong i = 0; i < len; i += 1) {
        ulong j = random_ul_range(i);
        if (i != j) {
            arr[i] = arr[j];
        }
        arr[j] = i;
    }
}

ulong random_ul() {
    ulong result = 0;
    result |= (ulong)rand() << 33;
    result |= (ulong)rand() << 2;
    result |= (ulong)rand() & 3;
    return result;
}

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
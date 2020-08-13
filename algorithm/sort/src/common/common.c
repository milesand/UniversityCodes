#include "common.h"

void swap(ulong *x, ulong *y) {
    if (x == y) {
        return;
    }
    *x ^= *y;
    *y ^= *x;
    *x ^= *y;
    return;
}
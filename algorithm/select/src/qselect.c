#include "./qselect.h"

#include "./common/common.h"
#include <stddef.h>

ulong *qselect_inner(ulong *arr, ulong len, ulong nth);
ulong partition(ulong *arr, ulong len, ulong pivot_idx);
void process_full_group(ulong *arr, ulong len, ulong group, ulong gap);
ulong median_remaining_3(ulong *arr, ulong len);
ulong median_remaining_4(ulong *arr, ulong len);


ulong *qselect(ulong *arr, ulong len, ulong nth) {
    if (len <= nth) {
        return NULL;
    }
    return qselect_inner(arr, len, nth);
}

ulong *qselect_inner(ulong *arr, ulong len, ulong nth) {
    for (;;) {
        if (len == 1 && nth == 0) {
            return arr;
        }
        ulong full_groups = len / 5;
        for (ulong group = 0; group < full_groups; group += 1) {
            process_full_group(arr, len, group, full_groups);
        }

        ulong m = 2 * full_groups;
        ulong m_len = full_groups;
        ulong median;

        switch(len % 5) {
        case 0:
            break;
        case 1:
        case 2:
            swap(&arr[m + m_len], &arr[len - 1]);
            m_len += 1;
            break;
        case 3:
            median = median_remaining_3(arr, len);
            if (len != 3) {
                swap(&arr[m + m_len], &arr[median]);
            } else {
                swap(&arr[1], &arr[median]);
                m = 1;
            }
            m_len += 1;
            break;
        case 4:
            median = median_remaining_4(arr, len);
            if (len != 4) {
                swap(&arr[m + m_len], &arr[median]);
            } else {
                swap(&arr[1], &arr[median]);
                m = 1;
            }
            m_len += 1;
            break;
        default:
            // WTF
            break;
        }

        ulong pivot_idx = qselect_inner(&arr[m], m_len, (m_len - 1) / 2) - arr;
        ulong mid = partition(arr, len, pivot_idx);

        if (nth < mid) {
            len = mid;
        } else {
            arr = &arr[mid];
            len -= mid;
            nth -= mid;
        }
    }
}

ulong partition(ulong *arr, ulong len, ulong pivot_idx) {
    ulong i = 0;
    ulong j = len - 1;
    for (;;) {
        while (arr[i] < arr[pivot_idx]) {
            i += 1;
        }
        while (arr[pivot_idx] < arr[j]) {
            j -= 1;
        }
        if (j <= i) {
            return j + 1;
        }
        swap(&arr[i], &arr[j]);
        if (pivot_idx == i) {
            pivot_idx = j;
        } else if (pivot_idx == j) {
            pivot_idx = i;
        }
        i += 1;
        j -= 1;
    }
}

void process_full_group(ulong *arr, ulong len, ulong group, ulong gap) {
    ulong idx[5];
    for (int i = 0; i < 5; i += 1) {
        idx[i] = group + i * gap;
    }
    if (arr[idx[0]] > arr[idx[3]]) {
        swap(&arr[idx[0]], &arr[idx[3]]);
    }
    if (arr[idx[1]] > arr[idx[4]]) {
        swap(&arr[idx[1]], &arr[idx[4]]);
    }
    if (arr[idx[3]] > arr[idx[4]]) {
        swap(&arr[idx[0]], &arr[idx[1]]);
        swap(&arr[idx[3]], &arr[idx[4]]);
    }
    if (arr[idx[2]] < arr[idx[1]]) {
        if (arr[idx[1]] < arr[idx[3]]) {
            if (arr[idx[0]] < arr[idx[1]]) {
                swap(&arr[idx[1]], &arr[idx[2]]);
            } else {
                swap(&arr[idx[0]], &arr[idx[2]]);
            }
        } else {
            if (arr[idx[2]] < arr[idx[3]]) {
                swap(&arr[idx[3]], &arr[idx[2]]);
            }
        }
    } else {
        if (arr[idx[3]] < arr[idx[2]]) {
            if (arr[idx[1]] < arr[idx[3]]) {
                swap(&arr[idx[3]], &arr[idx[2]]);
            } else {
                swap(&arr[idx[1]], &arr[idx[2]]);
            }
        } else {
            if (arr[idx[2]] < arr[idx[0]]) {
                swap(&arr[idx[0]], &arr[idx[2]]);
            }
        }
    }
}

ulong median_remaining_3(ulong *arr, ulong len) {
    if (arr[len - 2] > arr[len - 1]) {
        swap(&arr[len - 2], &arr[len - 1]);
    }
    if (arr[len - 3] < arr[len - 2]) {
        return len - 2;
    } else if (arr[len - 3] < arr[len - 1]) {
        return len - 3;
    } else {
        return len - 1;
    }
}

ulong median_remaining_4(ulong *arr, ulong len) {
    if (arr[len - 4] > arr[len - 2]) {
        swap(&arr[len - 4], &arr[len - 2]);
    }
    if (arr[len - 3] > arr[len - 1]) {
        swap(&arr[len - 3], &arr[len - 1]);
    }

    if (arr[len - 4] < arr[len - 3]) {
        if (arr[len - 2] < arr[len - 3]) {
            return len - 2;
        } else {
            return len - 3;
        }
    } else {
        if (arr[len - 1] < arr[len - 4]) {
            return len - 1;
        } else {
            return len - 4;
        }
    }
}
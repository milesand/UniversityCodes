// This is a quick hack-and-slash implementation, without fancy type wrappers
// or things like that. This is because most of my time was spent in writing
// PHP, creating an AWS EC2 instance, and figuring out how to create docker
// image to properly deploy the code written in PHP to my teammates and to
// the aforementioned EC2 instance. My teammates are not exactly the best
// workers in the world, and our professor wants us to have a gigantic dream
// or something. Anyways, here we go.

#include <limits.h>
#include <stdio.h>

#define INF UINT_MAX
#define NONE UINT_MAX

typedef struct {
    unsigned int from;
    unsigned int cost;
} Pair;

typedef struct {
    unsigned int vertex;
    Pair pair;
} HeapEntry;

int main() {
    char *cities[8] = {
        "Los Angeles", "San francisco", "Denver", "Chicago",
        "Boston", "New york", "Miami", "New orleans"
    };
    // There are 8 vertices, and the edges are directed.
    // So, 64 potential edges.
    unsigned int graph[64];
    for (unsigned int i = 0; i < 64; i += 1) {
        graph[i] = INF;
    }
    graph[1 * 8 + 0] = 300;
    graph[2 * 8 + 0] = 1000;
    graph[2 * 8 + 1] = 800;
    graph[3 * 8 + 2] = 1200;
    graph[4 * 8 + 3] = 1500;
    graph[4 * 8 + 5] = 250;
    graph[5 * 8 + 3] = 1000;
    graph[5 * 8 + 6] = 900;
    graph[5 * 8 + 7] = 1400;
    graph[6 * 8 + 7] = 1000;
    graph[7 * 8 + 0] = 1700;

    HeapEntry heap[8];
    // index of vertex in the heap.
    unsigned int vertex_loc[8];
    for (unsigned int i = 0; i < 8; i += 1) {
        heap[i].vertex = i;
        heap[i].pair.from = NONE;
        heap[i].pair.cost = INF;
        vertex_loc[i] = i;
    }

    // We start from vertex 4, that is Boston.
    heap[0].vertex = 4;
    heap[0].pair.cost = 0;
    vertex_loc[4] = 0; 
    heap[4].vertex = 0;
    vertex_loc[0] = 4;

    Pair result[8];
    for (unsigned int i = 0; i < 8; i += 1) {
        result[i].from = NONE;
        result[i].cost = INF;
    }
    
    unsigned int heap_len = 8;
    while (heap_len != 0) {
        // remove an element from heap and keep invariants
        HeapEntry chosen = heap[0];
        if (chosen.pair.cost == INF) {
            break;
        }
        result[chosen.vertex] = chosen.pair;

        heap_len -= 1;
        HeapEntry tmp = heap[heap_len];
        unsigned int idx = 0;
        while (2 * idx + 1 < heap_len) {
            unsigned int left_idx = 2 * idx + 1;
            unsigned int right_idx = 2 * idx + 2;
            HeapEntry left = heap[left_idx];
            HeapEntry right;
            if (right_idx < heap_len) {
                right = heap[right_idx];
            } else {
                right.pair.cost = INF;
            }
            unsigned int child_idx;
            HeapEntry child;
            if (right.pair.cost < left.pair.cost) {
                child_idx = right_idx;
                child = right;
            } else {
                child_idx = left_idx;
                child = left;
            }
            if (child.pair.cost < tmp.pair.cost) {
                heap[idx] = child;
                vertex_loc[child.vertex] = idx;
                idx = child_idx;
            } else {
                break;
            }
        }
        heap[idx] = tmp;
        vertex_loc[tmp.vertex] = idx;

        for (unsigned int i = 0; i < 8; i += 1) {
            unsigned int edge = 8 * chosen.vertex + i;
            if (graph[edge] == INF) {
                continue;
            }
            unsigned int idx = vertex_loc[i];
            unsigned int old_cost = heap[idx].pair.cost;
            unsigned int new_cost = chosen.pair.cost + graph[edge];
            if (new_cost >= old_cost) {
                continue;
            }
            while (idx != 0) {
                unsigned int parent_idx = (idx - 1) / 2;
                HeapEntry parent = heap[parent_idx];
                if (parent.pair.cost <= new_cost) {
                    break;
                }
                heap[idx] = parent;
                vertex_loc[parent.vertex] = idx;
                idx = parent_idx;
            }
            heap[idx].vertex = i;
            heap[idx].pair.from = chosen.vertex;
            heap[idx].pair.cost = new_cost;
            vertex_loc[i] = idx;
        }
    }

    unsigned int path[8];
    for (unsigned int i = 0; i < 8; i += 1) {
        path[0] = i;
        char *city = cities[i];
        unsigned int path_len = 1;
        unsigned int cost = result[i].cost;
        unsigned int from = result[i].from;
        while (from != NONE) {
            path[path_len] = from;
            path_len += 1;
            from = result[from].from;
        }
        path_len -= 1;
        printf("City #%u %s; Cost: %u, Path: %s", i + 1, city, cost, cities[path[path_len]]);
        while (path_len != 0) {
            path_len -= 1;
            printf(" => %s", cities[path[path_len]]);
        }
        putchar('\n');
    }
}
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

////////
// Rc //
////////

typedef struct RcInner {
    unsigned int count;
    struct RcInner *pre;
    unsigned int idx;
} RcInner;

// A reference-counted smart pointer.
typedef RcInner * Rc;

// Create new Rc.
// Returns NULL on allocation error.
Rc Rc_new(Rc pre, unsigned int idx) {
    Rc rc = (Rc) malloc(sizeof(RcInner));
    if (rc == NULL) {
        return NULL;
    }
    rc->count = 1;
    rc->pre = pre;
    rc->idx = idx;
    return rc;
}

// Increment the counter for this Rc.
Rc Rc_clone(Rc rc) {
    rc->count += 1;
    return rc;
}

// Destroy this Rc.
// This decrements the counter for this Rc. If the counter hits 0,
// the memory is freed and its contents are also destroyed.
void Rc_drop(Rc rc) {
    for (;;) {
        if (rc == NULL) {
            return;
        }
        rc->count -= 1;
        if (rc->count != 0) {
            return;
        }
        // rc->count == 0.
        Rc old = rc;
        // Read rc->pre before we free rc.
        rc = rc->pre;
        free(old);
    }
}

////////////
// Vector //
////////////

typedef struct {
    Rc rc;
    unsigned int steps;
} Element;

void Element_drop(Element *elem) {
    Rc_drop(elem->rc);
}

// A dynamic, growable array.
// Pretty much a stack.
typedef struct {
    Element *ptr;
    unsigned int len;
    unsigned int cap;
} Vec;

void Vec_init(Vec *vec) {
    // No allocation if this stays empty!
    vec->cap = 0;
}

// Returns 0 on success, 1 on allocation error.
int Vec_push(Vec *vec, Element elem) {
    // Check if this vec was just initialized. If so, allocate.
    if (vec->cap == 0) {
        vec->ptr = (Element *) malloc(sizeof(Element) * 4);
        if (vec->ptr == NULL) {
            return 1;
        }
        vec->cap = 4;
        vec->len = 1;
        *(vec->ptr) = elem;
        return 0;
    }
    // If vec is full, double capacity and realloc.
    if (vec->len == vec->cap) {
        vec->cap *= 2;
        vec->ptr = (Element *) realloc(vec->ptr, vec->cap * sizeof(Element));
        if (vec->ptr == NULL) {
            return 1;
        }
    }
    (vec->ptr)[vec->len] = elem;
    vec->len += 1;
    return 0;
}

// Returns 0 on success, 1 on empty vec.
// When 1 is returned, location pointed by dst will stay as-is.
int Vec_pop(Vec *vec, Element *dst) {
    if (vec->len == 0) {
        return 1;
    }
    vec -> len -= 1;
    *dst = (vec->ptr)[vec->len];
    return 0;
}

// Destroy this vector and its contents.
void Vec_drop(Vec *vec) {
    if (vec->cap != 0) {
        Element elem;
        while (Vec_pop(vec, &elem) == 0) {
            Element_drop(&elem);
        }
        free(vec->ptr);
    }
}

///////////////////////
// Actual maze stuff //
///////////////////////

// '#' denotes wall, '.' unvisited cell, 'x' exit.
// ',' will be visited cell, for memo-purpose.
typedef char Cell;

typedef struct {
    unsigned int x;
    unsigned int y;
} Coordinate;

typedef struct {
    unsigned int width;
    unsigned int height;
    Cell *cells;
} Maze;

int visit_cell(Cell *cells, Vec *stack, Element *elem, unsigned int idx) {
    switch (cells[idx]) {
    case 'x':
        return 1;
    case '.':
        cells[idx] = ',';
        Element new_elem;
        new_elem.steps = elem->steps + 1;
        new_elem.rc = Rc_new(Rc_clone(elem->rc), idx);
        if (Vec_push(stack, new_elem) != 0) {
            return 2;
        }
    default:
        return 0;
    }
}

void idx_to_coordinate(unsigned int idx, unsigned int width, Coordinate *dst) {
    dst->x = idx % width;
    dst->y = idx / width;
}

// Find a route from start to exit of this maze.
// Solution will be written to location pointed by solution_out.
// Note that this reports the first solution it finds, possibly not the shortest one.
// Will cause buffer overflow if location pointed by solution_out is smaller than
// needed to store the first route found.
//
// Returns:
// * A positive number on success. This number represents number of coordinates written to solution_out.
// * 0 if input was valid, but no solution was found.
// * -1 if there was an allocation error.
// * -2 if input was invalid.
int Maze_solve(Maze *maze, Coordinate *start, Coordinate *solution_out) {
    if (maze == NULL || start == NULL || solution_out == NULL || maze->cells == NULL) {
        return -2;
    }
    if (start->x >= maze->width || start->y >= maze->height) {
        return -2;
    }
    unsigned int start_idx = start->y * maze->width + start->x;
    if ((maze->cells)[start_idx] == 'x') {
        *solution_out = *start;
        return 1;
    }
    unsigned int maze_size = maze->width * maze->height;
    unsigned int last_idx;
    Vec stack;
    Vec_init(&stack);
    Element elem;
    elem.rc = Rc_new(NULL, start_idx);
    if (elem.rc == NULL) {
        Vec_drop(&stack);
        return -1;
    }
    elem.steps = 1;
    // Create a copy of maze->cells because we want to keep maze->cells as-is,
    // but we want a way to record visited locations, too.
    Cell *cells = (Cell *) malloc(sizeof(Cell) * maze_size);
    if (cells == NULL) {
        Vec_drop(&stack);
        return -1;
    }
    memcpy(cells, maze->cells, sizeof(Cell) * maze_size);
    for (;;) { // Main loop
        unsigned int idx = elem.rc->idx;
        int ret;
        if (idx >= 10) {
            // up
            ret = visit_cell(cells, &stack, &elem, idx - 10);
            if (ret == 1) {
                last_idx = idx - 10;
                break;
            } else if (ret == 2) {
                Vec_drop(&stack);
                Element_drop(&elem);
                free(cells);
                return -1;
            }
        }
        if (idx >= 1) {
            // left
            ret = visit_cell(cells, &stack, &elem, idx - 1);
            if (ret == 1) {
                last_idx = idx - 1;
                break;
            } else if (ret == 2) {
                Vec_drop(&stack);
                Element_drop(&elem);
                free(cells);
                return -1;
            }
        }
        if (idx + 10 <= maze_size) {
            // down
            ret = visit_cell(cells, &stack, &elem, idx + 10);
            if (ret == 1) {
                last_idx = idx + 10;
                break;
            } else if (ret == 2) {
                Vec_drop(&stack);
                Element_drop(&elem);
                free(cells);
                return -1;
            }
        }
        if (idx + 1 <= maze_size) {
            // right
            ret = visit_cell(cells, &stack, &elem, idx + 1);
            if (ret == 1) {
                last_idx = idx + 1;
                break;
            } else if (ret == 2) {
                Vec_drop(&stack);
                Element_drop(&elem);
                free(cells);
                return -1;
            }
        }
        Element_drop(&elem);
        if (Vec_pop(&stack, &elem) == 1) {
            Vec_drop(&stack);
            free(cells);
            return 0;
        }
    }
    unsigned int i = elem.steps;
    int ret = i + 1;
    idx_to_coordinate(last_idx, maze->width, &solution_out[i]);
    Rc node = elem.rc;
    do {
        i -= 1;
        idx_to_coordinate(node->idx, maze->width, &solution_out[i]);
        node = node->pre;
    } while (node != NULL);
    Vec_drop(&stack);
    Element_drop(&elem);
    free(cells);
    return ret;
}

int main() {
    Maze maze;
    maze.width = 10;
    maze.height = 10;
    maze.cells =
        "##########"
        "....#....#"
        "#...#....#"
        "#.###..#.#"
        "#...#..#.#"
        "#.#.#..#.#"
        "#.#.#..#.#"
        "#.#.#..#.#"
        "#.#....#.x"
        "##########";
    Coordinate solution[47];
    Coordinate start;
    start.x = 0;
    start.y = 1;
    int ret = Maze_solve(&maze, &start, solution);
    if (ret == 0) {
        puts("No solution found");
        return 1;
    } else if (ret == -1) {
        puts("Allocation Error");
        return 1;
    } else if (ret == -2) {
        puts("Invalid input");
        return 1;
    }
    for (int i = 0; i < ret; i += 1) {
        Coordinate coord = solution[i];
        printf("%d, %d\n", coord.x, coord.y);
    }
    return 0;
}
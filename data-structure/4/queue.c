#include "queue.h"

#include <stdlib.h>
#include <string.h>

void Queue_resize_if_necessary(Queue *queue);

// Initialize this queue.
void Queue_init(Queue *queue) {
    queue->ptr = NULL;
    queue->cap = 0;
    queue->front = 0;
    queue->back = 0;
}

// Enqueue an element into this queue.
void Queue_enqueue(Queue *queue, Node *node) {
    Queue_resize_if_necessary(queue);
    queue->ptr[queue->back] = node;
    queue->back = (queue->back + 1) % queue->cap; 
}

// Dequeue an element from this queue.
// Returns NULL if the queue is empty.
Node *Queue_dequeue(Queue *queue) {
    if (queue->front == queue->back) {
        return NULL;
    }
    Node *ret = queue->ptr[queue->front];
    queue->front = (queue->front + 1) % queue->cap;
    return ret;
}

// Drop this queue and free memory allocated for it.
// Note that we don't drop the nodes stored in the queue.
// This is because we are 'borrowing' those nodes from someone else.
// It is that someone else's responsibility to drop those nodes.
void Queue_drop(Queue *queue) {
    free(queue->ptr);
}

// Called before enqueue. Checks if there's a room for one more value,
// and if not, reallocate and move stuff.
void Queue_resize_if_necessary(Queue *queue) {
    if (queue->cap == 0) {
        queue->ptr = (Node **) malloc(sizeof(Node *) * 4);
        queue->cap = 4;
        return;
    }

    // move the inner data around to keep this a proper ring buffer.
    if ((queue->back + 1) % queue->cap == queue->front) {
        unsigned int old_cap = queue->cap;
        queue->cap *= 2;
        queue->ptr = (Node **) realloc(queue->ptr, sizeof(Node *) * queue->cap);
        // Case 1: queue->front <= queue->back
        // [ooooooo.] -> [ooooooo.........]
        //  F      B      F      B
        // This is no-op, so we skip it.
        if (queue->front > queue->back) {
            // Case 2, if-branch:
            // [xx.ooooo] -> [...oooooxx......]
            //    BF             F      B
            // Case 3, else-branch:
            // [ooooo.xx] -> [ooooo.........xx]
            //       BF            B        F
            if (old_cap - queue->back >= queue->front) {
                memcpy(&(queue->ptr[old_cap]), queue->ptr, sizeof(Node *) * queue->back);
                queue->back += old_cap;
            } else {
                unsigned new_front = queue->cap - old_cap + queue->front;
                memcpy(
                    &(queue->ptr[new_front]),
                    &(queue->ptr[queue->front]),
                    sizeof(Node *) * (old_cap - queue->front)
                );
                queue->front = new_front;
            }
        }
    }
}
#ifndef QUEUE_H
#define QUEUE_H

#include "tree.h"

typedef struct {
    Node **ptr;
    unsigned int cap;
    unsigned int front;
    unsigned int back;
} Queue;

void Queue_init(Queue *queue);
void Queue_enqueue(Queue *queue, Node *value);
Node *Queue_dequeue(Queue *queue);
void Queue_drop(Queue *queue);

#endif
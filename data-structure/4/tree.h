#ifndef TREE_H
#define TREE_H

#include "maybe.h"

typedef struct Node {
    int value;
    struct Node *left;
    struct Node *right;
} Node;

typedef Node *BinaryTree;

Node *Node_new(int value);

MaybeInt BinaryTree_min(BinaryTree tree);
MaybeInt BinaryTree_max(BinaryTree tree);
void BinaryTree_level_traverse(BinaryTree tree, void (*visitor)(void *, int), void *state);
void BinaryTree_drop(BinaryTree tree);

#endif
#include "tree.h"
#include "queue.h"
#include "maybe.h"
#include <stdlib.h>

// Create a new Node.
Node *Node_new(int value) {
    Node *ret = (Node *) malloc(sizeof(Node));
    ret->value = value;
    ret->left = NULL;
    ret->right = NULL;
    return ret;
}

// Find the minimum value stored in this binary tree.
// Returns MaybeInt with tag 0 if this tree is empty,
// MaybeInt with tag 1 and value if it isn't.
MaybeInt BinaryTree_min(BinaryTree tree) {
    MaybeInt ret;
    MaybeInt tmp;

    if (tree == NULL) {
        ret.tag = 0;
        return ret;
    }
    ret.tag = 1;
    ret.value = tree->value;

    tmp = BinaryTree_min(tree->left);
    if (tmp.tag == 1 && tmp.value < ret.value) {
        ret.value = tmp.value;
    }

    tmp = BinaryTree_min(tree->right);
    if (tmp.tag == 1 && tmp.value < ret.value) {
        ret.value = tmp.value;
    }

    return ret;
}

// Find the maximum value stored in this binary tree.
// Returns MaybeInt with tag 0 if this tree is empty,
// MaybeInt with tag 1 and value if it isn't.
// Actually a copy-paste-edit version of BinaryTree_min.
MaybeInt BinaryTree_max(BinaryTree tree) {
    MaybeInt ret;
    MaybeInt tmp;

    if (tree == NULL) {
        ret.tag = 0;
        return ret;
    }
    ret.tag = 1;
    ret.value = tree->value;

    tmp = BinaryTree_max(tree->left);
    if (tmp.tag == 1 && tmp.value > ret.value) {
        ret.value = tmp.value;
    }

    tmp = BinaryTree_max(tree->right);
    if (tmp.tag == 1 && tmp.value > ret.value) {
        ret.value = tmp.value;
    }

    return ret;
}

// Level-traverse this tree, calling (*visitor)(state, value) for each value encountered.
// Generics and closure would've been nice here.
void BinaryTree_level_traverse(BinaryTree tree, void (*visitor)(void *, int), void *state) {
    Queue q;
    Queue_init(&q);
    while (tree != NULL) {
        (*visitor)(state, tree->value);
        if (tree->left != NULL) {
            Queue_enqueue(&q, tree->left);
        }
        if (tree->right != NULL) {
            Queue_enqueue(&q, tree->right);
        }
        tree = Queue_dequeue(&q);
    }
    Queue_drop(&q);
}

// Drop this tree and free the memory allocated for it.
void BinaryTree_drop(BinaryTree tree) {
    while (tree != NULL) {
        BinaryTree_drop(tree->left);
        BinaryTree tmp = tree->right;
        free(tree);
        tree = tmp;
    }
}
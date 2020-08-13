#include "tree.h"

#include <stdio.h>

void visit(void *state, int value) {
    char *first = (char *) state;
    if (*first == 1) {
        *first = 0;
        printf("%d", value);
    } else {
        printf(", %d", value);
    }
}

int main() {
    BinaryTree tree = Node_new(8);
    tree->left = Node_new(5);
    tree->right = Node_new(4);
    tree->left->left = Node_new(9);
    tree->left->right = Node_new(7);
    tree->right->right = Node_new(11);
    tree->left->right->left = Node_new(1);
    tree->left->right->right = Node_new(12);
    tree->right->right->left = Node_new(3);
    tree->left->right->right->left = Node_new(2);

    MaybeInt min = BinaryTree_min(tree);
    if (min.tag == 0) {
        puts("Minimum value: None");
    } else {
        printf("Minimum value: %d\n", min.value);
    }

    MaybeInt max = BinaryTree_max(tree);
    if (max.tag == 0) {
        puts("Maximum value: None");
    } else {
        printf("Maximum value: %d\n", max.value);
    }

    puts("Level order traversal:");
    char first = 1;
    BinaryTree_level_traverse(tree, visit, (void *) &first);
    putchar('\n');

    return 0;
}
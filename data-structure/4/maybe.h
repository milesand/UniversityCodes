#ifndef MAYBE_H
#define MAYBE_H

// I miss generics and sum types

// If tag is 0, then value is uninitialized and must not be read.
// If tag is 1, then value is initialized.
// Else, it is an error.
typedef struct {
    char tag;
    int value;
} MaybeInt;

#endif
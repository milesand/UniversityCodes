# Artificial Intelligence

Pieces of code I wrote for AI class assignments.
Each directory contains its own README, which was handed in as a part of the assignment.

## `perceptron`

Perceptron, without learning capability.

The actual perceptron code lives in `include/perceptron.hpp` due to use of
templates. `src/main.cpp` generates a perceptron randomly/with user input and
tests it against AND gate case.

Run following to build:

```sh
g++ -I. src/main.cpp
```

## `perceptron_learn`

Perceptron, with learning capability.

Again, the perceptron code lives in `include/perceptron.hpp`. `src/main.cpp`
runs a training loop based on training data given via standard input.
`training_data` contains training data formatted for the program.

To build:

```sh
g++ -I. src/main.cpp
```

To run:

```sh
./a.out < training_data/and.csv  # or.csv / xor.csv works too
```

## `nn`

Basic neural network.

The most significant files are `perceptron.hpp` and `layers.hpp`
in `include`, which implement the `Perceptron` class, and `InputLayer` &
`ComputationLayer` classes respectively.

Binaries may be generated with `make all`, which results in `bin/and`,
`bin/or`, `bin/xor`, and `bin/donut`. Each can be generated individually with,
for example, `make bin/and`. All binaries share the same interface; Parameters
like learning rate and RNG seed my be set using command line arguments, and
they generate log of network weights over epochs through standard output.

`plot/run.py` generates images from outputs of binaries.

Some cases that were included in original assignment:

- `and`, `or`, `xor` and `donut` with `--seed=9`
- `donut` with `--seed=36`
- `xor` with `--seed=0`
  - with `--lrate=0.3177`
  - with `--lrate=0.3178`

The inner README.md contains more information.

## `mnist`

MNIST Dataset with multiple models, using Tensorflow 2.
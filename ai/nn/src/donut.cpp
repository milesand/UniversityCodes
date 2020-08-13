#include <array>
#include <cmath>
#include <utility>

#include "include/layers.hpp"
#include "include/activations.hpp"
#include "include/run.hpp"

typedef InputLayer<2> Layer0;
typedef ComputationLayer<4, Tanh, Layer0> Layer1;
typedef ComputationLayer<4, Tanh, Layer1> Layer2;
typedef ComputationLayer<4, Tanh, Layer2> Layer3;
typedef ComputationLayer<4, Tanh, Layer3> Layer4;
typedef ComputationLayer<1, Sigmoid, Layer4> Network;

typedef std::array<float, 2> CaseInput;
typedef std::array<float, 1> CaseOutput;
typedef std::pair<CaseInput, CaseOutput> Case;

int main(const int argc, const char *argv[]) {
    std::array<Case, 9> cases = {{
        {{0.0, 0.0}, {0.0}},
        {{0.0, 1.0}, {0.0}},
        {{1.0, 0.0}, {0.0}},
        {{1.0, 1.0}, {0.0}},
        {{0.5, 1.0}, {0.0}},
        {{1.0, 0.5}, {0.0}},
        {{0.0, 0.5}, {0.0}},
        {{0.5, 0.0}, {0.0}},
        {{0.5, 0.5}, {1.0}}
    }};
    return run<Network>(argc, argv, cases.cbegin(), cases.cend());
}
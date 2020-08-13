#include <array>
#include <cmath>
#include <utility>

#include "include/layers.hpp"
#include "include/activations.hpp"
#include "include/run.hpp"

typedef InputLayer<2> Layer0;
typedef ComputationLayer<1, Sigmoid, Layer0> Network;

typedef std::array<float, 2> CaseInput;
typedef std::array<float, 1> CaseOutput;
typedef std::pair<CaseInput, CaseOutput> Case;

int main(const int argc, const char *argv[]) {
    std::array<Case, 4> cases = {{
        {{0.0, 0.0}, {0.0}},
        {{0.0, 1.0}, {1.0}},
        {{1.0, 0.0}, {1.0}},
        {{1.0, 1.0}, {1.0}}
    }};
    return run<Network>(argc, argv, cases.cbegin(), cases.cend());
}
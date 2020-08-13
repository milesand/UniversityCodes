#include <array>
#include <cmath>
#include <iostream>
#include <random>

#include "include/perceptron.hpp"

int main() {
    std::random_device rd;
    // one_inclusive is actually the least float value larger than 1.0.
    // uniform_real_distribution only allows half-closed intervals that are
    // open on the larger side; This 'closes' that side. Well, sort of.
    float one_inclusive = std::nextafter(1.0, 2.0);
    std::uniform_real_distribution<float> dist(-1.0, one_inclusive);

    // Initial weight values for our perceptron.
    std::array<float, 2> w;
    for (auto i = w.begin(); i != w.end(); ++i) {
        // Generate a random float in [-1.0, 1.0]
        *i = dist(rd);
    }

    // Generate another float for threshold, and use the array we generated
    // for weights.
    StaticPerceptron<2> p(dist(rd), w.begin());

    std::array<std::array<float, 3>, 4> cases = {{
        {0.0, 0.0, 0.0},
        {0.0, 1.0, 0.0},
        {1.0, 0.0, 0.0},
        {1.0, 1.0, 1.0}
    }};

    for (;;) {
        // Pretty-print current perceptron state.
        std::cout << "Running with t = " << p.threshold << ", w = [";
        if (p.weights.size() != 0) {
            auto i = p.weights.begin();
            std::cout << *i;
            for (++i; i != p.weights.end(); ++i) {
                std::cout << ", " << *i;
            }
        }
        std::cout << "]" << std::endl;

        // Run test cases and count wrong results.
        size_t wrong_count = 0;
        
        for (auto c = cases.begin(); c != cases.end(); ++c) {
            float expected = (*c)[2];
            float got = p.run(c->begin());

            std::cout << (*c)[0] << ", " << (*c)[1] << " -> " << got << std::endl;

            if (got != expected) {
                wrong_count += 1;
            }
        }

        if (wrong_count == 0) {
            std::cout << "All cases passed" << std::endl;
            break;
        }
        std::cout << wrong_count << "/4 cases failed" << std::endl;

        // Some cases failed. Get new t and w values from... the user?
        std::cout << "Input new threshold and weights: ";
        std::cin >> p.threshold >> p.weights[0] >> p.weights[1];
    }

    return 0;
}
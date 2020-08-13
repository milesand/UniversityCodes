#include <array>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <random>
#include <utility>
#include <vector>

#include "include/perceptron.hpp"

const float learn_rate = 0.01;

// Read training data from stdin, and print perceptron's internal values as csv to stdout.

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
    StaticPerceptron<2> p(dist(rd), w.begin());;

    // Initialize training data from stdin. This handles a crude variant of csv.
    std::vector<std::array<float, 3>> cases;
    while (!std::cin.eof()) {
        std::array<float, 3> c;

        std::cin >> c[0];
        if (std::cin.eof()) {
            break;
        }
        if (std::cin.get() != ',') {
            std::cout << "Error: invalid input 1" << std::endl;
            return 1;
        }

        std::cin >> c[1];
        if (std::cin.get() != ',') {
            std::cout << "Error: invalid input 2" << std::endl;
            return 1;
        }

        std::cin >> c[2];
        if (std::cin.get() != '\n' && !std::cin.eof()) {
            std::cout << "Error: invalid input 3" << std::endl;
            return 1;
        }

        cases.push_back(c);
    }

    for (;;) {
        // Print current perceptron state in csv format.
        std::cout << p.threshold;
        for (auto i = p.weights.begin(); i != p.weights.end(); ++i) {
            std::cout << "," << *i;
        }

        // Run test cases and count wrong results.
        size_t wrong_count = 0;
        
        for (auto c = cases.begin(); c != cases.end(); ++c) {
            float expected = (*c)[2];
            float got = p.run(c->begin());

            if (got != expected) {
                wrong_count += 1;
            }
        }

        std::cout << "," << wrong_count << std::endl;

        if (wrong_count == 0) {
            break;
        }

        std::vector<std::pair<std::array<float, 3>::const_iterator, float>> train;
        for (auto c = cases.cbegin(); c != cases.end(); ++c) {
            train.emplace_back(c->cbegin(), (*c)[2]);
        }

        p.learn_step(train.cbegin(), train.cend(), learn_rate);
    }

    return 0;
}
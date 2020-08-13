#ifndef PERCEPTRON_H
#define PERCEPTRON_H

#include <cstddef>
#include <array>

// Perceptron with compile-time defined number of incoming connections.
template <std::size_t N>
class StaticPerceptron {
public:
    // fields are public because the assignment requires these to be changed
    // and just having them public is easier than formal setters. 

    // Threshold of this perceptron.
    float threshold;

    // Weights to be applied to input.
    std::array<float, N> weights;

    // Constructor. First N values in the given iterator will be used as weight
    // for this perceptron. User must make sure these values are accessible.
    // Failing to do so may result in an undefined behavior.
    template <typename Iterator>
    StaticPerceptron(float threshold, Iterator weights);

    // Run the numbers using the input iterator as incoming connection.
    // First N values in the given iterator will be accessed; user must make
    // sure these values are accessible. Failing to do so may result in an
    // undefined behavior.
    template<typename Iterator>
    float run(Iterator inputs);
};

// Template stuff needs to live in header file!
// I thought I'd put implementations in a separate file and #include it here,
// but VSCode doesn't seem to like that very much.

template <std::size_t N>
template <typename Iterator>
StaticPerceptron<N>::StaticPerceptron(float threshold, Iterator weights):
    threshold(threshold)
{
    if (N == 0) {
        return;
    }

    // Advance iterator only (N-1) times. We only require N values to be
    // accessible from the iterator; that would imply 'can be advanced (N-1)
    // times', but probably not 'can be advanced N times'. Well, I'm not really
    // sure that's a problem, but still.
    auto iter = this->weights.begin();
    auto end = this->weights.end();
    *iter = *weights;
    for (++iter; iter != end; ++iter) {
        ++weights;
        *iter = *weights;
    }
}

template <std::size_t N>
template <typename Iterator>
float StaticPerceptron<N>::run(Iterator inputs) {
    float acc = -this->threshold;

    if (N != 0) {
        auto iter = this->weights.begin();
        auto end = this->weights.end();

        acc += (*iter) * (*inputs);
        for (++iter; iter != end; ++iter) {
            ++inputs;
            acc += (*iter) * (*inputs);
        }
    }

    // Activation function here. This might work better as a separate function
    // but I don't think there's a way to effectively hide this 'implementation
    // detail' in C++ that way. Making it a private static function member of
    // StaticPerceptron<N> might work, but I think that generates N different
    // copies of this exactly same snippet... Hmm.
    if (acc >= 0.0) {
        return 1.0;
    }
    return 0.0;
}

#endif
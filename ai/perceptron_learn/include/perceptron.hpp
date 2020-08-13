#ifndef PERCEPTRON_H
#define PERCEPTRON_H

#include <cstddef>
#include <array>
#include <tuple>

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

    // Train this perceptron with given data, once.
    // TrainingDataIterator should be an iterator over std::pair<Iterator, float> where
    // first is an iterator over input float values, and second is the expected output.
    template <typename Iter>
    void learn_step(Iter begin, Iter end, float learn_rate);
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

template <std::size_t N>
template <typename Iter>
void StaticPerceptron<N>::learn_step(Iter begin, Iter end, float learn_rate) {
    if (N != 0) {
        for (Iter i = begin; i != end; ++i) {
            auto pair = *i;
            auto input_iter = pair.first;
            float expected = pair.second;

            // We need each value of the input *after* running them through the perceptron.
            // So we'll collect them to an array first.
            std::array<float, N> inputs;
            {
                auto j = inputs.begin();
                *j = *input_iter;
                for (++j; j != inputs.end(); ++j) {
                    ++input_iter;
                    *j = *input_iter;
                }
            }

            float got = this->run(inputs.cbegin());
            float diff = expected - got;
            
            auto w = this->weights.begin();
            for (auto j = inputs.cbegin(); j != inputs.cend(); ++j) {
                // We'll pretend that our activation function is identity function,
                // thus its derivative is 1 everywhere.
                float dw = learn_rate * diff * (*j); // * 1
                *w += dw;
                ++w;
            }

            // Update the threshold.
            // Threshold is treated as a weight with input always equal to -1.
            float dt = learn_rate * diff * -1;
            this->threshold += dt;
        }
    }
}

#endif
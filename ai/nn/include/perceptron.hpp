#ifndef PERCEPTRON_HPP
#define PERCEPTRON_HPP

#include <cstddef>
#include <array>
#include <tuple>
#include <ostream>

// Perceptron with N incoming connections, using float for calculation and
// Activation for activation function and gradient calculation.
// Activation must have two static methods: `float value(float)` and
// `float derivative(float)`.
template<std::size_t N, typename Activation>
class Perceptron {
private:
    // Threshold of this perceptron.
    float _bias;

    // Weights to be applied to input.
    std::array<float, N> weights;

    // Calculate net value, pre-activation.
    template<typename InputIterator>
    float net(InputIterator inputs) const;

public:
    Perceptron() {}

    // Constructor. First N items in `weights` will be accessed by incrementing
    // N times. Type `InputIterator` must satisfy
    // *LegacyInputIterator*, and its `value_type` must be `float`.
    template<typename InputIterator>
    Perceptron(float bias, InputIterator& weights);

    // This perceptron's bias.
    float bias() const;

    // Constant iterator to the beginning of weights.
    auto cbegin() const;

    // Constant iterator to the end of weights.
    auto cend() const;

    // Print this perceptron's bias and weights to given ostream.
    void print(std::ostream& ostream, const char* separator) const;

    // Calculate the output of this perceptron, using first N items in `inputs`
    // as incoming 'signal strength'. `inputs` will be incremented exactly
    // max(0, N - 1) times. Type `InputIterator` must satisfy
    // *LegacyInputIterator*, and its `value_type` must be `float`.
    template<typename InputIterator>
    float run(InputIterator inputs) const;

    // Update this perceptron's threshold and weights, using first N items in
    // `inputs` as inputs from the previous layer and 'delta value' from the
    // next layer. 'Delta value's corresponding to this perceptron will be
    // added to first N items in `delta_out`.
    //
    // 'Delta value's added to `delta_out` are actually product of incoming
    // weights and actual delta value calculated for this perceptron; `delta`
    // is also sum of such values regarding weights for connection between this
    // perceptron and perceptrons in the next layer. This is neccessary because
    // a perceptron does not have access to outgoing connections' weights.
    //
    // Both `ForwardIterator` and `MutableIterator`'s `value_type` must be
    // `float`; `ForwardIterator` must satisfy *LegacyForwardIterator*, and
    // `MutableIterator` must satisfy *LegacyInputIterator* and
    // *LegacyOutputIterator*.
    template<typename ForwardIterator, typename MutableIterator>
    void backpropagate(
        ForwardIterator inputs,
        float delta,
        float learning_rate,
        MutableIterator delta_out
    );
};

template<std::size_t N, typename Activation>
template<typename InputIterator>
Perceptron<N, Activation>::Perceptron(
    float bias,
    InputIterator& weights
): _bias(bias)
{
    if (N == 0) {
        return;
    }

    // Advance iterator only (N-1) times. We only require N values to be
    // accessible from the iterator; that would imply 'can be advanced (N-1)
    // times', but potentially not 'can be advanced N times'. Well, I'm not really
    // sure that's a problem, but still.
    auto iter = this->weights.begin();
    auto end = this->weights.end();
    *iter = *weights;
    for (++iter; iter != end; ++iter) {
        ++weights;
        *iter = *weights;
    }
}

template<std::size_t N, typename Activation>
void Perceptron<N, Activation>::print(std::ostream& ostream, const char* separator) const {
    ostream << this->_bias;
    for (auto i = this->cbegin(); i != this->cend(); ++i) {
        ostream << separator << *i;
    }
}

template<std::size_t N, typename Activation>
float Perceptron<N, Activation>::bias() const {
    return this->_bias;
}

template<std::size_t N, typename Activation>
auto Perceptron<N, Activation>::cbegin() const {
    return this->weights.cbegin();
}

template<std::size_t N, typename Activation>
auto Perceptron<N, Activation>::cend() const {
    return this->weights.cend();
}

template<std::size_t N, typename Activation>
template<typename InputIterator>
float Perceptron<N, Activation>::net(InputIterator inputs) const {
    float net = this->_bias;

    if (N != 0) {
        auto weights = this->weights.cbegin();
        net += (*weights) * (*inputs);
        for (++weights; weights != this->weights.cend(); ++weights) {
            ++inputs;
            net += (*weights) * (*inputs);
        }
    }

    return net;
}

template<std::size_t N, typename Activation>
template<typename InputIterator>
float Perceptron<N, Activation>::run(InputIterator inputs) const {
    float net = this->net(inputs);
    return Activation::activate(net);
}

template<std::size_t N, typename Activation>
template<typename ForwardIterator, typename MutableIterator>
void Perceptron<N, Activation>::backpropagate(
    ForwardIterator inputs,
    float delta,
    float learning_rate,
    MutableIterator delta_out
) {
    ForwardIterator inputs_clone(inputs);

    float net = this->net(inputs_clone);

    float my_delta = delta * Activation::derivative(net);
    this->_bias -= learning_rate * my_delta;
    if (N != 0) {
        auto weights = this->weights.begin();
        *delta_out += (*weights) * my_delta;
        *weights -= learning_rate * (*inputs) * my_delta;
        for (++weights; weights != this->weights.end(); ++weights) {
            ++inputs;
            ++delta_out;
            *delta_out += (*weights) * my_delta;
            *weights -= learning_rate * (*inputs) * my_delta;
        }
    }
}

#endif
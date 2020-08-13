#ifndef LAYERS_HPP
#define LAYERS_HPP

#include <array>
#include <ostream>

#include "include/perceptron.hpp"

// Input layer for a neural network with N inputs.
template<std::size_t N>
class InputLayer {
private:
    // Values recieved from input, stored for later use.
    std::array<float, N> outputs;

public:
    InputLayer() {}

    template<typename InputIterator>
    InputLayer(InputIterator& weights) {}

    // Store first N values in `inputs` for later layers to use.
    template<typename InputIterator>
    void run(InputIterator inputs);

    // A dummy function, for satisfying template requirement.
    template<typename InputIterator>
    void backpropagate(InputIterator deltas, float learning_rate) {}

    // Number of outputs this layer produces.
    static constexpr std::size_t size() { return N; }

    // Pretty-print(Okay, maybe not that pretty) this layer to given ostream.
    // This is no-op for input layer.
    bool print(std::ostream& ostream, const char* layer_sep, const char* perceptron_sep, const char* weight_sep) const { return false; }

    // Constant iterator this layer's output's beginning. Must be called after
    // calling `run`.
    auto outputs_cbegin() const { return this->outputs.cbegin(); }

    // Constant iterator this layer's output's end. Must be called after calling
    // `run`.
    auto outputs_cend() const { return this->outputs.cend(); }
};

template<std::size_t N>
template<typename InputIterator>
void InputLayer<N>::run(InputIterator inputs) {
    if (N != 0) {
        auto output = this->outputs.begin();
        *output = *inputs;
        for (++output; output != this->outputs.end(); ++output) {
            ++inputs;
            *output = *inputs;
        }
    }
}

// Computation layer with `N` perceptrons, using activation function provided by
// `Activation`. `Layer` should be the type of previous layer in the network.
template<
    std::size_t N,
    typename Activation,
    typename Layer
> class ComputationLayer {
private:
    // Array of perceptrons.
    std::array<Perceptron<Layer::size(), Activation>, N> perceptrons;
    // Buffer for outputs of this layer's perceptrons.
    std::array<float, N> outputs;
    Layer prev_layer;

    template<typename InputIterator>
    void set_weights_from_iterator(InputIterator& weights);

public:
    ComputationLayer() = delete;

    template<typename InputIterator>
    ComputationLayer(InputIterator& weights);

    // Initialize this layer's perceptrons using first value from weights as
    // threshold for the first perceptron, next I values for its weights, next
    // value as threshold for the second perceptron, and so on.
    template<typename InputIterator>
    ComputationLayer(InputIterator& weights, Layer previous_layer);

    // Calculate the output of this layer, given `inputs` for the input for the
    // network by delegating it to the previous layer and calculating this
    // layer's own output.
    template<typename InputIterator>
    void run(InputIterator inputs);

    // Train this layer and its previous layers by backpropagation, Assuming
    // same input values as the last `run`.
    // i-th value provided by `deltas` must be the sum of products of i-th
    // perceptron's outgoing connection's weight and the destination
    // perceptron's delta value. Since this description is very handful, users
    // may want to use `learn` which uses `backpropagate` internally.
    template<typename InputIterator>
    void backpropagate(InputIterator deltas, float learning_rate);

    // Train this layer and its previous layers by using `expected` as expected
    // output for each perceptron in this layer. Internally, uses square error
    // as loss function to calculate delta values and uses that for
    // backpropagation.
    template<typename InputIterator>
    void learn(InputIterator expected, float learning_rate);

    // Ideally there should be something for batched learning too. Maybe later.

    // Number of outputs this layer produces.
    static constexpr std::size_t size() { return N; }

    // Print this layer and its previous layers' perceptron's bias and weights to given output stream.
    bool print(std::ostream& ostream, const char* layer_sep, const char* perceptron_sep, const char* weight_sep) const;

    // Reference to the previous layer.
    const Layer& previous_layer() const { return this->prev_layer; }

    // Iterators to outputs.
    auto outputs_cbegin() const { return this->outputs.cbegin(); }
    auto outputs_cend() const { return this->outputs.cend(); }

    // Iterators to this layer's perceptrons.
    auto perceptrons_cbegin() const { return this->perceptrons.cbegin(); }
    auto perceptrons_cend() const { return this->perceptrons.cend(); }
};

template<
    std::size_t N,
    typename Activation,
    typename Layer
>
template<typename InputIterator>
ComputationLayer<N, Activation, Layer>::ComputationLayer(
    InputIterator& weights
): prev_layer(weights) {
    this->set_weights_from_iterator(weights);
}

template<
    std::size_t N,
    typename Activation,
    typename Layer
>
template<typename InputIterator>
ComputationLayer<N, Activation, Layer>::ComputationLayer(
    InputIterator& weights,
    Layer previous_layer
): prev_layer(previous_layer) {
    this->set_weights_from_iterator(weights);
}

template<
    std::size_t N,
    typename Activation,
    typename Layer
>
template<typename InputIterator>
void ComputationLayer<N, Activation, Layer>::set_weights_from_iterator(InputIterator& weights) {
    for (auto p = this->perceptrons.begin(); p != this->perceptrons.end(); ++p) {
        float threshold = *weights;
        ++weights;
        *p = Perceptron<Layer::size(), Activation>(threshold, weights);
        ++weights;
    }
}

template<
    std::size_t N,
    typename Activation,
    typename Layer
>
template<typename InputIterator>
void ComputationLayer<N, Activation, Layer>::run(
    InputIterator inputs
) {
    this->prev_layer.run(inputs);
    auto p = this->perceptrons.cbegin();
    for (auto o = this->outputs.begin(); o != this->outputs.end(); ++o) {
        auto i = this->prev_layer.outputs_cbegin();
        *o = p->run(i);
        ++p;
    }
}

template<
    std::size_t N,
    typename Activation,
    typename Layer
>
template<typename InputIterator>
void ComputationLayer<N, Activation, Layer>::backpropagate(
    InputIterator deltas,
    float learning_rate
) {
    std::array<float, Layer::size()> my_deltas;
    for (auto i = my_deltas.begin(); i != my_deltas.end(); ++i) {
        *i = 0.0;
    }
    if (N != 0) {
        auto i = this->prev_layer.outputs_cbegin();
        auto d = my_deltas.begin();
        auto p = this->perceptrons.begin();
        p->backpropagate(i, *deltas, learning_rate, d);
        for (++p; p != this->perceptrons.end(); ++p) {
            i = this->prev_layer.outputs_cbegin();
            d = my_deltas.begin();
            ++deltas;
            p->backpropagate(i, *deltas, learning_rate, d);
        }
    }
    this->prev_layer.backpropagate(my_deltas.cbegin(), learning_rate);
}

template<
    std::size_t N,
    typename Activation,
    typename Layer
>
template<typename InputIterator>
void ComputationLayer<N, Activation, Layer>::learn(
    InputIterator expected,
    float learning_rate
) {
    std::array<float, Layer::size()> deltas;
    auto outputs = this->outputs_cbegin();
    for (auto delta = deltas.begin(); delta != deltas.end(); ++delta) {
        // This delta value is binary cross entropy specific.
        // TODO: maybe refactor this.
        *delta = 1.0 / (1.0 - *outputs - *expected);
        ++outputs;
        ++expected;
    }
    this->backpropagate(deltas.begin(), learning_rate);
}

template<
    std::size_t N,
    typename Activation,
    typename Layer
>
bool ComputationLayer<N, Activation, Layer>::print(
    std::ostream& ostream,
    const char* layer_sep,
    const char* perceptron_sep, 
    const char* weight_sep
) const {
    bool prev_printed = this->prev_layer.print(ostream, layer_sep, perceptron_sep, weight_sep);
    if (N == 0) {
        return prev_printed;
    }
    if (prev_printed) {
        ostream << layer_sep;
    }
    auto p = this->perceptrons_cbegin();
    p->print(ostream, weight_sep);
    for (++p; p != this->perceptrons_cend(); ++p) {
        ostream << perceptron_sep;
        p->print(ostream, weight_sep);
    }
    return true;
}

#endif
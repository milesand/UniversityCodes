#include "include/activations.hpp"

#include <cmath>

float ReLU::activate(float at) {
    if (at >= 0.0) {
        return at;
    }
    return 0.0;
}

float ReLU::derivative(float at) {
    if (at >= 0.0) {
        return 1.0;
    }
    return 0.0;
}

float Tanh::activate(float at) {
    return std::tanh(at);
}

float Tanh::derivative(float at) {
    float x = std::tanh(at);
    return 1.0 - x * x;
}

float Sigmoid::activate(float at) {
    float x = exp(-at);
    return 1.0 / (1.0 + x);
}

float Sigmoid::derivative(float at) {
    float x = exp(-at);
    return x / ((1.0 + x) * (1.0 + x));
}
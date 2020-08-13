#ifndef ACTIVATIONS_HPP
#define ACTIVATIONS_HPP

class ReLU {
public:
    static float activate(float at);
    static float derivative(float at);
};

class Tanh {
public:
    static float activate(float at);
    static float derivative(float at);
};

class Sigmoid {
public:
    static float activate(float at);
    static float derivative(float at);
};

#endif
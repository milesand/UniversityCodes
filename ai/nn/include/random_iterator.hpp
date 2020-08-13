#ifndef RANDOM_ITERATOR_HPP
#define RANDOM_ITERATOR_HPP

#include <type_traits>

// Generate multiple random values using iterator interface.
// `Generator` must satisfy *UniformRandomBitGenerator*.
// `Distribution` must satisfy *RandomNumberDistribution*.
template<typename Generator, typename Distribution>
class RandomIterator {
private:
    // Random number generator.
    Generator* gen;
    // Random number distribution.
    Distribution dist;
    // Buffer for generated value.
    typename Distribution::result_type value;

public:
    RandomIterator(Generator* gen, Distribution dist):
        gen(gen), dist(dist), value(this->dist(*(this->gen))) {};

    RandomIterator& operator++();

    typename Distribution::result_type operator*();
};

template<typename Generator, typename Distribution>
RandomIterator<Generator, Distribution>& RandomIterator<Generator, Distribution>::operator++() {
    this->value = this->dist(*(this->gen));
    return *this;
}

template<typename Generator, typename Distribution>
typename Distribution::result_type RandomIterator<Generator, Distribution>::operator*() {
    return this->value;
}

#endif
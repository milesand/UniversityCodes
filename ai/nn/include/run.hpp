#ifndef RUN_HPP
#define RUN_HPP

#include <cstdlib>
#include <cstring>

#include <iostream>
#include <random>

#include "include/random_iterator.hpp"

template<typename Layer, typename ForwardIterator>
int run(const int argc, const char *argv[], ForwardIterator cases_begin, ForwardIterator cases_end) {

    bool seed_flag = false;
    unsigned int seed;

    float learning_rate = 0.001;

    float error_threshold = 0.03;

    unsigned int print_interval = 0;

    for (int argi = 1; argi < argc; argi += 1) {
        const char* arg = argv[argi];
        if (std::strncmp(arg, "--seed=", 7) == 0) {
            seed = std::strtol(arg + 7, NULL, 0);
            seed_flag = true;
            continue;
        }
        if (std::strncmp(arg, "--lrate=", 8) == 0) {
            learning_rate = strtof(arg + 8, NULL);
            continue;
        }
        if (std::strncmp(arg, "--ethr=", 7) == 0) {
            error_threshold = strtof(arg + 7, NULL);
            continue;
        }
        if (std::strncmp(arg, "--pintv=", 8) == 0) {
            print_interval = strtol(arg + 8, NULL, 0);
            continue;
        }

        std::cout << "Unrecognized argument: " << arg << std::endl;
        return 1;
    }

    if (learning_rate <= 0.0) {
        std::cerr << "Invalid learning rate " << learning_rate << "; Should be larger than 0\n";
        std::cerr << "Using default value of 0.001 instead\n";
        learning_rate = 0.001;
    }

    print_interval += 1;

    if (!seed_flag) {
        std::random_device rd;
        seed = rd();
    }
    
    float end_inclusive = std::nextafter(1.0, 1.0);
    std::uniform_real_distribution<float> dist(-1.0, end_inclusive);
    std::mt19937 gen(seed);
    RandomIterator<decltype(gen), decltype(dist)> random_iter(&gen, dist);

    Layer nn(random_iter);
    
    unsigned int epoch = 1;
    for (;;) {
        float error = 0.0;
        int case_count = 0;
        for (ForwardIterator cases = cases_begin; cases != cases_end; ++cases) {
            nn.run(cases->first.cbegin());
            auto expected = cases->second.cbegin();
            for (auto out = nn.outputs_cbegin(); out != nn.outputs_cend(); ++out) {
                case_count += 1;
                error -= (*expected) * log(*out) + (1.0 - *expected) * log(1.0 - *out);
                ++expected;
            }
        }
        error /= case_count;
        if (print_interval == 1 || epoch % print_interval == 1 || error < error_threshold) {
            std::cout << "Epoch " << epoch << "\n{\n  [";
            nn.print(std::cout, "]\n}\n{\n  [", "],\n  [", ", ");
            std::cout << "]\n}\n" << error << "\n\n";
        }
        if (error < error_threshold) {
            break;
        }
        for (ForwardIterator cases = cases_begin; cases != cases_end; ++cases) {
            nn.run(cases->first.cbegin());
            nn.learn(cases->second.cbegin(), learning_rate);
        }
        epoch += 1;
    }

    return 0;
}

#endif
CXX=g++
CXXFLAGS=-Wall -g
CPPFLAGS=-I.

all: bin/and bin/or bin/xor bin/donut



bin/and: build/and.o build/activations.o | bin
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ build/and.o build/activations.o

build/and.o: src/and.cpp include/layers.hpp include/activations.hpp include/run.hpp include/perceptron.hpp include/random_iterator.hpp | build
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c src/and.cpp



bin/or: build/or.o build/activations.o | bin
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ build/or.o build/activations.o

build/or.o: src/or.cpp include/layers.hpp include/activations.hpp include/run.hpp include/perceptron.hpp include/random_iterator.hpp | build
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c src/or.cpp



bin/xor: build/xor.o build/activations.o | bin
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ build/xor.o build/activations.o

build/xor.o: src/xor.cpp include/layers.hpp include/activations.hpp include/run.hpp include/perceptron.hpp include/random_iterator.hpp | build
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c src/xor.cpp



bin/donut: build/donut.o build/activations.o | bin
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ build/donut.o build/activations.o

build/donut.o: src/donut.cpp include/layers.hpp include/activations.hpp include/run.hpp include/perceptron.hpp include/random_iterator.hpp | build
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c src/donut.cpp



build/activations.o: src/activations.cpp include/activations.hpp | build
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c src/activations.cpp



bin:
	mkdir bin

build:
	mkdir build



clean:
	rm -rf build bin

.PHONY: clean all
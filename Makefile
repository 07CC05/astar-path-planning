CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Iinclude
SRC = src/PathFinder.cpp src/Terrain.cpp src/Visualizer.cpp

.PHONY: all test clean run

all: astar_demo astar_test

astar_demo: src/main.cpp $(SRC) $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) src/main.cpp $(SRC) -o astar_demo

astar_test: tests/test.cpp $(SRC) $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) tests/test.cpp $(SRC) -o astar_test

test: astar_test
	./astar_test

run: astar_demo
	./astar_demo compare maze 41 27 42
	./astar_demo scale
	./astar_demo dynamic

clean:
	rm -f astar_demo astar_test

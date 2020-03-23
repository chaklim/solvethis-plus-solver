all: solver

solver: solver.cpp
	$(CXX) -O3 -std=c++11 solver.cpp -o solver

clean:
	rm -f solver

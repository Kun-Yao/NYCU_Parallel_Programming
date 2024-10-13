CXX := g++ -m64
CXXFLAGS := -O3 -std=c++17 -Wall -pthread -mavx

all: pi.out

pi.out: pi.cpp
	$(CXX) $(CXXFLAGS) pi.cpp -o pi.out

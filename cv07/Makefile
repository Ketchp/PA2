CXX = g++
CXXFLAGS= -std=c++17 -Wall -pedantic -Wno-long-long -O2 -g -Werror
SOURCES = $(wildcard *.cpp)

all: main

main: $(SOURCES) 
	$(CXX) $(CXXFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f main

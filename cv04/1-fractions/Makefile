CXX = g++
CXXFLAGS = -g -std=c++17 -Wall -pedantic
LDFLAGS =

all: compile

compile: test

run: test
	./test

test: test.o fraction.o
	$(CXX) test.o fraction.o -o test $(LDFLAGS)

test.o: test.cpp fraction.hpp
	$(CXX) $(CXXFLAGS) -c test.cpp -o test.o

fraction.o: fraction.cpp fraction.hpp
	$(CXX) $(CXXFLAGS) -c fraction.cpp -o fraction.o

clean:
	-rm -f *.o
	-rm -f test

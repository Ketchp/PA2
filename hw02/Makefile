CXX = g++
CXXFLAGS= -std=c++14 -Wall -pedantic -Wno-long-long -O2 -g -Werror
SOURCES = $(wildcard *.cpp)

all: main

main: $(SOURCES) 
	$(CXX) $(CXXFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f main

#OBJ = parse.o

#all: $(TARGET) test

#$(TARGET).o: $(TARGET).cpp
#	$(CXX) -c -o $@ $< $(CXXFLAGS)

#$(TARGET): $(TARGET).o $(OBJ)
#	$(CXX) -o $@ $^ $(LDFLAGS)

#test: testScript
#	@./testScript

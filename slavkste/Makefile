CXX = g++
CXX_FLAGS = -O2 -Wall -std=c++17 -pedantic -Wno-long-long -Werror

LXX_FLAGS = -I/usr/include -L/usr/lib/x86_64-linux-gnu

LIBS = -lglut -lGLU -lGL

OBJ_DIR = obj
TARGET = slavkste

SRC_DIR = src
DEPS = $(TARGET)

rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,$(SRC_DIR),*.cpp)
OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))

all: compile doc

deps:
	mkdir -p $(OBJ_DIR)
	g++ -MM $(SRC_DIR)/*.cpp > Makefile.d

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) -c -o $@ $< $(CXX_FLAGS)

compile: deps
	+make $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(LXX_FLAGS) -o $(TARGET) $^ $(LIBS)

.PHONY: test
test: compile
	g++ -o examples/tests/tester -g -Wall -std=c++17 -pedantic \
 -Wno-long-long -Werror $(OBJ_DIR)/jsonParser.o $(OBJ_DIR)/linearAlgebra.o examples/tests/tester.cpp
	( cd ./examples/tests && exec valgrind --leak-check=full ./tester )


.PHONY: run
run:
	./$(TARGET)

.PHONY: doc
doc: $(HEADERS) $(SRC) Doxyfile README.md
	doxygen

.PHONY: clean
clean:
	rm -f Makefile.d
	rm -rf $(OBJ_DIR)
	rm -rf doc
	rm -f $(TARGET)
	rm -f examples/tests/tester

-include Makefile.d

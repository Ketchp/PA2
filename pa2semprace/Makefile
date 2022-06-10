CXX = g++
CXXFLAGS = -g -Wall -std=c++17 -pedantic -Wno-long-long -Werror -fsanitize=address

LXXFALGS = -I/usr/include -L/usr/lib/x86_64-linux-gnu -fsanitize=address

LIBS = -lglut -lGLU -lGL

TARGET_DIR = slavkste
TARGET = main

SRC_DIR = src
DEPS = $(TARGET)

all: deps
	mkdir -p $(TARGET_DIR)
	+make $(TARGET_DIR)/$(TARGET)

deps:
	g++ -MM $(SRC_DIR)/*.cpp > Makefile.d

$(TARGET_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)


$(TARGET_DIR)/$(TARGET): \
$(TARGET_DIR)/circle.o \
$(TARGET_DIR)/complexObject.o \
$(TARGET_DIR)/forceField.o \
$(TARGET_DIR)/game.o \
$(TARGET_DIR)/jsonParser.o \
$(TARGET_DIR)/levelLoader.o \
$(TARGET_DIR)/linearAlgebra.o \
$(TARGET_DIR)/main.o \
$(TARGET_DIR)/manifold.o \
$(TARGET_DIR)/object.o \
$(TARGET_DIR)/painter.o \
$(TARGET_DIR)/physicsAttributes.o \
$(TARGET_DIR)/physicsEngine.o \
$(TARGET_DIR)/physicsObject.o \
$(TARGET_DIR)/rectangle.o \
$(TARGET_DIR)/tags.o \
$(TARGET_DIR)/text.o \
$(TARGET_DIR)/window.o
	$(CXX) $(LXXFALGS) -o $@ $^ $(LIBS)

.PHONY: test
test: \
$(TARGET_DIR)/jsonParser.o \
$(TARGET_DIR)/linearAlgebra.o
	g++ -o examples/tests/tester -g -Wall -std=c++17 -pedantic \
 -Wno-long-long -Werror $^ examples/tests/tester.cpp
	( cd ./examples/tests && exec ./tester )

.PHONY: run
run:
	./$(TARGET_DIR)/$(TARGET)

.PHONY: doc
doc:
	echo TODO

.PHONY: clean
clean:
	rm -rf $(TARGET_DIR)
	rm -f Makefile.d


-include Makefile.d

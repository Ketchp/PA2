CXX = g++
CXXFLAGS = -g -std=c++17 -Wall -pedantic
LDFLAGS =
COPY_FLAG = 0

all: compile

compile: test

.PHONY: test.o

valgrind: test
	valgrind ./test

run: test
	./test

copy: set_flag valgrind

set_flag:
	$(eval COPY_FLAG=1 )

test: test.o CExam.o CList.o CTeacher.o SStudent.o string.o
	$(CXX) test.o CExam.o CList.o CTeacher.o SStudent.o string.o -o test $(LDFLAGS)

test.o: test.cpp CTeacher.hpp string.hpp CExam.hpp CList.hpp SStudent.hpp
	$(CXX) $(CXXFLAGS) -c test.cpp -D "COPY_ALLOWED=$(COPY_FLAG)" -o test.o

CExam.o: CExam.cpp CExam.hpp CTeacher.hpp string.hpp CList.hpp SStudent.hpp
	$(CXX) $(CXXFLAGS) -c CExam.cpp -o CExam.o

CList.o:  CList.cpp CList.hpp SStudent.hpp string.hpp
	$(CXX) $(CXXFLAGS) -c CList.cpp -o CList.o

CTeacher.o: CTeacher.cpp CTeacher.hpp string.hpp
	$(CXX) $(CXXFLAGS) -c CTeacher.cpp -o CTeacher.o

SStudent.o: SStudent.cpp SStudent.hpp string.hpp
	$(CXX) $(CXXFLAGS) -c SStudent.cpp -o SStudent.o

string.o: string.cpp string.hpp
	$(CXX) $(CXXFLAGS) -c string.cpp -o string.o

clean:
	-rm -f *.o test

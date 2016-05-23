CXX=g++
CXXFLAGS=-Wall -Wextra -g -std=c++11 -O2

all: test

clean:
	$(RM) *.o
	$(RM) test

test: test.cpp FileIODevice.o FileSystem.o MemIODevice.o

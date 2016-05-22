CXX=g++
CXXFLAGS=-Wall -Wextra -g

all: test

clean:
	$(RM) *.o
	$(RM) test

test: test.cpp FileIODevice.o FileSystem.o

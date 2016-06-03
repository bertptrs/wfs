CXX=g++
CXXFLAGS=-Wall -Wextra -g -std=c++11 -O2

OBJS = FileIODevice.o \
	   FileSystem.o \
	   MemIODevice.o \
	   BlockTable.o

all: wfs

wfs: wfs.cpp $(OBJS)

test: runner
	./$<

clean:
	$(RM) *.o
	$(RM) test

runner: runner.cpp $(OBJS)

runner.cpp: $(wildcard tests/*.hpp)
	cxxtestgen --error-printer -o $@ $^

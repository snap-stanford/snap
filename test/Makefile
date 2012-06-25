#
# Makefile for non-Microsoft compilers
#

CC = g++
CXXFLAGS += -std=c++98 -Wall
CXXFLAGS += -O3

## Main application file
MAIN = run-all-tests
TESTS = test-TUNGraph.cpp
DEPH = 
DEPCPP =
LIBS += -lrt -lproc -lsnap -lgtest

all: $(MAIN)

# COMPILE
$(MAIN): $(MAIN).cpp $(DEPH) $(DEPCPP)
	$(CC) $(CXXFLAGS) -o $(MAIN) $(MAIN).cpp $(TESTS) $(DEPCPP) $(LDFLAGS) $(LIBS)

clean:
	rm -f *.o $(MAIN) $(MAIN).exe
	rm -rf Debug Release
	rm -rf *.graph


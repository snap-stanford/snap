#
# Makefile for non-Microsoft compilers
#

CC = g++
CXXFLAGS += -std=c++98 -Wall
CXXFLAGS += -O3

## Main application file
DEMOS = demo-TUNGraph
DEPH = 
DEPCPP =
LIBS += -lrt -lproc -lsnap -lgtest

all: $(DEMOS)

# COMPILE
demo-TUNGraph: demo-TUNGraph.cpp
	$(CC) $(CXXFLAGS) -o demo-TUNGraph demo-TUNGraph.cpp $(DEPCPP) $(LDFLAGS) $(LIBS)

clean:
	rm -f *.o $(DEMOS) $(DEMOS).exe
	rm -rf Debug Release
	rm -rf *.graph


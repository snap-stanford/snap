#
# Makefile for non-Microsoft compilers
#

CC = g++
CXXFLAGS += -std=c++98 -Wall
CXXFLAGS += -O3

## Main application file
DEMOS = \
	demo-TUNGraph demo-TNGraph demo-TNEGraph \
	demo-TNodeNet demo-TNodeEDatNet
DEPH = 
DEPCPP =
LIBS += -lrt -lproc -lsnap -lgtest

all: $(DEMOS)

# COMPILE
demo-TUNGraph: demo-TUNGraph.cpp
	$(CC) $(CXXFLAGS) -o demo-TUNGraph demo-TUNGraph.cpp $(DEPCPP) $(LDFLAGS) $(LIBS)

demo-TNGraph: demo-TNGraph.cpp
	$(CC) $(CXXFLAGS) -o demo-TNGraph demo-TNGraph.cpp $(DEPCPP) $(LDFLAGS) $(LIBS)

demo-TNEGraph: demo-TNEGraph.cpp
	$(CC) $(CXXFLAGS) -o demo-TNEGraph demo-TNEGraph.cpp $(DEPCPP) $(LDFLAGS) $(LIBS)

demo-TNodeNet: demo-TNodeNet.cpp
	$(CC) $(CXXFLAGS) -o demo-TNodeNet demo-TNodeNet.cpp $(DEPCPP) $(LDFLAGS) $(LIBS)

demo-TNodeEDatNet: demo-TNodeEDatNet.cpp
	$(CC) $(CXXFLAGS) -o demo-TNodeEDatNet demo-TNodeEDatNet.cpp $(DEPCPP) $(LDFLAGS) $(LIBS)

clean:
	rm -f *.o $(DEMOS) $(DEMOS).exe
	rm -rf Debug Release
	rm -rf *.graph


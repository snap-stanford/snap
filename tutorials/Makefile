#
# Makefile for non-Microsoft compilers
#	tested only on Linux

include ../Makefile.config

## Main application file
DEMOS = \
	demo-TUNGraph demo-TNGraph demo-TNEGraph \
	demo-TNodeNet demo-TNodeEDatNet demo-TNodeEdgeNet \
	demo-subgraph

all: $(DEMOS)

# COMPILE
demo-TUNGraph: demo-TUNGraph.cpp $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o demo-TUNGraph demo-TUNGraph.cpp $(CSNAP)/Snap.o -I$(CSNAP) $(LDFLAGS) $(LIBS)

demo-TNGraph: demo-TNGraph.cpp $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o demo-TNGraph demo-TNGraph.cpp $(CSNAP)/Snap.o -I$(CSNAP) $(LDFLAGS) $(LIBS)

demo-TNEGraph: demo-TNEGraph.cpp $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o demo-TNEGraph demo-TNEGraph.cpp $(CSNAP)/Snap.o -I$(CSNAP) $(LDFLAGS) $(LIBS)

demo-TNodeNet: demo-TNodeNet.cpp $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o demo-TNodeNet demo-TNodeNet.cpp $(CSNAP)/Snap.o -I$(CSNAP) $(LDFLAGS) $(LIBS)

demo-TNodeEDatNet: demo-TNodeEDatNet.cpp $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o demo-TNodeEDatNet demo-TNodeEDatNet.cpp $(CSNAP)/Snap.o -I$(CSNAP) $(LDFLAGS) $(LIBS)

demo-TNodeEdgeNet: demo-TNodeEdgeNet.cpp $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o demo-TNodeEdgeNet demo-TNodeEdgeNet.cpp $(CSNAP)/Snap.o -I$(CSNAP) $(LDFLAGS) $(LIBS)

demo-subgraph: demo-subgraph.cpp $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o demo-subgraph demo-subgraph.cpp $(CSNAP)/Snap.o -I$(CSNAP) $(LDFLAGS) $(LIBS)

$(CSNAP)/Snap.o:
	$(MAKE) -C $(CSNAP)

clean:
	rm -f *.o $(DEMOS) $(DEMOS).exe
	rm -rf Debug Release
	rm -rf *.Err *.graph *.net


#
# Makefile for non-Microsoft compilers
#	tested only on Linux

include ../Makefile.config

## Main application file
DEMOS = \
	demo-TUNGraph demo-TNGraph demo-TNEGraph \
	demo-TNodeNet demo-TNodeEDatNet demo-TNodeEdgeNet \
	demo-subgraph demo-ggen \
	demo-gio demo-gviz demo-cncom

all: $(DEMOS)

# COMPILE
demo-TUNGraph: demo-TUNGraph.cpp $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o demo-TUNGraph demo-TUNGraph.cpp $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

demo-TNGraph: demo-TNGraph.cpp $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o demo-TNGraph demo-TNGraph.cpp $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

demo-TNEGraph: demo-TNEGraph.cpp $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o demo-TNEGraph demo-TNEGraph.cpp $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

demo-TNodeNet: demo-TNodeNet.cpp $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o demo-TNodeNet demo-TNodeNet.cpp $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

demo-TNodeEDatNet: demo-TNodeEDatNet.cpp $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o demo-TNodeEDatNet demo-TNodeEDatNet.cpp $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

demo-TNodeEdgeNet: demo-TNodeEdgeNet.cpp $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o demo-TNodeEdgeNet demo-TNodeEdgeNet.cpp $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

demo-subgraph: demo-subgraph.cpp $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o demo-subgraph demo-subgraph.cpp $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)
  
demo-ggen: demo-ggen.cpp $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o demo-ggen demo-ggen.cpp $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

demo-gio: demo-gio.cpp $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o demo-gio demo-gio.cpp $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

demo-gviz: demo-gviz.cpp $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o demo-gviz demo-gviz.cpp $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

demo-cncom: demo-cncom.cpp $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o demo-cncom demo-cncom.cpp $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)


$(CSNAP)/Snap.o:
	$(MAKE) -C $(CSNAP)

clean:
	rm -f *.o $(DEMOS) $(DEMOS).exe
	rm -rf Debug Release
	rm -rf *.Err *.graph *.net


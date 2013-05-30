#
# Makefile for non-Microsoft compilers
#	tested only on Linux

include ../Makefile.config

## Main application file
DEMOS = \
	demo-TUNGraph demo-TNGraph demo-TNEGraph \
	demo-TNodeNet demo-TNodeEDatNet demo-TNodeEdgeNet \
        demo-TNEANet \
	demo-subgraph demo-ggen \
	demo-gio demo-gviz \
	demo-cncom demo-bfsdfs \
	demo-triad \
	demo-THash \
	demo-topology-benchmark \
	demo-hashvec-benchmark \
	\

all: $(DEMOS)

# COMPILE
demo-%: demo-%.cpp $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

$(CSNAP)/Snap.o:
	$(MAKE) -C $(CSNAP)

clean:
	rm -f *.o $(DEMOS) *.exe
	rm -rf Debug Release
	rm -rf *.Err demo*.dat


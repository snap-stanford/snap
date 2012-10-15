#
# Makefile for non-Microsoft compilers
# 	tested only on Linux
#

include ../Makefile.config
LIBS += -lgtest

## Main application file
MAIN = run-all-tests
TESTS = \
	test-TUNGraph.cpp test-TNGraph.cpp test-TNEGraph.cpp \
	test-TNodeNet.cpp test-TNodeEDatNet.cpp test-TNodeEdgeNet.cpp \
	test-subgraph.cpp \
	test-TStrPool.cpp \
	test-TSysTm.cpp

all: $(MAIN)

# COMPILE
$(MAIN): $(MAIN).cpp $(TESTS) $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $(MAIN) $(MAIN).cpp $(TESTS) $(CSNAP)/Snap.o -I$(CSNAP) $(LDFLAGS) $(LIBS)

$(CSNAP)/Snap.o:
	$(MAKE) -C $(CSNAP)

test: $(MAIN)
	./$(MAIN)

clean:
	rm -f *.o $(MAIN) $(MAIN).exe
	rm -rf Debug Release
	rm -rf *.graph *.pool *.net *.Err


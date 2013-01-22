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
	test-TSysTm.cpp \
	test-helper.cpp \
	test-ggen.cpp \
	test-gio.cpp \
	test-cncom.cpp \
	test-bfsdfs.cpp \
	test-THash.cpp

	#test-gviz.cpp \

all: $(MAIN)
run: test

# COMPILE
$(MAIN): $(MAIN).cpp $(TESTS) $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $(MAIN) $(MAIN).cpp $(TESTS) $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

$(CSNAP)/Snap.o:
	$(MAKE) -C $(CSNAP)

test: $(MAIN)
	./$(MAIN)

clean:
	rm -f *.o $(MAIN) $(MAIN).exe
	rm -rf Debug Release
	rm -rf test*.dat *.Err


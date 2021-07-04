#
# Makefile for non-Microsoft compilers
# 	tested only on Linux, Mac OS X
#
# Prerequisites:
#	- download google test: https://code.google.com/p/googletest/
#
# Compile and run all the tests:
#	make run
#
# Note:
#   graphviz test files were generated with graphviz version 2.38.0

include ../Makefile.config
CXXFLAGS += $(CXXOPENMP)
LIBS += -lgtest
# use the following line for linking the static library
#LIBS += $(HOME)/miniconda3/lib/libgtest.a

## Main application file
MAIN = run-all-tests

TEST_SRCS = \
	test-helper.cpp \
	test-TUNGraph.cpp test-TNGraph.cpp \
	test-TNEGraph.cpp test-TNEANet.cpp \
	test-TNodeNet.cpp test-TNodeEDatNet.cpp test-TNodeEdgeNet.cpp \
	test-TTable.cpp \
	test-TUndirNet.cpp test-TDirNet.cpp \
	test-TMMNet.cpp \
	test-TModeNet.cpp \
	test-TCrossNet.cpp \
	test-multimodal.cpp \
	test-subgraph.cpp \
	test-TStrPool.cpp \
	test-TSysTm.cpp \
	test-ggen.cpp \
	test-gio.cpp \
	test-gviz.cpp \
	test-cncom.cpp \
	test-bfsdfs.cpp \
	test-alg.cpp \
	test-triad.cpp \
	test-THash.cpp \
	test-THashSet.cpp \
	test-TAttr.cpp \
	test-flow.cpp \
	test-randwalk.cpp \
	test-priority-queue.cpp \
	test-sim.cpp \
	test-TVec.cpp

TEST_OBJS = $(TEST_SRCS:.cpp=.o)

all: $(MAIN)
run: test

# COMPILE
.cpp.o:
	$(CC) $(CXXFLAGS) -I$(CSNAP) -I$(CGLIB) -c $<

$(MAIN): $(MAIN).o $(TEST_OBJS) $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $(MAIN) $^ -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

$(CSNAP)/Snap.o:
	$(MAKE) -C $(CSNAP)

test: $(MAIN)
	./$(MAIN)

clean:
	rm -f *.o $(MAIN) $(MAIN).exe
	rm -rf Debug Release
	rm -rf demo*.dat test*.dat *.Err
	rm -rf graphviz/test_*
	rm -rf table/p1.txt


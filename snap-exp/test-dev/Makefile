#
# Makefile for non-Microsoft compiler
#	tested only on Linux
#
include ../../Makefile.config

HEADER = Engine.h
CPP = Engine.cpp

TEST = \
  ExplicitStringTableTest1 ExplicitStringTableTest3 ExplicitStringTableTest5 \
  PhysicalOrderSortTest1 ParallelGroupByTest \
  TablePageRank LoadTest FilterTest AggrTest GroupTest IdTest ArithOpsTest \
  SetOperationsTest TableTest

#TEST = RowBasedTableTest
#TEST = ExplicitStringTableTest2
#TEST = ExplicitStringTableTest4
#TEST = ExplicitStringTableTest6
#TEST = PhysicalOrderSortTest2
#TEST = ThresholdJoinTest 

all: $(TEST)

# COMPILE:
ThresholdJoinTest: ThresholdJoinTest.cpp $(EXSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)
	
ParallelGroupByTest: ParallelGroupByTest.cpp $(EXSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)
	
TablePageRank: TablePageRank.cpp $(EXSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

ExplicitStringTableTest%: ExplicitStringTableTest%.cpp $(EXSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)
	
PhysicalOrderSortTest%: PhysicalOrderSortTest%.cpp $(EXSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)
	
GenerateMockTsv: GenerateMockTsv.cpp $(EXSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)
	
#RowBasedTableTest: RowBasedTableTest.cpp $(EXSNAP)/Snap.o
#	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)
	
LoadTest: LoadTest.cpp $(EXSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

FilterTest: FilterTest.cpp $(EXSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

SimJoinTest: SimJoinTest.cpp $(EXSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

SimJoinPerGroupTest: SimJoinPerGroupTest.cpp $(EXSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

AggrTest: AggrTest.cpp $(EXSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

ToGraphTest: ToGraphTest.cpp $(EXSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

ToGraphSeqTest: ToGraphSeqTest.cpp $(EXSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

GroupTest: GroupTest.cpp $(EXSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

IdTest: IdTest.cpp $(EXSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

ArithOpsTest: ArithOpsTest.cpp $(EXSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

SetOperationsTest: SetOperationsTest.cpp $(EXSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

GraphOperationsTest: GraphOperationsTest.cpp $(EXSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

TableTes%: TableTes%.cpp $(EXSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

Benchmark: Benchmark.cpp $(EXSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

BenchmarkAshton: BenchmarkAshton.cpp $(EXSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)


PARALLEL = ParJoinTest ParSelectTest ParSelect2Test ParGroupAggrTest ParToGraphTest

par: $(PARALLEL)

ParJoinTest: ParJoinTest.cpp $(EXSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

ParSelectTest: ParSelectTest.cpp $(EXSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

ParSelect2Test: ParSelect2Test.cpp $(EXSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

ParGroupAggrTest: ParGroupAggrTest.cpp $(EXSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

ParToGraphTest: ParToGraphTest.cpp $(EXSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

ParLargeGraph: ParLargeGraph.cpp $(EXSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

testTriangleCount: testTriangleCount.cpp  $(EXSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

testPageRank: testPageRank.cpp  $(EXSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $@ $@.cpp $(EXSNAP)/Snap.o -I$(EXSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

$(EXSNAP)/Snap.o:
	$(MAKE) -C $(EXSNAP)

clean:
	rm -f *.o $(TEST) $(PARALLEL) *.exe *.out testTriangleCount testPageRank
	rm -rf Debug Release
	rm -rf *.Err demo*.dat
	$(MAKE) clean -C $(EXSNAP)


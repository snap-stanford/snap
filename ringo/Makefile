#
# Makefile for non-Microsoft compilers
#	tested only on Linux

include ../Makefile.config

HEADER = Engine.h
CPP = Engine.cpp

TEST = GroupTest IdTest ArithOpsTest SetOperationsTest TableTest TableTest1 Benchmark BenchmarkAshton

all: $(TEST)

# COMPILE
GroupTest: GroupTest.cpp $(CSNAP)/Snap.o
	$(CC) -o $@ $@.cpp $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

IdTest: IdTest.cpp $(CSNAP)/Snap.o
	$(CC) -o $@ $@.cpp $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

ArithOpsTest: ArithOpsTest.cpp $(CSNAP)/Snap.o
	$(CC) -o $@ $@.cpp $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

SetOperationsTest: SetOperationsTest.cpp $(CSNAP)/Snap.o
	$(CC) -o $@ $@.cpp $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

TableTes%: TableTes%.cpp $(CSNAP)/Snap.o
	$(CC) -o $@ $@.cpp $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

Benchmark: Benchmark.cpp $(CSNAP)/Snap.o
	$(CC) -o $@ $@.cpp $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)
	
BenchmarkAshton: BenchmarkAshton.cpp $(CSNAP)/Snap.o
	$(CC) -o $@ $@.cpp $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

$(CSNAP)/Snap.o:
	$(MAKE) -C $(CSNAP)

clean:
	rm -f *.o $(TEST) *.exe *.out
	rm -rf Debug Release
	rm -rf *.Err demo*.dat

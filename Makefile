#
# Makefile for non-Microsoft compilers
#	tested only on Linux

include ../Makefile.config

HEADER = Engine.h Predicate.h TMetric.h Table.h
CPP = Engine.cpp Predicate.cpp Table.cpp

TEST = TableTest TableTest1 Benchmark

all: $(TEST)

# COMPILE
TableTes%: TableTes%.cpp Engine.o
	$(CC) -o $@ $@.cpp Engine.o $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

Benchmark: Benchmark.cpp Engine.o
	$(CC) -o $@ $@.cpp Engine.o $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

$(CSNAP)/Snap.o:
	$(MAKE) -C $(CSNAP)

Engine.o: $(HEADER) $(CPP) $(CSNAP)/Snap.o
	$(CC) -c $(CXXFLAGS) Engine.cpp -I$(CSNAP) -I$(CGLIB)

lib: Engine.o
	rm -f libengine.a
	ar -cvq libengine.a Engine.o

libinstall: lib
	cp libengine.a /usr/local/lib/libenginea

clean:
	rm -f *.o $(TEST) *.exe *.out
	rm -rf Debug Release
	rm -rf *.Err demo*.dat

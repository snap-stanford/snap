#
# Makefile for non-Microsoft compilers
#	tested only on Linux

include ../Makefile.config
MAIN = Benchmark

all: $(MAIN)

# COMPILE
$(MAIN): *.h *.cpp $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -o $(MAIN) $(MAIN).cpp Table.cpp Predicate.cpp $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

$(CSNAP)/Snap.o:
	$(MAKE) -C $(CSNAP)

clean:
	rm -f *.o $(MAIN) *.exe
	rm -rf Debug Release
	rm -rf *.Err demo*.dat

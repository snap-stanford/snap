#
# Makefile for non-Microsoft compilers
#	tested only on Linux

include ../Makefile.config

all: Table

# COMPILE
Table: *.h *.cpp $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -c $@.cpp $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

$(CSNAP)/Snap.o:
	$(MAKE) -C $(CSNAP)

clean:
	rm -f *.o Table *.exe
	rm -rf Debug Release
	rm -rf *.Err demo*.dat

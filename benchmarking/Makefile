#
# Makefile for non-Microsoft compilers
#	tested only on Linux

include ../Makefile.config

## Main application file
DEMOS = \
  demo-snap-specialized \
  demo-snap-specialized-smart-pointers \
	demo-topology-benchmark

all: $(DEMOS)

# COMPILE
demo-%: demo-%.cpp $(CSNAP)/Snap.o
	$(CC) $(CXXFLAGS) -lproc -o $@ $@.cpp $(CSNAP)/Snap.o -I$(CSNAP) -I$(CGLIB) $(LDFLAGS) $(LIBS)

$(CSNAP)/Snap.o:
	$(MAKE) -C $(CSNAP)

clean:
	rm -f *.o $(DEMOS) *.exe
	rm -rf Debug Release
	rm -rf *.Err demo*.dat


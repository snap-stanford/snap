#
# Makefile for non-Microsoft compilers
#

GLIB = ../../glib-core
SNAP = ../../snap-core

## Linux  (uncomment the 2 lines below for compilation on Linux)
CXXFLAGS += -std=c++98 -Wall
LDFLAGS += -lrt

## CygWin (uncomment the 2 lines below for compilation on CygWin)
#CXXFLAGS += -Wall
#LDFLAGS += 

## Main application file
MAIN = ncpplot

all: $(MAIN)

opt: CXXFLAGS += -O4
opt: LDFLAGS += -O4
opt: $(MAIN)

# COMPILE
$(MAIN): $(MAIN).cpp $(SNAP)/ncp.cpp $(SNAP)/Snap.o
	g++ -o $(MAIN) $(MAIN).cpp $(SNAP)/ncp.cpp $(SNAP)/Snap.o -I$(GLIB) -I$(SNAP) $(LDFLAGS)
	
$(SNAP)/Snap.o: 
	make -C $(SNAP)
	#g++ -c $(CXXFLAGS) $(SNAP)/Snap.cpp -I$(GLIB) -I$(SNAP)

clean:
	rm -f *.o  $(MAIN)  $(MAIN).exe
	rm -rf Debug Release

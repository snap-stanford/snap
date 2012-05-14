#
# Makefile for non-Microsoft compilers
#

## Linux  (uncomment the 2 lines below for compilation on Linux)
#CXXFLAGS += -std=c++98 -Wall
#LDFLAGS += -lrt

## CygWin (uncomment the 2 lines below for compilation on CygWin)
CXXFLAGS += -Wall
LDFLAGS += 

## Main application file
MAIN = cascades

all: $(MAIN)

opt: CXXFLAGS += -O4
opt: LDFLAGS += -O4
opt: $(MAIN)

# COMPILE
$(MAIN): $(MAIN).cpp Snap.o 
	g++ -o $(MAIN) $(MAIN).cpp Snap.o -I../../glib-core -I../../snap-core 	 $(LDFLAGS)
Snap.o: 
	g++ -c $(CXXFLAGS) ../../snap-core/Snap.cpp -I../../glib-core -I../../snap-core

clean:
	rm -f *.o  $(MAIN)  $(MAIN).exe
	rm -rf Debug Release

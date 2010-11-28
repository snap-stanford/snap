#
# MemeTracker by Jure Leskovec
#
# Makefile for testing with non-Microsoft compilers
#

## Linux  (uncomment the 2 lines below for compilation on Linux)
CXXFLAGS += -std=c++98 -Wall
LDFLAGS += -lrt

## CygWin (uncomment the 2 lines below for compilation on CygWin)
#CXXFLAGS += -Wall
#LDFLAGS += 

SNAP = ../../libs/snap
GLIB = ../../libs/glib

all: MemeWorks

opt: CXXFLAGS += -O4
opt: LDFLAGS += -O4
opt: MemeWorks

# COMPILE
MemeWorks: MemeWorks.cpp Snap.o memes.o spinn3r.o
	g++ $(LDFLAGS) -o MemeWorks MemeWorks.cpp memes.o spinn3r.o Snap.o -I$(GLIB) -I$(SNAP)

memes.o: $(SNAP)/memes.cpp $(SNAP)/memes.h
	g++ -c $(CXXFLAGS) $(SNAP)/memes.cpp -I$(GLIB) -I$(SNAP)

spinn3r.o: $(SNAP)/spinn3r.cpp $(SNAP)/spinn3r.h
	g++ -c $(CXXFLAGS) $(SNAP)/spinn3r.cpp -I$(GLIB) -I$(SNAP)
	
Snap.o: 
	g++ -c $(CXXFLAGS) $(SNAP)/Snap.cpp -I$(GLIB) -I$(SNAP)

clean:
	rm -f *.o  MemeWorks  MemeWorks.exe


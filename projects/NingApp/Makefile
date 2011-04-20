#
# MemeTracker by Jure Leskovec
#
# Makefile for testing with non-Microsoft compilers
#

## Linux  (uncomment the 2 lines below for compilation on Linux)
#CXXFLAGS += -std=c++98 -Wall
#LDFLAGS += -lrt

## CygWin (uncomment the 2 lines below for compilation on CygWin)
#CXXFLAGS += -Wall
#LDFLAGS += 

SNAP = ../../core/snap
GLIB = ../../core/glib

all: NingApp

opt: CXXFLAGS += -O4
opt: LDFLAGS += -O4
opt: NingApp

# COMPILE
NingApp: NingApp.cpp Snap.o 
	g++ $(LDFLAGS) -o NingApp NingApp.cpp Snap.o -I$(GLIB) -I$(SNAP)

#NingApp: NingApp.cpp Snap.o ning.o
#	g++ $(LDFLAGS) -o NingApp NingApp.cpp ning.o Snap.o -I$(GLIB) -I$(SNAP)

ning.o: ning.cpp ning.h
	g++ -c $(CXXFLAGS) ning.cpp -I$(GLIB) -I$(SNAP)
	
Snap.o: 
	g++ -c $(CXXFLAGS) $(SNAP)/Snap.cpp -I$(GLIB) -I$(SNAP)

clean:
	rm -f *.o  NingApp  NingApp.exe


#
# Small makefile for testing with non-Microsoft compilers
#

## Linux (uncomment if using linux)
#XXFLAGS += -std=c++98 -Wall
#LDFLAGS += -lrt

## CygWin (uncomment if using cygwin)
CXXFLAGS += -Wall
LDFLAGS += 

CC = g++

all: testSnap

testSnap: testSnap.cpp Snap.o 
	$(CC) -o testSnap testSnap.cpp Snap.o -I../glib-core $(LDFLAGS) 

Snap.o:
	$(CC) -c $(CXXFLAGS) Snap.cpp -I../glib-core

clean:
	rm -f *.o testSnap testSnap.exe *.plt *.tab *.png

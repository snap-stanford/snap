#
# Small makefile for testing with non-Microsoft compilers
#

GLIB = glib-core
SNAP = snap-core

CGLIB = ../$(GLIB)
CSNAP = ../$(SNAP)

EXSNAP = ../../$(SNAP)

UNAME := $(shell uname)
UNAMEO := $(shell uname -o)

ifeq ($(UNAME), Linux)
  # Linux flags
  CC = g++
  CXXFLAGS += -std=c++98 -Wall
  LDFLAGS += 
  LIBS += -lrt

else ifeq ($(UNAME), Darwin)
  # OS X flags
  CC = g++
  CXXFLAGS += -std=c++98 -Wall
  LDFLAGS += 
  LIBS += 

else ifeq ($(UNAMEO), Cygwin)
  # Cygwin flags
  CC = g++
  CXXFLAGS += -Wall
  LDFLAGS += 
  LIBS += 

endif

all: testSnap

testSnap: testSnap.cpp Snap.o 
	$(CC) -o testSnap testSnap.cpp Snap.o $(LDFLAGS) $(LIBS)

Snap.o:
	$(CC) -c $(CXXFLAGS) Snap.cpp -I$(CGLIB)

clean:
	rm -f *.o testSnap testSnap.exe *.plt *.tab *.png


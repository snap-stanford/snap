#
# Small makefile for testing with non-Microsoft compilers
#

GLIB = "../glib-core"

UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
  # Linux flags
  CXXFLAGS += -std=c++98 -Wall
  LDFLAGS += -lrt
else ifeq ($(UNAME), Darwin)
  # OS X flags
  CXXFLAGS += -std=c++98 -Wall
endif

## CygWin (uncomment if using cygwin)
#CXXFLAGS += -Wall
#LDFLAGS += 

CC = g++

all: testSnap

testSnap: testSnap.cpp Snap.o 
	$(CC) -o testSnap testSnap.cpp Snap.o -I$(GLIB) $(LDFLAGS) 

Snap.o:
	$(CC) -c $(CXXFLAGS) Snap.cpp -I$(GLIB)

clean:
	rm -f *.o testSnap testSnap.exe *.plt *.tab *.png

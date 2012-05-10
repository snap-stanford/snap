#
# Makefile for non-Microsoft compilers
#

## Linux  (uncomment the 2 lines below for compilation on Linux)
#CXXFLAGS += -std=c++98 -Wall
#LDFLAGS += -lrt

## CygWin (uncomment the 2 lines below for compilation on CygWin)
#CXXFLAGS += -Wall
#LDFLAGS += 

## Main application file
MAIN = agmgen

all: $(MAIN)

opt: CXXFLAGS += -O4
opt: LDFLAGS += -O4
opt: $(MAIN)

# COMPILE
$(MAIN): $(MAIN).cpp ../../snap/agm.h ../../snap/agm.cpp Snap.o 
	g++ -o $(MAIN) $(MAIN).cpp ../../snap/agm.cpp Snap.o -I../../glib -I../../snap $(LDFLAGS)
	
Snap.o: 
	g++ -c $(CXXFLAGS) ../../snap/Snap.cpp -I../../glib -I../../snap

clean:
	rm -f *.o  $(MAIN)  $(MAIN).exe
	rm -rf Debug Release

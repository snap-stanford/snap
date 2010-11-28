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
MAIN = cliques

all: $(MAIN)

opt: CXXFLAGS += -O4
opt: LDFLAGS += -O4
opt: $(MAIN)

# COMPILE
$(MAIN): $(MAIN)main.cpp ../../snap/cliques.h ../../snap/cliques.cpp Snap.o 
	g++ $(LDFLAGS) -o $(MAIN) $(MAIN)main.cpp ../../snap/cliques.cpp Snap.o -I../../glib -I../../snap
	
Snap.o: 
	g++ -c $(CXXFLAGS) ../../snap/Snap.cpp -I../../glib -I../../snap

clean:
	rm -f *.o  $(MAIN)  $(MAIN).exe
	rm -rf Debug Release

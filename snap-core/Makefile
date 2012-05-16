#
# Small makefile for testing with non-Microsoft compilers
#

include ../Makefile.config

all: testSnap

testSnap: testSnap.cpp Snap.o 
	$(CC) -o testSnap testSnap.cpp Snap.o -I$(CGLIB) $(LDFLAGS) $(LIBS)

Snap.o:
	$(CC) -c $(CXXFLAGS) Snap.cpp -I$(CGLIB)

clean:
	rm -f *.o testSnap testSnap.exe *.plt *.tab *.png


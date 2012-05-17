#
# Small makefile for testing with non-Microsoft compilers
#

## Linux (uncomment if using linux)
CXXFLAGS += -std=c++98 -Wall
LDFLAGS += -lrt

## CygWin (uncomment if using cygwin)
#CXXFLAGS += -Wall
#LDFLAGS += 

#CC = g++ -g
CC = g++

all: compareGraphs testTaobao plotTaobao plotDegDist communityMining binning communityGraphing communityOverlay triangleTesting graphTest prediction

communityMining: communityMining.cpp Snap.o taobaonet.cpp BinTree.o IntFltTree.o BinaryHeap.o
	$(CC) $(LDFLAGS) -o communityMining communityMining.cpp taobaonet.cpp Snap.o BinTree.o IntFltTree.o BinaryHeap.o -I../GLib -I../SnapLib

testTaobao: testTaobao.cpp Snap.o taobaonet.cpp 
	$(CC) $(LDFLAGS) -o testTaobao testTaobao.cpp taobaonet.cpp Snap.o -I../GLib -I../SnapLib

compareGraphs: compareGraphs.cpp Snap.o taobaonet.cpp graphing.h
	$(CC) $(LDFLAGS) -o compareGraphs compareGraphs.cpp taobaonet.cpp Snap.o -I../GLib -I../SnapLib

graphTest: graphTest.cpp Snap.o graphing.h
	$(CC) $(LDFLAGS) -o graphTest graphTest.cpp Snap.o -I../GLib -I../SnapLib

prediction: prediction.cpp Snap.o taobaonet.cpp graphing.h
	$(CC) $(LDFLAGS) -o prediction prediction.cpp taobaonet.cpp Snap.o -I../GLib -I../SnapLib

triangleTesting: triangleTesting.cpp Snap.o taobaonet.cpp graphing.h
	$(CC) $(LDFLAGS) -o triangleTesting triangleTesting.cpp taobaonet.cpp Snap.o -I../GLib -I../SnapLib

featureGen: featureGen.cpp Snap.o taobaonet.cpp graphing.h
	$(CC) $(LDFLAGS) -o featureGen featureGen.cpp taobaonet.cpp Snap.o -I../GLib -I../SnapLib

selectSplit: selectSplit.cpp Snap.o taobaonet.cpp graphing.h
	$(CC) $(LDFLAGS) -o selectSplit selectSplit.cpp taobaonet.cpp Snap.o -I../GLib -I../SnapLib

binning:: binning.cpp Snap.o taobaonet.cpp
	$(CC) $(LDFLAGS) -o binning binning.cpp taobaonet.cpp Snap.o -I../GLib -I../SnapLib

communityGraphing:: communityGraphing.cpp Snap.o taobaonet.cpp
	$(CC) $(LDFLAGS) -o communityGraphing communityGraphing.cpp taobaonet.cpp Snap.o -I../GLib -I../SnapLib

communityOverlay:: communityOverlay.cpp Snap.o taobaonet.cpp
	$(CC) $(LDFLAGS) -o communityOverlay communityOverlay.cpp taobaonet.cpp Snap.o -I../GLib -I../SnapLib

communityStats:: communityStats.cpp Snap.o taobaonet.cpp
	$(CC) $(LDFLAGS) -o communityStats communityStats.cpp taobaonet.cpp Snap.o -I../GLib -I../SnapLib

plotTaobao: plotTaobao.cpp Snap.o taobaonet.h
	$(CC) $(LDFLAGS) -o plotTaobao plotTaobao.cpp taobaonet.cpp Snap.o -I../GLib -I../SnapLib

plotDegDist: plotDegDist.cpp Snap.o
	$(CC) $(LDFLAGS) -o plotDegDist plotDegDist.cpp Snap.o -I../GLib -I../SnapLib

BinTree.o: BinTree.cpp
	$(CC) -c $(CXXFLAGS) BinTree.cpp -I../GLib -I../SnapLib

IntFltTree.o: IntFltTree.cpp BinTree.o
	$(CC) -c $(CXXFLAGS) IntFltTree.cpp -I../GLib -I../SnapLib

BinaryHeap.o: BinaryHeap.cpp IntFltTree.o
	$(CC) -c $(CXXFLAGS) BinaryHeap.cpp -I../GLib -I../SnapLib

Snap.o: 
	$(CC) -c $(CXXFLAGS) ../SnapLib/Snap.cpp -I../GLib -I../SnapLib

clean:
	rm -f *.o communityMining compareGraphs plotTaobao testTaobao plotDegDist *.plt *.tab *.png *.pdf

tidy:
	rm -f *.plt *.tab *.png

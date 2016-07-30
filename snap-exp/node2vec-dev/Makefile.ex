#CXXFLAGS += -O3 -std=c++98 -Wall
#LDFLAGS += -lrt -O3
#MAIN = node2vec
#all: $(MAIN)

##compile
#$(MAIN): $(MAIN).cpp randomwalk.cpp word2vec.cpp Snap.o
#	g++ -o $(MAIN) $(MAIN).cpp Snap.o -I./glib -I./snap $(LDFLAGS)

#Snap.o:
#	g++ -c $(CXXFLAGS) ./snap/Snap.cpp -I./glib -I./snap
#
#clean:
#	rm -f $(MAIN) Snap.o
MAIN = node2vec
DEPH =
DEPCPP =

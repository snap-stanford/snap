#
# Small makefile for testing with non-Microsoft compilers
#

# gcc: g++, cygwin: g++, pathscale: pathcc, pgi: pgCC
CXX = g++

## CygWin
CXXFLAGS += -Wall
LDFLAGS += 

## Linux
# CXXFLAGS += -std=c++98
# LDFLAGS += -lrt

MAINOBJS = base.o mine.o testBase.o

all: CXXFLAGS += -g -ggdb
all: testBase

#opt: CXXFLAGS += -march=opteron -mfpmath=sse,387 -mmmx -msse -msse2 -msse3 -3dnow -m64 \
#	-O9 -fomit-frame-pointer -fprefetch-loop-arrays -fschedule-insns2 -ftree-loop-linear \
#	-ftree-loop-im -ftree-loop-ivcanon -fivopts -ftree-vectorize
opt: CXXFLAGS += -march=i686 -msse -mfpmath=sse,387 -fomit-frame-pointer -fschedule-insns2 \
	-ftree-loop-linear -ftree-loop-im -ftree-loop-ivcanon -fivopts -ftree-vectorize
opt: testBase

paranoid: CXXFLAGS += -Wextra -Wfloat-equal -Wundef -Wunsafe-loop-optimizations \
	-Wredundant-decls -Wpointer-arith -Winline -Wcast-qual \
	-Wcast-align -Wwrite-strings -Wconversion -Wsign-compare \
	-Wmissing-field-initializers -Wmissing-format-attribute -Wnormalized=nfc \
	-Winline -Wlong-long \
	-DFLTWARN
#	-Wshadow
#	-Wunreachable-code
#	-Wpadded
paranoid: testBase

testBase: $(MAINOBJS)
	$(CXX) $(LDFLAGS) -o testBase $(MAINOBJS)

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $<
	
clean:
	rm -f *.o *.gch testBase testBase.exe

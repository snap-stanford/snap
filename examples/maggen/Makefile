#
# Makefile for non-Microsoft compilers
#

include ../../Makefile.config
include Makefile.ex

all: $(MAIN)

# COMPILE
$(MAIN): $(MAIN).cpp $(DEPH) $(DEPCPP) $(EXSNAP)/Snap.o 
	$(CC) -o $(MAIN) $(MAIN).cpp $(DEPH) $(DEPCPP) $(EXSNAP)/Snap.o -I$(EXGLIB) -I$(EXSNAP) $(LDFLAGS) $(LIBS)

$(EXSNAP)/Snap.o: 
	make -C $(EXSNAP)

clean:
	rm -f *.o  $(MAIN)  $(MAIN).exe
	rm -rf Debug Release

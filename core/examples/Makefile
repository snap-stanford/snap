#
# Makefile for non-Microsoft compilers
#

## Linux  (uncomment the 2 lines below for compilation on Linux)
#CXXFLAGS += -std=c++98 -Wall
#LDFLAGS += -lrt

## CygWin (uncomment the 2 lines below for compilation on CygWin)
#CXXFLAGS += -Wall
#LDFLAGS += 

all: MakeAll

opt: CXXFLAGS += -O4
opt: LDFLAGS += -O4
opt: MakeAll

MakeAll:
	$(MAKE) -C cascades
	$(MAKE) -C centrality
	$(MAKE) -C community
	$(MAKE) -C concomp
	$(MAKE) -C forestfire
	$(MAKE) -C kcores
	$(MAKE) -C krongen
	$(MAKE) -C kronfit
	$(MAKE) -C mkdatasets
	$(MAKE) -C motifs
	$(MAKE) -C ncpplot
	$(MAKE) -C netevol
	$(MAKE) -C netstat
	$(MAKE) -C testgraph

clean:
	$(MAKE) clean -C cascades
	$(MAKE) clean -C centrality
	$(MAKE) clean -C community
	$(MAKE) clean -C concomp
	$(MAKE) clean -C forestfire
	$(MAKE) clean -C kcores
	$(MAKE) clean -C krongen
	$(MAKE) clean -C kronfit
	$(MAKE) clean -C mkdatasets
	$(MAKE) clean -C motifs
	$(MAKE) clean -C ncpplot
	$(MAKE) clean -C netevol
	$(MAKE) clean -C netstat
	$(MAKE) clean -C testgraph
	rm -rf Debug Release

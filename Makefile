#
# Makefile for non-Microsoft compilers
#

all: MakeAll

opt: CXXFLAGS += -O4
opt: LDFLAGS += -O4
opt: MakeAll

MakeAll:
	$(MAKE) -C snap-core
	$(MAKE) -C examples

clean:
	$(MAKE) clean -C snap-core
	$(MAKE) clean -C examples

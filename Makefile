#
# Makefile for non-Microsoft compilers
#

all: MakeAll

MakeAll:
	$(MAKE) -C snap-core
	$(MAKE) -C examples

clean:
	$(MAKE) clean -C snap-core
	$(MAKE) clean -C examples
	$(MAKE) clean -C test
	$(MAKE) clean -C tutorials

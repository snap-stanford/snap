#
#	Makefile to generate User and Developer Reference manuals
#
#	Prerequisites:
#	- install the following programs on your computer
#		doxygen, http://www.stack.nl/~dimitri/doxygen/index.html
#		GraphViz, http://www.graphviz.org
#	

DATE := $(shell date +%Y%m%d)
TIME := $(shell date +%Y%m%d-%H%M%S)
STIME := $(shell date +"%Y-%m-%d %H:%M:%S")
SLVER := $(VERSION)

DOXYGEN = doxygen
ifeq ($(shell uname), Darwin)
	# OS X values
	DOXYGEN = /Applications/Doxygen.app/Contents/Resources/doxygen
endif

dummy:

all: dev user

dev: doxycopy
	rm -rf snapdoc-dev log-doxygen-dev.txt
	sed "s/00000000/$(STIME)/" Doxyfile-dev | sed "s/11111111/$(SLVER)/" > Doxyfile-dev-tmp
	$(DOXYGEN) Doxyfile-dev-tmp

user: doxycopy
	rm -rf snapdoc-user log-doxygen-user.txt
	sed "s/00000000/$(STIME)/" Doxyfile-user | sed "s/11111111/$(SLVER)/" > Doxyfile-user-tmp
	$(DOXYGEN) Doxyfile-user-tmp

doxycopy:
	rm -rf snap
	mkdir snap
	mkdir snap/glib-core snap/snap-core snap/snap-adv
	python doxyblock.py ../glib-core snap/glib-core
	python doxyblock.py ../snap-core snap/snap-core
	python doxyblock.py ../snap-adv snap/snap-adv
	#python doxyblock.py ../examples snap/examples
	#python doxyblock.py ../projects snap/projects
	#python doxyblock.py ../utils snap/utils

install:
	#ssh rok@snap mv public_html/snapdoc-dev public_html/snapdoc-dev-$(TIME)
	scp -r snapdoc-dev rok@snap:public_html/snapdoc-dev-$(TIME)
	ssh rok@snap ln -fsn snapdoc-dev-$(TIME) public_html/snapdoc-dev
	#ssh rok@snap mv public_html/snapdoc-user public_html/snapdoc-user-$(TIME)
	scp -r snapdoc-user rok@snap:public_html/snapdoc-user-$(TIME)
	ssh rok@snap ln -fsn snapdoc-user-$(TIME) public_html/snapdoc-user

clean:
	rm -rf snap
	rm -rf snapdoc-dev* snapdoc-user*
	rm -rf Doxyfile-*-tmp
	rm -rf log-doxygen*.txt doxygen_*.tmp


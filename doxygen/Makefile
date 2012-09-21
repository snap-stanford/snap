DATE := $(shell date +%Y%m%d)
TIME := $(shell date +%Y%m%d-%H%M%S)
STIME := $(shell date +"%Y-%m-%d %H:%M:%S")

dummy:

all: dev user

dev: doxycopy
	rm -rf snapdoc-dev log-doxygen-dev.txt
	sed "s/00000000/$(STIME)/" Doxyfile-dev > Doxyfile-dev-tmp
	doxygen Doxyfile-dev-tmp

user: doxycopy
	rm -rf snapdoc-user log-doxygen-user.txt
	sed "s/00000000/$(STIME)/" Doxyfile-user > Doxyfile-user-tmp
	doxygen Doxyfile-user-tmp

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


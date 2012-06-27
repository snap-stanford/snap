DATE := $(shell date +%Y%m%d)

dummy:

all: dev user

dev: doxycopy
	rm -rf snapdoc-dev log-doxygen-dev.txt
	sed "s/00000000/$(DATE)/" Doxyfile-dev > Doxyfile-dev-tmp
	doxygen Doxyfile-dev-tmp

user: doxycopy
	rm -rf snapdoc-user log-doxygen-user.txt
	sed "s/00000000/$(DATE)/" Doxyfile-user > Doxyfile-user-tmp
	doxygen Doxyfile-user-tmp

doxycopy:
	rm -rf snap
	mkdir snap
	mkdir snap/glib-core snap/snap-core \
		snap/examples snap/projects snap/utils
	python doxyblock.py ../glib-core snap/glib-core
	python doxyblock.py ../snap-core snap/snap-core
	python doxyblock.py ../examples snap/examples
	python doxyblock.py ../projects snap/projects
	python doxyblock.py ../utils snap/utils

clean:
	rm -rf snap
	rm -rf snapdoc-dev* snapdoc-user*
	rm -rf Doxyfile-*-tmp
	rm -rf log-doxygen*.txt doxygen_*.tmp


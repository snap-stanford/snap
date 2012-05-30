
dummy:

all: dev user

dev: doxycopy
	rm -rf html-dev log-doxygen-dev.txt
	doxygen Doxyfile-dev

user: doxycopy
	rm -rf html-user log-doxygen-user.txt
	doxygen Doxyfile-user

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
	rm -rf html-dev* html-user*
	rm -rf log-doxygen*.txt doxygen_*.tmp


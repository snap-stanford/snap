
all:

dev:
	rm -rf snap
	mkdir snap
	mkdir snap/glib-core snap/snap-core \
		snap/examples snap/projects snap/utils
	python doxyblock.py ../glib-core snap/glib-core
	python doxyblock.py ../snap-core snap/snap-core
	python doxyblock.py ../examples snap/examples
	python doxyblock.py ../projects snap/projects
	python doxyblock.py ../utils snap/utils
	rm -rf html log-doxygen.txt
	doxygen Doxyfile-dev

clean:
	rm -rf html snap log-doxygen.txt doxygen_*.tmp
	
	


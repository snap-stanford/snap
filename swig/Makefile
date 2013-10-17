#
# Makefile for SWIG processing of SNAP Python
#	Use this Makefile to compile SNAP Python from scratch
# 

# set the path to your SNAP directory here
GITDIR = ..
SNAPDIR = $(GITDIR)/$(SNAP)
GLIBDIR = $(GITDIR)/$(GLIB)

# include compilation parameters
include $(GITDIR)/Makefile.config

UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
  # Linux flags
  CXXFLAGS += -fPIC -shared -D__STDC_LIMIT_MACROS
  LDFLAGS += -lrt
else ifeq ($(UNAME), Darwin)
  # OS X flags
  LDFLAGS += -lpython -dynamiclib
else ifeq ($(shell uname -o), Cygwin)
  # Cygwin flags
  CXXFLAGS += -shared -D__STDC_LIMIT_MACROS
  LIBS += -lpython2.6
endif

all: snap.py _snap.so

snap_wrap.cxx: snap.i snap_types.i tvec.i pneanet.i pungraph.i pngraph.i pgraph.i \
	snapswig.h snap_types.h printgraph.h goodgraph.cpp
	swig -python -c++ -w302,312,317,325,362,383,384,389,401,503,508,509 -O -D_CMPWARN -I$(SNAPDIR) -I$(GLIBDIR) snap.i

snap_wrap.o: snap_wrap.cxx
	g++ $(CXXFLAGS) -c snap_wrap.cxx -I$(SNAPDIR) -I$(GLIBDIR)  -I/usr/include/python2.6 -I/usr/include/python2.7

Snap.o: 
	$(CC) $(CXXFLAGS) -c $(SNAPDIR)/Snap.cpp -I$(SNAPDIR) -I$(GLIBDIR)

_snap.so: snap_wrap.o Snap.o
	g++ $(LDFLAGS) $(CXXFLAGS) snap_wrap.o Snap.o $(LIBS) -o _snap.so

snap.py: snap_wrap.cxx

install: setup.py snap.py _snap.so
	sudo python setup.py install

dist: setup.py snap.py _snap.so
	python setup.py sdist

install-win: setup.py snap.py _snap.pyd
	/cygdrive/c/Python27/python.exe setup.py install

dist-win: setup.py snap.py _snap.pyd
	cp MANIFEST.win MANIFEST
	/cygdrive/c/Python27/python.exe setup.py sdist

clean:
	rm -f *.o *_wrap.cxx _*.so *.pyc snap.py _snap.*



AM_CPPFLAGS=
AM_LDFLAGS=

# public headers
# it will be moved to include/Makefile.am
include_HEADERS = \
    alg.h \
    anf.h \
    attr.h \
    bfsdfs.h \
    bignet.h \
    centr.h \
    cmty.h \
    cncom.h \
    conv.h \
    coreper.h \
    ff.h \
    flow.h \
    gbase.h \
    ggen.h \
    ghash.h \
    gio.h \
    graph.h \
    graphmp.h \
    gstat.h \
    gsvd.h \
    gviz.h \
    kcore.h \
    network.h \
    networkmp.h \
    numpy.h \
    randwalk.h \
    Snap.h \
    statplot.h \
    stdafx.h \
    subgraph.h \
    table.h \
    timenet.h \
    triad.h \
    util.h \
    $(NULL)

# libsnap:
lib_LTLIBRARIES=libsnap-core.la

OTHER_SOURCES= \
    alg.cpp \
    anf.cpp \
    attr.cpp \
    centr.cpp \
    cmty.cpp \
    cncom.cpp \
    coreper.cpp \
    ff.cpp \
    flow.cpp \
    gbase.cpp \
    ggen.cpp \
    ghash.cpp \
    gio.cpp \
    graph.cpp \
    graphmp.cpp \
    gstat.cpp \
    gsvd.cpp \
    gviz.cpp \
    network.cpp \
    networkmp.cpp \
    numpy.cpp \
    statplot.cpp \
    subgraph.cpp \
    testSnap.cpp \
    timenet.cpp \
    table.cpp \
    triad.cpp \
    util.cpp \
    $(NULL)

libsnap_core_la_SOURCES= \
    Snap.cpp \
    $(NULL)

libsnap_core_la_CPPFLAGS=

AM_CPPFLAGS+= \
    $(NULL)

DEFS+= \
    -I$(top_srcdir)/glib-core \
    -I$(top_srcdir)/glib-adv \
    -I$(top_srcdir)/snap-core \
    -I$(top_srcdir)/snap-adv \
    -I$(top_srcdir)/snap-exp \
    -I$(top_builddir)/ \
    `getconf LFS_CFLAGS` \
    `getconf LFS64_CFLAGS` \
    -D_GNU_SOURCE \
    -DHAVE_MMAP64=1 \
    -I/usr/include/libiberty \
    $(NULL)

if DEBUG
# use "valgrind --tool=memcheck --leak-check=yes" to check memory leak, MemWatch will drag the program.
#DEFS+=-DMEMWATCH=1
DEFS+= -DDEBUG=1
DEFS+= -DNDEBUG=1

AM_CPPFLAGS+=-g -Wall

else
AM_CPPFLAGS+=-O3
endif

bin_PROGRAMS=testsnap

testsnap_SOURCES= \
    testSnap.cpp \
    #dummy.cpp \
    $(NULL)

# force the autoconf use g++ to link the c program with c++ lib
dummy.cpp:
	touch $@

#testsnap_LDADD = -lsnap-exp -lsnap-adv -lsnap-core -lglib-adv -lglib-core
testsnap_LDADD = $(AM_LDFLAGS)

testsnap_CPPFLAGS = $(AM_CPPFLAGS)
testsnap_LDFLAGS = $(AM_LDFLAGS)
#testsnap_LDFLAGS = -lsnap-exp -lsnap-adv -lsnap-core -lglib-adv -lglib-core

testsnap_LDFLAGS += \
    -L$(top_builddir)/glib-core \
    -L$(top_builddir)/glib-adv \
    -L$(top_builddir)/snap-core \
    -L$(top_builddir)/snap-adv \
    -L$(top_builddir)/snap-exp \
    $(top_builddir)/glib-core/libglib-core.la \
    $(top_builddir)/snap-core/libsnap-core.la \
    #$(top_builddir)/snap-adv/libsnap-adv.la \
    #$(top_builddir)/snap-exp/libsnap-exp.la \
    #$(top_builddir)/glib-adv/libglib-adv.la \
    #-lsnap-exp -lsnap-adv -lsnap-core -lglib-adv -lglib-core


EXT_FLAGS=
@MK@GITNUMTMP=$(shell cd "$(top_srcdir)"; A=$$(git show | head -n 1 | awk '{print $$2}'); echo $${A:0:7}; cd - > /dev/null )
#@MK@SVNNUMTMP=$(shell cd "$(top_srcdir)"; LC_ALL=C svn info | grep -i Revision | awk '{print $$2}'; cd - > /dev/null )
#@MK@ifeq ($(SVNNUMTMP),)
#EXT_FLAGS+= -DSVN_VERSION='"${GITNUMTMP}"'
#@MK@else
#EXT_FLAGS+= -DSVN_VERSION='"${SVNNUMTMP}"'
#@MK@endif
@MK@ifeq ($(GITNUMTMP),)
@MK@else
EXT_FLAGS+= -DSVN_VERSION='"${GITNUMTMP}"'
@MK@endif
DEFS+=$(EXT_FLAGS)



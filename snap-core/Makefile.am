
AM_CPPFLAGS=
AM_LDFLAGS=

# public headers
# it will be moved to include/Makefile.am
nobase_include_HEADERS = \
    $(top_srcdir)/snap-core/alg.h \
    $(top_srcdir)/snap-core/anf.h \
    $(top_srcdir)/snap-core/attr.h \
    $(top_srcdir)/snap-core/bfsdfs.h \
    $(top_srcdir)/snap-core/bignet.h \
    $(top_srcdir)/snap-core/centr.h \
    $(top_srcdir)/snap-core/cmty.h \
    $(top_srcdir)/snap-core/cncom.h \
    $(top_srcdir)/snap-core/conv.h \
    $(top_srcdir)/snap-core/coreper.h \
    $(top_srcdir)/snap-core/ff.h \
    $(top_srcdir)/snap-core/flow.h \
    $(top_srcdir)/snap-core/gbase.h \
    $(top_srcdir)/snap-core/ggen.h \
    $(top_srcdir)/snap-core/ghash.h \
    $(top_srcdir)/snap-core/gio.h \
    $(top_srcdir)/snap-core/graph.h \
    $(top_srcdir)/snap-core/graphmp.h \
    $(top_srcdir)/snap-core/gstat.h \
    $(top_srcdir)/snap-core/gsvd.h \
    $(top_srcdir)/snap-core/gviz.h \
    $(top_srcdir)/snap-core/kcore.h \
    $(top_srcdir)/snap-core/network.h \
    $(top_srcdir)/snap-core/networkmp.h \
    $(top_srcdir)/snap-core/numpy.h \
    $(top_srcdir)/snap-core/randwalk.h \
    $(top_srcdir)/snap-core/Snap.h \
    $(top_srcdir)/snap-core/statplot.h \
    $(top_srcdir)/snap-core/stdafx.h \
    $(top_srcdir)/snap-core/subgraph.h \
    $(top_srcdir)/snap-core/table.h \
    $(top_srcdir)/snap-core/timenet.h \
    $(top_srcdir)/snap-core/triad.h \
    $(top_srcdir)/snap-core/util.h \
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

noinst_PROGRAMS=testsnap

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




AM_CPPFLAGS=
AM_LDFLAGS=

# public headers
# it will be moved to include/Makefile.am
include_HEADERS = \
    base.h \
    bd.h \
    bits.h \
    blobbs.h \
    ds.h \
    dt.h \
    env.h \
    fl.h \
    gnuplot.h \
    hash.h \
    hashmp.h \
    html.h \
    http.h \
    json.h \
    linalg.h \
    lx.h \
    md5.h \
    os.h \
    priorityqueue.h \
    shash.h \
    ss.h \
    ssmp.h \
    stdafx.h \
    tm.cpp \
    tm.h \
    unicode.h \
    unicodestring.h \
    url.h \
    ut.h \
    wch.h \
    xfl.h \
    xmath.h \
    xml.h \
    xmlser.h \
    zipfl.h \
    $(NULL)

# libglib:
lib_LTLIBRARIES=libglib-core.la
#libglib_core_la_SOURCES= \
    base.cpp \
    bd.cpp \
    bits.cpp \
    blobbs.cpp \
    dt.cpp \
    env.cpp \
    fl.cpp \
    gnuplot.cpp \
    hash.cpp \
    html.cpp \
    http.cpp \
    json.cpp \
    linalg.cpp \
    lx.cpp \
    md5.cpp \
    os.cpp \
    ss.cpp \
    ssmp.cpp \
    unicode.cpp \
    unicodestring.cpp \
    url.cpp \
    ut.cpp \
    wch.cpp \
    xfl.cpp \
    xmath.cpp \
    xml.cpp \
    zipfl.cpp \
    $(NULL)
libglib_core_la_SOURCES= \
    base.cpp \
    $(NULL)

libglib_core_la_CPPFLAGS=

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



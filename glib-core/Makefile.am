
AM_CPPFLAGS=
AM_LDFLAGS=

# public headers
# it will be moved to include/Makefile.am
nobase_include_HEADERS = \
    $(top_srcdir)/glib-core/base.h \
    $(top_srcdir)/glib-core/bd.h \
    $(top_srcdir)/glib-core/bits.h \
    $(top_srcdir)/glib-core/blobbs.h \
    $(top_srcdir)/glib-core/ds.h \
    $(top_srcdir)/glib-core/dt.h \
    $(top_srcdir)/glib-core/env.h \
    $(top_srcdir)/glib-core/fl.h \
    $(top_srcdir)/glib-core/gnuplot.h \
    $(top_srcdir)/glib-core/hash.h \
    $(top_srcdir)/glib-core/hashmp.h \
    $(top_srcdir)/glib-core/html.h \
    $(top_srcdir)/glib-core/http.h \
    $(top_srcdir)/glib-core/json.h \
    $(top_srcdir)/glib-core/linalg.h \
    $(top_srcdir)/glib-core/lx.h \
    $(top_srcdir)/glib-core/md5.h \
    $(top_srcdir)/glib-core/os.h \
    $(top_srcdir)/glib-core/priorityqueue.h \
    $(top_srcdir)/glib-core/shash.h \
    $(top_srcdir)/glib-core/ss.h \
    $(top_srcdir)/glib-core/ssmp.h \
    $(top_srcdir)/glib-core/stdafx.h \
    $(top_srcdir)/glib-core/tm.cpp \
    $(top_srcdir)/glib-core/tm.h \
    $(top_srcdir)/glib-core/unicode.h \
    $(top_srcdir)/glib-core/unicodestring.h \
    $(top_srcdir)/glib-core/url.h \
    $(top_srcdir)/glib-core/ut.h \
    $(top_srcdir)/glib-core/wch.h \
    $(top_srcdir)/glib-core/xfl.h \
    $(top_srcdir)/glib-core/xmath.h \
    $(top_srcdir)/glib-core/xml.h \
    $(top_srcdir)/glib-core/xmlser.h \
    $(top_srcdir)/glib-core/zipfl.h \
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




AM_CPPFLAGS=
AM_LDFLAGS=

# public headers
# it will be moved to include/Makefile.am
nobase_include_HEADERS = \
    $(top_srcdir)/snap-adv/agmattr.h \
    $(top_srcdir)/snap-adv/agmdirected.h \
    $(top_srcdir)/snap-adv/agmfast.h \
    $(top_srcdir)/snap-adv/agmfit.h \
    $(top_srcdir)/snap-adv/agm.h \
    $(top_srcdir)/snap-adv/cascdynetinf.h \
    $(top_srcdir)/snap-adv/cascnetinf.h \
    $(top_srcdir)/snap-adv/circles.h \
    $(top_srcdir)/snap-adv/cliques.h \
    $(top_srcdir)/snap-adv/graphcounter.h \
    $(top_srcdir)/snap-adv/kronecker.h \
    $(top_srcdir)/snap-adv/mag.h \
    $(top_srcdir)/snap-adv/ncp.h \
    $(top_srcdir)/snap-adv/rolx.h \
    $(top_srcdir)/snap-adv/subgraphenum.h \
    $(NULL)

# libsnap:
lib_LTLIBRARIES=libsnap-adv.la
libsnap_adv_la_SOURCES= \
    agmattr.cpp \
    agm.cpp \
    agmdirected.cpp \
    agmfast.cpp \
    agmfit.cpp \
    cascdynetinf.cpp \
    cascnetinf.cpp \
    cliques.cpp \
    graphcounter.cpp \
    kronecker.cpp \
    mag.cpp \
    ncp.cpp \
    rolx.cpp \
    subgraphenum.cpp \
    $(NULL)

libsnap_adv_la_CPPFLAGS=

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



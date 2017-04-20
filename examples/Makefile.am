
AM_LDFLAGS=
AM_CPPFLAGS=

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
    -D_FILE_OFFSET_BITS=64 \
    -DHAVE_MMAP64=1 \
    -DUSE_FINDHTML=1 \
    $(NULL)

AM_CPPFLAGS+= \
    $(NULL)

AM_LDFLAGS += \
    -L$(top_builddir)/glib-core \
    -L$(top_builddir)/glib-adv \
    -L$(top_builddir)/snap-core \
    -L$(top_builddir)/snap-adv \
    -L$(top_builddir)/snap-exp \
    -lsnap-adv -lsnap-core -lglib-core
    #-lsnap-exp -lsnap-adv -lsnap-core -lglib-adv -lglib-core

#AM_CPPFLAGS += `pkg-config --cflags libxml-2.0`
#AM_LDFLAGS += `pkg-config --libs libxml-2.0`
AM_CPPFLAGS += $(LIBXML2_CFLAGS)
AM_LDFLAGS += $(LIBXML2_LDFLAGS) $(LIBXML2_LIBS)

#AM_CPPFLAGS += `pkg-config --cflags libxslt`
#AM_LDFLAGS += `pkg-config --libs libxslt`
AM_CPPFLAGS += $(LIBXSLT_CFLAGS)
AM_LDFLAGS += $(LIBXSLT_LDFLAGS) $(LIBXSLT_LIBS)

#AM_LDFLAGS += -pthread

if DEBUG
# use "valgrind --tool=memcheck --leak-check=yes" to check memory leak, MemWatch will drag the program.
#DEFS+=-DMEMWATCH
DEFS+= -DDEBUG=1
DEFS+= -DNDEBUG=1

AM_LDFLAGS += -lbfd
AM_CPPFLAGS+=-g -Wall

else
AM_CPPFLAGS+=-O3 -Wall
endif

#AM_CPPFLAGS+=-fpermissive


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

#AM_CPPFLAGS+=$(EXT_FLAGS)



bin_PROGRAMS=snapagmgen snapgraphgen snapkrongen

OTHER_BIN=snaplshtest snapmultimodal snaptestconv snapzygote

noinst_PROGRAMS=snapagmfit snapbigclam snapcascades snapcentrality snapcesna snapcircles snapcliques snapcoda snapcommunity snapconcomp snapflows snapforestfire snapgraphhash snapinfopath snapkcores snapkronem snapkronfit snapmagfit snapmaggen snapmkdatasets snapmotifs snapncpplot snapnetevol snapnetinf snapnetstat snaprandwalk snaprolx snaptestgraph

snapagmfit_SOURCES= \
    agmfit/agmfitmain.cpp \
    $(NULL)

snapagmgen_SOURCES= \
    agmgen/agmgen.cpp \
    $(NULL)

snapbigclam_SOURCES= \
    bigclam/bigclam.cpp \
    $(NULL)

snapcascades_SOURCES= \
    cascades/cascades.cpp \
    $(NULL)

snapcentrality_SOURCES= \
    centrality/centrality.cpp \
    $(NULL)

snapcesna_SOURCES= \
    cesna/cesna.cpp \
    $(NULL)

snapcircles_SOURCES= \
    circles/circles.cpp \
    $(NULL)

snapcliques_SOURCES= \
    cliques/cliquesmain.cpp \
    $(NULL)

snapcoda_SOURCES= \
    coda/coda.cpp \
    $(NULL)

snapcommunity_SOURCES= \
    community/community.cpp \
    $(NULL)

snapconcomp_SOURCES= \
    concomp/concomp.cpp \
    $(NULL)


snapflows_SOURCES= \
    flows/flows.cpp \
    $(NULL)

#snapflows_CPPFLAGS =
snapflows_LDFLAGS = $(DEFS) -fopenmp
snapflows_LDADD = $(AM_LDFLAGS)



snapforestfire_SOURCES= \
    forestfire/forestfire.cpp \
    #dummy.cpp \
    $(NULL)

# force the autoconf use g++ to link the c program with c++ lib
dummy.cpp:
	touch $@

#snapforestfire_LDADD = $(top_builddir)/src/libsnap-core.la -lbfd -liberty -ldl
snapforestfire_LDADD = $(AM_LDFLAGS)

snapforestfire_CPPFLAGS = $(AM_CPPFLAGS)
snapforestfire_LDFLAGS = $(AM_LDFLAGS)
#snapforestfire_LDFLAGS = -lchsetdet -lcrc -lpf -lz
#$(LIBPF_LDFLAGS) $(LIBCHSETDET_LDFLAGS) $(HUNSPELL_LDFLAGS) $(LIBXML2_LDFLAGS) $(LIBXSLT_LDFLAGS) $(LIBCRC_LDFLAGS) $(ZLIB_LDFLAGS)


snapgraphgen_SOURCES= \
    graphgen/graphgen.cpp \
    $(NULL)

snapgraphhash_SOURCES= \
    graphhash/graphhash.cpp \
    $(NULL)

snapinfopath_SOURCES= \
    infopath/infopath.cpp \
    $(NULL)

snapkcores_SOURCES= \
    kcores/kcores.cpp \
    $(NULL)

snapkronem_SOURCES= \
    kronem/kronem.cpp \
    $(NULL)

snapkronfit_SOURCES= \
    kronfit/kronfit.cpp \
    $(NULL)

snapkrongen_SOURCES= \
    krongen/krongen.cpp \
    $(NULL)

#snaplshtest_SOURCES= \
    #lshtest/lshtest.cpp \
    #$(NULL)

snapmagfit_SOURCES= \
    magfit/magfit.cpp \
    $(NULL)

snapmaggen_SOURCES= \
    maggen/maggen.cpp \
    $(NULL)

snapmkdatasets_SOURCES= \
    mkdatasets/mkdatasets.cpp \
    $(NULL)

snapmotifs_SOURCES= \
    motifs/motifs.cpp \
    $(NULL)

#snapmultimodal_SOURCES= \
    #multimodal/subgraph-exp.cpp \
    #$(NULL)

snapncpplot_SOURCES= \
    ncpplot/ncpplot.cpp \
    $(NULL)

snapnetevol_SOURCES= \
    netevol/netevol.cpp \
    $(NULL)

snapnetinf_SOURCES= \
    netinf/netinf.cpp \
    $(NULL)

snapnetstat_SOURCES= \
    netstat/netstat.cpp \
    $(NULL)

snaprandwalk_SOURCES= \
    randwalk/randwalk.cpp \
    $(NULL)

snaprolx_SOURCES= \
    rolx/testrolx.cpp \
    $(NULL)

#snaptestconv_SOURCES= \
    #testconv/convtest.cpp \
    #$(NULL)

snaptestgraph_SOURCES= \
    testgraph/testgraph.cpp \
    $(NULL)

#snapzygote_SOURCES= \
    #zygote/zydemo.cpp \
    #$(NULL)

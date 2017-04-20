SUBDIRS= glib-core snap-core snap-adv examples # snap-exp glib-adv

#DOC_FILES= \
#    $(top_srcdir)/doc/attrtype.htm \
#    $(NULL)

noinst_HEADERS= \
    autoclean.sh \
    autogen.sh \
    $(DOC_FILES) \
    $(NULL)

EXTRA_DIST = libsnap.pc.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libsnap.pc

EXT_FLAGS=
@MK@GITNUMTMP=$(shell cd "$(top_srcdir)"; A=$$(git show | head -n 1 | awk '{print $$2}'); echo $${A:0:7}; cd - > /dev/null )
#@MK@SVNNUMTMP=$(shell cd "$(top_srcdir)"; LC_ALL=C svn info | grep -i Revision | awk '{print $$2}'; cd - > /dev/null )
#@MK@ifeq ($(SVNNUMTMP),)
#EXT_FLAGS= -DSVN_VERSION="${GITNUMTMP}"
#@MK@else
#EXT_FLAGS= -DSVN_VERSION="${SVNNUMTMP}"
#@MK@endif
@MK@ifeq ($(GITNUMTMP),)
@MK@else
EXT_FLAGS+= -DSVN_VERSION="${GITNUMTMP}"
@MK@endif
CFLAGS+=$(EXT_FLAGS)
CXXFLAGS+=$(EXT_FLAGS)


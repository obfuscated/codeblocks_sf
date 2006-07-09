INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libdragscroll.la

libdragscroll_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libdragscroll_la_LIBADD = ../../../sdk/libcodeblocks.la

libdragscroll_la_SOURCES = dragscroll.cpp \
			dragscrollcfg.cpp

noinst_HEADERS = dragscroll.h \
			dragscrollcfg.h

EXTRA_DIST = DragScroll.cbp \
		DragScroll-unix.cbp


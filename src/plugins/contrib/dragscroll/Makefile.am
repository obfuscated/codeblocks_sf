INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libdragscroll.la

libdragscroll_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libdragscroll_la_LIBADD = ../../../sdk/libcodeblocks.la \
				../../../sdk/wxscintilla/libwxscintilla.la

libdragscroll_la_SOURCES = cbdragscroll.cpp \
			cbdragscrollcfg.cpp

noinst_HEADERS = cbdragscroll.h \
			cbdragscrollcfg.h

EXTRA_DIST = dragscroll.cbp \
		dragscroll-unix.cbp


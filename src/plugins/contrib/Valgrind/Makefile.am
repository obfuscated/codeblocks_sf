SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libValgrind.la

libcodestat_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libcodestat_la_LIBADD = $(PLUGIN_WX_LIBS) ../../../sdk/libcodeblocks.la

libcodestat_la_SOURCES = Valgrind.cpp

noinst_HEADERS = Valgrind.h

EXTRA_DIST = 
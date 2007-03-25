SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libclasswizard.la

libclasswizard_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libclasswizard_la_LIBADD = $(PLUGIN_WX_LIBS) ../../sdk/libcodeblocks.la

libclasswizard_la_SOURCES = classwizard.cpp classwizarddlg.cpp
noinst_HEADERS = classwizard.h classwizarddlg.h

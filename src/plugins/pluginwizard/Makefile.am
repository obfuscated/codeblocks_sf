SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libpluginwizard.la

libpluginwizard_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libpluginwizard_la_LIBADD = ../../sdk/libcodeblocks.la

libpluginwizard_la_SOURCES = enterinfodlg.cpp \
			pluginwizard.cpp \
			pluginwizarddlg.cpp

noinst_HEADERS = enterinfodlg.h \
			pluginwizard.h \
			pluginwizarddlg.h

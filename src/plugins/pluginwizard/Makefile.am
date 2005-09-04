SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libpluginwizard.la

libpluginwizard_la_LDFLAGS = -module

libpluginwizard_la_LIBADD = ../../sdk/libcodeblocks.la \
				../../sdk/wxscintilla/libwxscintilla.la

libpluginwizard_la_SOURCES = enterinfodlg.cpp \
			pluginwizard.cpp \
			pluginwizarddlg.cpp

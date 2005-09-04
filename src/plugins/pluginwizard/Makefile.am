SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

AM_CXXFLAGS = -Wall -g

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libpluginwizard.la

libpluginwizard_la_LDFLAGS = -module

libpluginwizard_la_LIBADD = $(top_srcdir)/src/sdk/libcodeblocks.la \
				$(top_srcdir)/src/sdk/wxscintilla/libwxscintilla.la

libpluginwizard_la_SOURCES = enterinfodlg.cpp \
			pluginwizard.cpp \
			pluginwizarddlg.cpp

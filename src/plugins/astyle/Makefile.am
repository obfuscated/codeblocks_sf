SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-Iastyle \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

AM_CXXFLAGS = -Wall -g

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libastyle.la

libastyle_la_LDFLAGS = -module

libastyle_la_LIBADD = $(top_srcdir)/src/sdk/libcodeblocks.la \
				$(top_srcdir)/src/sdk/wxscintilla/libwxscintilla.la

libastyle_la_SOURCES = astyleconfigdlg.cpp \
			astyleplugin.cpp \
			formattersettings.cpp \
			astyle/ASBeautifier.cpp \
			astyle/ASFormatter.cpp

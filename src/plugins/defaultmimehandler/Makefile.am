SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

AM_CXXFLAGS = -Wall -g

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libdefaultmimehandler.la

libdefaultmimehandler_la_LDFLAGS = -module

libdefaultmimehandler_la_LIBADD = $(top_srcdir)/src/sdk/libcodeblocks.la \
				$(top_srcdir)/src/sdk/wxscintilla/libwxscintilla.la

libdefaultmimehandler_la_SOURCES = defaultmimehandler.cpp \
			editmimetypesdlg.cpp

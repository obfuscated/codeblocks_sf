SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libdefaultmimehandler.la

libdefaultmimehandler_la_LDFLAGS = -module

libdefaultmimehandler_la_LIBADD = ../../sdk/libcodeblocks.la \
				../../sdk/wxscintilla/libwxscintilla.la

libdefaultmimehandler_la_SOURCES = defaultmimehandler.cpp \
			editmimetypesdlg.cpp

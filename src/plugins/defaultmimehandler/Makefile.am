SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libdefaultmimehandler.la

libdefaultmimehandler_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libdefaultmimehandler_la_LIBADD = ../../sdk/libcodeblocks.la

libdefaultmimehandler_la_SOURCES = defaultmimehandler.cpp \
			editmimetypesdlg.cpp

noinst_HEADERS = defaultmimehandler.h \
			editmimetypesdlg.h \
			mimetypesarray.h

SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libCccc.la

libCccc_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libCccc_la_LIBADD = $(WX_LIBS) ../../../sdk/libcodeblocks.la

libCccc_la_SOURCES = Cccc.cpp

noinst_HEADERS = Cccc.h

EXTRA_DIST = $(srcdir)/*.cbp

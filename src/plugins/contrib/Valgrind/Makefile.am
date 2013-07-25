SUBDIRS = resources

AM_CPPFLAGS = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libValgrind.la

libValgrind_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libValgrind_la_LIBADD = ../../../sdk/libcodeblocks.la $(WX_LIBS)

libValgrind_la_SOURCES = Valgrind.cpp \
			ValgrindListLog.cpp \
			valgrind_config.cpp

noinst_HEADERS = Valgrind.h \
			ValgrindListLog.h \
			valgrind_config.h

EXTRA_DIST = $(srcdir)/*.cbp \
		wxsmith/valgrind_config_panel.wxs

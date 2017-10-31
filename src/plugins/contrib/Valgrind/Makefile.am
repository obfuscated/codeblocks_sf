SUBDIRS = resources

AM_CPPFLAGS = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libValgrind.la

libValgrind_la_LDFLAGS = @MODULE_SHARED_LDFLAGS@ -version-info 0:1:0 -no-undefined -avoid-version

libValgrind_la_LIBADD = ../../../sdk/libcodeblocks.la \
			$(WX_LIBS) \
			$(CB_TINYXML_LIBS)

libValgrind_la_SOURCES = Valgrind.cpp \
			ValgrindListLog.cpp \
			valgrind_config.cpp

noinst_HEADERS = Valgrind.h \
			ValgrindListLog.h \
			valgrind_config.h

EXTRA_DIST = $(srcdir)/*.cbp \
		$(srcdir)/update* \
		wxsmith/valgrind_config_panel.wxs

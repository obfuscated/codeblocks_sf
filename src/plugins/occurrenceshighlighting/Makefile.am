SUBDIRS = resources

AM_CPPFLAGS = $(WX_CXXFLAGS) \
		$(CB_SQUIRREL_CFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include \
		-I$(top_srcdir)/src/include/scripting/sqplus \
		-I$(top_srcdir)/src/include/scripting/bindings

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = liboccurrenceshighlighting.la

liboccurrenceshighlighting_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

liboccurrenceshighlighting_la_LIBADD = ../../sdk/libcodeblocks.la $(WX_LIBS)

liboccurrenceshighlighting_la_SOURCES = occurrenceshighlighting.cpp \
			occurrencespanel.cpp \
			occurrenceshighlightingconfigurationpanel.cpp \
			highlighter.cpp

noinst_HEADERS = occurrenceshighlighting.h \
			occurrencespanel.h \
			occurrenceshighlightingconfigurationpanel.h \
			highlighter.h

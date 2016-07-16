SUBDIRS = resources

AM_CPPFLAGS = $(WX_CXXFLAGS) \
		$(CB_TINYXML_CFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libCppCheck.la

libCppCheck_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libCppCheck_la_LIBADD = ../../../sdk/libcodeblocks.la $(WX_LIBS)

libCppCheck_la_SOURCES = CppCheck.cpp \
            CppCheckListLog.cpp \
            ConfigPanel.cpp

noinst_HEADERS = CppCheck.h \
            CppCheckListLog.h \
            ConfigPanel.h

EXTRA_DIST = $(srcdir)/*.cbp \
		wxsmith

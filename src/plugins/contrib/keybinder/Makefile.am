INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libkeybinder.la

libkeybinder_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libkeybinder_la_LIBADD = ../../../sdk/libcodeblocks.la

libkeybinder_la_SOURCES = cbkeybinder.cpp \
			keybinder.cpp \
			menuutils.cpp

noinst_HEADERS = cbkeybinder.h \
			debugging.h \
			keybinder.h \
			keybinderdef.h \
			menuutils.h

EXTRA_DIST = keybinder.cbp

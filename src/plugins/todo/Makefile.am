SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

AM_CXXFLAGS = -Wall -g

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libtodo.la

libtodo_la_LDFLAGS = -module

libtodo_la_LIBADD = $(top_srcdir)/src/sdk/libcodeblocks.la \
				$(top_srcdir)/src/sdk/wxscintilla/libwxscintilla.la

libtodo_la_SOURCES = addtododlg.cpp \
			todolist.cpp \
			todolistview.cpp \
			todosettingsdlg.cpp

SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libtodo.la

libtodo_la_LDFLAGS = -module

libtodo_la_LIBADD = ../../sdk/libcodeblocks.la \
				../../sdk/wxscintilla/libwxscintilla.la

libtodo_la_SOURCES = addtododlg.cpp \
			todolist.cpp \
			todolistview.cpp \
			todosettingsdlg.cpp

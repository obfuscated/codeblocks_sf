SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libtodo.la

libtodo_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libtodo_la_LIBADD = ../../sdk/libcodeblocks.la

libtodo_la_SOURCES = addtododlg.cpp \
			asktypedlg.cpp \
			todolist.cpp \
			todolistview.cpp \
			todosettingsdlg.cpp

noinst_HEADERS = addtododlg.h \
			asktypedlg.h \
			todolist.h \
			todolistview.h \
			todosettingsdlg.h

SUBDIRS = resources

AM_CPPFLAGS = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libtodo.la

libtodo_la_LDFLAGS = @MODULE_SHARED_LDFLAGS@ -version-info 0:1:0 -no-undefined -avoid-version

libtodo_la_LIBADD = ../../sdk/libcodeblocks.la $(WX_LIBS)

libtodo_la_SOURCES = addtododlg.cpp \
			asktypedlg.cpp \
			todo_parser.cpp \
			todolist.cpp \
			todolistview.cpp \
			todosettingsdlg.cpp

noinst_HEADERS = addtododlg.h \
			asktypedlg.h \
			todo_parser.h \
			todolist.h \
			todolistview.h \
			todosettingsdlg.h

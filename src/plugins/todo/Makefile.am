SUBDIRS = resources

AM_CPPFLAGS = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include \
		-I$(top_srcdir)/src/include/mozilla_chardet \
		-I$(top_srcdir)/src/include/mozilla_chardet/mfbt \
		-I$(top_srcdir)/src/include/mozilla_chardet/nsprpub/pr/include \
		-I$(top_srcdir)/src/include/mozilla_chardet/xpcom \
		-I$(top_srcdir)/src/include/mozilla_chardet/xpcom/base \
		-I$(top_srcdir)/src/include/mozilla_chardet/xpcom/glue


pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libtodo.la

libtodo_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libtodo_la_LIBADD = ../../sdk/libcodeblocks.la $(WX_LIBS)

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

SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libcodesnippets.la

libcodesnippets_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libcodesnippets_la_LIBADD = $(PLUGIN_WX_LIBS) ../../../sdk/libcodeblocks.la

libcodesnippets_la_SOURCES = 	codesnippets.cpp \
				codesnippetstreectrl.cpp \
				codesnippetswindow.cpp \
				snippetitemdata.cpp \
				editsnippetdlg.cpp

noinst_HEADERS =  	codesnippets.h \
			codesnippetstreectrl.h \
			codesnippetswindow.h \
			snippetitemdata.h \
			editsnippetdlg.h

EXTRA_DIST = 	codesnippets.cbp \
		codesnippets-unix.cbp \
		update.bat \
		update \
		readme.html \
		license.txt

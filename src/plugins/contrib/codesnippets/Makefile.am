SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libcodesnippets.la

libcodesnippets_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libcodesnippets_la_LIBADD = ../../../sdk/libcodeblocks.la

libcodesnippets_la_SOURCES = 	codesnippets.cpp \
				codesnippetstreectrl.cpp \
				codesnippetswindow.cpp \
				snippetitemdata.cpp

noinst_HEADERS =  	codesnippets.h \
			codesnippetstreectrl.h \
			codesnippetswindow.h \
			snippetitemdata.h

EXTRA_DIST = 	codesnippets.cbp \
		codesnippets-unix.cbp \
		update.bat \
		update \
		readme.html \
		license.txt



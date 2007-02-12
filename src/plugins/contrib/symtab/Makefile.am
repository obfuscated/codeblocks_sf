SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libSymTab.la

libSymTab_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libSymTab_la_LIBADD = ../../../sdk/libcodeblocks.la

libSymTab_la_SOURCES = symtabconfig.cpp \
			symtab.cpp \
			symtabexec.cpp

noinst_HEADERS = symtabconfig.h \
			symtabexec.h \
			symtab.h

EXTRA_DIST = symtab.cbp \
			symtab-unix.cbp

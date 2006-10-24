SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libSymTab.la

libsymtab_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libsymtab_la_LIBADD = ../../../sdk/libcodeblocks.la

libsymtab_la_SOURCES = symtabconfig.cpp \
			symtab.cpp \
			symtabexec.cpp

noinst_HEADERS = symtabconfig.h \
			symtabexec.h \
			symtab.h

EXTRA_DIST = symtab.cbp \
			symtab.unix.cbp

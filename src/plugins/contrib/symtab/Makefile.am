SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libsymtab.la

libsymtab_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libsymtab_la_LIBADD = ../../../sdk/libcodeblocks.la

libsymtab_la_SOURCES = cbsymtabconfig.cpp \
			cbsymtab.cpp \
			cbsymtabexec.cpp

noinst_HEADERS = cbsymtabconfig.h \
			cbsymtabexec.h \
			cbsymtab.h

EXTRA_DIST = cbsymtab.cbp \
			cbsymtab.unix.cbp

SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libcodestat.la

libcodestat_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libcodestat_la_LIBADD = ../../../sdk/libcodeblocks.la \
				../../../sdk/wxscintilla/libwxscintilla.la

libcodestat_la_SOURCES = codestatconfig.cpp \
			codestat.cpp \
			codestatexec.cpp

noinst_HEADERS = codestatconfig.h \
			codestatexec.h \
			codestat.h \
			language_def.h

EXTRA_DIST = Installation\ Instructions.txt \
			Build\ Instructions.txt \
			codestat.cbp

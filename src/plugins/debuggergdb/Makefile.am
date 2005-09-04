SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

AM_CXXFLAGS = -Wall -g

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libdebuggergdb.la

libdebuggergdb_la_LDFLAGS = -module

libdebuggergdb_la_LIBADD = $(top_srcdir)/src/sdk/libcodeblocks.la \
				$(top_srcdir)/src/sdk/wxscintilla/libwxscintilla.la

libdebuggergdb_la_SOURCES = backtracedlg.cpp \
			debuggergdb.cpp \
			debuggeroptionsdlg.cpp \
			debuggertree.cpp \
			disassemblydlg.cpp

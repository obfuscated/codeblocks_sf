SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libdebuggergdb.la

libdebuggergdb_la_LDFLAGS = -module

libdebuggergdb_la_LIBADD = ../../sdk/libcodeblocks.la \
				../../sdk/wxscintilla/libwxscintilla.la

libdebuggergdb_la_SOURCES = backtracedlg.cpp \
			debuggergdb.cpp \
			debuggeroptionsdlg.cpp \
			debuggertree.cpp \
			disassemblydlg.cpp

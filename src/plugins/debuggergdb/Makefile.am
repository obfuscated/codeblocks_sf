SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libdebuggergdb.la

libdebuggergdb_la_LDFLAGS = -module -version-info 0:1:0

libdebuggergdb_la_LIBADD = ../../sdk/libcodeblocks.la \
				../../sdk/wxscintilla/libwxscintilla.la

libdebuggergdb_la_SOURCES = backtracedlg.cpp \
			debuggergdb.cpp \
			debuggeroptionsdlg.cpp \
			debuggertree.cpp \
			disassemblydlg.cpp

noinst_HEADERS = backtracedlg.h \
			debuggergdb.h \
			debuggeroptionsdlg.h \
			debuggertree.h \
			disassemblydlg.h

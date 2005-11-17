SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libdebuggergdb.la

libdebuggergdb_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libdebuggergdb_la_LIBADD = ../../sdk/libcodeblocks.la \
				../../sdk/wxscintilla/libwxscintilla.la

libdebuggergdb_la_SOURCES = backtracedlg.cpp \
			breakpointsdlg.cpp \
			cdb_driver.cpp \
			debugger_defs.cpp \
			debuggerdriver.cpp \
			debuggergdb.cpp \
			debuggeroptionsdlg.cpp \
			debuggertree.cpp \
			disassemblydlg.cpp \
			editbreakpointdlg.cpp \
			editwatchdlg.cpp \
			editwatchesdlg.cpp \
			gdb_driver.cpp

noinst_HEADERS = backtracedlg.h \
			breakpointsdlg.h \
			cdb_commands.h \
			cdb_driver.h \
			debugger_defs.h \
			debuggerdriver.h \
			debuggergdb.h \
			debuggeroptionsdlg.h \
			debuggertree.h \
			disassemblydlg.h \
			editbreakpointdlg.h \
			editwatchdlg.h \
			editwatchesdlg.h \
			gdb_commands.h \
			gdb_driver.h

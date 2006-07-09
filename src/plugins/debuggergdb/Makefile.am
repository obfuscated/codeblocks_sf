SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include \
		-I$(top_srcdir)/src/sdk/scripting/include \
    	-I$(top_srcdir)/src/sdk/scripting/sqplus

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libdebuggergdb.la

libdebuggergdb_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libdebuggergdb_la_LIBADD = ../../sdk/libcodeblocks.la

libdebuggergdb_la_SOURCES = backtracedlg.cpp \
			breakpointsdlg.cpp \
			cdb_driver.cpp \
			cpuregistersdlg.cpp \
			debugger_defs.cpp \
			debuggerdriver.cpp \
			debuggergdb.cpp \
			debuggeroptionsdlg.cpp \
			debuggerstate.cpp \
			debuggertree.cpp \
			disassemblydlg.cpp \
			editbreakpointdlg.cpp \
			editwatchdlg.cpp \
			editwatchesdlg.cpp \
			examinememorydlg.cpp \
			gdb_driver.cpp \
			threadsdlg.cpp

noinst_HEADERS = backtracedlg.h \
			breakpointsdlg.h \
			cdb_commands.h \
			cdb_driver.h \
			cpuregistersdlg.h \
			debugger_defs.h \
			debuggerdriver.h \
			debuggergdb.h \
			debuggeroptionsdlg.h \
			debuggerstate.h \
			debuggertree.h \
			disassemblydlg.h \
			editbreakpointdlg.h \
			editwatchdlg.h \
			editwatchesdlg.h \
			examinememorydlg.h \
			gdb_commands.h \
			gdb_driver.h \
			threadsdlg.h

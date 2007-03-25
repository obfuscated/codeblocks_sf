SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		$(WX_GTK2_CFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxscintilla/include \
		-I$(top_srcdir)/src/include/scripting/include \
    	-I$(top_srcdir)/src/include/scripting/sqplus

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libdebugger.la

libdebugger_la_LDFLAGS = $(WX_GTK2_LIBS) -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libdebugger_la_LIBADD = $(PLUGIN_WX_LIBS) ../../sdk/libcodeblocks.la

libdebugger_la_SOURCES = backtracedlg.cpp \
			breakpointsdlg.cpp \
			cdb_driver.cpp \
			cpuregistersdlg.cpp \
			databreakpointdlg.cpp \
			debugger_defs.cpp \
			debuggerdriver.cpp \
			debuggergdb.cpp \
			debuggeroptionsdlg.cpp \
			debuggeroptionsprjdlg.cpp \
			debuggerstate.cpp \
			debuggertree.cpp \
			disassemblydlg.cpp \
			editbreakpointdlg.cpp \
			editwatchdlg.cpp \
			editwatchesdlg.cpp \
			examinememorydlg.cpp \
			gdb_driver.cpp \
			gdb_tipwindow.cpp \
			threadsdlg.cpp

noinst_HEADERS = backtracedlg.h \
			breakpointsdlg.h \
			cdb_commands.h \
			cdb_driver.h \
			cpuregistersdlg.h \
			databreakpointdlg.h \
			debugger_defs.h \
			debuggerdriver.h \
			debuggergdb.h \
			debuggeroptionsdlg.h \
			debuggeroptionsprjdlg.h \
			debuggerstate.h \
			debuggertree.h \
			disassemblydlg.h \
			editbreakpointdlg.h \
			editwatchdlg.h \
			editwatchesdlg.h \
			examinememorydlg.h \
			gdb_commands.h \
			gdb_driver.h \
			gdb_tipwindow.h \
			threadsdlg.h

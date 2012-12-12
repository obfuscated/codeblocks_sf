SUBDIRS = depslib resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include \
		-I$(top_srcdir)/src/include/scripting/include \
		-I$(top_srcdir)/src/include/scripting/sqplus \
		-I$(top_srcdir)/src/plugins/compilergcc/depslib/src

AM_CXXFLAGS = -DDEPSLIB_UNIX

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libcompiler.la

libcompiler_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libcompiler_la_LIBADD = $(PLUGIN_WX_LIBS) ../../sdk/libcodeblocks.la \
				depslib/src/libdepslib.la

libcompiler_la_SOURCES = advancedcompileroptionsdlg.cpp \
			compiler_defs.cpp \
			compilerCYGWIN.cpp \
			compilererrors.cpp \
			compilerflagdlg.cpp \
			compilergcc.cpp \
			compilerGDC.cpp \
			compilerICC.cpp \
			compilerLCC.cpp \
			compilermessages.cpp \
			compilerMINGW.cpp \
			compilerMINGWgenerator.cpp \
			compilerMSVC.cpp \
			compilerMSVC8.cpp \
			compilerMSVC10.cpp \
			compileroptionsdlg.cpp \
			compilerOW.cpp \
			compilerOWgenerator.cpp \
			compilerG95.cpp \
			compilerGNUARM.cpp \
			compilerGNUFortran.cpp \
			compilerXML.cpp \
			directcommands.cpp

noinst_HEADERS = advancedcompileroptionsdlg.h \
			compiler_defs.h \
			compilerCYGWIN.h \
			compilererrors.h \
			compilerflagdlg.h \
			compilergcc.h \
			compilerG95.h \
			compilerGDC.h \
			compilerICC.h \
			compilerLCC.h \
			compilermessages.h \
			compilerMINGW.h \
			compilerMINGWgenerator.h \
			compilerMSVC.h \
			compilerMSVC8.h \
			compilerMSVC10.h \
			compileroptionsdlg.h \
			compilerOW.h \
			compilerOWgenerator.h \
			compilerGNUARM.h \
			compilerGNUFortran.h \
			compilerXML.h \
			directcommands.h



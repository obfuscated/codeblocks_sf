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
			compilerBCC.cpp \
			compiler_defs.cpp \
			compilerCYGWIN.cpp \
			compilerDMC.cpp \
			compilerDMD.cpp \
			compilererrors.cpp \
			compilergcc.cpp \
			compilerGDC.cpp \
			compilerLDC.cpp \
			compilerICC.cpp \
			compilerLCC.cpp \
			compilermessages.cpp \
			compilerMINGW.cpp \
			compilerMINGWgenerator.cpp \
			compilerMSVC.cpp \
			compileroptionsdlg.cpp \
			compilerOW.cpp \
			compilerOWgenerator.cpp \
			compilerSDCC.cpp \
			compilerGNUARM.cpp \
			compilerGNUAVR.cpp \
			compilerGNUMSP430.cpp \
			compilerGNUPOWERPC.cpp \
			compilerGNUTRICORE.cpp \
			compilerTcc.cpp \
			directcommands.cpp \
			makefilegenerator.cpp

noinst_HEADERS = advancedcompileroptionsdlg.h \
			compilerBCC.h \
			compiler_defs.h \
			compilerCYGWIN.h \
			compilerDMC.h \
			compilerDMD.h \
			compilererrors.h \
			compilergcc.h \
			compilerGDC.h \
			compilerLDC.h \
			compilerICC.h \
			compilerLCC.h \
			compilermessages.h \
			compilerMINGW.h \
			compilerMINGWgenerator.h \
			compilerMSVC.h \
			compileroptionsdlg.h \
			compilerOW.h \
			compilerOWgenerator.h \
			compilerSDCC.h \
			compilerGNUARM.h \
			compilerGNUAVR.h \
			compilerGNUMSP430.h \
			compilerGNUPOWERPC.h \
			compilerGNUTRICORE.h \
			compilerTcc.h \
			directcommands.h \
			makefilegenerator.h

EXTRA_DIST = compilerMSVC8.h \
			compilerMSVC8.cpp

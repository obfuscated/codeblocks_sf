SUBDIRS = depslib resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include \
		-I$(top_srcdir)/src/plugins/compilergcc/depslib/src

AM_CXXFLAGS = -DDEPSLIB_UNIX

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libcompiler.la

libcompiler_la_LDFLAGS = -module

libcompiler_la_LIBADD = ../../sdk/libcodeblocks.la \
				../../sdk/wxscintilla/libwxscintilla.la \
				depslib/src/libdepslib.la

libcompiler_la_SOURCES = advancedcompileroptionsdlg.cpp \
			compilerBCC.cpp \
			compilerDMC.cpp \
			compilererrors.cpp \
			compilergcc.cpp \
			compilermessages.cpp \
			compilerMINGW.cpp \
			compilerMSVC.cpp \
			compileroptionsdlg.cpp \
			compilerOW.cpp \
			compilerSDCC.cpp \
			directcommands.cpp \
			makefilegenerator.cpp

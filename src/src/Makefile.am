SUBDIRS = wxAUI resources

INCLUDES = $(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/sdk/wxscintilla/include \
			-I$(top_srcdir)/src/sdk \
			-I$(top_srcdir)/src/sdk/wxFlatNotebook \
			-I$(top_srcdir)/src/src/wxAUI

if CODEBLOCKS_LINUX
AM_CPPFLAGS = -DENABLE_BINRELOC -DAPP_PREFIX="\"@prefix@\""
endif

bin_PROGRAMS = codeblocks

codeblocks_LDFLAGS = 

codeblocks_LDADD =  -LwxAUI -lwxaui \
			-L../sdk -lcodeblocks \
			$(WX_LIBS) \
			$(WX_XRC)

codeblocks_SOURCES = app.cpp \
			appglobals.cpp \
			compilersettingsdlg.cpp \
			dlgabout.cpp \
			dlgaboutplugin.cpp \
			environmentsettingsdlg.cpp \
			main.cpp \
			prefix.cpp \
			printdlg.cpp \
			startherepage.cpp

noinst_HEADERS = app.h \
			appglobals.h \
			compilersettingsdlg.h \
			dlgabout.h \
			dlgaboutplugin.h \
			environmentsettingsdlg.h \
			main.h \
			prefix.h \
			printdlg.h \
			startherepage.h

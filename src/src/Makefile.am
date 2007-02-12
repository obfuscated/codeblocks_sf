SUBDIRS = wxAUI resources

INCLUDES = $(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/include/wxscintilla/include \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/include/scripting/include \
			-I$(top_srcdir)/src/include/scripting/sqplus \
			-I$(top_srcdir)/src/include/wxFlatNotebook/include \
			-I$(top_srcdir)/src/src/wxAUI

if CODEBLOCKS_LINUX
AM_CPPFLAGS = -DENABLE_BINRELOC -DAPP_PREFIX="\"@prefix@\""
endif

bin_PROGRAMS = codeblocks

codeblocks_LDFLAGS = 

codeblocks_LDADD =  -LwxAUI -lwxaui \
			-L../sdk -lcodeblocks \
			$(WX_LIBS)

codeblocks_SOURCES = app.cpp \
			appglobals.cpp \
			compilersettingsdlg.cpp \
			crashhandler.cpp \
			dlgabout.cpp \
			dlgaboutplugin.cpp \
			environmentsettingsdlg.cpp \
			main.cpp \
			prefix.cpp \
			printdlg.cpp \
			scriptconsole.cpp \
			scriptingsettingsdlg.cpp \
			splashscreen.cpp \
			startherepage.cpp

noinst_HEADERS = app.h \
			appglobals.h \
			compilersettingsdlg.h \
			crashhandler.h \
			dlgabout.h \
			dlgaboutplugin.h \
			environmentsettingsdlg.h \
			main.h \
			prefix.h \
			printdlg.h \
			scriptconsole.h \
			scriptingsettingsdlg.h \
			splashscreen.h \
			startherepage.h

man_MANS = codeblocks.1

EXTRA_DIST = associations.cpp \
            associations.h \
            $(man_MANS)

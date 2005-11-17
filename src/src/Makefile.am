SUBDIRS = wxDockit wxKeyBinder resources

INCLUDES = $(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/sdk/wxscintilla/include \
			-I$(top_srcdir)/src/sdk \
			-I$(top_srcdir)/src/src/wxDockit/include \
			-I$(top_srcdir)/src/src/wxKeyBinder/include

if CODEBLOCKS_LINUX
AM_CPPFLAGS = -DENABLE_BINRELOC -DAPP_PREFIX="\"@prefix@\""
endif

bin_PROGRAMS = codeblocks

codeblocks_LDFLAGS = 

codeblocks_LDADD =  $(WX_LIBS) \
			-LwxDockit -lwxdockit \
			-LwxKeyBinder -lwxkeybinder \
			-L../sdk -lcodeblocks \
			$(WX_XRC)

codeblocks_SOURCES = app.cpp \
			dlgabout.cpp \
			dlgaboutplugin.cpp \
			environmentsettingsdlg.cpp \
			main.cpp \
			prefix.cpp \
			printdlg.cpp \
			startherepage.cpp

noinst_HEADERS = app.h \
			dlgabout.h \
			dlgaboutplugin.h \
			environmentsettingsdlg.h \
			globals.h \
			main.h \
			prefix.h \
			printdlg.h \
			startherepage.h

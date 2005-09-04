SUBDIRS = wxDockit resources

INCLUDES = $(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/sdk/wxscintilla/include \
			-I$(top_srcdir)/src/sdk \
			-IwxDockit/include

AM_CXXFLAGS = -Wall -g -DENABLE_BINRELOC

bin_PROGRAMS = codeblocks

codeblocks_SOURCES = app.cpp dlgabout.cpp dlgaboutplugin.cpp environmentsettingsdlg.cpp impexpconfig.cpp main.cpp prefix.c printdlg.cpp startherepage.cpp

codeblocks_LDFLAGS = 

codeblocks_LDADD =  $(WX_LIBS) \
			-LwxDockit -lwxdockit \
			-L$(top_srcdir)/src/sdk -lcodeblocks \
			$(WX_XRC)


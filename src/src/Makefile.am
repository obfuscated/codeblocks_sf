SUBDIRS = wxDockit resources

INCLUDES = $(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/sdk/wxscintilla/include \
			-I$(top_srcdir)/src/sdk \
			-I$(top_srcdir)/src/src/wxDockit/include

if LINUX
AM_CPPFLAGS = -DENABLE_BINRELOC
endif

bin_PROGRAMS = codeblocks

codeblocks_SOURCES = app.cpp dlgabout.cpp dlgaboutplugin.cpp environmentsettingsdlg.cpp impexpconfig.cpp main.cpp prefix.cpp printdlg.cpp startherepage.cpp

codeblocks_LDFLAGS = 

codeblocks_LDADD =  $(WX_LIBS) \
			-LwxDockit -lwxdockit \
			-L../sdk -lcodeblocks \
			$(WX_XRC)


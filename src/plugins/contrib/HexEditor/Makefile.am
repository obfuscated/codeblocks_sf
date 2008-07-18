INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libhexeditor.la

libhexeditor_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libhexeditor_la_LIBADD = $(PLUGIN_WX_LIBS) ../../../sdk/libcodeblocks.la

libhexeditor_la_SOURCES = \
	FileContentBase.cpp \
	FileContentBuffered.cpp \
	HexEditor.cpp \
	HexEditPanel.cpp

noinst_HEADERS = \
	FileContentBase.h \
	FileContentBuffered.h \
	HexEditor.h \
	HexEditPanel.h


EXTRA_DIST = \
	wxsmith/HexEditPanel.wxs \
	HexEditor-lin.cbp \
	HexEditor-win.cbp \
	manifest.xml

pkgdata_DATA = hexeditor.zip
CLEANFILES = $(pkgdata_DATA)

hexeditor.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/hexeditor.zip manifest.xml > /dev/null

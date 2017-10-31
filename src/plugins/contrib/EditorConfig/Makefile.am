AM_CPPFLAGS = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libEditorConfig.la

libEditorConfig_la_LDFLAGS = @MODULE_SHARED_LDFLAGS@ -version-info 0:1:0 -no-undefined -avoid-version

libEditorConfig_la_LIBADD = ../../../sdk/libcodeblocks.la \
			$(WX_LIBS) \
			$(CB_TINYXML_LIBS)

libEditorConfig_la_SOURCES = EditorConfigUI.cpp \
			EditorConfigCommon.cpp \
			EditorConfig.cpp

noinst_HEADERS = EditorConfig.h \
			EditorConfigCommon.h \
			EditorConfigUI.h

EXTRA_DIST = $(srcdir)/*.cbp \
			wxsmith/EditorConfigUI.wxs \
			manifest.xml

pkgdata_DATA = EditorConfig.zip
CLEANFILES = $(pkgdata_DATA)

EditorConfig.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/EditorConfig.zip manifest.xml > /dev/null

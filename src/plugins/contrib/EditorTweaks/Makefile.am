INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libEditorTweaks.la

libEditorTweaks_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libEditorTweaks_la_LIBADD = $(PLUGIN_WX_LIBS) ../../../sdk/libcodeblocks.la

libEditorTweaks_la_SOURCES = EditorTweaks.cpp

noinst_HEADERS = EditorTweaks.h

EXTRA_DIST = 	$(srcdir)/*.cbp \
		manifest.xml

pkgdata_DATA = EditorTweaks.zip

CLEANFILES = $(pkgdata_DATA)

EditorTweaks.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/EditorTweaks.zip manifest.xml > /dev/null

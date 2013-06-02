INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libEditorTweaks.la

libEditorTweaks_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libEditorTweaks_la_LIBADD = ../../../sdk/libcodeblocks.la $(WX_LIBS)

libEditorTweaks_la_SOURCES = EditorTweaks.cpp \
		EditorTweaksConfDlg.cpp

noinst_HEADERS = EditorTweaks.h \
		EditorTweaksConfDlg.h

EXTRA_DIST = 	$(srcdir)/*.cbp \
		EditorTweaksConfDlg.xrc \
		manifest.xml \
		$(srcdir)/wxsmith/*.wxs

pkgdata_DATA = EditorTweaks.zip

CLEANFILES = $(pkgdata_DATA)

EditorTweaks.zip: $(EXTRA_DIST)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/EditorTweaks.zip manifest.xml *.xrc > /dev/null

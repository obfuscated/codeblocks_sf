AM_CPPFLAGS = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libReopenEditor.la

libReopenEditor_la_LDFLAGS = @MODULE_SHARED_LDFLAGS@ -version-info 0:1:0 -no-undefined -avoid-version

libReopenEditor_la_LIBADD = ../../../sdk/libcodeblocks.la \
							$(WX_LIBS)
libReopenEditor_la_SOURCES = ReopenEditor.cpp \
			ReopenEditorConfDLg.cpp \
			ReopenEditorListView.cpp

noinst_HEADERS = ReopenEditor.h \
		ReopenEditorConfDLg.h \
		ReopenEditorListView.h

EXTRA_DIST = 	$(srcdir)/*.cbp \
		$(srcdir)/*.xrc \
		$(srcdir)/wxsmith/*.wxs \
		manifest.xml

pkgdata_DATA = ReopenEditor.zip

CLEANFILES = $(pkgdata_DATA)

ReopenEditor.zip: manifest.xml ReopenEditorConfDLg.xrc
	PWD=`pwd` cd $(srcdir) && zip -R $(PWD)/ReopenEditor.zip manifest.xml  *.xrc > /dev/null

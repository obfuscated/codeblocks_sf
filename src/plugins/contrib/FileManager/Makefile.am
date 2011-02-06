INCLUDES = $(WX_CXXFLAGS) \
		$(CB_GIO2_CFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include \
		-I$(top_srcdir)/src/include/wxFlatNotebook/include

CXXFLAGS = @CXXFLAGS@ -DCB_AUI -D__GIO__

pluginlibdir = $(pkglibdir)/plugins
pkgdata_DATA = FileManager.zip

pluginlib_LTLIBRARIES = libFileManager.la

libFileManager_la_LDFLAGS = $(CB_GIO2_LIBS) -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libFileManager_la_LIBADD = $(PLUGIN_WX_LIBS) ../../../sdk/libcodeblocks.la

libFileManager_la_SOURCES = FileManager.cpp FileExplorer.cpp FileExplorerSettings.cpp FileExplorerUpdater.cpp directorymonitor.cpp se_globals.cpp

noinst_HEADERS = FileManager.h FileExplorer.h FileExplorerSettings.h FileExplorerUpdater.h directorymonitor.h se_globals.h

EXTRA_DIST = FileManager.cbp FileManager-unix.cbp Resources/fileexplorersettings.xrc manifest.xml

CLEANFILES = $(pkgdata_DATA)

FileManager.zip: manifest.xml Resources/fileexplorersettings.xrc
	PWD=`pwd` cd $(srcdir) && zip -j9 $(PWD)/FileManager.zip manifest.xml Resources/fileexplorersettings.xrc > /dev/null

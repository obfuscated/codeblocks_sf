INCLUDES = $(WX_CXXFLAGS) \
		$(CB_GAMIN_CFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include \
		-I$(top_srcdir)/src/include/mozilla_chardet \
		-I$(top_srcdir)/src/include/mozilla_chardet/mfbt \
		-I$(top_srcdir)/src/include/mozilla_chardet/nsprpub/pr/include \
		-I$(top_srcdir)/src/include/mozilla_chardet/xpcom \
		-I$(top_srcdir)/src/include/mozilla_chardet/xpcom/base \
		-I$(top_srcdir)/src/include/mozilla_chardet/xpcom/glue

CXXFLAGS = @CXXFLAGS@ -D__FAM__

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libFileManager.la

libFileManager_la_LDFLAGS = $(CB_GAMIN_LIBS) -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libFileManager_la_LIBADD = ../../../sdk/libcodeblocks.la $(WX_LIBS)

libFileManager_la_SOURCES = FileManager.cpp \
			FileExplorer.cpp \
			FileExplorerSettings.cpp \
			FileExplorerUpdater.cpp \
			directorymonitor.cpp \
			se_globals.cpp

noinst_HEADERS =  	FileManager.h \
			FileExplorer.h \
			FileExplorerSettings.h \
			FileExplorerUpdater.h \
			directorymonitor.h \
			se_globals.h

EXTRA_DIST = 	$(srcdir)/FileManager.cbp \
		$(srcdir)/FileManager-unix.cbp \
		$(srcdir)/Resources/fileexplorersettings.xrc \
		$(srcdir)/manifest.xml

pkgdata_DATA = FileManager.zip
CLEANFILES = $(pkgdata_DATA)

FileManager.zip: manifest.xml Resources/fileexplorersettings.xrc
	PWD=`pwd` cd $(srcdir) && zip -j9 $(PWD)/FileManager.zip manifest.xml Resources/fileexplorersettings.xrc > /dev/null

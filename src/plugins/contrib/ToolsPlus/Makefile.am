INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include \

CXXFLAGS = @CXXFLAGS@

pluginlibdir = $(pkglibdir)/plugins
pkgdata_DATA = ToolsPlus.zip

pluginlib_LTLIBRARIES = libToolsPlus.la

libToolsPlus_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libToolsPlus_la_LIBADD = $(PLUGIN_WX_LIBS) ../../../sdk/libcodeblocks.la

libToolsPlus_la_SOURCES = ToolsPlus.cpp CmdConfigDialog.cpp PipedProcessCtrl.cpp se_globals.cpp ShellCtrlBase.cpp shellproperties.cpp

noinst_HEADERS = ToolsPlus.h CmdConfigDialog.h PipedProcessCtrl.h se_globals.h ShellCtrlBase.h shellproperties.h

EXTRA_DIST = ToolsPlus.cbp ToolsPlus-unix.cbp

CLEANFILES = $(pkgdata_DATA)

ToolsPlus.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip -j9 $(PWD)/ToolsPlus.zip manifest.xml > /dev/null


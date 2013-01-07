SUBDIRS = Resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include \
		-I$(top_srcdir)/src/include/mozilla_chardet \
		-I$(top_srcdir)/src/include/mozilla_chardet/mfbt \
		-I$(top_srcdir)/src/include/mozilla_chardet/nsprpub/pr/include \
		-I$(top_srcdir)/src/include/mozilla_chardet/xpcom \
		-I$(top_srcdir)/src/include/mozilla_chardet/xpcom/base \
		-I$(top_srcdir)/src/include/mozilla_chardet/xpcom/glue

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libToolsPlus.la

libToolsPlus_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libToolsPlus_la_LIBADD = $(PLUGIN_WX_LIBS) ../../../sdk/libcodeblocks.la

libToolsPlus_la_SOURCES = CmdConfigDialog.cpp \
			PipedProcessCtrl.cpp \
			se_globals.cpp \
			ShellCtrlBase.cpp \
			shellproperties.cpp \
			ToolsPlus.cpp


noinst_HEADERS =  	CmdConfigDialog.h \
			PipedProcessCtrl.h \
			se_globals.h \
			ShellCtrlBase.h \
			shellproperties.h \
			ToolsPlus.h


EXTRA_DIST = 	$(srcdir)/ToolsPlus.cbp \
		$(srcdir)/ToolsPlus-unix.cbp \
		$(srcdir)/update.bat \
		$(srcdir)/update \
		$(srcdir)/manifest.xml

pkgdata_DATA = ToolsPlus.zip
CLEANFILES = $(pkgdata_DATA)

ToolsPlus.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip -j9 $(PWD)/ToolsPlus.zip manifest.xml > /dev/null


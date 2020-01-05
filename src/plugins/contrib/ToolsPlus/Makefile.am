SUBDIRS = Resources

AM_CPPFLAGS = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libToolsPlus.la

libToolsPlus_la_LDFLAGS = @MODULE_SHARED_LDFLAGS@ -version-info 0:1:0 -no-undefined -avoid-version

libToolsPlus_la_LIBADD = ../../../sdk/libcodeblocks.la $(WX_LIBS)

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


EXTRA_DIST = 	$(srcdir)/*.cbp \
		$(srcdir)/update* \
		$(srcdir)/manifest.xml

pkgdata_DATA = ToolsPlus.zip
CLEANFILES = $(pkgdata_DATA)

ToolsPlus.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip -j9 $(PWD)/ToolsPlus.zip manifest.xml > /dev/null


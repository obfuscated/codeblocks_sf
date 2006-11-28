INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

libdir = $(pkgdatadir)/plugins
pkgdata_DATA = RegExTestbed.zip

lib_LTLIBRARIES = libRegExTestbed.la

libRegExTestbed_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libRegExTestbed_la_LIBADD = ../../../sdk/libcodeblocks.la

libRegExTestbed_la_SOURCES = regexdlg.cpp RegExTestbed.cpp

noinst_HEADERS = regexdlg.h RegExTestbed.h

EXTRA_DIST = RegExTestbed.cbp RegExTestbed-unix.cbp regexdlg.xrc manifest.xml wxsmith\RegExDlg.wxs

CLEANFILES = $(pkgdata_DATA)

RegExTestbed.zip: manifest.xml regexdlg.xrc
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/RegExTestbed.zip manifest.xml regexdlg.xrc > /dev/null

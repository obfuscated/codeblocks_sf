INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxscintilla/include

libdir = $(pkgdatadir)/plugins
pkgdata_DATA = RegExTestbed.zip

lib_LTLIBRARIES = libRegExTestbed.la

libRegExTestbed_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libRegExTestbed_la_LIBADD = $(PLUGIN_WX_LIBS) ../../../sdk/libcodeblocks.la

libRegExTestbed_la_SOURCES = regexdlg.cpp RegExTestbed.cpp

noinst_HEADERS = regexdlg.h RegExTestbed.h

EXTRA_DIST = $(srcdir)/RegExTestbed.cbp \
	$(srcdir)/RegExTestbed-unix.cbp \
	$(srcdir)/regexdlg.xrc \
	$(srcdir)/manifest.xml \
	$(srcdir)/wxsmith/RegExDlg.wxs

CLEANFILES = $(pkgdata_DATA)

RegExTestbed.zip: manifest.xml regexdlg.xrc
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/RegExTestbed.zip manifest.xml regexdlg.xrc > /dev/null

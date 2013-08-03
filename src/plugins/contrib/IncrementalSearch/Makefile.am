AM_CPPFLAGS = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libIncrementalSearch.la

libIncrementalSearch_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libIncrementalSearch_la_LIBADD = ../../../sdk/libcodeblocks.la \
							$(WX_LIBS)
libIncrementalSearch_la_SOURCES = IncrementalSearch.cpp \
				IncrementalSearchConfDlg.cpp

noinst_HEADERS = IncrementalSearch.h \
		IncrementalSearchConfDlg.h

EXTRA_DIST = 	$(srcdir)/*.cbp \
		IncrementalSearchToolbar.xrc \
		IncrementalSearchConfDlg.xrc \
		manifest.xml \
		$(srcdir)/update* \
		$(srcdir)/images/*.png \
		$(srcdir)/images/16x16/*.png

pkgdata_DATA = IncrementalSearch.zip

CLEANFILES = $(pkgdata_DATA)

IncrementalSearch.zip: manifest.xml IncrementalSearchToolbar.xrc IncrementalSearchConfDlg.xrc
	PWD=`pwd` cd $(srcdir) && zip -R $(PWD)/IncrementalSearch.zip manifest.xml *.xrc images/*.png images/16x16/*.png > /dev/null

confimagesdir = $(datadir)/@PACKAGE@/images/settings

dist_confimages_DATA = $(srcdir)/*.png

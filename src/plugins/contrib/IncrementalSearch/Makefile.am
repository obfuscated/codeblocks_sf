AM_CPPFLAGS = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libIncrementalSearch.la

libIncrementalSearch_la_LDFLAGS = @MODULE_SHARED_LDFLAGS@ -version-info 0:1:0 -no-undefined -avoid-version

libIncrementalSearch_la_LIBADD = ../../../sdk/libcodeblocks.la \
							$(WX_LIBS)
libIncrementalSearch_la_SOURCES = IncrementalSearch.cpp \
				IncrementalSearchConfDlg.cpp

noinst_HEADERS = IncrementalSearch.h \
		IncrementalSearchConfDlg.h

resources_forZIP = \
	IncrementalSearchToolbar.xrc \
	IncrementalSearchConfDlg.xrc \
	manifest.xml \
	images/16x16/incsearchcase.png \
	images/16x16/incsearchclear.png \
	images/16x16/incsearchfocus.png \
	images/16x16/incsearchhighlight.png \
	images/16x16/incsearchnext.png \
	images/16x16/incsearchprev.png \
	images/16x16/incsearchregex.png \
	images/16x16/incsearchselectedonly.png \
	images/22x22/incsearchcase.png \
	images/22x22/incsearchclear.png \
	images/22x22/incsearchfocus.png \
	images/22x22/incsearchhighlight.png \
	images/22x22/incsearchnext.png \
	images/22x22/incsearchprev.png \
	images/22x22/incsearchregex.png \
	images/22x22/incsearchselectedonly.png \
	images/32x32/incsearchcase.png \
	images/32x32/incsearchclear.png \
	images/32x32/incsearchfocus.png \
	images/32x32/incsearchhighlight.png \
	images/32x32/incsearchnext.png \
	images/32x32/incsearchprev.png \
	images/32x32/incsearchregex.png \
	images/32x32/incsearchselectedonly.png

EXTRA_DIST = \
	$(srcdir)/*.cbp \
	$(srcdir)/update* \
	$(resources_forZIP)

pkgdata_DATA = IncrementalSearch.zip

CLEANFILES = $(pkgdata_DATA)

IncrementalSearch.zip: $(resources_forZIP)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/IncrementalSearch.zip $(resources_forZIP) > /dev/null

confimagesdir = $(datadir)/@PACKAGE@/images/settings

dist_confimages_DATA = $(srcdir)/*.png

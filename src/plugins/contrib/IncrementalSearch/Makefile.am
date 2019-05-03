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
	\
	images/20x20/incsearchcase.png \
	images/20x20/incsearchclear.png \
	images/20x20/incsearchfocus.png \
	images/20x20/incsearchhighlight.png \
	images/20x20/incsearchnext.png \
	images/20x20/incsearchprev.png \
	images/20x20/incsearchregex.png \
	images/20x20/incsearchselectedonly.png \
	\
	images/22x22/incsearchcase.png \
	images/22x22/incsearchclear.png \
	images/22x22/incsearchfocus.png \
	images/22x22/incsearchhighlight.png \
	images/22x22/incsearchnext.png \
	images/22x22/incsearchprev.png \
	images/22x22/incsearchregex.png \
	images/22x22/incsearchselectedonly.png \
	\
	images/24x24/incsearchcase.png \
	images/24x24/incsearchclear.png \
	images/24x24/incsearchfocus.png \
	images/24x24/incsearchhighlight.png \
	images/24x24/incsearchnext.png \
	images/24x24/incsearchprev.png \
	images/24x24/incsearchregex.png \
	images/24x24/incsearchselectedonly.png \
	\
	images/28x28/incsearchcase.png \
	images/28x28/incsearchclear.png \
	images/28x28/incsearchfocus.png \
	images/28x28/incsearchhighlight.png \
	images/28x28/incsearchnext.png \
	images/28x28/incsearchprev.png \
	images/28x28/incsearchregex.png \
	images/28x28/incsearchselectedonly.png \
	\
	images/32x32/incsearchcase.png \
	images/32x32/incsearchclear.png \
	images/32x32/incsearchfocus.png \
	images/32x32/incsearchhighlight.png \
	images/32x32/incsearchnext.png \
	images/32x32/incsearchprev.png \
	images/32x32/incsearchregex.png \
	images/32x32/incsearchselectedonly.png \
	images/40x40/incsearchcase.png \
	images/40x40/incsearchclear.png \
	images/40x40/incsearchfocus.png \
	images/40x40/incsearchhighlight.png \
	images/40x40/incsearchnext.png \
	images/40x40/incsearchprev.png \
	images/40x40/incsearchregex.png \
	images/40x40/incsearchselectedonly.png \
	images/48x48/incsearchcase.png \
	images/48x48/incsearchclear.png \
	images/48x48/incsearchfocus.png \
	images/48x48/incsearchhighlight.png \
	images/48x48/incsearchnext.png \
	images/48x48/incsearchprev.png \
	images/48x48/incsearchregex.png \
	images/48x48/incsearchselectedonly.png \
	images/56x56/incsearchcase.png \
	images/56x56/incsearchclear.png \
	images/56x56/incsearchfocus.png \
	images/56x56/incsearchhighlight.png \
	images/56x56/incsearchnext.png \
	images/56x56/incsearchprev.png \
	images/56x56/incsearchregex.png \
	images/56x56/incsearchselectedonly.png \
	images/64x64/incsearchcase.png \
	images/64x64/incsearchclear.png \
	images/64x64/incsearchfocus.png \
	images/64x64/incsearchhighlight.png \
	images/64x64/incsearchnext.png \
	images/64x64/incsearchprev.png \
	images/64x64/incsearchregex.png \
	images/64x64/incsearchselectedonly.png

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

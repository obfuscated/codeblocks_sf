AM_CPPFLAGS = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libDoxyBlocks.la

libDoxyBlocks_la_LDFLAGS = @MODULE_SHARED_LDFLAGS@ -version-info 0:1:0 -no-undefined -avoid-version

libDoxyBlocks_la_LIBADD = ../../../sdk/libcodeblocks.la \
			$(WX_LIBS) \
			$(CB_TINYXML_LIBS)

libDoxyBlocks_la_SOURCES = AutoDoc.cpp \
				Config.cpp \
				ConfigPanel.cpp \
				DoxyBlocks.cpp \
				ExtractDocs.cpp \
				DoxyBlocksLogger.cpp

noinst_HEADERS = Config.h \
		ConfigPanel.h \
		DoxyBlocks.h \
		Expressions.h \
		DoxyBlocksLogger.h \
		version.h

EXTRA_DIST = $(srcdir)/*.cbp \
		wxsmith/ConfigPanel.wxs \
		manifest.xml \
		ChangeLog.txt \
		DoxyBlocks.rc \
		$(srcdir)/update* \
		$(srcdir)/images/16x16/*.png
		$(srcdir)/images/22x22/*.png

pkgdata_DATA = DoxyBlocks.zip

CLEANFILES = $(pkgdata_DATA)

DoxyBlocks.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip -R $(PWD)/DoxyBlocks.zip manifest.xml > /dev/null

confimagesdir = $(datadir)/@PACKAGE@/images/settings
dist_confimages_DATA = $(srcdir)/*.png

images16x16dir = $(datadir)/@PACKAGE@/images/DoxyBlocks/16x16
dist_images16x16_DATA = \
	$(srcdir)/images/16x16/chm.png \
	$(srcdir)/images/16x16/comment_block.png \
	$(srcdir)/images/16x16/comment_line.png \
	$(srcdir)/images/16x16/configure.png \
	$(srcdir)/images/16x16/doxywizard.png \
	$(srcdir)/images/16x16/extract.png \
	$(srcdir)/images/16x16/html.png

images22x22dir = $(datadir)/@PACKAGE@/images/DoxyBlocks/22x22
dist_images22x22_DATA = \
	$(srcdir)/images/22x22/chm.png \
	$(srcdir)/images/22x22/comment_block.png \
	$(srcdir)/images/22x22/comment_line.png \
	$(srcdir)/images/22x22/configure.png \
	$(srcdir)/images/22x22/doxywizard.png \
	$(srcdir)/images/22x22/extract.png \
	$(srcdir)/images/22x22/html.png

images32x32dir = $(datadir)/@PACKAGE@/images/DoxyBlocks/32x32
dist_images32x32_DATA = \
	$(srcdir)/images/32x32/chm.png \
	$(srcdir)/images/32x32/comment_block.png \
	$(srcdir)/images/32x32/comment_line.png \
	$(srcdir)/images/32x32/configure.png \
	$(srcdir)/images/32x32/doxywizard.png \
	$(srcdir)/images/32x32/extract.png \
	$(srcdir)/images/32x32/html.png

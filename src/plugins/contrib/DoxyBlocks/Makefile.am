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

images20x20dir = $(datadir)/@PACKAGE@/images/DoxyBlocks/20x20
dist_images20x20_DATA = \
	$(srcdir)/images/20x20/chm.png \
	$(srcdir)/images/20x20/comment_block.png \
	$(srcdir)/images/20x20/comment_line.png \
	$(srcdir)/images/20x20/configure.png \
	$(srcdir)/images/20x20/doxywizard.png \
	$(srcdir)/images/20x20/extract.png \
	$(srcdir)/images/20x20/html.png

images22x22dir = $(datadir)/@PACKAGE@/images/DoxyBlocks/22x22
dist_images22x22_DATA = \
	$(srcdir)/images/22x22/chm.png \
	$(srcdir)/images/22x22/comment_block.png \
	$(srcdir)/images/22x22/comment_line.png \
	$(srcdir)/images/22x22/configure.png \
	$(srcdir)/images/22x22/doxywizard.png \
	$(srcdir)/images/22x22/extract.png \
	$(srcdir)/images/22x22/html.png

images24x24dir = $(datadir)/@PACKAGE@/images/DoxyBlocks/24x24
dist_images24x24_DATA = \
	$(srcdir)/images/24x24/chm.png \
	$(srcdir)/images/24x24/comment_block.png \
	$(srcdir)/images/24x24/comment_line.png \
	$(srcdir)/images/24x24/configure.png \
	$(srcdir)/images/24x24/doxywizard.png \
	$(srcdir)/images/24x24/extract.png \
	$(srcdir)/images/24x24/html.png

images28x28dir = $(datadir)/@PACKAGE@/images/DoxyBlocks/28x28
dist_images28x28_DATA = \
	$(srcdir)/images/28x28/chm.png \
	$(srcdir)/images/28x28/comment_block.png \
	$(srcdir)/images/28x28/comment_line.png \
	$(srcdir)/images/28x28/configure.png \
	$(srcdir)/images/28x28/doxywizard.png \
	$(srcdir)/images/28x28/extract.png \
	$(srcdir)/images/28x28/html.png

images32x32dir = $(datadir)/@PACKAGE@/images/DoxyBlocks/32x32
dist_images32x32_DATA = \
	$(srcdir)/images/32x32/chm.png \
	$(srcdir)/images/32x32/comment_block.png \
	$(srcdir)/images/32x32/comment_line.png \
	$(srcdir)/images/32x32/configure.png \
	$(srcdir)/images/32x32/doxywizard.png \
	$(srcdir)/images/32x32/extract.png \
	$(srcdir)/images/32x32/html.png

images40x40dir = $(datadir)/@PACKAGE@/images/DoxyBlocks/40x40
dist_images40x40_DATA = \
	$(srcdir)/images/40x40/chm.png \
	$(srcdir)/images/40x40/comment_block.png \
	$(srcdir)/images/40x40/comment_line.png \
	$(srcdir)/images/40x40/configure.png \
	$(srcdir)/images/40x40/doxywizard.png \
	$(srcdir)/images/40x40/extract.png \
	$(srcdir)/images/40x40/html.png

images48x48dir = $(datadir)/@PACKAGE@/images/DoxyBlocks/48x48
dist_images48x48_DATA = \
	$(srcdir)/images/48x48/chm.png \
	$(srcdir)/images/48x48/comment_block.png \
	$(srcdir)/images/48x48/comment_line.png \
	$(srcdir)/images/48x48/configure.png \
	$(srcdir)/images/48x48/doxywizard.png \
	$(srcdir)/images/48x48/extract.png \
	$(srcdir)/images/48x48/html.png

images56x56dir = $(datadir)/@PACKAGE@/images/DoxyBlocks/56x56
dist_images56x56_DATA = \
	$(srcdir)/images/56x56/chm.png \
	$(srcdir)/images/56x56/comment_block.png \
	$(srcdir)/images/56x56/comment_line.png \
	$(srcdir)/images/56x56/configure.png \
	$(srcdir)/images/56x56/doxywizard.png \
	$(srcdir)/images/56x56/extract.png \
	$(srcdir)/images/56x56/html.png

images64x64dir = $(datadir)/@PACKAGE@/images/DoxyBlocks/64x64
dist_images64x64_DATA = \
	$(srcdir)/images/64x64/chm.png \
	$(srcdir)/images/64x64/comment_block.png \
	$(srcdir)/images/64x64/comment_line.png \
	$(srcdir)/images/64x64/configure.png \
	$(srcdir)/images/64x64/doxywizard.png \
	$(srcdir)/images/64x64/extract.png \
	$(srcdir)/images/64x64/html.png

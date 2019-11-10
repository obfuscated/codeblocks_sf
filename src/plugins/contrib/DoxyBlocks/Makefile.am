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

resources_forZIP = \
	images/16x16/chm.png \
	images/16x16/comment_block.png \
	images/16x16/comment_line.png \
	images/16x16/configure.png \
	images/16x16/doxywizard.png \
	images/16x16/extract.png \
	images/16x16/html.png \
	\
	images/20x20/chm.png \
	images/20x20/comment_block.png \
	images/20x20/comment_line.png \
	images/20x20/configure.png \
	images/20x20/doxywizard.png \
	images/20x20/extract.png \
	images/20x20/html.png \
	\
	images/24x24/chm.png \
	images/24x24/comment_block.png \
	images/24x24/comment_line.png \
	images/24x24/configure.png \
	images/24x24/doxywizard.png \
	images/24x24/extract.png \
	images/24x24/html.png \
	\
	images/28x28/chm.png \
	images/28x28/comment_block.png \
	images/28x28/comment_line.png \
	images/28x28/configure.png \
	images/28x28/doxywizard.png \
	images/28x28/extract.png \
	images/28x28/html.png \
	\
	images/32x32/chm.png \
	images/32x32/comment_block.png \
	images/32x32/comment_line.png \
	images/32x32/configure.png \
	images/32x32/doxywizard.png \
	images/32x32/extract.png \
	images/32x32/html.png \
	\
	images/40x40/chm.png \
	images/40x40/comment_block.png \
	images/40x40/comment_line.png \
	images/40x40/configure.png \
	images/40x40/doxywizard.png \
	images/40x40/extract.png \
	images/40x40/html.png \
	\
	images/48x48/chm.png \
	images/48x48/comment_block.png \
	images/48x48/comment_line.png \
	images/48x48/configure.png \
	images/48x48/doxywizard.png \
	images/48x48/extract.png \
	images/48x48/html.png \
	\
	images/56x56/chm.png \
	images/56x56/comment_block.png \
	images/56x56/comment_line.png \
	images/56x56/configure.png \
	images/56x56/doxywizard.png \
	images/56x56/extract.png \
	images/56x56/html.png \
	\
	images/64x64/chm.png \
	images/64x64/comment_block.png \
	images/64x64/comment_line.png \
	images/64x64/configure.png \
	images/64x64/doxywizard.png \
	images/64x64/extract.png \
	images/64x64/html.png

EXTRA_DIST = $(srcdir)/*.cbp \
		wxsmith/ConfigPanel.wxs \
		manifest.xml \
		ChangeLog.txt \
		DoxyBlocks.rc \
		$(srcdir)/update* \
		$(resources_forZIP)

pkgdata_DATA = DoxyBlocks.zip

CLEANFILES = $(pkgdata_DATA)

DoxyBlocks.zip: manifest.xml $(resources_forZIP)
	PWD=`pwd` cd $(srcdir) && zip -R $(PWD)/DoxyBlocks.zip manifest.xml $(resources_forZIP) > /dev/null

confimagesdir = $(datadir)/@PACKAGE@/images/settings
dist_confimages_DATA = $(srcdir)/*.png

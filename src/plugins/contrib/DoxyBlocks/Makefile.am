SUBDIRS = images

AM_CPPFLAGS = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libDoxyBlocks.la

libDoxyBlocks_la_LDFLAGS = @MODULE_SHARED_LDFLAGS@ -version-info 0:1:0 -no-undefined -avoid-version

libDoxyBlocks_la_LIBADD = ../../../sdk/libcodeblocks.la \
			$(WX_LIBS)

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
		$(srcdir)/images/*.png \
		$(srcdir)/images/16x16/*.png

pkgdata_DATA = DoxyBlocks.zip

CLEANFILES = $(pkgdata_DATA)

DoxyBlocks.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip -R $(PWD)/DoxyBlocks.zip manifest.xml images/*.png images/16x16/*.png > /dev/null

confimagesdir = $(datadir)/@PACKAGE@/images/settings

dist_confimages_DATA = $(srcdir)/*.png

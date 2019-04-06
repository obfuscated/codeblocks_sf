AM_CPPFLAGS = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include \
		-I$(top_srcdir)/src/include/mozilla_chardet \
		-I$(top_srcdir)/src/include/mozilla_chardet/mfbt \
		-I$(top_srcdir)/src/include/mozilla_chardet/nsprpub/pr/include \
		-I$(top_srcdir)/src/include/mozilla_chardet/xpcom \
		-I$(top_srcdir)/src/include/mozilla_chardet/xpcom/base \
		-I$(top_srcdir)/src/include/mozilla_chardet/xpcom/glue \
		-I$(srcdir)/../wxContribItems/wxthings/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libThreadSearch.la

libThreadSearch_la_LDFLAGS = @MODULE_SHARED_LDFLAGS@ -version-info 0:1:0 -no-undefined -avoid-version

libThreadSearch_la_LIBADD = ../../../sdk/libcodeblocks.la ../wxContribItems/wxthings/libwxcustombutton.la $(WX_LIBS)

libThreadSearch_la_SOURCES = DirectoryParamsPanel.cpp \
			InsertIndexManager.cpp \
			SearchInPanel.cpp \
			TextFileSearcher.cpp \
			TextFileSearcherRegEx.cpp \
			TextFileSearcherText.cpp \
			ThreadSearch.cpp \
			ThreadSearchConfPanel.cpp \
			ThreadSearchControlIds.cpp \
			ThreadSearchEvent.cpp \
			ThreadSearchFindData.cpp \
			ThreadSearchLoggerBase.cpp \
			ThreadSearchLoggerList.cpp \
			ThreadSearchLoggerTree.cpp \
			ThreadSearchThread.cpp \
			ThreadSearchTrace.cpp \
			ThreadSearchView.cpp \
			ThreadSearchViewManagerBase.cpp \
			ThreadSearchViewManagerLayout.cpp \
			ThreadSearchViewManagerMessagesNotebook.cpp

noinst_HEADERS =  	DirectoryParamsPanel.h \
			InsertIndexManager.h \
			logging.h \
			SearchInPanel.h \
			TextFileSearcher.h \
			TextFileSearcherRegEx.h \
			TextFileSearcherText.h \
			ThreadSearch.h \
			ThreadSearchCommon.h \
			ThreadSearchConfPanel.h \
			ThreadSearchControlIds.h \
			ThreadSearchEvent.h \
			ThreadSearchFindData.h \
			ThreadSearchLoggerBase.h \
			ThreadSearchLoggerList.h \
			ThreadSearchLoggerTree.h \
			ThreadSearchThread.h \
			ThreadSearchTrace.h \
			ThreadSearchView.h \
			ThreadSearchViewManagerBase.h \
			ThreadSearchViewManagerLayout.h \
			ThreadSearchViewManagerMessagesNotebook.h

EXTRA_DIST = 	$(srcdir)/*.cbp \
		$(srcdir)/ThreadSearch.wxg \
		$(srcdir)/update* \
		$(srcdir)/manifest.xml \
		$(srcdir)/resources/ThreadSearch.xcf

pkgdata_DATA = ThreadSearch.zip
CLEANFILES = $(pkgdata_DATA)

ThreadSearch.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/ThreadSearch.zip manifest.xml > /dev/null

confimagesdir = $(datadir)/@PACKAGE@/images/settings
dist_confimages_DATA = $(srcdir)/resources/*.png

images16x16dir = $(datadir)/@PACKAGE@/images/ThreadSearch/16x16
dist_images16x16_DATA = \
	$(srcdir)/resources/images/16x16/findfdisabled.png \
	$(srcdir)/resources/images/16x16/findf.png \
	$(srcdir)/resources/images/16x16/folderdisabled.png \
	$(srcdir)/resources/images/16x16/folder.png \
	$(srcdir)/resources/images/16x16/folderselected.png \
	$(srcdir)/resources/images/16x16/openfilesdisabled.png \
	$(srcdir)/resources/images/16x16/openfiles.png \
	$(srcdir)/resources/images/16x16/openfilesselected.png \
	$(srcdir)/resources/images/16x16/optionsactive.png \
	$(srcdir)/resources/images/16x16/optionsdisabled.png \
	$(srcdir)/resources/images/16x16/options.png \
	$(srcdir)/resources/images/16x16/projectdisabled.png \
	$(srcdir)/resources/images/16x16/project.png \
	$(srcdir)/resources/images/16x16/projectselected.png \
	$(srcdir)/resources/images/16x16/showdirdisabled.png \
	$(srcdir)/resources/images/16x16/showdir.png \
	$(srcdir)/resources/images/16x16/stopdisabled.png \
	$(srcdir)/resources/images/16x16/stop.png \
	$(srcdir)/resources/images/16x16/targetdisabled.png \
	$(srcdir)/resources/images/16x16/target.png \
	$(srcdir)/resources/images/16x16/targetselected.png \
	$(srcdir)/resources/images/16x16/workspacedisabled.png \
	$(srcdir)/resources/images/16x16/workspace.png \
	$(srcdir)/resources/images/16x16/workspaceselected.png

images22x22dir = $(datadir)/@PACKAGE@/images/ThreadSearch/22x22
dist_images22x22_DATA = \
	$(srcdir)/resources/images/22x22/findfdisabled.png \
	$(srcdir)/resources/images/22x22/findf.png \
	$(srcdir)/resources/images/22x22/folderdisabled.png \
	$(srcdir)/resources/images/22x22/folder.png \
	$(srcdir)/resources/images/22x22/folderselected.png \
	$(srcdir)/resources/images/22x22/openfilesdisabled.png \
	$(srcdir)/resources/images/22x22/openfiles.png \
	$(srcdir)/resources/images/22x22/openfilesselected.png \
	$(srcdir)/resources/images/22x22/optionsactive.png \
	$(srcdir)/resources/images/22x22/optionsdisabled.png \
	$(srcdir)/resources/images/22x22/options.png \
	$(srcdir)/resources/images/22x22/projectdisabled.png \
	$(srcdir)/resources/images/22x22/project.png \
	$(srcdir)/resources/images/22x22/projectselected.png \
	$(srcdir)/resources/images/22x22/showdirdisabled.png \
	$(srcdir)/resources/images/22x22/showdir.png \
	$(srcdir)/resources/images/22x22/stopdisabled.png \
	$(srcdir)/resources/images/22x22/stop.png \
	$(srcdir)/resources/images/22x22/targetdisabled.png \
	$(srcdir)/resources/images/22x22/target.png \
	$(srcdir)/resources/images/22x22/targetselected.png \
	$(srcdir)/resources/images/22x22/workspacedisabled.png \
	$(srcdir)/resources/images/22x22/workspace.png \
	$(srcdir)/resources/images/22x22/workspaceselected.png

images32x32dir = $(datadir)/@PACKAGE@/images/ThreadSearch/32x32
dist_images32x32_DATA = \
	$(srcdir)/resources/images/32x32/findfdisabled.png \
	$(srcdir)/resources/images/32x32/findf.png \
	$(srcdir)/resources/images/32x32/folderdisabled.png \
	$(srcdir)/resources/images/32x32/folder.png \
	$(srcdir)/resources/images/32x32/folderselected.png \
	$(srcdir)/resources/images/32x32/openfilesdisabled.png \
	$(srcdir)/resources/images/32x32/openfiles.png \
	$(srcdir)/resources/images/32x32/openfilesselected.png \
	$(srcdir)/resources/images/32x32/optionsactive.png \
	$(srcdir)/resources/images/32x32/optionsdisabled.png \
	$(srcdir)/resources/images/32x32/options.png \
	$(srcdir)/resources/images/32x32/projectdisabled.png \
	$(srcdir)/resources/images/32x32/project.png \
	$(srcdir)/resources/images/32x32/projectselected.png \
	$(srcdir)/resources/images/32x32/showdirdisabled.png \
	$(srcdir)/resources/images/32x32/showdir.png \
	$(srcdir)/resources/images/32x32/stopdisabled.png \
	$(srcdir)/resources/images/32x32/stop.png \
	$(srcdir)/resources/images/32x32/targetdisabled.png \
	$(srcdir)/resources/images/32x32/target.png \
	$(srcdir)/resources/images/32x32/targetselected.png \
	$(srcdir)/resources/images/32x32/workspacedisabled.png \
	$(srcdir)/resources/images/32x32/workspace.png \
	$(srcdir)/resources/images/32x32/workspaceselected.png

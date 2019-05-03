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

images20x20dir = $(datadir)/@PACKAGE@/images/ThreadSearch/20x20
dist_images20x20_DATA = \
	$(srcdir)/resources/images/20x20/findfdisabled.png \
	$(srcdir)/resources/images/20x20/findf.png \
	$(srcdir)/resources/images/20x20/folderdisabled.png \
	$(srcdir)/resources/images/20x20/folder.png \
	$(srcdir)/resources/images/20x20/folderselected.png \
	$(srcdir)/resources/images/20x20/openfilesdisabled.png \
	$(srcdir)/resources/images/20x20/openfiles.png \
	$(srcdir)/resources/images/20x20/openfilesselected.png \
	$(srcdir)/resources/images/20x20/optionsactive.png \
	$(srcdir)/resources/images/20x20/optionsdisabled.png \
	$(srcdir)/resources/images/20x20/options.png \
	$(srcdir)/resources/images/20x20/projectdisabled.png \
	$(srcdir)/resources/images/20x20/project.png \
	$(srcdir)/resources/images/20x20/projectselected.png \
	$(srcdir)/resources/images/20x20/showdirdisabled.png \
	$(srcdir)/resources/images/20x20/showdir.png \
	$(srcdir)/resources/images/20x20/stopdisabled.png \
	$(srcdir)/resources/images/20x20/stop.png \
	$(srcdir)/resources/images/20x20/targetdisabled.png \
	$(srcdir)/resources/images/20x20/target.png \
	$(srcdir)/resources/images/20x20/targetselected.png \
	$(srcdir)/resources/images/20x20/workspacedisabled.png \
	$(srcdir)/resources/images/20x20/workspace.png \
	$(srcdir)/resources/images/20x20/workspaceselected.png

images24x24dir = $(datadir)/@PACKAGE@/images/ThreadSearch/24x24
dist_images24x24_DATA = \
	$(srcdir)/resources/images/24x24/findfdisabled.png \
	$(srcdir)/resources/images/24x24/findf.png \
	$(srcdir)/resources/images/24x24/folderdisabled.png \
	$(srcdir)/resources/images/24x24/folder.png \
	$(srcdir)/resources/images/24x24/folderselected.png \
	$(srcdir)/resources/images/24x24/openfilesdisabled.png \
	$(srcdir)/resources/images/24x24/openfiles.png \
	$(srcdir)/resources/images/24x24/openfilesselected.png \
	$(srcdir)/resources/images/24x24/optionsactive.png \
	$(srcdir)/resources/images/24x24/optionsdisabled.png \
	$(srcdir)/resources/images/24x24/options.png \
	$(srcdir)/resources/images/24x24/projectdisabled.png \
	$(srcdir)/resources/images/24x24/project.png \
	$(srcdir)/resources/images/24x24/projectselected.png \
	$(srcdir)/resources/images/24x24/showdirdisabled.png \
	$(srcdir)/resources/images/24x24/showdir.png \
	$(srcdir)/resources/images/24x24/stopdisabled.png \
	$(srcdir)/resources/images/24x24/stop.png \
	$(srcdir)/resources/images/24x24/targetdisabled.png \
	$(srcdir)/resources/images/24x24/target.png \
	$(srcdir)/resources/images/24x24/targetselected.png \
	$(srcdir)/resources/images/24x24/workspacedisabled.png \
	$(srcdir)/resources/images/24x24/workspace.png \
	$(srcdir)/resources/images/24x24/workspaceselected.png

images28x28dir = $(datadir)/@PACKAGE@/images/ThreadSearch/28x28
dist_images28x28_DATA = \
	$(srcdir)/resources/images/28x28/findfdisabled.png \
	$(srcdir)/resources/images/28x28/findf.png \
	$(srcdir)/resources/images/28x28/folderdisabled.png \
	$(srcdir)/resources/images/28x28/folder.png \
	$(srcdir)/resources/images/28x28/folderselected.png \
	$(srcdir)/resources/images/28x28/openfilesdisabled.png \
	$(srcdir)/resources/images/28x28/openfiles.png \
	$(srcdir)/resources/images/28x28/openfilesselected.png \
	$(srcdir)/resources/images/28x28/optionsactive.png \
	$(srcdir)/resources/images/28x28/optionsdisabled.png \
	$(srcdir)/resources/images/28x28/options.png \
	$(srcdir)/resources/images/28x28/projectdisabled.png \
	$(srcdir)/resources/images/28x28/project.png \
	$(srcdir)/resources/images/28x28/projectselected.png \
	$(srcdir)/resources/images/28x28/showdirdisabled.png \
	$(srcdir)/resources/images/28x28/showdir.png \
	$(srcdir)/resources/images/28x28/stopdisabled.png \
	$(srcdir)/resources/images/28x28/stop.png \
	$(srcdir)/resources/images/28x28/targetdisabled.png \
	$(srcdir)/resources/images/28x28/target.png \
	$(srcdir)/resources/images/28x28/targetselected.png \
	$(srcdir)/resources/images/28x28/workspacedisabled.png \
	$(srcdir)/resources/images/28x28/workspace.png \
	$(srcdir)/resources/images/28x28/workspaceselected.png

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

images40x40dir = $(datadir)/@PACKAGE@/images/ThreadSearch/40x40
dist_images40x40_DATA = \
	$(srcdir)/resources/images/40x40/findfdisabled.png \
	$(srcdir)/resources/images/40x40/findf.png \
	$(srcdir)/resources/images/40x40/folderdisabled.png \
	$(srcdir)/resources/images/40x40/folder.png \
	$(srcdir)/resources/images/40x40/folderselected.png \
	$(srcdir)/resources/images/40x40/openfilesdisabled.png \
	$(srcdir)/resources/images/40x40/openfiles.png \
	$(srcdir)/resources/images/40x40/openfilesselected.png \
	$(srcdir)/resources/images/40x40/optionsactive.png \
	$(srcdir)/resources/images/40x40/optionsdisabled.png \
	$(srcdir)/resources/images/40x40/options.png \
	$(srcdir)/resources/images/40x40/projectdisabled.png \
	$(srcdir)/resources/images/40x40/project.png \
	$(srcdir)/resources/images/40x40/projectselected.png \
	$(srcdir)/resources/images/40x40/showdirdisabled.png \
	$(srcdir)/resources/images/40x40/showdir.png \
	$(srcdir)/resources/images/40x40/stopdisabled.png \
	$(srcdir)/resources/images/40x40/stop.png \
	$(srcdir)/resources/images/40x40/targetdisabled.png \
	$(srcdir)/resources/images/40x40/target.png \
	$(srcdir)/resources/images/40x40/targetselected.png \
	$(srcdir)/resources/images/40x40/workspacedisabled.png \
	$(srcdir)/resources/images/40x40/workspace.png \
	$(srcdir)/resources/images/40x40/workspaceselected.png

images48x48dir = $(datadir)/@PACKAGE@/images/ThreadSearch/48x48
dist_images48x48_DATA = \
	$(srcdir)/resources/images/48x48/findfdisabled.png \
	$(srcdir)/resources/images/48x48/findf.png \
	$(srcdir)/resources/images/48x48/folderdisabled.png \
	$(srcdir)/resources/images/48x48/folder.png \
	$(srcdir)/resources/images/48x48/folderselected.png \
	$(srcdir)/resources/images/48x48/openfilesdisabled.png \
	$(srcdir)/resources/images/48x48/openfiles.png \
	$(srcdir)/resources/images/48x48/openfilesselected.png \
	$(srcdir)/resources/images/48x48/optionsactive.png \
	$(srcdir)/resources/images/48x48/optionsdisabled.png \
	$(srcdir)/resources/images/48x48/options.png \
	$(srcdir)/resources/images/48x48/projectdisabled.png \
	$(srcdir)/resources/images/48x48/project.png \
	$(srcdir)/resources/images/48x48/projectselected.png \
	$(srcdir)/resources/images/48x48/showdirdisabled.png \
	$(srcdir)/resources/images/48x48/showdir.png \
	$(srcdir)/resources/images/48x48/stopdisabled.png \
	$(srcdir)/resources/images/48x48/stop.png \
	$(srcdir)/resources/images/48x48/targetdisabled.png \
	$(srcdir)/resources/images/48x48/target.png \
	$(srcdir)/resources/images/48x48/targetselected.png \
	$(srcdir)/resources/images/48x48/workspacedisabled.png \
	$(srcdir)/resources/images/48x48/workspace.png \
	$(srcdir)/resources/images/48x48/workspaceselected.png

images56x56dir = $(datadir)/@PACKAGE@/images/ThreadSearch/56x56
dist_images56x56_DATA = \
	$(srcdir)/resources/images/56x56/findfdisabled.png \
	$(srcdir)/resources/images/56x56/findf.png \
	$(srcdir)/resources/images/56x56/folderdisabled.png \
	$(srcdir)/resources/images/56x56/folder.png \
	$(srcdir)/resources/images/56x56/folderselected.png \
	$(srcdir)/resources/images/56x56/openfilesdisabled.png \
	$(srcdir)/resources/images/56x56/openfiles.png \
	$(srcdir)/resources/images/56x56/openfilesselected.png \
	$(srcdir)/resources/images/56x56/optionsactive.png \
	$(srcdir)/resources/images/56x56/optionsdisabled.png \
	$(srcdir)/resources/images/56x56/options.png \
	$(srcdir)/resources/images/56x56/projectdisabled.png \
	$(srcdir)/resources/images/56x56/project.png \
	$(srcdir)/resources/images/56x56/projectselected.png \
	$(srcdir)/resources/images/56x56/showdirdisabled.png \
	$(srcdir)/resources/images/56x56/showdir.png \
	$(srcdir)/resources/images/56x56/stopdisabled.png \
	$(srcdir)/resources/images/56x56/stop.png \
	$(srcdir)/resources/images/56x56/targetdisabled.png \
	$(srcdir)/resources/images/56x56/target.png \
	$(srcdir)/resources/images/56x56/targetselected.png \
	$(srcdir)/resources/images/56x56/workspacedisabled.png \
	$(srcdir)/resources/images/56x56/workspace.png \
	$(srcdir)/resources/images/56x56/workspaceselected.png

images64x64dir = $(datadir)/@PACKAGE@/images/ThreadSearch/64x64
dist_images64x64_DATA = \
	$(srcdir)/resources/images/64x64/findfdisabled.png \
	$(srcdir)/resources/images/64x64/findf.png \
	$(srcdir)/resources/images/64x64/folderdisabled.png \
	$(srcdir)/resources/images/64x64/folder.png \
	$(srcdir)/resources/images/64x64/folderselected.png \
	$(srcdir)/resources/images/64x64/openfilesdisabled.png \
	$(srcdir)/resources/images/64x64/openfiles.png \
	$(srcdir)/resources/images/64x64/openfilesselected.png \
	$(srcdir)/resources/images/64x64/optionsactive.png \
	$(srcdir)/resources/images/64x64/optionsdisabled.png \
	$(srcdir)/resources/images/64x64/options.png \
	$(srcdir)/resources/images/64x64/projectdisabled.png \
	$(srcdir)/resources/images/64x64/project.png \
	$(srcdir)/resources/images/64x64/projectselected.png \
	$(srcdir)/resources/images/64x64/showdirdisabled.png \
	$(srcdir)/resources/images/64x64/showdir.png \
	$(srcdir)/resources/images/64x64/stopdisabled.png \
	$(srcdir)/resources/images/64x64/stop.png \
	$(srcdir)/resources/images/64x64/targetdisabled.png \
	$(srcdir)/resources/images/64x64/target.png \
	$(srcdir)/resources/images/64x64/targetselected.png \
	$(srcdir)/resources/images/64x64/workspacedisabled.png \
	$(srcdir)/resources/images/64x64/workspace.png \
	$(srcdir)/resources/images/64x64/workspaceselected.png

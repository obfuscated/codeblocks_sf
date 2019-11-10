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

imagesForZip = \
	resources/images/16x16/findfdisabled.png \
	resources/images/16x16/findf.png \
	resources/images/16x16/folderdisabled.png \
	resources/images/16x16/folder.png \
	resources/images/16x16/folderselected.png \
	resources/images/16x16/openfilesdisabled.png \
	resources/images/16x16/openfiles.png \
	resources/images/16x16/openfilesselected.png \
	resources/images/16x16/optionsactive.png \
	resources/images/16x16/optionsdisabled.png \
	resources/images/16x16/options.png \
	resources/images/16x16/projectdisabled.png \
	resources/images/16x16/project.png \
	resources/images/16x16/projectselected.png \
	resources/images/16x16/showdirdisabled.png \
	resources/images/16x16/showdir.png \
	resources/images/16x16/stopdisabled.png \
	resources/images/16x16/stop.png \
	resources/images/16x16/targetdisabled.png \
	resources/images/16x16/target.png \
	resources/images/16x16/targetselected.png \
	resources/images/16x16/workspacedisabled.png \
	resources/images/16x16/workspace.png \
	resources/images/16x16/workspaceselected.png \
	\
	resources/images/20x20/findfdisabled.png \
	resources/images/20x20/findf.png \
	resources/images/20x20/folderdisabled.png \
	resources/images/20x20/folder.png \
	resources/images/20x20/folderselected.png \
	resources/images/20x20/openfilesdisabled.png \
	resources/images/20x20/openfiles.png \
	resources/images/20x20/openfilesselected.png \
	resources/images/20x20/optionsactive.png \
	resources/images/20x20/optionsdisabled.png \
	resources/images/20x20/options.png \
	resources/images/20x20/projectdisabled.png \
	resources/images/20x20/project.png \
	resources/images/20x20/projectselected.png \
	resources/images/20x20/showdirdisabled.png \
	resources/images/20x20/showdir.png \
	resources/images/20x20/stopdisabled.png \
	resources/images/20x20/stop.png \
	resources/images/20x20/targetdisabled.png \
	resources/images/20x20/target.png \
	resources/images/20x20/targetselected.png \
	resources/images/20x20/workspacedisabled.png \
	resources/images/20x20/workspace.png \
	resources/images/20x20/workspaceselected.png \
	\
	resources/images/24x24/findfdisabled.png \
	resources/images/24x24/findf.png \
	resources/images/24x24/folderdisabled.png \
	resources/images/24x24/folder.png \
	resources/images/24x24/folderselected.png \
	resources/images/24x24/openfilesdisabled.png \
	resources/images/24x24/openfiles.png \
	resources/images/24x24/openfilesselected.png \
	resources/images/24x24/optionsactive.png \
	resources/images/24x24/optionsdisabled.png \
	resources/images/24x24/options.png \
	resources/images/24x24/projectdisabled.png \
	resources/images/24x24/project.png \
	resources/images/24x24/projectselected.png \
	resources/images/24x24/showdirdisabled.png \
	resources/images/24x24/showdir.png \
	resources/images/24x24/stopdisabled.png \
	resources/images/24x24/stop.png \
	resources/images/24x24/targetdisabled.png \
	resources/images/24x24/target.png \
	resources/images/24x24/targetselected.png \
	resources/images/24x24/workspacedisabled.png \
	resources/images/24x24/workspace.png \
	resources/images/24x24/workspaceselected.png \
	\
	resources/images/28x28/findfdisabled.png \
	resources/images/28x28/findf.png \
	resources/images/28x28/folderdisabled.png \
	resources/images/28x28/folder.png \
	resources/images/28x28/folderselected.png \
	resources/images/28x28/openfilesdisabled.png \
	resources/images/28x28/openfiles.png \
	resources/images/28x28/openfilesselected.png \
	resources/images/28x28/optionsactive.png \
	resources/images/28x28/optionsdisabled.png \
	resources/images/28x28/options.png \
	resources/images/28x28/projectdisabled.png \
	resources/images/28x28/project.png \
	resources/images/28x28/projectselected.png \
	resources/images/28x28/showdirdisabled.png \
	resources/images/28x28/showdir.png \
	resources/images/28x28/stopdisabled.png \
	resources/images/28x28/stop.png \
	resources/images/28x28/targetdisabled.png \
	resources/images/28x28/target.png \
	resources/images/28x28/targetselected.png \
	resources/images/28x28/workspacedisabled.png \
	resources/images/28x28/workspace.png \
	resources/images/28x28/workspaceselected.png \
	\
	resources/images/32x32/findfdisabled.png \
	resources/images/32x32/findf.png \
	resources/images/32x32/folderdisabled.png \
	resources/images/32x32/folder.png \
	resources/images/32x32/folderselected.png \
	resources/images/32x32/openfilesdisabled.png \
	resources/images/32x32/openfiles.png \
	resources/images/32x32/openfilesselected.png \
	resources/images/32x32/optionsactive.png \
	resources/images/32x32/optionsdisabled.png \
	resources/images/32x32/options.png \
	resources/images/32x32/projectdisabled.png \
	resources/images/32x32/project.png \
	resources/images/32x32/projectselected.png \
	resources/images/32x32/showdirdisabled.png \
	resources/images/32x32/showdir.png \
	resources/images/32x32/stopdisabled.png \
	resources/images/32x32/stop.png \
	resources/images/32x32/targetdisabled.png \
	resources/images/32x32/target.png \
	resources/images/32x32/targetselected.png \
	resources/images/32x32/workspacedisabled.png \
	resources/images/32x32/workspace.png \
	resources/images/32x32/workspaceselected.png \
	\
	resources/images/40x40/findfdisabled.png \
	resources/images/40x40/findf.png \
	resources/images/40x40/folderdisabled.png \
	resources/images/40x40/folder.png \
	resources/images/40x40/folderselected.png \
	resources/images/40x40/openfilesdisabled.png \
	resources/images/40x40/openfiles.png \
	resources/images/40x40/openfilesselected.png \
	resources/images/40x40/optionsactive.png \
	resources/images/40x40/optionsdisabled.png \
	resources/images/40x40/options.png \
	resources/images/40x40/projectdisabled.png \
	resources/images/40x40/project.png \
	resources/images/40x40/projectselected.png \
	resources/images/40x40/showdirdisabled.png \
	resources/images/40x40/showdir.png \
	resources/images/40x40/stopdisabled.png \
	resources/images/40x40/stop.png \
	resources/images/40x40/targetdisabled.png \
	resources/images/40x40/target.png \
	resources/images/40x40/targetselected.png \
	resources/images/40x40/workspacedisabled.png \
	resources/images/40x40/workspace.png \
	resources/images/40x40/workspaceselected.png \
	\
	resources/images/48x48/findfdisabled.png \
	resources/images/48x48/findf.png \
	resources/images/48x48/folderdisabled.png \
	resources/images/48x48/folder.png \
	resources/images/48x48/folderselected.png \
	resources/images/48x48/openfilesdisabled.png \
	resources/images/48x48/openfiles.png \
	resources/images/48x48/openfilesselected.png \
	resources/images/48x48/optionsactive.png \
	resources/images/48x48/optionsdisabled.png \
	resources/images/48x48/options.png \
	resources/images/48x48/projectdisabled.png \
	resources/images/48x48/project.png \
	resources/images/48x48/projectselected.png \
	resources/images/48x48/showdirdisabled.png \
	resources/images/48x48/showdir.png \
	resources/images/48x48/stopdisabled.png \
	resources/images/48x48/stop.png \
	resources/images/48x48/targetdisabled.png \
	resources/images/48x48/target.png \
	resources/images/48x48/targetselected.png \
	resources/images/48x48/workspacedisabled.png \
	resources/images/48x48/workspace.png \
	resources/images/48x48/workspaceselected.png \
	\
	resources/images/56x56/findfdisabled.png \
	resources/images/56x56/findf.png \
	resources/images/56x56/folderdisabled.png \
	resources/images/56x56/folder.png \
	resources/images/56x56/folderselected.png \
	resources/images/56x56/openfilesdisabled.png \
	resources/images/56x56/openfiles.png \
	resources/images/56x56/openfilesselected.png \
	resources/images/56x56/optionsactive.png \
	resources/images/56x56/optionsdisabled.png \
	resources/images/56x56/options.png \
	resources/images/56x56/projectdisabled.png \
	resources/images/56x56/project.png \
	resources/images/56x56/projectselected.png \
	resources/images/56x56/showdirdisabled.png \
	resources/images/56x56/showdir.png \
	resources/images/56x56/stopdisabled.png \
	resources/images/56x56/stop.png \
	resources/images/56x56/targetdisabled.png \
	resources/images/56x56/target.png \
	resources/images/56x56/targetselected.png \
	resources/images/56x56/workspacedisabled.png \
	resources/images/56x56/workspace.png \
	resources/images/56x56/workspaceselected.png \
	\
	resources/images/64x64/findfdisabled.png \
	resources/images/64x64/findf.png \
	resources/images/64x64/folderdisabled.png \
	resources/images/64x64/folder.png \
	resources/images/64x64/folderselected.png \
	resources/images/64x64/openfilesdisabled.png \
	resources/images/64x64/openfiles.png \
	resources/images/64x64/openfilesselected.png \
	resources/images/64x64/optionsactive.png \
	resources/images/64x64/optionsdisabled.png \
	resources/images/64x64/options.png \
	resources/images/64x64/projectdisabled.png \
	resources/images/64x64/project.png \
	resources/images/64x64/projectselected.png \
	resources/images/64x64/showdirdisabled.png \
	resources/images/64x64/showdir.png \
	resources/images/64x64/stopdisabled.png \
	resources/images/64x64/stop.png \
	resources/images/64x64/targetdisabled.png \
	resources/images/64x64/target.png \
	resources/images/64x64/targetselected.png \
	resources/images/64x64/workspacedisabled.png \
	resources/images/64x64/workspace.png \
	resources/images/64x64/workspaceselected.png

EXTRA_DIST = 	$(srcdir)/*.cbp \
		$(srcdir)/ThreadSearch.wxg \
		$(srcdir)/update* \
		$(srcdir)/manifest.xml \
		$(srcdir)/resources/ThreadSearch.xcf \
		$(imagesForZip)

pkgdata_DATA = ThreadSearch.zip
CLEANFILES = $(pkgdata_DATA)

ThreadSearch.zip: manifest.xml $(imagesForZip)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/ThreadSearch.zip manifest.xml > /dev/null
	PWD=`pwd` cd $(srcdir)/resources && \
		zip $(PWD)/ThreadSearch.zip $(imagesForZip:resources/images/%=images/%) \
			> /dev/null

confimagesdir = $(datadir)/@PACKAGE@/images/settings
dist_confimages_DATA = $(srcdir)/resources/*.png

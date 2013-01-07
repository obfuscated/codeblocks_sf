INCLUDES = $(WX_CXXFLAGS) \
		$(WX_GTK2_CFLAGS) \
		$(WX_X11_CFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include \
		-I$(top_srcdir)/src/plugins/contrib/codesnippets \
		-I$(top_srcdir)/src/plugins/contrib/codesnippets/Search \
 		-I$(top_srcdir)/src/plugins/contrib/codesnippets/editor \
		-I$(top_srcdir)/src/include/mozilla_chardet \
		-I$(top_srcdir)/src/include/mozilla_chardet/mfbt \
		-I$(top_srcdir)/src/include/mozilla_chardet/nsprpub/pr/include \
		-I$(top_srcdir)/src/include/mozilla_chardet/xpcom \
		-I$(top_srcdir)/src/include/mozilla_chardet/xpcom/base \
		-I$(top_srcdir)/src/include/mozilla_chardet/xpcom/glue

pluginlibdir = $(pkglibdir)/plugins

##pluginlib_LTLIBRARIES = codesnippets
bin_PROGRAMS = codesnippets

##codesnippets_LIBADD = $(PLUGIN_WX_LIBS) ../../../../sdk/libcodeblocks.la
##codesnippets_LDADD = $(PLUGIN_WX_LIBS) ../../../../sdk/libcodeblocks.la
codesnippets_LDADD =  -L../../../../base/tinyxml -ltinyxml \
            -L../../../../sdk ../../../../sdk/libcodeblocks.la \
			$(PLUGIN_WX_LIBS) $(WX_LIBS) $(WX_GTK2_LIBS) $(WX_X11_LIBS)

codesnippets_SOURCES =	$(srcdir)/../version.cpp \
		$(srcdir)/../Search/DirectoryParamsPanel.cpp \
		$(srcdir)/../Search/InsertIndexManager.cpp \
		$(srcdir)/../Search/MainPanel.cpp \
		$(srcdir)/../Search/SearchInPanel.cpp \
		$(srcdir)/../Search/TextFileSearcher.cpp \
		$(srcdir)/../Search/TextFileSearcherRegEx.cpp \
		$(srcdir)/../Search/TextFileSearcherText.cpp \
		$(srcdir)/../Search/ThreadSearch.cpp \
		$(srcdir)/../Search/ThreadSearchConfPanel.cpp \
		$(srcdir)/../Search/ThreadSearchEvent.cpp \
		$(srcdir)/../Search/ThreadSearchFindData.cpp \
		$(srcdir)/../Search/ThreadSearchFrame.cpp \
		$(srcdir)/../Search/ThreadSearchLoggerBase.cpp \
		$(srcdir)/../Search/ThreadSearchLoggerList.cpp \
		$(srcdir)/../Search/ThreadSearchLoggerTree.cpp \
		$(srcdir)/../Search/ThreadSearchThread.cpp \
		$(srcdir)/../Search/ThreadSearchTrace.cpp \
		$(srcdir)/../Search/ThreadSearchView.cpp \
		$(srcdir)/../Search/ThreadSearchViewManagerBase.cpp \
		$(srcdir)/../Search/ThreadSearchViewManagerLayout.cpp \
		$(srcdir)/../Search/ThreadSearchViewManagerMessagesNotebook.cpp \
		$(srcdir)/../editor/dragscroll.cpp \
		$(srcdir)/../editor/dragscrollcfg.cpp \
		$(srcdir)/../editor/dragscrollevent.cpp \
		$(srcdir)/../editor/editproperties.cpp \
		$(srcdir)/../editor/prefs.cpp \
		$(srcdir)/../editor/scbeditor.cpp \
		$(srcdir)/../editor/seditorbase.cpp \
		$(srcdir)/../editor/seditorcolourset.cpp \
		$(srcdir)/../editor/seditormanager.cpp \
		$(srcdir)/../editor/sprintdlg.cpp \
		$(srcdir)/../codesnippetsapp.cpp \
		$(srcdir)/../codesnippetsevent.cpp \
		$(srcdir)/../codesnippetstreectrl.cpp \
		$(srcdir)/../codesnippetswindow.cpp \
		$(srcdir)/../csutils.cpp \
		$(srcdir)/../editsnippetframe.cpp \
		$(srcdir)/../GenericMessageBox.cpp \
		$(srcdir)/../settingsdlg.cpp \
		$(srcdir)/../settingsdlgform.cpp \
		$(srcdir)/../snippetitemdata.cpp \
		$(srcdir)/../snippetproperty.cpp \
		$(srcdir)/../snippetpropertyform.cpp \
		$(srcdir)/../snippetsconfig.cpp \
		$(srcdir)/../snippetsimages.cpp

noinst_HEADERS =  	$(srcdir)/../version.h \
		$(srcdir)/../Search/DirectoryParamsPanel.h \
		$(srcdir)/../Search/InsertIndexManager.h \
		$(srcdir)/../Search/MainPanel.h \
		$(srcdir)/../Search/SearchInPanel.h \
		$(srcdir)/../Search/TextFileSearcher.h \
		$(srcdir)/../Search/TextFileSearcherRegEx.h \
		$(srcdir)/../Search/TextFileSearcherText.h \
		$(srcdir)/../Search/ThreadSearch.h \
		$(srcdir)/../Search/ThreadSearchConfPanel.h \
		$(srcdir)/../Search/ThreadSearchControlIds.h \
		$(srcdir)/../Search/ThreadSearchEvent.h \
		$(srcdir)/../Search/ThreadSearchFindData.h \
		$(srcdir)/../Search/ThreadSearchFrame.h \
		$(srcdir)/../Search/ThreadSearchLoggerBase.h \
		$(srcdir)/../Search/ThreadSearchLoggerList.h \
		$(srcdir)/../Search/ThreadSearchLoggerTree.h \
		$(srcdir)/../Search/ThreadSearchThread.h \
		$(srcdir)/../Search/ThreadSearchTrace.h \
		$(srcdir)/../Search/ThreadSearchView.h \
		$(srcdir)/../Search/ThreadSearchViewManagerBase.h \
		$(srcdir)/../Search/ThreadSearchViewManagerLayout.h \
		$(srcdir)/../Search/ThreadSearchViewManagerMessagesNotebook.h \
		$(srcdir)/../Search/logging.h \
		$(srcdir)/../editor/defsext.h \
		$(srcdir)/../editor/dragscroll.h \
		$(srcdir)/../editor/dragscrollcfg.h \
		$(srcdir)/../editor/dragscrollevent.h \
		$(srcdir)/../editor/editproperties.h \
		$(srcdir)/../editor/prefs.h \
		$(srcdir)/../editor/scbeditor.h \
		$(srcdir)/../editor/seditorbase.h \
		$(srcdir)/../editor/seditorcolourset.h \
		$(srcdir)/../editor/seditormanager.h \
		$(srcdir)/../editor/sprintdlg.h \
		$(srcdir)/../codesnippetsapp.h \
		$(srcdir)/../codesnippetsevent.h \
		$(srcdir)/../codesnippetstreectrl.h \
		$(srcdir)/../codesnippetswindow.h \
		$(srcdir)/../csutils.h \
		$(srcdir)/../editsnippetframe.h \
		$(srcdir)/../menuidentifiers.h \
		$(srcdir)/../GenericMessageBox.h \
		$(srcdir)/../settingsdlg.h \
		$(srcdir)/../settingsdlgform.h \
		$(srcdir)/../snippetitemdata.h \
		$(srcdir)/../snippetproperty.h \
		$(srcdir)/../snippetpropertyform.h \
		$(srcdir)/../snippetsconfig.h \
		$(srcdir)/../snippetsimages.h

pkgdata_DATA = codesnippets.zip
CLEANFILES = $(pkgdata_DATA)

imagedir = $(pkgdatadir)/images/codesnippets

dist_image_DATA =  allsnippets.png \
		category.png \
		snippet.png \
		findf.png \
		findfdisabled.png \
		options.png \
		optionsdisabled.png \
		stop.png \
		stopdisabled.png

EXTRA_DIST = manifest.xml

codesnippets.zip: $(EXTRA_DIST)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/codesnippets.zip manifest.xml > /dev/null


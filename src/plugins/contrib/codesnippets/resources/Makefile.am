AM_CPPFLAGS = $(WX_CXXFLAGS) \
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

##codesnippets_LIBADD = $(WX_LIBS) ../../../../sdk/libcodeblocks.la
##codesnippets_LDADD = $(WX_LIBS) ../../../../sdk/libcodeblocks.la
codesnippets_LDADD =  -L../../../../sdk ../../../../sdk/libcodeblocks.la \
            ../../../../base/tinyxml/libtinyxml.la \
			$(WX_LIBS) $(WX_GTK2_LIBS) $(WX_X11_LIBS)

codesnippets_SOURCES =	../version.cpp \
		../Search/DirectoryParamsPanel.cpp \
		../Search/InsertIndexManager.cpp \
		../Search/MainPanel.cpp \
		../Search/SearchInPanel.cpp \
		../Search/TextFileSearcher.cpp \
		../Search/TextFileSearcherRegEx.cpp \
		../Search/TextFileSearcherText.cpp \
		../Search/ThreadSearch.cpp \
		../Search/ThreadSearchConfPanel.cpp \
		../Search/ThreadSearchEvent.cpp \
		../Search/ThreadSearchFindData.cpp \
		../Search/ThreadSearchFrame.cpp \
		../Search/ThreadSearchLoggerBase.cpp \
		../Search/ThreadSearchLoggerList.cpp \
		../Search/ThreadSearchLoggerTree.cpp \
		../Search/ThreadSearchThread.cpp \
		../Search/ThreadSearchTrace.cpp \
		../Search/ThreadSearchView.cpp \
		../Search/ThreadSearchViewManagerBase.cpp \
		../Search/ThreadSearchViewManagerLayout.cpp \
		../Search/ThreadSearchViewManagerMessagesNotebook.cpp \
		../editor/dragscroll.cpp \
		../editor/dragscrollcfg.cpp \
		../editor/dragscrollevent.cpp \
		../editor/editproperties.cpp \
		../editor/prefs.cpp \
		../editor/scbeditor.cpp \
		../editor/seditorbase.cpp \
		../editor/seditorcolourset.cpp \
		../editor/seditormanager.cpp \
		../editor/sprintdlg.cpp \
		../codesnippetsapp.cpp \
		../codesnippetsevent.cpp \
		../codesnippetstreectrl.cpp \
		../codesnippetswindow.cpp \
		../csutils.cpp \
		../editsnippetframe.cpp \
		../GenericMessageBox.cpp \
		../settingsdlg.cpp \
		../settingsdlgform.cpp \
		../snippetitemdata.cpp \
		../snippetproperty.cpp \
		../snippetpropertyform.cpp \
		../snippetsconfig.cpp \
		../snippetsimages.cpp

noinst_HEADERS =  	../version.h \
		../Search/DirectoryParamsPanel.h \
		../Search/InsertIndexManager.h \
		../Search/MainPanel.h \
		../Search/SearchInPanel.h \
		../Search/TextFileSearcher.h \
		../Search/TextFileSearcherRegEx.h \
		../Search/TextFileSearcherText.h \
		../Search/ThreadSearch.h \
		../Search/ThreadSearchConfPanel.h \
		../Search/ThreadSearchControlIds.h \
		../Search/ThreadSearchEvent.h \
		../Search/ThreadSearchFindData.h \
		../Search/ThreadSearchFrame.h \
		../Search/ThreadSearchLoggerBase.h \
		../Search/ThreadSearchLoggerList.h \
		../Search/ThreadSearchLoggerTree.h \
		../Search/ThreadSearchThread.h \
		../Search/ThreadSearchTrace.h \
		../Search/ThreadSearchView.h \
		../Search/ThreadSearchViewManagerBase.h \
		../Search/ThreadSearchViewManagerLayout.h \
		../Search/ThreadSearchViewManagerMessagesNotebook.h \
		../Search/logging.h \
		../editor/defsext.h \
		../editor/dragscroll.h \
		../editor/dragscrollcfg.h \
		../editor/dragscrollevent.h \
		../editor/editproperties.h \
		../editor/prefs.h \
		../editor/scbeditor.h \
		../editor/seditorbase.h \
		../editor/seditorcolourset.h \
		../editor/seditormanager.h \
		../editor/sprintdlg.h \
		../codesnippetsapp.h \
		../codesnippetsevent.h \
		../codesnippetstreectrl.h \
		../codesnippetswindow.h \
		../csutils.h \
		../editsnippetframe.h \
		../menuidentifiers.h \
		../GenericMessageBox.h \
		../settingsdlg.h \
		../settingsdlgform.h \
		../snippetitemdata.h \
		../snippetproperty.h \
		../snippetpropertyform.h \
		../snippetsconfig.h \
		../snippetsimages.h

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


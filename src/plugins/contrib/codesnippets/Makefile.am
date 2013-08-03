SUBDIRS = resources

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

pluginlib_LTLIBRARIES = libcodesnippets.la

libcodesnippets_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libcodesnippets_la_LIBADD = ../../../sdk/libcodeblocks.la $(WX_LIBS) $(WX_GTK2_LIBS) $(WX_X11_LIBS)

libcodesnippets_la_SOURCES = codesnippets.cpp \
		Search/DirectoryParamsPanel.cpp \
		Search/InsertIndexManager.cpp \
		Search/MainPanel.cpp \
		Search/SearchInPanel.cpp \
		Search/TextFileSearcher.cpp \
		Search/TextFileSearcherRegEx.cpp \
		Search/TextFileSearcherText.cpp \
		Search/ThreadSearch.cpp \
		Search/ThreadSearchConfPanel.cpp \
		Search/ThreadSearchEvent.cpp \
		Search/ThreadSearchFindData.cpp \
		Search/ThreadSearchFrame.cpp \
		Search/ThreadSearchLoggerBase.cpp \
		Search/ThreadSearchLoggerList.cpp \
		Search/ThreadSearchLoggerTree.cpp \
		Search/ThreadSearchThread.cpp \
		Search/ThreadSearchTrace.cpp \
		Search/ThreadSearchView.cpp \
		Search/ThreadSearchViewManagerBase.cpp \
		Search/ThreadSearchViewManagerLayout.cpp \
		Search/ThreadSearchViewManagerMessagesNotebook.cpp \
		editor/dragscroll.cpp \
		editor/dragscrollcfg.cpp \
		editor/dragscrollevent.cpp \
		editor/editproperties.cpp \
		editor/prefs.cpp \
		editor/scbeditor.cpp \
		editor/seditorbase.cpp \
		editor/seditorcolourset.cpp \
		editor/seditormanager.cpp \
		editor/sprintdlg.cpp \
		codesnippetsevent.cpp \
		codesnippetstreectrl.cpp \
		codesnippetswindow.cpp \
		csutils.cpp \
		editsnippetframe.cpp \
		finddialogs.cpp \
		GenericMessageBox.cpp \
		settingsdlg.cpp \
		settingsdlgform.cpp \
		snippetitemdata.cpp \
		snippetproperty.cpp \
		snippetpropertyform.cpp \
		snippetsconfig.cpp \
		snippetsimages.cpp \
		version.cpp


noinst_HEADERS =  	codesnippets.h \
		Search/DirectoryParamsPanel.h \
		Search/InsertIndexManager.h \
		Search/MainPanel.h \
		Search/SearchInPanel.h \
		Search/TextFileSearcher.h \
		Search/TextFileSearcherRegEx.h \
		Search/TextFileSearcherText.h \
		Search/ThreadSearch.h \
		Search/ThreadSearchConfPanel.h \
		Search/ThreadSearchControlIds.h \
		Search/ThreadSearchEvent.h \
		Search/ThreadSearchFindData.h \
		Search/ThreadSearchFrame.h \
		Search/ThreadSearchLoggerBase.h \
		Search/ThreadSearchLoggerList.h \
		Search/ThreadSearchLoggerTree.h \
		Search/ThreadSearchThread.h \
		Search/ThreadSearchTrace.h \
		Search/ThreadSearchView.h \
		Search/ThreadSearchViewManagerBase.h \
		Search/ThreadSearchViewManagerLayout.h \
		Search/ThreadSearchViewManagerMessagesNotebook.h \
		Search/logging.h \
		editor/defsext.h \
		editor/dragscroll.h \
		editor/dragscrollcfg.h \
		editor/dragscrollevent.h \
		editor/editproperties.h \
		editor/prefs.h \
		editor/scbeditor.h \
		editor/seditorbase.h \
		editor/seditorcolourset.h \
		editor/seditormanager.h \
		editor/sprintdlg.h \
		codesnippetsevent.h \
		codesnippetstreectrl.h \
		codesnippetswindow.h \
		csutils.h \
		editsnippetframe.h \
		finddialogs.h \
		menuidentifiers.h \
		GenericMessageBox.h \
		settingsdlg.h \
		settingsdlgform.h \
		snippetitemdata.h \
		snippetproperty.h \
		snippetpropertyform.h \
		snippetsconfig.h \
		snippetsimages.h \
		FileImport.h \
		version.h \
		wx_pch.h

man_MANS = codesnippets.1

EXTRA_DIST = 	$(srcdir)/*.cbp \
		generic.rc \
		generic.Manifest \
		$(srcdir)/update* \
		readme.html \
		license.txt \
		manifest.xml \
		$(man_MANS)


INCLUDES = $(WX_CXXFLAGS) \
        $(WX_GTK2_CFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxscintilla/include \
		-I$(top_srcdir)/src/plugins/contrib/codesnippets \
		-I$(top_srcdir)/src/plugins/contrib/codesnippets/Search \
		-I$(top_srcdir)/src/plugins/contrib/codesnippets/editor \
        -I$(top_srcdir)/src/include/wxFlatNotebook/include


libdir = $(pkgdatadir)/plugins

##lib_LTLIBRARIES = codesnippets
bin_PROGRAMS = codesnippets

codesnippets_LDFLAGS = $(WX_GTK2_LIBS)

##codesnippets_LIBADD = $(PLUGIN_WX_LIBS) ../../../../sdk/libcodeblocks.la
##codesnippets_LDADD = $(PLUGIN_WX_LIBS) ../../../../sdk/libcodeblocks.la
codesnippets_LDADD =  -L../../../../base/tinyxml -ltinyxml \
            -L../../../../sdk ../../../../sdk/libcodeblocks.la \
			$(PLUGIN_WX_LIBS) $(WX_LIBS)

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
		../messageBoxForm.cpp \
		../messagebox.cpp \
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
		../messageBoxForm.h \
		../messagebox.h \
		../settingsdlg.h \
		../settingsdlgform.h \
		../snippetitemdata.h \
		../snippetproperty.h \
		../snippetpropertyform.h \
		../snippetsconfig.h \
		../snippetsimages.h

pkgdatadir = $(datadir)/@PACKAGE@/images/codesnippets

dist_pkgdata_DATA = 	allsnippets.png \
			category.png \
			snippet.png \
			findf.png \
			findfdisabled.png \
			options.png \
            optionsdisabled.png \
            stop.png \
            stopdisabled.png

EXTRA_DIST = manifest.xml

codesnippets.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/codesnippets.zip manifest.xml > /dev/null

install-data-local: codesnippets.zip
	$(mkinstalldirs) $(DESTDIR)$(datadir)/@PACKAGE@/; \
	if test -f $(srcdir)/codesnippets.zip; then p=$(srcdir)/codesnippets.zip; else p=codesnippets.zip; fi; \
	$(INSTALL_DATA) $$p $(DESTDIR)$(datadir)/@PACKAGE@/codesnippets.zip


SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxscintilla/include \
		-I$(top_srcdir)/src/include/wxFlatNotebook/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libThreadSearch.la

libThreadSearch_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libThreadSearch_la_LIBADD = $(PLUGIN_WX_LIBS) ../../../sdk/libcodeblocks.la

libThreadSearch_la_SOURCES = DirectoryParamsPanel.cpp \
			SearchInPanel.cpp \
			TextFileSearcher.cpp \
			TextFileSearcherRegEx.cpp \
			TextFileSearcherText.cpp \
			ThreadSearch.cpp \
			ThreadSearchConfPanel.cpp \
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
			logging.h \
			SearchInPanel.h \
			TextFileSearcher.h \
			TextFileSearcherRegEx.h \
			TextFileSearcherText.h \
			ThreadSearch.h \
			ThreadSearchConfPanel.h \
			ThreadSearchControlIds.h \
			ThreadSearchEvent.h \
			ThreadSearchFindData.h \
			ThreadSearchLoggerBase.h \
			ThreadSearchLoggerList.h \
			ThreadSearchLoggerTree.h \
			ThreadSearchThread.h \
			ThreadSearchTraceh \
			ThreadSearchView.h \
			ThreadSearchViewManagerBase.h \
			ThreadSearchViewManagerLayout.h \
			ThreadSearchViewManagerMessagesNotebook.h

EXTRA_DIST = 	ThreadSearch.cbp \
		ThreadSearch-unix.cbp \
		ThreadSearch-unix.wxg \
		update.bat \
		update \
		manifest.xml

pkgdata_DATA = ThreadSearch.zip
CLEANFILES = $(pkgdata_DATA)

ThreadSearch.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/ThreadSearch.zip manifest.xml > /dev/null


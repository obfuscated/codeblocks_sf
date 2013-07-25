SUBDIRS = resources

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

libThreadSearch_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

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

EXTRA_DIST = 	$(srcdir)/ThreadSearch.cbp \
		$(srcdir)/ThreadSearch-unix.cbp \
		$(srcdir)/ThreadSearch.wxg \
		$(srcdir)/update.bat \
		$(srcdir)/update \
		$(srcdir)/manifest.xml

pkgdata_DATA = ThreadSearch.zip
CLEANFILES = $(pkgdata_DATA)

ThreadSearch.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/ThreadSearch.zip manifest.xml > /dev/null


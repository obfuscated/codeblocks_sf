INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libCscope.la

libCscope_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libCscope_la_LIBADD = ../../../sdk/libcodeblocks.la $(WX_LIBS)

libCscope_la_SOURCES = CscopeConfig.cpp \
			CscopeEntryData.cpp \
			CscopeParserThread.cpp \
			CscopePlugin.cpp \
			CscopeProcess.cpp \
			CscopeStatusMessage.cpp \
			CscopeTab.cpp \
			CscopeView.cpp

noinst_HEADERS = CscopeConfig.h \
		CscopeEntryData.h \
		CscopeParserThread.h \
		CscopePlugin.h \
		CscopeProcess.h \
		CscopeStatusMessage.h \
		CscopeTab.h \
		CscopeView.h

EXTRA_DIST = 	$(srcdir)/*.cbp \
		manifest.xml

pkgdata_DATA = Cscope.zip

CLEANFILES = $(pkgdata_DATA)

Cscope.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/Cscope.zip manifest.xml > /dev/null

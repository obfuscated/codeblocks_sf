SUBDIRS = lib_finder

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include \
		-I$(srcdir)/../wxContribItems/wxFlatNotebook/include \
		-I$(top_srcdir)/src/include/scripting/include \
		-I$(top_srcdir)/src/include/scripting/sqplus \
		-I$(top_srcdir)/src/include/scripting/squirrel \
		-I$(top_srcdir)/src/include/scripting/bindings \
		-I$(top_srcdir)/src/include/mozilla_chardet \
		-I$(top_srcdir)/src/include/mozilla_chardet/mfbt \
		-I$(top_srcdir)/src/include/mozilla_chardet/nsprpub/pr/include \
		-I$(top_srcdir)/src/include/mozilla_chardet/xpcom \
		-I$(top_srcdir)/src/include/mozilla_chardet/xpcom/base \
		-I$(top_srcdir)/src/include/mozilla_chardet/xpcom/glue

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = liblib_finder.la

liblib_finder_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

liblib_finder_la_LIBADD = ../../../sdk/libcodeblocks.la \
		../wxContribItems/wxFlatNotebook/libwxflatnotebook.la \
		$(WX_LIBS)

liblib_finder_la_SOURCES = \
		librariesdlg.cpp \
		dirlistdlg.cpp \
		projectmissinglibs.cpp \
		headersdetectordlg.cpp \
		webresourcesmanager.cpp \
		projectconfiguration.cpp \
		resultmap.cpp \
		libraryresult.cpp \
		projectconfigurationpanel.cpp \
		pkgconfigmanager.cpp \
		librarydetectionmanager.cpp \
		libselectdlg.cpp \
		processingdlg.cpp \
		defsdownloaddlg.cpp \
		lib_finder.cpp

noinst_HEADERS = \
		librariesdlg.h \
		projectconfiguration.h \
		dirlistdlg.h \
		lib_finder.h \
		projectmissinglibs.h \
		webresourcesmanager.h \
		defsdownloaddlg.h \
		libraryresult.h \
		libselectdlg.h \
		headersdetectordlg.h \
		librarydetectionmanager.h \
		pkgconfigmanager.h \
		resultmap.h \
		processingdlg.h \
		librarydetectionconfig.h \
		projectconfigurationpanel.h

EXTRA_DIST = \
	$(srcdir)/*.cbp \
	update \
	update.bat \
	readme.txt \
	manifest.xml \
	$(srcdir)/wxsmith/*.wxs

pkgdata_DATA = lib_finder.zip
CLEANFILES = $(pkgdata_DATA)

lib_finder.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/lib_finder.zip manifest.xml > /dev/null

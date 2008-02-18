INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libBrowseTracker.la

libBrowseTracker_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libBrowseTracker_la_LIBADD = $(PLUGIN_WX_LIBS) ../../../sdk/libcodeblocks.la

libBrowseTracker_la_SOURCES = BrowseTracker.cpp \
			BrowseSelector.cpp \
			Version.cpp \
			BrowseMarks.cpp \
			BrowseTrackerCfg.cpp \
			BrowseTrackerLayout.cpp \
			ConfigDlg.cpp \
			ProjectData.cpp

noinst_HEADERS =  	BrowseTracker.h \
			BrowseSelector.h \
			Version.h \
			wx_pch.h \
			BrowseMarks.h \
			BrowseXpms.h \
			BrowseTrackerDefs.h \
			BrowseTrackerCfg.h \
			BrowseTrackerLayout.h \
			ConfigDlg.h \
			ProjectData.h

EXTRA_DIST = 	BrowseTracker.cbp \
		BrowseTracker-unix.cbp

pkgdata_DATA = BrowseTracker.zip
CLEANFILES = $(pkgdata_DATA)

BrowseTracker.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/BrowseTracker.zip manifest.xml > /dev/null
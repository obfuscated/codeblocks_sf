AM_CPPFLAGS = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libBrowseTracker.la

libBrowseTracker_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libBrowseTracker_la_LIBADD = ../../../sdk/libcodeblocks.la $(WX_LIBS)

libBrowseTracker_la_SOURCES = BrowseTracker.cpp \
			BrowseSelector.cpp \
			Version.cpp \
			BrowseMarks.cpp \
			BrowseTrackerConfPanel.cpp \
			BrowseTrackerLayout.cpp \
			ConfigPanel.cpp \
			ProjectData.cpp \
			JumpTracker.cpp \
			JumpData.cpp

noinst_HEADERS =  	BrowseTracker.h \
			BrowseSelector.h \
			Version.h \
			wx_pch.h \
			BrowseMarks.h \
			BrowseXpms.h \
			BrowseTrackerDefs.h \
			BrowseTrackerConfPanel.h \
			BrowseTrackerLayout.h \
			ConfigPanel.h \
			ProjectData.h \
			JumpTracker.h \
			JumpData.h

EXTRA_DIST = 	BrowseTracker.cbp \
		BrowseTracker-unix.cbp \
		manifest.xml

pkgdata_DATA = BrowseTracker.zip
CLEANFILES = $(pkgdata_DATA)

BrowseTracker.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/BrowseTracker.zip manifest.xml > /dev/null

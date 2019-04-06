AM_CPPFLAGS = $(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libBrowseTracker.la

libBrowseTracker_la_LDFLAGS = @MODULE_SHARED_LDFLAGS@ -version-info 0:1:0 -no-undefined -avoid-version

libBrowseTracker_la_LIBADD = ../../../sdk/libcodeblocks.la \
			$(WX_LIBS) \
			$(CB_TINYXML_LIBS)

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

resources_forZIP = \
	BrowseTrackerToolbar.xrc \
	manifest.xml \
	images/16x16/mark_clear.png \
	images/16x16/mark_next.png \
	images/16x16/mark.png \
	images/16x16/mark_prev.png \
	images/16x16/next.png \
	images/16x16/prev.png \
	images/22x22/mark_clear.png \
	images/22x22/mark_next.png \
	images/22x22/mark.png \
	images/22x22/mark_prev.png \
	images/22x22/next.png \
	images/22x22/prev.png \
	images/32x32/mark_clear.png \
	images/32x32/mark_next.png \
	images/32x32/mark.png \
	images/32x32/mark_prev.png \
	images/32x32/next.png \
	images/32x32/prev.png

EXTRA_DIST = \
	$(srcdir)/*.cbp \
	$(resources_forZIP)

pkgdata_DATA = BrowseTracker.zip
CLEANFILES = $(pkgdata_DATA)

BrowseTracker.zip: $(resources_forZIP)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/BrowseTracker.zip $(resources_forZIP) > /dev/null

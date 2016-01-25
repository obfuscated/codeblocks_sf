if ! HAVE_WX29
TREELIST_INCLUDE=-I$(srcdir)/../wxContribItems/wxTreeList/include
TREELIST_LA=../wxContribItems/wxTreeList/libwxtreelist.la
TREELIST_SRC=./wxtreelist/wxsTreeListCtrl.cpp
TREELIST_HDR=./wxtreelist/wxsTreeListCtrl.h
else
TREELIST_DIST=$(srcdir)/wxtreelist/wxsTreeListCtrl.cpp $(srcdir)/wxtreelist/wxsTreeListCtrl.h
endif

AM_CPPFLAGS = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include \
		$(WXPROPGRID_INCLUDE) \
		-I$(top_srcdir)/src/plugins/contrib/wxSmith \
		-I$(top_srcdir)/src/plugins/contrib/wxSmith/wxwidgets \
		-I$(top_srcdir)/src/plugins/contrib/wxSmith/wxwidgets/properties \
		-I$(top_srcdir)/src/plugins/contrib/wxSmith/properties \
		-I$(srcdir)/../wxContribItems/wxchart/include \
		-I$(srcdir)/../wxContribItems/wxFlatNotebook/include \
		-I$(srcdir)/../wxContribItems/wxImagePanel/include \
		-I$(srcdir)/../wxContribItems/wxthings/include \
		-I$(srcdir)/../wxContribItems/KWIC/include \
		$(TREELIST_INCLUDE) \
		-I$(srcdir)/../wxContribItems/wxSpeedButton \
		-I$(srcdir)/../wxContribItems/wxled/gizmosled/include \
		-I$(srcdir)/../wxContribItems/wxled/lcdWindow/include \
		-I$(srcdir)/../wxContribItems/wxled/led/include \
		-I$(srcdir)/../wxContribItems/wxled/ledpanel/include \
		-I$(srcdir)/../wxContribItems/wxmathplot


pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libwxsmithcontribitems.la

libwxsmithcontribitems_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libwxsmithcontribitems_la_LIBADD = ../../../sdk/libcodeblocks.la \
		       ../wxSmith/libwxsmithlib.la \
			../wxContribItems/wxchart/libwxchartctrl.la \
			../wxContribItems/wxFlatNotebook/libwxflatnotebook.la \
			../wxContribItems/wxImagePanel/libwximagepanel.la \
			../wxContribItems/wxSpeedButton/libwxspeedbutton.la \
			../wxContribItems/KWIC/libwxkwic.la \
			$(TREELIST_LA) \
			../wxContribItems/wxthings/libwxcustombutton.la \
			../wxContribItems/wxled/libwxled.la \
			../wxContribItems/wxmathplot/libwxmathplot.la \
			$(WX_LIBS)

EXTRA_DIST = $(srcdir)/wxchart/*.xpm \
		$(srcdir)/wxchart/*.png \
		$(srcdir)/wximagebutton/*.xpm \
		$(srcdir)/wximagepanel/*.xpm \
		$(srcdir)/wxspeedbutton/*.xpm \
		$(srcdir)/wxthings/*.xpm \
		$(srcdir)/wxthings/*.png \
		$(srcdir)/wxflatnotebook/images/*.xpm \
		$(srcdir)/wxflatnotebook/images/*.png \
		$(srcdir)/wxSmithKWIC/images/*.xpm \
		$(srcdir)/wxtreelist/images/*.xpm \
		$(TREELIST_DIST) \
		$(srcdir)/wxgbsizer/images/*.xpm \
		$(srcdir)/wxled/images/*.xpm \
		$(srcdir)/wxSmithPlot/images/*.xpm \
		$(srcdir)/wxSmithPlot/test/*.h \
		$(srcdir)/wxSmithPlot/test/*.cpp \
		$(srcdir)/wxSmithPlot/test/resource.rc \
		$(srcdir)/wxSmithPlot/test/*.cbp \
		$(srcdir)/wxSmithPlot/test/wxsmith/*.wxs \
		$(srcdir)/*.cbp \
		manifest.xml

pkgdata_DATA = wxsmithcontribitems.zip
CLEANFILES = $(pkgdata_DATA)

wxsmithcontribitems.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/wxsmithcontribitems.zip manifest.xml > /dev/null

libwxsmithcontribitems_la_SOURCES = \
./wxflatnotebook/wxsFlatNotebook.cpp \
./wxchart/wxschart.cpp \
./wxthings/wxscustombutton.cpp \
./wximagebutton/wxsimagebutton.cpp \
./wximagepanel/wxsimagepanel.cpp \
./wxspeedbutton/wxsSpeedButton.cpp \
./wxSmithKWIC/wxsangularmeter.cpp \
./wxSmithKWIC/wxsangularregulator.cpp \
./wxSmithKWIC/wxsbmpcheckbox.cpp \
./wxSmithKWIC/wxsbmpswitcher.cpp \
./wxSmithKWIC/wxslcdclock.cpp \
./wxSmithKWIC/wxslcddisplay.cpp \
./wxSmithKWIC/wxslinearmeter.cpp \
./wxSmithKWIC/wxslinearregulator.cpp \
$(TREELIST_SRC) \
./wxgbsizer/wxsGridBagSizer.cpp \
./wxled/wxsLcdWindow.cpp \
./wxled/wxsLed.cpp \
./wxled/wxsLedNumber.cpp \
./wxled/wxsLedPanel.cpp \
./wxled/wxsStateLed.cpp \
./wxSmithPlot/wxsAxis.cpp \
./wxSmithPlot/wxsMarker.cpp \
./wxSmithPlot/wxsMathPlot.cpp \
./wxSmithPlot/wxsText.cpp \
./wxSmithPlot/wxsVector.cpp \
./wxSmithContribItems.cpp

noinst_HEADERS = \
./wxflatnotebook/wxsFlatNotebook.h \
./wxchart/wxschart.h \
./wxthings/wxscustombutton.h \
./wximagebutton/wxsimagebutton.h \
./wximagepanel/wxsimagepanel.h \
./wxspeedbutton/wxsSpeedButton.h \
./wxSmithKWIC/wxsangularmeter.h \
./wxSmithKWIC/wxsangularregulator.h \
./wxSmithKWIC/wxsbmpcheckbox.h \
./wxSmithKWIC/wxsbmpswitcher.h \
./wxSmithKWIC/wxslcdclock.h \
./wxSmithKWIC/wxslcddisplay.h \
./wxSmithKWIC/wxslinearmeter.h \
./wxSmithKWIC/wxslinearregulator.h \
$(TREELIST_HDR) \
./wxgbsizer/wxsGridBagSizer.h \
./wxled/wxsLcdWindow.h \
./wxled/wxsLed.h \
./wxled/wxsLedNumber.h \
./wxled/wxsLedPanel.h \
./wxled/wxsStateLed.h \
./wxSmithPlot/wxsAxis.h \
./wxSmithPlot/wxsMarker.h \
./wxSmithPlot/wxsMathPlot.h \
./wxSmithPlot/wxsText.h \
./wxSmithPlot/wxsVector.h \
./wxSmithContribItems.h

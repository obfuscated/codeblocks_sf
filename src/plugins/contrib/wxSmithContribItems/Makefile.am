INCLUDES = $(WX_CXXFLAGS) \
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
		-I$(srcdir)/../wxContribItems/wxTreeList/include \
		-I$(srcdir)/../wxContribItems/wxSpeedButton \
		-I$(srcdir)/../wxContribItems/wxled/gizmosled/include \
		-I$(srcdir)/../wxContribItems/wxled/lcdWindow/include \
		-I$(srcdir)/../wxContribItems/wxled/led/include \
		-I$(srcdir)/../wxContribItems/wxled/ledpanel/include


pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libwxsmithcontribitems.la

libwxsmithcontribitems_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libwxsmithcontribitems_la_LIBADD = $(WX_LIBS) ../../../sdk/libcodeblocks.la \
		       ../wxSmith/libwxsmithlib.la \
			../wxContribItems/wxchart/libwxchartctrl.la \
			../wxContribItems/wxFlatNotebook/libwxflatnotebook.la \
			../wxContribItems/wxImagePanel/libwximagepanel.la \
			../wxContribItems/wxSpeedButton/libwxspeedbutton.la \
			../wxContribItems/KWIC/libwxkwic.la \
			../wxContribItems/wxTreeList/libwxtreelist.la \
			../wxContribItems/wxthings/libwxcustombutton.la \
			../wxContribItems/wxled/libwxled.la

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
		$(srcdir)/wxgbsizer/images/*.xpm \
		$(srcdir)/wxled/images/*.xpm \
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
./wxtreelist/wxsTreeListCtrl.cpp \
./wxgbsizer/wxsGridBagSizer.cpp \
./wxled/wxsLcdWindow.cpp \
./wxled/wxsLed.cpp \
./wxled/wxsLedNumber.cpp \
./wxled/wxsLedPanel.cpp \
./wxled/wxsStateLed.cpp \
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
./wxtreelist/wxsTreeListCtrl.h \
./wxgbsizer/wxsGridBagSizer.h \
./wxled/wxsLcdWindow.h \
./wxled/wxsLed.h \
./wxled/wxsLedNumber.h \
./wxled/wxsLedPanel.h \
./wxled/wxsStateLed.h \
./wxSmithContribItems.h

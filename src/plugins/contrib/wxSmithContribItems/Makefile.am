SUBDIRS = wxchart/wxchart-1.0 wxflatnotebook/wxFlatNotebook wxthings/wxthings .

AM_CPPFLAGS = -DWXMAKINGDLL_PROPGRID -DwxPG_USE_WXMODULE=0

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include \
		-I$(top_srcdir)/src/sdk/wxpropgrid/include \
		-I$(top_srcdir)/src/plugins/contrib/wxSmith \
		-I$(srcdir)/wxchart/wxchart-1.0/include \
		-I$(srcdir)/wxflatnotebook/wxFlatNotebook/include \
		-I$(srcdir)/wxthings/wxthings/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libwxsmithcontribitems.la

libwxsmithcontribitems_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libwxsmithcontribitems_la_LIBADD = $(PLUGIN_WX_LIBS) ../../../sdk/libcodeblocks.la \
		       ../wxSmith/libwxsmithlib.la \
			wxchart/wxchart-1.0/libwxchartctrl.la \
			wxflatnotebook/wxFlatNotebook/libwxflatnotebook.la \
			wxthings/wxthings/libwxcustombutton.la

EXTRA_DIST = $(srcdir)/wxchart/wxchart-1.0/include/wx/chartart/*.xpm \
		$(srcdir)/wxchart/wxchart-1.0/include/wx/chartart/*.bmp \
		$(srcdir)/wxchart/*.xpm \
		$(srcdir)/wxchart/*.png \
		$(srcdir)/wxthings/*.xpm \
		$(srcdir)/wxthings/*.png \
		$(srcdir)/wxflatnotebook/images/*.xpm \
		$(srcdir)/wxflatnotebook/images/*.png \
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
./wxSmithContribItems.cpp

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = wxsmith-contrib.pc

noinst_HEADERS = \
./wxflatnotebook/wxsFlatNotebook.h \
./wxchart/wxschart.h \
./wxthings/wxscustombutton.h \
./wxSmithContribItems.h

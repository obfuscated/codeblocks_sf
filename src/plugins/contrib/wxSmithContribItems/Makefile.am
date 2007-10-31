AM_CPPFLAGS = -DWXMAKINGLIB_PROPGRID

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxFlatNotebook/include \
		-I$(top_srcdir)/src/include/wxscintilla/include \
		-I$(top_srcdir)/src/include/propgrid/include \
		-I$(top_srcdir)/src/plugins/contrib/wxSmith \
		-I$(srcdir)/wxchart/wxchart-1.0/include \
		-I$(srcdir)/wxthings/wxthings/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libwxsmithcontribitems.la

libwxsmithcontribitems_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libwxsmithcontribitems_la_LIBADD = $(PLUGIN_WX_LIBS) ../../../sdk/libcodeblocks.la \
		       ../wxSmith/libwxsmithlib.la

EXTRA_DIST = ./wxchart/wxchart-1.0/include/wx/chartart/*.xpm \
		./wxchart/wxchart-1.0/include/wx/chartart/*.bmp \
		./wxchart/*.xpm \
		./wxchart/*.png \
		./wxthings/*.xpm \
		./wxthings/*.png \
		./wxflatnotebook/images/*.xpm \
		./wxflatnotebook/images/*.png \
		$(srcdir)/*.cbp \
		manifest.xml

pkgdata_DATA = wxsmithcontribitems.zip
CLEANFILES = $(pkgdata_DATA)

wxsmithcontribitems.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/wxsmithcontribitems.zip manifest.xml > /dev/null

libwxsmithcontribitems_la_SOURCES = \
./wxchart/wxchart-1.0/src/legendwindow.cpp \
./wxchart/wxchart-1.0/src/points.cpp \
./wxchart/wxchart-1.0/src/chartcolors.cpp \
./wxchart/wxchart-1.0/src/label.cpp \
./wxchart/wxchart-1.0/src/chartwindow.cpp \
./wxchart/wxchart-1.0/src/chart.cpp \
./wxchart/wxchart-1.0/src/piechartpoints.cpp \
./wxchart/wxchart-1.0/src/xaxis.cpp \
./wxchart/wxchart-1.0/src/bar3dchartpoints.cpp \
./wxchart/wxchart-1.0/src/axis.cpp \
./wxchart/wxchart-1.0/src/xaxiswindow.cpp \
./wxchart/wxchart-1.0/src/legend.cpp \
./wxchart/wxchart-1.0/src/yaxiswindow.cpp \
./wxchart/wxchart-1.0/src/barchartpoints.cpp \
./wxchart/wxchart-1.0/src/pie3dchartpoints.cpp \
./wxchart/wxchart-1.0/src/chartctrl.cpp \
./wxchart/wxchart-1.0/src/yaxis.cpp \
./wxchart/wxschart.cpp \
./wxthings/wxthings/src/toggle.cpp \
./wxthings/wxscustombutton.cpp \
./wxflatnotebook/wxsFlatNotebook.cpp \
./wxSmithContribItems.cpp

noinst_HEADERS = \
./wxchart/wxschart.h \
./wxchart/wxchart-1.0/include/wx/yaxis.h \
./wxchart/wxchart-1.0/include/wx/piechartpoints.h \
./wxchart/wxchart-1.0/include/wx/label.h \
./wxchart/wxchart-1.0/include/wx/chartcolors.h \
./wxchart/wxchart-1.0/include/wx/pie3dchartpoints.h \
./wxchart/wxchart-1.0/include/wx/points.h \
./wxchart/wxchart-1.0/include/wx/legend.h \
./wxchart/wxchart-1.0/include/wx/legendwindow.h \
./wxchart/wxchart-1.0/include/wx/chartpointstypes.h \
./wxchart/wxchart-1.0/include/wx/chartwindow.h \
./wxchart/wxchart-1.0/include/wx/barchartpoints.h \
./wxchart/wxchart-1.0/include/wx/chartctrl.h \
./wxchart/wxchart-1.0/include/wx/axis.h \
./wxchart/wxchart-1.0/include/wx/charttypes.h \
./wxchart/wxchart-1.0/include/wx/chartdef.h \
./wxchart/wxchart-1.0/include/wx/xaxiswindow.h \
./wxchart/wxchart-1.0/include/wx/bar3dchartpoints.h \
./wxchart/wxchart-1.0/include/wx/yaxiswindow.h \
./wxchart/wxchart-1.0/include/wx/xaxis.h \
./wxchart/wxchart-1.0/include/wx/chart.h \
./wxchart/wxchart-1.0/include/wx/chartpoints.h \
./wxthings/wxthings/include/wx/things/toggle.h \
./wxthings/wxthings/include/wx/things/thingdef.h \
./wxthings/wxscustombutton.h \
./wxflatnotebook/wxsFlatNotebook.h \
./wxSmithContribItems.h

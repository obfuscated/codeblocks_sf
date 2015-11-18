SUBDIRS = wxmathplot

AM_CPPFLAGS = $(WX_CXXFLAGS) \
                -I$(top_srcdir)/src/include \
                -I$(top_srcdir)/src/sdk/wxscintilla/include \
                $(WXPROPGRID_INCLUDE) \
                -I$(top_srcdir)/src/plugins/contrib/wxSmith \
                -I$(top_srcdir)/src/plugins/contrib/wxSmithPlot/wxmathplot

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libwxSmithPlot.la

libwxSmithPlot_la_LDFLAGS = -pthread -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libwxSmithPlot_la_LIBADD = ../../../sdk/libcodeblocks.la \
                       ../wxSmith/libwxsmithlib.la \
                       ./wxmathplot/libwxsmithplot_wxmathplot.la \
                       $(WX_LIBS)


EXTRA_DIST = 	$(srcdir)/*.cpp \
		$(srcdir)/*.h \
		$(srcdir)/*.cbp \
		$(srcdir)/manifest.xml \
		$(srcdir)/images/*.bmp \
		$(srcdir)/images/*.xpm \
		$(srcdir)/test/*.h \
		$(srcdir)/test/*.cpp \
		$(srcdir)/test/resource.rc \
		$(srcdir)/test/*.cbp \
		$(srcdir)/test/wxsmith/*.wxs

pkgdata_DATA = wxSmithPlot.zip

CLEANFILES = $(pkgdata_DATA)

wxSmithPlot.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/wxSmithPlot.zip manifest.xml > /dev/null

libwxSmithPlot_la_SOURCES = \
./wxsAxis.cpp \
./wxsText.cpp \
./wxsMarker.cpp \
./wxsMathPlot.cpp \
./wxsVector.cpp \
./wxsSmithPlot.cpp

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = wxsmithplot.pc

noinst_HEADERS = \
./wxsAxis.h \
./wxsText.h \
./wxsMarker.h \
./wxsMathPlot.h \
./wxsVector.h \
./wxsSmithPlot.h

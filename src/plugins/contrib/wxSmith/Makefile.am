SUBDIRS = propgrid properties wxwidgets . plugin

AM_CPPFLAGS = -DWXMAKINGLIB_PROPGRID

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxFlatNotebook/include \
		-I$(top_srcdir)/src/include/wxscintilla/include \
                -I$(top_srcdir)/src/include/scripting/include \
                -I$(top_srcdir)/src/include/scripting/squirrel \
                -I$(top_srcdir)/src/include/scripting/sqplus \
                -I$(top_srcdir)/src/include/scripting/bindings \
		-I$(top_srcdir)/src/plugins/contrib/wxSmith/propgrid/contrib/include

lib_LTLIBRARIES = libwxsmithlib.la

libwxsmithlib_la_LDFLAGS = -version-info 0:1:0 -shared

libwxsmithlib_la_LIBADD = $(PLUGIN_WX_LIBS) \
				../../../sdk/libcodeblocks.la \
				properties/libwxsmith_properties.la \
				wxwidgets/libwxsmith_wxwidgets.la \
				propgrid/libpropgrid.la

libwxsmithlib_la_SOURCES = \
./wxsadvqpp.cpp \
./wxsadvqppchild.cpp \
./wxscoder.cpp \
./wxscodinglang.cpp \
./wxseditor.cpp \
./wxsextresmanager.cpp \
./wxsgui.cpp \
./wxsguifactory.cpp \
./wxsmith.cpp \
./wxsmithmime.cpp \
./wxsproject.cpp \
./wxsresource.cpp \
./wxsresourcefactory.cpp \
./wxsresourcetree.cpp \
./wxsresourcetreeitemdata.cpp \
./wxsstoringsplitterwindow.cpp \
./wxsversionconverter.cpp \
./wxssettings.cpp \
./wxsautoresourcetreeimage.cpp

sdkdir=$(includedir)/wxsmith/
 
sdk_HEADERS = \
./wxsadvqpp.h  \
./wxsautoresourcetreeimage.h \
./wxscodinglang.h \
./wxsresourcetree.h \
./wxsresourcetreeitemdata.h 
 
noinst_HEADERS = \
./wxsadvqppchild.h \
./wxscoder.h \
./wxseditor.h \
./wxsextresmanager.h \
./wxsgui.h \
./wxsguifactory.h \
./wxsmith.h \
./wxsmithmime.h \
./wxsproject.h \
./wxsresource.h \
./wxsresourcefactory.h \
./wxsstoringsplitterwindow.h \
./wxsversionconverter.h \
./wxssettings.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = wxsmith.pc

EXTRA_DIST = \
$(srcdir)/*.cbp \
$(srcdir)/*.bat \
manifest.xml

pkgdata_DATA = wxsmith.zip
CLEANFILES = $(pkgdata_DATA)

wxsmith.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/wxsmith.zip manifest.xml > /dev/null

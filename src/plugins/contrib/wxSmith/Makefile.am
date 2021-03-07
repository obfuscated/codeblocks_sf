SUBDIRS = properties wxwidgets . plugin

AM_CPPFLAGS = $(WX_CXXFLAGS) \
		$(CB_SQUIRREL_CFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

lib_LTLIBRARIES = libwxsmithlib.la

libwxsmithlib_la_LDFLAGS = -version-info 0:1:0 -shared

libwxsmithlib_la_LIBADD = ../../../sdk/libcodeblocks.la \
				properties/libwxsmith_properties.la \
				wxwidgets/libwxsmith_wxwidgets.la \
				$(WX_LIBS)

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
./wxsadvqppchild.h \
./wxsautoresourcetreeimage.h \
./wxscodinglang.h \
./wxsgui.h \
./wxsguifactory.h \
./wxseditor.h \
./wxsproject.h \
./wxsresource.h \
./wxsresourcefactory.h \
./wxsresourcetree.h \
./wxsresourcetreeitemdata.h

noinst_HEADERS = \
./wxscoder.h \
./wxsextresmanager.h \
./wxsmith.h \
./wxsmithmime.h \
./wxsstoringsplitterwindow.h \
./wxsversionconverter.h \
./wxssettings.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = wxsmith.pc

EXTRA_DIST = \
$(srcdir)/*.cbp \
$(srcdir)/*.bat \
$(srcdir)/update* \
$(srcdir)/wxsmith/* \
manifest.xml \
wxs.doxy

pkgdata_DATA = wxsmith.zip
CLEANFILES = $(pkgdata_DATA)

wxsmith.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/wxsmith.zip manifest.xml > /dev/null

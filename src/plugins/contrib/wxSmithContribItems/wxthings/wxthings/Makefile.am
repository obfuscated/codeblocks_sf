AM_CPPFLAGS = -DWXMAKINGDLL_PROPGRID -DwxPG_USE_WXMODULE=0

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/plugins/contrib/wxSmith \
		-I$(top_srcdir)/src/sdk/wxpropgrid/include \
		-I$(top_srcdir)/src/plugins/contrib/wxSmithContribItems/wxthings/wxthings/include

wxsmithcontribitemsdir=$(libdir)/wxSmithContribItems

wxsmithcontribitems_LTLIBRARIES = libwxcustombutton.la

libwxcustombutton_la_LDFLAGS = -version-info 0:1:0

libwxcustombutton_la_LIBADD = $(WX_LIBS)

libwxcustombutton_la_SOURCES = \
./src/toggle.cpp

sdkdir=$(includedir)/wxSmithContribItems/wxthings/wxthings/include/wx/things

sdk_HEADERS = \
./include/wx/things/toggle.h \
./include/wx/things/thingdef.h


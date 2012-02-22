AM_CPPFLAGS = -DwxPG_USE_WXMODULE=0

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include \
		-I$(top_srcdir)/src/sdk/wxpropgrid/include

noinst_LTLIBRARIES = libwxsmith_properties.la

libwxsmith_properties_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libwxsmith_properties_la_LIBADD = ../../../../sdk/libcodeblocks.la

libwxsmith_properties_la_SOURCES = \
./wxsarraystringeditordlg.cpp \
./wxsboolproperty.cpp \
./wxspropertystream.cpp \
./wxsflagsproperty.cpp \
./wxsfloatproperty.cpp \
./wxsemptyproperty.cpp \
./wxsstringproperty.cpp \
./wxslongproperty.cpp \
./wxsenumproperty.cpp \
./wxsarraystringproperty.cpp \
./wxsquickpropspanel.cpp \
./wxsproperty.cpp \
./wxspropertygridmanager.cpp \
./wxscustomeditorproperty.cpp \
./wxspropertycontainer.cpp \
./wxseditenumproperty.cpp

sdkdir=$(includedir)/wxsmith/properties

sdk_HEADERS = \
./wxspropertycontainer.h \
./wxsproperties.h \
./wxsboolproperty.h \
./wxspropertystream.h \
./wxsflagsproperty.h \
./wxsfloatproperty.h \
./wxsemptyproperty.h \
./wxslongproperty.h \
./wxsarraystringeditordlg.h \
./wxsenumproperty.h \
./wxsarraystringproperty.h \
./wxsstringproperty.h \
./wxsquickpropspanel.h \
./wxsproperty.h \
./wxspropertygridmanager.h \
./wxscustomeditorproperty.h \
./wxseditenumproperty.h

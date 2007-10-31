AM_CPPFLAGS = -DWXMAKINGLIB_PROPGRID

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxFlatNotebook/include \
		-I$(top_srcdir)/src/include/wxscintilla/include \
		-I$(top_srcdir)/src/include/wxFlatNotebook \
		-I$(top_srcdir)/src/include/propgrid/include

noinst_LTLIBRARIES = libwxsmith_wxwidgets_properties.la

libwxsmith_wxwidgets_properties_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libwxsmith_wxwidgets_properties_la_LIBADD = ../../../../../sdk/libcodeblocks.la

libwxsmith_wxwidgets_properties_la_SOURCES = \
./wxspositionsizeproperty.cpp \
./wxsfonteditordlg.cpp \
./wxsarraystringcheckeditordlg.cpp \
./wxscolourproperty.cpp \
./wxsstyleproperty.cpp \
./wxsarraystringcheckproperty.cpp \
./wxsfontproperty.cpp \
./wxssizerflagsproperty.cpp \
./wxsfontfaceeditordlg.cpp \
./wxsbitmapiconeditordlg.cpp \
./wxsdimensionproperty.cpp \
./wxsbitmapiconproperty.cpp \
./wxssimplefonteditordlg.cpp

noinst_HEADERS = \
./wxsdimensionproperty.h \
./wxsbitmapiconproperty.h \
./wxsarraystringcheckeditordlg.h \
./wxsfonteditordlg.h \
./wxspositionsizeproperty.h \
./wxsstyleproperty.h \
./wxscolourproperty.h \
./wxsarraystringcheckproperty.h \
./wxsfontproperty.h \
./wxssizerflagsproperty.h \
./wxsfontfaceeditordlg.h \
./wxsbitmapiconeditordlg.h \
./wxssimplefonteditordlg.h

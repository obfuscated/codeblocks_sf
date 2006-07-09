INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include \
		-I$(top_srcdir)/src/sdk/propgrid/include

noinst_LTLIBRARIES = libwxsmith_properties.la

libwxsmith_properties_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libwxsmith_properties_la_LIBADD = ../../../../sdk/libcodeblocks.la

libwxsmith_properties_la_SOURCES = wxs2intproperty.cpp \
wxsadvimageproperty.cpp \
wxsboolproperty.cpp \
wxsborderproperty.cpp \
wxsenumproperty.cpp \
wxsintproperty.cpp \
wxsplacementproperty.cpp \
wxsstringlistproperty.cpp \
wxsstringproperty.cpp \
wxsstyleproperty.cpp \
wxscolourproperty.cpp \
wxsfontproperty.cpp \
wxsstringlistcheckproperty.cpp \
wxscheckstringseditor.cpp

noinst_HEADERS = wxs2intproperty.h \
wxsadvimageproperty.h \
wxsboolproperty.h \
wxsborderproperty.h \
wxsenumproperty.h \
wxsintproperty.h \
wxsplacementproperty.h \
wxsstringlistproperty.h \
wxsstringproperty.h \
wxsstyleproperty.h \
wxscolourproperty.h \
wxsfontproperty.h \
wxsstringlistcheckproperty.h \
wxscheckstringseditor.h

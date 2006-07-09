INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include \
		-I$(top_srcdir)/src/sdk/wxFlatNotebook \
		-I$(top_srcdir)/src/sdk/propgrid/include

noinst_LTLIBRARIES = libwxsmith_resources.la

libwxsmith_resources_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libwxsmith_resources_la_LIBADD = ../../../../sdk/libcodeblocks.la

libwxsmith_resources_la_SOURCES = wxswindowres.cpp \
wxswindowresdataobject.cpp \
wxsdialogpreviewdlg.cpp \
wxsframepreviewfrm.cpp

noinst_HEADERS = wxswindowres.h \
wxswindowresdataobject.h \
wxsdialogpreviewdlg.h \
wxsframepreviewfrm.h

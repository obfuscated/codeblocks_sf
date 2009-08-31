INCLUDES = $(WX_CXXFLAGS) \
        $(WX_GTK2_CFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libMouseSap.la

libMouseSap_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libMouseSap_la_LIBADD = $(PLUGIN_WX_LIBS) ../../../sdk/libcodeblocks.la

libMouseSap_la_SOURCES = MouseSap.cpp \
			MouseSapcfg.cpp \
			dragscrollevent.cpp

noinst_HEADERS = MouseSap.h \
		MouseSapcfg.h \
		dragscrollevent.h

EXTRA_DIST = MouseSap.cbp \
		MouseSap-unix.cbp \
		manifest.xml

pkgdata_DATA = MouseSap.zip

CLEANFILES = $(pkgdata_DATA)

MouseSap.zip:
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/MouseSap.zip manifest.xml > /dev/null

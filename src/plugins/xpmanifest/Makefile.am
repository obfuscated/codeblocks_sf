INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libxpmanifest.la

libxpmanifest_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libxpmanifest_la_LIBADD = ../../sdk/libcodeblocks.la $(WX_LIBS)

libxpmanifest_la_SOURCES = windowsxplooknfeel.cpp
noinst_HEADERS = windowsxplooknfeel.h

EXTRA_DIST = manifest.xml
pkgdata_DATA = xpmanifest.zip
CLEANFILES = $(pkgdata_DATA)

xpmanifest.zip: $(EXTRA_DIST)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/xpmanifest.zip manifest.xml > /dev/null

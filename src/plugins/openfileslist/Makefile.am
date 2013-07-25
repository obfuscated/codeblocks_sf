AM_CPPFLAGS = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libopenfileslist.la

libopenfileslist_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libopenfileslist_la_LIBADD = ../../sdk/libcodeblocks.la $(WX_LIBS)

libopenfileslist_la_SOURCES = openfileslistplugin.cpp
noinst_HEADERS = openfileslistplugin.h

pkgdata_DATA = openfileslist.zip
CLEANFILES = $(pkgdata_DATA)

EXTRA_DIST = manifest.xml

openfileslist.zip: $(EXTRA_DIST)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/openfileslist.zip manifest.xml > /dev/null


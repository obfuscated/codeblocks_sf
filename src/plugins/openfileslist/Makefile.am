INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libopenfileslist.la

libopenfileslist_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libopenfileslist_la_LIBADD = $(PLUGIN_WX_LIBS) ../../sdk/libcodeblocks.la

libopenfileslist_la_SOURCES = openfileslistplugin.cpp
noinst_HEADERS = openfileslistplugin.h

pkgdata_DATA = openfileslist.zip
CLEANFILES = $(pkgdata_DATA)

EXTRA_DIST = manifest.xml

openfileslist.zip: $(EXTRA_DIST)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/openfileslist.zip manifest.xml > /dev/null


INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libautosave.la

libautosave_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libautosave_la_LIBADD = ../../sdk/libcodeblocks.la

libautosave_la_SOURCES = autosave.cpp
noinst_HEADERS = autosave.h
EXTRA_DIST = autosave.xrc manifest.xml
pkgdata_DATA = autosave.zip
CLEANFILES = $(pkgdata_DATA)

autosave.zip:
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/autosave.zip manifest.xml *.xrc > /dev/null

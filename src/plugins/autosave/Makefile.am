INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libautosave.la

libautosave_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libautosave_la_LIBADD =  $(WX_LIBS) ../../sdk/libcodeblocks.la

libautosave_la_SOURCES = autosave.cpp
noinst_HEADERS = autosave.h
EXTRA_DIST = autosave.xrc manifest.xml
pkgdata_DATA = autosave.zip
CLEANFILES = $(pkgdata_DATA)

autosave.zip: $(EXTRA_DIST)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/autosave.zip manifest.xml *.xrc > /dev/null

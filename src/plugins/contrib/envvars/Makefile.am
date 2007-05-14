INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxscintilla/include

libdir = $(pkgdatadir)/plugins
pkgdata_DATA = envvars.zip

lib_LTLIBRARIES = libenvvars.la

libenvvars_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libenvvars_la_LIBADD = $(PLUGIN_WX_LIBS) ../../../sdk/libcodeblocks.la

libenvvars_la_SOURCES = envvars.cpp envvars_cdfdlg.cpp envvars_common.cpp

noinst_HEADERS = envvars.h envvars_cdfdlg.h envvars_common.h

EXTRA_DIST = envvars.cbp envvars-unix.cbp envvars.xrc manifest.xml

CLEANFILES = $(pkgdata_DATA)

envvars.zip:
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/envvars.zip manifest.xml *.xrc > /dev/null

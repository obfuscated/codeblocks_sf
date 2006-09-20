INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

libdir = $(pkgdatadir)/plugins
pkgdata_DATA = envvars.zip

lib_LTLIBRARIES = libenvvars.la

libenvvars_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libenvvars_la_LIBADD = ../../../sdk/libcodeblocks.la

libenvvars_la_SOURCES = envvars.cpp

noinst_HEADERS = envvars.h

EXTRA_DIST = envvars.cbp envvars-unix.cbp envvars.xrc manifest.xml

CLEANFILES = $(pkgdata_DATA)

envvars.zip:
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/envvars.zip manifest.xml *.xrc > /dev/null

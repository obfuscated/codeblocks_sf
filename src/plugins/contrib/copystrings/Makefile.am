AM_CPPFLAGS = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libcopystrings.la

libcopystrings_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libcopystrings_la_LIBADD = ../../../sdk/libcodeblocks.la $(WX_LIBS)

libcopystrings_la_SOURCES = copystrings.cpp

noinst_HEADERS = copystrings.h

EXTRA_DIST = 	$(srcdir)/*.cbp \
		manifest.xml

pkgdata_DATA = copystrings.zip

CLEANFILES = $(pkgdata_DATA)

copystrings.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/copystrings.zip manifest.xml > /dev/null

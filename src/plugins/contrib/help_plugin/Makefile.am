SUBDIRS = bzip2 zlib

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxscintilla/include \
		-include "sdk.h"

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libhelp_plugin.la

libhelp_plugin_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libhelp_plugin_la_LIBADD = $(PLUGIN_WX_LIBS) ../../../sdk/libcodeblocks.la \
				bzip2/libbzip2.la \
				zlib/libz.la

libhelp_plugin_la_SOURCES = help_common.cpp \
			HelpConfigDialog.cpp \
			help_plugin.cpp \
			man2html.cpp

noinst_HEADERS = help_common.h \
			HelpConfigDialog.h \
			help_plugin.h \
			man2html.h \
			defs.h

EXTRA_DIST = $(srcdir)/*.cbp \
			help_plugin.xrc \
			manifest.xml \
			Readme.txt

pkgdata_DATA = help_plugin.zip
CLEANFILES = $(pkgdata_DATA)

help_plugin.zip: manifest.xml help_plugin.xrc
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/help_plugin.zip manifest.xml *.xrc > /dev/null

DIST_SUBDIRS = bzip2 zlib
SUBDIRS =
if ! HAVE_BZIP2
SUBDIRS += bzip2
endif
if ! HAVE_ZLIB
SUBDIRS += zlib
endif

AM_CPPFLAGS = $(WX_CXXFLAGS) \
		$(CB_SQUIRREL_CFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include \
		-I$(top_srcdir)/src/include/scripting/bindings \
		-I$(top_srcdir)/src/include/scripting/sqplus

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libhelp_plugin.la

libhelp_plugin_la_LDFLAGS = @MODULE_SHARED_LDFLAGS@ -version-info 0:1:0 -no-undefined -avoid-version

libhelp_plugin_la_LIBADD = ../../../sdk/libcodeblocks.la \
					$(WX_LIBS) \
					$(CB_SQUIRREL_LIBS)


if ! HAVE_BZIP2
libhelp_plugin_la_LIBADD += bzip2/libbzip2.la
AM_CPPFLAGS += -I$(srcdir)/bzip2
else
libhelp_plugin_la_LIBADD += -lbz2
endif
if ! HAVE_ZLIB
libhelp_plugin_la_LIBADD += zlib/libz.la
AM_CPPFLAGS += -I$(srcdir)/zlib
else
libhelp_plugin_la_LIBADD += -lz
endif

libhelp_plugin_la_SOURCES = help_common.cpp \
			HelpConfigDialog.cpp \
			help_plugin.cpp \
			man2html.cpp \
			MANFrame.cpp

noinst_HEADERS = help_common.h \
			HelpConfigDialog.h \
			help_plugin.h \
			man2html.h \
			defs.h \
			MANFrame.h
resources_forZIP = \
	help_plugin.xrc \
	manifest.xml \
	images/16x16/zoomin.png \
	images/16x16/zoomout.png \
	images/20x20/zoomin.png \
	images/20x20/zoomout.png \
	images/24x24/zoomin.png \
	images/24x24/zoomout.png \
	images/28x28/zoomin.png \
	images/28x28/zoomout.png \
	images/32x32/zoomin.png \
	images/32x32/zoomout.png \
	images/40x40/zoomin.png \
	images/40x40/zoomout.png \
	images/48x48/zoomin.png \
	images/48x48/zoomout.png \
	images/56x56/zoomin.png \
	images/56x56/zoomout.png \
	images/64x64/zoomin.png \
	images/64x64/zoomout.png

EXTRA_DIST = $(srcdir)/*.cbp \
			Readme.txt \
			$(resources_forZIP)

pkgdata_DATA = help_plugin.zip
CLEANFILES = $(pkgdata_DATA)

help_plugin.zip: $(resources_forZIP)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/help_plugin.zip $(resources_forZIP) > /dev/null

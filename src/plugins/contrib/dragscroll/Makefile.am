INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libdragscroll.la

libdragscroll_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libdragscroll_la_LIBADD = $(PLUGIN_WX_LIBS) ../../../sdk/libcodeblocks.la

libdragscroll_la_SOURCES = dragscroll.cpp \
			dragscrollcfg.cpp

noinst_HEADERS = dragscroll.h \
			dragscrollcfg.h

EXTRA_DIST = DragScroll.cbp \
		DragScroll-unix.cbp \
		manifest.xml

pkgdata_DATA = dragscroll.zip
CLEANFILES = $(pkgdata_DATA)

dragscroll.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/dragscroll.zip manifest.xml > /dev/null

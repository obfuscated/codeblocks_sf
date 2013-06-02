INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/src \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libdragscroll.la

libdragscroll_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libdragscroll_la_LIBADD = ../../../sdk/libcodeblocks.la $(WX_LIBS)

libdragscroll_la_SOURCES = dragscroll.cpp \
			dragscrollcfg.cpp \
			dragscrollevent.cpp

noinst_HEADERS = dragscroll.h \
			dragscrollcfg.h \
			dragscrollevent.h

EXTRA_DIST = DragScroll.cbp \
		DragScroll-unix.cbp \
		manifest.xml

pkgdata_DATA = dragscroll.zip
CLEANFILES = $(pkgdata_DATA)

dragscroll.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/dragscroll.zip manifest.xml > /dev/null

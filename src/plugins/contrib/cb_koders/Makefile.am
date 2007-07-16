INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libcb_koders.la

libcb_koders_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libcb_koders_la_LIBADD = $(PLUGIN_WX_LIBS) ../../../sdk/libcodeblocks.la

libcb_koders_la_SOURCES = kodersdialog.cpp \
			cb_koders.cpp

noinst_HEADERS = kodersdialog.h \
			cb_koders.h

EXTRA_DIST = wxsmith/KodersDialog.wxs \
			cb_koders.cbp \
			manifest.xml

pkgdata_DATA = cb_koders.zip
CLEANFILES = $(pkgdata_DATA)

cb_koders.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/cb_koders.zip manifest.xml > /dev/null

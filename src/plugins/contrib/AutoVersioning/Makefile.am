INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libAutoVersioning.la

libAutoVersioning_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libAutoVersioning_la_LIBADD = $(PLUGIN_WX_LIBS) ../../../sdk/libcodeblocks.la

libAutoVersioning_la_SOURCES = AutoVersioning.cpp \
            avChangesDlg.cpp \
            avHeader.cpp \
            avSvnRevision.cpp \
            avVersionEditorDlg.cpp


noinst_HEADERS = AutoVersioning.h \
            avChangesDlg.h \
            avConfig.h \
            avHeader.h \
            avSvnRevision.h \
            avVersionEditorDlg.h

EXTRA_DIST = wxsmith/avChangesDlg.wxs \
            wxSmith/avVersionEditorDlg.wxs \
            AutoVersioning.cbp \
            AutoVersioning-unix.cbp \
            manifest.xml

pkgdata_DATA = AutoVersioning.zip
CLEANFILES = $(pkgdata_DATA)

byogames.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/AutoVersioning.zip manifest.xml > /dev/null

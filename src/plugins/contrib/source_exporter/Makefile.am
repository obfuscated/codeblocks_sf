SUBDIRS = wxPdfDocument

AM_CPPFLAGS = $(WX_CXXFLAGS) \
		$(CB_FONTCONFIG_CFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include \
		-I$(top_srcdir)/src/plugins/contrib/source_exporter/wxPdfDocument/include \
		-include "sdk.h" \
		-DwxPDF_USE_WXMODULE=0

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libexporter.la

libexporter_la_LDFLAGS = @MODULE_SHARED_LDFLAGS@ -version-info 0:1:0 -no-undefined -avoid-version

libexporter_la_LIBADD = ../../../sdk/libcodeblocks.la \
				wxPdfDocument/libwxPdfDocument.la \
				$(WX_LIBS) $(CB_FONTCONFIG_LIBS)

libexporter_la_SOURCES = BaseExporter.cpp \
				exporter.cpp \
				HTMLExporter.cpp \
				ODTExporter.cpp \
				PDFExporter.cpp \
				RTFExporter.cpp

noinst_HEADERS = BaseExporter.h \
				exporter.h \
				HTMLExporter.h \
				ODTExporter.h \
				PDFExporter.h \
				RTFExporter.h

EXTRA_DIST = $(srcdir)/*.cbp manifest.xml

pkgdata_DATA = exporter.zip
CLEANFILES = $(pkgdata_DATA)

exporter.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/exporter.zip manifest.xml > /dev/null

SUBDIRS = wxPdfDocument

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxscintilla/include \
		-I$(top_srcdir)/src/plugins/contrib/source_exporter/wxPdfDocument/include \
		-include "sdk.h"

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libexporter.la

libexporter_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libexporter_la_LIBADD = ../../../sdk/libcodeblocks.la \
				wxPdfDocument/libwxPdfDocument.la

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

EXTRA_DIST = *.cbp manifest.xml

pkgdata_DATA = exporter.zip
CLEANFILES = $(pkgdata_DATA)

exporter.zip:
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/exporter.zip manifest.xml > /dev/null

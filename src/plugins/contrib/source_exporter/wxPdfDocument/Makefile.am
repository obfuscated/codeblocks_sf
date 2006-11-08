INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/plugins/contrib/source_exporter/wxPdfDocument/include

noinst_LTLIBRARIES = libwxPdfDocument.la

libwxPdfDocument_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libwxPdfDocument_la_SOURCES = src/pdfannotation.cpp \
							src/pdfbarcode.cpp \
							src/pdfcolor.cpp \
							src/pdfdoc.cpp \
							src/pdfencrypt.cpp \
							src/pdffont.cpp \
							src/pdfform.cpp \
							src/pdfgraphics.cpp \
							src/pdfimage.cpp \
							src/pdfkernel.cpp \
							src/pdftemplate.cpp \
							src/pdfxml.cpp

noinst_HEADERS = include/wx/pdfbarcode.h \
				include/wx/pdfdoc.h \
				include/wx/pdfdocdef.h \
				include/wx/pdfencrypt.h \
				include/wx/pdffont.h \
				include/wx/pdfform.h \
				include/wx/pdfgraphics.h \
				include/wx/pdfimage.h \
				include/wx/pdfproperties.h \
				include/wx/pdftemplate.h \
				include/wx/pdfxml.h

EXTRA_DIST = src/pdfcolordata.inc \
			src/pdffontdata.inc

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/plugins/contrib/source_exporter/wxPdfDocument/include

noinst_LTLIBRARIES = libwxPdfDocument.la

libwxPdfDocument_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libwxPdfDocument_la_SOURCES = src/pdfdoc.cpp \
							src/pdfencrypt.cpp \
							src/pdffont.cpp \
							src/pdfimage.cpp

noinst_HEADERS = include/wx/pdfdoc.h \
				include/wx/pdfdocdef.h \
				include/wx/pdfencrypt.h \
				include/wx/pdffont.h \
				include/wx/pdfimage.h

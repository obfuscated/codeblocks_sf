INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/plugins/contrib/source_exporter/wxPdfDocument/include

noinst_LTLIBRARIES = libwxPdfDocument.la

libwxPdfDocument_la_CPPFLAGS = -DwxPDF_USE_WXMODULE=0

libwxPdfDocument_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libwxPdfDocument_la_SOURCES = src/pdfannotation.cpp \
				src/pdfbarcode.cpp \
				src/pdfcffdecoder.cpp \
				src/pdfcffindex.cpp \
				src/pdfcolour.cpp \
				src/pdfdc.cpp \
				src/pdfdecode.cpp \
				src/pdfdocument.cpp \
				src/pdfencoding.cpp \
				src/pdfencrypt.cpp \
				src/pdffont.cpp \
				src/pdffontdatacore.cpp \
				src/pdffontdata.cpp \
				src/pdffontdataopentype.cpp \
				src/pdffontdatatruetype.cpp \
				src/pdffontdatatype0.cpp \
				src/pdffontdatatype1.cpp \
				src/pdffontdescription.cpp \
				src/pdffontdetails.cpp \
				src/pdffontextended.cpp \
				src/pdffontmanager.cpp \
				src/pdffontparser.cpp \
				src/pdffontparsertruetype.cpp \
				src/pdffontparsertype1.cpp \
				src/pdffontsubsetcff.cpp \
				src/pdffontsubsettruetype.cpp \
				src/pdffontvolt.cpp \
				src/pdfform.cpp \
				src/pdfgradient.cpp \
				src/pdfgraphics.cpp \
				src/pdfimage.cpp \
				src/pdfkernel.cpp \
				src/pdflayer.cpp \
				src/pdfobjects.cpp \
				src/pdfocg.cpp \
				src/pdfparser.cpp \
				src/pdfpattern.cpp \
				src/pdfrijndael.cpp \
				src/pdftemplate.cpp \
				src/pdfutility.cpp \
				src/pdfxml.cpp

noinst_HEADERS = include/wx/pdfannotation.h \
				include/wx/pdfarraydouble.h \
				include/wx/pdfarraytypes.h \
				include/wx/pdfbarcode.h \
				include/wx/pdfbookmark.h \
				include/wx/pdfcffdecoder.h \
				include/wx/pdfcffindex.h \
				include/wx/pdfcjkfontdata.h \
				include/wx/pdfcolour.h \
				include/wx/pdfcoonspatchmesh.h \
				include/wx/pdfcorefontdata.h \
				include/wx/pdfdc28.h \
				include/wx/pdfdc29.h \
				include/wx/pdfdc.h \
				include/wx/pdfdocdef.h \
				include/wx/pdfdoc.h \
				include/wx/pdfdocument.h \
				include/wx/pdfencoding.h \
				include/wx/pdfencrypt.h \
				include/wx/pdffontdatacore.h \
				include/wx/pdffontdata.h \
				include/wx/pdffontdataopentype.h \
				include/wx/pdffontdatatruetype.h \
				include/wx/pdffontdatatype0.h \
				include/wx/pdffontdatatype1.h \
				include/wx/pdffontdescription.h \
				include/wx/pdffontdetails.h \
				include/wx/pdffontextended.h \
				include/wx/pdffont.h \
				include/wx/pdffontmacosx.h \
				include/wx/pdffontmanager.h \
				include/wx/pdffontparser.h \
				include/wx/pdffontparsertruetype.h \
				include/wx/pdffontparsertype1.h \
				include/wx/pdffontsubsetcff.h \
				include/wx/pdffontsubsettruetype.h \
				include/wx/pdffontvolt.h \
				include/wx/pdfform.h \
				include/wx/pdfgradient.h \
				include/wx/pdfgraphics.h \
				include/wx/pdfimage.h \
				include/wx/pdfinfo.h \
				include/wx/pdflayer.h \
				include/wx/pdflinestyle.h \
				include/wx/pdflinks.h \
				include/wx/pdfobjects.h \
				include/wx/pdfparser.h \
				include/wx/pdfpattern.h \
				include/wx/pdfproperties.h \
				include/wx/pdfrijndael.h \
				include/wx/pdfshape.h \
				include/wx/pdfspotcolour.h \
				include/wx/pdftemplate.h \
				include/wx/pdfutility.h \
				include/wx/pdfxml.h

EXTRA_DIST = src/pdfcjkfontdata.inc \
				src/pdfcmapdata.inc \
				src/pdfcodepagedata.inc \
				src/pdfcolourdata.inc \
				src/pdfcorefontdata.inc \
				src/pdfcorefontkerning.inc \
				src/pdfdc28.inc \
				src/pdfdc29.inc \
				src/pdfglyphnames.inc

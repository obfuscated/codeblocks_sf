SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxFlatNotebook/include \
		-I$(top_srcdir)/src/include/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libcodecompletion.la

libcodecompletion_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libcodecompletion_la_LIBADD = $(PLUGIN_WX_LIBS) ../../sdk/libcodeblocks.la

libcodecompletion_la_SOURCES = ccdebuginfo.cpp \
			ccoptionsdlg.cpp \
			ccoptionsprjdlg.cpp \
			classbrowser.cpp \
			classbrowserbuilderthread.cpp \
			codecompletion.cpp \
			insertclassmethoddlg.cpp \
			nativeparser.cpp \
			selectincludefile.cpp \
			parser/parser.cpp \
			parser/parserthread.cpp \
			parser/token.cpp \
			parser/tokenizer.cpp \
			parser/searchtree.cpp

noinst_HEADERS = ccdebuginfo.h \
			ccoptionsdlg.h \
			ccoptionsprjdlg.h \
			classbrowser.h \
			classbrowserbuilderthread.h \
			codecompletion.h \
			insertclassmethoddlg.h \
			nativeparser.h \
			selectincludefile.h \
			parser/parser.h \
			parser/parserthread.h \
			parser/token.h \
			parser/tokenizer.h \
			parser/searchtree.h

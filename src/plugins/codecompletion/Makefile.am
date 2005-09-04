SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

AM_CXXFLAGS = -Wall -g

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libcodecompletion.la

libcodecompletion_la_LDFLAGS = -module

libcodecompletion_la_LIBADD = $(top_srcdir)/src/sdk/libcodeblocks.la \
				$(top_srcdir)/src/sdk/wxscintilla/libwxscintilla.la

libcodecompletion_la_SOURCES = cclist.cpp \
			cclistctrl.cpp \
			ccoptionsdlg.cpp \
			ccrenderer.cpp \
			classbrowser.cpp \
			codecompletion.cpp \
			insertclassmethoddlg.cpp \
			nativeparser.cpp \
			parser/parser.cpp \
			parser/parserthread.cpp \
			parser/token.cpp \
			parser/tokenizer.cpp

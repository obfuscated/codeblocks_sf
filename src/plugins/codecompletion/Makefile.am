SUBDIRS = resources

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include \
			-I$(top_srcdir)/src/include/mozilla_chardet \
			-I$(top_srcdir)/src/include/mozilla_chardet/mfbt \
			-I$(top_srcdir)/src/include/mozilla_chardet/nsprpub/pr/include \
			-I$(top_srcdir)/src/include/mozilla_chardet/xpcom \
			-I$(top_srcdir)/src/include/mozilla_chardet/xpcom/base \
			-I$(top_srcdir)/src/include/mozilla_chardet/xpcom/glue

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libcodecompletion.la

libcodecompletion_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libcodecompletion_la_LIBADD = $(PLUGIN_WX_LIBS) ../../sdk/libcodeblocks.la

libcodecompletion_la_SOURCES = ccoptionsdlg.cpp \
			ccoptionsprjdlg.cpp \
			cctreectrl.cpp \
			classbrowser.cpp \
			classbrowserbuilderthread.cpp \
			codecompletion.cpp \
			coderefactoring.cpp \
			insertclassmethoddlg.cpp \
			nativeparser.cpp \
			nativeparser_base.cpp \
			selectincludefile.cpp \
			systemheadersthread.cpp \
			parser/ccdebuginfo.cpp \
			parser/cclogger.cpp \
			parser/parser.cpp \
			parser/parserthread.cpp \
			parser/parserthreadedtask.cpp \
			parser/profiletimer.cpp \
			parser/token.cpp \
			parser/tokenizer.cpp \
			parser/tokentree.cpp \
			parser/searchtree.cpp \
			parser/expression.cpp

noinst_HEADERS = ccoptionsdlg.h \
			ccoptionsprjdlg.h \
			cctreectrl.h \
			classbrowser.h \
			classbrowserbuilderthread.h \
			codecompletion.h \
			coderefactoring.h \
			insertclassmethoddlg.h \
			nativeparser.h \
			nativeparser_base.h \
			selectincludefile.h \
			systemheadersthread.h \
			parser/ccdebuginfo.h \
			parser/cclogger.h \
			parser/parser.h \
			parser/parserthread.h \
			parser/parserthreadedtask.h \
			parser/profiletimer.h \
			parser/token.h \
			parser/tokenizer.h \
			parser/tokentree.h \
			parser/searchtree.h \
			parser/expression.h

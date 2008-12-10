INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxscintilla/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libHexEditor.la

libHexEditor_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libHexEditor_la_LIBADD = $(PLUGIN_WX_LIBS) ../../../sdk/libcodeblocks.la

libHexEditor_la_SOURCES = \
	FileContentBase.cpp \
	FileContentBuffered.cpp \
	HexEditor.cpp \
	HexEditPanel.cpp \
	ExpressionExecutor.cpp \
	ExpressionParser.cpp \
	ExpressionPreprocessed.cpp \
	ExpressionTester.cpp \
	ExpressionTestCases.cpp \
	TestCasesDlg.cpp \
	SelectStoredExpressionDlg.cpp \
	CharacterView.cpp \
	DigitView.cpp \
	HexEditLineBuffer.cpp \
	HexEditViewBase.cpp

noinst_HEADERS = \
	FileContentBase.h \
	FileContentBuffered.h \
	HexEditor.h \
	HexEditPanel.h \
	ExpressionExecutor.h \
	ExpressionParser.h \
	ExpressionPreprocessed.h \
	ExpressionTester.h \
	ExpressionTestCases.h \
	TestCasesDlg.h \
	SelectStoredExpressionDlg.h \
	CharacterView.h \
	DigitView.h \
	HexEditLineBuffer.h \
	HexEditViewBase.h



EXTRA_DIST = \
	wxsmith/HexEditPanel.wxs \
	wxsmith/ExpressionTester.wxs \
	wxsmith/SelectStoredExpressionDlg.wxs \
	wxsmith/TestCasesDlg.wxs \
	HexEditor-unix.cbp \
	HexEditor-win.cbp \
	manifest.xml

pkgdata_DATA = HexEditor.zip
CLEANFILES = $(pkgdata_DATA)

HexEditor.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/HexEditor.zip manifest.xml > /dev/null

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libHexEditor.la

libHexEditor_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libHexEditor_la_LIBADD = ../../../sdk/libcodeblocks.la $(WX_LIBS)

libHexEditor_la_SOURCES = \
	CharacterView.cpp \
	DigitView.cpp \
	ExpressionExecutor.cpp \
	ExpressionParser.cpp \
	ExpressionPreprocessed.cpp \
	ExpressionTestCases.cpp \
	ExpressionTester.cpp \
	FileContentBase.cpp \
	FileContentBuffered.cpp \
	FileContentDisk.cpp \
	HexEditLineBuffer.cpp \
	HexEditor.cpp \
	HexEditPanel.cpp \
	HexEditViewBase.cpp \
	SearchDialog.cpp \
	SelectStoredExpressionDlg.cpp \
	TestCasesDlg.cpp


noinst_HEADERS = \
	CharacterView.h \
	DigitView.h \
	ExpressionExecutor.h \
	ExpressionParser.h \
	ExpressionPreprocessed.h \
	ExpressionTestCases.h \
	ExpressionTester.h \
	FileContentBase.h \
	FileContentBuffered.h \
	FileContentDisk.h \
	HexEditLineBuffer.h \
	HexEditor.h \
	HexEditPanel.h \
	HexEditViewBase.h \
	SearchDialog.h \
	SelectStoredExpressionDlg.h \
	TestCasesBase.h \
	TestCasesDlg.h \
	TestCasesHelper.h



EXTRA_DIST = \
	wxsmith/HexEditPanel.wxs \
	wxsmith/ExpressionTester.wxs \
	wxsmith/SearchDialog.wxs \
	wxsmith/SelectStoredExpressionDlg.wxs \
	wxsmith/TestCasesDlg.wxs \
	$(srcdir)/*.cbp \
	manifest.xml

pkgdata_DATA = HexEditor.zip
CLEANFILES = $(pkgdata_DATA)

HexEditor.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/HexEditor.zip manifest.xml > /dev/null

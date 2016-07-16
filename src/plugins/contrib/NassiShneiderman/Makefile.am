AM_CPPFLAGS = $(WX_CXXFLAGS) \
		$(CB_TINYXML_CFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include \
		-DBOOST_SYSTEM_NO_DEPRECATED

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libNassiShneiderman.la

libNassiShneiderman_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libNassiShneiderman_la_LIBADD = ../../../sdk/libcodeblocks.la $(WX_LIBS)

libNassiShneiderman_la_SOURCES = bricks.cpp \
		cbEditorPanel.cpp \
		commands.cpp \
		CParser.cpp \
		DataObject.cpp \
		FileContent.cpp \
		GraphBricks.cpp \
		GraphFabric.cpp \
		HooverDrawlet.cpp \
		InsertBrickTask.cpp \
		logger.cpp \
		NassiDiagramWindow.cpp \
		NassiDropTarget.cpp \
		NassiEditorPanel.cpp \
		NassiFileContent.cpp \
		NassiPlugin.cpp \
		NassiView.cpp \
		parseactions.cpp \
		PasteTask.cpp \
		RedHatchDrawlet.cpp \
		RedLineDrawlet.cpp \
		Task.cpp \
		TextCtrl.cpp \
		TextCtrlTask.cpp \
		TextGraph.cpp

noinst_HEADERS = bricks.h \
		cbEditorPanel.h \
		commands.h \
		DataObject.h \
		FileContent.h \
		GraphBricks.h \
		GraphFabric.h \
		HooverDrawlet.h \
		InsertBrickTask.h \
		logger.h \
		NassiBrickVisitor.h \
		NassiDiagramWindow.h \
		NassiDropTarget.h \
		NassiEditorPanel.h \
		NassiFileContent.h \
		NassiPlugin.h \
		NassiView.h \
		parseactions.h \
		parserules.h \
		PasteTask.h \
		RedHatchDrawlet.h \
		RedLineDrawlet.h \
		Task.h \
		TextCtrl.h \
		TextCtrlTask.h \
		TextGraph.h


EXTRA_DIST = 	$(srcdir)/*.cbp \
		$(srcdir)/rc/*.xpm \
		$(srcdir)/EditTextTask.h \
		$(srcdir)/EditTextTask.cpp \
		$(srcdir)/HooverLine.h \
		$(srcdir)/HooverLine.cpp \
		$(srcdir)/TaskSelect.h \
		$(srcdir)/TaskSelect.cpp \
		$(srcdir)/TaskTest.h \
		$(srcdir)/TaskTest.cpp \
		manifest.xml

pkgdata_DATA = NassiShneiderman.zip

CLEANFILES = $(pkgdata_DATA)

NassiShneiderman.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/NassiShneiderman.zip manifest.xml > /dev/null

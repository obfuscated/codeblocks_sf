AM_CPPFLAGS = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include \
		$(BOOST_CPPFLAGS) \
		-DBOOST_SYSTEM_NO_DEPRECATED

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libNassiShneiderman.la

libNassiShneiderman_la_LDFLAGS = @MODULE_SHARED_LDFLAGS@ -version-info 0:1:0 -no-undefined -avoid-version

libNassiShneiderman_la_LIBADD = ../../../sdk/libcodeblocks.la $(WX_LIBS) $(BOOST_LDFLAGS) $(BOOST_SYSTEM_LIB)

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

resources_forZIP = \
		images/16x16/blocktool.png \
		images/16x16/breaktool.png \
		images/16x16/commentToggletool.png \
		images/16x16/continuetool.png \
		images/16x16/dowhiletool.png \
		images/16x16/fortool.png \
		images/16x16/glassntool.png \
		images/16x16/glassptool.png \
		images/16x16/iftool.png \
		images/16x16/instrtool.png \
		images/16x16/returntool.png \
		images/16x16/selecttool.png \
		images/16x16/sourceToggletool.png \
		images/16x16/switchtool.png \
		images/16x16/whiletool.png \
		images/22x22/blocktool.png \
		images/22x22/breaktool.png \
		images/22x22/commentToggletool.png \
		images/22x22/continuetool.png \
		images/22x22/dowhiletool.png \
		images/22x22/fortool.png \
		images/22x22/glassntool.png \
		images/22x22/glassptool.png \
		images/22x22/iftool.png \
		images/22x22/instrtool.png \
		images/22x22/returntool.png \
		images/22x22/selecttool.png \
		images/22x22/sourceToggletool.png \
		images/22x22/switchtool.png \
		images/22x22/whiletool.png \
		images/32x32/blocktool.png \
		images/32x32/breaktool.png \
		images/32x32/commentToggletool.png \
		images/32x32/continuetool.png \
		images/32x32/dowhiletool.png \
		images/32x32/fortool.png \
		images/32x32/glassntool.png \
		images/32x32/glassptool.png \
		images/32x32/iftool.png \
		images/32x32/instrtool.png \
		images/32x32/returntool.png \
		images/32x32/selecttool.png \
		images/32x32/sourceToggletool.png \
		images/32x32/switchtool.png \
		images/32x32/whiletool.png \
		manifest.xml \
		nassi_shneiderman_toolbar.xrc

EXTRA_DIST = $(srcdir)/*.cbp \
		$(srcdir)/rc/*.xpm \
		$(srcdir)/EditTextTask.h \
		$(srcdir)/EditTextTask.cpp \
		$(srcdir)/HooverLine.h \
		$(srcdir)/HooverLine.cpp \
		$(srcdir)/TaskSelect.h \
		$(srcdir)/TaskSelect.cpp \
		$(srcdir)/TaskTest.h \
		$(srcdir)/TaskTest.cpp \
		$(resources_forZIP)

pkgdata_DATA = NassiShneiderman.zip

CLEANFILES = $(pkgdata_DATA)

NassiShneiderman.zip: $(resources_forZIP)
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/NassiShneiderman.zip $(resources_forZIP) > /dev/null

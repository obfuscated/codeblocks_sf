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
		images/20x20/blocktool.png \
		images/20x20/breaktool.png \
		images/20x20/commentToggletool.png \
		images/20x20/continuetool.png \
		images/20x20/dowhiletool.png \
		images/20x20/fortool.png \
		images/20x20/glassntool.png \
		images/20x20/glassptool.png \
		images/20x20/iftool.png \
		images/20x20/instrtool.png \
		images/20x20/returntool.png \
		images/20x20/selecttool.png \
		images/20x20/sourceToggletool.png \
		images/20x20/switchtool.png \
		images/20x20/whiletool.png \
		images/24x24/blocktool.png \
		images/24x24/breaktool.png \
		images/24x24/commentToggletool.png \
		images/24x24/continuetool.png \
		images/24x24/dowhiletool.png \
		images/24x24/fortool.png \
		images/24x24/glassntool.png \
		images/24x24/glassptool.png \
		images/24x24/iftool.png \
		images/24x24/instrtool.png \
		images/24x24/returntool.png \
		images/24x24/selecttool.png \
		images/24x24/sourceToggletool.png \
		images/24x24/switchtool.png \
		images/24x24/whiletool.png \
		images/28x28/blocktool.png \
		images/28x28/breaktool.png \
		images/28x28/commentToggletool.png \
		images/28x28/continuetool.png \
		images/28x28/dowhiletool.png \
		images/28x28/fortool.png \
		images/28x28/glassntool.png \
		images/28x28/glassptool.png \
		images/28x28/iftool.png \
		images/28x28/instrtool.png \
		images/28x28/returntool.png \
		images/28x28/selecttool.png \
		images/28x28/sourceToggletool.png \
		images/28x28/switchtool.png \
		images/28x28/whiletool.png \
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
		images/40x40/blocktool.png \
		images/40x40/breaktool.png \
		images/40x40/commentToggletool.png \
		images/40x40/continuetool.png \
		images/40x40/dowhiletool.png \
		images/40x40/fortool.png \
		images/40x40/glassntool.png \
		images/40x40/glassptool.png \
		images/40x40/iftool.png \
		images/40x40/instrtool.png \
		images/40x40/returntool.png \
		images/40x40/selecttool.png \
		images/40x40/sourceToggletool.png \
		images/40x40/switchtool.png \
		images/40x40/whiletool.png \
		images/48x48/blocktool.png \
		images/48x48/breaktool.png \
		images/48x48/commentToggletool.png \
		images/48x48/continuetool.png \
		images/48x48/dowhiletool.png \
		images/48x48/fortool.png \
		images/48x48/glassntool.png \
		images/48x48/glassptool.png \
		images/48x48/iftool.png \
		images/48x48/instrtool.png \
		images/48x48/returntool.png \
		images/48x48/selecttool.png \
		images/48x48/sourceToggletool.png \
		images/48x48/switchtool.png \
		images/48x48/whiletool.png \
		images/56x56/blocktool.png \
		images/56x56/breaktool.png \
		images/56x56/commentToggletool.png \
		images/56x56/continuetool.png \
		images/56x56/dowhiletool.png \
		images/56x56/fortool.png \
		images/56x56/glassntool.png \
		images/56x56/glassptool.png \
		images/56x56/iftool.png \
		images/56x56/instrtool.png \
		images/56x56/returntool.png \
		images/56x56/selecttool.png \
		images/56x56/sourceToggletool.png \
		images/56x56/switchtool.png \
		images/56x56/whiletool.png \
		images/64x64/blocktool.png \
		images/64x64/breaktool.png \
		images/64x64/commentToggletool.png \
		images/64x64/continuetool.png \
		images/64x64/dowhiletool.png \
		images/64x64/fortool.png \
		images/64x64/glassntool.png \
		images/64x64/glassptool.png \
		images/64x64/iftool.png \
		images/64x64/instrtool.png \
		images/64x64/returntool.png \
		images/64x64/selecttool.png \
		images/64x64/sourceToggletool.png \
		images/64x64/switchtool.png \
		images/64x64/whiletool.png \
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

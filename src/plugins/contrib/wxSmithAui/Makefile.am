INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include \
		-I$(top_srcdir)/src/sdk/wxpropgrid/include \
		-I$(top_srcdir)/src/plugins/contrib/wxSmith \
		-I$(srcdir)/wxAuiManager \
		-I$(srcdir)/wxAuiNotebook \
		-I$(srcdir)/wxAuiToolBar

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libwxSmithAui.la

libwxSmithAui_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libwxSmithAui_la_LIBADD = $(PLUGIN_WX_LIBS) ../../../sdk/libcodeblocks.la \
		       ../wxSmith/libwxsmithlib.la

EXTRA_DIST = $(srcdir)/images/*.xpm \
		$(srcdir)/images/*.xcf \
		$(srcdir)/wxsmith/*.wxs \
		$(srcdir)/*.cbp \
		manifest.xml

pkgdata_DATA = wxSmithAui.zip

CLEANFILES = $(pkgdata_DATA)

wxSmithAui.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/wxSmithAui.zip manifest.xml > /dev/null

libwxSmithAui_la_SOURCES = \
./wxAuiManager/wxsAuiManager.cpp \
./wxAuiManager/wxsAuiManagerParentQP.cpp \
./wxAuiManager/wxsAuiPaneInfoProperty.cpp \
./wxAuiNotebook/wxsAuiNotebook.cpp \
./wxAuiNotebook/wxSmithAuiNotebook.cpp \
./wxAuiToolBar/wxsAuiToolBar.cpp \
./wxAuiToolBar/wxsAuiToolBarItem.cpp \
./wxAuiToolBar/wxsAuiToolBarItemBase.cpp \
./wxAuiToolBar/wxsAuiToolBarLabel.cpp \
./wxAuiToolBar/wxsAuiToolBarSeparator.cpp \
./wxAuiToolBar/wxsAuiToolBarSpacer.cpp \
./wxAuiToolBar/wxSmithAuiToolBar.cpp \
./wxSmithAui.cpp

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = wxsmithaui.pc

noinst_HEADERS = \
./wxAuiManager/wxsAuiManager.h \
./wxAuiManager/wxsAuiManagerParentQP.h \
./wxAuiManager/wxsAuiPaneInfoProperty.h \
./wxAuiManager/wxSmithAuiManager.h \
./wxAuiNotebook/wxsAuiNotebook.h \
./wxAuiNotebook/wxSmithAuiNotebook.h \
./wxAuiToolBar/wxsAuiToolBar.h \
./wxAuiToolBar/wxsAuiToolBarItem.h \
./wxAuiToolBar/wxsAuiToolBarItemBase.h \
./wxAuiToolBar/wxsAuiToolBarLabel.h \
./wxAuiToolBar/wxsAuiToolBarSeparator.h \
./wxAuiToolBar/wxsAuiToolBarSpacer.h \
./wxAuiToolBar/wxSmithAuiToolBar.h \
./wxSmithAui.h

SUBDIRS = defitems icons properties

AM_CPPFLAGS = -DWXMAKINGLIB_PROPGRID -DwxPG_COMPATIBILITY_1_0_0

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxFlatNotebook/include \
		-I$(top_srcdir)/src/include/wxscintilla/include \
		-I$(top_srcdir)/src/include/wxFlatNotebook \
		-I$(top_srcdir)/src/include/propgrid/include \
		-I$(top_srcdir)/src/include/scripting/include \
		-I$(top_srcdir)/src/include/scripting/squirrel \
		-I$(top_srcdir)/src/include/scripting/sqplus \
		-I$(top_srcdir)/src/include/scripting/bindings

noinst_LTLIBRARIES = libwxsmith_wxwidgets.la

libwxsmith_wxwidgets_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libwxsmith_wxwidgets_la_LIBADD = ../../../../sdk/libcodeblocks.la \
				    properties/libwxsmith_wxwidgets_properties.la \
				    defitems/libwxsmith_wxwidgets_defitems.la

libwxsmith_wxwidgets_la_SOURCES = \
./wxssizer.cpp \
./wxsitemeditordragassist.cpp \
./wxsitemeditorcontent.cpp \
./wxscorrector.cpp \
./wxsnewwindowdlg.cpp \
./wxsitemres.cpp \
./wxspanelres.cpp \
./wxwidgetsguiconfigpanel.cpp \
./wxsevents.cpp \
./wxspredefinedids.cpp \
./wxwidgetsres.cpp \
./wxwidgetsguiappadoptingdlg.cpp \
./wxsitemundobuffer.cpp \
./wxsitemrestreedata.cpp \
./wxscontainer.cpp \
./wxsdrawingwindow.cpp \
./wxsitemfactory.cpp \
./wxsbaseproperties.cpp \
./wxsitem.cpp \
./wxscustomwidgetxmlhandler.cpp \
./wxsframeres.cpp \
./wxsstyle.cpp \
./wxseventseditor.cpp \
./wxswidget.cpp \
./wxwidgetsresfactory.cpp \
./wxsparent.cpp \
./wxsitemresdataobject.cpp \
./wxsitemresdata.cpp \
./wxsitemeditor.cpp \
./wxsdialogres.cpp \
./wxwidgetsgui.cpp \
./wxssizerparentqp.cpp \
./wxstool.cpp \
./wxstoolspace.cpp \
./wxsgridpanel.cpp \
./wxsdeleteitemres.cpp \
./wxscodercontext.cpp \
./wxscodegenerator.cpp

noinst_HEADERS = \
./wxsitemresdata.h \
./wxspredefinedids.h \
./wxsdrawingwindow.h \
./wxsitemeditorcontent.h \
./wxsbaseproperties.h \
./wxwidgetsgui.h \
./wxsexproperties.h \
./wxssizer.h \
./wxsitemres.h \
./wxwidgetsres.h \
./wxscustomwidgetxmlhandler.h \
./wxswidget.h \
./wxscorrector.h \
./wxsnewwindowdlg.h \
./wxsiteminfo.h \
./wxseventseditor.h \
./wxsitemeditordragassist.h \
./wxwidgetsresfactory.h \
./wxsparent.h \
./wxsitemresdataobject.h \
./wxwidgetsguiconfigpanel.h \
./wxsevents.h \
./wxsitemeditor.h \
./wxwidgetsguiappadoptingdlg.h \
./wxsdialogres.h \
./wxsitemundobuffer.h \
./wxsitemrestreedata.h \
./wxscontainer.h \
./wxsitemfactory.h \
./wxsitem.h \
./wxsstyle.h \
./wxsframeres.h \
./wxspanelres.h \
./wxssizerparentqp.h \
./wxstool.h \
./wxstoolspace.h \
./wxsgridpanel.h \
./wxsdeleteitemres.h \
./wxscodercontext.h \
./wxscodegenerator.h \
./wxsflags.h

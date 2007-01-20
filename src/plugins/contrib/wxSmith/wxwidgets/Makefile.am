SUBDIRS = defitems icons properties

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxFlatNotebook/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include \
		-I$(top_srcdir)/src/sdk/wxFlatNotebook \
		-I$(top_srcdir)/src/sdk/propgrid/include

noinst_LTLIBRARIES = libwxsmith_wxwidgets.la

libwxsmith_wxwidgets_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libwxsmith_wxwidgets_la_LIBADD = ../../../../sdk/libcodeblocks.la \
				    properties/libwxsmith_wxwidgets_properties.la \
				    defwidgets/libwxsmith_wxwidgets_defitems.la

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
./wxssizerparentqp.cpp

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
./wxssizerparentqp.h


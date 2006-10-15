SUBDIRS = defwidgets properties resources smithicons

if PRECOMPILE_HEADERS
BUILT_SOURCES = $(srcdir)/wxsheaders.h.gch
else
BUILT_SOURCES = 
endif

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/sdk \
		-I$(top_srcdir)/src/sdk/wxFlatNotebook/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include \
		-I$(top_srcdir)/src/sdk/propgrid/include

libdir = $(pkgdatadir)/plugins

lib_LTLIBRARIES = libwxsmith.la

libwxsmith_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libwxsmith_la_LIBADD = ../../../sdk/libcodeblocks.la \
				defwidgets/libwxsmith_defwidgets.la \
				properties/libwxsmith_properties.la \
				resources/libwxsmith_resources.la

libwxsmith_la_SOURCES = ./widget.cpp \
./wxscodegen.cpp \
./wxscoder.cpp \
./wxscontainer.cpp \
./wxsdefevthandler.cpp \
./wxsdefsizer.cpp \
./wxsdefwidget.cpp \
./wxsdragwindow.cpp \
./wxsdrawingwindow.cpp \
./wxseditor.cpp \
./wxsevent.cpp \
./wxseventseditor.cpp \
./wxsglobals.cpp \
./wxsmith.cpp \
./wxsnewwindowdlg.cpp \
./wxsproject.cpp \
./wxsproperties.cpp \
./wxspropertiesman.cpp \
./wxsresource.cpp \
./wxssplitterwindow.cpp \
./wxswidgetevents.cpp \
./wxswidgetfactory.cpp \
./wxswindoweditor.cpp \
./wxsimportxrcdlg.cpp \
./wxswinundobuffer.cpp \
./wxsresourcetree.cpp \
./wxssettingsdlg.cpp \
./wxssizerparentqp.cpp \
./wxsstandardqp.cpp \
./wxswidgetmanager.cpp \
./wxsbaseproperties.cpp \
./wxsprojectconfigurationdlg.cpp \
./wxspredefinedids.cpp \
./wxsmithmime.cpp \
./wxsextresmanager.cpp \
./wxswizard.cpp \
./wxsmithwizard.cpp \
./wxscustomwidgetxmlhandler.cpp

noinst_HEADERS = ./widget.h \
./wxscodegen.h \
./wxscoder.h \
./wxscontainer.h \
./wxsdefevthandler.h \
./wxsdefsizer.h \
./wxsdefwidget.h \
./wxsdragwindow.h \
./wxsdrawingwindow.h \
./wxseditor.h \
./wxsevent.h \
./wxseventseditor.h \
./wxsglobals.h \
./wxsmith.h \
./wxsnewwindowdlg.h \
./wxsproject.h \
./wxsproperties.h \
./wxspropertiesman.h \
./wxsresource.h \
./wxssplitterwindow.h \
./wxsstdwidget.h \
./wxsstyle.h \
./wxswidgetevents.h \
./wxswidgetfactory.h \
./wxswindoweditor.h \
./wxsstandardqp.h \
./wxsimportxrcdlg.h \
./wxswinundobuffer.h \
./wxsresourcetree.h \
./wxssettingsdlg.h \
./wxsheaders.h \
./wxssizerparentqp.h \
./wxsbaseproperties.h \
./wxswidgetmanager.h \
./wxsprojectconfigurationdlg.h \
./wxspredefinedids.h \
./wxsmithmime.h \
./wxsextresmanager.h \
./wxswizard.h \
./wxsmithwizard.h \
./wxscustomwidgetxmlhandler.h \
./wxscodeparams.h

EXTRA_DIST = *.cbp \
			*.xrc \
			*.bat \
			*.unix \
			manifest.xml

pkgdata_DATA = wxsmith.zip
CLEANFILES = $(pkgdata_DATA)

wxsmith.zip:
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/wxsmith.zip manifest.xml *.xrc > /dev/null

$(srcdir)/wxsheaders.h.gch: $(srcdir)/wxsheaders.h $(srcdir)/../../../sdk/sdk.h.gch
	-rm -rf $(srcdir)/wxsheaders.h.gch
	$(CXXCOMPILE) -o $(srcdir)/wxsheaders.h.gch -xc++-header $(srcdir)/wxsheaders.h

clean-local:
	-rm -rf $(srcdir)/wxsheaders.h.gch

SUBDIRS = tinyxml wxscintilla resources

INCLUDES = $(WX_CXXFLAGS) \
			-Iwxscintilla/include \
			-Itinyxml

AM_CXXFLAGS = -Wall -g

lib_LTLIBRARIES = libcodeblocks.la

libcodeblocks_la_LDFLAGS = 

libcodeblocks_la_LIBADD =  $(WX_LIBS) \
			tinyxml/libtinyxml.la \
			wxscintilla/libwxscintilla.la

libcodeblocks_la_SOURCES = \
Managers/Managers.cpp \
Managers/StdAfx.cpp \
autodetectcompilers.cpp \
cbeditor.cpp \
cbeditorprintout.cpp \
cbplugin.cpp \
cbproject.cpp \
cbthreadpool.cpp \
cbworkspace.cpp \
compileoptionsbase.cpp \
compiler.cpp \
compilerfactory.cpp \
compileroptions.cpp \
compiletargetbase.cpp \
configmanager.cpp \
configuretoolsdlg.cpp \
confirmreplacedlg.cpp \
customvars.cpp \
devcpploader.cpp \
editarrayfiledlg.cpp \
editarrayorderdlg.cpp \
editarraystringdlg.cpp \
editorbase.cpp \
editorcolorset.cpp \
editorconfigurationdlg.cpp \
editorlexerloader.cpp \
editormanager.cpp \
editpathdlg.cpp \
edittooldlg.cpp \
externaldepsdlg.cpp \
filegroupsandmasks.cpp \
finddlg.cpp \
globals.cpp \
importers_globals.cpp \
incrementalselectlistdlg.cpp \
macrosmanager.cpp \
managedthread.cpp \
manager.cpp \
menuitemsmanager.cpp \
messagelog.cpp \
messagemanager.cpp \
msvc7loader.cpp \
msvc7workspaceloader.cpp \
msvcloader.cpp \
msvcworkspacebase.cpp \
msvcworkspaceloader.cpp \
multiselectdlg.cpp \
newfromtemplatedlg.cpp \
personalitymanager.cpp \
pipedprocess.cpp \
pluginmanager.cpp \
pluginsconfigurationdlg.cpp \
printing_types.cpp \
projectbuildtarget.cpp \
projectfileoptionsdlg.cpp \
projectlayoutloader.cpp \
projectloader.cpp \
projectmanager.cpp \
projectoptionsdlg.cpp \
projectsfilemasksdlg.cpp \
projecttemplateloader.cpp \
replacedlg.cpp \
sdk_events.cpp \
searchresultslog.cpp \
selecttargetdlg.cpp \
simplelistlog.cpp \
simpletextlog.cpp \
templatemanager.cpp \
toolsmanager.cpp \
workspaceloader.cpp \
xtra_classes.cpp \
xtra_res.cpp

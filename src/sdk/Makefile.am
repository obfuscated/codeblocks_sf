SUBDIRS = tinyxml wxscintilla resources

INCLUDES = $(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/sdk/wxscintilla/include \
			-I$(top_srcdir)/src/sdk/tinyxml

lib_LTLIBRARIES = libcodeblocks.la

libcodeblocks_la_LDFLAGS = -version-info @CODEBLOCKS_VERSION_INFO@

libcodeblocks_la_LIBADD =  $(WX_LIBS) \
			tinyxml/libtinyxml.la \
			wxscintilla/libwxscintilla.la

libcodeblocks_la_SOURCES = Managers/Managers.cpp \
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

noinst_HEADERS = Managers/Managers.h \
Managers/StdAfx.h \
autodetectcompilers.h \
cbeditor.h \
cbeditorprintout.h \
cbexception.h \
cbplugin.h \
cbproject.h \
cbthreadpool.h \
cbworkspace.h \
compileoptionsbase.h \
compilerfactory.h \
compiler.h \
compileroptions.h \
compiletargetbase.h \
configmanager.h \
configuretoolsdlg.h \
confirmreplacedlg.h \
customvars.h \
devcpploader.h \
editarrayfiledlg.h \
editarrayorderdlg.h \
editarraystringdlg.h \
editorbase.h \
editorcolorset.h \
editorconfigurationdlg.h \
editorlexerloader.h \
editormanager.h \
editpathdlg.h \
edittooldlg.h \
externaldepsdlg.h \
filegroupsandmasks.h \
finddlg.h \
findreplacebase.h \
globals.h \
ibaseloader.h \
ibaseworkspaceloader.h \
importers_globals.h \
incrementalselectlistdlg.h \
licenses.h \
macrosmanager.h \
managedthread.h \
manager.h \
managerproxy.h \
menuitemsmanager.h \
messagelog.h \
messagemanager.h \
msvc7loader.h \
msvc7workspaceloader.h \
msvcloader.h \
msvcworkspacebase.h \
msvcworkspaceloader.h \
multiselectdlg.h \
newfromtemplatedlg.h \
openfilestree.h \
personalitymanager.h \
pipedprocess.h \
pluginmanager.h \
pluginsconfigurationdlg.h \
printing_types.h \
projectbuildtarget.h \
projectfileoptionsdlg.h \
projectlayoutloader.h \
projectloader.h \
projectmanager.h \
projectoptionsdlg.h \
projectsfilemasksdlg.h \
projecttemplateloader.h \
replacedlg.h \
sanitycheck.h \
sdk_events.h \
searchresultslog.h \
selecttargetdlg.h \
settings.h \
simplelistlog.h \
simpletextlog.h \
templatemanager.h \
toolsmanager.h \
workspaceloader.h \
xtra_classes.h \
xtra_res.h

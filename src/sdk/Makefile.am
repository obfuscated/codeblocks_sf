SUBDIRS = scripting wxscintilla wxFlatNotebook resources

INCLUDES = $(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/include/wxscintilla/include \
			-I$(top_srcdir)/src/include/tinyxml \
			-I$(top_srcdir)/src/include/scripting/include \
			-I$(top_srcdir)/src/include/scripting/sqplus \
			-I$(top_srcdir)/src/include/wxFlatNotebook/include

lib_LTLIBRARIES = libcodeblocks.la

libcodeblocks_la_LDFLAGS = -version-info @CODEBLOCKS_VERSION_INFO@

libcodeblocks_la_LIBADD =  $(WX_LIBS) \
			scripting/bindings/libsqbindings.la \
			scripting/sqplus/libsqplus.la \
			scripting/sqstdlib/libsqstdlib.la \
			scripting/squirrel/libsquirrel.la \
			../base/tinyxml/libtinyxml.la \
			wxscintilla/libwxscintilla.la \
			wxFlatNotebook/libwxflatnotebook.la

libcodeblocks_la_SOURCES = annoyingdialog.cpp \
						autodetectcompilers.cpp \
						base64.cpp \
						blockallocated.cpp \
						cbexception.cpp \
						cbeditor.cpp \
						cbeditorprintout.cpp \
						cbplugin.cpp \
						cbproject.cpp \
						cbthreadpool.cpp \
						cbworkspace.cpp \
						compileoptionsbase.cpp \
						compiler.cpp \
						compilercommandgenerator.cpp \
						compilerfactory.cpp \
						compileroptions.cpp \
						compiletargetbase.cpp \
						configmanager.cpp \
						configmanager-revision.cpp \
						configurationpanel.cpp \
						configuretoolsdlg.cpp \
						confirmreplacedlg.cpp \
						crc32.cpp \
						editarrayfiledlg.cpp \
						editarrayorderdlg.cpp \
						editarraystringdlg.cpp \
						editkeywordsdlg.cpp \
						editorbase.cpp \
						editorcolourset.cpp \
						editorconfigurationdlg.cpp \
						editorlexerloader.cpp \
						editormanager.cpp \
						editor_hooks.cpp \
						editpairdlg.cpp \
						editpathdlg.cpp \
						edittooldlg.cpp \
						encodingdetector.cpp \
						externaldepsdlg.cpp \
						filefilters.cpp \
						filegroupsandmasks.cpp \
						filemanager.cpp \
						finddlg.cpp \
						genericmultilinenotesdlg.cpp \
						globals.cpp \
						importers_globals.cpp \
						incrementalselectlistdlg.cpp \
						infowindow.cpp \
						logmanager.cpp \
						macrosmanager.cpp \
						managedthread.cpp \
						manager.cpp \
						menuitemsmanager.cpp \
						messagelog.cpp \
						messagemanager.cpp \
						multiselectdlg.cpp \
						newfromtemplatedlg.cpp \
						personalitymanager.cpp \
						pipedprocess.cpp \
						pluginmanager.cpp \
						pluginsconfigurationdlg.cpp \
						printing_types.cpp \
						projectbuildtarget.cpp \
						projectdepsdlg.cpp \
						projectfile.cpp \
						projectfileoptionsdlg.cpp \
						projectlayoutloader.cpp \
						projectloader.cpp \
						projectloader_hooks.cpp \
						projectmanager.cpp \
						projectoptionsdlg.cpp \
						projectsfilemasksdlg.cpp \
						projecttemplateloader.cpp \
						replacedlg.cpp \
						scriptingmanager.cpp \
						scriptsecuritywarningdlg.cpp \
						sdk_events.cpp \
						searchresultslog.cpp \
						selecttargetdlg.cpp \
						simplelistlog.cpp \
						simpletextlog.cpp \
						templatemanager.cpp \
						toolsmanager.cpp \
						uservarmanager.cpp \
						virtualbuildtargetsdlg.cpp \
						workspaceloader.cpp \
						xtra_res.cpp  \
						nullptr.cpp

SUBDIRS = scripting tinyxml wxscintilla resources propgrid wxFlatNotebook

if PRECOMPILE_HEADERS
BUILT_SOURCES = $(srcdir)/sdk.h.gch
else
BUILT_SOURCES = 
endif

sdkdir=$(includedir)/codeblocks

INCLUDES = $(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/sdk/wxscintilla/include \
			-I$(top_srcdir)/src/sdk/tinyxml \
			-I$(top_srcdir)/src/sdk/scripting/include \
			-I$(top_srcdir)/src/sdk/scripting/sqplus \
			-I$(top_srcdir)/src/sdk/wxFlatNotebook \
			-I$(top_srcdir)/src/sdk/propgrid/include

lib_LTLIBRARIES = libcodeblocks.la

libcodeblocks_la_LDFLAGS = -version-info @CODEBLOCKS_VERSION_INFO@

libcodeblocks_la_LIBADD =  $(WX_LIBS) \
			scripting/bindings/libsqbindings.la \
			scripting/sqplus/libsqplus.la \
			scripting/sqstdlib/libsqstdlib.la \
			scripting/squirrel/libsquirrel.la \
			tinyxml/libtinyxml.la \
			wxscintilla/libwxscintilla.la \
			wxFlatNotebook/libwxflatnotebook.la \
			propgrid/libpropgrid.la

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
						devcpploader.cpp \
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
						globals.cpp \
						importers_globals.cpp \
						incrementalselectlistdlg.cpp \
						infowindow.cpp \
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
						xtra_classes.cpp \
						xtra_res.cpp

ALLHEADERS = annoyingdialog.h \
				autodetectcompilers.h \
				backgroundthread.h \
				base64.h \
				blockallocated.h \
				cbeditor.h \
				cbeditorprintout.h \
				cbexception.h \
				cbplugin.h \
				cbproject.h \
				cbthreadedtask.h \
				cbthreadpool.h \
				cbthreadpool_extras.h \
				cbtool.h \
				cbworkspace.h \
				compileoptionsbase.h \
				compiler.h \
				compilercommandgenerator.h \
				compilerfactory.h \
				compileroptions.h \
				compiletargetbase.h \
				configmanager.h \
				configurationpanel.h \
				configuretoolsdlg.h \
				confirmreplacedlg.h \
				crc32.h \
				devcpploader.h \
				editarrayfiledlg.h \
				editarrayorderdlg.h \
				editarraystringdlg.h \
				editkeywordsdlg.h \
				editorbase.h \
				editorcolourset.h \
				editorconfigurationdlg.h \
				editorlexerloader.h \
				editormanager.h \
				editor_hooks.h \
				editpairdlg.h \
				editpathdlg.h \
				edittooldlg.h \
				encodingdetector.h \
				externaldepsdlg.h \
				filefilters.h \
				filegroupsandmasks.h \
				filemanager.h \
				finddlg.h \
				findreplacebase.h \
				globals.h \
				ibaseloader.h \
				ibaseworkspaceloader.h \
				id.h \
				importers_globals.h \
				incrementalselectlistdlg.h \
				infowindow.h \
				licenses.h \
				macrosmanager.h \
				managedthread.h \
				manager.h \
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
				projectdepsdlg.h \
				projectfile.h \
				projectfileoptionsdlg.h \
				projectlayoutloader.h \
				projectloader.h \
				projectloader_hooks.h \
				projectmanager.h \
				projectoptionsdlg.h \
				projectsfilemasksdlg.h \
				projecttemplateloader.h \
				replacedlg.h \
				safedelete.h \
				scriptingmanager.h \
				sdk.h \
				sdk_common.h \
				sdk_precomp.h \
				sdk_events.h \
				searchresultslog.h \
				selecttargetdlg.h \
				settings.h \
				simplelistlog.h \
				simpletextlog.h \
				templatemanager.h \
				toolsmanager.h \
				uservarmanager.h \
				virtualbuildtargetsdlg.h \
				workspaceloader.h \
				xtra_classes.h \
				xtra_res.h

sdk_HEADERS = $(ALLHEADERS)

$(srcdir)/sdk.h.gch: $(ALLHEADERS)
	-rm -f $(srcdir)/sdk.h.gch
	$(CXXCOMPILE) -o $(srcdir)/sdk.h.gch -xc++-header $(srcdir)/sdk.h

clean-local:
	-rm -f $(srcdir)/sdk.h.gch

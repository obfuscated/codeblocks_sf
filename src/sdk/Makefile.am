SUBDIRS = as tinyxml wxscintilla resources propgrid wxFlatNotebook

if PRECOMPILE_HEADERS
BUILT_SOURCES = $(srcdir)/sdk.h.gch
else
BUILT_SOURCES = 
endif

INCLUDES = $(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/sdk/wxscintilla/include \
			-I$(top_srcdir)/src/sdk/tinyxml \
			-I$(top_srcdir)/src/sdk/as/include \
			-I$(top_srcdir)/src/sdk/wxFlatNotebook \
			-I$(top_srcdir)/src/sdk/propgrid/include

lib_LTLIBRARIES = libcodeblocks.la

libcodeblocks_la_LDFLAGS = -version-info @CODEBLOCKS_VERSION_INFO@

libcodeblocks_la_LIBADD =  $(WX_LIBS) \
			as/libangelscript.la \
			tinyxml/libtinyxml.la \
			wxscintilla/libwxscintilla.la \
			wxFlatNotebook/libwxflatnotebook.la \
			propgrid/libpropgrid.la

libcodeblocks_la_SOURCES = annoyingdialog.cpp \
						autodetectcompilers.cpp \
						base64.cpp \
						cbexception.cpp \
						blockallocated.cpp \
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
						editorcolorset.cpp \
						editorconfigurationdlg.cpp \
						editorlexerloader.cpp \
						editormanager.cpp \
						editpairdlg.cpp \
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
						projectdepsdlg.cpp \
						projectfile.cpp \
						projectfileoptionsdlg.cpp \
						projectlayoutloader.cpp \
						projectloader.cpp \
						projectmanager.cpp \
						projectoptionsdlg.cpp \
						projectsfilemasksdlg.cpp \
						projecttemplateloader.cpp \
						replacedlg.cpp \
						scriptingmanager.cpp \
						sdk_events.cpp \
						searchresultslog.cpp \
						selecttargetdlg.cpp \
						settings.cpp \
						simplelistlog.cpp \
						simpletextlog.cpp \
						templatemanager.cpp \
						toolsmanager.cpp \
						uservarmanager.cpp \
						workspaceloader.cpp \
						xtra_classes.cpp \
						xtra_res.cpp

noinst_HEADERS = annoyingdialog.h \
				autodetectcompilers.h \
				base64.h \
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
				editorcolorset.h \
				editorconfigurationdlg.h \
				editorlexerloader.h \
				editormanager.h \
				editpairdlg.h \
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
				projectmanager.h \
				projectoptionsdlg.h \
				projectsfilemasksdlg.h \
				projecttemplateloader.h \
				replacedlg.h \
				sanitycheck.h \
				scriptingmanager.h \
				sdk_events.h \
				searchresultslog.h \
				selecttargetdlg.h \
				settings.h \
				simplelistlog.h \
				simpletextlog.h \
				templatemanager.h \
				toolsmanager.h \
				uservarmanager.h \
				workspaceloader.h \
				xtra_classes.h \
				xtra_res.h

$(srcdir)/sdk.h.gch: $(srcdir)/sdk.h
	-rm -f $(srcdir)/sdk.h.gch
	$(CXXCOMPILE) -o $(srcdir)/sdk.h.gch -xc++-header $(srcdir)/sdk.h

clean-local:
	-rm -f $(srcdir)/sdk.h.gch

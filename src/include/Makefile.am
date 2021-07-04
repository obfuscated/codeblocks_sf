SUBDIRS = scripting
if ! HAVE_TINYXML
SUBDIRS += tinyxml
endif
DIST_SUBDIRS = scripting tinyxml

if PRECOMPILE_HEADERS
BUILT_SOURCES = sdk.h.gch \
		sdk_precomp.h.gch
else
BUILT_SOURCES =
endif

AM_CPPFLAGS = $(WX_CXXFLAGS) \
			-I$(top_builddir)/src/include \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/sdk/wxscintilla/include

# Because we are not generating any programs/libraries with this makefile,
# $(CXXCOMPILE) is empty.
# Create the basic one here:
CXXCOMPILE = $(CXX) $(DEFS) $(DEFAULT_INCLUDES) \
        $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)

sdkdir=$(includedir)/codeblocks

ALLHEADERS = annoyingdialog.h \
				autodetectcompilers.h \
				backgroundthread.h \
				base64.h \
				blockallocated.h \
				cbart_provider.h \
				cbauibook.h \
				cbcolourmanager.h \
				cbdebugger_interfaces.h \
				cbeditor.h \
				cbeditorprintout.h \
				cbexception.h \
				cbfunctor.h \
				cbplugin.h \
				cbproject.h \
				cbstatusbar.h \
				cbstyledtextctrl.h \
				cbthreadedtask.h \
				cbthreadpool.h \
				cbtool.h \
				cbtreectrl.h \
				cbworkspace.h \
				ccmanager.h \
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
				cygwin.h \
				debuggermanager.h \
				editarrayfiledlg.h \
				editarrayorderdlg.h \
				editarraystringdlg.h \
				editorbase.h \
				editorcolourset.h \
				editorlexerloader.h \
				editormanager.h \
				editor_hooks.h \
				editor_utils.h \
				editpairdlg.h \
				editpathdlg.h \
				edittooldlg.h \
				encodingdetector.h \
				externaldepsdlg.h \
				filefilters.h \
				filegroupsandmasks.h \
				filemanager.h \
				findreplacedlg.h \
				findreplacebase.h \
				genericmultilinenotesdlg.h \
				globals.h \
				ibaseloader.h \
				ibaseworkspaceloader.h \
				id.h \
				importers_globals.h \
				incremental_select_helper.h \
				infowindow.h \
				licenses.h \
				logger.h \
				loggers.h \
				logmanager.h \
				macrosmanager.h \
				manager.h \
				menuitemsmanager.h \
				misctreeitemdata.h \
				multiselectdlg.h \
				newfromtemplatedlg.h \
				personalitymanager.h \
				pipedprocess.h \
				pluginmanager.h \
				pluginsconfigurationdlg.h \
				prep.h \
				printing_types.h \
				projectbuildtarget.h \
				projectfile.h \
				projectfileoptionsdlg.h \
				projectlayoutloader.h \
				projectloader.h \
				projectloader_hooks.h \
				projectmanager.h \
				projectsfilemasksdlg.h \
				projecttemplateloader.h \
				safedelete.h \
				scriptingmanager.h \
				scriptsecuritywarningdlg.h \
				scrollingdialog.h \
				sdk.h \
				sdk_common.h \
				sdk_precomp.h \
				sdk_events.h \
				searchresultslog.h \
				selecttargetdlg.h \
				settings.h \
				templatemanager.h \
				tinywxuni.h \
				toolsmanager.h \
				uservarmanager.h \
				workspaceloader.h \
				wxstringhash.h \
				xtra_res.h

sdk_HEADERS = $(ALLHEADERS)

sdk.h.gch: sdk_precomp.h.gch $(ALLHEADERS)
	-rm -f sdk.h.gch
	$(CXXCOMPILE) -o sdk.h.gch -xc++-header $(srcdir)/sdk.h

sdk_precomp.h.gch: $(ALLHEADERS)
	-rm -f sdk_precomp.h.gch
	$(CXXCOMPILE) -o sdk_precomp.h.gch -xc++-header $(srcdir)/sdk_precomp.h

clean-local:
	-rm -f sdk.h.gch
	-rm -f sdk_precomp.h.gch

EXTRA_DIST = autorevision.h


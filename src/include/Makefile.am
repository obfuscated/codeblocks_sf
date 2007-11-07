SUBDIRS = scripting tinyxml wxscintilla wxFlatNotebook

if PRECOMPILE_HEADERS
BUILT_SOURCES = $(top_srcdir)/src/include/sdk_precomp.h.gch
else
BUILT_SOURCES = 
endif

INCLUDES = $(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/include/wxscintilla/include \
			-I$(top_srcdir)/src/include/tinyxml \
			-I$(top_srcdir)/src/include/scripting/include \
			-I$(top_srcdir)/src/include/scripting/sqplus \
			-I$(top_srcdir)/src/include/wxFlatNotebook/include

# Because we are not generating any programs/libraries with this makefile,
# $(CXXCOMPILE) is empty.
# Create the basic one here:
CXXCOMPILE = $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
        $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)

sdkdir=$(includedir)/codeblocks

ALLHEADERS = annoyingdialog.h \
				autodetectcompilers.h \
				backgroundthread.h \
				base64.h \
				blockallocated.h \
				cbeditor.h \
				cbeditorprintout.h \
				cbexception.h \
				cbfunctor.h \
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
				genericmultilinenotesdlg.h \
				globals.h \
				ibaseloader.h \
				ibaseworkspaceloader.h \
				id.h \
				importers_globals.h \
				incrementalselectlistdlg.h \
				infowindow.h \
				licenses.h \
				logger.h \
				logmanager.h \
				macrosmanager.h \
				managedthread.h \
				manager.h \
				menuitemsmanager.h \
				messagelog.h \
				messagemanager.h \
				multiselectdlg.h \
				newfromtemplatedlg.h \
				openfilestree.h \
				personalitymanager.h \
				pipedprocess.h \
				pluginmanager.h \
				pluginsconfigurationdlg.h \
				prep.h \
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
				scriptsecuritywarningdlg.h \
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
				xtra_res.h

sdk_HEADERS = $(ALLHEADERS)

$(top_srcdir)/src/include/sdk_precomp.h.gch: $(ALLHEADERS)
	-rm -f $(top_srcdir)/src/include/sdk_precomp.h.gch
	$(CXXCOMPILE) -o $(top_srcdir)/src/include/sdk_precomp.h.gch -xc++-header $(srcdir)/sdk_precomp.h

clean-local:
	-rm -f $(top_srcdir)/src/include/sdk_precomp.h.gch

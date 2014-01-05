SUBDIRS = scripting tinyxml

if PRECOMPILE_HEADERS
BUILT_SOURCES = $(top_builddir)/src/include/sdk.h.gch \
		$(top_builddir)/src/include/sdk_precomp.h.gch
else
BUILT_SOURCES =
endif

AM_CPPFLAGS = $(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/sdk/wxscintilla/include \
			-I$(top_srcdir)/src/include/tinyxml \
			-I$(top_srcdir)/src/include/scripting/include \
 			-I$(top_srcdir)/src/include/scripting/sqplus \
			-I$(top_srcdir)/src/include/mozilla_chardet \
			-I$(top_srcdir)/src/include/mozilla_chardet/mfbt \
			-I$(top_srcdir)/src/include/mozilla_chardet/nsprpub/pr/include \
			-I$(top_srcdir)/src/include/mozilla_chardet/xpcom \
			-I$(top_srcdir)/src/include/mozilla_chardet/xpcom/base \
			-I$(top_srcdir)/src/include/mozilla_chardet/xpcom/glue

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
				debuggermanager.h \
				editarrayfiledlg.h \
				editarrayorderdlg.h \
				editarraystringdlg.h \
				editorbase.h \
				editorcolourset.h \
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
				findreplacedlg.h \
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
				loggers.h \
				logmanager.h \
				macrosmanager.h \
				managedthread.h \
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
				toolsmanager.h \
				uservarmanager.h \
				workspaceloader.h \
				xtra_res.h \
				mozilla_chardet/CharDistribution.h \
				mozilla_chardet/JpCntx.h \
				mozilla_chardet/nsBig5Prober.h \
				mozilla_chardet/nsCharSetProber.h \
				mozilla_chardet/nsCodingStateMachine.h \
				mozilla_chardet/nsEscCharsetProber.h \
				mozilla_chardet/nsEUCJPProber.h \
				mozilla_chardet/nsEUCKRProber.h \
				mozilla_chardet/nsEUCTWProber.h \
				mozilla_chardet/nsGB2312Prober.h \
				mozilla_chardet/nsHebrewProber.h \
				mozilla_chardet/nsLatin1Prober.h \
				mozilla_chardet/nsMBCSGroupProber.h \
				mozilla_chardet/nsPkgInt.h \
				mozilla_chardet/nsSBCharSetProber.h \
				mozilla_chardet/nsSBCSGroupProber.h \
				mozilla_chardet/nsSJISProber.h \
				mozilla_chardet/nsUniversalDetector.h \
				mozilla_chardet/nsUTF8Prober.h \
				mozilla_chardet/mfbt/mozilla/Assertions.h \
				mozilla_chardet/mfbt/mozilla/Attributes.h \
				mozilla_chardet/mfbt/mozilla/Compiler.h \
				mozilla_chardet/mfbt/mozilla/Likely.h \
				mozilla_chardet/mfbt/mozilla/NullPtr.h \
				mozilla_chardet/mfbt/mozilla/StandardInteger.h \
				mozilla_chardet/mfbt/mozilla/Types.h \
				mozilla_chardet/mfbt/mozilla/Util.h \
				mozilla_chardet/nsprpub/pr/include/prcpucfg.h \
				mozilla_chardet/nsprpub/pr/include/prcpucfg_darwin.h \
				mozilla_chardet/nsprpub/pr/include/prcpucfg_linux.h \
				mozilla_chardet/nsprpub/pr/include/prcpucfg_openbsd.h \
				mozilla_chardet/nsprpub/pr/include/prcpucfg_win95.h \
				mozilla_chardet/nsprpub/pr/include/prcpucfg_winnt.h \
				mozilla_chardet/nsprpub/pr/include/prmem.h \
				mozilla_chardet/nsprpub/pr/include/prtypes.h \
				mozilla_chardet/xpcom/base/ErrorList.h \
				mozilla_chardet/xpcom/base/nscore.h \
				mozilla_chardet/xpcom/base/nsError.h \
				mozilla_chardet/xpcom/glue/nsDebug.h \
				mozilla_chardet/xpcom/glue/nsMemory.h


sdk_HEADERS = $(ALLHEADERS)

$(top_builddir)/src/include/sdk.h.gch: $(top_builddir)/src/include/sdk_precomp.h.gch $(ALLHEADERS)
	-rm -f $(top_builddir)/src/include/sdk.h.gch
	$(CXXCOMPILE) -o $(top_builddir)/src/include/sdk.h.gch -xc++-header $(srcdir)/sdk.h

$(top_builddir)/src/include/sdk_precomp.h.gch: $(ALLHEADERS)
	-rm -f $(top_builddir)/src/include/sdk_precomp.h.gch
	$(CXXCOMPILE) -o $(top_builddir)/src/include/sdk_precomp.h.gch -xc++-header $(srcdir)/sdk_precomp.h

clean-local:
	-rm -f $(top_builddir)/src/include/sdk.h.gch
	-rm -f $(top_builddir)/src/include/sdk_precomp.h.gch

EXTRA_DIST = autorevision.h

SUBDIRS = scripting wxscintilla $(WXPROPGRID_DIR) resources
DIST_SUBDIRS = scripting wxscintilla  wxpropgrid resources

AM_CPPFLAGS = $(WX_CXXFLAGS) \
			$(CB_GLIB2_CFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/sdk/wxscintilla/include \
			 $(WXPROPGRID_INCLUDE) \
			-I$(top_srcdir)/src/include/tinyxml \
			-I$(top_srcdir)/src/include/scripting/include \
			-I$(top_srcdir)/src/include/scripting/bindings \
 			-I$(top_srcdir)/src/include/scripting/sqplus \
 			-I$(top_srcdir)/src/include/mozilla_chardet \
 			-I$(top_srcdir)/src/include/mozilla_chardet/mfbt \
 			-I$(top_srcdir)/src/include/mozilla_chardet/nsprpub/pr/include \
 			-I$(top_srcdir)/src/include/mozilla_chardet/xpcom \
 			-I$(top_srcdir)/src/include/mozilla_chardet/xpcom/base \
 			-I$(top_srcdir)/src/include/mozilla_chardet/xpcom/glue

lib_LTLIBRARIES = libcodeblocks.la

libcodeblocks_la_LDFLAGS = -version-info @CODEBLOCKS_VERSION_INFO@

libcodeblocks_la_LIBADD = scripting/bindings/libsqbindings.la \
			scripting/sqplus/libsqplus.la \
			scripting/sqstdlib/libsqstdlib.la \
			scripting/squirrel/libsquirrel.la \
			../base/tinyxml/libtinyxml.la \
			wxscintilla/libwxscintilla.la \
			$(WX_LIBS) \
			$(CB_GLIB2_LIBS) \
			$(WXPROPGRID_LIB)

libcodeblocks_la_SOURCES = annoyingdialog.cpp \
						autodetectcompilers.cpp \
						base64.cpp \
						blockallocated.cpp \
						cbauibook.cpp \
						cbcolourmanager.cpp \
						cbdebugger_interfaces.cpp \
						cbeditor.cpp \
						cbeditorprintout.cpp \
						cbexception.cpp \
						cbplugin.cpp \
						cbproject.cpp \
						cbstatusbar.cpp \
						cbstyledtextctrl.cpp \
						cbthreadpool.cpp \
						cbtreectrl.cpp \
						cbworkspace.cpp \
						ccmanager.cpp \
						compileoptionsbase.cpp \
						compiler.cpp \
						compilercommandgenerator.cpp \
						compilerfactory.cpp \
						compileroptions.cpp \
						compiletargetbase.cpp \
						configmanager-revision.cpp \
						configmanager.cpp \
						configurationpanel.cpp \
						configuretoolsdlg.cpp \
						confirmreplacedlg.cpp \
						crc32.cpp \
						debuggermanager.cpp \
						editarrayfiledlg.cpp \
						editarrayorderdlg.cpp \
						editarraystringdlg.cpp \
						editor_hooks.cpp \
						editorbase.cpp \
						editorcolourset.cpp \
						editorlexerloader.cpp \
						editormanager.cpp \
						editpairdlg.cpp \
						editpathdlg.cpp \
						edittooldlg.cpp \
						encodingdetector.cpp \
						externaldepsdlg.cpp \
						filefilters.cpp \
						filegroupsandmasks.cpp \
						filemanager.cpp \
						findreplacedlg.cpp \
						genericmultilinenotesdlg.cpp \
						globals.cpp \
						importers_globals.cpp \
						incrementalselectlistdlg.cpp \
						infowindow.cpp \
						loggers.cpp \
						logmanager.cpp \
						macrosmanager.cpp \
						manager.cpp \
						menuitemsmanager.cpp \
						multiselectdlg.cpp \
						newfromtemplatedlg.cpp \
						personalitymanager.cpp \
						pipedprocess.cpp \
						pluginmanager.cpp \
						pluginsconfigurationdlg.cpp \
						printing_types.cpp \
						projectbuildtarget.cpp \
						projectfile.cpp \
						projectfileoptionsdlg.cpp \
						projectlayoutloader.cpp \
						projectloader.cpp \
						projectloader_hooks.cpp \
						projectmanager.cpp \
						projectsfilemasksdlg.cpp \
						projecttemplateloader.cpp \
						scriptingmanager.cpp \
						scriptsecuritywarningdlg.cpp \
						scrollingdialog.cpp \
						sdk_events.cpp \
						searchresultslog.cpp \
						selecttargetdlg.cpp \
						templatemanager.cpp \
						toolsmanager.cpp \
						uservarmanager.cpp \
						workspaceloader.cpp \
						xtra_res.cpp  \
						mozilla_chardet/src/CharDistribution.cpp \
						mozilla_chardet/src/JpCntx.cpp \
						mozilla_chardet/src/nsCharSetProber.cpp \
						mozilla_chardet/src/nsEscCharsetProber.cpp \
						mozilla_chardet/src/nsEscSM.cpp \
						mozilla_chardet/src/nsEUCJPProber.cpp \
						mozilla_chardet/src/nsLatin1Prober.cpp \
						mozilla_chardet/src/nsMBCSGroupProber.cpp \
						mozilla_chardet/src/nsMBCSSM.cpp \
						mozilla_chardet/src/nsSJISProber.cpp \
						mozilla_chardet/src/nsUniversalDetector.cpp \
						mozilla_chardet/src/nsUTF8Prober.cpp

EXTRA_DIST = mozilla_chardet/src/JISFreq.tab

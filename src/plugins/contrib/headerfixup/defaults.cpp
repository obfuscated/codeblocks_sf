/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "bindings.h"
#include "globals.h"

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void Bindings::SetDefaults()
{
  SetDefaultsCodeBlocks();
  SetDefaultsWxWidgets();
  SetDefaultsSTL();
  SetDefaultsCLibrary();
}

void Bindings::SetDefaultsCodeBlocks()
{
    wxString strCodeBlocks = _T(
    "AbstractJob;backgroundthread.h|"
    "AddBuildTarget;projectbuildtarget.h|"
    "AddFile;projectfile.h|"
    "Agony;backgroundthread.h|"
    "AnnoyingDialog;annoyingdialog.h|"
    "AppendArray;globals.h|"
    "AutoDetectCompilers;autodetectcompilers.h|"
    "BackgroundThread;backgroundthread.h|"
    "BackgroundThreadPool;backgroundthread.h|"
    "BlkAllc;blockallocated.h|"
    "BlockAllocated;blockallocated.h|"
    "BlockAllocator;blockallocated.h|"
    "cbAssert;cbexception.h|"
    "cbC2U;globals.h|"
    "cbCodeCompletionPlugin;cbplugin.h|"
    "cbCompilerPlugin;cbplugin.h|"
    "cbConfigurationDialog;configurationpanel.h|"
    "cbConfigurationPanel;configurationpanel.h|"
    "cbDebuggerPlugin;cbplugin.h|"
    "cbDirAccessCheck;globals.h|"
    "cbEditor;cbeditor.h|"
    "cbEditorPrintout;cbeditorprintout.h|"
    "cbEventFunctor;cbfunctor.h|"
    "cbException;cbexception.h|"
    "cbExecuteProcess;cbexecute.h|"
    "cbLoadBitmap;globals.h|"
    "cbMessageBox;globals.h|"
    "cbMimePlugin;cbplugin.h|"
    "cbPlugin;cbplugin.h|"
    "cbProject;cbproject.h|"
    "cbRead;globals.h|"
    "cbReadFileContents;globals.h|"
    "cbSaveTinyXMLDocument;globals.h|"
    "cbSaveToFile;globals.h|"
    "cbStyledTextCtrl;cbeditor.h|"
    "cbSyncExecute;cbexecute.h|"
    "cbThreadedTask;cbthreadtask.h|"
    "cbThreadPool;cbthreadpool.h|"
    "cbThrow;cbexception.h|"
    "cbTool;cbtool.h|"
    "cbToolPlugin;cbplugin.h|"
    "cbU2C;globals.h|"
    "cbWizardPlugin;cbplugin.h|"
    "cbWorkerThread;cbthreadpool_extras.h|"
    "cbWorkspace;cbworkspace.h|"
    "cbWrite;globals.h|"
    "CfgMgrBldr;configmanager.h|"
    "cgCompiler;cbplugin.h|"
    "cgContribPlugin;cbplugin.h|"
    "cgCorePlugin;cbplugin.h|"
    "cgEditor;cbplugin.h|"
    "cgUnknown;cbplugin.h|"
    "ChooseDirectory;globals.h|"
    "clogFull;compiler.h|"
    "clogNone;compiler.h|"
    "clogSimple;compiler.h|"
    "cltError;compiler.h|"
    "cltInfo;compiler.h|"
    "cltNormal;compiler.h|"
    "cltWarning;compiler.h|"
    "CodeBlocksDockEvent;sdk_events.h|"
    "CodeBlocksEvent;sdk_events.h|"
    "CodeBlocksLayoutEvent;sdk_events.h|"
    "CodeBlocksLogEvent;sdk_events.h|"
    "CompileOptionsBase;compileoptionsbase.h|"
    "Compiler;compiler.h|"
    "CompilerCommandGenerator;compilercommandgenerator.h|"
    "CompilerFactory;compilerfactory.h|"
    "CompilerOptions;compileroptions.h|"
    "CompilerPrograms;compiler.h|"
    "CompilerSwitches;compiler.h|"
    "CompilerTool;compiler.h|"
    "CompilerToolsVector;compiler.h|"
    "CompileTargetBase;compiletargetbase.h|"
    "CompOption;compileroptions.h|"
    "ConfigManager;configmanager.h|"
    "ConfigureToolsDlg;configuretoolsdlg.h|"
    "ConfigManagerContainer;configmanager.h|"
    "ConfirmReplaceDlg;confirmreplacedlg.h|"
    "CreateDir;globals.h|"
    "CreateDirRecursively;globals.h|"
    "CSS;loggers.h|"
    "Death;backgroundthread.h|"
    "DelayedDelete;filemanager.h|"
    "DetectEncodingAndConvert;globals.h|"
    "DuplicateBuildTarget;projectbuildtarget.h|"
    "EditArrayFileDlg;editarrayfiledlg.h|"
    "EditArrayOrderDlg;editarrayorderdlg.h|"
    "EditArrayStringDlg;editarraystringdlg.h|"
    "EditKeywordsDlg;editkeywordsdlg.h|"
    "EditorBase;editorbase.h|"
    "EditorColourSet;editorcolourset.h|"
    "EditorConfigurationDlg;editorconfigurationdlg.h|"
    "EditorHooks;editor_hooks.h|"
    "EditorLexerLoader;editorlexerloader.h|"
    "EditorManager;editormanager.h|"
    "EditPairDlg;editpairdlg.h|"
    "EditPathDlg;editpathdlg.h|"
    "EditToolDlg;edittooldlg.h|"
    "EncodingDetector;encodingdetector.h|"
    "ExternalDepsDlg;externaldepsdlg.h|"
    "FileGroups;filegroupsandmasks.h|"
    "FileLoader;filemanager.h|"
    "FileLogger;loggers.h|"
    "FileManager;filemanager.h|"
    "FileSet;projecttemplateloader.h|"
    "FileSetFile;projecttemplateloader.h|"
    "FilesGroupsAndMasks;filegroupsandmasks.h|"
    "FileTreeData;cbproject.h|"
    "FileType;globals.h|"
    "FileTypeOf;globals.h|"
    "FindDlg;finddlg.h|"
    "FindReplaceBase;findreplacebase.h|"
    "GenericMultiLineNotesDlg;genericmultilinenotesdlg.h|"
    "GetActiveEditor;editorbase.h|"
    "GetActiveProject;cbproject.h|"
    "GetArrayFromString;globals.h|"
    "GetBuiltinActiveEditor;cbeditor.h|"
    "GetBuiltinEditor;cbeditor.h|"
    "GetBuildTarget;projectbuildtarget.h|"
    "GetColourSet;editorcolourset.h|"
    "GetConfigManager;configmanager.h|"
    "GetConfigurationPanel;configurationpanel.h|"
    "GetCurrentlyCompilingTarget;projectbuildtarget.h|"
    "GetEditor;editorbase.h|"
    "GetEditorManager;editormanager.h|"
    "GetFile;projectfile.h|"
    "GetFileByFilename;projectfile.h|"
    "GetFileManager;filemanager.h|"
    "GetLogManager;logmanager.h|"
    "GetMacrosManager;macrosmanager.h|"
    "GetMessageManager;messagemanager.h|"
    "GetNotebook;wx/wxFlatNotebook/wxFlatNotebook.h|"
    "GetParentProject;cbproject.h|"
    "GetPersonalityManager;personalitymanager.h|"
    "GetPlatformsFromString;globals.h|"
    "GetPluginManager;pluginmanager.h|"
    "GetProjectConfigurationPanel;configurationpanel.h|"
    "GetProjectFile;projectfile.h|"
    "GetProjectManager;projectmanager.h|"
    "GetProjects;cbproject.h|"
    "GetScriptingManager;scriptingmanager.h|"
    "GetStringFromArray;globals.h|"
    "GetStringFromPlatforms;globals.h|"
    "GetToolsManager;toolsmanager.h|"
    "GetTopEditor;gettopeditor.h|"
    "GetUserVariableManager;uservarmanager.h|"
    "GetWorkspace;cbworkspace.h|"
    "HTMLFileLogger;loggers.h|"
    "IBaseLoader;ibaseloader.h|"
    "IBaseWorkspaceLoader;ibaseworkspaceloader.h|"
    "ID;id.h|"
    "IEventFunctorBase;cbfunctor.h|"
    "IFunctorBase;cbfunctor.h|"
    "ImportersGlobals;importer_globals.h|"
    "IncrementalSelectListDlg;incrementalselectlistdlg.h|"
    "InfoWindow;infowindow.h|"
    "IsBuiltinOpen;cbeditor.h|"
    "ISerializable;configmanager.h|"
    "IsOpen;cbproject.h|"
    "IsWindowReallyShown;globals.h|"
    "JobQueue;backgroundthread.h|"
    "FileLoader;filemanager.h|"
    "FileTreeData;cbproject.h|"
    "ListCtrlLogger;loggers.h|"
    "LoaderBase;filemanager.h|"
    "LoadPNGWindows2000Hack;globals.h|"
    "LoadProject;cbproject.h|"
    "Logger;logger.h|"
    "LogManager;logmanager.h|"
    "LogSlot;logmanager.h|"
    "MacrosManager;macrosmanager.h|"
    "MakeCommand;compiletargetbase.h|"
    "ManagedThread;managerthread.h|"
    "Manager;manager.h|"
    "MenuItemsManager;menuitemsmanager.h|"
    "MessageManager;messagemanager.h|"
    "Mgr;manager.h|"
    "MiscTreeItemData;misctreeitemdata.h|"
    "MultiSelectDlg;multiselectdlg.h|"
    "NewFromTemplateDlg;newfromtemplatedlg.h|"
    "NewProject;cbproject.h|"
    "NormalizePath;globals.h|"
    "NotifyMissingFile;globals.h|"
    "NullLoader;filemanager.h|"
    "NullLogger;logger.h|"
    "OptionColour;editorcolourset.h|"
    "OptionSet;editorcolourset.h|"
    "PCHMode;cbproject.h|"
    "pchObjectDir;cbproject.h|"
    "pchSourceDir;cbproject.h|"
    "pchSourceFile;cbproject.h|"
    "PlaceWindow;globals.h|"
    "PersonalityManager;personalitymanager.h|"
    "pfCustomBuild;projectfile.h|"
    "pfDetails;projectfile.h|"
    "PipedProcess;pipedprocess.h|"
    "PluginElement;pluginmanager.h|"
    "PluginInfo;pluginmanager.h|"
    "PluginManager;pluginmanager.h|"
    "PluginRegistrant;cbplugin.h|"
    "PluginsConfigurationDlg;pluginsconfigurationdlg.h|"
    "PluginType;globals.h|"
    "ProjectBuildTarget;projectbuildtarget.h|"
    "ProjectDepsDlg;projectdepsdlg.h|"
    "ProjectFile;projectfile.h|"
    "ProjectFileOptionsDlg;projectfileoptionsdlg.h|"
    "ProjectFilesVector;projectfile.h|"
    "ProjectLayoutLoader;projectlayoutloader.h|"
    "ProjectLoader;projectloader.h|"
    "ProjectLoaderHooks;projectloader_hooks.h|"
    "ProjectManager;projectmanager.h|"
    "ProjectOptionsDlg;projectoptionsdlg.h|"
    "ProjectsFileMasksDlg;projectsfilemasksdlg.h|"
    "ProjectTemplateLoader;projecttemplateloader.h|"
    "ptCodeCompletion;globals.h|"
    "ptCompiler;globals.h|"
    "ptDebugger;globals.h|"
    "ptMime;globals.h|"
    "ptNone;globals.h|"
    "ptOther;globals.h|"
    "ptTool;globals.h|"
    "ptWizard;globals.h|"
    "QuoteStringIfNeeded;globals.h|"
    "RegExStruct;compiler.h|"
    "ReplaceDlg;replacedlg.h|"
    "RestoreTreeState;globals.h|"
    "SaveTreeState;globals.h|"
    "ScriptingManager;scriptingmanager.h|"
    "ScriptSecurityWarningDlg;scriptsecuritywarningdlg.h|"
    "sdAllGlobal;configmanager.h|"
    "sdAllKnown;configmanager.h|"
    "sdAllUser;configmanager.h|"
    "sdBase;configmanager.h|"
    "sdConfig;configmanager.h|"
    "sdCurrent;configmanager.h|"
    "sdDataGlobal;configmanager.h|"
    "sdDataUser;configmanager.h|"
    "sdHome;configmanager.h|"
    "sdPath;configmanager.h|"
    "sdPluginsGlobal;configmanager.h|"
    "sdPluginsUser;configmanager.h|"
    "sdScriptsGlobal;configmanager.h|"
    "sdScriptsUser;configmanager.h|"
    "sdTemp;configmanager.h|"
    "SearchDirs;configmanager.h|"
    "SearchResultsLog;searchresultslog.h|"
    "SelectTargetDlg;selecttargetdlg.h|"
    "SeqDelete;safedelete.h|"
    "Stacker;infowindow.h|"
    "StdoutLogger;loggers.h|"
    "TemplateManager;templatemanager.h|"
    "TemplateOption;projecttemplateloader.h|"
    "TextCtrlLogger;loggers.h|"
    "TimestampTextCtrlLogger;loggers.h|"
    "ToolsManager;toolsmanager.h|"
    "ttCommandsOnly;compiletargetbase.h|"
    "ttConsoleOnly;compiletargetbase.h|"
    "ttDynamicLib;compiletargetbase.h|"
    "ttExecutable;compiletargetbase.h|"
    "ttNative;compiletargetbase.h|"
    "ttStaticLib;compiletargetbase.h|"
    "UnixFilename;globals.h|"
    "URLEncode;globals.h|"
    "URLLoader;filemanager.h|"
    "UsesCommonControls6;globals.h|"
    "UserVariableManager;uservarmanager.h|"
    "VirtualBuildTargetsDlg;virtualbuildtargetsdlg.h|"
    "WorkspaceLoader;workspaceloader.h|"
    "wxToolBarAddOnXmlHandler;xtra_res.h|"
    "wxBase64;base64.h|"
    "wxCrc32;crc32.h");

    const wxArrayString arCodeBlocks = GetArrayFromString(strCodeBlocks, _T("|"));
    for(std::size_t i = 0; i < arCodeBlocks.GetCount(); ++i)
    {
        const wxArrayString arTmp = GetArrayFromString(arCodeBlocks.Item(i), _T(";"));
        AddBinding(_T("CodeBlocks"), arTmp.Item(0), arTmp.Item(1) );
    }
}// SetDefaultsCodeBlocks

void Bindings::SetDefaultsWxWidgets()
{
    /////////////
    // v 2.6.4 //
    /////////////

    // All macros
    wxString strWxWidgets_2_6_4 = _T(
    "DECLARE_APP;wx/app.h|"
    "DECLARE_CLASS;wx/object.h|"
    "DECLARE_ABSTRACT_CLASS;wx/object.h|"
    "DECLARE_DYNAMIC_CLASS;wx/object.h|"
    "DECLARE_EVENT_TYPE;wx/event.h|"
    "DECLARE_EVENT_MACRO;wx/event.h|"
    "DECLARE_EVENT_TABLE_ENTRY;wx/event.h|"
    "IMPLEMENT_APP;wx/app.h|"
    "IMPLEMENT_ABSTRACT_CLASS;wx/object.h|"
    "IMPLEMENT_ABSTRACT_CLASS2;wx/object.h|"
    "IMPLEMENT_CLASS;wx/object.h|"
    "IMPLEMENT_CLASS2;wx/object.h|"
    "IMPLEMENT_DYNAMIC_CLASS;wx/object.h|"
    "IMPLEMENT_DYNAMIC_CLASS2;wx/object.h|"
    "DEFINE_EVENT_TYPE;wx/event.h|"
    "BEGIN_EVENT_TABLE;wx/event.h|"
    "END_EVENT_TABLE;wx/event.h|"
    "EVT_CUSTOM;wx/event.h|"
    "EVT_CUSTOM_RANGE;wx/event.h|"
    "EVT_COMMAND;wx/event.h|"
    "EVT_COMMAND_RANGE;wx/event.h|"
    "EVT_NOTIFY;wx/event.h|"
    "EVT_NOTIFY_RANGE;wx/event.h|"
    "EVT_BUTTON;wx/button.h|"
    "EVT_CHECKBOX;wx/checkbox.h|"
    "EVT_CHOICE;wx/choice.h|"
    "EVT_CHOICE;wx/choice.h|"
    "EVT_COMBOBOX;wx/combobox.h|"
    "EVT_LISTBOX;wx/listbox.h|"
    "EVT_LISTBOX_DCLICK;wx/listbox.h|"
    "EVT_RADIOBOX;wx/radiobox.h|"
    "EVT_RADIOBUTTON;wx/radiobut.h|"
    "EVT_SCROLLBAR;wx/scrolbar.h|"
    "EVT_SLIDER;wx/slider.h|"
    "EVT_TOGGLEBUTTON;wx/tglbtn.h|"
    "WX_APPEND_ARRAY;wx/dynarray.h|"
    "WX_CLEAR_ARRAY;wx/dynarray.h|"
    "WX_DECLARE_OBJARRAY;wx/dynarray.h|"
    "WX_DEFINE_ARRAY;wx/dynarray.h|"
    "WX_DEFINE_OBJARRAY;wx/dynarray.h|"
    "WX_DEFINE_SORTED_ARRAY;wx/dynarray.h|"
    "WX_DECLARE_STRING_HASH_MAP;wx/hashmap.h|"
    "WX_DECLARE_HASH_MAP;wx/hashmap.h|"
    "wxASSERT;wx/debug.h|"
    "wxASSERT_MIN_BITSIZE;wx/debug.h|"
    "wxASSERT_MSG;wx/debug.h|"
    "wxBITMAP;wx/gdicmn.h|"
    "wxCOMPILE_TIME_ASSERT;wx/debug.h|"
    "wxCOMPILE_TIME_ASSERT2;wx/debug.h|"
    "wxCRIT_SECT_DECLARE;wx/thread.h|"
    "wxCRIT_SECT_DECLARE_MEMBER;wx/thread.h|"
    "wxCRIT_SECT_LOCKER;wx/thread.h|"
    "wxDYNLIB_FUNCTION;wx/dynlib.h|"
    "wxENTER_CRIT_SECT;wx/thread.h|"
    "wxFAIL;wx/debug.h|"
    "wxFAIL_MSG;wx/debug.h|"
    "wxICON;wx/gdicmn.h|"
    "wxLEAVE_CRIT_SECT;wx/thread.h|"
    "wxLL;wx/longlong.h|"
    "wxTRANSLATE;wx/intl.h|"
    "wxULL;wx/longlong.h|"

    // All ::wx methods
    "wxBeginBusyCursor;wx/utils.h|"
    "wxBell;wx/utils.h|"
    "wxClientDisplayRect;wx/gdicmn.h|"
    "wxClipboardOpen;wx/clipbrd.h|"
    "wxCloseClipboard;wx/clipbrd.h|"
    "wxColourDisplay;wx/gdicmn.h|"
    "wxConcatFiles;wx/filefn.h|"
    "wxCopyFile;wx/filefn.h|"
    "wxCreateDynamicObject;wx/object.h|"
    "wxCreateFileTipProvider;wx/tipdlg.h|"
    "wxDDECleanUp;wx/dde.h|"
    "wxDDEInitialize;wx/dde.h|"
    "wxDebugMsg;wx/utils.h|"
    "wxDirExists;wx/filefn.h|"
    "wxDirSelector;wx/dirdlg.h|"
    "wxDisplayDepth;wx/gdicmn.h|"
    "wxDisplaySize;wx/gdicmn.h|"
    "wxDisplaySizeMM;wx/gdicmn.h|"
    "wxDos2UnixFilename;wx/filefn.h|"
    "wxDROP_ICON;wx/dnd.h|"
    "wxEmptyClipboard;wx/clipbrd.h|"
    "wxEnableTopLevelWindows;wx/utils.h|"
    "wxEndBusyCursor;wx/utils.h|"
    "wxEntry;wx/app.h|"
    "wxEnumClipboardFormats;wx/clipbrd.h|"
    "wxError;wx/utils.h|"
    "wxExecute;wx/utils.h|"
    "wxExit;wx/app.h|"
    "wxFatalError;wx/utils.h|"
    "wxFileExists;wx/filefn.h|"
    "wxFileModificationTime;wx/filefn.h|"
    "wxFileNameFromPath;wx/filefn.h|"
    "wxFileSelector;wx/filedlg.h|"
    "wxFindFirstFile;wx/filefn.h|"
    "wxFindMenuItemId;wx/utils.h|"
    "wxFindNextFile;wx/filefn.h|"
    "wxFindWindowAtPoint;wx/utils.h|"
    "wxFindWindowAtPointer;wx/windows.h|"
    "wxFindWindowByLabel;wx/utils.h|"
    "wxFindWindowByName;wx/utils.h|"
    "wxGetActiveWindow;wx/windows.h|"
    "wxGetApp;wx/app.h|"
    "wxGetBatteryState;wx/utils.h|"
    "wxGetClipboardData;wx/clipbrd.h|"
    "wxGetClipboardFormatName;wx/clipbrd.h|"
    "wxGetColourFromUser;wx/colordlg.h|"
    "wxGetCwd;wx/filefn.h|"
    "wxGetDiskSpace;wx/filefn.h|"
    "wxGetDisplayName;wx/utils.h|"
    "wxGetElapsedTime;wx/timer.h|"
    "wxGetEmailAddress;wx/utils.h|"
    "wxGetFileKind;wx/filefn.h|"
    "wxGetFontFromUser;wx/fontdlg.h|"
    "wxGetFreeMemory;wx/utils.h|"
    "wxGetFullHostName;wx/utils.h|"
    "wxGetHomeDir;wx/utils.h|"
    "wxGetHostName;wx/utils.h|"
    "wxGetKeyState;wx/utils.h|"
    "wxGetLocalTime;wx/timer.h|"
    "wxGetLocalTimeMillis;wx/timer.h|"
    "wxGetMousePosition;wx/utils.h|"
    "wxGetMouseState;wx/utils.h|"
    "wxGetMultipleChoice;wx/choicdlg.h|"
    "wxGetMultipleChoices;wx/choicdlg.h|"
    "wxGetNumberFromUser;wx/numdlg.h|"
    "wxGetOsDescription;wx/utils.h|"
    "wxGetOSDirectory;wx/filefn.h|"
    "wxGetOsVersion;wx/utils.h|"
    "wxGetPasswordFromUser;wx/textdlg.h|"
    "wxGetPowerType;wx/utils.h|"
    "wxGetPrinterCommand;wx/dcps.h|"
    "wxGetPrinterFile;wx/dcps.h|"
    "wxGetPrinterMode;wx/dcps.h|"
    "wxGetPrinterOptions;wx/dcps.h|"
    "wxGetPrinterOrientation;wx/dcps.h|"
    "wxGetPrinterPreviewCommand;wx/dcps.h|"
    "wxGetPrinterScaling;wx/dcps.h|"
    "wxGetPrinterTranslation;wx/dcps.h|"
    "wxGetProcessId;wx/utils.h|"
    "wxGetResource;wx/utils.h|"
    "wxGetSingleChoice;wx/choicdlg.h|"
    "wxGetSingleChoiceData;wx/choicdlg.h|"
    "wxGetSingleChoiceIndex;wx/choicdlg.h|"
    "wxGetStockLabel;wx/stockitem.h|"
    "wxGetTempFileName;wx/filefn.h|"
    "wxGetTextFromUser;wx/textdlg.h|"
    "wxGetTopLevelParent;wx/window.h|"
    "wxGetTranslation;wx/intl.h|"
    "wxGetUserHome;wx/utils.h|"
    "wxGetUserId;wx/utils.h|"
    "wxGetUserName;wx/utils.h|"
    "wxGetUTCTime;wx/timer.h|"
    "wxGetWorkingDirectory;wx/filefn.h|"
    "wxHandleFatalExceptions;wx/app.h|"
    "wxInitAllImageHandlers;wx/image.h|"
    "wxInitialize;wx/app.h|"
    "wxIsAbsolutePath;wx/filefn.h|"
    "wxIsBusy;wx/utils.h|"
    "wxIsClipboardFormatAvailable;wx/clipbrd.h|"
    "wxIsDebuggerRunning;wx/debug.h|"
    "wxIsEmpty;wx/wxchar.h|"
    "wxIsMainThread;wx/thread.h|"
    "wxIsWild;wx/filefn.h|"
    "wxKill;wx/app.h|"
    "wxLaunchDefaultBrowser;wx/utils.h|"
    "wxLoadUserResource;wx/utils.h|"
    "wxLogDebug;wx/log.h|"
    "wxLogError;wx/log.h|"
    "wxLogFatalError;wx/log.h|"
    "wxLogMessage;wx/log.h|"
    "wxLogStatus;wx/log.h|"
    "wxLogSysError;wx/log.h|"
    "wxLogTrace;wx/log.h|"
    "wxLogVerbose;wx/log.h|"
    "wxLogWarning;wx/log.h|"
    "wxMakeMetafilePlaceable;wx/gdicmn.h|"
    "wxMatchWild;wx/filefn.h|"
    "wxMessageBox;wx/msgdlg.h|"
    "wxMicroSleep;wx/utils.h|"
    "wxMilliSleep;wx/utils.h|"
    "wxMkdir;wx/filefn.h|"
    "wxMutexGuiEnter;wx/thread.h|"
    "wxMutexGuiLeave;wx/thread.h|"
    "wxNewId;wx/utils.h|"
    "wxNow;wx/utils.h|"
    "wxOnAssert;wx/debug.h|"
    "wxOpenClipboard;wx/clipbrd.h|"
    "wxParseCommonDialogsFilter;wx/filefn.h|"
    "wxPathOnly;wx/filefn.h|"
    "wxPostDelete;wx/utils.h|"
    "wxPostEvent;wx/app.h|"
    "wxRegisterClipboardFormat;wx/clipbrd.h|"
    "wxRegisterId;wx/utils.h|"
    "wxRemoveFile;wx/filefn.h|"
    "wxRenameFile;wx/filefn.h|"
    "wxRmdir;wx/filefn.h|"
    "wxSafeShowMessage;wx/log.h|"
    "wxSafeYield;wx/utils.h|"
    "wxSetClipboardData;wx/clipbrd.h|"
    "wxSetCursor;wx/gdicmn.h|"
    "wxSetDisplayName;wx/utils.h|"
    "wxSetPrinterCommand;wx/dcps.h|"
    "wxSetPrinterFile;wx/dcps.h|"
    "wxSetPrinterMode;wx/dcps.h|"
    "wxSetPrinterOptions;wx/dcps.h|"
    "wxSetPrinterOrientation;wx/dcps.h|"
    "wxSetPrinterPreviewCommand;wx/dcps.h|"
    "wxSetPrinterScaling;wx/dcps.h|"
    "wxSetPrinterTranslation;wx/dcps.h|"
    "wxSetWorkingDirectory;wx/filefn.h|"
    "wxShell;wx/utils.h|"
    "wxShowTip;wx/tipdlg.h|"
    "wxShutdown;wx/utils.h|"
    "wxSleep;wx/utils.h|"
    "wxSnprintf;wx/wxchar.h|"
    "wxSplitPath;wx/filefn.h|"
    "wxStartTimer;wx/timer.h|"
    "wxStrcmp;wx/wxchar.h|"
    "wxStricmp;wx/wxchar.h|"
    "wxStringEq;wx/utils.h|"
    "wxStripMenuCodes;wx/utils.h|"
    "wxStrlen;wx/wxchar.h|"
    "wxSysErrorCode;wx/log.h|"
    "wxSysErrorMsg;wx/log.h|"
    "wxTrace;wx/memory.h|"
    "wxTraceLevel;wx/memory.h|"
    "wxTransferFileToStream;wx/docview.h|"
    "wxTransferStreamToFile;wx/docview.h|"
    "wxTrap;wx/debug.h|"
    "wxUninitialize;wx/app.h|"
    "wxUnix2DosFilename;wx/filefn.h|"
    "wxUsleep;wx/utils.h|"
    "wxVsnprintf;wx/wxchar.h|"
    "wxWakeUpIdle;wx/app.h|"
    "wxWriteResource;wx/utils.h|"
    "wxYield;wx/app.h|"

    // All ::wx classes
    "wxAcceleratorEntry;wx/accel.h|"
    "wxAcceleratorTable;wx/accel.h|"
    "wxAccessible;wx/access.h|"
    "wxActivateEvent;wx/event.h|"
    "wxApp;wx/app.h|"
    "wxArchiveClassFactory;wx/archive.h|"
    "wxArchiveEntry;wx/archive.h|"
    "wxArchiveInputStream;wx/archive.h|"
    "wxArchiveIterator;wx/archive.h|"
    "wxArchiveNotifier;wx/archive.h|"
    "wxArchiveOutputStream;wx/archive.h|"
    "wxArray;wx/dynarray.h|"
    "wxArrayString;wx/arrstr.h|"
    "wxArtProvider;wx/artprov.h|"
    "wxAutomationObject;wx/msw/ole/automtn.h|"
    "wxBitmap;wx/bitmap.h|"
    "wxBitmapButton;wx/bmpbuttn.h|"
    "wxBitmapDataObject;wx/dataobj.h|"
    "wxBitmapHandler;wx/bitmap.h|"
    "wxBoxSizer;wx/sizer.h|"
    "wxBrush;wx/brush.h|"
    "wxBrushList;wx/gdicmn.h|"
    "wxBufferedDC;wx/dcbuffer.h|"
    "wxBufferedInputStream;wx/stream.h|"
    "wxBufferedOutputStream;wx/stream.h|"
    "wxBufferedPaintDC;wx/dcbuffer.h|"
    "wxBusyCursor;wx/utils.h|"
    "wxBusyInfo;wx/busyinfo.h|"
    "wxButton;wx/button.h|"
    "wxCalculateLayoutEvent;wx/laywin.h|"
    "wxCalendarCtrl;wx/calctrl.h|"
    "wxCalendarDateAttr;wx/calctrl.h|"
    "wxCalendarEvent;wx/calctrl.h|"
    "wxCaret;wx/caret.h|"
    "wxCheckBox;wx/checkbox.h|"
    "wxCheckListBox;wx/checklst.h|"
    "wxChoice;wx/choice.h|"
    "wxChoicebook;wx/choicebk.h|"
    "wxClassInfo;wx/object.h|"
    "wxClient;wx/ipc.h|"
    "wxClientData;wx/clntdata.h|"
    "wxClientDataContainer;wx/clntdata.h|"
    "wxClientDC;wx/dcclient.h|"
    "wxClipboard;wx/clipbrd.h|"
    "wxCloseEvent;wx/event.h|"
    "wxCmdLineParser;wx/cmdline.h|"
    "wxColour;wx/colour.h|"
    "wxColourData;wx/cmndata.h|"
    "wxColourDatabase;wx/gdicmn.h|"
    "wxColourDialog;wx/colordlg.h|"
    "wxComboBox;wx/combobox.h|"
    "wxCommand;wx/cmdproc.h|"
    "wxCommandEvent;wx/event.h|"
    "wxCommandProcessor;wx/cmdproc.h|"
    "wxCondition;wx/thread.h|"
    "wxConfigBase;wx/config.h|"
    "wxConnection;wx/ipc.h|"
    "wxContextHelp;wx/cshelp.h|"
    "wxContextHelpButton;wx/cshelp.h|"
    "wxContextMenuEvent;wx/event.h|"
    "wxControl;wx/control.h|"
    "wxControlWithItems;wx/ctrlsub.h|"
    "wxCountingOutputStream;wx/stream.h|"
    "wxCriticalSection;wx/thread.h|"
    "wxCriticalSectionLocker;wx/thread.h|"
    "wxCSConv;wx/strconv.h|"
    "wxCurrentTipProvider;wx/tipdlg.h|"
    "wxCursor;wx/cursor.h|"
    "wxCustomDataObject;wx/dataobj.h|"
    "wxDataFormat;wx/dataobj.h|"
    "wxDataInputStream;wx/datstrm.h|"
    "wxDataObject;wx/dataobj.h|"
    "wxDataObjectComposite;wx/dataobj.h|"
    "wxDataObjectSimple;wx/dataobj.h|"
    "wxDataOutputStream;wx/datstrm.h|"
    "wxDateEvent;wx/dateevt.h|"
    "wxDatePickerCtrl;wx/datectrl.h|"
    "wxDateSpan;wx/datetime.h|"
    "wxDateTime;wx/datetime.h|"
    "wxDb;wx/db.h|"
    "wxDbColDataPtr;wx/db.h|"
    "wxDbColDef;wx/db.h|"
    "wxDbColFor;wx/db.h|"
    "wxDbColInf;wx/db.h|"
    "wxDbConnectInf;wx/db.h|"
    "wxDbGridColInfo;wx/dbgrid.h|"
    "wxDbGridTableBase;wx/dbgrid.h|"
    "wxDbIdxDef;wx/db.h|"
    "wxDbInf;wx/db.h|"
    "wxDbTable;wx/dbtable.h|"
    "wxDbTableInf;wx/db.h|"
    "wxDC;wx/dc.h|"
    "wxDCClipper;wx/dc.h|"
    "wxDDEClient;wx/dde.h|"
    "wxDDEConnection;wx/dde.h|"
    "wxDDEServer;wx/dde.h|"
    "wxDebugContext;wx/memory.h|"
    "wxDebugReport;wx/debugrpt.h|"
    "wxDebugReportCompress;wx/debugrpt.h|"
    "wxDebugReportPreview;wx/debugrpt.h|"
    "wxDebugReportUpload;wx/debugrpt.h|"
    "wxDebugStreamBuf;wx/memory.h|"
    "wxDelegateRendererNative;wx/renderer.h|"
    "wxDialog;wx/dialog.h|"
    "wxDialUpEvent;wx/dialup.h|"
    "wxDialUpManager;wx/dialup.h|"
    "wxDir;wx/dir.h|"
    "wxDirDialog;wx/dirdlg.h|"
    "wxDirTraverser;wx/dir.h|"
    "wxDisplay;wx/display.h|"
    "wxDllLoader;wx/dynlib.h|"
    "wxDocChildFrame;wx/docview.h|"
    "wxDocManager;wx/docview.h|"
    "wxDocMDIChildFrame;wx/docmdi.h|"
    "wxDocMDIParentFrame;wx/docmdi.h|"
    "wxDocParentFrame;wx/docview.h|"
    "wxDocTemplate;wx/docview.h|"
    "wxDocument;wx/docview.h|"
    "wxDragImage;wx/dragimag.h|"
    "wxDragResult;wx/dnd.h|"
    "wxDropFilesEvent;wx/event.h|"
    "wxDropSource;wx/dnd.h|"
    "wxDropTarget;wx/dnd.h|"
    "wxDynamicLibrary;wx/dynlib.h|"
    "wxDynamicLibraryDetails;wx/dynlib.h|"
    "wxEncodingConverter;wx/encconv.h|"
    "wxEraseEvent;wx/event.h|"
    "wxEvent;wx/event.h|"
    "wxEvtHandler;wx/event.h|"
    "wxFFile;wx/ffile.h|"
    "wxFFileInputStream;wx/wfstream.h|"
    "wxFFileOutputStream;wx/wfstream.h|"
    "wxFFileStream;wx/wfstream.h|"
    "wxFile;wx/file.h|"
    "wxFileConfig;wx/fileconf.h|"
    "wxFileDataObject;wx/dataobj.h|"
    "wxFileDialog;wx/filedlg.h|"
    "wxFileDropTarget;wx/dnd.h|"
    "wxFileHistory;wx/docview.h|"
    "wxFileInputStream;wx/wfstream.h|"
    "wxFileName;wx/filename.h|"
    "wxFileOutputStream;wx/wfstream.h|"
    "wxFileStream;wx/wfstream.h|"
    "wxFileSystem;wx/filesys.h|"
    "wxFileSystemHandler;wx/filesys.h|"
    "wxFileType;wx/mimetype.h|"
    "wxFilterInputStream;wx/stream.h|"
    "wxFilterOutputStream;wx/stream.h|"
    "wxFindDialogEvent;wx/fdrepdlg.h|"
    "wxFindReplaceData;wx/fdrepdlg.h|"
    "wxFindReplaceDialog;wx/fdrepdlg.h|"
    "wxFinite;wx/math.h|"
    "wxFlexGridSizer;wx/sizer.h|"
    "wxFocusEvent;wx/event.h|"
    "wxFont;wx/font.h|"
    "wxFontData;wx/cmndata.h|"
    "wxFontDialog;wx/fontdlg.h|"
    "wxFontEnumerator;wx/fontenum.h|"
    "wxFontList;wx/gdicmn.h|"
    "wxFontMapper;wx/fontmap.h|"
    "wxFrame;wx/frame.h|"
    "wxFSFile;wx/filesys.h|"
    "wxFTP;wx/protocol/ftp.h|"
    "wxGauge;wx/gauge.h|"
    "wxGBPosition;wx/gbsizer.h|"
    "wxGBSizerItem;wx/gbsizer.h|"
    "wxGBSpan;wx/gbsizer.h|"
    "wxGDIObject;wx/gdiobj.h|"
    "wxGenericDirCtrl;wx/dirctrl.h|"
    "wxGenericValidator;wx/valgen.h|"
    "wxGetenv;wx/utils.h|"
    "wxGetVariantCast;wx/variant.h|"
    "wxGLCanvas;wx/glcanvas.h|"
    "wxGLContext;wx/glcanvas.h|"
    "wxGrid;wx/grid.h|"
    "wxGridBagSizer;wx/gbsizer.h|"
    "wxGridCellAttr;wx/grid.h|"
    "wxGridCellBoolEditor;wx/grid.h|"
    "wxGridCellBoolRenderer;wx/grid.h|"
    "wxGridCellChoiceEditor;wx/grid.h|"
    "wxGridCellEditor;wx/grid.h|"
    "wxGridCellFloatEditor;wx/grid.h|"
    "wxGridCellFloatRenderer;wx/grid.h|"
    "wxGridCellNumberEditor;wx/grid.h|"
    "wxGridCellNumberRenderer;wx/grid.h|"
    "wxGridCellRenderer;wx/grid.h|"
    "wxGridCellStringRenderer;wx/grid.h|"
    "wxGridCellTextEditor;wx/grid.h|"
    "wxGridEditorCreatedEvent;wx/grid.h|"
    "wxGridEvent;wx/grid.h|"
    "wxGridRangeSelectEvent;wx/grid.h|"
    "wxGridSizeEvent;wx/grid.h|"
    "wxGridSizer;wx/sizer.h|"
    "wxGridTableBase;wx/grid.h|"
    "wxHashMap;wx/hashmap.h|"
    "wxHashSet;wx/hashset.h|"
    "wxHashTable;wx/hash.h|"
    "wxHelpController;wx/help.h|"
    "wxHelpControllerHelpProvider;wx/cshelp.h|"
    "wxHelpEvent;wx/event.h|"
    "wxHelpProvider;wx/cshelp.h|"
    "wxHtmlCell;wx/html/htmlcell.h|"
    "wxHtmlColourCell;wx/html/htmlcell.h|"
    "wxHtmlContainerCell;wx/html/htmlcell.h|"
    "wxHtmlDCRenderer;wx/html/htmprint.h|"
    "wxHtmlEasyPrinting;wx/html/htmprint.h|"
    "wxHtmlFilter;wx/html/htmlfilt.h|"
    "wxHtmlHelpController;wx/html/helpctrl.h|"
    "wxHtmlHelpData;wx/html/helpdata.h|"
    "wxHtmlHelpFrame;wx/html/helpfrm.h|"
    "wxHtmlLinkInfo;wx/html/htmlcell.h|"
    "wxHtmlListBox;wx/htmllbox.h|"
    "wxHtmlParser;wx/html/htmlpars.h|"
    "wxHtmlPrintout;wx/html/htmprint.h|"
    "wxHtmlTag;wx/html/htmltag.h|"
    "wxHtmlTagHandler;wx/html/htmlpars.h|"
    "wxHtmlTagsModule;wx/html/winpars.h|"
    "wxHtmlWidgetCell;wx/html/htmlcell.h|"
    "wxHtmlWindow;wx/html/htmlwin.h|"
    "wxHtmlWinParser;wx/html/winpars.h|"
    "wxHtmlWinTagHandler;wx/html/winpars.h|"
    "wxHTTP;wx/protocol/http.h|"
    "wxIcon;wx/icon.h|"
    "wxIconBundle;wx/iconbndl.h|"
    "wxIconizeEvent;wx/event.h|"
    "wxIconLocation;wx/iconloc.h|"
    "wxIdleEvent;wx/event.h|"
    "wxImage;wx/image.h|"
    "wxImageHandler;wx/image.h|"
    "wxImageList;wx/imaglist.h|"
    "wxIndividualLayoutConstraint;wx/layout.h|"
    "wxInitDialogEvent;wx/event.h|"
    "wxInputStream;wx/stream.h|"
    "wxIPaddress;wx/socket.h|"
    "wxIPV4address;wx/socket.h|"
    "wxIsNaN;wx/math.h|"
    "wxJoystick;wx/joystick.h|"
    "wxJoystickEvent;wx/event.h|"
    "wxKeyEvent;wx/event.h|"
    "wxLayoutAlgorithm;wx/laywin.h|"
    "wxLayoutConstraints;wx/layout.h|"
    "wxList;wx/list.h|"
    "wxListbook;wx/listbook.h|"
    "wxListCtrl;wx/listctrl.h|"
    "wxListEvent;wx/listctrl.h|"
    "wxListItem;wx/listctrl.h|"
    "wxListItemAttr;wx/listctrl.h|"
    "wxListView;wx/listctrl.h|"
    "wxLocale;wx/intl.h|"
    "wxLog;wx/log.h|"
    "wxLogChain;wx/log.h|"
    "wxLogGui;wx/log.h|"
    "wxLogNull;wx/log.h|"
    "wxLogPassThrough;wx/log.h|"
    "wxLogStderr;wx/log.h|"
    "wxLogStream;wx/log.h|"
    "wxLogTextCtrl;wx/log.h|"
    "wxLogWindow;wx/log.h|"
    "wxLongLong;wx/longlong.h|"
    "wxLongLongFmtSpec;wx/longlong.h|"
    "wxMask;wx/bitmap.h|"
    "wxMaximizeEvent;wx/event.h|"
    "wxMBConv;wx/strconv.h|"
    "wxMBConvFile;wx/strconv.h|"
    "wxMBConvUTF16;wx/strconv.h|"
    "wxMBConvUTF32;wx/strconv.h|"
    "wxMBConvUTF7;wx/strconv.h|"
    "wxMBConvUTF8;wx/strconv.h|"
    "wxMDIChildFrame;wx/mdi.h|"
    "wxMDIClientWindow;wx/mdi.h|"
    "wxMDIParentFrame;wx/mdi.h|"
    "wxMediaCtrl;wx/mediactrl.h|"
    "wxMediaEvent;wx/mediactrl.h|"
    "wxMemoryBuffer;wx/buffer.h|"
    "wxMemoryDC;wx/dcmemory.h|"
    "wxMemoryFSHandler;wx/fs_mem.h|"
    "wxMemoryInputStream;wx/mstream.h|"
    "wxMemoryOutputStream;wx/mstream.h|"
    "wxMenu;wx/menu.h|"
    "wxMenuBar;wx/menu.h|"
    "wxMenuEvent;wx/event.h|"
    "wxMenuItem;wx/menuitem.h|"
    "wxMessageDialog;wx/msgdlg.h|"
    "wxMetafile;wx/metafile.h|"
    "wxMetafileDC;wx/metafile.h|"
    "wxMimeTypesManager;wx/mimetype.h|"
    "wxMiniFrame;wx/minifram.h|"
    "wxMirrorDC;wx/dcmirror.h|"
    "wxModule;wx/module.h|"
    "wxMouseCaptureChangedEvent;wx/event.h|"
    "wxMouseEvent;wx/event.h|"
    "wxMoveEvent;wx/event.h|"
    "wxMultiChoiceDialog;wx/choicdlg.h|"
    "wxMutex;wx/thread.h|"
    "wxMutexLocker;wx/thread.h|"
    "wxNode;wx/list.h|"
    "wxNotebook;wx/notebook.h|"
    "wxNotebookEvent;wx/notebook.h|"
    "wxNotebookSizer;wx/sizer.h|"
    "wxNotifyEvent;wx/event.h|"
    "wxObjArray;wx/dynarray.h|"
    "wxObject;wx/object.h|"
    "wxObjectRefData;wx/object.h|"
    "wxOpenErrorTraverser;wx/dir.h|"
    "wxOutputStream;wx/stream.h|"
    "wxPageSetupDialog;wx/printdlg.h|"
    "wxPageSetupDialogData;wx/cmndata.h|"
    "wxPaintDC;wx/dcclient.h|"
    "wxPaintEvent;wx/event.h|"
    "wxPalette;wx/palette.h|"
    "wxPanel;wx/panel.h|"
    "wxPaperSize;wx/cmndata.h|"
    "wxPasswordEntryDialog;wx/textdlg.h|"
    "wxPathList;wx/filefn.h|"
    "wxPen;wx/pen.h|"
    "wxPenList;wx/gdicmn.h|"
    "wxPoint;wx/gdicmn.h|"
    "wxPostScriptDC;wx/dcps.h|"
    "wxPreviewCanvas;wx/print.h|"
    "wxPreviewControlBar;wx/print.h|"
    "wxPreviewFrame;wx/print.h|"
    "wxPrintData;wx/cmndata.h|"
    "wxPrintDialog;wx/printdlg.h|"
    "wxPrintDialogData;wx/cmndata.h|"
    "wxPrinter;wx/print.h|"
    "wxPrinterDC;wx/dcprint.h|"
    "wxPrintout;wx/print.h|"
    "wxPrintPreview;wx/print.h|"
    "wxProcess;wx/process.h|"
    "wxProgressDialog;wx/progdlg.h|"
    "wxPropertySheetDialog;wx/propdlg.h|"
    "wxProtocol;wx/protocol/protocol.h|"
    "wxQuantize;wx/quantize.h|"
    "wxQueryLayoutInfoEvent;wx/laywin.h|"
    "wxRadioBox;wx/radiobox.h|"
    "wxRadioButton;wx/radiobut.h|"
    "wxRealPoint;wx/gdicmn.h|"
    "wxRect;wx/gdicmn.h|"
    "wxRecursionGuard;wx/recguard.h|"
    "wxRecursionGuardFlag;wx/recguard.h|"
    "wxRegEx;wx/regex.h|"
    "wxRegion;wx/region.h|"
    "wxRegionIterator;wx/region.h|"
    "wxRegKey;wx/msw/registry.h|"
    "wxRendererNative;wx/renderer.h|"
    "wxRendererVersion;wx/renderer.h|"
    "wxSashEvent;wx/sashwin.h|"
    "wxSashLayoutWindow;wx/laywin.h|"
    "wxSashWindow;wx/sashwin.h|"
    "wxScopedArray;wx/ptr_scpd.h|"
    "wxScopedPtr;wx/ptr_scpd.h|"
    "wxScopedTiedPtr;wx/ptr_scpd.h|"
    "wxScreenDC;wx/dcscreen.h|"
    "wxScrollBar;wx/scrolbar.h|"
    "wxScrolledWindow;wx/scrolwin.h|"
    "wxScrollEvent;wx/event.h|"
    "wxScrollWinEvent;wx/event.h|"
    "wxSemaphore;wx/thread.h|"
    "wxServer;wx/ipc.h|"
    "wxSetCursorEvent;wx/event.h|"
    "wxSetEnv;wx/utils.h|"
    "wxSimpleHelpProvider;wx/cshelp.h|"
    "wxSingleChoiceDialog;wx/choicdlg.h|"
    "wxSingleInstanceChecker;wx/snglinst.h|"
    "wxSize;wx/gdicmn.h|"
    "wxSizeEvent;wx/event.h|"
    "wxSizer;wx/sizer.h|"
    "wxSizerFlags;wx/sizer.h|"
    "wxSizerItem;wx/sizer.h|"
    "wxSlider;wx/slider.h|"
    "wxSockAddress;wx/socket.h|"
    "wxSocketBase;wx/socket.h|"
    "wxSocketClient;wx/socket.h|"
    "wxSocketEvent;wx/socket.h|"
    "wxSocketInputStream;wx/sckstrm.h|"
    "wxSocketOutputStream;wx/sckstrm.h|"
    "wxSocketServer;wx/socket.h|"
    "wxSound;wx/sound.h|"
    "wxSpinButton;wx/spinbutt.h|"
    "wxSpinCtrl;wx/spinctrl.h|"
    "wxSpinEvent;wx/spinctrl.h|"
    "wxSplashScreen;wx/splash.h|"
    "wxSplitterEvent;wx/splitter.h|"
    "wxSplitterRenderParams;wx/renderer.h|"
    "wxSplitterWindow;wx/splitter.h|"
    "wxStackFrame;wx/stackwalk.h|"
    "wxStackWalker;wx/stackwalk.h|"
    "wxStandardPaths;wx/stdpaths.h|"
    "wxStaticBitmap;wx/statbmp.h|"
    "wxStaticBox;wx/statbox.h|"
    "wxStaticLine;wx/statline.h|"
    "wxStaticText;wx/stattext.h|"
    "wxStatusBar;wx/statusbr.h|"
    "wxStdDialogButtonSizer;wx/sizer.h|"
    "wxStopWatch;wx/stopwatch.h|"
    "wxStreamBase;wx/stream.h|"
    "wxStreamBuffer;wx/stream.h|"
    "wxStreamToTextRedirector;wx/textctrl.h|"
    "wxString;wx/string.h|"
    "wxStringBuffer;wx/string.h|"
    "wxStringBufferLength;wx/string.h|"
    "wxStringClientData;clntdata.h|"
    "wxStringInputStream;wx/sstream.h|"
    "wxStringOutputStream;wx/sstream.h|"
    "wxStringTokenizer;wx/tokenzr.h|"
    "wxSystemOptions;wx/sysopt.h|"
    "wxSystemSettings;wx/settings.h|"
    "wxTaskBarIcon;wx/taskbar.h|"
    "wxTCPClient;wx/sckipc.h|"
    "wxTCPServer;wx/sckipc.h|"
    "wxTempFile;wx/file.h|"
    "wxTempFileOutputStream;wx/wfstream.h|"
    "wxTextAttr;wx/textctrl.h|"
    "wxTextCtrl;wx/textctrl.h|"
    "wxTextDataObject;wx/dataobj.h|"
    "wxTextDropTarget;wx/dnd.h|"
    "wxTextEntryDialog;wx/textdlg.h|"
    "wxTextFile;wx/textfile.h|"
    "wxTextInputStream;wx/txtstrm.h|"
    "wxTextOutputStream;wx/txtstrm.h|"
    "wxTextValidator;wx/valtext.h|"
    "wxTheClipboard;wx/clipbrd.h|"
    "wxThread;wx/thread.h|"
    "wxThreadHelper;wx/thread.h|"
    "wxTimer;wx/timer.h|"
    "wxTimerEvent;wx/timer.h|"
    "wxTimeSpan;wx/datetime.h|"
    "wxTipProvider;wx/tipdlg.h|"
    "wxTipWindow;wx/tipwin.h|"
    "wxToggleButton;wx/tglbtn.h|"
    "wxToolBar;wx/toolbar.h|"
    "wxToolTip;wx/tooltip.h|"
    "wxTopLevelWindow;wx/toplevel.h|"
    "wxTreeCtrl;wx/treectrl.h|"
    "wxTreeEvent;wx/treectrl.h|"
    "wxTreeItemData;wx/treectrl.h|"
    "wxUnsetEnv;wx/utils.h|"
    "wxUpdateUIEvent;wx/event.h|"
    "wxURI;wx/uri.h|"
    "wxURL;wx/url.h|"
    "wxVaCopy;wx/defs.h|"
    "wxValidator;wx/validate.h|"
    "wxVariant;wx/variant.h|"
    "wxVariantData;wx/variant.h|"
    "wxView;wx/docview.h|"
    "wxVListBox;wx/vlbox.h|"
    "wxVScrolledWindow;wx/vscroll.h|"
    "wxWindow;wx/window.h|"
    "wxWizard;wx/wizard.h|"
    "wxWizardEvent;wx/wizard.h|"
    "wxWizardPage;wx/wizard.h|"
    "wxWizardPageSimple;wx/wizard.h|"
    "wxXmlResource;wx/xrc/xmlres.h|"
    "wxXmlResourceHandler;wx/xrc/xmlres.h|"
    "wxZipClassFactory;wx/zipstrm.h|"
    "wxZipEntry;wx/zipstrm.h|"
    "wxZipInputStream;wx/zipstrm.h|"
    "wxZipNotifier;wx/zipstrm.h|"
    "wxZipOutputStream;wx/zipstrm.h|"
    "wxZlibInputStream;wx/zstream.h|"
    "wxZlibOutputStream;wx/zstream.h");

    const wxArrayString arWxWidgets_2_6_4 = GetArrayFromString(strWxWidgets_2_6_4, _T("|"));
    for(std::size_t i = 0; i < arWxWidgets_2_6_4.GetCount(); ++i)
    {
        const wxArrayString arTmp = GetArrayFromString(arWxWidgets_2_6_4.Item(i), _T(";"));
        AddBinding(_T("wxWidgets_2_6_4"), arTmp.Item(0), arTmp.Item(1) );
    }

    /////////////
    // v 2.8.8 //
    /////////////

    // All macros
    wxString strWxWidgets_2_8_8 = _T(
    "DECLARE_APP;wx/app.h|"
    "DECLARE_ABSTRACT_CLASS;wx/object.h|"
    "DECLARE_CLASS;wx/object.h|"
    "DECLARE_DYNAMIC_CLASS;wx/object.h|"
    "IMPLEMENT_APP;wx/app.h|"
    "IMPLEMENT_ABSTRACT_CLASS;wx/object.h|"
    "IMPLEMENT_ABSTRACT_CLASS2;wx/object.h|"
    "IMPLEMENT_CLASS;wx/object.h|"
    "IMPLEMENT_CLASS2;wx/object.h|"
    "IMPLEMENT_DYNAMIC_CLASS;wx/object.h|"
    "IMPLEMENT_DYNAMIC_CLASS2;wx/object.h|"
    "DECLARE_EVENT_TYPE;wx/event.h|"
    "DECLARE_EVENT_MACRO;wx/event.h|"
    "DECLARE_EVENT_TABLE_ENTRY;wx/event.h|"
    "DEFINE_EVENT_TYPE;wx/event.h|"
    "BEGIN_EVENT_TABLE;wx/event.h|"
    "END_EVENT_TABLE;wx/event.h|"
    "EVT_CUSTOM;wx/event.h|"
    "EVT_CUSTOM_RANGE;wx/event.h|"
    "EVT_COMMAND;wx/event.h|"
    "EVT_COMMAND_RANGE;wx/event.h|"
    "EVT_NOTIFY;wx/event.h|"
    "EVT_NOTIFY_RANGE;wx/event.h|"
    "EVT_BUTTON;wx/button.h|"
    "EVT_CHECKBOX;wx/checkbox.h|"
    "EVT_CHOICE;wx/choice.h|"
    "EVT_CHOICE;wx/choice.h|"
    "EVT_COMBOBOX;wx/combobox.h|"
    "EVT_LISTBOX;wx/listbox.h|"
    "EVT_LISTBOX_DCLICK;wx/listbox.h|"
    "EVT_RADIOBOX;wx/radiobox.h|"
    "EVT_RADIOBUTTON;wx/radiobut.h|"
    "EVT_SCROLLBAR;wx/scrolbar.h|"
    "EVT_SLIDER;wx/slider.h|"
    "EVT_TOGGLEBUTTON;wx/tglbtn.h|"
    "WX_APPEND_ARRAY;wx/dynarray.h|"
    "WX_PREPEND_ARRAY;wx/dynarray.h|"
    "WX_CLEAR_ARRAY;wx/dynarray.h|"
    "WX_DECLARE_OBJARRAY;wx/dynarray.h|"
    "WX_DEFINE_ARRAY;wx/dynarray.h|"
    "WX_DEFINE_OBJARRAY;wx/dynarray.h|"
    "WX_DEFINE_SORTED_ARRAY;wx/dynarray.h|"
    "WX_DECLARE_STRING_HASH_MAP;wx/hashmap.h|"
    "WX_DECLARE_HASH_MAP;wx/hashmap.h|"
    "wxASSERT;wx/debug.h|"
    "wxASSERT_MIN_BITSIZE;wx/debug.h|"
    "wxASSERT_MSG;wx/debug.h|"
    "wxBITMAP;wx/gdicmn.h|"
    "wxCOMPILE_TIME_ASSERT;wx/debug.h|"
    "wxCOMPILE_TIME_ASSERT2;wx/debug.h|"
    "wxCRIT_SECT_DECLARE;wx/thread.h|"
    "wxCRIT_SECT_DECLARE_MEMBER;wx/thread.h|"
    "wxCRIT_SECT_LOCKER;wx/thread.h|"
    "wxDYNLIB_FUNCTION;wx/dynlib.h|"
    "wxENTER_CRIT_SECT;wx/thread.h|"
    "wxFAIL;wx/debug.h|"
    "wxFAIL_MSG;wx/debug.h|"
    "wxICON;wx/gdicmn.h|"
    "wxLEAVE_CRIT_SECT;wx/thread.h|"
    "wxLL;wx/longlong.h|"
    "wxTRANSLATE;wx/intl.h|"
    "wxULL;wx/longlong.h|"

    // All ::wx methods
    "wxAboutBox;wx/aboutdlg.h|"
    "wxBeginBusyCursor;wx/utils.h|"
    "wxBell;wx/utils.h|"
    "wxClientDisplayRect;wx/gdicmn.h|"
    "wxClipboardOpen;wx/clipbrd.h|"
    "wxCloseClipboard;wx/clipbrd.h|"
    "wxColourDisplay;wx/gdicmn.h|"
    "wxConcatFiles;wx/filefn.h|"
    "wxCopyFile;wx/filefn.h|"
    "wxCreateDynamicObject;wx/object.h|"
    "wxCreateFileTipProvider;wx/tipdlg.h|"
    "wxDDECleanUp;wx/dde.h|"
    "wxDDEInitialize;wx/dde.h|"
    "wxDebugMsg;wx/utils.h|"
    "wxDirExists;wx/filefn.h|"
    "wxDirSelector;wx/dirdlg.h|"
    "wxDisplayDepth;wx/gdicmn.h|"
    "wxDisplaySize;wx/gdicmn.h|"
    "wxDisplaySizeMM;wx/gdicmn.h|"
    "wxDos2UnixFilename;wx/filefn.h|"
    "wxDROP_ICON;wx/dnd.h|"
    "wxEmptyClipboard;wx/clipbrd.h|"
    "wxEnableTopLevelWindows;wx/utils.h|"
    "wxEndBusyCursor;wx/utils.h|"
    "wxEntry;wx/app.h|"
    "wxEntryCleanup;wx/init.h|"
    "wxEntryStart;wx/init.h|"
    "wxEnumClipboardFormats;wx/clipbrd.h|"
    "wxError;wx/utils.h|"
    "wxExecute;wx/utils.h|"
    "wxExit;wx/app.h|"
    "wxFatalError;wx/utils.h|"
    "wxFileExists;wx/filefn.h|"
    "wxFileModificationTime;wx/filefn.h|"
    "wxFileNameFromPath;wx/filefn.h|"
    "wxFileSelector;wx/filedlg.h|"
    "wxFindFirstFile;wx/filefn.h|"
    "wxFindMenuItemId;wx/utils.h|"
    "wxFindNextFile;wx/filefn.h|"
    "wxFindWindowAtPoint;wx/utils.h|"
    "wxFindWindowAtPointer;wx/windows.h|"
    "wxFindWindowByLabel;wx/utils.h|"
    "wxFindWindowByName;wx/utils.h|"
    "wxGenericAboutBox;wx/aboutdlg.h\nwx/generic/aboutdlgg.h|"
    "wxGetActiveWindow;wx/windows.h|"
    "wxGetApp;wx/app.h|"
    "wxGetBatteryState;wx/utils.h|"
    "wxGetClipboardData;wx/clipbrd.h|"
    "wxGetClipboardFormatName;wx/clipbrd.h|"
    "wxGetColourFromUser;wx/colordlg.h|"
    "wxGetCwd;wx/filefn.h|"
    "wxGetDiskSpace;wx/filefn.h|"
    "wxGetDisplayName;wx/utils.h|"
    "wxGetElapsedTime;wx/timer.h|"
    "wxGetEmailAddress;wx/utils.h|"
    "wxGetFileKind;wx/filefn.h|"
    "wxGetFontFromUser;wx/fontdlg.h|"
    "wxGetFreeMemory;wx/utils.h|"
    "wxGetFullHostName;wx/utils.h|"
    "wxGetHomeDir;wx/utils.h|"
    "wxGetHostName;wx/utils.h|"
    "wxGetKeyState;wx/utils.h|"
    "wxGetLocalTime;wx/timer.h|"
    "wxGetLocalTimeMillis;wx/timer.h|"
    "wxGetMousePosition;wx/utils.h|"
    "wxGetMouseState;wx/utils.h|"
    "wxGetMultipleChoice;wx/choicdlg.h|"
    "wxGetMultipleChoices;wx/choicdlg.h|"
    "wxGetNumberFromUser;wx/numdlg.h|"
    "wxGetOsDescription;wx/utils.h|"
    "wxGetOSDirectory;wx/filefn.h|"
    "wxGetOsVersion;wx/utils.h|"
    "wxGetPasswordFromUser;wx/textdlg.h|"
    "wxGetPowerType;wx/utils.h|"
    "wxGetPrinterCommand;wx/dcps.h|"
    "wxGetPrinterFile;wx/dcps.h|"
    "wxGetPrinterMode;wx/dcps.h|"
    "wxGetPrinterOptions;wx/dcps.h|"
    "wxGetPrinterOrientation;wx/dcps.h|"
    "wxGetPrinterPreviewCommand;wx/dcps.h|"
    "wxGetPrinterScaling;wx/dcps.h|"
    "wxGetPrinterTranslation;wx/dcps.h|"
    "wxGetProcessId;wx/utils.h|"
    "wxGetResource;wx/utils.h|"
    "wxGetSingleChoice;wx/choicdlg.h|"
    "wxGetSingleChoiceData;wx/choicdlg.h|"
    "wxGetSingleChoiceIndex;wx/choicdlg.h|"
    "wxGetStockLabel;wx/stockitem.h|"
    "wxGetTempFileName;wx/filefn.h|"
    "wxGetTextFromUser;wx/textdlg.h|"
    "wxGetTopLevelParent;wx/window.h|"
    "wxGetTranslation;wx/intl.h|"
    "wxGetUserHome;wx/utils.h|"
    "wxGetUserId;wx/utils.h|"
    "wxGetUserName;wx/utils.h|"
    "wxGetUTCTime;wx/timer.h|"
    "wxGetWorkingDirectory;wx/filefn.h|"
    "wxHandleFatalExceptions;wx/app.h|"
    "wxInitAllImageHandlers;wx/image.h|"
    "wxInitialize;wx/app.h|"
    "wxIsAbsolutePath;wx/filefn.h|"
    "wxIsBusy;wx/utils.h|"
    "wxIsClipboardFormatAvailable;wx/clipbrd.h|"
    "wxIsDebuggerRunning;wx/debug.h|"
    "wxIsEmpty;wx/wxchar.h|"
    "wxIsMainThread;wx/thread.h|"
    "wxIsPlatform64Bit;wx/utils.h|"
    "wxIsPlatformLittleEndian;wx/utils.h|"
    "wxIsWild;wx/filefn.h|"
    "wxKill;wx/app.h|"
    "wxLaunchDefaultBrowser;wx/utils.h|"
    "wxLoadUserResource;wx/utils.h|"
    "wxLogDebug;wx/log.h|"
    "wxLogError;wx/log.h|"
    "wxLogFatalError;wx/log.h|"
    "wxLogMessage;wx/log.h|"
    "wxLogStatus;wx/log.h|"
    "wxLogSysError;wx/log.h|"
    "wxLogTrace;wx/log.h|"
    "wxLogVerbose;wx/log.h|"
    "wxLogWarning;wx/log.h|"
    "wxMakeMetafilePlaceable;wx/gdicmn.h|"
    "wxMatchWild;wx/filefn.h|"
    "wxMessageBox;wx/msgdlg.h|"
    "wxMicroSleep;wx/utils.h|"
    "wxMilliSleep;wx/utils.h|"
    "wxMkdir;wx/filefn.h|"
    "wxMutexGuiEnter;wx/thread.h|"
    "wxMutexGuiLeave;wx/thread.h|"
    "wxNewId;wx/utils.h|"
    "wxNow;wx/utils.h|"
    "wxOnAssert;wx/debug.h|"
    "wxOpenClipboard;wx/clipbrd.h|"
    "wxParseCommonDialogsFilter;wx/filefn.h|"
    "wxPathOnly;wx/filefn.h|"
    "wxPostDelete;wx/utils.h|"
    "wxPostEvent;wx/app.h|"
    "wxRegisterClipboardFormat;wx/clipbrd.h|"
    "wxRegisterId;wx/utils.h|"
    "wxRemoveFile;wx/filefn.h|"
    "wxRenameFile;wx/filefn.h|"
    "wxRmdir;wx/filefn.h|"
    "wxSafeShowMessage;wx/log.h|"
    "wxSafeYield;wx/utils.h|"
    "wxSetClipboardData;wx/clipbrd.h|"
    "wxSetCursor;wx/gdicmn.h|"
    "wxSetDisplayName;wx/utils.h|"
    "wxSetPrinterCommand;wx/dcps.h|"
    "wxSetPrinterFile;wx/dcps.h|"
    "wxSetPrinterMode;wx/dcps.h|"
    "wxSetPrinterOptions;wx/dcps.h|"
    "wxSetPrinterOrientation;wx/dcps.h|"
    "wxSetPrinterPreviewCommand;wx/dcps.h|"
    "wxSetPrinterScaling;wx/dcps.h|"
    "wxSetPrinterTranslation;wx/dcps.h|"
    "wxSetWorkingDirectory;wx/filefn.h|"
    "wxShell;wx/utils.h|"
    "wxShowTip;wx/tipdlg.h|"
    "wxShutdown;wx/utils.h|"
    "wxSleep;wx/utils.h|"
    "wxSnprintf;wx/wxchar.h|"
    "wxSplitPath;wx/filefn.h|"
    "wxStartTimer;wx/timer.h|"
    "wxStrcmp;wx/wxchar.h|"
    "wxStricmp;wx/wxchar.h|"
    "wxStringEq;wx/string.h|"
    "wxStringMatch;wx/string.h|"
    "wxStringTokenize;wx/string.h|"
    "wxStripMenuCodes;wx/utils.h|"
    "wxStrlen;wx/wxchar.h|"
    "wxSysErrorCode;wx/log.h|"
    "wxSysErrorMsg;wx/log.h|"
    "wxTrace;wx/memory.h|"
    "wxTraceLevel;wx/memory.h|"
    "wxTransferFileToStream;wx/docview.h|"
    "wxTransferStreamToFile;wx/docview.h|"
    "wxTrap;wx/debug.h|"
    "wxUninitialize;wx/app.h|"
    "wxUnix2DosFilename;wx/filefn.h|"
    "wxUsleep;wx/utils.h|"
    "wxVsnprintf;wx/wxchar.h|"
    "wxWakeUpIdle;wx/app.h|"
    "wxWriteResource;wx/utils.h|"
    "wxYield;wx/app.h|"

    // All ::wx classes
    "wxAboutDialogInfo;wx/aboutdlg.h|"
    "wxAcceleratorEntry;wx/accel.h|"
    "wxAcceleratorTable;wx/accel.h|"
    "wxAccessible;wx/access.h|"
    "wxActivateEvent;wx/event.h|"
    "wxActiveXContainer;wx/msw/ole/activex.h|"
    "wxActiveXEvent;wx/msw/ole/activex.h|"
    "wxAnimation;wx/animate.h|"
    "wxAnimationCtrl;wx/animate.h|"
    "wxApp;wx/app.h|"
    "wxAppTraits;wx/apptrait.h|"
    "wxArchiveClassFactory;wx/archive.h|"
    "wxArchiveEntry;wx/archive.h|"
    "wxArchiveInputStream;wx/archive.h|"
    "wxArchiveIterator;wx/archive.h|"
    "wxArchiveNotifier;wx/archive.h|"
    "wxArchiveOutputStream;wx/archive.h|"
    "wxArray;wx/dynarray.h|"
    "wxArrayString;wx/arrstr.h|"
    "wxArtProvider;wx/artprov.h|"
    "wxAuiDockArt;wx/aui/dockart.h|"
    "wxAuiTabArt;wx/aui/auibook.h|"
    "wxAuiManager;wx/aui/aui.h|"
    "wxAuiNotebook;wx/aui/auibook.h|"
    "wxAuiPaneInfo;wx/aui/aui.h|"
    "wxAutomationObject;wx/msw/ole/automtn.h|"
    "wxBitmap;wx/bitmap.h|"
    "wxBitmapButton;wx/bmpbuttn.h|"
    "wxBitmapComboBox;wx/bmpcbox.h|"
    "wxBitmapDataObject;wx/dataobj.h|"
    "wxBitmapHandler;wx/bitmap.h|"
    "wxBoxSizer;wx/sizer.h|"
    "wxBrush;wx/brush.h|"
    "wxBrushList;wx/gdicmn.h|"
    "wxBufferedDC;wx/dcbuffer.h|"
    "wxBufferedInputStream;wx/stream.h|"
    "wxBufferedOutputStream;wx/stream.h|"
    "wxBufferedPaintDC;wx/dcbuffer.h|"
    "wxBusyCursor;wx/utils.h|"
    "wxBusyInfo;wx/busyinfo.h|"
    "wxButton;wx/button.h|"
    "wxCalculateLayoutEvent;wx/laywin.h|"
    "wxCalendarCtrl;wx/calctrl.h|"
    "wxCalendarDateAttr;wx/calctrl.h|"
    "wxCalendarEvent;wx/calctrl.h|"
    "wxCaret;wx/caret.h|"
    "wxCheckBox;wx/checkbox.h|"
    "wxCheckListBox;wx/checklst.h|"
    "wxChildFocusEvent;wx/event.h|"
    "wxChoice;wx/choice.h|"
    "wxChoicebook;wx/choicebk.h|"
    "wxClassInfo;wx/object.h|"
    "wxClient;wx/ipc.h|"
    "wxClientData;wx/clntdata.h|"
    "wxClientDataContainer;wx/clntdata.h|"
    "wxClientDC;wx/dcclient.h|"
    "wxClipboard;wx/clipbrd.h|"
    "wxClipboardTextEvent;wx/event.h|"
    "wxCloseEvent;wx/event.h|"
    "wxCmdLineParser;wx/cmdline.h|"
    "wxCollapsiblePane;wx/collpane.h|"
    "wxCollapsiblePaneEvent;wx/collpane.h|"
    "wxColour;wx/colour.h|"
    "wxColourData;wx/cmndata.h|"
    "wxColourDatabase;wx/gdicmn.h|"
    "wxColourDialog;wx/colordlg.h|"
    "wxColourPickerCtrl;wx/clrpicker.h|"
    "wxColourPickerEvent;wx/clrpicker.h|"
    "wxComboBox;wx/combobox.h|"
    "wxComboCtrl;wx/combo.h|"
    "wxComboPopup;wx/combo.h|"
    "wxCommand;wx/cmdproc.h|"
    "wxCommandEvent;wx/event.h|"
    "wxCommandProcessor;wx/cmdproc.h|"
    "wxCondition;wx/thread.h|"
    "wxConfigBase;wx/config.h|"
    "wxConnection;wx/ipc.h|"
    "wxContextHelp;wx/cshelp.h|"
    "wxContextHelpButton;wx/cshelp.h|"
    "wxContextMenuEvent;wx/event.h|"
    "wxControl;wx/control.h|"
    "wxControlWithItems;wx/ctrlsub.h|"
    "wxCountingOutputStream;wx/stream.h|"
    "wxCriticalSection;wx/thread.h|"
    "wxCriticalSectionLocker;wx/thread.h|"
    "wxCSConv;wx/strconv.h|"
    "wxCursor;wx/cursor.h|"
    "wxCustomDataObject;wx/dataobj.h|"
    "wxDataFormat;wx/dataobj.h|"
    "wxDatagramSocket;wx/socket.h|"
    "wxDataInputStream;wx/datstrm.h|"
    "wxDataObject;wx/dataobj.h|"
    "wxDataObjectComposite;wx/dataobj.h|"
    "wxDataObjectSimple;wx/dataobj.h|"
    "wxDataOutputStream;wx/datstrm.h|"
    "wxDataViewColumn;wx/dataview.h|"
    "wxDataViewCtrl;wx/dataview.h|"
    "wxDataViewEvent;wx/dataview.h|"
    "wxDataViewListModelNotifier;wx/dataview.h|"
    "wxDataViewModel;wx/dataview.h|"
    "wxDataViewListModel;wx/dataview.h|"
    "wxDataViewSortedListModel;wx/dataview.h|"
    "wxDataViewRenderer;wx/dataview.h|"
    "wxDataViewTextRenderer;wx/dataview.h|"
    "wxDataViewProgressRenderer;wx/dataview.h|"
    "wxDataViewToggleRenderer;wx/dataview.h|"
    "wxDataViewBitmapRenderer;wx/dataview.h|"
    "wxDataViewDateRenderer;wx/dataview.h|"
    "wxDataViewCustomRenderer;wx/dataview.h|"
    "wxDateEvent;wx/dateevt.h|"
    "wxDatePickerCtrl;wx/datectrl.h|"
    "wxDateSpan;wx/datetime.h|"
    "wxDateTime;wx/datetime.h|"
    "wxDb;wx/db.h|"
    "wxDbColDataPtr;wx/db.h|"
    "wxDbColDef;wx/db.h|"
    "wxDbColFor;wx/db.h|"
    "wxDbColInf;wx/db.h|"
    "wxDbConnectInf;wx/db.h|"
    "wxDbGridColInfo;wx/dbgrid.h|"
    "wxDbGridTableBase;wx/dbgrid.h|"
    "wxDbIdxDef;wx/db.h|"
    "wxDbInf;wx/db.h|"
    "wxDbTable;wx/dbtable.h|"
    "wxDbTableInf;wx/db.h|"
    "wxDC;wx/dc.h|"
    "wxDCClipper;wx/dc.h|"
    "wxDDEClient;wx/dde.h|"
    "wxDDEConnection;wx/dde.h|"
    "wxDDEServer;wx/dde.h|"
    "wxDebugContext;wx/memory.h|"
    "wxDebugReport;wx/debugrpt.h|"
    "wxDebugReportCompress;wx/debugrpt.h|"
    "wxDebugReportPreview;wx/debugrpt.h|"
    "wxDebugReportPreviewStd;wx/debugrpt.h|"
    "wxDebugReportUpload;wx/debugrpt.h|"
    "wxDebugStreamBuf;wx/memory.h|"
    "wxDelegateRendererNative;wx/renderer.h|"
    "wxDialog;wx/dialog.h|"
    "wxDialUpEvent;wx/dialup.h|"
    "wxDialUpManager;wx/dialup.h|"
    "wxDir;wx/dir.h|"
    "wxDirDialog;wx/dirdlg.h|"
    "wxDirPickerCtrl;wx/filepicker.h|"
    "wxDirTraverser;wx/dir.h|"
    "wxDisplay;wx/display.h|"
    "wxDllLoader;wx/dynlib.h|"
    "wxDocChildFrame;wx/docview.h|"
    "wxDocManager;wx/docview.h|"
    "wxDocMDIChildFrame;wx/docmdi.h|"
    "wxDocMDIParentFrame;wx/docmdi.h|"
    "wxDocParentFrame;wx/docview.h|"
    "wxDocTemplate;wx/docview.h|"
    "wxDocument;wx/docview.h|"
    "wxDragImage;wx/dragimag.h|"
    "wxDragResult;wx/dnd.h|"
    "wxDropFilesEvent;wx/event.h|"
    "wxDropSource;wx/dnd.h|"
    "wxDropTarget;wx/dnd.h|"
    "wxDynamicLibrary;wx/dynlib.h|"
    "wxDynamicLibraryDetails;wx/dynlib.h|"
    "wxEncodingConverter;wx/encconv.h|"
    "wxEraseEvent;wx/event.h|"
    "wxEvent;wx/event.h|"
    "wxEvtHandler;wx/event.h|"
    "wxFFile;wx/ffile.h|"
    "wxFFileInputStream;wx/wfstream.h|"
    "wxFFileOutputStream;wx/wfstream.h|"
    "wxFFileStream;wx/wfstream.h|"
    "wxFile;wx/file.h|"
    "wxFileConfig;wx/fileconf.h|"
    "wxFileDataObject;wx/dataobj.h|"
    "wxFileDialog;wx/filedlg.h|"
    "wxFileDropTarget;wx/dnd.h|"
    "wxFileHistory;wx/docview.h|"
    "wxFileInputStream;wx/wfstream.h|"
    "wxFileName;wx/filename.h|"
    "wxFileOutputStream;wx/wfstream.h|"
    "wxFilePickerCtrl;wx/filepicker.h|"
    "wxFileDirPickerEvent;wx/filepicker.h|"
    "wxFileStream;wx/wfstream.h|"
    "wxFileSystem;wx/filesys.h|"
    "wxFileSystemHandler;wx/filesys.h|"
    "wxFileType;wx/mimetype.h|"
    "wxFilterClassFactory;wx/stream.h|"
    "wxFilterInputStream;wx/stream.h|"
    "wxFilterOutputStream;wx/stream.h|"
    "wxFindDialogEvent;wx/fdrepdlg.h|"
    "wxFindReplaceData;wx/fdrepdlg.h|"
    "wxFindReplaceDialog;wx/fdrepdlg.h|"
    "wxFinite;wx/math.h|"
    "wxFlexGridSizer;wx/sizer.h|"
    "wxFocusEvent;wx/event.h|"
    "wxFont;wx/font.h|"
    "wxFontData;wx/cmndata.h|"
    "wxFontDialog;wx/fontdlg.h|"
    "wxFontEnumerator;wx/fontenum.h|"
    "wxFontList;wx/gdicmn.h|"
    "wxFontMapper;wx/fontmap.h|"
    "wxFontPickerCtrl;wx/fontpicker.h|"
    "wxFontPickerEvent;wx/fontpicker.h|"
    "wxFrame;wx/frame.h|"
    "wxFSFile;wx/filesys.h|"
    "wxFTP;wx/protocol/ftp.h|"
    "wxGauge;wx/gauge.h|"
    "wxGBPosition;wx/gbsizer.h|"
    "wxGBSizerItem;wx/gbsizer.h|"
    "wxGBSpan;wx/gbsizer.h|"
    "wxGDIObject;wx/gdiobj.h|"
    "wxGenericDirCtrl;wx/dirctrl.h|"
    "wxGenericValidator;wx/valgen.h|"
    "wxGetenv;wx/utils.h|"
    "wxGetVariantCast;wx/variant.h|"
    "wxGLCanvas;wx/glcanvas.h|"
    "wxGLContext;wx/glcanvas.h|"
    "wxGraphicsBrush;wx/graphics.h|"
    "wxGraphicsContext;wx/graphics.h|"
    "wxGraphicsFont;wx/graphics.h|"
    "wxGraphicsMatrix;wx/graphics.h|"
    "wxGraphicsObject;wx/graphics.h|"
    "wxGraphicsPath;wx/graphics.h|"
    "wxGraphicsPen;wx/graphics.h|"
    "wxGraphicsRenderer;wx/graphics.h|"
    "wxGrid;wx/grid.h|"
    "wxGridBagSizer;wx/gbsizer.h|"
    "wxGridCellAttr;wx/grid.h|"
    "wxGridCellBoolEditor;wx/grid.h|"
    "wxGridCellBoolRenderer;wx/grid.h|"
    "wxGridCellChoiceEditor;wx/grid.h|"
    "wxGridCellEditor;wx/grid.h|"
    "wxGridCellRenderer;wx/grid.h|"
    "wxGridCellFloatEditor;wx/grid.h|"
    "wxGridCellFloatRenderer;wx/grid.h|"
    "wxGridCellNumberEditor;wx/grid.h|"
    "wxGridCellNumberRenderer;wx/grid.h|"
    "wxGridCellStringRenderer;wx/grid.h|"
    "wxGridCellTextEditor;wx/grid.h|"
    "wxGridEditorCreatedEvent;wx/grid.h|"
    "wxGridEvent;wx/grid.h|"
    "wxGridRangeSelectEvent;wx/grid.h|"
    "wxGridSizeEvent;wx/grid.h|"
    "wxGridSizer;wx/sizer.h|"
    "wxGridTableBase;wx/grid.h|"
    "wxHashMap;wx/hashmap.h|"
    "wxHashSet;wx/hashset.h|"
    "wxHashTable;wx/hash.h|"
    "wxHelpController;wx/help.h|"
    "wxHelpControllerHelpProvider;wx/cshelp.h|"
    "wxHelpEvent;wx/event.h|"
    "wxHelpProvider;wx/cshelp.h|"
    "wxHtmlCell;wx/html/htmlcell.h|"
    "wxHtmlCellEvent;wx/html/htmlwin.h|"
    "wxHtmlColourCell;wx/html/htmlcell.h|"
    "wxHtmlContainerCell;wx/html/htmlcell.h|"
    "wxHtmlDCRenderer;wx/html/htmprint.h|"
    "wxHtmlEasyPrinting;wx/html/htmprint.h|"
    "wxHtmlFilter;wx/html/htmlfilt.h|"
    "wxHtmlHelpController;wx/html/helpctrl.h|"
    "wxHtmlHelpData;wx/html/helpdata.h|"
    "wxHtmlHelpDialog;wx/html/helpdlg.h|"
    "wxHtmlHelpFrame;wx/html/helpfrm.h|"
    "wxHtmlHelpWindow;wx/html/helpwnd.h|"
    "wxHtmlModalHelp;wx/html/helpctrl.h|"
    "wxHtmlLinkInfo;wx/html/htmlcell.h|"
    "wxHtmlLinkEvent;wx/html/htmlwin.h|"
    "wxHtmlListBox;wx/htmllbox.h|"
    "wxHtmlParser;wx/html/htmlpars.h|"
    "wxHtmlPrintout;wx/html/htmprint.h|"
    "wxHtmlTag;wx/html/htmltag.h|"
    "wxHtmlTagHandler;wx/html/htmlpars.h|"
    "wxHtmlTagsModule;wx/html/winpars.h|"
    "wxHtmlWidgetCell;wx/html/htmlcell.h|"
    "wxHtmlWindow;wx/html/htmlwin.h|"
    "wxHtmlWinParser;wx/html/winpars.h|"
    "wxHtmlWinTagHandler;wx/html/winpars.h|"
    "wxHTTP;wx/protocol/http.h|"
    "wxHyperlinkCtrl;wx/hyperlink.h|"
    "wxHyperlinkEvent;wx/hyperlink.h|"
    "wxIcon;wx/icon.h|"
    "wxIconBundle;wx/iconbndl.h|"
    "wxIconizeEvent;wx/event.h|"
    "wxIconLocation;wx/iconloc.h|"
    "wxIdleEvent;wx/event.h|"
    "wxImage;wx/image.h|"
    "wxImageHandler;wx/image.h|"
    "wxImageList;wx/imaglist.h|"
    "wxIndividualLayoutConstraint;wx/layout.h|"
    "wxInitDialogEvent;wx/event.h|"
    "wxInputStream;wx/stream.h|"
    "wxIPaddress;wx/socket.h|"
    "wxIPV4address;wx/socket.h|"
    "wxIsNaN;wx/math.h|"
    "wxJoystick;wx/joystick.h|"
    "wxJoystickEvent;wx/event.h|"
    "wxKeyEvent;wx/event.h|"
    "wxLayoutAlgorithm;wx/laywin.h|"
    "wxLayoutConstraints;wx/layout.h|"
    "wxList;wx/list.h|"
    "wxListbook;wx/listbook.h|"
    "wxListBox;wx/listbox.h|"
    "wxListCtrl;wx/listctrl.h|"
    "wxListEvent;wx/listctrl.h|"
    "wxListItem;wx/listctrl.h|"
    "wxListItemAttr;wx/listctrl.h|"
    "wxListView;wx/listctrl.h|"
    "wxLocale;wx/intl.h|"
    "wxLog;wx/log.h|"
    "wxLogChain;wx/log.h|"
    "wxLogGui;wx/log.h|"
    "wxLogNull;wx/log.h|"
    "wxLogPassThrough;wx/log.h|"
    "wxLogStderr;wx/log.h|"
    "wxLogStream;wx/log.h|"
    "wxLogTextCtrl;wx/log.h|"
    "wxLogWindow;wx/log.h|"
    "wxLongLong;wx/longlong.h|"
    "wxLongLongFmtSpec;wx/longlong.h|"
    "wxMask;wx/bitmap.h|"
    "wxMaximizeEvent;wx/event.h|"
    "wxMBConv;wx/strconv.h|"
    "wxMBConvFile;wx/strconv.h|"
    "wxMBConvUTF16;wx/strconv.h|"
    "wxMBConvUTF32;wx/strconv.h|"
    "wxMBConvUTF7;wx/strconv.h|"
    "wxMBConvUTF8;wx/strconv.h|"
    "wxMDIChildFrame;wx/mdi.h|"
    "wxMDIClientWindow;wx/mdi.h|"
    "wxMDIParentFrame;wx/mdi.h|"
    "wxMediaCtrl;wx/mediactrl.h|"
    "wxMediaEvent;wx/mediactrl.h|"
    "wxMemoryBuffer;wx/buffer.h|"
    "wxMemoryDC;wx/dcmemory.h|"
    "wxMemoryFSHandler;wx/fs_mem.h|"
    "wxMemoryInputStream;wx/mstream.h|"
    "wxMemoryOutputStream;wx/mstream.h|"
    "wxMenu;wx/menu.h|"
    "wxMenuBar;wx/menu.h|"
    "wxMenuEvent;wx/event.h|"
    "wxMenuItem;wx/menuitem.h|"
    "wxMessageDialog;wx/msgdlg.h|"
    "wxMetafile;wx/metafile.h|"
    "wxMetafileDC;wx/metafile.h|"
    "wxMimeTypesManager;wx/mimetype.h|"
    "wxMiniFrame;wx/minifram.h|"
    "wxMirrorDC;wx/dcmirror.h|"
    "wxModule;wx/module.h|"
    "wxMouseCaptureChangedEvent;wx/event.h|"
    "wxMouseCaptureLostEvent;wx/event.h|"
    "wxMouseEvent;wx/event.h|"
    "wxMoveEvent;wx/event.h|"
    "wxMultiChoiceDialog;wx/choicdlg.h|"
    "wxMutex;wx/thread.h|"
    "wxMutexLocker;wx/thread.h|"
    "wxNode;wx/list.h|"
    "wxNotebook;wx/notebook.h|"
    "wxNotebookEvent;wx/notebook.h|"
    "wxNotebookSizer;wx/sizer.h|"
    "wxNotifyEvent;wx/event.h|"
    "wxObjArray;wx/dynarray.h|"
    "wxObject;wx/object.h|"
    "wxObjectRefData;wx/object.h|"
    "wxOpenErrorTraverser;wx/dir.h|"
    "wxOutputStream;wx/stream.h|"
    "wxOwnerDrawnComboBox;wx/odcombo.h|"
    "wxPageSetupDialog;wx/printdlg.h|"
    "wxPageSetupDialogData;wx/cmndata.h|"
    "wxPaintDC;wx/dcclient.h|"
    "wxPaintEvent;wx/event.h|"
    "wxPalette;wx/palette.h|"
    "wxPanel;wx/panel.h|"
    "wxPaperSize;wx/cmndata.h|"
    "wxPasswordEntryDialog;wx/textdlg.h|"
    "wxPathList;wx/filefn.h|"
    "wxPen;wx/pen.h|"
    "wxPenList;wx/gdicmn.h|"
    "wxPickerBase;wx/pickerbase.h|"
    "wxPlatformInfo;wx/platinfo.h|"
    "wxPoint;wx/gdicmn.h|"
    "wxPostScriptDC;wx/dcps.h|"
    "wxPowerEvent;wx/power.h|"
    "wxPreviewCanvas;wx/print.h|"
    "wxPreviewControlBar;wx/print.h|"
    "wxPreviewFrame;wx/print.h|"
    "wxPrintData;wx/cmndata.h|"
    "wxPrintDialog;wx/printdlg.h|"
    "wxPrintDialogData;wx/cmndata.h|"
    "wxPrinter;wx/print.h|"
    "wxPrinterDC;wx/dcprint.h|"
    "wxPrintout;wx/print.h|"
    "wxPrintPreview;wx/print.h|"
    "wxProcess;wx/process.h|"
    "wxProcessEvent;wx/process.h|"
    "wxProgressDialog;wx/progdlg.h|"
    "wxPropertySheetDialog;wx/propdlg.h|"
    "wxProtocol;wx/protocol/protocol.h|"
    "wxQuantize;wx/quantize.h|"
    "wxQueryLayoutInfoEvent;wx/laywin.h|"
    "wxRadioBox;wx/radiobox.h|"
    "wxRadioButton;wx/radiobut.h|"
    "wxRealPoint;wx/gdicmn.h|"
    "wxRect;wx/gdicmn.h|"
    "wxRecursionGuard;wx/recguard.h|"
    "wxRecursionGuardFlag;wx/recguard.h|"
    "wxRegEx;wx/regex.h|"
    "wxRegion;wx/region.h|"
    "wxRegionIterator;wx/region.h|"
    "wxRegKey;wx/msw/registry.h|"
    "wxRendererNative;wx/renderer.h|"
    "wxRendererVersion;wx/renderer.h|"
    "wxRichTextAttr;wx/richtext/richtextbuffer.h|"
    "wxRichTextBuffer;wx/richtext/richtextbuffer.h|"
    "wxRichTextCharacterStyleDefinition;wx/richtext/richtextstyles.h|"
    "wxRichTextCtrl;wx/richtext/richtextctrl.h|"
    "wxRichTextEvent;wx/richtext/richtextctrl.h|"
    "wxRichTextFileHandler;wx/richtext/richtextbuffer.h|"
    "wxRichTextFormattingDialog;wx/richtext/richtextformatdlg.h|"
    "wxRichTextFormattingDialogFactory;wx/richtext/richtextformatdlg.h|"
    "wxRichTextHeaderFooterData;wx/richtext/richtextprint.h|"
    "wxRichTextHTMLHandler;wx/richtext/richtexthtml.h|"
    "wxRichTextListStyleDefinition;wx/richtext/richtextstyles.h|"
    "wxRichTextParagraphStyleDefinition;wx/richtext/richtextstyles.h|"
    "wxRichTextPrinting;wx/richtext/richtextprint.h|"
    "wxRichTextPrintout;wx/richtext/richtextprint.h|"
    "wxRichTextRange;wx/richtext/richtextbuffer.h|"
    "wxRichTextStyleDefinition;wx/richtext/richtextstyles.h|"
    "wxRichTextStyleComboCtrl;wx/richtext/richtextstyles.h|"
    "wxRichTextStyleListBox;wx/richtext/richtextstyles.h|"
    "wxRichTextStyleListCtrl;wx/richtext/richtextstyles.h|"
    "wxRichTextStyleOrganiserDialog;wx/richtext/richtextstyledlg.h|"
    "wxRichTextStyleSheet;wx/richtext/richtextstyles.h|"
    "wxRichTextXMLHandler;wx/richtext/richtextxml.h|"
    "wxSashEvent;wx/sashwin.h|"
    "wxSashLayoutWindow;wx/laywin.h|"
    "wxSashWindow;wx/sashwin.h|"
    "wxScopedArray;wx/ptr_scpd.h|"
    "wxScopedPtr;wx/ptr_scpd.h|"
    "wxScopedTiedPtr;wx/ptr_scpd.h|"
    "wxScreenDC;wx/dcscreen.h|"
    "wxScrollBar;wx/scrolbar.h|"
    "wxScrolledWindow;wx/scrolwin.h|"
    "wxScrollEvent;wx/event.h|"
    "wxScrollWinEvent;wx/event.h|"
    "wxSearchCtrl;wx/srchctrl.h|"
    "wxSemaphore;wx/thread.h|"
    "wxServer;wx/ipc.h|"
    "wxSetCursorEvent;wx/event.h|"
    "wxSetEnv;wx/utils.h|"
    "wxSimpleHelpProvider;wx/cshelp.h|"
    "wxSimpleHtmlListBox;wx/htmllbox.h|"
    "wxSingleChoiceDialog;wx/choicdlg.h|"
    "wxSingleInstanceChecker;wx/snglinst.h|"
    "wxSize;wx/gdicmn.h|"
    "wxSizeEvent;wx/event.h|"
    "wxSizer;wx/sizer.h|"
    "wxSizerFlags;wx/sizer.h|"
    "wxSizerItem;wx/sizer.h|"
    "wxSlider;wx/slider.h|"
    "wxSockAddress;wx/socket.h|"
    "wxSocketBase;wx/socket.h|"
    "wxSocketClient;wx/socket.h|"
    "wxSocketEvent;wx/socket.h|"
    "wxSocketInputStream;wx/sckstrm.h|"
    "wxSocketOutputStream;wx/sckstrm.h|"
    "wxSocketServer;wx/socket.h|"
    "wxSound;wx/sound.h|"
    "wxSpinButton;wx/spinbutt.h|"
    "wxSpinCtrl;wx/spinctrl.h|"
    "wxSpinEvent;wx/spinctrl.h|"
    "wxSplashScreen;wx/splash.h|"
    "wxSplitterEvent;wx/splitter.h|"
    "wxSplitterRenderParams;wx/renderer.h|"
    "wxSplitterWindow;wx/splitter.h|"
    "wxStackFrame;wx/stackwalk.h|"
    "wxStackWalker;wx/stackwalk.h|"
    "wxStandardPaths;wx/stdpaths.h|"
    "wxStaticBitmap;wx/statbmp.h|"
    "wxStaticBox;wx/statbox.h|"
    "wxStaticBoxSizer;wx/sizer.h|"
    "wxStaticLine;wx/statline.h|"
    "wxStaticText;wx/stattext.h|"
    "wxStatusBar;wx/statusbr.h|"
    "wxStdDialogButtonSizer;wx/sizer.h|"
    "wxStopWatch;wx/stopwatch.h|"
    "wxStreamBase;wx/stream.h|"
    "wxStreamBuffer;wx/stream.h|"
    "wxStreamToTextRedirector;wx/textctrl.h|"
    "wxString;wx/string.h|"
    "wxStringBuffer;wx/string.h|"
    "wxStringBufferLength;wx/string.h|"
    "wxStringClientData;clntdata.h|"
    "wxStringInputStream;wx/sstream.h|"
    "wxStringOutputStream;wx/sstream.h|"
    "wxStringTokenizer;wx/tokenzr.h|"
    "wxSymbolPickerDialog;wx/richtext/richtextsymboldlg.h|"
    "wxSysColourChangedEvent;wx/event.h|"
    "wxSystemOptions;wx/sysopt.h|"
    "wxSystemSettings;wx/settings.h|"
    "wxTarClassFactory;wx/tarstrm.h|"
    "wxTarEntry;wx/tarstrm.h|"
    "wxTarInputStream;wx/tarstrm.h|"
    "wxTarOutputStream;wx/tarstrm.h|"
    "wxTaskBarIcon;wx/taskbar.h|"
    "wxTCPClient;wx/sckipc.h|"
    "wxTCPServer;wx/sckipc.h|"
    "wxTempFile;wx/file.h|"
    "wxTempFileOutputStream;wx/wfstream.h|"
    "wxTextAttr;wx/textctrl.h|"
    "wxTextAttrEx;wx/richtext/richtextbuffer.h|"
    "wxTextCtrl;wx/textctrl.h|"
    "wxTextDataObject;wx/dataobj.h|"
    "wxTextDropTarget;wx/dnd.h|"
    "wxTextEntryDialog;wx/textdlg.h|"
    "wxTextFile;wx/textfile.h|"
    "wxTextInputStream;wx/txtstrm.h|"
    "wxTextOutputStream;wx/txtstrm.h|"
    "wxTextValidator;wx/valtext.h|"
    "wxTheClipboard;wx/clipbrd.h|"
    "wxThread;wx/thread.h|"
    "wxThreadHelper;wx/thread.h|"
    "wxTimer;wx/timer.h|"
    "wxTimerEvent;wx/timer.h|"
    "wxTimeSpan;wx/datetime.h|"
    "wxTipProvider;wx/tipdlg.h|"
    "wxTipWindow;wx/tipwin.h|"
    "wxToggleButton;wx/tglbtn.h|"
    "wxToolBar;wx/toolbar.h|"
    "wxToolbook;wx/toolbook.h|"
    "wxToolTip;wx/tooltip.h|"
    "wxTopLevelWindow;wx/toplevel.h|"
    "wxTreebook;wx/treebook.h|"
    "wxTreebookEvent;wx/treebook.h|"
    "wxTreeCtrl;wx/treectrl.h|"
    "wxTreeEvent;wx/treectrl.h|"
    "wxTreeItemData;wx/treectrl.h|"
    "wxTreeItemId;wx/treebase.h|"
    "wxUnsetEnv;wx/utils.h|"
    "wxUpdateUIEvent;wx/event.h|"
    "wxURI;wx/uri.h|"
    "wxURL;wx/url.h|"
    "wxURLDataObject;wx/dataobj.h|"
    "wxVaCopy;wx/defs.h|"
    "wxValidator;wx/validate.h|"
    "wxVariant;wx/variant.h|"
    "wxVariantData;wx/variant.h|"
    "wxView;wx/docview.h|"
    "wxVListBox;wx/vlbox.h|"
    "wxVScrolledWindow;wx/vscroll.h|"
    "wxWindow;wx/window.h|"
    "wxWindowUpdateLocker;wx/wupdlock.h|"
    "wxWindowCreateEvent;wx/event.h|"
    "wxWindowDC;wx/dcclient.h|"
    "wxWindowDestroyEvent;wx/event.h|"
    "wxWindowDisabler;wx/utils.h|"
    "wxWizard;wx/wizard.h|"
    "wxWizardEvent;wx/wizard.h|"
    "wxWizardPage;wx/wizard.h|"
    "wxWizardPageSimple;wx/wizard.h|"
    "wxXmlDocument;wx/xml/xml.h|"
    "wxXmlNode;wx/xml/xml.h|"
    "wxXmlProperty;wx/xml/xml.h|"
    "wxXmlResource;wx/xrc/xmlres.h|"
    "wxXmlResourceHandler;wx/xrc/xmlres.h|"
    "wxZipClassFactory;wx/zipstrm.h|"
    "wxZipEntry;wx/zipstrm.h|"
    "wxZipInputStream;wx/zipstrm.h|"
    "wxZipNotifier;wx/zipstrm.h|"
    "wxZipOutputStream;wx/zipstrm.h|"
    "wxZlibInputStream;wx/zstream.h|"
    "wxZlibOutputStream;wx/zstream.h");

    const wxArrayString arWxWidgets_2_8_8 = GetArrayFromString(strWxWidgets_2_8_8, _T("|"));
    for(std::size_t i = 0; i < arWxWidgets_2_8_8.GetCount(); ++i)
    {
        const wxArrayString arTmp = GetArrayFromString(arWxWidgets_2_8_8.Item(i), _T(";"));
        AddBinding(_T("wxWidgets_2_8_8"), arTmp.Item(0), arTmp.Item(1) );
    }
}// SetDefaultsWxWidgets

void Bindings::SetDefaultsSTL()
{
  AddBinding(_T("STL"),_T("adjacent_find"),                      _T("algorithm"));
  AddBinding(_T("STL"),_T("binary_search"),                      _T("algorithm"));
  AddBinding(_T("STL"),_T("checked_copy"),                       _T("algorithm"));
  AddBinding(_T("STL"),_T("checked_copy_backward"),              _T("algorithm"));
  AddBinding(_T("STL"),_T("checked_fill_n"),                     _T("algorithm"));
  AddBinding(_T("STL"),_T("checked_generate_n"),                 _T("algorithm"));
  AddBinding(_T("STL"),_T("checked_merge"),                      _T("algorithm"));
  AddBinding(_T("STL"),_T("checked_remove_copy"),                _T("algorithm"));
  AddBinding(_T("STL"),_T("checked_remove_copy_if"),             _T("algorithm"));
  AddBinding(_T("STL"),_T("checked_replace_copy"),               _T("algorithm"));
  AddBinding(_T("STL"),_T("checked_replace_copy_if"),            _T("algorithm"));
  AddBinding(_T("STL"),_T("checked_reverse_copy"),               _T("algorithm"));
  AddBinding(_T("STL"),_T("checked_rotate_copy"),                _T("algorithm"));
  AddBinding(_T("STL"),_T("checked_set_difference"),             _T("algorithm"));
  AddBinding(_T("STL"),_T("checked_set_intersection"),           _T("algorithm"));
  AddBinding(_T("STL"),_T("checked_set_symmetric_difference"),   _T("algorithm"));
  AddBinding(_T("STL"),_T("checked_set_union"),                  _T("algorithm"));
  AddBinding(_T("STL"),_T("checked_unique_copy"),                _T("algorithm"));
  AddBinding(_T("STL"),_T("copy"),                               _T("algorithm"));
  AddBinding(_T("STL"),_T("copy_backward"),                      _T("algorithm"));
  AddBinding(_T("STL"),_T("count"),                              _T("algorithm"));
  AddBinding(_T("STL"),_T("count_if"),                           _T("algorithm"));
  AddBinding(_T("STL"),_T("equal"),                              _T("algorithm"));
  AddBinding(_T("STL"),_T("equal_range"),                        _T("algorithm"));
  AddBinding(_T("STL"),_T("fill"),                               _T("algorithm"));
  AddBinding(_T("STL"),_T("fill_n"),                             _T("algorithm"));
  AddBinding(_T("STL"),_T("find"),                               _T("algorithm"));
  AddBinding(_T("STL"),_T("find_end"),                           _T("algorithm"));
  AddBinding(_T("STL"),_T("find_first_of"),                      _T("algorithm"));
  AddBinding(_T("STL"),_T("find_if"),                            _T("algorithm"));
  AddBinding(_T("STL"),_T("for_each"),                           _T("algorithm"));
  AddBinding(_T("STL"),_T("generate"),                           _T("algorithm"));
  AddBinding(_T("STL"),_T("generate_n"),                         _T("algorithm"));
  AddBinding(_T("STL"),_T("includes"),                           _T("algorithm"));
  AddBinding(_T("STL"),_T("inplace_merge"),                      _T("algorithm"));
  AddBinding(_T("STL"),_T("iter_swap"),                          _T("algorithm"));
  AddBinding(_T("STL"),_T("lexicographical_compare"),            _T("algorithm"));
  AddBinding(_T("STL"),_T("lower_bound"),                        _T("algorithm"));
  AddBinding(_T("STL"),_T("make_heap"),                          _T("algorithm"));
  AddBinding(_T("STL"),_T("max"),                                _T("algorithm"));
  AddBinding(_T("STL"),_T("max_element"),                        _T("algorithm"));
  AddBinding(_T("STL"),_T("merge"),                              _T("algorithm"));
  AddBinding(_T("STL"),_T("min"),                                _T("algorithm"));
  AddBinding(_T("STL"),_T("min_element"),                        _T("algorithm"));
  AddBinding(_T("STL"),_T("mismatch"),                           _T("algorithm"));
  AddBinding(_T("STL"),_T("next_permutation"),                   _T("algorithm"));
  AddBinding(_T("STL"),_T("nth_element"),                        _T("algorithm"));
  AddBinding(_T("STL"),_T("partial_sort"),                       _T("algorithm"));
  AddBinding(_T("STL"),_T("partial_sort_copy"),                  _T("algorithm"));
  AddBinding(_T("STL"),_T("partition"),                          _T("algorithm"));
  AddBinding(_T("STL"),_T("pop_heap"),                           _T("algorithm"));
  AddBinding(_T("STL"),_T("prev_permutation"),                   _T("algorithm"));
  AddBinding(_T("STL"),_T("push_heap"),                          _T("algorithm"));
  AddBinding(_T("STL"),_T("random_shuffle"),                     _T("algorithm"));
  AddBinding(_T("STL"),_T("remove"),                             _T("algorithm"));
  AddBinding(_T("STL"),_T("remove_copy"),                        _T("algorithm"));
  AddBinding(_T("STL"),_T("remove_copy_if"),                     _T("algorithm"));
  AddBinding(_T("STL"),_T("remove_if"),                          _T("algorithm"));
  AddBinding(_T("STL"),_T("replace"),                            _T("algorithm"));
  AddBinding(_T("STL"),_T("replace_copy"),                       _T("algorithm"));
  AddBinding(_T("STL"),_T("replace_copy_if"),                    _T("algorithm"));
  AddBinding(_T("STL"),_T("replace_if"),                         _T("algorithm"));
  AddBinding(_T("STL"),_T("reverse"),                            _T("algorithm"));
  AddBinding(_T("STL"),_T("reverse_copy"),                       _T("algorithm"));
  AddBinding(_T("STL"),_T("rotate"),                             _T("algorithm"));
  AddBinding(_T("STL"),_T("rotate_copy"),                        _T("algorithm"));
  AddBinding(_T("STL"),_T("search"),                             _T("algorithm"));
  AddBinding(_T("STL"),_T("search_n"),                           _T("algorithm"));
  AddBinding(_T("STL"),_T("set_difference"),                     _T("algorithm"));
  AddBinding(_T("STL"),_T("set_intersection"),                   _T("algorithm"));
  AddBinding(_T("STL"),_T("set_symmetric_difference"),           _T("algorithm"));
  AddBinding(_T("STL"),_T("set_union"),                          _T("algorithm"));
  AddBinding(_T("STL"),_T("sort"),                               _T("algorithm"));
  AddBinding(_T("STL"),_T("sort_heap"),                          _T("algorithm"));
  AddBinding(_T("STL"),_T("stable_partition"),                   _T("algorithm"));
  AddBinding(_T("STL"),_T("stable_sort"),                        _T("algorithm"));
  AddBinding(_T("STL"),_T("swap"),                               _T("algorithm"));
  AddBinding(_T("STL"),_T("swap_ranges"),                        _T("algorithm"));
  AddBinding(_T("STL"),_T("transform"),                          _T("algorithm"));
  AddBinding(_T("STL"),_T("unchecked_copy"),                     _T("algorithm"));
  AddBinding(_T("STL"),_T("unchecked_copy_backward"),            _T("algorithm"));
  AddBinding(_T("STL"),_T("unchecked_fill_n"),                   _T("algorithm"));
  AddBinding(_T("STL"),_T("unchecked_generate_n"),               _T("algorithm"));
  AddBinding(_T("STL"),_T("unchecked_merge"),                    _T("algorithm"));
  AddBinding(_T("STL"),_T("unchecked_remove_copy"),              _T("algorithm"));
  AddBinding(_T("STL"),_T("unchecked_remove_copy_if"),           _T("algorithm"));
  AddBinding(_T("STL"),_T("unchecked_replace_copy"),             _T("algorithm"));
  AddBinding(_T("STL"),_T("unchecked_replace_copy_if"),          _T("algorithm"));
  AddBinding(_T("STL"),_T("unchecked_reverse_copy"),             _T("algorithm"));
  AddBinding(_T("STL"),_T("unchecked_rotate_copy"),              _T("algorithm"));
  AddBinding(_T("STL"),_T("unchecked_set_difference"),           _T("algorithm"));
  AddBinding(_T("STL"),_T("unchecked_set_intersection"),         _T("algorithm"));
  AddBinding(_T("STL"),_T("unchecked_set_symmetric_difference"), _T("algorithm"));
  AddBinding(_T("STL"),_T("unchecked_set_union"),                _T("algorithm"));
  AddBinding(_T("STL"),_T("unchecked_unique_copy"),              _T("algorithm"));
  AddBinding(_T("STL"),_T("unique"),                             _T("algorithm"));
  AddBinding(_T("STL"),_T("unique_copy"),                        _T("algorithm"));
  AddBinding(_T("STL"),_T("upper_bound"),                        _T("algorithm"));
  AddBinding(_T("STL"),_T("bitset"),                             _T("bitset"));
  AddBinding(_T("STL"),_T("arg"),                                _T("complex"));
  AddBinding(_T("STL"),_T("complex"),                            _T("complex"));
  AddBinding(_T("STL"),_T("conj"),                               _T("complex"));
  AddBinding(_T("STL"),_T("imag"),                               _T("complex"));
  AddBinding(_T("STL"),_T("norm"),                               _T("complex"));
  AddBinding(_T("STL"),_T("polar"),                              _T("complex"));
  AddBinding(_T("STL"),_T("real"),                               _T("complex"));
  AddBinding(_T("STL"),_T("deque"),                              _T("deque"));
  AddBinding(_T("STL"),_T("bad_exception"),                      _T("exception"));
  AddBinding(_T("STL"),_T("exception"),                          _T("exception"));
  AddBinding(_T("STL"),_T("set_terminate"),                      _T("exception"));
  AddBinding(_T("STL"),_T("set_unexpected"),                     _T("exception"));
  AddBinding(_T("STL"),_T("terminate"),                          _T("exception"));
  AddBinding(_T("STL"),_T("terminate_handler"),                  _T("exception"));
  AddBinding(_T("STL"),_T("uncaught_exception"),                 _T("exception"));
  AddBinding(_T("STL"),_T("unexpected"),                         _T("exception"));
  AddBinding(_T("STL"),_T("unexpected_handler"),                 _T("exception"));
  AddBinding(_T("STL"),_T("basic_filebuf"),                      _T("fstream"));
  AddBinding(_T("STL"),_T("basic_fstream"),                      _T("fstream"));
  AddBinding(_T("STL"),_T("basic_ifstream"),                     _T("fstream"));
  AddBinding(_T("STL"),_T("basic_ofstream"),                     _T("fstream"));
  AddBinding(_T("STL"),_T("filebuf"),                            _T("fstream"));
  AddBinding(_T("STL"),_T("fstream"),                            _T("fstream"));
  AddBinding(_T("STL"),_T("ifstream"),                           _T("fstream"));
  AddBinding(_T("STL"),_T("ofstream"),                           _T("fstream"));
  AddBinding(_T("STL"),_T("wfilebuf"),                           _T("fstream"));
  AddBinding(_T("STL"),_T("wfstream"),                           _T("fstream"));
  AddBinding(_T("STL"),_T("wifstream"),                          _T("fstream"));
  AddBinding(_T("STL"),_T("wofstream"),                          _T("fstream"));
  AddBinding(_T("STL"),_T("binary_function"),                    _T("functional"));
  AddBinding(_T("STL"),_T("binary_negate"),                      _T("functional"));
  AddBinding(_T("STL"),_T("bind1st"),                            _T("functional"));
  AddBinding(_T("STL"),_T("bind2nd"),                            _T("functional"));
  AddBinding(_T("STL"),_T("binder1st"),                          _T("functional"));
  AddBinding(_T("STL"),_T("binder2nd"),                          _T("functional"));
  AddBinding(_T("STL"),_T("const_mem_fun_ref_t"),                _T("functional"));
  AddBinding(_T("STL"),_T("const_mem_fun_t"),                    _T("functional"));
  AddBinding(_T("STL"),_T("const_mem_fun1_ref_t"),               _T("functional"));
  AddBinding(_T("STL"),_T("const_mem_fun1_t"),                   _T("functional"));
  AddBinding(_T("STL"),_T("divides"),                            _T("functional"));
  AddBinding(_T("STL"),_T("equal_to"),                           _T("functional"));
  AddBinding(_T("STL"),_T("greater"),                            _T("functional"));
  AddBinding(_T("STL"),_T("greater_equal"),                      _T("functional"));
  AddBinding(_T("STL"),_T("less"),                               _T("functional"));
  AddBinding(_T("STL"),_T("less_equal"),                         _T("functional"));
  AddBinding(_T("STL"),_T("logical_and"),                        _T("functional"));
  AddBinding(_T("STL"),_T("logical_not"),                        _T("functional"));
  AddBinding(_T("STL"),_T("logical_or"),                         _T("functional"));
  AddBinding(_T("STL"),_T("mem_fun"),                            _T("functional"));
  AddBinding(_T("STL"),_T("mem_fun_ref"),                        _T("functional"));
  AddBinding(_T("STL"),_T("mem_fun_ref_t"),                      _T("functional"));
  AddBinding(_T("STL"),_T("mem_fun_t"),                          _T("functional"));
  AddBinding(_T("STL"),_T("mem_fun1_ref_t"),                     _T("functional"));
  AddBinding(_T("STL"),_T("mem_fun1_t"),                         _T("functional"));
  AddBinding(_T("STL"),_T("minus"),                              _T("functional"));
  AddBinding(_T("STL"),_T("modulus"),                            _T("functional"));
  AddBinding(_T("STL"),_T("multiplies"),                         _T("functional"));
  AddBinding(_T("STL"),_T("negate"),                             _T("functional"));
  AddBinding(_T("STL"),_T("not_equal_to"),                       _T("functional"));
  AddBinding(_T("STL"),_T("not1"),                               _T("functional"));
  AddBinding(_T("STL"),_T("not2"),                               _T("functional"));
  AddBinding(_T("STL"),_T("plus"),                               _T("functional"));
  AddBinding(_T("STL"),_T("pointer_to_binary_function"),         _T("functional"));
  AddBinding(_T("STL"),_T("pointer_to_unary_function"),          _T("functional"));
  AddBinding(_T("STL"),_T("ptr_fun"),                            _T("functional"));
  AddBinding(_T("STL"),_T("unary_function"),                     _T("functional"));
  AddBinding(_T("STL"),_T("unary_negate"),                       _T("functional"));
  AddBinding(_T("STL"),_T("hash_compare"),                       _T("hash_map"));
  AddBinding(_T("STL"),_T("hash_map"),                           _T("hash_map"));
  AddBinding(_T("STL"),_T("hash_multimap"),                      _T("hash_map"));
  AddBinding(_T("STL"),_T("value_compare"),                      _T("hash_map"));
  AddBinding(_T("STL"),_T("hash_multiset"),                      _T("hash_set"));
  AddBinding(_T("STL"),_T("hash_set"),                           _T("hash_set"));
  AddBinding(_T("STL"),_T("get_money"),                          _T("iomanip"));
  AddBinding(_T("STL"),_T("get_time"),                           _T("iomanip"));
  AddBinding(_T("STL"),_T("put_money"),                          _T("iomanip"));
  AddBinding(_T("STL"),_T("put_time"),                           _T("iomanip"));
  AddBinding(_T("STL"),_T("resetiosflags"),                      _T("iomanip"));
  AddBinding(_T("STL"),_T("setbase"),                            _T("iomanip"));
  AddBinding(_T("STL"),_T("setfill"),                            _T("iomanip"));
  AddBinding(_T("STL"),_T("setiosflags"),                        _T("iomanip"));
  AddBinding(_T("STL"),_T("setprecision"),                       _T("iomanip"));
  AddBinding(_T("STL"),_T("setw"),                               _T("iomanip"));
  AddBinding(_T("STL"),_T("cerr"),                               _T("iostream"));
  AddBinding(_T("STL"),_T("cin"),                                _T("iostream"));
  AddBinding(_T("STL"),_T("clog"),                               _T("iostream"));
  AddBinding(_T("STL"),_T("cout"),                               _T("iostream"));
  AddBinding(_T("STL"),_T("endl"),                               _T("iostream"));
  AddBinding(_T("STL"),_T("wcerr"),                              _T("iostream"));
  AddBinding(_T("STL"),_T("wcin"),                               _T("iostream"));
  AddBinding(_T("STL"),_T("wclog"),                              _T("iostream"));
  AddBinding(_T("STL"),_T("wcout"),                              _T("iostream"));
  AddBinding(_T("STL"),_T("basic_istream"),                      _T("istream"));
  AddBinding(_T("STL"),_T("basic_iostream"),                     _T("istream"));
  AddBinding(_T("STL"),_T("istream"),                            _T("istream"));
  AddBinding(_T("STL"),_T("iostream"),                           _T("istream"));
  AddBinding(_T("STL"),_T("wistream"),                           _T("istream"));
  AddBinding(_T("STL"),_T("wiostream"),                          _T("istream"));
  AddBinding(_T("STL"),_T("advance"),                            _T("iterator"));
  AddBinding(_T("STL"),_T("back_inserter"),                      _T("iterator"));
  AddBinding(_T("STL"),_T("distance"),                           _T("iterator"));
  AddBinding(_T("STL"),_T("front_inserter"),                     _T("iterator"));
  AddBinding(_T("STL"),_T("inserter"),                           _T("iterator"));
  AddBinding(_T("STL"),_T("make_move_iterator"),                 _T("iterator"));
  AddBinding(_T("STL"),_T("back_insert_iterator"),               _T("iterator"));
  AddBinding(_T("STL"),_T("bidirectional_iterator_tag"),         _T("iterator"));
  AddBinding(_T("STL"),_T("checked_array_iterator"),             _T("iterator"));
  AddBinding(_T("STL"),_T("forward_iterator_tag"),               _T("iterator"));
  AddBinding(_T("STL"),_T("front_insert_iterator"),              _T("iterator"));
  AddBinding(_T("STL"),_T("input_iterator_tag"),                 _T("iterator"));
  AddBinding(_T("STL"),_T("insert_iterator"),                    _T("iterator"));
  AddBinding(_T("STL"),_T("istream_iterator"),                   _T("iterator"));
  AddBinding(_T("STL"),_T("istreambuf_iterator"),                _T("iterator"));
  AddBinding(_T("STL"),_T("iterator"),                           _T("iterator"));
  AddBinding(_T("STL"),_T("iterator_traits"),                    _T("iterator"));
  AddBinding(_T("STL"),_T("move_iterator"),                      _T("iterator"));
  AddBinding(_T("STL"),_T("ostream_iterator"),                   _T("iterator"));
  AddBinding(_T("STL"),_T("ostreambuf_iterator"),                _T("iterator"));
  AddBinding(_T("STL"),_T("output_iterator_tag"),                _T("iterator"));
  AddBinding(_T("STL"),_T("random_access_iterator_tag"),         _T("iterator"));
  AddBinding(_T("STL"),_T("reverse_iterator"),                   _T("iterator"));
  AddBinding(_T("STL"),_T("float_denorm_style"),                 _T("limits"));
  AddBinding(_T("STL"),_T("float_round_style"),                  _T("limits"));
  AddBinding(_T("STL"),_T("numeric_limits"),                     _T("limits"));
  AddBinding(_T("STL"),_T("list"),                               _T("list"));
  AddBinding(_T("STL"),_T("codecvt"),                            _T("locale"));
  AddBinding(_T("STL"),_T("codecvt_base"),                       _T("locale"));
  AddBinding(_T("STL"),_T("codecvt_byname"),                     _T("locale"));
  AddBinding(_T("STL"),_T("collate"),                            _T("locale"));
  AddBinding(_T("STL"),_T("collate_byname"),                     _T("locale"));
  AddBinding(_T("STL"),_T("ctype"),                              _T("locale"));
  AddBinding(_T("STL"),_T("ctype_base"),                         _T("locale"));
  AddBinding(_T("STL"),_T("ctype_byname"),                       _T("locale"));
  AddBinding(_T("STL"),_T("locale"),                             _T("locale"));
  AddBinding(_T("STL"),_T("messages"),                           _T("locale"));
  AddBinding(_T("STL"),_T("messages_base"),                      _T("locale"));
  AddBinding(_T("STL"),_T("messages_byname"),                    _T("locale"));
  AddBinding(_T("STL"),_T("money_base"),                         _T("locale"));
  AddBinding(_T("STL"),_T("money_get"),                          _T("locale"));
  AddBinding(_T("STL"),_T("money_put"),                          _T("locale"));
  AddBinding(_T("STL"),_T("moneypunct"),                         _T("locale"));
  AddBinding(_T("STL"),_T("moneypunct_byname"),                  _T("locale"));
  AddBinding(_T("STL"),_T("num_get"),                            _T("locale"));
  AddBinding(_T("STL"),_T("num_put"),                            _T("locale"));
  AddBinding(_T("STL"),_T("numpunct"),                           _T("locale"));
  AddBinding(_T("STL"),_T("numpunct_byname"),                    _T("locale"));
  AddBinding(_T("STL"),_T("time_base"),                          _T("locale"));
  AddBinding(_T("STL"),_T("time_get"),                           _T("locale"));
  AddBinding(_T("STL"),_T("time_put_byname"),                    _T("locale"));
  AddBinding(_T("STL"),_T("map"),                                _T("map"));
  AddBinding(_T("STL"),_T("multimap"),                           _T("map"));
  AddBinding(_T("STL"),_T("value_compare"),                      _T("map"));
  AddBinding(_T("STL"),_T("allocator"),                          _T("memory"));
  AddBinding(_T("STL"),_T("auto_ptr"),                           _T("memory"));
  AddBinding(_T("STL"),_T("checked_uninitialized_fill_n"),       _T("memory"));
  AddBinding(_T("STL"),_T("get_temporary_buffer"),               _T("memory"));
  AddBinding(_T("STL"),_T("raw_storage_iterator "),              _T("memory"));
  AddBinding(_T("STL"),_T("return_temporary_buffer"),            _T("memory"));
  AddBinding(_T("STL"),_T("unchecked_uninitialized_fill_n"),     _T("memory"));
  AddBinding(_T("STL"),_T("uninitialized_copy"),                 _T("memory"));
  AddBinding(_T("STL"),_T("uninitialized_fill"),                 _T("memory"));
  AddBinding(_T("STL"),_T("uninitialized_fill_n"),               _T("memory"));
  AddBinding(_T("STL"),_T("accumulate"),                         _T("numeric"));
  AddBinding(_T("STL"),_T("adjacent_difference"),                _T("numeric"));
  AddBinding(_T("STL"),_T("checked_adjacent_difference"),        _T("numeric"));
  AddBinding(_T("STL"),_T("checked_partial_sum"),                _T("numeric"));
  AddBinding(_T("STL"),_T("inner_product"),                      _T("numeric"));
  AddBinding(_T("STL"),_T("partial_sum"),                        _T("numeric"));
  AddBinding(_T("STL"),_T("unchecked_adjacent_difference"),      _T("numeric"));
  AddBinding(_T("STL"),_T("unchecked_partial_sum"),              _T("numeric"));
  AddBinding(_T("STL"),_T("basic_ostream"),                      _T("ostream"));
  AddBinding(_T("STL"),_T("ostream"),                            _T("ostream"));
  AddBinding(_T("STL"),_T("wostream"),                           _T("ostream"));
  AddBinding(_T("STL"),_T("priority_queue "),                    _T("queue"));
  AddBinding(_T("STL"),_T("queue"),                              _T("queue"));
  AddBinding(_T("STL"),_T("multiset"),                           _T("set"));
  AddBinding(_T("STL"),_T("set"),                                _T("set"));
  AddBinding(_T("STL"),_T("basic_istringstream"),                _T("sstream"));
  AddBinding(_T("STL"),_T("basic_ostringstream"),                _T("sstream"));
  AddBinding(_T("STL"),_T("basic_stringbuf"),                    _T("sstream"));
  AddBinding(_T("STL"),_T("basic_stringstream"),                 _T("sstream"));
  AddBinding(_T("STL"),_T("istringstream"),                      _T("sstream"));
  AddBinding(_T("STL"),_T("ostringstream"),                      _T("sstream"));
  AddBinding(_T("STL"),_T("stringbuf"),                          _T("sstream"));
  AddBinding(_T("STL"),_T("stringstream"),                       _T("sstream"));
  AddBinding(_T("STL"),_T("wistringstream"),                     _T("sstream"));
  AddBinding(_T("STL"),_T("wostringstream"),                     _T("sstream"));
  AddBinding(_T("STL"),_T("wstringbuf"),                         _T("sstream"));
  AddBinding(_T("STL"),_T("wstringstream"),                      _T("sstream"));
  AddBinding(_T("STL"),_T("stack"),                              _T("stack"));
  AddBinding(_T("STL"),_T("domain_error"),                       _T("stdexcept"));
  AddBinding(_T("STL"),_T("invalid_argument "),                  _T("stdexcept"));
  AddBinding(_T("STL"),_T("length_error"),                       _T("stdexcept"));
  AddBinding(_T("STL"),_T("logic_error"),                        _T("stdexcept"));
  AddBinding(_T("STL"),_T("out_of_range"),                       _T("stdexcept"));
  AddBinding(_T("STL"),_T("overflow_error"),                     _T("stdexcept"));
  AddBinding(_T("STL"),_T("range_error"),                        _T("stdexcept"));
  AddBinding(_T("STL"),_T("runtime_error "),                     _T("stdexcept"));
  AddBinding(_T("STL"),_T("underflow_error "),                   _T("stdexcept"));
  AddBinding(_T("STL"),_T("basic_streambuf"),                    _T("streambuf"));
  AddBinding(_T("STL"),_T("streambuf"),                          _T("streambuf"));
  AddBinding(_T("STL"),_T("wstreambuf"),                         _T("streambuf"));
  AddBinding(_T("STL"),_T("basic_string"),                       _T("string"));
  AddBinding(_T("STL"),_T("char_traits"),                        _T("string"));
  AddBinding(_T("STL"),_T("string"),                             _T("string"));
  AddBinding(_T("STL"),_T("wstring"),                            _T("string"));
  AddBinding(_T("STL"),_T("istrstream"),                         _T("strstream"));
  AddBinding(_T("STL"),_T("ostrstream"),                         _T("strstream"));
  AddBinding(_T("STL"),_T("strstream"),                          _T("strstream"));
  AddBinding(_T("STL"),_T("strstreambuf"),                       _T("strstream"));
  AddBinding(_T("STL"),_T("pair"),                               _T("utility"));
  AddBinding(_T("STL"),_T("make_pair"),                          _T("utility"));
  AddBinding(_T("STL"),_T("gslice"),                             _T("valarray"));
  AddBinding(_T("STL"),_T("gslice_array"),                       _T("valarray"));
  AddBinding(_T("STL"),_T("indirect_array"),                     _T("valarray"));
  AddBinding(_T("STL"),_T("mask_array"),                         _T("valarray"));
  AddBinding(_T("STL"),_T("slice"),                              _T("valarray"));
  AddBinding(_T("STL"),_T("slice_array"),                        _T("valarray"));
  AddBinding(_T("STL"),_T("valarray"),                           _T("valarray"));
  AddBinding(_T("STL"),_T("vector"),                             _T("vector"));
}// SetDefaultsSTL

void Bindings::SetDefaultsCLibrary()
{
  AddBinding(_T("C_Library"),_T("assert"),     _T("cassert"));
  AddBinding(_T("C_Library"),_T("isalnum"),    _T("cctype"));
  AddBinding(_T("C_Library"),_T("isalpha"),    _T("cctype"));
  AddBinding(_T("C_Library"),_T("iscntrl"),    _T("cctype"));
  AddBinding(_T("C_Library"),_T("isdigit"),    _T("cctype"));
  AddBinding(_T("C_Library"),_T("isgraph"),    _T("cctype"));
  AddBinding(_T("C_Library"),_T("islower"),    _T("cctype"));
  AddBinding(_T("C_Library"),_T("isprint"),    _T("cctype"));
  AddBinding(_T("C_Library"),_T("ispunct"),    _T("cctype"));
  AddBinding(_T("C_Library"),_T("isspace"),    _T("cctype"));
  AddBinding(_T("C_Library"),_T("isupper"),    _T("cctype"));
  AddBinding(_T("C_Library"),_T("isxdigit"),   _T("cctype"));
  AddBinding(_T("C_Library"),_T("tolower"),    _T("cctype"));
  AddBinding(_T("C_Library"),_T("toupper"),    _T("cctype"));
  AddBinding(_T("C_Library"),_T("setlocale"),  _T("clocale"));
  AddBinding(_T("C_Library"),_T("localeconv"), _T("clocale"));
  AddBinding(_T("C_Library"),_T("fclose"),     _T("cstdio"));
  AddBinding(_T("C_Library"),_T("fopen"),      _T("cstdio"));
  AddBinding(_T("C_Library"),_T("fflush"),     _T("cstdio"));
  AddBinding(_T("C_Library"),_T("freopen"),    _T("cstdio"));
  AddBinding(_T("C_Library"),_T("setbuf"),     _T("cstdio"));
  AddBinding(_T("C_Library"),_T("setvbuf"),    _T("cstdio"));
  AddBinding(_T("C_Library"),_T("fprintf"),    _T("cstdio"));
  AddBinding(_T("C_Library"),_T("fscan"),      _T("cstdio"));
  AddBinding(_T("C_Library"),_T("printf"),     _T("cstdio"));
  AddBinding(_T("C_Library"),_T("scanf"),      _T("cstdio"));
  AddBinding(_T("C_Library"),_T("sprintf"),    _T("cstdio"));
  AddBinding(_T("C_Library"),_T("sscanf"),     _T("cstdio"));
  AddBinding(_T("C_Library"),_T("vfprintf"),   _T("cstdio"));
  AddBinding(_T("C_Library"),_T("vprintf"),    _T("cstdio"));
  AddBinding(_T("C_Library"),_T("vsprintf"),   _T("cstdio"));
  AddBinding(_T("C_Library"),_T("fgetc"),      _T("cstdio"));
  AddBinding(_T("C_Library"),_T("fgets"),      _T("cstdio"));
  AddBinding(_T("C_Library"),_T("fputc"),      _T("cstdio"));
  AddBinding(_T("C_Library"),_T("fputs"),      _T("cstdio"));
  AddBinding(_T("C_Library"),_T("getc"),       _T("cstdio"));
  AddBinding(_T("C_Library"),_T("getchar"),    _T("cstdio"));
  AddBinding(_T("C_Library"),_T("gets"),       _T("cstdio"));
  AddBinding(_T("C_Library"),_T("putc"),       _T("cstdio"));
  AddBinding(_T("C_Library"),_T("putchar"),    _T("cstdio"));
  AddBinding(_T("C_Library"),_T("puts"),       _T("cstdio"));
  AddBinding(_T("C_Library"),_T("ungetc"),     _T("cstdio"));
  AddBinding(_T("C_Library"),_T("fread"),      _T("cstdio"));
  AddBinding(_T("C_Library"),_T("fwrite"),     _T("cstdio"));
  AddBinding(_T("C_Library"),_T("fgetpos"),    _T("cstdio"));
  AddBinding(_T("C_Library"),_T("fseek"),      _T("cstdio"));
  AddBinding(_T("C_Library"),_T("fsetpos"),    _T("cstdio"));
  AddBinding(_T("C_Library"),_T("ftell"),      _T("cstdio"));
  AddBinding(_T("C_Library"),_T("rewind"),     _T("cstdio"));
  AddBinding(_T("C_Library"),_T("clearerr"),   _T("cstdio"));
  AddBinding(_T("C_Library"),_T("feof"),       _T("cstdio"));
  AddBinding(_T("C_Library"),_T("ferror"),     _T("cstdio"));
  AddBinding(_T("C_Library"),_T("perrer"),     _T("cstdio"));
  AddBinding(_T("C_Library"),_T("FILE"),       _T("cstdio"));
  AddBinding(_T("C_Library"),_T("abort"),      _T("cstdlib"));
  AddBinding(_T("C_Library"),_T("abs"),        _T("cstdlib"));
  AddBinding(_T("C_Library"),_T("atexit"),     _T("cstdlib"));
  AddBinding(_T("C_Library"),_T("atof"),       _T("cstdlib"));
  AddBinding(_T("C_Library"),_T("atoi"),       _T("cstdlib"));
  AddBinding(_T("C_Library"),_T("atol"),       _T("cstdlib"));
  AddBinding(_T("C_Library"),_T("bsearch"),    _T("cstdlib"));
  AddBinding(_T("C_Library"),_T("calloc"),     _T("cstdlib"));
  AddBinding(_T("C_Library"),_T("div"),        _T("cstdlib"));
  AddBinding(_T("C_Library"),_T("exit"),       _T("cstdlib"));
  AddBinding(_T("C_Library"),_T("free"),       _T("cstdlib"));
  AddBinding(_T("C_Library"),_T("getenv"),     _T("cstdlib"));
  AddBinding(_T("C_Library"),_T("labs"),       _T("cstdlib"));
  AddBinding(_T("C_Library"),_T("ldiv"),       _T("cstdlib"));
  AddBinding(_T("C_Library"),_T("malloc"),     _T("cstdlib"));
  AddBinding(_T("C_Library"),_T("mblen"),      _T("cstdlib"));
  AddBinding(_T("C_Library"),_T("mbstowcs"),   _T("cstdlib"));
  AddBinding(_T("C_Library"),_T("mbtowc"),     _T("cstdlib"));
  AddBinding(_T("C_Library"),_T("qsort"),      _T("cstdlib"));
  AddBinding(_T("C_Library"),_T("rand"),       _T("cstdlib"));
  AddBinding(_T("C_Library"),_T("realloc"),    _T("cstdlib"));
  AddBinding(_T("C_Library"),_T("srand"),      _T("cstdlib"));
  AddBinding(_T("C_Library"),_T("strtod"),     _T("cstdlib"));
  AddBinding(_T("C_Library"),_T("strtol"),     _T("cstdlib"));
  AddBinding(_T("C_Library"),_T("strtoul"),    _T("cstdlib"));
  AddBinding(_T("C_Library"),_T("system"),     _T("cstdlib"));
  AddBinding(_T("C_Library"),_T("wcstombs"),   _T("cstdlib"));
  AddBinding(_T("C_Library"),_T("wctomb"),     _T("cstdlib"));
  AddBinding(_T("C_Library"),_T("memchr"),     _T("cstring"));
  AddBinding(_T("C_Library"),_T("memcmp"),     _T("cstring"));
  AddBinding(_T("C_Library"),_T("memcpy"),     _T("cstring"));
  AddBinding(_T("C_Library"),_T("memmove"),    _T("cstring"));
  AddBinding(_T("C_Library"),_T("memset"),     _T("cstring"));
  AddBinding(_T("C_Library"),_T("strcat"),     _T("cstring"));
  AddBinding(_T("C_Library"),_T("strchr"),     _T("cstring"));
  AddBinding(_T("C_Library"),_T("strcmp"),     _T("cstring"));
  AddBinding(_T("C_Library"),_T("strcoll"),    _T("cstring"));
  AddBinding(_T("C_Library"),_T("strcpy"),     _T("cstring"));
  AddBinding(_T("C_Library"),_T("strcspn"),    _T("cstring"));
  AddBinding(_T("C_Library"),_T("strerror"),   _T("cstring"));
  AddBinding(_T("C_Library"),_T("strlen"),     _T("cstring"));
  AddBinding(_T("C_Library"),_T("strncat"),    _T("cstring"));
  AddBinding(_T("C_Library"),_T("strncmp"),    _T("cstring"));
  AddBinding(_T("C_Library"),_T("strncpy"),    _T("cstring"));
  AddBinding(_T("C_Library"),_T("strpbrk"),    _T("cstring"));
  AddBinding(_T("C_Library"),_T("strrchr"),    _T("cstring"));
  AddBinding(_T("C_Library"),_T("strspn"),     _T("cstring"));
  AddBinding(_T("C_Library"),_T("strstr"),     _T("cstring"));
  AddBinding(_T("C_Library"),_T("strtok"),     _T("cstring"));
  AddBinding(_T("C_Library"),_T("strxfrm"),    _T("cstring"));
  AddBinding(_T("C_Library"),_T("asctime"),    _T("ctime"));
  AddBinding(_T("C_Library"),_T("clock"),      _T("ctime"));
  AddBinding(_T("C_Library"),_T("ctime"),      _T("ctime"));
  AddBinding(_T("C_Library"),_T("difftime"),   _T("ctime"));
  AddBinding(_T("C_Library"),_T("gmtime"),     _T("ctime"));
  AddBinding(_T("C_Library"),_T("localtime"),  _T("ctime"));
  AddBinding(_T("C_Library"),_T("mktime"),     _T("ctime"));
  AddBinding(_T("C_Library"),_T("strftime"),   _T("ctime"));
  AddBinding(_T("C_Library"),_T("time"),       _T("ctime"));
}// SetDefaultsCLibrary

/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>

#include "app.h"
#include "appglobals.h"
#include "batchbuild.h"
#include "cbauibook.h"
#include "cbstyledtextctrl.h"
#include "compilersettingsdlg.h"
#include "debuggersettingsdlg.h"
#include "dlgabout.h"
#include "dlgaboutplugin.h"
#include "environmentsettingsdlg.h"
#include "infopane.h"
#include "infowindow.h"
#include "main.h"
#include "notebookstyles.h"
#include "printdlg.h"
#include "scriptconsole.h"
#include "scriptingsettingsdlg.h"
#include "startherepage.h"
#include "switcherdlg.h"
#if wxUSE_STATUSBAR
    #include "cbstatusbar.h"
#endif

#include <wx/dnd.h>
#include <wx/fileconf.h>
#include <wx/filename.h>
#include <wx/gauge.h>
#include <wx/printdlg.h>
#include <wx/sstream.h>
#include <wx/tipdlg.h>
#include <wx/tokenzr.h>
#include <wx/xrc/xmlres.h>

#include <annoyingdialog.h>
#include <cbexception.h>
#include <cbplugin.h>
#include <cbproject.h>
#include <cbworkspace.h>
#include <configmanager.h>
#include <debuggermanager.h>
#include <editorcolourset.h>
#include <editormanager.h>
#include <filefilters.h>
#include <globals.h>
#include <logmanager.h>
#include <personalitymanager.h>
#include <pluginmanager.h>
#include <projectmanager.h>
#include <scriptingmanager.h>
#include <sdk_events.h>
#include <templatemanager.h>
#include <toolsmanager.h>
#include <uservarmanager.h>

#include "debugger_interface_creator.h"
#include "debuggermenu.h"

class cbFileDropTarget : public wxFileDropTarget
{
public:
    cbFileDropTarget(MainFrame *frame):m_frame(frame){}
    virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
    {
        if (!m_frame) return false;
        return m_frame->OnDropFiles(x,y,filenames);
    }
private:
    MainFrame* m_frame;
};

const static wxString gDefaultLayout = _T("Code::Blocks default");
static wxString gDefaultLayoutData; // this will keep the "hardcoded" default layout
static wxString gDefaultMessagePaneLayoutData; // this will keep default layout

const static wxString gMinimalLayout = _T("Code::Blocks minimal");
static wxString gMinimalLayoutData; // this will keep the "hardcoded" default layout
static wxString gMinimalMessagePaneLayoutData; // this will keep default layout

// In <wx/defs.h> wxID_FILE[X] exists only from 1..9,
// so create our own here with a *continuous* numbering!
// The wxID_FILE[X] enum usually starts at 5050 until 5059,
// followed by wxID_OK starting at 5100. (wxWidgets v2.6, v2.8 and v2.9)
// so we use the space in between starting from 5060
// and hoping it doesn't change too much in <wx/defs.h> ;-)
enum
{
    wxID_CBFILE01   = 5060, // Recent files...
    wxID_CBFILE02, // 5061
    wxID_CBFILE03, // 5062
    wxID_CBFILE04, // 5063
    wxID_CBFILE05, // 5064
    wxID_CBFILE06, // 5065
    wxID_CBFILE07, // 5066
    wxID_CBFILE08, // 5067
    wxID_CBFILE09, // 5068
    wxID_CBFILE10, // 5069
    wxID_CBFILE11, // 5070
    wxID_CBFILE12, // 5071
    wxID_CBFILE13, // 5072
    wxID_CBFILE14, // 5073
    wxID_CBFILE15, // 5074
    wxID_CBFILE16, // 5075
    wxID_CBFILE17, // 5076  // Starting here for recent projects...
    wxID_CBFILE18, // 5077
    wxID_CBFILE19, // 5078
    wxID_CBFILE20, // 5079
    wxID_CBFILE21, // 5080
    wxID_CBFILE22, // 5081
    wxID_CBFILE23, // 5082
    wxID_CBFILE24, // 5083
    wxID_CBFILE25, // 5084
    wxID_CBFILE26, // 5085
    wxID_CBFILE27, // 5086
    wxID_CBFILE28, // 5087
    wxID_CBFILE29, // 5088
    wxID_CBFILE30, // 5089
    wxID_CBFILE31, // 5090
    wxID_CBFILE32  // 5091
};

int idToolNew                           = XRCID("idToolNew");
int idFileNew                           = XRCID("idFileNew");
int idFileNewEmpty                      = XRCID("idFileNewEmpty");
int idFileNewProject                    = XRCID("idFileNewProject");
int idFileNewTarget                     = XRCID("idFileNewTarget");
int idFileNewFile                       = XRCID("idFileNewFile");
int idFileNewCustom                     = XRCID("idFileNewCustom");
int idFileNewUser                       = XRCID("idFileNewUser");
int idFileOpen                          = XRCID("idFileOpen");
int idFileReopen                        = XRCID("idFileReopen");
int idFileOpenRecentFileClearHistory    = XRCID("idFileOpenRecentFileClearHistory");
int idFileOpenRecentProjectClearHistory = XRCID("idFileOpenRecentProjectClearHistory");
int idFileImportProjectDevCpp           = XRCID("idFileImportProjectDevCpp");
int idFileImportProjectMSVC             = XRCID("idFileImportProjectMSVC");
int idFileImportProjectMSVCWksp         = XRCID("idFileImportProjectMSVCWksp");
int idFileImportProjectMSVS             = XRCID("idFileImportProjectMSVS");
int idFileImportProjectMSVSWksp         = XRCID("idFileImportProjectMSVSWksp");
int idFileSave                          = XRCID("idFileSave");
int idFileSaveAs                        = XRCID("idFileSaveAs");
int idFileSaveAllFiles                  = XRCID("idFileSaveAllFiles");
int idFileSaveProject                   = XRCID("idFileSaveProject");
int idFileSaveProjectAs                 = XRCID("idFileSaveProjectAs");
int idFileSaveProjectAllProjects        = XRCID("idFileSaveProjectAllProjects");
int idFileSaveProjectTemplate           = XRCID("idFileSaveProjectTemplate");
int idFileOpenDefWorkspace              = XRCID("idFileOpenDefWorkspace");
int idFileSaveWorkspace                 = XRCID("idFileSaveWorkspace");
int idFileSaveWorkspaceAs               = XRCID("idFileSaveWorkspaceAs");
int idFileSaveAll                       = XRCID("idFileSaveAll");
int idFileCloseWorkspace                = XRCID("idFileCloseWorkspace");
int idFileClose                         = XRCID("idFileClose");
int idFileCloseAll                      = XRCID("idFileCloseAll");
int idFileCloseProject                  = XRCID("idFileCloseProject");
int idFileCloseAllProjects              = XRCID("idFileCloseAllProjects");
int idFilePrintSetup                    = XRCID("idFilePrintSetup");
int idFilePrint                         = XRCID("idFilePrint");
int idFileExit                          = XRCID("idFileExit");
int idFileNext                          = wxNewId();
int idFilePrev                          = wxNewId();

int idEditUndo                    = XRCID("idEditUndo");
int idEditRedo                    = XRCID("idEditRedo");
int idEditClearHistory            = XRCID("idEditClearHistory");
int idEditCopy                    = XRCID("idEditCopy");
int idEditCut                     = XRCID("idEditCut");
int idEditPaste                   = XRCID("idEditPaste");
int idEditSwapHeaderSource        = XRCID("idEditSwapHeaderSource");
int idEditGotoMatchingBrace       = XRCID("idEditGotoMatchingBrace");
int idEditHighlightMode           = XRCID("idEditHighlightMode");
int idEditHighlightModeText       = XRCID("idEditHighlightModeText");
int idEditBookmarks               = XRCID("idEditBookmarks");
int idEditBookmarksToggle         = XRCID("idEditBookmarksToggle");
int idEditBookmarksPrevious       = XRCID("idEditBookmarksPrevious");
int idEditBookmarksNext           = XRCID("idEditBookmarksNext");
int idEditFolding                 = XRCID("idEditFolding");
int idEditFoldAll                 = XRCID("idEditFoldAll");
int idEditUnfoldAll               = XRCID("idEditUnfoldAll");
int idEditToggleAllFolds          = XRCID("idEditToggleAllFolds");
int idEditFoldBlock               = XRCID("idEditFoldBlock");
int idEditUnfoldBlock             = XRCID("idEditUnfoldBlock");
int idEditToggleFoldBlock         = XRCID("idEditToggleFoldBlock");
int idEditEOLMode                 = XRCID("idEditEOLMode");
int idEditEOLCRLF                 = XRCID("idEditEOLCRLF");
int idEditEOLCR                   = XRCID("idEditEOLCR");
int idEditEOLLF                   = XRCID("idEditEOLLF");
int idEditEncoding                = XRCID("idEditEncoding");
int idEditEncodingDefault         = XRCID("idEditEncodingDefault");
int idEditEncodingUseBom          = XRCID("idEditEncodingUseBom");
int idEditEncodingAscii           = XRCID("idEditEncodingAscii");
int idEditEncodingUtf7            = XRCID("idEditEncodingUtf7");
int idEditEncodingUtf8            = XRCID("idEditEncodingUtf8");
int idEditEncodingUnicode         = XRCID("idEditEncodingUnicode");
int idEditEncodingUtf16           = XRCID("idEditEncodingUtf16");
int idEditEncodingUtf32           = XRCID("idEditEncodingUtf32");
int idEditEncodingUnicode16BE     = XRCID("idEditEncodingUnicode16BE");
int idEditEncodingUnicode16LE     = XRCID("idEditEncodingUnicode16LE");
int idEditEncodingUnicode32BE     = XRCID("idEditEncodingUnicode32BE");
int idEditEncodingUnicode32LE     = XRCID("idEditEncodingUnicode32LE");
int idEditSpecialCommands         = XRCID("idEditSpecialCommands");
int idEditSpecialCommandsMovement = XRCID("idEditSpecialCommandsMovement");
int idEditParaUp                  = XRCID("idEditParaUp");
int idEditParaUpExtend            = XRCID("idEditParaUpExtend");
int idEditParaDown                = XRCID("idEditParaDown");
int idEditParaDownExtend          = XRCID("idEditParaDownExtend");
int idEditWordPartLeft            = XRCID("idEditWordPartLeft");
int idEditWordPartLeftExtend      = XRCID("idEditWordPartLeftExtend");
int idEditWordPartRight           = XRCID("idEditWordPartRight");
int idEditWordPartRightExtend     = XRCID("idEditWordPartRightExtend");
int idEditSpecialCommandsZoom     = XRCID("idEditSpecialCommandsZoom");
int idEditZoomIn                  = XRCID("idEditZoomIn");
int idEditZoomOut                 = XRCID("idEditZoomOut");
int idEditZoomReset               = XRCID("idEditZoomReset");
int idEditSpecialCommandsLine     = XRCID("idEditSpecialCommandsLine");
int idEditLineCut                 = XRCID("idEditLineCut");
int idEditLineDelete              = XRCID("idEditLineDelete");
int idEditLineDuplicate           = XRCID("idEditLineDuplicate");
int idEditLineTranspose           = XRCID("idEditLineTranspose");
int idEditLineCopy                = XRCID("idEditLineCopy");
int idEditLinePaste               = XRCID("idEditLinePaste");
int idEditSpecialCommandsCase     = XRCID("idEditSpecialCommandsCase");
int idEditUpperCase               = XRCID("idEditUpperCase");
int idEditLowerCase               = XRCID("idEditLowerCase");
int idEditSpecialCommandsOther    = XRCID("idEditSpecialCommandsOther");
int idEditInsertNewLine           = XRCID("idEditInsertNewLine");
int idEditGotoLineEnd             = XRCID("idEditGotoLineEnd");
int idEditSelectAll               = XRCID("idEditSelectAll");
int idEditCommentSelected         = XRCID("idEditCommentSelected");
int idEditUncommentSelected       = XRCID("idEditUncommentSelected");
int idEditToggleCommentSelected   = XRCID("idEditToggleCommentSelected");
int idEditStreamCommentSelected   = XRCID("idEditStreamCommentSelected");
int idEditBoxCommentSelected      = XRCID("idEditBoxCommentSelected");

int idViewLayoutDelete       = XRCID("idViewLayoutDelete");
int idViewLayoutSave         = XRCID("idViewLayoutSave");
int idViewToolbars           = XRCID("idViewToolbars");
int idViewToolMain           = XRCID("idViewToolMain");
int idViewToolDebugger       = XRCID("idViewToolDebugger");
int idViewManager            = XRCID("idViewManager");
int idViewLogManager         = XRCID("idViewLogManager");
int idViewStatusbar          = XRCID("idViewStatusbar");
int idViewScriptConsole      = XRCID("idViewScriptConsole");
int idViewFocusEditor        = XRCID("idViewFocusEditor");
int idViewFocusManagement    = XRCID("idViewFocusManagement");
int idViewFocusLogsAndOthers = XRCID("idViewFocusLogsAndOthers");
int idViewSwitchTabs         = XRCID("idViewSwitchTabs");
int idViewFullScreen         = XRCID("idViewFullScreen");

int idSearchFind                = XRCID("idSearchFind");
int idSearchFindInFiles         = XRCID("idSearchFindInFiles");
int idSearchFindNext            = XRCID("idSearchFindNext");
int idSearchFindPrevious        = XRCID("idSearchFindPrevious");
int idSearchReplace             = XRCID("idSearchReplace");
int idSearchReplaceInFiles      = XRCID("idSearchReplaceInFiles");
int idSearchGotoLine            = XRCID("idSearchGotoLine");
int idSearchGotoNextChanged     = XRCID("idSearchGotoNextChanged");
int idSearchGotoPreviousChanged = XRCID("idSearchGotoPreviousChanged");

int idSettingsEnvironment    = XRCID("idSettingsEnvironment");
int idSettingsGlobalUserVars = XRCID("idSettingsGlobalUserVars");
int idSettingsEditor         = XRCID("idSettingsEditor");
int idSettingsCompiler       = XRCID("idSettingsCompiler");
int idSettingsDebugger       = XRCID("idSettingsDebugger");
int idPluginsManagePlugins   = XRCID("idPluginsManagePlugins");
int idSettingsScripting      = XRCID("idSettingsScripting");

int idHelpTips    = XRCID("idHelpTips");
int idHelpPlugins = XRCID("idHelpPlugins");

int idLeftSash              = XRCID("idLeftSash");
int idBottomSash            = XRCID("idBottomSash");
int idCloseFullScreen       = XRCID("idCloseFullScreen");
int idShiftTab              = wxNewId();
int idCtrlAltTab            = wxNewId();
int idStartHerePageLink     = wxNewId();
int idStartHerePageVarSubst = wxNewId();

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_ERASE_BACKGROUND(MainFrame::OnEraseBackground)
    EVT_SIZE(MainFrame::OnSize)
    EVT_CLOSE(MainFrame::OnApplicationClose)

    EVT_UPDATE_UI(idFileOpenRecentFileClearHistory,    MainFrame::OnFileMenuUpdateUI)
    EVT_UPDATE_UI(idFileOpenRecentProjectClearHistory, MainFrame::OnFileMenuUpdateUI)
    EVT_UPDATE_UI(idFileSave,                          MainFrame::OnFileMenuUpdateUI)
    EVT_UPDATE_UI(idFileSaveAs,                        MainFrame::OnFileMenuUpdateUI)
    EVT_UPDATE_UI(idFileSaveAllFiles,                  MainFrame::OnFileMenuUpdateUI)
    EVT_UPDATE_UI(idFileOpenDefWorkspace,              MainFrame::OnFileMenuUpdateUI)
    EVT_UPDATE_UI(idFileSaveWorkspace,                 MainFrame::OnFileMenuUpdateUI)
    EVT_UPDATE_UI(idFileSaveWorkspaceAs,               MainFrame::OnFileMenuUpdateUI)
    EVT_UPDATE_UI(idFileCloseWorkspace,                MainFrame::OnFileMenuUpdateUI)
    EVT_UPDATE_UI(idFileClose,                         MainFrame::OnFileMenuUpdateUI)
    EVT_UPDATE_UI(idFileCloseAll,                      MainFrame::OnFileMenuUpdateUI)
    EVT_UPDATE_UI(idFilePrintSetup,                    MainFrame::OnFileMenuUpdateUI)
    EVT_UPDATE_UI(idFilePrint,                         MainFrame::OnFileMenuUpdateUI)
    EVT_UPDATE_UI(idFileSaveProject,                   MainFrame::OnProjectMenuUpdateUI)
    EVT_UPDATE_UI(idFileSaveProjectAs,                 MainFrame::OnProjectMenuUpdateUI)
    EVT_UPDATE_UI(idFileSaveProjectAllProjects,        MainFrame::OnProjectMenuUpdateUI)
    EVT_UPDATE_UI(idFileSaveProjectTemplate,           MainFrame::OnProjectMenuUpdateUI)
    EVT_UPDATE_UI(idFileSaveAll,                       MainFrame::OnProjectMenuUpdateUI)
    EVT_UPDATE_UI(idFileCloseProject,                  MainFrame::OnProjectMenuUpdateUI)
    EVT_UPDATE_UI(idFileCloseAllProjects,              MainFrame::OnProjectMenuUpdateUI)

    EVT_UPDATE_UI(idEditUndo,                  MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditRedo,                  MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditClearHistory,          MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditCopy,                  MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditCut,                   MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditPaste,                 MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditSwapHeaderSource,      MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditGotoMatchingBrace,     MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditFoldAll,               MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditUnfoldAll,             MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditToggleAllFolds,        MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditFoldBlock,             MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditUnfoldBlock,           MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditToggleFoldBlock,       MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditEOLCRLF,               MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditEOLCR,                 MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditEOLLF,                 MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditEncoding,              MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditSelectAll,             MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditBookmarksToggle,       MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditBookmarksNext,         MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditBookmarksPrevious,     MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditCommentSelected,       MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditUncommentSelected,     MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditToggleCommentSelected, MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditStreamCommentSelected, MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditBoxCommentSelected,    MainFrame::OnEditMenuUpdateUI)

    EVT_UPDATE_UI(idSearchFind,                MainFrame::OnSearchMenuUpdateUI)
    EVT_UPDATE_UI(idSearchFindInFiles,         MainFrame::OnSearchMenuUpdateUI)
    EVT_UPDATE_UI(idSearchFindNext,            MainFrame::OnSearchMenuUpdateUI)
    EVT_UPDATE_UI(idSearchFindPrevious,        MainFrame::OnSearchMenuUpdateUI)
    EVT_UPDATE_UI(idSearchReplace,             MainFrame::OnSearchMenuUpdateUI)
    EVT_UPDATE_UI(idSearchReplaceInFiles,      MainFrame::OnSearchMenuUpdateUI)
    EVT_UPDATE_UI(idSearchGotoLine,            MainFrame::OnSearchMenuUpdateUI)
    EVT_UPDATE_UI(idSearchGotoNextChanged,     MainFrame::OnSearchMenuUpdateUI)
    EVT_UPDATE_UI(idSearchGotoPreviousChanged, MainFrame::OnSearchMenuUpdateUI)

    EVT_UPDATE_UI(idViewToolMain,           MainFrame::OnViewMenuUpdateUI)
    EVT_UPDATE_UI(idViewLogManager,         MainFrame::OnViewMenuUpdateUI)
    EVT_UPDATE_UI(idViewManager,            MainFrame::OnViewMenuUpdateUI)
    EVT_UPDATE_UI(idViewStatusbar,          MainFrame::OnViewMenuUpdateUI)
    EVT_UPDATE_UI(idViewScriptConsole,      MainFrame::OnViewMenuUpdateUI)
    EVT_UPDATE_UI(idViewFocusEditor,        MainFrame::OnViewMenuUpdateUI)
    EVT_UPDATE_UI(idViewFocusManagement,    MainFrame::OnViewMenuUpdateUI)
    EVT_UPDATE_UI(idViewFocusLogsAndOthers, MainFrame::OnViewMenuUpdateUI)
    EVT_UPDATE_UI(idViewFullScreen,         MainFrame::OnViewMenuUpdateUI)

    EVT_MENU(idFileNewEmpty,   MainFrame::OnFileNewWhat)
    EVT_MENU(idFileNewProject, MainFrame::OnFileNewWhat)
    EVT_MENU(idFileNewTarget,  MainFrame::OnFileNewWhat)
    EVT_MENU(idFileNewFile,    MainFrame::OnFileNewWhat)
    EVT_MENU(idFileNewCustom,  MainFrame::OnFileNewWhat)
    EVT_MENU(idFileNewUser,    MainFrame::OnFileNewWhat)

    EVT_MENU(idToolNew, MainFrame::OnFileNew)
    EVT_MENU(idFileOpen,                          MainFrame::OnFileOpen)
    EVT_MENU(idFileOpenRecentProjectClearHistory, MainFrame::OnFileOpenRecentProjectClearHistory)
    EVT_MENU(idFileOpenRecentFileClearHistory,    MainFrame::OnFileOpenRecentClearHistory)
    EVT_MENU_RANGE(wxID_CBFILE01, wxID_CBFILE16,  MainFrame::OnFileReopen)
    EVT_MENU_RANGE(wxID_CBFILE17, wxID_CBFILE32,  MainFrame::OnFileReopenProject)
    EVT_MENU(idFileImportProjectDevCpp,           MainFrame::OnFileImportProjectDevCpp)
    EVT_MENU(idFileImportProjectMSVC,             MainFrame::OnFileImportProjectMSVC)
    EVT_MENU(idFileImportProjectMSVCWksp,         MainFrame::OnFileImportProjectMSVCWksp)
    EVT_MENU(idFileImportProjectMSVS,             MainFrame::OnFileImportProjectMSVS)
    EVT_MENU(idFileImportProjectMSVSWksp,         MainFrame::OnFileImportProjectMSVSWksp)
    EVT_MENU(idFileSave,                          MainFrame::OnFileSave)
    EVT_MENU(idFileSaveAs,                        MainFrame::OnFileSaveAs)
    EVT_MENU(idFileSaveAllFiles,                  MainFrame::OnFileSaveAllFiles)
    EVT_MENU(idFileSaveProject,                   MainFrame::OnFileSaveProject)
    EVT_MENU(idFileSaveProjectAs,                 MainFrame::OnFileSaveProjectAs)
    EVT_MENU(idFileSaveProjectTemplate,           MainFrame::OnFileSaveProjectTemplate)
    EVT_MENU(idFileSaveProjectAllProjects,        MainFrame::OnFileSaveProjectAllProjects)
    EVT_MENU(idFileOpenDefWorkspace,              MainFrame::OnFileOpenDefWorkspace)
    EVT_MENU(idFileSaveWorkspace,                 MainFrame::OnFileSaveWorkspace)
    EVT_MENU(idFileSaveWorkspaceAs,               MainFrame::OnFileSaveWorkspaceAs)
    EVT_MENU(idFileSaveAll,                       MainFrame::OnFileSaveAll)
    EVT_MENU(idFileCloseWorkspace,                MainFrame::OnFileCloseWorkspace)
    EVT_MENU(idFileClose,                         MainFrame::OnFileClose)
    EVT_MENU(idFileCloseAll,                      MainFrame::OnFileCloseAll)
    EVT_MENU(idFileCloseProject,                  MainFrame::OnFileCloseProject)
    EVT_MENU(idFileCloseAllProjects,              MainFrame::OnFileCloseAllProjects)
    EVT_MENU(idFilePrint,                         MainFrame::OnFilePrint)
    EVT_MENU(idFileExit,                          MainFrame::OnFileQuit)
    EVT_MENU(idFileNext,                          MainFrame::OnFileNext)
    EVT_MENU(idFilePrev,                          MainFrame::OnFilePrev)

    EVT_MENU(idEditUndo,                  MainFrame::OnEditUndo)
    EVT_MENU(idEditRedo,                  MainFrame::OnEditRedo)
    EVT_MENU(idEditClearHistory,          MainFrame::OnEditClearHistory)
    EVT_MENU(idEditCopy,                  MainFrame::OnEditCopy)
    EVT_MENU(idEditCut,                   MainFrame::OnEditCut)
    EVT_MENU(idEditPaste,                 MainFrame::OnEditPaste)
    EVT_MENU(idEditSwapHeaderSource,      MainFrame::OnEditSwapHeaderSource)
    EVT_MENU(idEditGotoMatchingBrace,     MainFrame::OnEditGotoMatchingBrace)
    EVT_MENU(idEditHighlightModeText,     MainFrame::OnEditHighlightMode)
    EVT_MENU(idEditFoldAll,               MainFrame::OnEditFoldAll)
    EVT_MENU(idEditUnfoldAll,             MainFrame::OnEditUnfoldAll)
    EVT_MENU(idEditToggleAllFolds,        MainFrame::OnEditToggleAllFolds)
    EVT_MENU(idEditFoldBlock,             MainFrame::OnEditFoldBlock)
    EVT_MENU(idEditUnfoldBlock,           MainFrame::OnEditUnfoldBlock)
    EVT_MENU(idEditToggleFoldBlock,       MainFrame::OnEditToggleFoldBlock)
    EVT_MENU(idEditEOLCRLF,               MainFrame::OnEditEOLMode)
    EVT_MENU(idEditEOLCR,                 MainFrame::OnEditEOLMode)
    EVT_MENU(idEditEOLLF,                 MainFrame::OnEditEOLMode)
    EVT_MENU(idEditEncodingDefault,       MainFrame::OnEditEncoding)
    EVT_MENU(idEditEncodingUseBom,        MainFrame::OnEditEncoding)
    EVT_MENU(idEditEncodingAscii,         MainFrame::OnEditEncoding)
    EVT_MENU(idEditEncodingUtf7,          MainFrame::OnEditEncoding)
    EVT_MENU(idEditEncodingUtf8,          MainFrame::OnEditEncoding)
    EVT_MENU(idEditEncodingUnicode,       MainFrame::OnEditEncoding)
    EVT_MENU(idEditEncodingUtf16,         MainFrame::OnEditEncoding)
    EVT_MENU(idEditEncodingUtf32,         MainFrame::OnEditEncoding)
    EVT_MENU(idEditEncodingUnicode16BE,   MainFrame::OnEditEncoding)
    EVT_MENU(idEditEncodingUnicode16LE,   MainFrame::OnEditEncoding)
    EVT_MENU(idEditEncodingUnicode32BE,   MainFrame::OnEditEncoding)
    EVT_MENU(idEditEncodingUnicode32LE,   MainFrame::OnEditEncoding)
    EVT_MENU(idEditParaUp,                MainFrame::OnEditParaUp)
    EVT_MENU(idEditParaUpExtend,          MainFrame::OnEditParaUpExtend)
    EVT_MENU(idEditParaDown,              MainFrame::OnEditParaDown)
    EVT_MENU(idEditParaDownExtend,        MainFrame::OnEditParaDownExtend)
    EVT_MENU(idEditWordPartLeft,          MainFrame::OnEditWordPartLeft)
    EVT_MENU(idEditWordPartLeftExtend,    MainFrame::OnEditWordPartLeftExtend)
    EVT_MENU(idEditWordPartRight,         MainFrame::OnEditWordPartRight)
    EVT_MENU(idEditWordPartRightExtend,   MainFrame::OnEditWordPartRightExtend)
    EVT_MENU(idEditZoomIn,                MainFrame::OnEditZoomIn)
    EVT_MENU(idEditZoomOut,               MainFrame::OnEditZoomOut)
    EVT_MENU(idEditZoomReset,             MainFrame::OnEditZoomReset)
    EVT_MENU(idEditLineCut,               MainFrame::OnEditLineCut)
    EVT_MENU(idEditLineDelete,            MainFrame::OnEditLineDelete)
    EVT_MENU(idEditLineDuplicate,         MainFrame::OnEditLineDuplicate)
    EVT_MENU(idEditLineTranspose,         MainFrame::OnEditLineTranspose)
    EVT_MENU(idEditLineCopy,              MainFrame::OnEditLineCopy)
    EVT_MENU(idEditLinePaste,             MainFrame::OnEditLinePaste)
    EVT_MENU(idEditUpperCase,             MainFrame::OnEditUpperCase)
    EVT_MENU(idEditLowerCase,             MainFrame::OnEditLowerCase)
    EVT_MENU(idEditInsertNewLine,         MainFrame::OnEditInsertNewLine)
    EVT_MENU(idEditGotoLineEnd,           MainFrame::OnEditGotoLineEnd)
    EVT_MENU(idEditSelectAll,             MainFrame::OnEditSelectAll)
    EVT_MENU(idEditBookmarksToggle,       MainFrame::OnEditBookmarksToggle)
    EVT_MENU(idEditBookmarksNext,         MainFrame::OnEditBookmarksNext)
    EVT_MENU(idEditBookmarksPrevious,     MainFrame::OnEditBookmarksPrevious)
    EVT_MENU(idEditCommentSelected,       MainFrame::OnEditCommentSelected)
    EVT_MENU(idEditUncommentSelected,     MainFrame::OnEditUncommentSelected)
    EVT_MENU(idEditToggleCommentSelected, MainFrame::OnEditToggleCommentSelected)
    EVT_MENU(idEditStreamCommentSelected, MainFrame::OnEditStreamCommentSelected)
    EVT_MENU(idEditBoxCommentSelected,    MainFrame::OnEditBoxCommentSelected)

    EVT_MENU(idSearchFind,                MainFrame::OnSearchFind)
    EVT_MENU(idSearchFindInFiles,         MainFrame::OnSearchFind)
    EVT_MENU(idSearchFindNext,            MainFrame::OnSearchFindNext)
    EVT_MENU(idSearchFindPrevious,        MainFrame::OnSearchFindNext)
    EVT_MENU(idSearchReplace,             MainFrame::OnSearchReplace)
    EVT_MENU(idSearchReplaceInFiles,      MainFrame::OnSearchReplace)
    EVT_MENU(idSearchGotoLine,            MainFrame::OnSearchGotoLine)
    EVT_MENU(idSearchGotoNextChanged,     MainFrame::OnSearchGotoNextChanged)
    EVT_MENU(idSearchGotoPreviousChanged, MainFrame::OnSearchGotoPrevChanged)

    EVT_MENU(idViewLayoutSave,            MainFrame::OnViewLayoutSave)
    EVT_MENU(idViewLayoutDelete,          MainFrame::OnViewLayoutDelete)
    EVT_MENU(idViewToolMain,              MainFrame::OnToggleBar)
    EVT_MENU(XRCID("idViewToolDebugger"), MainFrame::OnToggleBar)
    EVT_MENU(idViewLogManager,            MainFrame::OnToggleBar)
    EVT_MENU(idViewManager,               MainFrame::OnToggleBar)
    EVT_MENU(idViewStatusbar,             MainFrame::OnToggleStatusBar)
    EVT_MENU(idViewScriptConsole,         MainFrame::OnViewScriptConsole)
    EVT_MENU(idViewFocusEditor,           MainFrame::OnFocusEditor)
    EVT_MENU(idViewFocusManagement,       MainFrame::OnFocusManagement)
    EVT_MENU(idViewFocusLogsAndOthers,    MainFrame::OnFocusLogsAndOthers)
    EVT_MENU(idViewSwitchTabs,            MainFrame::OnSwitchTabs)
    EVT_MENU(idViewFullScreen,            MainFrame::OnToggleFullScreen)

    EVT_MENU(idSettingsEnvironment,    MainFrame::OnSettingsEnvironment)
    EVT_MENU(idSettingsGlobalUserVars, MainFrame::OnGlobalUserVars)
    EVT_MENU(idSettingsEditor,         MainFrame::OnSettingsEditor)
    EVT_MENU(idSettingsCompiler,       MainFrame::OnSettingsCompiler)
    EVT_MENU(idSettingsDebugger,       MainFrame::OnSettingsDebugger)
    EVT_MENU(idPluginsManagePlugins,   MainFrame::OnSettingsPlugins)
    EVT_MENU(idSettingsScripting,      MainFrame::OnSettingsScripting)

    EVT_MENU(wxID_ABOUT, MainFrame::OnHelpAbout)
    EVT_MENU(idHelpTips, MainFrame::OnHelpTips)

    EVT_MENU(idStartHerePageLink,     MainFrame::OnStartHereLink)
    EVT_MENU(idStartHerePageVarSubst, MainFrame::OnStartHereVarSubst)

    EVT_CBAUIBOOK_LEFT_DCLICK(ID_NBEditorManager, MainFrame::OnNotebookDoubleClick)
    EVT_NOTEBOOK_PAGE_CHANGED(ID_NBEditorManager, MainFrame::OnPageChanged)

    /// CloseFullScreen event handling
    EVT_BUTTON(idCloseFullScreen, MainFrame::OnToggleFullScreen)

    /// Shift-Tab bug workaround
    EVT_MENU(idShiftTab,   MainFrame::OnShiftTab)
    EVT_MENU(idCtrlAltTab, MainFrame::OnCtrlAltTab)

END_EVENT_TABLE()

MainFrame::MainFrame(wxWindow* parent)
       : wxFrame(parent, -1, _T("MainWin"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE),
       m_LayoutManager(this),
       m_pAccel(0L),
       m_pFilesHistory(0),
       m_pProjectsHistory(0),
       m_pCloseFullScreenBtn(0L),
       m_pEdMan(0L),
       m_pPrjMan(0L),
       m_pLogMan(0L),
       m_pInfoPane(0L),
       m_pToolbar(0L),
       m_ToolsMenu(0L),
       m_HelpPluginsMenu(0L),
       m_StartupDone(false), // one-time flag
       m_InitiatedShutdown(false),
       m_AutoHideLockCounter(0),
       m_LastCtrlAltTabWindow(0),
       m_LastLayoutIsTemp(false),
       m_pScriptConsole(0),
       m_pBatchBuildDialog(0),
       m_pProgressBar(0)
{
    Manager::Get(this); // provide manager with handle to MainFrame (this)

    // register event sinks
    RegisterEvents();

    // New: Allow drag and drop of files into the editor
    SetDropTarget(new cbFileDropTarget(this));

    // Accelerator table
    wxAcceleratorEntry entries[8];
    entries[0].Set(wxACCEL_CTRL | wxACCEL_SHIFT,  (int) 'W', idFileCloseAll);
    entries[1].Set(wxACCEL_CTRL | wxACCEL_SHIFT,  WXK_F4,    idFileCloseAll);
    entries[2].Set(wxACCEL_CTRL,                  (int) 'W', idFileClose);
    entries[3].Set(wxACCEL_CTRL,                  WXK_F4,    idFileClose);
    entries[4].Set(wxACCEL_CTRL,                  WXK_F6,    idFileNext);
    entries[5].Set(wxACCEL_CTRL | wxACCEL_SHIFT,  WXK_F6,    idFilePrev);
    entries[6].Set(wxACCEL_SHIFT,                 WXK_TAB,   idShiftTab);
    entries[7].Set(wxACCEL_CTRL | wxACCEL_ALT,    WXK_TAB,   idCtrlAltTab);
    m_pAccel = new wxAcceleratorTable(8, entries);

    SetAcceleratorTable(*m_pAccel);

    // add file filters for supported projects/workspaces
    FileFilters::AddDefaultFileFilters();

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("app"));
    m_SmallToolBar = cfg->ReadBool(_T("/environment/toolbar_size"), true);
    CreateIDE();

#ifdef __WXMSW__
    SetIcon(wxICON(A_MAIN_ICON));
#else
    SetIcon(wxIcon(app));
#endif // __WXMSW__

#if wxUSE_STATUSBAR
    // even it is possible that the statusbar is not visible at the moment, create the statusbar so the plugins can create their own fields on the it:
    DoCreateStatusBar();
    SetStatusText(_("Welcome to ")+ appglobals::AppName + _T("!"));

    wxStatusBar *sb = GetStatusBar();
    if (sb)
        sb->Show(cfg->ReadBool(_T("/main_frame/statusbar"), true));
#endif // wxUSE_STATUSBAR
    SetTitle(appglobals::AppName + _T(" v") + appglobals::AppVersion);

    ScanForPlugins();

    // save default view
    wxString deflayout = cfg->Read(_T("/main_frame/layout/default"));
    if (deflayout.IsEmpty())
        cfg->Write(_T("/main_frame/layout/default"), gDefaultLayout);
    DoFixToolbarsLayout();
    gDefaultLayoutData = m_LayoutManager.SavePerspective(); // keep the "hardcoded" layout handy
    gDefaultMessagePaneLayoutData = m_pInfoPane->SaveTabOrder();
    SaveViewLayout(gDefaultLayout, gDefaultLayoutData, gDefaultMessagePaneLayoutData);

    // generate default minimal layout
    wxAuiPaneInfoArray& panes = m_LayoutManager.GetAllPanes();
    for (size_t i = 0; i < panes.GetCount(); ++i)
    {
        wxAuiPaneInfo& info = panes[i];
        if (!(info.name == _T("MainPane")))
            info.Hide();
    }
    gMinimalLayoutData = m_LayoutManager.SavePerspective(); // keep the "hardcoded" layout handy
    gMinimalMessagePaneLayoutData = m_pInfoPane->SaveTabOrder();
    SaveViewLayout(gMinimalLayout, gMinimalLayoutData, gMinimalMessagePaneLayoutData);

    LoadWindowState();

    ShowHideStartPage();

    RegisterScriptFunctions();
    RunStartupScripts();

    Manager::Get()->GetLogManager()->DebugLog(_T("Initializing plugins..."));
}

MainFrame::~MainFrame()
{
    SetAcceleratorTable(wxNullAcceleratorTable);
    delete m_pAccel;

    DeInitPrinting();
}

void MainFrame::RegisterEvents()
{
    Manager* m = Manager::Get();

    m->RegisterEventSink(cbEVT_EDITOR_UPDATE_UI,       new cbEventFunctor<MainFrame, CodeBlocksEvent>(this, &MainFrame::OnEditorUpdateUI));

    m->RegisterEventSink(cbEVT_PROJECT_ACTIVATE,       new cbEventFunctor<MainFrame, CodeBlocksEvent>(this, &MainFrame::OnProjectActivated));
    m->RegisterEventSink(cbEVT_PROJECT_OPEN,           new cbEventFunctor<MainFrame, CodeBlocksEvent>(this, &MainFrame::OnProjectOpened));
    m->RegisterEventSink(cbEVT_PROJECT_CLOSE,          new cbEventFunctor<MainFrame, CodeBlocksEvent>(this, &MainFrame::OnProjectClosed));
    m->RegisterEventSink(cbEVT_EDITOR_CLOSE,           new cbEventFunctor<MainFrame, CodeBlocksEvent>(this, &MainFrame::OnEditorClosed));
    m->RegisterEventSink(cbEVT_EDITOR_OPEN,            new cbEventFunctor<MainFrame, CodeBlocksEvent>(this, &MainFrame::OnEditorOpened));
    m->RegisterEventSink(cbEVT_EDITOR_ACTIVATED,       new cbEventFunctor<MainFrame, CodeBlocksEvent>(this, &MainFrame::OnEditorActivated));
    m->RegisterEventSink(cbEVT_EDITOR_SAVE,            new cbEventFunctor<MainFrame, CodeBlocksEvent>(this, &MainFrame::OnEditorSaved));
    m->RegisterEventSink(cbEVT_EDITOR_MODIFIED,        new cbEventFunctor<MainFrame, CodeBlocksEvent>(this, &MainFrame::OnEditorModified));

    m->RegisterEventSink(cbEVT_ADD_DOCK_WINDOW,        new cbEventFunctor<MainFrame, CodeBlocksDockEvent>(this, &MainFrame::OnRequestDockWindow));
    m->RegisterEventSink(cbEVT_REMOVE_DOCK_WINDOW,     new cbEventFunctor<MainFrame, CodeBlocksDockEvent>(this, &MainFrame::OnRequestUndockWindow));
    m->RegisterEventSink(cbEVT_SHOW_DOCK_WINDOW,       new cbEventFunctor<MainFrame, CodeBlocksDockEvent>(this, &MainFrame::OnRequestShowDockWindow));
    m->RegisterEventSink(cbEVT_HIDE_DOCK_WINDOW,       new cbEventFunctor<MainFrame, CodeBlocksDockEvent>(this, &MainFrame::OnRequestHideDockWindow));
    m->RegisterEventSink(cbEVT_DOCK_WINDOW_VISIBILITY, new cbEventFunctor<MainFrame, CodeBlocksDockEvent>(this, &MainFrame::OnDockWindowVisibility));

    m->RegisterEventSink(cbEVT_PLUGIN_ATTACHED,        new cbEventFunctor<MainFrame, CodeBlocksEvent>(this, &MainFrame::OnPluginLoaded));
    m->RegisterEventSink(cbEVT_PLUGIN_RELEASED,        new cbEventFunctor<MainFrame, CodeBlocksEvent>(this, &MainFrame::OnPluginUnloaded));
    m->RegisterEventSink(cbEVT_PLUGIN_INSTALLED,       new cbEventFunctor<MainFrame, CodeBlocksEvent>(this, &MainFrame::OnPluginInstalled));
    m->RegisterEventSink(cbEVT_PLUGIN_UNINSTALLED,     new cbEventFunctor<MainFrame, CodeBlocksEvent>(this, &MainFrame::OnPluginUninstalled));

    m->RegisterEventSink(cbEVT_UPDATE_VIEW_LAYOUT,     new cbEventFunctor<MainFrame, CodeBlocksLayoutEvent>(this, &MainFrame::OnLayoutUpdate));
    m->RegisterEventSink(cbEVT_QUERY_VIEW_LAYOUT,      new cbEventFunctor<MainFrame, CodeBlocksLayoutEvent>(this, &MainFrame::OnLayoutQuery));
    m->RegisterEventSink(cbEVT_SWITCH_VIEW_LAYOUT,     new cbEventFunctor<MainFrame, CodeBlocksLayoutEvent>(this, &MainFrame::OnLayoutSwitch));

    m->RegisterEventSink(cbEVT_ADD_LOG_WINDOW,         new cbEventFunctor<MainFrame, CodeBlocksLogEvent>(this, &MainFrame::OnAddLogWindow));
    m->RegisterEventSink(cbEVT_REMOVE_LOG_WINDOW,      new cbEventFunctor<MainFrame, CodeBlocksLogEvent>(this, &MainFrame::OnRemoveLogWindow));
    m->RegisterEventSink(cbEVT_HIDE_LOG_WINDOW,        new cbEventFunctor<MainFrame, CodeBlocksLogEvent>(this, &MainFrame::OnHideLogWindow));
    m->RegisterEventSink(cbEVT_SWITCH_TO_LOG_WINDOW,   new cbEventFunctor<MainFrame, CodeBlocksLogEvent>(this, &MainFrame::OnSwitchToLogWindow));
    m->RegisterEventSink(cbEVT_GET_ACTIVE_LOG_WINDOW,  new cbEventFunctor<MainFrame, CodeBlocksLogEvent>(this, &MainFrame::OnGetActiveLogWindow));
    m->RegisterEventSink(cbEVT_SHOW_LOG_MANAGER,       new cbEventFunctor<MainFrame, CodeBlocksLogEvent>(this, &MainFrame::OnShowLogManager));
    m->RegisterEventSink(cbEVT_HIDE_LOG_MANAGER,       new cbEventFunctor<MainFrame, CodeBlocksLogEvent>(this, &MainFrame::OnHideLogManager));
    m->RegisterEventSink(cbEVT_LOCK_LOG_MANAGER,       new cbEventFunctor<MainFrame, CodeBlocksLogEvent>(this, &MainFrame::OnLockLogManager));
    m->RegisterEventSink(cbEVT_UNLOCK_LOG_MANAGER,     new cbEventFunctor<MainFrame, CodeBlocksLogEvent>(this, &MainFrame::OnUnlockLogManager));
}

void MainFrame::ShowTips(bool forceShow)
{
    bool showAtStartup = Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/show_tips"), true);
    if (forceShow || showAtStartup)
    {
        wxString tipsFile = ConfigManager::GetDataFolder() + _T("/tips.txt");
        long tipsIndex = Manager::Get()->GetConfigManager(_T("app"))->ReadInt(_T("/next_tip"), 0);
        wxTipProvider* tipProvider = wxCreateFileTipProvider(tipsFile, tipsIndex);
        showAtStartup = wxShowTip(this, tipProvider, showAtStartup);
        Manager::Get()->GetConfigManager(_T("app"))->Write(_T("/show_tips"), showAtStartup);
        Manager::Get()->GetConfigManager(_T("app"))->Write(_T("/next_tip"), (int)tipProvider->GetCurrentTip());
        delete tipProvider;
    }
}

void MainFrame::CreateIDE()
{
    int leftW = Manager::Get()->GetConfigManager(_T("app"))->ReadInt(_T("/main_frame/layout/left_block_width"), 200);
//    int bottomH = Manager::Get()->GetConfigManager(_T("app"))->ReadInt(_T("/main_frame/layout/bottom_block_height"), 150);
    SetSize(800, 600);
    wxSize clientsize = GetClientSize();

    // Create CloseFullScreen Button, and hide it initially
    m_pCloseFullScreenBtn = new wxButton(this, idCloseFullScreen, _( "Close full screen" ), wxDefaultPosition );
    m_pCloseFullScreenBtn->Show( false );

    // management panel
    m_LayoutManager.AddPane( Manager::Get()->GetProjectManager()->GetNotebook(),
                             wxAuiPaneInfo().Name(wxT("ManagementPane")).Caption(_("Management")).
                                 BestSize(wxSize(leftW, clientsize.GetHeight())).
                                 MinSize(wxSize(100,100)).Left().Layer(1) );

    // logs manager
    SetupGUILogging();
    SetupDebuggerUI();

    CreateMenubar();

    m_pEdMan  = Manager::Get()->GetEditorManager();
    m_pPrjMan = Manager::Get()->GetProjectManager();
    m_pLogMan = Manager::Get()->GetLogManager();

    CreateToolbars();
    SetToolBar(0);

    // editor manager
    m_LayoutManager.AddPane(m_pEdMan->GetNotebook(), wxAuiPaneInfo().Name(wxT("MainPane")).
                            CentrePane());

    // script console
    m_pScriptConsole = new ScriptConsole(this, -1);
    m_LayoutManager.AddPane(m_pScriptConsole, wxAuiPaneInfo().Name(wxT("ScriptConsole")).
                            Caption(_("Scripting console")).Float().MinSize(100,100).FloatingPosition(300, 200));

    DoUpdateLayout();
    DoUpdateLayoutColours();
    DoUpdateEditorStyle();

    m_pEdMan->GetNotebook()->SetDropTarget(new cbFileDropTarget(this));
    m_pPrjMan->GetNotebook()->SetDropTarget(new cbFileDropTarget(this));
}


void MainFrame::SetupGUILogging()
{
    // allow new docked windows to use be 3/4 of the available space, the default (0.3) is sometimes too small, especially for "Logs & others"
    m_LayoutManager.SetDockSizeConstraint(0.75,0.75);

    int bottomH = Manager::Get()->GetConfigManager(_T("app"))->ReadInt(_T("/main_frame/layout/bottom_block_height"), 150);
    wxSize clientsize = GetClientSize();

    LogManager* mgr = Manager::Get()->GetLogManager();

    if (!Manager::IsBatchBuild())
    {
        m_pInfoPane = new InfoPane(this);
        m_LayoutManager.AddPane(m_pInfoPane, wxAuiPaneInfo().
                                  Name(wxT("MessagesPane")).Caption(_("Logs & others")).
                                  BestSize(wxSize(clientsize.GetWidth(), bottomH)).//MinSize(wxSize(50,50)).
                                  Bottom());

        wxWindow* log;

        for(size_t i = LogManager::app_log; i < ::max_logs; ++i)
        {
            if ((log = mgr->Slot(i).GetLogger()->CreateControl(m_pInfoPane)))
                m_pInfoPane->AddLogger(mgr->Slot(i).GetLogger(), log, mgr->Slot(i).title, mgr->Slot(i).icon);
        }
    }
    else
    {
        m_pBatchBuildDialog = new BatchLogWindow(this, _("Code::Blocks - Batch build"));
        wxSizer* s = new wxBoxSizer(wxVERTICAL);
        m_pInfoPane = new InfoPane(m_pBatchBuildDialog);
        s->Add(m_pInfoPane, 1, wxEXPAND);
        m_pBatchBuildDialog->SetSizer(s);

        // setting &g_null_log causes the app to crash on exit for some reason...
        mgr->SetLog(new NullLogger, LogManager::app_log);
        mgr->SetLog(new NullLogger, LogManager::debug_log);
    }

    mgr->NotifyUpdate();
    m_pInfoPane->SetDropTarget(new cbFileDropTarget(this));
}

void MainFrame::SetupDebuggerUI()
{
    m_debuggerMenuHandler = new DebuggerMenuHandler;
    m_debuggerToolbarHandler = new DebuggerToolbarHandler(m_debuggerMenuHandler);
    m_debuggerMenuHandler->SetEvtHandlerEnabled(false);
    m_debuggerToolbarHandler->SetEvtHandlerEnabled(false);
    wxWindow* window = Manager::Get()->GetAppWindow();
    if (window)
    {
        window->PushEventHandler(m_debuggerMenuHandler);
        window->PushEventHandler(m_debuggerToolbarHandler);
    }
    m_debuggerMenuHandler->SetEvtHandlerEnabled(true);
    m_debuggerToolbarHandler->SetEvtHandlerEnabled(true);

    Manager::Get()->GetDebuggerManager()->SetInterfaceFactory(new DebugInterfaceFactory);
    m_debuggerMenuHandler->RegisterDefaultWindowItems();
}

DECLARE_INSTANCE_TYPE(MainFrame);

void MainFrame::RegisterScriptFunctions()
{
    SqPlus::SQClassDef<MainFrame>("MainFrame").
                    func(&MainFrame::Open, "Open");

    SqPlus::BindVariable(this, "App", SqPlus::VAR_ACCESS_READ_ONLY);
}

void MainFrame::RunStartupScripts()
{
    ConfigManager* mgr = Manager::Get()->GetConfigManager(_T("scripting"));
    wxArrayString keys = mgr->EnumerateKeys(_T("/startup_scripts"));

    for (size_t i = 0; i < keys.GetCount(); ++i)
    {
        ScriptEntry se;
        wxString ser;
        if (mgr->Read(_T("/startup_scripts/") + keys[i], &ser))
        {
            se.SerializeIn(ser);
            if (!se.enabled)
                continue;

            try
            {
                wxString startup = se.script;
                if (wxFileName(se.script).IsRelative())
                    startup = ConfigManager::LocateDataFile(se.script, sdScriptsUser | sdScriptsGlobal);
                if (!startup.IsEmpty())
                {
                    if (!se.registered)
                        Manager::Get()->GetScriptingManager()->LoadScript(startup);
                    else if (!se.menu.IsEmpty())
                        Manager::Get()->GetScriptingManager()->RegisterScriptMenu(se.menu, startup, false);
                    else
                        Manager::Get()->GetLogManager()->LogWarning(F(_("Startup script/function '%s' not loaded: invalid configuration"), se.script.wx_str()));
                }
                else
                    Manager::Get()->GetLogManager()->LogWarning(F(_("Startup script '%s' not found"), se.script.wx_str()));
            }
            catch (SquirrelError& exception)
            {
                Manager::Get()->GetScriptingManager()->DisplayErrors(&exception);
            }
        }
    }
}

void MainFrame::PluginsUpdated(cbPlugin* /*plugin*/, int /*status*/)
{
    Freeze();

    // menu
    RecreateMenuBar();

    // update view->toolbars because we re-created the menubar
    PluginElementsArray plugins = Manager::Get()->GetPluginManager()->GetPlugins();
    for (unsigned int i = 0; i < plugins.GetCount(); ++i)
    {
        cbPlugin* plug = plugins[i]->plugin;
        const PluginInfo* info = Manager::Get()->GetPluginManager()->GetPluginInfo(plug);
        if (!info)
            continue;

        if (m_PluginsTools[plug]) // if plugin has a toolbar
        {
            // toolbar exists; add the menu item
            wxMenu* viewToolbars = 0;
            GetMenuBar()->FindItem(idViewToolMain, &viewToolbars);
            if (viewToolbars)
            {
                if (viewToolbars->FindItem(info->title) != wxNOT_FOUND)
                    continue;
                wxMenuItem* item = AddPluginInMenus(viewToolbars, plug,
                                                    (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&MainFrame::OnToggleBar,
                                                    -1, true);
                if (item)
                {
                    item->Check(IsWindowReallyShown(m_PluginsTools[plug]));
                }
            }
        }
    }

    Thaw();
}

void MainFrame::RecreateMenuBar()
{
    Freeze();

    wxMenuBar* m = GetMenuBar();
    SetMenuBar(0); // unhook old menubar
    CreateMenubar(); // create new menubar
    delete m; // delete old menubar

    // update layouts menu
    for (LayoutViewsMap::iterator it = m_LayoutViews.begin(); it != m_LayoutViews.end(); ++it)
    {
        if (it->first.IsEmpty())
            continue;
        SaveViewLayout(it->first, it->second, m_LayoutMessagePane[it->first], it->first == m_LastLayoutName);
    }

    Thaw();
}

void MainFrame::CreateMenubar()
{
    CodeBlocksEvent event(cbEVT_MENUBAR_CREATE_BEGIN);
    Manager::Get()->ProcessEvent(event);

    int tmpidx;
    wxMenuBar* mbar=0L;
    wxMenu *hl=0L, *tools=0L, *plugs=0L, *pluginsM=0L;
    wxMenuItem *tmpitem=0L;

    wxString resPath = ConfigManager::GetDataFolder();
    wxXmlResource *myres = wxXmlResource::Get();
    myres->Load(resPath + _T("/resources.zip#zip:main_menu.xrc"));
    mbar = myres->LoadMenuBar(_T("main_menu_bar"));
    if (!mbar)
        mbar = new wxMenuBar(); // Some error happened.
    if (mbar)
        SetMenuBar(mbar);

    // Find Menus that we'll change later

    tmpidx = mbar->FindMenu(_("&Edit"));
    if (tmpidx!=wxNOT_FOUND)
    {
        mbar->FindItem(idEditHighlightModeText, &hl);
        if (hl)
        {
            EditorColourSet* theme = Manager::Get()->GetEditorManager()->GetColourSet();
            if (theme)
            {
                wxArrayString langs = theme->GetAllHighlightLanguages();
                for (size_t i = 0; i < langs.GetCount(); ++i)
                {
                    if (i > 0 && !(i % 20))
                        hl->Break(); // break into columns every 20 items
                    int id = wxNewId();
                    hl->AppendRadioItem(id, langs[i],
                                wxString::Format(_("Switch highlighting mode for current document to \"%s\""), langs[i].wx_str()));
                    Connect(id, -1, wxEVT_COMMAND_MENU_SELECTED,
                            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                            &MainFrame::OnEditHighlightMode);
                }
            }
        }
    }

    tmpidx = mbar->FindMenu(_("&Tools"));
    if (tmpidx!=wxNOT_FOUND)
        tools = mbar->GetMenu(tmpidx);

    tmpidx = mbar->FindMenu(_("P&lugins"));
    if (tmpidx!=wxNOT_FOUND)
        plugs = mbar->GetMenu(tmpidx);

    if ((tmpitem = mbar->FindItem(idHelpPlugins,NULL)))
        pluginsM = tmpitem->GetSubMenu();

    m_ToolsMenu       = tools    ? tools    : new wxMenu();
    m_PluginsMenu     = plugs    ? plugs    : new wxMenu();
    m_HelpPluginsMenu = pluginsM ? pluginsM : new wxMenu();

    // core modules: create menus
    Manager::Get()->GetProjectManager()->CreateMenu(mbar);
    Manager::Get()->GetEditorManager()->CreateMenu(mbar);
    Manager::Get()->GetDebuggerManager()->SetMenuHandler(m_debuggerMenuHandler);

    // ask all plugins to rebuild their menus
    PluginElementsArray plugins = Manager::Get()->GetPluginManager()->GetPlugins();
    for (unsigned int i = 0; i < plugins.GetCount(); ++i)
    {
        cbPlugin* plug = plugins[i]->plugin;
        if (plug && plug->IsAttached())
        {
            if (plug->GetType() == ptTool)
                DoAddPlugin(plug);
            else
            {
                AddPluginInSettingsMenu(plug);
                AddPluginInHelpPluginsMenu(plug);
                try
                {
                    plug->BuildMenu(mbar);
                }
                catch (cbException& e)
                {
                    e.ShowErrorMessage();
                }
            }
        }
    }

    Manager::Get()->GetToolsManager()->BuildToolsMenu(m_ToolsMenu);

    // Ctrl+Tab workaround for non windows platforms:
    if ((platform::carbon) || (platform::gtk))
    {
        // Find the menu item for tab switching:
        tmpidx = mbar->FindMenu(_("&View"));
        if (tmpidx != wxNOT_FOUND)
        {
            wxMenu* view = mbar->GetMenu(tmpidx);
            wxMenuItem* switch_item = view->FindItem(idViewSwitchTabs);
            if (switch_item)
            {
                // Change the accelerator for this menu item:
                wxString accel;
                if      (platform::carbon)
                    accel = wxT("Alt+Tab");
                else if (platform::gtk)
                    accel = wxT("Ctrl+,");
                switch_item->SetItemLabel(wxString(_("S&witch tabs")) + wxT("\t") + accel);
            }
        }
    }

    SetMenuBar(mbar);
    InitializeRecentFilesHistory();

    CodeBlocksEvent event2(cbEVT_MENUBAR_CREATE_END);
    Manager::Get()->ProcessEvent(event2);
}

void MainFrame::CreateToolbars()
{
    wxXmlResource *myres = wxXmlResource::Get();
    if (m_pToolbar)
    {
        SetToolBar(0L);
        m_pToolbar = 0L;
    }

    wxString resPath = ConfigManager::GetDataFolder();
    wxString xrcToolbarName = _T("main_toolbar");
    if (m_SmallToolBar) // Insert logic here
        xrcToolbarName += _T("_16x16");
    myres->Load(resPath + _T("/resources.zip#zip:*.xrc"));
    Manager::Get()->GetLogManager()->DebugLog(_T("Loading toolbar..."));

    m_pToolbar = Manager::Get()->CreateEmptyToolbar();
    Manager::Get()->AddonToolBar(m_pToolbar, xrcToolbarName);

    m_pToolbar->Realize();

    m_pToolbar->SetInitialSize();

    // add toolbars in docking system
    m_LayoutManager.AddPane(m_pToolbar, wxAuiPaneInfo().
                          Name(wxT("MainToolbar")).Caption(_("Main Toolbar")).
                          ToolbarPane().Top());
    m_LayoutManager.AddPane(m_debuggerToolbarHandler->GetToolbar(), wxAuiPaneInfo().
                          Name(wxT("DebuggerToolbar")).Caption(_("Debugger Toolbar")).
                          ToolbarPane().Top());
    DoUpdateLayout();

    // ask all plugins to rebuild their toolbars
    PluginElementsArray plugins = Manager::Get()->GetPluginManager()->GetPlugins();
    for (unsigned int i = 0; i < plugins.GetCount(); ++i)
    {
        cbPlugin* plug = plugins[i]->plugin;
        if (plug && plug->IsAttached())
            DoAddPluginToolbar(plug);
    }

    Manager::ProcessPendingEvents();
}

void MainFrame::AddToolbarItem(int id, const wxString& title, const wxString& shortHelp, const wxString& longHelp, const wxString& image)
{
    m_pToolbar->AddTool(id, title, cbLoadBitmap(image, wxBITMAP_TYPE_PNG));
    m_pToolbar->SetToolShortHelp(id, shortHelp);
    m_pToolbar->SetToolLongHelp(id, longHelp);
}

void MainFrame::ScanForPlugins()
{
    m_PluginIDsMap.clear();

    PluginManager* m_PluginManager = Manager::Get()->GetPluginManager();

    // user paths first
    wxString path = ConfigManager::GetPluginsFolder(false);
    Manager::Get()->GetLogManager()->Log(_("Scanning for plugins in ") + path);
    int count = m_PluginManager->ScanForPlugins(path);

    // global paths
    path = ConfigManager::GetPluginsFolder(true);
    Manager::Get()->GetLogManager()->Log(_("Scanning for plugins in ") + path);
    count += m_PluginManager->ScanForPlugins(path);

    // actually load plugins
    if (count > 0)
    {
        Manager::Get()->GetLogManager()->Log(_("Loading:"));
        m_PluginManager->LoadAllPlugins();
    }

    CodeBlocksEvent event(cbEVT_PLUGIN_LOADING_COMPLETE);
    Manager::Get()->GetPluginManager()->NotifyPlugins(event);
}

wxMenuItem* MainFrame::AddPluginInMenus(wxMenu* menu, cbPlugin* plugin, wxObjectEventFunction callback, int pos, bool checkable)
{
    wxMenuItem* item = 0;
    if (!plugin || !menu)
        return item;

    const PluginInfo* info = Manager::Get()->GetPluginManager()->GetPluginInfo(plugin);
    if (!info)
        return 0;

    PluginIDsMap::iterator it;
    for (it = m_PluginIDsMap.begin(); it != m_PluginIDsMap.end(); ++it)
    {
        if (it->second == info->name)
        {
            item = menu->FindItem(it->first);
            if (item)
                return item;
        }
    }

    int id = wxNewId();
    wxString title = info->title + (menu == m_HelpPluginsMenu ? _T("...") : wxEmptyString);
    m_PluginIDsMap[id] = info->name;
    if (pos == -1)
        pos = menu->GetMenuItemCount();

    while(!item)
    {
        #if wxCHECK_VERSION(2, 9, 0)
        if (!pos || title.CmpNoCase(menu->FindItemByPosition(pos - 1)->GetItemLabelText()) > 0)
        #else
        if (!pos || title.CmpNoCase(menu->FindItemByPosition(pos - 1)->GetLabel()) > 0)
        #endif
            item = menu->Insert(pos, id, title, wxEmptyString, checkable ? wxITEM_CHECK : wxITEM_NORMAL);

        --pos;
    }

    Connect( id,  wxEVT_COMMAND_MENU_SELECTED, callback );
    return item;
}

void MainFrame::AddPluginInPluginsMenu(cbPlugin* plugin)
{
    // "Plugins" menu is special case because it contains "Manage plugins",
    // which must stay at the end of the menu
    // So we insert entries, not append...

    // this will insert a separator when the first plugin is added in the "Plugins" menu
    if (m_PluginsMenu->GetMenuItemCount() == 1)
         m_PluginsMenu->Insert(0, wxID_SEPARATOR, _T(""));

    AddPluginInMenus(m_PluginsMenu, plugin,
                    (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&MainFrame::OnPluginsExecuteMenu,
                    m_PluginsMenu->GetMenuItemCount() - 2);
}

void MainFrame::AddPluginInSettingsMenu(cbPlugin* /*plugin*/)
{
}

void MainFrame::AddPluginInHelpPluginsMenu(cbPlugin* plugin)
{
    AddPluginInMenus(m_HelpPluginsMenu, plugin,
                    (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&MainFrame::OnHelpPluginMenu);
}

void MainFrame::RemovePluginFromMenus(const wxString& pluginName)
{
    //Manager::Get()->GetLogManager()->DebugLog("Unloading %s plugin", pluginName.wx_str());
    if (pluginName.IsEmpty())
        return;

    // look for plugin's id
    wxArrayInt id;
    PluginIDsMap::iterator it = m_PluginIDsMap.begin();
    while (it != m_PluginIDsMap.end())
    {
        if (pluginName.Matches(it->second))
        {
            id.Add(it->first);
            PluginIDsMap::iterator it2 = it;
            ++it;
            m_PluginIDsMap.erase(it2);
        }
        else
            ++it;
    }
    //Manager::Get()->GetLogManager()->DebugLog("id=%d", id);
    if (id.GetCount() == 0)
        return; // not found

    for (unsigned int i = 0; i < id.GetCount(); ++i)
    {
        Disconnect( id[i],  wxEVT_COMMAND_MENU_SELECTED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &MainFrame::OnPluginsExecuteMenu );
        m_PluginIDsMap.erase(id[i]);
        m_PluginsMenu->Delete(id[i]);
        m_HelpPluginsMenu->Delete(id[i]);
    }
}

void MainFrame::LoadWindowState()
{
    wxArrayString subs = Manager::Get()->GetConfigManager(_T("app"))->EnumerateSubPaths(_T("/main_frame/layout"));
    for (size_t i = 0; i < subs.GetCount(); ++i)
    {
        wxString name = Manager::Get()->GetConfigManager(_T("app"))->Read(_T("/main_frame/layout/") + subs[i] + _T("/name"));
        wxString layout = Manager::Get()->GetConfigManager(_T("app"))->Read(_T("/main_frame/layout/") + subs[i] + _T("/data"));
        wxString layoutMP = Manager::Get()->GetConfigManager(_T("app"))->Read(_T("/main_frame/layout/") + subs[i] + _T("/dataMessagePane"));
        SaveViewLayout(name, layout, layoutMP);
    }
    wxString deflayout = Manager::Get()->GetConfigManager(_T("app"))->Read(_T("/main_frame/layout/default"));
    LoadViewLayout(deflayout);

    // load manager and messages selected page
    Manager::Get()->GetProjectManager()->GetNotebook()->SetSelection(Manager::Get()->GetConfigManager(_T("app"))->ReadInt(_T("/main_frame/layout/left_block_selection"), 0));
    m_pInfoPane->SetSelection(Manager::Get()->GetConfigManager(_T("app"))->ReadInt(_T("/main_frame/layout/bottom_block_selection"), 0));

    // Cryogen 23/3/10 wxAuiNotebook can't set it's own tab position once instantiated, for some reason. This code fails in InfoPane::InfoPane().
    // Moved here as this seems like a resonable place to do UI setup. Feel free to move it elsewhere.
    if (Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/infopane_tabs_bottom"), false))
        m_pInfoPane->SetWindowStyleFlag(m_pInfoPane->GetWindowStyleFlag() | wxAUI_NB_BOTTOM);

#ifndef __WXMAC__
    int x = 0;
    int y = 0;
#else
    int x = 0;
    int y = wxSystemSettings::GetMetric(wxSYS_MENU_Y, this); // make sure it doesn't hide under the menu bar
#endif
    int w = 800;
    int h = 600;

    // load window size and position
    wxRect rect(Manager::Get()->GetConfigManager(_T("app"))->ReadInt(_T("/main_frame/layout/left"),   x),
                Manager::Get()->GetConfigManager(_T("app"))->ReadInt(_T("/main_frame/layout/top"),    y),
                Manager::Get()->GetConfigManager(_T("app"))->ReadInt(_T("/main_frame/layout/width"),  w),
                Manager::Get()->GetConfigManager(_T("app"))->ReadInt(_T("/main_frame/layout/height"), h));
    // maximize if needed
    Maximize(Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/main_frame/layout/maximized"), true));
    // set size and position
    SetSize(rect);
}

void MainFrame::SaveWindowState()
{
    DoCheckCurrentLayoutForChanges(false);

    // first delete all previous layouts, otherwise they might remain
    // if the new amount of layouts is less than the previous, because only the first layouts will be overwritten
    wxArrayString subs = Manager::Get()->GetConfigManager(_T("app"))->EnumerateSubPaths(_T("/main_frame/layout"));
    for (size_t i = 0; i < subs.GetCount(); ++i)
    {
        Manager::Get()->GetConfigManager(_T("app"))->DeleteSubPath(_T("/main_frame/layout/") + subs[i]);
    }

    int count = 0;
    for (LayoutViewsMap::iterator it = m_LayoutViews.begin(); it != m_LayoutViews.end(); ++it)
    {
        if (it->first.IsEmpty())
            continue;
        ++count;
        wxString key = wxString::Format(_T("/main_frame/layout/view%d/"), count);
        Manager::Get()->GetConfigManager(_T("app"))->Write(key + _T("name"), it->first);
        Manager::Get()->GetConfigManager(_T("app"))->Write(key + _T("data"), it->second);

        if (!m_LayoutMessagePane[it->first].IsEmpty())
            Manager::Get()->GetConfigManager(_T("app"))->Write(key + _T("dataMessagePane"), m_LayoutMessagePane[it->first]);
    }

    // save manager and messages selected page
    Manager::Get()->GetConfigManager(_T("app"))->Write(_T("/main_frame/layout/left_block_selection"), Manager::Get()->GetProjectManager()->GetNotebook()->GetSelection());
    Manager::Get()->GetConfigManager(_T("app"))->Write(_T("/main_frame/layout/bottom_block_selection"), m_pInfoPane->GetSelection());

    // save window size and position
    Manager::Get()->GetConfigManager(_T("app"))->Write(_T("/main_frame/layout/maximized"), IsMaximized());
    if (!IsMaximized() && !IsIconized())
    {
        Manager::Get()->GetConfigManager(_T("app"))->Write(_T("/main_frame/layout/left"), GetPosition().x);
        Manager::Get()->GetConfigManager(_T("app"))->Write(_T("/main_frame/layout/top"), GetPosition().y);
        Manager::Get()->GetConfigManager(_T("app"))->Write(_T("/main_frame/layout/width"), GetSize().x);
        Manager::Get()->GetConfigManager(_T("app"))->Write(_T("/main_frame/layout/height"), GetSize().y);
    }
}

void MainFrame::LoadViewLayout(const wxString& name, bool isTemp)
{
    if (m_LastLayoutName != name && !DoCheckCurrentLayoutForChanges(true))
        return;

    m_LastLayoutIsTemp = isTemp;

    wxString layout = m_LayoutViews[name];
    wxString layoutMP = m_LayoutMessagePane[name];
    if (layoutMP.IsEmpty())
        layoutMP = m_LayoutMessagePane[gDefaultLayout];
    if (layout.IsEmpty())
    {
        layout = m_LayoutViews[gDefaultLayout];
        SaveViewLayout(name, layout, layoutMP, false);
        DoSelectLayout(name);
    }
    else
        DoSelectLayout(name);

    // first load taborder of MessagePane, so LoadPerspective can restore the last selected tab
    m_pInfoPane->LoadTabOrder(layoutMP);
    m_LayoutManager.LoadPerspective(layout, false);
    DoFixToolbarsLayout();
    DoUpdateLayout();

    m_PreviousLayoutName = m_LastLayoutName;
    m_LastLayoutName = name;
    m_LastLayoutData = layout;
    m_LastMessagePaneLayoutData = layoutMP;

    CodeBlocksLayoutEvent evt(cbEVT_SWITCHED_VIEW_LAYOUT);
    evt.layout = name;
    Manager::Get()->ProcessEvent(evt);
}

void MainFrame::SaveViewLayout(const wxString& name, const wxString& layout, const wxString& layoutMP, bool select)
{
    if (name.IsEmpty())
        return;
    m_LayoutViews[name] = layout;
    m_LayoutMessagePane[name] = layoutMP;
    wxMenu* viewLayouts = 0;
    GetMenuBar()->FindItem(idViewLayoutSave, &viewLayouts);
    if (viewLayouts && viewLayouts->FindItem(name) == wxNOT_FOUND)
    {
        int id = wxNewId();
        viewLayouts->InsertCheckItem(viewLayouts->GetMenuItemCount() - 3, id, name, wxString::Format(_("Switch to %s perspective"), name.wx_str()));
        Connect( id,  wxEVT_COMMAND_MENU_SELECTED,
            (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&MainFrame::OnViewLayout);
        m_PluginIDsMap[id] = name;
    }
    if (select)
    {
        DoSelectLayout(name);
        m_LastLayoutName = name;
    }
}

bool MainFrame::LayoutDifferent(const wxString& layout1,const wxString& layout2,const wxString& delimiter)
{
    wxStringTokenizer strTok;
    unsigned long j;

    strTok.SetString(layout1, delimiter);
    wxArrayString arLayout1;
    while(strTok.HasMoreTokens())
    {
        wxStringTokenizer strTokColon(strTok.GetNextToken(), _T(";"));
        while(strTokColon.HasMoreTokens())
        {
            wxString theToken = strTokColon.GetNextToken();
            if (theToken.StartsWith(_T("state=")))
            {
                theToken=theToken.Right(theToken.Len() - wxString(_T("state=")).Len());
                theToken.ToULong(&j);
                // we filter out the hidden/show state
                theToken=wxString::Format(_("state=%lu"),j & wxAuiPaneInfo::optionHidden);
            }
               arLayout1.Add(theToken);
        }
    }

    strTok.SetString(layout2, delimiter);
    wxArrayString arLayout2;
    while(strTok.HasMoreTokens())
    {
        wxStringTokenizer strTokColon(strTok.GetNextToken(), _T(";"));
        while(strTokColon.HasMoreTokens())
        {
            wxString theToken = strTokColon.GetNextToken();
            if (theToken.StartsWith(_T("state=")))
            {
                theToken=theToken.Right(theToken.Len() - wxString(_T("state=")).Len());
                theToken.ToULong(&j);
                // we filter out the hidden/show state
                theToken=wxString::Format(_("state=%lu"),j & wxAuiPaneInfo::optionHidden);
            }
               arLayout2.Add(theToken);
        }
    }

    arLayout1.Sort();
    arLayout2.Sort();

    return arLayout1 != arLayout2;
}

bool MainFrame::LayoutMessagePaneDifferent(const wxString& layout1,const wxString& layout2, bool checkSelection)
{
    wxStringTokenizer strTok;
    wxArrayString arLayout1;
    wxArrayString arLayout2;

    strTok.SetString(layout1.BeforeLast('|'), _T(";"));
    while(strTok.HasMoreTokens())
    {
        arLayout1.Add(strTok.GetNextToken());
    }

    strTok.SetString(layout2.BeforeLast('|'), _T(";"));
    while(strTok.HasMoreTokens())
    {
        arLayout2.Add(strTok.GetNextToken());
    }

    if(checkSelection)
    {
        arLayout1.Add(layout1.AfterLast('|'));
        arLayout2.Add(layout2.AfterLast('|'));
    }
    arLayout1.Sort();
    arLayout2.Sort();

    return arLayout1 != arLayout2;
}

bool MainFrame::DoCheckCurrentLayoutForChanges(bool canCancel)
{
    DoFixToolbarsLayout();
    wxString lastlayout = m_LayoutManager.SavePerspective();
    wxString lastmessagepanelayout = m_pInfoPane->SaveTabOrder();
    if (!m_LastLayoutName.IsEmpty() &&
        (LayoutDifferent(lastlayout, m_LastLayoutData) ||
         LayoutMessagePaneDifferent(lastmessagepanelayout, m_LastMessagePaneLayoutData, Manager::Get()->GetConfigManager(_T("message_manager"))->ReadBool(_T("/save_selection_change_in_mp"), true)) ))
    {
        AnnoyingDialog dlg(_("Layout changed"),
                            wxString::Format(_("The perspective '%s' has changed. Do you want to save it?"), m_LastLayoutName.wx_str()),
                            wxART_QUESTION,
                            canCancel ? AnnoyingDialog::YES_NO_CANCEL : AnnoyingDialog::YES_NO,
                            wxID_YES);
        switch (dlg.ShowModal())
        {
            case wxID_YES:
                SaveViewLayout(m_LastLayoutName, lastlayout, lastmessagepanelayout, false);
                break;
            case wxID_CANCEL:
                DoSelectLayout(m_LastLayoutName);
                return false;
            default:
                break;
        }
    }
    return true;
}

void MainFrame::DoFixToolbarsLayout()
{
    // because the user might change the toolbar icons size, we must cater for it...
    wxAuiPaneInfoArray& panes = m_LayoutManager.GetAllPanes();
    for (size_t i = 0; i < panes.GetCount(); ++i)
    {
        wxAuiPaneInfo& info = panes[i];
        if (info.state & wxAuiPaneInfo::optionToolbar)
        {
            info.best_size = info.window->GetSize();
            info.floating_size = wxDefaultSize;
        }
    }
}

void MainFrame::DoSelectLayout(const wxString& name)
{
    wxMenu* viewLayouts = 0;
    GetMenuBar()->FindItem(idViewLayoutSave, &viewLayouts);
    if (viewLayouts)
    {
        wxMenuItemList& items = viewLayouts->GetMenuItems();
        for (size_t i = 0; i < items.GetCount(); ++i)
        {
            if (!items[i]->IsCheckable())
                continue;
            #if wxCHECK_VERSION(2, 9, 0)
            items[i]->Check(items[i]->GetItemLabel().IsSameAs(name));
            #else
            items[i]->Check(items[i]->GetText().IsSameAs(name));
            #endif
        }

        if (!m_LastLayoutIsTemp)
            Manager::Get()->GetConfigManager(_T("app"))->Write(_T("/main_frame/layout/default"), name);
    }
}

void MainFrame::DoAddPluginStatusField(cbPlugin* plugin)
{
#if wxUSE_STATUSBAR
    cbStatusBar *sbar = (cbStatusBar *)GetStatusBar();
    if (!sbar)
        return;
    plugin->CreateStatusField(sbar);
    sbar->AdjustFieldsSize();
#endif
}

void MainFrame::DoAddPluginToolbar(cbPlugin* plugin)
{
    wxToolBar *tb = Manager::Get()->CreateEmptyToolbar();
    if (plugin->BuildToolBar(tb))
    {
        SetToolBar(0);

#if defined __WXMSW__ && !wxCHECK_VERSION(2, 8, 9)
        // HACK: for all windows versions (including XP *without* using a manifest file),
        //       the best size for a toolbar is not correctly calculated by wxWidgets/wxAUI/whatever.
        //       so we try to help the situation a little. It's not perfect, but it works.
        // not needed for versions >= 2.8.9: fixed in upstream, toolbars with standard-controls
        // are much too large with it (at least on w2k).
        if (!UsesCommonControls6()) // all windows versions, including XP without a manifest file
        {
            // calculate the total width of all wxWindow* in the toolbar (if any)
            int w = 0;
            int ccount = 0;
            for (wxWindowList::compatibility_iterator node = tb->GetChildren().GetFirst(); node; node = node->GetNext())
            {
                wxWindow *win = (wxWindow *)node->GetData();
                if (win)
                {
                    w += win->GetSize().GetWidth();
                    ++ccount;
                }
            }
            #if wxCHECK_VERSION(2, 8, 0)
            wxSize s(w + tb->GetEffectiveMinSize().GetWidth() - (ccount * (tb->GetToolSize().GetWidth() / 3)), 0);
            tb->SetInitialSize(s);
            #else
            wxSize s(w + tb->GetBestFittingSize().GetWidth() - (ccount * (tb->GetToolSize().GetWidth() / 3)), 0);
            tb->SetBestFittingSize(s);
            #endif
        }
        else
            tb->SetInitialSize();
        // end of HACK
#else
        tb->SetInitialSize();
#endif

        // add View->Toolbars menu item for toolbar
        wxMenu* viewToolbars = 0;
        GetMenuBar()->FindItem(idViewToolMain, &viewToolbars);
        if (viewToolbars)
        {
            wxMenuItem* item = AddPluginInMenus(viewToolbars, plugin,
                                                (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&MainFrame::OnToggleBar,
                                                -1, true);
            if (item)
            {
                item->Check(true);
                m_PluginsTools[plugin] = tb;
            }
        }

        const PluginInfo* info = Manager::Get()->GetPluginManager()->GetPluginInfo(plugin);
        if (!info)
            cbThrow(_T("No plugin info?!?"));

        static int row = 1;
        m_LayoutManager.AddPane(tb, wxAuiPaneInfo().
                              Name(info->name + _T("Toolbar")).Caption(info->title + _(" Toolbar")).
                              ToolbarPane().Top().Row(row++));
        DoUpdateLayout();
    }
    else
        delete tb;
}

void MainFrame::DoAddPlugin(cbPlugin* plugin)
{
    //Manager::Get()->GetLogManager()->DebugLog(_T("Adding plugin: %s"), plugin->GetInfo()->name.wx_str());
    AddPluginInSettingsMenu(plugin);
    AddPluginInHelpPluginsMenu(plugin);
    if (plugin->GetType() == ptTool)
    {
        AddPluginInPluginsMenu(plugin);
    }
    // offer menu and toolbar space for other plugins
    else
    {
        // menu
        try
        {
            wxMenuBar* mbar = GetMenuBar();
            plugin->BuildMenu(mbar);
        }
        catch (cbException& e)
        {
            e.ShowErrorMessage();
        }
        // toolbar
        DoAddPluginToolbar(plugin);
        DoAddPluginStatusField(plugin);
    }
}

bool MainFrame::Open(const wxString& filename, bool addToHistory)
{
    wxFileName fn(filename);
    fn.Normalize(); // really important so that two same files with different names are not loaded twice
    wxString name = fn.GetFullPath();
    //Manager::Get()->GetLogManager()->DebugLog(_T("Opening file '%s'"), sname.wx_str());
    Manager::Get()->GetLogManager()->DebugLog(_T("Opening file ") + name);
    bool ret = OpenGeneric(name, addToHistory);
    return ret;
}

wxString MainFrame::ShowOpenFileDialog(const wxString& caption, const wxString& filter)
{
    wxFileDialog dlg(this,
                     caption,
                     wxEmptyString,
                     wxEmptyString,
                     filter,
                     wxFD_OPEN | compatibility::wxHideReadonly);
    wxString sel;
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
        sel = dlg.GetPath();
    return sel;
}

bool MainFrame::OpenGeneric(const wxString& filename, bool addToHistory)
{
    if (filename.IsEmpty())
        return false;
    wxFileName fname(filename);
    fname.ClearExt();
    fname.SetExt(_T("cbp"));
    switch (FileTypeOf(filename))
    {
        //
        // Workspace
        //
        case ftCodeBlocksWorkspace:
            // verify that it's not the same as the one already open
            if (filename == Manager::Get()->GetProjectManager()->GetWorkspace()->GetFilename())
                return true;
            else
            {
                if (DoCloseCurrentWorkspace())
                {
                    wxBusyCursor wait; // loading a worspace can take some time -> showhourglass
                    ShowHideStartPage(true); // hide startherepage, so we can use full tab-range
                    bool ret = Manager::Get()->GetProjectManager()->LoadWorkspace(filename);
                    if (!ret)
                        ShowHideStartPage(); // show/hide startherepage, dependant of settings, if loading failed
                    else if (addToHistory)
                        AddToRecentProjectsHistory(Manager::Get()->GetProjectManager()->GetWorkspace()->GetFilename());
                    return ret;
                }
                else
                    return false;
            }
            break;

        //
        // Project
        //
        case ftCodeBlocksProject:
        {
            // Make a check whether the project exists in current workspace
            cbProject* prj = Manager::Get()->GetProjectManager()->IsOpen(fname.GetFullPath());
            if (!prj)
            {
                wxBusyCursor wait; // loading a worspace can take some time -> showhourglass
                return DoOpenProject(filename, addToHistory);
            }
            else
            {
                // NOTE (Morten#1#): A message here will prevent batch-builds from working and is shown sometimes even if correct
                Manager::Get()->GetProjectManager()->SetProject(prj, false);
                return true;
            }
        }
        //
        // Source files
        //
        case ftHeader:
            // fallthrough
        case ftSource:
            // fallthrough
        case ftResource:
            return DoOpenFile(filename, addToHistory);
        //
        // For all other files, ask MIME plugin for a suitable handler
        //
        default:
        {
            cbMimePlugin* plugin = Manager::Get()->GetPluginManager()->GetMIMEHandlerForFile(filename);
            // warn user that "Files extension handler" is disabled
            if (!plugin)
            {
                cbMessageBox(_("Could not open file ") + filename + _(",\nbecause no extension handler could be found."), _("Error"), wxICON_ERROR);
                return false;
            }
            if (plugin->OpenFile(filename) == 0)
            {
                AddToRecentFilesHistory(filename);
                return true;
            }
            return false;
        }
    }
    return true;
}

bool MainFrame::DoOpenProject(const wxString& filename, bool addToHistory)
{
//    Manager::Get()->GetLogManager()->DebugLog(_T("Opening project '%s'"), filename.wx_str());
    if (!wxFileExists(filename))
    {
        cbMessageBox(_("The project file does not exist..."), _("Error"), wxICON_ERROR);
        return false;
    }

    ShowHideStartPage(true); // hide startherepage, so we can use full tab-range
    cbProject* prj = Manager::Get()->GetProjectManager()->LoadProject(filename, true);
    if (prj)
    {
        if (addToHistory)
            AddToRecentProjectsHistory(prj->GetFilename());
        return true;
    }
    ShowHideStartPage(); // show/hide startherepage, dependant of settings, if loading failed
    return false;
}

bool MainFrame::DoOpenFile(const wxString& filename, bool addToHistory)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->Open(filename);
    if (ed)
    {
        // Cryogen 24/3/10 Activate the editor after opening. Partial fix for bug #14087.
        ed->Activate();
        if (addToHistory)
            AddToRecentFilesHistory(ed->GetFilename());
        return true;
    }
    return false;
}

bool MainFrame::DoCloseCurrentWorkspace()
{
    return Manager::Get()->GetProjectManager()->CloseWorkspace();
}

void MainFrame::DoCreateStatusBar()
{
#if wxUSE_STATUSBAR
    wxCoord width[16]; // 16 max

    wxClientDC dc(this);
    wxFont font = dc.GetFont();
    int h;
    int num = 0;

    width[num++] = -1; // main field
//    width[num++] = 128; // progress bar
    dc.GetTextExtent(_(" WINDOWS-1252 "),           &width[num++], &h);
    dc.GetTextExtent(_(" Line 12345, Column 123 "), &width[num++], &h);
    dc.GetTextExtent(_(" Overwrite "),              &width[num++], &h);
    dc.GetTextExtent(_(" Modified "),               &width[num++], &h);
    dc.GetTextExtent(_(" Read/Write....."),         &width[num++], &h);
    dc.GetTextExtent(_(" name_of_profile "),        &width[num++], &h);

    CreateStatusBar(num);
    SetStatusWidths(num, width);

    // here for later usage
//    m_pProgressBar = new wxGauge(GetStatusBar(), -1, 100);
#endif // wxUSE_STATUSBAR
}

void MainFrame::DoUpdateStatusBar()
{
#if wxUSE_STATUSBAR
    if (!GetStatusBar())
        return;
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    wxString personality(Manager::Get()->GetPersonalityManager()->GetPersonality());
    if (ed)
    {
        int panel = 0;
        int pos = ed->GetControl()->GetCurrentPos();
        wxString msg;
        SetStatusText(ed->GetFilename(), panel++);
        SetStatusText(ed->GetEncodingName(), panel++);
        msg.Printf(_("Line %d, Column %d"), ed->GetControl()->GetCurrentLine() + 1, ed->GetControl()->GetColumn(pos) + 1);
        SetStatusText(msg, panel++);
        SetStatusText(ed->GetControl()->GetOvertype() ? _("Overwrite") : _("Insert"), panel++);
        #if wxCHECK_VERSION(2, 9, 0)
        SetStatusText(ed->GetModified() ? _("Modified") : _T(""), panel++);
        #else
        SetStatusText(ed->GetModified() ? _("Modified") : wxEmptyString, panel++);
        #endif
        SetStatusText(ed->GetControl()->GetReadOnly() ? _("Read only") : _("Read/Write"), panel++);
        SetStatusText(personality, panel++);
    }
    else
    {
        int panel = 0;
        EditorBase *eb = Manager::Get()->GetEditorManager()->GetActiveEditor();
        if ( eb )
            SetStatusText(eb->GetFilename(), panel++);
        else
            SetStatusText(_("Welcome to ") + appglobals::AppName + _T("!"), panel++);
        SetStatusText(wxEmptyString, panel++);
        SetStatusText(wxEmptyString, panel++);
        SetStatusText(wxEmptyString, panel++);
        SetStatusText(wxEmptyString, panel++);
        SetStatusText(wxEmptyString, panel++);
        SetStatusText(personality, panel++);
    }
#endif // wxUSE_STATUSBAR
}

void MainFrame::DoUpdateEditorStyle(cbAuiNotebook* target, const wxString& prefix, long defaultStyle)
{
    if (!target)
        return;

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("app"));
    target->SetTabCtrlHeight(1);

    long nbstyle = cfg->ReadInt(_T("/environment/tabs_style"), 0);
    switch (nbstyle)
    {
        case 1: // simple style
            target->SetArtProvider(new wxAuiSimpleTabArt());
            break;

        case 2: // VC 7.1 style
            target->SetArtProvider(new NbStyleVC71());
            break;

        case 3: // Firefox 2 style
            target->SetArtProvider(new NbStyleFF2());
            break;

        default: // default style
#if defined(__WXGTK__) && (USE_GTK_NOTEBOOK) && !wxCHECK_VERSION(2, 9, 4)
            target->SetArtProvider(new NbStyleGTK());
#else
            target->SetArtProvider(new wxAuiDefaultTabArt());
#endif
            break;
    }

    target->SetTabCtrlHeight(-1);

    nbstyle = defaultStyle;
    if (cfg->ReadBool(_T("/environment/") + prefix + _T("_tabs_bottom")))
        nbstyle |= wxAUI_NB_BOTTOM;

    if (cfg->ReadBool(_T("/environment/tabs_list")))
        nbstyle |= wxAUI_NB_WINDOWLIST_BUTTON;

    target->SetWindowStyleFlag(nbstyle);
}

void MainFrame::DoUpdateEditorStyle()
{
    long style = wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS;
    long closestyle = Manager::Get()->GetConfigManager(_T("app"))->ReadInt(_T("/environment/tabs_closestyle"), 0);
    switch (closestyle)
    {
        case 1: // current tab
            style |= wxAUI_NB_CLOSE_ON_ACTIVE_TAB;
            break;

        case 2: // right side
            style |= wxAUI_NB_CLOSE_BUTTON;
            break;

        default: // all tabs (default)
            style |= wxAUI_NB_CLOSE_ON_ALL_TABS;
            break;
    }

    cbAuiNotebook* an = Manager::Get()->GetEditorManager()->GetNotebook();

    DoUpdateEditorStyle(an, _T("editor"), style | wxNO_FULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN);

    an = m_pInfoPane;
    DoUpdateEditorStyle(an, _T("infopane"), style);

    an = Manager::Get()->GetProjectManager()->GetNotebook();
    DoUpdateEditorStyle(an, _T("project"), wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_TAB_MOVE);
}

void MainFrame::DoUpdateLayoutColours()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("app"));
    wxAuiDockArt* art = m_LayoutManager.GetArtProvider();

#ifndef __WXGTK__
    m_LayoutManager.SetFlags(wxAUI_MGR_DEFAULT | wxAUI_MGR_ALLOW_ACTIVE_PANE | wxAUI_MGR_TRANSPARENT_DRAG);
#else // #ifndef __WXGTK__
    // workaround for a wxWidgets-bug that makes C::B crash when a floating window gets docked and composite-effects are enabled
    m_LayoutManager.SetFlags((wxAUI_MGR_DEFAULT | wxAUI_MGR_ALLOW_ACTIVE_PANE | wxAUI_MGR_TRANSPARENT_DRAG | wxAUI_MGR_VENETIAN_BLINDS_HINT)& ~wxAUI_MGR_TRANSPARENT_HINT);
#endif // #ifndef __WXGTK__

    art->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE,   cfg->ReadInt(_T("/environment/aui/border_size"), art->GetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE)));
    art->SetMetric(wxAUI_DOCKART_SASH_SIZE,          cfg->ReadInt(_T("/environment/aui/sash_size"), art->GetMetric(wxAUI_DOCKART_SASH_SIZE)));
    art->SetMetric(wxAUI_DOCKART_CAPTION_SIZE,       cfg->ReadInt(_T("/environment/aui/caption_size"), art->GetMetric(wxAUI_DOCKART_CAPTION_SIZE)));
    art->SetColour(wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR,              cfg->ReadColour(_T("/environment/aui/active_caption_colour"), art->GetColour(wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR)));
    art->SetColour(wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR,     cfg->ReadColour(_T("/environment/aui/active_caption_gradient_colour"), art->GetColour(wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR)));
    art->SetColour(wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR,         cfg->ReadColour(_T("/environment/aui/active_caption_text_colour"), art->GetColour(wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR)));
    art->SetColour(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR,            cfg->ReadColour(_T("/environment/aui/inactive_caption_colour"), art->GetColour(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR)));
    art->SetColour(wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR,   cfg->ReadColour(_T("/environment/aui/inactive_caption_gradient_colour"), art->GetColour(wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR)));
    art->SetColour(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR,       cfg->ReadColour(_T("/environment/aui/inactive_caption_text_colour"), art->GetColour(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR)));

    DoUpdateLayout();
}

void MainFrame::DoUpdateLayout()
{
    if (!m_StartupDone)
        return;

    DoFixToolbarsLayout();
    m_LayoutManager.Update();
}

void MainFrame::DoUpdateAppTitle()
{
    EditorBase* ed = Manager::Get()->GetEditorManager() ? Manager::Get()->GetEditorManager()->GetActiveEditor() : 0L;
    cbProject* prj = 0;
    if (ed && ed->IsBuiltinEditor())
    {
        ProjectFile* prjf = ((cbEditor*)ed)->GetProjectFile();
        if (prjf)
            prj = prjf->GetParentProject();
    }
    else
        prj = Manager::Get()->GetProjectManager() ? Manager::Get()->GetProjectManager()->GetActiveProject() : 0L;
    wxString projname;
    wxString edname;
    wxString fulltitle;
    if (ed || prj)
    {
        if (prj)
        {
            if (Manager::Get()->GetProjectManager()->GetActiveProject() == prj)
                projname = wxString(_T(" [")) + prj->GetTitle() + _T("]");
            else
                projname = wxString(_T(" (")) + prj->GetTitle() + _T(")");
        }
        if (ed)
            edname = ed->GetTitle();
        fulltitle = edname + projname;
        if (!fulltitle.IsEmpty())
            fulltitle.Append(_T(" - "));
    }
    fulltitle.Append(appglobals::AppName);
    fulltitle.Append(_T(" "));
    fulltitle.Append(appglobals::AppVersion);
    SetTitle(fulltitle);
}

void MainFrame::ShowHideStartPage(bool forceHasProject)
{
    if (Manager::IsBatchBuild())
        return;

    // we use the 'forceHasProject' param because when a project is opened
    // the EVT_PROJECT_OPEN event is fired *before* ProjectManager::GetProjects()
    // and ProjectManager::GetActiveProject() are updated...

    if (m_InitiatedShutdown)
    {
        EditorBase* sh = Manager::Get()->GetEditorManager()->GetEditor(g_StartHereTitle);
        if (sh)
            sh->Destroy();
        return;
    }

    bool show = !forceHasProject &&
                Manager::Get()->GetProjectManager()->GetProjects()->GetCount() == 0 &&
                Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/start_here_page"), true);

    EditorBase* sh = Manager::Get()->GetEditorManager()->GetEditor(g_StartHereTitle);
    if (show && !sh)
        sh = new StartHerePage(this, Manager::Get()->GetEditorManager()->GetNotebook());
    else if (!show && sh)
        sh->Destroy();

    DoUpdateAppTitle();
}

void MainFrame::ShowHideScriptConsole()
{
    if (Manager::IsBatchBuild())
        return;
    bool isVis = IsWindowReallyShown(m_pScriptConsole);
    CodeBlocksDockEvent evt(isVis ? cbEVT_HIDE_DOCK_WINDOW : cbEVT_SHOW_DOCK_WINDOW);
    evt.pWindow = m_pScriptConsole;
    Manager::Get()->ProcessEvent(evt);
}

void MainFrame::OnStartHereLink(wxCommandEvent& event)
{
    wxCommandEvent evt;
    evt.SetId(idFileNewProject);
    wxString link = event.GetString();
    if (link.IsSameAs(_T("CB_CMD_NEW_PROJECT")))
        OnFileNewWhat(evt);
    else if (link.IsSameAs(_T("CB_CMD_OPEN_PROJECT")))
        DoOnFileOpen(true);
//    else if (link.IsSameAs(_T("CB_CMD_CONF_ENVIRONMENT")))
//        OnSettingsEnvironment(evt);
//    else if (link.IsSameAs(_T("CB_CMD_CONF_EDITOR")))
//        Manager::Get()->GetEditorManager()->Configure();
//    else if (link.IsSameAs(_T("CB_CMD_CONF_COMPILER")))
//        OnSettingsCompilerDebugger(evt);
    else if (link.StartsWith(_T("CB_CMD_OPEN_HISTORY_")))
    {
        wxFileHistory* hist = link.StartsWith(_T("CB_CMD_OPEN_HISTORY_PROJECT_")) ? m_pProjectsHistory : m_pFilesHistory;
        unsigned long count;
        link.AfterLast(_T('_')).ToULong(&count);
        --count;
        if (count < hist->GetCount())
        {
            if (!OpenGeneric(hist->GetHistoryFile(count), true))
                AskToRemoveFileFromHistory(hist, count);
        }
    }
    else if (link.StartsWith(_T("CB_CMD_DELETE_HISTORY_")))
    {
        wxFileHistory* hist = link.StartsWith(_T("CB_CMD_DELETE_HISTORY_PROJECT_")) ? m_pProjectsHistory : m_pFilesHistory;
        unsigned long count;
        link.AfterLast(_T('_')).ToULong(&count);
        --count;
        if (count < hist->GetCount())
           AskToRemoveFileFromHistory(hist, count, false);
    }
}

void MainFrame::AskToRemoveFileFromHistory(wxFileHistory* hist, int id, bool cannot_open)
{
    wxString question(_("Do you want to remove it from the recent files list?"));
    wxString query(wxEmptyString);
    if (cannot_open)
    {
        query << _("The file cannot be opened (probably it's not available anymore).")
              << _T("\n") << question;
    }
    else
        query << question;

    AnnoyingDialog dialog(_("Question"), query, wxART_QUESTION);
    PlaceWindow(&dialog);
    if (dialog.ShowModal() == wxID_YES)
    {
        hist->RemoveFileFromHistory(id);
        // update start here page
        EditorBase* sh = Manager::Get()->GetEditorManager()->GetEditor(g_StartHereTitle);
        if (sh)
            ((StartHerePage*)sh)->Reload();
    }
}

void MainFrame::OnStartHereVarSubst(wxCommandEvent& event)
{
    EditorBase* sh = Manager::Get()->GetEditorManager()->GetEditor(g_StartHereTitle);
    if (!sh)
        return;

    // replace history vars

    wxString buf = event.GetString();
    wxString links;

    links << _T("<table>\n<tr><td colspan=\"2\"><b>");
    links << _("Recent projects");
    links << _T("</b></td></tr>\n");
    if (m_pProjectsHistory->GetCount())
    {
        for (size_t i = 0; i < m_pProjectsHistory->GetCount(); ++i)
        {
            links << _T("<tr><td width=\"50\"><img alt=\"\" width=\"20\" src=\"blank.png\" />");
            links << wxString::Format(_T("<a href=\"CB_CMD_DELETE_HISTORY_PROJECT_%lu\"><img alt=\"\" src=\"trash_16x16.png\" /></a>"),
                                      static_cast<unsigned long>(i + 1));
            links << _T("<img alt=\"\"  width=\"10\" src=\"blank.png\" /></td><td width=\"10\">");
            links << wxString::Format(_T("<a href=\"CB_CMD_OPEN_HISTORY_PROJECT_%lu\">%s</a>"),
                                      static_cast<unsigned long>(i + 1), m_pProjectsHistory->GetHistoryFile(i).wx_str());
            links << _T("</td></tr>\n");
        }
    }
    else
    {
        links << _T("<tr><td style=\"width:2em;\"></td><td>&nbsp;&nbsp;&nbsp;&nbsp;");
        links << _("No recent projects");
        links << _T("</td></tr>\n");
    }

    links << _T("</table>\n<table>\n<tr><td colspan=\"2\"><b>");
    links << _("Recent files");
    links <<_T("</b></td></tr>\n");
    if (m_pFilesHistory->GetCount())
    {
        for (size_t i = 0; i < m_pFilesHistory->GetCount(); ++i)
        {
            links << _T("<tr><td width=\"50\"><img alt=\"\" width=\"20\" src=\"blank.png\" />");
            links << wxString::Format(_T("<a href=\"CB_CMD_DELETE_HISTORY_FILE_%lu\"><img alt=\"\" src=\"trash_16x16.png\" /></a>"),
                                      static_cast<unsigned long>(i + 1));
            links << _T("<img alt=\"\"  width=\"10\" src=\"blank.png\" /></td><td width=\"10\">");
            links << wxString::Format(_T("<a href=\"CB_CMD_OPEN_HISTORY_FILE_%lu\">%s</a>"),
                                      static_cast<unsigned long>(i + 1), m_pFilesHistory->GetHistoryFile(i).wx_str());
            links << _T("</td></tr>\n");
        }
    }
    else
    {
        links << _T("<tr><td style=\"width:2em;\"></td><td>&nbsp;&nbsp;&nbsp;&nbsp;");
        links << _("No recent files");
        links << _T("</td></tr>\n");
    }

    links << _T("</table>\n");


    // update page
    buf.Replace(_T("CB_VAR_RECENT_FILES_AND_PROJECTS"), links);
    buf.Replace(_T("CB_TXT_NEW_PROJECT"), _("Create a new project"));
    buf.Replace(_T("CB_TXT_OPEN_PROJECT"), _("Open an existing project"));
    buf.Replace(_T("CB_TXT_VISIT_FORUMS"), _("Visit the Code::Blocks forums"));
    buf.Replace(_T("CB_TXT_REPORT_BUG"), _("Report a bug"));
    buf.Replace(_T("CB_TXT_REQ_NEW_FEATURE"), _("Request a new feature"));
    ((StartHerePage*)sh)->SetPageContent(buf);
}

void MainFrame::InitializeRecentFilesHistory()
{
    TerminateRecentFilesHistory();

    wxMenuBar* mbar = GetMenuBar();
    if (!mbar)
        return;
    int pos = mbar->FindMenu(_("&File"));
    if (pos != wxNOT_FOUND)
    {
        m_pFilesHistory = new wxFileHistory(16, wxID_CBFILE01);

        wxMenu* menu = mbar->GetMenu(pos);
        if (!menu)
            return;
        wxMenu* recentFiles = 0;
        menu->FindItem(idFileOpenRecentFileClearHistory, &recentFiles);
        if (recentFiles)
        {
            wxArrayString files = Manager::Get()->GetConfigManager(_T("app"))->ReadArrayString(_T("/recent_files"));
            for (int i = (int)files.GetCount() - 1; i >= 0; --i)
            {
                if (wxFileExists(files[i]))
                    m_pFilesHistory->AddFileToHistory(files[i]);
            }
            if (m_pFilesHistory->GetCount() > 0)
            {
                recentFiles->InsertSeparator(0);
                for (size_t i = 0; i < m_pFilesHistory->GetCount(); ++i)
                {
                    recentFiles->Insert(recentFiles->GetMenuItemCount() - 2, wxID_CBFILE01 + i,
                        wxString::Format(_T("&%lu "), static_cast<unsigned long>(i + 1)) + m_pFilesHistory->GetHistoryFile(i));
                }
            }
        }
        wxMenu* recentProjects = 0;
        menu->FindItem(idFileOpenRecentProjectClearHistory, &recentProjects);
        if (recentProjects)
        {
            m_pProjectsHistory = new wxFileHistory(16, wxID_CBFILE17);

            wxArrayString files = Manager::Get()->GetConfigManager(_T("app"))->ReadArrayString(_T("/recent_projects"));
            for (int i = (int)files.GetCount() - 1; i >= 0; --i)
            {
                if (wxFileExists(files[i]))
                    m_pProjectsHistory->AddFileToHistory(files[i]);
            }
            if (m_pProjectsHistory->GetCount() > 0)
            {
                recentProjects->InsertSeparator(0);
                for (size_t i = 0; i < m_pProjectsHistory->GetCount(); ++i)
                {
                    recentProjects->Insert(recentProjects->GetMenuItemCount() - 2, wxID_CBFILE17 + i,
                        wxString::Format(_T("&%lu "), static_cast<unsigned long>(i + 1)) + m_pProjectsHistory->GetHistoryFile(i));
                }
            }
        }
    }
}

void MainFrame::AddToRecentFilesHistory(const wxString& FileName)
{
    wxString filename = FileName;
#ifdef __WXMSW__
    // for windows, look for case-insensitive matches
    // if found, don't add it
    wxString low = filename.Lower();
    for (size_t i = 0; i < m_pFilesHistory->GetCount(); ++i)
    {
        if (low == m_pFilesHistory->GetHistoryFile(i).Lower())
        {    // it exists, set filename to the existing name, so it can become
            // the most recent one
            filename = m_pFilesHistory->GetHistoryFile(i);
            break;
        }
    }
#endif

    m_pFilesHistory->AddFileToHistory(filename);

    // because we append "clear history" menu to the end of the list,
    // each time we must add a history item we have to:
    // a) remove "Clear history" (Biplab#1: Don't remove or you'll loose icon)
    // b) clear the menu (Biplab#1: except the last item)
    // c) fill it with the history items (Biplab#1: by inserting them)
    // and d) append "Clear history"... (Biplab#1: Not needed, item has not been removed)
    wxMenuBar* mbar = GetMenuBar();
    if (!mbar)
        return;
    int pos = mbar->FindMenu(_("&File"));
    if (pos == wxNOT_FOUND)
        return;
    wxMenu* menu = mbar->GetMenu(pos);
    if (!menu)
        return;
    wxMenu* recentFiles = 0;
    menu->FindItem(idFileOpenRecentFileClearHistory, &recentFiles);
    if (recentFiles)
    {
        while (recentFiles->GetMenuItemCount() > 1)
            recentFiles->Delete(recentFiles->GetMenuItems()[0]);
        if (m_pFilesHistory->GetCount() > 0)
        {
            recentFiles->InsertSeparator(0);
            for (size_t i = 0; i < m_pFilesHistory->GetCount(); ++i)
            {
                recentFiles->Insert(recentFiles->GetMenuItemCount() - 2, wxID_CBFILE01 + i,
                    wxString::Format(_T("&%lu "), static_cast<unsigned long>(i + 1)) + m_pFilesHistory->GetHistoryFile(i));
            }
        }
    }

    // update start here page
    EditorBase* sh = Manager::Get()->GetEditorManager()->GetEditor(g_StartHereTitle);
    if (sh)
        ((StartHerePage*)sh)->Reload();
}

void MainFrame::AddToRecentProjectsHistory(const wxString& FileName)
{
    wxString filename = FileName;
#ifdef __WXMSW__
    // for windows, look for case-insensitive matches
    // if found, don't add it
    wxString low = filename.Lower();
    for (size_t i = 0; i < m_pProjectsHistory->GetCount(); ++i)
    {
        if (low == m_pProjectsHistory->GetHistoryFile(i).Lower())
        {    // it exists, set filename to the existing name, so it can become
            // the most recent one
            filename = m_pProjectsHistory->GetHistoryFile(i);
            break;
        }
    }
#endif

    m_pProjectsHistory->AddFileToHistory(filename);

    // because we append "clear history" menu to the end of the list,
    // each time we must add a history item we have to:
    // a) remove "Clear history"
    // b) clear the menu
    // c) fill it with the history items
    // and d) append "Clear history"...
    wxMenuBar* mbar = GetMenuBar();
    if (!mbar)
        return;
    int pos = mbar->FindMenu(_("&File"));
    if (pos == wxNOT_FOUND)
        return;
    wxMenu* menu = mbar->GetMenu(pos);
    if (!menu)
        return;
    wxMenu* recentProjects = 0;
    menu->FindItem(idFileOpenRecentProjectClearHistory, &recentProjects);
    if (recentProjects)
    {
        while (recentProjects->GetMenuItemCount() > 1)
            recentProjects->Delete(recentProjects->GetMenuItems()[0]);
        if (m_pProjectsHistory->GetCount() > 0)
        {
            recentProjects->InsertSeparator(0);
            for (size_t i = 0; i < m_pProjectsHistory->GetCount(); ++i)
            {
                recentProjects->Insert(recentProjects->GetMenuItemCount() - 2, wxID_CBFILE17 + i,
                    wxString::Format(_T("&%lu "), static_cast<unsigned long>(i + 1)) + m_pProjectsHistory->GetHistoryFile(i));
            }
        }
    }

    // update start here page
    EditorBase* sh = Manager::Get()->GetEditorManager()->GetEditor(g_StartHereTitle);
    if (sh)
        ((StartHerePage*)sh)->Reload();
}

void MainFrame::TerminateRecentFilesHistory()
{
    if (m_pFilesHistory)
    {
        wxArrayString files;
        for (unsigned int i = 0; i < m_pFilesHistory->GetCount(); ++i)
            files.Add(m_pFilesHistory->GetHistoryFile(i));
        Manager::Get()->GetConfigManager(_T("app"))->Write(_T("/recent_files"), files);

        wxMenuBar* mbar = GetMenuBar();
        if (mbar)
        {
            int pos = mbar->FindMenu(_("&File"));
            if (pos != wxNOT_FOUND)
            {
                wxMenu* menu = mbar->GetMenu(pos);
                if (menu)
                {
                    wxMenu* recentFiles = 0;
                    menu->FindItem(idFileOpenRecentFileClearHistory, &recentFiles);
                    if (recentFiles)
                    {
                        if (!Manager::IsAppShuttingDown())
                        {
                            while (recentFiles->GetMenuItemCount() > 1)
                                recentFiles->Delete(recentFiles->GetMenuItems()[0]);
                        }
                        else
                            m_pFilesHistory->RemoveMenu(recentFiles);
                    }
                }
            }
        }
        delete m_pFilesHistory;
        m_pFilesHistory = 0;
    }

    if (m_pProjectsHistory)
    {
        wxArrayString files;
        for (unsigned int i = 0; i < m_pProjectsHistory->GetCount(); ++i)
            files.Add(m_pProjectsHistory->GetHistoryFile(i));
        Manager::Get()->GetConfigManager(_T("app"))->Write(_T("/recent_projects"), files);

        wxMenuBar* mbar = GetMenuBar();
        if (mbar)
        {
            int pos = mbar->FindMenu(_("&File"));
            if (pos != wxNOT_FOUND)
            {
                wxMenu* menu = mbar->GetMenu(pos);
                if (menu)
                {
                    wxMenu* recentProjects = 0;
                    menu->FindItem(idFileOpenRecentProjectClearHistory, &recentProjects);
                    if (recentProjects)
                    {
                        if (!Manager::IsAppShuttingDown())
                        {
                            while (recentProjects->GetMenuItemCount() > 1)
                                recentProjects->Delete(recentProjects->GetMenuItems()[0]);
                        }
                        else
                            m_pProjectsHistory->RemoveMenu(recentProjects);
                    }
                }
            }
        }
        delete m_pProjectsHistory;
        m_pProjectsHistory = 0;
    }
}

wxString MainFrame::GetEditorDescription(EditorBase* eb)
{
    wxString descr = wxEmptyString;
    cbProject* prj = NULL;
    if(eb && eb->IsBuiltinEditor())
    {
        ProjectFile* prjf = ((cbEditor*)eb)->GetProjectFile();
        if(prjf)
            prj = prjf->GetParentProject();
    }
    else
        prj = Manager::Get()->GetProjectManager() ? Manager::Get()->GetProjectManager()->GetActiveProject() : 0L;
    if(prj)
    {
        descr = wxString(_("Project: ")) + _T("<b>") + prj->GetTitle() + _T("</b>");
        if(Manager::Get()->GetProjectManager()->GetActiveProject() == prj)
            descr += wxString(_(" (Active)"));
        descr += wxString(_T("<br>"));
    }
    if(eb)
        descr += eb->GetFilename();
    return descr;
}

////////////////////////////////////////////////////////////////////////////////
// event handlers
////////////////////////////////////////////////////////////////////////////////

void MainFrame::OnPluginsExecuteMenu(wxCommandEvent& event)
{
    wxString pluginName = m_PluginIDsMap[event.GetId()];
    if (!pluginName.IsEmpty())
        Manager::Get()->GetPluginManager()->ExecutePlugin(pluginName);
    else
        Manager::Get()->GetLogManager()->DebugLog(F(_T("No plugin found for ID %d"), event.GetId()));
}

void MainFrame::OnPluginSettingsMenu(wxCommandEvent& event)
{
    wxString pluginName = m_PluginIDsMap[event.GetId()];
    if (!pluginName.IsEmpty())
        Manager::Get()->GetPluginManager()->ConfigurePlugin(pluginName);
    else
        Manager::Get()->GetLogManager()->DebugLog(F(_T("No plugin found for ID %d"), event.GetId()));
}

void MainFrame::OnHelpPluginMenu(wxCommandEvent& event)
{
    wxString pluginName = m_PluginIDsMap[event.GetId()];
    if (!pluginName.IsEmpty())
    {
        const PluginInfo* pi = Manager::Get()->GetPluginManager()->GetPluginInfo(pluginName);
        if (!pi)
        {
            Manager::Get()->GetLogManager()->DebugLog(_T("No plugin info for ") + pluginName);
            return;
        }
        dlgAboutPlugin dlg(this, pi);
        PlaceWindow(&dlg);
        dlg.ShowModal();
    }
    else
        Manager::Get()->GetLogManager()->DebugLog(F(_T("No plugin found for ID %d"), event.GetId()));
}

void MainFrame::OnFileNewWhat(wxCommandEvent& event)
{
    int id = event.GetId();
    if (id != idFileNewEmpty)
    {
        // wizard-based

        TemplateOutputType tot = totProject;
        if (id == idFileNewProject)     tot = totProject;
        else if (id == idFileNewTarget) tot = totTarget;
        else if (id == idFileNewFile)   tot = totFiles;
        else if (id == idFileNewCustom) tot = totCustom;
        else if (id == idFileNewUser)   tot = totUser;
        else
            return;

        wxString filename;
        cbProject* prj = TemplateManager::Get()->New(tot, &filename);
        // verify that the open files are still in sync
        // the new file might have overwritten an existing one)
        Manager::Get()->GetEditorManager()->CheckForExternallyModifiedFiles();

        // If both are empty it means that the wizard has failed
        if (!prj && filename.IsEmpty())
            return;

        // Send the new project event
        CodeBlocksEvent evtNew(cbEVT_PROJECT_NEW, 0, prj);
        Manager::Get()->GetPluginManager()->NotifyPlugins(evtNew);

        if (prj)
        {
            prj->Save();
            prj->SaveAllFiles();
        }

        if (!filename.IsEmpty())
        {
            if (prj)
                AddToRecentProjectsHistory(filename);
            else
                AddToRecentFilesHistory(filename);
        }
        if (prj && tot == totProject) // Created project should be parsed
        {
            CodeBlocksEvent evtOpen(cbEVT_PROJECT_OPEN, 0, prj);
            Manager::Get()->GetPluginManager()->NotifyPlugins(evtOpen);
        }
        return;
    }

    // new empty file quick shortcut code below

    cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
    if (project)
        wxSetWorkingDirectory(project->GetBasePath());
    cbEditor* ed = Manager::Get()->GetEditorManager()->New();

    // initially start change-collection if configured on empty files
    if (ed)
        ed->GetControl()->SetChangeCollection(Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/margin/use_changebar"), true));

    if (ed && ed->IsOK())
        AddToRecentFilesHistory(ed->GetFilename());

    if (!ed || !project)
        return;

    wxString oldname = ed->GetFilename();
    if (cbMessageBox(_("Do you want to add this new file in the active project (has to be saved first)?"),
                    _("Add file to project"),
                    wxYES_NO | wxICON_QUESTION) == wxID_YES &&
        ed->SaveAs() && ed->IsOK())
    {
        wxArrayInt targets;
        if (Manager::Get()->GetProjectManager()->AddFileToProject(ed->GetFilename(), project, targets) != 0)
        {
            ProjectFile* pf = project->GetFileByFilename(ed->GetFilename(), false);
            ed->SetProjectFile(pf);
            Manager::Get()->GetProjectManager()->RebuildTree();
        }
    }
    // verify that the open files are still in sync
    // the new file might have overwritten an existing one)
    Manager::Get()->GetEditorManager()->CheckForExternallyModifiedFiles();
}

bool MainFrame::OnDropFiles(wxCoord /*x*/, wxCoord /*y*/, const wxArrayString& files)
{
    bool success = true; // Safe case initialisation

    // first check to see if a workspace is passed. If so, only this will be loaded
    wxString foundWorkspace;
    for (unsigned int i = 0; i < files.GetCount(); ++i)
    {
        FileType ft = FileTypeOf(files[i]);
        if (ft == ftCodeBlocksWorkspace || ft == ftMSVC6Workspace || ft == ftMSVC7Workspace)
        {
            foundWorkspace = files[i];
            break;
        }
    }

    if (!foundWorkspace.IsEmpty())
      success &= OpenGeneric(foundWorkspace);
    else
    {
        wxBusyCursor useless;
        wxPaintEvent e;
        ProcessEvent(e);
#if !wxCHECK_VERSION(2, 8, 11)
        Freeze();
#endif
        for (unsigned int i = 0; i < files.GetCount(); ++i)
          success &= OpenGeneric(files[i]);
#if !wxCHECK_VERSION(2, 8, 11)
        Thaw();
#endif
    }
    return success;
}

void MainFrame::OnFileNew(wxCommandEvent& /*event*/)
{
    wxMenu* popup = 0;
    wxMenuBar* bar = GetMenuBar();
    if (!bar)
        return;

    bar->FindItem(idFileNewProject, &popup);
    if (popup)
        PopupMenu(popup); // this will lead us in OnFileNewWhat() - the meat is there ;)
}

// in case we are opening a project (bProject == true) we do not want to interfere
// with 'last type of files' (probably the call was open (existing) project on the
// start here page --> so we know it's a project --> set the filter accordingly
// but as said don't force the 'last used type of files' to change, that should
// only change when an open file is carried out (so (source) file <---> project (file) )
// TODO : when regular file open and user manually sets filter to project files --> will change
//      the last type : is that expected behaviour ???
void MainFrame::DoOnFileOpen(bool bProject)
{
    wxString Filters = FileFilters::GetFilterString();
    // the value returned by GetIndexForFilterAll() is updated by GetFilterString()
    int StoredIndex = FileFilters::GetIndexForFilterAll();
    wxString Path;
    ConfigManager* mgr = Manager::Get()->GetConfigManager(_T("app"));
    if (mgr)
    {
        if (!bProject)
        {
            wxString Filter = mgr->Read(_T("/file_dialogs/file_new_open/filter"));
            if (!Filter.IsEmpty())
                FileFilters::GetFilterIndexFromName(Filters, Filter, StoredIndex);
            Path = mgr->Read(_T("/file_dialogs/file_new_open/directory"), Path);
        }
        else
            FileFilters::GetFilterIndexFromName(Filters, _("Code::Blocks project/workspace files"), StoredIndex);
    }
    wxFileDialog dlg(this,
                            _("Open file"),
                            Path,
                            wxEmptyString,
                            Filters,
                            wxFD_OPEN | wxFD_MULTIPLE | compatibility::wxHideReadonly);
    dlg.SetFilterIndex(StoredIndex);

    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        // store the last used filter and directory
        // as said : don't do this in case of an 'open project'
        if (mgr && !bProject)
        {
            int Index = dlg.GetFilterIndex();
            wxString Filter;
            if (FileFilters::GetFilterNameFromIndex(Filters, Index, Filter))
                mgr->Write(_T("/file_dialogs/file_new_open/filter"), Filter);
            wxString Test = dlg.GetDirectory();
            mgr->Write(_T("/file_dialogs/file_new_open/directory"), dlg.GetDirectory());
        }
        wxArrayString files;
        dlg.GetPaths(files);
        OnDropFiles(0,0,files);
    }
}

void MainFrame::OnFileOpen(wxCommandEvent& /*event*/)
{
    DoOnFileOpen(false); // through file menu (not sure if we are opening a project)
}

void MainFrame::OnFileReopenProject(wxCommandEvent& event)
{
    size_t id = event.GetId() - wxID_CBFILE17;
    wxString fname = m_pProjectsHistory->GetHistoryFile(id);
    if (!OpenGeneric(fname, true))
        AskToRemoveFileFromHistory(m_pProjectsHistory, id);
}

void MainFrame::OnFileOpenRecentProjectClearHistory(wxCommandEvent& /*event*/)
{
    while (m_pProjectsHistory->GetCount())
        m_pProjectsHistory->RemoveFileFromHistory(0);
    Manager::Get()->GetConfigManager(_T("app"))->DeleteSubPath(_T("/recent_projects"));

    // update start here page
    InitializeRecentFilesHistory();
    EditorBase* sh = Manager::Get()->GetEditorManager()->GetEditor(g_StartHereTitle);
    if (sh)
        ((StartHerePage*)sh)->Reload();
}

void MainFrame::OnFileReopen(wxCommandEvent& event)
{
    size_t id = event.GetId() - wxID_CBFILE01;
    wxString fname = m_pFilesHistory->GetHistoryFile(id);
    if (!OpenGeneric(fname, true))
        AskToRemoveFileFromHistory(m_pFilesHistory, id);
}

void MainFrame::OnFileOpenRecentClearHistory(wxCommandEvent& /*event*/)
{
    while (m_pFilesHistory->GetCount())
        m_pFilesHistory->RemoveFileFromHistory(0);
    Manager::Get()->GetConfigManager(_T("app"))->DeleteSubPath(_T("/recent_files"));

    // update start here page
    InitializeRecentFilesHistory();
    EditorBase* sh = Manager::Get()->GetEditorManager()->GetEditor(g_StartHereTitle);
    if (sh)
        ((StartHerePage*)sh)->Reload();
}

void MainFrame::OnFileSave(wxCommandEvent& /*event*/)
{
    if (!Manager::Get()->GetEditorManager()->SaveActive())
    {
        wxString msg;
        msg.Printf(_("File %s could not be saved..."), Manager::Get()->GetEditorManager()->GetActiveEditor()->GetFilename().wx_str());
        cbMessageBox(msg, _("Error saving file"), wxICON_ERROR);
    }
    DoUpdateStatusBar();
}

void MainFrame::OnFileSaveAs(wxCommandEvent& /*event*/)
{
    Manager::Get()->GetEditorManager()->SaveActiveAs();
    DoUpdateStatusBar();
}

void MainFrame::OnFileSaveAllFiles(wxCommandEvent& /*event*/)
{
    Manager::Get()->GetEditorManager()->SaveAll();
    DoUpdateStatusBar();
}

void MainFrame::OnFileSaveProject(wxCommandEvent& /*event*/)
{
    // no need to call SaveActiveProjectAs(), because this is handled in cbProject::Save()
    if (Manager::Get()->GetProjectManager()->SaveActiveProject())
        AddToRecentProjectsHistory(Manager::Get()->GetProjectManager()->GetActiveProject()->GetFilename());
    DoUpdateStatusBar();
    DoUpdateAppTitle();
}

void MainFrame::OnFileSaveProjectAs(wxCommandEvent& /*event*/)
{
    if (Manager::Get()->GetProjectManager()->SaveActiveProjectAs())
        AddToRecentProjectsHistory(Manager::Get()->GetProjectManager()->GetActiveProject()->GetFilename());
    DoUpdateStatusBar();
    DoUpdateAppTitle();
}

void MainFrame::OnFileSaveProjectAllProjects(wxCommandEvent& /*event*/)
{
    Manager::Get()->GetProjectManager()->SaveAllProjects();
    DoUpdateStatusBar();
    DoUpdateAppTitle();
}

void MainFrame::OnFileSaveAll(wxCommandEvent& /*event*/)
{
    Manager::Get()->GetConfigManager(_T("app"))->Flush();
    Manager::Get()->GetEditorManager()->SaveAll();
    Manager::Get()->GetProjectManager()->SaveAllProjects();

    if (Manager::Get()->GetProjectManager()->GetWorkspace()->GetModified()
        && !Manager::Get()->GetProjectManager()->GetWorkspace()->IsDefault()
        && Manager::Get()->GetProjectManager()->SaveWorkspace())
    {
        AddToRecentProjectsHistory(Manager::Get()->GetProjectManager()->GetWorkspace()->GetFilename());
    }
    DoUpdateStatusBar();
    DoUpdateAppTitle();
}

void MainFrame::OnFileSaveProjectTemplate(wxCommandEvent& /*event*/)
{
    TemplateManager::Get()->SaveUserTemplate(Manager::Get()->GetProjectManager()->GetActiveProject());
}

void MainFrame::OnFileCloseProject(wxCommandEvent& /*event*/)
{
    // we 're not actually shutting down here, but we want to check if the
    // active project is still opening files (still busy)
    if (!ProjectManager::CanShutdown() || !EditorManager::CanShutdown())
    {
        wxBell();
        return;
    }
    Manager::Get()->GetProjectManager()->CloseActiveProject();
    DoUpdateStatusBar();
}

void MainFrame::OnFileCloseAllProjects(wxCommandEvent& /*event*/)
{
    if (!ProjectManager::CanShutdown() || !EditorManager::CanShutdown())
    {
        wxBell();
        return;
    }
    DoCloseCurrentWorkspace();
    DoUpdateStatusBar();
}

void MainFrame::OnFileImportProjectDevCpp(wxCommandEvent& /*event*/)
{
    OpenGeneric(ShowOpenFileDialog(_("Import Dev-C++ project"), FileFilters::GetFilterString(_T('.') + FileFilters::DEVCPP_EXT)), false);
}

void MainFrame::OnFileImportProjectMSVC(wxCommandEvent& /*event*/)
{
    OpenGeneric(ShowOpenFileDialog(_("Import MS Visual C++ 6.0 project"), FileFilters::GetFilterString(_T('.') + FileFilters::MSVC6_EXT)), false);
}

void MainFrame::OnFileImportProjectMSVCWksp(wxCommandEvent& /*event*/)
{
    OpenGeneric(ShowOpenFileDialog(_("Import MS Visual C++ 6.0 workspace"), FileFilters::GetFilterString(_T('.') + FileFilters::MSVC6_WORKSPACE_EXT)), false);
}

void MainFrame::OnFileImportProjectMSVS(wxCommandEvent& /*event*/)
{
    OpenGeneric(ShowOpenFileDialog(_("Import MS Visual Studio 7.0+ project"), FileFilters::GetFilterString(_T('.') + FileFilters::MSVC7_EXT)), false);
}

void MainFrame::OnFileImportProjectMSVSWksp(wxCommandEvent& /*event*/)
{
    OpenGeneric(ShowOpenFileDialog(_("Import MS Visual Studio 7.0+ solution"), FileFilters::GetFilterString(_T('.') + FileFilters::MSVC7_WORKSPACE_EXT)), false);
}

void MainFrame::OnFileOpenDefWorkspace(wxCommandEvent& /*event*/)
{
    ProjectManager *pman = Manager::Get()->GetProjectManager();
    if (!pman->GetWorkspace()->IsDefault() && !pman->LoadWorkspace())
    {
        // do not add the default workspace in recent projects list
        // it's always one menu click away
        cbMessageBox(_("Can't open default workspace (file exists?)"), _("Warning"), wxICON_WARNING);
    }
}

void MainFrame::OnFileSaveWorkspace(wxCommandEvent& /*event*/)
{
    if (Manager::Get()->GetProjectManager()->SaveWorkspace())
        AddToRecentProjectsHistory(Manager::Get()->GetProjectManager()->GetWorkspace()->GetFilename());
}

void MainFrame::OnFileSaveWorkspaceAs(wxCommandEvent& /*event*/)
{
    if (Manager::Get()->GetProjectManager()->SaveWorkspaceAs(_T("")))
        AddToRecentProjectsHistory(Manager::Get()->GetProjectManager()->GetWorkspace()->GetFilename());
}

void MainFrame::OnFileCloseWorkspace(wxCommandEvent& /*event*/)
{
    DoCloseCurrentWorkspace();
}

void MainFrame::OnFileClose(wxCommandEvent& WXUNUSED(event))
{
    Manager::Get()->GetEditorManager()->CloseActive();
    DoUpdateStatusBar();
    Refresh();
}

void MainFrame::OnFileCloseAll(wxCommandEvent& WXUNUSED(event))
{
    Manager::Get()->GetEditorManager()->CloseAll();
    DoUpdateStatusBar();
}

void MainFrame::OnFileNext(wxCommandEvent& /*event*/)
{
    Manager::Get()->GetEditorManager()->ActivateNext();
    DoUpdateStatusBar();
}

void MainFrame::OnFilePrev(wxCommandEvent& /*event*/)
{
    Manager::Get()->GetEditorManager()->ActivatePrevious();
    DoUpdateStatusBar();
}

void MainFrame::OnFilePrint(wxCommandEvent& /*event*/)
{
    PrintDialog dlg(this);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
        Manager::Get()->GetEditorManager()->Print(dlg.GetPrintScope(), dlg.GetPrintColourMode(), dlg.GetPrintLineNumbers());
}

void MainFrame::OnFileQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MainFrame::OnEraseBackground(wxEraseEvent& event)
{
    // for flicker-free display
    event.Skip();
}

void MainFrame::OnSize(wxSizeEvent& event)
{

    // for flicker-free display
    event.Skip();
}

void MainFrame::OnApplicationClose(wxCloseEvent& event)
{
    if (m_InitiatedShutdown)
        return;

    CodeBlocksEvent evt(cbEVT_APP_START_SHUTDOWN);
    Manager::Get()->ProcessEvent(evt);
    Manager::Yield();

    m_InitiatedShutdown = true;
    Manager::BlockYields(true);

    ProjectManager* prjman = Manager::Get()->GetProjectManager();
    if (prjman)
    {
        cbProject* prj = prjman->GetActiveProject();
        if (prj && prj->GetCurrentlyCompilingTarget())
        {
            event.Veto();
            wxBell();
            m_InitiatedShutdown = false;
            Manager::BlockYields(false);
            return;
        }
    }
    if (!ProjectManager::CanShutdown() || !EditorManager::CanShutdown())
    {
        event.Veto();
        wxBell();
        m_InitiatedShutdown = false;
        Manager::BlockYields(false);
        return;
    }

    if (!DoCloseCurrentWorkspace())
    {
        event.Veto();
        m_InitiatedShutdown = false;
        Manager::BlockYields(false);
        return;
    }

    Manager::Get()->GetLogManager()->DebugLog(_T("Deinitializing plugins..."));
    CodeBlocksEvent evtShutdown(cbEVT_APP_START_SHUTDOWN);
    Manager::Get()->ProcessEvent(evtShutdown);

    if (!Manager::IsBatchBuild())
        SaveWindowState();

    m_LayoutManager.DetachPane(Manager::Get()->GetProjectManager()->GetNotebook());
    m_LayoutManager.DetachPane(m_pInfoPane);
    m_LayoutManager.DetachPane(Manager::Get()->GetEditorManager()->GetNotebook());

    m_LayoutManager.UnInit();
    TerminateRecentFilesHistory();

    // remove all other event handlers from this window
    // this stops it from crashing, when no plugins are loaded
    while (GetEventHandler() != this)
        PopEventHandler(false);

    // Hide the window
    Hide();

    if (!Manager::IsBatchBuild())
    {
        m_pInfoPane->Destroy();
        m_pInfoPane = 0L;
    }

    Manager::Shutdown(); // Shutdown() is not Free(), Manager is automatically destroyed at exit

    Destroy();
}

void MainFrame::OnEditSwapHeaderSource(wxCommandEvent& /*event*/)
{
    Manager::Get()->GetEditorManager()->SwapActiveHeaderSource();
    DoUpdateStatusBar();
}

void MainFrame::OnEditGotoMatchingBrace(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
        ed->GotoMatchingBrace();
}

void MainFrame::OnEditBookmarksToggle(wxCommandEvent& /*event*/)
{
    EditorBase* ed = Manager::Get()->GetEditorManager()->GetActiveEditor();
    if (ed)
        ed->ToggleBookmark();
}

void MainFrame::OnEditBookmarksNext(wxCommandEvent& /*event*/)
{
    EditorBase* ed = Manager::Get()->GetEditorManager()->GetActiveEditor();
    if (ed)
        ed->GotoNextBookmark();
}

void MainFrame::OnEditBookmarksPrevious(wxCommandEvent& /*event*/)
{
    EditorBase* ed = Manager::Get()->GetEditorManager()->GetActiveEditor();
    if (ed)
        ed->GotoPreviousBookmark();
}

void MainFrame::OnEditUndo(wxCommandEvent& /*event*/)
{
    EditorBase* ed = Manager::Get()->GetEditorManager()->GetActiveEditor();
    if (ed)
        ed->Undo();
}

void MainFrame::OnEditRedo(wxCommandEvent& /*event*/)
{
    EditorBase* ed = Manager::Get()->GetEditorManager()->GetActiveEditor();
    if (ed)
        ed->Redo();
}

void MainFrame::OnEditClearHistory(wxCommandEvent& /*event*/)
{
    EditorBase* ed = Manager::Get()->GetEditorManager()->GetActiveEditor();
    if (ed)
        ed->ClearHistory();
}

void MainFrame::OnEditCopy(wxCommandEvent& /*event*/)
{
    EditorBase* ed = Manager::Get()->GetEditorManager()->GetActiveEditor();
    if (ed)
        ed->Copy();
}

void MainFrame::OnEditCut(wxCommandEvent& /*event*/)
{
    EditorBase* ed = Manager::Get()->GetEditorManager()->GetActiveEditor();
    if (ed)
        ed->Cut();
}

void MainFrame::OnEditPaste(wxCommandEvent& /*event*/)
{
    EditorBase* ed = Manager::Get()->GetEditorManager()->GetActiveEditor();
    if (ed)
        ed->Paste();
}

void MainFrame::OnEditParaUp(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
        ed->GetControl()->ParaUp();
}

void MainFrame::OnEditParaUpExtend(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
        ed->GetControl()->ParaUpExtend();
}

void MainFrame::OnEditParaDown(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
        ed->GetControl()->ParaDown();
}

void MainFrame::OnEditParaDownExtend(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
        ed->GetControl()->ParaDownExtend();
}

void MainFrame::OnEditWordPartLeft(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
        ed->GetControl()->WordPartLeft();
}

void MainFrame::OnEditWordPartLeftExtend(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
        ed->GetControl()->WordPartLeftExtend();
}

void MainFrame::OnEditWordPartRight(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
        ed->GetControl()->WordPartRight();
}

void MainFrame::OnEditWordPartRightExtend(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
        ed->GetControl()->WordPartRightExtend();
}

void MainFrame::OnEditZoomIn(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
        ed->GetControl()->ZoomIn();
}

void MainFrame::OnEditZoomOut(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
        ed->GetControl()->ZoomOut();
}

void MainFrame::OnEditZoomReset(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
        ed->GetControl()->SetZoom(0);
}

void MainFrame::OnEditLineCut(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
        ed->GetControl()->LineCut();
}

void MainFrame::OnEditLineDelete(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
        ed->GetControl()->LineDelete();
}

void MainFrame::OnEditLineDuplicate(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
        ed->GetControl()->LineDuplicate();
}

void MainFrame::OnEditLineTranspose(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
        ed->GetControl()->LineTranspose();
}

void MainFrame::OnEditLineCopy(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
        ed->GetControl()->LineCopy();
}

void MainFrame::OnEditLinePaste(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
    {
        //We want to undo all in one step
        ed->GetControl()->BeginUndoAction();

        int pos = ed->GetControl()->GetCurrentPos();
        int line = ed->GetControl()->LineFromPosition(pos);
        ed->GetControl()->GotoLine(line);
        int column = pos - ed->GetControl()->GetCurrentPos();
        ed->GetControl()->Paste();
        pos = ed->GetControl()->GetCurrentPos();
        ed->GetControl()->GotoPos(pos+column);

        ed->GetControl()->EndUndoAction();
    }
}

void MainFrame::OnEditUpperCase(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
        ed->GetControl()->UpperCase();
}

void MainFrame::OnEditLowerCase(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
        ed->GetControl()->LowerCase();
}

void MainFrame::OnEditInsertNewLine(wxCommandEvent& event)
{
    OnEditGotoLineEnd(event);
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
        ed->GetControl()->NewLine();
}

void MainFrame::OnEditGotoLineEnd(wxCommandEvent& event)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
    {
        cbStyledTextCtrl* control = ed->GetControl();
        const int pos = control->GetLineEndPosition(control->GetCurrentLine());
        control->GotoPos(pos);
    }
}

void MainFrame::OnEditSelectAll(wxCommandEvent& /*event*/)
{
    EditorBase* eb = Manager::Get()->GetEditorManager()->GetActiveEditor();
    if (eb)
        eb->SelectAll();
}

/* This is a shameless rip-off of the original OnEditCommentSelected function,
 * now more suitingly named OnEditToggleCommentSelected (because that's what
 * it does :)
 */
void MainFrame::OnEditCommentSelected(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
    {
        cbStyledTextCtrl* stc = ed->GetControl();
        CommentToken comment =
            Manager::Get()->GetEditorManager()->GetColourSet()->GetCommentToken( ed->GetLanguage() );
        if (comment.lineComment==wxEmptyString && comment.streamCommentStart==wxEmptyString)
            return;

        stc->BeginUndoAction();
        if ( wxSCI_INVALID_POSITION != stc->GetSelectionStart() )
        {
            int startLine = stc->LineFromPosition( stc->GetSelectionStart() );
            int endLine   = stc->LineFromPosition( stc->GetSelectionEnd() );
            int curLine=startLine;
            /**
                Fix a glitch: when selecting multiple lines and the caret
                is at the start of the line after the last line selected,
                the code would, wrongly, (un)comment that line too.
                This fixes it.
            */
            if (startLine != endLine && // selection is more than one line
                stc->GetColumn( stc->GetSelectionEnd() ) == 0) // and the caret is at the start of the line
            {
                // don't take into account the line the caret is on,
                // because it contains no selection (caret_column == 0)...
                --endLine;
            }

            while( curLine <= endLine )
            {
                // For each line: comment.
                if (comment.lineComment!=wxEmptyString)
                    stc->InsertText( stc->PositionFromLine( curLine ), comment.lineComment );
                else // if the language doesn't support line comments use stream comments
                {
                    stc->InsertText( stc->PositionFromLine( curLine ), comment.streamCommentStart );
                    stc->InsertText( stc->GetLineEndPosition( curLine ), comment.streamCommentEnd );
                }
                ++curLine;
            } // end while
            stc->SetSelectionVoid(stc->PositionFromLine(startLine),stc->PositionFromLine(endLine)+stc->LineLength(endLine));
        }
        stc->EndUndoAction();
    }
}

/* See above (OnEditCommentSelected) for details. */
void MainFrame::OnEditUncommentSelected(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
    {
        cbStyledTextCtrl* stc = ed->GetControl();
        CommentToken comment =
            Manager::Get()->GetEditorManager()->GetColourSet()->GetCommentToken( ed->GetLanguage() );

        if (comment.lineComment==wxEmptyString && comment.streamCommentStart==wxEmptyString)
            return;

        stc->BeginUndoAction();
        if ( wxSCI_INVALID_POSITION != stc->GetSelectionStart() )
        {
            int startLine = stc->LineFromPosition( stc->GetSelectionStart() );
            int endLine   = stc->LineFromPosition( stc->GetSelectionEnd() );
            int curLine   = startLine;
            /**
                Fix a glitch: when selecting multiple lines and the caret
                is at the start of the line after the last line selected,
                the code would, wrongly, (un)comment that line too.
                This fixes it.
            */
            if (startLine != endLine && // selection is more than one line
            stc->GetColumn( stc->GetSelectionEnd() ) == 0) // and the caret is at the start of the line
            {
                // don't take into account the line the caret is on,
                // because it contains no selection (caret_column == 0)...
                --endLine;
            }

            while( curLine <= endLine )
            {
                // For each line: if it is commented, uncomment.
                wxString strLine = stc->GetLine( curLine );

                bool startsWithComment;
                bool endsWithComment;

                // check for line comment
                startsWithComment = strLine.Strip( wxString::leading ).StartsWith( comment.lineComment );
                if ( startsWithComment )
                {      // we know the comment is there (maybe preceded by white space)
                    int Pos = strLine.Find(comment.lineComment);
                    int start = stc->PositionFromLine( curLine ) + Pos;
                    int end = start + comment.lineComment.Length();
                    stc->SetTargetStart( start );
                    stc->SetTargetEnd( end );
                    stc->ReplaceTarget( wxEmptyString );
                }

                // check for stream comment
                startsWithComment = strLine.Strip( wxString::leading  ).StartsWith( comment.streamCommentStart ); // check for stream comment start
                endsWithComment = strLine.Strip( wxString::trailing ).EndsWith( comment.streamCommentEnd); // check for stream comment end
                if ( startsWithComment && endsWithComment )
                {
                    int Pos;
                    int start;
                    int end;

                    // we know the start comment is there (maybe preceded by white space)
                    Pos = strLine.Find(comment.streamCommentStart);
                    start = stc->PositionFromLine( curLine ) + Pos;
                    end = start + comment.streamCommentStart.Length();
                    stc->SetTargetStart( start );
                    stc->SetTargetEnd( end );
                    stc->ReplaceTarget( wxEmptyString );

                    // we know the end comment is there too (maybe followed by white space)
                    // attention!! we have to subtract the length of the comment we already removed
                    Pos = strLine.rfind(comment.streamCommentEnd,strLine.npos) - comment.streamCommentStart.Length();
                    start = stc->PositionFromLine( curLine ) + Pos;
                    end = start + comment.streamCommentEnd.Length();
                    stc->SetTargetStart( start );
                    stc->SetTargetEnd( end );
                    stc->ReplaceTarget( wxEmptyString );
                }
                ++curLine;
            } // end while
            stc->SetSelectionVoid(stc->PositionFromLine(startLine),stc->PositionFromLine(endLine)+stc->LineLength(endLine));
        }
        stc->EndUndoAction();
    }
}

void MainFrame::OnEditToggleCommentSelected(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
    {
        cbStyledTextCtrl* stc = ed->GetControl();
        wxString comment =
            Manager::Get()->GetEditorManager()->GetColourSet()->GetCommentToken( ed->GetLanguage() ).lineComment;
        if (comment==wxEmptyString)
            return;

        stc->BeginUndoAction();
        if ( wxSCI_INVALID_POSITION != stc->GetSelectionStart() )
        {
            int startLine = stc->LineFromPosition( stc->GetSelectionStart() );
            int endLine   = stc->LineFromPosition( stc->GetSelectionEnd() );
            int curLine   = startLine;
            /**
                Fix a glitch: when selecting multiple lines and the caret
                is at the start of the line after the last line selected,
                the code would, wrongly, (un)comment that line too.
                This fixes it.
            */
            if (startLine != endLine && // selection is more than one line
            stc->GetColumn( stc->GetSelectionEnd() ) == 0) // and the caret is at the start of the line
            {
                // don't take into account the line the caret is on,
                // because it contains no selection (caret_column == 0)...
                --endLine;
            }

            while( curLine <= endLine )
            {
                // For each line: If it's commented, uncomment. Otherwise, comment.
                wxString strLine = stc->GetLine( curLine );
                int commentPos = strLine.Strip( wxString::leading ).Find( comment );

                if ( -1 == commentPos || commentPos > 0 )
                    stc->InsertText( stc->PositionFromLine( curLine ), comment );
                else
                {      // we know the comment is there (maybe preceded by white space)
                    int Pos = strLine.Find(comment);
                    int start = stc->PositionFromLine( curLine ) + Pos;
                    int end = start + comment.Length();
                    stc->SetTargetStart( start );
                    stc->SetTargetEnd( end );
                    stc->ReplaceTarget( wxEmptyString );
                }
                ++curLine;
            }
            stc->SetSelectionVoid(stc->PositionFromLine(startLine),stc->PositionFromLine(endLine)+stc->LineLength(endLine));
        }
        stc->EndUndoAction();
    }
}

void MainFrame::OnEditStreamCommentSelected(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
    {
        cbStyledTextCtrl* stc = ed->GetControl();
        CommentToken comment =
            Manager::Get()->GetEditorManager()->GetColourSet()->GetCommentToken( ed->GetLanguage() );
        if (comment.streamCommentStart==wxEmptyString)
            return;

        stc->BeginUndoAction();
        if ( wxSCI_INVALID_POSITION != stc->GetSelectionStart() )
        {
            int startPos = stc->GetSelectionStart();
            int endPos   = stc->GetSelectionEnd();
            if ( startPos == endPos )
            {   // if nothing selected stream comment current *word* first
                startPos = stc->WordStartPosition(stc->GetCurrentPos(), true);
                endPos   = stc->WordEndPosition  (stc->GetCurrentPos(), true);
                if ( startPos == endPos )
                {   // if nothing selected stream comment current *line*
                    startPos = stc->PositionFromLine  (stc->LineFromPosition(startPos));
                    endPos   = stc->GetLineEndPosition(stc->LineFromPosition(startPos));
                }
            }
            else
            {
                /**
                    Fix a glitch: when selecting multiple lines and the caret
                    is at the start of the line after the last line selected,
                    the code would, wrongly, (un)comment that line too.
                    This fixes it.
                */
                if (stc->GetColumn( stc->GetSelectionEnd() ) == 0) // and the caret is at the start of the line
                {
                    // don't take into account the line the caret is on,
                    // because it contains no selection (caret_column == 0)...
                    --endPos;
                }
            }
            // stream comment block
            int p1 = startPos - 1;
            while (stc->GetCharAt(p1) == _T(' ') && p1 > 0)
                --p1;
            p1 -= 1;
            int p2 = endPos;
            while (stc->GetCharAt(p2) == _T(' ') && p2 < stc->GetLength())
                ++p2;
            const wxString start = stc->GetTextRange(p1, p1 + comment.streamCommentStart.Length());
            const wxString end = stc->GetTextRange(p2, p2 + comment.streamCommentEnd.Length());
            if (start == comment.streamCommentStart && end == comment.streamCommentEnd)
            {
                stc->SetTargetStart(p1);
                stc->SetTargetEnd(p2 + 2);
                wxString target = stc->GetTextRange(p1 + 2, p2);
                stc->ReplaceTarget(target);
                stc->GotoPos(p1 + target.Length());
            }
            else
            {
                stc->InsertText( startPos, comment.streamCommentStart );
                // we already inserted some characters so out endPos changed
                startPos += comment.streamCommentStart.Length();
                endPos += comment.streamCommentStart.Length();
                stc->InsertText( endPos, comment.streamCommentEnd );
                stc->SetSelectionVoid(startPos,endPos);
            }

        }
        stc->EndUndoAction();
    }
}

void MainFrame::OnEditBoxCommentSelected(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
    {

        cbStyledTextCtrl* stc = ed->GetControl();
        CommentToken comment =
            Manager::Get()->GetEditorManager()->GetColourSet()->GetCommentToken( ed->GetLanguage() );
        if (comment.boxCommentStart==wxEmptyString)
            return;

        wxString nlc;
        switch (stc->GetEOLMode())
        {
            case wxSCI_EOL_CRLF: nlc=_T("\r\n"); break;
            case wxSCI_EOL_CR:   nlc=_T("\r");   break;
            case wxSCI_EOL_LF:   nlc=_T("\n");   break;

        }

        stc->BeginUndoAction();
        if ( wxSCI_INVALID_POSITION != stc->GetSelectionStart() )
        {
            int startLine = stc->LineFromPosition( stc->GetSelectionStart() );
            int endLine   = stc->LineFromPosition( stc->GetSelectionEnd() );
            int curLine   = startLine;
            /**
                Fix a glitch: when selecting multiple lines and the caret
                is at the start of the line after the last line selected,
                the code would, wrongly, (un)comment that line too.
                This fixes it.
            */
            if (startLine != endLine && // selection is more than one line
            stc->GetColumn( stc->GetSelectionEnd() ) == 0) // and the caret is at the start of the line
            {
                // don't take into account the line the caret is on,
                // because it contains no selection (caret_column == 0)...
                --endLine;
            }



            if (startLine == endLine) // if selection is only one line ...
            {
                // ... then insert streamcomment tokens at the beginning and the end of the line
                stc->InsertText( stc->PositionFromLine  ( curLine ), comment.streamCommentStart );
                stc->InsertText( stc->GetLineEndPosition( curLine ), comment.streamCommentEnd   );
            }
            else // selection is more than one line
            {
                // insert boxcomment start token
                stc->InsertText( stc->PositionFromLine( curLine ), comment.boxCommentStart );
                ++curLine; // we already commented the first line about 9 lines above
                while( curLine <= endLine )
                {
                    // For each line: comment.
                    stc->InsertText( stc->PositionFromLine( curLine ), comment.boxCommentMid );
                    ++curLine;
                } // end while

                // insert boxcomment end token and add a new line character
                stc->InsertText( stc->PositionFromLine( curLine ), comment.boxCommentEnd + nlc);
            } // end if
            stc->SetSelectionVoid(stc->PositionFromLine(startLine),stc->PositionFromLine(endLine)+stc->LineLength(endLine));
        }
        stc->EndUndoAction();
    }
}

void MainFrame::OnEditHighlightMode(wxCommandEvent& event)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
    {
        EditorColourSet* theme = Manager::Get()->GetEditorManager()->GetColourSet();
        if (theme)
        {
            HighlightLanguage lang = theme->GetHighlightLanguage(_T(""));
            if (event.GetId() != idEditHighlightModeText)
            {
                wxMenu* hl = 0;
                GetMenuBar()->FindItem(idEditHighlightModeText, &hl);
                if (hl)
                {
                    wxMenuItem* item = hl->FindItem(event.GetId());
                    if (item)
                    #if wxCHECK_VERSION(2, 9, 0)
                        lang = theme->GetHighlightLanguage(item->GetItemLabelText());
                    #else
                        lang = theme->GetHighlightLanguage(item->GetLabel());
                    #endif
                }
            }
            ed->SetLanguage(lang);
        }
    }
}

void MainFrame::OnEditFoldAll(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
        ed->FoldAll();
}

void MainFrame::OnEditUnfoldAll(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
        ed->UnfoldAll();
}

void MainFrame::OnEditToggleAllFolds(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
        ed->ToggleAllFolds();
}

void MainFrame::OnEditFoldBlock(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
        ed->FoldBlockFromLine();
}

void MainFrame::OnEditUnfoldBlock(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
        ed->UnfoldBlockFromLine();
}

void MainFrame::OnEditToggleFoldBlock(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
        ed->ToggleFoldBlockFromLine();
}

void MainFrame::OnEditEOLMode(wxCommandEvent& event)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
    {
        int mode = -1;

        if (event.GetId() == idEditEOLCRLF)
            mode = wxSCI_EOL_CRLF;
        else if (event.GetId() == idEditEOLCR)
            mode = wxSCI_EOL_CR;
        else if (event.GetId() == idEditEOLLF)
            mode = wxSCI_EOL_LF;

        if (mode != -1 && mode != ed->GetControl()->GetEOLMode())
        {
            ed->GetControl()->BeginUndoAction();
            ed->GetControl()->ConvertEOLs(mode);
            ed->GetControl()->SetEOLMode(mode);
            ed->GetControl()->EndUndoAction();
        }
    }
}

void MainFrame::OnEditEncoding(wxCommandEvent& event)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed)
        return;

    if ( event.GetId() == idEditEncodingUseBom )
    {
        ed->SetUseBom( !ed->GetUseBom() );
        return;
    }

    wxFontEncoding encoding = wxFONTENCODING_SYSTEM;

    if ( event.GetId() == idEditEncodingDefault )
        encoding = wxFONTENCODING_SYSTEM;
    else if ( event.GetId() == idEditEncodingAscii )
        encoding = wxFONTENCODING_ISO8859_1;
    else if ( event.GetId() == idEditEncodingUtf7 )
        encoding = wxFONTENCODING_UTF7;
    else if ( event.GetId() == idEditEncodingUtf8 )
        encoding = wxFONTENCODING_UTF8;
    else if ( event.GetId() == idEditEncodingUtf16 )
        encoding = wxFONTENCODING_UTF16;
    else if ( event.GetId() == idEditEncodingUtf32 )
        encoding = wxFONTENCODING_UTF32;
    else if ( event.GetId() == idEditEncodingUnicode )
        encoding = wxFONTENCODING_UNICODE;
    else if ( event.GetId() == idEditEncodingUnicode16BE )
        encoding = wxFONTENCODING_UTF16BE;
    else if ( event.GetId() == idEditEncodingUnicode16LE )
        encoding = wxFONTENCODING_UTF16LE;
    else if ( event.GetId() == idEditEncodingUnicode32BE )
        encoding = wxFONTENCODING_UTF32BE;
    else if ( event.GetId() == idEditEncodingUnicode32LE )
        encoding = wxFONTENCODING_UTF32LE;

    ed->SetEncoding(encoding);
}

void MainFrame::OnViewLayout(wxCommandEvent& event)
{
    LoadViewLayout(m_PluginIDsMap[event.GetId()]);
}

void MainFrame::OnViewLayoutSave(wxCommandEvent& /*event*/)
{
    wxString def = Manager::Get()->GetConfigManager(_T("app"))->Read(_T("/main_frame/layout/default"));
    wxString name = wxGetTextFromUser(_("Enter the name for this perspective"), _("Save current perspective"), def);
    if (!name.IsEmpty())
    {
        DoFixToolbarsLayout();
        SaveViewLayout(name, m_LayoutManager.SavePerspective(), m_pInfoPane->SaveTabOrder(), true);
    }
}

void MainFrame::OnViewLayoutDelete(wxCommandEvent& /*event*/)
{
    if (m_LastLayoutName == gDefaultLayout)
    {
        if (cbMessageBox(_("The default perspective cannot be deleted. It can always be reverted to "
                        "a predefined state though.\nDo you want to revert it now?"),
                        _("Confirmation"),
                        wxICON_QUESTION | wxYES_NO | wxNO_DEFAULT) == wxID_YES)
        {
            m_LayoutViews[gDefaultLayout] = gDefaultLayoutData;
            m_LayoutMessagePane[gDefaultLayout] = gDefaultMessagePaneLayoutData;
            LoadViewLayout(gDefaultLayout);
        }
        return;
    }

    if (m_LastLayoutName == gMinimalLayout)
    {
        if (cbMessageBox(_("The minimal layout cannot be deleted. It can always be reverted to "
                        "a predefined state though.\nDo you want to revert it now?"),
                        _("Confirmation"),
                        wxICON_QUESTION | wxYES_NO | wxNO_DEFAULT) == wxID_YES)
        {
            wxString tempLayout = m_PreviousLayoutName;
            m_LayoutViews[gMinimalLayout] = gMinimalLayoutData;
            m_LayoutMessagePane[gMinimalLayout] = gMinimalMessagePaneLayoutData;
            LoadViewLayout(gMinimalLayout);
            m_PreviousLayoutName = tempLayout;
        }
        return;
    }

    if (cbMessageBox(wxString::Format(_("Are you really sure you want to delete the perspective '%s'?"), m_LastLayoutName.wx_str()),
                    _("Confirmation"),
                    wxICON_QUESTION | wxYES_NO | wxNO_DEFAULT) == wxID_YES)
    {
        // first delete it from the hashmap
        LayoutViewsMap::iterator it = m_LayoutViews.find(m_LastLayoutName);
        if (it != m_LayoutViews.end())
            m_LayoutViews.erase(it);
        it = m_LayoutMessagePane.find(m_LastLayoutName);
        if (it != m_LayoutMessagePane.end())
            m_LayoutMessagePane.erase(it);

        // now delete the menu item too
        wxMenu* viewLayouts = 0;
        GetMenuBar()->FindItem(idViewLayoutSave, &viewLayouts);
        if (viewLayouts)
        {
            int id = viewLayouts->FindItem(m_LastLayoutName);
            if (id != wxNOT_FOUND)
                viewLayouts->Delete(id);
            // delete the id from the map too
            PluginIDsMap::iterator it2 = m_PluginIDsMap.find(id);
            if (it2 != m_PluginIDsMap.end())
                m_PluginIDsMap.erase(it2);
        }

        cbMessageBox(wxString::Format(_("Perspective '%s' deleted.\nWill now revert to perspective '%s'..."), m_LastLayoutName.wx_str(), gDefaultLayout.wx_str()),
                        _("Information"), wxICON_INFORMATION);

        // finally, revert to the default layout
        m_LastLayoutName = gDefaultLayout; // do not ask to save old layout ;)
        LoadViewLayout(gDefaultLayout);
    }
}

void MainFrame::OnNotebookDoubleClick(CodeBlocksEvent& /*event*/)
{
    if (m_LastLayoutName == gMinimalLayout)
        LoadViewLayout(m_PreviousLayoutName.IsEmpty()?Manager::Get()->GetConfigManager(_T("app"))->Read(_T("/environment/view/layout_to_toggle"),gDefaultLayout):m_PreviousLayoutName);
    else
    {
        ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("app"));
        if (cfg->ReadBool(_T("/environment/view/dbl_clk_maximize"), true))
            LoadViewLayout(gMinimalLayout);
    }
}

void MainFrame::OnViewScriptConsole(wxCommandEvent& /*event*/)
{
    ShowHideScriptConsole();
}

void MainFrame::OnSearchFind(wxCommandEvent& event)
{
    bool bDoMultipleFiles = (event.GetId() == idSearchFindInFiles);
    if (!bDoMultipleFiles)
        bDoMultipleFiles = !Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    Manager::Get()->GetEditorManager()->ShowFindDialog(false, bDoMultipleFiles);
}

void MainFrame::OnSearchFindNext(wxCommandEvent& event)
{
    bool bNext = !(event.GetId() == idSearchFindPrevious);
    Manager::Get()->GetEditorManager()->FindNext(bNext);
}

void MainFrame::OnSearchReplace(wxCommandEvent& event)
{
    bool bDoMultipleFiles = (event.GetId() == idSearchReplaceInFiles);
    if (!bDoMultipleFiles)
    {
        bDoMultipleFiles = !Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    }
    Manager::Get()->GetEditorManager()->ShowFindDialog(true, bDoMultipleFiles);
}

void MainFrame::OnSearchGotoLine(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed)
        return;

    int max = ed->GetControl()->LineFromPosition(ed->GetControl()->GetLength()) + 1;

    /**
    @remarks We use wxGetText instead of wxGetNumber because wxGetNumber *must*
    provide an initial line number...which doesn't make sense, and just keeps the
    user deleting the initial line number everytime he instantiates the dialog.
    However, this is just a temporary hack, because the default dialog used isn't
    that suitable either.
    */
    wxString strLine = wxGetTextFromUser( wxString::Format(_("Line (1 - %d): "), max),
                                        _("Goto line"),
                                        _T( "" ),
                                        this );
    long int line = 0;
    strLine.ToLong(&line);
    if ( line >= 1 && line <= max )
    {
        ed->UnfoldBlockFromLine(line - 1);
        ed->GotoLine(line - 1);
    }
}

void MainFrame::OnSearchGotoNextChanged(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
        ed->GotoNextChanged();
}

void MainFrame::OnSearchGotoPrevChanged(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
        ed->GotoPreviousChanged();
}

void MainFrame::OnHelpAbout(wxCommandEvent& WXUNUSED(event))
{
    dlgAbout dlg(this);
    PlaceWindow(&dlg, pdlHead);
    dlg.ShowModal();
}

void MainFrame::OnHelpTips(wxCommandEvent& /*event*/)
{
    ShowTips(true);
}

void MainFrame::OnFileMenuUpdateUI(wxUpdateUIEvent& event)
{
    if (Manager::isappShuttingDown())
    {
        event.Skip();
        return;
    }

    EditorBase*  ed   = Manager::Get()->GetEditorManager() ? Manager::Get()->GetEditorManager()->GetActiveEditor() : 0;
    cbProject*   prj  = Manager::Get()->GetProjectManager() ? Manager::Get()->GetProjectManager()->GetActiveProject() : 0L;
    EditorBase*  sh   = Manager::Get()->GetEditorManager()->GetEditor(g_StartHereTitle);
    cbWorkspace* wksp = Manager::Get()->GetProjectManager()->GetWorkspace();
    wxMenuBar*   mbar = GetMenuBar();

    bool canCloseProject = (ProjectManager::CanShutdown() && EditorManager::CanShutdown())
                            && prj && !prj->GetCurrentlyCompilingTarget();
    bool canClose        = ed && !(sh && Manager::Get()->GetEditorManager()->GetEditorsCount() == 1);
    bool canSaveFiles    = ed && !(sh && Manager::Get()->GetEditorManager()->GetEditorsCount() == 1);
    bool canSaveAll      =     (prj && prj->GetModified())
                            || (wksp && !wksp->IsDefault() && wksp->GetModified())
                            || canSaveFiles;

    mbar->Enable(idFileCloseProject,                  canCloseProject);
    mbar->Enable(idFileOpenRecentFileClearHistory,    m_pFilesHistory->GetCount());
    mbar->Enable(idFileOpenRecentProjectClearHistory, m_pProjectsHistory->GetCount());
    mbar->Enable(idFileClose,                         canClose);
    mbar->Enable(idFileCloseAll,                      canClose);
    mbar->Enable(idFileSave,                          ed && ed->GetModified());
    mbar->Enable(idFileSaveAs,                        canSaveFiles);
    mbar->Enable(idFileSaveAllFiles,                  canSaveFiles);
    mbar->Enable(idFileSaveProject,                   prj && prj->GetModified() && canCloseProject);
    mbar->Enable(idFileSaveProjectAs,                 prj && canCloseProject);
    mbar->Enable(idFileOpenDefWorkspace,              canCloseProject);
    mbar->Enable(idFileSaveWorkspace,                 Manager::Get()->GetProjectManager() && canCloseProject);
    mbar->Enable(idFileSaveWorkspaceAs,               Manager::Get()->GetProjectManager() && canCloseProject);
    mbar->Enable(idFileCloseWorkspace,                Manager::Get()->GetProjectManager() && canCloseProject);
    mbar->Enable(idFileSaveAll,                       canSaveAll);
    mbar->Enable(idFilePrint,                         Manager::Get()->GetEditorManager() && Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor());

    if (m_pToolbar)
    {
        m_pToolbar->EnableTool(idFileSave,         ed && ed->GetModified());
        m_pToolbar->EnableTool(idFileSaveAllFiles, canSaveFiles);
        m_pToolbar->EnableTool(idFileSaveAll,      canSaveAll);
        m_pToolbar->EnableTool(idFilePrint,        Manager::Get()->GetEditorManager() && Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor());
    }

    event.Skip();
}

void MainFrame::OnEditMenuUpdateUI(wxUpdateUIEvent& event)
{
    if (Manager::isappShuttingDown())
    {
        event.Skip();
        return;
    }

    cbEditor*   ed = NULL;
    EditorBase* eb = NULL;
    bool hasSel    = false;
    bool canUndo   = false;
    bool canRedo   = false;
    bool canPaste  = false;
    bool canCut    = false;
    bool canSelAll = false;

    if (Manager::Get()->GetEditorManager() && !Manager::isappShuttingDown())
    {
        ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
        eb = Manager::Get()->GetEditorManager()->GetActiveEditor();
    }

    wxMenuBar* mbar = GetMenuBar();

    if (eb)
    {
        canUndo   = eb->CanUndo();
        canRedo   = eb->CanRedo();
        hasSel    = eb->HasSelection();
        canPaste  = eb->CanPaste();
        canCut    = !eb->IsReadOnly() && hasSel;
        canSelAll = eb->CanSelectAll();
    }

    mbar->Enable(idEditUndo,                  canUndo);
    mbar->Enable(idEditRedo,                  canRedo);
    mbar->Enable(idEditClearHistory,          canUndo || canRedo);
    mbar->Enable(idEditCut,                   canCut);
    mbar->Enable(idEditCopy,                  hasSel);
    mbar->Enable(idEditPaste,                 canPaste);
    mbar->Enable(idEditSwapHeaderSource,      ed);
    mbar->Enable(idEditGotoMatchingBrace,     ed);
    mbar->Enable(idEditHighlightMode,         ed);
    mbar->Enable(idEditSelectAll,             canSelAll);
    mbar->Enable(idEditBookmarks,             ed);
    mbar->Enable(idEditFolding,               ed &&
                                              Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/folding/show_folds"), false));
    mbar->Enable(idEditEOLMode,               ed);
    mbar->Enable(idEditEncoding,              ed);
    mbar->Enable(idEditSpecialCommands,       ed);
    mbar->Enable(idEditSpecialCommandsCase,   ed && hasSel);
    mbar->Enable(idEditCommentSelected,       ed);
    mbar->Enable(idEditUncommentSelected,     ed);
    mbar->Enable(idEditToggleCommentSelected, ed);
    mbar->Enable(idEditStreamCommentSelected, ed);
    mbar->Enable(idEditBoxCommentSelected,    ed);

    if (ed)
    {
        // OK... this was the strangest/silliest/most-frustrating bug ever in the computer programs history...
        // Under wxGTK it seems that if you try to Check() a menu item if its container Menu is disabled,
        // you enter an endless message loop eating 100% CPU...
        // DARN!
        // This fixes the dreaded 'linux-hang-on-close-project' bug.

        switch (ed->GetControl()->GetEOLMode())
        {
            case wxSCI_EOL_CRLF:
                mbar->Check(idEditEOLCRLF, true);
                break;
            case wxSCI_EOL_CR:
                mbar->Check(idEditEOLCR,   true);
                break;
            case wxSCI_EOL_LF:
                mbar->Check(idEditEOLLF,   true);
                break;
        }

        bool defenc = ed && (   ed->GetEncoding() == wxFONTENCODING_SYSTEM
                             || ed->GetEncoding() == wxLocale::GetSystemEncoding() );

        mbar->Check(idEditEncodingDefault,     defenc);
        mbar->Check(idEditEncodingUseBom,      ed && ed->GetUseBom());
        mbar->Check(idEditEncodingAscii,       ed && ed->GetEncoding() == wxFONTENCODING_ISO8859_1);
        mbar->Check(idEditEncodingUtf7,        ed && ed->GetEncoding() == wxFONTENCODING_UTF7);
        mbar->Check(idEditEncodingUtf8,        ed && ed->GetEncoding() == wxFONTENCODING_UTF8);
        mbar->Check(idEditEncodingUnicode,     ed && ed->GetEncoding() == wxFONTENCODING_UNICODE);
        mbar->Check(idEditEncodingUtf16,       ed && ed->GetEncoding() == wxFONTENCODING_UTF16);
        mbar->Check(idEditEncodingUtf32,       ed && ed->GetEncoding() == wxFONTENCODING_UTF32);
        mbar->Check(idEditEncodingUnicode16BE, ed && ed->GetEncoding() == wxFONTENCODING_UTF16BE);
        mbar->Check(idEditEncodingUnicode16LE, ed && ed->GetEncoding() == wxFONTENCODING_UTF16LE);
        mbar->Check(idEditEncodingUnicode32BE, ed && ed->GetEncoding() == wxFONTENCODING_UTF32BE);
        mbar->Check(idEditEncodingUnicode32LE, ed && ed->GetEncoding() == wxFONTENCODING_UTF32LE);

        wxMenu* hl = 0;
        mbar->FindItem(idEditHighlightModeText, &hl);
        if (hl)
            mbar->Check(hl->FindItem(ed->GetColourSet()->GetLanguageName(ed->GetLanguage())), true);
    }

    if (m_pToolbar)
    {
        m_pToolbar->EnableTool(idEditUndo,  canUndo);
        m_pToolbar->EnableTool(idEditRedo,  canRedo);
        m_pToolbar->EnableTool(idEditCut,   canCut);
        m_pToolbar->EnableTool(idEditCopy,  hasSel);
        m_pToolbar->EnableTool(idEditPaste, canPaste);
    }

    event.Skip();
}

void MainFrame::OnViewMenuUpdateUI(wxUpdateUIEvent& event)
{
    if (Manager::isappShuttingDown())
    {
        event.Skip();
        return;
    }
    wxMenuBar* mbar   = GetMenuBar();
    cbEditor*  ed     = Manager::Get()->GetEditorManager() ? Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor() : 0;
    bool       manVis = m_LayoutManager.GetPane(Manager::Get()->GetProjectManager()->GetNotebook()).IsShown();

    mbar->Check(idViewManager,             manVis);
    mbar->Check(idViewLogManager,          m_LayoutManager.GetPane(m_pInfoPane).IsShown());
    mbar->Check(idViewStatusbar,           GetStatusBar() && GetStatusBar()->IsShown());
    mbar->Check(idViewScriptConsole,       m_LayoutManager.GetPane(m_pScriptConsole).IsShown());
    mbar->Check(idViewFullScreen,          IsFullScreen());
    mbar->Enable(idViewFocusEditor,        ed);
    mbar->Enable(idViewFocusManagement,    manVis);
    mbar->Enable(idViewFocusLogsAndOthers, m_pInfoPane->IsShown());

    // toolbars
    mbar->Check(idViewToolMain,     m_LayoutManager.GetPane(m_pToolbar).IsShown());
    mbar->Check(idViewToolDebugger, m_LayoutManager.GetPane(m_debuggerToolbarHandler->GetToolbar(false)).IsShown());
    wxMenu* viewToolbars = 0;
    GetMenuBar()->FindItem(idViewToolMain, &viewToolbars);
    if (viewToolbars)
    {
        for (size_t i = 0; i < viewToolbars->GetMenuItemCount(); ++i)
        {
            wxMenuItem* item = viewToolbars->GetMenuItems()[i];
            wxString pluginName = m_PluginIDsMap[item->GetId()];
            if (!pluginName.IsEmpty())
            {
                cbPlugin* plugin = Manager::Get()->GetPluginManager()->FindPluginByName(pluginName);
                if (plugin)
                    item->Check(m_LayoutManager.GetPane(m_PluginsTools[plugin]).IsShown());
            }
        }
    }

    event.Skip();
}

void MainFrame::OnSearchMenuUpdateUI(wxUpdateUIEvent& event)
{
    if (Manager::isappShuttingDown())
    {
        event.Skip();
        return;
    }
    cbEditor* ed = Manager::Get()->GetEditorManager() ? Manager::Get()->GetEditorManager()->GetBuiltinEditor(Manager::Get()->GetEditorManager()->GetActiveEditor()) : 0;

    bool enableGotoChanged = false;

    if (ed)
        enableGotoChanged = Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/margin/use_changebar"), true) && (ed->CanUndo() || ed->CanRedo());

    wxMenuBar* mbar = GetMenuBar();

    // 'Find' and 'Replace' are always enabled for (find|replace)-in-files
    mbar->Enable(idSearchFind,                ed);
    mbar->Enable(idSearchFindNext,            ed);
    mbar->Enable(idSearchFindPrevious,        ed);
    mbar->Enable(idSearchReplace,             ed);
    mbar->Enable(idSearchGotoLine,            ed);
    mbar->Enable(idSearchGotoNextChanged,     enableGotoChanged);
    mbar->Enable(idSearchGotoPreviousChanged, enableGotoChanged);

//    if (m_pToolbar)
//    {
//        m_pToolbar->EnableTool(idSearchFind, ed);
//        m_pToolbar->EnableTool(idSearchReplace, ed);
//    }

    event.Skip();
}

void MainFrame::OnProjectMenuUpdateUI(wxUpdateUIEvent& event)
{
    if (Manager::isappShuttingDown())
    {
        event.Skip();
        return;
    }
    cbProject* prj = Manager::Get()->GetProjectManager() ? Manager::Get()->GetProjectManager()->GetActiveProject() : 0L;
    wxMenuBar* mbar = GetMenuBar();

    bool canCloseProject = (ProjectManager::CanShutdown() && EditorManager::CanShutdown());
    mbar->Enable(idFileCloseProject,           prj && canCloseProject);
    mbar->Enable(idFileCloseAllProjects,       prj && canCloseProject);
    mbar->Enable(idFileSaveProject,            prj && prj->GetModified() && canCloseProject);
    mbar->Enable(idFileSaveProjectAs,          prj && canCloseProject);
    mbar->Enable(idFileSaveProjectAllProjects, prj && canCloseProject);
    mbar->Enable(idFileSaveProjectTemplate,    prj && canCloseProject);

    event.Skip();
}

void MainFrame::OnEditorUpdateUI(CodeBlocksEvent& event)
{
    if (Manager::isappShuttingDown())
    {
        event.Skip();
        return;
    }

    if (Manager::Get()->GetEditorManager() && event.GetEditor() == Manager::Get()->GetEditorManager()->GetActiveEditor())
        DoUpdateStatusBar();

    event.Skip();
}

void MainFrame::OnToggleBar(wxCommandEvent& event)
{
    wxWindow* win = 0;
    if (event.GetId() == idViewManager)
        win = Manager::Get()->GetProjectManager()->GetNotebook();
    else if (event.GetId() == idViewLogManager)
        win = m_pInfoPane;
    else if (event.GetId() == idViewToolMain)
        win = m_pToolbar;
    else if (event.GetId() == idViewToolDebugger)
        win = m_debuggerToolbarHandler->GetToolbar();
    else
    {
        wxString pluginName = m_PluginIDsMap[event.GetId()];
        if (!pluginName.IsEmpty())
        {
            cbPlugin* plugin = Manager::Get()->GetPluginManager()->FindPluginByName(pluginName);
            if (plugin)
                win = m_PluginsTools[plugin];
        }
    }

    if (win)
    {
        // use last visible size as BestSize, Logs & others does no longer "forget" it's size
        if (!event.IsChecked())
             m_LayoutManager.GetPane(win).BestSize(win->GetSize());

        m_LayoutManager.GetPane(win).Show(event.IsChecked());
        DoUpdateLayout();
    }
}

void MainFrame::OnToggleStatusBar(wxCommandEvent& /*event*/)
{
    cbStatusBar* sb = (cbStatusBar*)GetStatusBar();
    if (!sb) return;

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("app"));
    const bool show = !cfg->ReadBool(_T("/main_frame/statusbar"), true);
    cfg->Write(_T("/main_frame/statusbar"), show);

    DoUpdateStatusBar();
    sb->Show(show);
    if ( show ) SendSizeEvent();
    DoUpdateLayout();
}

void MainFrame::OnFocusEditor(wxCommandEvent& /*event*/)
{
    EditorManager* edman = Manager::Get()->GetEditorManager();
    cbAuiNotebook* nb = edman?edman->GetNotebook():nullptr;
    if (nb)
        nb->FocusActiveTabCtrl();
}

void MainFrame::OnFocusManagement(wxCommandEvent& /*event*/)
{
    cbAuiNotebook* nb = m_pPrjMan ? m_pPrjMan->GetNotebook():nullptr;
    if (nb)
        nb->FocusActiveTabCtrl();
}

void MainFrame::OnFocusLogsAndOthers(wxCommandEvent& /*event*/)
{
    if (m_pInfoPane)
        m_pInfoPane->FocusActiveTabCtrl();
}

void MainFrame::OnSwitchTabs(wxCommandEvent& /*event*/)
{
    // Get the notebook from the editormanager:
    cbAuiNotebook* nb = Manager::Get()->GetEditorManager()->GetNotebook();
    if (!nb)
        return;

    // Create container and add all open editors:
    wxSwitcherItems items;
    items.AddGroup(_("Open files"), wxT("editors"));
    if (!Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/tabs_stacked_based_switching")))
    {   // Switch tabs editor with tab order
        for (size_t i = 0; i < nb->GetPageCount(); ++i)
        {
            wxString title = nb->GetPageText(i);
            wxWindow* window = nb->GetPage(i);

            items.AddItem(title, title, GetEditorDescription(static_cast<EditorBase*> (window)), i, nb->GetPageBitmap(i)).SetWindow(window);
        }

        // Select the focused editor:
        int idx = items.GetIndexForFocus();
        if (idx != wxNOT_FOUND)
            items.SetSelection(idx);
    }
    else
    {   // Switch tabs editor with last used order
        int index = 0;
        cbNotebookStack* body;
        for (body = Manager::Get()->GetEditorManager()->GetNotebookStack(); body != NULL; body = body->next)
        {
            index = nb->GetPageIndex(body->window);
            if (index == wxNOT_FOUND)
                continue;
            wxString title = nb->GetPageText(index);
            items.AddItem(title, title, GetEditorDescription(static_cast<EditorBase*> (body->window)), index, nb->GetPageBitmap(index)).SetWindow(body->window);
        }

        // Select the focused editor:
        if(items.GetItemCount() > 2)
            items.SetSelection(2); // CTRL + TAB directly select the last editor, not the current one
        else
            items.SetSelection(items.GetItemCount()-1);
    }

    // Create the switcher dialog
    wxSwitcherDialog dlg(items, wxGetApp().GetTopWindow());

    // Ctrl+Tab workaround for non windows platforms:
    if      (platform::cocoa)
        dlg.SetModifierKey(WXK_ALT);
    else if (platform::gtk)
        dlg.SetExtraNavigationKey(wxT(','));

    // Finally show the dialog:
    int answer = dlg.ShowModal();

    // If necessary change the selected editor:
    if ((answer == wxID_OK) && (dlg.GetSelection() != -1))
    {
        wxSwitcherItem& item = items.GetItem(dlg.GetSelection());
        wxWindow* win = item.GetWindow();
        if (win)
        {
            nb->SetSelection(item.GetId());
            win->SetFocus();
        }
    }
}

void MainFrame::OnToggleFullScreen(wxCommandEvent& /*event*/)
{
    ShowFullScreen( !IsFullScreen(), wxFULLSCREEN_NOTOOLBAR// | wxFULLSCREEN_NOSTATUSBAR
                    | wxFULLSCREEN_NOBORDER | wxFULLSCREEN_NOCAPTION );

    // Create full screen-close button if we're in full screen
    if ( IsFullScreen() )
    {
        //
        // Show the button to the bottom-right of the container
        //
        wxSize containerSize = GetClientSize();
        wxSize buttonSize = m_pCloseFullScreenBtn->GetSize();

        // Align
        m_pCloseFullScreenBtn->Move( containerSize.GetWidth() - buttonSize.GetWidth(),
                    containerSize.GetHeight() - buttonSize.GetHeight() );

        m_pCloseFullScreenBtn->Show( true );
        m_pCloseFullScreenBtn->Raise();
    }
    else
    {
        m_pCloseFullScreenBtn->Show( false );
    }
}

void MainFrame::OnPluginInstalled(CodeBlocksEvent& event)
{
    PluginsUpdated(event.GetPlugin(), Installed);
}

void MainFrame::OnPluginUninstalled(CodeBlocksEvent& event)
{
    if (Manager::IsAppShuttingDown())
        return;
    PluginsUpdated(event.GetPlugin(), Uninstalled);
}

void MainFrame::OnPluginLoaded(CodeBlocksEvent& event)
{
    cbPlugin* plug = event.GetPlugin();
    if (plug)
    {
        DoAddPlugin(plug);
        const PluginInfo* info = Manager::Get()->GetPluginManager()->GetPluginInfo(plug);
        wxString msg = info ? info->title : wxString(_("<Unknown plugin>"));
        Manager::Get()->GetLogManager()->DebugLog(F(_T("%s plugin activated"), msg.wx_str()));
    }
}

void MainFrame::OnPluginUnloaded(CodeBlocksEvent& event)
{
    if (Manager::IsAppShuttingDown())
        return;

    cbPlugin* plugin = event.GetPlugin();

#if wxUSE_STATUSBAR
    cbStatusBar *sb = (cbStatusBar*)GetStatusBar();
    if ( sb )
        sb->RemoveField(plugin);
#endif

    // remove toolbar, if any
    if (m_PluginsTools[plugin])
    {
        m_LayoutManager.DetachPane(m_PluginsTools[plugin]);
        m_PluginsTools[plugin]->Destroy();
        m_PluginsTools.erase(plugin);
        DoUpdateLayout();
    }

    PluginsUpdated(plugin, Unloaded);
}

void MainFrame::OnSettingsEnvironment(wxCommandEvent& /*event*/)
{
    bool tbarsmall = m_SmallToolBar;
    bool needRestart = false;

    EnvironmentSettingsDlg dlg(this, m_LayoutManager.GetArtProvider());
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        DoUpdateEditorStyle();
        DoUpdateLayoutColours();

        m_SmallToolBar = Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/toolbar_size"), true);
        needRestart = m_SmallToolBar != tbarsmall;
        Manager::Get()->GetLogManager()->NotifyUpdate();
        ShowHideStartPage();
    }
    if (needRestart)
        cbMessageBox(_("Code::Blocks needs to be restarted for the changes to take effect."), _("Information"), wxICON_INFORMATION);
}

void MainFrame::OnGlobalUserVars(wxCommandEvent& /*event*/)
{
    Manager::Get()->GetUserVariableManager()->Configure();
}

void MainFrame::OnSettingsEditor(wxCommandEvent& /*event*/)
{
    Manager::Get()->GetEditorManager()->Configure();
}

void MainFrame::OnSettingsCompiler(wxCommandEvent& /*event*/)
{
    CompilerSettingsDlg dlg(this);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        CodeBlocksEvent event(cbEVT_SETTINGS_CHANGED);
        event.SetInt(cbSettingsType::Compiler);
        Manager::Get()->ProcessEvent(event);
    }
}

void MainFrame::OnSettingsDebugger(wxCommandEvent& /*event*/)
{
    DebuggerSettingsDlg dlg(this);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        CodeBlocksEvent event(cbEVT_SETTINGS_CHANGED);
        event.SetInt(cbSettingsType::Debugger);
        Manager::Get()->ProcessEvent(event);
    }
}

void MainFrame::OnSettingsPlugins(wxCommandEvent& /*event*/)
{
    Manager::Get()->GetPluginManager()->Configure();
}

void MainFrame::OnSettingsScripting(wxCommandEvent& /*event*/)
{
    ScriptingSettingsDlg dlg(this);
    if (dlg.ShowModal() == wxID_OK)
        RunStartupScripts();
}

void MainFrame::OnProjectActivated(CodeBlocksEvent& event)
{
    DoUpdateAppTitle();
    event.Skip();
}

void MainFrame::OnProjectOpened(CodeBlocksEvent& event)
{
    ShowHideStartPage(true);
    event.Skip();
}

void MainFrame::OnEditorOpened(CodeBlocksEvent& event)
{
    DoUpdateAppTitle();
    event.Skip();
}

void MainFrame::OnEditorActivated(CodeBlocksEvent& event)
{
    DoUpdateAppTitle();
    DoUpdateStatusBar();
    event.Skip();
}

void MainFrame::OnEditorClosed(CodeBlocksEvent& event)
{
    DoUpdateAppTitle();
    DoUpdateStatusBar();
    event.Skip();
}

void MainFrame::OnEditorSaved(CodeBlocksEvent& event)
{
    DoUpdateAppTitle();
    event.Skip();
}

void MainFrame::OnEditorModified(CodeBlocksEvent& event)
{
    DoUpdateAppTitle();
    event.Skip();
}

void MainFrame::OnProjectClosed(CodeBlocksEvent& event)
{
    ShowHideStartPage();
    event.Skip();
}

void MainFrame::OnPageChanged(wxNotebookEvent& event)
{
    DoUpdateAppTitle();
    event.Skip();
}

void MainFrame::OnShiftTab(wxCommandEvent& /*event*/)
{
    // Must make sure it's cbEditor and not EditorBase
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
        ed->DoUnIndent();
}

void MainFrame::OnCtrlAltTab(wxCommandEvent& /*event*/)
{
    wxCommandEvent dummy;
    switch (m_LastCtrlAltTabWindow)
    {
      case 1:  // Focus is on the Mgmt. panel -> Cycle to Editor
        m_LastCtrlAltTabWindow = 2;
        OnFocusEditor(dummy);
        break;
      case 2:  // Focus is on the Editor -> Cycle to Logs & others
        m_LastCtrlAltTabWindow = 3;
        OnFocusLogsAndOthers(dummy);
        break;
      case 3:  // Focus is on Logs & others -> fall through
      default: // Focus (cycle to) the Mgmt. panel
        m_LastCtrlAltTabWindow = 1;
        OnFocusManagement(dummy);
    }
}

void MainFrame::OnRequestDockWindow(CodeBlocksDockEvent& event)
{
    if (Manager::isappShuttingDown())
        return;

    wxAuiPaneInfo info;
    wxString name = event.name;
    if (name.IsEmpty())
    {
        static int idx = 0;
        name = wxString::Format(_T("UntitledPane%d"), ++idx);
    }
// TODO (mandrav##): Check for existing pane with the same name
    info.Name(name);
    info.Caption(event.title.IsEmpty() ? name : event.title);
    switch (event.dockSide)
    {
        case CodeBlocksDockEvent::dsLeft:     info.Left();   break;
        case CodeBlocksDockEvent::dsRight:    info.Right();  break;
        case CodeBlocksDockEvent::dsTop:      info.Top();    break;
        case CodeBlocksDockEvent::dsBottom:   info.Bottom(); break;
        case CodeBlocksDockEvent::dsFloating: info.Float();  break;
        default:                                             break;
    }
    info.Show(event.shown);
    info.BestSize(event.desiredSize);
    info.FloatingSize(event.floatingSize);
    info.FloatingPosition(event.floatingPos);
    info.MinSize(event.minimumSize);
    info.Layer(event.stretch ? 1 : 0);

    if (event.row != -1)
        info.Row(event.row);
    if (event.column != -1)
        info.Position(event.column);
    info.CloseButton(event.hideable ? true : false);
    m_LayoutManager.AddPane(event.pWindow, info);
    DoUpdateLayout();
}

void MainFrame::OnRequestUndockWindow(CodeBlocksDockEvent& event)
{
    wxAuiPaneInfo info = m_LayoutManager.GetPane(event.pWindow);
    if (info.IsOk())
    {
        m_LayoutManager.DetachPane(event.pWindow);
        DoUpdateLayout();
    }
}

void MainFrame::OnRequestShowDockWindow(CodeBlocksDockEvent& event)
{
    m_LayoutManager.GetPane(event.pWindow).Show();
    DoUpdateLayout();

    CodeBlocksDockEvent evt(cbEVT_DOCK_WINDOW_VISIBILITY);
    evt.pWindow = event.pWindow;
    Manager::Get()->ProcessEvent(evt);
}

void MainFrame::OnRequestHideDockWindow(CodeBlocksDockEvent& event)
{
    m_LayoutManager.GetPane(event.pWindow).Hide();
    DoUpdateLayout();

    CodeBlocksDockEvent evt(cbEVT_DOCK_WINDOW_VISIBILITY);
    evt.pWindow = event.pWindow;
    Manager::Get()->ProcessEvent(evt);
}

void MainFrame::OnDockWindowVisibility(CodeBlocksDockEvent& /*event*/)
{
//    if (m_ScriptConsoleID != -1 && event.GetId() == m_ScriptConsoleID)
//        ShowHideScriptConsole();
}

void MainFrame::OnLayoutUpdate(CodeBlocksLayoutEvent& WXUNUSED(event))
{
    DoFixToolbarsLayout();
    DoUpdateLayout();
}

void MainFrame::OnLayoutQuery(CodeBlocksLayoutEvent& event)
{
    event.layout = !m_LastLayoutName.IsEmpty() ? m_LastLayoutName : gDefaultLayout;
    event.StopPropagation();
}

void MainFrame::OnLayoutSwitch(CodeBlocksLayoutEvent& event)
{
    LoadViewLayout(event.layout, true);
}

void MainFrame::OnAddLogWindow(CodeBlocksLogEvent& event)
{
    if (Manager::IsAppShuttingDown())
        return;
    wxWindow* p = event.window;
    if (p)
        m_pInfoPane->AddNonLogger(p, event.title, event.icon);
    else
    {
        p = event.logger->CreateControl(m_pInfoPane);
        if (p)
            m_pInfoPane->AddLogger(event.logger, p, event.title, event.icon);
    }
    Manager::Get()->GetLogManager()->NotifyUpdate();
}

void MainFrame::OnRemoveLogWindow(CodeBlocksLogEvent& event)
{
    if (Manager::IsAppShuttingDown())
        return;
    if (event.window)
        m_pInfoPane->RemoveNonLogger(event.window);
    else
        m_pInfoPane->DeleteLogger(event.logger);
}

void MainFrame::OnHideLogWindow(CodeBlocksLogEvent& event)
{
    if (event.window)
        m_pInfoPane->HideNonLogger(event.window);
    else if (event.logger)
        m_pInfoPane->Hide(event.logger);
}

void MainFrame::OnSwitchToLogWindow(CodeBlocksLogEvent& event)
{
    if (event.window)
        m_pInfoPane->ShowNonLogger(event.window);
    else if (event.logger)
        m_pInfoPane->Show(event.logger);
}

void MainFrame::OnGetActiveLogWindow(CodeBlocksLogEvent& event)
{
    bool is_logger;
    int page_index = m_pInfoPane->GetCurrentPage(is_logger);

    event.logger = NULL;
    event.window = NULL;

    if (is_logger)
        event.logger = m_pInfoPane->GetLogger(page_index);
    else
        event.window = m_pInfoPane->GetWindow(page_index);
}

void MainFrame::OnShowLogManager(CodeBlocksLogEvent& /*event*/)
{
    if (!Manager::Get()->GetConfigManager(_T("message_manager"))->ReadBool(_T("/auto_hide"), false))
        return;

    m_LayoutManager.GetPane(m_pInfoPane).Show(true);
    DoUpdateLayout();
}

void MainFrame::OnHideLogManager(CodeBlocksLogEvent& /*event*/)
{
    if (!Manager::Get()->GetConfigManager(_T("message_manager"))->ReadBool(_T("/auto_hide"), false) ||
           m_AutoHideLockCounter > 0)
        return;

    m_LayoutManager.GetPane(m_pInfoPane).Show(false);
    DoUpdateLayout();
}

void MainFrame::OnLockLogManager(CodeBlocksLogEvent& /*event*/)
{
    if (!Manager::Get()->GetConfigManager(_T("message_manager"))->ReadBool(_T("/auto_hide"), false))
        return;
    ++m_AutoHideLockCounter;
}

void MainFrame::OnUnlockLogManager(CodeBlocksLogEvent& /*event*/)
{
    if (!Manager::Get()->GetConfigManager(_T("message_manager"))->ReadBool(_T("/auto_hide"), false) &&
           m_AutoHideLockCounter > 0)
        return;
    if (--m_AutoHideLockCounter == 0)
    {
        m_LayoutManager.GetPane(m_pInfoPane).Show(false);
        DoUpdateLayout();
    }
}

void MainFrame::StartupDone()
{
    m_StartupDone = true;
    DoUpdateLayout();
}

#if wxUSE_STATUSBAR
wxStatusBar *MainFrame::OnCreateStatusBar(int number, long style, wxWindowID id, const wxString& name)
{
    cbStatusBar *statusBar = new cbStatusBar(this, id, style, name);
    statusBar->SetFieldsCount(number);

    return statusBar;
}
#endif

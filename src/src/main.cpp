/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*
* $Id$
* $Date$
*/

#include "app.h"
#include "main.h"
#include "globals.h"
#include "environmentsettingsdlg.h"
#include "impexpconfig.h"
#include <cbworkspace.h>

#if defined(_MSC_VER) && defined( _DEBUG )
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif

#include <wx/tipdlg.h>
#include <wx/dnd.h>

#include <configmanager.h>
#include <cbproject.h>
#include <cbplugin.h>
#include <sdk_events.h>
#include <projectmanager.h>
#include <editormanager.h>
#include <messagemanager.h>
#include <pluginmanager.h>
#include <templatemanager.h>
#include <toolsmanager.h>
#include <personalitymanager.h>

#include "dlgaboutplugin.h"
#include "dlgabout.h"
#include "startherepage.h"
#include "printdlg.h"
#include <wx/printdlg.h>
#include <wx/util.h>
#include <wx/dockpanel.h>
#include <wx/filename.h>

class wxMyFileDropTarget : public wxFileDropTarget
{
public:
    wxMyFileDropTarget::wxMyFileDropTarget(MainFrame *frame):m_frame(frame){}
    virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
    {
        if(!m_frame) return false;
        return m_frame->OnDropFiles(x,y,filenames);
    }
private:
    MainFrame* m_frame;
};

int idFileNew = XRCID("idFileNew");
int idFileOpen = XRCID("idFileOpen");
int idFileReopen = XRCID("idFileReopen");
int idFileOpenRecentClearHistory = XRCID("idFileOpenRecentClearHistory");
int idFileSave = XRCID("idFileSave");
int idFileSaveAs = XRCID("idFileSaveAs");
int idFileSaveAllFiles = XRCID("idFileSaveAllFiles");
int idFileSaveWorkspace = XRCID("idFileSaveWorkspace");
int idFileSaveWorkspaceAs = XRCID("idFileSaveWorkspaceAs");
int idFileClose = XRCID("idFileClose");
int idFileCloseAll = XRCID("idFileCloseAll");
int idFilePrintSetup = XRCID("idFilePrintSetup");
int idFilePrint = XRCID("idFilePrint");
int idFileExit = XRCID("idFileExit");
int idFileNext = wxNewId();
int idFilePrev = wxNewId();

int idEditUndo = XRCID("idEditUndo");
int idEditRedo = XRCID("idEditRedo");
int idEditCopy = XRCID("idEditCopy");
int idEditCut = XRCID("idEditCut");
int idEditPaste = XRCID("idEditPaste");
int idEditSwapHeaderSource = XRCID("idEditSwapHeaderSource");
int idEditBookmarks = XRCID("idEditBookmarks");
int idEditBookmarksToggle = XRCID("idEditBookmarksToggle");
int idEditBookmarksPrevious = XRCID("idEditBookmarksPrevious");
int idEditBookmarksNext = XRCID("idEditBookmarksNext");
int idEditFolding = XRCID("idEditFolding");
int idEditFoldAll = XRCID("idEditFoldAll");
int idEditUnfoldAll = XRCID("idEditUnfoldAll");
int idEditToggleAllFolds = XRCID("idEditToggleAllFolds");
int idEditFoldBlock = XRCID("idEditFoldBlock");
int idEditUnfoldBlock = XRCID("idEditUnfoldBlock");
int idEditToggleFoldBlock = XRCID("idEditToggleFoldBlock");
int idEditEOLMode = XRCID("idEditEOLMode");
int idEditEOLCRLF = XRCID("idEditEOLCRLF");
int idEditEOLCR = XRCID("idEditEOLCR");
int idEditEOLLF = XRCID("idEditEOLLF");
int idEditSelectAll = XRCID("idEditSelectAll");
int idEditCommentSelected = XRCID("idEditCommentSelected");
int idEditUncommentSelected = XRCID("idEditUncommentSelected");
int idEditToggleCommentSelected = XRCID("idEditToggleCommentSelected");
int idEditAutoComplete = XRCID("idEditAutoComplete");

int idViewToolMain = XRCID("idViewToolMain");
int idViewManager = XRCID("idViewManager");
int idViewOpenFilesTree = XRCID("idViewOpenFilesTree");
int idViewMessageManager = XRCID("idViewMessageManager");
int idViewStatusbar = XRCID("idViewStatusbar");
int idViewFocusEditor = XRCID("idViewFocusEditor");
int idViewFullScreen = XRCID("idViewFullScreen");

int idSearchFind = XRCID("idSearchFind");
int idSearchFindNext = XRCID("idSearchFindNext");
int idSearchFindPrevious = XRCID("idSearchFindPrevious");
int idSearchReplace = XRCID("idSearchReplace");
int idSearchGotoLine = XRCID("idSearchGotoLine");

int idProjectNew = XRCID("idProjectNew");
int idProjectNewEmptyProject = XRCID("idProjectNewEmptyProject");
int idProjectOpen = XRCID("idProjectOpen");
int idProjectSaveProject = XRCID("idProjectSaveProject");
int idProjectSaveProjectAs = XRCID("idProjectSaveProjectAs");
int idProjectSaveAllProjects = XRCID("idProjectSaveAllProjects");
int idProjectSaveTemplate = XRCID("idProjectSaveTemplate");
int idProjectCloseProject = XRCID("idProjectCloseProject");
int idProjectCloseAllProjects = XRCID("idProjectCloseAllProjects");
int idProjectImport = XRCID("idProjectImport");
int idProjectImportDevCpp = XRCID("idProjectImportDevCpp");
int idProjectImportMSVC = XRCID("idProjectImportMSVC");
int idProjectImportMSVCWksp = XRCID("idProjectImportMSVCWksp");
int idProjectImportMSVS = XRCID("idProjectImportMSVS");
int idProjectImportMSVSWksp = XRCID("idProjectImportMSVSWksp");

int idSettingsEnvironment = XRCID("idSettingsEnvironment");
int idSettingsEditor = XRCID("idSettingsEditor");
int idPluginsManagePlugins = XRCID("idPluginsManagePlugins");
int idSettingsConfigurePlugins = XRCID("idSettingsConfigurePlugins");
int idSettingsImpExpConfig = XRCID("idSettingsImpExpConfig");

int idHelpTips = XRCID("idHelpTips");
int idHelpPlugins = XRCID("idHelpPlugins");

int idLeftSash = XRCID("idLeftSash");
int idBottomSash = XRCID("idBottomSash");
int idCloseFullScreen = XRCID("idCloseFullScreen");
int idShiftTab = wxNewId();
DLLIMPORT extern int ID_EditorManagerCloseButton;

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_CLOSE(MainFrame::OnApplicationClose)

    EVT_UPDATE_UI(idFileOpenRecentClearHistory, MainFrame::OnFileMenuUpdateUI)
    EVT_UPDATE_UI(idFileSave, MainFrame::OnFileMenuUpdateUI)
    EVT_UPDATE_UI(idFileSaveAs, MainFrame::OnFileMenuUpdateUI)
    EVT_UPDATE_UI(idFileSaveAllFiles, MainFrame::OnFileMenuUpdateUI)
    EVT_UPDATE_UI(idFileSaveWorkspace, MainFrame::OnFileMenuUpdateUI)
    EVT_UPDATE_UI(idFileSaveWorkspaceAs, MainFrame::OnFileMenuUpdateUI)
    EVT_UPDATE_UI(idFileClose, MainFrame::OnFileMenuUpdateUI)
    EVT_UPDATE_UI(idFileCloseAll, MainFrame::OnFileMenuUpdateUI)
    EVT_UPDATE_UI(idFilePrintSetup, MainFrame::OnFileMenuUpdateUI)
    EVT_UPDATE_UI(idFilePrint, MainFrame::OnFileMenuUpdateUI)
    EVT_UPDATE_UI(idProjectSaveProject, MainFrame::OnProjectMenuUpdateUI)
    EVT_UPDATE_UI(idProjectSaveProjectAs, MainFrame::OnProjectMenuUpdateUI)
    EVT_UPDATE_UI(idProjectSaveAllProjects, MainFrame::OnProjectMenuUpdateUI)
    EVT_UPDATE_UI(idProjectSaveTemplate, MainFrame::OnProjectMenuUpdateUI)
    EVT_UPDATE_UI(idProjectCloseProject, MainFrame::OnProjectMenuUpdateUI)
    EVT_UPDATE_UI(idProjectCloseAllProjects, MainFrame::OnProjectMenuUpdateUI)

    EVT_UPDATE_UI(idEditUndo, MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditRedo, MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditCopy, MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditCut, MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditPaste, MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditSwapHeaderSource, MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditFoldAll, MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditUnfoldAll, MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditToggleAllFolds, MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditFoldBlock, MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditUnfoldBlock, MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditToggleFoldBlock, MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditEOLCRLF, MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditEOLCR, MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditEOLLF, MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditSelectAll, MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditBookmarksToggle, MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditBookmarksNext, MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditBookmarksPrevious, MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditCommentSelected, MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditAutoComplete, MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditUncommentSelected, MainFrame::OnEditMenuUpdateUI)
    EVT_UPDATE_UI(idEditToggleCommentSelected, MainFrame::OnEditMenuUpdateUI)

    EVT_UPDATE_UI(idSearchFind, MainFrame::OnSearchMenuUpdateUI)
    EVT_UPDATE_UI(idSearchFindNext, MainFrame::OnSearchMenuUpdateUI)
    EVT_UPDATE_UI(idSearchFindPrevious, MainFrame::OnSearchMenuUpdateUI)
    EVT_UPDATE_UI(idSearchReplace, MainFrame::OnSearchMenuUpdateUI)
    EVT_UPDATE_UI(idSearchGotoLine, MainFrame::OnSearchMenuUpdateUI)

    EVT_UPDATE_UI(idViewToolMain, MainFrame::OnViewMenuUpdateUI)
    EVT_UPDATE_UI(idViewMessageManager, MainFrame::OnViewMenuUpdateUI)
    EVT_UPDATE_UI(idViewManager, MainFrame::OnViewMenuUpdateUI)
    EVT_UPDATE_UI(idViewStatusbar, MainFrame::OnViewMenuUpdateUI)
    EVT_UPDATE_UI(idViewFocusEditor, MainFrame::OnViewMenuUpdateUI)
    EVT_UPDATE_UI(idViewFullScreen, MainFrame::OnViewMenuUpdateUI)

    EVT_EDITOR_UPDATE_UI(MainFrame::OnEditorUpdateUI)

    EVT_PLUGIN_ATTACHED(MainFrame::OnPluginLoaded)
    // EVT_PLUGIN_RELEASED(MainFrame::OnPluginUnloaded)

    EVT_MENU(idFileNew, MainFrame::OnFileNewEmpty)
    EVT_MENU(idFileOpen,  MainFrame::OnFileOpen)
    EVT_MENU(idFileOpenRecentClearHistory, MainFrame::OnFileOpenRecentClearHistory)
    EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, MainFrame::OnFileReopen)
    EVT_MENU(idFileSave,  MainFrame::OnFileSave)
    EVT_MENU(idFileSaveAs,  MainFrame::OnFileSaveAs)
    EVT_MENU(idFileSaveAllFiles,  MainFrame::OnFileSaveAllFiles)
    EVT_MENU(idFileSaveWorkspace,  MainFrame::OnFileSaveWorkspace)
    EVT_MENU(idFileSaveWorkspaceAs,  MainFrame::OnFileSaveWorkspaceAs)
    EVT_BUTTON(ID_EditorManagerCloseButton,MainFrame::OnFileClose)
    EVT_MENU(idFileClose,  MainFrame::OnFileClose)
    EVT_MENU(idFileCloseAll,  MainFrame::OnFileCloseAll)
    EVT_MENU(idFilePrintSetup,  MainFrame::OnFilePrintSetup)
    EVT_MENU(idFilePrint,  MainFrame::OnFilePrint)
    EVT_MENU(idFileExit,  MainFrame::OnFileQuit)
    EVT_MENU(idFileNext,  MainFrame::OnFileNext)
    EVT_MENU(idFilePrev,  MainFrame::OnFilePrev)

    EVT_MENU(idEditUndo,  MainFrame::OnEditUndo)
    EVT_MENU(idEditRedo,  MainFrame::OnEditRedo)
    EVT_MENU(idEditCopy,  MainFrame::OnEditCopy)
    EVT_MENU(idEditCut,  MainFrame::OnEditCut)
    EVT_MENU(idEditPaste,  MainFrame::OnEditPaste)
    EVT_MENU(idEditSwapHeaderSource,  MainFrame::OnEditSwapHeaderSource)
    EVT_MENU(idEditFoldAll,  MainFrame::OnEditFoldAll)
    EVT_MENU(idEditUnfoldAll,  MainFrame::OnEditUnfoldAll)
    EVT_MENU(idEditToggleAllFolds,  MainFrame::OnEditToggleAllFolds)
    EVT_MENU(idEditFoldBlock,  MainFrame::OnEditFoldBlock)
    EVT_MENU(idEditUnfoldBlock,  MainFrame::OnEditUnfoldBlock)
    EVT_MENU(idEditToggleFoldBlock,  MainFrame::OnEditToggleFoldBlock)
    EVT_MENU(idEditEOLCRLF,  MainFrame::OnEditEOLMode)
    EVT_MENU(idEditEOLCR,  MainFrame::OnEditEOLMode)
    EVT_MENU(idEditEOLLF,  MainFrame::OnEditEOLMode)
    EVT_MENU(idEditSelectAll,  MainFrame::OnEditSelectAll)
    EVT_MENU(idEditBookmarksToggle,  MainFrame::OnEditBookmarksToggle)
    EVT_MENU(idEditBookmarksNext,  MainFrame::OnEditBookmarksNext)
    EVT_MENU(idEditBookmarksPrevious,  MainFrame::OnEditBookmarksPrevious)
    EVT_MENU(idEditCommentSelected, MainFrame::OnEditCommentSelected)
    EVT_MENU(idEditAutoComplete, MainFrame::OnEditAutoComplete)
    EVT_MENU(idEditUncommentSelected, MainFrame::OnEditUncommentSelected)
    EVT_MENU(idEditToggleCommentSelected, MainFrame::OnEditToggleCommentSelected)

    EVT_MENU(idSearchFind,  MainFrame::OnSearchFind)
    EVT_MENU(idSearchFindNext,  MainFrame::OnSearchFindNext)
    EVT_MENU(idSearchFindPrevious,  MainFrame::OnSearchFindNext)
    EVT_MENU(idSearchReplace,  MainFrame::OnSearchReplace)
    EVT_MENU(idSearchGotoLine,  MainFrame::OnSearchGotoLine)

    EVT_MENU(idViewToolMain, MainFrame::OnToggleBar)
    EVT_MENU(idViewMessageManager, MainFrame::OnToggleBar)
    EVT_MENU(idViewManager, MainFrame::OnToggleBar)
    EVT_MENU(idViewOpenFilesTree, MainFrame::OnToggleOpenFilesTree)
    EVT_MENU(idViewStatusbar, MainFrame::OnToggleStatusBar)
    EVT_MENU(idViewFocusEditor, MainFrame::OnFocusEditor)
    EVT_MENU(idViewFullScreen, MainFrame::OnToggleFullScreen)

    EVT_MENU(idProjectNewEmptyProject, MainFrame::OnProjectNewEmpty)
    EVT_MENU(idProjectNew, MainFrame::OnProjectNew)
    EVT_MENU(idProjectOpen,  MainFrame::OnProjectOpen)
    EVT_MENU(idProjectSaveProject,  MainFrame::OnProjectSaveProject)
    EVT_MENU(idProjectSaveProjectAs,  MainFrame::OnProjectSaveProjectAs)
    EVT_MENU(idProjectSaveAllProjects,  MainFrame::OnProjectSaveAllProjects)
    EVT_MENU(idProjectSaveTemplate,  MainFrame::OnProjectSaveTemplate)
    EVT_MENU(idProjectCloseProject,  MainFrame::OnProjectCloseProject)
    EVT_MENU(idProjectCloseAllProjects,  MainFrame::OnProjectCloseAllProjects)
    EVT_MENU(idProjectImportDevCpp,  MainFrame::OnProjectImportDevCpp)
    EVT_MENU(idProjectImportMSVC,  MainFrame::OnProjectImportMSVC)
    EVT_MENU(idProjectImportMSVCWksp,  MainFrame::OnProjectImportMSVCWksp)
    EVT_MENU(idProjectImportMSVS,  MainFrame::OnProjectImportMSVS)
    EVT_MENU(idProjectImportMSVSWksp,  MainFrame::OnProjectImportMSVSWksp)

	EVT_MENU(idSettingsEnvironment, MainFrame::OnSettingsEnvironment)
	EVT_MENU(idSettingsEditor, MainFrame::OnSettingsEditor)
    EVT_MENU(idPluginsManagePlugins, MainFrame::OnSettingsPlugins)
    EVT_MENU(idSettingsImpExpConfig, MainFrame::OnSettingsImpExpConfig)

    EVT_MENU(wxID_ABOUT, MainFrame::OnHelpAbout)
    EVT_MENU(idHelpTips, MainFrame::OnHelpTips)

	EVT_MENU(idStartHerePageLink, MainFrame::OnStartHereLink)
	EVT_MENU(idStartHerePageVarSubst, MainFrame::OnStartHereVarSubst)

	EVT_LAYOUT_CHANGED(MainFrame::OnLayoutChanged)

	EVT_PROJECT_ACTIVATE(MainFrame::OnProjectActivated)
	EVT_PROJECT_OPEN(MainFrame::OnProjectOpened)
	EVT_PROJECT_CLOSE(MainFrame::OnProjectClosed)
	EVT_EDITOR_CLOSE(MainFrame::OnEditorClosed)
	EVT_EDITOR_OPEN(MainFrame::OnEditorOpened)
	EVT_EDITOR_SAVE(MainFrame::OnEditorSaved)

    EVT_NOTEBOOK_PAGE_CHANGED(ID_NBEditorManager, MainFrame::OnPageChanged)

	/// CloseFullScreen event handling
	EVT_BUTTON( idCloseFullScreen, MainFrame::OnToggleFullScreen )

	/// Shift-Tab bug workaround
	EVT_MENU(idShiftTab,MainFrame::OnShiftTab)

END_EVENT_TABLE()

MainFrame::MainFrame(wxLocale& lang, wxWindow* parent)
       : wxFrame(parent, -1, _T("MainWin"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE),
	   pLayoutManager(0),
	   pSlideBar(0),
	   pPane(0),
	   pDockWindow1(0),
	   pDockWindow2(0),
	   m_pAccel(0L),
	   m_locale(lang),
	   m_pCloseFullScreenBtn(0L),
       m_pNotebook(0L),
	   m_pEdMan(0L),
	   m_pPrjMan(0L),
	   m_pMsgMan(0L),
	   m_pToolbar(0L),
       m_ToolsMenu(0L),
       m_SettingsMenu(0L),
       m_HelpPluginsMenu(0L),
       m_ReconfiguringPlugins(false)
{
#if defined( _MSC_VER ) && defined( _DEBUG )
	int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
	//tmpFlag |= _CRTDBG_CHECK_ALWAYS_DF;
	_CrtSetDbgFlag( tmpFlag );
#endif

    // New: Allow drag and drop of files into the editor
    SetDropTarget(new wxMyFileDropTarget(this));

    // Accelerator table
    wxAcceleratorEntry entries[7];

    entries[0].Set(wxACCEL_CTRL | wxACCEL_SHIFT,  (int) 'W', idFileCloseAll);
    entries[1].Set(wxACCEL_CTRL | wxACCEL_SHIFT,  WXK_F4, idFileCloseAll);
    entries[2].Set(wxACCEL_CTRL,  (int) 'W', idFileClose);
    entries[3].Set(wxACCEL_CTRL,  WXK_F4, idFileClose);
    entries[4].Set(wxACCEL_CTRL,  WXK_F6, idFileNext);
    entries[5].Set(wxACCEL_CTRL | wxACCEL_SHIFT,  WXK_F6, idFilePrev);
    entries[6].Set(wxACCEL_SHIFT,  WXK_TAB, idShiftTab);
    m_pAccel = new wxAcceleratorTable(7, entries);

    this->SetAcceleratorTable(*m_pAccel);

    m_SmallToolBar = CFG_READ(_T("/environment/toolbar_size"), 1L) == 1;
	CreateIDE();

#ifdef __WXMSW__
    SetIcon(wxICON(A_MAIN_ICON));
#else
    SetIcon(wxIcon(app));
#endif // __WXMSW__

    DoCreateStatusBar();
#if wxUSE_STATUSBAR
    SetStatusText(_("Welcome to ")+wxString(APP_NAME"!"));
#endif // wxUSE_STATUSBAR

    SetTitle(APP_NAME _T(" v") APP_VERSION);

    ScanForPlugins();
    LoadWindowState();

    InitPrinting();
    ShowHideStartPage();

    ConfigManager::AddConfiguration(_("Application"), _T("/main_frame"));
    ConfigManager::AddConfiguration(_("Environment"), _T("/environment"));
}

MainFrame::~MainFrame()
{
	delete pLayoutManager;

    this->SetAcceleratorTable(wxNullAcceleratorTable);
    delete m_pAccel;

    DeInitPrinting();
	//Manager::Get()->Free();
}

void MainFrame::ShowTips(bool forceShow)
{
    bool showAtStartup = CFG_READ(_T("/show_tips"), 1) != 0;
    if (forceShow || showAtStartup)
    {
        wxLogNull null; // disable error message if tips file does not exist
        wxString tipsFile = CFG_READ(_T("/app_path")) + _T("/tips.txt");
        long tipsIndex = CFG_READ(_T("/next_tip"), (long)0);
        wxTipProvider* tipProvider = wxCreateFileTipProvider(tipsFile, tipsIndex);
        showAtStartup = wxShowTip(this, tipProvider, showAtStartup);
        delete tipProvider;
        CFG_WRITE(_T("/show_tips"), showAtStartup);
        CFG_WRITE(_T("/next_tip"), (long)tipProvider->GetCurrentTip());
    }
}

void MainFrame::CreateIDE()
{
	int leftW = CFG_READ(_T("/main_frame/layout/left_block_width"), 200);
	int bottomH = CFG_READ(_T("/main_frame/layout/bottom_block_height"), 150);
	SetSize(800,600);
	wxSize clientsize = GetClientSize();

	// Create CloseFullScreen Button, and hide it initially
	m_pCloseFullScreenBtn = new wxButton(this, idCloseFullScreen, _( "Close Fullscreen" ), wxDefaultPosition );
	m_pCloseFullScreenBtn->Show( false );

    pSlideBar = new wxSlideBar( this, 0 );
    pPane = new wxPane( this, 0, "Client Pane" );
    pPane->ShowHeader(false);
    pPane->ShowCloseButton( false );
	m_pNotebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, /*wxNB_LEFT | */wxCLIP_CHILDREN/* | wxNB_MULTILINE*/);
	Manager::Get(this, m_pNotebook, 0);//pPane);

    pDockWindow1 = new wxDockWindow( this, 0, "Management", wxPoint( 64, 64 ), wxSize( leftW, clientsize.GetHeight() ) );
    pDockWindow1->SetClient( m_pNotebook );

    pDockWindow2 = new wxDockWindow( this, 0, "Messages", wxPoint( 96, 96 ), wxSize( clientsize.GetWidth(), bottomH ), "d1" );
    pDockWindow2->SetClient( Manager::Get()->GetMessageManager() );

    // setup dockmanager
	pLayoutManager = new wxLayoutManager( this );
    pLayoutManager->AddDefaultHosts();
    pLayoutManager->AddDockWindow( pDockWindow1 );
    pLayoutManager->AddDockWindow( pDockWindow2 );
    pLayoutManager->SetLayout( wxDWF_SPLITTER_BORDERS, pPane );

    // auto-dock some dockwindows
    HostInfo hi;
    hi = pLayoutManager->GetDockHost( wxDEFAULT_LEFT_HOST );
    hi.pHost->SetAreaSize(leftW);
    pLayoutManager->DockWindow( pDockWindow1, hi );
    hi = pLayoutManager->GetDockHost( wxDEFAULT_RIGHT_HOST );
    hi.pHost->SetAreaSize(leftW);
    hi = pLayoutManager->GetDockHost( wxDEFAULT_TOP_HOST );
    hi.pHost->SetAreaSize(bottomH);
    hi = pLayoutManager->GetDockHost( wxDEFAULT_BOTTOM_HOST );
    hi.pHost->SetAreaSize(bottomH);
    pLayoutManager->DockWindow( pDockWindow2, hi );

	CreateMenubar();

	m_pEdMan = Manager::Get()->GetEditorManager();
	m_pPrjMan = Manager::Get()->GetProjectManager();
	m_pMsgMan = Manager::Get()->GetMessageManager();

    m_pMsgMan->SetContainerWindow(pDockWindow2);

    CreateToolbars();
    SetToolBar(0);

    pSlideBar->SetMode( wxSLIDE_MODE_COMPACT );
    pPane->SetClient(m_pEdMan->GetPanel());
}

wxMenu* MainFrame::RecreateMenu(wxMenuBar* mbar, const wxString& name)
{
    wxMenu* menu = 0;
    int idx = mbar->FindMenu(name);
    if (idx != wxNOT_FOUND)
        menu = mbar->GetMenu(idx);

    if (!menu)
    {
        menu = new wxMenu();
        mbar->Append(menu, name);
    }
    else
    {
        while (menu->GetMenuItemCount() > 0)
        {
            menu->Destroy(menu->GetMenuItems()[0]);
        }
    }

    return menu;
}

void MainFrame::CreateMenubar()
{
	int tmpidx;
	wxMenuBar* mbar=0L;
	wxMenu *tools=0L, *plugs=0L, *pluginsM=0L, *settingsPlugins=0L;
	wxMenuItem *tmpitem=0L;

    wxString resPath = CFG_READ(_T("data_path"), wxEmptyString);
    wxXmlResource *myres = wxXmlResource::Get();
    myres->Load(resPath + _T("/resources.zip#zip:main_menu.xrc"));
    mbar = myres->LoadMenuBar(_T("main_menu_bar"));
    if(!mbar)
    {
      mbar = new wxMenuBar(); // Some error happened.
      SetMenuBar(mbar);
    }

    // Find Menus that we'll change later

    tmpidx=mbar->FindMenu(_("&Tools"));
    if(tmpidx!=wxNOT_FOUND)
        tools = mbar->GetMenu(tmpidx);

    tmpidx=mbar->FindMenu(_("P&lugins"));
    if(tmpidx!=wxNOT_FOUND)
        plugs = mbar->GetMenu(tmpidx);

    if((tmpitem = mbar->FindItem(idSettingsConfigurePlugins,NULL)))
        settingsPlugins = tmpitem->GetSubMenu();
    if((tmpitem = mbar->FindItem(idHelpPlugins,NULL)))
        pluginsM = tmpitem->GetSubMenu();

	m_ToolsMenu = tools ? tools : new wxMenu();
	m_PluginsMenu = plugs ? plugs : new wxMenu();
	m_SettingsMenu = settingsPlugins ? settingsPlugins : new wxMenu();
	m_HelpPluginsMenu = pluginsM ? pluginsM : new wxMenu();

	// core modules: create menus
	PRJMAN()->CreateMenu(mbar);
	EDMAN()->CreateMenu(mbar);
	MSGMAN()->CreateMenu(mbar);

	// ask all plugins to rebuild their menus
	PluginElementsArray plugins = Manager::Get()->GetPluginManager()->GetPlugins();
	for (unsigned int i = 0; i < plugins.GetCount(); ++i)
	{
		cbPlugin* plug = plugins[i]->plugin;
		if (plug && plug->IsAttached())
		{
			if (plug->GetType() == ptTool)
			{
                DoAddPlugin(plug);
            }
			else
			{
                AddPluginInSettingsMenu(plug);
                AddPluginInHelpPluginsMenu(plug);
				plug->BuildMenu(mbar);
            }
		}
	}

	Manager::Get()->GetToolsManager()->BuildToolsMenu(m_ToolsMenu);

	SetMenuBar(mbar);
    InitializeRecentFilesHistory();
}

void MainFrame::CreateToolbars()
{
	wxXmlResource *myres = wxXmlResource::Get();
	if (m_pToolbar)
	{
		SetToolBar(0L);
//		delete m_pToolbar;
		m_pToolbar = 0L;
	}

    // *** Begin new Toolbar routine ***
    wxString resPath = CFG_READ(_T("data_path"), wxEmptyString);
    wxString xrcToolbarName = _T("main_toolbar");
    if(m_SmallToolBar) // Insert logic here
        xrcToolbarName += _T("_16x16");
    myres->Load(resPath + _T("/resources.zip#zip:*.xrc"));
    MSGMAN()->DebugLog(_("Loading toolbar..."));
    wxToolBar *mytoolbar = myres->LoadToolBar(this,xrcToolbarName);

    if(mytoolbar==0L)
    {
        MSGMAN()->DebugLog(_("failed!"));
        int flags = wxTB_HORIZONTAL;
        int major;
        int minor;
        // version==wxWINDOWS_NT && major==5 && minor==1 => windowsXP
        bool isXP = wxGetOsVersion(&major, &minor) == wxWINDOWS_NT && major == 5 && minor == 1;
        if (!isXP)
            flags |= wxTB_FLAT;
        mytoolbar = CreateToolBar(flags, wxID_ANY);
        if(m_SmallToolBar)
            mytoolbar->SetToolBitmapSize(wxSize(16, 16));
        else
            mytoolbar->SetToolBitmapSize(wxSize(22, 22));
    }

    m_pToolbar=mytoolbar;
	m_pToolbar->Realize();
    SetToolBar(0);
    // *** End new Toolbar routine ***

    pSlideBar->AddWindow( m_pToolbar, "Main" );

	// ask all plugins to rebuild their toolbars
	PluginElementsArray plugins = Manager::Get()->GetPluginManager()->GetPlugins();
	for (unsigned int i = 0; i < plugins.GetCount(); ++i)
	{
		cbPlugin* plug = plugins[i]->plugin;
		if (plug && plug->IsAttached())
		{
            DoAddPluginToolbar(plug);
		}
	}

	wxSafeYield();
}

void MainFrame::AddToolbarItem(int id, const wxString& title, const wxString& shortHelp, const wxString& longHelp, const wxString& image)
{
	m_pToolbar->AddTool(id, title, wxBitmap(image, wxBITMAP_TYPE_PNG));
	m_pToolbar->SetToolShortHelp(id, shortHelp);
	m_pToolbar->SetToolLongHelp(id, longHelp);
}

void MainFrame::ScanForPlugins()
{
    m_PluginIDsMap.clear();

    PluginManager* m_PluginManager = Manager::Get()->GetPluginManager();

    wxString path = CFG_READ(_T("data_path")) + _T("/plugins");
    MSGMAN()->Log(_("Scanning for plugins in %s..."), path.c_str());
    int count = m_PluginManager->ScanForPlugins(path);
    MSGMAN()->AppendLog(_("Found %d plugins: "), count);

    // actually load plugins
    m_PluginManager->LoadAllPlugins();
//    MSGMAN()->DebugLog(_("%d plugins loaded"), count);
}

void MainFrame::AddPluginInMenus(wxMenu* menu, cbPlugin* plugin, wxObjectEventFunction callback, int pos)
{
    if (!plugin || !menu)
		return;

    PluginIDsMap::iterator it;
    for (it = m_PluginIDsMap.begin(); it != m_PluginIDsMap.end(); ++it)
    {
        if (it->second == plugin->GetInfo()->name)
        {
            if (menu->FindItem(it->first) != 0)
                return;
        }
    }

    int id = wxNewId();
    m_PluginIDsMap[id] = plugin->GetInfo()->name;
    if (pos == -1)
        menu->Append(id, plugin->GetInfo()->title);
    else
        menu->Insert(pos, id, plugin->GetInfo()->title);
    Connect( id,  wxEVT_COMMAND_MENU_SELECTED, callback );
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

void MainFrame::AddPluginInSettingsMenu(cbPlugin* plugin)
{
    if(!plugin)
        return;
    if (!plugin->GetInfo()->hasConfigure)
        return;
    AddPluginInMenus(m_SettingsMenu, plugin,
                    (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&MainFrame::OnPluginSettingsMenu);
}

void MainFrame::AddPluginInHelpPluginsMenu(cbPlugin* plugin)
{
    AddPluginInMenus(m_HelpPluginsMenu, plugin,
                    (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&MainFrame::OnHelpPluginMenu);
}

void MainFrame::RemovePluginFromMenus(const wxString& pluginName)
{
    //MSGMAN()->DebugLog("Unloading %s plugin", pluginName.c_str());
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
    //MSGMAN()->DebugLog("id=%d", id);
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
		m_SettingsMenu->Delete(id[i]);
	}
}

void MainFrame::LoadWindowState()
{
	wxLogNull ln; // no logging needed

    const wxString& personalityKey = Manager::Get()->GetPersonalityManager()->GetPersonalityKey();

    wxString path = wxGetUserHome();
    path << _T("/") << personalityKey;
    path << _T(".cb_layout.bin");
    wxFileInputStream fi( path );
    if (fi.Ok())
    {
        wxUtil::ReadWindowLayout( fi, this );
        pLayoutManager->LoadFromStream( fi );
        pSlideBar->LoadFromStream( fi );
    }

    // toolbar visibility
	if (m_pToolbar)
        m_pToolbar->Show(CFG_READ(personalityKey + _T("/main_frame/layout/toolbar_show"), 1));

	// load manager and messages selected page
	Manager::Get()->GetNotebook()->SetSelection(CFG_READ(personalityKey + _T("/main_frame/layout/left_block_selection"), 0L));
	MSGMAN()->SetSelection(CFG_READ(personalityKey + _T("/main_frame/layout/bottom_block_selection"), 0L));

    // close message manager (if auto-hiding)
    MSGMAN()->Close();
}

void MainFrame::SaveWindowState()
{
	wxLogNull ln; // no logging needed

    const wxString& personalityKey = Manager::Get()->GetPersonalityManager()->GetPersonalityKey();

    wxString path = wxGetUserHome();
    path << _T("/") << personalityKey;
    path << _T(".cb_layout.bin");
    wxFileOutputStream fo( path );
    wxUtil::WriteWindowLayout( fo, this );
    pLayoutManager->SaveToStream( fo );
    pSlideBar->SaveToStream( fo );

	// save manager and messages selected page
	CFG_WRITE(personalityKey + _T("/main_frame/layout/left_block_selection"), Manager::Get()->GetNotebook()->GetSelection());
	CFG_WRITE(personalityKey + _T("/main_frame/layout/bottom_block_selection"), MSGMAN()->GetSelection());
}

void MainFrame::DoAddPluginToolbar(cbPlugin* plugin)
{
    wxToolBar* tb = new wxToolBar(this, 0);
    tb->SetToolBitmapSize(m_SmallToolBar ? wxSize(16, 16) : wxSize(22, 22));
    if (plugin->BuildToolBar(tb))
    {
        SetToolBar(0);
        pSlideBar->AddWindow( tb, plugin->GetInfo()->name, wxBF_EXPAND_X );
    }
    else
        delete tb;
}

void MainFrame::DoAddPlugin(cbPlugin* plugin)
{
    //MSGMAN()->DebugLog(_("Adding plugin: %s"), plugin->GetInfo()->name.c_str());
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
        plugin->BuildMenu(GetMenuBar());
        // toolbar
        DoAddPluginToolbar(plugin);
    }
}

bool MainFrame::Open(const wxString& filename, bool addToHistory)
{
    wxFileName fn(filename);
    fn.Normalize(); // really important so that two same files with different names are not loaded twice
    wxString name = fn.GetFullPath();
    //MSGMAN()->DebugLog(_("Opening file '%s'"), sname.c_str());
    MSGMAN()->DebugLog(_("Opening file %s"), name.c_str());
    bool ret = OpenGeneric(name, addToHistory);
	return ret;
}

wxString MainFrame::ShowOpenFileDialog(const wxString& caption, const wxString& filter)
{
    wxFileDialog* dlg = new wxFileDialog(this,
                            caption,
                            wxEmptyString,
                            wxEmptyString,
                            filter,
                            wxOPEN);
    wxString sel;
    if (dlg->ShowModal() == wxID_OK)
		sel = dlg->GetPath();
    delete dlg;
    return sel;
}

bool MainFrame::OpenGeneric(const wxString& filename, bool addToHistory)
{
    if (filename.IsEmpty())
        return false;
    switch(FileTypeOf(filename))
    {
        //
        // Workspaces
        //
        case ftCodeBlocksWorkspace:
            // fallthrough
        case ftMSVCWorkspace:
            // fallthrough
        case ftMSVSWorkspace:
            if (DoCloseCurrentWorkspace())
            {
                PRJMAN()->LoadWorkspace(filename);
                m_FilesHistory.AddFileToHistory(filename);
            }
            else
                return false;
            break;

        //
        // Projects
        //
        case ftCodeBlocksProject:
            // fallthrough
        case ftDevCppProject:
            // fallthrough
        case ftMSVCProject:
            // fallthrough
        case ftMSVSProject:
            return DoOpenProject(filename, addToHistory);

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
            return plugin && plugin->OpenFile(filename) == 0;
        }
    }
    return true;
}

bool MainFrame::DoOpenProject(const wxString& filename, bool addToHistory)
{
//    MSGMAN()->DebugLog(_("Opening project '%s'"), filename.c_str());
    cbProject* prj = PRJMAN()->LoadProject(filename);
    if (prj)
    {
		if (addToHistory)
			m_FilesHistory.AddFileToHistory(prj->GetFilename());
        return true;
    }
    return false;
}

bool MainFrame::DoOpenFile(const wxString& filename, bool addToHistory)
{
    if (EDMAN()->Open(filename))
    {
		if (addToHistory)
			m_FilesHistory.AddFileToHistory(filename);
        return true;
    }
    return false;
}

bool MainFrame::DoCloseCurrentWorkspace()
{
    return PRJMAN()->CloseWorkspace();
}

void MainFrame::DoCreateStatusBar()
{
#if wxUSE_STATUSBAR
#define COUNT 5
    CreateStatusBar(COUNT);
    int statusWidths[COUNT] = {-1, 148, 64, 64, 96};
    SetStatusWidths(COUNT, statusWidths);
#undef COUNT
#endif // wxUSE_STATUSBAR
}

void MainFrame::DoUpdateStatusBar()
{
#if wxUSE_STATUSBAR
    if (!GetStatusBar())
        return;
    cbEditor* ed = EDMAN()->GetBuiltinActiveEditor();
    if (ed)
    {
        int pos = ed->GetControl()->GetCurrentPos();
        wxString msg;
        msg.Printf(_("Line %d, Column %d"), ed->GetControl()->GetCurrentLine() + 1, ed->GetControl()->GetColumn(pos) + 1);
        SetStatusText(msg, 1);
        SetStatusText(ed->GetControl()->GetOvertype() ? _("Overwrite") : _("Insert"), 2);
        SetStatusText(ed->GetModified() ? _("Modified") : wxEmptyString, 3);
        SetStatusText(ed->GetControl()->GetReadOnly() ? _("Read only") : _("Read/Write"), 4);
    }
    else
    {
        SetStatusText(wxEmptyString, 1);
        SetStatusText(wxEmptyString, 2);
        SetStatusText(wxEmptyString, 3);
        SetStatusText(wxEmptyString, 4);
    }
#endif // wxUSE_STATUSBAR
}

void MainFrame::DoUpdateAppTitle()
{
	EditorBase* ed = EDMAN() ? EDMAN()->GetActiveEditor() : 0L;
	cbProject* prj = 0;
	if(ed && ed->IsBuiltinEditor())
	{
	    ProjectFile* prjf = ((cbEditor*)ed)->GetProjectFile();
	    if(prjf)
            prj = prjf->project;
	}
	else
        prj = PRJMAN() ? PRJMAN()->GetActiveProject() : 0L;
	wxString projname=_T(""),edname=_T(""),fulltitle=_T("");
	if(ed || prj)
	{
        if(prj)
        {
            if(PRJMAN()->GetActiveProject() == prj)
                projname = wxString(_T("[")) + prj->GetTitle() + _T("] ");
            else
                projname = wxString(_T("(")) + prj->GetTitle() + _T(") ");
        }
        if(ed)
            edname = ed->GetTitle();
        fulltitle = projname + edname;
        if(!fulltitle.IsEmpty())
            fulltitle.Append(_T(" - "));
        fulltitle.Append(APP_NAME);
	}
	else
        fulltitle = APP_NAME" v"APP_VERSION;
    SetTitle(fulltitle);
}

void MainFrame::ShowHideStartPage(bool forceHasProject)
{
    // we use the 'forceHasProject' param because when a project is opened
    // the EVT_PROJECT_OPEN event is fired *before* ProjectManager::GetProjects()
    // and ProjectManager::GetActiveProject() are updated...
    if(Manager::isappShuttingDown())
        return;
    bool show = !forceHasProject &&
                PRJMAN()->GetProjects()->GetCount() == 0 &&
                CFG_READ(_T("/environment/start_here_page"), 1);

    EditorBase* sh = EDMAN()->GetEditor(g_StartHereTitle);
    if (show && !sh)
        sh = new StartHerePage(this, EDMAN()->GetNotebook());
    else if (!show && sh)
        sh->Destroy();
}

void MainFrame::OnStartHereLink(wxCommandEvent& event)
{
    wxCommandEvent evt;
    wxString link = event.GetString();
    if (link.Matches(_T("CB_CMD_NEW_PROJECT")))
        TemplateManager::Get()->NewProject();
    else if (link.Matches(_T("CB_CMD_OPEN_PROJECT")))
        OnFileOpen(evt);
    else if (link.Matches(_T("CB_CMD_CONF_ENVIRONMENT")))
        OnSettingsEnvironment(evt);
    else if (link.Matches(_T("CB_CMD_CONF_EDITOR")))
        Manager::Get()->GetEditorManager()->Configure();
    else if (link.Matches(_T("CB_CMD_CONF_COMPILER")))
    {
        PluginsArray arr = Manager::Get()->GetPluginManager()->GetCompilerOffers();
        if (arr.GetCount() != 0)
            arr[0]->Configure();
    }
    else if (link.StartsWith(_T("CB_CMD_OPEN_HISTORY_")))
    {
    	// history file
// NOTE (mandrav#1#): This thing works for up to 9 history files.
//                    The good thing is that the current start here page
//                    displays only 5.
//                    Things could be done better though...
    	wxChar num = link.Last();
        for (int i = 0; i < (int)m_FilesHistory.GetCount(); ++i)
        {
        	if (num - _T('1') == i)
        	{
        		OpenGeneric(m_FilesHistory.GetHistoryFile(i), true);
        		break;
        	}
        }
    }
}

void MainFrame::OnStartHereVarSubst(wxCommandEvent& event)
{
	wxString buf = event.GetString();

	// replace history vars
	for (int i = 0; i < 9; ++i)
	{
		wxString base;
		base.Printf(_T("CB_VAR_HISTORY_FILE_%d"), i + 1);
		if (i < (int)m_FilesHistory.GetCount())
            buf.Replace(base, m_FilesHistory.GetHistoryFile(i));
        else
            buf.Replace(base, _T(""));
	}

    // update page
    EditorBase* sh = EDMAN()->GetEditor(g_StartHereTitle);
	if (sh)
        ((StartHerePage*)sh)->SetPageContent(buf);
}

void MainFrame::InitializeRecentFilesHistory()
{
    wxMenuBar* mbar = GetMenuBar();
    if (!mbar)
        return;
    int pos = mbar->FindMenu(_("&File"));
    if (pos != wxNOT_FOUND)
    {
        wxMenu* menu = mbar->GetMenu(pos);
        if (!menu)
            return;
        wxMenu* recentFiles = 0;
        menu->FindItem(idFileOpenRecentClearHistory, &recentFiles);
        if (recentFiles)
        {
            m_FilesHistory.UseMenu(recentFiles);
            ConfigManager::Get()->SetPath(_T("/recent_files"));
            m_FilesHistory.Load(*ConfigManager::Get());
            ConfigManager::Get()->SetPath(_T("/"));
        }
    }
}

void MainFrame::TerminateRecentFilesHistory()
{
    ConfigManager::Get()->SetPath(_T("/recent_files"));
    m_FilesHistory.Save(*ConfigManager::Get());
    ConfigManager::Get()->SetPath(_T("/"));

    wxMenuBar* mbar = GetMenuBar();
    if (!mbar)
        return;
    int pos = mbar->FindMenu(_("&File"));
    if (pos != wxNOT_FOUND)
    {
        wxMenu* menu = mbar->GetMenu(pos);
        if (!menu)
            return;
        wxMenu* recentFiles = 0;
        menu->FindItem(idFileOpenRecentClearHistory, &recentFiles);
        if (recentFiles)
            m_FilesHistory.RemoveMenu(recentFiles);
    }
}

// event handlers

void MainFrame::OnPluginsExecuteMenu(wxCommandEvent& event)
{
    wxString pluginName = m_PluginIDsMap[event.GetId()];
    if (!pluginName.IsEmpty())
        Manager::Get()->GetPluginManager()->ExecutePlugin(pluginName);
    else
        MSGMAN()->DebugLog(_("No plugin found for ID %d"), event.GetId());
}

void MainFrame::OnPluginSettingsMenu(wxCommandEvent& event)
{
    wxString pluginName = m_PluginIDsMap[event.GetId()];
    if (!pluginName.IsEmpty())
        Manager::Get()->GetPluginManager()->ConfigurePlugin(pluginName);
    else
        MSGMAN()->DebugLog(_("No plugin found for ID %d"), event.GetId());
}

void MainFrame::OnHelpPluginMenu(wxCommandEvent& event)
{
    wxString pluginName = m_PluginIDsMap[event.GetId()];
    if (!pluginName.IsEmpty())
    {
        const PluginInfo* pi = Manager::Get()->GetPluginManager()->GetPluginInfo(pluginName);
        if (!pi)
        {
            MSGMAN()->DebugLog(_("No plugin info for %s!"), pluginName.c_str());
            return;
        }
        dlgAboutPlugin* dlg = new dlgAboutPlugin(this, pi);
        dlg->ShowModal();
        delete dlg;
    }
    else
        MSGMAN()->DebugLog(_("No plugin found for ID %d"), event.GetId());
}

void MainFrame::OnFileNewEmpty(wxCommandEvent& event)
{
	cbProject* project = PRJMAN()->GetActiveProject();
	if (project)
        wxSetWorkingDirectory(project->GetBasePath());
    cbEditor* ed = EDMAN()->New();

	if (!ed || !project)
		return;

	if (wxMessageBox(_("Do you want to add this new file in the active project?"),
					_("Add file to project"),
					wxYES_NO | wxICON_QUESTION) == wxYES)
	{
        wxArrayInt targets;
		if (PRJMAN()->AddFileToProject(ed->GetFilename(), project, targets) != 0)
		{
            ProjectFile* pf = project->GetFileByFilename(ed->GetFilename(), false);
			ed->SetProjectFile(pf);
			PRJMAN()->RebuildTree();
		}
	}
	// verify that the open files are still in sync
	// the new file might have overwritten an existing one)
	EDMAN()->CheckForExternallyModifiedFiles();
}

bool MainFrame::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& files)
{
    // first check to see if a workspace is passed. If so, only this will be loaded
    wxString foundWorkspace;
    for (unsigned int i = 0; i < files.GetCount(); ++i)
    {
        FileType ft = FileTypeOf(files[i]);
        if (ft == ftCodeBlocksWorkspace || ft == ftMSVCWorkspace || ft == ftMSVSWorkspace)
        {
            foundWorkspace = files[i];
            break;
        }
    }

    if (!foundWorkspace.IsEmpty())
        OpenGeneric(foundWorkspace);
    else
    {
        for (unsigned int i = 0; i < files.GetCount(); ++i)
        {
            OpenGeneric(files[i]);
        }
    }
    return true;
}

void MainFrame::OnFileOpen(wxCommandEvent& event)
{
    wxFileDialog* dlg = new wxFileDialog(this,
                            _("Open file"),
                            wxEmptyString,
                            wxEmptyString,
                            SOURCE_FILES_DIALOG_FILTER,
                            wxOPEN | wxMULTIPLE);
    dlg->SetFilterIndex(SOURCE_FILES_FILTER_INDEX);

    if (dlg->ShowModal() == wxID_OK)
    {
        wxArrayString files;
        dlg->GetPaths(files);
        OnDropFiles(0,0,files);
    }

    delete dlg;
}

void MainFrame::OnFileReopen(wxCommandEvent& event)
{
    wxString fname = m_FilesHistory.GetHistoryFile(event.GetId() - wxID_FILE1);
    Open(fname, true);
}

void MainFrame::OnFileOpenRecentClearHistory(wxCommandEvent& event)
{
    while (m_FilesHistory.GetCount())
	{
        m_FilesHistory.RemoveFileFromHistory(0);
		ConfigManager::Get()->DeleteGroup(_T("/recent_files"));
	}
}

void MainFrame::OnFileSave(wxCommandEvent& event)
{
    if (!EDMAN()->SaveActive())
    {
        wxString msg;
        msg.Printf(_("File %s could not be saved..."), EDMAN()->GetActiveEditor()->GetFilename().c_str());
        wxMessageBox(msg, _("Error saving file"));
    }
    DoUpdateStatusBar();
}

void MainFrame::OnFileSaveAs(wxCommandEvent& event)
{
    if (!EDMAN()->SaveActiveAs())
    {
        wxString msg;
        msg.Printf(_("File %s could not be saved..."), EDMAN()->GetActiveEditor()->GetFilename().c_str());
        wxMessageBox(msg, _("Error saving file"));
    }
    DoUpdateStatusBar();
}

void MainFrame::OnFileSaveAllFiles(wxCommandEvent& event)
{
    EDMAN()->SaveAll();
    DoUpdateStatusBar();
}

void MainFrame::OnFileSaveWorkspace(wxCommandEvent& event)
{
    if (PRJMAN()->SaveWorkspace())
        m_FilesHistory.AddFileToHistory(PRJMAN()->GetWorkspace()->GetFilename());
}

void MainFrame::OnFileSaveWorkspaceAs(wxCommandEvent& event)
{
    if (PRJMAN()->SaveWorkspaceAs(_T("")))
        m_FilesHistory.AddFileToHistory(PRJMAN()->GetWorkspace()->GetFilename());
}

void MainFrame::OnFileClose(wxCommandEvent& WXUNUSED(event))
{
    EDMAN()->CloseActive();
    DoUpdateStatusBar();
    Refresh();
}

void MainFrame::OnFileCloseAll(wxCommandEvent& WXUNUSED(event))
{
    EDMAN()->CloseAll();
    DoUpdateStatusBar();
}

void MainFrame::OnFileNext(wxCommandEvent& event)
{
    EDMAN()->ActivateNext();
    DoUpdateStatusBar();
}

void MainFrame::OnFilePrev(wxCommandEvent& event)
{
    EDMAN()->ActivatePrevious();
    DoUpdateStatusBar();
}

void MainFrame::OnFilePrintSetup(wxCommandEvent& event)
{
    wxPrintDialog dlg(this, g_printData);
    if (dlg.ShowModal() == wxID_OK)
        *g_printData = dlg.GetPrintDialogData().GetPrintData();
}

void MainFrame::OnFilePrint(wxCommandEvent& event)
{
    PrintDialog dlg(this);
    if (dlg.ShowModal() == wxID_OK)
        EDMAN()->Print(dlg.GetPrintScope(), dlg.GetPrintColorMode());
}

void MainFrame::OnFileQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}

void MainFrame::OnApplicationClose(wxCloseEvent& event)
{
    if (!ProjectManager::CanShutdown() || !EditorManager::CanShutdown())
    {
        event.Veto();
        wxBell();
        return;
    }

    if (!DoCloseCurrentWorkspace())
    {
        event.Veto();
        return;
    }

    SaveWindowState();
    TerminateRecentFilesHistory();

// NOTE (mandrav#1#): The following two lines, make the app crash on exit with wx2.6.1-ansi...
//    Hide(); // Hide the window
//    Refresh();

    // unhook editor manager's notebook from the layout, or else bad things happen ;)
    pPane->SetClient(0);

    // remove all other event handlers from this window
    // this stops it from crashing, when no plugins are loaded
    while (GetEventHandler() != this)
        PopEventHandler(false);

	Manager::Get()->Free();
	ConfigManager::Get()->Flush();
    Destroy();
}

void MainFrame::OnEditSwapHeaderSource(wxCommandEvent& event)
{
    EDMAN()->SwapActiveHeaderSource();
    DoUpdateStatusBar();
}

void MainFrame::OnEditBookmarksToggle(wxCommandEvent& event)
{
    cbEditor* ed = EDMAN()->GetBuiltinActiveEditor();
    if (ed)
		ed->MarkerToggle(BOOKMARK_MARKER);
}

void MainFrame::OnEditBookmarksNext(wxCommandEvent& event)
{
    cbEditor* ed = EDMAN()->GetBuiltinActiveEditor();
    if (ed)
		ed->MarkerNext(BOOKMARK_MARKER);
}

void MainFrame::OnEditBookmarksPrevious(wxCommandEvent& event)
{
    cbEditor* ed = EDMAN()->GetBuiltinActiveEditor();
    if (ed)
		ed->MarkerPrevious(BOOKMARK_MARKER);
}

void MainFrame::OnEditUndo(wxCommandEvent& event)
{
    cbEditor* ed = EDMAN()->GetBuiltinActiveEditor();
    if (ed)
        ed->GetControl()->Undo();
}

void MainFrame::OnEditRedo(wxCommandEvent& event)
{
    cbEditor* ed = EDMAN()->GetBuiltinActiveEditor();
    if (ed)
        ed->GetControl()->Redo();
}

void MainFrame::OnEditCopy(wxCommandEvent& event)
{
    cbEditor* ed = EDMAN()->GetBuiltinActiveEditor();
    if (ed)
        ed->GetControl()->Copy();
}

void MainFrame::OnEditCut(wxCommandEvent& event)
{
    cbEditor* ed = EDMAN()->GetBuiltinActiveEditor();
    if (ed)
        ed->GetControl()->Cut();
}

void MainFrame::OnEditPaste(wxCommandEvent& event)
{
    cbEditor* ed = EDMAN()->GetBuiltinActiveEditor();
    if (ed)
        ed->GetControl()->Paste();
}

void MainFrame::OnEditSelectAll(wxCommandEvent& event)
{
    cbEditor* ed = EDMAN()->GetBuiltinActiveEditor();
    if (ed)
        ed->GetControl()->SelectAll();
}

/* This is a shameless rip-off of the original OnEditCommentSelected function,
 * now more suitingly named OnEditToggleCommentSelected (because that's what
 * it does :)
 */
void MainFrame::OnEditCommentSelected(wxCommandEvent& event)
{
    cbEditor* ed = EDMAN()->GetBuiltinActiveEditor();
	if( ed )
	{
        ed->GetControl()->BeginUndoAction();
		cbStyledTextCtrl *stc = ed->GetControl();
		if( wxSCI_INVALID_POSITION != stc->GetSelectionStart() )
		{
			int startLine = stc->LineFromPosition( stc->GetSelectionStart() );
			int endLine   = stc->LineFromPosition( stc->GetSelectionEnd() );
			wxString strLine, str;

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

			while( startLine <= endLine )
			{
				// For each line: comment.
				strLine = stc->GetLine( startLine );

                // Comment
                /// @todo This should be language-dependent. We're currently assuming C++
                stc->InsertText( stc->PositionFromLine( startLine ), _T( "//" ) );

				startLine++;
			}
		}
		ed->GetControl()->EndUndoAction();
	}
}

/* See above (OnEditCommentSelected) for details. */
void MainFrame::OnEditUncommentSelected(wxCommandEvent& event)
{
    cbEditor* ed = EDMAN()->GetBuiltinActiveEditor();
	if( ed )
	{
        ed->GetControl()->BeginUndoAction();
		cbStyledTextCtrl *stc = ed->GetControl();
		if( wxSCI_INVALID_POSITION != stc->GetSelectionStart() )
		{
			int startLine = stc->LineFromPosition( stc->GetSelectionStart() );
			int endLine   = stc->LineFromPosition( stc->GetSelectionEnd() );
			wxString strLine, str;

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

			while( startLine <= endLine )
			{
				// For each line: if it is commented, uncomment.
				strLine = stc->GetLine( startLine );
				int commentPos = strLine.Strip( wxString::leading ).Find( _T( "//" ) );

				if( commentPos ==0 )
				{
					// Uncomment
					strLine.Replace( _T( "//" ), _T( "" ), false );

					// Update
					int start = stc->PositionFromLine( startLine );
					stc->SetTargetStart( start );
					// The +2 is for the '//' we erased
					stc->SetTargetEnd( start + strLine.Length() + 2 );
					stc->ReplaceTarget( strLine );
				}

				startLine++;
			}
		}
		ed->GetControl()->EndUndoAction();
	}
}

void MainFrame::OnEditToggleCommentSelected(wxCommandEvent& event)
{
    cbEditor* ed = EDMAN()->GetBuiltinActiveEditor();
	if( ed )
	{
        ed->GetControl()->BeginUndoAction();
		cbStyledTextCtrl *stc = ed->GetControl();
		if( wxSCI_INVALID_POSITION != stc->GetSelectionStart() )
		{
			int startLine = stc->LineFromPosition( stc->GetSelectionStart() );
			int endLine   = stc->LineFromPosition( stc->GetSelectionEnd() );
			wxString strLine, str;

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

			while( startLine <= endLine )
			{
				// For each line: If it's commented, uncomment. Otherwise, comment.
				strLine = stc->GetLine( startLine );
				int commentPos = strLine.Strip( wxString::leading ).Find( _T( "//" ) );

				if( -1 == commentPos || commentPos > 0 )
				{
					// Comment
					/// @todo This should be language-dependent. We're currently assuming C++
					stc->InsertText( stc->PositionFromLine( startLine ), _T( "//" ) );
				}
				else
				{
					// Uncomment
					strLine.Replace( _T( "//" ), _T( "" ), false );

					// Update
					int start = stc->PositionFromLine( startLine );
					stc->SetTargetStart( start );
					// The +2 is for the '//' we erased
					stc->SetTargetEnd( start + strLine.Length() + 2 );
					stc->ReplaceTarget( strLine );
				}

				startLine++;
			}
		}
		ed->GetControl()->EndUndoAction();
	}
}

void MainFrame::OnEditAutoComplete(wxCommandEvent& event)
{
    cbEditor* ed = EDMAN()->GetBuiltinActiveEditor();
    if (ed)
		ed->AutoComplete();
}

void MainFrame::OnEditFoldAll(wxCommandEvent& event)
{
    cbEditor* ed = EDMAN()->GetBuiltinActiveEditor();
    if (ed)
		ed->FoldAll();
}

void MainFrame::OnEditUnfoldAll(wxCommandEvent& event)
{
    cbEditor* ed = EDMAN()->GetBuiltinActiveEditor();
    if (ed)
		ed->UnfoldAll();
}

void MainFrame::OnEditToggleAllFolds(wxCommandEvent& event)
{
    cbEditor* ed = EDMAN()->GetBuiltinActiveEditor();
    if (ed)
		ed->ToggleAllFolds();
}

void MainFrame::OnEditFoldBlock(wxCommandEvent& event)
{
    cbEditor* ed = EDMAN()->GetBuiltinActiveEditor();
    if (ed)
		ed->FoldBlockFromLine();
}

void MainFrame::OnEditUnfoldBlock(wxCommandEvent& event)
{
    cbEditor* ed = EDMAN()->GetBuiltinActiveEditor();
    if (ed)
		ed->UnfoldBlockFromLine();
}

void MainFrame::OnEditToggleFoldBlock(wxCommandEvent& event)
{
    cbEditor* ed = EDMAN()->GetBuiltinActiveEditor();
    if (ed)
		ed->ToggleFoldBlockFromLine();
}

void MainFrame::OnEditEOLMode(wxCommandEvent& event)
{
    cbEditor* ed = EDMAN()->GetBuiltinActiveEditor();
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

void MainFrame::OnSearchFind(wxCommandEvent& event)
{
	EDMAN()->ShowFindDialog(false);
}

void MainFrame::OnSearchFindNext(wxCommandEvent& event)
{
	if (event.GetId() == idSearchFindPrevious)
		EDMAN()->FindNext(false);
	else
		EDMAN()->FindNext(true);
}

void MainFrame::OnSearchReplace(wxCommandEvent& event)
{
	EDMAN()->ShowFindDialog(true);
}

void MainFrame::OnSearchGotoLine(wxCommandEvent& event)
{
    cbEditor* ed = EDMAN()->GetBuiltinActiveEditor();
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
    wxString strLine = wxGetTextFromUser( _("Line: "),
								_("Goto line"),
								_T( "" ),
								this );
	long int line = 0;
	strLine.ToLong(&line);
	if ( line > 1 && line <= max )
		ed->GetControl()->GotoPos(ed->GetControl()->PositionFromLine(line - 1));
}

void MainFrame::OnProjectNewEmpty(wxCommandEvent& event)
{
    PRJMAN()->NewProject();
	// verify that the open files are still in sync
	// the new file might have overwritten an existing one)
	EDMAN()->CheckForExternallyModifiedFiles();
}

void MainFrame::OnProjectNew(wxCommandEvent& event)
{
    TemplateManager::Get()->NewProject();
	// verify that the open files are still in sync
	// the new file might have overwritten an existing one)
	EDMAN()->CheckForExternallyModifiedFiles();
}

void MainFrame::OnProjectOpen(wxCommandEvent& event)
{
    wxFileDialog* dlg = new wxFileDialog(this,
                            _("Open project"),
                            wxEmptyString,
                            wxEmptyString,
                            CODEBLOCKS_FILES_FILTER,
                            wxOPEN | wxMULTIPLE);

    if (dlg->ShowModal() == wxID_OK)
    {
        wxArrayString files;
        dlg->GetPaths(files);
        OnDropFiles(0,0,files);
    }

    delete dlg;
}

void MainFrame::OnProjectSaveProject(wxCommandEvent& event)
{
    if (PRJMAN()->SaveActiveProject() ||
        PRJMAN()->SaveActiveProjectAs())
        m_FilesHistory.AddFileToHistory(PRJMAN()->GetActiveProject()->GetFilename());
    DoUpdateStatusBar();
}

void MainFrame::OnProjectSaveProjectAs(wxCommandEvent& event)
{
    if (PRJMAN()->SaveActiveProjectAs())
        m_FilesHistory.AddFileToHistory(PRJMAN()->GetActiveProject()->GetFilename());
    DoUpdateStatusBar();
}

void MainFrame::OnProjectSaveAllProjects(wxCommandEvent& event)
{
    PRJMAN()->SaveAllProjects();
    DoUpdateStatusBar();
}

void MainFrame::OnProjectSaveTemplate(wxCommandEvent& event)
{
    TemplateManager::Get()->SaveUserTemplate(PRJMAN()->GetActiveProject());
}

void MainFrame::OnProjectCloseProject(wxCommandEvent& event)
{
    // we 're not actually shutting down here, but we want to check if the
    // active project is still opening files (still busy)
    if (!ProjectManager::CanShutdown() || !EditorManager::CanShutdown())
    {
        wxBell();
        return;
    }
    PRJMAN()->CloseActiveProject();
    DoUpdateStatusBar();
}

void MainFrame::OnProjectCloseAllProjects(wxCommandEvent& event)
{
    if (!ProjectManager::CanShutdown() || !EditorManager::CanShutdown())
    {
        wxBell();
        return;
    }
    PRJMAN()->CloseWorkspace();
    DoUpdateStatusBar();
}

void MainFrame::OnProjectImportDevCpp(wxCommandEvent& event)
{
    OpenGeneric(ShowOpenFileDialog(_("Import Dev-C++ project"), DEVCPP_FILES_FILTER), false);
}

void MainFrame::OnProjectImportMSVC(wxCommandEvent& event)
{
    OpenGeneric(ShowOpenFileDialog(_("Import MS Visual C++ project"), MSVC_FILES_FILTER), false);
}

void MainFrame::OnProjectImportMSVCWksp(wxCommandEvent& event)
{
    OpenGeneric(ShowOpenFileDialog(_("Import MS Visual C++ workspace"), MSVC_WORKSPACE_FILES_FILTER), false);
}

void MainFrame::OnProjectImportMSVS(wxCommandEvent& event)
{
    OpenGeneric(ShowOpenFileDialog(_("Import MS Visual Studio project"), MSVS_FILES_FILTER), false);
}

void MainFrame::OnProjectImportMSVSWksp(wxCommandEvent& event)
{
    OpenGeneric(ShowOpenFileDialog(_("Import MS Visual Studio solution"), MSVS_WORKSPACE_FILES_FILTER), false);
}

void MainFrame::OnHelpAbout(wxCommandEvent& WXUNUSED(event))
{
    dlgAbout* dlg = new dlgAbout(this);
    dlg->ShowModal();
    delete dlg;
}

void MainFrame::OnHelpTips(wxCommandEvent& event)
{
    ShowTips(true);
}

void MainFrame::OnFileMenuUpdateUI(wxUpdateUIEvent& event)
{
    if(Manager::isappShuttingDown())
    {
        event.Skip();
        return;
    }
    EditorBase* ed = EDMAN() ? EDMAN()->GetActiveEditor() : 0;
    wxMenuBar* mbar = GetMenuBar();

    bool canCloseProject = (ProjectManager::CanShutdown() && EditorManager::CanShutdown());
    mbar->Enable(idProjectCloseProject,canCloseProject);
    mbar->Enable(idFileOpenRecentClearHistory, m_FilesHistory.GetCount());
    mbar->Enable(idFileOpenRecentClearHistory, m_FilesHistory.GetCount());
    mbar->Enable(idFileClose, ed);
    mbar->Enable(idFileCloseAll, ed);
    mbar->Enable(idFileSave, ed && ed->GetModified());
    mbar->Enable(idFileSaveAs, ed);
    mbar->Enable(idFileSaveAllFiles, ed);
    mbar->Enable(idFileSaveWorkspace, PRJMAN() && canCloseProject);
    mbar->Enable(idFileSaveWorkspaceAs, PRJMAN() && canCloseProject);
    mbar->Enable(idFilePrint, EDMAN() && EDMAN()->GetActiveEditor());

	if (m_pToolbar)
	{
		m_pToolbar->EnableTool(idFileSave, ed && ed->GetModified());
	}

	event.Skip();
}

void MainFrame::OnEditMenuUpdateUI(wxUpdateUIEvent& event)
{
    if(Manager::isappShuttingDown())
    {
        event.Skip();
        return;
    }

    cbEditor* ed = NULL;
    bool hasSel = false;
    bool canUndo = false;
    bool canRedo = false;
    bool canPaste = false;
    int eolMode = -1;

    if(EDMAN() && !Manager::isappShuttingDown())
        ed = EDMAN()->GetBuiltinActiveEditor();

    wxMenuBar* mbar = GetMenuBar();

    if(ed)
    {
        eolMode = ed->GetControl()->GetEOLMode();
        canUndo = ed->GetControl()->CanUndo();
        canRedo = ed->GetControl()->CanRedo();
        hasSel = (ed->GetControl()->GetSelectionStart() != ed->GetControl()->GetSelectionEnd());
#ifdef __WXGTK__
        canPaste = true;
#else
        canPaste = ed->GetControl()->CanPaste();
#endif
    }

    mbar->Enable(idEditUndo, ed && canUndo);
    mbar->Enable(idEditRedo, ed && canRedo);
    mbar->Enable(idEditCut, ed && hasSel);
    mbar->Enable(idEditCopy, ed && hasSel);
    mbar->Enable(idEditPaste, ed && canPaste);
    mbar->Enable(idEditSwapHeaderSource, ed);
    mbar->Enable(idEditFoldAll, ed);
    mbar->Enable(idEditUnfoldAll, ed);
    mbar->Enable(idEditToggleAllFolds, ed);
    mbar->Enable(idEditSelectAll, ed);
    mbar->Enable(idEditBookmarksToggle, ed);
    mbar->Enable(idEditBookmarksNext, ed);
    mbar->Enable(idEditBookmarksPrevious, ed);
	mbar->Enable(idEditFoldBlock, ed);
	mbar->Enable(idEditUnfoldBlock, ed);
	mbar->Enable(idEditToggleFoldBlock, ed);
	mbar->Enable(idEditEOLCRLF, ed);
	mbar->Enable(idEditEOLCR, ed);
	mbar->Enable(idEditEOLLF, ed);
	mbar->Check(idEditEOLCRLF, eolMode == wxSCI_EOL_CRLF);
	mbar->Check(idEditEOLCR, eolMode == wxSCI_EOL_CR);
	mbar->Check(idEditEOLLF, eolMode == wxSCI_EOL_LF);
	mbar->Enable(idEditCommentSelected, ed);
	mbar->Enable(idEditAutoComplete, ed);
	mbar->Enable(idEditUncommentSelected, ed);
	mbar->Enable(idEditToggleCommentSelected, ed);

	if (m_pToolbar)
	{
		m_pToolbar->EnableTool(idEditUndo, ed && canUndo);
		m_pToolbar->EnableTool(idEditRedo, ed && canRedo);
		m_pToolbar->EnableTool(idEditCut, ed && hasSel);
		m_pToolbar->EnableTool(idEditCopy, ed && hasSel);
		m_pToolbar->EnableTool(idEditPaste, ed && canPaste);
	}

	event.Skip();
}

void MainFrame::OnViewMenuUpdateUI(wxUpdateUIEvent& event)
{
    if(Manager::isappShuttingDown())
    {
        event.Skip();
        return;
    }
    wxMenuBar* mbar = GetMenuBar();
    cbEditor* ed = EDMAN() ? EDMAN()->GetBuiltinActiveEditor() : 0;
    bool manVis = pDockWindow1->GetDockPanel()->IsDocked() || pDockWindow1->IsShown();

    mbar->Check(idViewToolMain, pSlideBar->IsShown());
    mbar->Check(idViewManager, manVis);
    mbar->Check(idViewOpenFilesTree, m_pEdMan && m_pEdMan->IsOpenFilesTreeVisible());
    mbar->Enable(idViewOpenFilesTree, manVis && m_pEdMan);
    mbar->Check(idViewMessageManager, pDockWindow2->GetDockPanel()->IsDocked() || pDockWindow2->IsShown());
    mbar->Check(idViewStatusbar, GetStatusBar() && GetStatusBar()->IsShown());
    mbar->Check(idViewFullScreen, IsFullScreen());
    mbar->Enable(idViewFocusEditor, ed);

	event.Skip();
}

void MainFrame::OnSearchMenuUpdateUI(wxUpdateUIEvent& event)
{
    if(Manager::isappShuttingDown())
    {
        event.Skip();
        return;
    }
    cbEditor* ed = EDMAN() ? EDMAN()->GetBuiltinEditor(EDMAN()->GetActiveEditor()) : 0;
    wxMenuBar* mbar = GetMenuBar();

    mbar->Enable(idSearchFind, ed);
    mbar->Enable(idSearchFindNext, ed);
    mbar->Enable(idSearchFindPrevious, ed);
    mbar->Enable(idSearchReplace, ed);
    mbar->Enable(idSearchGotoLine, ed);

	if (m_pToolbar)
	{
		m_pToolbar->EnableTool(idSearchFind, ed);
		m_pToolbar->EnableTool(idSearchReplace, ed);
	}

	event.Skip();
}

void MainFrame::OnProjectMenuUpdateUI(wxUpdateUIEvent& event)
{
    if(Manager::isappShuttingDown())
    {
        event.Skip();
        return;
    }
    cbProject* prj = PRJMAN() ? PRJMAN()->GetActiveProject() : 0L;
    wxMenuBar* mbar = GetMenuBar();

    bool canCloseProject = (ProjectManager::CanShutdown() && EditorManager::CanShutdown());
	mbar->Enable(idProjectCloseProject, prj && canCloseProject);
    mbar->Enable(idProjectCloseAllProjects, prj && canCloseProject);
    mbar->Enable(idProjectSaveProject, prj && prj->GetModified() && canCloseProject);
    mbar->Enable(idProjectSaveProjectAs, prj && canCloseProject);
    mbar->Enable(idProjectSaveAllProjects, prj && canCloseProject);
    mbar->Enable(idProjectSaveTemplate, prj && canCloseProject);

	event.Skip();
}

void MainFrame::OnEditorUpdateUI(CodeBlocksEvent& event)
{
    if(Manager::isappShuttingDown())
    {
        event.Skip();
        return;
    }
	if (EDMAN() && event.GetEditor() == EDMAN()->GetActiveEditor())
	{
		DoUpdateStatusBar();
	}
	event.Skip();
}

void MainFrame::OnToggleOpenFilesTree(wxCommandEvent& event)
{
    if (EDMAN()->OpenFilesTreeSupported())
        EDMAN()->ShowOpenFilesTree(!EDMAN()->IsOpenFilesTreeVisible());
}

void MainFrame::OnToggleBar(wxCommandEvent& event)
{
	if (event.GetId() == idViewManager)
    {
        pDockWindow1->Show(!(pDockWindow1->GetDockPanel()->IsDocked() || pDockWindow1->IsShown()));
    }
	else if (event.GetId() == idViewMessageManager)
	{
        pDockWindow2->Show(!(pDockWindow2->GetDockPanel()->IsDocked() || pDockWindow2->IsShown()));
    }
	else if (event.GetId() == idViewToolMain)
	{
		pSlideBar->Show(!pSlideBar->IsShown());
// under Windows, the toolbar doesn't disappear immediately...
#ifdef __WXMSW__
        SendSizeEvent(); // make sure everything is laid out properly
        wxSafeYield();
#endif // __WXMSW__
	}
}

void MainFrame::OnToggleStatusBar(wxCommandEvent& event)
{
    wxStatusBar* sb = GetStatusBar();
    if (sb)
    {
        sb->Hide();
        SetStatusBar(NULL);
    }
    else
        DoCreateStatusBar();

// under Windows, the statusbar doesn't disappear immediately...
#ifdef __WXMSW__
    SendSizeEvent(); // make sure everything is laid out properly
	wxSafeYield();
#endif // __WXMSW__
	DoUpdateStatusBar();
}

void MainFrame::OnFocusEditor(wxCommandEvent& event)
{
    cbEditor* ed = EDMAN() ? EDMAN()->GetBuiltinEditor(EDMAN()->GetActiveEditor()) : 0;
    if (ed)
        ed->GetControl()->SetFocus();
}

void MainFrame::OnToggleFullScreen(wxCommandEvent& event)
{
    ShowFullScreen( !IsFullScreen(), wxFULLSCREEN_NOTOOLBAR | wxFULLSCREEN_NOSTATUSBAR
                    | wxFULLSCREEN_NOBORDER | wxFULLSCREEN_NOCAPTION );

    // Create fullscreen-close button if we're in fullscreen
    if( IsFullScreen() )
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

void MainFrame::OnPluginLoaded(CodeBlocksEvent& event)
{
    cbPlugin* plug = event.GetPlugin();
    if (plug)
	{
        if (!m_ReconfiguringPlugins)
            DoAddPlugin(plug);
        wxString msg = plug->GetInfo()->title;
        MSGMAN()->DebugLog(_("%s plugin loaded"), msg.c_str());
	}
}

#if 0
void MainFrame::OnPluginUnloaded(CodeBlocksEvent& event)
{
    cbPlugin* plug = event.GetPlugin();
    if (plug)
    {
        if (!m_ReconfiguringPlugins)
        {
            RemovePluginFromMenus(plug->GetInfo()->name);
//            CreateToolbars();
            CreateMenubar();
		}
        wxString msg = plug->GetInfo()->title;
        MSGMAN()->DebugLog(_("%s plugin unloaded"), msg.c_str());
    }
}
#endif

void MainFrame::OnSettingsEnvironment(wxCommandEvent& event)
{
    bool tbarsmall = m_SmallToolBar;
    bool needRestart = false;
    bool edmanCloseBtn = CFG_READ(_T("/editor/show_close_button"), (long int)0);

	EnvironmentSettingsDlg dlg(this);
	if (dlg.ShowModal() == wxID_OK)
	{
        m_SmallToolBar = CFG_READ(_T("/environment/toolbar_size"), (long int)1) == 1;
        needRestart = m_SmallToolBar != tbarsmall;
        MSGMAN()->EnableAutoHide(CFG_READ(_T("/message_manager/auto_hide"), 0L));
        ShowHideStartPage();

        if (CFG_READ(_T("/editor/show_close_button"), (long int)0) != edmanCloseBtn)
        {
        	wxMessageBox(_("Some of the changes you made will be applied after you restart Code::Blocks."),
                            _("Information"),
                            wxICON_INFORMATION);
        }
	}
	if (needRestart)
        wxMessageBox(_("Code::Blocks needs to be restarted for the changes to take effect."), _("Information"), wxICON_INFORMATION);
}

void MainFrame::OnSettingsEditor(wxCommandEvent& event)
{
	EDMAN()->Configure();
}

void MainFrame::OnSettingsPlugins(wxCommandEvent& event)
{
    m_ReconfiguringPlugins = true;
	if (Manager::Get()->GetPluginManager()->Configure() == wxID_OK)
	{
        // mandrav: disabled on-the-fly plugins enabling/disabling (still has glitches)
        wxMessageBox(_("Changes will take effect on the next startup."),
                    _("Information"),
                    wxICON_INFORMATION);
//        wxBusyCursor busy;
//        CreateMenubar();
//        CreateToolbars();
	}
    m_ReconfiguringPlugins = false;
}

void MainFrame::OnSettingsImpExpConfig(wxCommandEvent& event)
{
    ImpExpConfig dlg(this);
    dlg.ShowModal();
}

void MainFrame::OnLayoutChanged(wxEvent& event)
{
	if (!m_pEdMan || event.GetEventObject() != pLayoutManager)
        return;
    m_pEdMan->RefreshOpenFilesTree();
}

void MainFrame::OnProjectActivated(CodeBlocksEvent& event)
{
	DoUpdateAppTitle();
	event.Skip();
}

void MainFrame::OnProjectOpened(CodeBlocksEvent& event)
{
    ShowHideStartPage(true);
    DoUpdateAppTitle();
    event.Skip();
}

void MainFrame::OnEditorOpened(CodeBlocksEvent& event)
{
    DoUpdateAppTitle();
    event.Skip();
}

void MainFrame::OnEditorClosed(CodeBlocksEvent& event)
{
    DoUpdateAppTitle();
    event.Skip();
}

void MainFrame::OnEditorSaved(CodeBlocksEvent& event)
{
    DoUpdateAppTitle();
    event.Skip();
}

void MainFrame::OnProjectClosed(CodeBlocksEvent& event)
{
    ShowHideStartPage();
    DoUpdateAppTitle();
    event.Skip();
}

void MainFrame::OnPageChanged(wxNotebookEvent& event)
{
    DoUpdateAppTitle();
    event.Skip();
}

void MainFrame::OnShiftTab(wxCommandEvent& event)
{
    cbEditor* ed = EDMAN()->GetBuiltinActiveEditor(); // Must make sure it's cbEditor and not EditorBase
    if(ed)
        ed->DoUnIndent();
}

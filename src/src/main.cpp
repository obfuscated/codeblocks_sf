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
#include "cbworkspace.h"

#if defined(_MSC_VER) && defined( _DEBUG )
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif

#include <wx/tipdlg.h>
#include <wx/dnd.h>

#include "../sdk/configmanager.h"
#include "../sdk/cbproject.h"
#include "../sdk/cbplugin.h"
#include "../sdk/sdk_events.h"
#include "../sdk/projectmanager.h"
#include "../sdk/editormanager.h"
#include "../sdk/messagemanager.h"
#include "../sdk/pluginmanager.h"
#include "../sdk/templatemanager.h"
#include "../sdk/toolsmanager.h"

#include "dlgaboutplugin.h"
#include "dlgabout.h"
#include "printdlg.h"
#include <wx/printdlg.h>

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
int idFileSaveWorkspaceAs = XRCID("idFileSaveWorkspaceAs");
int idFileClose = XRCID("idFileClose");
int idFileCloseAll = XRCID("idFileCloseAll");
int idFilePrintSetup = XRCID("idFilePrintSetup");
int idFilePrint = XRCID("idFilePrint");
int idFileExit = XRCID("idFileExit");

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

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_SIZE(MainFrame::OnSize)
    EVT_CLOSE(MainFrame::OnApplicationClose)

    EVT_UPDATE_UI(idFileOpenRecentClearHistory, MainFrame::OnFileMenuUpdateUI)
    EVT_UPDATE_UI(idFileSave, MainFrame::OnFileMenuUpdateUI)
    EVT_UPDATE_UI(idFileSaveAs, MainFrame::OnFileMenuUpdateUI)
    EVT_UPDATE_UI(idFileSaveAllFiles, MainFrame::OnFileMenuUpdateUI)
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
    EVT_MENU(idFileSaveWorkspaceAs,  MainFrame::OnFileSaveWorkspaceAs)
    EVT_MENU(idFileClose,  MainFrame::OnFileClose)
    EVT_MENU(idFileCloseAll,  MainFrame::OnFileCloseAll)
    EVT_MENU(idFilePrintSetup,  MainFrame::OnFilePrintSetup)
    EVT_MENU(idFilePrint,  MainFrame::OnFilePrint)
    EVT_MENU(idFileExit,  MainFrame::OnFileQuit)

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
    EVT_MENU(idProjectOpen,  MainFrame::OnFileOpen)
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
	
	EVT_SASH_DRAGGED(-1, MainFrame::OnDragSash)
	
	EVT_PROJECT_ACTIVATE(MainFrame::OnProjectActivated)
	
	/// CloseFullScreen event handling
	EVT_BUTTON( idCloseFullScreen, MainFrame::OnToggleFullScreen )
END_EVENT_TABLE()

MainFrame::MainFrame(wxWindow* parent)
       : wxMDIParentFrame(parent, -1, "MainWin", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE),
	   m_pCloseFullScreenBtn(0L),
       m_pNotebook(0L),
	   m_pLeftSash(0L),
	   m_pBottomSash(0L),
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
    
    m_SmallToolBar = ConfigManager::Get()->Read("/environment/toolbar_size", (long int)0) == 1;
	CreateIDE();
	m_pEdMan->SetEditorInterfaceType(eitMDI);

#ifdef __WXMSW__
    SetIcon(wxICON(A_MAIN_ICON));
#else
    SetIcon(wxIcon(app));
#endif // __WXMSW__

    DoCreateStatusBar();
#if wxUSE_STATUSBAR
    SetStatusText(_("Welcome to "APP_NAME"!"));
#endif // wxUSE_STATUSBAR

    SetTitle(_(APP_NAME" v"APP_VERSION));

    ScanForPlugins();
    LoadWindowState();
	
#ifdef __WXMSW__
    SendSizeEvent(); // make sure everything is laid out properly
	wxSafeYield();
	// Make deliberately huge - it will be resized by m_pBottomSash.
	// This is to avoid a nasty UI glitch where the MessageManager logs would
	// not be correctly laid out until *manually* resizing m_pBottomSash...
	m_pMsgMan->SetSize(wxSize(2048, 2048));
#endif // __WXMSW__

    InitPrinting();

    ConfigManager::AddConfiguration(_("Application"), "/main_frame");
    ConfigManager::AddConfiguration(_("Environment"), "/environment");
}

MainFrame::~MainFrame()
{
    DeInitPrinting();
	//Manager::Get()->Free();
}

void MainFrame::ShowTips(bool forceShow)
{
    bool showAtStartup = ConfigManager::Get()->Read("/show_tips", 1) != 0;
    if (forceShow || showAtStartup)
    {
        wxLogNull null; // disable error message if tips file does not exist
        wxString tipsFile = ConfigManager::Get()->Read("/app_path") + "/tips.txt";
        long tipsIndex = ConfigManager::Get()->Read("/next_tip", (long)0);
        wxTipProvider* tipProvider = wxCreateFileTipProvider(tipsFile, tipsIndex);
        showAtStartup = wxShowTip(this, tipProvider, showAtStartup);
        delete tipProvider;
        ConfigManager::Get()->Write("/show_tips", showAtStartup);
        ConfigManager::Get()->Write("/next_tip", (long)tipProvider->GetCurrentTip());
    }
}

void MainFrame::CreateIDE()
{
	int leftW = ConfigManager::Get()->Read("/main_frame/layout/left_block_width", 200);
	int bottomH = ConfigManager::Get()->Read("/main_frame/layout/bottom_block_height", 150);

	// Create CloseFullScreen Button, and hide it initially
	m_pCloseFullScreenBtn = new wxButton(this, idCloseFullScreen, _( "Close Fullscreen" ), wxDefaultPosition );
	m_pCloseFullScreenBtn->Show( false );
	
	wxSize clientsize = GetClientSize();
	m_pLeftSash = new wxSashLayoutWindow(this, idLeftSash, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);
	m_pLeftSash->SetDefaultSize(wxSize(leftW, clientsize.GetHeight()));
	m_pLeftSash->SetOrientation(wxLAYOUT_VERTICAL);
	m_pLeftSash->SetAlignment(wxLAYOUT_LEFT);
	m_pLeftSash->SetSashVisible(wxSASH_RIGHT, true);

	m_pNotebook = new wxNotebook(m_pLeftSash, wxID_ANY, wxDefaultPosition, wxDefaultSize, /*wxNB_LEFT | */wxCLIP_CHILDREN/* | wxNB_MULTILINE*/);

	m_pBottomSash = new wxSashLayoutWindow(this, idBottomSash, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);
	m_pBottomSash->SetDefaultSize(wxSize(1000 - leftW, bottomH));
	m_pBottomSash->SetOrientation(wxLAYOUT_HORIZONTAL);
	m_pBottomSash->SetAlignment(wxLAYOUT_BOTTOM);
	m_pBottomSash->SetSashVisible(wxSASH_TOP, true);

	Manager::Get(this, m_pNotebook);
	Manager::Get()->GetMessageManager()->Reparent(m_pBottomSash);
	Manager::Get()->GetMessageManager()->SetSize(wxSize(200, 30));

	CreateMenubar();

	m_pEdMan = Manager::Get()->GetEditorManager();
	m_pPrjMan = Manager::Get()->GetProjectManager();
	m_pMsgMan = Manager::Get()->GetMessageManager();
 
	if (ConfigManager::Get()->Read("/main_frame/layout/toolbar_show", 1))
        CreateToolbars();
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
	
    wxString resPath = ConfigManager::Get()->Read("data_path", wxEmptyString);
    wxXmlResource *myres = wxXmlResource::Get();
    myres->Load(resPath + "/resources.zip#zip:main_menu.xrc");
    mbar = myres->LoadMenuBar("main_menu_bar");
    if(!mbar)
    {
      mbar = new wxMenuBar(); // Some error happened.
      SetMenuBar(mbar);
    }
    
    // Find Menus that we'll change later
    
    tmpidx=mbar->FindMenu("&Tools");
    if(tmpidx!=wxNOT_FOUND)
        tools = mbar->GetMenu(tmpidx);

    tmpidx=mbar->FindMenu("P&lugins");
    if(tmpidx!=wxNOT_FOUND)
        plugs = mbar->GetMenu(tmpidx);
        
    if(tmpitem = mbar->FindItem(idSettingsConfigurePlugins,NULL))
        settingsPlugins = tmpitem->GetSubMenu();
    if(tmpitem = mbar->FindItem(idHelpPlugins,NULL))
        pluginsM = tmpitem->GetSubMenu();
    
	m_ToolsMenu = tools ? tools : new wxMenu();
	m_PluginsMenu = plugs ? plugs : new wxMenu();
	m_SettingsMenu = settingsPlugins ? settingsPlugins : new wxMenu();
	m_HelpPluginsMenu = pluginsM ? pluginsM : new wxMenu();

	// core modules: create menus
	m_pPrjMan->CreateMenu(mbar);
	m_pEdMan->CreateMenu(mbar);
	m_pMsgMan->CreateMenu(mbar);

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
		delete m_pToolbar;
		m_pToolbar = 0L;
	}
    // *** Begin new Toolbar routine ***
    wxString resPath = ConfigManager::Get()->Read("data_path", wxEmptyString);
    wxString xrcToolbarName = "main_toolbar";
    if(m_SmallToolBar) // Insert logic here
        xrcToolbarName += "_16x16";
    myres->Load(resPath + "/resources.zip#zip:*.xrc");
    m_pMsgMan->DebugLog("Loading toolbar...");
    wxToolBar *mytoolbar = myres->LoadToolBar(this,xrcToolbarName);
    
    if(mytoolbar==0L)
    {
        m_pMsgMan->DebugLog(wxString("failed!"));          
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
    SetToolBar(m_pToolbar);
    // *** End new Toolbar routine ***

//    wxString res = ConfigManager::Get()->Read("data_path") + "/images/";
	// ask all plugins to rebuild their toolbars
	PluginElementsArray plugins = Manager::Get()->GetPluginManager()->GetPlugins();
	for (unsigned int i = 0; i < plugins.GetCount(); ++i)
	{
		cbPlugin* plug = plugins[i]->plugin;
		if (plug && plug->IsAttached())
		{
			if (plug->GetType() != ptTool)
				plug->BuildToolBar(m_pToolbar);
		}
	}

	wxSafeYield();
//	m_pToolbar->SetRows(2);
	m_pToolbar->Realize();
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

    wxString path = ConfigManager::Get()->Read("data_path") + "/plugins";
    m_pMsgMan->Log(_("Scanning for plugins in %s..."), path.c_str());
    int count = m_PluginManager->ScanForPlugins(path);
    m_pMsgMan->AppendLog(_("Found %d plugins: "), count);
    
    // actually load plugins
    m_PluginManager->LoadAllPlugins();
//    m_pMsgMan->DebugLog(_("%d plugins loaded"), count);
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
         m_PluginsMenu->Insert(0, wxID_ANY, "");

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
    //m_pMsgMan->DebugLog("Unloading %s plugin", pluginName.c_str());
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
    //m_pMsgMan->DebugLog("id=%d", id);
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
    SetSize(ConfigManager::Get()->Read("/main_frame/left", 0L),
            ConfigManager::Get()->Read("/main_frame/top", 0L),
            ConfigManager::Get()->Read("/main_frame/width", 640),
            ConfigManager::Get()->Read("/main_frame/height", 480));

	// sash sizes are set on creation in CreateIDE()
	DoUpdateLayout();

	// load manager and messages selected page
	Manager::Get()->GetNotebook()->SetSelection(ConfigManager::Get()->Read("/main_frame/layout/left_block_selection", 0L));
	m_pMsgMan->SetSelection(ConfigManager::Get()->Read("/main_frame/layout/bottom_block_selection", 0L));

	// load manager and messages visibility state
	m_pLeftSash->Show(ConfigManager::Get()->Read("/main_frame/layout/left_block_show", 1));
	m_pBottomSash->Show(ConfigManager::Get()->Read("/main_frame/layout/bottom_block_show", 1));

    // the toolbar visibility is handled in CreateIDE

    // maximized?
    if (ConfigManager::Get()->Read("/main_frame/maximized", 0L))
        Maximize();
}

void MainFrame::SaveWindowState()
{
    ConfigManager::Get()->Write("/main_frame/maximized", IsMaximized());
    if (!IsMaximized() && !IsIconized())
    {
        ConfigManager::Get()->Write("/main_frame/left", GetPosition().x);
        ConfigManager::Get()->Write("/main_frame/top", GetPosition().y);
        ConfigManager::Get()->Write("/main_frame/width", GetSize().x);
        ConfigManager::Get()->Write("/main_frame/height", GetSize().y);
    }

	// save block sizes
	ConfigManager::Get()->Write("/main_frame/layout/left_block_width", m_pLeftSash->GetSize().GetWidth());
	ConfigManager::Get()->Write("/main_frame/layout/bottom_block_height", m_pBottomSash->GetSize().GetHeight());

	// save manager and messages selected page
	ConfigManager::Get()->Write("/main_frame/layout/left_block_selection", Manager::Get()->GetNotebook()->GetSelection());
	ConfigManager::Get()->Write("/main_frame/layout/bottom_block_selection", m_pMsgMan->GetSelection());

    // save manager and messages visibility state
    // only if *not* in fullscreen mode (in this case the values were saved
    // before going fullscreen)
    if (!IsFullScreen())
    {
        ConfigManager::Get()->Write("/main_frame/layout/left_block_show", m_pLeftSash->IsShown());
        ConfigManager::Get()->Write("/main_frame/layout/bottom_block_show", m_pBottomSash->IsShown());
	}

    // toolbar visibility
	ConfigManager::Get()->Write("/main_frame/layout/toolbar_show", m_pToolbar != 0);
}

void MainFrame::DoAddPlugin(cbPlugin* plugin)
{
    //m_pMsgMan->DebugLog(_("Adding plugin: %s"), plugin->GetInfo()->name.c_str());
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
        plugin->BuildToolBar(GetToolBar());
    }
}

bool MainFrame::Open(const wxString& filename, bool addToHistory)
{
	wxSafeYield();
    bool ret = OpenGeneric(filename, addToHistory);
    DoUpdateLayout();
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
                m_pPrjMan->LoadWorkspace(filename);
                m_FilesHistory.AddFileToHistory(filename);
            }
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
            DoOpenProject(filename, addToHistory);
            break;

        //
        // All other files
        //
        default: return DoOpenFile(filename, addToHistory);
    }
    return true;
}

bool MainFrame::DoOpenProject(const wxString& filename, bool addToHistory)
{
//    m_pMsgMan->DebugLog(_("Opening project '%s'"), filename.c_str());
    cbProject* prj = m_pPrjMan->LoadProject(filename);
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
    //m_pMsgMan->DebugLog(_("Opening file '%s'"), filename.c_str());
    if (m_pEdMan->Open(filename))
    {
		if (addToHistory)
			m_FilesHistory.AddFileToHistory(filename);
        return true;
    }
    return false;
}

bool MainFrame::DoCloseCurrentWorkspace()
{
    return m_pPrjMan->CloseWorkspace();
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
    cbEditor* ed = m_pEdMan->GetActiveEditor();
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

void MainFrame::DoUpdateLayout()
{
	if (!m_pEdMan)
		return;
	wxLayoutAlgorithm layout;
	switch (m_pEdMan->GetEditorInterfaceType())
	{
		case eitMDI:
			layout.LayoutMDIFrame(this);
			break;
		case eitTabbed:
			layout.LayoutMDIFrame(this);
//			layout.LayoutFrame(this, m_pEdMan);
			break;
	}
	
	/**
	@attention Hack for fixing wxSashWindow oddness...Resize with 'height-1'.
	This fixes the oddness. However, we resize again to 'height' to retain the
	original height.
	We resize here so that the bottom sash gets fixed both on startup, and when
	it's hidden/shown later.
	*/
	int w, h;
	m_pBottomSash->GetSize( &w, &h );
	m_pBottomSash->SetSize( w, h-1 );
	m_pBottomSash->SetSize( w, h );
}

void MainFrame::DoUpdateAppTitle()
{
	cbProject* prj = m_pPrjMan ? m_pPrjMan->GetActiveProject() : 0L;
	if (prj)
	    SetTitle(APP_NAME" - " + prj->GetTitle());
	else
		SetTitle(_(APP_NAME" v"APP_VERSION));
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
            ConfigManager::Get()->SetPath("/recent_files");
            m_FilesHistory.Load(*ConfigManager::Get());
            ConfigManager::Get()->SetPath("/");
        }
    }
}

void MainFrame::TerminateRecentFilesHistory()
{
    ConfigManager::Get()->SetPath("/recent_files");
    m_FilesHistory.Save(*ConfigManager::Get());
    ConfigManager::Get()->SetPath("/");

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
        m_pMsgMan->DebugLog(_("No plugin found for ID %d"), event.GetId());
}

void MainFrame::OnPluginSettingsMenu(wxCommandEvent& event)
{
    wxString pluginName = m_PluginIDsMap[event.GetId()];
    if (!pluginName.IsEmpty())
        Manager::Get()->GetPluginManager()->ConfigurePlugin(pluginName);
    else
        m_pMsgMan->DebugLog(_("No plugin found for ID %d"), event.GetId());
}

void MainFrame::OnHelpPluginMenu(wxCommandEvent& event)
{
    wxString pluginName = m_PluginIDsMap[event.GetId()];
    if (!pluginName.IsEmpty())
    {
        const PluginInfo* pi = Manager::Get()->GetPluginManager()->GetPluginInfo(pluginName);
        if (!pi)
        {
            m_pMsgMan->DebugLog(_("No plugin info for %s!"), pluginName.c_str());
            return;
        }
        dlgAboutPlugin* dlg = new dlgAboutPlugin(this, pi);
        dlg->ShowModal();
        delete dlg;
    }
    else
        m_pMsgMan->DebugLog(_("No plugin found for ID %d"), event.GetId());
}

void MainFrame::OnSize(wxSizeEvent& WXUNUSED(event))
{
	DoUpdateLayout();
}

void MainFrame::OnFileNewEmpty(wxCommandEvent& event)
{
	cbProject* project = m_pPrjMan->GetActiveProject();
	if (project)
        wxSetWorkingDirectory(project->GetBasePath());
    cbEditor* ed = m_pEdMan->New();

	if (!ed || !project)
		return;

	if (wxMessageBox(_("Do you want to add this new file in the active project?"),
					_("Add file to project"),
					wxYES_NO | wxICON_QUESTION) == wxYES)
	{
        wxArrayInt targets;
		if (m_pPrjMan->AddFileToProject(ed->GetFilename(), project, targets) != 0)
		{
            ProjectFile* pf = project->GetFileByFilename(ed->GetFilename(), false);
			ed->SetProjectFile(pf);
			m_pPrjMan->RebuildTree();
		}
	}
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

void MainFrame::OnFileOpen(wxCommandEvent& WXUNUSED(event))
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
		ConfigManager::Get()->DeleteGroup("/recent_files");
	}
}

void MainFrame::OnFileSave(wxCommandEvent& event)
{
    if (!m_pEdMan->SaveActive())
    {
        wxString msg;
        msg.Printf(_("File %s could not be saved..."), m_pEdMan->GetActiveEditor()->GetFilename().c_str());
        wxMessageBox(msg, _("Error saving file"));
    }
    DoUpdateStatusBar();
}

void MainFrame::OnFileSaveAs(wxCommandEvent& event)
{
    if (!m_pEdMan->SaveActiveAs())
    {
        wxString msg;
        msg.Printf(_("File %s could not be saved..."), m_pEdMan->GetActiveEditor()->GetFilename().c_str());
        wxMessageBox(msg, _("Error saving file"));
    }
    DoUpdateStatusBar();
}

void MainFrame::OnFileSaveAllFiles(wxCommandEvent& event)
{
    m_pEdMan->SaveAll();
    DoUpdateStatusBar();
}

void MainFrame::OnFileSaveWorkspaceAs(wxCommandEvent& event)
{
    if (m_pPrjMan->SaveWorkspaceAs(""))
        m_FilesHistory.AddFileToHistory(m_pPrjMan->GetWorkspace()->GetFilename());
}

void MainFrame::OnFileClose(wxCommandEvent& WXUNUSED(event))
{
    m_pEdMan->CloseActive();
    DoUpdateStatusBar();
}

void MainFrame::OnFileCloseAll(wxCommandEvent& WXUNUSED(event))
{
    m_pEdMan->CloseAll();
    DoUpdateStatusBar();
}

void MainFrame::OnFilePrintSetup(wxCommandEvent& event)
{
    wxPrintDialog dlg;
    if (dlg.ShowModal() == wxID_OK)
        *g_printData = dlg.GetPrintDialogData().GetPrintData();
}

void MainFrame::OnFilePrint(wxCommandEvent& event)
{
    PrintDialog dlg(this);
    if (dlg.ShowModal() == wxID_OK)
        m_pEdMan->Print(dlg.GetPrintScope(), dlg.GetPrintColorMode());
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
        wxMessageBox(_("Code::Blocks is still opening files.\n"
                        "Please wait for it to finish loading and then close it..."),
                        _("Information"),
                        wxICON_INFORMATION);
        return;
    }

    if (!DoCloseCurrentWorkspace())
    {
        event.Veto();
        return;
    }

    SaveWindowState();
    TerminateRecentFilesHistory();
    
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
    m_pEdMan->SwapActiveHeaderSource();
    DoUpdateStatusBar();
}

void MainFrame::OnEditBookmarksToggle(wxCommandEvent& event)
{
    cbEditor* ed = m_pEdMan->GetActiveEditor();
    if (ed)
		ed->MarkerToggle(BOOKMARK_MARKER);
}

void MainFrame::OnEditBookmarksNext(wxCommandEvent& event)
{
    cbEditor* ed = m_pEdMan->GetActiveEditor();
    if (ed)
		ed->MarkerNext(BOOKMARK_MARKER);
}

void MainFrame::OnEditBookmarksPrevious(wxCommandEvent& event)
{
    cbEditor* ed = m_pEdMan->GetActiveEditor();
    if (ed)
		ed->MarkerPrevious(BOOKMARK_MARKER);
}

void MainFrame::OnEditUndo(wxCommandEvent& event)
{
    cbEditor* ed = m_pEdMan->GetActiveEditor();
    if (ed)
        ed->GetControl()->Undo();
}

void MainFrame::OnEditRedo(wxCommandEvent& event)
{
    cbEditor* ed = m_pEdMan->GetActiveEditor();
    if (ed)
        ed->GetControl()->Redo();
}

void MainFrame::OnEditCopy(wxCommandEvent& event)
{
    cbEditor* ed = m_pEdMan->GetActiveEditor();
    if (ed)
        ed->GetControl()->Copy();
}

void MainFrame::OnEditCut(wxCommandEvent& event)
{
    cbEditor* ed = m_pEdMan->GetActiveEditor();
    if (ed)
        ed->GetControl()->Cut();
}

void MainFrame::OnEditPaste(wxCommandEvent& event)
{
    cbEditor* ed = m_pEdMan->GetActiveEditor();
    if (ed)
        ed->GetControl()->Paste();
}

void MainFrame::OnEditSelectAll(wxCommandEvent& event)
{
    cbEditor* ed = m_pEdMan->GetActiveEditor();
    if (ed)
        ed->GetControl()->SelectAll();
}

void MainFrame::OnEditCommentSelected(wxCommandEvent& event)
{
	cbEditor* ed = m_pEdMan->GetActiveEditor();
	if( ed )
	{
        ed->GetControl()->BeginUndoAction();
		cbStyledTextCtrl *stc = ed->GetControl();
		if( wxSTC_INVALID_POSITION != stc->GetSelectionStart() )
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
				int commentPos = strLine.Strip( wxString::leading ).Find( _( "//" ) );
				
				if( -1 == commentPos || commentPos > 0 )
				{
					// Comment
					/// @todo This should be language-dependent. We're currently assuming C++
					stc->InsertText( stc->PositionFromLine( startLine ), _( "//" ) );
				}
				else
				{
					// Uncomment
					strLine.Replace( _( "//" ), _( "" ), false );
						
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

void MainFrame::OnEditFoldAll(wxCommandEvent& event)
{
    cbEditor* ed = m_pEdMan->GetActiveEditor();
    if (ed)
		ed->FoldAll();
}

void MainFrame::OnEditUnfoldAll(wxCommandEvent& event)
{
    cbEditor* ed = m_pEdMan->GetActiveEditor();
    if (ed)
		ed->UnfoldAll();
}

void MainFrame::OnEditToggleAllFolds(wxCommandEvent& event)
{
    cbEditor* ed = m_pEdMan->GetActiveEditor();
    if (ed)
		ed->ToggleAllFolds();
}


void MainFrame::OnEditFoldBlock(wxCommandEvent& event)
{
    cbEditor* ed = m_pEdMan->GetActiveEditor();
    if (ed)
		ed->FoldBlockFromLine();
}

void MainFrame::OnEditUnfoldBlock(wxCommandEvent& event)
{
    cbEditor* ed = m_pEdMan->GetActiveEditor();
    if (ed)
		ed->UnfoldBlockFromLine();
}

void MainFrame::OnEditToggleFoldBlock(wxCommandEvent& event)
{
    cbEditor* ed = m_pEdMan->GetActiveEditor();
    if (ed)
		ed->ToggleFoldBlockFromLine();
}

void MainFrame::OnEditEOLMode(wxCommandEvent& event)
{
    cbEditor* ed = m_pEdMan->GetActiveEditor();
    if (ed)
    {
        int mode = -1;
        
        if (event.GetId() == idEditEOLCRLF)
            mode = wxSTC_EOL_CRLF;
        else if (event.GetId() == idEditEOLCR)
            mode = wxSTC_EOL_CR;
        else if (event.GetId() == idEditEOLLF)
            mode = wxSTC_EOL_LF;
        
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
	m_pEdMan->ShowFindDialog(false);
}

void MainFrame::OnSearchFindNext(wxCommandEvent& event)
{
	if (event.GetId() == idSearchFindPrevious)
		m_pEdMan->FindNext(false);
	else
		m_pEdMan->FindNext(true);
}

void MainFrame::OnSearchReplace(wxCommandEvent& event)
{
	m_pEdMan->ShowFindDialog(true);
}

void MainFrame::OnSearchGotoLine(wxCommandEvent& event)
{
	cbEditor* ed = m_pEdMan->GetActiveEditor();
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
								_( "" ),
								this );
	int line = atol( strLine.c_str() );
	if ( line > 1 && line <= max )
		ed->GetControl()->GotoPos(ed->GetControl()->PositionFromLine(line - 1));
}

void MainFrame::OnProjectNewEmpty(wxCommandEvent& event)
{
    m_pPrjMan->NewProject();
}

void MainFrame::OnProjectNew(wxCommandEvent& event)
{
    TemplateManager::Get()->NewProject();
}

void MainFrame::OnProjectSaveProject(wxCommandEvent& event)
{
    if (m_pPrjMan->SaveActiveProject() ||
        m_pPrjMan->SaveActiveProjectAs())
        m_FilesHistory.AddFileToHistory(m_pPrjMan->GetActiveProject()->GetFilename());
    DoUpdateStatusBar();
}

void MainFrame::OnProjectSaveProjectAs(wxCommandEvent& event)
{
    if (m_pPrjMan->SaveActiveProjectAs())
        m_FilesHistory.AddFileToHistory(m_pPrjMan->GetActiveProject()->GetFilename());
    DoUpdateStatusBar();
}

void MainFrame::OnProjectSaveAllProjects(wxCommandEvent& event)
{
    m_pPrjMan->SaveAllProjects();
    DoUpdateStatusBar();
}

void MainFrame::OnProjectSaveTemplate(wxCommandEvent& event)
{
    TemplateManager::Get()->SaveUserTemplate(m_pPrjMan->GetActiveProject());
}

void MainFrame::OnProjectCloseProject(wxCommandEvent& event)
{
    // we 're not actually shutting down here, but we want to check if the
    // active project is still opening files (still busy)
    if (!ProjectManager::CanShutdown() || !EditorManager::CanShutdown())
    {
        wxMessageBox(_("This project is still opening files.\n"
                        "Please wait for it to finish loading and then close it..."),
                        _("Information"),
                        wxICON_INFORMATION);
    }
    m_pPrjMan->CloseActiveProject();
    DoUpdateStatusBar();
}

void MainFrame::OnProjectCloseAllProjects(wxCommandEvent& event)
{
    m_pPrjMan->CloseWorkspace();
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
    cbEditor* ed = m_pEdMan ? m_pEdMan->GetActiveEditor() : 0L;
    wxMenuBar* mbar = GetMenuBar();

    mbar->Enable(idFileOpenRecentClearHistory, m_FilesHistory.GetCount());
    mbar->Enable(idFileClose, ed);
    mbar->Enable(idFileCloseAll, ed);
    mbar->Enable(idFileSave, ed && ed->GetModified());
    mbar->Enable(idFileSaveAs, ed);
    mbar->Enable(idFileSaveAllFiles, ed);
    mbar->Enable(idFileSaveWorkspaceAs, m_pPrjMan && m_pPrjMan->GetActiveProject());
    mbar->Enable(idFilePrint, m_pEdMan && m_pEdMan->GetActiveEditor());
	
	if (m_pToolbar)
	{
		m_pToolbar->EnableTool(idFileSave, ed && ed->GetModified());
	}
	
	event.Skip();
}

void MainFrame::OnEditMenuUpdateUI(wxUpdateUIEvent& event)
{
    cbEditor* ed = m_pEdMan ? m_pEdMan->GetActiveEditor() : 0L;
    wxMenuBar* mbar = GetMenuBar();
	bool hasSel = ed ? ed->GetControl()->GetSelectionStart() != ed->GetControl()->GetSelectionEnd() : false;
	bool canUndo = ed ? ed->GetControl()->CanUndo() : false;
	bool canRedo = ed ? ed->GetControl()->CanRedo() : false;
	bool canPaste = ed ? ed->GetControl()->CanPaste() : false;
    int eolMode = ed ? ed->GetControl()->GetEOLMode() : -1;

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
	mbar->Check(idEditEOLCRLF, eolMode == wxSTC_EOL_CRLF);
	mbar->Check(idEditEOLCR, eolMode == wxSTC_EOL_CR);
	mbar->Check(idEditEOLLF, eolMode == wxSTC_EOL_LF);
	mbar->Enable(idEditCommentSelected, ed);

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
    wxMenuBar* mbar = GetMenuBar();
    cbEditor* ed = m_pEdMan ? m_pEdMan->GetActiveEditor() : 0L;

    mbar->Check(idViewToolMain, m_pToolbar && m_pToolbar->IsShown());
    mbar->Check(idViewManager, m_pLeftSash && m_pLeftSash->IsShown());
    mbar->Check(idViewOpenFilesTree, m_pEdMan && m_pEdMan->IsOpenFilesTreeVisible());
    mbar->Check(idViewMessageManager, m_pBottomSash && m_pBottomSash->IsShown());
    mbar->Check(idViewStatusbar, GetStatusBar() && GetStatusBar()->IsShown());
    mbar->Check(idViewFullScreen, IsFullScreen());
    mbar->Enable(idViewFocusEditor, ed);

	event.Skip();
}

void MainFrame::OnSearchMenuUpdateUI(wxUpdateUIEvent& event)
{
    cbEditor* ed = m_pEdMan ? m_pEdMan->GetActiveEditor() : 0L;
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
    cbProject* prj = m_pPrjMan ? m_pPrjMan->GetActiveProject() : 0L;
    wxMenuBar* mbar = GetMenuBar();
    
	mbar->Enable(idProjectCloseProject, prj);
    mbar->Enable(idProjectCloseAllProjects, prj);
    mbar->Enable(idProjectSaveProject, prj && prj->GetModified());
    mbar->Enable(idProjectSaveProjectAs, prj);
    mbar->Enable(idProjectSaveAllProjects, prj);
    mbar->Enable(idProjectSaveTemplate, prj);
	
	event.Skip();
}

void MainFrame::OnEditorUpdateUI(CodeBlocksEvent& event)
{
	if (m_pEdMan && event.GetEditor() == m_pEdMan->GetActiveEditor())
	{
		event.GetEditor()->HighlightBraces(); // brace highlighting
		DoUpdateStatusBar();
	}
	event.Skip();
}

void MainFrame::OnToggleOpenFilesTree(wxCommandEvent& event)
{
    if (m_pEdMan->OpenFilesTreeSupported())
        m_pEdMan->ShowOpenFilesTree(!m_pEdMan->IsOpenFilesTreeVisible());
}

void MainFrame::OnToggleBar(wxCommandEvent& event)
{
	if (event.GetId() == idViewManager)
    {
        m_pLeftSash->Show(!m_pLeftSash->IsShown());
        m_pLeftSash->SetSize( m_pLeftSash->GetSize() );
    }
	else if (event.GetId() == idViewMessageManager)
	{
		m_pBottomSash->Show(!m_pBottomSash->IsShown());
    }
	else if (event.GetId() == idViewToolMain)
	{
		if (m_pToolbar)
		{
			SetToolBar(0L);
			delete m_pToolbar;
			m_pToolbar = 0L;
		}
		else
			CreateToolbars();
	}

	DoUpdateLayout();
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
    cbEditor* ed = m_pEdMan ? m_pEdMan->GetActiveEditor() : 0L;
    if (ed)
        ed->GetControl()->SetFocus();
}

void MainFrame::OnToggleFullScreen(wxCommandEvent& event)
{
    if (!IsFullScreen())
    {
        // we are going to toggle to fullscreen: save current sashes state
        // so that we can restore it when leaving fullscreen...
        ConfigManager::Get()->Write("/main_frame/layout/left_block_show", m_pLeftSash->IsShown());
        ConfigManager::Get()->Write("/main_frame/layout/bottom_block_show", m_pBottomSash->IsShown());
	}

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
        
        m_pLeftSash->Show(false);
        m_pBottomSash->Show(false);

        m_pCloseFullScreenBtn->Show( true );
        m_pCloseFullScreenBtn->Raise();
    }
    else
    {
        m_pCloseFullScreenBtn->Show( false );
        
        // leaving fullscreen: restore sashes state
        m_pLeftSash->Show(ConfigManager::Get()->Read("/main_frame/layout/left_block_show", 1));
        m_pBottomSash->Show(ConfigManager::Get()->Read("/main_frame/layout/bottom_block_show", 1));
    }
    /// @todo Check whether hiding all panes is desirable.
    /// Perhaps make it customizable?
    
    // Update layout
    DoUpdateLayout();
}

void MainFrame::OnPluginLoaded(CodeBlocksEvent& event)
{
    cbPlugin* plug = event.GetPlugin();
    if (plug)
	{
        if (!m_ReconfiguringPlugins)
            DoAddPlugin(plug);
        wxString msg = plug->GetInfo()->title;
        m_pMsgMan->DebugLog(_("%s plugin loaded"), msg.c_str());
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
            CreateToolbars();
            CreateMenubar();
		}
        wxString msg = plug->GetInfo()->title;
        m_pMsgMan->DebugLog(_("%s plugin unloaded"), msg.c_str());
    }
}
#endif

void MainFrame::OnSettingsEnvironment(wxCommandEvent& event)
{
    bool tbarsmall = m_SmallToolBar;

	EnvironmentSettingsDlg dlg(this);
	if (dlg.ShowModal() == wxID_OK)
	{
        m_SmallToolBar = ConfigManager::Get()->Read("/environment/toolbar_size", (long int)0) == 1;
        if (m_SmallToolBar != tbarsmall)
            CreateToolbars();
	}
}

void MainFrame::OnSettingsEditor(wxCommandEvent& event)
{
	m_pEdMan->Configure();
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

void MainFrame::OnDragSash(wxSashEvent& event)
{
	if (event.GetDragStatus() == wxSASH_STATUS_OUT_OF_RANGE)
		return;

	wxRect rect = event.GetDragRect();
	if (event.GetId() == idLeftSash)
		m_pLeftSash->SetDefaultSize(wxSize(rect.x, 0)); // resize left sash
	else if (event.GetId() == idBottomSash)
		m_pBottomSash->SetDefaultSize(wxSize(0, GetClientSize().GetHeight() - rect.y)); // resize bottom sash
	DoUpdateLayout();
}

void MainFrame::OnProjectActivated(CodeBlocksEvent& event)
{
	DoUpdateAppTitle();
	event.Skip();
}

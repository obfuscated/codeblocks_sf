/***************************************************************
 * Name:      ThreadSearch
 * Purpose:   ThreadSearch Code::Blocks plugin
 *            Most of the interactions with C::B are handled here.
 * Author:    Jerome ANTOINE
 * Created:   2007-10-08
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#include <sdk.h> // Code::Blocks SDK
#ifndef CB_PRECOMP
    #include <wx/combobox.h>
    #include <wx/menu.h>
    #include <wx/toolbar.h>
    #include <wx/xrc/xmlres.h>
    #include "cbeditor.h"
    #include "configmanager.h"
    #include "sdk_events.h"
#endif

#include "cbstyledtextctrl.h"
#include "ThreadSearch.h"
#include "ThreadSearchView.h"
#include "ThreadSearchConfPanel.h"
#include "ThreadSearchControlIds.h"
#include "logging.h" //(pecan 2007/7/26)

// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
    PluginRegistrant<ThreadSearch> reg(_T("ThreadSearch"));
}

// ----------------------------------------------------------------------------
// CodeBlocks main.cpp managers all the following UI entires in one routine.
// So if only one changes, all will change. Therefore, to enable/disable copy/paste,
// we have to capture all the following to see if it really belongs to us
int idEditUndo = XRCID("idEditUndo");
int idEditRedo = XRCID("idEditRedo");
int idEditCopy = XRCID("idEditCopy");
int idEditCut = XRCID("idEditCut");
int idEditPaste = XRCID("idEditPaste");
int idEditSwapHeaderSource = XRCID("idEditSwapHeaderSource");
int idEditGotoMatchingBrace = XRCID("idEditGotoMatchingBrace");
int idEditBookmarks = XRCID("idEditBookmarks");
int idEditBookmarksToggle = XRCID("idEditBookmarksToggle");
int idEditBookmarksPrevious = XRCID("idEditBookmarksPrevious");
int idEditBookmarksNext = XRCID("idEditBookmarksNext");
int idEditFoldAll = XRCID("idEditFoldAll");
int idEditUnfoldAll = XRCID("idEditUnfoldAll");
int idEditToggleAllFolds = XRCID("idEditToggleAllFolds");
int idEditFoldBlock = XRCID("idEditFoldBlock");
int idEditUnfoldBlock = XRCID("idEditUnfoldBlock");
int idEditToggleFoldBlock = XRCID("idEditToggleFoldBlock");
int idEditEOLCRLF = XRCID("idEditEOLCRLF");
int idEditEOLCR = XRCID("idEditEOLCR");
int idEditEOLLF = XRCID("idEditEOLLF");
int idEditEncoding = XRCID("idEditEncoding");
int idEditSelectAll = XRCID("idEditSelectAll");
int idEditCommentSelected = XRCID("idEditCommentSelected");
int idEditUncommentSelected = XRCID("idEditUncommentSelected");
int idEditToggleCommentSelected = XRCID("idEditToggleCommentSelected");
int idEditAutoComplete = XRCID("idEditAutoComplete");
// ----------------------------------------------------------------------------

int idMenuEditCopy = XRCID("idEditCopy");
int idMenuEditPaste = XRCID("idEditPaste");

// events handling
BEGIN_EVENT_TABLE(ThreadSearch, cbPlugin)
    // add any events you want to handle here
    EVT_UPDATE_UI (idMenuViewThreadSearch,   ThreadSearch::OnMnuViewThreadSearchUpdateUI)
    EVT_MENU      (idMenuViewThreadSearch,   ThreadSearch::OnMnuViewThreadSearch)
    EVT_UPDATE_UI (idMenuViewFocusThreadSearch,   ThreadSearch::OnMnuViewFocusThreadSearchUpdateUI)
    EVT_MENU      (idMenuViewFocusThreadSearch,   ThreadSearch::OnMnuViewFocusThreadSearch)
    EVT_UPDATE_UI (idMenuSearchThreadSearch, ThreadSearch::OnMnuSearchThreadSearchUpdateUI)
    EVT_MENU      (idMenuSearchThreadSearch, ThreadSearch::OnMnuSearchThreadSearch)
    EVT_MENU      (idMenuCtxThreadSearch,    ThreadSearch::OnCtxThreadSearch)
    EVT_MENU      (idMenuEditCopy,           ThreadSearch::OnMnuEditCopy)
    EVT_UPDATE_UI (idMenuEditCopy,           ThreadSearch::OnMnuEditCopyUpdateUI)
    EVT_MENU      (idMenuEditPaste,          ThreadSearch::OnMnuEditPaste)
    EVT_TOOL      (idBtnOptions,             ThreadSearch::OnBtnOptionsClick)
    EVT_TOOL      (idBtnSearch,              ThreadSearch::OnBtnSearchClick)
    EVT_TEXT_ENTER(idCboSearchExpr,          ThreadSearch::OnCboSearchExprEnter)
    EVT_TEXT      (idCboSearchExpr,          ThreadSearch::OnCboSearchExprEnter)
    EVT_TEXT_ENTER(idTxtSearchDirPath,       ThreadSearch::OnCboSearchExprEnter)
    EVT_TEXT_ENTER(idTxtSearchMask,          ThreadSearch::OnCboSearchExprEnter)
// ---------------------------------------------------------------------------
    // CodeBlocks main.cpp managers all the following UI entires in ONE routine.
    // So if only one changes, all may change.
    //Therefore, to enable/disable copy/paste, we have to capture all the following
    // to see if the event actually belongs to us.
    EVT_UPDATE_UI(idEditUndo, ThreadSearch::OnMnuEditCopyUpdateUI)
    EVT_UPDATE_UI(idEditRedo, ThreadSearch::OnMnuEditCopyUpdateUI)
    EVT_UPDATE_UI(idEditCopy, ThreadSearch::OnMnuEditCopyUpdateUI)
    EVT_UPDATE_UI(idEditCut, ThreadSearch::OnMnuEditCopyUpdateUI)
    EVT_UPDATE_UI(idEditPaste, ThreadSearch::OnMnuEditCopyUpdateUI)
    EVT_UPDATE_UI(idEditSwapHeaderSource, ThreadSearch::OnMnuEditCopyUpdateUI)
    EVT_UPDATE_UI(idEditGotoMatchingBrace, ThreadSearch::OnMnuEditCopyUpdateUI)
    EVT_UPDATE_UI(idEditFoldAll, ThreadSearch::OnMnuEditCopyUpdateUI)
    EVT_UPDATE_UI(idEditUnfoldAll, ThreadSearch::OnMnuEditCopyUpdateUI)
    EVT_UPDATE_UI(idEditToggleAllFolds, ThreadSearch::OnMnuEditCopyUpdateUI)
    EVT_UPDATE_UI(idEditFoldBlock, ThreadSearch::OnMnuEditCopyUpdateUI)
    EVT_UPDATE_UI(idEditUnfoldBlock, ThreadSearch::OnMnuEditCopyUpdateUI)
    EVT_UPDATE_UI(idEditToggleFoldBlock, ThreadSearch::OnMnuEditCopyUpdateUI)
    EVT_UPDATE_UI(idEditEOLCRLF, ThreadSearch::OnMnuEditCopyUpdateUI)
    EVT_UPDATE_UI(idEditEOLCR, ThreadSearch::OnMnuEditCopyUpdateUI)
    EVT_UPDATE_UI(idEditEOLLF, ThreadSearch::OnMnuEditCopyUpdateUI)
    EVT_UPDATE_UI(idEditEncoding, ThreadSearch::OnMnuEditCopyUpdateUI)
    EVT_UPDATE_UI(idEditSelectAll, ThreadSearch::OnMnuEditCopyUpdateUI)
    EVT_UPDATE_UI(idEditBookmarksToggle, ThreadSearch::OnMnuEditCopyUpdateUI)
    EVT_UPDATE_UI(idEditBookmarksNext, ThreadSearch::OnMnuEditCopyUpdateUI)
    EVT_UPDATE_UI(idEditBookmarksPrevious, ThreadSearch::OnMnuEditCopyUpdateUI)
    EVT_UPDATE_UI(idEditCommentSelected, ThreadSearch::OnMnuEditCopyUpdateUI)
    EVT_UPDATE_UI(idEditAutoComplete, ThreadSearch::OnMnuEditCopyUpdateUI)
    EVT_UPDATE_UI(idEditUncommentSelected, ThreadSearch::OnMnuEditCopyUpdateUI)
    EVT_UPDATE_UI(idEditToggleCommentSelected, ThreadSearch::OnMnuEditCopyUpdateUI)
END_EVENT_TABLE()

// constructor
ThreadSearch::ThreadSearch()
             :m_SearchedWord(wxEmptyString),
              m_pThreadSearchView(NULL),
              m_pViewManager(NULL),
              m_pToolbar(NULL),
              m_CtxMenuIntegration(true),
              m_UseDefValsForThreadSearch(true),
              m_ShowSearchControls(true),
              m_ShowDirControls(false),
              m_ShowCodePreview(true),
              m_DeletePreviousResults(true),
              m_LoggerType(ThreadSearchLoggerBase::TypeList),
              m_DisplayLogHeaders(true),
              m_DrawLogLines(false),
              m_pCboSearchExpr(0),
              m_SplitterMode(wxSPLIT_VERTICAL),
              m_FileSorting(InsertIndexManager::SortByFilePath)
{
    // Make sure our resources are available.
    // In the generated boilerplate code we have no resources but when
    // we add some, it will be nice that this code is in place already ;)
    // ThreadSearch plugin has no resources in zip
    if(!Manager::LoadResource(_T("ThreadSearch.zip")))
    {
        NotifyMissingFile(_T("ThreadSearch.zip"));
    }
}

// destructor
ThreadSearch::~ThreadSearch()
{
}

void ThreadSearch::OnAttach()
{
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be TRUE...
    // You should check for it in other functions, because if it
    // is FALSE, it means that the application did *not* "load"
    // (see: does not need) this plugin...

    #if LOGGING
     wxLog::EnableLogging(true);
     m_pLog = new wxLogWindow(Manager::Get()->GetAppWindow(), _T(" ThreadSearch Plugin"), true, false);
     wxLog::SetActiveTarget( m_pLog);
     m_pLog->Flush();
     m_pLog->GetFrame()->SetSize(20,30,600,300);
     LOGIT( _T("ThreadSearch Plugin Logging Started"));
    #endif

    bool showPanel;
    int  sashPosition;
    ThreadSearchViewManagerBase::eManagerTypes mgrType;
    wxArrayString searchPatterns;

    // Loads configuration from default.conf
    LoadConfig(showPanel, sashPosition, mgrType, searchPatterns);

    // Adds window to the manager
    m_pThreadSearchView = new ThreadSearchView(*this);
    m_pThreadSearchView->SetSearchHistory(searchPatterns);

    // Builds manager
    m_pViewManager = ThreadSearchViewManagerBase::BuildThreadSearchViewManagerBase(m_pThreadSearchView, true, mgrType);

    // Ensure view is shown or hidden
    m_pViewManager->ShowView(showPanel);

    // Sets splitter sash in the middle of the width of the window
    // and creates columns as it is not managed in ctor on Linux
    int x, y;
    m_pThreadSearchView->GetSize(&x, &y);
    m_pThreadSearchView->SetSashPosition(x/2);
    m_pThreadSearchView->Update();

    // Set the splitter posn from the config
    if (sashPosition != 0)
        m_pThreadSearchView->SetSashPosition(sashPosition);

    // Shows/Hides search widgets on the Messages notebook ThreadSearch panel
    m_pThreadSearchView->ShowSearchControls(m_ShowSearchControls);

    // true if it enters in OnRelease for the first time
    m_OnReleased = false;
}

void ThreadSearch::OnRelease(bool appShutDown)
{
    // do de-initialization for your plugin
    // if appShutDown is false, the plugin is unloaded because Code::Blocks is being shut down,
    // which means you must not use any of the SDK Managers
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be FALSE...

    // --------------------------------------------------------------
    // Carefull! This routine can be entered consecutive times
    // --------------------------------------------------------------
    if ( m_OnReleased ) return;
    m_OnReleased = true;

    // Removes Thread search menu item from the View menu
    RemoveMenuItems();

    m_pToolbar = 0;

    if ( m_pThreadSearchView != 0 )
    {
        m_pViewManager->RemoveViewFromManager();
        m_pThreadSearchView->Destroy();
    }

    delete m_pViewManager;
    m_pViewManager = 0;
}


void ThreadSearch::OnThreadSearchViewDestruction()
{
    // Method is called from view destructor.
    // Destruction is either made by plugin or
    // Messages Notebook.

    // We show code preview to save a consistent
    // value of splitter sash position.
    m_pThreadSearchView->ApplySplitterSettings(m_ShowCodePreview, m_SplitterMode);

    // Saves configuration to default.conf
    SaveConfig(m_pViewManager->IsViewShown(),
               m_pThreadSearchView->GetSashPosition(),
               m_pViewManager->GetManagerType(),
               m_pThreadSearchView->GetSearchHistory());

    // Reset of the pointer as view is being deleted
    m_pThreadSearchView = NULL;
}


int ThreadSearch::Configure()
{
    if ( !IsAttached() )
        return -1;

    // Creates and displays the configuration dialog for the plugin
    cbConfigurationDialog dlg(Manager::Get()->GetAppWindow(), wxID_ANY, wxT("Thread search"));
    cbConfigurationPanel* panel = GetConfigurationPanel(&dlg);
    if (panel)
    {
        dlg.AttachConfigurationPanel(panel);
        PlaceWindow(&dlg);
        return dlg.ShowModal() == wxID_OK ? 0 : -1;
    }
    return -1;
}

void ThreadSearch::BuildMenu(wxMenuBar* menuBar)
{
    //The application is offering its menubar for your plugin,
    //to add any menu items you want...
    //Append any items you need in the menu...
    //NOTE: Be careful in here... The application's menubar is at your disposal.
    size_t i;
    int idx = menuBar->FindMenu(_("&View"));
    if (idx != wxNOT_FOUND)
    {
        wxMenu* menu = menuBar->GetMenu(idx);
        wxMenuItemList& items = menu->GetMenuItems();

        // find the first separator and insert before it
        for (i = 0; i < items.GetCount(); ++i)
        {
            if (items[i]->IsSeparator())
            {
                menu->InsertCheckItem(i, idMenuViewThreadSearch, _("Thread search"),
                                      _("Toggle displaying the 'Thread search' panel"));
                break;
            }
        }

        if ( i == items.GetCount() )
        {
            // not found, just append
            menu->AppendCheckItem(idMenuViewThreadSearch, _("Thread search"),
                                  _("Toggle displaying the 'Thread search' panel"));
        }

        menu->Append(idMenuViewFocusThreadSearch, _("Focus Thread Search"),
                     _("Makes the search box of the Thread search panel the focused control"));
    }

    idx = menuBar->FindMenu(_("Sea&rch"));
    if (idx != wxNOT_FOUND)
    {
        wxMenu* menu = menuBar->GetMenu(idx);
        wxMenuItemList& items = menu->GetMenuItems();

        // find the first separator and insert separator + entry before it
        for (i = 0; i < items.GetCount(); ++i)
        {
            if (items[i]->IsSeparator())
            {
                menu->Insert(i, idMenuSearchThreadSearch, _("Thread search"),
                                _("Perform a Threaded search with the current word"));
                menu->InsertSeparator(i);
                break;
            }
        }

        if ( i == items.GetCount() )
        {
            // not found, just append
            menu->Append(idMenuSearchThreadSearch, _("Thread search"),
                            _("Perform a Threaded search with the current word"));
            menu->AppendSeparator();
        }
    }
}

void ThreadSearch::RemoveMenuItems()
{
    // Removes 'Thread search' item from View and Search menu
    wxMenuBar* menuBar = Manager::Get()->GetAppFrame()->GetMenuBar();
    int idx = menuBar->FindMenu(_("&View"));
    if (idx != wxNOT_FOUND)
    {
        wxMenu* viewMenu = menuBar->GetMenu(idx);
        if ( viewMenu != 0 )
        {
            viewMenu->Remove(idMenuViewThreadSearch);
        }
    }

    idx = menuBar->FindMenu(_("Sea&rch"));
    if (idx != wxNOT_FOUND)
    {
        wxMenu* searchMenu = menuBar->GetMenu(idx);
        if ( searchMenu != 0 )
        {
            searchMenu->Remove(idMenuSearchThreadSearch);
        }
    }
}


void ThreadSearch::OnMnuViewThreadSearch(wxCommandEvent& event)
{
    if ( !IsAttached() )
        return;

    m_pViewManager->ShowView(event.IsChecked());
}


void ThreadSearch::OnMnuSearchThreadSearch(wxCommandEvent& event)
{
    if ( !IsAttached() )
        return;

    // Need to get the cursor word first and ensure it is consistent.
    if ( (GetCursorWord(m_SearchedWord) == true) && (m_SearchedWord.IsEmpty() == false) )
    {
        // m_SearchedWord is Ok => Search
        RunThreadSearch(m_SearchedWord, true);
    }
    else
    {
        // Word is KO, just show the panel
        m_pViewManager->ShowView(true);
    }
}

void ThreadSearch::OnMnuViewFocusThreadSearch(wxCommandEvent& event)
{
    if ( !IsAttached() )
        return;

    GetCursorWord(m_SearchedWord);

    m_pViewManager->ShowView(true);
    m_pThreadSearchView->FocusSearchCombo(m_SearchedWord);
}


void ThreadSearch::OnCtxThreadSearch(wxCommandEvent& event)
{
    if ( !IsAttached() )
        return;

    // m_SearchedWord was set in BuildModuleMenu
    RunThreadSearch(m_SearchedWord, true);
}


void ThreadSearch::OnMnuViewThreadSearchUpdateUI(wxUpdateUIEvent& event)
{
    if ( !IsAttached() )
        return;

    Manager::Get()->GetAppFrame()->GetMenuBar()->Check(idMenuViewThreadSearch, m_pViewManager->IsViewShown());
}


void ThreadSearch::OnMnuSearchThreadSearchUpdateUI(wxUpdateUIEvent& event)
{
    if ( !IsAttached() )
        return;

    event.Enable(m_pThreadSearchView->IsSearchRunning() == false);
}

void ThreadSearch::OnMnuViewFocusThreadSearchUpdateUI(wxUpdateUIEvent& event)
{
    if ( !IsAttached() )
        return;

    event.Enable(m_pThreadSearchView->IsSearchRunning() == false);
}


void ThreadSearch::BuildModuleMenu(const ModuleType type, wxMenu* pMenu, const FileTreeData* data)
{
    wxMenuItem* pMenuItem = NULL;
    if (!pMenu || !IsAttached())
        return;

    // Triggs editor events if 'Find occurrences' is integrated in context menu
    if ( (type == mtEditorManager) && (m_CtxMenuIntegration == true) )
    {
        // Gets current word
        if ( GetCursorWord(m_SearchedWord) == true )
        {
            wxString sText = _("Find occurrences of: '") + m_SearchedWord + wxT("'");

            // Tries to find the 'Find implementation' item to adds the
            // 'Find occurrences' item just after or appends it at the end
            int dIndex = GetInsertionMenuIndex(pMenu);
            if ( dIndex >= 0 )
            {
                pMenuItem = pMenu->Insert(dIndex, idMenuCtxThreadSearch, sText);
            }
            else
            {
                pMenu->AppendSeparator();
                pMenuItem = pMenu->Append(idMenuCtxThreadSearch, sText);
            }

            // Disables item if a threaded search is running
            pMenuItem->Enable(!m_pThreadSearchView->IsSearchRunning());
        }
    }
}


int ThreadSearch::GetInsertionMenuIndex(const wxMenu* const pCtxMenu)
{
    if ( !IsAttached() )
        return -1;

    // Looks after the "Find implementation of:" menu item
    const wxMenuItemList ItemsList = pCtxMenu->GetMenuItems();
    for (int i = 0; i < (int)ItemsList.GetCount(); ++i)
    {
        #if wxCHECK_VERSION(2, 9, 0)
        if (ItemsList[i]->GetItemLabelText().StartsWith(_("Find implementation of:")) )
        #else
        if (ItemsList[i]->GetLabel().StartsWith(_("Find implementation of:")) )
        #endif
        {
            return ++i;
        }
    }
    return -1;
}


cbConfigurationPanel* ThreadSearch::GetConfigurationPanel(wxWindow* parent)
{
    if ( !IsAttached() )
        return NULL;

    return new ThreadSearchConfPanel(*this, parent);
}


void ThreadSearch::Notify()
{
    if ( !IsAttached() )
        return;

    m_pThreadSearchView->Update();
    SaveConfig(m_pViewManager->IsViewShown(),
               m_pThreadSearchView->GetSashPosition(),
               m_pViewManager->GetManagerType(),
               m_pThreadSearchView->GetSearchHistory());
}


void ThreadSearch::LoadConfig(bool& showPanel, int& sashPosition,
                              ThreadSearchViewManagerBase::eManagerTypes& mgrType,
                              wxArrayString& searchPatterns)
{
    if ( !IsAttached() )
        return;

    ConfigManager* pCfg = Manager::Get()->GetConfigManager(_T("ThreadSearch"));

    m_FindData.SetMatchWord       (pCfg->ReadBool(wxT("/MatchWord"),             true));
    m_FindData.SetStartWord       (pCfg->ReadBool(wxT("/StartWord"),             false));
    m_FindData.SetMatchCase       (pCfg->ReadBool(wxT("/MatchCase"),             true));
    m_FindData.SetRegEx           (pCfg->ReadBool(wxT("/RegEx"),                 false));
    m_FindData.SetHiddenSearch    (pCfg->ReadBool(wxT("/HiddenSearch"),          true));
    m_FindData.SetRecursiveSearch (pCfg->ReadBool(wxT("/RecursiveSearch"),       true));

    m_CtxMenuIntegration         = pCfg->ReadBool(wxT("/CtxMenuIntegration"),    true);
    m_UseDefValsForThreadSearch  = pCfg->ReadBool(wxT("/UseDefaultValues"),      true);
    m_ShowSearchControls         = pCfg->ReadBool(wxT("/ShowSearchControls"),    true);
    m_ShowDirControls            = pCfg->ReadBool(wxT("/ShowDirControls"),       false);
    m_ShowCodePreview            = pCfg->ReadBool(wxT("/ShowCodePreview"),       true);
    m_DeletePreviousResults      = pCfg->ReadBool(wxT("/DeletePreviousResults"), true);
    m_DisplayLogHeaders          = pCfg->ReadBool(wxT("/DisplayLogHeaders"),     true);
    m_DrawLogLines               = pCfg->ReadBool(wxT("/DrawLogLines"),          false);

    showPanel                    = pCfg->ReadBool(wxT("/ShowPanel"),             true);

    m_FindData.SetScope           (pCfg->ReadInt (wxT("/Scope"),                 ScopeProjectFiles));

    m_FindData.SetSearchPath      (pCfg->Read    (wxT("/DirPath"),               wxEmptyString));
    m_FindData.SetSearchMask      (pCfg->Read    (wxT("/Mask"),                  wxT("*.cpp;*.c;*.h")));

    sashPosition                 = pCfg->ReadInt(wxT("/SplitterPosn"),           0);
    int splitterMode             = pCfg->ReadInt(wxT("/SplitterMode"),           wxSPLIT_VERTICAL);
    m_SplitterMode               = wxSPLIT_VERTICAL;
    if ( splitterMode == wxSPLIT_HORIZONTAL )
    {
        m_SplitterMode = wxSPLIT_HORIZONTAL;
    }

    int managerType              = pCfg->ReadInt(wxT("/ViewManagerType"),        ThreadSearchViewManagerBase::TypeMessagesNotebook);
    mgrType                      = ThreadSearchViewManagerBase::TypeMessagesNotebook;
    if ( managerType == ThreadSearchViewManagerBase::TypeLayout )
    {
        mgrType = ThreadSearchViewManagerBase::TypeLayout;
    }

    int loggerType               = pCfg->ReadInt(wxT("/LoggerType"),             ThreadSearchLoggerBase::TypeList);
    m_LoggerType                 = ThreadSearchLoggerBase::TypeList;
    if ( loggerType == ThreadSearchLoggerBase::TypeTree )
    {
        m_LoggerType = ThreadSearchLoggerBase::TypeTree;
    }

    searchPatterns = pCfg->ReadArrayString(wxT("/SearchPatterns"));
}


void ThreadSearch::SaveConfig(bool showPanel, int sashPosition,
                              ThreadSearchViewManagerBase::eManagerTypes mgrType,
                              const wxArrayString& searchPatterns)
{
    ConfigManager* pCfg = Manager::Get()->GetConfigManager(_T("ThreadSearch"));

    pCfg->Write(wxT("/MatchWord"),             m_FindData.GetMatchWord());
    pCfg->Write(wxT("/StartWord"),             m_FindData.GetStartWord());
    pCfg->Write(wxT("/MatchCase"),             m_FindData.GetMatchCase());
    pCfg->Write(wxT("/RegEx"),                 m_FindData.GetRegEx());
    pCfg->Write(wxT("/HiddenSearch"),          m_FindData.GetHiddenSearch());
    pCfg->Write(wxT("/RecursiveSearch"),       m_FindData.GetRecursiveSearch());

    pCfg->Write(wxT("/CtxMenuIntegration"),    m_CtxMenuIntegration);
    pCfg->Write(wxT("/UseDefaultValues"),      m_UseDefValsForThreadSearch);
    pCfg->Write(wxT("/ShowSearchControls"),    m_ShowSearchControls);
    pCfg->Write(wxT("/ShowDirControls"),       m_ShowDirControls);
    pCfg->Write(wxT("/ShowCodePreview"),       m_ShowCodePreview);
    pCfg->Write(wxT("/DeletePreviousResults"), m_DeletePreviousResults);
    pCfg->Write(wxT("/DisplayLogHeaders"),     m_DisplayLogHeaders);
    pCfg->Write(wxT("/DrawLogLines"),          m_DrawLogLines);

    pCfg->Write(wxT("/ShowPanel"),             showPanel);

    pCfg->Write(wxT("/Scope"),                 m_FindData.GetScope());

    pCfg->Write(wxT("/DirPath"),               m_FindData.GetSearchPath());
    pCfg->Write(wxT("/Mask"),                  m_FindData.GetSearchMask());

    pCfg->Write(wxT("/SplitterPosn"),          sashPosition);
    pCfg->Write(wxT("/SplitterMode"),          (int)m_SplitterMode);
    pCfg->Write(wxT("/ViewManagerType"),       m_pViewManager->GetManagerType());
    pCfg->Write(wxT("/LoggerType"),            m_LoggerType);
    pCfg->Write(wxT("/FileSorting"),           m_FileSorting);

    pCfg->Write(wxT("/SearchPatterns"),        searchPatterns);
}


bool ThreadSearch::BuildToolBar(wxToolBar* toolBar)
{
    if ( !IsAttached() || !toolBar )
        return false;

    m_pToolbar = toolBar;
    m_pThreadSearchView->SetToolBar(toolBar);

    wxString prefix;
    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("app"));
    if (cfg->ReadBool(_T("/environment/toolbar_size"),true))
    {
        prefix = ConfigManager::GetDataFolder() + _T("/images/ThreadSearch/16x16/");
        m_pToolbar->SetToolBitmapSize(wxSize(16,16));
    }
    else
    {
        prefix = ConfigManager::GetDataFolder() + _T("/images/ThreadSearch/22x22/");
        m_pToolbar->SetToolBitmapSize(wxSize(22,22));
    }

    m_pCboSearchExpr = new wxComboBox(toolBar, idCboSearchExpr, wxEmptyString, wxDefaultPosition,
                                      wxSize(130, -1), 0, NULL, wxCB_DROPDOWN);
    m_pCboSearchExpr->SetToolTip(_("Text to search"));

    toolBar->AddControl(m_pCboSearchExpr);
    toolBar->AddTool(idBtnSearch,_(""),wxBitmap(prefix + wxT("findf.png"), wxBITMAP_TYPE_PNG),wxBitmap(prefix + wxT("findfdisabled.png"), wxBITMAP_TYPE_PNG),wxITEM_NORMAL,_("Run search")); //Control(pBtnSearch);
    toolBar->AddTool(idBtnOptions,_(""),wxBitmap(prefix + wxT("options.png"), wxBITMAP_TYPE_PNG),wxBitmap(prefix + wxT("optionsdisabled.png"), wxBITMAP_TYPE_PNG),wxITEM_NORMAL,_("Show options window")); //Control(pBtnSearch);

    toolBar->Realize();
    #if wxCHECK_VERSION(2, 8, 0)
    toolBar->SetInitialSize();
    #else
    toolBar->SetBestFittingSize();
    #endif

    return true;
}


void ThreadSearch::OnBtnOptionsClick(wxCommandEvent &event)
{
    if ( !IsAttached() )
        return;

    m_pThreadSearchView->OnBtnOptionsClick(event);
}


void ThreadSearch::OnBtnSearchClick(wxCommandEvent &event)
{
    if ( !IsAttached() )
        return;

    // Behaviour differs if a search is running.
    if ( m_pThreadSearchView->IsSearchRunning() )
    {
        // In this case, user wants to stops search,
        // we just transmit event
        m_pThreadSearchView->OnBtnSearchClick(event);

    }
    else
    {
        // User wants to search for a word.
        // Forwarding the event would search for the view combo text whereas we want
        // to look for the toolbar combo text.
        wxComboBox* pCboBox = static_cast<wxComboBox*>(m_pToolbar->FindControl(idCboSearchExpr));
        wxASSERT(pCboBox != NULL);
        RunThreadSearch(pCboBox->GetValue());
    }
}

void ThreadSearch::RunThreadSearch(const wxString& text, bool isCtxSearch/*=false*/)
{
    if ( !IsAttached() )
        return;

    ThreadSearchFindData findData = m_FindData;

    // User may prefer to set default options for contextual search
    if ( (isCtxSearch == true) && (m_UseDefValsForThreadSearch == true) )
    {
        findData.SetMatchCase(true);
        findData.SetMatchWord(true);
        findData.SetStartWord(false);
        findData.SetRegEx    (false);
    }

    // m_SearchedWord was set in BuildModuleMenu
    findData.SetFindText(text);

    // Displays m_pThreadSearchView in manager
    m_pViewManager->ShowView(true);

    // Runs the search through a worker thread
    m_pThreadSearchView->ThreadedSearch(findData);
}


void ThreadSearch::OnCboSearchExprEnter(wxCommandEvent &event)
{
    if ( !IsAttached() )
        return;

    // Event handler used when user clicks on enter after typing
    // in combo box text control.
    // Runs a multi threaded search with combo text
    wxComboBox* pCboBox = static_cast<wxComboBox*>(m_pToolbar->FindControl(idCboSearchExpr));
    wxASSERT(pCboBox != NULL);
    if ( event.GetEventType() == wxEVT_COMMAND_TEXT_ENTER )
        RunThreadSearch(pCboBox->GetValue());
}


void ThreadSearch::ShowToolBar(bool show)
{
    if ( !IsAttached() )
        return;

    bool isShown = IsWindowReallyShown(m_pToolbar);

    if ( show != isShown )
    {
        CodeBlocksDockEvent evt(show ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
        evt.pWindow = (wxWindow*)m_pToolbar;
        evt.shown = show;
        Manager::Get()->ProcessEvent(evt);
    }
}


bool ThreadSearch::IsToolbarVisible()
{
    if ( !IsAttached() )
        return false;

    return IsWindowReallyShown(m_pToolbar);
}


bool ThreadSearch::GetCursorWord(wxString& sWord)
{
    bool wordFound = false;
    sWord = wxEmptyString;

    // Gets active editor
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if ( ed != NULL )
    {
        cbStyledTextCtrl* control = ed->GetControl();

        sWord = control->GetSelectedText();
        if (sWord != wxEmptyString)
        {
            sWord.Trim(true);
            sWord.Trim(false);

            wxString::size_type pos = sWord.find(wxT('\n'));
            if (pos != wxString::npos)
            {
                sWord.Remove(pos, sWord.length() - pos);
                sWord.Trim(true);
                sWord.Trim(false);
            }

            return sWord;
        }

        // Gets word under cursor
        int pos = control->GetCurrentPos();
        int ws  = control->WordStartPosition(pos, true);
        int we  = control->WordEndPosition(pos, true);
        const wxString word = control->GetTextRange(ws, we);
        if (!word.IsEmpty()) // Avoid empty strings
        {
            sWord.Clear();
            while (--ws > 0)
            {
                const wxChar ch = control->GetCharAt(ws);
                if (ch <= _T(' '))
                    continue;
                else if (ch == _T('~'))
                    sWord << _T("~");
                break;
            }
            // m_SearchedWord will be used if 'Find occurrences' ctx menu is clicked
            sWord << word;
            wordFound = true;
        }
    }

    return wordFound;
}


void ThreadSearch::OnMnuEditCopy(wxCommandEvent& event)
{
       if ( !IsAttached() )
    {
        event.Skip();
        return;
    }

    wxWindow* pFocused = wxWindow::FindFocus();

    // if the following window have the focus, own the copy.
    if ( pFocused == m_pCboSearchExpr )
    {
        if ( m_pCboSearchExpr->CanCopy() )
            m_pCboSearchExpr->Copy();
        LOGIT( _T("OnMnuEditcopy for m_pCboSearchExpr") );
    }
    else if ( pFocused == m_pThreadSearchView->m_pCboSearchExpr )
    {
        if ( m_pThreadSearchView->m_pCboSearchExpr->CanCopy() )
            m_pThreadSearchView->m_pCboSearchExpr->Copy();
        LOGIT( _T("OnMnuEditcopy for m_pThreadSearchView->m_pCboSearchExpr") );
    }
    else if ( pFocused == static_cast<wxWindow*>(m_pThreadSearchView->m_pSearchPreview) )
    {
        bool hasSel = m_pThreadSearchView->m_pSearchPreview->GetSelectionStart() != m_pThreadSearchView->m_pSearchPreview->GetSelectionEnd();
        if (hasSel)
            m_pThreadSearchView->m_pSearchPreview->Copy();
        LOGIT( _T("OnMnuEditcopy for m_pSearchPreview") );
    }
    else
    {
        event.Skip();
    }

    // If you Skip(), CB main.cpp will wrongly paste your text into the current editor
    // because CB main.cpp thinks it owns the clipboard.
    //- event.Skip();
    return; //own the event
}


void ThreadSearch::OnMnuEditCopyUpdateUI(wxUpdateUIEvent& event)
{
    if ( !IsAttached() )
    {
        event.Skip(); return;
    }

    wxWindow* pFocused = wxWindow::FindFocus();
    if (not pFocused) return;

    wxMenuBar* mbar = Manager::Get()->GetAppFrame()->GetMenuBar();
    if (not mbar) return;

    bool hasSel = false;
    // if the following window have the focus, own the copy.
    if ( pFocused == m_pCboSearchExpr )
    {
        //event.Enable(m_pCboSearchExpr->CanCopy());
        hasSel =  m_pCboSearchExpr->CanCopy() ;
        //LOGIT( _T("OnMnuEditCopyUpdateUI m_pCboSearchExpr") );
    }
    else if ( pFocused == m_pThreadSearchView->m_pCboSearchExpr )
    {
        //event.Enable(m_pThreadSearchView->m_pCboSearchExpr->CanCopy());
        hasSel = m_pThreadSearchView->m_pCboSearchExpr->CanCopy();
        //LOGIT( _T("OnMnuEditCopyUpdateUI m_pThreadSearchView->m_pCboSearchExpr") );
    }
    else if ( pFocused == static_cast<wxWindow*>(m_pThreadSearchView->m_pSearchPreview) )
    {
        hasSel = m_pThreadSearchView->m_pSearchPreview->GetSelectionStart() != m_pThreadSearchView->m_pSearchPreview->GetSelectionEnd();
        //LOGIT( _T("OnMnuEditCopyUpdateUI m_pSearchPreview") );
    }
    if ( hasSel )
    {
        mbar->Enable(idMenuEditCopy, hasSel);
        wxToolBar* pMainToolBar = (wxToolBar*) ::wxFindWindowByName(wxT("toolbar"), NULL);
        if (pMainToolBar) pMainToolBar->EnableTool(idMenuEditCopy, hasSel);
        return;
    }

    event.Skip();
    return;

}
// ----------------------------------------------------------------------------
void ThreadSearch::OnMnuEditPaste(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // Process clipboard data only if we have the focus

    // ----------------------------------------------------------------
    // NB:  A bug in CB main.cpp causes a ctrl-v to always paste into the
    //      current editor. Here, we'll make checks to see if the paste
    //      is for our search combo boxes and paste the data there.
    //      If the focused window is one of ours that shouldn't get pasted
    //      data, we'll simply ignore it.
    //      If the window isn't one of ours, we'll event.Skip();
    // ----------------------------------------------------------------

       if ( !IsAttached() )
        { event.Skip(); return; }

    if (not m_IsAttached) {event.Skip(); return;}

    wxWindow* pFocused = wxWindow::FindFocus();
    if (not pFocused) { event.Skip(); return; }

    wxString focusedStr = pFocused->GetName();
//    DBGLOG(wxT("OnMnuEditPaste:Focused[%p][%s]"), pFocused, focusedStr.c_str());

    // don't allow paste when the following windows have the focus
    if ( (pFocused == m_pThreadSearchView->m_pSearchPreview) ||
         (pFocused == (wxWindow*)m_pThreadSearchView->m_pLogger) )
    {
        return;
    }

    // if the following window have the focus, own the paste.
    if ( (pFocused != m_pCboSearchExpr)
        && (pFocused != m_pThreadSearchView->m_pCboSearchExpr) )
        { event.Skip(); return;}

    if (pFocused == m_pCboSearchExpr)
        m_pCboSearchExpr->Paste();
    if (pFocused == m_pThreadSearchView->m_pCboSearchExpr)
        m_pThreadSearchView->m_pCboSearchExpr->Paste();

    // If you Skip(), CB main.cpp will wrongly paste your text into the current editor
    // because CB main.cpp thinks it owns the clipboard.
    //- event.Skip();
    return; //own the event
}//OnMnuEditPaste


void ThreadSearch::SetManagerType(ThreadSearchViewManagerBase::eManagerTypes mgrType)
{
    // Is type different from current one ?
    if ( mgrType != m_pViewManager->GetManagerType() )
    {
        // Get show state and destroy current view manager.
        bool show(true);
        if ( m_pViewManager != NULL )
        {
            show = m_pViewManager->IsViewShown();
            m_pViewManager->RemoveViewFromManager();
            delete m_pViewManager;
        }

        // Create and show new view manager.
        m_pViewManager = ThreadSearchViewManagerBase::BuildThreadSearchViewManagerBase(m_pThreadSearchView, true, mgrType);
        m_pViewManager->ShowView(show);
    }
}





/***************************************************************
 * Name:      ThreadSearch
 * Purpose:   ThreadSearch Code::Blocks plugin
 *            Most of the interactions with C::B are handled here.
 * Author:    Jerome ANTOINE
 * Created:   2007-10-08
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#if defined(CB_PRECOMP)
    #include "sdk.h"
#endif

#ifndef CB_PRECOMP
    #include <wx/bmpbuttn.h>
    #include <wx/combobox.h>
    #include <wx/menu.h>
    #include <wx/toolbar.h>
    #include <wx/xrc/xmlres.h>

    #include "configmanager.h"
    #include "sdk_events.h"
#endif

#include "prep.h"
#include "cbstyledtextctrl.h"
#include "ThreadSearch.h"
#include "ThreadSearchView.h"
#include "ThreadSearchConfPanel.h"
#include "ThreadSearchControlIds.h"
#include "cbauibook.h"
#include "version.h" //(pecan 2008/2/28)
#include "MainPanel.h"
#include "codesnippetsevent.h"
#include "snippetsconfig.h"
#include "scbeditor.h"
#include "seditormanager.h"
#include "editormanager.h"
#include "manager.h"

// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
#if !wxCHECK_VERSION(3, 0, 0)
    // SashSize can not be changed in wx>=2.9
    int m_nSashSize = 2;
#endif
    // -- next stmt commented because we're only emulating a plugin --
    //-PluginRegistrant<ThreadSearch> reg(_T("ThreadSearch"));
}

// ----------------------------------------------------------------------------
// CodeBlocks main.cpp managers all the edit EVT_UPATE_UI entires in one routine.
// So if only one changes, all will change. Therefore, to enable/disable copy/paste,
// we have to capture the entire edit UPDATE_UI range to see if it really belongs to us
// ----------------------------------------------------------------------------
int idEditUndo = XRCID("idEditUndo");                   //First main.cpp UPDATE_UI id
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
int idEditAutoComplete = XRCID("idEditAutoComplete");   //Last main.cpp UPDATE_UI id
// ----------------------------------------------------------------------------

int idMenuEditCopy = XRCID("idEditCopy");
int idMenuEditPaste = XRCID("idEditPaste");

// ----------------------------------------------------------------------------
// events handling
// ----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(ThreadSearch, cbPlugin)
    // add any events you want to handle here

    // Update UI "View/Search/Options" not used in CodeSnippets
    //-EVT_UPDATE_UI (idMenuViewThreadSearch,   ThreadSearch::OnMnuViewThreadSearchUpdateUI)

    //-EVT_MENU      (idMenuViewThreadSearch,   ThreadSearch::OnMnuViewThreadSearch)
    EVT_UPDATE_UI (idMenuSearchThreadSearch, ThreadSearch::OnMnuSearchThreadSearchUpdateUI)
    EVT_MENU      (idMenuSearchThreadSearch, ThreadSearch::OnMnuSearchThreadSearch)
    EVT_MENU      (idMenuCtxThreadSearch,    ThreadSearch::OnCtxThreadSearch)
    EVT_MENU      (idMenuEditCopy,           ThreadSearch::OnMnuEditCopy)
    EVT_UPDATE_UI (idMenuEditCopy,           ThreadSearch::OnMnuEditCopyUpdateUI)
    EVT_MENU      (idMenuEditPaste,          ThreadSearch::OnMnuEditPaste)
    EVT_BUTTON    (idBtnOptions,             ThreadSearch::OnBtnOptionsClick)
    EVT_BUTTON    (idBtnSearch,              ThreadSearch::OnBtnSearchClick)
    EVT_TEXT_ENTER(idCboSearchExpr,          ThreadSearch::OnCboSearchExprEnter)
    EVT_TEXT      (idCboSearchExpr,          ThreadSearch::OnCboSearchExprEnter)
    EVT_SPLITTER_SASH_POS_CHANGED(wxID_ANY,  ThreadSearch::OnSashPositionChanged)

    EVT_CODESNIPPETS_NEW_INDEX (wxID_ANY,    ThreadSearch::OnCodeSnippetsNewIndex)
    // ---------------------------------------------------------------------------
    // CodeBlocks main.cpp manages all the following UI entires in ONE routine.
    // So if only one changes, all may change.
    // Therefore, to enable/disable copy/paste, we have to capture the entire range
    // to see if the event actually belongs to us.
    // ---------------------------------------------------------------------------
    // do not use range here, because we don't know which Id is greater , so we (can?) run into
    // an assert in debug mode (at least on wx2.9)
//    EVT_UPDATE_UI_RANGE(idEditUndo,idEditAutoComplete, ThreadSearch::OnMnuEditCopyUpdateUI)
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
// ----------------------------------------------------------------------------
ThreadSearch::ThreadSearch(wxWindow* parent)
// ----------------------------------------------------------------------------
             :m_SearchedWord(wxEmptyString),
              m_pThreadSearchView(NULL),
              m_pViewManager(NULL),
              m_pToolbar(NULL),
              m_CtxMenuIntegration(true),
              m_UseDefValsForThreadSearch(true),
              m_ShowSearchControls(true),
              m_ShowDirControls(false),
              m_ShowCodePreview(true),
              m_LoggerType(ThreadSearchLoggerBase::TypeList),
              m_DisplayLogHeaders(true),
              m_DrawLogLines(false),
              m_pCboSearchExpr(0),
              m_SplitterMode(wxSPLIT_VERTICAL),
              m_FileSorting(InsertIndexManager::SortByFilePath)
{
    // -- following removed-- because We're only emulating a plugin
    // Make sure our resources are available.
    // In the generated boilerplate code we have no resources but when
    // we add some, it will be nice that this code is in place already ;)
    // ThreadSearch plugin has no resources in zip
    ////if(!Manager::LoadResource(_T("ThreadSearch.zip")))
    ////{
    ////    //-NotifyMissingFile(_T("ThreadSearch.zip"));
    ////}
    m_pParent =  parent ;   //parent is ThreadSearchFrame //(pecan 2008/4/03)
    m_CodeSnippetsIndexFilename = wxEmptyString;
    GetConfig()->SetThreadSearchPlugin( this );
    m_EdNotebookSashPosition = 0;
    m_pMainPanel = 0;
    m_bSashWindowResizing = false;

}//ctor
// ----------------------------------------------------------------------------
ThreadSearch::~ThreadSearch()
// ----------------------------------------------------------------------------
{
    //dtor
    GetConfig()->SetThreadSearchPlugin(0);
}
// ----------------------------------------------------------------------------
void ThreadSearch::OnAttach()
// ----------------------------------------------------------------------------
{
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be TRUE...
    // You should check for it in other functions, because if it
    // is FALSE, it means that the application did *not* "load"
    // (see: does not need) this plugin...

    m_pThreadSearchViewParentBak = 0;
    m_pEdNotebookParentBak = 0;

    //-m_pAppWindow = Manager::Get()->GetAppWindow();
    //-if ( not m_pAppWindow ) m_pAppWindow = wxTheApp->GetTopWindow();

    #if LOGGING
     LOGIT( _T("SnippetsSearch (ThreadSearch) Plugin Logging Started"));
    #endif

    bool showPanel;
    int  sashPosition;
    ThreadSearchViewManagerBase::eManagerTypes mgrType;
    wxArrayString searchPatterns;

    // Loads configuration from default.conf
    LoadConfig(showPanel, sashPosition, mgrType, searchPatterns);

    // Adds window to the manager
    m_pThreadSearchView = new ThreadSearchView( *this);
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

    // new main panel with splitter window      //(pecan 2008/3/08)
    m_pMainPanel = new MainPanel( m_pParent );
    m_pMainSplitter = m_pMainPanel->m_pSplitterWindow;
    m_pMainSplitter->SetMinimumPaneSize( 20 ); // user cannot now unsplit
    m_pMainSizer = new wxBoxSizer( wxVERTICAL );
    m_pMainSizer->Add(m_pMainPanel, 1, wxEXPAND|wxALL, 1 );
    m_pParent->SetSizer( m_pMainSizer);
    m_pParent->Layout();


    //(pecan 2008/3/05)
    // Reparent ThreadSearchView and cbEditor Notebook and add
    // them to the app splitter window. OnRelease() will reverse
    // the process to avoid wxWidgets.Destroy() crashes.
    // The old panels must be Show(false) or they'll get drawn over
    // the new panels on wierd situations( esp. with OS screen managers);
    // Adding a wxSIMPLE_BORDER seems to be the only type the ThreadSearch and
    // Notebook will exhibit. Better than nothing.
    m_pThreadSearchViewParentBak = m_pThreadSearchView->GetParent();
    m_pThreadSearchView->SetWindowStyle(m_pThreadSearchView->GetWindowStyle()|wxSIMPLE_BORDER);
    m_pThreadSearchView->Reparent( m_pMainSplitter );
    m_pMainPanel->m_pSplitterWindow->ReplaceWindow( m_pMainPanel->m_pSearchPanel, m_pThreadSearchView);
    m_pMainPanel->m_pSearchPanel->Show(false);

    //-m_pEdNotebook = (wxWindow*)(Manager::Get()->GetEditorManager()->GetNotebook());
    m_pEdNotebook = GetConfig()->GetEditorManager(m_pParent)->GetNotebook();
    //if( GetConfig()->IsApplication() )
    {
        m_pEdNotebookParentBak = m_pEdNotebook->GetParent();
        m_pEdNotebook->SetWindowStyle(m_pEdNotebook->GetWindowStyle()|wxSIMPLE_BORDER);
        m_pEdNotebook->Reparent( m_pMainSplitter );
        m_pMainPanel->m_pSplitterWindow->ReplaceWindow( m_pMainPanel->m_pNotebkPanel, m_pEdNotebook );
        m_pMainPanel->m_pNotebkPanel->Show(false);  //Hide this for now
    }

    m_pMainPanel->m_pSplitterWindow->SetSashPosition( 0 );
    m_pMainPanel->m_pSplitterWindow->SetSashGravity(0.3);
    // The sash size gets set initially, but when the user moves it, it gets set
    // to one line only. I've found no solution.
#if !wxCHECK_VERSION(3, 0, 0)
    // deprected in wx2.9 and does nothing atall (at least there)
    m_pMainPanel->m_pSplitterWindow->SetSashSize(m_nSashSize);
#endif
        ////// nogo on the sash size adjustment
        ////m_pMainPanel->m_pSplitterWindow->Connect(wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED,
        ////            (wxObjectEventFunction)(wxEventFunction)(wxSplitterEventFunction)
        ////            &ThreadSearch::OnSashPositionChanged, NULL, this);
        ////m_pMainPanel->m_pSplitterWindow->Connect(wxEVT_IDLE,
        ////            (wxObjectEventFunction)(wxEventFunction)(wxIdleEventFunction)
        ////            &ThreadSearch::OnIdle, NULL, this);
    m_pMainPanel->m_pSplitterWindow->Unsplit();

    //(pecan 2008/3/05)
    ////    // Testing a flat sizer version of ThreadSearchView for bug comparison
    ////    // Add a sizer so that ThreadSearch panel is resizable within a
    ////    // stand alone frame.
    ////    m_pMainSizer = new wxBoxSizer( wxVERTICAL );
    ////    m_pMainSizer->Add((wxPanel*)m_pThreadSearchView, 1, wxEXPAND );
    ////    Manager::Get()->GetAppFrame()->SetSizer( m_pMainSizer );
    ////    //-m_pMainSizer->Add((wxPanel*)(Manager::Get()->GetEditorManager()->GetNotebook()), 1, wxEXPAND );
    ////    m_pMainSizer->Layout();

    // Re-Set the ThreadSearchView splitter posn from the config
    // It got messed up by the full window UnSplit() above
    if (sashPosition != 0)
        m_pThreadSearchView->SetSashPosition(sashPosition);

    //(pecan 2008/3/06)
    // code from loggers.cpp:188 to set font size etc.
    int size = Manager::Get()->GetConfigManager(_T("message_manager"))->ReadInt(_T("/log_font_size"), platform::macosx ? 10 : 8);
    bool fixedPitchFont = true; //<= temporary hack
    wxFont default_font(size, fixedPitchFont ? wxFONTFAMILY_MODERN : wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    m_Conf_font = default_font;
        // unused for now
        //wxFont bold_font(default_font);
        //wxFont italic_font(default_font);
        //bold_font.SetWeight(wxFONTWEIGHT_BOLD);
        //wxFont bigger_font(bold_font);
        //bigger_font.SetPointSize(size + 2);
        //wxFont small_font(default_font);
        //small_font.SetPointSize(size - 4);
        //italic_font.SetStyle(wxFONTSTYLE_ITALIC);
    //(pecan 2008/3/06)

    // true if it enters in OnRelease for the first time
    m_OnReleased = false;
}

// ----------------------------------------------------------------------------
void ThreadSearch::OnRelease(bool /*appShutDown*/)
// ----------------------------------------------------------------------------
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

    // If we re-parented ThreadSearchView split window, reverse it //(pecan 2008/3/13)
    if ( m_pThreadSearchViewParentBak )
    {
        m_pThreadSearchView->Reparent(m_pThreadSearchViewParentBak);
        m_pMainPanel->m_pSplitterWindow->ReplaceWindow( m_pThreadSearchView, m_pMainPanel->m_pSearchPanel);
    }
    if( m_pEdNotebookParentBak )
    {
        m_pEdNotebook->Reparent ( m_pEdNotebookParentBak );
        m_pMainPanel->m_pSplitterWindow->ReplaceWindow( m_pEdNotebook, m_pMainPanel->m_pNotebkPanel);
    }
    if (m_pMainPanel)
        m_pMainPanel->Destroy();

    // Removes Thread search menu item from the View menu
    RemoveMenuItems();

    m_pToolbar = NULL;

    if ( m_pThreadSearchView != NULL )
    {
        m_pViewManager->RemoveViewFromManager();
        m_pThreadSearchView->Destroy();
    }

    delete m_pViewManager;
    m_pViewManager = NULL;
}


// ----------------------------------------------------------------------------
void ThreadSearch::OnThreadSearchViewDestruction()
// ----------------------------------------------------------------------------
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


// ----------------------------------------------------------------------------
void ThreadSearch::BuildMenu(wxMenuBar* menuBar)
// ----------------------------------------------------------------------------
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
                //-menu->InsertCheckItem(i, idMenuViewThreadSearch, wxT("Snippets search"),
                //-                      wxT("Toggle displaying the 'Snippets search' panel"));
                menu->Insert(i, idMenuViewThreadSearch, wxT("Snippets search"),
                                      wxT("Toggle displaying the 'Snippets search' panel"));
                break;
            }
        }

        if ( i == items.GetCount() )
        {
            // not found, just append
            //-menu->AppendCheckItem(idMenuViewThreadSearch, wxT("Snippets search"),
            //-                      wxT("Toggle displaying the 'Snippets search' panel"));
            menu->Append(idMenuViewThreadSearch, wxT("Snippets search"),
                                  wxT("Toggle displaying the 'Snippets search' panel"));
        }
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
                //-menu->Insert(i, idMenuSearchThreadSearch, wxT("Snippets search"),
                //-                wxT("Perform a search with the current word"));
                //-menu->InsertSeparator(i);
                break;
            }
        }

        if ( i == items.GetCount() )
        {
            // not found, just append
            //-menu->Append(idMenuSearchThreadSearch, wxT("Snippets search"),
            //-                wxT("Perform a search with the current word"));
            //-menu->AppendSeparator();
        }
    }
}

// ----------------------------------------------------------------------------
void ThreadSearch::RemoveMenuItems()
// ----------------------------------------------------------------------------
{
    // Removes 'Thread search' item from View and Search menu
    wxMenuBar* menuBar = Manager::Get()->GetAppFrame()->GetMenuBar();
    int idx = menuBar->FindMenu(_("&View"));
    if (idx != wxNOT_FOUND)
    {
        wxMenu* viewMenu = menuBar->GetMenu(idx);
        if ( viewMenu != NULL )
        {
            viewMenu->Remove(idMenuViewThreadSearch);
        }
    }

    idx = menuBar->FindMenu(_("Sea&rch"));
    if (idx != wxNOT_FOUND)
    {
        wxMenu* searchMenu = menuBar->GetMenu(idx);
        if ( searchMenu != NULL )
        {
            searchMenu->Remove(idMenuSearchThreadSearch);
        }
    }
}


// ----------------------------------------------------------------------------
void ThreadSearch::OnMnuViewThreadSearch(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    if ( !IsAttached() )
        return;

    m_pViewManager->ShowView(event.IsChecked());
}


// ----------------------------------------------------------------------------
void ThreadSearch::OnMnuSearchThreadSearch(wxCommandEvent& WXUNUSED(event))
// ----------------------------------------------------------------------------
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
        // Word is OK, just show the panel
        m_pViewManager->ShowView(true);
    }
}


// ----------------------------------------------------------------------------
void ThreadSearch::OnCtxThreadSearch(wxCommandEvent& WXUNUSED(event))
// ----------------------------------------------------------------------------
{
    if ( !IsAttached() )
        return;

    // m_SearchedWord was set in BuildModuleMenu
    RunThreadSearch(m_SearchedWord, true);
}


// ----------------------------------------------------------------------------
void ThreadSearch::OnMnuViewThreadSearchUpdateUI(wxUpdateUIEvent& WXUNUSED(event))
// ----------------------------------------------------------------------------
{
    // UpdateUI on "View/Search/Options" not used in CodeSnippets
    // CodeSnippets uses View/Options to invoke ThreadSearch configuration
    // See: ThreadSearchFrame::InitThreadSearchFrame()
    return;

    if ( !IsAttached() )
        return;

    Manager::Get()->GetAppFrame()->GetMenuBar()->Check(idMenuViewThreadSearch, m_pViewManager->IsViewShown());
}
// ----------------------------------------------------------------------------
void ThreadSearch::OnMnuSearchThreadSearchUpdateUI(wxUpdateUIEvent& event)
// ----------------------------------------------------------------------------
{
    if ( !IsAttached() )
        return;

    event.Enable(m_pThreadSearchView->IsSearchRunning() == false);
}
// ----------------------------------------------------------------------------
void ThreadSearch::BuildModuleMenu(const ModuleType type, wxMenu* pMenu, const FileTreeData* /*data*/)
// ----------------------------------------------------------------------------
{
    wxMenuItem* pMenuItem = NULL;
    if (!pMenu || !IsAttached())
        return;

    // Triggs editor events if 'Find occurrences' is integrated in context menu
    if ( (type == mtEditorManager) && (m_CtxMenuIntegration == true) )
    {
        // Gets current word
        if ( GetCursorWord(m_SearchedWord) == true )
        {   //(pecan 2008/2/23)
            wxString srchString = m_SearchedWord.Mid(0,16);
            if ( m_SearchedWord.Length() > 16 ) srchString << _T("...");
            //-wxString sText = wxT("Find occurrences of: '") + m_SearchedWord + wxT("'");
            wxString sText = wxT("Find occurrences of: '") + srchString + wxT("'");
            //(pecan 2008/2/23)

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


// ----------------------------------------------------------------------------
int ThreadSearch::GetInsertionMenuIndex(const wxMenu* const pCtxMenu)
// ----------------------------------------------------------------------------
{
    if ( !IsAttached() )
        return -1;

    // Looks after the "Find implementation of:" menu item
    const wxMenuItemList ItemsList = pCtxMenu->GetMenuItems();
    for (int i = 0; i < (int)ItemsList.GetCount(); ++i)
    {
        #if wxCHECK_VERSION(3, 0, 0)
        if (ItemsList[i]->GetItemLabelText().StartsWith(_T("Find implementation of:")) )
        #else
        if (ItemsList[i]->GetLabel().StartsWith(_T("Find implementation of:")) )
        #endif
        {
            return ++i;
        }
    }
    return -1;
}


// ----------------------------------------------------------------------------
cbConfigurationPanel* ThreadSearch::GetConfigurationPanel(wxWindow* parent)
// ----------------------------------------------------------------------------
{
    if ( !IsAttached() )
        return NULL;

    return new ThreadSearchConfPanel(*this, parent);
}


// ----------------------------------------------------------------------------
void ThreadSearch::Notify()
// ----------------------------------------------------------------------------
{
    // Issued from ThreadSearchConfPanel::OnApply()

    if ( !IsAttached() )
        return;

    // Clear the logger list/tree since user changed options //(pecan 2008/5/05)
    m_pThreadSearchView->Clear();

    m_pThreadSearchView->Update();
    SaveConfig(m_pViewManager->IsViewShown(),
               m_pThreadSearchView->GetSashPosition(),
               m_pViewManager->GetManagerType(),
               m_pThreadSearchView->GetSearchHistory());

    // if User activated "Search In Snippets Tree", refresh FileLinks array
    // else release any previous array memory
    if ( m_FindData.MustSearchInProject() == true )
    {
        // Tell CodeSnippetsTreeCtrl to fill global fileLink array with FileLinks
        CodeSnippetsEvent evt(wxEVT_CODESNIPPETS_GETFILELINKS, 0);
        //-evt.SetSnippetString(_T("Testing GetFileLinksMapArray"));
        evt.ProcessCodeSnippetsEvent(evt);
    }
    else
        GetConfig()->GetFileLinksMapArray().clear();
}


// ----------------------------------------------------------------------------
void ThreadSearch::LoadConfig(bool& showPanel, int& sashPosition,
// ----------------------------------------------------------------------------
                              ThreadSearchViewManagerBase::eManagerTypes& mgrType,
                              wxArrayString& searchPatterns)
{
    if ( !IsAttached() )
        return;

    ConfigManager* pCfg = Manager::Get()->GetConfigManager(_T("SnippetsSearch"));

    m_FindData.SetMatchWord       (pCfg->ReadBool(wxT("/MatchWord"),          true));
    m_FindData.SetStartWord       (pCfg->ReadBool(wxT("/StartWord"),          false));
    m_FindData.SetMatchCase       (pCfg->ReadBool(wxT("/MatchCase"),          true));
    m_FindData.SetRegEx           (pCfg->ReadBool(wxT("/RegEx"),              false));
    m_FindData.SetHiddenSearch    (pCfg->ReadBool(wxT("/HiddenSearch"),       true));
    m_FindData.SetRecursiveSearch (pCfg->ReadBool(wxT("/RecursiveSearch"),    true));

    m_CtxMenuIntegration         = pCfg->ReadBool(wxT("/CtxMenuIntegration"), true);
    m_UseDefValsForThreadSearch  = pCfg->ReadBool(wxT("/UseDefaultValues"),   true);
    m_ShowSearchControls         = pCfg->ReadBool(wxT("/ShowSearchControls"), true);
    m_ShowDirControls            = pCfg->ReadBool(wxT("/ShowDirControls"),    false);
    m_ShowCodePreview            = pCfg->ReadBool(wxT("/ShowCodePreview"),    true);
    m_DisplayLogHeaders          = pCfg->ReadBool(wxT("/DisplayLogHeaders"),  true);
    m_DrawLogLines               = pCfg->ReadBool(wxT("/DrawLogLines"),       false);

    showPanel                    = pCfg->ReadBool(wxT("/ShowPanel"),          true);

    m_FindData.SetScope           (pCfg->ReadInt (wxT("/Scope"),              ScopeSnippetFiles));

    m_FindData.SetSearchPath      (pCfg->Read    (wxT("/DirPath"),            wxEmptyString));
    m_FindData.SetSearchMask      (pCfg->Read    (wxT("/Mask"),               wxT("*.cpp;*.c;*.h")));

    sashPosition                 = pCfg->ReadInt(wxT("/SplitterPosn"),        0);
    int splitterMode             = pCfg->ReadInt(wxT("/SplitterMode"),        wxSPLIT_VERTICAL);
    m_SplitterMode               = wxSPLIT_VERTICAL;
    if ( splitterMode == wxSPLIT_HORIZONTAL )
    {
        m_SplitterMode = wxSPLIT_HORIZONTAL;
    }

    int managerType              = pCfg->ReadInt(wxT("/ViewManagerType"),     ThreadSearchViewManagerBase::TypeMessagesNotebook);
    mgrType                      = ThreadSearchViewManagerBase::TypeMessagesNotebook;
    if ( managerType == ThreadSearchViewManagerBase::TypeLayout )
    {
        mgrType = ThreadSearchViewManagerBase::TypeLayout;
    }

    int loggerType               = pCfg->ReadInt(wxT("/LoggerType"),          ThreadSearchLoggerBase::TypeList);
    m_LoggerType                 = ThreadSearchLoggerBase::TypeList;
    if ( loggerType == ThreadSearchLoggerBase::TypeTree )
    {
        m_LoggerType = ThreadSearchLoggerBase::TypeTree;
    }

    searchPatterns = pCfg->ReadArrayString(wxT("/SearchPatterns"));
}
// ----------------------------------------------------------------------------
void ThreadSearch::SaveConfig(bool showPanel, int sashPosition,
                          ThreadSearchViewManagerBase::eManagerTypes /*mgrType*/,
                          const wxArrayString& searchPatterns)
// ----------------------------------------------------------------------------
{
    ConfigManager* pCfg = Manager::Get()->GetConfigManager(_T("SnippetsSearch"));

    pCfg->Write(wxT("/MatchWord"),          m_FindData.GetMatchWord());
    pCfg->Write(wxT("/StartWord"),          m_FindData.GetStartWord());
    pCfg->Write(wxT("/MatchCase"),          m_FindData.GetMatchCase());
    pCfg->Write(wxT("/RegEx"),              m_FindData.GetRegEx());
    pCfg->Write(wxT("/HiddenSearch"),       m_FindData.GetHiddenSearch());
    pCfg->Write(wxT("/RecursiveSearch"),    m_FindData.GetRecursiveSearch());

    pCfg->Write(wxT("/CtxMenuIntegration"), m_CtxMenuIntegration);
    pCfg->Write(wxT("/UseDefaultValues"),   m_UseDefValsForThreadSearch);
    pCfg->Write(wxT("/ShowSearchControls"), m_ShowSearchControls);
    pCfg->Write(wxT("/ShowDirControls"),    m_ShowDirControls);
    pCfg->Write(wxT("/ShowCodePreview"),    m_ShowCodePreview);
    pCfg->Write(wxT("/DisplayLogHeaders"),  m_DisplayLogHeaders);
    pCfg->Write(wxT("/DrawLogLines"),       m_DrawLogLines);

    pCfg->Write(wxT("/ShowPanel"),          showPanel);

    pCfg->Write(wxT("/Scope"),              m_FindData.GetScope());

    pCfg->Write(wxT("/DirPath"),            m_FindData.GetSearchPath());
    pCfg->Write(wxT("/Mask"),               m_FindData.GetSearchMask());

    pCfg->Write(wxT("/SplitterPosn"),       sashPosition);
    pCfg->Write(wxT("/SplitterMode"),       (int)m_SplitterMode);
    pCfg->Write(wxT("/ViewManagerType"),    m_pViewManager->GetManagerType());
    pCfg->Write(wxT("/LoggerType"),         m_LoggerType);
    pCfg->Write(wxT("/FileSorting"),        m_FileSorting);

    pCfg->Write(wxT("/SearchPatterns"),     searchPatterns);

}
// ----------------------------------------------------------------------------
bool ThreadSearch::BuildToolBar(wxToolBar* toolBar)
// ----------------------------------------------------------------------------
{

    if ( !IsAttached() || !toolBar )
        return false;

    m_pToolbar = toolBar;
    m_pThreadSearchView->SetToolBar(toolBar);

    //-wxString prefix                = ConfigManager::GetDataFolder() + _T("/images/SnippetsSearch/");
    wxString prefix                = ConfigManager::GetDataFolder() + _T("/images/codesnippets/");
    m_pCboSearchExpr               = new wxComboBox    (toolBar, idCboSearchExpr, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN);
    wxBitmapButton* pBtnSearch     = new wxBitmapButton(toolBar, idBtnSearch, wxBitmap(prefix + wxT("findf.png"), wxBITMAP_TYPE_PNG));
    wxBitmapButton* pBtnOptions    = new wxBitmapButton(toolBar, idBtnOptions, wxBitmap(prefix + wxT("options.png"), wxBITMAP_TYPE_PNG));

    m_pCboSearchExpr->SetToolTip(wxT("Text to search"));
    pBtnSearch->SetToolTip(wxT("Run search"));
    pBtnOptions->SetToolTip(wxT("Show options window"));

    pBtnSearch->SetBitmapDisabled(wxBitmap(prefix + wxT("findfdisabled.png"), wxBITMAP_TYPE_PNG));
    pBtnOptions->SetBitmapDisabled(wxBitmap(prefix + wxT("optionsdisabled.png"), wxBITMAP_TYPE_PNG));

    toolBar->AddControl(m_pCboSearchExpr);
    toolBar->AddControl(pBtnSearch);
    toolBar->AddControl(pBtnOptions);

    toolBar->Realize();
    toolBar->SetInitialSize();

    return true;
}
// ----------------------------------------------------------------------------
void ThreadSearch::OnBtnOptionsClick(wxCommandEvent &event)
// ----------------------------------------------------------------------------
{
    // This routine reached only when run as a plugin.

    if ( !IsAttached() )
        return;

    m_pThreadSearchView->ProcessEvent(event);
}


// ----------------------------------------------------------------------------
void ThreadSearch::OnBtnSearchClick(wxCommandEvent &event)
// ----------------------------------------------------------------------------
{
    if ( !IsAttached() )
        return;

    // Behaviour differs if a search is running.
    if ( m_pThreadSearchView->IsSearchRunning() )
    {
        // In this case, user wants to stops search,
        // we just transmit event
        m_pThreadSearchView->ProcessEvent(event);

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
// ----------------------------------------------------------------------------
void ThreadSearch::RunThreadSearch(const wxString& text, bool isCtxSearch/*=false*/)
// ----------------------------------------------------------------------------
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
        //-Manager::Get()->ProcessEvent(evt);
    }
}


bool ThreadSearch::IsToolbarVisible()
{
    if ( !IsAttached() )
        return false;

    return IsWindowReallyShown(m_pToolbar);
}
// ----------------------------------------------------------------------------
bool ThreadSearch::GetCursorWord(wxString& sWord)
// ----------------------------------------------------------------------------
{
    bool wordFound = false;
    sWord = wxEmptyString;

    // Gets active editor
    //-cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    ScbEditor* ed = GetConfig()->GetEditorManager(m_pParent)->GetBuiltinActiveEditor();
    if ( ed != NULL )
    {
        cbStyledTextCtrl* control = ed->GetControl();

        // Gets word under cursor
        int pos = control->GetCurrentPos();
        int ws  = control->WordStartPosition(pos, true);
        int we  = control->WordEndPosition(pos, true);
        if ( ws < we ) // Avoid empty strings
        {
            // m_SearchedWord will be used if 'Find occurrences' ctx menu is clicked
            sWord = control->GetTextRange(ws, we);
            wordFound = true;
        }
        //(pecan 2008/2/23)
        // Get marked selection
        //int start = control->GetSelectionStart();
        //int end   = control->GetSelectionEnd();
        const wxString selectedText = control->GetSelectedText();
        if ( not selectedText.IsEmpty() )
        {
            sWord = selectedText;
            wordFound = true;
        }
        //(pecan 2008/2/23)
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
// ----------------------------------------------------------------------------
void ThreadSearch::UserResizingWindow(wxSizeEvent & WXUNUSED(event))
// ----------------------------------------------------------------------------
{
    // Called from parent ThreadSearchFrame::OnResizingWindow()

    // On the first resizing of the frame, the editor notebook splitter sash
    // is getting set back to the middle of the frme, not where the user
    // placed it. Here we set a flag to say that thats what's happening
    // OnSashPositionchanged will compensate.

    #if defined(LOGGING)
    //LOGIT( _T("ThreadSearch::OnSizeWindow"));
    #endif
    if (m_pMainPanel && m_pMainPanel->m_pSplitterWindow && m_pThreadSearchView && m_pEdNotebook)
    if (m_pMainPanel->m_pSplitterWindow->IsSplit())
    {
        #if defined(LOGGING)
        //LOGIT( _T("UserResizingWindow:Sash@[%d]"), m_pMainPanel->m_pSplitterWindow->GetSashPosition() );
        #endif
        m_bSashWindowResizing = true;
        //-GetConfig()->GetThreadSearchPlugin()->ResetNotebookSashPosition();
    }
    return;
}
// ----------------------------------------------------------------------------
void ThreadSearch::OnSashPositionChanged(wxSplitterEvent& event)
// ----------------------------------------------------------------------------
{

    if ( m_OnReleased ) return;
    // This does not solve the sash size problem. The Sash still resets
    // to a single line when moved.
    //-m_pMainPanel->m_pSplitterWindow->SetSashSize(m_nSashSize);
    //-m_bSashPositionChanged = true;

    // Put the sash back to the user placed position after frame resizing set it
    // to the middle of the frame.

    #if defined(LOGGING)
     //LOGIT( _T("OnSashPositionChanged:New[%d]"), event.GetSashPosition() );
    #endif
    if (not m_bSashWindowResizing)
    {   // If not resizing, remember where user placed the sash
        m_EdNotebookSashPosition = event.GetSashPosition();
        event.Skip();
    }

    if ( m_bSashWindowResizing)
    {   // if resizing frame, put the sash back to where user placed it.
        m_bSashWindowResizing = false;
        ResetNotebookSashPosition();
    }
}
// ----------------------------------------------------------------------------
void ThreadSearch::OnIdle(wxIdleEvent& event)
// ----------------------------------------------------------------------------
{
    // This does not solve the problem. The Sash still resets
    // to a single line when moved.

    if ( m_OnReleased ) return;
    if (m_bSashPositionChanged)
    {
#if !wxCHECK_VERSION(3, 0, 0)
        // deprected in wx3 and does nothing atall (at least there)
        m_pMainPanel->m_pSplitterWindow->SetSashSize(m_nSashSize);
#endif
        m_pMainPanel->m_pSplitterWindow->Refresh();
        m_bSashPositionChanged = false;
        #if defined(LOGGING)
        LOGIT( _T("ThreadSearch:OnIdle:") );
        #endif
    }
    event.Skip();
}
// ----------------------------------------------------------------------------
void ThreadSearch::OnCodeSnippetsNewIndex(CodeSnippetsEvent& event)
// ----------------------------------------------------------------------------
{
    // EVT_CODESNIPPETS_NEW_INDEX user loaded new xml index file

    wxString newIndex = event.GetSnippetString();
    do{
        if ( newIndex.IsEmpty() ) break;
        if ( not m_CodeSnippetsIndexFilename.IsEmpty() ){
            //-Manager::Get()->GetEditorManager()->Close( m_CodeSnippetsIndexFilename );
            GetConfig()->GetEditorManager(m_pParent)->Close( m_CodeSnippetsIndexFilename );
            m_pThreadSearchView->Clear();
        }
        m_CodeSnippetsIndexFilename = newIndex;

        // Tell CodeSnippetsTreeCtrl to fill global fileLink array with FileLinks
        if ( m_FindData.MustSearchInCodeSnippetsTree() == true )
        {   // Tell CodeSnippetsTreeCtrl to fill global fileLink array with FileLinks
            CodeSnippetsEvent evt(wxEVT_CODESNIPPETS_GETFILELINKS, 0);
            evt.SetSnippetString(_T("Testing GetFileLinks"));
            evt.ProcessCodeSnippetsEvent(evt);
        }
        else
            GetConfig()->ClearFileLinksMapArray();
        #if defined(LOGGING)
        LOGIT( _T("ThreadSearch::OnCodeSnippetsNewIndex[%s]"), m_CodeSnippetsIndexFilename.c_str());
        #endif
    }while(false);

    event.Skip();
    return;
}
// ----------------------------------------------------------------------------
void ThreadSearch::SplitThreadSearchWindow()
// ----------------------------------------------------------------------------
{
    if (m_pMainPanel && m_pMainPanel->m_pSplitterWindow && m_pThreadSearchView && m_pEdNotebook)
    if ( not m_pMainPanel->m_pSplitterWindow->IsSplit())
    {//(pecan 2008/3/11)
        m_pMainPanel->m_pSplitterWindow->SplitHorizontally(
                m_pThreadSearchView, (wxWindow*)m_pEdNotebook  );
        //int width, height; //resize window by another half for some extra viewing
        //m_ThreadSearchPlugin.m_pParent->GetSize( &width, &height);
        //wxSize winSize(width,int((height>>1)+height));
        //m_ThreadSearchPlugin.m_pParent->SetSize( winSize);
        if (m_EdNotebookSashPosition)
            m_pMainPanel->m_pSplitterWindow->SetSashPosition( m_EdNotebookSashPosition );
        else // read last sash position from config file
        {
            int sashPosn = m_pMainPanel->m_pSplitterWindow->GetSashPosition();
            ConfigManager* pCfg = Manager::Get()->GetConfigManager(_T("SnippetsSearch"));
            m_EdNotebookSashPosition = pCfg->ReadInt( wxT("/EdNotebookSashPosn"), sashPosn);
            m_pMainPanel->m_pSplitterWindow->SetSashPosition( m_EdNotebookSashPosition );

        }//else
    }//if MainPanel

}
// ----------------------------------------------------------------------------
void ThreadSearch::ResetNotebookSashPosition()
// ----------------------------------------------------------------------------
{
    if (m_pMainPanel && m_pMainPanel->m_pSplitterWindow && m_pThreadSearchView && m_pEdNotebook)
    if ( m_pMainPanel->m_pSplitterWindow->IsSplit())
    {//(pecan 2008/3/11)
        //m_pMainPanel->m_pSplitterWindow->SplitHorizontally(
        //        m_pThreadSearchView, (wxWindow*)m_pEdNotebook  );
        if (m_EdNotebookSashPosition)
            m_pMainPanel->m_pSplitterWindow->SetSashPosition( m_EdNotebookSashPosition );
    }
}
// ----------------------------------------------------------------------------
void ThreadSearch::UnsplitThreadSearchWindow()
// ----------------------------------------------------------------------------
{
    if (m_pMainPanel && m_pMainPanel->m_pSplitterWindow && m_pThreadSearchView && m_pEdNotebook)
    if ( m_pMainPanel->m_pSplitterWindow->IsSplit())
    {  //(pecan 2008/4/26)
        // record current position of split
        m_EdNotebookSashPosition = m_pMainPanel->m_pSplitterWindow->GetSashPosition();
        ConfigManager* pCfg = Manager::Get()->GetConfigManager(_T("SnippetsSearch"));
        pCfg->Write( wxT("/EdNotebookSashPosn"), m_EdNotebookSashPosition);
        m_pMainPanel->m_pSplitterWindow->Unsplit();
    }
}
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

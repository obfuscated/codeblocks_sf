/***************************************************************
 * Name:      ThreadSearchView
 * Purpose:   This class implements the panel that is added to
 *            Code::Blocks Message notebook or C::B layout.
 * Author:    Jerome ANTOINE
 * Created:   2007-10-08
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#include "sdk.h"
#include <wx/bitmap.h>
#include <wx/bmpbuttn.h>
#include <wx/statline.h>
#ifndef CB_PRECOMP
    #include <wx/combobox.h>
    #include <wx/menu.h>
    #include <wx/sizer.h>
    #include <wx/splitter.h>
    #include <wx/statbox.h>
    #include <wx/stattext.h>
    #include <wx/settings.h>
    #include <wx/toolbar.h>

    #include "cbeditor.h"
    #include "configmanager.h"
    #include "editorcolourset.h"
    #include "logmanager.h"
#endif

#include "cbstyledtextctrl.h"
#include "encodingdetector.h"
#include "SearchInPanel.h"
#include "DirectoryParamsPanel.h"
#include "ThreadSearch.h"
#include "ThreadSearchView.h"
#include "ThreadSearchEvent.h"
#include "ThreadSearchThread.h"
#include "ThreadSearchFindData.h"
#include "ThreadSearchConfPanel.h"
#include "ThreadSearchControlIds.h"
#include "wx/tglbtn.h"


// Max number of items in search history combo box
const unsigned int MAX_NB_SEARCH_ITEMS = 20;

// Timer value for events handling (events sent by worker thread)
const          int TIMER_PERIOD        = 100;


ThreadSearchView::ThreadSearchView(ThreadSearch& threadSearchPlugin)
                 :wxPanel(Manager::Get()->GetAppWindow())
                 ,m_ThreadSearchPlugin(threadSearchPlugin)
                 ,m_Timer(this, idTmrListCtrlUpdate)
                 ,m_StoppingThread(0)
{
    m_pFindThread = NULL;
    m_pToolBar    = NULL;
    wxString prefix;
    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("app"));
    if (cfg->ReadBool(_T("/environment/toolbar_size"),true))
    {
        prefix = ConfigManager::GetDataFolder() + _T("/images/ThreadSearch/16x16/");
    }
    else
    {
        prefix = ConfigManager::GetDataFolder() + _T("/images/ThreadSearch/22x22/");
    }

    // begin wxGlade: ThreadSearchView::ThreadSearchView
    m_pSplitter = new wxSplitterWindow(this, -1, wxDefaultPosition, wxSize(1,1), wxSP_3D|wxSP_BORDER|wxSP_PERMIT_UNSPLIT);
    m_pPnlListLog = new wxPanel(m_pSplitter, -1, wxDefaultPosition, wxSize(1,1));
    m_pPnlPreview = new wxPanel(m_pSplitter, -1, wxDefaultPosition, wxSize(1,1));
    m_pSizerSearchDirItems_staticbox = new wxStaticBox(this, -1, _("Directory parameters"));
    const wxString m_pCboSearchExpr_choices[] = {

    };
    m_pCboSearchExpr = new wxComboBox(this, idCboSearchExpr, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, m_pCboSearchExpr_choices, wxCB_DROPDOWN|wxTE_PROCESS_ENTER);
    m_pBtnSearch = new wxBitmapButton(this, idBtnSearch, wxBitmap(prefix + wxT("findf.png"), wxBITMAP_TYPE_PNG), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
    m_pBtnOptions = new wxBitmapButton(this, idBtnOptions, wxBitmap(prefix + wxT("options.png"), wxBITMAP_TYPE_PNG), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);//wxT("Options"));
    m_pStaticLine1 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
    m_pStaTxtSearchIn = new wxStaticText(this, -1, _("Search in "));
    m_pPnlSearchIn = new SearchInPanel(this, -1);
    m_pStaticLine2 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
    m_pBtnShowDirItems = new wxBitmapButton(this, idBtnShowDirItemsClick, wxBitmap(prefix + wxT("showdir.png"), wxBITMAP_TYPE_PNG), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
    m_pPnlDirParams = new DirectoryParamsPanel(&threadSearchPlugin.GetFindData(), this, -1);
    m_pSearchPreview = new cbStyledTextCtrl(m_pPnlPreview, wxID_ANY, wxDefaultPosition, wxSize(1,1));
    m_pLogger = ThreadSearchLoggerBase::BuildThreadSearchLoggerBase(*this, m_ThreadSearchPlugin,
                                                                    m_ThreadSearchPlugin.GetLoggerType(),
                                                                    m_ThreadSearchPlugin.GetFileSorting(),
                                                                    m_pPnlListLog,
                                                                    idWndLogger);

    set_properties();
    do_layout();
    // end wxGlade

    // Dynamic event connections.
    // Static events table (BEGIN_EVENT_TABLE) doesn't work for all events
    long id = m_pSearchPreview->GetId();
    Connect(id, wxEVT_SCI_MARGINCLICK,
            (wxObjectEventFunction) (wxEventFunction) (wxScintillaEventFunction)
            &ThreadSearchView::OnMarginClick);

    Connect(id, wxEVT_CONTEXT_MENU,
            (wxObjectEventFunction) (wxEventFunction) (wxContextMenuEventFunction)
            &ThreadSearchView::OnContextMenu);

    Connect(wxEVT_THREAD_SEARCH_ERROR,
            (wxObjectEventFunction)&ThreadSearchView::OnThreadSearchErrorEvent);
}


ThreadSearchView::~ThreadSearchView()
{
    if ( m_pFindThread != NULL )
    {
        StopThread();
    }

    // I don't know if it is necessay to remove event connections
    // so I do it myself
    long id = m_pSearchPreview->GetId();
    Disconnect(id, wxEVT_SCI_MARGINCLICK,
            (wxObjectEventFunction) (wxEventFunction) (wxScintillaEventFunction)
            &ThreadSearchView::OnMarginClick);

    Disconnect(id, wxEVT_CONTEXT_MENU,
            (wxObjectEventFunction) (wxEventFunction) (wxContextMenuEventFunction)
            &ThreadSearchView::OnContextMenu);

    Disconnect(wxEVT_THREAD_SEARCH_ERROR,
            (wxObjectEventFunction)&ThreadSearchView::OnThreadSearchErrorEvent);

    m_ThreadSearchPlugin.OnThreadSearchViewDestruction();

    delete m_pLogger;
    m_pLogger = NULL;
}

// As SearchInPanel and DirectoryParamsPanel are generic, their
// events are managed by parent ie this ThreadSearchView class.
BEGIN_EVENT_TABLE(ThreadSearchView, wxPanel)
    // begin wxGlade: ThreadSearchView::event_table
    EVT_TEXT_ENTER(idCboSearchExpr, ThreadSearchView::OnCboSearchExprEnter)
    EVT_TEXT_ENTER(idSearchDirPath, ThreadSearchView::OnCboSearchExprEnter)
    EVT_TEXT_ENTER(idSearchMask, ThreadSearchView::OnCboSearchExprEnter)
    EVT_BUTTON(idBtnSearch, ThreadSearchView::OnBtnSearchClick)
    EVT_BUTTON(idBtnOptions, ThreadSearchView::OnBtnOptionsClick)

    EVT_MENU(idOptionDialog, ThreadSearchView::OnShowOptionsDialog)
    EVT_MENU(idOptionWholeWord, ThreadSearchView::OnQuickOptions)
    EVT_MENU(idOptionStartWord, ThreadSearchView::OnQuickOptions)
    EVT_MENU(idOptionMatchCase, ThreadSearchView::OnQuickOptions)
    EVT_MENU(idOptionRegEx, ThreadSearchView::OnQuickOptions)

    EVT_UPDATE_UI(idOptionWholeWord, ThreadSearchView::OnQuickOptionsUpdateUI)
    EVT_UPDATE_UI(idOptionStartWord, ThreadSearchView::OnQuickOptionsUpdateUI)
    EVT_UPDATE_UI(idOptionMatchCase, ThreadSearchView::OnQuickOptionsUpdateUI)
    EVT_UPDATE_UI(idOptionRegEx, ThreadSearchView::OnQuickOptionsUpdateUI)

    EVT_BUTTON(idBtnShowDirItemsClick, ThreadSearchView::OnBtnShowDirItemsClick)
    EVT_SPLITTER_DCLICK(-1, ThreadSearchView::OnSplitterDoubleClick)
    // end wxGlade

    EVT_TOGGLEBUTTON(idBtnSearchOpenFiles,      ThreadSearchView::OnBtnSearchOpenFiles)
    EVT_TOGGLEBUTTON(idBtnSearchTargetFiles,    ThreadSearchView::OnBtnSearchTargetFiles)
    EVT_TOGGLEBUTTON(idBtnSearchProjectFiles,   ThreadSearchView::OnBtnSearchProjectFiles)
    EVT_TOGGLEBUTTON(idBtnSearchWorkspaceFiles, ThreadSearchView::OnBtnSearchWorkspaceFiles)
    EVT_TOGGLEBUTTON(idBtnSearchDirectoryFiles, ThreadSearchView::OnBtnSearchDirectoryFiles)

    EVT_TIMER(idTmrListCtrlUpdate,          ThreadSearchView::OnTmrListCtrlUpdate)
END_EVENT_TABLE();


void ThreadSearchView::OnThreadSearchErrorEvent(const ThreadSearchEvent& event)
{
    Manager::Get()->GetLogManager()->Log(F(_T("ThreadSearch: %s"), event.GetString().wx_str()));
}

void ThreadSearchView::OnCboSearchExprEnter(wxCommandEvent &/*event*/)
{
    // Event handler used when user clicks on enter after typing
    // in combo box text control.
    // Runs a multi threaded search.
    ThreadSearchFindData findData = m_ThreadSearchPlugin.GetFindData();
    findData.SetFindText(m_pCboSearchExpr->GetValue());
    ThreadedSearch(findData);
}


void ThreadSearchView::OnBtnSearchClick(wxCommandEvent &/*event*/)
{
    // User clicked on Search/Cancel
    // m_ThreadSearchEventsArray is shared by two threads, we
    // use m_MutexSearchEventsArray to have a safe access.
    // As button action depends on m_ThreadSearchEventsArray,
    // we lock the mutex to process it correctly.
    if ( m_MutexSearchEventsArray.Lock() == wxMUTEX_NO_ERROR )
    {
        int nbEvents = m_ThreadSearchEventsArray.GetCount();
        m_MutexSearchEventsArray.Unlock();
        if ( m_pFindThread != NULL )
        {
            // A threaded search is running...
            UpdateSearchButtons(false);
            StopThread();
        }
        else if ( nbEvents > 0 )
        {
            // A threaded search has run but the events array is
            // not completely processed...
            UpdateSearchButtons(false);
            if ( ClearThreadSearchEventsArray() == false )
            {
                cbMessageBox(_("Failed to clear events array."), _("Error"), wxICON_ERROR);
            }
        }
        else
        {
            // We start the thread search
            ThreadSearchFindData findData = m_ThreadSearchPlugin.GetFindData();
            findData.SetFindText(m_pCboSearchExpr->GetValue());
            ThreadedSearch(findData);
        }
    }
}


void ThreadSearchView::OnBtnOptionsClick(wxCommandEvent &/*event*/)
{
    wxMenu menu;
    menu.Append(idOptionDialog, _("Options"), _("Shows the options dialog"));
    menu.AppendSeparator();
    menu.AppendCheckItem(idOptionWholeWord, _("Whole word"), _("Search text matches only whole words"));
    menu.AppendCheckItem(idOptionStartWord, _("Start word"), _("Matches only word starting with search expression"));
    menu.AppendCheckItem(idOptionMatchCase, _("Match case"), _("Case sensitive search."));
    menu.AppendCheckItem(idOptionRegEx, _("Regular expression"), _("Search expression is a regular expression"));

    PopupMenu(&menu);
}

void ThreadSearchView::OnShowOptionsDialog(wxCommandEvent &/*event*/)
{
    // Displays a dialog box with a ThreadSearchConfPanel.
    // All parameters can be set on this dialog.
    // It is the same as doing 'Settings/environment/Thread search'
    // Settings are updated by the cbConfigurationDialog
    cbConfigurationDialog* pDlg       = new cbConfigurationDialog(Manager::Get()->GetAppWindow(), -1, _("Options"));
    ThreadSearchConfPanel* pConfPanel = new ThreadSearchConfPanel(m_ThreadSearchPlugin, pDlg);

    pDlg->AttachConfigurationPanel(pConfPanel);
    pDlg->ShowModal();
    pDlg->Destroy();
}

void ThreadSearchView::OnQuickOptions(wxCommandEvent &event)
{
    ThreadSearchFindData findData = m_ThreadSearchPlugin.GetFindData();
    switch (event.GetId())
    {
    case idOptionWholeWord:
        findData.SetMatchWord(event.IsChecked());
        m_ThreadSearchPlugin.SetFindData(findData);
        break;
    case idOptionStartWord:
        findData.SetStartWord(event.IsChecked());
        m_ThreadSearchPlugin.SetFindData(findData);
        break;
    case idOptionMatchCase:
        findData.SetMatchCase(event.IsChecked());
        m_ThreadSearchPlugin.SetFindData(findData);
        break;
    case idOptionRegEx:
        findData.SetRegEx(event.IsChecked());
        m_ThreadSearchPlugin.SetFindData(findData);
        break;
    default:
        break;
    }
}

void ThreadSearchView::OnQuickOptionsUpdateUI(wxUpdateUIEvent &event)
{
    ThreadSearchFindData &findData = m_ThreadSearchPlugin.GetFindData();
    switch (event.GetId())
    {
    case idOptionWholeWord:
        event.Check(findData.GetMatchWord());
        break;
    case idOptionStartWord:
        event.Check(findData.GetStartWord());
        break;
    case idOptionMatchCase:
        event.Check(findData.GetMatchCase());
        break;
    case idOptionRegEx:
        event.Check(findData.GetRegEx());
        break;
    default:
        break;
    }
}

void ThreadSearchView::OnBtnShowDirItemsClick(wxCommandEvent& WXUNUSED(event))
{
    // Shows/Hides directory parameters panel and updates button label.
    wxSizer* pTopSizer = GetSizer();
    wxASSERT(m_pSizerSearchDirItems && pTopSizer);

    bool show = !m_pPnlDirParams->IsShown();
    m_ThreadSearchPlugin.SetShowDirControls(show);

    pTopSizer->Show(m_pSizerSearchDirItems, show, true);
    if ( show == true )
    {
        m_pBtnShowDirItems->SetToolTip(_("Hide dir items"));
    }
    else
    {
        m_pBtnShowDirItems->SetToolTip(_("Show dir items"));
    }
    pTopSizer->Layout();
}


void ThreadSearchView::OnSplitterDoubleClick(wxSplitterEvent &/*event*/)
{
    m_ThreadSearchPlugin.SetShowCodePreview(false);
    ApplySplitterSettings(false, m_pSplitter->GetSplitMode());

    // Informs user on how to show code preview later.
    cbMessageBox(_("To re-enable code preview, check the \"Show code preview editor\" in ThreadSearch options panel."),
                 _("ThreadSearchInfo"), wxICON_INFORMATION);
}

// wxGlade: add ThreadSearchView event handlers


void ThreadSearchView::set_properties()
{
    wxString prefix;
    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("app"));
    if (cfg->ReadBool(_T("/environment/toolbar_size"),true))
    {
        prefix = ConfigManager::GetDataFolder() + _T("/images/ThreadSearch/16x16/");
    }
    else
    {
        prefix = ConfigManager::GetDataFolder() + _T("/images/ThreadSearch/22x22/");
    }
    // begin wxGlade: ThreadSearchView::set_properties
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    m_pCboSearchExpr->SetMinSize(wxSize(180, -1));
    m_pBtnSearch->SetToolTip(_("Search in files"));
    m_pBtnSearch->SetBitmapDisabled(wxBitmap(prefix + wxT("findfdisabled.png"), wxBITMAP_TYPE_PNG));
    m_pBtnSearch->SetSize(m_pBtnSearch->GetBestSize());
    m_pBtnOptions->SetToolTip(_("Options"));
    m_pBtnOptions->SetBitmapDisabled(wxBitmap(prefix + wxT("optionsdisabled.png"), wxBITMAP_TYPE_PNG));
    m_pBtnOptions->SetSize(m_pBtnOptions->GetBestSize());
    m_pBtnShowDirItems->SetToolTip(_("Show dir Items"));
    m_pBtnShowDirItems->SetBitmapDisabled(wxBitmap(prefix + wxT("showdirdisabled.png"), wxBITMAP_TYPE_PNG));
    m_pBtnShowDirItems->SetSize(m_pBtnShowDirItems->GetBestSize());
    m_pPnlPreview->SetMinSize(wxSize(25, -1));
    // end wxGlade

    m_pSearchPreview->SetReadOnly(true);

    ThreadSearchFindData findData;
    m_ThreadSearchPlugin.GetFindData(findData);

    m_pPnlDirParams->SetSearchDirHidden(findData.GetHiddenSearch());
    m_pPnlDirParams->SetSearchDirRecursively(findData.GetRecursiveSearch());
    m_pPnlDirParams->SetSearchDirPath(findData.GetSearchPath());
    m_pPnlDirParams->SetSearchMask(findData.GetSearchMask());

    m_pPnlSearchIn->SetSearchInOpenFiles(findData.MustSearchInOpenFiles());
    m_pPnlSearchIn->SetSearchInTargetFiles(findData.MustSearchInTarget());
    m_pPnlSearchIn->SetSearchInProjectFiles(findData.MustSearchInProject());
    m_pPnlSearchIn->SetSearchInWorkspaceFiles(findData.MustSearchInWorkspace());
    m_pPnlSearchIn->SetSearchInDirectory(findData.MustSearchInDirectory());
}


void ThreadSearchView::do_layout()
{
#if wxCHECK_VERSION(2, 9, 0)
    #define wxADJUST_MINSIZE 0
#endif
    // begin wxGlade: ThreadSearchView::do_layout
    wxBoxSizer* m_pSizerTop = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* m_pSizerSplitter = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* m_pSizerListLog = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* m_pSizerSearchPreview = new wxBoxSizer(wxHORIZONTAL);
    m_pSizerSearchDirItems = new wxStaticBoxSizer(m_pSizerSearchDirItems_staticbox, wxHORIZONTAL);
    m_pSizerSearchItems = new wxBoxSizer(wxHORIZONTAL);
    m_pSizerSearchItems->Add(m_pCboSearchExpr, 2, wxALL|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    m_pSizerSearchItems->Add(m_pBtnSearch, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    m_pSizerSearchItems->Add(m_pBtnOptions, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    m_pSizerSearchItems->Add(m_pStaticLine1, 0, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_VERTICAL, 2);
    m_pSizerSearchItems->Add(m_pStaTxtSearchIn, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    m_pSizerSearchItems->Add(m_pPnlSearchIn, 0, wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 0);
    m_pSizerSearchItems->Add(m_pStaticLine2, 0, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_VERTICAL, 2);
    m_pSizerSearchItems->Add(m_pBtnShowDirItems, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    m_pSizerTop->Add(m_pSizerSearchItems, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL, 0);
    m_pSizerSearchDirItems->Add(m_pPnlDirParams, 1, wxALIGN_CENTER_VERTICAL, 0);
    m_pSizerTop->Add(m_pSizerSearchDirItems, 0, wxBOTTOM|wxEXPAND, 4);
    m_pSizerSearchPreview->Add(m_pSearchPreview, 1, wxEXPAND|wxADJUST_MINSIZE, 0);
    m_pPnlPreview->SetAutoLayout(true);
    m_pPnlPreview->SetSizer(m_pSizerSearchPreview);
    m_pSizerListLog->Add(m_pLogger->GetWindow(), 1, wxEXPAND|wxFIXED_MINSIZE, 0);
    m_pPnlListLog->SetAutoLayout(true);
    m_pPnlListLog->SetSizer(m_pSizerListLog);
    m_pSplitter->SplitVertically(m_pPnlPreview, m_pPnlListLog);
    m_pSizerSplitter->Add(m_pSplitter, 1, wxEXPAND|wxADJUST_MINSIZE, 0);
    m_pSizerTop->Add(m_pSizerSplitter, 1, wxEXPAND, 0);
    SetAutoLayout(true);
    SetSizer(m_pSizerTop);
    m_pSizerTop->Fit(this);
    m_pSizerTop->SetSizeHints(this);
    // end wxGlade

    m_pSplitter->SetMinimumPaneSize(50);
}


void ThreadSearchView::OnThreadExit()
{
    // This method must be called only from ThreadSearchThread::OnExit
    // because delete is performed in the base class.
    // We reset the pointer to be sure it is not used.
    if ( m_pFindThread != NULL )
    {
        m_pFindThread = NULL;
    }

    if ( m_StoppingThread > 0 )
    {
        m_StoppingThread--;
    }
}


void ThreadSearchView::ThreadedSearch(const ThreadSearchFindData& aFindData)
{
    // We don't search empty patterns
    if ( aFindData.GetFindText() != wxEmptyString )
    {
        ThreadSearchFindData findData(aFindData);

        // Prepares logger
        m_pLogger->OnSearchBegin(aFindData);

        // Two steps thread creation
        m_pFindThread = new ThreadSearchThread(this, findData);
        if ( m_pFindThread != NULL )
        {
            if ( m_pFindThread->Create() == wxTHREAD_NO_ERROR )
            {
                // Thread execution
                if ( m_pFindThread->Run() != wxTHREAD_NO_ERROR )
                {
                    m_pFindThread->Delete();
                    m_pFindThread = NULL;
                    cbMessageBox(_("Failed to run search thread"));
                }
                else
                {
                    // Update combo box search history
                    AddExpressionToSearchCombos(findData.GetFindText(), findData.GetSearchPath(),
                                                findData.GetSearchMask());
                    UpdateSearchButtons(true, cancel);
                    EnableControls(false);

                    // Starts the timer used to managed events sent by m_pFindThread
                    m_Timer.Start(TIMER_PERIOD, wxTIMER_CONTINUOUS);
                }
            }
            else
            {
                // Error
                m_pFindThread->Delete();
                m_pFindThread = NULL;
                cbMessageBox(_("Failed to create search thread (2)"));
            }
        }
        else
        {
            // Error
            cbMessageBox(_("Failed to create search thread (1)"));
        }
    }
    else
    {
        // Error
        cbMessageBox(_("Search expression is empty !"));
    }
}


bool ThreadSearchView::UpdatePreview(const wxString& file, long line)
{
    bool success(true);

    if ( line > 0 )
    {
        // Line display begins at 1 but line index at 0
        line--;
    }

    // Disable read only
    m_pSearchPreview->Enable(false);
    m_pSearchPreview->SetReadOnly(false);

    // Loads file if different from current loaded
    wxFileName filename(file);
    if ( (m_PreviewFilePath != file) || (m_PreviewFileDate != filename.GetModificationTime()) )
    {
        ConfigManager* mgr = Manager::Get()->GetConfigManager(_T("editor"));

        // Remember current file path and modification time
        m_PreviewFilePath = file;
        m_PreviewFileDate = filename.GetModificationTime();

        EncodingDetector enc(m_PreviewFilePath, false);
        success = enc.IsOK();
        m_pSearchPreview->SetText(enc.GetWxStr());

        // Colorize
        cbEditor::ApplyStyles(m_pSearchPreview);
        EditorColourSet EdColSet;
        EdColSet.Apply(EdColSet.GetLanguageForFilename(m_PreviewFilePath), m_pSearchPreview);

        SetFoldingIndicator(mgr->ReadInt(_T("/folding/indicator"), 2));
        UnderlineFoldedLines(mgr->ReadBool(_T("/folding/underline_folded_line"), true));
    }

    if ( success == true )
    {
        // Display the selected line
        int onScreen = m_pSearchPreview->LinesOnScreen() >> 1;
        m_pSearchPreview->GotoLine(line - onScreen);
        m_pSearchPreview->GotoLine(line + onScreen);
        m_pSearchPreview->GotoLine(line);
        m_pSearchPreview->EnsureVisible(line);

        int startPos = m_pSearchPreview->PositionFromLine(line);
        int endPos   = m_pSearchPreview->GetLineEndPosition(line);
        m_pSearchPreview->SetSelectionVoid(endPos, startPos);
    }

    // Enable read only
    m_pSearchPreview->SetReadOnly(true);
    m_pSearchPreview->Enable(true);

    return success;
}


void ThreadSearchView::OnLoggerClick(const wxString& file, long line)
{
    // Updates preview editor with selected file at requested line.
    // We don't check that file changed since search was performed.
    UpdatePreview(file, line);
}


void ThreadSearchView::OnLoggerDoubleClick(const wxString& file, long line)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->Open(file);
    if (!line || !ed)
        return;

    line -= 1;
    ed->Activate();

    // cbEditor already implements the line centering
    ed->GotoLine(line);

    // Show even if line is folded
    if (cbStyledTextCtrl* control = ed->GetControl()) {
        control->EnsureVisible(line);

        wxFocusEvent ev(wxEVT_SET_FOCUS);
        ev.SetWindow(this);
        #if wxCHECK_VERSION(2, 9, 0)
        control->GetEventHandler()->AddPendingEvent(ev);
        #else
        control->AddPendingEvent(ev);
        #endif
    }
}

void ThreadSearchView::OnMarginClick(wxScintillaEvent& event)
{
    // We handle click on preview editor margin
    // For now, all is read only; we manage folding but not
    // breakpoints and bookmarks.
    switch (event.GetMargin())
    {
        case 2: // folding margin
        {
            int lineYpix = event.GetPosition();
            int line = m_pSearchPreview->LineFromPosition(lineYpix);

            m_pSearchPreview->ToggleFold(line);
            break;
        }
        case 1: // bookmarks and breakpoints margin
        {
            // For now I ignore bookmarks and breakpoints
            /*
            #define BOOKMARK_MARKER    2
            int line = m_pSearchPreview->LineFromPosition(event.GetPosition());

            if ( m_pSearchPreview->MarkerGet(line) & (1 << BOOKMARK_MARKER) )
            {
                m_pSearchPreview->MarkerDelete(line, BOOKMARK_MARKER);
            }
            else
            {
                m_pSearchPreview->MarkerAdd(line, BOOKMARK_MARKER);
            }
            */
            break;
        }
        default:
        {
            break;
        }
    }
}


void ThreadSearchView::OnContextMenu(wxContextMenuEvent& event)
{
    event.StopPropagation();
}


void ThreadSearchView::AddExpressionToSearchCombos(const wxString& expression, const wxString& path, const wxString& mask)
{
    // We perform tests on view combo and don't check toolbar one
    // because their contents are identical
    // Gets toolbar combo pointer
    wxComboBox* pToolBarCombo = static_cast<wxComboBox*>(m_pToolBar->FindControl(idCboSearchExpr));

    // Updates combos box with new item
    // Search item index
    int index = m_pCboSearchExpr->FindString(expression);

    // Removes item if already in combos box
    if ( index != wxNOT_FOUND )
    {
        m_pCboSearchExpr->Delete(index);
        pToolBarCombo->Delete(index);
    }

    // Removes last item if max nb item is reached
    if ( m_pCboSearchExpr->GetCount() > MAX_NB_SEARCH_ITEMS )
    {
        // Removes last one
        m_pCboSearchExpr->Delete(m_pCboSearchExpr->GetCount()-1);
        pToolBarCombo->Delete(m_pCboSearchExpr->GetCount()-1);
    }

    // Adds it to combos
    m_pCboSearchExpr->Insert(expression, 0);
    m_pCboSearchExpr->SetSelection(0);
    pToolBarCombo->Insert(expression, 0);
    pToolBarCombo->SetSelection(0);

    m_pPnlDirParams->AddExpressionToCombos(path, mask);
}


void ThreadSearchView::FocusSearchCombo(const wxString &searchWord)
{
    if (!searchWord.empty())
        m_pCboSearchExpr->SetValue(searchWord);
    m_pCboSearchExpr->SetFocus();
}


void ThreadSearchView::Update()
{
    ThreadSearchFindData findData;
    m_ThreadSearchPlugin.GetFindData(findData);

    m_pPnlSearchIn->SetSearchInOpenFiles     (findData.MustSearchInOpenFiles());
    m_pPnlSearchIn->SetSearchInTargetFiles   (findData.MustSearchInTarget   ());
    m_pPnlSearchIn->SetSearchInProjectFiles  (findData.MustSearchInProject  ());
    m_pPnlSearchIn->SetSearchInWorkspaceFiles(findData.MustSearchInWorkspace());
    m_pPnlSearchIn->SetSearchInDirectory     (findData.MustSearchInDirectory());

    m_pPnlDirParams->SetSearchDirHidden      (findData.GetHiddenSearch());
    m_pPnlDirParams->SetSearchDirRecursively (findData.GetRecursiveSearch());
    m_pPnlDirParams->SetSearchDirPath        (findData.GetSearchPath());
    m_pPnlDirParams->SetSearchMask           (findData.GetSearchMask());

    ShowSearchControls(m_ThreadSearchPlugin.GetShowSearchControls());
    SetLoggerType(m_ThreadSearchPlugin.GetLoggerType());
    m_pLogger->Update();

    ApplySplitterSettings(m_ThreadSearchPlugin.GetShowCodePreview(), m_ThreadSearchPlugin.GetSplitterMode());
}


void ThreadSearchView::OnBtnSearchOpenFiles(wxCommandEvent &event)
{
    m_ThreadSearchPlugin.GetFindData().UpdateSearchScope(ScopeOpenFiles, m_pPnlSearchIn->GetSearchInOpenFiles());
    event.Skip();
}


void ThreadSearchView::OnBtnSearchTargetFiles(wxCommandEvent &event)
{
    m_ThreadSearchPlugin.GetFindData().UpdateSearchScope(ScopeTargetFiles, m_pPnlSearchIn->GetSearchInTargetFiles());
    m_ThreadSearchPlugin.GetFindData().UpdateSearchScope(ScopeProjectFiles, false);
    m_ThreadSearchPlugin.GetFindData().UpdateSearchScope(ScopeWorkspaceFiles, false);
    event.Skip();
}


void ThreadSearchView::OnBtnSearchProjectFiles(wxCommandEvent &event)
{
    m_ThreadSearchPlugin.GetFindData().UpdateSearchScope(ScopeProjectFiles, m_pPnlSearchIn->GetSearchInProjectFiles());
    m_ThreadSearchPlugin.GetFindData().UpdateSearchScope(ScopeTargetFiles, false);
    m_ThreadSearchPlugin.GetFindData().UpdateSearchScope(ScopeWorkspaceFiles, false);
    event.Skip();
}


void ThreadSearchView::OnBtnSearchWorkspaceFiles(wxCommandEvent &event)
{
    m_ThreadSearchPlugin.GetFindData().UpdateSearchScope(ScopeWorkspaceFiles, m_pPnlSearchIn->GetSearchInWorkspaceFiles());
    m_ThreadSearchPlugin.GetFindData().UpdateSearchScope(ScopeTargetFiles, false);
    m_ThreadSearchPlugin.GetFindData().UpdateSearchScope(ScopeProjectFiles, false);
    event.Skip();
}


void ThreadSearchView::OnBtnSearchDirectoryFiles(wxCommandEvent &event)
{
    m_ThreadSearchPlugin.GetFindData().UpdateSearchScope(ScopeDirectoryFiles, m_pPnlSearchIn->GetSearchInDirectory());
    event.Skip();
}


void ThreadSearchView::EnableControls(bool enable)
{
    // Used to disable search parameters controls during
    // threaded search in notebook panel and toolbar.
    long idsArray[] = {
        idBtnDirSelectClick,
        idBtnOptions,
        idCboSearchExpr,
        idChkSearchDirRecurse,
        idChkSearchDirHidden,
        idBtnSearchOpenFiles,
        idBtnSearchTargetFiles,
        idBtnSearchProjectFiles,
        idBtnSearchWorkspaceFiles,
        idBtnSearchDirectoryFiles,
        idSearchDirPath,
        idSearchMask
    };

    long toolBarIdsArray[] = {
        idCboSearchExpr
    };

    for ( unsigned int i = 0; i < sizeof(idsArray)/sizeof(idsArray[0]); ++i )
    {
        wxWindow* pWnd = wxWindow::FindWindow(idsArray[i]);
        if ( pWnd != 0 )
        {
            pWnd->Enable(enable);
        }
        else
        {
            cbMessageBox(wxString::Format(_("Failed to Enable window (id=%ld)"), idsArray[i]).c_str(),
                         _("Error"), wxOK|wxICON_ERROR, this);
        }
    }

    for ( unsigned int i = 0; i < sizeof(toolBarIdsArray)/sizeof(toolBarIdsArray[0]); ++i )
    {
        m_pToolBar->FindControl(toolBarIdsArray[i])->Enable(enable);
    }

    m_pToolBar->EnableTool(idBtnOptions,enable);
    m_pToolBar->Update();
}


void ThreadSearchView::PostThreadSearchEvent(const ThreadSearchEvent& event)
{
    // Clone the worker thread event to the mutex protected m_ThreadSearchEventsArray
    if ( m_MutexSearchEventsArray.Lock() == wxMUTEX_NO_ERROR )
    {
        // Events are handled in ThreadSearchView::OnTmrListCtrlUpdate
        // ThreadSearchView::OnTmrListCtrlUpdate is called automatically
        // by m_Timer wxTimer
        m_ThreadSearchEventsArray.Add(event.Clone());
        m_MutexSearchEventsArray.Unlock();
    }
}


void ThreadSearchView::OnTmrListCtrlUpdate(wxTimerEvent& /*event*/)
{
    if ( m_MutexSearchEventsArray.Lock() == wxMUTEX_NO_ERROR )
    {
        if ( m_ThreadSearchEventsArray.GetCount() > 0 )
        {
            ThreadSearchEvent *pEvent = static_cast<ThreadSearchEvent*>(m_ThreadSearchEventsArray[0]);
            m_pLogger->OnThreadSearchEvent(*pEvent);
            delete pEvent;
            m_ThreadSearchEventsArray.RemoveAt(0,1);
        }

        if ( (m_ThreadSearchEventsArray.GetCount() == 0) && (m_pFindThread == NULL) )
        {
            // Thread search is finished (m_pFindThread == NULL) and m_ThreadSearchEventsArray
            // is empty (m_ThreadSearchEventsArray.GetCount() == 0).
            // We stop the timer to spare resources
            m_Timer.Stop();

            m_pLogger->OnSearchEnd();

            // Restores label and enables all search params graphical widgets.
            UpdateSearchButtons(true, search);
            EnableControls(true);
        }

        m_MutexSearchEventsArray.Unlock();
    }
}


bool ThreadSearchView::ClearThreadSearchEventsArray()
{
    bool success = (m_MutexSearchEventsArray.Lock() == wxMUTEX_NO_ERROR);
    if ( success == true )
    {
        size_t i                  = m_ThreadSearchEventsArray.GetCount();
        ThreadSearchEvent* pEvent = NULL;
        while ( i != 0 )
        {
            pEvent = static_cast<ThreadSearchEvent*>(m_ThreadSearchEventsArray[0]);
            delete pEvent;
            m_ThreadSearchEventsArray.RemoveAt(0,1);
            i--;
        }

        m_MutexSearchEventsArray.Unlock();
    }

    return success;
}


bool ThreadSearchView::StopThread()
{
    bool success = false;
    if ( (m_StoppingThread == 0) && (m_pFindThread != NULL) )
    {
        // A search thread is running. We stop it.
        m_StoppingThread++;
        m_pFindThread->Delete();

        // We stop the timer responsible for list control update and we wait twice
        // its period to delete all waiting events (not processed yet)
        m_Timer.Stop();
        wxThread::Sleep(2*TIMER_PERIOD);

        success = ClearThreadSearchEventsArray();
        if ( success == false )
        {
            cbMessageBox(_("Failed to clear events array."), _("Error"), wxICON_ERROR);
        }

        // Restores label and enables all search params graphical widgets.
        UpdateSearchButtons(true, search);
        EnableControls(true);
    }

    return success;
}

bool ThreadSearchView::IsSearchRunning()
{
    bool searchRunning = (m_pFindThread != 0);

    if ( m_MutexSearchEventsArray.Lock() == wxMUTEX_NO_ERROR )
    {
        // If user clicked on Cancel or thread is finished, there may be remaining
        // events to display in the array. In this case, we consider the search is
        // stil running even if thread is over.
        searchRunning = searchRunning || (m_ThreadSearchEventsArray.GetCount() > 0);
        m_MutexSearchEventsArray.Unlock();
    }

    return searchRunning;
}


void ThreadSearchView::UpdateSearchButtons(bool enable, eSearchButtonLabel label)
{
    // Labels and pictures paths
    wxString searchButtonLabels[]        = {_("Search"), _("Cancel search"), wxEmptyString};

    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("app"));
    int toolbar_size = cfg->ReadBool(_T("/environment/toolbar_size"),true)?1:0;
    wxString prefix = ConfigManager::GetDataFolder() + _T("/images/ThreadSearch/") + (toolbar_size==1?_T("16x16/"):_T("22x22/"));

    wxString searchButtonPathsEnabled[]  = {prefix + wxT("findf.png"),
                                            prefix + wxT("stop.png") ,
                                            wxEmptyString};

    wxString searchButtonPathsDisabled[] = {prefix + wxT("findfdisabled.png"),
                                            prefix + wxT("stopdisabled.png") ,
                                            wxEmptyString};

    // Gets toolbar search button pointer
    // Changes label/bitmap only if requested
    if ( label != skip )
    {
        m_pBtnSearch->SetToolTip(searchButtonLabels[label]);
        m_pBtnSearch->SetBitmapLabel   (wxBitmap(searchButtonPathsEnabled [label], wxBITMAP_TYPE_PNG));
        m_pBtnSearch->SetBitmapDisabled(wxBitmap(searchButtonPathsDisabled[label], wxBITMAP_TYPE_PNG));
        //{ Toolbar buttons
        m_pToolBar->SetToolNormalBitmap(idBtnSearch,wxBitmap(searchButtonPathsEnabled [label], wxBITMAP_TYPE_PNG));
        m_pToolBar->SetToolDisabledBitmap(idBtnSearch,wxBitmap(searchButtonPathsDisabled[label], wxBITMAP_TYPE_PNG));
        //}
    }

    // Sets enable state
    m_pBtnSearch->Enable(enable);
    m_pToolBar->EnableTool(idBtnSearch,enable);
}


void ThreadSearchView::ShowSearchControls(bool show)
{
    bool     redraw    = false;
    wxSizer* pTopSizer = GetSizer();

    // ThreadSearchPlugin update
    m_ThreadSearchPlugin.SetShowSearchControls(show);

    // We show/hide search controls only if necessary
    if ( m_pBtnSearch->IsShown() != show )
    {
        pTopSizer->Show(m_pSizerSearchItems, show, true);
        redraw = true;
    }

    // When we show search controls, user might have hidden the
    // directory search controls to spare space.
    // In this case, we restore dir control show state
    if ( show == true )
    {
        show = m_ThreadSearchPlugin.GetShowDirControls();
    }

    if ( m_pPnlDirParams->IsShown() != show )
    {
        pTopSizer->Show(m_pSizerSearchDirItems, show, true);
        redraw = true;
    }

    if ( redraw == true )
    {
        pTopSizer->Layout();
    }
}


void ThreadSearchView::ApplySplitterSettings(bool showCodePreview, long splitterMode)
{
    if ( showCodePreview == true )
    {
        if ( (m_pSplitter->IsSplit() == false) || (splitterMode != m_pSplitter->GetSplitMode()) )
        {
            if ( m_pSplitter->IsSplit() == true ) m_pSplitter->Unsplit();
            if ( splitterMode == wxSPLIT_HORIZONTAL )
    {
                m_pSplitter->SplitHorizontally(m_pPnlListLog, m_pPnlPreview);
            }
            else
        {
            m_pSplitter->SplitVertically(m_pPnlPreview, m_pPnlListLog);
        }
    }
    }
    else
    {
        if ( m_pSplitter->IsSplit() == true )
        {
            m_pSplitter->Unsplit(m_pPnlPreview);
        }
    }
}


void ThreadSearchView::SetLoggerType(ThreadSearchLoggerBase::eLoggerTypes lgrType)
{
    if ( lgrType != m_pLogger->GetLoggerType() )
    {
        delete m_pLogger;
        m_pLogger = ThreadSearchLoggerBase::BuildThreadSearchLoggerBase(*this
                                                                       , m_ThreadSearchPlugin
                                                                       , lgrType
                                                                       , m_ThreadSearchPlugin.GetFileSorting()
                                                                       , m_pPnlListLog
                                                                       , idWndLogger);
        m_pPnlListLog->GetSizer()->Add(m_pLogger->GetWindow(), 1, wxEXPAND|wxFIXED_MINSIZE, 0);
        wxSizer* pTopSizer = m_pPnlListLog->GetSizer();
        pTopSizer->Layout();
    }
}


void ThreadSearchView::SetSashPosition(int position, const bool redraw)
{
    m_pSplitter->SetSashPosition(position, redraw);
}


int ThreadSearchView::GetSashPosition() const
{
    return m_pSplitter->GetSashPosition();
}


void ThreadSearchView::SetSearchHistory(const wxArrayString& searchPatterns, const wxArrayString& searchDirs,
                                        const wxArrayString& searchMasks)
{
    m_pCboSearchExpr->Append(searchPatterns);
    if ( searchPatterns.GetCount() > 0 )
    {
        m_pCboSearchExpr->SetSelection(0);
    }
    m_pPnlDirParams->SetSearchHistory(searchDirs, searchMasks);
}


wxArrayString ThreadSearchView::GetSearchHistory() const
{
    return m_pCboSearchExpr->GetStrings();
}

wxArrayString ThreadSearchView::GetSearchDirsHistory() const
{
    return m_pPnlDirParams->GetSearchDirsHistory();
}

wxArrayString ThreadSearchView::GetSearchMasksHistory() const
{
    return m_pPnlDirParams->GetSearchMasksHistory();
}

// BEGIN Duplicated from cbeditor.cpp to apply folding options
void ThreadSearchView::SetMarkerStyle(int marker, int markerType, wxColor fore, wxColor back)
{
    m_pSearchPreview->MarkerDefine(marker, markerType);
    m_pSearchPreview->MarkerSetForeground(marker, fore);
    m_pSearchPreview->MarkerSetBackground(marker, back);
}


void ThreadSearchView::UnderlineFoldedLines(bool underline)
{
    m_pSearchPreview->SetFoldFlags(underline? 16 : 0);
}


void ThreadSearchView::SetFoldingIndicator(int id)
{
    //Arrow
    if(id == 0)
    {
        SetMarkerStyle(wxSCI_MARKNUM_FOLDEROPEN, wxSCI_MARK_ARROWDOWN, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
        SetMarkerStyle(wxSCI_MARKNUM_FOLDER, wxSCI_MARK_ARROW, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
        SetMarkerStyle(wxSCI_MARKNUM_FOLDERSUB, wxSCI_MARK_BACKGROUND, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
        SetMarkerStyle(wxSCI_MARKNUM_FOLDERTAIL, wxSCI_MARK_BACKGROUND, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
        SetMarkerStyle(wxSCI_MARKNUM_FOLDEREND, wxSCI_MARK_ARROW, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
        SetMarkerStyle(wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_ARROWDOWN, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
        SetMarkerStyle(wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_BACKGROUND, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
    }

    //Circle
    else if(id == 1)
    {
        SetMarkerStyle(wxSCI_MARKNUM_FOLDEROPEN, wxSCI_MARK_CIRCLEMINUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
        SetMarkerStyle(wxSCI_MARKNUM_FOLDER, wxSCI_MARK_CIRCLEPLUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
        SetMarkerStyle(wxSCI_MARKNUM_FOLDERSUB, wxSCI_MARK_VLINE, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
        SetMarkerStyle(wxSCI_MARKNUM_FOLDERTAIL, wxSCI_MARK_LCORNERCURVE, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
        SetMarkerStyle(wxSCI_MARKNUM_FOLDEREND, wxSCI_MARK_CIRCLEPLUSCONNECTED, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
        SetMarkerStyle(wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_CIRCLEMINUSCONNECTED, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
        SetMarkerStyle(wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_TCORNER, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
    }

    //Square
    else if(id == 2)
    {
        SetMarkerStyle(wxSCI_MARKNUM_FOLDEROPEN, wxSCI_MARK_BOXMINUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
        SetMarkerStyle(wxSCI_MARKNUM_FOLDER, wxSCI_MARK_BOXPLUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
        SetMarkerStyle(wxSCI_MARKNUM_FOLDERSUB, wxSCI_MARK_VLINE, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
        SetMarkerStyle(wxSCI_MARKNUM_FOLDERTAIL, wxSCI_MARK_LCORNER, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
        SetMarkerStyle(wxSCI_MARKNUM_FOLDEREND, wxSCI_MARK_BOXPLUSCONNECTED, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
        SetMarkerStyle(wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_BOXMINUSCONNECTED, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
        SetMarkerStyle(wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_TCORNER, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
    }

    //Simple
    else if(id == 3)
    {
        SetMarkerStyle(wxSCI_MARKNUM_FOLDEROPEN, wxSCI_MARK_MINUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
        SetMarkerStyle(wxSCI_MARKNUM_FOLDER, wxSCI_MARK_PLUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
        SetMarkerStyle(wxSCI_MARKNUM_FOLDERSUB, wxSCI_MARK_BACKGROUND, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
        SetMarkerStyle(wxSCI_MARKNUM_FOLDERTAIL, wxSCI_MARK_BACKGROUND, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
        SetMarkerStyle(wxSCI_MARKNUM_FOLDEREND, wxSCI_MARK_PLUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
        SetMarkerStyle(wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_MINUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
        SetMarkerStyle(wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_BACKGROUND, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
    }
}
// END Duplicated from cbeditor.cpp to apply folding options

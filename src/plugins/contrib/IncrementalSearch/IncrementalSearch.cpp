/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * Copyright: 2008 Jens Lody
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk.h"

#ifndef CB_PRECOMP
    #include <wx/menu.h>
    #include <wx/settings.h>
    #include <wx/toolbar.h>
    #include <wx/xrc/xmlres.h>

    #include <configmanager.h>
    #include <editormanager.h>
    #include <cbeditor.h>
    #include <logmanager.h>
#endif

#include <wx/combo.h>
#include <wx/listbox.h>
#include <cbstyledtextctrl.h>
#include "IncrementalSearch.h"
#include "IncrementalSearchConfDlg.h"

// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
    PluginRegistrant<IncrementalSearch> reg(_T("IncrementalSearch"));
    const int idIncSearchFocus = wxNewId();
    const int idIncSearchCombo = wxNewId();
}

class cbIncSearchComboPopUp :
    public wxListBox,
    public wxComboPopup
{
public:

    // Create popup control
    virtual bool Create(wxWindow* parent)
    {
        ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));
        wxArrayString choices = cfg->ReadArrayString(_T("/incremental_search/last_searched_items"));
        m_MaxHistoryLen = cfg->ReadInt(_T("/incremental_search/max_items_in_history"),20);

        return wxListBox::Create(parent,wxID_ANY,wxPoint(0,0),wxSize(250,-1), choices, wxLB_SINGLE);
    }

    virtual wxWindow *GetControl() { return this; }

    virtual void SetStringValue(const wxString& s)
    {
        if (s.Length() > 0)
        {
            // Search item index
            int index = FindString(s);

            // Removes item if already in combos box
            if ( index != wxNOT_FOUND )
            {
                Delete(index);
            }

            // Removes last item if max nb item is reached
            if ( GetCount() >= m_MaxHistoryLen)
            {
                // Removes last one
                Delete(GetCount()-1);
            }

            // Adds it to combos
            Insert(s, 0);
            Select(0);
        }
    }

    virtual wxString GetStringValue() const
    {
        return wxListBox::GetStringSelection();
    }

    void SetMaxHistoryLen(int len)
    {
        m_MaxHistoryLen = len;
        // Removes last item until max len is reached
        while ( GetCount() > m_MaxHistoryLen)
        {
            // Removes last one
            Delete(GetCount()-1);
        }
    }
private:
    // Do mouse hot-tracking (which is typical in list popups)
    // and needed for wxMSW
    void OnMouseMove(wxMouseEvent& event)
    {
        SetSelection(HitTest(wxPoint(event.GetX(), event.GetY())));
    }

    // On mouse left up, set the value and close the popup
    void OnMouseClick(cb_unused wxMouseEvent& event)
    {
        Dismiss();

        wxCommandEvent evt(wxEVT_COMMAND_TEXT_UPDATED, idIncSearchCombo);
        Manager::Get()->GetAppFrame()->GetEventHandler()->ProcessEvent(evt);

    }

    unsigned int m_MaxHistoryLen;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(cbIncSearchComboPopUp, wxListBox)
    EVT_MOTION(cbIncSearchComboPopUp::OnMouseMove)
    EVT_LEFT_UP(cbIncSearchComboPopUp::OnMouseClick)
END_EVENT_TABLE()

// events handling
BEGIN_EVENT_TABLE(IncrementalSearch, cbPlugin)
    EVT_MENU(idIncSearchFocus, IncrementalSearch::OnFocusToolbar)
    EVT_TOOL(XRCID("idIncSearchClear"), IncrementalSearch::OnClearText)
    EVT_TOOL(XRCID("idIncSearchPrev"), IncrementalSearch::OnSearchPrev)
    EVT_TOOL(XRCID("idIncSearchNext"), IncrementalSearch::OnSearchNext)
    EVT_TOOL(XRCID("idIncSearchHighlight"), IncrementalSearch::OnToggleHighlight)
    EVT_TOOL(XRCID("idIncSearchSelectOnly"), IncrementalSearch::OnToggleSelectedOnly)
    EVT_TOOL(XRCID("idIncSearchMatchCase"), IncrementalSearch::OnToggleMatchCase)
    EVT_TOOL(XRCID("idIncSearchUseRegex"), IncrementalSearch::OnToggleUseRegex)
    EVT_TEXT(idIncSearchCombo, IncrementalSearch::OnTextChanged)
    EVT_TEXT_ENTER(idIncSearchCombo, IncrementalSearch::OnSearchNext)
#ifndef __WXMSW__
    EVT_MENU(XRCID("idEditPaste"), IncrementalSearch::OnMenuEditPaste)
#endif
END_EVENT_TABLE()

// constructor
IncrementalSearch::IncrementalSearch():
        m_SearchText(wxEmptyString),
        m_textCtrlBG_Default( wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW) ),
        m_pToolbar(0l),
        m_pTextCtrl(0l),
        m_pEditor(0l),
        m_NewPos(-1),
        m_OldPos(-1),
        m_SelStart(-1),
        m_SelEnd(-1),
        m_MinPos(-1),
        m_MaxPos(-1),
        m_flags(0),
        m_Highlight(false),
        m_SelectedOnly(false),
        m_IndicFound(21),
        m_IndicHighlight(22),
        m_LengthFound(0),
        m_LastInsertionPoint(0)

{
    // Make sure our resources are available.
    // In the generated boilerplate code we have no resources but when
    // we add some, it will be nice that this code is in place already ;)
    if (!Manager::LoadResource(_T("IncrementalSearch.zip")))
    {
        NotifyMissingFile(_T("IncrementalSearch.zip"));
    }
}

// destructor
IncrementalSearch::~IncrementalSearch()
{
}

void IncrementalSearch::OnAttach()
{
    // do whatever initialization you need for your plugin
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be TRUE...
    // You should check for it in other functions, because if it
    // is FALSE, it means that the application did *not* "load"
    // (see: does not need) this plugin...
    m_pEditor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    wxMenuBar* mbar = Manager::Get()->GetAppFrame()->GetMenuBar();
    if (mbar->FindItem(idIncSearchFocus)) // if BuildMenu is called afterwards this may not exist yet
        mbar->Enable(idIncSearchFocus, m_pEditor && m_pEditor->GetControl());

    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_ACTIVATED, new cbEventFunctor<IncrementalSearch, CodeBlocksEvent>(this, &IncrementalSearch::OnEditorEvent));
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_DEACTIVATED, new cbEventFunctor<IncrementalSearch, CodeBlocksEvent>(this, &IncrementalSearch::OnEditorEvent));
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_OPEN, new cbEventFunctor<IncrementalSearch, CodeBlocksEvent>(this, &IncrementalSearch::OnEditorEvent));
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_CLOSE, new cbEventFunctor<IncrementalSearch, CodeBlocksEvent>(this, &IncrementalSearch::OnEditorEvent));

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));
    int sel=-1;
    sel = cfg->ReadInt(_T("/incremental_search/highlight_default_state"),0);
    m_Highlight = (sel == 1) || ((sel == 2) && cfg->ReadBool(_T("/incremental_search/highlight_all_occurrences"),false));
    sel = cfg->ReadInt(_T("/incremental_search/selected_default_state"),0);
    m_SelectedOnly = (sel == 1) || ((sel == 2) && cfg->ReadBool(_T("/incremental_search/search_selected_only"),false));
    sel = cfg->ReadInt(_T("/incremental_search/match_case_default_state"),0);
    m_flags |= ((sel == 1) || ((sel == 2) && cfg->ReadInt(_T("/incremental_search/match_case"),false)))?wxSCI_FIND_MATCHCASE:0;
    sel = cfg->ReadInt(_T("/incremental_search/regex_default_state"),0);
    m_flags |= ((sel == 1) || ((sel == 2) && cfg->ReadInt(_T("/incremental_search/regex"),false)))?wxSCI_FIND_REGEXP:0;
}

void IncrementalSearch::OnRelease(bool /*appShutDown*/)
{
    // do de-initialization for your plugin
    // if appShutDown is true, the plugin is unloaded because Code::Blocks is being shut down,
    // which means you must not use any of the SDK Managers
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be FALSE...
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));
    if (cfg->ReadInt(_T("/incremental_search/highlight_default_state"),0) == 2)
    {
        cfg->Write(_T("/incremental_search/highlight_all_occurrences"),m_Highlight);
    }
    if (cfg->ReadInt(_T("/incremental_search/selected_default_state"),0) == 2)
    {
        cfg->Write(_T("/incremental_search/search_selected_only"),m_SelectedOnly);
    }
    if (cfg->ReadInt(_T("/incremental_search/match_case_default_state"),0) == 2)
    {
        cfg->Write(_T("/incremental_search/match_case"),m_flags & wxSCI_FIND_MATCHCASE);
    }
    if (cfg->ReadInt(_T("/incremental_search/regex_default_state"),0) == 2)
    {
        cfg->Write(_T("/incremental_search/regex"),m_flags & wxSCI_FIND_REGEXP);
    }
    cfg->Write(_T("/incremental_search/last_searched_items"), m_pChoice->GetStrings());
    m_pTextCtrl->Disconnect(wxEVT_KEY_DOWN);
    m_pTextCtrl->Disconnect(wxEVT_KILL_FOCUS);

    // TODO : KILLERBOT : menu entries should be removed, right ?????
    // TODO : JENS : no, the menubar gets recreated after a plugin changes (install, uninstall or unload), see MainFrame::PluginsUpdated(plugin, status)
}

cbConfigurationPanel* IncrementalSearch::GetConfigurationPanel(wxWindow* parent)
{
    if ( !IsAttached() )
        return NULL;

    IncrementalSearchConfDlg* cfg = new IncrementalSearchConfDlg(parent);
    return cfg;
}

void IncrementalSearch::BuildMenu(wxMenuBar* menuBar)
{
    //The application is offering its menubar for your plugin,
    //to add any menu items you want...
    //Append any items you need in the menu...
    //NOTE: Be careful in here... The application's menubar is at your disposal.
    if (!m_IsAttached || !menuBar)
    {
        return;
    }
    int idx = menuBar->FindMenu(_("Sea&rch"));
    if (idx != wxNOT_FOUND)
    {
        wxMenu* menu = menuBar->GetMenu(idx);
        wxMenuItemList& items = menu->GetMenuItems();
        wxMenuItem* itemTmp = new wxMenuItem(   menu,
                                                idIncSearchFocus,
                                                _("&Incremental search\tCtrl-I"),
                                                _("Set focus on Incremental Search input and show the toolbar, if hidden") );

        itemTmp->SetBitmap(wxBitmap(wxXmlResource::Get()->LoadBitmap(_T("MenuBitmap"))));
        // find "Find previous" and insert after it
        size_t i = 0;
        for (i = 0; i < items.GetCount(); ++i)
        {
#if wxCHECK_VERSION(2,8,5)
            if (items[i]->GetLabelText(items[i]->GetItemLabelText()) == _("Find previous"))
#else
            if (items[i]->GetLabelFromText(items[i]->GetLabel()) == _("Find previous"))
#endif

            {
                ++i;
                break;
            }
        }
        // if not found, just append with seperator
        if(i == items.GetCount())
        {
            menu->InsertSeparator(i++);
        }
        menu->Insert(i, itemTmp);
        menuBar->Enable(idIncSearchFocus, m_pEditor && m_pEditor->GetControl());
    }
}

void IncrementalSearch::OnEditorEvent(CodeBlocksEvent& event)
{
    if (!m_pToolbar || !m_pComboCtrl || !m_pTextCtrl) // skip if toolBar is not (yet) build
    {
        event.Skip();
        return;
    }
    m_pEditor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    m_pComboCtrl->Enable(m_pEditor && m_pEditor->GetControl());
    wxMenuBar* mbar = Manager::Get()->GetAppFrame()->GetMenuBar();
    mbar->Enable(idIncSearchFocus, m_pComboCtrl->IsEnabled());

    m_pToolbar->EnableTool(XRCID("idIncSearchClear"), !m_SearchText.empty());

    if (m_pComboCtrl->IsEnabled())
    {
        m_SearchText=m_pTextCtrl->GetValue();
        m_pToolbar->EnableTool(XRCID("idIncSearchPrev"), !m_SearchText.empty() && ((m_flags & wxSCI_FIND_REGEXP) == 0));
        m_pToolbar->EnableTool(XRCID("idIncSearchNext"), !m_SearchText.empty());
        m_NewPos=m_pEditor->GetControl()->GetCurrentPos();
        m_OldPos=m_NewPos;
    }
    else
    {
        m_pToolbar->EnableTool(XRCID("idIncSearchPrev"), false);
        m_pToolbar->EnableTool(XRCID("idIncSearchNext"), false);
    }
    event.Skip();
}

bool IncrementalSearch::BuildToolBar(wxToolBar* toolBar)
{
    //The application is offering its toolbar for your plugin,
    //to add any toolbar items you want...
    //Append any items you need on the toolbar...
//    return false;
    //Build toolbar
    if (!m_IsAttached || !toolBar)
    {
        return false;
    }
    wxString is16x16 = Manager::isToolBar16x16(toolBar) ? _T("_16x16") : _T("");
    Manager::Get()->AddonToolBar(toolBar,_T("incremental_search_toolbar") + is16x16);
    m_pToolbar  = toolBar;
    m_pToolbar->EnableTool(XRCID("idIncSearchClear"), false);
    m_pToolbar->EnableTool(XRCID("idIncSearchPrev"), false);
    m_pToolbar->EnableTool(XRCID("idIncSearchNext"), false);
    m_pToolbar->SetInitialSize();

    m_pComboCtrl = new wxComboCtrl(toolBar, idIncSearchCombo, wxEmptyString, wxDefaultPosition, wxSize(160,-1));
    if (m_pComboCtrl)
    {
#if !wxCHECK_VERSION(3,0,0) || WXWIN_COMPATIBILITY_2_8
        m_pComboCtrl->SetTextIndent(0);
#endif // !wxCHECK_VERSION(3,0,0) || WXWIN_COMPATIBILITY_2_8
        m_pToolbar->InsertControl(1, m_pComboCtrl);
        m_pToolbar->Realize();
        m_pTextCtrl = m_pComboCtrl->GetTextCtrl();
        if (m_pTextCtrl)
        {
            m_pTextCtrl->SetWindowStyleFlag(wxTE_PROCESS_ENTER|wxTE_NOHIDESEL|wxBORDER_NONE);
            m_pChoice = new cbIncSearchComboPopUp();
            m_pComboCtrl->SetPopupControl(m_pChoice);
            m_pTextCtrl->Connect(wxEVT_KEY_DOWN,
                                 (wxObjectEventFunction) (wxEventFunction) (wxCharEventFunction)
                                 &IncrementalSearch::OnKeyDown , 0, this);
            m_pTextCtrl->Connect(wxEVT_KILL_FOCUS ,
                                   (wxObjectEventFunction)(wxEventFunction)(wxFocusEventFunction)
                                   &IncrementalSearch::OnKillFocus, 0, this);
            m_textCtrlBG_Default = m_pTextCtrl->GetBackgroundColour();
            m_pComboCtrl->Enable(m_pEditor && m_pEditor->GetControl());
            m_pToolbar->ToggleTool(XRCID("idIncSearchHighlight"),m_Highlight);
            m_pToolbar->ToggleTool(XRCID("idIncSearchSelectOnly"),m_SelectedOnly);
            m_pToolbar->ToggleTool(XRCID("idIncSearchMatchCase"),m_flags & wxSCI_FIND_MATCHCASE);
            m_pToolbar->ToggleTool(XRCID("idIncSearchUseRegex"),m_flags & wxSCI_FIND_REGEXP);
            return true;
        }
    }
    return false;
}

void IncrementalSearch::SetMaxHistoryLen(int len)
{
    if (!m_pChoice)
        return;

    m_pChoice->SetMaxHistoryLen(len);
}

void IncrementalSearch::OnKeyDown(wxKeyEvent& event)
{
    if(m_pTextCtrl)
    {
        m_LastInsertionPoint = m_pTextCtrl->GetInsertionPoint();
    }

    if(!m_IsAttached || !m_pEditor || !m_pEditor->GetControl() )
    {
        event.Skip();
        return;
    }
    if(event.GetModifiers() == wxMOD_ALT && event.GetKeyCode() == WXK_DELETE)
    {
        DoClearText();
    }
    else if(event.GetModifiers() == wxMOD_SHIFT && event.GetKeyCode() == WXK_RETURN)
    {
        if(m_pToolbar->GetToolEnabled(XRCID("idIncSearchPrev")))
           DoSearchPrev();
    }
    else if(event.GetModifiers() == wxMOD_NONE && event.GetKeyCode() == WXK_ESCAPE)
    {
        cbStyledTextCtrl* control = m_pEditor->GetControl();
        // delete all stylings for found phrases
        control->SetIndicatorCurrent(m_IndicFound);
        control->IndicatorClearRange(0, control->GetLength());
        // then for highlighted phrases
        control->SetIndicatorCurrent(m_IndicHighlight);
        control->IndicatorClearRange(0, control->GetLength());
        control->GotoPos(m_NewPos);
        if(Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/incremental_search/select_found_text_on_escape"),false))
        {
            m_SelStart = m_NewPos;
            m_SelEnd = m_SelStart + m_LengthFound;
            m_OldPos = m_NewPos;
            control->SetSelectionVoid(m_SelStart, m_SelEnd);
        }
        control->SetFocus();
    }
    else
    {
        event.Skip();
    }
}

void IncrementalSearch::OnFocusToolbar(wxCommandEvent& /*event*/)
{
    if (!m_IsAttached)
    {
        return;
    }
    DoFocusToolbar();

    if (Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/incremental_search/select_text_on_focus"), false))
    {
        m_pTextCtrl->SetSelection(-1,-1);
    }
    else
    {
        m_pTextCtrl->SetInsertionPoint(m_LastInsertionPoint);
    }
}

void IncrementalSearch::DoFocusToolbar()
{
    if (!IsWindowReallyShown(m_pToolbar))
    {
        CodeBlocksDockEvent evt(cbEVT_SHOW_DOCK_WINDOW);
        evt.pWindow = (wxWindow*)m_pToolbar;
        Manager::Get()->ProcessEvent(evt);
    }
    m_pTextCtrl->SetFocus();
}

void IncrementalSearch::OnToggleHighlight(wxCommandEvent& /*event*/)
{
    DoToggleHighlight(m_pToolbar->GetToolState(XRCID("idIncSearchHighlight")));
}

void IncrementalSearch::DoToggleHighlight(bool checked)
{
    m_Highlight = checked;
    if ( !m_pEditor || !m_pEditor->GetControl())
    {
        return;
    }
    SearchText();
}

void IncrementalSearch::OnToggleSelectedOnly(wxCommandEvent& /*event*/)
{
    DoToggleSelectedOnly(m_pToolbar->GetToolState(XRCID("idIncSearchSelectOnly")));
}

void IncrementalSearch::DoToggleSelectedOnly(bool checked)
{
    m_SelectedOnly = checked;
    if (!m_pEditor || !m_pEditor->GetControl())
    {
        return;
    }
    SearchText();
}

void IncrementalSearch::OnToggleMatchCase(wxCommandEvent& /*event*/)
{
    DoToggleMatchCase(m_pToolbar->GetToolState(XRCID("idIncSearchMatchCase")));
}

void IncrementalSearch::DoToggleMatchCase(bool checked)
{
    if(checked)
    {
        m_flags |= wxSCI_FIND_MATCHCASE;
    }
    else
    {
        m_flags &=  ~wxSCI_FIND_MATCHCASE;
    }
    if (!m_pEditor || !m_pEditor->GetControl())
    {
        return;
    }
    SearchText();
}

void IncrementalSearch::OnToggleUseRegex(wxCommandEvent& /*event*/)
{
    DoToggleUseRegex(m_pToolbar->GetToolState(XRCID("idIncSearchUseRegex")));
}

void IncrementalSearch::DoToggleUseRegex(bool checked)
{
    if(checked)
    {
        m_flags |= wxSCI_FIND_REGEXP;
    }
    else
    {
        m_flags &=  ~wxSCI_FIND_REGEXP;
    }
    if (!m_pEditor || !m_pEditor->GetControl())
    {
        return;
    }
    SearchText();
}

void IncrementalSearch::OnTextChanged(wxCommandEvent& /*event*/)
{
    if (!m_pEditor || !m_pEditor->GetControl())
    {
        return;
    }
    SearchText();
}

void IncrementalSearch::OnKillFocus(wxCommandEvent& /*event*/)
{
    if(m_pTextCtrl)
    {
        m_LastInsertionPoint = m_pTextCtrl->GetInsertionPoint();
    }
}

void IncrementalSearch::VerifyPosition()
{
    if (!m_pEditor || !m_pEditor->GetControl())
    {
        return;
    }
    // if selection changed, the user has clicked with the mous inside the editor
    // so set new startposition for search and remember it
    cbStyledTextCtrl* control = m_pEditor->GetControl();
    m_SelStart=control->GetSelectionStart();
    m_SelEnd=control->GetSelectionEnd();
    if (m_OldPos != m_SelEnd)
    {
        m_OldPos=m_SelEnd;
        m_NewPos=m_SelEnd;
    }
}

void IncrementalSearch::SetRange()
{
    if (!m_pEditor || !m_pEditor->GetControl())
    {
        return;
    }
    if ( m_SelectedOnly)
    {
        m_MinPos=m_SelStart;
        m_MaxPos=m_SelEnd;
    }
    else
    {
        m_MinPos=0;
        m_MaxPos=m_pEditor->GetControl()->GetLength();
    }
    m_NewPos=std::min(m_NewPos, m_MaxPos);
    m_NewPos=std::max(m_NewPos, m_MinPos);
}

void IncrementalSearch::SearchText()
{
    // fetch the entered text
    m_SearchText=m_pTextCtrl->GetValue();
    // renew the start position, the user might have changed it by moving the caret
    VerifyPosition();
    SetRange();
    if (!m_SearchText.empty())
    {
        // perform search
        m_pToolbar->EnableTool(XRCID("idIncSearchClear"), true);
        m_pToolbar->EnableTool(XRCID("idIncSearchPrev"), (m_flags & wxSCI_FIND_REGEXP) == 0);
        m_pToolbar->EnableTool(XRCID("idIncSearchNext"), true);
        m_pChoice->SetStringValue(m_SearchText);
        DoSearch(m_NewPos);
    }
    else
    {
        // if no text
        m_pToolbar->EnableTool(XRCID("idIncSearchClear"), false);
        m_pToolbar->EnableTool(XRCID("idIncSearchPrev"), false);
        m_pToolbar->EnableTool(XRCID("idIncSearchNext"), false);
        // reset the backgroundcolor of the text-control
        m_pTextCtrl->SetBackgroundColour(m_textCtrlBG_Default);
        // windows does not update the backgroundcolor immediately, so we have to force it here
        #ifdef __WXMSW__
        m_pTextCtrl->Refresh();
        m_pTextCtrl->Update();
        #endif
    }
    HighlightText();
}

void IncrementalSearch::OnClearText(wxCommandEvent& /*event*/)
{
    DoClearText();
}

void IncrementalSearch::DoClearText()
{
    m_pTextCtrl->Clear();
    SearchText();
}

void IncrementalSearch::OnSearchPrev(wxCommandEvent& /*event*/)
{
    DoSearchPrev();
}

void IncrementalSearch::DoSearchPrev()
{
    VerifyPosition();
    // (re-)define m_MinPos and m_MaxPos, they might have changed
    SetRange();
    // we search backward from one character before the ending of the last found phrase
    DoSearch(m_NewPos + m_LengthFound - 1, m_MaxPos, m_MinPos);
    HighlightText();
}

void IncrementalSearch::OnSearchNext(wxCommandEvent& /*event*/)
{
    DoSearchNext();
}

void IncrementalSearch::DoSearchNext()
{
    VerifyPosition();
    // start search from the next character
    // (re-)define m_MinPos and m_MaxPos, they might have changed
    SetRange();
    // we search backward from one character before the ending of the last found phrase
    DoSearch(m_NewPos + 1, m_MinPos, m_MaxPos);
    HighlightText();
}

void IncrementalSearch::HighlightText()
{
    if (!m_pEditor || !m_pEditor->GetControl())
    {
        return;
    }
    cbStyledTextCtrl* control = m_pEditor->GetControl();
    // first delete all stylings for found phrases
    control->SetIndicatorCurrent(m_IndicFound);
    control->IndicatorClearRange(0, control->GetLength());
    // then for highlighted phrases
    control->SetIndicatorCurrent(m_IndicHighlight);
    control->IndicatorClearRange(0, control->GetLength());
    // then set the new ones (if any)
    if (m_NewPos != wxSCI_INVALID_POSITION && !m_SearchText.empty())
    {
        ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));
        wxColour colourTextFound(cfg->ReadColour(_T("/incremental_search/text_found_colour"), wxColour(160, 32, 240)));

        // center last found phrase on the screen, if wanted
        if (cfg->ReadBool(_T("/incremental_search/center_found_text_on_screen"), true))
        {
            int line = control->LineFromPosition(m_NewPos);
            int onScreen = control->LinesOnScreen() >> 1;
            int l1 = line - onScreen;
            int l2 = line + onScreen;
            for (int l=l1; l<=l2;l+=2)      // unfold visible lines on screen
                control->EnsureVisible(l);
            control->GotoLine(l1);          // center selection on screen
            control->GotoLine(l2);
        }
        // make sure found text is visible, even if it's in a column far right
        control->GotoPos(m_NewPos+m_SearchText.length());
        control->EnsureCaretVisible();
        control->GotoPos(m_NewPos);
        control->SearchAnchor();
        // and highlight it
        cbStyledTextCtrl* ctrlLeft = m_pEditor->GetLeftSplitViewControl();
        ctrlLeft->IndicatorSetForeground(m_IndicFound, colourTextFound);
        ctrlLeft->IndicatorSetStyle(m_IndicFound, wxSCI_INDIC_HIGHLIGHT);
#ifndef wxHAVE_RAW_BITMAP
        // If wxWidgets is build without rawbitmap-support, the indicators become opaque
        // and hide the text, so we show them under the text.
        // Not enabled as default, because the readability is a little bit worse.
        ctrlLeft->IndicatorSetUnder(m_IndicFound,true);
#endif
        ctrlLeft->SetIndicatorCurrent(m_IndicFound);

         cbStyledTextCtrl* ctrlRight = m_pEditor->GetRightSplitViewControl();
        if(ctrlRight)
        {
            ctrlRight->IndicatorSetForeground(m_IndicFound, colourTextFound);
            ctrlRight->IndicatorSetStyle(m_IndicFound, wxSCI_INDIC_HIGHLIGHT);
#ifndef wxHAVE_RAW_BITMAP
            ctrlRight->IndicatorSetUnder(m_IndicFound,true);
#endif
            ctrlRight->SetIndicatorCurrent(m_IndicFound);
        }
        control->IndicatorFillRange(m_NewPos, m_LengthFound);

        if (m_Highlight)
        {
            // highlight all occurrences of the found phrase if wanted
            wxColour colourTextHighlight(cfg->ReadColour(_T("/incremental_search/highlight_colour"), wxColour(255, 165, 0)));
            ctrlLeft->IndicatorSetForeground(m_IndicHighlight, colourTextHighlight);
            ctrlLeft->IndicatorSetStyle(m_IndicHighlight, wxSCI_INDIC_HIGHLIGHT);
            ctrlLeft->SetIndicatorCurrent(m_IndicHighlight);
            if(ctrlRight)
            {
                ctrlRight->IndicatorSetForeground(m_IndicHighlight, colourTextHighlight);
                ctrlRight->IndicatorSetStyle(m_IndicHighlight, wxSCI_INDIC_HIGHLIGHT);
                ctrlRight->SetIndicatorCurrent(m_IndicHighlight);
            }
            int actualLength=0; // needed for regex-search, because the length of found text can vary
            for ( int pos = control->FindText(m_MinPos, m_MaxPos, m_SearchText, m_flags, &actualLength);
                    pos != wxSCI_INVALID_POSITION && actualLength > 0;
                    pos = control->FindText(pos+=1, m_MaxPos, m_SearchText, m_flags, &actualLength) )
            {
                // check that this occurrence is not the same as the one we just found
                if ( pos > (m_NewPos + m_LengthFound) || pos < m_NewPos )
                {
                    // highlight it
                    control->EnsureVisible(control->LineFromPosition(pos)); // make sure line is Visible, if it was folded
                    control->IndicatorFillRange(pos, actualLength);
                }
            }
        }
    }
    // reset selection, without moving caret, as SetSelection does
    control->SetAnchor(m_SelStart);
    control->SetCurrentPos(m_SelEnd);
    // make sure Toolbar (textctrl) is still focused, to make it reachable for keystrokes
    DoFocusToolbar();
}

void IncrementalSearch::DoSearch(int fromPos, int startPos, int endPos)
{
    if (!m_pEditor || !m_pEditor->GetControl())
    {
        return;
    }
    cbStyledTextCtrl* control = m_pEditor->GetControl();
    if (startPos == wxSCI_INVALID_POSITION && endPos == wxSCI_INVALID_POSITION)
    {
        startPos = m_MinPos;
        endPos = m_MaxPos;
    }
    // reset the backgroundcolor of the text-control
    m_pTextCtrl->SetBackgroundColour(m_textCtrlBG_Default);

    m_NewPos=control->FindText(fromPos, endPos, m_SearchText, m_flags, &m_LengthFound);

    if (m_NewPos == wxSCI_INVALID_POSITION || m_LengthFound == 0)
    {
        ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));
        wxColour colourTextCtrlBG_Wrapped(cfg->ReadColour(_T("/incremental_search/wrapped_colour"), wxColour(127, 127, 255)));
        // if not found or out of range, wrap search
        // show that search wrapped, by colouring the textCtrl
        m_pTextCtrl->SetBackgroundColour(colourTextCtrlBG_Wrapped);
        // search again
        m_NewPos=control->FindText(startPos, endPos, m_SearchText, m_flags, &m_LengthFound);
        if (m_NewPos == wxSCI_INVALID_POSITION  || m_LengthFound == 0)
        {
            wxColour colourTextCtrlBG_NotFound(cfg->ReadColour(_T("/incremental_search/text_not_found_colour"), wxColour(255, 127, 127)));
            // if still not found, show it by colouring the textCtrl
            m_pTextCtrl->SetBackgroundColour(colourTextCtrlBG_NotFound);
        }
    }
    // windows does not update the backgroundcolor immediately, so we have to force it here
    #ifdef __WXMSW__
    m_pTextCtrl->Refresh();
    m_pTextCtrl->Update();
    #endif
}

#ifndef __WXMSW__
void IncrementalSearch::OnMenuEditPaste(wxCommandEvent& event)
{
    // Process clipboard data only if we have the focus
    if ( !IsAttached() )
    {
        event.Skip();
        return;
    }

    wxWindow* pFocused = wxWindow::FindFocus();
    if (!pFocused)
    {
        event.Skip();
        return;
    }

    if (pFocused == m_pTextCtrl)
        m_pTextCtrl->Paste();
    else
        event.Skip();
}
#endif

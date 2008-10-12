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
    #include <wx/xrc/xmlres.h>
    #include <configmanager.h>
    #include <editormanager.h>
    #include <cbeditor.h>
#endif

#include <cbstyledtextctrl.h>
#include "IncrementalSearch.h"
#include "IncrementalSearchConfDlg.h"

// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
    PluginRegistrant<IncrementalSearch> reg(_T("IncrementalSearch"));
    int idIncSearchFocus = wxNewId();
}


// events handling
BEGIN_EVENT_TABLE(IncrementalSearch, cbPlugin)
    EVT_MENU(idIncSearchFocus, IncrementalSearch::OnFocusToolbar)
    EVT_TOOL(XRCID("idIncSearchClear"), IncrementalSearch::OnClearText)
    EVT_TOOL(XRCID("idIncSearchPrev"), IncrementalSearch::OnSearchPrev)
    EVT_TOOL(XRCID("idIncSearchNext"), IncrementalSearch::OnSearchNext)
    EVT_TOOL(XRCID("idIncSearchHighlight"), IncrementalSearch::OnToggleHighlight)
    EVT_TOOL(XRCID("idIncSearchSelectOnly"), IncrementalSearch::OnToggleSelectedOnly)
    EVT_TOOL(XRCID("idIncSearchMatchCase"), IncrementalSearch::OnToggleMatchCase)
    EVT_TEXT(XRCID("idIncSearchText"), IncrementalSearch::OnTextChanged)
    EVT_TEXT_ENTER(XRCID("idIncSearchText"), IncrementalSearch::OnSearchNext)
END_EVENT_TABLE()

// constructor
IncrementalSearch::IncrementalSearch():
        m_SearchText(wxEmptyString),
        m_textCtrlBG_Default( wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW) ),
        m_pToolbar(0l),
        m_pTextCtrl(0l),
        m_pControl(0l),
        m_NewPos(-1),
        m_OldPos(-1),
        m_SelStart(-1),
        m_SelEnd(-1),
        m_MinPos(-1),
        m_MaxPos(-1),
        m_flags(0),
        m_Highlight(false),
        m_SelectedOnly(false)

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
    m_pControl = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor() ? Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor()->GetControl() : 0;
    wxMenuBar* mbar = Manager::Get()->GetAppFrame()->GetMenuBar();
    mbar->Enable(idIncSearchFocus, m_pControl);

    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_ACTIVATED, new cbEventFunctor<IncrementalSearch, CodeBlocksEvent>(this, &IncrementalSearch::OnEditorEvent));
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_OPEN, new cbEventFunctor<IncrementalSearch, CodeBlocksEvent>(this, &IncrementalSearch::OnEditorEvent));
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_CLOSE, new cbEventFunctor<IncrementalSearch, CodeBlocksEvent>(this, &IncrementalSearch::OnEditorEvent));

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));
    int sel=-1;
    sel = cfg->ReadInt(_T("/incremental_search/highlight_default_state"),0);
    m_Highlight = (sel == 1) || ((sel == 2) && cfg->ReadBool(_T("/incremental_search/highlight_all_occurrences"),false));
    sel = cfg->ReadInt(_T("/incremental_search/selected_default_state"),0);
    m_SelectedOnly = (sel == 1) || ((sel == 2) && cfg->ReadBool(_T("/incremental_search/search_selected_only"),false));
    sel = cfg->ReadInt(_T("/incremental_search/match_case_default_state"),0);
    m_flags = ((sel == 1) || ((sel == 2) && cfg->ReadInt(_T("/incremental_search/match_case"),false)))?wxSCI_FIND_MATCHCASE:0;
}

void IncrementalSearch::OnRelease(bool appShutDown)
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
    int idx = menuBar->FindMenu(_("Search"));
    if (idx != wxNOT_FOUND)
    {
        wxMenu* menu = menuBar->GetMenu(idx);
        wxMenuItemList& items = menu->GetMenuItems();
        wxMenuItem* itemTmp = new wxMenuItem(   menu,
                                                idIncSearchFocus,
                                                _("&Incremental search\tCtrl-I"),
                                                _("Set focus on Incremental Search input and show the toolbar, if hidden") );

        itemTmp->SetBitmap(wxBitmap(wxXmlResource::Get()->LoadBitmap(_T("MenuBitmap"))));
        itemTmp->Enable(m_pControl);
        // find "Find previous" and insert after it
        for (size_t i = 0; i < items.GetCount(); ++i)
        {
#if wxCHECK_VERSION(2,8,5)
            if (items[i]->GetLabelText(items[i]->GetItemLabelText()) == _("Find previous"))
#else
            if (items[i]->GetLabelFromText(items[i]->GetLabel()) == _("Find previous"))
#endif

            {
                menu->Insert(++i, itemTmp);
                return;
            }
        }

        // not found, just append
        menu->AppendSeparator();
        menu->Append(itemTmp);
    }
}

void IncrementalSearch::OnEditorEvent(CodeBlocksEvent& event)
{
    if (!m_pToolbar) // skip if toolBar is not (yet) build
    {
        event.Skip();
        return;
    }
    m_pControl = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor() ? Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor()->GetControl() : 0;
    m_pTextCtrl->Enable(m_pControl != 0);
    wxMenuBar* mbar = Manager::Get()->GetAppFrame()->GetMenuBar();
    mbar->Enable(idIncSearchFocus, m_pControl);

    m_pToolbar->EnableTool(XRCID("idIncSearchClear"), !m_SearchText.empty());

    if (m_pControl)
    {
        m_SearchText=m_pTextCtrl->GetValue();
        m_pToolbar->EnableTool(XRCID("idIncSearchPrev"), !m_SearchText.empty());
        m_pToolbar->EnableTool(XRCID("idIncSearchNext"), !m_SearchText.empty());
        m_NewPos=m_pControl->GetCurrentPos();
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
    toolBar->Realize();
    m_pToolbar  =toolBar;
    m_pToolbar->EnableTool(XRCID("idIncSearchClear"), false);
    m_pToolbar->EnableTool(XRCID("idIncSearchPrev"), false);
    m_pToolbar->EnableTool(XRCID("idIncSearchNext"), false);
    m_pToolbar->SetInitialSize();

    m_pTextCtrl = XRCCTRL(*toolBar, "idIncSearchText", wxTextCtrl);
    if (m_pTextCtrl)
    {
        m_textCtrlBG_Default = m_pTextCtrl->GetBackgroundColour();
        m_pToolbar->ToggleTool(XRCID("idIncSearchHighlight"),m_Highlight);
        m_pToolbar->ToggleTool(XRCID("idIncSearchSelectOnly"),m_SelectedOnly);
        m_pToolbar->ToggleTool(XRCID("idIncSearchMatchCase"),m_flags & wxSCI_FIND_MATCHCASE);
        return true;
    }
    else
    {
        return false;
    }
}
void IncrementalSearch::OnFocusToolbar(wxCommandEvent& event)
{
    if (!m_IsAttached)
    {
        return;
    }
    if (!IsWindowReallyShown(m_pToolbar))
    {
        CodeBlocksDockEvent evt(cbEVT_SHOW_DOCK_WINDOW);
        evt.pWindow = (wxWindow*)m_pToolbar;
        Manager::Get()->ProcessEvent(evt);
    }
    m_pTextCtrl->SetFocus();
}

void IncrementalSearch::OnToggleHighlight(wxCommandEvent& event)
{
    m_Highlight = m_pToolbar->GetToolState(XRCID("idIncSearchHighlight"));
    if (!m_pControl)
    {
        return;
    }
    SearchText();
}

void IncrementalSearch::OnToggleSelectedOnly(wxCommandEvent& event)
{
    m_SelectedOnly = m_pToolbar->GetToolState(XRCID("idIncSearchSelectOnly"));
    if (!m_pControl)
    {
        return;
    }
    SearchText();
}

void IncrementalSearch::OnToggleMatchCase(wxCommandEvent& event)
{
    m_flags = m_pToolbar->GetToolState(XRCID("idIncSearchMatchCase"))?wxSCI_FIND_MATCHCASE:0;
    if (!m_pControl)
    {
        return;
    }
    SearchText();
}

void IncrementalSearch::OnTextChanged(wxCommandEvent& event)
{
    if (!m_pControl)
    {
        return;
    }
    SearchText();
}

void IncrementalSearch::VerifyPosition()
{
    // if selection changed, the user has clicked with the mous inside the editor
    // so set new startposition for search and remember it
    m_SelStart=m_pControl->GetSelectionStart();
    m_SelEnd=m_pControl->GetSelectionEnd();
    if (m_OldPos != m_SelEnd)
    {
        m_OldPos=m_SelEnd;
        m_NewPos=m_SelEnd;
    }
}

void IncrementalSearch::VerifyRange()
{
    // if we should only search in selection, we have to verify that we did not leave its range
    // searchForward can work without it, because we can specify a range to search in, but searchBackward
    // uses FindPrev and so we can not specify a range.
    // to make the functions more similar to each other, we use it in both
    if ( m_SelectedOnly)
    {
        m_MinPos=m_SelStart;
        m_MaxPos=m_SelEnd;
        if ((m_NewPos + (int)m_SearchText.length()) > m_SelEnd || m_NewPos < m_SelStart)
        {
            m_NewPos=wxSCI_INVALID_POSITION;
        }
    }
    else
    {
        m_MinPos=0;
        m_MaxPos=m_pControl->GetLength();
    }
}

void IncrementalSearch::SearchText()
{
    // fetch the entered text
    m_SearchText=m_pTextCtrl->GetValue();
    // renew the start position, the user might have changed it by moving the caret
    VerifyPosition();
    if (!m_SearchText.empty())
    {
        // perform search
        m_pToolbar->EnableTool(XRCID("idIncSearchClear"), true);
        m_pToolbar->EnableTool(XRCID("idIncSearchPrev"), true);
        m_pToolbar->EnableTool(XRCID("idIncSearchNext"), true);
        SearchForward(m_NewPos);
    }
    else
    {
        // if no text
        m_pToolbar->EnableTool(XRCID("idIncSearchClear"), false);
        m_pToolbar->EnableTool(XRCID("idIncSearchPrev"), false);
        m_pToolbar->EnableTool(XRCID("idIncSearchNext"), false);
    }
    HighlightText();
}

void IncrementalSearch::OnClearText(wxCommandEvent& event)
{
    m_pTextCtrl->Clear();
    SearchText();
}

void IncrementalSearch::OnSearchPrev(wxCommandEvent& event)
{
    VerifyPosition();
    // we search backward from one character before the ending of the last found phrase
    SearchBackward(m_NewPos+m_SearchText.length()-1);
    HighlightText();
}

void IncrementalSearch::OnSearchNext(wxCommandEvent& event)
{
    VerifyPosition();
    // start search from the next character
    SearchForward(m_NewPos + 1);
    HighlightText();
}

void IncrementalSearch::HighlightText()
{
    // first delete all stylings for found phrases
    m_pControl->StartStyling(0, 0x40);
    m_pControl->SetStyling(m_pControl->GetLength(), 0x00);
    // then for highlighted phrases
    m_pControl->StartStyling(0, 0x80);
    m_pControl->SetStyling(m_pControl->GetLength(), 0x00);
    // then set the new ones (if any)
    if (m_NewPos != wxSCI_INVALID_POSITION && !m_SearchText.empty())
    {
        ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));
        wxColour colourTextFound(   cfg->ReadInt(_T("/incremental_search/text_found_colour_red_value"),   0xa0),
                                    cfg->ReadInt(_T("/incremental_search/text_found_colour_green_value"), 0x20),
                                    cfg->ReadInt(_T("/incremental_search/text_found_colour_blue_value"),  0xf0) );

        // center last found phrase on the screen, if wanted
        if (cfg->ReadBool(_T("/incremental_search/center_found_text_on_screen"), true))
        {
            int line = m_pControl->LineFromPosition(m_NewPos);
            int onScreen = m_pControl->LinesOnScreen() >> 1;
            int l1 = line - onScreen;
            int l2 = line + onScreen;
            for (int l=l1; l<=l2;l+=2)      // unfold visible lines on screen
                m_pControl->EnsureVisible(l);
            m_pControl->GotoLine(l1);          // center selection on screen
            m_pControl->GotoLine(l2);
        }
        m_pControl->GotoPos(m_NewPos);
        m_pControl->SearchAnchor();
        // and highlight it
        m_pControl->IndicatorSetForeground(1, colourTextFound);
        m_pControl->IndicatorSetStyle(1, wxSCI_INDIC_HIGHLIGHT);
        m_pControl->StartStyling(m_NewPos,0x40);
        m_pControl->SetStyling(m_SearchText.Len(), 0x40);
        // make sure line is Visible, if it was folded
        m_pControl->EnsureVisible(m_pControl->LineFromPosition(m_NewPos));
        if (m_Highlight)
        {
            // highlight all occurrences of the found phrase if wanted
            wxColour colourTextHighlight(   cfg->ReadInt(_T("/incremental_search/highlight_colour_red_value"),   0xff),
                                            cfg->ReadInt(_T("/incremental_search/highlight_colour_green_value"), 0xa5),
                                            cfg->ReadInt(_T("/incremental_search/highlight_colour_blue_value"),  0x00) );
            m_pControl->IndicatorSetForeground(2, colourTextHighlight);
            m_pControl->IndicatorSetStyle(2, wxSCI_INDIC_HIGHLIGHT);
            for ( int pos = m_pControl->FindText(m_MinPos, m_MaxPos, m_SearchText, m_flags);
                    pos != wxSCI_INVALID_POSITION ;
                    pos = m_pControl->FindText(pos+=1, m_MaxPos, m_SearchText, m_flags) )
            {
                // check that this occurrence is not the same as the one we just found
                if ( pos != m_NewPos )
                {
                    // highlight it
                    m_pControl->EnsureVisible(m_pControl->LineFromPosition(pos)); // make sure line is Visible, if it was folded
                    m_pControl->StartStyling(pos, 0x80);
                    m_pControl->SetStyling(m_SearchText.Len(), 0x80);
                }
            }
        }
    }
    // reset selection, without moving caret, as SetSelection does
    m_pControl->SetAnchor(m_SelStart);
    m_pControl->SetCurrentPos(m_SelEnd);
}

void IncrementalSearch::SearchForward(int fromPos)
{
    // reset the backgroundcolor of the text-control
    m_pTextCtrl->SetBackgroundColour(m_textCtrlBG_Default);

    // jump to startposition for search
    m_pControl->GotoPos(fromPos);
    m_pControl->SearchAnchor();
    // do the search
    m_NewPos=m_pControl->SearchNext(m_flags,m_SearchText);
    // verify that we are in allowed range (in case we only search in selection),
    // and define Min- and MaxPos (the selection can have changed since last search)
    VerifyRange();
    if (m_NewPos == wxSCI_INVALID_POSITION)
    {
        ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));
        wxColour colourTextCtrlBG_Wrapped(  cfg->ReadInt(_T("/incremental_search/wrapped_colour_red_value"),   0x7f),
                                            cfg->ReadInt(_T("/incremental_search/wrapped_colour_green_value"), 0x7f),
                                            cfg->ReadInt(_T("/incremental_search/wrapped_colour_blue_value"),  0xff) );
        // if not found or out of range, wrap search
        m_pControl->GotoPos(m_MinPos);
        m_pControl->SearchAnchor();
        // show that search wrapped, by colouring the textCtrl
        m_pTextCtrl->SetBackgroundColour(colourTextCtrlBG_Wrapped);
        // search again
        m_NewPos=m_pControl->SearchNext(m_flags,m_SearchText);
        VerifyRange();
        if (m_NewPos == wxSCI_INVALID_POSITION)
        {
            wxColour colourTextCtrlBG_NotFound( cfg->ReadInt(_T("/incremental_search/text_not_found_colour_red_value"),   0xff),
                                                cfg->ReadInt(_T("/incremental_search/text_not_found_colour_green_value"), 0x7f),
                                                cfg->ReadInt(_T("/incremental_search/text_not_found_colour_blue_value"),  0x7f) );
            // if still not found, show it by colouring the textCtrl
            m_pTextCtrl->SetBackgroundColour(colourTextCtrlBG_NotFound);
        }
    }
}

void IncrementalSearch::SearchBackward(int fromPos)
{
    // reset the backgroundcolor of the text-control
    m_pTextCtrl->SetBackgroundColour(m_textCtrlBG_Default);

    // jump to startposition for search
    m_pControl->GotoPos(fromPos);
    m_pControl->SearchAnchor();
    // do the search
    m_NewPos=m_pControl->SearchPrev(m_flags,m_SearchText);
    // verify that we are in allowed range (in case we only search in selection),
    // and define Min- and MaxPos (the selection can have changed since last search)
    VerifyRange();
    if (m_NewPos == wxSCI_INVALID_POSITION)
    {
        ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));
        wxColour colourTextCtrlBG_Wrapped(  cfg->ReadInt(_T("/incremental_search/wrapped_colour_red_value"),   0x7f),
                                            cfg->ReadInt(_T("/incremental_search/wrapped_colour_green_value"), 0x7f),
                                            cfg->ReadInt(_T("/incremental_search/wrapped_colour_blue_value"),  0xff) );
        // if not found or out of range, wrap search
        m_pControl->GotoPos(m_MaxPos);
        m_pControl->SearchAnchor();
        // show that search wrapped, by colouring the textCtrl
        m_pTextCtrl->SetBackgroundColour(colourTextCtrlBG_Wrapped);
        // search again
        m_NewPos=m_pControl->SearchPrev(m_flags,m_SearchText);
        VerifyRange();
        if (m_NewPos == wxSCI_INVALID_POSITION)
        {
            wxColour colourTextCtrlBG_NotFound( cfg->ReadInt(_T("/incremental_search/text_not_found_colour_red_value"),   0xff),
                                                cfg->ReadInt(_T("/incremental_search/text_not_found_colour_green_value"), 0x7f),
                                                cfg->ReadInt(_T("/incremental_search/text_not_found_colour_blue_value"),  0x7f) );
            // if still not found, show it by colouring the textCtrl
            m_pTextCtrl->SetBackgroundColour(colourTextCtrlBG_NotFound);
        }
    }
}

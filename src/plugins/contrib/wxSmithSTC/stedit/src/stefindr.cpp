///////////////////////////////////////////////////////////////////////////////
// Name:        stefindr.cpp
// Purpose:     wxSTEditorFindReplaceData
// Author:      John Labenski, parts taken from wxGuide by Otto Wyss
// Modified by:
// Created:     11/05/2002
// RCS-ID:
// Copyright:   (c) John Labenski, Otto Wyss
// Licence:     wxWidgets licence
///////////////////////////////////////////////////////////////////////////////

#include "precomp.h"

#include <wx/stedit/stefindr.h>
#include <wx/stedit/stedit.h>
#include <wx/stedit/steart.h>
#include "stedlgs_wdr.h"
#include "wxext.h"

//-----------------------------------------------------------------------------
// Static functions for prepending strings to wxArrayString and wxComboBoxes
//-----------------------------------------------------------------------------

void wxSTEPrependArrayString(const wxString &str, wxArrayString &strArray, int count)
{
    const int idx = strArray.Index(str);
    if (idx == 0)
        return;
    if (idx != wxNOT_FOUND)
        strArray.RemoveAt(idx);

    strArray.Insert(str, 0);
    if ((count > 0) && ((int)strArray.GetCount() > count))
        strArray.RemoveAt(count, strArray.GetCount()-count);
}

void wxSTEPrependComboBoxString(const wxString &str, int max_strings, wxComboBox *combo)
{
    wxCHECK_RET(combo, wxT("Invalid combobox in wxSTEPrependComboBoxString"));

    int pos = combo->FindString(str);
    if (pos == 0)
        return;
    if (pos != wxNOT_FOUND)
        combo->Delete(pos);

    combo->Insert(str, 0);
    combo->SetSelection(0);

    if (max_strings > 0)
    {
        while ((int)combo->GetCount() > max_strings)
            combo->Delete(combo->GetCount()-1);
    }
}

void wxSTEInitComboBoxStrings(const wxArrayString& values, wxComboBox* combo)
{
    wxCHECK_RET(combo, wxT("Invalid combobox in wxSTEditorFindReplaceDialog::InitComboBoxStrings"));

    combo->Clear();

    for (size_t n = 0; n < values.GetCount(); n++)
        combo->Append(values[n]);

    if (combo->GetCount() > 0)
        combo->SetSelection(0);
}

//-----------------------------------------------------------------------------
// wxSTEditorFindReplaceData
//-----------------------------------------------------------------------------
int wxSTEditorFindReplaceData::STEToScintillaFlags(int ste_flags)
{
    int sci_flags = 0;
    if (STE_HASBIT(ste_flags, STE_FR_MATCHCASE)) sci_flags |= wxSTC_FIND_MATCHCASE;
    if (STE_HASBIT(ste_flags, STE_FR_WHOLEWORD)) sci_flags |= wxSTC_FIND_WHOLEWORD;
    if (STE_HASBIT(ste_flags, STE_FR_WORDSTART)) sci_flags |= wxSTC_FIND_WORDSTART;
    if (STE_HASBIT(ste_flags, STE_FR_REGEXP   )) sci_flags |= wxSTC_FIND_REGEXP;
    if (STE_HASBIT(ste_flags, STE_FR_POSIX    )) sci_flags |= wxSTC_FIND_POSIX;
    return sci_flags;
}
int wxSTEditorFindReplaceData::ScintillaToSTEFlags(int sci_flags)
{
    int ste_flags = 0;
    if (STE_HASBIT(sci_flags, wxSTC_FIND_MATCHCASE)) ste_flags |= STE_FR_MATCHCASE;
    if (STE_HASBIT(sci_flags, wxSTC_FIND_WHOLEWORD)) ste_flags |= STE_FR_WHOLEWORD;
    if (STE_HASBIT(sci_flags, wxSTC_FIND_WORDSTART)) ste_flags |= STE_FR_WORDSTART;
    if (STE_HASBIT(sci_flags, wxSTC_FIND_REGEXP   )) ste_flags |= STE_FR_REGEXP;
    if (STE_HASBIT(sci_flags, wxSTC_FIND_POSIX    )) ste_flags |= STE_FR_POSIX;
    return ste_flags;
}

bool wxSTEditorFindReplaceData::LoadConfig(wxConfigBase &config,
                                           const wxString &configPath)
{
    m_loaded_config = true; // maybe it failed, but we tried at least once

    wxString key = wxSTEditorOptions::FixConfigPath(configPath, false);
    long val;
    if (config.Read(key + wxT("/FindFlags"), &val))
    {
        SetFlags(int(val));
        return true;
    }
    return false;
}

void wxSTEditorFindReplaceData::SaveConfig(wxConfigBase &config,
                                           const wxString &configPath) const
{
    wxString key = wxSTEditorOptions::FixConfigPath(configPath, false);
    config.Write(key + wxT("/FindFlags"), GetFlags());
}

//-----------------------------------------------------------------------------
// wxSTEditorFindReplacePanel
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxSTEditorFindReplacePanel, wxPanel)

BEGIN_EVENT_TABLE(wxSTEditorFindReplacePanel, wxPanel)
    EVT_TEXT        (ID_STEDLG_FIND_COMBO,    wxSTEditorFindReplacePanel::OnFindComboText)
    EVT_TEXT        (ID_STEDLG_REPLACE_COMBO, wxSTEditorFindReplacePanel::OnFindComboText)
    EVT_CHECKBOX    (wxID_ANY,                wxSTEditorFindReplacePanel::OnCheckBox)
    EVT_RADIOBOX    (wxID_ANY,                wxSTEditorFindReplacePanel::OnCheckBox)
    EVT_RADIOBUTTON (wxID_ANY,                wxSTEditorFindReplacePanel::OnCheckBox)
    EVT_BUTTON      (wxID_ANY,                wxSTEditorFindReplacePanel::OnButton)
    EVT_MENU        (wxID_ANY,                wxSTEditorFindReplacePanel::OnMenu)

    EVT_STC_MARGINCLICK (wxID_ANY, wxSTEditorFindReplacePanel::OnMarginClick)
    EVT_STE_MARGINDCLICK(wxID_ANY, wxSTEditorFindReplacePanel::OnMarginClick)
    EVT_STC_DOUBLECLICK (wxID_ANY, wxSTEditorFindReplacePanel::OnMarginClick)

#ifdef __WXMSW__
    EVT_IDLE        (wxSTEditorFindReplacePanel::OnIdle)
#endif

    //EVT_ACTIVATE (wxSTEditorFindReplacePanel::OnActivate)
END_EVENT_TABLE()

wxSTEditorFindReplacePanel::~wxSTEditorFindReplacePanel()
{
    m_findCombo    = NULL;
    m_replaceCombo = NULL;

    delete m_insertMenu;
}

void wxSTEditorFindReplacePanel::Init()
{
    m_created = false;
    
    m_targetWin = NULL;

    m_flags = 0;
    m_findReplaceData = NULL;

    m_find_insert_pos    = 0;
    m_replace_insert_pos = 0;

    m_findCombo    = NULL;
    m_replaceCombo = NULL;

    m_findCombo          = NULL;
    m_replaceCombo       = NULL;
    m_wholewordCheckBox  = NULL;
    m_matchcaseCheckBox  = NULL;
    m_backwardsCheckBox  = NULL;
    m_wordstartCheckBox  = NULL;
    m_regexpFindCheckBox = NULL;
    m_wraparoundCheckBox = NULL;
    m_findallCheckBox    = NULL;
    m_bookmarkallCheckBox = NULL;

    m_scopewholeRadioButton   = NULL;
    m_scopecursorRadioButton  = NULL;
    m_scopealldocsRadioButton = NULL;

    m_findButton        = NULL;
    m_replaceButton     = NULL;
    m_replaceFindButton = NULL;
    m_replaceAllButton  = NULL;

    m_insertMenu        = NULL;

    m_resultEditor      = NULL;
}

wxSizer *FindSizerSizer(wxSizer *sizer, wxSizer *topSizer)
{
    wxSizerItemList &sizerList = topSizer->GetChildren();

    for (wxSizerItemList::iterator it = sizerList.begin();
         it != sizerList.end();
         it++)
    {
        wxSizerItem *item = *it;

        if (item->IsSizer())
        {
            if (item->GetSizer() == sizer)
                return topSizer;
            else
            {
                wxSizer *foundSizer = FindSizerSizer(sizer, item->GetSizer());
                if (foundSizer)
                    return foundSizer;
            }
        }
    }
    return NULL;
}

wxSizer *FindSizerWindow(wxWindow *win, wxSizer *topSizer)
{
    wxSizerItemList &sizerList = topSizer->GetChildren();

    for (wxSizerItemList::iterator it = sizerList.begin();
         it != sizerList.end();
         it++)
    {
        wxSizerItem *item = *it;

        if (item->IsWindow() && (item->GetWindow() == win))
            return topSizer;
        else if (item->IsSizer())
        {
            wxSizer *foundSizer = FindSizerWindow(win, item->GetSizer());
            if (foundSizer)
                return foundSizer;
        }
    }
    return NULL;
}

bool wxSTEditorFindReplacePanel::Create(wxWindow *parent, wxWindowID winid,
                                        wxSTEditorFindReplaceData *data,
                                        const wxPoint& pos, const wxSize& size,
                                        long style, const wxString& name)
{
    if ( !wxPanel::Create(parent, winid, pos, size, style, name) )
        return false;

    wxSizer* frSizer = wxSTEditorFindReplaceSizer(this, false, false);

    m_findCombo    = wxStaticCast(FindWindow(ID_STEDLG_FIND_COMBO   ), wxComboBox);
    m_replaceCombo = wxStaticCast(FindWindow(ID_STEDLG_REPLACE_COMBO), wxComboBox);

    m_wholewordCheckBox   = wxStaticCast(FindWindow(ID_STEDLG_WHOLEWORD_CHECKBOX  ), wxCheckBox);
    m_matchcaseCheckBox   = wxStaticCast(FindWindow(ID_STEDLG_MATCHCASE_CHECKBOX  ), wxCheckBox);
    m_backwardsCheckBox   = wxStaticCast(FindWindow(ID_STEDLG_BACKWARDS_CHECKBOX  ), wxCheckBox);
    m_wordstartCheckBox   = wxStaticCast(FindWindow(ID_STEDLG_WORDSTART_CHECKBOX  ), wxCheckBox);
    m_regexpFindCheckBox  = wxStaticCast(FindWindow(ID_STEDLG_REGEXP_FIND_CHECKBOX), wxCheckBox);
    m_wraparoundCheckBox  = wxStaticCast(FindWindow(ID_STEDLG_WRAP_CHECKBOX       ), wxCheckBox);
    m_findallCheckBox     = wxStaticCast(FindWindow(ID_STEDLG_FINDALL_CHECKBOX    ), wxCheckBox);
    m_bookmarkallCheckBox = wxStaticCast(FindWindow(ID_STEDLG_BOOKMARKALL_CHECKBOX), wxCheckBox);

    m_scopewholeRadioButton   = wxStaticCast(FindWindow(ID_STEDLG_SCOPEWHOLE_RADIOBUTTON  ), wxRadioButton);
    m_scopecursorRadioButton  = wxStaticCast(FindWindow(ID_STEDLG_SCOPECURSOR_RADIOBUTTON ), wxRadioButton);
    m_scopealldocsRadioButton = wxStaticCast(FindWindow(ID_STEDLG_SCOPEALLDOCS_RADIOBUTTON), wxRadioButton);

    m_findButton        = wxStaticCast(FindWindow(ID_STEDLG_FIND_BUTTON       ), wxButton);
    m_replaceButton     = wxStaticCast(FindWindow(ID_STEDLG_REPLACE_BUTTON    ), wxButton);
    m_replaceFindButton = wxStaticCast(FindWindow(ID_STEDLG_REPLACEFIND_BUTTON), wxButton);
    m_replaceAllButton  = wxStaticCast(FindWindow(ID_STEDLG_REPLACEALL_BUTTON ), wxButton);

    m_insertMenu = wxSTEditorMenuManager::CreateInsertCharsMenu(NULL,
        STE_MENU_INSERTCHARS_CHARS|STE_MENU_INSERTCHARS_REGEXP);

    if (!data)
    {
        Enable(false);
        return false;
    }

    // Set the data and update the button state based on it's values
    SetData(data);

    if (HasFlag(STE_FR_NOUPDOWN))
    {
        m_backwardsCheckBox->SetValue(false);
        FindSizerWindow(m_backwardsCheckBox, frSizer)->Show(m_backwardsCheckBox, false);
    }

    if (HasFlag(STE_FR_NOMATCHCASE))
    {
        m_matchcaseCheckBox->SetValue(true);
        FindSizerWindow(m_matchcaseCheckBox, frSizer)->Show(m_matchcaseCheckBox, false);
    }

    if (HasFlag(STE_FR_NOWHOLEWORD))
    {
        m_wholewordCheckBox->SetValue(false);
        FindSizerWindow(m_wholewordCheckBox, frSizer)->Show(m_wholewordCheckBox, false);
    }

    if (HasFlag(STE_FR_NOWORDSTART))
    {
        m_wordstartCheckBox->SetValue(false);
        FindSizerWindow(m_wordstartCheckBox, frSizer)->Show(m_wordstartCheckBox, false);
    }

    if (HasFlag(STE_FR_NOWRAPAROUND))
    {
        m_wraparoundCheckBox->SetValue(false);
        FindSizerWindow(m_wraparoundCheckBox, frSizer)->Show(m_wraparoundCheckBox, false);
    }

    if (HasFlag(STE_FR_NOREGEXP))
    {
        m_regexpFindCheckBox->SetValue(false);
        FindSizerWindow(m_regexpFindCheckBox, frSizer)->Show(m_regexpFindCheckBox, false);
    }

    if (HasFlag(STE_FR_NOALLDOCS))
    {
        m_scopealldocsRadioButton->Show(false);
        // you can't find in all docs, remove that flag, set find from cursor
        if ( m_findReplaceData->HasFlag(STE_FR_ALLDOCS) ||
            (!m_findReplaceData->HasFlag(STE_FR_WHOLEDOC) &&
             !m_findReplaceData->HasFlag(STE_FR_FROMCURSOR)) )
            m_findReplaceData->SetFlags((m_findReplaceData->GetFlags() & ~STE_FR_SEARCH_MASK) | STE_FR_FROMCURSOR);
    }

    if (!HasFlag(wxFR_REPLACEDIALOG))
    {
        wxSizer *sizer = FindSizerWindow(m_replaceCombo, frSizer);
        if (sizer)
        {
            sizer->Show(FindWindow(ID_STEDLG_REPLACE_TEXT), false);
            sizer->Show(m_replaceCombo, false);
            sizer->Show(FindWindow(ID_STEDLG_REPLACE_BITMAPBUTTON), false);
        }

        wxSizer *replaceSizer = FindSizerWindow(m_replaceButton, frSizer);
        sizer = FindSizerSizer(replaceSizer, frSizer);
        if (sizer)
            sizer->Hide(replaceSizer);
    }

    wxFlexGridSizer *rootSizer = new wxFlexGridSizer( 1, 0, 0 );
    rootSizer->AddGrowableCol( 0 );
    rootSizer->AddGrowableRow( 1 );
    rootSizer->Add(frSizer, 0, wxGROW, 0);

    m_resultEditor = new wxSTEditor(this, wxID_ANY); // create before SetData
    m_resultEditor->Show(false);
    m_resultEditor->SetStyleBits(5); // want to show indicators
    m_resultEditor->SetMarginType(STE_MARGIN_NUMBER, wxSTC_MARGIN_NUMBER);
    m_resultEditor->SetMarginWidth(STE_MARGIN_NUMBER, m_resultEditor->TextWidth(wxSTC_STYLE_LINENUMBER, wxT("_9999")));
    m_resultEditor->SetMarginSensitive(STE_MARGIN_NUMBER, true); // don't select line
    m_resultEditor->SetMarginSensitive(STE_MARGIN_MARKER, true); // don't select line
    m_resultEditor->SetMarginWidth(STE_MARGIN_MARKER, 16);
    m_resultEditor->SetMarginWidth(STE_MARGIN_FOLD, 0);
    m_resultEditor->SetReadOnly(true);
    m_resultEditor->RegisterStyles(wxSTEditorStyles::GetGlobalEditorStyles());
    m_resultEditor->RegisterLangs(wxSTEditorLangs::GetGlobalEditorLangs());

    rootSizer->Add(m_resultEditor, 1, wxGROW, 0);
    //rootSizer->Show(m_resultEditor, m_findReplaceData->HasFlag(STE_FR_FINDALL));

    SetSizer(rootSizer);
    rootSizer->Layout();
    Layout();
    rootSizer->SetSizeHints( this );
    //rootSizer->Fit( this );

    m_created = true;

    UpdateFindFlags();
    UpdateButtons();
    m_findCombo->SetFocus();

    return true;
}

void wxSTEditorFindReplacePanel::SetData(wxSTEditorFindReplaceData *data)
{
    wxCHECK_RET(data, wxT("Invalid find replace data in wxSTEditorFindReplaceDialog::SetData"));
    m_findReplaceData = data;

    // setup the find/replace comboboxes
    wxSTEInitComboBoxStrings(*m_findReplaceData->GetFindStrings(), m_findCombo);
    wxSTEInitComboBoxStrings(*m_findReplaceData->GetReplaceStrings(), m_replaceCombo);

    // setup the options checkboxes
    int flags = m_findReplaceData->GetFlags();

    m_wholewordCheckBox->SetValue(STE_HASBIT(flags, wxFR_WHOLEWORD));
    m_matchcaseCheckBox->SetValue(STE_HASBIT(flags, wxFR_MATCHCASE));
    m_backwardsCheckBox->SetValue(!STE_HASBIT(flags, wxFR_DOWN));

    m_wordstartCheckBox->SetValue(STE_HASBIT(flags, STE_FR_WORDSTART));
    m_regexpFindCheckBox->SetValue(STE_HASBIT(flags, STE_FR_REGEXP));
    m_wraparoundCheckBox->SetValue(STE_HASBIT(flags, STE_FR_WRAPAROUND));

    m_findallCheckBox->SetValue(STE_HASBIT(flags, STE_FR_FINDALL));
    m_bookmarkallCheckBox->SetValue(STE_HASBIT(flags, STE_FR_BOOKMARKALL));

    // setup the scope radio buttons
    if (STE_HASBIT(flags, STE_FR_FROMCURSOR))
        m_scopecursorRadioButton->SetValue(true);
    else if (STE_HASBIT(flags, STE_FR_ALLDOCS))
        m_scopealldocsRadioButton->SetValue(true);
    else
        m_scopewholeRadioButton->SetValue(true);
}

wxWindow* wxSTEditorFindReplacePanel::GetTargetWindow() const
{
    return m_targetWin ? m_targetWin : GetParent();
}

wxSTEditor* wxSTEditorFindReplacePanel::GetEditor() const
{
    wxWindow* targetWindow = GetTargetWindow();
    wxSTEditor* edit = NULL;

    if (targetWindow)
    {
        if (wxDynamicCast(targetWindow, wxSTEditorNotebook))
            edit = wxDynamicCast(targetWindow, wxSTEditorNotebook)->GetEditor();
        else if (wxDynamicCast(targetWindow, wxSTEditorSplitter))
            edit = wxDynamicCast(targetWindow, wxSTEditorSplitter)->GetEditor();
        else if (wxDynamicCast(targetWindow, wxSTEditor))
            edit = wxDynamicCast(targetWindow, wxSTEditor);
    }

    return edit;
}

void wxSTEditorFindReplacePanel::SendEvent(const wxEventType& evtType)
{
    wxFindDialogEvent event(evtType, GetId());
    event.SetEventObject(this);
    event.SetFindString(m_findCombo->GetValue());
    event.SetFlags(GetFindFlags());
    event.SetExtraLong(-1);

    if (evtType != wxEVT_COMMAND_FIND_CLOSE)
        wxSTEPrependComboBoxString(m_findCombo->GetValue(), m_findReplaceData->GetMaxStrings(), m_findCombo);

    if ( HasFlag(wxFR_REPLACEDIALOG) )
    {
        wxSTEPrependComboBoxString(m_replaceCombo->GetValue(), m_findReplaceData->GetMaxStrings(), m_replaceCombo);
        event.SetReplaceString(m_replaceCombo->GetValue());
    }

    Send(event);
}

void wxSTEditorFindReplacePanel::Send(wxFindDialogEvent& event)
{
    // we copy the data to dialog->GetData() as well
    m_findReplaceData->SetFlags(event.GetFlags());

    m_findReplaceData->SetFindString(event.GetFindString());
    if (event.GetFindString().Length())
        m_findReplaceData->AddFindString(event.GetFindString());

    if ( HasFlag(wxFR_REPLACEDIALOG) &&
         (event.GetEventType() == wxEVT_COMMAND_FIND_REPLACE ||
          event.GetEventType() == wxEVT_COMMAND_FIND_REPLACE_ALL) )
    {
        m_findReplaceData->SetReplaceString(event.GetReplaceString());
        m_findReplaceData->AddReplaceString(event.GetReplaceString());
    }

    // translate wxEVT_COMMAND_FIND_NEXT to wxEVT_COMMAND_FIND if needed
    if ( event.GetEventType() == wxEVT_COMMAND_FIND_NEXT )
    {
        if ( m_findReplaceData->GetFindString() != m_lastSearch )
        {
            event.SetEventType(wxEVT_COMMAND_FIND);
            m_lastSearch = m_findReplaceData->GetFindString();
        }
    }

    // ExtraLong is the line number pressed in the find all editor
    //  when -1 it means that we want a new find all search
    if (m_findReplaceData->HasFlag(STE_FR_FINDALL) && m_resultEditor &&
        (event.GetExtraLong() == -1) &&
        ((event.GetEventType() == wxEVT_COMMAND_FIND) ||
         (event.GetEventType() == wxEVT_COMMAND_FIND_NEXT)))
    {
        m_findReplaceData->GetFindAllStrings()->Clear();
        m_resultEditor->SetReadOnly(false);
        m_resultEditor->SetText(wxEmptyString);
        m_resultEditor->SetReadOnly(true);
    }

    wxWindow *target = GetTargetWindow();

    // first send event to ourselves then to the target
    if ( !GetEventHandler()->ProcessEvent(event) && target )
    {
        // the event is not propagated upwards to the parent automatically
        // because the dialog is a top level window, so do it manually as
        // in 9 cases of 10 the message must be processed by the dialog
        // owner and not the dialog itself
        (void)target->GetEventHandler()->ProcessEvent(event);
    }

    if (m_findReplaceData->HasFlag(STE_FR_FINDALL) && m_resultEditor &&
        (event.GetExtraLong() == -1) &&
        ((event.GetEventType() == wxEVT_COMMAND_FIND) ||
         (event.GetEventType() == wxEVT_COMMAND_FIND_NEXT)))
    {
        wxSTEditor* edit = GetEditor();
        if (edit)
        {
            m_resultEditor->SetLanguage(edit->GetLanguageId());
        }

        wxArrayString* findAllStrings = m_findReplaceData->GetFindAllStrings();
        size_t n, count = findAllStrings->GetCount();
        wxString str;
        for (n = 0; n < count; n++)
            str += findAllStrings->Item(n).AfterFirst(wxT('|'));

        m_resultEditor->Clear();
        m_resultEditor->ClearAllIndicators();

        m_resultEditor->SetReadOnly(false);
        m_resultEditor->SetText(str);
        m_resultEditor->SetReadOnly(true);
        m_resultEditor->Colourise(0, -1);

        wxSTEditorStyles::GetGlobalEditorStyles().SetEditorStyle( 3, STE_STYLE_STRING,
                                                                m_resultEditor, false);
        wxSTEditorStyles::GetGlobalEditorStyles().SetEditorStyle( 4, STE_STYLE_NUMBER,
                                                                m_resultEditor, false);

        for (n = 0; n < count; n++)
        {
            str = findAllStrings->Item(n).AfterFirst(wxT('|'));
            int pos = m_resultEditor->PositionFromLine((int)n);
            m_resultEditor->StartStyling(pos, 31);
            int length = (int)str.BeforeFirst(wxT('(')).Length() - 1;
            m_resultEditor->SetStyling(length, 3);

            pos = pos + length + 1;
            m_resultEditor->StartStyling(pos, 31);
            length = (int)str.AfterFirst(wxT('(')).BeforeFirst(wxT(')')).Length() + 2;
            m_resultEditor->SetStyling(length, 4);
        }

        m_resultEditor->IndicateAllStrings(m_findReplaceData->GetFindString(),
                                           m_findReplaceData->GetFlags(),
                                           wxSTC_INDIC0_MASK);
    }

    UpdateButtons();
}

void wxSTEditorFindReplacePanel::OnButton(wxCommandEvent& event)
{
    switch (event.GetId())
    {
        case ID_STEDLG_FIND_BITMAPBUTTON  :
        {
            // set the clientdata of the menu to the combo it's for, see OnMenu
            wxRect r = ((wxWindow*)event.GetEventObject())->GetRect();
            m_insertMenu->SetClientData((void*)m_findCombo);
            m_insertMenu->Enable(ID_STEDLG_MENU_INSERTMENURE, m_regexpFindCheckBox->IsChecked());
            PopupMenu(m_insertMenu, r.GetRight(), r.GetTop());
            break;
        }
        case ID_STEDLG_REPLACE_BITMAPBUTTON  :
        {
            wxRect r = ((wxWindow*)event.GetEventObject())->GetRect();
            m_insertMenu->SetClientData((void*)m_replaceCombo);
            m_insertMenu->Enable(ID_STEDLG_MENU_INSERTMENURE, m_regexpFindCheckBox->IsChecked());
            PopupMenu(m_insertMenu, r.GetRight(), r.GetTop());
            break;
        }
        case ID_STEDLG_FIND_BUTTON        : SendEvent(wxEVT_COMMAND_FIND_NEXT); break;
        case ID_STEDLG_REPLACE_BUTTON     : SendEvent(wxEVT_COMMAND_FIND_REPLACE); break;
        case ID_STEDLG_REPLACEFIND_BUTTON : SendEvent(wxEVT_COMMAND_FIND_REPLACE);
                                            SendEvent(wxEVT_COMMAND_FIND_NEXT); break;
        case ID_STEDLG_REPLACEALL_BUTTON  : SendEvent(wxEVT_COMMAND_FIND_REPLACE_ALL); break;
        case wxID_CANCEL                  : SendEvent(wxEVT_COMMAND_FIND_CLOSE); event.Skip(); break;
        default : break;
    }
}

void wxSTEditorFindReplacePanel::OnMenu(wxCommandEvent& event)
{
    wxString c;
    int ipos = 0;

    switch (event.GetId())
    {
        case ID_STEDLG_INSERTMENU_TAB : c = wxT("\t"); break;
        case ID_STEDLG_INSERTMENU_CR  : c = wxT("\r"); break;
        case ID_STEDLG_INSERTMENU_LF  : c = wxT("\n"); break;

        case ID_STEDLG_INSERTMENURE_ANYCHAR   : c = wxT("."); break;
        case ID_STEDLG_INSERTMENURE_RANGE     : c = wxT("[]");  ipos = -1; break;
        case ID_STEDLG_INSERTMENURE_NOTRANGE  : c = wxT("[^]"); ipos = -1; break;
        case ID_STEDLG_INSERTMENURE_BEGINLINE : c = wxT("^"); break;
        case ID_STEDLG_INSERTMENURE_ENDLINE   : c = wxT("$"); break;
        case ID_STEDLG_INSERTMENURE_TAGEXPR   :
        {
            if (!STE_HASBIT(GetFindFlags(), STE_FR_POSIX))
                { c = wxT("\\(\\)");  ipos = -2; }
            else
                { c = wxT("()");  ipos = -1; }
            break;
        }
        case ID_STEDLG_INSERTMENURE_0MATCHES  : c = wxT("*"); break;
        case ID_STEDLG_INSERTMENURE_1MATCHES  : c = wxT("+"); break;
        case ID_STEDLG_INSERTMENURE_01MATCHES : c = wxT("?"); break;

        case ID_STEDLG_INSERTMENURE_ALPHANUM  : c = wxT("[a-zA-Z][0-9]"); break;
        case ID_STEDLG_INSERTMENURE_ALPHA     : c = wxT("[a-zA-Z]"); break;
        case ID_STEDLG_INSERTMENURE_NUMERIC   : c = wxT("[0-9]"); break;
        case ID_STEDLG_INSERTMENURE_TAB       : c = wxT("\\t"); break;
        default : break;
    }

    if (c.Length()) // this must have been for the m_insertMenu
    {
        wxComboBox* cBox = wxStaticCast(m_insertMenu->GetClientData(), wxComboBox);
        wxCHECK_RET(cBox, wxT("Unexpected missing control"));
        int pos = (cBox == m_findCombo) ? m_find_insert_pos : m_replace_insert_pos;

        wxString s = cBox->GetValue();

        if (pos >= int(s.Length()))
            s += c;
        else if (pos == 0)
            s = c + s;
        else
            s = s.Mid(0, pos) + c + s.Mid(pos);

        cBox->SetValue(s);
        cBox->SetFocus();
        cBox->SetInsertionPoint(pos + (int)c.Length() + ipos);
    }
}

void wxSTEditorFindReplacePanel::OnActivate(wxActivateEvent &event)
{
    event.Skip();

    if (event.GetActive())
    {
        SelectFindString();
        UpdateButtons();
    }
}

void wxSTEditorFindReplacePanel::OnIdle(wxIdleEvent &event)
{
    if (IsShown())
    {
        // This is a really ugly hack because the combo forgets it's insertion
        //   point in MSW whenever it loses focus
        wxWindow* focus = FindFocus();
        if (m_findCombo && (focus == m_findCombo))
            m_find_insert_pos = m_findCombo->GetInsertionPoint();
        if (m_replaceCombo && (focus == m_replaceCombo))
            m_replace_insert_pos = m_replaceCombo->GetInsertionPoint();
    }

    event.Skip();
}

void wxSTEditorFindReplacePanel::UpdateFindFlags()
{
    m_flags = 0;

    if (m_matchcaseCheckBox->GetValue())   m_flags |= wxFR_MATCHCASE;
    if (m_wholewordCheckBox->GetValue())   m_flags |= wxFR_WHOLEWORD;
    if (!m_backwardsCheckBox->GetValue())  m_flags |= wxFR_DOWN;

    if (m_wordstartCheckBox->GetValue())   m_flags |= STE_FR_WORDSTART;
    if (m_regexpFindCheckBox->GetValue())  m_flags |= STE_FR_REGEXP;
    if (m_wraparoundCheckBox->GetValue())  m_flags |= STE_FR_WRAPAROUND;

    if (m_findallCheckBox->GetValue())     m_flags |= STE_FR_FINDALL;
    if (m_bookmarkallCheckBox->GetValue()) m_flags |= STE_FR_BOOKMARKALL;

    if (m_scopewholeRadioButton->GetValue())        m_flags |= STE_FR_WHOLEDOC;
    else if (m_scopecursorRadioButton->GetValue())  m_flags |= STE_FR_FROMCURSOR;
    else if (m_scopealldocsRadioButton->GetValue()) m_flags |= STE_FR_ALLDOCS;

    if (m_resultEditor->IsShown() != STE_HASBIT(m_flags, STE_FR_FINDALL))
    {
        InvalidateBestSize();
        SetMinSize(wxSize(10, 10));
        GetSizer()->SetMinSize(wxSize(10, 10));
        GetSizer()->Show(m_resultEditor, STE_HASBIT(m_flags, STE_FR_FINDALL));
        GetSizer()->Layout();
        GetSizer()->SetSizeHints(this);
    }
}

void wxSTEditorFindReplacePanel::SelectFindString()
{
    wxString value = m_findCombo->GetValue();
    if (value.Len() > 0u)
        m_findCombo->SetSelection(0, (int)value.Len());
}

void wxSTEditorFindReplacePanel::OnFindComboText(wxCommandEvent& WXUNUSED(event))
{
    UpdateButtons();
}

void wxSTEditorFindReplacePanel::OnCheckBox(wxCommandEvent &event)
{
    UpdateFindFlags();
    UpdateButtons();
    event.Skip();
}

// FIXME - This is a hack for a bug in GTK (not wxWidgets) where if you enable
// a button you cannot click on it without capturing and releasing the mouse.
void wxSTE_WIN_ENABLE(wxWindow* win, bool enable)
{
    if (win && (win->IsEnabled() != enable))
    {
        win->Enable(enable);

#ifdef __WXGTK__
        if (enable && win->IsShown())
        {
            if (!win->HasCapture())
                win->CaptureMouse();
            if (win->HasCapture())
                win->ReleaseMouse();
        }
#endif // __WXGTK__
    }
}

void wxSTEditorFindReplacePanel::UpdateButtons()
{
    if (!m_created) return; // skip initial events sent from combobox in GTK

    // Can't search backwards when using regexp
    if (m_regexpFindCheckBox->GetValue() && m_backwardsCheckBox->IsEnabled())
    {
        m_backwardsCheckBox->SetValue(false);
        m_backwardsCheckBox->Enable(false);
    }
    else if (!m_regexpFindCheckBox->GetValue() && !m_backwardsCheckBox->IsEnabled())
    {
        m_backwardsCheckBox->Enable(true);
    }

    // update the find/replace button state
    const wxString findStr = m_findCombo->GetValue();
    bool enable = findStr.Length() > 0u;

    wxSTEditor *edit = GetEditor();

    int flags = GetFindFlags();

    if (enable)
    {
        bool changed = edit ? ((edit->GetFindString() != findStr)||(edit->GetFindFlags() != flags)) : true;
        enable &= ((edit && edit->CanFind()) ? true : changed);
    }

    wxSTE_WIN_ENABLE(m_findButton, enable);

    if (HasFlag(wxFR_REPLACEDIALOG))
    {
        // Don't want recursive find
        if (m_findReplaceData->StringCmp(findStr, m_replaceCombo->GetValue(), flags))
            enable = false;

        wxSTE_WIN_ENABLE(m_replaceAllButton, enable);

        wxString selText = edit ? edit->GetSelectedText() : wxString(wxEmptyString);

        // can only replace if already selecting the "find" text
        if (enable && edit && !edit->SelectionIsFindString(findStr, flags))
            enable = false;
        else if (!m_regexpFindCheckBox->IsChecked() && !m_findReplaceData->StringCmp(findStr, selText, flags))
            enable = false;

        wxSTE_WIN_ENABLE(m_replaceButton, enable);
        wxSTE_WIN_ENABLE(m_replaceFindButton, enable);
    }
}

void wxSTEditorFindReplacePanel::OnMarginClick( wxStyledTextEvent &event )
{
    if (!m_resultEditor) return; // set after editor is fully created

    if (event.GetEventType() == wxEVT_STE_MARGINDCLICK)
        return;

    wxSTEditor *editor = wxStaticCast(event.GetEventObject(), wxSTEditor);
    int pos = event.GetPosition();

    if (event.GetEventType() == wxEVT_STC_DOUBLECLICK) // event pos not set correctly
        pos = editor->GetCurrentPos();

    int line = editor->LineFromPosition(pos);

    if (editor->GetLine(line).Strip(wxString::both).IsEmpty())
        return;

    wxArrayString* findAllStrings = m_findReplaceData->GetFindAllStrings();

    if ((line < 0) || (line >= (int)findAllStrings->GetCount()))
        return;

    editor->MarkerDeleteAll(STE_MARKER_BOOKMARK);
    editor->MarkerAdd(line, STE_MARKER_BOOKMARK);

    wxFindDialogEvent fEvent(wxEVT_COMMAND_FIND_NEXT, GetId());
    fEvent.SetEventObject(this);
    fEvent.SetFindString(m_findCombo->GetValue());
    fEvent.SetFlags(GetFindFlags());
    fEvent.SetExtraLong(line);
    Send(fEvent);
}

//-----------------------------------------------------------------------------
// wxSTEditorFindReplaceDialog
//-----------------------------------------------------------------------------
const wxString wxSTEditorFindReplaceDialogNameStr = wxT("wxSTEditorFindReplaceDialogNameStr");

IMPLEMENT_DYNAMIC_CLASS(wxSTEditorFindReplaceDialog, wxDialog)

BEGIN_EVENT_TABLE(wxSTEditorFindReplaceDialog, wxDialog)
    EVT_BUTTON   (wxID_ANY, wxSTEditorFindReplaceDialog::OnButton)
    EVT_CHECKBOX (wxID_ANY, wxSTEditorFindReplaceDialog::OnButton)

    EVT_SIZE     (wxSTEditorFindReplaceDialog::OnSize)
    EVT_ACTIVATE (wxSTEditorFindReplaceDialog::OnActivate)
    EVT_CLOSE    (wxSTEditorFindReplaceDialog::OnCloseWindow)
END_EVENT_TABLE()

wxSTEditorFindReplaceDialog::~wxSTEditorFindReplaceDialog() {}

void wxSTEditorFindReplaceDialog::Init()
{
    m_findReplacePanel = NULL;
}

bool wxSTEditorFindReplaceDialog::Create(wxWindow *parent,
                                         wxSTEditorFindReplaceData *data,
                                         const wxString& title,
                                         int style, const wxString &name)
{
    if (!wxDialog::Create(parent, ID_STE_FINDREPLACE_DIALOG, title,
                           wxDefaultPosition, wxDefaultSize,
                           wxDEFAULT_DIALOG_STYLE_RESIZE | wxFRAME_FLOAT_ON_PARENT | style,
                           name))
                           //wxDEFAULT_FRAME_STYLE | wxRESIZE_BORDER | wxFRAME_FLOAT_ON_PARENT | style,
    {
        return false;
    }

    m_findReplacePanel = new wxSTEditorFindReplacePanel(this, wxID_ANY, data, wxDefaultPosition,
                                   wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER | style);
    m_findReplacePanel->SetTargetWindow(parent); // assume this, they can override later

    // use sizer since child file replace panel will use it to resize us
    //wxFlexGridSizer* rootSizer = new wxFlexGridSizer(1, 0, 0);
    //rootSizer->AddGrowableCol( 0 );
    //rootSizer->AddGrowableRow( 0 );
    wxBoxSizer* rootSizer = new wxBoxSizer(wxVERTICAL);
    rootSizer->Add(m_findReplacePanel, 1, wxGROW);
    SetSizer(rootSizer);
    rootSizer->SetSizeHints(this);

    // set the last user set size, but only if it's bigger than the size we
    // are already
    wxSize dialogSize = data ? data->GetDialogSize() : wxDefaultSize;
    wxSize size = GetSize();

    if (m_findReplacePanel->m_resultEditor && m_findReplacePanel->m_resultEditor->IsShown() &&
        (dialogSize != wxDefaultSize) &&
        ((dialogSize.x > size.x) || (dialogSize.y > size.y)))
    {
        SetSize(wxMax(dialogSize.x, size.x), wxMax(dialogSize.y, size.y));
    }
    Centre();
    SetIcon(wxSTEditorArtProvider::GetIcon((style & wxFR_REPLACEDIALOG) ? wxART_STEDIT_REPLACE : wxART_STEDIT_FIND, wxDialogIconSize));
    return true;
}

void wxSTEditorFindReplaceDialog::OnCloseWindow(wxCloseEvent &event)
{
    if (m_findReplacePanel)
        m_findReplacePanel->SendEvent(wxEVT_COMMAND_FIND_CLOSE);

    event.Skip();
}

void wxSTEditorFindReplaceDialog::OnActivate(wxActivateEvent &event)
{
    event.Skip();

    if (event.GetActive() && m_findReplacePanel)
        m_findReplacePanel->OnActivate(event);
}

void wxSTEditorFindReplaceDialog::OnButton(wxCommandEvent& event)
{
    switch (event.GetId())
    {
        case ID_STEDLG_FINDALL_CHECKBOX :
        {
            // wxWidgets needs help resizing the shown/hidden results editor
            //   This ugly hack works in any case
            //wxSize s = GetSize();
            //wxSize minSize = m_findReplacePanel->GetSize();
            //wxPrintf(wxT("DLG %d %d    %d %d\n"), s.GetWidth(), s.GetHeight(), minSize.GetWidth(), minSize.GetHeight());

            InvalidateBestSize();
            SetMinSize(wxSize(10,10));
            GetSizer()->SetMinSize(wxSize(10,10));
            m_findReplacePanel->GetSizer()->SetSizeHints(this);

            break;
        }
        case wxID_CANCEL : Destroy();
        default : event.Skip();
    }
}

void wxSTEditorFindReplaceDialog::OnSize(wxSizeEvent &event)
{
/*
    if (GetSize() != m_findReplacePanel->GetSizer()->CalcMin())
    {
        InvalidateBestSize();
        SetMinSize(wxSize(10,10));
        GetSizer()->SetMinSize(wxSize(10,10));
        m_findReplacePanel->GetSizer()->SetMinSize(wxSize(10,10));
        m_findReplacePanel->GetSizer()->SetSizeHints(m_findReplacePanel);
        GetSizer()->SetSizeHints(this);
        //SetClientSize(GetSizer()->CalcMin());
    }
    else
*/

    // remember the size of the find dialog for find all
    if (m_findReplacePanel && m_findReplacePanel->GetData() &&
        m_findReplacePanel->m_resultEditor && m_findReplacePanel->m_resultEditor->IsShown())
    {
        m_findReplacePanel->GetData()->SetDialogSize(GetSize());
    }


        event.Skip();

    //wxPrintf(wxT("wxSTEditorFindReplaceDialog::OnSize %d %d   %d %d\n"), GetSize().x, GetSize().y, event.GetSize().x, event.GetSize().y);
}

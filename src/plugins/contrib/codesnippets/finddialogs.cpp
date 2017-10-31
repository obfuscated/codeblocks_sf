//////////////////////////////////////////////////////////////////////////////
// File:        dialogs.cpp
// Purpose:     Find/Replace, Goto dialogs
// Maintainer:  Otto Wyss
// Created:     2003-01-20
// RCS-ID:      $Id$
// Copyright:   (c) 2005 wyoGuide
// Licence:     wxWindows
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// information
//----------------------------------------------------------------------------
/*
	This file is part of Code Snippets, a plugin for Code::Blocks
	Copyright (C) 2006 Arto Jonsson
	Copyright (C) 2007 Pecan Heber

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
// RCS-ID: $Id$


//----------------------------------------------------------------------------
// headers
//----------------------------------------------------------------------------

// For compilers that support precompilation, includes <wx/wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all 'standard' wxWidgets headers)
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

// wxWidgets headers
#include <wx/accel.h>    // accelerator support
#include <wx/config.h>
//#include <wx/confbase.h>
//#include <wx/fileconf.h>
//#include <wx/msw/regconf.h> (wxRegConfig class)


#include "defsext.h"     // Additional definitions
#include "finddialogs.h"     // Dialogs

#define g_appname wxT("Find")
//----------------------------------------------------------------------------
// resources
//----------------------------------------------------------------------------


//============================================================================
// declarations
//============================================================================

const int MAXHISTORY = 10;
const wxString FINDREPLACEDLG = _T("FindReplaceDlg");
const wxString DIRECTORYHISTORY = _T("History/Directory");
const wxString FINDSTRHISTORY = _T("History/Findtext");
const wxString REPLACESTRHISTORY = _T("History/Replacetext");


//============================================================================
// implementation
//============================================================================

//----------------------------------------------------------------------------
// myFindReplaceDlg
//----------------------------------------------------------------------------

// static variables
static wxArrayString m_finddirHist;
static wxArrayString m_findstrHist;
static wxArrayString m_replacestrHist;

BEGIN_EVENT_TABLE (myFindReplaceDlg, wxScrollingDialog)
    EVT_BUTTON (wxID_CANCEL,     myFindReplaceDlg::OnCancel)
    EVT_BUTTON (wxID_OK,         myFindReplaceDlg::OnOkay)
    EVT_BUTTON (myID_REPLACE,    myFindReplaceDlg::OnReplace)
    EVT_BUTTON (myID_REPLACEALL, myFindReplaceDlg::OnReplaceAll)
    EVT_MENU   (wxID_HELP,       myFindReplaceDlg::OnHelp)
END_EVENT_TABLE()

myFindReplaceDlg::myFindReplaceDlg (wxWindow *parent,
                                    const wxString &findstr,
                                    const wxString &replacestr,
                                    wxUint32 flags,
                                    long style)
               : wxScrollingDialog (parent, -1, _("Dialog"),
                           wxDefaultPosition, wxDefaultSize,
                           style | wxDEFAULT_DIALOG_STYLE) {

    m_style = 0;

    //accelerators (for help)
    const int nEntries = 1 ;
    wxAcceleratorEntry entries[nEntries];
    entries[0].Set (wxACCEL_NORMAL, WXK_F1, wxID_HELP);
    wxAcceleratorTable accel (nEntries, entries);
    SetAcceleratorTable (accel);

    // layout the dialog
    m_findpane = new wxBoxSizer (wxVERTICAL);

    // find, replace text and options, direction
    wxBoxSizer *findsizer = new wxBoxSizer (wxHORIZONTAL);
    findsizer->Add (new wxStaticText (this, -1, _("Search for:"),
                                      wxDefaultPosition, wxSize(80, -1)),
                    0, wxALIGN_CENTRE_VERTICAL | wxRIGHT, 6);
    m_findstr = new wxComboBox (this, myID_DLG_FIND_TEXT, findstr,
                                 wxDefaultPosition, wxSize(200, -1));
    findsizer->Add (m_findstr, 1, wxALIGN_CENTRE_VERTICAL);
    m_findpane->Add (findsizer, 0, wxEXPAND | wxBOTTOM, 6);

    m_fdirsizer = new wxBoxSizer (wxHORIZONTAL);
    m_fdirsizer->Add (new wxStaticText (this, -1, _("In directories:"),
                                        wxDefaultPosition, wxSize(80, -1)),
                      0, wxALIGN_CENTRE_VERTICAL | wxRIGHT, 6);
    wxString finddir;
    m_finddir = new wxComboBox (this, -1, finddir,
                                wxDefaultPosition, wxSize(200, -1),
                                //0, NULL); //AMD64 ambiguity betw int vs wxArray
                                (int)0, (const wxString*) NULL);
    m_fdirsizer->Add (m_finddir, 1, wxALIGN_CENTRE_VERTICAL);
    m_findpane->Show (m_fdirsizer, false);
    m_findpane->Add (m_fdirsizer, 0, wxEXPAND | wxBOTTOM, 6);

    m_specsizer = new wxBoxSizer (wxHORIZONTAL);
    m_specsizer->Add (new wxStaticText (this, -1, _("With filespec:"),
                                        wxDefaultPosition, wxSize(80, -1)),
                      0, wxALIGN_CENTRE_VERTICAL | wxRIGHT, 6);
    m_findspec = new wxTextCtrl (this, -1, wxEmptyString,
                                 wxDefaultPosition, wxSize(200, -1));
    m_specsizer->Add (m_findspec, 1, wxALIGN_CENTRE_VERTICAL);
    m_findpane->Show (m_specsizer, false);
    m_findpane->Add (m_specsizer, 0, wxEXPAND | wxBOTTOM, 6);

    m_replsizer = new wxBoxSizer (wxHORIZONTAL);
    m_replsizer->Add (new wxStaticText (this, -1, _("Replace with:"),
                                        wxDefaultPosition, wxSize(80, -1)),
                      0, wxALIGN_CENTRE_VERTICAL | wxRIGHT, 6);
    m_replacestr = new wxComboBox (this, -1, replacestr,
                                    wxDefaultPosition, wxSize(200, -1));
    m_replsizer->Add (m_replacestr, 1, wxALIGN_CENTRE_VERTICAL);
    m_findpane->Show (m_replsizer, false);
    m_findpane->Add (m_replsizer, 0, wxEXPAND | wxBOTTOM, 6);

    // options
    m_optionsizer = new wxBoxSizer (wxVERTICAL);
    m_matchcase = new wxCheckBox (this, -1, _("Match &case"));
    m_matchcase->SetValue ((flags & myFR_MATCHCASE) > 0);
    m_optionsizer->Add (m_matchcase, 0, wxBOTTOM, 6);
    m_wholeword = new wxCheckBox (this, -1, _("Whole &word"));
    m_wholeword->SetValue ((flags & myFR_WHOLEWORD) > 0);
    m_wholeword->Enable (false);
    m_optionsizer->Add (m_wholeword, 0, wxBOTTOM, 6);
    m_findregex = new wxCheckBox (this, -1, _("Regular &expression"));
    m_findregex->SetValue ((flags & myFR_FINDREGEX) > 0);
    m_optionsizer->Add (m_findregex, 0, wxBOTTOM, 6);
    m_subfolder = new wxCheckBox (this, -1, _("Sub &directories"));
    m_subfolder->SetValue ((flags & myFR_SUBFOLDER) > 0);
    m_optionsizer->Add (m_subfolder, 0);

    // directions
    m_dirssizer = new wxBoxSizer (wxVERTICAL);
    static const wxString directions[] = {_("&Upwards"), _("&Downwards")};
    m_direction = new wxRadioBox (this, -1, _("Direction"),
                                  wxDefaultPosition, wxDefaultSize,
                                  WXSIZEOF(directions), directions,
                                  1, wxRA_SPECIFY_COLS);
    m_direction->SetSelection(1);
    m_dirssizer->Add (m_direction, 0);

    // options and directions
    m_optionpane = new wxBoxSizer (wxHORIZONTAL);
    m_optionpane->Add (m_optionsizer, 1, wxALIGN_TOP|wxALIGN_LEFT);
    m_optionpane->Add (m_dirssizer, 0, wxALIGN_TOP|wxALIGN_RIGHT);
    m_findpane->Add (0, 6);
    m_findpane->Add (m_optionpane, 0, wxEXPAND);

    // buttons
    m_buttonpane = new wxBoxSizer (wxVERTICAL);
    m_findButton = new wxButton (this, wxID_OK, _("&Find"));
    m_findButton->SetDefault();
    m_buttonpane->Add (m_findButton, 0, wxEXPAND|wxALIGN_TOP|wxBOTTOM, 6);
    m_replaceButton = new wxButton (this, myID_REPLACE, _("&Replace"));
    m_buttonpane->Add (m_replaceButton, 0, wxEXPAND|wxALIGN_TOP|wxBOTTOM, 6);
    m_buttonpane->Show (m_replaceButton, false);
    m_replaceAllButton = new wxButton (this, myID_REPLACEALL, _("Replace &all"));
    m_buttonpane->Add (m_replaceAllButton, 0, wxEXPAND|wxALIGN_TOP|wxBOTTOM, 6);
    m_buttonpane->Show (m_replaceAllButton, false);
    m_cancelButton = new wxButton (this, wxID_CANCEL, _("Cancel"));
    m_buttonpane->Add (m_cancelButton, 0, wxEXPAND|wxALIGN_BOTTOM);

    m_totalpane = new wxBoxSizer (wxHORIZONTAL);
    m_totalpane->Add (m_findpane, 0, wxEXPAND | wxALL, 10);
    m_totalpane->Add (m_buttonpane, 0, wxEXPAND | wxALL, 10);

    m_findstr->SetFocus();
    m_findstr->SetSelection (-1, -1);
    SetSizerAndFit (m_totalpane);

    // load history
    LoadDirHistory ();
    LoadFindHistory ();
    LoadReplaceHistory ();
    UpdateDirHistory ();
    UpdateFindHistory ();
    UpdateReplaceHistory ();

}

myFindReplaceDlg::~myFindReplaceDlg () {

    // store history
    StoreDirHistory ();
    StoreFindHistory ();
    StoreReplaceHistory ();

}

//----------------------------------------------------------------------------
// event handlers

void myFindReplaceDlg::OnCancel (wxCommandEvent &WXUNUSED(event)) {
    EndModal (wxID_CANCEL);
}

void myFindReplaceDlg::OnHelp (wxCommandEvent &WXUNUSED(event)) {
    ////(pecan 2007/4/05)
    //-g_help->Display(_T("dialogs.html#findreplace"));
}

void myFindReplaceDlg::OnOkay (wxCommandEvent &WXUNUSED(event)) {
    UpdateFindHistory (m_findstr->GetValue ());
    if (m_style & myFR_REPLACEDIALOG) {
        UpdateReplaceHistory (m_replacestr->GetValue ());
    }else if (m_style & myFR_FINDINFILES) {
        UpdateDirHistory (m_finddir->GetValue ());
    }
    EndModal (wxID_OK);
}

void myFindReplaceDlg::OnReplace (wxCommandEvent &WXUNUSED(event)) {
    UpdateFindHistory (m_findstr->GetValue ());
    if (m_style & myFR_REPLACEDIALOG) {
        UpdateReplaceHistory (m_replacestr->GetValue ());
    }else if (m_style & myFR_FINDINFILES) {
        UpdateDirHistory (m_finddir->GetValue ());
    }
    EndModal (myID_REPLACE);
}

void myFindReplaceDlg::OnReplaceAll (wxCommandEvent &WXUNUSED(event)) {
    UpdateFindHistory (m_findstr->GetValue ());
    if (m_style & myFR_REPLACEDIALOG) {
        UpdateReplaceHistory (m_replacestr->GetValue ());
    }else if (m_style & myFR_FINDINFILES) {
        UpdateDirHistory (m_finddir->GetValue ());
    }
    EndModal (myID_REPLACEALL);
}

//----------------------------------------------------------------------------
// private functions

wxString myFindReplaceDlg::GetFindDirectory () {
    return m_finddir->GetValue ();
}

void myFindReplaceDlg::SetFindDirectory (const wxString &finddir) {
    m_finddir->SetValue (finddir);
}

wxString myFindReplaceDlg::GetFindFilespec () {
    return m_findspec->GetValue ();
}

void myFindReplaceDlg::SetFindFilespec (const wxString &findspec) {
    m_findspec->SetValue (findspec);
}

wxString myFindReplaceDlg::GetFindString () {
    return m_findstr->GetValue ();
}

void myFindReplaceDlg::SetFindString (const wxString &findstr) {
    m_findstr->SetSelection (0);
    m_findstr->SetValue (findstr);
}

wxString myFindReplaceDlg::GetReplaceString () {
    return m_replacestr->GetValue ();
}

void myFindReplaceDlg::SetReplaceString (const wxString &replacestr) {
    m_replacestr->SetSelection (0);
    m_replacestr->SetValue (replacestr);
}

int myFindReplaceDlg::GetFlags () {
    // flags (myFR_DOWN, myFR_MATCHCASE, myFR_WHOLEWORD, myFR_FINDREGEX)
    int flags = 0;
    if (m_direction->GetSelection()!=0) flags |= myFR_DOWN;
    if (m_matchcase->GetValue()) flags |= myFR_MATCHCASE;
    if (m_wholeword->GetValue()) flags |= myFR_WHOLEWORD;
    if (m_findregex->GetValue()) flags |= myFR_FINDREGEX;
    if (m_subfolder->GetValue()) flags |= myFR_SUBFOLDER;
    return flags;
}

void myFindReplaceDlg::SetFlags (int flags) {
    if (flags & myFR_DOWN) {
        m_direction->SetSelection (1);
    }else{
        m_direction->SetSelection (0);
    }
    m_matchcase->SetValue ((flags & myFR_MATCHCASE) > 0);
    m_wholeword->SetValue ((flags & myFR_WHOLEWORD) > 0);
    m_findregex->SetValue ((flags & myFR_FINDREGEX) > 0);
    m_subfolder->SetValue ((flags & myFR_SUBFOLDER) > 0);
}

int myFindReplaceDlg::ShowModal (long style) {
    UpdateDirHistory ();
    UpdateFindHistory ();
    UpdateReplaceHistory ();
    m_style = style;
    if (m_style & myFR_REPLACEDIALOG) {
        SetTitle(_("Find and Replace"));
        m_findpane->Show (m_fdirsizer, false);
        m_findpane->Show (m_specsizer, false);
        m_findpane->Show (m_replsizer, true);
        m_optionsizer->Show (m_wholeword, true);
        m_optionsizer->Show (m_subfolder, false);
        m_dirssizer->Show (m_direction, true);
        m_buttonpane->Show (m_replaceButton, true);
        m_buttonpane->Show (m_replaceAllButton, true);
    }else if (m_style & myFR_FINDINFILES) {
        SetTitle(_("Find in Files"));
        m_findpane->Show (m_fdirsizer, true);
        m_findpane->Show (m_specsizer, true);
        m_findpane->Show (m_replsizer, false);
        m_optionsizer->Show (m_wholeword, false);
        m_optionsizer->Show (m_subfolder, true);
        m_dirssizer->Show (m_direction, false);
        m_buttonpane->Show (m_replaceButton, false);
        m_buttonpane->Show (m_replaceAllButton, false);
        m_wholeword->Enable (false);
    }else{
        SetTitle(_("Find"));
        m_findpane->Show (m_fdirsizer, false);
        m_findpane->Show (m_specsizer, false);
        m_findpane->Show (m_replsizer, false);
        m_optionsizer->Show (m_wholeword, true);
        m_optionsizer->Show (m_subfolder, false);
        m_dirssizer->Show (m_direction, true);
        m_buttonpane->Show (m_replaceButton, false);
        m_buttonpane->Show (m_replaceAllButton, false);
        m_wholeword->Enable (true);
    }
    m_direction->Enable (true);
    m_matchcase->Enable (!(style & myFR_NOMATCHCASE));
    m_wholeword->Enable (!(style & myFR_NOWHOLEWORD));
    m_findregex->Enable (!(style & myFR_NOFINDREGEX));
    m_findstr->SetFocus();
    m_findstr->SetSelection (-1, -1);
    Fit ();
    return wxScrollingDialog::ShowModal ();
}

void myFindReplaceDlg::LoadDirHistory () {
    if (!m_finddirHist.IsEmpty()) return;
    wxConfig *cfg = new wxConfig (g_appname);
    wxString group = FINDREPLACEDLG + _T("/") + DIRECTORYHISTORY;
    wxString key;
    wxString value;
    int i;
    for (i = 0; i < MAXHISTORY; ++i) {
        key = group + wxString::Format (_T("%d"), i);
        if (cfg->Read (key, &value)) m_finddirHist.Add (value);
    }
    delete cfg;
}

void myFindReplaceDlg::StoreDirHistory () {
    wxConfig *cfg = new wxConfig (g_appname);
    wxString group = FINDREPLACEDLG + _T("/") + DIRECTORYHISTORY;
    wxString key;
    wxString value;
    int i;
    for (i = 0; i < (int)m_finddirHist.GetCount(); ++i) {
        key = group + wxString::Format (_T("%d"), i);
        cfg->Write (key, m_finddirHist[i]);
    }
    delete cfg;
}

void myFindReplaceDlg::UpdateDirHistory (wxString item) {
// FIXME:   //(pecan 2007/4/05)
//    wxString value = m_finddir->GetValue();
//    if (!item.IsEmpty()) {
//        int i = m_finddirHist.Index (item);
//        if (i != wxNOT_FOUND) m_finddirHist.Remove (item);
//        if (m_finddirHist.GetCount() == MAXHISTORY) {
//            m_finddirHist.Remove (m_finddirHist.Last());
//        }
//        m_finddirHist.Insert (item, 0);
//    }
//    // since m_finddir->Clear() clears to much
//    while (m_finddir->GetCount() > 0) m_finddir->Delete (0);
//    int i;
//    for (i = 0; i < (int)m_finddirHist.GetCount(); ++i) {
//        m_finddir->Append (m_finddirHist[i]);
//    }
//    m_finddir->SetValue(value);
}

void myFindReplaceDlg::LoadFindHistory () {
// FIXME: //(pecan 2007/4/05)
//    if (!m_findstrHist.IsEmpty()) return;
//    wxConfig *cfg = new wxConfig (g_appname);
//    wxString group = FINDREPLACEDLG + _T("/") + FINDSTRHISTORY;
//    wxString key;
//    wxString value;
//    int i;
//    for (i = 0; i < MAXHISTORY; ++i) {
//        key = group + wxString::Format (_T("%d"), i);
//        if (cfg->Read (key, &value)) m_findstrHist.Add (value);
//    }
//    delete cfg;
}

void myFindReplaceDlg::StoreFindHistory () {
    wxConfig *cfg = new wxConfig (g_appname);
    wxString group = FINDREPLACEDLG + _T("/") + FINDSTRHISTORY;
    wxString key;
    wxString value;
    int i;
    for (i = 0; i < (int)m_findstrHist.GetCount(); ++i) {
        key = group + wxString::Format (_T("%d"), i);
        cfg->Write (key, m_findstrHist[i]);
    }
    delete cfg;
}

void myFindReplaceDlg::UpdateFindHistory (wxString item) {
// FIXME: //(pecan 2007/4/05)
//    wxString value = m_findstr->GetValue();
//    if (!item.IsEmpty()) {
//        int i = m_findstrHist.Index (item);
//        if (i != wxNOT_FOUND) m_findstrHist.Remove (item);
//        if (m_findstrHist.GetCount() == MAXHISTORY) {
//            m_findstrHist.Remove (m_findstrHist.Last());
//        }
//        m_findstrHist.Insert (item, 0);
//    }
//    // since m_findstr->Clear() clears to much
//    while (m_findstr->GetCount() > 0) m_findstr->Delete (0);
//    int i;
//    for (i = 0; i < (int)m_findstrHist.GetCount(); ++i) {
//        m_findstr->Append (m_findstrHist[i]);
//    }
//    m_findstr->SetValue(value);
}

void myFindReplaceDlg::LoadReplaceHistory ()
{// FIXME: //(pecan 2007/4/05)
//    if (!m_replacestrHist.IsEmpty()) return;
//    wxConfig *cfg = new wxConfig (g_appname);
//    wxString group = FINDREPLACEDLG + _T("/") + REPLACESTRHISTORY;
//    wxString key;
//    wxString value;
//    int i;
//    for (i = 0; i < MAXHISTORY; ++i) {
//        key = group + wxString::Format (_T("%d"), i);
//        if (cfg->Read (key, &value)) m_replacestrHist.Add (value);
//    }
//    delete cfg;
}

void myFindReplaceDlg::StoreReplaceHistory ()
{// FIXME: //(pecan 2007/4/05)
//    wxConfig *cfg = new wxConfig (g_appname);
//    wxString group = FINDREPLACEDLG + _T("/") + REPLACESTRHISTORY;
//    wxString key;
//    wxString value;
//    int i;
//    for (i = 0; i < (int)m_replacestrHist.GetCount(); ++i) {
//        key = group + wxString::Format (_T("%d"), i);
//        cfg->Write (key, m_replacestrHist[i]);
//    }
//    delete cfg;
}

void myFindReplaceDlg::UpdateReplaceHistory (wxString item)
{// FIXME: //(pecan 2007/4/05)
//    wxString value = m_replacestr->GetValue();
//    if (!item.IsEmpty()) {
//        int i = m_replacestrHist.Index (item);
//        if (i != wxNOT_FOUND) m_replacestrHist.Remove (item);
//        if (m_replacestrHist.GetCount() == MAXHISTORY) {
//            m_replacestrHist.Remove (m_replacestrHist.Last());
//        }
//        m_replacestrHist.Insert (item, 0);
//    }
//    // since m_replacestr->Clear() clears to much
//    while (m_replacestr->GetCount() > 0) m_replacestr->Delete (0);
//    int i;
//    for (i = 0; i < (int)m_replacestrHist.GetCount(); ++i) {
//        m_replacestr->Append (m_replacestrHist[i]);
//    }
//    m_replacestr->SetValue(value);
}


//----------------------------------------------------------------------------
// myGotoDlg
//----------------------------------------------------------------------------

BEGIN_EVENT_TABLE (myGotoDlg, wxScrollingDialog)
    EVT_MENU   (wxID_HELP, myGotoDlg::OnHelp)
END_EVENT_TABLE()

myGotoDlg::myGotoDlg (wxWindow *parent,
                      long style)
         : wxScrollingDialog (parent, -1, _("Goto line"),
                     wxDefaultPosition, wxDefaultSize,
                     style | wxDEFAULT_DIALOG_STYLE) {

    //accelerators (for help)
    const int nEntries = 1 ;
    wxAcceleratorEntry entries[nEntries];
    entries[0].Set (wxACCEL_NORMAL, WXK_F1, wxID_HELP);
    wxAcceleratorTable accel (nEntries, entries);
    SetAcceleratorTable (accel);

    // goto
    wxBoxSizer *gotopane = new wxBoxSizer (wxHORIZONTAL);
    gotopane->Add (new wxStaticText (this, -1, _("Go to:"),
                                     wxDefaultPosition, wxSize(60, -1)),
                   0, wxALIGN_TOP | wxALIGN_LEFT);
    gotopane->Add (6, 0);
    m_position = new wxTextCtrl (this, -1, _T(""),
                                 wxDefaultPosition, wxSize(60, -1));
    gotopane->Add (m_position, 0, wxALIGN_TOP | wxALIGN_RIGHT);

    // buttons
    wxBoxSizer *buttonpane = new wxBoxSizer (wxVERTICAL);
    m_gotoButton = new wxButton (this, wxID_OK, _("&Goto"));
    m_gotoButton->SetDefault();
    buttonpane->Add (m_gotoButton, 0, wxEXPAND | wxALIGN_TOP | wxBOTTOM, 6);
    m_cancelButton = new wxButton (this, wxID_CANCEL, _("Cancel"));
    buttonpane->Add (m_cancelButton, 0, wxEXPAND | wxALIGN_BOTTOM);

    wxBoxSizer *totalpane = new wxBoxSizer (wxHORIZONTAL);
    totalpane->Add (gotopane, 0, wxEXPAND | wxALL, 10);
    totalpane->Add (buttonpane, 0, wxEXPAND | wxALL, 10);

    m_position->SetFocus();
    m_position->SetSelection (-1, -1);
    SetSizerAndFit (totalpane);

}

//----------------------------------------------------------------------------
// event handlers

void myGotoDlg::OnHelp (wxCommandEvent &WXUNUSED(event)) {
    // //(pecan 2007/4/05)
    //g_help->Display(_T("dialogs.html#gotoposition"));
}

//----------------------------------------------------------------------------
// private functions

int myGotoDlg::GetPosition () {
    long position;
    if (m_position->GetValue ().ToLong(&position)) return position;
    return -1;
}

void myGotoDlg::SetPosition (int position) {
    m_position->SetValue (wxString::Format(_T("%d"), position));
}

int myGotoDlg::ShowModal (long WXUNUSED(style)) {
    m_position->SetFocus();
    m_position->SetSelection (-1, -1);
    return wxScrollingDialog::ShowModal ();
}

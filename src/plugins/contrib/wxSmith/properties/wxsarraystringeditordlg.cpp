/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "wxsarraystringeditordlg.h"

#include <wx/tokenzr.h>

//(*InternalHeaders(wxsArrayStringEditorDlg)
#include <wx/string.h>
#include <wx/intl.h>
//*)

//(*IdInit(wxsArrayStringEditorDlg)
const long wxsArrayStringEditorDlg::ID_TEXTCTRL1 = wxNewId();
//*)

#include "prep.h"

BEGIN_EVENT_TABLE(wxsArrayStringEditorDlg,wxScrollingDialog)
    //(*EventTable(wxsArrayStringEditorDlg)
    //*)
END_EVENT_TABLE()

wxsArrayStringEditorDlg::wxsArrayStringEditorDlg(wxWindow* parent,wxArrayString& _Data,wxWindowID id):
    Data(_Data)
{
    //(*Initialize(wxsArrayStringEditorDlg)
    wxButton* Button1;
    wxButton* Button2;
    wxBoxSizer* BoxSizer2;
    wxBoxSizer* BoxSizer1;
    wxStaticBoxSizer* StaticBoxSizer1;

    Create(parent, id, _("Edit items"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Enter items (one item per line)"));
    Items = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxSize(350,200), wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    StaticBoxSizer1->Add(Items, 1, wxALIGN_CENTER_VERTICAL, 2);
    BoxSizer1->Add(StaticBoxSizer1, 1, wxALIGN_CENTER_HORIZONTAL, 5);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    Button1 = new wxButton(this, wxID_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("wxID_OK"));
    Button1->SetDefault();
    BoxSizer2->Add(Button1, 1, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    Button2 = new wxButton(this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("wxID_CANCEL"));
    BoxSizer2->Add(Button2, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(BoxSizer2, 0, wxALIGN_RIGHT, 5);
    SetSizer(BoxSizer1);
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);

    Connect(wxID_OK,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsArrayStringEditorDlg::OnOK);
    Connect(wxID_CANCEL,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsArrayStringEditorDlg::OnCancel);
    //*)

    size_t Count = Data.Count();
    for ( size_t i = 0; i<Count; i++ )
    {
        Items->AppendText(Data[i]);
        Items->AppendText(_T('\n'));
    }
}

wxsArrayStringEditorDlg::~wxsArrayStringEditorDlg()
{
    //(*Destroy(wxsArrayStringEditorDlg)
    //*)
}


void wxsArrayStringEditorDlg::OnOK(cb_unused wxCommandEvent& event)
{
    wxStringTokenizer Tknz(Items->GetValue(),_T("\n"),wxTOKEN_RET_EMPTY);
    Data.Clear();

    while ( Tknz.HasMoreTokens() )
    {
        wxString Line = Tknz.GetNextToken();
        Data.Add(Line);
    }

    EndModal(wxID_OK);
}

void wxsArrayStringEditorDlg::OnCancel(cb_unused wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}

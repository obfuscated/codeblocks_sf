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

#include "wxsfontfaceeditordlg.h"
#include <wx/fontdlg.h>

//(*InternalHeaders(wxsFontFaceEditorDlg)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(wxsFontFaceEditorDlg)
const long wxsFontFaceEditorDlg::ID_TEXTCTRL1 = wxNewId();
const long wxsFontFaceEditorDlg::ID_BUTTON1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(wxsFontFaceEditorDlg,wxScrollingDialog)
    //(*EventTable(wxsFontFaceEditorDlg)
    //*)
END_EVENT_TABLE()

wxsFontFaceEditorDlg::wxsFontFaceEditorDlg(wxWindow* parent,wxString& _Face,wxWindowID id):
    Face(_Face)
{
    //(*Initialize(wxsFontFaceEditorDlg)
    wxBoxSizer* BoxSizer2;
    wxBoxSizer* BoxSizer1;
    wxStaticBoxSizer* StaticBoxSizer1;

    Create(parent, id, _("Selecting font face"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Face name"));
    FaceName = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    StaticBoxSizer1->Add(FaceName, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 3);
    Button1 = new wxButton(this, ID_BUTTON1, _("Pick"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    StaticBoxSizer1->Add(Button1, 0, wxLEFT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 3);
    BoxSizer1->Add(StaticBoxSizer1, 1, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    BoxSizer2->Add(60,-1,1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button2 = new wxButton(this, wxID_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("wxID_OK"));
    Button2->SetDefault();
    BoxSizer2->Add(Button2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button3 = new wxButton(this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("wxID_CANCEL"));
    BoxSizer2->Add(Button3, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2->Add(-1,-1,1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(BoxSizer2, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(BoxSizer1);
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);
    Center();

    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsFontFaceEditorDlg::OnButton1Click);
    Connect(wxID_OK,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsFontFaceEditorDlg::OnButton2Click);
    //*)
    FaceName->SetValue(Face);
}

wxsFontFaceEditorDlg::~wxsFontFaceEditorDlg()
{
    //(*Destroy(wxsFontFaceEditorDlg)
    //*)
}

void wxsFontFaceEditorDlg::OnButton2Click(wxCommandEvent& event)
{
    Face = FaceName->GetValue();
    EndModal(wxID_OK);
}

void wxsFontFaceEditorDlg::OnButton1Click(wxCommandEvent& event)
{
    wxFont Font = ::wxGetFontFromUser();
    if ( Font.Ok() )
    {
        FaceName->SetValue(Font.GetFaceName());
    }
}

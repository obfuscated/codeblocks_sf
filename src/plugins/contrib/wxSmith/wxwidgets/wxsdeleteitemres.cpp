/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2007  Bartlomiej Swiecki
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

#include "wxsdeleteitemres.h"

//(*InternalHeaders(wxsDeleteItemRes)
#include <wx/button.h>
#include <wx/string.h>
#include <wx/intl.h>
//*)

//(*IdInit(wxsDeleteItemRes)
const long wxsDeleteItemRes::ID_CHECKBOX1 = wxNewId();
const long wxsDeleteItemRes::ID_CHECKBOX2 = wxNewId();
const long wxsDeleteItemRes::ID_CHECKBOX3 = wxNewId();
//*)

BEGIN_EVENT_TABLE(wxsDeleteItemRes,wxScrollingDialog)
    //(*EventTable(wxsDeleteItemRes)
    //*)
END_EVENT_TABLE()

wxsDeleteItemRes::wxsDeleteItemRes()
{
    //(*Initialize(wxsDeleteItemRes)
    wxStaticText* StaticText1;
    wxBoxSizer* BoxSizer1;
    wxStdDialogButtonSizer* StdDialogButtonSizer1;
    wxStaticBoxSizer* StaticBoxSizer1;

    Create(0, wxID_ANY, _("Deleting resource"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Delete options"));
    m_PhysDeleteWXS = new wxCheckBox(this, ID_CHECKBOX1, _("Physically delete WXS file"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
    m_PhysDeleteWXS->SetValue(true);
    StaticBoxSizer1->Add(m_PhysDeleteWXS, 0, wxEXPAND, 5);
    m_DeleteSources = new wxCheckBox(this, ID_CHECKBOX2, _("Delete source files from project"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
    m_DeleteSources->SetValue(false);
    StaticBoxSizer1->Add(m_DeleteSources, 0, wxTOP|wxEXPAND, 5);
    m_PhysDeleteSources = new wxCheckBox(this, ID_CHECKBOX3, _("Physically delete source files"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX3"));
    m_PhysDeleteSources->SetValue(false);
    m_PhysDeleteSources->Disable();
    StaticBoxSizer1->Add(m_PhysDeleteSources, 0, wxTOP|wxEXPAND, 5);
    StaticText1 = new wxStaticText(this, wxID_ANY, _("Note that there\'s no undo\nafter deleting resource"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE, _T("wxID_ANY"));
    StaticBoxSizer1->Add(StaticText1, 1, wxTOP|wxEXPAND, 5);
    BoxSizer1->Add(StaticBoxSizer1, 1, wxEXPAND, 5);
    StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
    StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_OK, wxEmptyString));
    StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
    StdDialogButtonSizer1->Realize();
    BoxSizer1->Add(StdDialogButtonSizer1, 0, wxTOP|wxALIGN_CENTER_HORIZONTAL, 5);
    SetSizer(BoxSizer1);
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);

    Connect(ID_CHECKBOX2,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsDeleteItemRes::Onm_DeleteSourcesClick);
    //*)
}

wxsDeleteItemRes::~wxsDeleteItemRes()
{
    //(*Destroy(wxsDeleteItemRes)
    //*)
}


void wxsDeleteItemRes::Onm_DeleteSourcesClick(wxCommandEvent& event)
{
    m_PhysDeleteSources->Enable(m_DeleteSources->GetValue());
}

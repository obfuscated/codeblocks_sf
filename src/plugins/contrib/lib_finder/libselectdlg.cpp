/*
* This file is part of lib_finder plugin for Code::Blocks Studio
* Copyright (C) 2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "libselectdlg.h"

//(*InternalHeaders(LibSelectDlg)
#include <wx/checklst.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/radiobut.h>
#include <wx/string.h>
#include <wx/intl.h>
#include <wx/stattext.h>
//*)

#include <manager.h>
#include <configmanager.h>

//(*IdInit(LibSelectDlg)
const long LibSelectDlg::ID_STATICTEXT1 = wxNewId();
const long LibSelectDlg::ID_CHECKLISTBOX1 = wxNewId();
const long LibSelectDlg::ID_RADIOBUTTON1 = wxNewId();
const long LibSelectDlg::ID_RADIOBUTTON2 = wxNewId();
const long LibSelectDlg::ID_RADIOBUTTON3 = wxNewId();
const long LibSelectDlg::ID_CHECKBOX1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(LibSelectDlg,wxScrollingDialog)
	//(*EventTable(LibSelectDlg)
	//*)
	EVT_BUTTON(wxID_OK,LibSelectDlg::OnOk)
END_EVENT_TABLE()

LibSelectDlg::LibSelectDlg(wxWindow* parent,const wxArrayString& Names,bool addOnly)
{
	//(*Initialize(LibSelectDlg)
	wxBoxSizer* BoxSizer1;
	wxStdDialogButtonSizer* StdDialogButtonSizer1;
	wxStaticBoxSizer* StaticBoxSizer1;
	
	Create(parent, wxID_ANY, _("Setting up libraries"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Select libraries You want to set up:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	BoxSizer1->Add(StaticText1, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 10);
	m_Libraries = new wxCheckListBox(this, ID_CHECKLISTBOX1, wxDefaultPosition, wxSize(361,251), 0, 0, 0, wxDefaultValidator, _T("ID_CHECKLISTBOX1"));
	BoxSizer1->Add(m_Libraries, 1, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 10);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Previous settings"));
	m_DontClear = new wxRadioButton(this, ID_RADIOBUTTON1, _("Do not clear previous results (but remove duplicates)"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP, wxDefaultValidator, _T("ID_RADIOBUTTON1"));
	StaticBoxSizer1->Add(m_DontClear, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 10);
	m_ClearSelected = new wxRadioButton(this, ID_RADIOBUTTON2, _("Clear previous results for selected libraries"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON2"));
	m_ClearSelected->SetValue(true);
	StaticBoxSizer1->Add(m_ClearSelected, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 10);
	m_ClearAll = new wxRadioButton(this, ID_RADIOBUTTON3, _("Clear all previous libraries settings"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON3"));
	StaticBoxSizer1->Add(m_ClearAll, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 10);
	BoxSizer1->Add(StaticBoxSizer1, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 10);
	m_SetupGlobalVars = new wxCheckBox(this, ID_CHECKBOX1, _("Set up Global Variables"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	m_SetupGlobalVars->SetValue(true);
	BoxSizer1->Add(m_SetupGlobalVars, 0, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 10);
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_OK, wxEmptyString));
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
	StdDialogButtonSizer1->Realize();
	BoxSizer1->Add(StdDialogButtonSizer1, 0, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 10);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	Center();
	//*)

	if ( (m_AddOnly = addOnly) )
	{
	    m_DontClear->SetValue( true );
	    m_ClearSelected->SetValue( false );
	    m_ClearAll->SetValue( false );
	    m_DontClear->Disable();
	    m_ClearSelected->Disable();
	    m_ClearAll->Disable();
	}

	m_Libraries->Append(Names);

	ConfigManager* Cfg = Manager::Get()->GetConfigManager(_T("lib_finder"));
	switch ( Cfg->ReadInt(_T("libselect/previous"),1) )
	{
	    case 0: m_DontClear->SetValue(true); break;
	    case 2: m_ClearAll->SetValue(true); break;
	    case 1:
	    default: m_ClearSelected->SetValue(true); break;
	}
	m_SetupGlobalVars->SetValue(Cfg->ReadBool(_T("libselect/setup_global_vars"),true));
}

LibSelectDlg::~LibSelectDlg()
{
	//(*Destroy(LibSelectDlg)
	//*)
}

void LibSelectDlg::SetSelections(const wxArrayInt& Selections)
{
    m_Libraries->Freeze();

    // First clear all
    for ( size_t i=0; i<m_Libraries->GetCount(); i++ )
    {
        m_Libraries->Check(i,false);
    }

    // Next select only those which are on the list
    for ( size_t i=0; i<Selections.Count(); i++ )
    {
        m_Libraries->Check(Selections[i],true);
    }

    m_Libraries->Thaw();
}

wxArrayInt LibSelectDlg::GetSelections()
{
    wxArrayInt Ret;
    for ( size_t i=0; i<m_Libraries->GetCount(); i++ )
    {
        if ( m_Libraries->IsChecked(i) )
        {
            Ret.Add((int)i);
        }
    }
    return Ret;
}

bool LibSelectDlg::GetSetupGlobalVars()
{
    return m_SetupGlobalVars->GetValue();
}

bool LibSelectDlg::GetDontClearPrevious()
{
    return m_DontClear->GetValue();
}

bool LibSelectDlg::GetClearSelectedPrevious()
{
    return m_ClearSelected->GetValue();
}

bool LibSelectDlg::GetClearAllPrevious()
{
    return m_ClearAll->GetValue();
}

void LibSelectDlg::OnOk(wxCommandEvent& event)
{
	ConfigManager* Cfg = Manager::Get()->GetConfigManager(_T("lib_finder"));

	if ( !m_DontClear )
	{
        if ( m_DontClear->GetValue()     ) Cfg->Write(_T("libselect/previous"),0);
        if ( m_ClearSelected->GetValue() ) Cfg->Write(_T("libselect/previous"),1);
        if ( m_ClearAll->GetValue()      ) Cfg->Write(_T("libselect/previous"),2);
	}
	Cfg->Write(_T("libselect/setup_global_vars"),m_SetupGlobalVars->GetValue());
    event.Skip();
}

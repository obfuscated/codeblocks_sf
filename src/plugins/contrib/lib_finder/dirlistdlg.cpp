/*
* This file is part of lib_finder plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
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
* $Revision: 4504 $
* $Id: wxsmithpluginregistrants.cpp 4504 2007-10-02 21:52:30Z byo $
* $HeadURL: svn+ssh://byo@svn.berlios.de/svnroot/repos/codeblocks/trunk/src/plugins/contrib/wxSmith/plugin/wxsmithpluginregistrants.cpp $
*/

#include "dirlistdlg.h"

#include <wx/dirdlg.h>
#include <wx/string.h>
#include <wx/tokenzr.h>

//(*InternalHeaders(DirListDlg)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(DirListDlg)
const long DirListDlg::ID_TEXTCTRL1 = wxNewId();
const long DirListDlg::ID_BUTTON1 = wxNewId();
const long DirListDlg::ID_BUTTON2 = wxNewId();
const long DirListDlg::ID_BUTTON3 = wxNewId();
const long DirListDlg::ID_BUTTON4 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DirListDlg,wxDialog)
	//(*EventTable(DirListDlg)
	//*)
END_EVENT_TABLE()

DirListDlg::DirListDlg(wxWindow* parent,const wxArrayString& Dirs,wxWindowID id)
{
	//(*Initialize(DirListDlg)
	wxButton* Button1;
	wxButton* Button2;
	wxStaticBoxSizer* StaticBoxSizer1;
	
	Create(parent, id, _("List of directories with libraries"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Scanned directories:"));
	DirList = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxSize(292,194), wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	StaticBoxSizer1->Add(DirList, 1, wxBOTTOM|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	Button1 = new wxButton(this, ID_BUTTON1, _("Add dir"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	BoxSizer1->Add(Button1, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button2 = new wxButton(this, ID_BUTTON2, _("Clear All"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	BoxSizer1->Add(Button2, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1->Add(BoxSizer1, 0, wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 0);
	FlexGridSizer1->Add(StaticBoxSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	Button3 = new wxButton(this, ID_BUTTON3, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
	BoxSizer2->Add(Button3, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button4 = new wxButton(this, ID_BUTTON4, _("Next"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON4"));
	Button4->SetDefault();
	BoxSizer2->Add(Button4, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(BoxSizer2, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);
	
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DirListDlg::OnButton1Click);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DirListDlg::OnButton2Click);
	Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DirListDlg::OnButton3Click);
	Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DirListDlg::OnButton4Click);
	//*)

	wxString List;
	for ( size_t i=0; i<Dirs.Count(); i++ )
	{
	    List << Dirs[i] << _T("\n");
	}
	DirList->SetValue(List);
}

DirListDlg::~DirListDlg()
{
}

void DirListDlg::OnButton1Click(wxCommandEvent& event)
{
    wxString DirName = ::wxDirSelector();
    if ( DirName.empty() ) return;

    if ( !DirList->GetValue().empty() ) DirList->AppendText(_T("\n"));
    DirList->AppendText(DirName);
}

void DirListDlg::OnButton2Click(wxCommandEvent& event)
{
    DirList->Clear();
}

void DirListDlg::OnButton3Click(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}

void DirListDlg::OnButton4Click(wxCommandEvent& event)
{
    wxStringTokenizer Tknz(DirList->GetValue(),_T("\n"));
    Dirs.Clear();
    while ( Tknz.HasMoreTokens() )
    {
        Dirs.Add(Tknz.NextToken());
    }
    EndModal(wxID_OK);
}

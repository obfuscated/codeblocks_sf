/*
	This file is part of Code Snippets, a plugin for Code::Blocks
	Copyright (C) 2006 Arto Jonsson

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

#include "sdk.h"
#ifndef CB_PRECOMP
	#include "manager.h"
	#include "configmanager.h"
#endif

#include "editsnippetdlg.h"

BEGIN_EVENT_TABLE(EditSnippetDlg, wxDialog)
	EVT_BUTTON(wxID_OK, EditSnippetDlg::OnOK)
	EVT_BUTTON(wxID_CANCEL, EditSnippetDlg::OnCancel)
	EVT_BUTTON(wxID_HELP, EditSnippetDlg::OnHelp)
END_EVENT_TABLE()

EditSnippetDlg::EditSnippetDlg(const wxString& snippetName, const wxString& snippetText)
	: wxDialog(Manager::Get()->GetAppWindow(), wxID_ANY, _("Edit snippet"),
		wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxRESIZE_BORDER)
{
	InitDialog(snippetName, snippetText);

	// Load the window's size
	ConfigManager* cfgMan = Manager::Get()->GetConfigManager(_T("codesnippets"));
	SetSize(cfgMan->ReadInt(_T("editdlg_w"), 500), cfgMan->ReadInt(_T("editdlg_h"), 400));

	if (cfgMan->ReadBool(_T("editdlg_maximized"), false))
	{
		Maximize(true);
	}
}

EditSnippetDlg::~EditSnippetDlg()
{
	// Save the window's size
	ConfigManager* cfgMan = Manager::Get()->GetConfigManager(_T("codesnippets"));
	if (!IsMaximized())
	{
		wxSize windowSize = GetSize();
		cfgMan->Write(_T("editdlg_w"), windowSize.GetWidth());
		cfgMan->Write(_T("editdlg_h"), windowSize.GetHeight());

		cfgMan->Write(_T("editdlg_maximized"), false);
	}
	else
	{
		cfgMan->Write(_T("editdlg_maximized"), true);
	}
}

void EditSnippetDlg::InitDialog(const wxString& snippetName, const wxString& snippetText)
{
	wxBoxSizer* dlgSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* snippetDataSizer = new wxBoxSizer(wxVERTICAL);

	m_NameLbl = new wxStaticText(this, wxID_ANY, _("Name:"), wxDefaultPosition, wxDefaultSize, 0);
	snippetDataSizer->Add( m_NameLbl, 0, wxTOP|wxRIGHT|wxLEFT, 5 );

	m_SnippetNameCtrl = new wxTextCtrl(this, wxID_ANY, snippetName, wxDefaultPosition, wxDefaultSize, 0);
	snippetDataSizer->Add(m_SnippetNameCtrl, 0, wxEXPAND|wxALL, 5 );

	m_SnippetLbl = new wxStaticText(this, wxID_ANY, _("Snippet:"), wxDefaultPosition, wxDefaultSize, 0);
	snippetDataSizer->Add(m_SnippetLbl, 0, wxTOP|wxRIGHT|wxLEFT, 5);

	m_SnippetTextCtrl = new wxTextCtrl(this, wxID_ANY, snippetText, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_DONTWRAP|wxTE_PROCESS_TAB);
	snippetDataSizer->Add(m_SnippetTextCtrl, 1, wxALL|wxEXPAND, 5);

	dlgSizer->Add(snippetDataSizer, 1, wxEXPAND, 5);

	wxBoxSizer* buttonsSizer = new wxBoxSizer(wxHORIZONTAL);

	m_OKBtn = new wxButton(this, wxID_OK, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	buttonsSizer->Add(m_OKBtn, 0, wxALL, 5);

	m_CancelBtn = new wxButton(this, wxID_CANCEL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	buttonsSizer->Add(m_CancelBtn, 0, wxALL, 5);

	m_HelpBtn = new wxButton(this, wxID_HELP, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	buttonsSizer->Add(m_HelpBtn, 0, wxALL, 5);

	dlgSizer->Add(buttonsSizer, 0, wxALIGN_RIGHT, 5);

	SetSizer(dlgSizer);
	Layout();
}

wxString EditSnippetDlg::GetName()
{
	return m_SnippetNameCtrl->GetValue();
}

wxString EditSnippetDlg::GetText()
{
	return m_SnippetTextCtrl->GetValue();
}

void EditSnippetDlg::OnOK(wxCommandEvent& event)
{
	EndModal(wxID_OK);
}

void EditSnippetDlg::OnCancel(wxCommandEvent& event)
{
	EndModal(wxID_CANCEL);
}

void EditSnippetDlg::OnHelp(wxCommandEvent& event)
{
	// Link to the Wiki which contains information about the available macros
	wxLaunchDefaultBrowser(_T("http://wiki.codeblocks.org/index.php?title=Builtin_variables"));
}

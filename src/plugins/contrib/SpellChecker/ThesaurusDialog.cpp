/*
* This file is part of SpellChecker plugin for Code::Blocks Studio
* Copyright (C) 2009 Daniel Anselmi
*
* SpellChecker plugin is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* SpellChecker plugin is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with SpellChecker. If not, see <http://www.gnu.org/licenses/>.
*
*/
#include "ThesaurusDialog.h"

#include <wx/xrc/xmlres.h>


BEGIN_EVENT_TABLE(ThesaurusDialog,wxDialog)
END_EVENT_TABLE()

ThesaurusDialog::ThesaurusDialog(wxWindow* parent, wxString Word, std::map<wxString, std::vector< wxString > > &syn):
    wxDialog( parent, wxID_ANY, wxString(_T("Thesaurus"))),
    m_syn(syn)
{
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );

	wxStaticText *staticText;

	staticText = new wxStaticText( this, wxID_ANY, wxT("Looked up:"), wxDefaultPosition, wxDefaultSize, 0 );
	staticText->Wrap( -1 );
	bSizer2->Add( staticText, 0, wxALL, 0 );

	wxArrayString m_ChoiceLookedUpChoices;
	m_ChoiceLookedUp = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_ChoiceLookedUpChoices, 0 );
	m_ChoiceLookedUp->SetSelection( 0 );
	bSizer2->Add( m_ChoiceLookedUp, 0, wxALL|wxEXPAND, 5 );

	staticText = new wxStaticText( this, wxID_ANY, wxT("Meanings:"), wxDefaultPosition, wxDefaultSize, 0 );
	staticText->Wrap( -1 );
	bSizer2->Add( staticText, 0, wxALL, 0 );

	m_ListBoxMeanings = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	bSizer2->Add( m_ListBoxMeanings, 1, wxALL|wxEXPAND, 5 );

	bSizer3->Add( bSizer2, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );

	staticText = new wxStaticText( this, wxID_ANY, wxT("Replace with Synonym:"), wxDefaultPosition, wxDefaultSize, 0 );
	staticText->Wrap( -1 );
	bSizer4->Add( staticText, 0, wxALL, 0 );

	m_TextCtrlReplaceSynonym = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_TextCtrlReplaceSynonym, 0, wxALL|wxEXPAND, 5 );

	m_ListBoxSynonyme = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	bSizer4->Add( m_ListBoxSynonyme, 1, wxALL|wxEXPAND, 5 );

	bSizer3->Add( bSizer4, 1, wxEXPAND, 5 );

	bSizer1->Add( bSizer3, 1, wxEXPAND, 5 );

	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1->Realize();
	bSizer1->Add( m_sdbSizer1, 0, wxALIGN_CENTER_HORIZONTAL, 5 );


    m_ChoiceLookedUp->Append(Word);
    m_ChoiceLookedUp->Select(0);
	std::map<wxString, std::vector< wxString > >::iterator it;
	for ( it = syn.begin() ; it != syn.end(); it++ )
        m_ListBoxMeanings->Append(it->first);
	m_ListBoxMeanings->Select(0);
	UpdateSynonyme();




	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );


	Connect(XRCID("m_listBoxMeanings"),wxEVT_COMMAND_LISTBOX_SELECTED,(wxObjectEventFunction)&ThesaurusDialog::OnMeaningsSelected);
	Connect(XRCID("m_listBoxSynonym"),wxEVT_COMMAND_LISTBOX_SELECTED,(wxObjectEventFunction)&ThesaurusDialog::OnSynonymeSelected);

	// Connect Events
	m_ListBoxMeanings->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( ThesaurusDialog::OnMeaningsSelected ), NULL, this );
	m_ListBoxSynonyme->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( ThesaurusDialog::OnSynonymeSelected ), NULL, this );
}

void ThesaurusDialog::UpdateSynonyme()
{
    wxString meaning = m_ListBoxMeanings->GetString(m_ListBoxMeanings->GetSelection());

    m_ListBoxSynonyme->Clear();

    std::vector< wxString > syns = m_syn[meaning];
    for ( unsigned int i = 0 ; i < syns.size() ; i++ )
        m_ListBoxSynonyme->Append(syns[i]);
    m_ListBoxSynonyme->Select(0);
    UpdateSelectedSynonym();

}
void ThesaurusDialog::UpdateSelectedSynonym()
{
    wxString str = m_ListBoxSynonyme->GetString( m_ListBoxSynonyme->GetSelection() );

    int pos = str.find( _T('(') );
    if ( pos != wxNOT_FOUND )
    {
        str = str.Mid(0, pos-1);
        str.Trim();
    }
    m_TextCtrlReplaceSynonym->SetValue(str);
}
ThesaurusDialog::~ThesaurusDialog()
{
	m_ListBoxMeanings->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( ThesaurusDialog::OnMeaningsSelected ), NULL, this );
	m_ListBoxSynonyme->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( ThesaurusDialog::OnSynonymeSelected ), NULL, this );
}

wxString ThesaurusDialog::GetSelection()
{
    return m_TextCtrlReplaceSynonym->GetValue();
}

void ThesaurusDialog::OnMeaningsSelected(wxCommandEvent& event)
{
    UpdateSynonyme();
}

void ThesaurusDialog::OnSynonymeSelected(wxCommandEvent& event)
{
    UpdateSelectedSynonym();
}

///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 17 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "ConfigDlg.h"

///////////////////////////////////////////////////////////////////////////

ConfigDlg::ConfigDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("BrowseTracker Options"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	m_staticText2->Wrap( -1 );
	bSizer3->Add( m_staticText2, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 5 );
	
	
	bSizer3->Add( 0, 10, 0, 0, 5 );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	Cfg_BrowseMarksEnabled = new wxCheckBox( this, wxID_ANY, wxT("Enable BrowseMarks"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer7->Add( Cfg_BrowseMarksEnabled, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	bSizer3->Add( bSizer7, 0, wxALIGN_CENTER_HORIZONTAL|wxEXPAND|wxSHAPED, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );
	
	wxString Cfg_MarkStyleChoices[] = { wxT("Browse_Marks  "), wxT("Book_Marks"), wxT("Hide") };
	int Cfg_MarkStyleNChoices = sizeof( Cfg_MarkStyleChoices ) / sizeof( wxString );
	Cfg_MarkStyle = new wxRadioBox( this, wxID_ANY, wxT("Mark style"), wxDefaultPosition, wxDefaultSize, Cfg_MarkStyleNChoices, Cfg_MarkStyleChoices, 3, wxRA_SPECIFY_COLS );
	Cfg_MarkStyle->SetSelection( 2 );
	bSizer6->Add( Cfg_MarkStyle, 1, wxALL, 5 );
	
	bSizer3->Add( bSizer6, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	wxString Cfg_ToggleKeyChoices[] = { wxT("Left_Mouse"), wxT("Ctrl-Left_Mouse") };
	int Cfg_ToggleKeyNChoices = sizeof( Cfg_ToggleKeyChoices ) / sizeof( wxString );
	Cfg_ToggleKey = new wxRadioBox( this, wxID_ANY, wxT("Toggle BrowseMark Key"), wxDefaultPosition, wxDefaultSize, Cfg_ToggleKeyNChoices, Cfg_ToggleKeyChoices, 3, wxRA_SPECIFY_COLS );
	Cfg_ToggleKey->SetSelection( 0 );
	bSizer8->Add( Cfg_ToggleKey, 0, wxALL|wxEXPAND, 5 );
	
	bSizer3->Add( bSizer8, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, wxT(" Left_Mouse Toggle Delay (Milliseconds)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	bSizer9->Add( m_staticText4, 0, wxALL|wxEXPAND, 5 );
	
	Cfg_LeftMouseDelay = new wxSlider( this, wxID_ANY, 200, 0, 1000, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS|wxSL_HORIZONTAL|wxSL_LABELS );
	bSizer9->Add( Cfg_LeftMouseDelay, 1, wxALL|wxEXPAND, 5 );
	
	bSizer3->Add( bSizer9, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );
	
	wxString Cfg_ClearAllKeyChoices[] = { wxT("Ctrl-Left_Mouse"), wxT("Ctrl-Left_DblClick") };
	int Cfg_ClearAllKeyNChoices = sizeof( Cfg_ClearAllKeyChoices ) / sizeof( wxString );
	Cfg_ClearAllKey = new wxRadioBox( this, wxID_ANY, wxT("Clear All BrowseMarks"), wxDefaultPosition, wxDefaultSize, Cfg_ClearAllKeyNChoices, Cfg_ClearAllKeyChoices, 2, wxRA_SPECIFY_COLS );
	Cfg_ClearAllKey->SetSelection( 1 );
	bSizer10->Add( Cfg_ClearAllKey, 1, wxALL|wxEXPAND, 5 );
	
	bSizer3->Add( bSizer10, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1->Realize();
	bSizer11->Add( m_sdbSizer1, 1, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	bSizer3->Add( bSizer11, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizer3 );
	this->Layout();
}

ConfigDlg::~ConfigDlg()
{
}

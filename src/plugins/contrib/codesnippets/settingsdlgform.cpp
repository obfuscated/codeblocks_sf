///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  1 2007)
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

#include "settingsdlgform.h"

///////////////////////////////////////////////////////////////////////////

SettingsDlgForm::SettingsDlgForm( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer;
	bSizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	m_staticText = new wxStaticText( this, ID_DEFAULT, wxT("User Settings"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	bSizer1->Add( m_staticText, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 5 );

	bSizer->Add( bSizer1, 0, wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer;
	fgSizer = new wxFlexGridSizer( 1, 3, 0, 0 );
	fgSizer->AddGrowableCol( 1 );
	fgSizer->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText1 = new wxStaticText( this, ID_DEFAULT, wxT("External Editor"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	fgSizer->Add( m_staticText1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_ExtEditorTextCtrl = new wxTextCtrl( this, ID_DEFAULT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer->Add( m_ExtEditorTextCtrl, 1, wxALL|wxEXPAND, 5 );

	m_ExtEditorButton = new wxButton( this, ID_EXTEDITORBUTTON, wxT(",,,"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	fgSizer->Add( m_ExtEditorButton, 0, wxALIGN_RIGHT|wxALL|wxSHAPED, 5 );

	bSizer->Add( fgSizer, 0, wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 1, 3, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText2 = new wxStaticText( this, ID_DEFAULT, wxT("Snippet Folder"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	fgSizer1->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_SnippetFileTextCtrl = new wxTextCtrl( this, ID_SNIPPETFILE, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_SnippetFileTextCtrl, 1, wxALL|wxEXPAND, 5 );

	m_SnippetFileButton = new wxButton( this, ID_SNIPPETFILEBUTTON, wxT("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	fgSizer1->Add( m_SnippetFileButton, 0, wxALIGN_RIGHT|wxALL|wxSHAPED, 5 );

	bSizer->Add( fgSizer1, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText7 = new wxStaticText( this, wxID_ANY, wxT("Window Type"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( m_staticText7, 0, wxALL, 5 );

	m_RadioFloatBtn = new wxRadioButton( this, wxID_ANY, wxT("Floating"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	bSizer10->Add( m_RadioFloatBtn, 4, wxALL, 5 );

	m_RadioDockBtn = new wxRadioButton( this, wxID_ANY, wxT("Docked"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer10->Add( m_RadioDockBtn, 4, wxALL, 5 );

	m_RadioExternalBtn = new wxRadioButton( this, wxID_ANY, wxT("External"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer10->Add( m_RadioExternalBtn, 4, wxALL, 5 );

	bSizer10->Add( 0, 0, 1, wxEXPAND, 0 );

	bSizer->Add( bSizer10, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxVERTICAL );

	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer12->Add( m_staticline1, 0, wxALL, 5 );

	bSizer->Add( bSizer12, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );

	m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("Adaptive Mouse Speed Sensitivity"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_staticText4, 0, wxALIGN_CENTER|wxALL, 5 );

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxHORIZONTAL );

	bSizer7->Add( 0, 0, 1, wxEXPAND, 0 );

	m_MouseSpeedSlider = new wxSlider( this, wxID_ANY, 8, 1, 10, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS );
	bSizer7->Add( m_MouseSpeedSlider, 3, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 5 );

	bSizer7->Add( 0, 0, 1, wxEXPAND, 0 );

	bSizer2->Add( bSizer7, 1, wxEXPAND, 5 );

	bSizer->Add( bSizer2, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );

	m_staticText5 = new wxStaticText( this, wxID_ANY, wxT("Mouse_Movement_to_Scroll_Ratio"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_staticText5, 0, wxALIGN_CENTER|wxALL, 5 );

	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxHORIZONTAL );

	bSizer13->Add( 0, 0, 1, wxEXPAND, 0 );

	m_MouseScrollSlider = new wxSlider( this, wxID_ANY, 30, 10, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS );
	bSizer13->Add( m_MouseScrollSlider, 3, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 5 );

	bSizer13->Add( 0, 0, 1, wxEXPAND, 0 );

	bSizer3->Add( bSizer13, 1, wxEXPAND, 5 );

	bSizer->Add( bSizer3, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );

	m_staticText6 = new wxStaticText( this, wxID_ANY, wxT("Context Menu Delay (millisec)"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_staticText6, 0, wxALIGN_CENTER|wxALL, 5 );

	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxHORIZONTAL );

	bSizer14->Add( 0, 0, 1, wxEXPAND, 0 );

	m_MouseDelaylider = new wxSlider( this, wxID_ANY, 192, 100, 1000, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS );
	bSizer14->Add( m_MouseDelaylider, 3, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 5 );

	bSizer14->Add( 0, 0, 1, wxEXPAND, 0 );

	bSizer4->Add( bSizer14, 1, wxEXPAND, 5 );

	bSizer->Add( bSizer4, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );

	m_sdbSizer = new wxStdDialogButtonSizer();
	m_sdbSizer->AddButton( new wxButton( this, wxID_OK ) );
	m_sdbSizer->AddButton( new wxButton( this, wxID_CANCEL ) );
	m_sdbSizer->Realize();
	bSizer5->Add( m_sdbSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

	bSizer->Add( bSizer5, 0, wxEXPAND, 5 );

	this->SetSizer( bSizer );
	this->Layout();
}

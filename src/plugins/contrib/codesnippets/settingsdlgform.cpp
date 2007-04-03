

///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jul 24 2006)
// http://wxformbuilder.sourceforge.net/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifdef WX_PRECOMP
    #include "wx_pch.h"
#else
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include <wx/sizer.h>

#include "settingsdlgform.h"


///////////////////////////////////////////////////////////////////////////

SettingsDlgForm::SettingsDlgForm( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style ) : wxDialog( parent, id, title, pos, size, style )
{
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
	fgSizer->SetFlexibleDirection( wxVERTICAL );
	fgSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText1 = new wxStaticText( this, ID_DEFAULT, wxT("External Editor"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	fgSizer->Add( m_staticText1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_ExtEditorTextCtrl = new wxTextCtrl( this, ID_DEFAULT, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer->Add( m_ExtEditorTextCtrl, 1, wxALL|wxEXPAND, 5 );

	m_ExtEditorButton = new wxButton( this, ID_EXTEDITORBUTTON, wxT("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	fgSizer->Add( m_ExtEditorButton, 0, wxALIGN_CENTER|wxALL, 5 );

	bSizer->Add( fgSizer, 0, wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 1, 3, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxVERTICAL );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText2 = new wxStaticText( this, ID_DEFAULT, wxT("Snippets File"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	fgSizer1->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_SnippetFileTextCtrl = new wxTextCtrl( this, ID_SNIPPETFILE, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_SnippetFileTextCtrl, 1, wxALL|wxEXPAND, 5 );

	m_SnippetFileButton = new wxButton( this, ID_SNIPPETFILEBUTTON, wxT("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	fgSizer1->Add( m_SnippetFileButton, 0, wxALIGN_CENTER|wxALL, 5 );

	bSizer->Add( fgSizer1, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );

	bSizer->Add( bSizer2, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );

	m_sdbSizer = new wxStdDialogButtonSizer();
	m_sdbSizer->AddButton( new wxButton( this, wxID_OK ) );
	m_sdbSizer->AddButton( new wxButton( this, wxID_CANCEL ) );
	m_sdbSizer->Realize();
	bSizer3->Add( m_sdbSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

	bSizer->Add( bSizer3, 0, wxEXPAND, 5 );

	this->SetSizer( bSizer );
	this->Layout();
}

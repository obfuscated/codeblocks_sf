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

#include "settingsdlgform.h"

///////////////////////////////////////////////////////////////////////////

SettingsDlgForm::SettingsDlgForm( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxScrollingDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer;
	bSizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	m_staticText = new wxStaticText( this, ID_DEFAULT, wxT("User Settings"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	m_staticText->Wrap( -1 );
	bSizer1->Add( m_staticText, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 5 );

	bSizer->Add( bSizer1, 0, wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer;
	fgSizer = new wxFlexGridSizer( 1, 3, 0, 0 );
	fgSizer->AddGrowableCol( 1 );
	fgSizer->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText1 = new wxStaticText( this, ID_DEFAULT, wxT("External Editor"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	m_staticText1->Wrap( -1 );
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
	m_staticText2->Wrap( -1 );
	fgSizer1->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_SnippetFileTextCtrl = new wxTextCtrl( this, ID_SNIPPETFILE, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_SnippetFileTextCtrl, 1, wxALL|wxEXPAND, 5 );

	m_SnippetFileButton = new wxButton( this, ID_SNIPPETFILEBUTTON, wxT("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	fgSizer1->Add( m_SnippetFileButton, 0, wxALIGN_RIGHT|wxALL|wxSHAPED, 5 );

	bSizer->Add( fgSizer1, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText7 = new wxStaticText( this, wxID_ANY, wxT("Window Type"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	bSizer10->Add( m_staticText7, 0, wxALL, 5 );

	m_RadioFloatBtn = new wxRadioButton( this, wxID_ANY, wxT("Floating"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	bSizer10->Add( m_RadioFloatBtn, 4, wxALL, 5 );

	m_RadioDockBtn = new wxRadioButton( this, wxID_ANY, wxT("Docked"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( m_RadioDockBtn, 4, wxALL, 5 );

	m_RadioExternalBtn = new wxRadioButton( this, wxID_ANY, wxT("External"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( m_RadioExternalBtn, 4, wxALL, 5 );


	bSizer10->Add( 0, 0, 1, wxEXPAND, 0 );

	bSizer->Add( bSizer10, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer51;
	bSizer51 = new wxBoxSizer( wxHORIZONTAL );

	m_EditorsStayOnTopChkBox = new wxCheckBox( this, wxID_ANY, wxT("Editors Stay On Top"), wxDefaultPosition, wxDefaultSize, 0 );
	m_EditorsStayOnTopChkBox->SetValue(true);

	bSizer51->Add( m_EditorsStayOnTopChkBox, 0, wxALL, 5 );

	m_ToolTipsChkBox = new wxCheckBox( this, wxID_ANY, wxT("ToolTips"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ToolTipsChkBox->SetValue(true);
	
	bSizer51->Add( m_ToolTipsChkBox, 0, wxALL, 5 );
	
	bSizer->Add( bSizer51, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );

	m_CfgFolderStaticText = new wxStaticText( this, wxID_ANY, wxT("CfgFolder"), wxDefaultPosition, wxDefaultSize, 0 );
	m_CfgFolderStaticText->Wrap( -1 );
	bSizer6->Add( m_CfgFolderStaticText, 0, wxALL, 5 );

	m_CfgFolderTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_CfgFolderTextCtrl->Enable( false );

	bSizer6->Add( m_CfgFolderTextCtrl, 1, wxALL, 5 );

	bSizer->Add( bSizer6, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer61;
	bSizer61 = new wxBoxSizer( wxHORIZONTAL );

	m_IniFolderStaticText = new wxStaticText( this, wxID_ANY, wxT("IniFolder "), wxDefaultPosition, wxDefaultSize, 0 );
	m_IniFolderStaticText->Wrap( -1 );
	bSizer61->Add( m_IniFolderStaticText, 0, wxALL, 5 );

	m_IniFolderTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_IniFolderTextCtrl->Enable( false );

	bSizer61->Add( m_IniFolderTextCtrl, 1, wxALL, 5 );

	bSizer->Add( bSizer61, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );

	m_sdbSizer = new wxStdDialogButtonSizer();
	m_sdbSizerOK = new wxButton( this, wxID_OK );
	m_sdbSizer->AddButton( m_sdbSizerOK );
	m_sdbSizerCancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer->AddButton( m_sdbSizerCancel );
	m_sdbSizer->Realize();
	bSizer5->Add( m_sdbSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

	bSizer->Add( bSizer5, 0, wxEXPAND, 5 );

	this->SetSizer( bSizer );
	this->Layout();
}

SettingsDlgForm::~SettingsDlgForm()
{
}

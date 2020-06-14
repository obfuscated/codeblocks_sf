
#include <wx/richtext/richtextctrl.h>

#include "configmanager.h"
#include "MouseSapCfg.h"

BEGIN_EVENT_TABLE(cbMouseSapCfg,cbConfigurationPanel)
//	//(*EventTable(cbMouseSapCfg)
//	EVT_BUTTON(ID_DONEBUTTON,cbMouseSapCfg::OnDoneButtonClick)
//	//*)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
///              MS WINDOWS ONLY
#if defined( __WXMSW__ ) //<------------ Windows ---------------------
// ----------------------------------------------------------------------------
cbMouseSapCfg::cbMouseSapCfg(wxWindow* parent, MouseSap* pOwner, wxWindowID /*id*/)
// ----------------------------------------------------------------------------
    :pOwnerClass(pOwner)
{
    cbConfigurationPanel::Create(parent, -1, wxDefaultPosition, wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );

	bSizer2->SetMinSize(wxSize( -1,50 ));
	StaticText1 = new wxStaticText( this, wxID_ANY, _("MouseSap Configuration"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( StaticText1, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

	bSizer2->Add( 0, 0, 1, wxEXPAND, 0 );

	bSizer1->Add( bSizer2, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	m_pluginEnabled = new wxCheckBox( this, wxID_ANY, _("MouseSap Enabled"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_pluginEnabled, 0, wxALL, 5 );
	bSizer1->Add( bSizer5, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	wxString docString1 = wxT("This plugin adds editor functions using the shiftKey, ctrlKey and  middleMouse button.\n\n")
    wxT("If selected text & shift-middleMouse, \t\tpaste selected text at current cursor position.\n")
    wxT("If selected text & shift-middleMouse "); //insert bold "inside" below

    wxString docString2 = wxT(" selection, copy to clipboard(like ctrl-c).\n")
    wxT("If selected text & shift-switch editor, \t\tcopy selection to clipboard.\n")
    wxT("If selected text & shift-ctrl-middleMouse "); //insert bold "inside" below

    wxString docString3 = wxT(" selection, overwrite selection with clipboard data.\n")
    wxT("If no selected text & shift-middleMouse,\t\tpaste clipboard data at cursor position(like ctrl-v).");

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxHORIZONTAL );
    int xsize; int ysize;
    parent->GetClientSize(&xsize, &ysize);
    wxRichTextCtrl* richTextCtrl = new wxRichTextCtrl(
        this, wxID_ANY, wxEmptyString, wxDefaultPosition,
        wxSize(xsize-(xsize/8), ysize-(ysize/2)), wxVSCROLL | wxHSCROLL | wxBORDER_NONE | wxWANTS_CHARS);
    wxFont textFont = wxFont(12, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    wxFont boldFont = wxFont(12, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    wxFont italicFont = wxFont(12, wxFONTFAMILY_ROMAN, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL);
    wxFont font(12, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    richTextCtrl->SetFont(font);
    wxRichTextCtrl& r = *richTextCtrl;
    r.BeginSuppressUndo();
    //r.BeginParagraphSpacing(0, 20);
    r.BeginParagraphSpacing(0, 0);
    //r.BeginAlignment(wxTEXT_ALIGNMENT_CENTRE);
    r.BeginAlignment(wxTEXT_ALIGNMENT_LEFT);

    r.WriteText(docString1);
    r.BeginBold();
    r.BeginFontSize(12);
    r.WriteText(wxT("inside"));
    r.EndFontSize();
    r.EndBold();

    r.WriteText(docString2);
    r.BeginBold();
    r.BeginFontSize(12);
    r.WriteText(wxT("inside"));
    r.EndFontSize();
    r.EndBold();

    r.WriteText(docString3);
    r.Enable(false);

	bSizer7->Add( richTextCtrl, 0, wxALL, 5 );
	bSizer1->Add( bSizer7, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();
}
#endif //defined( __WXMSW__ )

// ----------------------------------------------------------------------------
///                  LINUX ONLY
#if !defined( __WXMSW__ ) //<-------------- must be linux which may not have wxRichTextCtrl
// ----------------------------------------------------------------------------
cbMouseSapCfg::cbMouseSapCfg(wxWindow* parent, MouseSap* pOwner, wxWindowID /*id*/)
// ----------------------------------------------------------------------------
    :pOwnerClass(pOwner)
{
    cbConfigurationPanel::Create(parent, -1, wxDefaultPosition, wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );

	bSizer2->SetMinSize(wxSize( -1,50 ));
	StaticText1 = new wxStaticText( this, wxID_ANY, _("MouseSap Configuration"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( StaticText1, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

	bSizer2->Add( 0, 0, 1, wxEXPAND, 0 );

	bSizer1->Add( bSizer2, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	m_pluginEnabled = new wxCheckBox( this, wxID_ANY, _("MouseSap Enabled"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_pluginEnabled, 0, wxALL, 5 );
	bSizer1->Add( bSizer5, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );
	wxString docString = wxT("This plugin adds editor functions using the shiftKey, ctrlKey and  middleMouse button.\n\n")
	wxT("If selected text && shift-middleMouse,                  paste selected text at current cursor position.\n")
	wxT("If selected text && shift-middleMouse inside selection,  copy to clipboard(like ctrl-c).\n")
	wxT("If selected text && shift-switch editor,                 copy selection to clipboard.\n")
    wxT("If selected text && shift-ctrl-middleMouse inside selection,  overwrite selection with clipboard data.\n")
    wxT("If no selected text && Shift-middleMouse,             paste clipboard data at cursor position(like ctrl-v).");

    wxStaticText* staticText1 = new wxStaticText(this, wxID_ANY,
                                   docString,
                                   wxDefaultPosition, wxDefaultSize, 0);
	bSizer6->Add( staticText1, 0, wxALL, 5 );
	bSizer1->Add( bSizer6, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();

}
#endif //NOT defined (__WXMSW__ )

// ----------------------------------------------------------------------------
void cbMouseSapCfg::OnApply()
// ----------------------------------------------------------------------------
{
    pOwnerClass->OnDialogDone(this);
}
// ----------------------------------------------------------------------------
cbMouseSapCfg::~cbMouseSapCfg()
// ----------------------------------------------------------------------------
{
}
// ----------------------------------------------------------------------------
void cbMouseSapCfg::OnDoneButtonClick(wxCommandEvent& /*event*/)
// ----------------------------------------------------------------------------
{
    //EndModal(0);
}
// ----------------------------------------------------------------------------
wxString cbMouseSapCfg::GetBitmapBaseName() const
{
    //probing
    //LOGIT( _T("Config:%s"),ConfigManager::GetConfigFolder().GetData()  );
    //LOGIT( _T("Plugins:%s"),ConfigManager::GetPluginsFolder().GetData() );
    //LOGIT( _T("Data:%s"),ConfigManager::GetDataFolder().GetData() );
    //LOGIT( _T("Executable:%s"),ConfigManager::GetExecutableFolder().GetData() );

    wxString pngName = _T("generic-plugin");
    //if file exist "./share/codeblocks/images/settings/cbMouseSap.png";
    #ifdef __WXGTK__
     if ( ::wxFileExists(ConfigManager::GetDataFolder() + _T("/images/settings/MouseSap.png")) )
    #else
     if ( ::wxFileExists(ConfigManager::GetDataFolder() + _T("\\images\\settings\\MouseSap.png")) )
    #endif
    	pngName = _T("MouseSap") ;
    // else return "generic-plugin"
    return pngName;
}
// ----------------------------------------------------------------------------
//

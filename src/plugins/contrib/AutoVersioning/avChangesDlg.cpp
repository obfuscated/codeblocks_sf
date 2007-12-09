#include "avChangesDlg.h"

//(*InternalHeaders(avChangesDlg)
#include <wx/bitmap.h>
#include <wx/font.h>
#include <wx/fontenum.h>
#include <wx/fontmap.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/string.h>
//*)

//(*IdInit(avChangesDlg)
const long avChangesDlg::ID_CHANGES_STATICTEXT = wxNewId();
const long avChangesDlg::ID_CHANGES_TEXTCTRL = wxNewId();
const long avChangesDlg::ID_ACCEPT_BUTTON = wxNewId();
//*)

BEGIN_EVENT_TABLE(avChangesDlg,wxDialog)
	//(*EventTable(avChangesDlg)
	//*)
END_EVENT_TABLE()

avChangesDlg::avChangesDlg(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(avChangesDlg)
	Create(parent, id, _("AutoVersioning :: Changes Log"), wxDefaultPosition, wxDefaultSize, wxCAPTION|wxRESIZE_BORDER, _T("id"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	lblChanges = new wxStaticText(this, ID_CHANGES_STATICTEXT, _("Enter every change followed by an enter except the last one. \nLeave it in blank if you plan to write the changes later."), wxDefaultPosition, wxDefaultSize, 0, _T("ID_CHANGES_STATICTEXT"));
	BoxSizer1->Add(lblChanges, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	txtChanges = new wxTextCtrl(this, ID_CHANGES_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxSize(500,250), wxTE_MULTILINE|wxHSCROLL|wxVSCROLL, wxDefaultValidator, _T("ID_CHANGES_TEXTCTRL"));
	BoxSizer1->Add(txtChanges, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	btnAccept = new wxButton(this, ID_ACCEPT_BUTTON, _("Accept"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_ACCEPT_BUTTON"));
	btnAccept->SetDefault();
	BoxSizer1->Add(btnAccept, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	Connect(ID_ACCEPT_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&avChangesDlg::OnBtnAcceptClick);
	//*)
}

avChangesDlg::~avChangesDlg()
{
	//(*Destroy(avChangesDlg)
	//*)
}

//{Event Functions
void avChangesDlg::OnBtnAcceptClick(wxCommandEvent& event)
{
    m_changes = txtChanges->GetValue();
    EndModal(0);
}
//}

//{Const Properties
const wxString avChangesDlg::Changes(){
    return m_changes;
}
//}

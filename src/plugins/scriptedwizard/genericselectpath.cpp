#include <sdk.h>
#include "genericselectpath.h"

BEGIN_EVENT_TABLE(GenericSelectPath,wxPanel)
	//(*EventTable(GenericSelectPath)
	//*)
END_EVENT_TABLE()

GenericSelectPath::GenericSelectPath(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(GenericSelectPath)
	Create(parent,id,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL);
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	lblDescr = new wxStaticText(this,ID_STATICTEXT1,_("Please select the location of XXX\non your computer. This is the top-level folder where\nXXX is installed."),wxDefaultPosition,wxDefaultSize,wxST_NO_AUTORESIZE);
	lblLabel = new wxStaticText(this,ID_STATICTEXT2,_("Location of XXX:"),wxDefaultPosition,wxDefaultSize,0);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	txtFolder = new wxTextCtrl(this,ID_TEXTCTRL1,_("Text"),wxDefaultPosition,wxDefaultSize,0);
	if ( 0 ) txtFolder->SetMaxLength(0);
	btnBrowse = new wxButton(this,ID_BUTTON1,_("..."),wxDefaultPosition,wxSize(22,22),0);
	if (false) btnBrowse->SetDefault();
	BoxSizer2->Add(txtFolder,1,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	BoxSizer2->Add(btnBrowse,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,0);
	BoxSizer1->Add(lblDescr,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	BoxSizer1->Add(lblLabel,0,wxLEFT|wxRIGHT|wxTOP|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	BoxSizer1->Add(BoxSizer2,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	this->SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	//*)

	// Set the control's name so that calls to wxWindow::FindWindowByName() succeed...
	txtFolder->SetName(_T("txtFolder"));
}

GenericSelectPath::~GenericSelectPath()
{
}

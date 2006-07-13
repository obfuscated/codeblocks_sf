#include "infopanel.h"

BEGIN_EVENT_TABLE(InfoPanel,wxPanel)
	//(*EventTable(InfoPanel)
	//*)
END_EVENT_TABLE()

InfoPanel::InfoPanel(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(InfoPanel)
	Create(parent,id,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL,_T(""));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	lblIntro = new wxStaticText(this,ID_STATICTEXT1,_("Welcome to the new console application wizard!\n\n\n\n\n\n\n\n\n\n\n\n\n\n"),wxDefaultPosition,wxDefaultSize,wxST_NO_AUTORESIZE,_("ID_STATICTEXT1"));
	chkSkip = new wxCheckBox(this,ID_CHECKBOX1,_("Skip this page next time"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_CHECKBOX1"));
	chkSkip->SetValue(false);
	BoxSizer1->Add(lblIntro,1,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	BoxSizer1->Add(chkSkip,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	this->SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	//*)
}

InfoPanel::~InfoPanel()
{
}


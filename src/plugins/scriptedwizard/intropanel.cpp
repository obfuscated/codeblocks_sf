#include "intropanel.h"

BEGIN_EVENT_TABLE(IntroPanel,wxPanel)
	//(*EventTable(IntroPanel)
	//*)
END_EVENT_TABLE()

IntroPanel::IntroPanel(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(IntroPanel)
	Create(parent,id,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL);
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	lblIntro = new wxStaticText(this,ID_STATICTEXT1,_("Welcome to the new console application wizard!\n\n\n\n\n\n\n\n\n\n\n\n\n\n"),wxDefaultPosition,wxDefaultSize,wxST_NO_AUTORESIZE);
	BoxSizer1->Add(lblIntro,1,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	this->SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	//*)
}

IntroPanel::~IntroPanel()
{
}


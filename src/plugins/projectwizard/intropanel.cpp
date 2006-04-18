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
	lblIntro = new wxStaticText(this,ID_STATICTEXT1,_("Welcome to the new console application wizard!\n\nThis wizard will guide you to create a new console\napplication.\n\nWhen you \'re ready to proceed, please click \"Next\"..."),wxDefaultPosition,wxDefaultSize,0);
	BoxSizer1->Add(lblIntro,1,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	this->SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	//*)
}

IntroPanel::~IntroPanel()
{
}


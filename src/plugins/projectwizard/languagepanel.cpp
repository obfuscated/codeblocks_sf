#include "languagepanel.h"

BEGIN_EVENT_TABLE(LanguagePanel,wxPanel)
	//(*EventTable(LanguagePanel)
	//*)
END_EVENT_TABLE()

LanguagePanel::LanguagePanel(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(LanguagePanel)
	wxBoxSizer* BoxSizer1;
	wxStaticText* StaticText1;
	wxStaticBoxSizer* StaticBoxSizer1;
	
	Create(parent,id,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL);
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	StaticText1 = new wxStaticText(this,ID_STATICTEXT1,_("The files for this project are available for the following\nlanguages. Please choose one from the list:"),wxDefaultPosition,wxDefaultSize,0);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL,this,_("Language"));
	lstLanguages = new wxListBox(this,ID_LISTBOX1,wxDefaultPosition,wxDefaultSize,0,0,0);
	lstLanguages->SetSelection(-1);
	StaticBoxSizer1->Add(lstLanguages,1,wxALL|wxALIGN_CENTER|wxEXPAND,4);
	BoxSizer1->Add(StaticText1,1,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	BoxSizer1->Add(StaticBoxSizer1,0,wxALL|wxALIGN_CENTER,8);
	this->SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	//*)
}

LanguagePanel::~LanguagePanel()
{
}

void LanguagePanel::SetChoices(const wxArrayString& langs, int defLang)
{
    lstLanguages->Clear();
    for (size_t i = 0; i < langs.GetCount(); ++i)
    {
        lstLanguages->Append(langs[i]);
    }
    lstLanguages->SetSelection(defLang);
}

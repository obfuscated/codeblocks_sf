#include "genericsinglechoicelist.h"

BEGIN_EVENT_TABLE(GenericSingleChoiceList,wxPanel)
	//(*EventTable(GenericSingleChoiceList)
	//*)
END_EVENT_TABLE()

GenericSingleChoiceList::GenericSingleChoiceList(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(GenericSingleChoiceList)
	wxBoxSizer* BoxSizer1;
	wxStaticBoxSizer* StaticBoxSizer1;

	Create(parent,id,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL,_T(""));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	lblDescr = new wxStaticText(this,ID_STATICTEXT1,_("Description"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT1"));
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL,this,_("Please make a selection"));
/* FIXME (Biplab#1#): This is a hack. Need to check this against newer wx for a fix */
    if (platform::linux && wxMinimumVersion<2, 8>::eval)
        GenericChoiceList = new wxListBox(this,ID_LISTBOX1,wxDefaultPosition,wxSize(100, 100),0,0,0,wxDefaultValidator,_("ID_LISTBOX1"));
    else
        GenericChoiceList = new wxListBox(this,ID_LISTBOX1,wxDefaultPosition,wxDefaultSize,0,0,0,wxDefaultValidator,_("ID_LISTBOX1"));
	GenericChoiceList->SetSelection(-1);
	StaticBoxSizer1->Add(GenericChoiceList,1,wxALL|wxALIGN_CENTER|wxEXPAND,4);
	BoxSizer1->Add(lblDescr,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	BoxSizer1->Add(StaticBoxSizer1,0,wxALL|wxALIGN_CENTER|wxEXPAND,8);
	this->SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	//*)

	GenericChoiceList->SetName(_T("GenericChoiceList"));
}

GenericSingleChoiceList::~GenericSingleChoiceList()
{
}

void GenericSingleChoiceList::SetChoices(const wxArrayString& choices, int defChoice)
{
    GenericChoiceList->Clear();
    for (size_t i = 0; i < choices.GetCount(); ++i)
    {
        GenericChoiceList->Append(choices[i]);
    }
    GenericChoiceList->SetSelection(defChoice);
}

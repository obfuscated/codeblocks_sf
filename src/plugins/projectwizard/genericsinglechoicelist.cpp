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
	lstSelections = new wxListBox(this,ID_LISTBOX1,wxDefaultPosition,wxDefaultSize,0,0,0,wxDefaultValidator,_("ID_LISTBOX1"));
	lstSelections->SetSelection(-1);
	StaticBoxSizer1->Add(lstSelections,1,wxALL|wxALIGN_CENTER|wxEXPAND,4);
	BoxSizer1->Add(lblDescr,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	BoxSizer1->Add(StaticBoxSizer1,0,wxALL|wxALIGN_CENTER|wxEXPAND,8);
	this->SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	//*)

	lstSelections->SetName(_T("lstSelections"));
}

GenericSingleChoiceList::~GenericSingleChoiceList()
{
}

void GenericSingleChoiceList::SetChoices(const wxArrayString& choices, int defChoice)
{
    lstSelections->Clear();
    for (size_t i = 0; i < choices.GetCount(); ++i)
    {
        lstSelections->Append(choices[i]);
    }
    lstSelections->SetSelection(defChoice);
}

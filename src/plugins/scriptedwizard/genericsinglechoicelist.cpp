/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#ifndef CB_PRECOMP
    #include <wx/arrstr.h>
    //(*InternalHeadersPCH(GenericSingleChoiceList)
    #include <wx/string.h>
    #include <wx/intl.h>
    //*)
#endif // CB_PRECOMP

#include "genericsinglechoicelist.h"


//(*IdInit(GenericSingleChoiceList)
const long GenericSingleChoiceList::ID_STATICTEXT1 = wxNewId();
const long GenericSingleChoiceList::ID_LISTBOX1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(GenericSingleChoiceList,wxPanel)
	//(*EventTable(GenericSingleChoiceList)
	//*)
END_EVENT_TABLE()

GenericSingleChoiceList::GenericSingleChoiceList(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(GenericSingleChoiceList)
	wxBoxSizer* BoxSizer1;
	wxStaticBoxSizer* StaticBoxSizer1;

	Create(parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("id"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	lblDescr = new wxStaticText(this, ID_STATICTEXT1, _("Description"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	BoxSizer1->Add(lblDescr, 0, wxALL|wxEXPAND, 8);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Please make a selection"));
	GenericChoiceList = new wxListBox(this, ID_LISTBOX1, wxDefaultPosition, wxSize(232,131), 0, 0, wxLB_SINGLE|wxLB_HSCROLL, wxDefaultValidator, _T("ID_LISTBOX1"));
	StaticBoxSizer1->Add(GenericChoiceList, 1, wxALL|wxEXPAND, 4);
	BoxSizer1->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND|wxFIXED_MINSIZE, 8);
	SetSizer(BoxSizer1);
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

    if (static_cast<size_t>(defChoice) < choices.GetCount())
        GenericChoiceList->SetSelection(defChoice);
    else if (choices.GetCount() > 0)
        GenericChoiceList->SetSelection(0);
}

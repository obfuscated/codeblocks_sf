#ifndef GENERICSINGLECHOICELIST_H
#define GENERICSINGLECHOICELIST_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//(*Headers(GenericSingleChoiceList)
#include <wx/intl.h>
#include <wx/listbox.h>
#include <wx/panel.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
//*)

#include <wx/dynarray.h>

class GenericSingleChoiceList: public wxPanel
{
	public:

		GenericSingleChoiceList(wxWindow* parent,wxWindowID id = -1);
		virtual ~GenericSingleChoiceList();

        void SetChoices(const wxArrayString& choices, int defChoice);
        int GetChoice() const { return GenericChoiceList->GetSelection(); }
        void SetChoice(int choice){ GenericChoiceList->SetSelection(choice); }
        void SetDescription(const wxString& descr)
        {
            lblDescr->SetLabel(descr);

            GetSizer()->Fit(this);
            GetSizer()->SetSizeHints(this);
        }

		//(*Identifiers(GenericSingleChoiceList)
		enum Identifiers
		{
		    ID_LISTBOX1 = 0x1000,
		    ID_STATICTEXT1
		};
		//*)

	protected:

		//(*Handlers(GenericSingleChoiceList)
		//*)

		//(*Declarations(GenericSingleChoiceList)
		wxStaticText* lblDescr;
		wxListBox* GenericChoiceList;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif

/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef GENERICSINGLECHOICELIST_H
#define GENERICSINGLECHOICELIST_H

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//(*Headers(GenericSingleChoiceList)
#include <wx/listbox.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
//*)

class wxArrayString;
class wxString;

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
		static const long ID_STATICTEXT1;
		static const long ID_LISTBOX1;
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

/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef GENERICSINGLECHOICELIST_H
#define GENERICSINGLECHOICELIST_H


//(*HeadersPCH(GenericSingleChoiceList)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/listbox.h>
#include <wx/panel.h>
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

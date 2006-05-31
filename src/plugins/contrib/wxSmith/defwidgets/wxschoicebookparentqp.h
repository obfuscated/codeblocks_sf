#ifndef WXSCHOICEBOOKPARENTQP_H
#define WXSCHOICEBOOKPARENTQP_H

#include "wxschoicebook.h"

//(*Headers(wxsChoicebookParentQP)
#include <wx/checkbox.h>
#include <wx/intl.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

class wxsChoicebookParentQP: public wxsQPPPanel
{
	public:

		wxsChoicebookParentQP(wxWindow* parent,wxsWidget* Modified,wxsChoicebookExtraParams* Params,wxWindowID id = -1);
		virtual ~wxsChoicebookParentQP();

		//(*Identifiers(wxsChoicebookParentQP)
		enum Identifiers
		{
		    ID_CHECKBOX1 = 0x1000,
		    ID_STATICTEXT1,
		    ID_TEXTCTRL1
		};
		//*)

	protected:

		//(*Handlers(wxsChoicebookParentQP)
		void OnLabelText(wxCommandEvent& event);
        void OnSelectionChange(wxCommandEvent& event);
        void OnReadTimer(wxTimerEvent& event);
        void OnWriteTimer(wxTimerEvent& event);
		//*)

		//(*Declarations(wxsChoicebookParentQP)
		wxFlexGridSizer* FlexGridSizer1;
		wxStaticText* StaticText1;
		wxStaticBoxSizer* StaticBoxSizer1;
		wxTextCtrl* Label;
		wxStaticBoxSizer* StaticBoxSizer2;
		wxCheckBox* Selection;
		//*)

    private:

        wxTimer WriteTimer;
        wxTimer ReadTimer;

        void ReadData();
        void SaveData();

        wxsWidget* Widget;
        wxsChoicebookExtraParams* Params;

		DECLARE_EVENT_TABLE()
};

#endif

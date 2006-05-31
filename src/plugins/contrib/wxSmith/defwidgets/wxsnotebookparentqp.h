#ifndef WXSNOTEBOOKPARENTQP_H
#define WXSNOTEBOOKPARENTQP_H

//(*Headers(wxsNotebookParentQP)
#include <wx/checkbox.h>
#include <wx/intl.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

#include "wxsnotebook.h"

class wxsNotebookParentQP: public wxsQPPPanel
{
	public:

		wxsNotebookParentQP(wxWindow* parent,wxsWidget* Modified,wxsNotebookExtraParams* Params,wxWindowID id = -1);
		virtual ~wxsNotebookParentQP();

		//(*Identifiers(wxsNotebookParentQP)
        enum Identifiers
        {
            ID_CHECKBOX1 = 0x1000,
            ID_STATICTEXT1,
            ID_TEXTCTRL1
        };
        //*)

	protected:

		//(*Handlers(wxsNotebookParentQP)
		void OnLabelText(wxCommandEvent& event);
        void OnSelectionChange(wxCommandEvent& event);
        void OnReadTimer(wxTimerEvent& event);
        void OnWriteTimer(wxTimerEvent& event);
        //*)

		//(*Declarations(wxsNotebookParentQP)
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
        wxsNotebookExtraParams* Params;

		DECLARE_EVENT_TABLE()
};

#endif

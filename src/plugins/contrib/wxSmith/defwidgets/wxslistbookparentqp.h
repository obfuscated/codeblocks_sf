#ifndef WXSLISTBOOKPARENTQP_H
#define WXSLISTBOOKPARENTQP_H

//(*Headers(wxsListbookParentQP)
#include <wx/checkbox.h>
#include <wx/intl.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

#include "wxslistbook.h"

class wxsListbookParentQP: public wxsQPPPanel
{
	public:

		wxsListbookParentQP(wxWindow* parent,wxsWidget* Modified,wxsListbookExtraParams* Params,wxWindowID id = -1);
		virtual ~wxsListbookParentQP();

		//(*Identifiers(wxsListbookParentQP)
        enum Identifiers
        {
            ID_CHECKBOX1 = 0x1000,
            ID_STATICTEXT1,
            ID_TEXTCTRL1
        };
        //*)

	protected:

		//(*Handlers(wxsListbookParentQP)
		void OnLabelText(wxCommandEvent& event);
        void OnSelectionChange(wxCommandEvent& event);
        void OnReadTimer(wxTimerEvent& event);
        void OnWriteTimer(wxTimerEvent& event);
        //*)

		//(*Declarations(wxsListbookParentQP)
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
        wxsListbookExtraParams* Params;

		DECLARE_EVENT_TABLE()
};

#endif

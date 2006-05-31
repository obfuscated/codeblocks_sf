#ifndef WXSSTANDARDQP_H
#define WXSSTANDARDQP_H

//(*Headers(wxsStandardQP)
#include <wx/checkbox.h>
#include <wx/intl.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)
#include <wx/timer.h>
#include "widget.h"

class wxsStandardQP: public wxsQPPPanel
{
	public:

		wxsStandardQP(wxWindow* parent,wxsWidget* Widget,wxWindowID id = -1);
		virtual ~wxsStandardQP();

		//(*Identifiers(wxsStandardQP)
        enum Identifiers
        {
            ID_CHECKBOX1 = 0x1000,
            ID_CHECKBOX2,
            ID_CHECKBOX3,
            ID_CHECKBOX4,
            ID_STATICTEXT1,
            ID_TEXTCTRL1,
            ID_TEXTCTRL2
        };
        //*)

	protected:

		//(*Handlers(wxsStandardQP)
		void OnTextChanged(wxCommandEvent& event);
        void OnChange(wxCommandEvent& event);
        void OnReadTimer(wxTimerEvent& event);
        void OnWriteTimer(wxTimerEvent& event);
        //*)

		//(*Declarations(wxsStandardQP)
        wxFlexGridSizer* MainSizer;
        wxStaticText* StaticText1;
        wxStaticBoxSizer* VariableSizer;
        wxTextCtrl* VarName;
        wxCheckBox* IsMember;
        wxStaticBoxSizer* IdentifierSizer;
        wxTextCtrl* Ident;
        wxStaticBoxSizer* FlagsSizer;
        wxCheckBox* Enabled;
        wxCheckBox* Focused;
        wxCheckBox* Hdden;
        //*)

	private:

        wxTimer ReadTimer;
        wxTimer WriteTimer;

        void ReadData();
        void SaveData();

        wxsWidget* Widget;

		DECLARE_EVENT_TABLE()
};

#endif

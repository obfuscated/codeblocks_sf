#ifndef WXSSIZERPARENTQP_H
#define WXSSIZERPARENTQP_H

#include "wxsglobals.h"

//(*Headers(wxsSizerParentQP)
#include <wx/checkbox.h>
#include <wx/intl.h>
#include <wx/panel.h>
#include <wx/radiobut.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
//*)
#include <wx/timer.h>
#include "widget.h"

class wxsSizerExtraParams;

class wxsSizerParentQP: public wxsQPPPanel
{
	public:

		wxsSizerParentQP(wxWindow* parent,wxsWidget* Modified,wxsSizerExtraParams* Params,wxWindowID id = -1);
		virtual ~wxsSizerParentQP();

		//(*Identifiers(wxsSizerParentQP)
        enum Identifiers
        {
            ID_CHECKBOX1 = 0x1000,
            ID_CHECKBOX2,
            ID_CHECKBOX3,
            ID_CHECKBOX4,
            ID_CHECKBOX5,
            ID_CHECKBOX6,
            ID_RADIOBUTTON10,
            ID_RADIOBUTTON11,
            ID_RADIOBUTTON12,
            ID_RADIOBUTTON4,
            ID_RADIOBUTTON5,
            ID_RADIOBUTTON6,
            ID_RADIOBUTTON7,
            ID_RADIOBUTTON8,
            ID_RADIOBUTTON9,
            ID_SPINCTRL1,
            ID_SPINCTRL2,
            ID_STATICTEXT1
        };
        //*)

	protected:

		//(*Handlers(wxsSizerParentQP)
		void OnBrdChange(wxCommandEvent& event);
        void OnBrdSizeChange(wxSpinEvent& event);
        void OnPlaceChange(wxCommandEvent& event);
        void OnTimer(wxTimerEvent& event);
        void OnProportionChange(wxSpinEvent& event);
        //*)

		//(*Declarations(wxsSizerParentQP)
        wxFlexGridSizer* FlexGridSizer1;
        wxStaticText* StaticText1;
        wxCheckBox* BrdTop;
        wxCheckBox* BrdLeft;
        wxCheckBox* BrdRight;
        wxCheckBox* BrdBottom;
        wxSpinCtrl* BrdSize;
        wxFlexGridSizer* FlexGridSizer3;
        wxGridSizer* GridSizer2;
        wxRadioButton* PlaceLT;
        wxRadioButton* PlaceCT;
        wxRadioButton* PlaceRT;
        wxRadioButton* PlaceLC;
        wxRadioButton* PlaceCC;
        wxRadioButton* PlaceRC;
        wxRadioButton* PlaceLB;
        wxRadioButton* PlaceCB;
        wxRadioButton* PlaceRB;
        wxBoxSizer* BoxSizer1;
        wxCheckBox* PlaceShp;
        wxCheckBox* PlaceExp;
        wxStaticBoxSizer* StaticBoxSizer3;
        wxSpinCtrl* Proportion;
        //*)

	private:

        void ReadData();
        void SaveData();
        inline int GetBorderFlags();
        inline int GetBorderSize();
        inline int GetPlacement();
        inline bool GetExpand();
        inline bool GetShaped();
        inline int GetProportion();

        wxsWidget* Widget;
        wxsSizerExtraParams* Params;
        wxTimer Timer;

		DECLARE_EVENT_TABLE()
};

#endif

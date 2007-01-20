#ifndef WXSSIZERPARENTQP_H
#define WXSSIZERPARENTQP_H

//(*Headers(wxsSizerParentQP)
#include <wx/checkbox.h>
#include <wx/intl.h>
#include <wx/panel.h>
#include <wx/radiobut.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
//*)

#include "wxssizer.h"
#include "../wxsadvqppchild.h"

class wxsSizerParentQP: public wxsAdvQPPChild
{
	public:

		wxsSizerParentQP(wxsAdvQPP* parent,wxsSizerExtra* Extra,wxWindowID id = -1);

		//(*Identifiers(wxsSizerParentQP)
		enum Identifiers
		{
		    ID_CHECKBOX1 = 0x1000,
		    ID_CHECKBOX2,
		    ID_CHECKBOX3,
		    ID_CHECKBOX4,
		    ID_SPINCTRL1,
		    ID_CHECKBOX7,
		    ID_RADIOBUTTON4,
		    ID_RADIOBUTTON5,
		    ID_RADIOBUTTON6,
		    ID_RADIOBUTTON7,
		    ID_RADIOBUTTON8,
		    ID_RADIOBUTTON9,
		    ID_RADIOBUTTON10,
		    ID_RADIOBUTTON11,
		    ID_RADIOBUTTON12,
		    ID_CHECKBOX6,
		    ID_CHECKBOX5,
		    ID_SPINCTRL2
		};
		//*)

	protected:

		//(*Handlers(wxsSizerParentQP)
		void OnBrdChange(wxCommandEvent& event);
		void OnBrdSizeChange(wxSpinEvent& event);
		void OnPlaceChange(wxCommandEvent& event);
		void OnProportionChange(wxSpinEvent& event);
		void OnBrdDlgChange(wxCommandEvent& event);
		//*)

		//(*Declarations(wxsSizerParentQP)
		wxFlexGridSizer* FlexGridSizer1;
		wxCheckBox* BrdTop;
		wxCheckBox* BrdLeft;
		wxCheckBox* BrdRight;
		wxCheckBox* BrdBottom;
		wxSpinCtrl* BrdSize;
		wxCheckBox* BrdDlg;
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

        virtual void Update();

	private:

        void ReadData();
        void SaveData();

        wxsSizerExtra* m_Extra;

		DECLARE_EVENT_TABLE()
};

#endif

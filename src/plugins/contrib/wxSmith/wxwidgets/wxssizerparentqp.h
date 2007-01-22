#ifndef WXSSIZERPARENTQP_H
#define WXSSIZERPARENTQP_H

//(*Headers(wxsSizerParentQP)
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/radiobut.h>
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
		static const long ID_CHECKBOX1;
		static const long ID_CHECKBOX2;
		static const long ID_CHECKBOX3;
		static const long ID_CHECKBOX4;
		static const long ID_SPINCTRL1;
		static const long ID_CHECKBOX7;
		static const long ID_RADIOBUTTON4;
		static const long ID_RADIOBUTTON5;
		static const long ID_RADIOBUTTON6;
		static const long ID_RADIOBUTTON7;
		static const long ID_RADIOBUTTON8;
		static const long ID_RADIOBUTTON9;
		static const long ID_RADIOBUTTON10;
		static const long ID_RADIOBUTTON11;
		static const long ID_RADIOBUTTON12;
		static const long ID_CHECKBOX6;
		static const long ID_CHECKBOX5;
		static const long ID_SPINCTRL2;
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

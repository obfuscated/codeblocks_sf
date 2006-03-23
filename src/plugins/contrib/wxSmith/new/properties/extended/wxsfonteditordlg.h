#ifndef WXSFONTEDITORDLG_H
#define WXSFONTEDITORDLG_H

#include "wxsfontproperty.h"

//(*Headers(wxsFontEditorDlg)
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/combobox.h>
#include <wx/dialog.h>
#include <wx/intl.h>
#include <wx/listbox.h>
#include <wx/radiobut.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

class wxsFontEditorDlg: public wxDialog
{
	public:

		wxsFontEditorDlg(wxWindow* parent,wxsFontData& Data,wxWindowID id = -1);
		virtual ~wxsFontEditorDlg();

		//(*Identifiers(wxsFontEditorDlg)
		enum Identifiers
		{
		    ID_BUTTON1 = 0x1000,
		    ID_BUTTON4,
		    ID_BUTTON5,
		    ID_BUTTON6,
		    ID_BUTTON7,
		    ID_BUTTON8,
		    ID_CHECKBOX1,
		    ID_CHECKBOX2,
		    ID_CHECKBOX3,
		    ID_CHECKBOX4,
		    ID_CHECKBOX5,
		    ID_CHECKBOX6,
		    ID_CHECKBOX7,
		    ID_CHECKBOX8,
		    ID_CHOICE1,
		    ID_CHOICE2,
		    ID_CHOICE3,
		    ID_COMBOBOX1,
		    ID_LISTBOX1,
		    ID_OK,
		    ID_RADIOBUTTON1,
		    ID_RADIOBUTTON2,
		    ID_RADIOBUTTON3,
		    ID_RADIOBUTTON4,
		    ID_RADIOBUTTON5,
		    ID_RADIOBUTTON6,
		    ID_RADIOBUTTON7,
		    ID_RADIOBUTTON8,
		    ID_SPINCTRL1,
		    ID_STATICTEXT1,
		    ID_TEXTCTRL1,
		    ID_TEXTCTRL2
		};
		//*)

	protected:

		//(*Handlers(wxsFontEditorDlg)
		void OnButton1Click(wxCommandEvent& event);
		void OnUpdateContent(wxCommandEvent& event);
		void OnButton2Click(wxCommandEvent& event);
		void OnButton1Click1(wxCommandEvent& event);
		void OnFaceAddClick(wxCommandEvent& event);
		void OnFaceDelClick(wxCommandEvent& event);
		void OnFaceEditClick(wxCommandEvent& event);
		void OnFaceUpClick(wxCommandEvent& event);
		void OnFaceDownClick(wxCommandEvent& event);
		void OnUpdatePreview(wxCommandEvent& event);
		void OnSizeValChange(wxSpinEvent& event);
		void OnBaseFontUseChange(wxCommandEvent& event);
		//*)

		//(*Declarations(wxsFontEditorDlg)
		wxBoxSizer* BoxSizer1;
		wxBoxSizer* BoxSizer4;
		wxBoxSizer* BoxSizer5;
		wxStaticBoxSizer* StaticBoxSizer1;
		wxChoice* FontType;
		wxStaticBoxSizer* StaticBoxSizer2;
		wxFlexGridSizer* FlexGridSizer2;
		wxBoxSizer* BaseFontSizer;
		wxCheckBox* BaseFontUse;
		wxStaticText* BaseFontTxt;
		wxChoice* BaseFontVal;
		wxCheckBox* FamUse;
		wxComboBox* FamVal;
		wxCheckBox* EncodUse;
		wxChoice* EncodVal;
		wxCheckBox* SizeUse;
		wxSpinCtrl* SizeVal;
		wxCheckBox* RelSizeUse;
		wxTextCtrl* RelSizeVal;
		wxBoxSizer* BoxSizer2;
		wxStaticBoxSizer* StaticBoxSizer3;
		wxCheckBox* StyleUse;
		wxRadioButton* StyleNorm;
		wxRadioButton* StyleItal;
		wxRadioButton* StyleSlant;
		wxStaticBoxSizer* StaticBoxSizer4;
		wxCheckBox* WeightUse;
		wxRadioButton* WeightLight;
		wxRadioButton* WeightNorm;
		wxRadioButton* WeightBold;
		wxStaticBoxSizer* StaticBoxSizer5;
		wxCheckBox* UnderUse;
		wxRadioButton* UnderYes;
		wxRadioButton* UnderNo;
		wxBoxSizer* BoxSizer6;
		wxStaticBoxSizer* StaticBoxSizer6;
		wxListBox* FaceList;
		wxBoxSizer* BoxSizer3;
		wxButton* FaceAdd;
		wxButton* FaceDel;
		wxButton* FaceEdit;
		wxButton* FaceUp;
		wxButton* FaceDown;
		wxStaticBoxSizer* StaticBoxSizer7;
		wxTextCtrl* TestArea;
		wxFlexGridSizer* FlexGridSizer1;
		wxButton* Button1;
		wxButton* Button2;
		wxButton* Button3;
		//*)

	private:
	
        void UpdateContent();
        void UpdatePreview();
        void ReadData(wxsFontData& Data);
        void StoreData(wxsFontData& Data);
	
        wxsFontData& Data;
        wxArrayString Encodings;
        bool Initialized;

		DECLARE_EVENT_TABLE()
};

#endif

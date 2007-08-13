#ifndef WXSSETTINGS_H
#define WXSSETTINGS_H

#include <wx/intl.h>
#include <configurationpanel.h>

//(*Headers(wxsSettings)
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/combobox.h>
#include <wx/panel.h>
#include <wx/radiobut.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
//*)

class wxsSettings: public cbConfigurationPanel
{
	public:

		wxsSettings(wxWindow* parent,wxWindowID id = -1);
		virtual ~wxsSettings();

	private:

		//(*Identifiers(wxsSettings)
		static const long ID_STATICTEXT2;
		static const long ID_COMBOBOX1;
		static const long ID_STATICTEXT3;
		static const long ID_BUTTON1;
		static const long ID_STATICTEXT4;
		static const long ID_BUTTON2;
		static const long ID_CHECKBOX7;
		static const long ID_SPINCTRL1;
		static const long ID_STATICTEXT1;
		static const long ID_CHECKBOX9;
		static const long ID_STATICTEXT5;
		static const long ID_RADIOBUTTON1;
		static const long ID_RADIOBUTTON2;
		static const long ID_STATICTEXT6;
		static const long ID_RADIOBUTTON3;
		static const long ID_RADIOBUTTON4;
		static const long ID_STATICTEXT7;
		static const long ID_SPINCTRL2;
		static const long ID_STATICTEXT8;
		static const long ID_CHECKBOX1;
		static const long ID_CHECKBOX2;
		static const long ID_CHECKBOX3;
		static const long ID_CHECKBOX4;
		static const long ID_STATICTEXT9;
		static const long ID_CHECKBOX5;
		static const long ID_STATICTEXT10;
		static const long ID_CHECKBOX6;
		static const long ID_STATICTEXT12;
		static const long ID_CHOICE1;
		static const long ID_STATICTEXT13;
		static const long ID_SPINCTRL3;
		static const long ID_CHECKBOX8;
		//*)

		//(*Handlers(wxsSettings)
		void OnDragTargetColClick(wxCommandEvent& event);
		void OnDragParentColClick(wxCommandEvent& event);
		void OnUseGridClick(wxCommandEvent& event);
		//*)

		//(*Declarations(wxsSettings)
		wxFlexGridSizer* FlexGridSizer1;
		wxFlexGridSizer* FlexGridSizer6;
		wxStaticBoxSizer* StaticBoxSizer1;
		wxFlexGridSizer* FlexGridSizer2;
		wxComboBox* m_DragAssistType;
		wxButton* m_DragTargetCol;
		wxButton* m_DragParentCol;
		wxCheckBox* m_UseGrid;
		wxSpinCtrl* m_GridSize;
		wxStaticText* StaticText1;
		wxCheckBox* m_Continous;
		wxFlexGridSizer* FlexGridSizer3;
		wxRadioButton* m_Icons16;
		wxRadioButton* m_Icons32;
		wxStaticText* StaticText6;
		wxFlexGridSizer* FlexGridSizer4;
		wxRadioButton* m_TIcons16;
		wxRadioButton* m_TIcons32;
		wxStaticBoxSizer* StaticBoxSizer2;
		wxFlexGridSizer* FlexGridSizer5;
		wxStaticText* StaticText7;
		wxSpinCtrl* spinProportion;
		wxStaticText* StaticText8;
		wxBoxSizer* BoxSizer2;
		wxCheckBox* chkTop;
		wxCheckBox* chkBottom;
		wxCheckBox* chkLeft;
		wxCheckBox* chkRight;
		wxStaticText* StaticText9;
		wxCheckBox* chkExpand;
		wxStaticText* StaticText10;
		wxCheckBox* chkShaped;
		wxStaticText* StaticText12;
		wxChoice* choicePlacement;
		wxStaticText* StaticText13;
		wxSpinCtrl* spinBorder;
		wxCheckBox* chkBorderDU;
		//*)

        wxString GetTitle() const { return _("wxSmith settings"); }
        wxString GetBitmapBaseName() const { return _T("wxsmith"); }

        void OnApply();
        void OnCancel(){}

		DECLARE_EVENT_TABLE()
};

#endif

#ifndef WXSMENUEDITOR_H
#define WXSMENUEDITOR_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//(*Headers(wxsMenuEditor)
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/radiobut.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/treectrl.h>
//*)

class wxsMenuEditor: public wxPanel
{
	public:

		wxsMenuEditor(wxWindow* parent,wxWindowID id = -1);
		virtual ~wxsMenuEditor();

		//(*Identifiers(wxsMenuEditor)
		static const long ID_TREECTRL1;
		static const long ID_RADIOBUTTON1;
		static const long ID_RADIOBUTTON4;
		static const long ID_RADIOBUTTON5;
		static const long ID_RADIOBUTTON2;
		static const long ID_RADIOBUTTON3;
		static const long ID_STATICLINE1;
		static const long ID_STATICTEXT6;
		static const long ID_TEXTCTRL4;
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL1;
		static const long ID_STATICTEXT2;
		static const long ID_TEXTCTRL2;
		static const long ID_STATICTEXT3;
		static const long ID_TEXTCTRL3;
		static const long ID_STATICTEXT4;
		static const long ID_CHECKBOX1;
		static const long ID_STATICTEXT5;
		static const long ID_CHECKBOX2;
		//*)

	protected:

		//(*Handlers(wxsMenuEditor)
		//*)

		//(*Declarations(wxsMenuEditor)
		wxBoxSizer*  BoxSizer1;
		wxStaticBoxSizer*  StaticBoxSizer1;
		wxTreeCtrl*  TreeCtrl1;
		wxStaticBoxSizer*  StaticBoxSizer2;
		wxGridSizer*  GridSizer1;
		wxRadioButton*  RadioButton1;
		wxRadioButton*  RadioButton4;
		wxRadioButton*  RadioButton5;
		wxRadioButton*  RadioButton2;
		wxRadioButton*  RadioButton3;
		wxStaticLine*  StaticLine1;
		wxFlexGridSizer*  FlexGridSizer1;
		wxStaticText*  StaticText6;
		wxTextCtrl*  TextCtrl4;
		wxStaticText*  StaticText1;
		wxTextCtrl*  TextCtrl1;
		wxStaticText*  StaticText2;
		wxTextCtrl*  TextCtrl2;
		wxStaticText*  StaticText3;
		wxTextCtrl*  TextCtrl3;
		wxStaticText*  StaticText4;
		wxCheckBox*  CheckBox1;
		wxStaticText*  StaticText5;
		wxCheckBox*  CheckBox2;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif

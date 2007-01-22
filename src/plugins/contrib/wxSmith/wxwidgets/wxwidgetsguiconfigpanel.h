#ifndef WXWIDGETSGUICONFIGPANEL_H
#define WXWIDGETSGUICONFIGPANEL_H

//(*Headers(wxWidgetsGUIConfigPanel)
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

#include "wxwidgetsgui.h"

/** \brief Configuration panel for wxWidgetsGUI class */
class wxWidgetsGUIConfigPanel: public cbConfigurationPanel
{
	public:

		wxWidgetsGUIConfigPanel(wxWindow* parent,wxWidgetsGUI* GUI);
		virtual ~wxWidgetsGUIConfigPanel();

		//(*Identifiers(wxWidgetsGUIConfigPanel)
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL1;
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		static const long ID_COMBOBOX1;
		static const long ID_STATICTEXT2;
		static const long ID_CHECKBOX2;
		static const long ID_CHECKBOX1;
		//*)

	private:

        virtual wxString GetTitle() const;
        virtual wxString GetBitmapBaseName() const;
        virtual void OnApply();
        virtual void OnCancel();

		//(*Handlers(wxWidgetsGUIConfigPanel)
		void OnInitAllChange(wxCommandEvent& event);
		void OnButton1Click(wxCommandEvent& event);
		void OnButton2Click(wxCommandEvent& event);
		//*)

		//(*Declarations(wxWidgetsGUIConfigPanel)
		wxBoxSizer* BoxSizer1;
		wxBoxSizer* BoxSizer2;
		wxStaticBoxSizer* StaticBoxSizer1;
		wxStaticText* StaticText1;
		wxTextCtrl* AutoLoad;
		wxBoxSizer* BoxSizer3;
		wxButton* Button1;
		wxButton* Button2;
		wxBoxSizer* BoxSizer4;
		wxStaticBoxSizer* StaticBoxSizer2;
		wxFlexGridSizer* FlexGridSizer1;
		wxComboBox* MainRes;
		wxStaticText* StaticText2;
		wxStaticBoxSizer* StaticBoxSizer3;
		wxBoxSizer* BoxSizer5;
		wxCheckBox* InitAll;
		wxCheckBox* InitAllNecessary;
		//*)

        wxWidgetsGUI* m_GUI;

		DECLARE_EVENT_TABLE()
};

#endif

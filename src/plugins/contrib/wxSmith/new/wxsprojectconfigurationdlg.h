/*

    WARNING !!!
   =============

   This file is going to be removed soon

*/


/*#ifndef WXSPROJECTCONFIGURATIONDLG_H
#define WXSPROJECTCONFIGURATIONDLG_H

#include "wxsglobals.h"
#include "wxsprojectconfig.h"

//(*Headers(wxsProjectConfigurationDlg)
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/dialog.h>
#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

class wxsProject;

class wxsProjectConfigurationDlg: public wxDialog
{
	public:
		wxsProjectConfigurationDlg(wxWindow* parent,wxsProject* Project,wxWindowID id = -1);
		virtual ~wxsProjectConfigurationDlg();

		//(*Identifiers(wxsProjectConfigurationDlg)
		enum Identifiers
		{
		    ID_BUTTON1 = 0x1000,
		    ID_BUTTON2,
		    ID_BUTTON3,
		    ID_BUTTON4,
		    ID_CHECKBOX1,
		    ID_CHECKBOX2,
		    ID_COMBOBOX1,
		    ID_STATICTEXT1,
		    ID_STATICTEXT2,
		    ID_TEXTCTRL1
		};
		//*)

	protected:

		//(*Handlers(wxsProjectConfigurationDlg)
		void OnInitAllChange(wxCommandEvent& event);
		void OnButton4Click(wxCommandEvent& event);
		void OnButton3Click(wxCommandEvent& event);
		void OnButton1Click(wxCommandEvent& event);
		void OnButton2Click(wxCommandEvent& event);
		//*)

		//(*Declarations(wxsProjectConfigurationDlg)
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
		wxBoxSizer* BoxSizer6;
		wxButton* Button3;
		wxButton* Button4;
		//*)

	private:

        wxsProject* Project;

		DECLARE_EVENT_TABLE()
};

#endif
*/

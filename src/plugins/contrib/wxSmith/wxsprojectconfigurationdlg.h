#ifndef WXSPROJECTCONFIGURATIONDLG_H
#define WXSPROJECTCONFIGURATIONDLG_H

//(*Headers(wxsProjectConfigurationDlg)
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/dialog.h>
#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/xrc/xmlres.h>
//*)

#include "wxsglobals.h"

class WXSCLASS wxsProject;

class wxsProjectConfigurationDlg: public wxDialog
{
	public:

		wxsProjectConfigurationDlg(wxWindow* parent,wxsProject* Project,wxWindowID id = -1);
		virtual ~wxsProjectConfigurationDlg();

		//(*Identifiers(wxsProjectConfigurationDlg)
		//*)

	protected:

		//(*Handlers(wxsProjectConfigurationDlg)
		void OnInit(wxInitDialogEvent& event);
		void OnInitAllChange(wxCommandEvent& event);
		void OnButton4Click(wxCommandEvent& event);
		void OnButton3Click(wxCommandEvent& event);
		void OnButton1Click(wxCommandEvent& event);
		void OnButton2Click(wxCommandEvent& event);
		//*)

		//(*Declarations(wxsProjectConfigurationDlg)
		wxStaticText* StaticText1;
		wxTextCtrl* AutoLoad;
		wxButton* Button1;
		wxButton* Button2;
		wxComboBox* MainRes;
		wxStaticText* StaticText2;
		wxCheckBox* InitAll;
		wxCheckBox* InitAllNecessary;
		wxButton* Button3;
		wxButton* Button4;
		//*)

	private:

        wxsProject* Project;

		DECLARE_EVENT_TABLE()
};

#endif

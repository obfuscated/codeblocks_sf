#ifndef DLGEDITPROJECTGLOB_H
#define DLGEDITPROJECTGLOB_H

//(*Headers(EditProjectGlobsDlg)
#include <wx/bmpbuttn.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/gbsizer.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

#include "cbProject.h"


class EditProjectGlobsDlg: public wxDialog
{
	public:

		EditProjectGlobsDlg(cbProject::Glob &glob, wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~EditProjectGlobsDlg();

		//(*Declarations(EditProjectGlobsDlg)
		wxBitmapButton* btnBrowse;
		wxBitmapButton* btnOther;
		wxCheckBox* chkRecursive;
		wxTextCtrl* txtPath;
		wxTextCtrl* txtWildcart;
		//*)

		cbProject::Glob GetGlob();

	protected:

		//(*Identifiers(EditProjectGlobsDlg)
		static const long ID_TEXTCTRL1;
		static const long ID_BITMAPBUTTON1;
		static const long ID_BITMAPBUTTON2;
		static const long ID_CHECKBOX1;
		static const long ID_TEXTCTRL2;
		//*)

	private:

		//(*Handlers(EditProjectGlobsDlg)
		void OnBrowseClick(wxCommandEvent& event);
		void OnOtherClick(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif

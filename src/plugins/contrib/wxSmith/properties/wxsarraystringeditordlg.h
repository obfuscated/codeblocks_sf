#ifndef WXSARRAYSTRINGEDITORDLG_H
#define WXSARRAYSTRINGEDITORDLG_H

//(*Headers(wxsArrayStringEditorDlg)
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
//*)

class wxsArrayStringEditorDlg: public wxDialog
{
	public:

		wxsArrayStringEditorDlg(wxWindow* parent,wxArrayString& Array,wxWindowID id = -1);
		virtual ~wxsArrayStringEditorDlg();

		//(*Identifiers(wxsArrayStringEditorDlg)
		static const long ID_TEXTCTRL1;
		//*)

	protected:

		//(*Handlers(wxsArrayStringEditorDlg)
		void OnOK(wxCommandEvent& event);
		//*)

		//(*Declarations(wxsArrayStringEditorDlg)
		wxTextCtrl* Items;
		//*)

	private:
	
        wxArrayString& Data;

		DECLARE_EVENT_TABLE()
};

#endif

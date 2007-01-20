#ifndef WXSARRAYSTRINGEDITORDLG_H
#define WXSARRAYSTRINGEDITORDLG_H

//(*Headers(wxsArrayStringEditorDlg)
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
//*)

class wxsArrayStringEditorDlg: public wxDialog
{
	public:

		wxsArrayStringEditorDlg(wxWindow* parent,wxArrayString& Array,wxWindowID id = -1);
		virtual ~wxsArrayStringEditorDlg();

		//(*Identifiers(wxsArrayStringEditorDlg)
		enum Identifiers
		{
		    ID_TEXTCTRL1 = 0x1000
		};
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

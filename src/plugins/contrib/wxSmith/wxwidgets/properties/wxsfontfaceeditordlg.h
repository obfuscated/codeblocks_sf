#ifndef WXSFONTFACEEDITORDLG_H
#define WXSFONTFACEEDITORDLG_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//(*Headers(wxsFontFaceEditorDlg)
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
//*)

class wxsFontFaceEditorDlg: public wxDialog
{
	public:

		wxsFontFaceEditorDlg(wxWindow* parent,wxString& Face,wxWindowID id = -1);
		virtual ~wxsFontFaceEditorDlg();

		//(*Identifiers(wxsFontFaceEditorDlg)
		enum Identifiers
		{
		    ID_BUTTON1 = 0x1000,
		    ID_TEXTCTRL1
		};
		//*)

	protected:

		//(*Handlers(wxsFontFaceEditorDlg)
		void OnButton2Click(wxCommandEvent& event);
		void OnButton1Click(wxCommandEvent& event);
		//*)

		//(*Declarations(wxsFontFaceEditorDlg)
		wxBoxSizer* BoxSizer1;
		wxStaticBoxSizer* StaticBoxSizer1;
		wxTextCtrl* FaceName;
		wxButton* Button1;
		wxBoxSizer* BoxSizer2;
		wxButton* Button2;
		wxButton* Button3;
		//*)

	private:
	
        wxString& Face;

		DECLARE_EVENT_TABLE()
};

#endif

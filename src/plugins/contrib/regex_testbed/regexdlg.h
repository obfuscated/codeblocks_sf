#ifndef REGEXDLG_H
#define REGEXDLG_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//(*Headers(RegExDlg)
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/xrc/xmlres.h>
//*)

class RegExDlg: public wxDialog
{
	public:

		RegExDlg(wxWindow* parent,wxWindowID id = -1);
		virtual ~RegExDlg();

		//(*Identifiers(RegExDlg)
		//*)

		void EndModal(int retCode);

	protected:

		//(*Handlers(RegExDlg)
		void OnTestClick(wxCommandEvent& event);
		void OnInit(wxInitDialogEvent& event);
		void OnRegExItemActivated(wxListEvent& event);
		//*)

		//(*Declarations(RegExDlg)
		wxTextCtrl* txtRegEx;
		wxTextCtrl* txtInput;
		wxButton* btnTest;
		wxButton* btnCancel;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif

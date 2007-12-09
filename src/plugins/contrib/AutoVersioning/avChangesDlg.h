#ifndef AVCHANGESDLG_H
#define AVCHANGESDLG_H

//(*Headers(avChangesDlg)
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

class avChangesDlg: public wxDialog
{
	public:

        const wxString Changes();

		avChangesDlg(wxWindow* parent,wxWindowID id=wxID_ANY);
		virtual ~avChangesDlg();

		//(*Identifiers(avChangesDlg)
		static const long ID_CHANGES_STATICTEXT;
		static const long ID_CHANGES_TEXTCTRL;
		static const long ID_ACCEPT_BUTTON;
		//*)

	protected:

		//(*Handlers(avChangesDlg)
		void OnBtnAcceptClick(wxCommandEvent& event);
		//*)

		//(*Declarations(avChangesDlg)
		wxBoxSizer* BoxSizer1;
		wxStaticText* lblChanges;
		wxTextCtrl* txtChanges;
		wxButton* btnAccept;
		//*)

	private:

        //Members
        wxString m_changes;

        DECLARE_EVENT_TABLE()
};

#endif

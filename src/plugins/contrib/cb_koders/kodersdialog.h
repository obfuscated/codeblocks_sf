#ifndef KODERSDIALOG_H
#define KODERSDIALOG_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//(*Headers(KodersDialog)
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/dialog.h>
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

class KodersDialog: public wxDialog
{
	public:

		KodersDialog(wxWindow* parent,wxWindowID id = -1);
		virtual ~KodersDialog();

    wxString GetSearch();
    wxString GetLanguage();
    wxString GetLicense();

		//(*Identifiers(KodersDialog)
		enum Identifiers
		{
		  ID_LBL_INTRO = 0x1000,
		  ID_TXT_SEARCH,
		  ID_BTN_SEARCH,
		  ID_LBL_FILTER,
		  ID_CBO_LANGUAGES,
		  ID_CBO_LICENSES
		};
		//*)

	protected:

		//(*Handlers(KodersDialog)
		void OnBtnSearchClick(wxCommandEvent& event);
		//*)

		//(*Declarations(KodersDialog)
		wxBoxSizer* bszMain;
		wxBoxSizer* bszIntro;
		wxStaticText* lblIntro;
		wxBoxSizer* bszSearch;
		wxTextCtrl* txtSearch;
		wxButton* btnSearch;
		wxBoxSizer* bszFilter;
		wxStaticText* lblFilter;
		wxComboBox* cboLanguages;
		wxComboBox* cboLicenses;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif

#ifndef KODERSDIALOG_H
#define KODERSDIALOG_H

#include <wx/wxprec.h>

#ifndef CB_PRECOMP
//(*Headers(KodersDialog)
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/dialog.h>
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)
#endif

class KodersDialog: public wxDialog
{
	public:

		KodersDialog(wxWindow* parent,wxWindowID id = -1);
		virtual ~KodersDialog();

		wxString GetSearch() const;
		wxString GetLanguage() const;
		wxString GetLicense() const;

		//(*Identifiers(KodersDialog)
		enum Identifiers
		{
		  ID_LBL_INTRO = 0x1000,
		  ID_TXT_SEARCH,
		  ID_BTN_SEARCH,
		  ID_LBL_FILTER,
		  ID_CHO_LANGUAGES,
		  ID_CHO_LICENSES
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
		wxChoice* choLanguages;
		wxChoice* choLicenses;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif // KODERSDIALOG_H

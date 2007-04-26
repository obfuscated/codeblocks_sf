#ifndef KODERSDIALOG_H
#define KODERSDIALOG_H

#include <wx/wxprec.h>

//(*Headers(KodersDialog)
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

class KodersDialog: public wxDialog
{
	public:

		KodersDialog(wxWindow* parent,wxWindowID id = -1);
		virtual ~KodersDialog();

		void     SetSearch(const wxString &search);
		wxString GetSearch() const;
		wxString GetLanguage() const;
		wxString GetLicense() const;

		//(*Identifiers(KodersDialog)
		static const long ID_LBL_INTRO;
		static const long ID_TXT_SEARCH;
		static const long ID_BTN_SEARCH;
		static const long ID_LBL_FILTER;
		static const long ID_CHO_LANGUAGES;
		static const long ID_CHO_LICENSES;
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

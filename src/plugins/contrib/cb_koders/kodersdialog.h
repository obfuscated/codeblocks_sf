#ifndef KODERSDIALOG_H
#define KODERSDIALOG_H


#include <wx/dialog.h>

class wxBoxSizer;
class wxButton;
class wxChoice;
class wxStaticText;
class wxTextCtrl;

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
		wxChoice* cboLanguages;
		wxChoice* cboLicenses;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif // KODERSDIALOG_H

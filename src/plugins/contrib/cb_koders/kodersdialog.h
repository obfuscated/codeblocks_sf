/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef KODERSDIALOG_H
#define KODERSDIALOG_H

#include <wx/wxprec.h>

//(*Headers(KodersDialog)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/button.h>
#include "scrollingdialog.h"
//*)

class KodersDialog: public wxScrollingDialog
{
	public:

		KodersDialog(wxWindow* parent,wxWindowID id = -1);
		virtual ~KodersDialog();

		void     SetSearch(const wxString &search);
		wxString GetSearch() const;
		wxString GetLanguage() const;

		//(*Identifiers(KodersDialog)
		static const long ID_LBL_INTRO;
		static const long ID_TXT_SEARCH;
		static const long ID_BTN_SEARCH;
		static const long ID_LBL_FILTER;
		static const long ID_CHO_LANGUAGES;
		//*)

	protected:

		//(*Handlers(KodersDialog)
		void OnBtnSearchClick(wxCommandEvent& event);
		//*)

		//(*Declarations(KodersDialog)
		wxBoxSizer* bszFilter;
		wxBoxSizer* bszMain;
		wxChoice* choLanguages;
		wxButton* btnSearch;
		wxBoxSizer* bszIntro;
		wxTextCtrl* txtSearch;
		wxBoxSizer* bszSearch;
		wxStaticText* lblFilter;
		wxStaticText* lblIntro;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif // KODERSDIALOG_H

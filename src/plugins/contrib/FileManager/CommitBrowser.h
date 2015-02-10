#ifndef COMMITBROWSER_H
#define COMMITBROWSER_H

#include <map>

//(*Headers(CommitBrowser)
#include <wx/dialog.h>
#include <wx/listctrl.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <wx/datectrl.h>
//*)

#include "FileExplorerUpdater.h"

class CommitBrowser: public wxDialog
{
	public:
		CommitBrowser(wxWindow* parent, const wxString& repo_path, const wxString &repo_type);
		virtual ~CommitBrowser();
        wxString GetSelectedCommit();
        wxString GetRepoBranch();

		//(*Declarations(CommitBrowser)
		wxButton* Button1;
		wxButton* Button2;
		wxStaticText* StaticText1;
		wxChoice* Choice1;
		wxListCtrl* CommitList;
        wxButton* ButtonMore;
        wxStaticText* CommitStatus;
		wxTextCtrl* TextCtrl1;
        wxCheckBox* CheckStartCommit;
        wxTextCtrl* StartCommit;
        wxCheckBox* CheckEndCommit;
        wxTextCtrl* EndCommit;
        wxCheckBox* CheckAfterDate;
        wxDatePickerCtrl* AfterDate;
        wxCheckBox* CheckBeforeDate;
        wxDatePickerCtrl* BeforeDate;
        wxTextCtrl* Grep;
		//*)
		long m_autofetch_count;
		CommitUpdater *m_updater;
		CommitUpdater *m_updater_commits;
		wxString m_update_queue;
		wxString m_update_commits_queue;
		wxString m_repo_path;
		wxString m_repo_type;

	protected:

		//(*Identifiers(CommitBrowser)
		//*)

	private:
        std::map<wxString, long> m_rev_fetch_amt;
		//(*Handlers(CommitBrowser)
		void OnButtonMore(wxCommandEvent& event);
		void OnButton2Click(wxCommandEvent& event);
		void OnButton1Click(wxCommandEvent& event);
		//*)
        void OnSearch(wxCommandEvent& event);
        void OnCheckCommitAfterDate(wxCommandEvent &event);
        void OnCheckCommitBeforeDate(wxCommandEvent &event);
        void OnCheckCommitStart(wxCommandEvent &event);
        void OnCheckCommitEnd(wxCommandEvent &event);
        void OnUpdateComplete(wxCommandEvent &event);
        void OnCommitsUpdateComplete(wxCommandEvent &event);
        void OnBranchSelected(wxCommandEvent &event);
        void OnListItemSelected(wxListEvent &event);

        void CommitsUpdaterQueue(const wxString &cmd);
        CommitUpdaterOptions GetCommitOptions();

		DECLARE_EVENT_TABLE()
};

#endif

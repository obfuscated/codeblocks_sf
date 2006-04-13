#ifndef FINDDLG_H
#define FINDDLG_H

#include "findreplacebase.h"

class FindDlg : public FindReplaceBase
{
	public:
		FindDlg(wxWindow* parent, const wxString& initial = wxEmptyString, bool hasSelection = false, bool findInFilesOnly = false);
		~FindDlg();
		wxString GetFindString();
		wxString GetReplaceString(){ return wxEmptyString; }
		bool IsFindInFiles();
        bool GetDeleteOldSearches();
		bool GetMatchWord();
		bool GetStartWord();
		bool GetMatchCase();
		bool GetRegEx();
		int GetDirection();
		int GetOrigin();
		int GetScope();
		bool GetRecursive(); // for find in search path
		bool GetHidden(); // for find in search path
		wxString GetSearchPath(); // for find in search path
		wxString GetSearchMask(); // for find in search path
        void UpdateUI();

		void OnFindChange(wxNotebookEvent& event);
		void OnRegEx(wxCommandEvent& event);
		void OnBrowsePath(wxCommandEvent& event);
        void OnRadioBox(wxCommandEvent& event);
        void OnActivate(wxActivateEvent& event);

	private:
        bool m_Complete;
		DECLARE_EVENT_TABLE()
};

#endif // FINDDLG_H

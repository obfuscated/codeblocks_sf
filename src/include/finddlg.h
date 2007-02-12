#ifndef FINDDLG_H
#define FINDDLG_H

#include "findreplacebase.h"

class wxNotebookEvent;
class wxCommandEvent;
class wxActivateEvent;

class FindDlg : public FindReplaceBase
{
	public:
		FindDlg(wxWindow* parent, const wxString& initial = wxEmptyString, bool hasSelection = false, bool findInFilesOnly = false, bool findInFilesActive = false);
		~FindDlg();
		wxString GetFindString() const;
		wxString GetReplaceString() const{ return wxEmptyString; }
		bool IsFindInFiles() const;
		bool GetDeleteOldSearches() const;
		bool GetMatchWord() const;
		bool GetStartWord() const;
		bool GetMatchCase() const;
		bool GetRegEx() const;
		bool GetAutoWrapSearch() const;
		bool GetFindUsesSelectedText() const;
		int GetDirection() const;
		int GetOrigin() const;
		int GetScope() const;
		bool GetRecursive() const; // for find in search path
		bool GetHidden() const; // for find in search path
		wxString GetSearchPath() const; // for find in search path
		wxString GetSearchMask() const; // for find in search path
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

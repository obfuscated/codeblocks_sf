#ifndef REPLACEDLG_H
#define REPLACEDLG_H

#include "findreplacebase.h"

class wxComboBox;

class ReplaceDlg : public FindReplaceBase
{
	public:
		ReplaceDlg(wxWindow* parent, const wxString& initial = wxEmptyString, bool hasSelection = false,
            bool findInFilesOnly = false, bool replaceInFilesActive = false);
		~ReplaceDlg();
		wxString GetFindString() const;
		wxString GetReplaceString() const;
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
		bool GetRecursive() const{ return false; }
		bool GetHidden() const{ return false; }
		wxString GetSearchPath() const{ return wxEmptyString; }
		wxString GetSearchMask() const{ return wxEmptyString; }

		void OnFindChange(wxNotebookEvent& event);
		void OnRegEx(wxCommandEvent& event);
		void OnActivate(wxActivateEvent& event);

	private:
        bool m_Complete;
		void FillComboWithLastValues(wxComboBox* combo, const wxString& configKey);
		void SaveComboValues(wxComboBox* combo, const wxString& configKey);
		DECLARE_EVENT_TABLE()
};

#endif // REPLACEDLG_H

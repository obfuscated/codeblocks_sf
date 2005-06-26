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
		bool GetMatchWord();
		bool GetStartWord();
		bool GetMatchCase();
		bool GetRegEx();
		int GetDirection();
		int GetOrigin();
		int GetScope();
		
		void OnFindChange(wxCommandEvent& event);
		void OnRegEx(wxCommandEvent& event);
	private:
        bool m_Complete;
		DECLARE_EVENT_TABLE()
};

#endif // FINDDLG_H

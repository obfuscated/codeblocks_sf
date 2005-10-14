#ifndef REPLACEDLG_H
#define REPLACEDLG_H

#include <wx/combobox.h>
#include "findreplacebase.h"

class ReplaceDlg : public FindReplaceBase
{
	public:
		ReplaceDlg(wxWindow* parent, const wxString& initial = wxEmptyString, bool hasSelection = false);
		~ReplaceDlg();
		wxString GetFindString();
		wxString GetReplaceString();
		bool IsFindInFiles(){ return false; }
		bool GetMatchWord();
		bool GetStartWord();
		bool GetMatchCase();
		bool GetRegEx();
		int GetDirection();
		int GetOrigin();
		int GetScope();
		bool GetRecursive(){ return false; }
		bool GetHidden(){ return false; }
		wxString GetSearchPath(){ return wxEmptyString; }
		wxString GetSearchMask(){ return wxEmptyString; }

		void OnRegEx(wxCommandEvent& event);
	private:
		void FillComboWithLastValues(wxComboBox* combo, const wxString& configKey);
		void SaveComboValues(wxComboBox* combo, const wxString& configKey);
		DECLARE_EVENT_TABLE()
};

#endif // REPLACEDLG_H

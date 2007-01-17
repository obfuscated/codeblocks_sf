#ifndef DEBUGGEROPTIONSPRJDLG_H
#define DEBUGGEROPTIONSPRJDLG_H

#include <wx/intl.h>
#include "configurationpanel.h"
#include <settings.h>

class cbProject;
class wxListBox;
class DebuggerGDB;

class DebuggerOptionsProjectDlg : public cbConfigurationPanel
{
	public:
		DebuggerOptionsProjectDlg(wxWindow* parent, DebuggerGDB* debugger, cbProject* project);
		virtual ~DebuggerOptionsProjectDlg();

        virtual wxString GetTitle() const { return _("Debugger options"); }
        virtual wxString GetBitmapBaseName() const { return _T("debugger"); }
        virtual void OnApply();
        virtual void OnCancel(){}
	protected:
        void OnAdd(wxCommandEvent& event);
        void OnEdit(wxCommandEvent& event);
        void OnDelete(wxCommandEvent& event);
        void OnUpdateUI(wxUpdateUIEvent& event);
	private:
		DebuggerGDB* m_pDBG;
        cbProject* m_pProject;
		wxArrayString m_OldPaths;
		DECLARE_EVENT_TABLE()
};

#endif // DEBUGGEROPTIONSPRJDLG_H

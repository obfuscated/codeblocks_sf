#ifndef EXTERNALDEPSDLG_H
#define EXTERNALDEPSDLG_H

#include "settings.h"
#include <wx/dialog.h>

class cbProject;
class ProjectBuildTarget;

class DLLIMPORT ExternalDepsDlg : public wxDialog
{
	public:
		ExternalDepsDlg(wxWindow* parent, cbProject* project, ProjectBuildTarget* target);
		virtual ~ExternalDepsDlg();
		
		void EndModal(int retCode);
	protected:
        void FillAdditional();
        void FillExternal();
        
        // BYO: Do___ functions xhanged to defines in .cpp because of incompatibilities
        // with unicode build
        
        //void DoAdd(const wxString& listbox, const wxString& message);
        //void DoEdit(const wxString& listbox, const wxString& message);
        //void DoDel(const wxString& listbox);

        void OnAddAdditional(wxCommandEvent& event);
        void OnEditAdditional(wxCommandEvent& event);
        void OnDelAdditional(wxCommandEvent& event);

        void OnAddExternal(wxCommandEvent& event);
        void OnEditExternal(wxCommandEvent& event);
        void OnDelExternal(wxCommandEvent& event);

        void OnUpdateUI(wxUpdateUIEvent& event);
        
        cbProject* m_pProject;
        ProjectBuildTarget* m_pTarget;
	private:
        DECLARE_EVENT_TABLE()
};

#endif // EXTERNALDEPSDLG_H

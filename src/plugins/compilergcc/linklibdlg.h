#ifndef LINKLIBDLG_H
#define LINKLIBDLG_H

#include <wx/dialog.h>

class cbProject;
class ProjectBuildTarget;
class Compiler;

class LinkLibDlg : public wxDialog
{
	public:
		LinkLibDlg(wxWindow* parent, cbProject* project, ProjectBuildTarget* target, Compiler* compiler, const wxString& lib);
		virtual ~LinkLibDlg();
		
		const wxString& GetLib(){ return m_Lib; }
        void EndModal(int retCode);
	protected:
        void OnBrowse(wxCommandEvent& event);
        void OnUpdateUI(wxUpdateUIEvent& event);
        cbProject* m_pProject;
        ProjectBuildTarget* m_pTarget;
        Compiler* m_pCompiler;
        wxString m_Lib;
	private:
        DECLARE_EVENT_TABLE();
};

#endif // LINKLIBDLG_H

#ifndef PROJECTOPTIONSDLG_H
#define PROJECTOPTIONSDLG_H

#include "settings.h"
#include <wx/dialog.h>

class wxTreeEvent;
class wxSpinEvent;
class cbProject;
class cbCompilerPlugin;

/*
 * No description
 */
class ProjectOptionsDlg : public wxDialog
{
	public:
		// class constructor
		ProjectOptionsDlg(wxWindow* parent, cbProject* project);
		// class destructor
		~ProjectOptionsDlg();

		void EndModal(int retCode);
		void OnFileOptionsClick(wxCommandEvent& event);
		void OnFileToggleMarkClick(wxCommandEvent& event);
        void OnProjectTypeChanged(wxCommandEvent& event);
		void OnBuildTargetChanged(wxCommandEvent& event);
		void OnAddBuildTargetClick(wxCommandEvent& event);
		void OnEditBuildTargetClick(wxCommandEvent& event);
		void OnCopyBuildTargetClick(wxCommandEvent& event);
		void OnRemoveBuildTargetClick(wxCommandEvent& event);
		void OnBrowseOutputFilenameClick(wxCommandEvent& event);
		void OnBrowseDirClick(wxCommandEvent& event);
		void OnEditDepsClick(wxCommandEvent& event);
		void OnExportTargetClick(wxCommandEvent& event);
		void OnBuildOrderClick(wxCommandEvent& event);
		void OnProjectBuildOptionsClick(wxCommandEvent& event);
		void OnProjectDepsClick(wxCommandEvent& event);
		void OnTargetBuildOptionsClick(wxCommandEvent& event);
		void OnUpdateUI(wxUpdateUIEvent& event);

		void OnScriptsOverviewSelChanged(wxTreeEvent& event);
		void OnCheckScripts(wxCommandEvent& event);
		void OnAddScript(wxCommandEvent& event);
		void OnRemoveScript(wxCommandEvent& event);
		void OnScriptMoveUp(wxSpinEvent& event);
		void OnScriptMoveDown(wxSpinEvent& event);
    private:
        void BuildScriptsTree();
        void AddPluginPanels();
        void FillScripts();
        void FillBuildTargets();
        void DoTargetChange();
        void DoBeforeTargetChange(bool force = false);
        bool DoCheckScripts(CompileTargetBase* base);
        bool IsScriptValid(const wxString& script);
        cbProject* m_Project;
        int m_Current_Sel; // current target selection (when selection changes it is the old selection - handy, eh?)
    	cbCompilerPlugin* m_pCompiler;
		DECLARE_EVENT_TABLE()
};

#endif // PROJECTOPTIONSDLG_H


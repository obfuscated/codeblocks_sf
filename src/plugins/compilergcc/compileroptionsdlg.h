#ifndef COMPILEROPTIONSDLG_H
#define COMPILEROPTIONSDLG_H

#include <settings.h>
#include "compilergcc.h"
#include "compileroptions.h"
#include <cbproject.h>

class DLLIMPORT ScopeTreeData : public wxTreeItemData
{
    public:
        ScopeTreeData(cbProject* project, ProjectBuildTarget* target){ m_Project = project; m_Target = target; }
        cbProject* GetProject(){ return m_Project; }
        void SetProject(cbProject* project){ m_Project = project; }
		ProjectBuildTarget* GetTarget(){ return m_Target; }
		void SetTarget(ProjectBuildTarget* target){ m_Target = target; }
    private:
        cbProject* m_Project;
		ProjectBuildTarget* m_Target;
};


class CompilerOptionsDlg : public wxDialog
{
	public:
		CompilerOptionsDlg(wxWindow* parent, CompilerGCC* compiler, cbProject* project = 0L, ProjectBuildTarget* target = 0L);
		~CompilerOptionsDlg();
	protected:
	private:
		void TextToOptions();
		void OptionsToText();
		void DoFillCompilerSets();
		void DoFillCompilerPrograms();
		void DoFillPrograms();
		void DoFillOthers();
		void DoFillCategories();
		void DoFillOptions();
		void DoFillTree(cbProject* focusProject = 0L, ProjectBuildTarget* focusTarget = 0L);
        void DoFillCompileOptions(const wxArrayString& array, wxTextCtrl* control);
        void DoFillCompileDirs(const wxArrayString& array, wxListBox* control);
        void DoGetCompileOptions(wxArrayString& array, wxTextCtrl* control);
        void DoGetCompileDirs(wxArrayString& array, wxListBox* control);
		void DoSaveOptions(int compilerIdx, ScopeTreeData* data = 0L);
		void DoLoadOptions(int compilerIdx, ScopeTreeData* data = 0L);
		void DoMakeRelative(wxFileName& path);
		void DoUpdateCompiler();
		void CompilerChanged(ScopeTreeData* data);

		void OnTreeSelectionChange(wxTreeEvent& event);
		void OnTreeSelectionChanging(wxTreeEvent& event);
		void OnCompilerChanged(wxCommandEvent& event);
		void OnCategoryChanged(wxCommandEvent& event);
		void OnOptionToggled(wxCommandEvent& event);
		void OnAddDirClick(wxCommandEvent& event);
		void OnEditDirClick(wxCommandEvent& event);
		void OnRemoveDirClick(wxCommandEvent& event);
		void OnAddVarClick(wxCommandEvent& event);
		void OnEditVarClick(wxCommandEvent& event);
		void OnRemoveVarClick(wxCommandEvent& event);
		void OnSetDefaultCompilerClick(wxCommandEvent& event);
		void OnAddCompilerClick(wxCommandEvent& event);
		void OnEditCompilerClick(wxCommandEvent& event);
		void OnRemoveCompilerClick(wxCommandEvent& event);
		void OnMasterPathClick(wxCommandEvent& event);
		void OnSelectProgramClick(wxCommandEvent& event);
		void OnAdvancedClick(wxCommandEvent& event);
		void OnUpdateUI(wxUpdateUIEvent& event);

		virtual void EndModal(int retCode);
		
		CompilerGCC* m_Compiler;
		CompilerOptions m_Options;
        wxArrayString m_LinkerOptions;
        wxArrayString m_CompilerOptions;
        wxArrayString m_IncludeDirs;
        wxArrayString m_LibDirs;
		wxArrayString m_CommandsBeforeBuild;
		wxArrayString m_CommandsAfterBuild;
		int m_LastCompilerIdx;
		int m_InitialCompilerIdx;
		cbProject* m_pProject;
		ProjectBuildTarget* m_pTarget;
		
		bool m_BuildingTree; // flag to ignore tree changing events while building it

		DECLARE_EVENT_TABLE()
};

#endif // COMPILEROPTIONSDLG_H


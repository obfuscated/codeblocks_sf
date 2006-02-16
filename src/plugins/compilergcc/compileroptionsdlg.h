#ifndef COMPILEROPTIONSDLG_H
#define COMPILEROPTIONSDLG_H

#include <settings.h>
#include <cbplugin.h>
#include <wx/spinbutt.h>
#include "compilergcc.h"
#include <compileroptions.h>
#include <cbproject.h>

class wxListBox;

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


class CompilerOptionsDlg : public cbConfigurationPanel
{
	public:
		CompilerOptionsDlg(wxWindow* parent, CompilerGCC* compiler, cbProject* project = 0L, ProjectBuildTarget* target = 0L);
		~CompilerOptionsDlg();

        virtual wxString GetTitle(){ return _("Global compiler settings"); }
        virtual wxString GetBitmapBaseName(){ return _T("compiler"); }
        virtual void OnApply();
        virtual void OnCancel(){}
	protected:
	private:
		void TextToOptions();
		void OptionsToText();
		void DoFillCompilerSets();
		void DoFillCompilerPrograms();
		void DoFillVars(const StringHash* vars = 0);
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
		void DoSaveCompilerPrograms(int compilerIdx);
		void CompilerChanged(ScopeTreeData* data);
		void UpdateCompilerForTargets(int compilerIdx);
		void AutoDetectCompiler();
		wxListBox* GetDirsListBox();
        CompileOptionsBase* GetVarsOwner();

		void OnTreeSelectionChange(wxTreeEvent& event);
		void OnTreeSelectionChanging(wxTreeEvent& event);
		void OnCompilerChanged(wxCommandEvent& event);
		void OnCategoryChanged(wxCommandEvent& event);
		void OnOptionToggled(wxCommandEvent& event);
		void OnAddDirClick(wxCommandEvent& event);
		void OnEditDirClick(wxCommandEvent& event);
		void OnRemoveDirClick(wxCommandEvent& event);
		void OnClearDirClick(wxCommandEvent& event);
		void OnAddVarClick(wxCommandEvent& event);
		void OnEditVarClick(wxCommandEvent& event);
		void OnRemoveVarClick(wxCommandEvent& event);
		void OnClearVarClick(wxCommandEvent& event);
		void OnSetDefaultCompilerClick(wxCommandEvent& event);
		void OnAddCompilerClick(wxCommandEvent& event);
		void OnEditCompilerClick(wxCommandEvent& event);
		void OnRemoveCompilerClick(wxCommandEvent& event);
		void OnResetCompilerClick(wxCommandEvent& event);
		void OnAddLibClick(wxCommandEvent& event);
		void OnEditLibClick(wxCommandEvent& event);
		void OnRemoveLibClick(wxCommandEvent& event);
		void OnClearLibClick(wxCommandEvent& event);
		void OnMoveLibUpClick(wxSpinEvent& event);
		void OnMoveLibDownClick(wxSpinEvent& event);
		void OnMoveDirUpClick(wxSpinEvent& event);
		void OnMoveDirDownClick(wxSpinEvent& event);
		void OnMasterPathClick(wxCommandEvent& event);
		void OnAutoDetectClick(wxCommandEvent& event);
		void OnSelectProgramClick(wxCommandEvent& event);
		void OnAdvancedClick(wxCommandEvent& event);
		void OnAddExtraPathClick(wxCommandEvent& event);
		void OnEditExtraPathClick(wxCommandEvent& event);
		void OnRemoveExtraPathClick(wxCommandEvent& event);
		void OnUpdateUI(wxUpdateUIEvent& event);
		void OnMyCharHook(wxKeyEvent& event);

		CompilerGCC* m_Compiler;
		CompilerOptions m_Options;
        wxArrayString m_LinkerOptions;
        wxArrayString m_LinkLibs;
        wxArrayString m_CompilerOptions;
        wxArrayString m_IncludeDirs;
        wxArrayString m_LibDirs;
        wxArrayString m_ResDirs;
		wxArrayString m_CommandsBeforeBuild;
		wxArrayString m_CommandsAfterBuild;
		bool m_AlwaysUsePost;
		int m_LastCompilerIdx;
		int m_InitialCompilerIdx;
		cbProject* m_pProject;
		ProjectBuildTarget* m_pTarget;

		bool m_BuildingTree; // flag to ignore tree changing events while building it

		DECLARE_EVENT_TABLE()
};

#endif // COMPILEROPTIONSDLG_H


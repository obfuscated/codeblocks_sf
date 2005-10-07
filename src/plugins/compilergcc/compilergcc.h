#ifndef COMPILERGCC_H
#define COMPILERGCC_H

#include <settings.h> // SDK
#include <sdk_events.h>
#include <compileoptionsbase.h>
#include <cbplugin.h>
#include <cbproject.h>
#include <simpletextlog.h>
#include "compilermessages.h"
#include <wx/process.h>
#include "compilererrors.h"
#include <compilerfactory.h>

#define MAX_TARGETS 64
#define DEFAULT_CONSOLE_SHELL _T("xterm -T $TITLE -e")

enum CompilerOptionsType
{
	cotGlobal = 0,
	cotProjectOrTarget
};

enum ErrorType
{
	etNone = 0,
	etError,
	etWarning
};

enum MultiProjectJob
{
    mpjNone,
    mpjCompile,
    mpjRebuild
};

class CompilerGCC : public cbCompilerPlugin
{
    public:
        CompilerGCC();
        virtual ~CompilerGCC();

        virtual void OnAttach();
        virtual void OnRelease(bool appShutDown);
        virtual void BuildMenu(wxMenuBar* menuBar); // offer for menu space by host
        virtual void BuildModuleMenu(const ModuleType type, wxMenu* menu, const wxString& arg); // offer for menu space by a module
        virtual bool BuildToolBar(wxToolBar* toolBar);

        virtual int Run(ProjectBuildTarget* target = 0L);
        virtual int Clean(ProjectBuildTarget* target = 0L);
        virtual int DistClean(ProjectBuildTarget* target = 0L);
        virtual int Compile(ProjectBuildTarget* target = 0L);
        virtual int CompileAll();
        virtual int RebuildAll();
        virtual int CreateDist();
        virtual int Rebuild(ProjectBuildTarget* target = 0L);
        virtual int CompileFile(const wxString& file);
        virtual int KillProcess();
		virtual bool IsRunning() const { return m_Process; }
		virtual int GetExitCode() const { return m_LastExitCode; }
		virtual int Configure(cbProject* project, ProjectBuildTarget* target = 0L);

		void SwitchCompiler(int compilerIdx);
		int GetCurrentCompilerIndex();

		// used to read from the external process
		void OnIdle(wxIdleEvent& event);
        void OnTimer(wxTimerEvent& event);

        void OnCompile(wxCommandEvent& event);
        void OnCompileFile(wxCommandEvent& event);
        void OnRebuild(wxCommandEvent& event);
        void OnCompileAll(wxCommandEvent& event);
        void OnRebuildAll(wxCommandEvent& event);
        void OnDistClean(wxCommandEvent& event);
        void OnClean(wxCommandEvent& event);
        void OnRun(wxCommandEvent& event);
		void OnProjectCompilerOptions(wxCommandEvent& event);
		void OnTargetCompilerOptions(wxCommandEvent& event);
        void OnCompileAndRun(wxCommandEvent& event);
        void OnKillProcess(wxCommandEvent& event);
		void OnSelectTarget(wxCommandEvent& event);
		void OnNextError(wxCommandEvent& event);
		void OnPreviousError(wxCommandEvent& event);
		void OnClearErrors(wxCommandEvent& event);
        void OnCreateDist(wxCommandEvent& event);
        void OnExportMakefile(wxCommandEvent& event);
        void OnUpdateUI(wxUpdateUIEvent& event);
        void OnConfig(wxCommandEvent& event);
    private:
		void SetupEnvironment();
		void SetEnvironmentForCompilerIndex(int idx, wxString& envPath);
		void OnProjectActivated(CodeBlocksEvent& event);
		/*void OnProjectPopupMenu(wxNotifyEvent& event);*/
		void OnGCCOutput(CodeBlocksEvent& event);
		void OnGCCError(CodeBlocksEvent& event);
		void OnGCCTerminated(CodeBlocksEvent& event);
        void OnJobEnd();

		void SaveOptions();
		void LoadOptions();
		bool DoPrepareMultiProjectCommand(MultiProjectJob job);
		void DoPrepareQueue();
        int DoRunQueue();
        bool DoCreateMakefile(bool temporary = true, const wxString& makefile = _T(""));
        void DoDeleteTempMakefile();
		void DoClearTargetMenu();
		void DoRecreateTargetMenu();
		void DoUpdateTargetMenu();
        FileTreeData* DoSwitchProjectTemporarily();
        ProjectBuildTarget* DoAskForTarget();
        int DoGUIAskForTarget();
        void ClearLog();
        bool CheckProject();
        void AskForActiveProject();
		void DoGotoNextError();
		void DoGotoPreviousError();
		void DoClearErrors();
        wxString ProjectMakefile();
        void AddOutputLine(const wxString& output, bool forceErrorColor = false);
        void PrintBanner();
        bool UseMake(ProjectBuildTarget* target = 0);
		bool CompilerValid(ProjectBuildTarget* target = 0);
		ProjectBuildTarget* GetBuildTargetForFile(ProjectFile* pf);
		ProjectBuildTarget* GetBuildTargetForFile(const wxString& file);

		// programs
		int m_CompilerIdx;
		CompilerPrograms m_EmptyCompilerPrograms; // always empty; returned on invalid compiler index

		wxString m_EnvironmentMsg;
        int m_PageIndex;
		int m_ListPageIndex;
        wxMenu* m_Menu;
        wxMenu* m_TargetMenu;
		wxToolBar* m_pToolbar;
		int m_TargetIndex;
        wxMenu* m_ErrorsMenu;
        cbProject* m_Project;
        wxProcess* m_Process;
        wxToolBar* m_pTbar;
        long int m_Pid;
        wxTimer m_timerIdleWakeUp;
        SimpleTextLog* m_Log;
        CompilerMessages* m_pListLog;
		wxComboBox* m_ToolTarget;
		wxStaticText* m_ToolTargetLabel;
		bool m_IsRun;
		bool m_RunAfterCompile;
		wxString m_CdRun;
		MultiProjectJob m_DoAllProjects; // for xxxAll() functions
		cbProject* m_BackupActiveProject;
		unsigned int m_ProjectIndex;
		wxString m_RunCmd;
		bool m_LastExitCode;
		CompilerErrors m_Errors;
		bool m_HasTargetAll;

		unsigned int m_QueueIndex;
        wxArrayString m_Queue;

		wxString m_OriginalPath;
		wxString m_LastTempMakefile;
        bool m_DeleteTempMakefile;

		wxString m_ConsoleShell;

        DECLARE_EVENT_TABLE()
};

CB_DECLARE_PLUGIN();

#endif // COMPILERGCC_H

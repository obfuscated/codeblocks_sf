/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef COMPILERGCC_H
#define COMPILERGCC_H

#include <queue>

#include <settings.h> // SDK
#include <sdk_events.h>
#include <compileoptionsbase.h>
#include <cbplugin.h>
#include <cbproject.h>
#include <logger.h>
#include "compilermessages.h"
#include <wx/process.h>
#include <wx/dynarray.h>
#include "compilererrors.h"
#include "compiler_defs.h"
#include <compilerfactory.h>
#include <wx/timer.h>
#include <wx/choice.h>

#define MAX_TARGETS 128

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

/// Helper enum for compiler's state. This state signifies the kind of build the compiler is working on.
enum BuildJob
{
    bjIdle = 0, ///< Not currently building
    bjWorkspace, ///< Building the workspace
    bjProject, ///< Building the project
    bjTarget ///< Building the target
};

/// Defines the current state of the compiler.
enum BuildState
{
    bsNone = 0,
    bsProjectPreBuild,
    bsTargetClean,
    bsTargetPreBuild,
    bsTargetBuild,
    bsTargetPostBuild,
    bsTargetDone,
    bsProjectPostBuild,
    bsProjectDone
};

enum LogTarget
{
    ltMessages  = 0x01,
    ltFile      = 0x02,

    ltAll       = 0xff
};

enum BuildAction
{
    baClean = 0,
    baBuild
};

class wxTimerEvent;
class wxComboBox;
class wxStaticText;
class wxGauge;
class BuildLogger;

class CompilerGCC : public cbCompilerPlugin
{
    public:
        CompilerGCC();
        virtual ~CompilerGCC();

        virtual void OnAttach();
        virtual void OnRelease(bool appShutDown);
        virtual void BuildMenu(wxMenuBar* menuBar); // offer for menu space by host
        virtual void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0); // offer for menu space by a module
        virtual bool BuildToolBar(wxToolBar* toolBar);

        virtual int Run(ProjectBuildTarget* target = 0L);
        virtual int Run(const wxString& target);
        virtual int RunSingleFile(const wxString& filename);
        virtual int Clean(const wxString& target);
        virtual int Clean(ProjectBuildTarget* target = 0L);
        virtual int DistClean(ProjectBuildTarget* target = 0L);
        virtual int DistClean(const wxString& target);
        virtual int Build(ProjectBuildTarget* target = 0L);
        virtual int Build(const wxString& target);
        virtual int Rebuild(ProjectBuildTarget* target = 0L);
        virtual int Rebuild(const wxString& target);
        virtual int CleanWorkspace(const wxString& target = wxEmptyString);
        virtual int BuildWorkspace(const wxString& target = wxEmptyString);
        virtual int RebuildWorkspace(const wxString& target = wxEmptyString);
        virtual int CompileFile(const wxString& file);
        virtual int CompileFileWithoutProject(const wxString& file);
        virtual int CompileFileWithMake(ProjectFile* pf, ProjectBuildTarget* bt);
        virtual int CompileFileDefault(cbProject* project, ProjectFile* pf, ProjectBuildTarget* bt);
        virtual int KillProcess();
        virtual bool IsRunning() const;
        virtual int GetExitCode() const { return m_LastExitCode; }
        virtual int Configure(cbProject* project, ProjectBuildTarget* target = 0L);

        int GetConfigurationPriority() const { return 0; }
        int GetConfigurationGroup() const { return cgCompiler; }
        cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent);

        void SwitchCompiler(const wxString& id);
        const wxString& GetCurrentCompilerID();

        // used to read from the external process
        void OnIdle(wxIdleEvent& event);
        void OnTimer(wxTimerEvent& event);

        void OnCompile(wxCommandEvent& event);
        void OnCompileFile(wxCommandEvent& event);
        void OnRebuild(wxCommandEvent& event);
        void OnCompileAll(wxCommandEvent& event);
        void OnRebuildAll(wxCommandEvent& event);
        void OnCleanAll(wxCommandEvent& event);
//        void OnDistClean(wxCommandEvent& event);
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
//        void OnCreateDist(wxCommandEvent& event);
        void OnExportMakefile(wxCommandEvent& event);
        void OnUpdateUI(wxUpdateUIEvent& event);
        void OnConfig(wxCommandEvent& event);
    private:
        friend class CompilerOptionsDlg;

        void Dispatcher(wxCommandEvent& event);
        void TextURL(wxTextUrlEvent& event);

        bool StopRunningDebugger();

        bool ReAllocProcesses();
        void AllocProcesses();
        void FreeProcesses();
        bool IsProcessRunning(int idx = -1) const;
        int GetNextAvailableProcessIndex() const;
        int GetActiveProcessCount() const;

        void SetupEnvironment();
        void SetEnvironmentForCompiler(const wxString& id, wxString& envPath);
        void OnProjectActivated(CodeBlocksEvent& event);
        void OnProjectLoaded(CodeBlocksEvent& event);
        void OnProjectUnloaded(CodeBlocksEvent& event);
        void OnCompileFileRequest(CodeBlocksEvent& event);
        void OnGCCOutput(CodeBlocksEvent& event);
        void OnGCCError(CodeBlocksEvent& event);
        void OnGCCTerminated(CodeBlocksEvent& event);
        void OnJobEnd(size_t procIndex, int exitCode);

        void SaveOptions();
        void LoadOptions();
        void DoPrepareQueue(bool clearLog=true);
        void NotifyCleanProject(const wxString& target);
        void NotifyCleanWorkspace();
        int DoRunQueue();
        bool DoCreateMakefile(bool temporary = true, const wxString& makefile = _T(""));
        void DoDeleteTempMakefile();
        void DoClearTargetMenu();
        void DoRecreateTargetMenu();
        void DoUpdateTargetMenu(int targetIndex);
        FileTreeData* DoSwitchProjectTemporarily();
        ProjectBuildTarget* DoAskForTarget();
        int DoGUIAskForTarget();
        void ClearLog();
        void PrepareCompileFile(wxFileName& file);
        void PrepareCompileFilePM(wxFileName& file);
        bool CheckProject();
        void AskForActiveProject();
        void StartCompileFile(wxFileName file);
        void DoGotoNextError();
        void DoGotoPreviousError();
        void DoClearErrors();
        wxString ProjectMakefile();
        void AddOutputLine(const wxString& output, bool forceErrorColour = false);
        void LogWarningOrError(CompilerLineType lt, cbProject* prj, const wxString& filename, const wxString& line, const wxString& msg);
        void LogMessage(const wxString& message, CompilerLineType lt = cltNormal, LogTarget log = ltAll, bool forceErrorColour = false, bool isTitle = false, bool updateProgress = false);
        void SaveBuildLog();
        void InitBuildLog(bool workspaceBuild);
        void PrintBanner(BuildAction action, cbProject* prj = 0, ProjectBuildTarget* target = 0);
        bool UseMake(cbProject* project = 0);
        bool CompilerValid(ProjectBuildTarget* target = 0);
        ProjectBuildTarget* GetBuildTargetForFile(ProjectFile* pf);
        ProjectBuildTarget* GetBuildTargetForFile(const wxString& file);
        wxString GetMakeCommandFor(MakeCommand cmd, cbProject* project, ProjectBuildTarget* target);
        int DoBuild(bool clean, bool build);
        int DoBuild(const wxString& target, bool clean, bool build, bool clearLog=true);
        int DoWorkspaceBuild(const wxString& target, bool clean, bool build, bool clearLog=true);
        void CalculateWorkspaceDependencies(wxArrayInt& deps);
        void CalculateProjectDependencies(cbProject* prj, wxArrayInt& deps);
        void InitBuildState(BuildJob job, const wxString& target);
        void ResetBuildState();
        void BuildStateManagement(); ///< This uses m_BuildJob.
        BuildState GetNextStateBasedOnJob();
        void NotifyJobDone(bool showNothingToBeDone = false);

        // wxArrayString from DirectCommands
        void AddToCommandQueue(const wxArrayString& commands);

        int GetTargetIndexFromName(cbProject* prj, const wxString& name);
        void UpdateProjectTargets(cbProject* project);
        wxString GetTargetString(int index = -1);
        void DoClean(const wxArrayString& commands);
        bool DoCleanWithMake(const wxString& cmd, bool showOutput=false);

        // active target, currently building project or active project
        wxString GetCurrentCompilerID(ProjectBuildTarget* target);

		// returns a string valid to be used as LD_LIBRARY_PATH (or equivalent)
		wxString GetDynamicLinkerPathForTarget(ProjectBuildTarget* target);

        // when a build is about to start, a preprocessing step runs
        // in PreprocessJob(), that fills m_BuildJobTargetsList with
        // BuildJobTarget. It is a simple pair of project->target which
        // are to be built in order
        struct BuildJobTarget
        {
            BuildJobTarget(cbProject* p = 0, const wxString& n = wxEmptyString) : project(p), targetName(n) {}
            cbProject* project;
            wxString targetName;
        };
        typedef std::queue<BuildJobTarget> BuildJobTargetsList;
        BuildJobTargetsList m_BuildJobTargetsList;

        void ExpandTargets(cbProject* project, const wxString& targetName, wxArrayString& result);
        void PreprocessJob(cbProject* project, const wxString& targetName);
        BuildJobTarget GetNextJob();
        BuildJobTarget& PeekNextJob();

        wxArrayString m_Targets; // list of targets contained in the active project
        int m_RealTargetsStartIndex;
        int m_RealTargetIndex;

        CompilerQueue m_CommandQueue;

        wxString m_CompilerId;

        wxString m_EnvironmentMsg;
        int m_PageIndex;
        int m_ListPageIndex;
        wxMenu* m_Menu;
        wxMenu* m_TargetMenu;
        int m_TargetIndex;
        wxMenu* m_ErrorsMenu;
        cbProject* m_Project;
        wxProcess** m_Processes;
        size_t m_ParallelProcessCount;
        wxToolBar* m_pTbar;
        long int* m_Pid;
        wxString* m_ProcessOutputFiles;
        wxTimer m_timerIdleWakeUp;
        BuildLogger* m_Log;
        CompilerMessages* m_pListLog;
        wxChoice* m_ToolTarget;
        bool m_RunAfterCompile;
        wxString m_CdRun;
        wxString m_RunCmd;
        int m_LastExitCode;
        CompilerErrors m_Errors;
        wxString m_LastTargetName;
        bool m_NotifiedMaxErrors;
        wxLongLong m_StartTimer;

        // build state management
        cbProject* m_pBuildingProject; // +
        wxString m_BuildingTargetName; // +
        BuildJob m_BuildJob;
        BuildState m_BuildState;
        BuildState m_NextBuildState;
        cbProject* m_pLastBuildingProject;
        ProjectBuildTarget* m_pLastBuildingTarget;
        // Clean and Build
        bool m_Clean;
        bool m_Build;
        // if set and we are reaching NotifyJobDone, we know that we have finished the
        // last step in a clean/build (aka rebuild)-process and send the cbEVT_COMPILER_FINISHED
        bool m_LastBuildStep;
        // to decide if post-build steps should run
        bool m_RunTargetPostBuild;
        bool m_RunProjectPostBuild;

        wxString m_OriginalPath;
        wxString m_LastTempMakefile;
        bool m_DeleteTempMakefile;

        bool m_IsWorkspaceOperation; // true for workspace commands
        bool m_IsCompileFileRequest; // true for plugin compile file requests

        wxString m_BuildLogFilename;
        wxString m_BuildLogTitle;
        wxString m_BuildLogContents;
        wxDateTime m_BuildStartTime;

        // build progress
        size_t m_MaxProgress;
        size_t m_CurrentProgress;
        bool m_LogBuildProgressPercentage;

        DECLARE_EVENT_TABLE()
};

#endif // COMPILERGCC_H

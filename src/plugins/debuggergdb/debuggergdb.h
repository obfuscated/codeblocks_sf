/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef DEBUGGERGDB_H
#define DEBUGGERGDB_H

#include <map>

#include <settings.h> // much of the SDK is here
#include <sdk_events.h>
#include <cbplugin.h>
#include <loggers.h>
#include <pipedprocess.h>
#include <wx/regex.h>

#include "remotedebugging.h"
#include "debuggerstate.h"
#include "debugger_defs.h"

class cbProject;
class TiXmlElement;
class DebuggerDriver;
class DebuggerCmd;
class Compiler;
struct TestIfBelogToProject;
class DebuggerConfiguration;

class DebuggerGDB : public cbDebuggerPlugin
{
        DebuggerState m_State;
    public:
        DebuggerGDB();
        ~DebuggerGDB() override;

        cbConfigurationPanel* GetProjectConfigurationPanel(wxWindow* parent,
                                                           cbProject* project) override;
        void OnAttachReal() override; // fires when the plugin is attached to the application
        void OnReleaseReal(bool appShutDown) override; // fires when the plugin is released from the application

        bool SupportsFeature(cbDebuggerFeature::Flags flag) override;

        cbDebuggerConfiguration* LoadConfig(const ConfigManagerWrapper &config) override;

        DebuggerConfiguration& GetActiveConfigEx();

        void RunCommand(int cmd);

        cb::shared_ptr<cbBreakpoint> AddBreakpoint(const wxString& filename, int line) override;
        cb::shared_ptr<cbBreakpoint> AddDataBreakpoint(const wxString& dataExpression) override;
        int GetBreakpointsCount() const override;
        cb::shared_ptr<cbBreakpoint> GetBreakpoint(int index) override;
        cb::shared_ptr<const cbBreakpoint> GetBreakpoint(int index) const override;
        void UpdateBreakpoint(cb::shared_ptr<cbBreakpoint> breakpoint) override;
        void DeleteBreakpoint(cb::shared_ptr<cbBreakpoint> breakpoint) override;
        void DeleteAllBreakpoints() override;
        void ShiftBreakpoint(int index, int lines_to_shift) override;
        void EnableBreakpoint(cb::shared_ptr<cbBreakpoint> breakpoint, bool enable) override;

        // stack frame calls;
        int GetStackFrameCount() const override;
        cb::shared_ptr<const cbStackFrame> GetStackFrame(int index) const override;
        void SwitchToFrame(int number) override;
        int GetActiveStackFrame() const override;

        // threads
        int GetThreadsCount() const override;
        cb::shared_ptr<const cbThread> GetThread(int index) const override;
        bool SwitchToThread(int thread_number) override;

        bool Debug(bool breakOnEntry) override;
        void Continue() override;
        void Next() override;
        void NextInstruction() override;
        void StepIntoInstruction() override;
        void Step() override;
        void StepOut() override;
        bool RunToCursor(const wxString& filename, int line, const wxString& line_text) override;
        void SetNextStatement(const wxString& filename, int line) override;
        void Break() override;
        void Stop() override;
        bool Validate(const wxString& line, const char cb);
        bool IsRunning() const override { return m_pProcess; }
        bool IsStopped() const override;
        bool IsBusy() const override;
        bool IsTemporaryBreak() const {return m_TemporaryBreak;}
        int GetExitCode() const override { return m_LastExitCode; }

        cb::shared_ptr<cbWatch> AddWatch(const wxString& symbol, bool update) override;
        cb::shared_ptr<cbWatch> AddMemoryRange(uint64_t address, uint64_t size,
                                               const wxString &symbol, bool update) override;
        void DeleteWatch(cb::shared_ptr<cbWatch> watch) override;
        bool HasWatch(cb::shared_ptr<cbWatch> watch) override;
        bool IsMemoryRangeWatch(const cb::shared_ptr<cbWatch> &watch);
        void ShowWatchProperties(cb::shared_ptr<cbWatch> watch) override;
        bool SetWatchValue(cb::shared_ptr<cbWatch> watch, const wxString &value) override;
        void ExpandWatch(cb::shared_ptr<cbWatch> watch) override;
        void CollapseWatch(cb::shared_ptr<cbWatch> watch) override;
        void UpdateWatch(cb::shared_ptr<cbWatch> watch) override;
        void UpdateWatches(const std::vector<cb::shared_ptr<cbWatch>> &watches) override;

        void AddWatchNoUpdate(const cb::shared_ptr<GDBWatch> &watch);

        void OnWatchesContextMenu(wxMenu &menu, const cbWatch &watch, wxObject *property,
                                  int &disabledMenus) override;

        void GetCurrentPosition(wxString &filename, int &line) override;
        void RequestUpdate(DebugWindows window) override;

        void AttachToProcess(const wxString& pid) override;
        void DetachFromProcess() override;
        bool IsAttachedToProcess() const override;

        void SendCommand(const wxString& cmd, bool debugLog) override;
        void DoSendCommand(const wxString& cmd);

        DebuggerState& GetState(){ return m_State; }

        void OnConfigurationChange(bool isActive) override;

        static wxArrayString ParseSearchDirs(const cbProject &project);
        static void SetSearchDirs(cbProject &project, const wxArrayString &dirs);

        static RemoteDebuggingMap ParseRemoteDebuggingMap(cbProject &project);
        static void SetRemoteDebuggingMap(cbProject &project, const RemoteDebuggingMap &map);

        void OnValueTooltip(const wxString &token, const wxRect &evalRect) override;
        bool ShowValueTooltip(int style) override;

        static void ConvertToGDBFriendly(wxString& str);
        static void ConvertToGDBFile(wxString& str);
        static void ConvertToGDBDirectory(wxString& str, wxString base = _T(""), bool relative = true);
        static void StripQuotes(wxString& str);

        void DebuggeeContinued();

        void DetermineLanguage();

    protected:
        cbProject* GetProject() override { return m_pProject; }
        void ResetProject() override { m_pProcess = NULL; }
        void ConvertDirectory(wxString& str, wxString base, bool relative) override;
        void CleanupWhenProjectClosed(cbProject *project) override;
        bool CompilerFinished(bool compilerFailed, StartType startType) override;
    protected:
        void AddSourceDir(const wxString& dir);
    private:
        void ParseOutput(const wxString& output);
        void DoWatches();
        void MarkAllWatchesAsUnchanged();
        int LaunchProcess(const wxString& cmd, const wxString& cwd);
        int LaunchProcessWithShell(const wxString &cmd, wxProcess *process, const wxString &cwd);

        int DoDebug(bool breakOnEntry);
        void DoBreak(bool temporary);

        void OnAddSymbolFile(wxCommandEvent& event);
        void DeleteAllProjectBreakpoints(cbProject* project);
        void OnBuildTargetSelected(CodeBlocksEvent& event);
        void OnGDBOutput(wxCommandEvent& event);
        void OnGDBError(wxCommandEvent& event);
        void OnGDBTerminated(wxCommandEvent& event);
        void OnIdle(wxIdleEvent& event);
        void OnTimer(wxTimerEvent& event);
        void OnShowFile(wxCommandEvent& event);
        void OnCursorChanged(wxCommandEvent& event);

        void SetupToolsMenu(wxMenu &menu) override;
        void KillConsole();
        void CheckIfConsoleIsClosed();

        void OnInfoFrame(wxCommandEvent& event);
        void OnInfoDLL(wxCommandEvent& event);
        void OnInfoFiles(wxCommandEvent& event);
        void OnInfoFPU(wxCommandEvent& event);
        void OnInfoSignals(wxCommandEvent& event);

        void OnMenuWatchDereference(wxCommandEvent& event);

        void OnUpdateTools(wxUpdateUIEvent &event);
        void OnPrintElements(wxCommandEvent &event);

        void OnUpdateCatchThrow(wxUpdateUIEvent &event);
        void OnCatchThrow(wxCommandEvent &event);
    private:
        PipedProcess* m_pProcess;
        bool m_LastExitCode;
        int m_Pid;
        int m_PidToAttach; // for "attach to process"
        wxRect m_EvalRect;
        wxTimer m_TimerPollDebugger;
        bool m_NoDebugInfo;

        bool m_StoppedOnSignal;

        // extra dialogs
        cbProject* m_pProject; // keep the currently debugged project handy
        wxString m_ActiveBuildTarget;

        // Linux console support
        bool     m_bIsConsole;
        bool     m_stopDebuggerConsoleClosed;
        int      m_nConsolePid;

        bool m_Canceled; // flag to avoid re-entering DoDebug when we shouldn't

        bool m_TemporaryBreak;

        WatchesContainer m_watches;
        MemoryRangeWatchesContainer m_memoryRanges;
        MapWatchesToType m_mapWatchesToType;

        cb::shared_ptr<GDBWatch> m_localsWatch, m_funcArgsWatch;
        wxString m_watchToDereferenceSymbol;
        wxObject *m_watchToDereferenceProperty;

        friend struct TestIfBelongToProject;

        int m_printElements;

        DECLARE_EVENT_TABLE()
};

#endif // DEBUGGERGDB_H


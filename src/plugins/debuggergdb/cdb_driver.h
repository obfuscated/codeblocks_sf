/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef CDB_DRIVER_H
#define CDB_DRIVER_H

#include "debuggerdriver.h"

class CDB_driver : public DebuggerDriver
{
    public:
        CDB_driver(DebuggerGDB* plugin);
        ~CDB_driver() override;

        wxString GetCommandLine(const wxString& debugger, const wxString& debuggee,
                                const wxString &userArguments) override;
        wxString GetCommandLine(const wxString& debugger, int pid,
                                const wxString &userArguments) override;
        void SetTarget(ProjectBuildTarget* target) override;
        void Prepare(bool isConsole, int printElements,
                     const RemoteDebugging &remoteDebugging) override;
        void Start(bool breakOnEntry) override;
        void Stop() override;

        void Continue() override;
        void Step() override;
        void StepInstruction() override;
        void StepIntoInstruction() override;
        void StepIn() override;
        void StepOut() override;
        void SetNextStatement(const wxString& filename, int line) override;
        void Backtrace() override;
        void Disassemble() override;
        void CPURegisters() override;
        void SwitchToFrame(size_t number) override;
        void SetVarValue(const wxString& var, const wxString& value) override;
        void SetMemoryRangeValue(uint64_t addr, const wxString& value) override;
        void MemoryDump() override;
        void Attach(int pid) override;
        void Detach() override;
        void RunningThreads() override;

        void InfoFrame() override;
        void InfoDLL() override;
        void InfoFiles() override;
        void InfoFPU() override;
        void InfoSignals() override;

        void EnableCatchingThrow(bool enable) override;

        void SwitchThread(cb_unused size_t threadIndex) override {} // not implemented

        void AddBreakpoint(cb::shared_ptr<DebuggerBreakpoint> bp) override;
        void RemoveBreakpoint(cb::shared_ptr<DebuggerBreakpoint> bp) override;
        void EvaluateSymbol(const wxString& symbol, const wxRect& tipRect) override;
        void UpdateWatches(cb::shared_ptr<GDBWatch> localsWatch,
                           cb::shared_ptr<GDBWatch> funcArgsWatch,
                           WatchesContainer &watches, bool ignoreAutoUpdate) override;
        void UpdateWatch(cb::shared_ptr<GDBWatch> const &watch) override;
        void UpdateMemoryRangeWatches(MemoryRangeWatchesContainer &watches,
                                      bool ignoreAutoUpdate) override;
        void UpdateMemoryRangeWatch(const cb::shared_ptr<GDBMemoryRangeWatch> &watch) override;
        void UpdateWatchLocalsArgs(cb::shared_ptr<GDBWatch> const &watch, bool locals) override;
        void ParseOutput(const wxString& output) override;
        bool IsDebuggingStarted() const override;
#ifdef __WXMSW__
        bool UseDebugBreakProcess() override { return true; }
#endif
    protected:
    private:
        void DoBacktrace(bool switchToFirst);
        wxString GetCommonCommandLine(const wxString& debugger);
    private:
        ProjectBuildTarget *m_Target;
        bool m_IsStarted;
};

#endif // CDB_DRIVER_H

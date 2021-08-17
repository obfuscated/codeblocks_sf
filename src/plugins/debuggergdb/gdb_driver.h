/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef GDB_DRIVER_H
#define GDB_DRIVER_H

#include "debuggerdriver.h"
#include "remotedebugging.h"
#include <wx/regex.h>

class GDB_driver : public DebuggerDriver
{
    public:
        GDB_driver(DebuggerGDB* plugin);
        ~GDB_driver() override;

        wxString GetCommandLine(const wxString& debugger, const wxString& debuggee,
                                const wxString &userArguments) override;
        wxString GetCommandLine(const wxString& debugger, int pid,
                                const wxString &userArguments) override;
        void SetTarget(ProjectBuildTarget* target) override;
        void Prepare(bool isConsole, int printElements, const RemoteDebugging &remoteDebugging) override;
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

        void SwitchThread(size_t threadIndex) override;

        void AddBreakpoint(cb::shared_ptr<DebuggerBreakpoint> bp) override;
        void RemoveBreakpoint(cb::shared_ptr<DebuggerBreakpoint> bp) override;
        void EvaluateSymbol(const wxString& symbol, const wxRect& tipRect) override;
        void UpdateWatches(cb::shared_ptr<GDBWatch> localsWatch,
                           cb::shared_ptr<GDBWatch> funcArgsWatch, WatchesContainer &watches,
                           bool ignoreAutoUpdate) override;
        void UpdateMemoryRangeWatches(MemoryRangeWatchesContainer &watches,
                                      bool ignoreAutoUpdate) override;
        void UpdateWatch(const cb::shared_ptr<GDBWatch> &watch) override;
        void UpdateMemoryRangeWatch(const cb::shared_ptr<GDBMemoryRangeWatch> &watch) override;
        void UpdateWatchLocalsArgs(cb::shared_ptr<GDBWatch> const &watch, bool locals) override;
        void ParseOutput(const wxString& output) override;
        bool IsDebuggingStarted() const override { return m_IsStarted; }

        void DetermineLanguage() override;
#ifdef __WXMSW__
        bool UseDebugBreakProcess() override;
#endif
        virtual wxString GetDisassemblyFlavour(void);

        wxString AsmFlavour() {return flavour;}
    protected:
    private:
        void HandleMainBreakPoint(const wxRegEx& reBreak, wxString line);

        bool m_CygwinPresent;

        // Seems to be intended to allow step before program has started.
        // Was always false.  HC changed to take value from DebuggerGDB::m_BreakOnEntry.
        bool m_BreakOnEntry;

        // Seems to be used to issue a InfoProgram command, then continue
        // True after first "Start()", until first break
        bool m_ManualBreakOnEntry;

        // Program is "running": after a "run" or a "start", and before "kill" or a "quit"
        bool m_IsStarted;

        // cursor update flags
        bool m_needsUpdate;
        bool m_forceUpdate;

        // GDB version
        long m_GDBVersionMajor;
        long m_GDBVersionMinor;
        wxString flavour;

        bool m_attachedToProcess;

        // for remote debugging usage (mainly)
        ProjectBuildTarget* m_pTarget;

        // True if we're running remote debugging session.
        bool m_isRemoteDebugging;

        int m_catchThrowIndex;

}; // GDB_driver

#endif // GDB_DRIVER_H

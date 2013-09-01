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
        virtual ~CDB_driver();

        virtual wxString GetCommandLine(const wxString& debugger,
                                        const wxString& debuggee,
                                        const wxString &userArguments);
        virtual wxString GetCommandLine(const wxString& debugger, int pid, const wxString &userArguments);
        virtual void SetTarget(ProjectBuildTarget* target);
        virtual void Prepare(bool isConsole, int printElements);
        virtual void Start(bool breakOnEntry);
        virtual void Stop();

        virtual void Continue();
        virtual void Step();
        virtual void StepInstruction();
        virtual void StepIntoInstruction();
        virtual void StepIn();
        virtual void StepOut();
        virtual void SetNextStatement(const wxString& filename, int line);
        virtual void Backtrace();
        virtual void Disassemble();
        virtual void CPURegisters();
        virtual void SwitchToFrame(size_t number);
        virtual void SetVarValue(const wxString& var, const wxString& value);
        virtual void MemoryDump();
        virtual void Attach(int pid);
        virtual void Detach();
        virtual void RunningThreads();

        void InfoFrame();
        void InfoDLL();
        void InfoFiles();
        void InfoFPU();
        void InfoSignals();

        void EnableCatchingThrow(bool enable);

        virtual void SwitchThread(cb_unused size_t threadIndex){} // not implemented

        virtual void AddBreakpoint(cb::shared_ptr<DebuggerBreakpoint> bp);
        virtual void RemoveBreakpoint(cb::shared_ptr<DebuggerBreakpoint> bp);
        virtual void EvaluateSymbol(const wxString& symbol, const wxRect& tipRect);
        virtual void UpdateWatches(cb::shared_ptr<GDBWatch> localsWatch, cb::shared_ptr<GDBWatch> funcArgsWatch,
                                   WatchesContainer &watches);
        virtual void UpdateWatch(cb::shared_ptr<GDBWatch> const &watch);
        virtual void UpdateWatchLocalsArgs(cb::shared_ptr<GDBWatch> const &watch, bool locals);
        virtual void ParseOutput(const wxString& output);
        virtual bool IsDebuggingStarted() const;
#ifdef __WXMSW__
        virtual bool UseDebugBreakProcess() { return true; }
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

#ifndef GDB_DRIVER_H
#define GDB_DRIVER_H

#include "debuggerdriver.h"

class GDB_driver : public DebuggerDriver
{
    public:
        GDB_driver(DebuggerGDB* plugin);
        virtual ~GDB_driver();

        virtual wxString GetCommandLine(const wxString& debugger, const wxString& debuggee);
        virtual wxString GetCommandLine(const wxString& debugger, int pid);
        virtual void Prepare(bool isConsole);
        virtual void Start(bool breakOnEntry);
        virtual void Stop();

        virtual void Continue();
        virtual void Step();
        virtual void StepIn();
        virtual void StepOut();
        virtual void Backtrace();
        virtual void Disassemble();
        virtual void Detach();

        virtual void AddBreakpoint(DebuggerBreakpoint* bp);
        virtual void RemoveBreakpoint(DebuggerBreakpoint* bp, bool deleteAlso = false);
        virtual void EvaluateSymbol(const wxString& symbol, wxTipWindow** tipWin, const wxRect& tipRect);
        virtual void UpdateWatches(bool doLocals, bool doArgs, DebuggerTree* tree);
        virtual void ParseOutput(const wxString& output);
    protected:
    private:
};

#endif // GDB_DRIVER_H

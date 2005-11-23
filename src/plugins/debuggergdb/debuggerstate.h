#ifndef DEBUGGERSTATE_H
#define DEBUGGERSTATE_H

#include "debugger_defs.h"

class DebuggerGDB;
class ProjectBuildTarget;

class DebuggerState
{
    public:
        DebuggerState(DebuggerGDB* plugin);
        ~DebuggerState();

        BreakpointsList& GetBreakpoints(){ return m_Breakpoints; }

        bool StartDriver(ProjectBuildTarget* target);
        void StopDriver();
        DebuggerDriver* GetDriver();

        void CleanUp();

        int AddBreakpoint(DebuggerBreakpoint* bp); // returns -1 if not found
        int AddBreakpoint(const wxString& file, int line, bool temp = false); // returns -1 if not found
        DebuggerBreakpoint* RemoveBreakpoint(const wxString& file, int line, bool deleteit = true);
        DebuggerBreakpoint* RemoveBreakpoint(int idx, bool deleteit = true);
        int HasBreakpoint(const wxString& file, int line); // returns -1 if not found
        DebuggerBreakpoint* GetBreakpoint(int idx);
        void ResetBreakpoint(int idx);
        void ApplyBreakpoints();
    protected:
        DebuggerGDB* m_pPlugin;
        DebuggerDriver* m_pDriver;
        WatchesArray m_Watches;
        BreakpointsList m_Breakpoints;
};

#endif // DEBUGGERSTATE_H

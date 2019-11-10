/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef DEBUGGERSTATE_H
#define DEBUGGERSTATE_H

#include <wx/string.h>
#include "debugger_defs.h"

class DebuggerGDB;
class ProjectBuildTarget;
class cbProject;

class DebuggerState
{
    public:
        DebuggerState(DebuggerGDB* plugin);
        ~DebuggerState();

        BreakpointsList const & GetBreakpoints() const { return m_Breakpoints; }

        bool StartDriver(ProjectBuildTarget* target);
        void StopDriver();

        /// Check so see if Driver exists before getting it
        bool HasDriver() const;

        /// Will always return a driver, or throw a code assertion error
        // (to fix multiple bugs in use of GetDriver without checking return value)
        DebuggerDriver* GetDriver();
        const DebuggerDriver* GetDriver() const;

        void CleanUp();

        int AddBreakpoint(cb::shared_ptr<DebuggerBreakpoint> bp); // returns -1 if not found
        cb::shared_ptr<DebuggerBreakpoint> AddBreakpoint(const wxString& file, int line, bool temp = false,
                                                         const wxString& lineText = wxEmptyString);
        cb::shared_ptr<DebuggerBreakpoint> AddBreakpoint(const wxString& dataAddr, bool onRead = false,
                                                         bool onWrite = true);
        void RemoveBreakpoint(int idx, bool removeFromDriver = true);
        void RemoveBreakpoint(cb::shared_ptr<DebuggerBreakpoint> bp, bool removeFromDriver = true);
        void RemoveAllBreakpoints();
        void RemoveAllProjectBreakpoints(cbProject* prj);

        // helpers to keep in sync with the editors
        void ShiftBreakpoint(cb::shared_ptr<DebuggerBreakpoint> bp, int nroflines);

        int HasBreakpoint(const wxString& file, int line, bool temp); // returns -1 if not found
        cb::shared_ptr<DebuggerBreakpoint> GetBreakpoint(int idx);
        cb::shared_ptr<DebuggerBreakpoint> GetBreakpointByNumber(int num);
        const cb::shared_ptr<DebuggerBreakpoint> GetBreakpointByNumber(int num) const;
        void ResetBreakpoint(cb::shared_ptr<DebuggerBreakpoint> bp);
        void ApplyBreakpoints();
    protected:
        wxString ConvertToValidFilename(const wxString& filename);

        DebuggerGDB* m_pPlugin;
        DebuggerDriver* m_pDriver;
        BreakpointsList m_Breakpoints;
};

#endif // DEBUGGERSTATE_H

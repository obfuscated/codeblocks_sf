#ifndef DEBUGGERDRIVER_H
#define DEBUGGERDRIVER_H

#include "debugger_defs.h"
#include <wx/regex.h>
#include <wx/tipwin.h>
#include <globals.h>

#define NOT_IMPLEMENTED() DebugLog(wxString(_U(__PRETTY_FUNCTION__)) + _T(": Not implemented in driver"))

class DebuggerGDB;
class DebuggerTree;
class BacktraceDlg;
class DisassemblyDlg;
class Compiler;

WX_DEFINE_ARRAY(DebuggerCmd*, DebuggerCommands);

class DebuggerDriver
{
    public:
        /** The priority used when adding a command in the queue.
            Low appends the new command in the queue, i.e. last in the queue.
            High inserts the command at the top of the queue.
        */
        enum QueuePriority
        {
            Low = 0,
            High
        };

        DebuggerDriver(DebuggerGDB* plugin);
        virtual ~DebuggerDriver();

        void Log(const wxString& msg);
        void DebugLog(const wxString& msg);

        ////////////////////////////////
        // BEFORE PROCESS STARTS - BEGIN
        ////////////////////////////////

        /** Inform the driver about the plugin's available (not necessarily visible) debugging windows. */
        virtual void SetDebugWindows(BacktraceDlg* b, DisassemblyDlg* d);

        /** Add a directory in search list. */
        virtual void AddDirectory(const wxString& dir);

        /** Clear directories search list. */
        virtual void ClearDirectories();

        /** Set the working directory. */
        virtual void SetWorkingDirectory(const wxString& dir);

        /** Get the command-line to launch the debugger. */
        virtual wxString GetCommandLine(const wxString& debugger, const wxString& debuggee) = 0;

        /** Get the command-line to launch the debugger. */
        virtual wxString GetCommandLine(const wxString& debugger, int pid) = 0;

        /** Prepares the debugging process by setting up search dirs etc.
            @param isConsole If true, the debuggee is a console executable.
        */
        virtual void Prepare(bool isConsole) = 0;

        /** Begin the debugging process by launching a program. */
        virtual void Start(bool breakOnEntry) = 0;

        ////////////////////////////////
        // BEFORE PROCESS STARTS - END
        ////////////////////////////////

        /** Stop debugging. */
        virtual void Stop() = 0;

        virtual void Continue() = 0;
        virtual void Step() = 0;
        virtual void StepIn() = 0;
        virtual void StepOut() = 0;
        virtual void Backtrace() = 0;
        virtual void Disassemble() = 0;

        /** Add a breakpoint.
            @param bp The breakpoint to add.
        */
        virtual void AddBreakpoint(DebuggerBreakpoint* bp) = 0;

        /** Remove a breakpoint.
            @param bp The breakpoint to remove. If NULL, all reakpoints are removed.
            @param deleteAlso If true, @c bp will be deleted when done.
        */
        virtual void RemoveBreakpoint(DebuggerBreakpoint* bp, bool deleteAlso = false) = 0;

        /** Evaluate a symbol.
            @param symbol The symbol to evaluate.
            @param tipWin The wxTipWindow* variable's address.
            @param tipRect The rect to use for the tip window.
        */
        virtual void EvaluateSymbol(const wxString& symbol, wxTipWindow** tipWin, const wxRect& tipRect) = 0;

        /** Update watches.
            @param doLocals Display values of local variables.
            @param doArgs Display values of function arguments.
            @param tree The watches tree control.
        */
        virtual void UpdateWatches(bool doLocals, bool doArgs, DebuggerTree* tree) = 0;

        /** Detach from running process. */
        virtual void Detach() = 0;

        /** Parse debugger's output. */
        virtual void ParseOutput(const wxString& output) = 0;

        /** The driver should return true if it needs the plugin to keep
        temporary breakpoints in the breakpoints list, false if not. */
        virtual bool KeepTempBreakpoints() = 0;

        /** Is the program stopped? */
        virtual bool IsStopped(){ return m_ProgramIsStopped; }
        /** Has the cursor changed? (cursor is the currently executing line of code) */
        virtual bool HasCursorChanged(){ return m_CursorChanged; }
        /** Get stopped file. */
        virtual wxString GetStopFile(){ return m_StopFile; }
        /** Get stopped line. */
        virtual long int GetStopLine(){ return m_StopLine; }

		void QueueCommand(DebuggerCmd* dcmd, QueuePriority prio = Low); ///< add a command in the queue. The DebuggerCmd will be deleted automatically when finished.
		DebuggerCmd* CurrentCommand(); ///< returns the currently executing command
		void RunQueue(); ///< runs the next command in the queue, if it is idle
		void RemoveTopCommand(bool deleteIt = true); ///< removes the top command (it has finished)
		void ClearQueue(); ///< clears the queue
    protected:
        DebuggerGDB* m_pDBG;
        wxArrayString m_Dirs;
        wxString m_WorkingDir;
        bool m_ProgramIsStopped;
        bool m_CursorChanged;

        BacktraceDlg* m_pBacktrace;
        DisassemblyDlg* m_pDisassembly;
        wxString m_StopFile;
        long int m_StopLine;

		// commands
		DebuggerCommands m_DCmds;
		bool m_QueueBusy;
    private:
};

#endif // DEBUGGERDRIVER_H

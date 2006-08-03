#ifndef DEBUGGERDRIVER_H
#define DEBUGGERDRIVER_H

#include "debugger_defs.h"
#include <wx/regex.h>
#include <globals.h>

#define NOT_IMPLEMENTED() DebugLog(wxString(cbC2U(__PRETTY_FUNCTION__)) + _T(": Not implemented in driver"))

class DebuggerGDB;
class DebuggerTree;
class BacktraceDlg;
class DisassemblyDlg;
class CPURegistersDlg;
class ExamineMemoryDlg;
class ThreadsDlg;
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
        virtual void SetDebugWindows(BacktraceDlg* b,
                                    DisassemblyDlg* d,
                                    CPURegistersDlg* r,
                                    ExamineMemoryDlg* m,
                                    ThreadsDlg* t);

        /** Add a directory in search list. */
        virtual void AddDirectory(const wxString& dir);

        /** Clear directories search list. */
        virtual void ClearDirectories();

        /** Set the working directory. */
        virtual void SetWorkingDirectory(const wxString& dir);

        /** Set the execution arguments. */
        virtual void SetArguments(const wxString& args);

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
        virtual void StepInstruction() = 0;
        virtual void StepIn() = 0;
        virtual void StepOut() = 0;
        virtual void Backtrace() = 0;
        virtual void Disassemble() = 0;
        virtual void CPURegisters() = 0;
        virtual void SwitchToFrame(size_t number) = 0;
        virtual void SetVarValue(const wxString& var, const wxString& value) = 0;
        virtual void MemoryDump() = 0;
        virtual void RunningThreads() = 0;

        virtual void InfoFrame() = 0;
        virtual void InfoDLL() = 0;
        virtual void InfoFiles() = 0;
        virtual void InfoFPU() = 0;
        virtual void InfoSignals() = 0;

        /** Add a breakpoint.
            @param bp The breakpoint to add.
            @param editor The editor this breakpoint is set (might be NULL).
        */
        virtual void AddBreakpoint(DebuggerBreakpoint* bp) = 0;

        /** Remove a breakpoint.
            @param bp The breakpoint to remove. If NULL, all reakpoints are removed.
        */
        virtual void RemoveBreakpoint(DebuggerBreakpoint* bp) = 0;

        /** Evaluate a symbol.
            @param symbol The symbol to evaluate.
            @param tipRect The rect to use for the tip window.
        */
        virtual void EvaluateSymbol(const wxString& symbol, const wxRect& tipRect) = 0;

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

        /** Is the program stopped? */
        virtual bool IsStopped(){ return m_ProgramIsStopped; }
        /** Get debugger cursor. */
        virtual const Cursor& GetCursor() const { return m_Cursor; }
        /** Set child PID (debuggee's). Usually set by debugger commands. */
        virtual void SetChildPID(unsigned long pid) { m_ChildPID = pid; }
        /** Get the child's (debuggee's) PID. */
        virtual unsigned long GetChildPID() const { return m_ChildPID; }
        /** Request to switch to another thread. */
        virtual void SwitchThread(size_t threadIndex) = 0;

        void QueueCommand(DebuggerCmd* dcmd, QueuePriority prio = Low); ///< add a command in the queue. The DebuggerCmd will be deleted automatically when finished.
        DebuggerCmd* CurrentCommand(); ///< returns the currently executing command
        void RunQueue(); ///< runs the next command in the queue, if it is idle
        void RemoveTopCommand(bool deleteIt = true); ///< removes the top command (it has finished)
        void ClearQueue(); ///< clears the queue
    protected:
        /** Called by implementations to reset the cursor. */
        virtual void ResetCursor();
        /** Called by implementations to notify cursor changes. */
        virtual void NotifyCursorChanged();

        // the debugger plugin
        DebuggerGDB* m_pDBG;

        // convenience properties for starting up
        wxArrayString m_Dirs;
        wxString m_WorkingDir;
        wxString m_Args;

        // cursor related
        bool m_ProgramIsStopped;
        wxString m_LastCursorAddress;
        Cursor m_Cursor;

        unsigned long m_ChildPID;

        // debugging windows pointers
        BacktraceDlg* m_pBacktrace;
        DisassemblyDlg* m_pDisassembly;
        CPURegistersDlg* m_pCPURegisters;
        ExamineMemoryDlg* m_pExamineMemory;
        ThreadsDlg* m_pThreads;

        // commands
        DebuggerCommands m_DCmds;
        bool m_QueueBusy;
    private:
};

#endif // DEBUGGERDRIVER_H

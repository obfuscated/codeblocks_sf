/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef DEBUGGERDRIVER_H
#define DEBUGGERDRIVER_H

#ifndef WX_PRECOMP
#   ifdef __WXMSW__
#       include <wx/msw/wrapwin.h>  // Needed to prevent GetCommandLine define bug.
#   endif
#endif

#include "debugger_defs.h"
#include <wx/regex.h>
#include <globals.h>

#define NOT_IMPLEMENTED()   \
    do {                    \
        DebugLog(wxString(cbC2U(__PRETTY_FUNCTION__)) + _T(": Not implemented in driver"));     \
        Log(wxString(cbC2U(__PRETTY_FUNCTION__)) + _T(": Not implemented in driver"));           \
    } while(0)

class DebuggerGDB;
class Compiler;
class ProjectBuildTarget;

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
        typedef std::vector<cb::shared_ptr<cbStackFrame> > StackFrameContainer;
        typedef std::vector<cb::shared_ptr<cbThread> > ThreadsContainer;

        DebuggerDriver(DebuggerGDB* plugin);
        virtual ~DebuggerDriver();

        void Log(const wxString& msg);
        void DebugLog(const wxString& msg);

        DebuggerGDB* GetDebugger() { return m_pDBG; }

        ////////////////////////////////
        // BEFORE PROCESS STARTS - BEGIN
        ////////////////////////////////

        /** Add a directory in search list. */
        virtual void AddDirectory(const wxString& dir);

        /** Clear directories search list. */
        virtual void ClearDirectories();

        /** Set the working directory. */
        virtual void SetWorkingDirectory(const wxString& dir);

        /** Set the execution arguments. */
        virtual void SetArguments(const wxString& args);

        /** Get the command-line to launch the debugger. */
        virtual wxString GetCommandLine(const wxString& debugger,
                                        const wxString& debuggee,
                                        const wxString &userArguments) = 0;

        /** Get the command-line to launch the debugger. */
        virtual wxString GetCommandLine(const wxString& debugger, int pid, const wxString &userArguments) = 0;

        /** Sets the target */
        virtual void SetTarget(ProjectBuildTarget* target) = 0;

        /** Prepares the debugging process by setting up search dirs etc.
            @param isConsole If true, the debuggee is a console executable.
        */
        virtual void Prepare(bool isConsole, int printElements) = 0;

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
        virtual void StepIntoInstruction() = 0;
        virtual void StepIn() = 0;
        virtual void StepOut() = 0;
        virtual void SetNextStatement(const wxString& filename, int line) = 0;
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

        virtual void EnableCatchingThrow(bool enable) = 0;

        /** Add a breakpoint.
            @param bp The breakpoint to add.
            @param editor The editor this breakpoint is set (might be NULL).
        */
        virtual void AddBreakpoint(cb::shared_ptr<DebuggerBreakpoint> bp) = 0;

        /** Remove a breakpoint.
            @param bp The breakpoint to remove. If NULL, all reakpoints are removed.
        */
        virtual void RemoveBreakpoint(cb::shared_ptr<DebuggerBreakpoint> bp) = 0;

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
        virtual void UpdateWatches(cb::shared_ptr<GDBWatch> localsWatch, cb::shared_ptr<GDBWatch> funcArgsWatch,
                                   WatchesContainer &watches) = 0;
        virtual void UpdateWatch(cb::shared_ptr<GDBWatch> const &watch) = 0;

        /** Attach to process */
        virtual void Attach(int pid) = 0;
        /** Detach from running process. */
        virtual void Detach() = 0;

        /** Parse debugger's output. */
        virtual void ParseOutput(const wxString& output) = 0;

        /** Is debugging started */
        virtual bool IsDebuggingStarted() const = 0;
        /** Is the program stopped? */
        bool IsProgramStopped() const { return m_ProgramIsStopped; }
        void MarkProgramStopped(bool stopped) { m_ProgramIsStopped = stopped; }
        /** Is the driver processing some commands? */
        bool IsQueueBusy() const { return m_QueueBusy; }
        /** Set child PID (debuggee's). Usually set by debugger commands. */
        void SetChildPID(long pid) { m_ChildPID = pid; }
        /** Get the child's (debuggee's) PID. */
        long GetChildPID() const { return m_ChildPID; }
        /** Request to switch to another thread. */
        virtual void SwitchThread(size_t threadIndex) = 0;

#ifdef __WXMSW__
        /** Ask the driver if the debugger should be interrupted with DebugBreakProcess or Ctrl+C event */
        virtual bool UseDebugBreakProcess() = 0;
#endif
        wxString GetDebuggersWorkingDirectory() const;

        /** Show a file/line without changing the cursor */
        void ShowFile(const wxString& file, int line);

        void QueueCommand(DebuggerCmd* dcmd, QueuePriority prio = Low); ///< add a command in the queue. The DebuggerCmd will be deleted automatically when finished.
        DebuggerCmd* CurrentCommand(); ///< returns the currently executing command
        void RunQueue(); ///< runs the next command in the queue, if it is idle
        void RemoveTopCommand(bool deleteIt = true); ///< removes the top command (it has finished)

        const StackFrameContainer & GetStackFrames() const; ///< returns the container with the current backtrace
        StackFrameContainer & GetStackFrames(); ///< returns the container with the current backtrace

        const ThreadsContainer & GetThreads() const; ///< returns the thread container with the current list of threads
        ThreadsContainer & GetThreads(); ///< returns the thread container with the current list of threads

        /** Get debugger's cursor. */
        const Cursor& GetCursor() const { return m_Cursor; }
        /** Set debugger's cursor. */
        void SetCursor(const Cursor& cursor) { m_Cursor = cursor; }

        void ResetCurrentFrame();
        int GetCurrentFrame() const { return m_currentFrameNo; }
        int GetUserSelectedFrame() const { return m_userSelectedFrameNo; }
        void SetCurrentFrame(int number, bool user_selected);

        void NotifyDebuggeeContinued();
        /** Called by implementations to notify cursor changes. */
        void NotifyCursorChanged();
    protected:
        /** Called by implementations to reset the cursor. */
        void ResetCursor();
    protected:
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

        long m_ChildPID;

        // commands
        DebuggerCommands m_DCmds;
        bool m_QueueBusy;

        StackFrameContainer m_backtrace;
        ThreadsContainer m_threads;
        int m_currentFrameNo;
        int m_userSelectedFrameNo;
};

#endif // DEBUGGERDRIVER_H

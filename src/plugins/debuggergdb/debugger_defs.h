/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef DEBUGGER_DEFS_H
#define DEBUGGER_DEFS_H

#include <wx/string.h>
#include <wx/dynarray.h>

class DebuggerDriver;
class DebuggerTree;

extern const int DEBUGGER_CURSOR_CHANGED; ///< wxCommandEvent ID fired when the cursor has changed.

/** Debugger cursor info.
  *
  * Contains info about the debugger's cursor, i.e. where it currently is at
  * (file, line, function, address).
  */
struct Cursor
{
    Cursor() : line(-1), changed(false) {}
    wxString file;
    wxString address;
    wxString function;
    long int line; ///< If -1, no line info
    bool changed;
};

/** Basic interface for debugger commands.
  *
  * Each command sent to the debugger, is a DebuggerCmd.
  * It encapsulates the call and parsing of return values.
  * The most important function is ParseOutput() inside
  * which it must parse the commands output.
  * It is guaranteed that the @c output argument to ParseOutput()
  * contains the full debugger response to the command given.
  *
  * @remarks This is not an abstract interface, i.e. you can
  * create instances of it. The default implementation just
  * logs the command's output. This way you can debug new commands.
  */
class DebuggerCmd
{
    public:
        DebuggerCmd(DebuggerDriver* driver, const wxString& cmd = _T(""), bool logToNormalLog = false);
        virtual ~DebuggerCmd(){}

        /** Executes an action.
          *
          * This allows for "dummy" debugger commands to enter the commands queue.
          * You can, for example, leave m_Cmd empty and just have Action()
          * do something GUI-related (like the watches command does).
          * Action() is called when the debugger command becomes current in the
          * commands queue. It is called after sending m_Cmd to the debugger (if not empty).
          */
        virtual void Action(){}

        /** Parses the command's output.
          * @param The output. This is the full output up to
          * (and including) the prompt.
          */
        virtual void ParseOutput(const wxString& output);

        wxString m_Cmd;         ///< the actual command
    protected:
        DebuggerDriver* m_pDriver; ///< the driver
        bool m_LogToNormalLog;  ///< if true, log to normal log, else the debug log
};

/** This command is similar to DebuggerCmd
  * The only difference is that instead of logging its output in the debugger log,
  * it displays it in a dialog.
  */
class DebuggerInfoCmd : public DebuggerCmd
{
    public:
        DebuggerInfoCmd(DebuggerDriver* driver, const wxString& cmd, const wxString& title)
            : DebuggerCmd(driver, cmd),
            m_Title(title)
        {
            m_Cmd = cmd;
        }
        virtual ~DebuggerInfoCmd(){}

        virtual void ParseOutput(const wxString& output);
        wxString m_Title;
};

/** Action-only debugger comand to signal the watches tree to update. */
class DbgCmd_UpdateWatchesTree : public DebuggerCmd
{
    public:
        DbgCmd_UpdateWatchesTree(DebuggerDriver* driver, DebuggerTree* tree);
        virtual ~DbgCmd_UpdateWatchesTree(){}
        virtual void Action();
    protected:
        DebuggerTree* m_pTree;
};

/** Debugger breakpoint interface.
  *
  * This is the struct used for debugger breakpoints.
  */
////////////////////////////////////////////////////////////////////////////////
struct DebuggerBreakpoint
{
	enum BreakpointType
	{
		bptCode = 0,	///< Normal file/line breakpoint
		bptFunction,	///< Function signature breakpoint
		bptData			///< Data breakpoint
	};
	
    /** Constructor.
      * Sets default values for members.
      */
    DebuggerBreakpoint()
        : type(bptCode),
        line(0),
        index(-1),
        temporary(false),
        enabled(true),
        active(true),
        useIgnoreCount(false),
        ignoreCount(0),
        useCondition(false),
        address(0),
        alreadySet(false),
        breakOnRead(false),
        breakOnWrite(true),
        userData(0)
    {}
    BreakpointType type; ///< The type of this breakpoint.
    wxString filename; ///< The filename for the breakpoint (kept as relative).
    wxString filenameAsPassed; ///< The filename for the breakpoint as passed to the debugger (i.e. full filename).
    int line; ///< The line for the breakpoint.
    long int index; ///< The breakpoint number. Set automatically. *Don't* write to it.
    bool temporary; ///< Is this a temporary (one-shot) breakpoint?
    bool enabled; ///< Is the breakpoint enabled?
    bool active; ///< Is the breakpoint active? (currently unused)
    bool useIgnoreCount; ///< Should this breakpoint be ignored for the first X passes? (@c x == @c ignoreCount)
    int ignoreCount; ///< The number of passes before this breakpoint should hit. @c useIgnoreCount must be true.
    bool useCondition; ///< Should this breakpoint hit only if a specific condition is met?
    wxString condition; ///< The condition that must be met for the breakpoint to hit. @c useCondition must be true.
    wxString func; ///< The function to set the breakpoint. If this is set, it is preferred over the filename/line combination.
    unsigned long int address; ///< The actual breakpoint address. This is read back from the debugger. *Don't* write to it.
    bool alreadySet; ///< Is this already set? Used to mark temporary breakpoints for removal.
    wxString lineText; ///< Optionally, the breakpoint line's text (used by GDB for setting breapoints on ctors/dtors).
    wxString breakAddress; ///< Valid only for type==bptData: address to break when read/written.
    bool breakOnRead; ///< Valid only for type==bptData: break when memory is read from.
    bool breakOnWrite; ///< Valid only for type==bptData: break when memory is written to.
    void* userData; ///< Custom user data.
};
WX_DEFINE_ARRAY(DebuggerBreakpoint*, BreakpointsList);

/** Watch variable format.
  *
  * @note not all formats are implemented for all debugger drivers.
  */
enum WatchFormat
{
    Undefined = 0, ///< Format is undefined (whatever the debugger uses by default).
    Decimal, ///< Variable should be displayed as decimal.
    Unsigned, ///< Variable should be displayed as unsigned.
    Hex, ///< Variable should be displayed as hexadecimal (e.g. 0xFFFFFFFF).
    Binary, ///< Variable should be displayed as binary (e.g. 00011001).
    Char, ///< Variable should be displayed as a single character (e.g. 'x').
    Float, ///< Variable should be displayed as floating point number (e.g. 14.35)

    // do not remove these
    Last, ///< used for iterations
    Any ///< used for watches searches
};

/// How to parse strings passed in DebuggerTree::BuildTree()
enum WatchStringFormat
{
    wsfGDB, ///< GDB format
    wsfCDB ///< CDB/NTSD format
};

/** Watch variable.
  *
  * This is used to define debugger watch variables.
  */
struct Watch
{
    Watch(const wxString& k, WatchFormat f = Undefined) : keyword(k), format(f), is_array(false), array_start(0), array_count(0) {}
    Watch(const Watch& rhs) : keyword(rhs.keyword), format(rhs.format), is_array(rhs.is_array), array_start(rhs.array_start), array_count(rhs.array_count) {}
    wxString keyword; ///< The symbol to watch.
    WatchFormat format; ///< The format to use for display.
    bool is_array; ///< True if it is an array, false if not.
    size_t array_start; ///< The array start (valid for array types only).
    size_t array_count; ///< The array count (valid for array types only).
};
WX_DECLARE_OBJARRAY(Watch, WatchesArray);

/** Stack frame.
  *
  * This keeps info about a specific stack frame.
  */
struct StackFrame
{
    StackFrame() : valid(false), number(0), address(0) {}
    /** Clear everything. */
    void Clear()
    {
        valid = false;
        number = 0;
        address = 0;
        function.Clear();
        file.Clear();
        line.Clear();
    }
    bool valid; ///< Is this stack frame valid?
    unsigned long int number; ///< Stack frame's number (used in backtraces).
    unsigned long int address; ///< Stack frame's address.
    wxString function; ///< Current function name.
    wxString file; ///< Current file.
    wxString line; ///< Current line in file.
};

#endif // DEBUGGER_DEFS_H

#ifndef DEBUGGER_DEFS_H
#define DEBUGGER_DEFS_H

#include <wx/string.h>
#include <wx/dynarray.h>

class DebuggerDriver;

/** Basic interface for debugger commands.
  *
  * Each command sent to the debugger, is an DebuggerCmd.
  * It encapsulates the call and parsing of return values.
  * The most important function is ParseOutput() inside
  * which it must parse the commands output.
  *
  * @remarks This is not an abstract interface, i.e. you can
  * create instances of it. The default implementation just
  * logs the command's output. This way you can debug new commands.
  */
class DebuggerCmd
{
    public:
        DebuggerCmd(DebuggerDriver* driver, const wxString& cmd = _T(""), bool logToNormalLog = false);
        virtual ~DebuggerCmd();

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

////////////////////////////////////////////////////////////////////////////////
// Breakpoints
////////////////////////////////////////////////////////////////////////////////
struct DebuggerBreakpoint
{
    DebuggerBreakpoint()
        : index(-1),
        line(0),
        temporary(false),
        enabled(true),
        active(true),
        useIgnoreCount(false),
        ignoreCount(0),
        useCondition(false),
        address(0),
        bpNum(-1)
    {}
    wxString filename;
	int index; // index in list of breakpoints
	int line;
	bool temporary;
	bool enabled;
	bool active;
	bool useIgnoreCount;
	int ignoreCount;
	bool useCondition;
	wxString condition;
	wxString func;
	unsigned long int address; ///< actual break address

	long int bpNum;
};
WX_DEFINE_ARRAY(DebuggerBreakpoint*, BreakpointsList);

////////////////////////////////////////////////////////////////////////////////
// Watch variables
////////////////////////////////////////////////////////////////////////////////
enum WatchFormat
{
    Undefined = 0,
    Decimal,
    Unsigned,
    Hex,
    Binary,
    Char,

    // do not remove this
    Last,
    Any
};

struct Watch
{
    Watch(const wxString& k, WatchFormat f = Undefined) : keyword(k), format(f) {}
    Watch(const Watch& rhs) : keyword(rhs.keyword), format(rhs.format) {}
    static wxString FormatCommand(WatchFormat format)
    {
        switch (format)
        {
            case Decimal:       return _T("/d");
            case Unsigned:      return _T("/u");
            case Hex:           return _T("/x");
            case Binary:        return _T("/t");
            case Char:          return _T("/c");
            default:            return wxEmptyString;
        }
    }
    wxString keyword;
    WatchFormat format;
};
WX_DECLARE_OBJARRAY(Watch, WatchesArray);

////////////////////////////////////////////////////////////////////////////////
// Stack frames
////////////////////////////////////////////////////////////////////////////////
struct StackFrame
{
    StackFrame() : valid(false), number(0), address(0) {}
    void Clear()
    {
        valid = false;
        number = 0;
        address = 0;
        function.Clear();
        file.Clear();
        line.Clear();
    }
    bool valid;
    long int number;
    unsigned long int address;
    wxString function;
    wxString file;
    wxString line;
};

#endif // DEBUGGER_DEFS_H

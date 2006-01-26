#ifndef GDB_DRIVER_H
#define GDB_DRIVER_H

#include "debuggerdriver.h"
#include <wx/dynarray.h>
#include <wx/regex.h>

struct ScriptedType
{
    wxString name;
    wxString regex_str;
    wxRegEx regex;
    wxString parse_func;
    wxString print_func;

    wxString gdb_func; // set by the driver

    ScriptedType(){}
    ScriptedType(const ScriptedType& rhs)
    {
        name = rhs.name;
        regex_str = rhs.regex_str;
        parse_func = rhs.parse_func;
        print_func = rhs.print_func;
        gdb_func = rhs.gdb_func;

        regex.Compile(regex_str);
    }
};

WX_DECLARE_OBJARRAY(ScriptedType, TypesArray);

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
        virtual void CPURegisters();
        virtual void Detach();

        virtual void AddBreakpoint(DebuggerBreakpoint* bp);
        virtual void RemoveBreakpoint(DebuggerBreakpoint* bp);
        virtual void EvaluateSymbol(const wxString& symbol, wxTipWindow** tipWin, const wxRect& tipRect);
        virtual void UpdateWatches(bool doLocals, bool doArgs, DebuggerTree* tree);
        virtual void ParseOutput(const wxString& output);

        wxString GetScriptedTypeCommand(const wxString& gdb_type, wxString& parse_func);
    protected:
    private:
        void InitializeScripting();
        void RegisterType(const wxString& name, const wxString& regex, const wxString& parse_func, const wxString& print_func);

        TypesArray m_Types;
};

#endif // GDB_DRIVER_H

#ifndef GDB_DRIVER_H
#define GDB_DRIVER_H

#include "debuggerdriver.h"
#include <wx/dynarray.h>
#include <wx/regex.h>

struct ScriptedType
{
    wxString name;          // STL String
    wxString regex_str;     // [^[:alnum:]_]*string[^[:alnum:]_]*
    wxRegEx regex;
    wxString eval_func;     // Evaluate_StlString
    wxString parse_func;    // Parse_StlString

    ScriptedType(){}
    ScriptedType(const ScriptedType& rhs)
    {
        name = rhs.name;
        regex_str = rhs.regex_str;
        eval_func = rhs.eval_func;
        parse_func = rhs.parse_func;

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
        virtual void StepInstruction();
        virtual void StepIn();
        virtual void StepOut();
        virtual void Backtrace();
        virtual void Disassemble();
        virtual void CPURegisters();
        virtual void SwitchToFrame(size_t number);
        virtual void Detach();

        void InfoFrame();
        void InfoDLL();
        void InfoFiles();
        void InfoFPU();
        void InfoSignals();
        void InfoThreads();

        virtual void AddBreakpoint(DebuggerBreakpoint* bp);
        virtual void RemoveBreakpoint(DebuggerBreakpoint* bp);
        virtual void EvaluateSymbol(const wxString& symbol, wxTipWindow** tipWin, const wxRect& tipRect);
        virtual void UpdateWatches(bool doLocals, bool doArgs, DebuggerTree* tree);
        virtual void ParseOutput(const wxString& output);

        wxString GetScriptedTypeCommand(const wxString& gdb_type, wxString& parse_func);
    protected:
    private:
        void InitializeScripting();
        void RegisterType(const wxString& name, const wxString& regex, const wxString& eval_func, const wxString& parse_func);

        TypesArray m_Types;
};

#endif // GDB_DRIVER_H

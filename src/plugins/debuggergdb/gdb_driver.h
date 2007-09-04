#ifndef GDB_DRIVER_H
#define GDB_DRIVER_H

#include "debuggerdriver.h"
#include "remotedebugging.h"
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
        virtual void Prepare(ProjectBuildTarget* target, bool isConsole);
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
        virtual void SetVarValue(const wxString& var, const wxString& value);
        virtual void MemoryDump();
        virtual void Detach();
        virtual void RunningThreads();

        void InfoFrame();
        void InfoDLL();
        void InfoFiles();
        void InfoFPU();
        void InfoSignals();

        virtual void SwitchThread(size_t threadIndex);

        virtual void AddBreakpoint(DebuggerBreakpoint* bp);
        virtual void RemoveBreakpoint(DebuggerBreakpoint* bp);
        virtual void EvaluateSymbol(const wxString& symbol, const wxRect& tipRect);
        virtual void UpdateWatches(bool doLocals, bool doArgs, DebuggerTree* tree);
        virtual void ParseOutput(const wxString& output);
        virtual wxString GetDisassemblyFlavour(void);

        wxString GetScriptedTypeCommand(const wxString& gdb_type, wxString& parse_func);
    protected:
    private:
        void InitializeScripting();
        void RegisterType(const wxString& name, const wxString& regex, const wxString& eval_func, const wxString& parse_func);
        void HandleMainBreakPoint(const wxRegEx& reBreak, wxString line);

        // win/Cygwin platform checking
        void DetectCygwinMount(void);
        void CorrectCygwinPath(wxString& path);

        // remote debugging
        RemoteDebugging* GetRemoteDebuggingInfo();

        bool m_CygwinPresent;
        wxString m_CygdrivePrefix;

        TypesArray m_Types;

        // Seems to be intended to allow step before program has started.
        // Was always false.  HC changed to take value from DebuggerGDB::m_BreakOnEntry.
        bool m_BreakOnEntry;

        // Seems to be used to issue a InfoProgram command, then continue
        // True after first "Start()", until first break
        bool m_ManualBreakOnEntry;

        // Program is "running": after a "run" or a "start", and before "kill" or a "quit"
        bool m_IsStarted;

        // cursor update flags
        bool m_needsUpdate;
        bool m_forceUpdate;

        // GDB version
        long m_GDBVersionMajor;
        long m_GDBVersionMinor;
        wxString flavour;

        bool want_debug_events;
        bool disable_debug_events;

        // for remote debugging usage (mainly)
        ProjectBuildTarget* m_pTarget;

}; // GDB_driver

#endif // GDB_DRIVER_H

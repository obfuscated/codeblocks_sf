#include <sdk.h>
#include "gdb_driver.h"
#include "gdb_commands.h"
#include <manager.h>
#include <configmanager.h>
#include <scriptingmanager.h>
#include <globals.h>

#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY(TypesArray);

// the ">>>>>>" is a hack: sometimes, especially when watching uninitialized char*
// some random control codes in the stream (like 'delete') will mess-up our prompt and the debugger
// will seem like frozen (only "stop" button available). Using this dummy prefix,
// we allow for a few characters to be "eaten" this way and still get our
// expected prompt back.
#define GDB_PROMPT _T("cb_gdb:")
#define FULL_GDB_PROMPT _T(">>>>>>") + GDB_PROMPT

//[Switching to thread 2 (Thread 1082132832 (LWP 12298))]#0  0x00002aaaac5a2aca in pthread_cond_wait@@GLIBC_2.3.2 () from /lib/libpthread.so.0
static wxRegEx reThreadSwitch(_T("^\\[Switching to thread .*\\]#0[ \t]+(0x[A-z0-9]+) in (.*) from (.*)"));
static wxRegEx reThreadSwitch2(_T("^\\[Switching to thread .*\\]#0[ \t]+(0x[A-z0-9]+) in (.*) from (.*):([0-9]+)"));
#ifdef __WXMSW__
    static wxRegEx reBreak(_T("([A-z]:)([^:]+):([0-9]+):[0-9]+:[begmidl]+:(0x[0-9A-z]+)"));
#else
    static wxRegEx reBreak(_T("\032\032([^:]+):([0-9]+):[0-9]+:[begmidl]+:(0x[0-9A-z]+)"));
#endif
static wxRegEx reBreak2(_T("^(0x[A-z0-9]+) in (.*) from (.*)"));
static wxRegEx reBreak3(_T("^(0x[A-z0-9]+) in (.*)"));

// scripting support
DECLARE_INSTANCE_TYPE(GDB_driver);

GDB_driver::GDB_driver(DebuggerGDB* plugin)
    : DebuggerDriver(plugin),
    m_BreakOnEntry(false),
    m_ManualBreakOnEntry(false),
    m_GDBVersionMajor(0),
    m_GDBVersionMinor(0)
{
    //ctor
}

GDB_driver::~GDB_driver()
{
    //dtor
}

void GDB_driver::InitializeScripting()
{
    // get a pointer to scripting engine
    if (!SquirrelVM::GetVMPtr())
    {
        m_pDBG->Log(_("Scripting engine not running. Debugger scripts disabled..."));
        return; // no scripting support...
    }

    // create a new object type for scripts, named DebuggerDriver
    SqPlus::SQClassDef<GDB_driver>("GDB_driver").
            func(&GDB_driver::RegisterType, "RegisterType");

    // run extensions script
    wxString script = ConfigManager::LocateDataFile(_T("gdb_types.script"), sdScriptsUser | sdScriptsGlobal);
    if (!script.IsEmpty())
    {
        Manager::Get()->GetScriptingManager()->LoadScript(script);
        try
        {
            SqPlus::SquirrelFunction<void> f("RegisterTypes");
            f(this);
        }
        catch (SquirrelError e)
        {
            m_pDBG->Log(wxString::Format(_T("Invalid debugger script: '%s'"), script.c_str()));
            m_pDBG->Log(cbC2U(e.desc));

            Manager::Get()->GetScriptingManager()->DisplayErrors(&e);
        }
    }
}

void GDB_driver::RegisterType(const wxString& name, const wxString& regex, const wxString& eval_func, const wxString& parse_func)
{
    // check if this type already exists
    for (size_t i = 0; i < m_Types.GetCount(); ++i)
    {
        ScriptedType& st = m_Types[i];
        if (st.name == name)
            return; // exists already...
    }

    ScriptedType st;
    st.name = name;
    st.regex_str = regex;
    st.regex.Compile(regex);
    st.eval_func = eval_func;
    st.parse_func = parse_func;

    m_Types.Add(st);
    m_pDBG->Log(_("Registered new type: ") + st.name);
}

wxString GDB_driver::GetScriptedTypeCommand(const wxString& gdb_type, wxString& parse_func)
{
    for (size_t i = 0; i < m_Types.GetCount(); ++i)
    {
        ScriptedType& st = m_Types[i];
        if (st.regex.Matches(gdb_type))
        {
//            Log(_T("Function to print '") + gdb_type + _T("' is ") + st.parse_func);
            parse_func = st.parse_func;
            return st.eval_func;
        }
    }
    return _T("");
}

wxString GDB_driver::GetCommandLine(const wxString& debugger, const wxString& debuggee)
{
    wxString cmd;
    cmd << debugger;
    cmd << _T(" -nx");          // don't run .gdbinit
    cmd << _T(" -fullname ");   // report full-path filenames when breaking
    cmd << _T(" -quiet");       // don't display version on startup
    cmd << _T(" -args ") << debuggee;
    return cmd;
}

wxString GDB_driver::GetCommandLine(const wxString& debugger, int pid)
{
    wxString cmd;
    cmd << debugger;
    cmd << _T(" -nx");          // don't run .gdbinit
    cmd << _T(" -fullname ");   // report full-path filenames when breaking
    cmd << _T(" -quiet");       // don't display version on startup
    cmd << _T(" -pid=") << wxString::Format(_T("%d"), pid);
    return cmd;
}

void GDB_driver::Prepare(bool isConsole)
{
    // default initialization

    // make sure we 're using the prompt that we know and trust ;)
    QueueCommand(new DebuggerCmd(this, wxString(_T("set prompt ")) + FULL_GDB_PROMPT));

    // debugger version
    QueueCommand(new DebuggerCmd(this, _T("show version")));
    // no confirmation
    QueueCommand(new DebuggerCmd(this, _T("set confirm off")));
    // no wrapping lines
    QueueCommand(new DebuggerCmd(this, _T("set width 0")));
    // no pagination
    QueueCommand(new DebuggerCmd(this, _T("set height 0")));
    // allow pending breakpoints
    QueueCommand(new DebuggerCmd(this, _T("set breakpoint pending on")));
    // show pretty function names in disassembly
    QueueCommand(new DebuggerCmd(this, _T("set print asm-demangle on")));
    // unwind stack on signal
    QueueCommand(new DebuggerCmd(this, _T("set unwindonsignal on")));

    int disassembly_flavour = Manager::Get()->GetConfigManager(_T("debugger"))
                              ->ReadInt(_T("disassembly_flavor"), 0);

//    Manager::Get()->GetMessageManager()->Log(_("Flavor is: %d"), disassembly_flavour);

    wxString flavour = _T("set disassembly-flavor ");
    switch (disassembly_flavour)
    {
        case 1: // AT & T
        {
            flavour << _T("att");
            break;
        }
        case 2: // Intel
        {
            flavour << _T("intel");
            break;
        }
        case 3: // Custom
        {
            wxString instruction_set = Manager::Get()->GetConfigManager(_T("debugger"))
                                       ->Read(_T("instruction_set"), wxEmptyString);
            flavour << instruction_set;
            break;
        }
        default: // including case 0: // System default
#ifndef __WXMSW__
            flavour << _T("att");
#else
            flavour << _T("intel");
#endif
    }// switch

#ifdef __WXMSW__
    if (isConsole)
        QueueCommand(new DebuggerCmd(this, _T("set new-console on")));
#endif
    QueueCommand(new DebuggerCmd(this, flavour));

    // define all scripted types
    m_Types.Clear();
    InitializeScripting();

    // pass user init-commands
    wxString init = Manager::Get()->GetConfigManager(_T("debugger"))->Read(_T("init_commands"), wxEmptyString);
    // commands are passed in one go, in case the user defines functions in there
    // or else it would lock up...
    QueueCommand(new DebuggerCmd(this, init));
//    wxArrayString initCmds = GetArrayFromString(init, _T('\n'));
//    for (unsigned int i = 0; i < initCmds.GetCount(); ++i)
//    {
//        QueueCommand(new DebuggerCmd(this, initCmds[i]));
//    }

    // set working directory
    if (!m_WorkingDir.IsEmpty())
        QueueCommand(new DebuggerCmd(this, _T("cd ") + m_WorkingDir));

    // add search dirs
    for (unsigned int i = 0; i < m_Dirs.GetCount(); ++i)
    {
        QueueCommand(new GdbCmd_AddSourceDir(this, m_Dirs[i]));
    }

    // set arguments
    if (!m_Args.IsEmpty())
        QueueCommand(new DebuggerCmd(this, _T("set args ") + m_Args));
}

void GDB_driver::Start(bool breakOnEntry)
{
    ResetCursor();
    
    // reset other states
    GdbCmd_DisassemblyInit::LastAddr.Clear();
    if (m_pDisassembly)
    {
        StackFrame sf;
        m_pDisassembly->Clear(sf);
    }

    // under windows, 'start' segfaults with wx projects...
#if (defined( __WXMSW__) || defined(__WXMAC__))
    m_BreakOnEntry = false;
    m_ManualBreakOnEntry = false;

    if (!Manager::Get()->GetConfigManager(_T("debugger"))->ReadBool(_T("do_not_run"), false))
        // start the process
        QueueCommand(new DebuggerCmd(this, _T("run")));
#else
    m_BreakOnEntry = breakOnEntry;
    m_ManualBreakOnEntry = true;

    if (!Manager::Get()->GetConfigManager(_T("debugger"))->ReadBool(_T("do_not_run"), false))
        // start the process
        QueueCommand(new DebuggerCmd(this, _T("start")));
#endif
}

void GDB_driver::Stop()
{
    ResetCursor();
    QueueCommand(new DebuggerCmd(this, _T("quit")));
}

void GDB_driver::Continue()
{
    ResetCursor();
    QueueCommand(new DebuggerCmd(this, _T("cont")));
}

void GDB_driver::Step()
{
    ResetCursor();
    QueueCommand(new DebuggerCmd(this, _T("next")));
}

void GDB_driver::StepInstruction()
{
    ResetCursor();
    QueueCommand(new DebuggerCmd(this, _T("nexti")));
}

void GDB_driver::StepIn()
{
    ResetCursor();
    QueueCommand(new DebuggerCmd(this, _T("step")));
}

void GDB_driver::StepOut()
{
    ResetCursor();
    QueueCommand(new DebuggerCmd(this, _T("finish")));
}

void GDB_driver::Backtrace()
{
    if (!m_pBacktrace)
        return;
    QueueCommand(new GdbCmd_Backtrace(this, m_pBacktrace));
}

void GDB_driver::Disassemble()
{
    if (!m_pDisassembly)
        return;
    QueueCommand(new GdbCmd_DisassemblyInit(this, m_pDisassembly));
}

void GDB_driver::CPURegisters()
{
    if (!m_pCPURegisters)
        return;
    QueueCommand(new GdbCmd_InfoRegisters(this, m_pCPURegisters));
}

void GDB_driver::SwitchToFrame(size_t number)
{
    ResetCursor();
    QueueCommand(new DebuggerCmd(this, wxString::Format(_T("frame %d"), number)));
}

void GDB_driver::SetVarValue(const wxString& var, const wxString& value)
{
    QueueCommand(new DebuggerCmd(this, wxString::Format(_T("set variable %s=%s"), var.c_str(), value.c_str())));
}

void GDB_driver::MemoryDump()
{
    if (!m_pExamineMemory)
        return;
    QueueCommand(new GdbCmd_ExamineMemory(this, m_pExamineMemory));
}

void GDB_driver::RunningThreads()
{
    QueueCommand(new GdbCmd_Threads(this, m_pThreads));
}

void GDB_driver::InfoFrame()
{
    QueueCommand(new DebuggerInfoCmd(this, _T("info frame"), _("Selected frame")));
}

void GDB_driver::InfoDLL()
{
    QueueCommand(new DebuggerInfoCmd(this, _T("info dll"), _("Loaded libraries")));
}

void GDB_driver::InfoFiles()
{
    QueueCommand(new DebuggerInfoCmd(this, _T("info files"), _("Files and targets")));
}

void GDB_driver::InfoFPU()
{
    QueueCommand(new DebuggerInfoCmd(this, _T("info float"), _("Floating point unit")));
}

void GDB_driver::InfoSignals()
{
    QueueCommand(new DebuggerInfoCmd(this, _T("info signals"), _("Signals handling")));
}

void GDB_driver::SwitchThread(size_t threadIndex)
{
    ResetCursor();
    QueueCommand(new DebuggerCmd(this, wxString::Format(_T("thread %d"), threadIndex)));
}

void GDB_driver::AddBreakpoint(DebuggerBreakpoint* bp)
{
    //Workaround for GDB to break on C++ constructor/destructor
    if (bp->func.IsEmpty() && !bp->lineText.IsEmpty())
    {
        wxRegEx reCtorDtor(_T("([0-9A-z_]+)::([~]?)([0-9A-z_]+)[ \t\(]*"));
        if (reCtorDtor.Matches(bp->lineText))
        {
            wxString strBase = reCtorDtor.GetMatch(bp->lineText, 1);
            wxString strDtor = reCtorDtor.GetMatch(bp->lineText, 2);
            wxString strMethod = reCtorDtor.GetMatch(bp->lineText, 3);
            if (strBase.IsSameAs(strMethod))
            {
                bp->func = strBase;
                bp->func << _T("::");
                bp->func << strDtor;
                bp->func << strMethod;
//                if (bp->temporary)
//                    bp->temporary = false;
                NotifyCursorChanged(); // to force breakpoints window update
            }
        }
    }
    //end GDB workaround

    QueueCommand(new GdbCmd_AddBreakpoint(this, bp));
}

void GDB_driver::RemoveBreakpoint(DebuggerBreakpoint* bp)
{
    QueueCommand(new GdbCmd_RemoveBreakpoint(this, bp));
}

void GDB_driver::EvaluateSymbol(const wxString& symbol, const wxRect& tipRect)
{
    QueueCommand(new GdbCmd_FindTooltipType(this, symbol, tipRect));
}

void GDB_driver::UpdateWatches(bool doLocals, bool doArgs, DebuggerTree* tree)
{
    // start updating watches tree
    tree->BeginUpdateTree();

    // locals before args because of precedence
    if (doLocals)
        QueueCommand(new GdbCmd_InfoLocals(this, tree));
    if (doArgs)
        QueueCommand(new GdbCmd_InfoArguments(this, tree));
    for (unsigned int i = 0; i < tree->GetWatches().GetCount(); ++i)
    {
        Watch& w = tree->GetWatches()[i];
        if (w.format == Undefined)
            QueueCommand(new GdbCmd_FindWatchType(this, tree, &w));
        else
            QueueCommand(new GdbCmd_Watch(this, tree, &w));
    }

    // run this action-only command to update the tree
    QueueCommand(new DbgCmd_UpdateWatchesTree(this, tree));
}

void GDB_driver::Detach()
{
    QueueCommand(new GdbCmd_Detach(this));
}

void GDB_driver::ParseOutput(const wxString& output)
{
    m_Cursor.changed = false;
    if (output.StartsWith(_T("gdb: ")) ||
        output.StartsWith(_T("ContinueDebugEvent ")))
    {
        return;
    }
    static wxString buffer;
    buffer << output << _T('\n');

    m_pDBG->DebugLog(output);

    int idx = buffer.First(GDB_PROMPT);
    if (idx != wxNOT_FOUND)
    {
        m_ProgramIsStopped = true;
        m_QueueBusy = false;
        DebuggerCmd* cmd = CurrentCommand();
        if (cmd)
        {
//            Log(_T("Command parsing output: ") + buffer.Left(idx));
            RemoveTopCommand(false);
            buffer.Remove(idx);
            // remove the '>>>>>>' part of the prompt (or what's left of it)
            int cnt = 6; // max 6 '>'
            while (buffer.Last() == _T('>') && cnt--)
                buffer.RemoveLast();
            if (buffer.Last() == _T('\n'))
                buffer.RemoveLast();
            cmd->ParseOutput(buffer.Left(idx));
            delete cmd;
            RunQueue();
        }
    }
    else
    {
//        m_ProgramIsStopped = false;
        return; // come back later
    }

    bool needsUpdate = false;
    bool forceUpdate = false;

    // non-command messages (e.g. breakpoint hits)
    // break them up in lines
    wxArrayString lines = GetArrayFromString(buffer, _T('\n'));
    for (unsigned int i = 0; i < lines.GetCount(); ++i)
    {
//            Log(_T("DEBUG: ") + lines[i]); // write it in the full debugger log

        // log GDB's version
        if (lines[i].StartsWith(_T("GNU gdb")))
        {
            // it's the gdb banner. Just display the version and "eat" the rest
            m_pDBG->Log(_("Debugger name and version: ") + lines[i]);
            // keep major and minor version numbers handy
            wxString major = lines[i].Right(lines[i].Length() - 8);
            wxString minor = major;
            major = major.BeforeFirst(_T('.')); // 6.3.2 -> 6
            minor = minor.AfterFirst(_T('.')); // 6.3.2 -> 3.2
            minor = minor.BeforeFirst(_T('.')); // 3.2 -> 3
            major.ToLong(&m_GDBVersionMajor);
            minor.ToLong(&m_GDBVersionMinor);
//            wxString log;
//            log.Printf(_T("Line: %s\nMajor: %s (%d)\nMinor: %s (%d)"),
//                        lines[i].c_str(),
//                        major.c_str(),
//                        m_GDBVersionMajor,
//                        minor.c_str(),
//                        m_GDBVersionMinor);
//            m_pDBG->Log(log);
            break;
        }

        // Is the program exited?
        else if (lines[i].StartsWith(_T("Program exited")))
        {
            m_pDBG->Log(lines[i]);
            QueueCommand(new DebuggerCmd(this, _T("quit")));
        }

        // no debug symbols?
        else if (lines[i].Contains(_T("(no debugging symbols found)")))
        {
            m_pDBG->Log(lines[i]);
        }

        // signal
        else if (lines[i].StartsWith(_T("Program received signal SIG")) &&
        !( lines[i].StartsWith(_T("Program received signal SIGINT")) || lines[i].StartsWith(_T("Program received signal SIGTRAP"))) )
        {
            Log(lines[i]);
            m_pDBG->BringAppToFront();
            if (IsWindowReallyShown(m_pBacktrace))
            {
                // don't ask; it's already shown
                // just grab the user's attention
                cbMessageBox(lines[i], _("Signal received"), wxICON_ERROR);
            }
            else if (cbMessageBox(wxString::Format(_("%s\nDo you want to view the backtrace?"), lines[i].c_str()), _("Signal received"), wxICON_ERROR | wxYES_NO) == wxID_YES)
            {
                // show the backtrace window
                CodeBlocksDockEvent evt(cbEVT_SHOW_DOCK_WINDOW);
                evt.pWindow = m_pBacktrace;
                Manager::Get()->GetAppWindow()->ProcessEvent(evt);
                forceUpdate = true;
            }
            needsUpdate = true;
            // the backtrace will be generated when NotifyPlugins() is called
            // and only if the backtrace window is shown
        }

        // general errors
        // we don't deal with them, just relay them back to the user
        else if (lines[i].StartsWith(_T("Error ")) ||
                lines[i].StartsWith(_T("Cannot evaluate")))
        {
            m_pDBG->Log(lines[i]);
        }

        // pending breakpoint resolved?
        // e.g.
        // Pending breakpoint "C:/Devel/codeblocks/trunk/src/sdk/cbproject.cpp:332" resolved
        else if (lines[i].StartsWith(_T("Pending breakpoint ")))
        {
            m_pDBG->Log(lines[i]);
        }

        // cursor change
        else if (lines[i].StartsWith(g_EscapeChars)) // ->->
        {
            // breakpoint, e.g.
            // C:/Devel/tmp/test_console_dbg/tmp/main.cpp:14:171:beg:0x401428
            if ( reBreak.Matches(lines[i]) )
            {
                if (m_ManualBreakOnEntry)
                {
                    m_ManualBreakOnEntry = false;
                    QueueCommand(new GdbCmd_InfoProgram(this), DebuggerDriver::High);
                    if (!m_BreakOnEntry)
                        Continue();
                }
                else
                {
                #ifdef __WXMSW__
                    m_Cursor.file = reBreak.GetMatch(lines[i], 1) + reBreak.GetMatch(lines[i], 2);
                    wxString lineStr = reBreak.GetMatch(lines[i], 3);
                    m_Cursor.address = reBreak.GetMatch(lines[i], 4);
                #else
                    m_Cursor.file = reBreak.GetMatch(lines[i], 1);
                    wxString lineStr = reBreak.GetMatch(lines[i], 2);
                    m_Cursor.address = reBreak.GetMatch(lines[i], 3);
                #endif
                    lineStr.ToLong(&m_Cursor.line);
                    m_Cursor.changed = true;
                    needsUpdate = true;
                }
            }
        }
        else
        {
            // other break info, e.g.
            // 0x7c9507a8 in ntdll!KiIntSystemCall () from C:\WINDOWS\system32\ntdll.dll
            wxRegEx* re = 0;
            if ( reBreak2.Matches(lines[i]) )
                re = &reBreak2;
            else if (reThreadSwitch.Matches(lines[i]))
                re = &reThreadSwitch;

            if ( re )
            {
                m_Cursor.file = re->GetMatch(lines[i], 3);
                m_Cursor.function = re->GetMatch(lines[i], 2);
                wxString lineStr = _T("");
                m_Cursor.address = re->GetMatch(lines[i], 1);
                m_Cursor.line = -1;
                m_Cursor.changed = true;
                needsUpdate = true;
            }
            else if ( reThreadSwitch2.Matches(lines[i]) )
            {
                m_Cursor.file = reThreadSwitch2.GetMatch(lines[i], 3);
                m_Cursor.function = reThreadSwitch2.GetMatch(lines[i], 2);
                wxString lineStr = reThreadSwitch2.GetMatch(lines[i], 4);
                m_Cursor.address = reThreadSwitch2.GetMatch(lines[i], 1);
                m_Cursor.line = -1;
                m_Cursor.changed = true;
                needsUpdate = true;
            }
            else if (reBreak3.Matches(lines[i]) )
            {
                m_Cursor.file=_T("");
                m_Cursor.function= reBreak3.GetMatch(lines[i], 2);
                m_Cursor.address = reBreak3.GetMatch(lines[i], 1);
                m_Cursor.line = -1;
                m_Cursor.changed = true;
                needsUpdate = true;
            }
        }
    }
    buffer.Clear();

    // if program is stopped, update various states
    if (needsUpdate)
    {
        if (m_Cursor.changed || forceUpdate)
            NotifyCursorChanged();
    }
}

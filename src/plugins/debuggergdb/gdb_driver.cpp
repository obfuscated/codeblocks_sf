/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#include "gdb_driver.h"
#include "gdb_commands.h"
#include "debuggeroptionsdlg.h"
#include "debuggerstate.h"
#include <cbdebugger_interfaces.h>
#include <manager.h>
#include <macrosmanager.h>
#include <configmanager.h>
#include <scriptingmanager.h>
#include <globals.h>
#include <infowindow.h>

#ifdef __WXMSW__
// for Registry detection of Cygwin
#include <windows.h>
#endif

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
static wxRegEx reThreadSwitch(_T("^\\[Switching to thread .*\\]#0[ \t]+(0x[A-Fa-f0-9]+) in (.*) from (.*)"));
static wxRegEx reThreadSwitch2(_T("^\\[Switching to thread .*\\]#0[ \t]+(0x[A-Fa-f0-9]+) in (.*) from (.*):([0-9]+)"));
#ifdef __WXMSW__
    static wxRegEx reBreak(_T("([A-Za-z]*[:]*)([^:]+):([0-9]+):[0-9]+:[begmidl]+:(0x[0-9A-Fa-f]+)"));
    static wxRegEx reBreak_or32(_T("\032\032([A-Za-z]:)([^:]+):([0-9]+):[0-9]+:[begmidl]+:(0x[0-9A-z]+)"));
#else
    static wxRegEx reBreak(_T("\032\032([^:]+):([0-9]+):[0-9]+:[begmidl]+:(0x[0-9A-Fa-f]+)"));
    static wxRegEx reBreak_or32(_T("")); // not used on linux, but make sure it exists otherwise compilation fails on linux (if (platform::windows) blabla)
#endif
static wxRegEx reBreak2(_T("^(0x[A-Fa-f0-9]+) in (.*) from (.*)"));
static wxRegEx reBreak3(_T("^(0x[A-Fa-f0-9]+) in (.*)"));
// Catchpoint 1 (exception thrown), 0x00007ffff7b982b0 in __cxa_throw () from /usr/lib/gcc/x86_64-pc-linux-gnu/4.4.4/libstdc++.so.6
static wxRegEx reCatchThrow(_T("^Catchpoint ([0-9]+) \\(exception thrown\\), (0x[0-9a-f]+) in (.+) from (.+)$"));
// Catchpoint 1 (exception thrown), 0x00401610 in __cxa_throw ()
static wxRegEx reCatchThrowNoFile(_T("^Catchpoint ([0-9]+) \\(exception thrown\\), (0x[0-9a-f]+) in (.+)$"));

// easily match cygwin paths
//static wxRegEx reCygwin(_T("/cygdrive/([A-Za-z])/"));

// Pending breakpoint "C:/Devel/libs/irr_svn/source/Irrlicht/CSceneManager.cpp:1077" resolved
#ifdef __WXMSW__
static wxRegEx rePendingFound(_T("^Pending[ \t]+breakpoint[ \t]+[\"]+([A-Za-z]:)([^:]+):([0-9]+)\".*"));
#else
static wxRegEx rePendingFound(_T("^Pending[ \t]+breakpoint[ \t]+[\"]+([^:]+):([0-9]+)\".*"));
#endif
// Breakpoint 2, irr::scene::CSceneManager::getSceneNodeFromName (this=0x3fa878, name=0x3fbed8 "MainLevel", start=0x3fa87c) at CSceneManager.cpp:1077
static wxRegEx rePendingFound1(_T("^Breakpoint[ \t]+([0-9]+),.*"));

// Temporary breakpoint 2, main () at /path/projects/tests/main.cpp:136
static wxRegEx reTempBreakFound(wxT("^[Tt]emporary[ \t]breakpoint[ \t]([0-9]+),.*"));


// [Switching to Thread -1234655568 (LWP 18590)]
// [New Thread -1234655568 (LWP 18590)]
static wxRegEx reChildPid1(_T("Thread[ \t]+[xA-Fa-f0-9-]+[ \t]+\\(LWP ([0-9]+)\\)]"));
// MinGW GDB 6.8 and later
// [New Thread 2684.0xf40] or [New thread 2684.0xf40]
static wxRegEx reChildPid2(_T("\\[New [tT]hread[ \t]+[0-9]+\\.[xA-Fa-f0-9-]+\\]"));
static wxRegEx reAttachedChildPid(wxT("Attaching to process ([0-9]+)"));

static wxRegEx reInferiorExited(wxT("^\\[Inferior[ \\t].+[ \\t]exited normally\\]$"), wxRE_EXTENDED);
static wxRegEx reInferiorExitedWithCode(wxT("^\\[[Ii]nferior[ \\t].+[ \\t]exited[ \\t]with[ \\t]code[ \\t]([0-9]+)\\]$"), wxRE_EXTENDED);

// scripting support
DECLARE_INSTANCE_TYPE(GDB_driver);
using SqPlus::Push;

GDB_driver::GDB_driver(DebuggerGDB* plugin) :
    DebuggerDriver(plugin),
    m_CygwinPresent(false),
    m_BreakOnEntry(false),
    m_ManualBreakOnEntry(false),
    m_IsStarted(false),
    m_GDBVersionMajor(0),
    m_GDBVersionMinor(0),
    m_attachedToProcess(false),
    m_catchThrowIndex(-1)
{
    //ctor
    m_needsUpdate = false;
    m_forceUpdate = false;
}

GDB_driver::~GDB_driver()
{
    //dtor
}

void GDB_driver::InitializeScripting()
{
    if (!m_pDBG->GetActiveConfigEx().GetFlag(DebuggerConfiguration::WatchScriptPrinters))
    {
        m_pDBG->DebugLog(_("Skip initializing the scripting!"));
        return;
    }

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

    wxString StlDebugCommand(_T("source $DATAPATH/scripts/stl-views-1.0.3.gdb"));
    Manager::Get()->GetMacrosManager()->ReplaceMacros(StlDebugCommand);
    QueueCommand(new DebuggerCmd(this, StlDebugCommand));
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

wxString GDB_driver::GetCommandLine(const wxString& debugger, const wxString& debuggee, const wxString &userArguments)
{
    wxString cmd;
    cmd << debugger;
    cmd << _T(" -nx");          // don't run .gdbinit
    cmd << _T(" -fullname ");   // report full-path filenames when breaking
    cmd << _T(" -quiet");       // don't display version on startup
    cmd << wxT(" ") << userArguments;
    cmd << _T(" -args ") << debuggee;
    return cmd;
}

wxString GDB_driver::GetCommandLine(const wxString& debugger, cb_unused int pid, const wxString &userArguments)
{
    wxString cmd;
    cmd << debugger;
    cmd << _T(" -nx");          // don't run .gdbinit
    cmd << _T(" -fullname ");   // report full-path filenames when breaking
    cmd << _T(" -quiet");       // don't display version on startup
    cmd << wxT(" ") << userArguments;
    return cmd;
}

void GDB_driver::SetTarget(ProjectBuildTarget* target)
{
    // init for remote debugging
    m_pTarget = target;
}

void GDB_driver::Prepare(bool isConsole, int printElements)
{
    // default initialization

    // for the possibility that the program to be debugged is compiled under Cygwin
    if (platform::windows)
        DetectCygwinMount();

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
    // disalbe result string truncations
    QueueCommand(new DebuggerCmd(this, wxString::Format(wxT("set print elements %d"), printElements)));

    if (platform::windows && isConsole)
        QueueCommand(new DebuggerCmd(this, _T("set new-console on")));

    flavour = m_pDBG->GetActiveConfigEx().GetDisassemblyFlavorCommand();
    QueueCommand(new DebuggerCmd(this, flavour));

    if (m_pDBG->GetActiveConfigEx().GetFlag(DebuggerConfiguration::CatchExceptions))
    {
        m_catchThrowIndex = -1;
        // catch exceptions
        QueueCommand(new GdbCmd_SetCatch(this, wxT("throw"), &m_catchThrowIndex));
    }

    // define all scripted types
    m_Types.Clear();
    InitializeScripting();

    // pass user init-commands
    wxString init = m_pDBG->GetActiveConfigEx().GetInitCommands();
    Manager::Get()->GetMacrosManager()->ReplaceMacros(init);
    // commands are passed in one go, in case the user defines functions in there
    // or else it would lock up...
    QueueCommand(new DebuggerCmd(this, init));
//    wxArrayString initCmds = GetArrayFromString(init, _T('\n'));
//    for (unsigned int i = 0; i < initCmds.GetCount(); ++i)
//    {
//        QueueCommand(new DebuggerCmd(this, initCmds[i]));
//    }

    // add search dirs
    for (unsigned int i = 0; i < m_Dirs.GetCount(); ++i)
        QueueCommand(new GdbCmd_AddSourceDir(this, m_Dirs[i]));

    // set arguments
    if (!m_Args.IsEmpty())
        QueueCommand(new DebuggerCmd(this, _T("set args ") + m_Args));

    RemoteDebugging* rd = GetRemoteDebuggingInfo();

    // send additional gdb commands before establishing remote connection
    if (rd)
    {
        if (!rd->additionalCmdsBefore.IsEmpty())
        {
            wxArrayString initCmds = GetArrayFromString(rd->additionalCmdsBefore, _T('\n'));
            for (unsigned int i = 0; i < initCmds.GetCount(); ++i)
                QueueCommand(new DebuggerCmd(this, initCmds[i]));
        }
    }

    // if performing remote debugging, now is a good time to try and connect to the target :)
    if (rd && rd->IsOk())
    {
        if (rd->connType == RemoteDebugging::Serial)
            QueueCommand(new GdbCmd_RemoteBaud(this, rd->serialBaud));
        QueueCommand(new GdbCmd_RemoteTarget(this, rd));
    }

    // run per-target additional commands (remote debugging)
    // moved after connection to remote target (if any)
    if (rd)
    {
        if (!rd->additionalCmds.IsEmpty())
        {
            wxArrayString initCmds = GetArrayFromString(rd->additionalCmds, _T('\n'));
            for (unsigned int i = 0; i < initCmds.GetCount(); ++i)
                QueueCommand(new DebuggerCmd(this, initCmds[i]));
        }
    }
}

// remote debugging
RemoteDebugging* GDB_driver::GetRemoteDebuggingInfo()
{
//    if (!m_pTarget)
//        return 0;

    // first, project-level (straight copy)
    m_MergedRDInfo = m_pDBG->GetRemoteDebuggingMap()[0];

    // then merge with target settings
    RemoteDebuggingMap::iterator it = m_pDBG->GetRemoteDebuggingMap().find(m_pTarget);
    if (it != m_pDBG->GetRemoteDebuggingMap().end())
    {
        m_MergedRDInfo.MergeWith(it->second);
    }
    return &m_MergedRDInfo;
}

// Cygwin check code
#ifdef __WXMSW__

enum{ BUFSIZE = 64 };

// routines to handle cygwin compiled programs on a Windows compiled C::B IDE
void GDB_driver::DetectCygwinMount(void)
{
    LONG lRegistryAPIresult;
    HKEY hKey_CU;
    HKEY hKey_LM;
    TCHAR szCygwinRoot[BUFSIZE];
    DWORD dwBufLen=BUFSIZE*sizeof(TCHAR);

    // checking if cygwin mounts are present under HKCU
    lRegistryAPIresult = RegOpenKeyEx( HKEY_CURRENT_USER,
                         TEXT("Software\\Cygnus Solutions\\Cygwin\\mounts v2"),
                         0, KEY_QUERY_VALUE, &hKey_CU );
    if ( lRegistryAPIresult == ERROR_SUCCESS )
    {
        // try to readback cygwin root (might not exist!)
        lRegistryAPIresult = RegQueryValueEx( hKey_CU, TEXT("cygdrive prefix"), NULL, NULL,
                             (LPBYTE) szCygwinRoot, &dwBufLen);
    }

    // lRegistryAPIresult can be erroneous for two reasons:
    // 1.) Cygwin entry is not present (could not be opened) in HKCU
    // 2.) "cygdrive prefix" is not present (could not be read) in HKCU
    if ( lRegistryAPIresult != ERROR_SUCCESS )
    {
        // Now check if probably present under HKLM
        lRegistryAPIresult = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                             TEXT("SOFTWARE\\Cygnus Solutions\\Cygwin\\mounts v2"),
                             0, KEY_QUERY_VALUE, &hKey_LM );
        if ( lRegistryAPIresult != ERROR_SUCCESS )
        {
            // cygwin definitely not installed
            m_CygwinPresent = false;
            return;
        }

        // try to readback cygwin root (now it really should exist here)
        lRegistryAPIresult = RegQueryValueEx( hKey_LM, TEXT("cygdrive prefix"), NULL, NULL,
                             (LPBYTE) szCygwinRoot, &dwBufLen);
    }

    // handle a possible query error
    if ( (lRegistryAPIresult != ERROR_SUCCESS) || (dwBufLen > BUFSIZE*sizeof(TCHAR)) )
    {
        // bit of an assumption, but we won't be able to find the root without it
        m_CygwinPresent = false;
        return;
    }

    // close opened keys
    RegCloseKey( hKey_CU ); // ignore key close errors
    RegCloseKey( hKey_LM ); // ignore key close errors

    m_CygwinPresent  = true;           // if we end up here all was OK
    m_CygdrivePrefix = (szCygwinRoot); // convert to wxString type for later use
}

void GDB_driver::CorrectCygwinPath(wxString& path)
{
    unsigned int i=0, EscCount=0;

    // preserve any escape characters at start of path - this is true for
    // breakpoints - value is 2, but made dynamic for safety as we
    // are only checking for the CDprefix not any furthur correctness
    if (path.GetChar(0)==g_EscapeChar)
    {
        while ( (i<path.Len()) && (path.GetChar(i)==g_EscapeChar) )
        {
            // get character
            EscCount++;
            i++;
        }
    }

    // prepare to convert to a valid path if Cygwin is being used

    // step over the escape characters
    wxString PathWithoutEsc(path); PathWithoutEsc.Remove(0, EscCount);

    if (PathWithoutEsc.StartsWith(m_CygdrivePrefix))
    {
        // remove cygwin prefix
        if (m_CygdrivePrefix.EndsWith(_T("/"))) // for the case   "/c/path"
          PathWithoutEsc.Remove(0, m_CygdrivePrefix.Len()  );
        else                                    // for cases e.g. "/cygdrive/c/path"
          PathWithoutEsc.Remove(0, m_CygdrivePrefix.Len()+1);

        // insert ':' after drive label by reading and removing drive the label
        // and adding ':' and the drive label back
        wxString DriveLetter = PathWithoutEsc.GetChar(0);
        PathWithoutEsc.Replace(DriveLetter, DriveLetter + _T(":"), false);
    }

    // Compile corrected path
    path = wxEmptyString;
    for (i=0; i<EscCount; i++)
        path += g_EscapeChar;
    path += PathWithoutEsc;
}
#else
    void GDB_driver::DetectCygwinMount(void){/* dummy */}
    void GDB_driver::CorrectCygwinPath(cb_unused wxString& path){/* dummy */}
#endif

#ifdef __WXMSW__
bool GDB_driver::UseDebugBreakProcess()
{
    RemoteDebugging* rd = GetRemoteDebuggingInfo();
    bool remoteDebugging = rd && rd->IsOk();
    return !remoteDebugging;
}
#endif

wxString GDB_driver::GetDisassemblyFlavour(void)
{
    return flavour;
}

// Only called from DebuggerGDB::Debug
// breakOnEntry was always false.  Changed by HC.
void GDB_driver::Start(bool breakOnEntry)
{
    m_attachedToProcess = false;
    ResetCursor();

    // reset other states
    GdbCmd_DisassemblyInit::Clear();
    if (Manager::Get()->GetDebuggerManager()->UpdateDisassembly())
    {
        cbDisassemblyDlg *disassembly_dialog = Manager::Get()->GetDebuggerManager()->GetDisassemblyDialog();
        disassembly_dialog->Clear(cbStackFrame());
    }

    // if performing remote debugging, use "continue" command
    RemoteDebugging* rd = GetRemoteDebuggingInfo();
    bool remoteDebugging = rd && rd->IsOk();

    m_BreakOnEntry = breakOnEntry && !remoteDebugging;

    if (!m_pDBG->GetActiveConfigEx().GetFlag(DebuggerConfiguration::DoNotRun))
    {
        m_ManualBreakOnEntry = !remoteDebugging;
        // start the process
        if (breakOnEntry)
            QueueCommand(new GdbCmd_Start(this, remoteDebugging ? _T("continue") : _T("start")));
        else
        {
            // if breakOnEntry is not set, we need to use 'run' to make gdb stop at a breakpoint at first instruction
            m_ManualBreakOnEntry=false;  // must be reset or gdb does not stop at first breakpoint
            QueueCommand(new GdbCmd_Start(this, remoteDebugging ? _T("continue") : _T("run")));
        }
        m_IsStarted = true;
    }
} // Start

void GDB_driver::Stop()
{
    ResetCursor();
    if (m_pDBG->IsAttachedToProcess())
        QueueCommand(new DebuggerCmd(this, wxT("kill")));
    QueueCommand(new DebuggerCmd(this, _T("quit")));
    m_IsStarted = false;
    m_attachedToProcess = false;
}

void GDB_driver::Continue()
{
    ResetCursor();
    if (m_IsStarted)
        QueueCommand(new GdbCmd_Continue(this));
    else
    {
        // if performing remote debugging, use "continue" command
        RemoteDebugging* rd = GetRemoteDebuggingInfo();
        bool remoteDebugging = rd && rd->IsOk();
        if (remoteDebugging)
            QueueCommand(new GdbCmd_Continue(this));
        else
            QueueCommand(new GdbCmd_Start(this, m_ManualBreakOnEntry ? wxT("start") : wxT("run")));
        m_ManualBreakOnEntry = false;
        m_IsStarted = true;
        m_attachedToProcess = false;
    }
}

void GDB_driver::Step()
{
    ResetCursor();
    QueueCommand(new DebuggerContinueBaseCmd(this, _T("next")));
}

void GDB_driver::StepInstruction()
{
    ResetCursor();
    QueueCommand(new GdbCmd_StepInstruction(this));
}

void GDB_driver::StepIntoInstruction()
{
    ResetCursor();
    QueueCommand(new GdbCmd_StepIntoInstruction(this));
}

void GDB_driver::StepIn()
{
    ResetCursor();
    QueueCommand(new DebuggerContinueBaseCmd(this, _T("step")));
}

void GDB_driver::StepOut()
{
    ResetCursor();
    QueueCommand(new DebuggerContinueBaseCmd(this, _T("finish")));
}

void GDB_driver::SetNextStatement(const wxString& filename, int line)
{
    ResetCursor();
    QueueCommand(new DebuggerCmd(this, wxString::Format(wxT("tbreak %s:%d"), filename.c_str(), line)));
    QueueCommand(new DebuggerContinueBaseCmd(this, wxString::Format(wxT("jump %s:%d"), filename.c_str(), line)));
}

void GDB_driver::Backtrace()
{
    QueueCommand(new GdbCmd_Backtrace(this));
}

void GDB_driver::Disassemble()
{
    if (platform::windows)
        QueueCommand(new GdbCmd_DisassemblyInit(this, flavour));
    else
        QueueCommand(new GdbCmd_DisassemblyInit(this));
}

void GDB_driver::CPURegisters()
{
    if (platform::windows)
        QueueCommand(new GdbCmd_InfoRegisters(this, flavour));
    else
        QueueCommand(new GdbCmd_InfoRegisters(this));
}

void GDB_driver::SwitchToFrame(size_t number)
{
    ResetCursor();
    QueueCommand(new DebuggerCmd(this, wxString::Format(_T("frame %lu"), static_cast<unsigned long>(number))));
}

void GDB_driver::SetVarValue(const wxString& var, const wxString& value)
{
    const wxString &cleanValue=CleanStringValue(value);
    QueueCommand(new DebuggerCmd(this, wxString::Format(_T("set variable %s=%s"), var.c_str(), cleanValue.c_str())));
}

void GDB_driver::MemoryDump()
{
    QueueCommand(new GdbCmd_ExamineMemory(this));
}

void GDB_driver::RunningThreads()
{
    if (Manager::Get()->GetDebuggerManager()->UpdateThreads())
        QueueCommand(new GdbCmd_Threads(this));
}

void GDB_driver::InfoFrame()
{
    QueueCommand(new DebuggerInfoCmd(this, _T("info frame"), _("Selected frame")));
}

void GDB_driver::InfoDLL()
{
    if (platform::windows)
        QueueCommand(new DebuggerInfoCmd(this, _T("info dll"), _("Loaded libraries")));
    else
        QueueCommand(new DebuggerInfoCmd(this, _T("info sharedlibrary"), _("Loaded libraries")));
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

void GDB_driver::EnableCatchingThrow(bool enable)
{
    if (enable)
        QueueCommand(new GdbCmd_SetCatch(this, wxT("throw"), &m_catchThrowIndex));
    else if (m_catchThrowIndex != -1)
    {
        QueueCommand(new DebuggerCmd(this, wxString::Format(wxT("delete %d"), m_catchThrowIndex)));
        m_catchThrowIndex = -1;
    }
}

void GDB_driver::SwitchThread(size_t threadIndex)
{
    ResetCursor();
    QueueCommand(new DebuggerCmd(this, wxString::Format(_T("thread %lu"), static_cast<unsigned long>(threadIndex))));
    if (Manager::Get()->GetDebuggerManager()->UpdateBacktrace())
        QueueCommand(new GdbCmd_Backtrace(this));
}

void GDB_driver::AddBreakpoint(cb::shared_ptr<DebuggerBreakpoint> bp)
{
    if (bp->type == DebuggerBreakpoint::bptData)
        QueueCommand(new GdbCmd_AddDataBreakpoint(this, bp));
    //Workaround for GDB to break on C++ constructor/destructor
    else
    {
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
}

void GDB_driver::RemoveBreakpoint(cb::shared_ptr<DebuggerBreakpoint> bp)
{
    if (bp && bp->index != -1)
        QueueCommand(new GdbCmd_RemoveBreakpoint(this, bp));
}

void GDB_driver::EvaluateSymbol(const wxString& symbol, const wxRect& tipRect)
{
    QueueCommand(new GdbCmd_FindTooltipType(this, symbol, tipRect));
}

void GDB_driver::UpdateWatches(cb_unused bool doLocals, cb_unused bool doArgs, WatchesContainer &watches)
{
    // FIXME (obfuscated#): add local and argument watches
    // FIXME : remove cb_unused from params when that's done

    for (WatchesContainer::iterator it = watches.begin(); it != watches.end(); ++it)
        QueueCommand(new GdbCmd_FindWatchType(this, *it));

    // run this action-only command to update the tree
    QueueCommand(new DbgCmd_UpdateWatchesTree(this));
}

void GDB_driver::UpdateWatch(const cb::shared_ptr<GDBWatch> &watch)
{
    QueueCommand(new GdbCmd_FindWatchType(this, watch));
    QueueCommand(new DbgCmd_UpdateWatchesTree(this));
}

void GDB_driver::Attach(int pid)
{
    m_IsStarted = true;
    m_attachedToProcess = true;
    SetChildPID(pid);
    QueueCommand(new GdbCmd_AttachToProcess(this, pid));
}

void GDB_driver::Detach()
{
    QueueCommand(new GdbCmd_Detach(this));
}

void GDB_driver::ParseOutput(const wxString& output)
{
    m_Cursor.changed = false;

    if (platform::windows && m_ChildPID == 0)
    {
        if (reChildPid2.Matches(output)) // [New Thread 2684.0xf40] or [New thread 2684.0xf40]
        {
            wxString pidStr = reChildPid2.GetMatch(output, 0);
            pidStr = pidStr.BeforeFirst(_T('.')); //[New Thread 2684.0xf40] -> [New Thread 2684
            pidStr = pidStr.AfterFirst(_T('d')); //[New Thread 2684 ->  2684
            long pid = 0;
            pidStr.ToLong(&pid);
            SetChildPID(pid);
            m_pDBG->Log(wxString::Format(_("Child process PID: %ld"), pid));
        }
    }
    else if (!platform::windows && m_ChildPID == 0)
    {
        if (reChildPid1.Matches(output)) // [Switching to Thread -1234655568 (LWP 18590)]
        {
            wxString pidStr = reChildPid1.GetMatch(output, 1);
            long pid = 0;
            pidStr.ToLong(&pid);
            SetChildPID(pid);
            m_pDBG->Log(wxString::Format(_("Child process PID: %ld"), pid));
        }
    }

    if (   output.StartsWith(_T("gdb: "))
        || output.StartsWith(_T("warning: "))
        || output.StartsWith(_T("Warning: "))
        || output.StartsWith(_T("ContinueDebugEvent ")) )
    {
        return;
    }

    static wxString buffer;
    buffer << output << _T('\n');

    m_pDBG->DebugLog(output);

    int idx = buffer.First(GDB_PROMPT);
    if (idx == wxNOT_FOUND)
    {
        // don't uncomment the following line
        // m_ProgramIsStopped is set to false in DebuggerDriver::RunQueue()
//        m_ProgramIsStopped = false;
        return; // come back later
    }

    m_QueueBusy = false;
    int changeFrameAddr = 0 ;
    DebuggerCmd* cmd = CurrentCommand();
    if (cmd)
    {
//        DebugLog(wxString::Format(_T("Command parsing output (cmd: %s): %s"), cmd->m_Cmd.c_str(), buffer.Left(idx).c_str()));
        RemoveTopCommand(false);
        buffer.Remove(idx);
        // remove the '>>>>>>' part of the prompt (or what's left of it)
        int cnt = 6; // max 6 '>'
        while (buffer.Last() == _T('>') && cnt--)
            buffer.RemoveLast();
        if (buffer.Last() == _T('\n'))
            buffer.RemoveLast();
        cmd->ParseOutput(buffer.Left(idx));

        //We do NOT want default output processing for a changed frame as it can result
        //in disassembly being done for a non-current location, since some of the frame
        //response lines are in the pattern of breakpoint output.
        GdbCmd_ChangeFrame *changeFrameCmd = dynamic_cast<GdbCmd_ChangeFrame*>(cmd);
        if (changeFrameCmd)
            changeFrameAddr = changeFrameCmd->AddrChgMode();

        delete cmd;
        RunQueue();
    }

    m_needsUpdate = false;
    m_forceUpdate = false;

    // non-command messages (e.g. breakpoint hits)
    // break them up in lines

    wxArrayString lines = GetArrayFromString(buffer, _T('\n'));
    for (unsigned int i = 0; i < lines.GetCount(); ++i)
    {
//            Log(_T("DEBUG: ") + lines[i]); // write it in the full debugger log

        // Check for possibility of a cygwin compiled program
        // convert to valid path
        if (platform::windows && m_CygwinPresent)
            CorrectCygwinPath(lines.Item(i));

        // log GDB's version
        if (lines[i].StartsWith(_T("GNU gdb")))
        {
            // it's the gdb banner. Just display the version and "eat" the rest
            m_pDBG->Log(_("Debugger name and version: ") + lines[i]);
            // keep major and minor version numbers handy
            wxRegEx re(_T("([0-9.]+)"));
            if (!re.Matches(lines[i]))
            {
                m_pDBG->Log(_T("Unable to determine the version of gdb"));
                break;
            }
            wxString major = re.GetMatch(lines[i],0);
            wxString minor = major;
            major = major.BeforeFirst(_T('.')); // 6.3.2 -> 6
            minor = minor.AfterFirst(_T('.'));  // 6.3.2 -> 3.2
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
        else if (   lines[i].StartsWith(_T("Error creating process"))
                 || lines[i].StartsWith(_T("Program exited"))
                 || lines[i].StartsWith(wxT("Program terminated with signal"))
                 || lines[i].StartsWith(wxT("During startup program exited"))
                 || lines[i].Contains(_T("program is not being run"))
                 || lines[i].Contains(_T("Target detached"))
                 || reInferiorExited.Matches(lines[i])
                 || reInferiorExitedWithCode.Matches(lines[i]) )
        {
            m_pDBG->Log(lines[i]);
            m_ProgramIsStopped = true;
            QueueCommand(new DebuggerCmd(this, _T("quit")));
            m_IsStarted = false;
        }

        // no debug symbols?
        else if (lines[i].Contains(_T("(no debugging symbols found)")))
            m_pDBG->Log(lines[i]);

        // signal
        else if (lines[i].StartsWith(_T("Program received signal SIG")))
        {
            m_ProgramIsStopped = true;
            m_QueueBusy = false;

            if (   lines[i].StartsWith(_T("Program received signal SIGINT"))
                || lines[i].StartsWith(_T("Program received signal SIGTRAP"))
                || lines[i].StartsWith(_T("Program received signal SIGSTOP")) )
            {
                // these are break/trace signals, just log them
                Log(lines[i]);
            }
            else
            {
                Log(lines[i]);
                m_pDBG->BringCBToFront();

                if (Manager::Get()->GetDebuggerManager()->ShowBacktraceDialog())
                    m_forceUpdate = true;

                InfoWindow::Display(_("Signal received"), _T("\n\n") + lines[i] + _T("\n\n"));
                m_needsUpdate = true;
                // the backtrace will be generated when NotifyPlugins() is called
                // and only if the backtrace window is shown
            }
        }

        // general errors
        // we don't deal with them, just relay them back to the user
        else if (   lines[i].StartsWith(_T("Error "))
                 || lines[i].StartsWith(_T("No such"))
                 || lines[i].StartsWith(_T("Cannot evaluate")) )
        {
            m_pDBG->Log(lines[i]);
        }

        else if (   (lines[i].StartsWith(_T("Cannot find bounds of current function")))
                 || (lines[i].StartsWith(_T("No stack"))) )
        {
            m_pDBG->Log(lines[i]);
            m_ProgramIsStopped = true;
        }

        // pending breakpoint resolved?
        // e.g.
        // Pending breakpoint "C:/Devel/libs/irr_svn/source/Irrlicht/CSceneManager.cpp:1077" resolved
        // Breakpoint 2, irr::scene::CSceneManager::getSceneNodeFromName (this=0x3fa878, name=0x3fbed8 "MainLevel", start=0x3fa87c) at CSceneManager.cpp:1077
        else if (lines[i].StartsWith(_T("Pending breakpoint ")))
        {
            m_pDBG->Log(lines[i]);

            // we face a problem here:
            // gdb sets a *new* breakpoint when the pending address is resolved.
            // this means we must update the breakpoint index we have stored
            // or else we can never remove this (because the breakpoint index doesn't match)...

            // Pending breakpoint "C:/Devel/libs/irr_svn/source/Irrlicht/CSceneManager.cpp:1077" resolved
            wxString bpstr = lines[i];

            if (rePendingFound.Matches(bpstr))
            {
                // there are cases where 'newbpstr' is not the next message
                // e.g. [Switching to thread...]
                // so we 'll loop over lines starting with [

                // Breakpoint 2, irr::scene::CSceneManager::getSceneNodeFromName (this=0x3fa878, name=0x3fbed8 "MainLevel", start=0x3fa87c) at CSceneManager.cpp:1077
                wxString newbpstr = lines[++i];
                while (i < lines.GetCount() - 1 && newbpstr.StartsWith(_T("[")))
                    newbpstr = lines[++i];

                if (rePendingFound1.Matches(newbpstr))
                {
//                    m_pDBG->Log(_T("MATCH"));

                    wxString file;
                    wxString lineStr;

                    if (platform::windows)
                    {
                        file = rePendingFound.GetMatch(bpstr, 1) + rePendingFound.GetMatch(bpstr, 2);
                        lineStr = rePendingFound.GetMatch(bpstr, 3);
                    }
                    else
                    {
                        file = rePendingFound.GetMatch(bpstr, 1);
                        lineStr = rePendingFound.GetMatch(bpstr, 2);
                    }

                    file = UnixFilename(file);
    //                m_pDBG->Log(wxString::Format(_T("file: %s, line: %s"), file.c_str(), lineStr.c_str()));
                    long line;
                    lineStr.ToLong(&line);
                    DebuggerState& state = m_pDBG->GetState();
                    int bpindex = state.HasBreakpoint(file, line - 1, false);
                    cb::shared_ptr<DebuggerBreakpoint> bp = state.GetBreakpoint(bpindex);
                    if (bp)
                    {
    //                    m_pDBG->Log(_T("Found BP!!! Updating index..."));
                        long index;
                        wxString indexStr = rePendingFound1.GetMatch(newbpstr, 1);
                        indexStr.ToLong(&index);
                        // finally! update the breakpoint index
                        bp->index = index;
                    }
                }
            }
        }

        else if (lines[i].StartsWith(wxT("Breakpoint ")))
        {
            if (rePendingFound1.Matches(lines[i]))
            {
                long index;
                rePendingFound1.GetMatch(lines[i],1).ToLong(&index);
                DebuggerState& state = m_pDBG->GetState();
                cb::shared_ptr<DebuggerBreakpoint> bp = state.GetBreakpointByNumber(index);
                if (bp && bp->wantsCondition)
                {
                    bp->wantsCondition = false;
                    QueueCommand(new GdbCmd_AddBreakpointCondition(this, bp));
                    m_needsUpdate = true;
                }
            }
        }

        else if (lines[i].StartsWith(wxT("Temporary breakpoint")))
        {
            if (reTempBreakFound.Matches(lines[i]))
            {
                long index;
                reTempBreakFound.GetMatch(lines[i],1).ToLong(&index);
                DebuggerState& state = m_pDBG->GetState();
                cb::shared_ptr<DebuggerBreakpoint> bp = state.GetBreakpointByNumber(index);
                state.RemoveBreakpoint(bp, false);
                Manager::Get()->GetDebuggerManager()->GetBreakpointDialog()->Reload();
            }
        }

        // cursor change
        else if (lines[i].StartsWith(g_EscapeChar)) // ->->
        {
            // breakpoint, e.g.
            // C:/Devel/tmp/test_console_dbg/tmp/main.cpp:14:171:beg:0x401428

            // Main breakpoint handler is wrapped into a function so we can use
            // the same code with different regular expressions - depending on
            // the platform.

            //NOTE: This also winds up matching response to a frame command which is generated as
            //part of a backtrace with autoswitch enabled, (from gdb7.2 mingw) as in:
            //(win32, x86, mingw gdb 7.2)
            //>>>>>>cb_gdb:
            //> frame 1
            //#1  0x6f826722 in wxInitAllImageHandlers () at ../../src/common/imagall.cpp:29
            //^Z^ZC:\dev\wxwidgets\wxWidgets-2.8.10\build\msw/../../src/common/imagall.cpp:29:961:beg:0x6f826722
            //>>>>>>cb_gdb:

            if (platform::windows && flavour.IsSameAs(_T("set disassembly-flavor or32")))
                HandleMainBreakPoint(reBreak_or32, lines[i]);
            else
                HandleMainBreakPoint(reBreak, lines[i]);
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
                m_needsUpdate = true;
            }
            else if ( reThreadSwitch2.Matches(lines[i]) )
            {
                m_Cursor.file = reThreadSwitch2.GetMatch(lines[i], 3);
                m_Cursor.function = reThreadSwitch2.GetMatch(lines[i], 2);
                wxString lineStr = reThreadSwitch2.GetMatch(lines[i], 4);
                m_Cursor.address = reThreadSwitch2.GetMatch(lines[i], 1);
                m_Cursor.line = -1;
                m_Cursor.changed = true;
                m_needsUpdate = true;
            }
            else if (reBreak3.Matches(lines[i]) )
            {
                m_Cursor.file=_T("");
                m_Cursor.function= reBreak3.GetMatch(lines[i], 2);
                m_Cursor.address = reBreak3.GetMatch(lines[i], 1);
                m_Cursor.line = -1;
                m_Cursor.changed = true;
                m_needsUpdate = true;
            }
            else if (reCatchThrow.Matches(lines[i]) )
            {
                m_Cursor.file = reCatchThrow.GetMatch(lines[i], 4);
                m_Cursor.function= reCatchThrow.GetMatch(lines[i], 3);
                m_Cursor.address = reCatchThrow.GetMatch(lines[i], 2);
                m_Cursor.line = -1;
                m_Cursor.changed = true;
                m_needsUpdate = true;
            }
            else if (reCatchThrowNoFile.Matches(lines[i]) )
            {
                m_Cursor.file = wxEmptyString;
                m_Cursor.function= reCatchThrowNoFile.GetMatch(lines[i], 3);
                m_Cursor.address = reCatchThrowNoFile.GetMatch(lines[i], 2);
                m_Cursor.line = -1;
                m_Cursor.changed = true;
                m_needsUpdate = true;
            }
        }
    }
    buffer.Clear();

    // if program is stopped, update various states
    if (m_needsUpdate)
    {
        if (1 == changeFrameAddr)
        {
            // clear to avoid change of disassembly address on (auto) frame change
            // when NotifyCursorChanged() executes
            m_Cursor.address.clear();
        }
        if (m_Cursor.changed)
        {
            m_ProgramIsStopped = true;
            m_QueueBusy = false;
        }
        if (m_Cursor.changed || m_forceUpdate)
            NotifyCursorChanged();
    }

    if (m_ProgramIsStopped)
        RunQueue();
}


void GDB_driver::HandleMainBreakPoint(const wxRegEx& reBreak_in, wxString line)
{
    if ( reBreak_in.Matches(line) )
    {
        if (m_ManualBreakOnEntry)
            QueueCommand(new GdbCmd_InfoProgram(this), DebuggerDriver::High);

        if (m_ManualBreakOnEntry && !m_BreakOnEntry)
            Continue();
        else
        {
            m_ManualBreakOnEntry = false;
            wxString lineStr;
            if (platform::windows)
            {
                m_Cursor.file = reBreak_in.GetMatch(line, 1) + reBreak_in.GetMatch(line, 2);
                lineStr = reBreak_in.GetMatch(line, 3);
                m_Cursor.address = reBreak_in.GetMatch(line, 4);
            }
            else
            {
                m_Cursor.file = reBreak_in.GetMatch( line, 1);
                lineStr = reBreak_in.GetMatch(line, 2);
                m_Cursor.address = reBreak_in.GetMatch( line, 3);
            }

            lineStr.ToLong(&m_Cursor.line);
            m_Cursor.changed = true;
            m_needsUpdate = true;
        }
    }
    else
    {
        m_pDBG->Log(_("The program has stopped on a breakpoint but the breakpoint format is not recognized:"));
        m_pDBG->Log(line);
        m_Cursor.changed = true;
        m_needsUpdate = true;
    }
}

#include <sdk.h>
#include "gdb_driver.h"
#include "gdb_commands.h"
#include "debuggerstate.h"
#include <manager.h>
#include <configmanager.h>
#include <scriptingmanager.h>
#include <globals.h>

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

// easily match cygwin paths
//static wxRegEx reCygwin(_T("/cygdrive/([A-Za-z])/"));

// Pending breakpoint "C:/Devel/libs/irr_svn/source/Irrlicht/CSceneManager.cpp:1077" resolved
#ifdef __WXMSW__
static wxRegEx rePendingFound(_T("^Pending[ \t]+breakpoint[ \t]+\"([A-Za-z]:)([^:]+):([0-9]+)\".*"));
#else
static wxRegEx rePendingFound(_T("^Pending[ \t]+breakpoint[ \t]+\"([^:]+):([0-9]+)\".*"));
#endif
// Breakpoint 2, irr::scene::CSceneManager::getSceneNodeFromName (this=0x3fa878, name=0x3fbed8 "MainLevel", start=0x3fa87c) at CSceneManager.cpp:1077
static wxRegEx rePendingFound1(_T("^Breakpoint[ \t]+([0-9]+),.*"));

// gdb: do_initial_child_stuff: process 1392
static wxRegEx reChildPid(_T("gdb: do_initial_child_stuff: process ([0-9]+)"));


// scripting support
DECLARE_INSTANCE_TYPE(GDB_driver);

GDB_driver::GDB_driver(DebuggerGDB* plugin)
    : DebuggerDriver(plugin),
    m_CygwinPresent(false),
    m_BreakOnEntry(false),
    m_ManualBreakOnEntry(false),
	m_IsStarted(false),
    m_GDBVersionMajor(0),
    m_GDBVersionMinor(0),
    want_debug_events(true),
    disable_debug_events(false)
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

    // for the possibility that the program to be debugged is compiled under Cygwin
    if(platform::windows)
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

    // want debug events
    if(platform::windows)
    {
        QueueCommand(new DebuggerCmd(this, _T("set debugevents on")));
        want_debug_events = true;
        disable_debug_events = false;
    }
    else
    {
        want_debug_events = false;
        disable_debug_events = false;
    }

    int disassembly_flavour = Manager::Get()->GetConfigManager(_T("debugger"))
                              ->ReadInt(_T("disassembly_flavor"), 0);

//    Manager::Get()->GetMessageManager()->Log(_("Flavor is: %d"), disassembly_flavour);

    flavour = _T("set disassembly-flavor ");
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

        if(platform::windows)
        {
            flavour << _T("att");
        }
        else
        {
            flavour << _T("intel");
        }
    }// switch

    if (platform::windows && isConsole)
        QueueCommand(new DebuggerCmd(this, _T("set new-console on")));

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

// Cygwin check code
#ifdef __WXMSW__

enum{ BUFSIZE = 64 };

// routines to handle cygwin compiled programs on a Windows compiled C::B IDE
void GDB_driver::DetectCygwinMount(void)
{

    LONG lRegistryAPIresult;
    HKEY hKey;
    TCHAR szCygwinRoot[BUFSIZE];
    DWORD dwBufLen=BUFSIZE*sizeof(TCHAR);

    // checking if cygwin is present
    lRegistryAPIresult = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                         TEXT("SOFTWARE\\Cygnus Solutions\\Cygwin\\mounts v2"),
                         0, KEY_QUERY_VALUE, &hKey );
    // cygwin not installed
    if( lRegistryAPIresult != ERROR_SUCCESS )
    {
        // no cygwin present
        m_CygwinPresent = false;
        return;
    }

    // readback cygwin root
    m_CygwinPresent = true;
    lRegistryAPIresult = RegQueryValueEx( hKey, TEXT("cygdrive prefix"), NULL, NULL,
                         (LPBYTE) szCygwinRoot, &dwBufLen);
    // query error
     if( (lRegistryAPIresult != ERROR_SUCCESS) || (dwBufLen > BUFSIZE*sizeof(TCHAR)) )
    {
        // bit of an assumption, but we won't be able to find the root without it
        m_CygwinPresent = false;
        return;
    }

    // close opened key
    lRegistryAPIresult = RegCloseKey( hKey );
    // key close error
    if( lRegistryAPIresult != ERROR_SUCCESS )
    {
        // shouldn't happen
        m_CygwinPresent = false;
        return;
    }

    if(m_CygwinPresent == true)
    {
        // convert to wxString type for later use
        m_CygdrivePrefix = (szCygwinRoot);
    }

}

void GDB_driver::CorrectCygwinPath(wxString& path)
{
    unsigned int i=0, escCount=0;
    // prepare to convert to a valid path if Cygwin is being used
    if(path.Contains(m_CygdrivePrefix))
    {
        // preserve any escape characters at start of path - this is true for
        // breakpoints - value is 2, but made dynamic for safety as we
        // are only checking for the CDprefix not any furthur correctness
        if(path.GetChar(0)== g_EscapeChars)
        {
            while(i<path.Len()& (path.GetChar(i)==g_EscapeChars))
            {
                // get character
                escCount+=1;
                i+=1;
            }
        }

        // step over the escape characters and remove cygwin prefix
        path.Remove(escCount, (m_CygdrivePrefix.Len())+1);
        // insert ':' after drive label by reading and removing drive the label
        // and adding ':' and the drive label back
        wxString DriveLetter = path.GetChar(escCount);
        path.Replace(DriveLetter, DriveLetter + _T(":"), false);
    }
}
#else
    void GDB_driver::DetectCygwinMount(void){/* dummy */}
    void GDB_driver::CorrectCygwinPath(wxString& path){/* dummy */}
#endif

wxString GDB_driver::GetDisassemblyFlavour(void)
{
    return flavour;
}

// Only called from DebuggerGDB::Debug
// breakOnEntry was always false.  Changed by HC.
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
    if(platform::windows || platform::macosx)
    {
        m_BreakOnEntry = false;
        m_ManualBreakOnEntry = false;

        if (!Manager::Get()->GetConfigManager(_T("debugger"))->ReadBool(_T("do_not_run"), false))
        {
            // start the process
            QueueCommand(new DebuggerCmd(this, _T("run")));
            m_IsStarted = true;
        }
    }
    else
    {
        m_BreakOnEntry = breakOnEntry;
        m_ManualBreakOnEntry = true;

        if (!Manager::Get()->GetConfigManager(_T("debugger"))->ReadBool(_T("do_not_run"), false))
        {
            // start the process
            QueueCommand(new DebuggerCmd(this, _T("start")));
            m_IsStarted = true;
        }
	}
} // Start

void GDB_driver::Stop()
{
    ResetCursor();
    QueueCommand(new DebuggerCmd(this, _T("quit")));
	m_IsStarted = false;
}

void GDB_driver::Continue()
{
    ResetCursor();
	if (m_IsStarted)
		QueueCommand(new DebuggerCmd(this, _T("cont")));
	else
	{
		QueueCommand(new DebuggerCmd(this, m_ManualBreakOnEntry ? _T("start") : _T("run")));
		m_IsStarted = true;
	}
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

    if(platform::windows)
        QueueCommand(new GdbCmd_DisassemblyInit(this, m_pDisassembly, flavour));
    else
        QueueCommand(new GdbCmd_DisassemblyInit(this, m_pDisassembly));
}

void GDB_driver::CPURegisters()
{
    if (!m_pCPURegisters)
        return;

    if(platform::windows)
        QueueCommand(new GdbCmd_InfoRegisters(this, m_pCPURegisters, flavour));
    else
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
	if (bp->type == DebuggerBreakpoint::bptData)
	{
		QueueCommand(new GdbCmd_AddDataBreakpoint(this, bp));
	}
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

void GDB_driver::RemoveBreakpoint(DebuggerBreakpoint* bp)
{
	if (bp && bp->index != -1)
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

	// Watch for initial debug info and grab the child PID
	// this is put here because we need this info even if
	// we don't get a prompt back.
	// It's "cheap" anyway because the line we 're after is
	// the very first line printed by gdb when running our
	// program. It then sets the child PID and never enters here
	// again because the "want_debug_events" condition below
	// is not satisfied anymore...
	if (platform::windows && want_debug_events && output.Contains(_T("do_initial_child_stuff")))
	{
		// got the line with the PID, parse it out:
		// e.g.
		// gdb: do_initial_child_stuff: process 1392
		if (reChildPid.Matches(output))
		{
			wxString pidStr = reChildPid.GetMatch(output, 1);
			long pid = 0;
			pidStr.ToLong(&pid);
			SetChildPID(pid);
			want_debug_events = false;
			disable_debug_events = true;
			m_pDBG->Log(wxString::Format(_("Child process PID: %d"), pid));
		}
	}

    if (!want_debug_events &&
		output.StartsWith(_T("gdb: ")) ||
        output.StartsWith(_T("Warning: ")) ||
        output.StartsWith(_T("ContinueDebugEvent ")))
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
//		m_ProgramIsStopped = false;
        return; // come back later
    }

	if (disable_debug_events)
	{
		// we don't want debug events anymore (we got the pid)
		QueueCommand(new DebuggerCmd(this, _T("set debugevents off")));
		disable_debug_events = false;
	}

	m_ProgramIsStopped = true;
	m_QueueBusy = false;
	DebuggerCmd* cmd = CurrentCommand();
	if (cmd)
	{
//		DebugLog(wxString::Format(_T("Command parsing output (cmd: %s): %s"), cmd->m_Cmd.c_str(), buffer.Left(idx).c_str()));
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
		if(platform::windows && m_CygwinPresent==true)
		{
			CorrectCygwinPath(lines.Item(i));
		}

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
			m_IsStarted = false;
        }

        // no debug symbols?
        else if (lines[i].Contains(_T("(no debugging symbols found)")))
        {
            m_pDBG->Log(lines[i]);
        }

        // signal
        else if (lines[i].StartsWith(_T("Program received signal SIG")))
        {
			if (lines[i].StartsWith(_T("Program received signal SIGINT")) ||
				lines[i].StartsWith(_T("Program received signal SIGTRAP")))
			{
				// these are break/trace signals, just log them
				Log(lines[i]);
			}
			else
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
					m_forceUpdate = true;
				}
				m_needsUpdate = true;
				// the backtrace will be generated when NotifyPlugins() is called
				// and only if the backtrace window is shown
			}
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
            // Breakpoint 2, irr::scene::CSceneManager::getSceneNodeFromName (this=0x3fa878, name=0x3fbed8 "MainLevel", start=0x3fa87c) at CSceneManager.cpp:1077
            wxString newbpstr = lines[i+1];

            if (rePendingFound.Matches(bpstr) &&
                rePendingFound1.Matches(newbpstr))
            {
//                m_pDBG->Log(_T("MATCH"));

                wxString file;
                wxString lineStr;

                if(platform::windows)
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
                int bpindex = state.HasBreakpoint(file, line - 1);
                DebuggerBreakpoint* bp = state.GetBreakpoint(bpindex);
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
            i += 1;
        }

        // cursor change
        else if (lines[i].StartsWith(g_EscapeChars)) // ->->
        {
            // breakpoint, e.g.
            // C:/Devel/tmp/test_console_dbg/tmp/main.cpp:14:171:beg:0x401428

            // Main breakpoint handler is wrapped into a function so we can use
            // the same code with different regular expressions - depending on
            // the platform.

            if(platform::windows && flavour == _T("set disassembly-flavor or32"))
            {
                HandleMainBreakPoint(reBreak_or32, lines[i]);
            }
            else
            {
                HandleMainBreakPoint(reBreak, lines[i]);
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
        }
    }
    buffer.Clear();

    // if program is stopped, update various states
    if (m_needsUpdate)
    {
        if (m_Cursor.changed || m_forceUpdate)
            NotifyCursorChanged();
    }
}


void GDB_driver::HandleMainBreakPoint(const wxRegEx& reBreak, wxString line)
{
    if ( reBreak.Matches(line) )
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
            wxString lineStr;
            if(platform::windows)
            {
                m_Cursor.file = reBreak.GetMatch(line, 1) + reBreak.GetMatch(line, 2);
                lineStr = reBreak.GetMatch(line, 3);
                m_Cursor.address = reBreak.GetMatch(line, 4);
            }
            else
            {
                m_Cursor.file = reBreak.GetMatch( line, 1);
                lineStr = reBreak.GetMatch( line, 2);
                m_Cursor.address = reBreak.GetMatch( line, 3);
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

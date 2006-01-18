#include <sdk.h>
#include "gdb_driver.h"
#include "gdb_commands.h"
#include <manager.h>
#include <configmanager.h>

#define GDB_PROMPT _T("(gdb)")

static wxRegEx reBreak2(_T("^(0x[A-z0-9]+) in (.*) from (.*)"));
#ifdef __WXMSW__
    static wxRegEx reBreak(_T("([A-z]:)([^:]+):([0-9]+):[0-9]+:[begmidl]+:(0x[0-9A-z]+)"));
#else
    static wxRegEx reBreak(_T("([^:]+):([0-9]+):[0-9]+:[begmidl]+:(0x[0-9A-z]+)"));
#endif

GDB_driver::GDB_driver(DebuggerGDB* plugin)
    : DebuggerDriver(plugin)
{
    //ctor
}

GDB_driver::~GDB_driver()
{
    //dtor
}

wxString GDB_driver::GetCommandLine(const wxString& debugger, const wxString& debuggee)
{
    wxString cmd;
    cmd << debugger;
    cmd << _T(" -nx");          // don't run .gdbinit
    cmd << _T(" -fullname ");   // report full-path filenames when breaking
    cmd << _T(" -args ") << debuggee;
    return cmd;
}

wxString GDB_driver::GetCommandLine(const wxString& debugger, int pid)
{
    wxString cmd;
    cmd << debugger;
    cmd << _T(" -nx");          // don't run .gdbinit
    cmd << _T(" -fullname ");   // report full-path filenames when breaking
    cmd << _T("-pid=") << wxString::Format(_T("%d"), pid);
    return cmd;
}

void GDB_driver::Prepare(bool isConsole)
{
    // default initialization

    // make sure we 're using the prompt that we know and trust ;)
	QueueCommand(new DebuggerCmd(this, wxString(_T("set prompt ")) + GDB_PROMPT));

    // send built-in init commands
	QueueCommand(new DebuggerCmd(this, _T("set confirm off")));
	// no wrapping lines
    QueueCommand(new DebuggerCmd(this, _T("set width 0")));
    // no pagination
    QueueCommand(new DebuggerCmd(this, _T("set height 0")));
    // allow pending breakpoints
    QueueCommand(new DebuggerCmd(this, _T("set breakpoint pending on")));
    // show pretty function names in disassembly
    QueueCommand(new DebuggerCmd(this, _T("set print asm-demangle on")));
#ifndef __WXMSW__
    QueueCommand(new DebuggerCmd(this, _T("set disassembly-flavor att")));
#else
	if (isConsole)
        QueueCommand(new DebuggerCmd(this, _T("set new-console on")));
    QueueCommand(new DebuggerCmd(this, _T("set disassembly-flavor intel")));
#endif

    // define utility functions
    wxString cmd;
    cmd << _T("define print_wxstring\n");
    cmd << _T("  output /c (*$arg0.m_pchData)@(($slen=(unsigned int)$arg0.Len())>100?100:$slen)\n");
    cmd << _T("end");
    QueueCommand(new DebuggerCmd(this, cmd));

    // pass user init-commands
    wxString init = Manager::Get()->GetConfigManager(_T("debugger"))->Read(_T("init_commands"), wxEmptyString);
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
}

void GDB_driver::Start(bool breakOnEntry)
{
    // start the process
    ResetCursor();
    QueueCommand(new DebuggerCmd(this, breakOnEntry ? _T("start") : _T("run")));
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

void GDB_driver::StepIn()
{
    ResetCursor();
    QueueCommand(new DebuggerCmd(this, _T("step")));
}

void GDB_driver::StepOut()
{
    ResetCursor();
    NOT_IMPLEMENTED();
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

void GDB_driver::EvaluateSymbol(const wxString& symbol, wxTipWindow** tipWin, const wxRect& tipRect)
{
    QueueCommand(new GdbCmd_FindTooltipType(this, symbol, tipWin, tipRect));
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
        m_QueueBusy = false;
        DebuggerCmd* cmd = CurrentCommand();
        if (cmd)
        {
//            Log(_T("Command parsing output: ") + buffer.Left(idx));
            RemoveTopCommand(false);
            buffer.Remove(idx);
            if (buffer.Last() == _T('\n'))
                buffer.RemoveLast();
            cmd->ParseOutput(buffer.Left(idx));
            delete cmd;
            RunQueue();
        }
    }
    else
        return; // come back later

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
            break;
        }

        // Is the program running?
        else if (lines[i].StartsWith(_T("Starting program:")))
            m_ProgramIsStopped = false;

        // Is the program exited?
        else if (lines[i].StartsWith(_T("Program exited")))
        {
            m_ProgramIsStopped = true;
            m_pDBG->Log(lines[i]);
            QueueCommand(new DebuggerCmd(this, _T("quit")));
        }

        // no debug symbols?
        else if (lines[i].Contains(_T("(no debugging symbols found)")))
        {
            m_pDBG->Log(lines[i]);
        }

        // signal
        else if (lines[i].StartsWith(_T("Program received signal")))
        {
            Log(lines[i]);
            m_pDBG->BringAppToFront();
            if (IsWindowReallyShown(m_pBacktrace))
            {
                // don't ask; it's already shown
                // just grab the user's attention
                wxMessageBox(lines[i], _("Signal received"), wxICON_ERROR);
            }
            else if (wxMessageBox(wxString::Format(_("%s\nDo you want to view the backtrace?"), lines[i].c_str()), _("Signal received"), wxICON_ERROR | wxYES_NO) == wxYES)
            {
                // show the backtrace window
                CodeBlocksDockEvent evt(cbEVT_SHOW_DOCK_WINDOW);
                evt.pWindow = m_pBacktrace;
                Manager::Get()->GetAppWindow()->ProcessEvent(evt);
            }
            // the backtrace will be generated when NotifyPlugins() is called
            // and only if the backtrace window is shown
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
                m_ProgramIsStopped = true;
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
                NotifyCursorChanged();
			}
        }
        else
        {
            // other break info, e.g.
            // 0x7c9507a8 in ntdll!KiIntSystemCall () from C:\WINDOWS\system32\ntdll.dll
			if ( reBreak2.Matches(lines[i]) )
			{
                m_ProgramIsStopped = true;

				m_Cursor.file = reBreak2.GetMatch(lines[i], 3);
				m_Cursor.function = reBreak2.GetMatch(lines[i], 2);
				wxString lineStr = _T("");
				m_Cursor.address = reBreak2.GetMatch(lines[i], 1);
				m_Cursor.line = -1;
                m_Cursor.changed = true;
                NotifyCursorChanged();
			}
        }
    }
    buffer.Clear();
}

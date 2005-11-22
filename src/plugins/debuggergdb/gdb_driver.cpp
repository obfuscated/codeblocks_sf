#include <sdk.h>
#include "gdb_driver.h"
#include "gdb_commands.h"
#include <manager.h>
#include <configmanager.h>

#define GDB_PROMPT _T("(gdb)")

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
    cmd << _T("-pid=") << pid;
    return cmd;
}

void GDB_driver::Prepare(bool isConsole)
{
    // default initialization

    // make sure we 're using the prompt that we know and trust ;)
	QueueCommand(new DebuggerCmd(this, wxString(_T("set prompt ")) + GDB_PROMPT));

    // send built-in init commands
	QueueCommand(new DebuggerCmd(this, _T("set confirm off")));
    QueueCommand(new DebuggerCmd(this, _T("set breakpoint pending on")));
#ifndef __WXMSW__
    QueueCommand(new DebuggerCmd(this, _T("set disassembly-flavor att")));
#else
	if (isConsole)
        QueueCommand(new DebuggerCmd(this, _T("set new-console on")));
    QueueCommand(new DebuggerCmd(this, _T("set disassembly-flavor intel")));
#endif

    // pass user init-commands
    wxString init = Manager::Get()->GetConfigManager(_T("debugger"))->Read(_T("init_commands"), wxEmptyString);
    wxArrayString initCmds = GetArrayFromString(init, _T('\n'));
    for (unsigned int i = 0; i < initCmds.GetCount(); ++i)
    {
        QueueCommand(new DebuggerCmd(this, initCmds[i]));
    }

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
    QueueCommand(new DebuggerCmd(this, breakOnEntry ? _T("start") : _T("run")));
}

void GDB_driver::Stop()
{
    QueueCommand(new DebuggerCmd(this, _T("quit")));
}

void GDB_driver::Continue()
{
    QueueCommand(new DebuggerCmd(this, _T("cont")));
}

void GDB_driver::Step()
{
    QueueCommand(new DebuggerCmd(this, _T("next")));
}

void GDB_driver::StepIn()
{
    QueueCommand(new DebuggerCmd(this, _T("step")));
}

void GDB_driver::StepOut()
{
    NOT_IMPLEMENTED();
}

void GDB_driver::Backtrace()
{
    if (!m_pBacktrace)
        return;
    QueueCommand(new GdbCmd_Backtrace(this, m_pBacktrace));
    m_pBacktrace->Show();
}

void GDB_driver::Disassemble()
{
    if (!m_pDisassembly)
        return;
    QueueCommand(new GdbCmd_Disassembly(this, m_pDisassembly));
    m_pDisassembly->Show();
}

void GDB_driver::AddBreakpoint(DebuggerBreakpoint* bp)
{
	QueueCommand(new GdbCmd_AddBreakpoint(this, bp));
}

void GDB_driver::RemoveBreakpoint(DebuggerBreakpoint* bp, bool deleteAlso)
{
	QueueCommand(new GdbCmd_RemoveBreakpoint(this, bp, deleteAlso));
}

void GDB_driver::EvaluateSymbol(const wxString& symbol, wxTipWindow** tipWin, const wxRect& tipRect)
{
    QueueCommand(new GdbCmd_TooltipEvaluation(this, symbol, tipWin, tipRect));
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
    if (output.StartsWith(_T("gdb: ")) ||
        output.StartsWith(_T("ContinueDebugEvent ")))
    {
        return;
    }
    static wxString buffer;
	buffer << output << _T('\n');
    m_CursorChanged = false;

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
            if (buffer[buffer.Length() - 1] == _T('\n'))
                buffer.Remove(buffer.Length() - 1);
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
            if (wxMessageBox(wxString::Format(_("%s\nDo you want to view the backtrace?"), lines[i].c_str()), _("Question"), wxICON_QUESTION | wxYES_NO) == wxYES)
            {
                Backtrace();
            }
        }

        // cursor change
        else if (lines[i].StartsWith(g_EscapeChars)) // ->->
        {
            //  breakpoint
            wxRegEx reSource;
			if (!reSource.IsValid())
			#ifdef __WXMSW__
				reSource.Compile(_T("([A-Za-z]:)([ A-Za-z0-9_/\\.~-]*):([0-9]*):[0-9]*:[begmidl]+:(0x[0-9A-Za-z]*)"));
			#else
				reSource.Compile(_T("([ A-Za-z0-9_/\\.~-]*):([0-9]*):[0-9]*:[begmidl]+:(0x[0-9A-Za-z]*)"));
			#endif
			if ( reSource.Matches(buffer) )
			{
                m_ProgramIsStopped = true;
			#ifdef __WXMSW__
				wxString file = reSource.GetMatch(buffer, 1) + reSource.GetMatch(buffer, 2);
				wxString lineStr = reSource.GetMatch(buffer, 3);
				wxString addr = reSource.GetMatch(buffer, 4);
            #else
				wxString file = reSource.GetMatch(buffer, 1);
				wxString lineStr = reSource.GetMatch(buffer, 2);
				wxString addr = reSource.GetMatch(buffer, 3);
            #endif
                if (m_pDisassembly && m_pDisassembly->IsShown())
                {
                    long int addrL;
                    addr.ToLong(&addrL, 16);
                    m_pDisassembly->SetActiveAddress(addrL);
                    QueueCommand(new GdbCmd_InfoRegisters(this, m_pDisassembly));
                }
				lineStr.ToLong(&m_StopLine);
				m_StopFile = file;
                m_CursorChanged = true;
			}
        }
    }
    buffer.Clear();
}

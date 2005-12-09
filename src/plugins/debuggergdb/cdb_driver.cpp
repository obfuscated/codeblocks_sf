#include <sdk.h>
#include "cdb_driver.h"
#include "cdb_commands.h"
#include <manager.h>
#include <configmanager.h>

#define CDB_PROMPT _T("0:000>")

CDB_driver::CDB_driver(DebuggerGDB* plugin)
    : DebuggerDriver(plugin)
{
    //ctor
}

CDB_driver::~CDB_driver()
{
    //dtor
}

wxString CDB_driver::GetCommandLine(const wxString& debugger, const wxString& debuggee)
{
    wxString cmd;
    cmd << debugger;
//    cmd << _T(" -g"); // ignore starting breakpoint
    cmd << _T(" -G"); // ignore ending breakpoint
    cmd << _T(" -lines"); // line info

    if (m_Dirs.GetCount() > 0)
    {
        // add symbols dirs
        cmd << _T(" -y ");
        for (unsigned int i = 0; i < m_Dirs.GetCount(); ++i)
        {
            cmd << m_Dirs[i] << wxPATH_SEP;
        }

        // add source dirs
        cmd << _T(" -srcpath ");
        for (unsigned int i = 0; i < m_Dirs.GetCount(); ++i)
        {
            cmd << m_Dirs[i] << wxPATH_SEP;
        }
    }

    // finally, add the program to debug
    cmd << _T(' ') << debuggee;

    if (!m_WorkingDir.IsEmpty())
        wxSetWorkingDirectory(m_WorkingDir);

    return cmd;
}

wxString CDB_driver::GetCommandLine(const wxString& debugger, int pid)
{
    wxString cmd;
    cmd << debugger;
//    cmd << _T(" -g"); // ignore starting breakpoint
    cmd << _T(" -G"); // ignore ending breakpoint
    cmd << _T(" -lines"); // line info

    if (m_Dirs.GetCount() > 0)
    {
        // add symbols dirs
        cmd << _T(" -y ");
        for (unsigned int i = 0; i < m_Dirs.GetCount(); ++i)
        {
            cmd << m_Dirs[i] << wxPATH_SEP;
        }

        // add source dirs
        cmd << _T(" -srcpath ");
        for (unsigned int i = 0; i < m_Dirs.GetCount(); ++i)
        {
            cmd << m_Dirs[i] << wxPATH_SEP;
        }
    }

    // finally, add the PID
    cmd << _T(" -p ") << pid;

    if (!m_WorkingDir.IsEmpty())
        wxSetWorkingDirectory(m_WorkingDir);

    return cmd;
}

void CDB_driver::Prepare(bool isConsole)
{
    // default initialization
}

void CDB_driver::Start(bool breakOnEntry)
{
    // start the process
    QueueCommand(new DebuggerCmd(this, _T("l+t"))); // source mode
    QueueCommand(new DebuggerCmd(this, _T("l+s"))); // show source lines
    QueueCommand(new DebuggerCmd(this, _T("l+o"))); // only source lines
    QueueCommand(new DebuggerCmd(this, _T("g")));
}

void CDB_driver::Stop()
{
    QueueCommand(new DebuggerCmd(this, _T("q")));
}

void CDB_driver::Continue()
{
    QueueCommand(new DebuggerCmd(this, _T("g")));
}

void CDB_driver::Step()
{
    QueueCommand(new DebuggerCmd(this, _T("p")));
    // print a stack frame to find out about the file we 've stopped
    QueueCommand(new DebuggerCmd(this, _T("k n 1")));
}

void CDB_driver::StepIn()
{
//    QueueCommand(new DebuggerCmd(this, _T("step")));
    NOT_IMPLEMENTED();
}

void CDB_driver::StepOut()
{
    NOT_IMPLEMENTED();
}

void CDB_driver::Backtrace()
{
    if (!m_pBacktrace)
        return;
    QueueCommand(new CdbCmd_Backtrace(this, m_pBacktrace));
    m_pBacktrace->Show();
}

void CDB_driver::Disassemble()
{
    if (!m_pDisassembly)
        return;
    QueueCommand(new CdbCmd_Disassembly(this, m_pDisassembly));
    m_pDisassembly->Show();
}

void CDB_driver::AddBreakpoint(DebuggerBreakpoint* bp)
{
	bp->bpNum = bp->index;
	QueueCommand(new CdbCmd_AddBreakpoint(this, bp));
}

void CDB_driver::RemoveBreakpoint(DebuggerBreakpoint* bp)
{
	QueueCommand(new CdbCmd_RemoveBreakpoint(this, bp));
}

void CDB_driver::EvaluateSymbol(const wxString& symbol, wxTipWindow** tipWin, const wxRect& tipRect)
{
    QueueCommand(new CdbCmd_TooltipEvaluation(this, symbol, tipWin, tipRect));
}

void CDB_driver::UpdateWatches(bool doLocals, bool doArgs, DebuggerTree* tree)
{
    // start updating watches tree
    tree->BeginUpdateTree();

    // locals before args because of precedence
    if (doLocals)
        QueueCommand(new CdbCmd_InfoLocals(this, tree));
//    if (doArgs)
//        QueueCommand(new CdbCmd_InfoArguments(this, tree));
    for (unsigned int i = 0; i < tree->GetWatches().GetCount(); ++i)
    {
        Watch& w = tree->GetWatches()[i];
        QueueCommand(new CdbCmd_Watch(this, tree, &w));
    }

    // run this action-only command to update the tree
    QueueCommand(new DbgCmd_UpdateWatchesTree(this, tree));
}

void CDB_driver::Detach()
{
    QueueCommand(new CdbCmd_Detach(this));
}

void CDB_driver::ParseOutput(const wxString& output)
{
    static wxString buffer;
	buffer << output << _T('\n');
    m_CursorChanged = false;

	m_pDBG->DebugLog(output);

    int idx = buffer.First(CDB_PROMPT);
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
    wxRegEx reBP(_T("Breakpoint ([0-9]+) hit"));
    wxArrayString lines = GetArrayFromString(buffer, _T('\n'));
    for (unsigned int i = 0; i < lines.GetCount(); ++i)
    {
//            Log(_T("DEBUG: ") + lines[i]); // write it in the full debugger log

        if (lines[i].StartsWith(_T("Cannot execute ")))
        {
            Log(lines[i]);
        }

        // Breakpoint 0 hit
        // >   38:     if (!RegisterClassEx (&wincl))
        else if (reBP.Matches(lines[i]))
        {
            Log(lines[i]);

            long int bpNum;
            reBP.GetMatch(lines[i], 1).ToLong(&bpNum);
            DebuggerBreakpoint* bp = m_pDBG->GetState().GetBreakpoint(bpNum);
            if (bp)
            {
                m_StopFile = bp->filename;
                m_StopLine = bp->line + 1;
//                if (bp->temporary)
//                    m_pDBG->GetState().RemoveBreakpoint(bp->index);
            }
            m_CursorChanged = true;
            m_ProgramIsStopped = true;
        }
        // one stack frame (to access current file; is there another way???)
        //  # ChildEBP RetAddr
        // 00 0012fe98 00401426 Win32GUI!WinMain+0x89 [c:\devel\tmp\win32 test\main.cpp @ 55]
        else if (lines[i].Contains(_T("ChildEBP")))
        {
//            wxRegEx ref(_T("([0-9]+) ([A-Fa-f0-9]+) ([A-Fa-f0-9]+) [^[]* \\[([A-Za-z]:)([ A-Za-z0-9_/\\.~-]*) @ ([0-9]+)\\]"));
            wxRegEx ref(_T("[ \t]([A-Za-z]+.*)[ \t]+\\[([A-Za-z]:)([ A-Za-z0-9_/\\.~-]*) @ ([0-9]+)\\]"));
            if (ref.Matches(lines[i + 1]))
            {
                ++i; // we 're interested in the next line
                m_StopAddress = ref.GetMatch(lines[i], 1);
                m_StopFile = ref.GetMatch(lines[i], 2) + ref.GetMatch(lines[i], 3);
                ref.GetMatch(lines[i], 4).ToLong(&m_StopLine);
                m_CursorChanged = true;
                m_ProgramIsStopped = true;
                if (m_pDisassembly && m_pDisassembly->IsShown())
                {
                    long int addrL;
                    m_StopAddress.ToLong(&addrL, 16);
                    m_pDisassembly->SetActiveAddress(addrL);
                    QueueCommand(new CdbCmd_InfoRegisters(this, m_pDisassembly));
                }
            }
        }

//        // log GDB's version
//        if (lines[i].StartsWith(_T("GNU gdb")))
//        {
//            // it's the gdb banner. Just display the version and "eat" the rest
//            m_pDBG->Log(_("Debugger name and version: ") + lines[i]);
//            break;
//        }
//
//        // Is the program running?
//        else if (lines[i].StartsWith(_T("Starting program:")))
//            m_ProgramIsStopped = false;
//
//        // Is the program exited?
//        else if (lines[i].StartsWith(_T("Program exited")))
//        {
//            m_ProgramIsStopped = true;
//            m_pDBG->Log(lines[i]);
//            QueueCommand(new DebuggerCmd(this, _T("quit")));
//        }
//
//        // no debug symbols?
//        else if (lines[i].Contains(_T("(no debugging symbols found)")))
//        {
//            m_pDBG->Log(lines[i]);
//        }
//
//        // signal
//        else if (lines[i].StartsWith(_T("Program received signal")))
//        {
//            Log(lines[i]);
//            if (wxMessageBox(wxString::Format(_("%s\nDo you want to view the backtrace?"), lines[i].c_str()), _("Question"), wxICON_QUESTION | wxYES_NO) == wxYES)
//            {
//                Backtrace();
//            }
//        }
//
//        // cursor change
//        else if (lines[i].StartsWith(g_EscapeChars)) // ->->
//        {
//            //  breakpoint
//            wxRegEx reSource;
//			if (!reSource.IsValid())
//			#ifdef __WXMSW__
//				reSource.Compile(_T("([A-Za-z]:)([ A-Za-z0-9_/\\.~-]*):([0-9]*):[0-9]*:[begmidl]+:(0x[0-9A-Za-z]*)"));
//			#else
//				reSource.Compile(_T("([ A-Za-z0-9_/\\.~-]*):([0-9]*):[0-9]*:[begmidl]:(0x[0-9A-Za-z]*)"));
//			#endif
//			if ( reSource.Matches(buffer) )
//			{
//                m_ProgramIsStopped = true;
//			#ifdef __WXMSW__
//				wxString file = reSource.GetMatch(buffer, 1) + reSource.GetMatch(buffer, 2);
//				wxString lineStr = reSource.GetMatch(buffer, 3);
//				wxString addr = reSource.GetMatch(buffer, 4);
//            #else
//				wxString file = reSource.GetMatch(buffer, 1);
//				wxString lineStr = reSource.GetMatch(buffer, 2);
//				wxString addr = reSource.GetMatch(buffer, 3);
//            #endif
//                if (m_pDisassembly && m_pDisassembly->IsShown())
//                {
//                    long int addrL;
//                    addr.ToLong(&addrL, 16);
//                    m_pDisassembly->SetActiveAddress(addrL);
//                    QueueCommand(new CdbCmd_InfoRegisters(this, m_pDisassembly));
//                }
//				lineStr.ToLong(&m_StopLine);
//				m_StopFile = file;
//                m_CursorChanged = true;
//			}
//        }
    }
    buffer.Clear();
}

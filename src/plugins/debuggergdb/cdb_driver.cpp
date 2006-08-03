#include <sdk.h>
#include "cdb_driver.h"
#include "cdb_commands.h"
#include <manager.h>
#include <configmanager.h>
#include <globals.h>

#define CDB_PROMPT _T("0:000>")

static wxRegEx reBP(_T("Breakpoint ([0-9]+) hit"));
// one stack frame (to access current file; is there another way???)
//  # ChildEBP RetAddr
// 00 0012fe98 00401426 Win32GUI!WinMain+0x89 [c:\devel\tmp\win32 test\main.cpp @ 55]
static wxRegEx reFile(_T("[ \t]([A-z]+.*)[ \t]+\\[([A-z]:)(.*) @ ([0-9]+)\\]"));

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
    cmd << _T(" -p ") << wxString::Format(_T("%d"), pid);

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

    if (!Manager::Get()->GetConfigManager(_T("debugger"))->ReadBool(_T("do_not_run"), false))
        QueueCommand(new DebuggerCmd(this, _T("g")));
}

void CDB_driver::Stop()
{
    ResetCursor();
    QueueCommand(new DebuggerCmd(this, _T("q")));
}

void CDB_driver::Continue()
{
    ResetCursor();
    QueueCommand(new DebuggerCmd(this, _T("g")));
}

void CDB_driver::Step()
{
    ResetCursor();
    QueueCommand(new DebuggerCmd(this, _T("p")));
    // print a stack frame to find out about the file we 've stopped
    QueueCommand(new DebuggerCmd(this, _T("k n 1")));
}

void CDB_driver::StepInstruction()
{
    ResetCursor();
    NOT_IMPLEMENTED();
}

void CDB_driver::StepIn()
{
    ResetCursor();
//    QueueCommand(new DebuggerCmd(this, _T("step")));
    NOT_IMPLEMENTED();
}

void CDB_driver::StepOut()
{
    ResetCursor();
    NOT_IMPLEMENTED();
}

void CDB_driver::Backtrace()
{
    if (!m_pBacktrace)
        return;
    QueueCommand(new CdbCmd_Backtrace(this, m_pBacktrace));
}

void CDB_driver::Disassemble()
{
    if (!m_pDisassembly)
        return;
    QueueCommand(new CdbCmd_DisassemblyInit(this, m_pDisassembly));
}

void CDB_driver::CPURegisters()
{
    if (!m_pCPURegisters)
        return;
    QueueCommand(new CdbCmd_InfoRegisters(this, m_pCPURegisters));
}

void CDB_driver::SwitchToFrame(size_t number)
{
    NOT_IMPLEMENTED();
}

void CDB_driver::SetVarValue(const wxString& var, const wxString& value)
{
    NOT_IMPLEMENTED();
}

void CDB_driver::MemoryDump()
{
    NOT_IMPLEMENTED();
}

void CDB_driver::RunningThreads()
{
    NOT_IMPLEMENTED();
}

void CDB_driver::InfoFrame()
{
    NOT_IMPLEMENTED();
}

void CDB_driver::InfoDLL()
{
    NOT_IMPLEMENTED();
}

void CDB_driver::InfoFiles()
{
    NOT_IMPLEMENTED();
}

void CDB_driver::InfoFPU()
{
    NOT_IMPLEMENTED();
}

void CDB_driver::InfoSignals()
{
    NOT_IMPLEMENTED();
}

void CDB_driver::AddBreakpoint(DebuggerBreakpoint* bp)
{
    QueueCommand(new CdbCmd_AddBreakpoint(this, bp));
}

void CDB_driver::RemoveBreakpoint(DebuggerBreakpoint* bp)
{
    QueueCommand(new CdbCmd_RemoveBreakpoint(this, bp));
}

void CDB_driver::EvaluateSymbol(const wxString& symbol, const wxRect& tipRect)
{
    QueueCommand(new CdbCmd_TooltipEvaluation(this, symbol, tipRect));
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
    m_Cursor.changed = false;
    static wxString buffer;
    buffer << output << _T('\n');

    m_pDBG->DebugLog(output);

    int idx = buffer.First(CDB_PROMPT);
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
            if (buffer[buffer.Length() - 1] == _T('\n'))
                buffer.Remove(buffer.Length() - 1);
            cmd->ParseOutput(buffer.Left(idx));
            delete cmd;
            RunQueue();
        }
    }
    else
    {
        m_ProgramIsStopped = false;
        return; // come back later
    }

    // non-command messages (e.g. breakpoint hits)
    // break them up in lines
    wxArrayString lines = GetArrayFromString(buffer, _T('\n'));
    for (unsigned int i = 0; i < lines.GetCount(); ++i)
    {
//            Log(_T("DEBUG: ") + lines[i]); // write it in the full debugger log

        if (lines[i].StartsWith(_T("Cannot execute ")))
        {
            Log(lines[i]);
        }

        else if (lines[i].Contains(_T("Access violation")))
        {
            Log(lines[i]);
            m_pDBG->BringAppToFront();
            if (IsWindowReallyShown(m_pBacktrace))
            {
                // don't ask; it's already shown
                // just grab the user's attention
                cbMessageBox(lines[i], _("Access violation"), wxICON_ERROR);
                Backtrace();
            }
            else if (cbMessageBox(wxString::Format(_("%s\n\nDo you want to view the backtrace?"), lines[i].c_str()), _("Access violation"), wxICON_ERROR | wxYES_NO) == wxID_YES)
            {
                // show the backtrace window
                CodeBlocksDockEvent evt(cbEVT_SHOW_DOCK_WINDOW);
                evt.pWindow = m_pBacktrace;
                Manager::Get()->GetAppWindow()->ProcessEvent(evt);
                Backtrace();
            }
            break;
        }

        // Breakpoint 0 hit
        // >   38:     if (!RegisterClassEx (&wincl))
        else if (reBP.Matches(lines[i]))
        {
            Log(lines[i]);

            long int bpNum;
            reBP.GetMatch(lines[i], 1).ToLong(&bpNum);
            DebuggerBreakpoint* bp = m_pDBG->GetState().GetBreakpointByNumber(bpNum);
            if (bp)
            {
                // force cursor notification because we don't have an actual address
                // available...
                m_Cursor.address = _T("deadbeef");

                m_Cursor.file = bp->filename;
                m_Cursor.line = bp->line + 1;
//                if (bp->temporary)
//                    m_pDBG->GetState().RemoveBreakpoint(bp->index);
            }
            else
                Log(wxString::Format(_T("Breakpoints inconsistency detected!\nNothing known about breakpoint %ld"), bpNum));
            m_Cursor.changed = true;
            NotifyCursorChanged();
        }
        // one stack frame (to access current file; is there another way???)
        else if (lines[i].Contains(_T("ChildEBP")))
        {
            if (reFile.Matches(lines[i + 1]))
            {
                ++i; // we 're interested in the next line
                m_Cursor.address = reFile.GetMatch(lines[i], 1);
                m_Cursor.file = reFile.GetMatch(lines[i], 2) + reFile.GetMatch(lines[i], 3);
//                if (m_Cursor.file.Last() == _T(' '))
//                    m_Cursor.file.RemoveLast(); // see regex reFile for the reason
                reFile.GetMatch(lines[i], 4).ToLong(&m_Cursor.line);
                m_Cursor.changed = true;
                NotifyCursorChanged();
            }
        }
    }
    buffer.Clear();
}

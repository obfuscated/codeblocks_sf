#include "debuggerdriver.h"
#include "debuggergdb.h"

DebuggerDriver::DebuggerDriver(DebuggerGDB* plugin)
    : m_pDBG(plugin),
    m_ProgramIsStopped(true),
    m_CursorChanged(false),
    m_pBacktrace(0),
    m_pDisassembly(0),
    m_QueueBusy(false)
{
    //ctor
}

DebuggerDriver::~DebuggerDriver()
{
    //dtor
    ClearQueue();
}

void DebuggerDriver::Log(const wxString& msg)
{
    m_pDBG->Log(msg);
}

void DebuggerDriver::DebugLog(const wxString& msg)
{
    m_pDBG->DebugLog(msg);
}

void DebuggerDriver::SetDebugWindows(BacktraceDlg* b, DisassemblyDlg* d)
{
    m_pBacktrace = b;
    m_pDisassembly = d;
}

void DebuggerDriver::ClearDirectories()
{
    m_Dirs.Clear();
}

void DebuggerDriver::AddDirectory(const wxString& dir)
{
    if (m_Dirs.Index(dir) == wxNOT_FOUND)
        m_Dirs.Add(dir);
}

void DebuggerDriver::SetWorkingDirectory(const wxString& dir)
{
    m_WorkingDir = dir;
}

void DebuggerDriver::QueueCommand(DebuggerCmd* dcmd, QueuePriority prio)
{
//    Log(_T("Queueing command: ") + dcmd->m_Cmd);
    if (prio == Low)
        m_DCmds.Add(dcmd);
    else
        m_DCmds.Insert(dcmd, 0);
    RunQueue();
}

DebuggerCmd* DebuggerDriver::CurrentCommand()
{
    return m_DCmds.GetCount() ? m_DCmds[0] : 0;
}

void DebuggerDriver::RunQueue()
{
    if (m_QueueBusy || !m_DCmds.GetCount())
        return;

//    Log(_T("Running command: ") + CurrentCommand()->m_Cmd);
    // don't send a command if empty; most debuggers repeat the last command this way...
    if (!CurrentCommand()->m_Cmd.IsEmpty())
    {
        m_QueueBusy = true;
        m_pDBG->SendCommand(CurrentCommand()->m_Cmd);
    }
}

void DebuggerDriver::RemoveTopCommand(bool deleteIt)
{
    if (m_QueueBusy || !m_DCmds.GetCount())
        return;

//    Log(_T("Removing command: ") + CurrentCommand()->m_Cmd);
    if (deleteIt)
        delete m_DCmds[0];
    m_DCmds.RemoveAt(0);
}

void DebuggerDriver::ClearQueue()
{
    int idx = 0;
    // if the first command in the queue is running, delete all others
    // (this will be deleted when done)
    if (m_QueueBusy && !m_DCmds.GetCount())
        idx = 1;
    for (int i = idx; i < (int)m_DCmds.GetCount(); ++i)
    {
        delete m_DCmds[i];
        m_DCmds.RemoveAt(i);
    }
}

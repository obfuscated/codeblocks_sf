/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#include "debuggerdriver.h"
#include "debuggergdb.h"

DebuggerDriver::DebuggerDriver(DebuggerGDB* plugin)
    : m_pDBG(plugin),
    m_ProgramIsStopped(true),
    m_ChildPID(0),
    m_pBacktrace(0),
    m_pDisassembly(0),
    m_pExamineMemory(0),
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

void DebuggerDriver::SetDebugWindows(BacktraceDlg* b,
                                    DisassemblyDlg* d,
                                    CPURegistersDlg* r,
                                    ExamineMemoryDlg* m,
                                    ThreadsDlg* t)
{
    m_pBacktrace = b;
    m_pDisassembly = d;
    m_pCPURegisters = r;
    m_pExamineMemory = m;
    m_pThreads = t;
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

void DebuggerDriver::SetArguments(const wxString& args)
{
    m_Args = args;
}

void DebuggerDriver::NotifyCursorChanged()
{
    if (!m_Cursor.changed || m_LastCursorAddress == m_Cursor.address)
        return;
    m_LastCursorAddress = m_Cursor.address;
    wxCommandEvent event(DEBUGGER_CURSOR_CHANGED);
    m_pDBG->ProcessEvent(event);
}

void DebuggerDriver::ResetCursor()
{
    m_LastCursorAddress.Clear();
    m_Cursor.address.Clear();
    m_Cursor.file.Clear();
    m_Cursor.function.Clear();
    m_Cursor.line = -1;
    m_Cursor.changed = false;
}

void DebuggerDriver::QueueCommand(DebuggerCmd* dcmd, QueuePriority prio)
{
//    DebugLog(_T("Queueing command: ") + dcmd->m_Cmd);
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
        m_ProgramIsStopped = false;
    }

    // Call Action()
    CurrentCommand()->Action();

    // If the command was an action (i.e. no command specified,
    // remove it from the queue and run the next command.
    // For other commands, this happens in driver's ParseOutput().
    if (CurrentCommand()->m_Cmd.IsEmpty())
    {
        RemoveTopCommand(true);
        RunQueue();
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

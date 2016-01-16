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

#include <cbdebugger_interfaces.h>

DebuggerDriver::DebuggerDriver(DebuggerGDB* plugin)
    : m_pDBG(plugin),
    m_ProgramIsStopped(true),
    m_ChildPID(0),
    m_QueueBusy(false),
    m_currentFrameNo(0),
    m_userSelectedFrameNo(-1)
{
    //ctor
}

DebuggerDriver::~DebuggerDriver()
{
    //dtor
    for (size_t ii = 0; ii < m_DCmds.GetCount(); ++ii)
        delete m_DCmds[ii];
    m_DCmds.Clear();
}

void DebuggerDriver::Log(const wxString& msg)
{
    m_pDBG->Log(msg);
}

void DebuggerDriver::DebugLog(const wxString& msg)
{
    m_pDBG->DebugLog(msg);
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

wxString DebuggerDriver::GetDebuggersWorkingDirectory() const
{
    wxString oldDir = wxGetCwd();
    wxSetWorkingDirectory(m_WorkingDir);
    wxString newDir = wxGetCwd();
    wxSetWorkingDirectory(oldDir);
    return newDir;
}

void DebuggerDriver::SetArguments(const wxString& args)
{
    m_Args = args;
}

void DebuggerDriver::ShowFile(const wxString& file, int line)
{
    wxCommandEvent event(DEBUGGER_SHOW_FILE_LINE);
    event.SetString(file);
    event.SetInt(line);
    m_pDBG->ProcessEvent(event);
}

void DebuggerDriver::NotifyCursorChanged()
{
    if (!m_Cursor.changed || m_LastCursorAddress == m_Cursor.address)
        return;
    m_LastCursorAddress = m_Cursor.address;
    wxCommandEvent event(DEBUGGER_CURSOR_CHANGED);
    m_pDBG->ProcessEvent(event);
}

void DebuggerDriver::NotifyDebuggeeContinued()
{
    m_pDBG->DebuggeeContinued();
    ResetCursor();
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
    if (m_QueueBusy || !m_DCmds.GetCount() || !IsProgramStopped())
        return;

    DebuggerCmd *command = CurrentCommand();

//    Log(_T("Running command: ") + CurrentCommand()->m_Cmd);
    // don't send a command if empty; most debuggers repeat the last command this way...
    if (!command->m_Cmd.IsEmpty())
    {
        m_QueueBusy = true;
        m_pDBG->DoSendCommand(command->m_Cmd);
        if (command->IsContinueCommand())
            m_ProgramIsStopped = false;
    }

    // Call Action()
    command->Action();

    // If the command was an action (i.e. no command specified,
    // remove it from the queue and run the next command.
    // For other commands, this happens in driver's ParseOutput().
    if (command->m_Cmd.IsEmpty())
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

DebuggerDriver::StackFrameContainer const & DebuggerDriver::GetStackFrames() const
{
    return m_backtrace;
}

DebuggerDriver::StackFrameContainer& DebuggerDriver::GetStackFrames()
{
    return m_backtrace;
}

const DebuggerDriver::ThreadsContainer & DebuggerDriver::GetThreads() const
{
    return m_threads;
}

DebuggerDriver::ThreadsContainer & DebuggerDriver::GetThreads()
{
    return m_threads;
}

void DebuggerDriver::SetCurrentFrame(int number, bool user_selected)
{
    m_currentFrameNo = number;
    if (user_selected)
        m_userSelectedFrameNo = number;
}

void DebuggerDriver::ResetCurrentFrame()
{
    m_currentFrameNo = 0;
    m_userSelectedFrameNo = -1;

    if (Manager::Get()->GetDebuggerManager()->UpdateBacktrace())
        Manager::Get()->GetDebuggerManager()->GetBacktraceDialog()->Reload();
}


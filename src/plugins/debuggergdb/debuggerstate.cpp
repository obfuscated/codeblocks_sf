#include <sdk.h>
#include "debuggerstate.h"
#include <compilerfactory.h>
#include "debuggergdb.h"
#include "projectbuildtarget.h"
#include "cdb_driver.h"
#include "gdb_driver.h"

DebuggerState::DebuggerState(DebuggerGDB* plugin)
    : m_pPlugin(plugin),
    m_pDriver(0)
{
}

DebuggerState::~DebuggerState()
{
}

bool DebuggerState::StartDriver(ProjectBuildTarget* target)
{
    StopDriver();
    int idx = target ? target->GetCompilerIndex() : CompilerFactory::GetDefaultCompilerIndex();
    if (idx == 1) // MSVC // TODO: do not hardcode these
        m_pDriver = new CDB_driver(m_pPlugin);
    else
        m_pDriver = new GDB_driver(m_pPlugin);
    return true;
}

void DebuggerState::StopDriver()
{
    if (m_pDriver)
        delete m_pDriver;
    m_pDriver = 0;
}

DebuggerDriver* DebuggerState::GetDriver()
{
    return m_pDriver;
}

void DebuggerState::CleanUp()
{
    if (m_pDriver)
        m_pDriver->RemoveBreakpoint(0);
    StopDriver();

    for (unsigned int i = 0; i < m_Breakpoints.GetCount(); ++i)
    {
        DebuggerBreakpoint* bp = m_Breakpoints[i];
        delete bp;
    }
    m_Breakpoints.Clear();
}

int DebuggerState::AddBreakpoint(const wxString& file, int line, bool temp)
{
    // do we have a bp there?
    int idx = HasBreakpoint(file, line);
    if (idx != -1)
        return idx;
    // create new bp
    DebuggerBreakpoint* bp = new DebuggerBreakpoint;
    bp->filename = file;
    bp->line = line;
    bp->temporary = temp;
    return AddBreakpoint(bp);
}

int DebuggerState::AddBreakpoint(DebuggerBreakpoint* bp)
{
    if (!bp)
        return -1;
    bp->index = m_Breakpoints.GetCount();
    m_Breakpoints.Add(bp);

    // notify driver if it is active
    if (m_pDriver)
        m_pDriver->AddBreakpoint(bp);
    return bp->index;
}

DebuggerBreakpoint* DebuggerState::RemoveBreakpoint(const wxString& file, int line, bool deleteit)
{
    return RemoveBreakpoint(HasBreakpoint(file, line), deleteit);
}

DebuggerBreakpoint* DebuggerState::RemoveBreakpoint(int idx, bool deleteit)
{
    // do we have a valid index?
    if (idx < 0 || idx >= (int)m_Breakpoints.GetCount())
        return 0;
    // yes, remove it from the list
    DebuggerBreakpoint* bp = m_Breakpoints[idx];
    m_Breakpoints.RemoveAt(idx);

    // reset bp->index for breakpoints after it, since list indices are now changed
    for (unsigned int x = idx; x < m_Breakpoints.GetCount(); ++x)
        m_Breakpoints[x]->index = x;

    // notify driver if it is active
    if (m_pDriver)
        m_pDriver->RemoveBreakpoint(bp);

    if (deleteit)
    {
        delete bp;
        return 0;
    }
    return bp;
}

int DebuggerState::HasBreakpoint(const wxString& file, int line)
{
//    Manager::Get()->GetMessageManager()->Log(m_PageIndex, _T("Looking for breakpoint at %s, line %d"), file.c_str(), line);
    for (unsigned int i = 0; i < m_Breakpoints.GetCount(); ++i)
    {
        DebuggerBreakpoint* bp = m_Breakpoints[i];
        if (bp->filename == file && bp->line == line)
            return i;
    }
    return -1;
}

DebuggerBreakpoint* DebuggerState::GetBreakpoint(int idx)
{
    if (idx < 0 || idx >= (int)m_Breakpoints.GetCount())
        return 0;
    return m_Breakpoints[idx];
}

void DebuggerState::ResetBreakpoint(int idx)
{
    DebuggerBreakpoint* bp = RemoveBreakpoint(idx, false);
    AddBreakpoint(bp);
}

void DebuggerState::ApplyBreakpoints()
{
    if (!m_pDriver)
        return;

    // remove any previously set temporary breakpoints
    int i = (int)m_Breakpoints.GetCount() - 1;
    while (i >= 0)
    {
        DebuggerBreakpoint* bp = m_Breakpoints[i];
        if (bp->temporary && bp->alreadySet)
            m_Breakpoints.RemoveAt(i);
        --i;
    }

//    Log(_T("Setting breakpoints"));
	m_pDriver->RemoveBreakpoint(0); // clear all breakpoints

    i = (int)m_Breakpoints.GetCount() - 1;
    while (i >= 0)
    {
        DebuggerBreakpoint* bp = m_Breakpoints[i];
        bp->bpNum = -1;
        bp->index = i;
        m_pDriver->AddBreakpoint(bp);
        --i;
	}
}

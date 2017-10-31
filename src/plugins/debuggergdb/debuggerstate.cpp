/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#include <cbexception.h>
#include "debuggerstate.h"
#include <compilerfactory.h>
#include "debuggergdb.h"
#include "debuggeroptionsdlg.h"
#include "projectbuildtarget.h"
#include "cdb_driver.h"
#include "gdb_driver.h"

#ifndef CB_PRECOMP
    #include <algorithm>

    #include "cbproject.h"
    #include "manager.h"
    #include "projectmanager.h"
#endif

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
    delete m_pDriver;
    m_pDriver = nullptr;

    if (m_pPlugin->GetActiveConfigEx().IsGDB())
        m_pDriver = new GDB_driver(m_pPlugin);
    else
        m_pDriver = new CDB_driver(m_pPlugin);
    m_pDriver->SetTarget(target);
    return true;
}

struct MatchDataAndTempBreakpoints
{
    bool operator()(const cb::shared_ptr<DebuggerBreakpoint> &bp) const
    {
        return bp->type == DebuggerBreakpoint::bptData || bp->temporary;
    }
};

void DebuggerState::StopDriver()
{
    delete m_pDriver;
    m_pDriver = nullptr;
    m_Breakpoints.erase(std::remove_if(m_Breakpoints.begin(), m_Breakpoints.end(), MatchDataAndTempBreakpoints()),
                        m_Breakpoints.end());
}

bool DebuggerState::HasDriver() const
{
    return m_pDriver != NULL;
}

DebuggerDriver* DebuggerState::GetDriver()
{
    cbAssert(m_pDriver != NULL);
    return m_pDriver;
}
const DebuggerDriver* DebuggerState::GetDriver() const
{
    cbAssert(m_pDriver != NULL);
    return m_pDriver;
}

void DebuggerState::CleanUp()
{
    // FIXME (obfuscated#): This is not a good API design! Replace with RemoveAllBreakpoints
    if (m_pDriver)
        m_pDriver->RemoveBreakpoint(cb::shared_ptr<DebuggerBreakpoint>());
    StopDriver();

    m_Breakpoints.clear();
}

// The compiler now uses absolute paths to source files so we don't need
// any absolute->relative filename conversions here anymore.
// Just adjust the path separators...
wxString DebuggerState::ConvertToValidFilename(const wxString& filename)
{
    wxString fname = filename;
    fname.Replace(_T("\\"), _T("/"));
    return fname;
} // end of ConvertToValidFilename

cb::shared_ptr<DebuggerBreakpoint> DebuggerState::AddBreakpoint(const wxString& file, int line,
                                                                bool temp, const wxString& lineText)
{
    wxString bpfile = ConvertToValidFilename(file);

    // do we have a bp there?
    int idx = HasBreakpoint(bpfile, line, temp);
    // if yes, remove old breakpoint first
    if (idx != -1)
        RemoveBreakpoint(idx);

    // create new bp
//    Manager::Get()->GetLogManager()->DebugLog(F(_T("DebuggerState::AddBreakpoint() : bp: file=%s, bpfile=%s"), file.c_str(), bpfile.c_str()));
    cb::shared_ptr<DebuggerBreakpoint> bp(new DebuggerBreakpoint);
    bp->type = DebuggerBreakpoint::bptCode;
    bp->filename = bpfile;
    bp->filenameAsPassed = file;
    bp->line = line;
    bp->temporary = temp;
    bp->lineText = lineText;
    bp->userData = Manager::Get()->GetProjectManager()->FindProjectForFile(file, nullptr, false, false);
    AddBreakpoint(bp);

    return bp;
}

cb::shared_ptr<DebuggerBreakpoint> DebuggerState::AddBreakpoint(const wxString& dataAddr, bool onRead, bool onWrite)
{
    cb::shared_ptr<DebuggerBreakpoint> bp(new DebuggerBreakpoint);
    bp->type = DebuggerBreakpoint::bptData;
    bp->breakAddress = dataAddr;
    bp->breakOnRead = onRead;
    bp->breakOnWrite = onWrite;
    AddBreakpoint(bp);

    return bp;
}

int DebuggerState::AddBreakpoint(cb::shared_ptr<DebuggerBreakpoint> bp)
{
    if (!bp)
        return -1;

    wxString bpfile = ConvertToValidFilename(bp->filename);
    bp->filename = bpfile;
    m_Breakpoints.push_back(bp);

    // notify driver if it is active
    if (m_pDriver)
        m_pDriver->AddBreakpoint(bp);
    return bp->index;
}

void DebuggerState::RemoveBreakpoint(cb::shared_ptr<DebuggerBreakpoint> bp, bool removeFromDriver)
{
    int index = 0;
    for (BreakpointsList::iterator it = m_Breakpoints.begin(); it != m_Breakpoints.end(); ++it, ++index)
    {
        if (*it == bp)
        {
            RemoveBreakpoint(index, removeFromDriver);
            return;
        }
    }
}

void DebuggerState::RemoveBreakpoint(int idx, bool removeFromDriver)
{
    // do we have a valid index?
    if (idx < 0 || idx >= (int)m_Breakpoints.size())
        return;
    // yes, remove it from the list
    BreakpointsList::iterator it = m_Breakpoints.begin();
    std::advance(it, idx);
    cb::shared_ptr<DebuggerBreakpoint> bp = *it;
    m_Breakpoints.erase(it);

    // notify driver if it is active
    if (m_pDriver && removeFromDriver)
        m_pDriver->RemoveBreakpoint(bp);
}

void DebuggerState::RemoveAllBreakpoints()
{
    if (m_pDriver)
    {
        for (BreakpointsList::iterator it = m_Breakpoints.begin(); it != m_Breakpoints.end(); ++it)
            m_pDriver->RemoveBreakpoint(*it);
    }
    m_Breakpoints.clear();
}

struct MatchProject
{
    MatchProject(cbProject *project_in) : project(project_in) {}
    bool operator()(const cb::shared_ptr<DebuggerBreakpoint> &bp)
    {
        return static_cast<cbProject*>(bp->userData) == project;
    }
private:
    cbProject *project;
};

void DebuggerState::RemoveAllProjectBreakpoints(cbProject* prj)
{
    BreakpointsList::iterator start = std::remove_if(m_Breakpoints.begin(), m_Breakpoints.end(), MatchProject(prj));
    if (m_pDriver)
    {
        for (BreakpointsList::iterator it = start; it != m_Breakpoints.end(); ++it)
            m_pDriver->RemoveBreakpoint(*it);
    }
    m_Breakpoints.erase(start, m_Breakpoints.end());
}

void DebuggerState::ShiftBreakpoint(cb::shared_ptr<DebuggerBreakpoint> bp, int nroflines)
{
    // notify driver if it is active
    if (m_pDriver)
    {
        m_pDriver->RemoveBreakpoint(bp);
        bp->line += nroflines;
        m_pDriver->AddBreakpoint(bp);
    }
    else
        bp->line += nroflines;
}

int DebuggerState::HasBreakpoint(const wxString& file, int line, bool temp)
{
    wxString bpfile = ConvertToValidFilename(file);
    int index = 0;
    for (BreakpointsList::iterator it = m_Breakpoints.begin(); it != m_Breakpoints.end(); ++it, ++index)
    {
        DebuggerBreakpoint* bp = (*it).get();
        if ((bp->filename == bpfile || bp->filenameAsPassed == file) && bp->line == line && bp->temporary == temp)
            return index;
    }
    return -1;
}

cb::shared_ptr<DebuggerBreakpoint> DebuggerState::GetBreakpoint(int idx)
{
    if (idx < 0 || idx >= (int)m_Breakpoints.size())
        return cb::shared_ptr<DebuggerBreakpoint>();
    return m_Breakpoints[idx];
}

cb::shared_ptr<DebuggerBreakpoint> DebuggerState::GetBreakpointByNumber(int num)
{
    for (BreakpointsList::iterator it = m_Breakpoints.begin(); it != m_Breakpoints.end(); ++it)
    {
        if ((*it)->index == num)
            return *it;
    }
    return cb::shared_ptr<DebuggerBreakpoint>();
}

const cb::shared_ptr<DebuggerBreakpoint> DebuggerState::GetBreakpointByNumber(int num) const
{
    for (BreakpointsList::const_iterator it = m_Breakpoints.begin(); it != m_Breakpoints.end(); ++it)
    {
        if ((*it)->index == num)
            return *it;
    }
    return cb::shared_ptr<DebuggerBreakpoint>();
}

void DebuggerState::ResetBreakpoint(cb::shared_ptr<DebuggerBreakpoint> bp)
{
    // notify driver if it is active
    if (m_pDriver)
    {
        m_pDriver->RemoveBreakpoint(bp);
        m_pDriver->AddBreakpoint(bp);
    }
}

struct MatchSetTempBreakpoint
{
    bool operator()(const cb::shared_ptr<DebuggerBreakpoint> &bp) const
    {
        return bp->temporary && bp->alreadySet;
    }
};

void DebuggerState::ApplyBreakpoints()
{
    if (!m_pDriver)
        return;

    m_Breakpoints.erase(std::remove_if(m_Breakpoints.begin(), m_Breakpoints.end(), MatchSetTempBreakpoint()),
                        m_Breakpoints.end());

    m_pDriver->RemoveBreakpoint(cb::shared_ptr<DebuggerBreakpoint>());
    m_pPlugin->Log(_("Setting breakpoints"));

    for (BreakpointsList::const_iterator it = m_Breakpoints.begin(); it != m_Breakpoints.end(); ++it)
        m_pDriver->AddBreakpoint(*it);
}

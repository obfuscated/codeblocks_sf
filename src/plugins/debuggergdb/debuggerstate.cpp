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
#include "projectbuildtarget.h"
#include "cdb_driver.h"
#include "gdb_driver.h"
#include "manager.h"
#include "projectmanager.h"

#ifndef CB_PRECOMP
    #include "cbproject.h"
#endif

DebuggerState::DebuggerState(DebuggerGDB* plugin)
    : m_pPlugin(plugin),
    m_pDriver(0),
    m_BpAutoIndex(0)
{
}

DebuggerState::~DebuggerState()
{
}

bool DebuggerState::StartDriver(ProjectBuildTarget* target)
{
    StopDriver();
    SetupBreakpointIndices();
    wxString idx = target ? target->GetCompilerID() : CompilerFactory::GetDefaultCompilerID();
    if (CompilerFactory::CompilerInheritsFrom(idx, _T("msvc*"))) // MSVC
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

bool DebuggerState::HasDriver()
{
	return m_pDriver != NULL;
}

DebuggerDriver* DebuggerState::GetDriver()
{
	cbAssert(m_pDriver != NULL);
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

// Re-number indices. Called before starting the debugging session
void DebuggerState::SetupBreakpointIndices()
{
    m_BpAutoIndex = 0;
    for (unsigned int i = 0; i < m_Breakpoints.GetCount(); ++i)
    {
        DebuggerBreakpoint* bp = m_Breakpoints[i];
        bp->index = ++m_BpAutoIndex;
    }
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

cbProject* DebuggerState::FindProjectForFile(const wxString& file)
{
//    Manager::Get()->GetLogManager()->DebugLog(F(_T("Searching for project containing: ") + file));
    ProjectsArray* projects = Manager::Get()->GetProjectManager()->GetProjects();
    for (size_t i = 0; i < projects->GetCount(); ++i)
    {
        cbProject* prj = projects->Item(i);
        if (prj->GetFileByFilename(file, false, false))
        {
//            Manager::Get()->GetLogManager()->DebugLog(F(_T("Got it: %s (%p)"), prj->GetTitle().c_str(), prj));
            return prj;
        }
    }
//    Manager::Get()->GetLogManager()->DebugLog(F(_T("Not found...")));
    return 0;
}

int DebuggerState::AddBreakpoint(const wxString& file, int line, bool temp, const wxString& lineText)
{
    wxString bpfile = ConvertToValidFilename(file);

    // do we have a bp there?
    int idx = HasBreakpoint(bpfile, line);
    // if yes, remove old breakpoint first
    if (idx != -1)
        RemoveBreakpoint(idx, true);
    // create new bp
//    Manager::Get()->GetLogManager()->DebugLog(_T("add bp: file=%s, bpfile=%s"), file.c_str(), bpfile.c_str());
    DebuggerBreakpoint* bp = new DebuggerBreakpoint;
    bp->type = DebuggerBreakpoint::bptCode;
    bp->filename = bpfile;
    bp->filenameAsPassed = file;
    bp->line = line;
    bp->temporary = temp;
    bp->lineText = lineText;
    bp->userData = FindProjectForFile(file);
    return AddBreakpoint(bp);
}

int DebuggerState::AddBreakpoint(const wxString& dataAddr, bool onRead, bool onWrite)
{
    DebuggerBreakpoint* bp = new DebuggerBreakpoint;
    bp->type = DebuggerBreakpoint::bptData;
    bp->breakAddress = dataAddr;
    bp->breakOnRead = onRead;
    bp->breakOnWrite = onWrite;
    return AddBreakpoint(bp);
}

int DebuggerState::AddBreakpoint(DebuggerBreakpoint* bp)
{
    if (!bp)
        return -1;

    wxString bpfile = ConvertToValidFilename(bp->filename);
    bp->filename = bpfile;

    bp->index = ++m_BpAutoIndex;
    m_Breakpoints.Add(bp);

    // notify driver if it is active
    if (m_pDriver)
        m_pDriver->AddBreakpoint(bp);
    return bp->index;
}

DebuggerBreakpoint* DebuggerState::RemoveBreakpoint(DebuggerBreakpoint* bp, bool deleteit)
{
    for (unsigned int i = 0; i < m_Breakpoints.GetCount(); ++i)
    {
		if (m_Breakpoints[i] == bp)
		{
			return RemoveBreakpoint(i, deleteit);
		}
    }
    return 0;
}

DebuggerBreakpoint* DebuggerState::RemoveBreakpoint(const wxString& file, int line, bool deleteit)
{
    wxString bpfile = ConvertToValidFilename(file);
    return RemoveBreakpoint(HasBreakpoint(bpfile, line), deleteit);
}

DebuggerBreakpoint* DebuggerState::RemoveBreakpoint(int idx, bool deleteit)
{
    // do we have a valid index?
    if (idx < 0 || idx >= (int)m_Breakpoints.GetCount())
        return 0;
    // yes, remove it from the list
    DebuggerBreakpoint* bp = m_Breakpoints[idx];
    m_Breakpoints.RemoveAt(idx);

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

void DebuggerState::RemoveAllBreakpoints(const wxString& file, bool deleteit)
{
    wxString bpfile = ConvertToValidFilename(file);
    bool fileonly = !bpfile.IsEmpty();
    for (int i = m_Breakpoints.GetCount() - 1; i >= 0; --i)
    {
        if (fileonly)
        {
            DebuggerBreakpoint* bp = m_Breakpoints[i];
            if (bp->filename != bpfile && bp->filenameAsPassed != file)
                continue;
        }
        RemoveBreakpoint(i, deleteit);
    }
}

int DebuggerState::RemoveBreakpointsRange(const wxString& file, int startline, int endline)
{
    int ret = 0;
    wxString bpfile = ConvertToValidFilename(file);
    for (int i = m_Breakpoints.GetCount() - 1; i >= 0; --i)
    {
        DebuggerBreakpoint* bp = m_Breakpoints[i];
        if (bp->line >= startline && bp->line <= endline && (bp->filename == bpfile || bp->filenameAsPassed == file))
        {
            ++ret;
            RemoveBreakpoint(i, true);
        }
    }
    return ret;
}

void DebuggerState::RemoveAllProjectBreakpoints(cbProject* prj)
{
//    Manager::Get()->GetLogManager()->DebugLog(F(_T("Removing all breakpoints of project: %p"), prj));
    for (int i = m_Breakpoints.GetCount() - 1; i >= 0; --i)
    {
        DebuggerBreakpoint* bp = m_Breakpoints[i];
        if (bp->userData == prj)
        {
//            Manager::Get()->GetLogManager()->DebugLog(F(_T("Got one")));
            RemoveBreakpoint(i, true);
        }
    }
}

void DebuggerState::ShiftBreakpoints(const wxString& file, int startline, int nroflines)
{
    wxString bpfile = ConvertToValidFilename(file);
    for (int i = m_Breakpoints.GetCount() - 1; i >= 0; --i)
    {
        DebuggerBreakpoint* bp = m_Breakpoints[i];
        if (bp->line >= startline && (bp->filename == bpfile || bp->filenameAsPassed == file))
        {
            // notify driver if it is active
            if (m_pDriver)
                m_pDriver->RemoveBreakpoint(bp);
            bp->line += nroflines;
            // notify driver if it is active
            if (m_pDriver)
                m_pDriver->AddBreakpoint(bp);
        }
    }
}

int DebuggerState::HasBreakpoint(const wxString& file, int line)
{
    wxString bpfile = ConvertToValidFilename(file);
    for (unsigned int i = 0; i < m_Breakpoints.GetCount(); ++i)
    {
        DebuggerBreakpoint* bp = m_Breakpoints[i];
        if ((bp->filename == bpfile || bp->filenameAsPassed == file) && bp->line == line)
            return i;
    }
    return -1;
}

int DebuggerState::HasBreakpoint(const wxString& dataAddr)
{
    for (unsigned int i = 0; i < m_Breakpoints.GetCount(); ++i)
    {
        DebuggerBreakpoint* bp = m_Breakpoints[i];
        if (bp->breakAddress == dataAddr)
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

DebuggerBreakpoint* DebuggerState::GetBreakpointByNumber(int num)
{
    for (unsigned int i = 0; i < m_Breakpoints.GetCount(); ++i)
    {
        DebuggerBreakpoint* bp = m_Breakpoints[i];
        if (bp->index == num)
            return bp;
    }
    return 0;
}

void DebuggerState::ResetBreakpoint(int idx)
{
    DebuggerBreakpoint* bp = RemoveBreakpoint(idx, false);
    AddBreakpoint(bp);
}

void DebuggerState::ResetBreakpoint(DebuggerBreakpoint* bp)
{
    for (unsigned int i = 0; i < m_Breakpoints.GetCount(); ++i)
    {
		if (m_Breakpoints[i] == bp)
		{
			RemoveBreakpoint(i);
			AddBreakpoint(bp);
			break;
		}
    }
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

    m_pPlugin->Log(_("Setting breakpoints"));
    m_pDriver->RemoveBreakpoint(0); // clear all breakpoints

    i = (int)m_Breakpoints.GetCount() - 1;
    while (i >= 0)
    {
        DebuggerBreakpoint* bp = m_Breakpoints[i];
        m_pDriver->AddBreakpoint(bp);
        --i;
    }
}

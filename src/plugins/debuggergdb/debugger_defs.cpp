#include <sdk.h>
#include "debugger_defs.h"
#include "debuggerdriver.h"
#include "debuggertree.h"

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(WatchesArray);

const int DEBUGGER_CURSOR_CHANGED = wxNewId();

DebuggerCmd::DebuggerCmd(DebuggerDriver* driver, const wxString& cmd, bool logToNormalLog)
    : m_Cmd(cmd),
    m_pDriver(driver),
    m_LogToNormalLog(logToNormalLog)
{
}

void DebuggerCmd::ParseOutput(const wxString& output)
{
    if (!output.IsEmpty() && m_LogToNormalLog)
        m_pDriver->Log(output);
}

DbgCmd_UpdateWatchesTree::DbgCmd_UpdateWatchesTree(DebuggerDriver* driver, DebuggerTree* tree)
    : DebuggerCmd(driver),
    m_pTree(tree)
{
}

void DbgCmd_UpdateWatchesTree::Action()
{
    m_pTree->EndUpdateTree();
}

#include "debugger_defs.h"
#include "debuggerdriver.h"

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(WatchesArray);

DebuggerCmd::DebuggerCmd(DebuggerDriver* driver, const wxString& cmd, bool logToNormalLog)
    : m_Cmd(cmd),
    m_pDriver(driver),
    m_LogToNormalLog(logToNormalLog)
{
}

DebuggerCmd::~DebuggerCmd()
{
}

void DebuggerCmd::ParseOutput(const wxString& output)
{
    if (!output.IsEmpty())
    {
        if (m_LogToNormalLog)
            m_pDriver->Log(output);
//                else
//                    m_pDriver->DebugLog(output);
    }
}

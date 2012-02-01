/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "cclogger.h"

#include <wx/event.h>

#include <logmanager.h> // F()

std::auto_ptr<CCLogger> CCLogger::s_Inst;

bool           g_EnableDebugTrace = false;
const wxString g_DebugTraceFile   = wxEmptyString;

CCLogger::CCLogger() :
    m_Parent(nullptr),
    m_LogId(0),
    m_DebugLogId(0)
{
}

/*static*/ CCLogger* CCLogger::Get()
{
    if (!s_Inst.get())
        s_Inst.reset(new CCLogger);
    return s_Inst.get();
}

void CCLogger::Init(wxEvtHandler* parent, int logId, int debugLogId)
{
    m_Parent     = parent;
    m_LogId      = logId;
    m_DebugLogId = debugLogId;
}

void CCLogger::Log(const wxString& msg)
{
    if (!m_Parent) return;

    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, m_LogId);
    evt.SetString(msg);
#if CC_PROCESS_LOG_EVENT_TO_PARENT
    m_Parent->ProcessEvent(evt);
#else
    wxPostEvent(m_Parent, evt);
#endif
}

void CCLogger::DebugLog(const wxString& msg)
{
    if (!m_Parent) return;

    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, m_DebugLogId);
    evt.SetString(msg);
#if CC_PROCESS_LOG_EVENT_TO_PARENT
    m_Parent->ProcessEvent(evt);
#else
    wxPostEvent(m_Parent, evt);
#endif
}

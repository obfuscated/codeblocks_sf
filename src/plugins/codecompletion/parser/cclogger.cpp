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
#include <wx/textfile.h>
#include <wx/utils.h> // wxNewId

#include <logmanager.h> // F()
#include <globals.h>    // cbC2U for cbAssert macro

std::auto_ptr<CCLogger> CCLogger::s_Inst;

bool           g_EnableDebugTrace     = false;
bool           g_EnableDebugTraceFile = false; // true
const wxString g_DebugTraceFile       = wxEmptyString;
long           g_idCCAddToken         = wxNewId();
long           g_idCCLogger           = wxNewId();
long           g_idCCDebugLogger      = wxNewId();
#define TRACE_TO_FILE(msg)                                           \
    if (g_EnableDebugTraceFile && !g_DebugTraceFile.IsEmpty())       \
    {                                                                \
        wxTextFile f(g_DebugTraceFile);                              \
        if ((f.Exists() && f.Open()) || (!f.Exists() && f.Create())) \
        {                                                            \
            f.AddLine(msg);                                          \
            cbAssert(f.Write() && f.Close());                        \
        }                                                            \
    }                                                                \

#define TRACE_THIS_TO_FILE(msg)                                      \
    if (!g_DebugTraceFile.IsEmpty())                                 \
    {                                                                \
        wxTextFile f(g_DebugTraceFile);                              \
        if ((f.Exists() && f.Open()) || (!f.Exists() && f.Create())) \
        {                                                            \
            f.AddLine(msg);                                          \
            cbAssert(f.Write() && f.Close());                        \
        }                                                            \
    }                                                                \


CCLogger::CCLogger() :
    m_Parent(nullptr),
    m_LogId(-1),
    m_DebugLogId(-1),
    m_AddTokenId(-1)
{
}

/*static*/ CCLogger* CCLogger::Get()
{
    if (!s_Inst.get())
        s_Inst.reset(new CCLogger);

    return s_Inst.get();
}

// Initialised from CodeCompletion constructor
void CCLogger::Init(wxEvtHandler* parent, int logId, int debugLogId, int addTokenId)
{
    m_Parent     = parent;
    m_LogId      = logId;
    m_DebugLogId = debugLogId;
    m_AddTokenId = addTokenId;
}

void CCLogger::AddToken(const wxString& msg)
{
    if (!m_Parent || m_AddTokenId<1) return;

    CodeBlocksThreadEvent evt(wxEVT_COMMAND_MENU_SELECTED, m_AddTokenId);
    evt.SetString(msg);
#if CC_PROCESS_LOG_EVENT_TO_PARENT
    m_Parent->ProcessEvent(evt);
#else
    wxPostEvent(m_Parent, evt);
#endif
}

void CCLogger::Log(const wxString& msg)
{
    if (!m_Parent || m_LogId<1) return;

    CodeBlocksThreadEvent evt(wxEVT_COMMAND_MENU_SELECTED, m_LogId);
    evt.SetString(msg);
#if CC_PROCESS_LOG_EVENT_TO_PARENT
    m_Parent->ProcessEvent(evt);
#else
    wxPostEvent(m_Parent, evt);
#endif
}

void CCLogger::DebugLog(const wxString& msg)
{
    if (!m_Parent || m_DebugLogId<1) return;

    CodeBlocksThreadEvent evt(wxEVT_COMMAND_MENU_SELECTED, m_DebugLogId);
    evt.SetString(msg);
#if CC_PROCESS_LOG_EVENT_TO_PARENT
    m_Parent->ProcessEvent(evt);
#else
    wxPostEvent(m_Parent, evt);
#endif
}

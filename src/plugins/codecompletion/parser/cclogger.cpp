/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision: 7658 $
 * $Id: token.cpp 7658 2012-01-03 09:49:22Z mortenmacfly $
 * $HeadURL: https://svn.berlios.de/svnroot/repos/codeblocks/trunk/src/plugins/codecompletion/parser/token.cpp $
 */

#include "cclogger.h"

#include <wx/event.h>

#include <logmanager.h> // F()

std::auto_ptr<CCLogger> CCLogger::s_Inst;

bool           g_EnableDebugTrace = false;
const wxString g_DebugTraceFile   = wxEmptyString;

// class CCLogger

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
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, m_DebugLogId);
    evt.SetString(msg);
#if CC_PROCESS_LOG_EVENT_TO_PARENT
    m_Parent->ProcessEvent(evt);
#else
    wxPostEvent(m_Parent, evt);
#endif
}

// class CCLockerTrack

CCLockerTrack::CCLockerTrack(const wxString& locker, const wxString& func,
              const wxString& file, int line, bool mainThread) :
    m_LockerName(locker),
    m_FuncName(func),
    m_FileName(file),
    m_Line(line),
    m_MainThread(mainThread)
{
    CCLogger::Get()->DebugLog(F(_T("%s.CCLockerTrack() : %s(), %d, %s, %d"), m_LockerName.wx_str(),
                                m_FuncName.wx_str(), m_MainThread, m_FileName.wx_str(), m_Line));
}

CCLockerTrack::~CCLockerTrack()
{
    CCLogger::Get()->DebugLog(F(_T("%s.~CCLockerTrack() : %s(), %d, %s, %d"), m_LockerName.wx_str(),
                                m_FuncName.wx_str(), m_MainThread, m_FileName.wx_str(), m_Line));
}

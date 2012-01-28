/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef CCLOGGER_H
#define CCLOGGER_H

#include <wx/event.h>
#include <wx/string.h>

#include <memory> // auto_ptr

#include <prep.h> // nullptr
#include <logmanager.h> // F()

#ifndef CC_LOG_SYNC_SEND
    #define CC_LOG_SYNC_SEND 0
#endif

#ifdef CC_PARSER_TEST
    #undef CC_LOG_SYNC_SEND
    #define CC_LOG_SYNC_SEND 1
#endif

extern bool           g_EnableDebugTrace; //!< Toggles tracing into file.
extern const wxString g_DebugTraceFile;   //!< Trace file name (if above is enabled).

class CCLogger
{
public:
    static CCLogger* Get()
    {
        if (!s_Inst.get())
            s_Inst.reset(new CCLogger);
        return s_Inst.get();
    }

    void Init(wxEvtHandler* parent, int logId, int debugLogId)
    {
        m_Parent     = parent;
        m_LogId      = logId;
        m_debugLogId = debugLogId;
    }

    void Log(const wxString& msg)
    {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, m_LogId);
        evt.SetString(msg);
#if CC_LOG_SYNC_SEND
        m_Parent->ProcessEvent(evt);
#else
        wxPostEvent(m_Parent, evt);
#endif
    }

    void DebugLog(const wxString& msg)
    {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, m_debugLogId);
        evt.SetString(msg);
#if CC_LOG_SYNC_SEND
        m_Parent->ProcessEvent(evt);
#else
        wxPostEvent(m_Parent, evt);
#endif
    }

protected:
    CCLogger() : m_Parent(nullptr), m_LogId(0) {}
    virtual ~CCLogger() {}
    CCLogger(const CCLogger&) {}
    CCLogger& operator= (const CCLogger&) { return *this; }
    friend class std::auto_ptr<CCLogger>;
    static std::auto_ptr<CCLogger> s_Inst;

private:
    wxEvtHandler* m_Parent;
    int m_LogId;
    int m_debugLogId;
};


#ifdef CC_ENABLE_LOCKER_TRACK
    #define TRACK_THREAD_LOCKER(NAME)                                            \
        CCLockerTrack NAME##Track(wxString(#NAME, wxConvUTF8),                   \
                                  wxString(__FUNCTION__, wxConvUTF8),            \
                                  wxString(__FILE__, wxConvUTF8),                \
                                  __LINE__,                                      \
                                  wxIsMainThread())
    #define THREAD_LOCKER_SUCCESS(NAME)                                          \
        CCLogger::Get()->DebugLog(F(_T("%s.Success() : %s(), %s, %d"),           \
                                    wxString(#NAME, wxConvUTF8).wx_str(),        \
                                    wxString(__FUNCTION__, wxConvUTF8).wx_str(), \
                                    wxString(__FILE__, wxConvUTF8).wx_str(),     \
                                    __LINE__))
#else
    #define TRACK_THREAD_LOCKER(NAME)
    #define THREAD_LOCKER_SUCCESS(NAME)
#endif

class CCLockerTrack
{
public:
    CCLockerTrack(const wxString& locker, const wxString& func, const wxString& file, int line,
                  bool mainThread) :
        m_LockerName(locker),
        m_FuncName(func),
        m_FileName(file),
        m_Line(line),
        m_MainThread(mainThread)
    {
        CCLogger::Get()->DebugLog(F(_T("%s.Lock() : %s(), %d, %s, %d"), m_LockerName.wx_str(),
                                    m_FuncName.wx_str(), m_MainThread, m_FileName.wx_str(), m_Line));
    }
    ~CCLockerTrack()
    {
        CCLogger::Get()->DebugLog(F(_T("%s.UnLock() : %s(), %d, %s, %d"), m_LockerName.wx_str(),
                                    m_FuncName.wx_str(), m_MainThread, m_FileName.wx_str(), m_Line));
    }

private:
    wxString m_LockerName;
    wxString m_FuncName;
    wxString m_FileName;
    int      m_Line;
    bool     m_MainThread;
};

#endif // CCLOGGER_H

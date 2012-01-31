/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef CCLOGGER_H
#define CCLOGGER_H

#include <wx/string.h>

#include <memory> // auto_ptr

#include <prep.h> // nullptr

class wxEvtHandler;

#ifndef CC_PROCESS_LOG_EVENT_TO_PARENT
    #define CC_PROCESS_LOG_EVENT_TO_PARENT 0
#endif

#ifdef CC_PARSER_TEST
    #undef CC_PROCESS_LOG_EVENT_TO_PARENT
    #define CC_PROCESS_LOG_EVENT_TO_PARENT 1
#endif

extern bool           g_EnableDebugTrace; //!< Toggles tracing into file.
extern const wxString g_DebugTraceFile;   //!< Trace file name (if above is enabled).

class CCLogger
{
public:
    static CCLogger* Get();
    void Init(wxEvtHandler* parent, int logId, int debugLogId);
    void Log(const wxString& msg);
    void DebugLog(const wxString& msg);

protected:
    CCLogger() : m_Parent(nullptr), m_LogId(0), m_DebugLogId(0) { ; }
    virtual ~CCLogger()                                         { ; }
    CCLogger(const CCLogger&)                                   { ; }
    CCLogger& operator= (const CCLogger&)                       { return *this; }

    friend class std::auto_ptr<CCLogger>;
    static std::auto_ptr<CCLogger> s_Inst;

private:
    wxEvtHandler* m_Parent;
    int           m_LogId;
    int           m_DebugLogId;
};

//#define CC_ENABLE_LOCKER_TRACK

#ifdef CC_ENABLE_LOCKER_TRACK
    #define THREAD_LOCKER_LOCK(NAME)                                             \
        CCLogger::Get()->DebugLog(F(_T("%s.Lock() : %s(), %s, %d"),              \
                                    wxString(#NAME, wxConvUTF8).wx_str(),        \
                                    wxString(__FUNCTION__, wxConvUTF8).wx_str(), \
                                    wxString(__FILE__, wxConvUTF8).wx_str(),     \
                                    __LINE__))
    #define THREAD_LOCKER_SUCCESS(NAME)                                          \
        CCLogger::Get()->DebugLog(F(_T("%s.Success() : %s(), %s, %d"),           \
                                    wxString(#NAME, wxConvUTF8).wx_str(),        \
                                    wxString(__FUNCTION__, wxConvUTF8).wx_str(), \
                                    wxString(__FILE__, wxConvUTF8).wx_str(),     \
                                    __LINE__))
    #define THREAD_LOCKER_FAIL(NAME)                                             \
        CCLogger::Get()->DebugLog(F(_T("%s.Fail() : %s(), %s, %d"),              \
                                    wxString(#NAME, wxConvUTF8).wx_str(),        \
                                    wxString(__FUNCTION__, wxConvUTF8).wx_str(), \
                                    wxString(__FILE__, wxConvUTF8).wx_str(),     \
                                    __LINE__))
    #define THREAD_LOCKER_ENTER(NAME)                                            \
        CCLogger::Get()->DebugLog(F(_T("%s.Enter() : %s(), %s, %d"),             \
                                    wxString(#NAME, wxConvUTF8).wx_str(),        \
                                    wxString(__FUNCTION__, wxConvUTF8).wx_str(), \
                                    wxString(__FILE__, wxConvUTF8).wx_str(),     \
                                    __LINE__))
    #define THREAD_LOCKER_ENTERED(NAME)                                          \
        CCLogger::Get()->DebugLog(F(_T("%s.Entered() : %s(), %s, %d"),           \
                                    wxString(#NAME, wxConvUTF8).wx_str(),        \
                                    wxString(__FUNCTION__, wxConvUTF8).wx_str(), \
                                    wxString(__FILE__, wxConvUTF8).wx_str(),     \
                                    __LINE__))
    #define THREAD_LOCKER_LEAVE(NAME)                                            \
        CCLogger::Get()->DebugLog(F(_T("%s.Leave() : %s(), %s, %d"),             \
                                    wxString(#NAME, wxConvUTF8).wx_str(),        \
                                    wxString(__FUNCTION__, wxConvUTF8).wx_str(), \
                                    wxString(__FILE__, wxConvUTF8).wx_str(),     \
                                    __LINE__))
#else
    #define THREAD_LOCKER_LOCK(NAME)
    #define THREAD_LOCKER_SUCCESS(NAME)
    #define THREAD_LOCKER_FAIL(NAME)
    #define THREAD_LOCKER_ENTER(NAME)
    #define THREAD_LOCKER_ENTERED(NAME)
    #define THREAD_LOCKER_LEAVE(NAME)
#endif

class CCLockerTrack
{
public:
    CCLockerTrack(const wxString& locker, const wxString& func,
                  const wxString& file, int line, bool mainThread);
    ~CCLockerTrack();

private:
    wxString m_LockerName;
    wxString m_FuncName;
    wxString m_FileName;
    int      m_Line;
    bool     m_MainThread;
};

#endif // CCLOGGER_H

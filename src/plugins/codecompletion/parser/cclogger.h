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
    CCLogger();
    virtual ~CCLogger()                                         { ; }
    CCLogger(const CCLogger&)                                   { ; }
    CCLogger& operator=(const CCLogger&)                        { return *this; }

    friend class std::auto_ptr<CCLogger>;
    static std::auto_ptr<CCLogger> s_Inst;

private:
    wxEvtHandler* m_Parent;
    int           m_LogId;
    int           m_DebugLogId;
};

// For tracking, either uncomment:
//#define CC_ENABLE_LOCKER_TRACK
// ...or:
//#define CC_ENABLE_LOCKER_ASSERT
// ..or none of the above.

#ifdef CC_ENABLE_LOCKER_TRACK
    // TRACKING MUTXES
    // [1] Implementations for tracking mutexes:
    #define THREAD_LOCKER_LOCK(NAME)                                             \
        CCLogger::Get()->DebugLog(F(_T("%s.Lock() : %s(), %s, %d"),              \
                                    wxString(#NAME, wxConvUTF8).wx_str(),        \
                                    wxString(__FUNCTION__, wxConvUTF8).wx_str(), \
                                    wxString(__FILE__, wxConvUTF8).wx_str(),     \
                                    __LINE__))
    #define THREAD_LOCKER_UNLOCK(NAME)                                           \
        CCLogger::Get()->DebugLog(F(_T("%s.Unlock() : %s(), %s, %d"),            \
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

    // [2] Cumulative convenient macros for tracking mutexes [USE THESE!]:
    #define CC_LOCKER_TRACK_MTX_LOCK(M)   \
    {                                     \
        THREAD_LOCKER_LOCK(M);            \
        if (M.Lock()==wxMUTEX_NO_ERROR)   \
          THREAD_LOCKER_SUCCESS(M);       \
        else                              \
          THREAD_LOCKER_FAIL(M);          \
    }
    #define CC_LOCKER_TRACK_MTX_UNLOCK(M) \
    {                                     \
        THREAD_LOCKER_UNLOCK(M);          \
        if (M.Unlock()==wxMUTEX_NO_ERROR) \
          THREAD_LOCKER_SUCCESS(M);       \
        else                              \
          THREAD_LOCKER_FAIL(M);          \
    }


    // TRACKING CRITICAL SECIONS
    // [1] Implementations for tracking critical sections:
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
    // [2] Cumulative convenient macros for tracking critical sections [USE THESE!]:
    #define CC_LOCKER_TRACK_CS_ENTER(CS) \
    {                                    \
         THREAD_LOCKER_ENTER(CS);        \
         CS.Enter();                     \
         THREAD_LOCKER_ENTERED(CS);      \
    }
    #define CC_LOCKER_TRACK_CS_LEAVE(CS) \
    {                                    \
          THREAD_LOCKER_LEAVE(CS);       \
          CS.Leave();                    \
    }
#elif defined CC_ENABLE_LOCKER_ASSERT
    #define CC_LOCKER_TRACK_CS_ENTER(CS)  CS.Enter();
    #define CC_LOCKER_TRACK_CS_LEAVE(CS)  CS.Leave();
    #define CC_LOCKER_TRACK_MTX_LOCK(M)   cbAssert(M.Lock()==wxMUTEX_NO_ERROR);
    #define CC_LOCKER_TRACK_MTX_UNLOCK(M) cbAssert(M.Unlock()==wxMUTEX_NO_ERROR);
#else
    #define CC_LOCKER_TRACK_CS_ENTER(CS)  CS.Enter();
    #define CC_LOCKER_TRACK_CS_LEAVE(CS)  CS.Leave();
    #define CC_LOCKER_TRACK_MTX_LOCK(M)   M.Lock();
    #define CC_LOCKER_TRACK_MTX_UNLOCK(M) M.Unlock();
#endif

#endif // CCLOGGER_H

/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef PROFILETIMER_H
#define PROFILETIMER_H

#include <wx/stopwatch.h>
#include <wx/string.h>

#include <map>

//#define CC_PARSER_PROFILE_TEST 1

#if CC_PARSER_PROFILE_TEST
    #define CC_PROFILE_TIMER()                                                                          \
        static ProfileTimerData __ptd;                                                                  \
        static size_t __i = ProfileTimer::Registry(&__ptd, wxString(__PRETTY_FUNCTION__, wxConvUTF8));  \
        __ptd.m_CallTimes += __i;                                                                       \
        ProfileTimerHelper __profileTimerHelper(__ptd)
    #define CC_PROFILE_TIMER_LOG() ProfileTimer::Log()
#else
    #define CC_PROFILE_TIMER()
    #define CC_PROFILE_TIMER_LOG()
#endif

class ProfileTimerData
{
public:
    ProfileTimerData();
    void Zero();

    wxStopWatch m_StopWatch;
    size_t      m_CallTimes;
    size_t      m_Count;
};

class ProfileTimerHelper
{
public:
    ProfileTimerHelper(ProfileTimerData& profileTimerData);
    ~ProfileTimerHelper();
private:
    ProfileTimerData& m_ProfileTimerData;
};

class ProfileTimer
{
public:
    static size_t Registry(ProfileTimerData* ptd, const wxString& funcName);
    static void   Log();

private:
    typedef std::map<ProfileTimerData*, wxString> ProfileMap;
    static ProfileMap                             m_ProfileMap;
};

#endif // PROFILETIMER_H

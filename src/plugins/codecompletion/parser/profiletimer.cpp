/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "profiletimer.h"

#include "manager.h"
#include "logmanager.h"

ProfileTimer::ProfileMap ProfileTimer::m_ProfileMap;

ProfileTimerData::ProfileTimerData() : m_CallTimes(0), m_Count(0)
{
    m_StopWatch.Pause();
}

void ProfileTimerData::Zero()
{
    m_StopWatch.Start();
    m_StopWatch.Pause();
    m_CallTimes = 0;
    m_Count = 0;
}

ProfileTimerHelper::ProfileTimerHelper(ProfileTimerData& profileTimerData) :
    m_ProfileTimerData(profileTimerData)
{
    if (m_ProfileTimerData.m_Count++ == 0)
        m_ProfileTimerData.m_StopWatch.Resume();
}

ProfileTimerHelper::~ProfileTimerHelper()
{
    if (--m_ProfileTimerData.m_Count == 0)
        m_ProfileTimerData.m_StopWatch.Pause();
}

/* static */ size_t ProfileTimer::Registry(ProfileTimerData* ptd, const wxString& funcName)
{
    m_ProfileMap[ptd] = funcName;
    return 1;
}

/* static */ void ProfileTimer::Log()
{
    for (ProfileMap::iterator it = m_ProfileMap.begin(); it != m_ProfileMap.end(); ++it)
    {
        const long totalTime = it->first->m_StopWatch.Time();
        wxString log;
        log.Printf(_T("\"%s\" used time is %ld minute(s), %ld.%03ld seconds; call times is %lu."),
                   it->second.wx_str(),
                   (totalTime / 60000),
                   (totalTime / 1000) % 60,
                   (totalTime % 1000),
                   static_cast<unsigned long>(it->first->m_CallTimes));
#ifndef CC_PARSER_TEST
        Manager::Get()->GetLogManager()->DebugLog(log);
#endif
        it->first->Zero();
    }
}

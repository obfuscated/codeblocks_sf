/*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "sdk_precomp.h"

#include "logmanager.h"


LogManager::LogManager()
{
    for(size_t i = 0; i < max_logs; ++i)
    {
        slot[i].index = i;
        slot[i].log = &g_null_log;
    }

    SetLog(new StdoutLog, stdout_log);
    SetLog(new StdoutLog, app_log);
    SetLog(new StdoutLog, debug_log);
    slot[stdout_log].title = _T("stdout");
    slot[app_log].title = _T("Code::Blocks");
    slot[debug_log].title = _T("Code::Blocks Debug");
}


size_t LogManager::SetLog(Logger* l, int i)
{
    unsigned int index = i;

    if(i <= no_index)
    {
        index = debug_log + 1;

        while(slot[index].GetLogger())
        if(++index >= max_logs)
        {
            delete l;
            return invalid_log;
        }
    }
    slot[index].SetLogger(l);
    return index;
}


void LogManager::NotifyUpdate()
{
    for(size_t i = 0; i < max_logs; ++i)
        if(slot[i].log)
            slot[i].log->UpdateSettings();

}




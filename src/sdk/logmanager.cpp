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

    SetLog(new StdoutLogger, stdout_log);
    SetLog(new StdoutLogger, app_log);
    SetLog(new StdoutLogger, debug_log);
    slot[stdout_log].title = _T("stdout");
    slot[app_log].title = _T("Code::Blocks");
    slot[debug_log].title = _T("Code::Blocks Debug");
}


size_t LogManager::SetLog(Logger* l, int i)
{
    unsigned int index = i;

    if(i <= no_index)
    {
        for(index = debug_log + 1; index < max_logs; ++index)
        {
            if(slot[index].GetLogger() != &g_null_log)
            {
                slot[index].SetLogger(l);
                return index;
            }
        }

        delete l;
        return invalid_log;
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




/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"

#ifndef WX_PRECOMP
#include <wx/bitmap.h>
#endif

#ifndef CB_PRECOMP
#include <wx/log.h>
#endif

#include "cbcolourmanager.h"
#include "logmanager.h"
#include "loggers.h"

template<> LogManager* Mgr<LogManager>::instance = nullptr;
template<> bool  Mgr<LogManager>::isShutdown = false;

static NullLogger g_null_log;

LogSlot::LogSlot() :
    log(nullptr),
    icon(nullptr)
{
}

LogSlot::~LogSlot()
{
    if (log != &g_null_log)
        delete log;
    delete icon;
}

size_t LogSlot::Index() const
{
    return index;
}

void LogSlot::SetLogger(Logger* in)
{
    if (log != &g_null_log)
        delete log;
    log = in;
}

Logger* LogSlot::GetLogger() const
{
    return log;
}

////////////////////////////////////////////////////////////////////////////////

LogManager::LogManager()
{
    for (size_t i = 0; i < max_logs; ++i)
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

    ColourManager *manager = Manager::Get()->GetColourManager();
    manager->RegisterColour(_("Logs"), _("Success text"), wxT("logs_success_text"), *wxBLUE);
    manager->RegisterColour(_("Logs"), _("Warning text"), wxT("logs_warning_text"), *wxBLUE);
    manager->RegisterColour(_("Logs"), _("Error text"), wxT("logs_error_text"), wxColour(0xf0, 0x00, 0x00));
    manager->RegisterColour(_("Logs"), _("Critical text"), wxT("logs_critical_text"), *wxWHITE);
    manager->RegisterColour(_("Logs"), _("Critical background"), wxT("logs_critical_back"), *wxRED);
    manager->RegisterColour(_("Logs"), _("Critical text (ListCtrl)"),
                            wxT("logs_critical_text_listctrl"), wxColour(0x0a, 0x00, 0x00));
    manager->RegisterColour(_("Logs"), _("Failure text"), wxT("logs_failure_text"), wxColour(0x00, 0x00, 0xa0));

    Register(_T("null"),   new Instantiator<NullLogger>);
    Register(_T("stdout"), new Instantiator<StdoutLogger>);
    Register(_T("text"),   new Instantiator<TextCtrlLogger>);
    Register(_T("file"),   new Instantiator<FileLogger, true>);
}

LogManager::~LogManager()
{
    for (inst_map_t::iterator i = instMap.begin(); i != instMap.end(); ++i)
        delete i->second;
}

void LogManager::ClearLogInternal(int i)
{
    if (i >= 0 && i <= max_logs && slot[i].log != &g_null_log)
        slot[i].log->Clear();
}

void LogManager::LogInternal(const wxString& msg, int i, Logger::level lv)
{
    if (i >= 0 && i <= max_logs && slot[i].log != &g_null_log)
        slot[i].log->Append(msg, lv);
}

size_t LogManager::SetLog(Logger* l, int i)
{
    unsigned int index = i;

    if (i <= no_index)
    {
        for (index = debug_log + 1; index < max_logs; ++index)
        {
            if (slot[index].GetLogger() == &g_null_log)
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
    for (size_t i = 0; i < max_logs; ++i)
    {
        if (slot[i].log)
            slot[i].log->UpdateSettings();
    }
}

void LogManager::DeleteLog(int i)
{
    SetLog(&g_null_log, i);
}

LogSlot& LogManager::Slot(int i)
{
    return slot[i];
}

size_t LogManager::FindIndex(Logger* l)
{
    for (unsigned int i = invalid_log; i < max_logs; ++i)
    {
        if (slot[i].log == l)
            return i;
    }
    return invalid_log;
}

wxArrayString LogManager::ListAvailable()
{
    wxArrayString as;

    for (inst_map_t::iterator i = instMap.begin(); i != instMap.end(); ++i)
        as.Add(i->first);

    return as;
}

bool LogManager::FilenameRequired(const wxString& name)
{
    inst_map_t::iterator i = instMap.find(name);

    if (i != instMap.end())
        return i->second->RequiresFilename();

    return false;
}

Logger* LogManager::New(const wxString& name)
{
    inst_map_t::iterator i;

    if ((i = instMap.find(name)) != instMap.end())
        return i->second->New();

    return new NullLogger;
}

void LogManager::Register(const wxString& name, InstantiatorBase* ins)
{
    instMap[name] = ins;
}

void LogManager::Panic(const wxString& msg, const wxString& component)
{
    wxString title(_T("Panic: "));
    title.Append(component);

    if (!component)
        title.Append(_T("Code::Blocks"));

    wxSafeShowMessage(title, msg);
}

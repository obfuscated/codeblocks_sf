/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/intl.h>
    #include <wx/datetime.h>
    #include <wx/menu.h>
    #include <wx/log.h>
    #include "manager.h"
    #include "messagemanager.h" // class's header file
    #include "editormanager.h"
    #include "configmanager.h"
    #include "simpletextlog.h"
#endif

#include <wx/laywin.h>
#include <wx/settings.h>
#include <wx/bitmap.h>

#include <wxFlatNotebook.h>


static const int idNB = wxNewId();
static const int idNB_TabTop = wxNewId();
static const int idNB_TabBottom = wxNewId();


BEGIN_EVENT_TABLE(MessageManager, wxEvtHandler)
    EVT_MENU(idNB_TabTop, MessageManager::OnTabPosition)
    EVT_MENU(idNB_TabBottom, MessageManager::OnTabPosition)
    EVT_APP_STARTUP_DONE(MessageManager::OnAppDoneStartup)
    EVT_APP_START_SHUTDOWN(MessageManager::OnAppStartShutdown)
    EVT_FLATNOTEBOOK_PAGE_CHANGED(idNB, MessageManager::OnPageChanged)
END_EVENT_TABLE()

// class constructor
MessageManager::MessageManager()
    : m_AppLog(-1),
    m_DebugLog(-1),
    m_LockCounter(0),
    m_AutoHide(false)
{
    SC_CONSTRUCTOR_BEGIN

    m_pNotebook = new wxFlatNotebook(Manager::Get()->GetAppWindow(), idNB);
    m_pNotebook->SetBookStyle(Manager::Get()->GetConfigManager(_T("app"))->ReadInt(_T("/environment/message_tabs_style"), wxFNB_BOTTOM | wxFNB_NO_X_BUTTON));
    m_pNotebook->SetImageList(new wxFlatNotebookImageList);

    wxMenu* NBmenu = new wxMenu(); // deleted automatically by wxFlatNotebook
    NBmenu->Append(idNB_TabTop, _("Tabs at top"));
    NBmenu->Append(idNB_TabBottom, _("Tabs at bottom"));
    m_pNotebook->SetRightClickMenu(NBmenu);

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("message_manager"));

    // add default log and debug images (index 0 and 1)
	wxBitmap bmp;
	wxString prefix;
    prefix = ConfigManager::GetDataFolder() + _T("/images/");
    bmp.LoadFile(prefix + _T("edit_16x16.png"), wxBITMAP_TYPE_PNG);
    m_pNotebook->GetImageList()->push_back(bmp);
    bmp.LoadFile(prefix + _T("contents_16x16.png"), wxBITMAP_TYPE_PNG);
    m_pNotebook->GetImageList()->push_back(bmp);

    m_AppLog = DoAddLog(new SimpleTextLog(), _("Code::Blocks"));

	bool hasDebugLog = cfg->ReadBool(_T("/has_debug_log"), false);
	if (hasDebugLog)
	{
		m_DebugLog = DoAddLog(new SimpleTextLog(), _("Code::Blocks Debug"));
		SetLogImage(m_DebugLog, bmp); // set debug log image
    }

    m_AutoHide = cfg->ReadBool(_T("/auto_hide"), false);
//    Open();

    Manager::Get()->GetAppWindow()->PushEventHandler(this);
}

// class destructor
MessageManager::~MessageManager()
{
    SC_DESTRUCTOR_BEGIN
    SC_DESTRUCTOR_END
    delete m_pNotebook->GetImageList();
    m_pNotebook->Destroy();
}

void MessageManager::CreateMenu(wxMenuBar* menuBar)
{
    SANITY_CHECK();
}

void MessageManager::ReleaseMenu(wxMenuBar* menuBar)
{
    SANITY_CHECK();
}

bool MessageManager::CheckLogId(int id)
{
    return id >= 0 && id < (int)m_Logs.size() && (m_Logs[id] != 0L);
}

void MessageManager::LogToStdOut(const wxChar* msg, ...)
{
    wxString tmp;
    va_list arg_list;

    va_start(arg_list, msg);
    tmp = wxString::FormatV(msg, arg_list);
    va_end(arg_list);

    fprintf(stdout, tmp.mb_str());
}

void MessageManager::Log(const wxChar* msg, ...)
{
    SANITY_CHECK();
    wxString tmp;
    va_list arg_list;

    va_start(arg_list, msg);
    tmp = wxString::FormatV(msg, arg_list);
    va_end(arg_list);

    m_Logs[m_AppLog]->AddLog(tmp);
}

void MessageManager::DebugLog(const wxChar* msg, ...)
{
    SANITY_CHECK();
	if (!CheckLogId(m_DebugLog))
		return;
    wxString tmp;
    va_list arg_list;

    va_start(arg_list, msg);
    tmp = wxString::FormatV(msg, arg_list);
    va_end(arg_list);

	wxDateTime timestamp = wxDateTime::UNow();
	wxString ts;
	ts.Printf(_T("%2.2d:%2.2d:%2.2d.%3.3d"), timestamp.GetHour(), timestamp.GetMinute(), timestamp.GetSecond(), timestamp.GetMillisecond());
    m_Logs[m_DebugLog]->AddLog(_T("[") + ts + _T("]: ") + tmp);
//    m_Logs[mltDebug]->AddLog(_T("[") + timestamp.Format(_T("%X.%l")) + _T("]: ") + tmp);
//    m_Logs[mltDebug]->AddLog(tmp);
}

void MessageManager::DebugLogWarning(const wxChar* msg, ...)
{
    SANITY_CHECK();
    wxString tmp;
    va_list arg_list;

    va_start(arg_list, msg);
    tmp = wxString::FormatV(msg, arg_list);
    va_end(arg_list);

    wxString typ = _("WARNING");
    wxSafeShowMessage(typ, typ + _T(":\n\n") + tmp);
    if (!CheckLogId(m_DebugLog))
        return;
    ((SimpleTextLog*)m_Logs[m_DebugLog])->GetTextControl()->SetDefaultStyle(wxTextAttr(*wxBLUE));
    DebugLog(typ + _T(": ") + tmp);
    ((SimpleTextLog*)m_Logs[m_DebugLog])->GetTextControl()->SetDefaultStyle(wxTextAttr(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT), wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW)));
}

void MessageManager::DebugLogError(const wxChar* msg, ...)
{
    SANITY_CHECK();

    wxString tmp;
    va_list arg_list;

    va_start(arg_list, msg);
    tmp = wxString::FormatV(msg, arg_list);
    va_end(arg_list);

    wxString typ = _("ERROR");
    wxSafeShowMessage(typ, typ + _T(":\n\n") + tmp);
    if (!CheckLogId(m_DebugLog))
        return;
    ((SimpleTextLog*)m_Logs[m_DebugLog])->GetTextControl()->SetDefaultStyle(wxTextAttr(*wxRED));
    DebugLog(typ + _T(": ") + tmp);
    ((SimpleTextLog*)m_Logs[m_DebugLog])->GetTextControl()->SetDefaultStyle(wxTextAttr(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT), wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW)));
}

// add a new log page
int MessageManager::AddLog(MessageLog* log, const wxString& title, const wxBitmap& bitmap)
{
    SANITY_CHECK(-1);
    return DoAddLog(log, title, bitmap);
}

void MessageManager::RemoveLog(MessageLog* log)
{
    // TODO
}

// add a new log page
int MessageManager::DoAddLog(MessageLog* log, const wxString& title, const wxBitmap& bitmap)
{
    SANITY_CHECK(-1);

    static int pageId = 0;
    int id = pageId++;

    log->m_PageId = id;
    m_Logs[id] = log;
    m_pNotebook->AddPage(log, title, false);
    SetLogImage(id, bitmap);
    return id;
}

void MessageManager::Log(int id, const wxChar* msg, ...)
{
    SANITY_CHECK();
    if (!CheckLogId(id))
        return;

    wxString tmp;
    va_list arg_list;

    va_start(arg_list, msg);
    tmp = wxString::FormatV(msg, arg_list);
    va_end(arg_list);

    m_Logs[id]->AddLog(tmp);
}

void MessageManager::AppendLog(const wxChar* msg, ...)
{
    SANITY_CHECK();
    wxString tmp;
    va_list arg_list;

    va_start(arg_list, msg);
    tmp = wxString::FormatV(msg, arg_list);
    va_end(arg_list);

    m_Logs[m_AppLog]->AddLog(tmp, false);
}

void MessageManager::AppendLog(int id, const wxChar* msg, ...)
{
    SANITY_CHECK();
    if (!CheckLogId(id))
        return;

    wxString tmp;
    va_list arg_list;

    va_start(arg_list, msg);
    tmp = wxString::FormatV(msg, arg_list);
    va_end(arg_list);

    m_Logs[id]->AddLog(tmp, false);
}

// switch to log page
void MessageManager::SwitchTo(int id)
{
    SANITY_CHECK();

    if (!CheckLogId(id))
        return;
    int index = m_pNotebook->GetPageIndex(m_Logs[id]);
    m_pNotebook->SetSelection(index);
}

void MessageManager::SetLogImage(int id, const wxBitmap& bitmap)
{
    SANITY_CHECK();
    if (!CheckLogId(id))
        return;

    int index = m_pNotebook->GetPageIndex(m_Logs[id]);
    if (!bitmap.Ok())
        m_pNotebook->SetPageImageIndex(index, 0); // default log image
    else
    {
        m_pNotebook->GetImageList()->push_back(bitmap);
        m_pNotebook->SetPageImageIndex(index, m_pNotebook->GetImageList()->size() - 1);
    }
}

void MessageManager::SetLogImage(MessageLog* log, const wxBitmap& bitmap)
{
    SANITY_CHECK();
    if (log)
        SetLogImage(log->GetPageId(), bitmap);
}

void MessageManager::EnableAutoHide(bool enable)
{
    m_AutoHide = enable;
    if (m_AutoHide)
        Close();
}

bool MessageManager::IsAutoHiding()
{
    return m_AutoHide;
}

void MessageManager::Open()
{
    if (!m_AutoHide)
        return;

    CodeBlocksDockEvent evt(cbEVT_SHOW_DOCK_WINDOW);
    evt.pWindow = m_pNotebook;
    Manager::Get()->GetAppWindow()->ProcessEvent(evt);
}

void MessageManager::Close(bool force)
{
    if (!m_AutoHide)
        return;
    if (!force && m_LockCounter > 0)
        return;

    m_LockCounter = 0;
    CodeBlocksDockEvent evt(cbEVT_HIDE_DOCK_WINDOW);
    evt.pWindow = m_pNotebook;
    Manager::Get()->GetAppWindow()->ProcessEvent(evt);
}

void MessageManager::LockOpen()
{
    if (!m_AutoHide)
        return;
    Open();
    ++m_LockCounter;
}

void MessageManager::Unlock(bool force)
{
    if (!m_AutoHide)
        return;
    --m_LockCounter;
    if (force || m_LockCounter <= 0)
    {
        m_LockCounter = 0;
        Close(true);
    }
}

void MessageManager::OnTabPosition(wxCommandEvent& event)
{
    long style = m_pNotebook->GetBookStyle();
    style &= ~wxFNB_BOTTOM;

    if (event.GetId() == idNB_TabBottom)
        style |= wxFNB_BOTTOM;
    m_pNotebook->SetBookStyle(style);
    // (style & wxFNB_BOTTOM) saves info only about the the tabs position
    Manager::Get()->GetConfigManager(_T("app"))->Write(_T("/environment/message_tabs_bottom"), (bool)(style & wxFNB_BOTTOM));
}

void MessageManager::OnAppDoneStartup(wxCommandEvent& event)
{
    event.Skip(); // allow others to process it too
}

void MessageManager::OnAppStartShutdown(wxCommandEvent& event)
{
    event.Skip(); // allow others to process it too
}

void MessageManager::OnPageChanged(wxFlatNotebookEvent& event)
{
    if (m_AutoHide && event.GetEventObject() == this)
    {
        Open();
    }

    event.Skip();
}

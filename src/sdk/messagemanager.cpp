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
* $Id$
* $Date$
*/

#include "sdk_precomp.h"
#include <wx/intl.h>
#include <wx/datetime.h>
#include <wx/menu.h>
#include <wx/imaglist.h>
#include <wx/log.h>
#include <wx/laywin.h>
#include <wx/settings.h>

#include "manager.h"
#include "messagemanager.h" // class's header file
#include "editormanager.h"
#include "configmanager.h"
#include "simpletextlog.h"
#include "managerproxy.h"

#define CBYIELD() \
  {                                                     \
      wxTheApp->Yield();                                \
  }

MessageManager* MessageManager::Get(wxWindow* parent)
{
    if(Manager::isappShuttingDown()) // The mother of all sanity checks
        MessageManager::Free();
    else if (!MessageManagerProxy::Get())
	{
		MessageManagerProxy::Set( new MessageManager(parent) );
		MessageManagerProxy::Get()->Log(_("MessageManager initialized"));
	}
    return MessageManagerProxy::Get();
}

void MessageManager::Free()
{
	if (MessageManagerProxy::Get())
	{
		delete MessageManagerProxy::Get();
		MessageManagerProxy::Set( 0L );
	}
}

BEGIN_EVENT_TABLE(MessageManager, wxNotebook)
    EVT_NOTEBOOK_PAGE_CHANGED(-1, MessageManager::OnSelChange)
END_EVENT_TABLE()

// class constructor
MessageManager::MessageManager(wxWindow* parent)
    : wxNotebook(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN | wxNB_BOTTOM),// | wxNB_MULTILINE),
    m_LockCounter(0),
    m_OpenSize(150),
    m_AutoHide(false),
    m_Open(false),
    m_SafebutSlow(false)
{
    SC_CONSTRUCTOR_BEGIN

    wxImageList* images = new wxImageList(16, 16);
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("message_manager"));

    // add default log and debug images (index 0 and 1)
	wxBitmap bmp;
	wxString prefix;
    prefix = ConfigManager::GetDataFolder() + _T("/images/");
    bmp.LoadFile(prefix + _T("edit_16x16.png"), wxBITMAP_TYPE_PNG);
    images->Add(bmp);
    bmp.LoadFile(prefix + _T("contents_16x16.png"), wxBITMAP_TYPE_PNG);
    images->Add(bmp);
    AssignImageList(images);

    m_Logs.clear();
    m_LogIDs.clear();
    DoAddLog(mltLog, new SimpleTextLog(this, _("Code::Blocks")));
	m_HasDebugLog = cfg->ReadBool(_T("/has_debug_log"), false);
	m_SafebutSlow = cfg->ReadBool(_T("/safe_but_slow"), false);

	if (m_HasDebugLog)
	{
		DoAddLog(mltDebug, new SimpleTextLog(this, _("Code::Blocks Debug")));
		SetPageImage(m_Logs[mltDebug]->GetPageIndex(), 1); // set debug log image
    }

	cfg->Write(_T("/safe_but_slow"), m_SafebutSlow);

    m_OpenSize = Manager::Get()->GetConfigManager(_T("app"))->ReadInt(_T("/main_frame/layout/bottom_block_height"), 150);
    m_AutoHide = cfg->ReadBool(_T("/auto_hide"), false);
//    Open();
    LogPage(mltDebug); // default logging page for stream operator
}

// class destructor
MessageManager::~MessageManager()
{
    SC_DESTRUCTOR_BEGIN
    SC_DESTRUCTOR_END
}

bool MessageManager::GetSafebutSlow()
{
    if(!this)
        return false;
    return m_SafebutSlow;
}

void MessageManager::SetSafebutSlow(bool flag, bool dosave)
{
    if(this)
    {
        m_SafebutSlow = flag;
        if(dosave)
            Manager::Get()->GetConfigManager(_T("message_manager"))->Write(_T("/safe_but_slow"), m_SafebutSlow);
    }

}

void MessageManager::CreateMenu(wxMenuBar* menuBar)
{
    SANITY_CHECK();
}

void MessageManager::ReleaseMenu(wxMenuBar* menuBar)
{
    SANITY_CHECK();
}

bool MessageManager::CheckLogType(MessageLogType type)
{
    SANITY_CHECK(false);
    if (type == mltOther)
    {
        DebugLog(_("Can't use MessageManager::Log(mltOther, ...); Use MessageManager::Log(id, ...)"));
        return false;
    }
	return true;
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

    m_Logs[mltLog]->AddLog(tmp);
    m_Logs[mltLog]->Refresh();

	if(!Manager::isappShuttingDown() && !m_SafebutSlow)
        CBYIELD();
}

void MessageManager::DebugLog(const wxChar* msg, ...)
{
    SANITY_CHECK();
	if (!m_HasDebugLog)
		return;
    wxString tmp;
    va_list arg_list;

    va_start(arg_list, msg);
    tmp = wxString::FormatV(msg, arg_list);
    va_end(arg_list);

	wxDateTime timestamp = wxDateTime::UNow();
	wxString ts;
	ts.Printf(_T("%2.2d:%2.2d:%2.2d.%3.3d"), timestamp.GetHour(), timestamp.GetMinute(), timestamp.GetSecond(), timestamp.GetMillisecond());
    m_Logs[mltDebug]->AddLog(_T("[") + ts + _T("]: ") + tmp);
//    m_Logs[mltDebug]->AddLog(_T("[") + timestamp.Format(_T("%X.%l")) + _T("]: ") + tmp);
//    m_Logs[mltDebug]->AddLog(tmp);
    m_Logs[mltDebug]->Refresh();

	if(!Manager::isappShuttingDown() && !m_SafebutSlow)
        CBYIELD();
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
    if (!m_HasDebugLog)
        return;
    ((SimpleTextLog*)m_Logs[mltDebug])->GetTextControl()->SetDefaultStyle(wxTextAttr(*wxBLUE));
    DebugLog(typ + _T(": ") + tmp);
    ((SimpleTextLog*)m_Logs[mltDebug])->GetTextControl()->SetDefaultStyle(wxTextAttr(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT), wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW)));
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
    if (!m_HasDebugLog)
        return;
    ((SimpleTextLog*)m_Logs[mltDebug])->GetTextControl()->SetDefaultStyle(wxTextAttr(*wxRED));
    DebugLog(typ + _T(": ") + tmp);
    ((SimpleTextLog*)m_Logs[mltDebug])->GetTextControl()->SetDefaultStyle(wxTextAttr(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT), wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW)));
}

// add a new log page
int MessageManager::AddLog(MessageLog* log)
{
    SANITY_CHECK(-1);
    return DoAddLog(mltOther, log);
}

// add a new log page
int MessageManager::DoAddLog(MessageLogType type, MessageLog* log)
{
    SANITY_CHECK(-1);
    if (!m_HasDebugLog && type == mltDebug)
        return -1;

    if (type != mltOther)
        m_Logs[type] = log;
    m_LogIDs[log->GetPageIndex()] = log;
    SetPageImage(log->GetPageIndex(), 0); // set default page image
    return log->GetPageIndex();
}

void MessageManager::Log(int id, const wxChar* msg, ...)
{
    SANITY_CHECK();
    if (!m_LogIDs[id])
        return;

    wxString tmp;
    va_list arg_list;

    va_start(arg_list, msg);
    tmp = wxString::FormatV(msg, arg_list);
    va_end(arg_list);

    m_LogIDs[id]->AddLog(tmp);
    m_LogIDs[id]->Refresh();

    if(!Manager::isappShuttingDown() && !m_SafebutSlow)
        CBYIELD();
}

void MessageManager::AppendLog(const wxChar* msg, ...)
{
    SANITY_CHECK();
    wxString tmp;
    va_list arg_list;

    va_start(arg_list, msg);
    tmp = wxString::FormatV(msg, arg_list);
    va_end(arg_list);

    m_Logs[mltLog]->AddLog(tmp, false);
    m_Logs[mltLog]->Refresh();
	if(!Manager::isappShuttingDown() && !m_SafebutSlow)
        CBYIELD();
}

void MessageManager::AppendLog(int id, const wxChar* msg, ...)
{
    SANITY_CHECK();
    if (!m_LogIDs[id])
        return;

    wxString tmp;
    va_list arg_list;

    va_start(arg_list, msg);
    tmp = wxString::FormatV(msg, arg_list);
    va_end(arg_list);

    m_LogIDs[id]->AddLog(tmp, false);
    m_LogIDs[id]->Refresh();
	if(!Manager::isappShuttingDown() && !m_SafebutSlow)
        CBYIELD();
}

// switch to log page
void MessageManager::SwitchTo(MessageLogType type)
{
    SANITY_CHECK();
    if (!m_HasDebugLog && type == mltDebug)
        return;

    if (!CheckLogType(type))
		return;
    DoSwitchTo(m_Logs[type]);
}

void MessageManager::SwitchTo(int id)
{
    SANITY_CHECK();
    DoSwitchTo(m_LogIDs[id]);
}

MessageLogType MessageManager::LogPage(MessageLogType lt)
{
    m_OtherPageLogTarget = m_Logs[lt] ? m_Logs[lt]->GetPageIndex() : 0;
    return lt;
}

MessageLogType MessageManager::LogPage(int pageIndex)
{
    m_OtherPageLogTarget = pageIndex;
    return mltOther;
}

void MessageManager::DoSwitchTo(MessageLog* ml)
{
    SANITY_CHECK();
    if (ml)
    {
        int index = ml->GetPageIndex();
        SetSelection(index);
    }
    else
        DebugLog(_("MessageManager::DoSwitchTo() invalid page..."));
}

void MessageManager::SetLogImage(int id, const wxBitmap& bitmap)
{
    SANITY_CHECK();
    if (!m_LogIDs[id] || !GetImageList())
        return;

    int idx = GetImageList()->Add(bitmap);
    SetPageImage(m_LogIDs[id]->GetPageIndex(), idx);
}

void MessageManager::SetLogImage(MessageLog* log, const wxBitmap& bitmap)
{
    SANITY_CHECK();
    if (!log || !GetImageList())
        return;

    int idx = GetImageList()->Add(bitmap);
    SetPageImage(log->GetPageIndex(), idx);
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

int MessageManager::GetOpenSize()
{
    return m_OpenSize;
}

void MessageManager::Open()
{
    if (!m_AutoHide || m_Open)
        return;

    if (m_pContainerWin)
        m_pContainerWin->Show(true);
    m_Open = true;
}

void MessageManager::Close(bool force)
{
    if (!m_AutoHide || !m_Open)
        return;
    if (!force && m_LockCounter > 0)
        return;

    m_LockCounter = 0;
    if (m_pContainerWin)
        m_pContainerWin->Show(false);
    m_Open = false;
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

void MessageManager::OnSelChange(wxNotebookEvent& event)
{
    if (m_AutoHide && event.GetEventObject() == this)
    {
        Open();
    }

    event.Skip();
}

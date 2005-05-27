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
    : wxNotebook(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN | wxNB_BOTTOM | wxNB_MULTILINE),
    m_LockCounter(0),
    m_OpenSize(150),
    m_AutoHide(false),
    m_Open(false)
{
    SC_CONSTRUCTOR_BEGIN
    
    wxImageList* images = new wxImageList(16, 16);

    // add default log and debug images (index 0 and 1)
	wxBitmap bmp;
	wxString prefix;
    prefix = ConfigManager::Get()->Read("data_path") + "/images/";
    bmp.LoadFile(prefix + "edit_16x16.png", wxBITMAP_TYPE_PNG);
    images->Add(bmp);
    bmp.LoadFile(prefix + "contents_16x16.png", wxBITMAP_TYPE_PNG);
    images->Add(bmp);
    AssignImageList(images);

    m_Logs.clear();
    m_LogIDs.clear();
    DoAddLog(mltLog, new SimpleTextLog(this, _("Code::Blocks")));
	m_HasDebugLog = ConfigManager::Get()->Read("/message_manager/has_debug_log", 0L);

	if (m_HasDebugLog)
	{
		DoAddLog(mltDebug, new SimpleTextLog(this, _("Code::Blocks Debug")));
		SetPageImage(m_Logs[mltDebug]->GetPageIndex(), 1); // set debug log image
    }

    ConfigManager::AddConfiguration(_("Message Manager"), "/message_manager");
    
    m_OpenSize = ConfigManager::Get()->Read("/main_frame/layout/bottom_block_height", 150);
    Open();
    m_AutoHide = ConfigManager::Get()->Read("/message_manager/auto_hide", 0L);
    LogPage(mltDebug); // default logging page for stream operator
}

// class destructor
MessageManager::~MessageManager()
{
    SC_DESTRUCTOR_BEGIN
    SC_DESTRUCTOR_END
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

void MessageManager::Log(const wxChar* msg, ...)
{
    SANITY_CHECK();
    wxString tmp;
    va_list arg_list;

    va_start(arg_list, msg);
    tmp = wxString::FormatV(msg, arg_list);
    va_end(arg_list);

    m_Logs[mltLog]->AddLog(tmp);
	wxYield(); //wxSafeYield(this,true);
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
    m_Logs[mltDebug]->AddLog("[" + timestamp.Format("%X.%l") + "]: " + tmp);
	wxYield(); //wxSafeYield(this,true);
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
    wxSafeShowMessage(typ, typ + ":\n\n" + tmp);
    DebugLog(typ + tmp);
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
    wxSafeShowMessage(typ, typ + ":\n\n" + tmp);
    DebugLog(typ + tmp);
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
	wxYield(); //wxSafeYield(this,true);
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
	wxYield(); //wxSafeYield(this,true);
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
	wxYield(); //wxSafeYield(this,true);
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
    switch (lt)
    {
        case mltDebug:
            if (m_HasDebugLog && CheckLogType(lt))
                m_OtherPageLogTarget = m_Logs[lt]->GetPageIndex();
            else
                m_OtherPageLogTarget = m_Logs[lt]->GetPageIndex();
            break;
        case mltLog:
            m_OtherPageLogTarget = m_Logs[lt]->GetPageIndex();
            break;
        default:
            break;
    }
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
    return m_Open ? GetSize().y : m_OpenSize;
}

void MessageManager::Open()
{
    if (!m_AutoHide || m_Open)
        return;
    m_Open = true;
    wxSashLayoutWindow* sash = (wxSashLayoutWindow*)GetParent();
    if (!sash)
        return;
    if (!sash->IsShown())
        sash->Show(true);
    sash->SetDefaultSize(wxSize(1, m_OpenSize));

	wxLayoutAlgorithm layout;
    layout.LayoutFrame(Manager::Get()->GetAppWindow(), Manager::Get()->GetEditorManager()->GetNotebook());
}

void MessageManager::Close(bool force)
{
    if (!m_AutoHide || !m_Open)
        return;
    if (!force && m_LockCounter > 0)
        return;

    m_LockCounter = 0;
    wxSashLayoutWindow* sash = (wxSashLayoutWindow*)GetParent();
    if (!sash)
        return;
//    DebugLog("before m_OpenSize=%d", m_OpenSize);
    m_OpenSize = sash->GetSize().y;
    sash->SetDefaultSize(wxSize(1, m_OpenSize - m_Logs[mltLog]->GetSize().y));
//    DebugLog("after m_OpenSize=%d, actual=%d", m_OpenSize, m_OpenSize - m_Logs[mltLog]->GetSize().y);
    m_Open = false;

	wxLayoutAlgorithm layout;
    layout.LayoutFrame(Manager::Get()->GetAppWindow(), Manager::Get()->GetEditorManager()->GetNotebook());
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

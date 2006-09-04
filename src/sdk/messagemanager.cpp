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
    #include "pluginmanager.h"
    #include "cbplugin.h"
    #include "messagelog.h"
    #include "simpletextlog.h"
    #include "globals.h"
    #include "sdk_events.h"
#endif

#include <wx/laywin.h>
#include <wx/settings.h>
#include <wx/bitmap.h>
#include <wx/utils.h>
#include <wx/filedlg.h>

#include "wxFlatNotebook/wxFlatNotebook.h"

// Custom window to shutdown the app when closed.
// used for batch builds only.
class BatchLogWindow : public wxDialog
{
    public:
        BatchLogWindow(wxWindow *parent, const wxChar *title)
            : wxDialog(parent, -1, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX | wxMINIMIZE_BOX)
        {
            wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
            wxFont font(8, wxMODERN, wxNORMAL, wxNORMAL);
            m_pText = new wxTextCtrl(this, -1, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_MULTILINE | wxTE_RICH2 | wxHSCROLL);
            m_pText->SetFont(font);

            sizer->Add(m_pText, 1, wxGROW);

            wxSize size;
            size.SetWidth(Manager::Get()->GetConfigManager(_T("message_manager"))->ReadInt(_T("/batch_build_log/width"), wxDefaultSize.GetWidth()));
            size.SetHeight(Manager::Get()->GetConfigManager(_T("message_manager"))->ReadInt(_T("/batch_build_log/height"), wxDefaultSize.GetHeight()));
            SetSize(size);

            SetSizer(sizer);
            sizer->Layout();
        }
        void EndModal(int retCode)
        {
            // allowed to close?
            // find compiler plugin
            PluginsArray arr = Manager::Get()->GetPluginManager()->GetCompilerOffers();
            if (arr.GetCount() != 0)
            {
                cbCompilerPlugin* compiler = static_cast<cbCompilerPlugin*>(arr[0]);
                if (compiler && compiler->IsRunning())
                {
                    if (cbMessageBox(_("The build is in progress. Are you sure you want to abort it?"),
                                    _("Abort build?"),
                                    wxICON_QUESTION | wxYES_NO) == wxID_YES)
                    {
                        compiler->KillProcess();
                        while (compiler->IsRunning())
                        {
                            wxMilliSleep(100);
                            Manager::Yield();
                        }
                        wxDialog::EndModal(retCode);
                    }
                    return;
                }
            }

            Manager::Get()->GetConfigManager(_T("message_manager"))->Write(_T("/batch_build_log/width"), (int)GetSize().GetWidth());
            Manager::Get()->GetConfigManager(_T("message_manager"))->Write(_T("/batch_build_log/height"), (int)GetSize().GetHeight());
            wxDialog::EndModal(retCode);
        }
        wxTextCtrl* m_pText;
};

static const int idNB = wxNewId();
static const int idNB_TabTop = wxNewId();
static const int idNB_TabBottom = wxNewId();

// 64 logs should be more than enough
static const int MAX_LOGS = 64;
static const int idNB_ShowHide[MAX_LOGS] =
{
    wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(),
    wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(),
    wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(),
    wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(),
    wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(),
    wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(),
    wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(),
    wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(),
};

BEGIN_EVENT_TABLE(MessageManager, wxEvtHandler)
    EVT_MENU_RANGE(idNB_ShowHide[0], idNB_ShowHide[MAX_LOGS - 1], MessageManager::OnShowHideLog)
    EVT_MENU(idNB_TabTop, MessageManager::OnTabPosition)
    EVT_MENU(idNB_TabBottom, MessageManager::OnTabPosition)
    EVT_APP_STARTUP_DONE(MessageManager::OnAppDoneStartup)
    EVT_APP_START_SHUTDOWN(MessageManager::OnAppStartShutdown)
    EVT_FLATNOTEBOOK_PAGE_CHANGED(idNB, MessageManager::OnPageChanged)
    EVT_FLATNOTEBOOK_CONTEXT_MENU(idNB, MessageManager::OnPageContextMenu)
END_EVENT_TABLE()

// class constructor
MessageManager::MessageManager()
    : m_AppLog(-1),
    m_DebugLog(-1),
    m_BatchBuildLog(-1),
    m_BatchBuildLogDialog(0),
    m_LockCounter(0),
    m_AutoHide(false)
{

    m_pNotebook = new wxFlatNotebook(Manager::Get()->GetAppWindow(), idNB);
    m_pNotebook->SetWindowStyleFlag(Manager::Get()->GetConfigManager(_T("app"))->ReadInt(_T("/environment/message_tabs_style"), wxFNB_BOTTOM | wxFNB_NO_X_BUTTON));
    m_pNotebook->SetImageList(new wxFlatNotebookImageList);

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("message_manager"));

    // add default log and debug images (index 0 and 1)
	wxBitmap bmp;
	wxString prefix;
    prefix = ConfigManager::GetDataFolder() + _T("/images/");
    bmp = cbLoadBitmap(prefix + _T("edit_16x16.png"), wxBITMAP_TYPE_PNG);
    m_pNotebook->GetImageList()->push_back(bmp);
    bmp = cbLoadBitmap(prefix + _T("contents_16x16.png"), wxBITMAP_TYPE_PNG);
    m_pNotebook->GetImageList()->push_back(bmp);

    m_AppLog = DoAddLog(new SimpleTextLog(), _("Code::Blocks"));

    m_DebugLog = DoAddLog(new SimpleTextLog(), _("Code::Blocks Debug"));
    SetLogImage(m_DebugLog, bmp); // set debug log image
	bool hasDebugLog = cfg->ReadBool(_T("/has_debug_log"), false);
	if (hasDebugLog)
	{
	    ShowLog(m_DebugLog, hasDebugLog);
    }

    m_AutoHide = cfg->ReadBool(_T("/auto_hide"), false);
//    Open();

    Manager::Get()->GetAppWindow()->PushEventHandler(this);
}

// class destructor
MessageManager::~MessageManager()
{
//    delete m_BatchBuildLogWindow;

    delete m_pNotebook->GetImageList();
    m_pNotebook->Destroy();
    for (LogsMap::iterator it = m_Logs.begin(); it != m_Logs.end(); ++it)
               delete (*it).second;
}

void MessageManager::CreateMenu(wxMenuBar* menuBar)
{
}

void MessageManager::ReleaseMenu(wxMenuBar* menuBar)
{
}

bool MessageManager::CheckLogId(int id)
{
    return m_Logs[id] != 0L;
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

void MessageManager::LogToStdOut(const wxString& msg)
{
    fputs(msg.mb_str(), stdout);
}

void MessageManager::Log(const wxString& msg)
{
    m_Logs[m_AppLog]->log->AddLog(msg);
}

void MessageManager::Log(const wxChar* msg, ...)
{
    wxString tmp;
    va_list arg_list;

    va_start(arg_list, msg);
    tmp = wxString::FormatV(msg, arg_list);
    va_end(arg_list);

    m_Logs[m_AppLog]->log->AddLog(tmp);
}

void MessageManager::DebugLog(const wxChar* msg, ...)
{
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
    m_Logs[m_DebugLog]->log->AddLog(_T("[") + ts + _T("]: ") + tmp);
//    m_Logs[mltDebug]->AddLog(_T("[") + timestamp.Format(_T("%X.%l")) + _T("]: ") + tmp);
//    m_Logs[mltDebug]->AddLog(tmp);
}

void MessageManager::DebugLogWarning(const wxChar* msg, ...)
{
    if (!CheckLogId(m_DebugLog))
        return;
    wxString tmp;
    va_list arg_list;

    va_start(arg_list, msg);
    tmp = wxString::FormatV(msg, arg_list);
    va_end(arg_list);

    wxString typ = _("WARNING");
//    wxSafeShowMessage(typ, typ + _T(":\n\n") + tmp);
    ((SimpleTextLog*)m_Logs[m_DebugLog]->log)->GetTextControl()->SetDefaultStyle(wxTextAttr(*wxBLUE));
    DebugLog(typ + _T(": ") + tmp);
    ((SimpleTextLog*)m_Logs[m_DebugLog]->log)->GetTextControl()->SetDefaultStyle(wxTextAttr(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT), wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW)));
}

void MessageManager::DebugLogError(const wxChar* msg, ...)
{
    if (!CheckLogId(m_DebugLog))
        return;
    wxString tmp;
    va_list arg_list;

    va_start(arg_list, msg);
    tmp = wxString::FormatV(msg, arg_list);
    va_end(arg_list);

    wxString typ = _("ERROR");
//    wxSafeShowMessage(typ, typ + _T(":\n\n") + tmp);
    ((SimpleTextLog*)m_Logs[m_DebugLog]->log)->GetTextControl()->SetDefaultStyle(wxTextAttr(*wxRED));
    DebugLog(typ + _T(": ") + tmp);
    ((SimpleTextLog*)m_Logs[m_DebugLog]->log)->GetTextControl()->SetDefaultStyle(wxTextAttr(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT), wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW)));
}

// add a new log page
int MessageManager::AddLog(MessageLog* log, const wxString& title, const wxBitmap& bitmap)
{
    return DoAddLog(log, title, bitmap);
}

void MessageManager::RemoveLog(MessageLog* log)
{
    int idx = m_pNotebook->GetPageIndex(log);
    m_pNotebook->RemovePage(idx);

    log->Hide();
    log->Reparent(Manager::Get()->GetAppWindow());

    // find it and remove it from the map
    for (LogsMap::iterator it = m_Logs.begin(); it != m_Logs.end(); ++it)
    {
        if (it->second->log == log)
        {
            delete (*it).second;
            m_Logs.erase(it);
            break;
        }
    }
}

void MessageManager::RemoveLog(int id)
{
    if (!CheckLogId(id))
        return;
    RemoveLog(m_Logs[id]->log);
}

void MessageManager::ShowLog(MessageLog* log, bool show)
{
    // do we manage it?
    int id = -1;
    LogStruct* ls = 0;
    for (LogsMap::iterator it = m_Logs.begin(); it != m_Logs.end(); ++it)
    {
        if (it->second->log == log)
        {
            id = it->first;
            ls = it->second;
            break;
        }
    }
    if (!ls)
        return;

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("message_manager"));

    if (show && !ls->visible)
    {
        // show
        m_pNotebook->InsertPage(id, log, ls->title, true);

        SetLogImage(id, ls->bitmap);
        ls->visible = true;

        log->Show(true);

        if (id == m_DebugLog)
            cfg->Write(_T("/has_debug_log"), (bool)true);
    }
    else if (!show && ls->visible)
    {
        // hide
        int id = m_pNotebook->GetPageIndex(log);
        if (id != -1)
            m_pNotebook->RemovePage(id);
        ls->visible = false;

        log->Show(false);

        if (id == m_DebugLog)
            cfg->Write(_T("/has_debug_log"), (bool)false);
    }
}

void MessageManager::ShowLog(int id, bool show)
{
    if (CheckLogId(id))
    {
        ShowLog(m_Logs[id]->log, show);
    }
}

wxDialog* MessageManager::GetBatchBuildDialog()
{
    if (!m_BatchBuildLogDialog)
        m_BatchBuildLogDialog = new BatchLogWindow(Manager::Get()->GetAppWindow(), _("Batch build"));
    return m_BatchBuildLogDialog;
}

void MessageManager::SetBatchBuildLog(int log)
{
    if (CheckLogId(log))
        m_BatchBuildLog = log;
}

// add a new log page
int MessageManager::DoAddLog(MessageLog* log, const wxString& title, const wxBitmap& bitmap)
{
    static int pageId = 0;
    int id = pageId++;

    log->m_PageId = id;
    LogStruct* ls = new LogStruct;
    ls->log = log;
    ls->visible = true;
    ls->title = title;
    ls->bitmap = bitmap;
    m_Logs[id] = ls;
    m_pNotebook->AddPage(log, title, false);
    SetLogImage(id, bitmap);
    return id;
}

void MessageManager::Log(int id, const wxString& msg)
{
    if (!CheckLogId(id))
        return;

    m_Logs[id]->log->AddLog(msg);

    if (Manager::IsBatchBuild() && id == m_BatchBuildLog)
    {
        // this log is the batch build log
        if (!m_BatchBuildLogDialog)
            GetBatchBuildDialog();
        BatchLogWindow* dlg = static_cast<BatchLogWindow*>(m_BatchBuildLogDialog);
        if (dlg->m_pText)
        {
            dlg->m_pText->AppendText(msg + _T('\n')); // log to build log window
            dlg->m_pText->ScrollLines(-1);
            Manager::ProcessPendingEvents();
        }
    }
}

void MessageManager::Log(int id, const wxChar* msg, ...)
{
    if (!CheckLogId(id))
        return;

    wxString tmp;
    va_list arg_list;

    va_start(arg_list, msg);
    tmp = wxString::FormatV(msg, arg_list);
    va_end(arg_list);

    m_Logs[id]->log->AddLog(tmp);

    if (Manager::IsBatchBuild() && id == m_BatchBuildLog)
    {
        // this log is the batch build log
        if (!m_BatchBuildLogDialog)
            GetBatchBuildDialog();
        BatchLogWindow* dlg = static_cast<BatchLogWindow*>(m_BatchBuildLogDialog);
        if (dlg->m_pText)
        {
            dlg->m_pText->AppendText(tmp + _T('\n')); // log to build log window
            dlg->m_pText->ScrollLines(-1);
            Manager::ProcessPendingEvents();
        }
    }
}

void MessageManager::AppendLog(const wxChar* msg, ...)
{
    wxString tmp;
    va_list arg_list;

    va_start(arg_list, msg);
    tmp = wxString::FormatV(msg, arg_list);
    va_end(arg_list);

    m_Logs[m_AppLog]->log->AddLog(tmp, false);
}

void MessageManager::AppendLog(int id, const wxChar* msg, ...)
{
    if (!CheckLogId(id))
        return;

    wxString tmp;
    va_list arg_list;

    va_start(arg_list, msg);
    tmp = wxString::FormatV(msg, arg_list);
    va_end(arg_list);

    m_Logs[id]->log->AddLog(tmp, false);
}

// switch to log page
void MessageManager::SwitchTo(int id)
{
    if (!CheckLogId(id))
        return;
    int index = m_pNotebook->GetPageIndex(m_Logs[id]->log);
    m_pNotebook->SetSelection(index);
}

void MessageManager::SetLogImage(int id, const wxBitmap& bitmap)
{
    if (!CheckLogId(id))
        return;

    int index = m_pNotebook->GetPageIndex(m_Logs[id]->log);
    if (!bitmap.Ok())
        m_pNotebook->SetPageImageIndex(index, 0); // default log image
    else
    {
        m_pNotebook->GetImageList()->push_back(bitmap);
        m_pNotebook->SetPageImageIndex(index, m_pNotebook->GetImageList()->size() - 1);
        m_Logs[id]->bitmap = bitmap;
    }
}

void MessageManager::SetLogImage(MessageLog* log, const wxBitmap& bitmap)
{
    if (log)
        SetLogImage(log->GetPageId(), bitmap);
}

void MessageManager::EnableAutoHide(bool enable)
{
    m_AutoHide = true; // hack to force Open() and Close() to work
    if (enable)
        Close();
    else
        Open();
    m_AutoHide = enable;
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

void MessageManager::ResetLogFont()
{
    for (LogsMap::iterator it = m_Logs.begin(); it != m_Logs.end(); ++it)
    {
        it->second->log->ResetLogFont();
    }
}

void MessageManager::OnTabPosition(wxCommandEvent& event)
{
    long style = m_pNotebook->GetWindowStyleFlag();
    style &= ~wxFNB_BOTTOM;

    if (event.GetId() == idNB_TabBottom)
        style |= wxFNB_BOTTOM;
    m_pNotebook->SetWindowStyleFlag(style);
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

void MessageManager::OnShowHideLog(wxCommandEvent& event)
{
    // find log from index
    int idx = event.GetId() - idNB_ShowHide[0];
    int count = 0;
    for (LogsMap::iterator it = m_Logs.begin(); it != m_Logs.end() && idx < MAX_LOGS; ++it)
    {
        if (idx == count++)
        {
            ShowLog(it->second->log, !it->second->visible);
            break;
        }
    }
}

void MessageManager::OnPageChanged(wxFlatNotebookEvent& event)
{
    if (m_AutoHide && event.GetEventObject() == this)
    {
        Open();
    }

    event.Skip();
}

void MessageManager::OnPageContextMenu(wxFlatNotebookEvent& event)
{
    wxMenu* NBmenu = new wxMenu();
    NBmenu->Append(idNB_TabTop, _("Tabs at top"));
    NBmenu->Append(idNB_TabBottom, _("Tabs at bottom"));
    NBmenu->AppendSeparator();

    wxMenu* sub = new wxMenu;
    int idx = 0;
    for (LogsMap::iterator it = m_Logs.begin(); it != m_Logs.end() && idx < MAX_LOGS; ++it, ++idx)
    {
        if (idx == 0)
            continue; // don't hide the app log
        wxMenuItem* item = sub->AppendCheckItem(idNB_ShowHide[idx], it->second->title);
        item->Check(it->second->visible);
    }

    NBmenu->Append(0, _("Show/hide"), sub);

    m_pNotebook->PopupMenu(NBmenu);
    delete NBmenu;
}

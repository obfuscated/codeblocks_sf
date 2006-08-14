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
    #include <wx/process.h>
    #include <wx/menu.h>
    #include <wx/msgdlg.h>
    #include "toolsmanager.h"
    #include "manager.h"
    #include "macrosmanager.h"
    #include "configmanager.h"
    #include "messagemanager.h"
    #include "configmanager.h"
    #include "pipedprocess.h"
    #include "globals.h"
    #include "sdk_events.h"
#endif

#include <wx/mdi.h>
#include <wx/listimpl.cpp>
#include "configuretoolsdlg.h"

WX_DEFINE_LIST(ToolsList);

const int idToolsConfigure = wxNewId();
const int idToolProcess = wxNewId();

BEGIN_EVENT_TABLE(ToolsManager, wxEvtHandler)
    EVT_MENU(idToolsConfigure, ToolsManager::OnConfigure)

    EVT_IDLE(ToolsManager::OnIdle)

    EVT_PIPEDPROCESS_STDOUT(idToolProcess, ToolsManager::OnToolStdOutput)
    EVT_PIPEDPROCESS_STDERR(idToolProcess, ToolsManager::OnToolErrOutput)
    EVT_PIPEDPROCESS_TERMINATED(idToolProcess, ToolsManager::OnToolTerminated)
END_EVENT_TABLE()

ToolsManager::ToolsManager()
    : m_Menu(0L),
    m_pProcess(0L),
    m_Pid(0)
{
    LoadTools();
    Manager::Get()->GetAppWindow()->PushEventHandler(this);
}

ToolsManager::~ToolsManager()
{
    // this is a core manager, so it is removed when the app is shutting down.
    // in this case, the app has already un-hooked us, so no need to do it ourselves...
//    Manager::Get()->GetAppWindow()->RemoveEventHandler(this);

    m_ItemsManager.Clear( m_Menu );

    // free-up any memory used for tools
    m_Tools.DeleteContents(true);
    m_Tools.Clear();
}

void ToolsManager::CreateMenu(wxMenuBar* menuBar)
{
}

void ToolsManager::ReleaseMenu(wxMenuBar* menuBar)
{
}

bool ToolsManager::Execute(const cbTool* tool)
{
    if (m_pProcess)
    {
        cbMessageBox(_("Another tool is currently executing.\n"
                        "Please allow for it to finish before launching another tool..."),
                        _("Error"), wxICON_ERROR);
        return false;
    }

    if (!tool)
        return false;

    wxString cmdline;
    wxString cmd = tool->GetCommand();
    wxString params = tool->GetParams();
    wxString dir = tool->GetWorkingDir();

    // hack to force-update macros
    Manager::Get()->GetMacrosManager()->RecalcVars(0, 0, 0);

    Manager::Get()->GetMacrosManager()->ReplaceMacros(cmd);
    Manager::Get()->GetMacrosManager()->ReplaceMacros(params);
    Manager::Get()->GetMacrosManager()->ReplaceMacros(dir);

    if (tool->GetLaunchOption() == cbTool::LAUNCH_NEW_CONSOLE_WINDOW)
    {
#ifndef __WXMSW__
        // for non-win platforms, use m_ConsoleTerm to run the console app
        wxString term = Manager::Get()->GetConfigManager(_T("app"))->Read(_T("/console_terminal"), DEFAULT_CONSOLE_TERM);
        term.Replace(_T("$TITLE"), _T("'") + tool->GetName() + _T("'"));
        cmdline << term << _T(" ");
        #define CONSOLE_RUNNER "cb_console_runner"
#else
        #define CONSOLE_RUNNER "cb_console_runner.exe"
#endif
        wxString baseDir = ConfigManager::GetExecutableFolder();
        if (wxFileExists(baseDir + wxT("/" CONSOLE_RUNNER)))
            cmdline << baseDir << wxT("/" CONSOLE_RUNNER " ");
    }

    cmdline << cmd << _T(" ") << params;
    if(!(Manager::Get()->GetMacrosManager()))
        return false; // We cannot afford the Macros Manager to fail here!
                      // What if it failed already?
    wxSetWorkingDirectory(dir);

    // log info so user can troubleshoot
    dir = wxGetCwd(); // read in the actual working dir
    Manager::Get()->GetMessageManager()->Log(_("Launching tool '%s': %s (in %s)"), tool->GetName().c_str(), cmdline.c_str(), dir.c_str());

    bool pipe = true;
    int flags = wxEXEC_ASYNC;

    switch (tool->GetLaunchOption())
    {
        case cbTool::LAUNCH_NEW_CONSOLE_WINDOW:
            pipe = false; // no need to pipe output channels...
            break;

        case cbTool::LAUNCH_HIDDEN:
            break; // use the default values of pipe and flags...

        case cbTool::LAUNCH_VISIBLE:
        case cbTool::LAUNCH_VISIBLE_DETACHED:
            flags |= wxEXEC_NOHIDE;
            pipe = false;
            break;
    }

    if (tool->GetLaunchOption() == cbTool::LAUNCH_VISIBLE_DETACHED)
    {
        int pid = wxExecute(cmdline, flags);

        if (!pid)
        {
            cbMessageBox(_("Couldn't execute tool. Check the log for details."), _("Error"), wxICON_ERROR);
            return false;
        }
        else
        {
            Manager::Get()->GetMessageManager()->SwitchTo(0); // switch to default log
        }
    }
    else
    {
        m_pProcess = new PipedProcess((void**)&m_pProcess, this, idToolProcess, pipe, dir);
        m_Pid = wxExecute(cmdline, flags, m_pProcess);

        if (!m_Pid)
        {
            cbMessageBox(_("Couldn't execute tool. Check the log for details."), _("Error"), wxICON_ERROR);
            delete m_pProcess;
            m_pProcess = 0;
            m_Pid = 0;
            return false;
        }
        else
        {
            Manager::Get()->GetMessageManager()->SwitchTo(0); // switch to default log
        }
    }

    return true;
}

void ToolsManager::AddTool(const cbTool* tool, bool save)
{
    if (tool)
        InsertTool(m_Tools.GetCount(), tool, save);
}

void ToolsManager::InsertTool(int position, const cbTool* tool, bool save)
{
    m_Tools.Insert(position, new cbTool(*tool));
    if (save)
        SaveTools();
}

void ToolsManager::RemoveToolByIndex(int index)
{
    int idx = 0;
    for (ToolsList::Node* node = m_Tools.GetFirst(); node; node = node->GetNext())
    {
        if (idx == index)
        {
            DoRemoveTool(node);
            return;
        }
        ++idx;
    }
}

void ToolsManager::DoRemoveTool(ToolsList::Node* node)
{
    if (node)
    {
        if (node->GetData()->GetMenuId() != -1)
            m_Menu->Delete(node->GetData()->GetMenuId());
        m_Tools.DeleteNode(node);
        SaveTools();
    }
}

cbTool* ToolsManager::GetToolByMenuId(int id)
{
    for (ToolsList::Node* node = m_Tools.GetFirst(); node; node = node->GetNext())
    {
        cbTool* tool = node->GetData();
        if (tool->GetMenuId() == id)
            return tool;
    }
    return 0L;
}

cbTool* ToolsManager::GetToolByIndex(int index)
{
    int idx = 0;
    for (ToolsList::Node* node = m_Tools.GetFirst(); node; node = node->GetNext())
    {
        cbTool* tool = node->GetData();
        if (idx == index)
            return tool;
        ++idx;
    }
    return 0L;
}

void ToolsManager::LoadTools()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("tools"));
    wxArrayString list = cfg->EnumerateSubPaths(_("/"));
    for (unsigned int i = 0; i < list.GetCount(); ++i)
    {
        cbTool tool;
        tool.SetName( cfg->Read(_T("/") + list[i] + _T("/name")));
        if (tool.GetName().IsEmpty())
            continue;
        tool.SetCommand(cfg->Read(_T("/") + list[i] + _T("/command")));
        if (tool.GetCommand().IsEmpty())
            continue;
        tool.SetParams(cfg->Read(_T("/") + list[i] + _T("/params")));
        tool.SetWorkingDir(cfg->Read(_T("/") + list[i] + _T("/workingDir")));
        tool.SetLaunchOption(static_cast<cbTool::eLaunchOption>(cfg->ReadInt(_T("/") + list[i] + _T("/launchOption"))));

        AddTool(&tool, false);
    }
    Manager::Get()->GetMessageManager()->Log(_("Configured %d tools"), m_Tools.GetCount());
}

void ToolsManager::SaveTools()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("tools"));
    wxArrayString list = cfg->EnumerateSubPaths(_("/"));
    for (unsigned int i = 0; i < list.GetCount(); ++i)
    {
        cfg->DeleteSubPath(list[i]);
    }

    int count = 0;
    for (ToolsList::Node* node = m_Tools.GetFirst(); node; node = node->GetNext())
    {
        cbTool* tool = node->GetData();
        wxString elem;

        // prepend a 0-padded 2-digit number to keep ordering
        wxString tmp;
        tmp.Printf(_T("tool%2.2d"), count++);

        elem << _T("/") << tmp  << _T("/");
        cfg->Write(elem + _T("name"), tool->GetName());
        cfg->Write(elem + _T("command"), tool->GetCommand());
        cfg->Write(elem + _T("params"), tool->GetParams());
        cfg->Write(elem + _T("workingDir"), tool->GetWorkingDir());
        cfg->Write(elem + _T("launchOption"), static_cast<int>(tool->GetLaunchOption()));
    }
}

void ToolsManager::BuildToolsMenu(wxMenu* menu)
{
    // clear previously added menu items
    m_ItemsManager.Clear(menu);

    // add menu items for tools
    m_Menu = menu;
    if (m_Menu->GetMenuItemCount() > 0)
    {
        m_ItemsManager.Add(menu, wxID_SEPARATOR, _T(""), _T(""));
    }

    for (ToolsList::Node* node = m_Tools.GetFirst(); node; node = node->GetNext())
    {
        cbTool* tool = node->GetData();
        if (tool->GetMenuId() == -1)
        {
            tool->SetMenuId(wxNewId());
        }
        m_ItemsManager.Add(menu, tool->GetMenuId(), tool->GetName(), tool->GetName());
        Connect(tool->GetMenuId(), -1, wxEVT_COMMAND_MENU_SELECTED,
                (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                &ToolsManager::OnToolClick);
    }

    if (m_Tools.GetCount() > 0)
    {
        m_ItemsManager.Add(menu, wxID_SEPARATOR, _T(""), _T(""));
    }
    m_ItemsManager.Add(menu, idToolsConfigure, _("&Configure tools..."), _("Add/remove user-defined tools"));
}

int ToolsManager::Configure()
{
    ConfigureToolsDlg dlg(Manager::Get()->GetAppWindow());
    PlaceWindow(&dlg);
    dlg.ShowModal();
    SaveTools();
    BuildToolsMenu(m_Menu);
    return 0;
} // end of Configure

// events

void ToolsManager::OnConfigure(wxCommandEvent& event)
{
    Configure();
}

void ToolsManager::OnToolClick(wxCommandEvent& event)
{
    cbTool* tool = GetToolByMenuId(event.GetId());
    if (!Execute(tool))
        cbMessageBox(_("Could not execute ") + tool->GetName());
}

void ToolsManager::OnIdle(wxIdleEvent& event)
{
    if (m_pProcess)
    {
        if (m_pProcess->HasInput())
        {
            event.RequestMore();
        }
    }
    else
        event.Skip();
}

void ToolsManager::OnToolStdOutput(CodeBlocksEvent& event)
{
    Manager::Get()->GetMessageManager()->Log(_T("stdout> %s"), event.GetString().c_str());
}

void ToolsManager::OnToolErrOutput(CodeBlocksEvent& event)
{
    Manager::Get()->GetMessageManager()->Log(_T("stderr> %s"), event.GetString().c_str());
}

void ToolsManager::OnToolTerminated(CodeBlocksEvent& event)
{
    m_Pid = 0;
    m_pProcess = 0;

    Manager::Get()->GetMessageManager()->Log(_T("Tool execution terminated with status %d"), event.GetInt());
}

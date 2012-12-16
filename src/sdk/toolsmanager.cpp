/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
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
    #include "logmanager.h"
    #include "configmanager.h"
    #include "pipedprocess.h"
    #include "globals.h"
    #include "sdk_events.h"
#endif

#include <wx/mdi.h>
#include <wx/listimpl.cpp>
#include "configuretoolsdlg.h"

template<> ToolsManager* Mgr<ToolsManager>::instance = 0;
template<> bool  Mgr<ToolsManager>::isShutdown = false;

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

    m_ItemsManager.Clear();

    // free-up any memory used for tools
    m_Tools.DeleteContents(true);
    m_Tools.Clear();
}

void ToolsManager::CreateMenu(cb_unused wxMenuBar* menuBar)
{
}

void ToolsManager::ReleaseMenu(cb_unused wxMenuBar* menuBar)
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

    if (!cmdline.Replace(_T("$SCRIPT"), cmd << _T(" ") << params))
        // if they didn't specify $SCRIPT, append:
        cmdline << cmd;

    if(!(Manager::Get()->GetMacrosManager()))
        return false; // We cannot afford the Macros Manager to fail here!
                      // What if it failed already?
    wxSetWorkingDirectory(dir);

    // log info so user can troubleshoot
    dir = wxGetCwd(); // read in the actual working dir
    #if wxCHECK_VERSION(2, 9, 0)
    Manager::Get()->GetLogManager()->Log(F(_("Launching tool '%s': %s (in %s)"), tool->GetName().wx_str(), cmdline.wx_str(), dir.wx_str()));
    #else
    Manager::Get()->GetLogManager()->Log(F(_("Launching tool '%s': %s (in %s)"), tool->GetName().c_str(), cmdline.c_str(), dir.c_str()));
    #endif

    bool pipe = true;
    int flags = wxEXEC_ASYNC;

    switch (tool->GetLaunchOption())
    {
        case cbTool::LAUNCH_NEW_CONSOLE_WINDOW:
            pipe = false; // no need to pipe output channels...
            break;

        case cbTool::LAUNCH_VISIBLE:
        case cbTool::LAUNCH_VISIBLE_DETACHED:
            flags |= wxEXEC_NOHIDE;
            pipe = false;
            break;

        case cbTool::LAUNCH_HIDDEN: // fall-through
        default:
            break; // use the default values of pipe and flags...
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
            CodeBlocksLogEvent evtSwitch(cbEVT_SWITCH_TO_LOG_WINDOW, LogManager::app_log);
            Manager::Get()->ProcessEvent(evtSwitch);        // switch to default log
         }
    }
    else
    {
        m_pProcess = new PipedProcess(&m_pProcess, this, idToolProcess, pipe, dir);
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
            CodeBlocksLogEvent evtSwitch(cbEVT_SWITCH_TO_LOG_WINDOW, LogManager::app_log);
            Manager::Get()->ProcessEvent(evtSwitch);        // switch to default log
        }
    }

    return true;
} // end of Execute

void ToolsManager::AddTool(const cbTool* tool, bool save)
{
    if (tool)
    {
        InsertTool(m_Tools.GetCount(), tool, save);
    }
} // end of AddTool

void ToolsManager::InsertTool(int position, const cbTool* tool, bool save)
{
    m_Tools.Insert(position, new cbTool(*tool));
    if (save)
    {
        SaveTools();
    }
} // end of InsertTool

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
    Manager::Get()->GetLogManager()->Log(F(_("Configured %d tools"), m_Tools.GetCount()));
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
    m_ItemsManager.Clear();

    // add menu items for tools
    m_Menu = menu;
    if (m_Menu->GetMenuItemCount() > 0)
    {
        m_ItemsManager.Add(menu, wxID_SEPARATOR, _T(""), _T(""));
    }

    for (ToolsList::Node* node = m_Tools.GetFirst(); node; node = node->GetNext())
    {
        cbTool* tool = node->GetData();
        if (tool->GetName() == CB_TOOLS_SEPARATOR)
        {
            m_ItemsManager.Add(menu, wxID_SEPARATOR, _T(""), _T(""));
            continue;
        }
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
    CodeBlocksEvent event(cbEVT_MENUBAR_CREATE_BEGIN);
    Manager::Get()->ProcessEvent(event);

    ConfigureToolsDlg dlg(Manager::Get()->GetAppWindow());
    PlaceWindow(&dlg);
    dlg.ShowModal();
    SaveTools();
    BuildToolsMenu(m_Menu);

    CodeBlocksEvent event2(cbEVT_MENUBAR_CREATE_END);
    Manager::Get()->ProcessEvent(event2);

    return 0;
} // end of Configure

// events

void ToolsManager::OnConfigure(cb_unused wxCommandEvent& event)
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
    Manager::Get()->GetLogManager()->Log(_T("stdout> ") + event.GetString());
}

void ToolsManager::OnToolErrOutput(CodeBlocksEvent& event)
{
    Manager::Get()->GetLogManager()->Log(_T("stderr> ") + event.GetString());
}

void ToolsManager::OnToolTerminated(CodeBlocksEvent& event)
{
    m_Pid = 0;
    m_pProcess = 0;

    Manager::Get()->GetLogManager()->Log(F(_T("Tool execution terminated with status %d"), event.GetInt()));
}

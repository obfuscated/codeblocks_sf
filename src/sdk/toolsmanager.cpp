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
    #include "globals.h"
#endif

#include <wx/mdi.h>
#include <wx/listimpl.cpp>
#include "configuretoolsdlg.h"

WX_DEFINE_LIST(ToolsList);

int idToolsConfigure = wxNewId();

BEGIN_EVENT_TABLE(ToolsManager, wxEvtHandler)
	EVT_MENU(idToolsConfigure, ToolsManager::OnConfigure)
END_EVENT_TABLE()

ToolsManager::ToolsManager()
	: m_Menu(0L)
{
    SC_CONSTRUCTOR_BEGIN
	LoadTools();
	Manager::Get()->GetAppWindow()->PushEventHandler(this);
}

ToolsManager::~ToolsManager()
{
    SC_DESTRUCTOR_BEGIN

    // this is a core manager, so it is removed when the app is shutting down.
    // in this case, the app has already un-hooked us, so no need to do it ourselves...
//	Manager::Get()->GetAppWindow()->RemoveEventHandler(this);

	m_ItemsManager.Clear( m_Menu );

    // free-up any memory used for tools
    m_Tools.DeleteContents(true);
    m_Tools.Clear();
    SC_DESTRUCTOR_END
}

void ToolsManager::CreateMenu(wxMenuBar* menuBar)
{
    SANITY_CHECK();
}

void ToolsManager::ReleaseMenu(wxMenuBar* menuBar)
{
    SANITY_CHECK();
}

bool ToolsManager::Execute(Tool* tool)
{
    SANITY_CHECK(false);
	if (!tool)
		return false;

	wxString cmdline;
	wxString cmd = tool->command;
	wxString params = tool->params;
	wxString dir = tool->workingDir;

	Manager::Get()->GetMacrosManager()->ReplaceMacros(cmd);
	Manager::Get()->GetMacrosManager()->ReplaceMacros(params);
	Manager::Get()->GetMacrosManager()->ReplaceMacros(dir);

	cmdline << cmd << _T(" ") << params;
    SANITY_CHECK(false);
    if(!(Manager::Get()->GetMacrosManager()))
        return false; // We cannot afford the Macros Manager to fail here!
                      // What if it failed already?
    wxSetWorkingDirectory(dir);
	wxProcess* process = new wxProcess();
	return wxExecute(cmdline, wxEXEC_ASYNC, process);
}

void ToolsManager::AddTool(const wxString& name, const wxString& command, const wxString& params, const wxString& workingDir, bool save)
{
    SANITY_CHECK();
	Tool tool;
	tool.name = name;
	tool.command = command;
	tool.params = params;
	tool.workingDir = workingDir;
	InsertTool(m_Tools.GetCount(), &tool, save);
}

void ToolsManager::AddTool(Tool* tool, bool save)
{
    SANITY_CHECK();
	if (tool)
		InsertTool(m_Tools.GetCount(), tool, save);
}

void ToolsManager::InsertTool(int position, Tool* tool, bool save)
{
    SANITY_CHECK();
	//Manager::Get()->GetMessageManager()->DebugLog("Creating tool: %s", tool->name.c_str());
	m_Tools.Insert(position, new Tool(*tool));
	if (save)
		SaveTools();
}

void ToolsManager::RemoveToolByIndex(int index)
{
    SANITY_CHECK();
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

void ToolsManager::RemoveToolByName(const wxString& name)
{
    SANITY_CHECK();
	for (ToolsList::Node* node = m_Tools.GetFirst(); node; node = node->GetNext())
	{
		Tool* tool = node->GetData();
		if (name.Matches(tool->name))
		{
			DoRemoveTool(node);
			return;
		}
	}
}

void ToolsManager::DoRemoveTool(ToolsList::Node* node)
{
    SANITY_CHECK();
	if (node)
	{
		if (node->GetData()->menuId != -1)
			m_Menu->Delete(node->GetData()->menuId);
		m_Tools.DeleteNode(node);
		SaveTools();
	}
}

Tool* ToolsManager::GetToolById(int id)
{
    SANITY_CHECK(0L);
	for (ToolsList::Node* node = m_Tools.GetFirst(); node; node = node->GetNext())
	{
		Tool* tool = node->GetData();
		if (tool->menuId == id)
			return tool;
	}
	return 0L;
}

Tool* ToolsManager::GetToolByIndex(int index)
{
    SANITY_CHECK(0L);
	int idx = 0;
	for (ToolsList::Node* node = m_Tools.GetFirst(); node; node = node->GetNext())
	{
		Tool* tool = node->GetData();
		if (idx == index)
			return tool;
		++idx;
	}
	return 0L;
}

void ToolsManager::LoadTools()
{
    SANITY_CHECK();

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("tools"));
    wxArrayString list = cfg->EnumerateSubPaths(_("/"));
    for (unsigned int i = 0; i < list.GetCount(); ++i)
	{
		Tool tool;
		tool.name = cfg->Read(_T("/") + list[i] + _T("/name"));
		if (tool.name.IsEmpty())
            continue;
		tool.command = cfg->Read(_T("/") + list[i] + _T("/command"));
		if (tool.command.IsEmpty())
            continue;
		tool.params = cfg->Read(_T("/") + list[i] + _T("/params"));
		tool.workingDir = cfg->Read(_T("/") + list[i] + _T("/workingDir"));

		AddTool(&tool, false);
	}
	Manager::Get()->GetMessageManager()->Log(_("Configured %d tools"), m_Tools.GetCount());
}

void ToolsManager::SaveTools()
{
    SANITY_CHECK();
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("tools"));
    wxArrayString list = cfg->EnumerateSubPaths(_("/"));
    for (unsigned int i = 0; i < list.GetCount(); ++i)
	{
	    cfg->DeleteSubPath(list[i]);
	}

	int count = 0;
	for (ToolsList::Node* node = m_Tools.GetFirst(); node; node = node->GetNext())
	{
		Tool* tool = node->GetData();
		wxString elem;

		// prepend a 0-padded 2-digit number to keep ordering
		wxString tmp;
		tmp.Printf(_("tool%2.2d"), count++);

		elem << _T("/") << tmp  << _T("/");
		cfg->Write(elem + _T("name"), tool->name);
		cfg->Write(elem + _T("command"), tool->command);
		cfg->Write(elem + _T("params"), tool->params);
		cfg->Write(elem + _T("workingDir"), tool->workingDir);
	}
}

void ToolsManager::BuildToolsMenu(wxMenu* menu)
{
    SANITY_CHECK();
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
		Tool* tool = node->GetData();
		if (tool->menuId == -1)
			tool->menuId = wxNewId();
        m_ItemsManager.Add(menu, tool->menuId, tool->name, tool->name);
		Connect(tool->menuId, -1, wxEVT_COMMAND_MENU_SELECTED,
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
    SANITY_CHECK(0);
	ConfigureToolsDlg dlg(Manager::Get()->GetAppWindow());
    PlaceWindow(&dlg);
	dlg.ShowModal();
	SaveTools();
	BuildToolsMenu(m_Menu);
	return 0;
}

// events

void ToolsManager::OnConfigure(wxCommandEvent& event)
{
    SANITY_CHECK();
	Configure();
}

void ToolsManager::OnToolClick(wxCommandEvent& event)
{
    SANITY_CHECK();
	Tool* tool = GetToolById(event.GetId());
	if (!Execute(tool))
		wxMessageBox(_("Could not execute ") + tool->name);
}

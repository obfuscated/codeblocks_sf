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
#include <wx/process.h>
#include <wx/menu.h>
#include <wx/mdi.h>
#include <wx/msgdlg.h>

#include "toolsmanager.h"
#include "manager.h"
#include "macrosmanager.h"
#include "configmanager.h"
#include "messagemanager.h"
#include "configuretoolsdlg.h"
#include "managerproxy.h"
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(ToolsList);

ToolsManager* ToolsManager::Get()
{
    if(Manager::isappShuttingDown()) // The mother of all sanity checks
        ToolsManager::Free();
    else 
    if (!ToolsManagerProxy::Get())
	{
        ToolsManagerProxy::Set( new ToolsManager() );
		Manager::Get()->GetMessageManager()->Log(_("ToolsManager initialized"));
	}
    return ToolsManagerProxy::Get();
}

void ToolsManager::Free()
{
	if (ToolsManagerProxy::Get())
	{
		delete ToolsManagerProxy::Get();
		ToolsManagerProxy::Set( 0L );
	}
}

int idToolsConfigure = wxNewId();

BEGIN_EVENT_TABLE(ToolsManager, wxEvtHandler)
	EVT_MENU(idToolsConfigure, ToolsManager::OnConfigure)
END_EVENT_TABLE()

ToolsManager::ToolsManager()
	: m_Menu(0L)
{
    SC_CONSTRUCTOR_BEGIN
	LoadTools();
	ConfigManager::AddConfiguration(_("Tools"), "/tools");
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

	cmdline << cmd << " " << params;
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
	wxString str;
	long cookie;
	
	ConfigManager::Get()->SetPath("/tools");
	bool cont = ConfigManager::Get()->GetFirstGroup(str, cookie);
	while (cont)
	{
		Tool tool;
		ConfigManager::Get()->Read("/tools/" + str + "/command", &tool.command);
		ConfigManager::Get()->Read("/tools/" + str + "/params", &tool.params);
		ConfigManager::Get()->Read("/tools/" + str + "/workingDir", &tool.workingDir);

		// remove ordering number
		if (str.GetChar(2) == ' ' && str.Left(2).IsNumber())
			str.Remove(0, 3);
		tool.name = str;

		AddTool(&tool, false);
		cont = ConfigManager::Get()->GetNextGroup(str, cookie);
	}
	ConfigManager::Get()->SetPath("/");
	Manager::Get()->GetMessageManager()->Log("Configured %d tools", m_Tools.GetCount());
}

void ToolsManager::SaveTools()
{
    SANITY_CHECK();
	int count = 0;
	ConfigManager::Get()->DeleteGroup("/tools");
	for (ToolsList::Node* node = m_Tools.GetFirst(); node; node = node->GetNext())
	{
		Tool* tool = node->GetData();
		wxString elem;
		
		// prepend a 0-padded 2-digit number to keep ordering
		wxString tmp;
		tmp.Printf("%2.2d", count++);
		
		elem << "/tools/" << tmp << " " << tool->name << "/";
		ConfigManager::Get()->Write(elem + "command", tool->command);
		ConfigManager::Get()->Write(elem + "params", tool->params);
		ConfigManager::Get()->Write(elem + "workingDir", tool->workingDir);
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
        m_ItemsManager.Add(menu, wxID_SEPARATOR, "", "");
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
        m_ItemsManager.Add(menu, wxID_SEPARATOR, "", "");
	}
    m_ItemsManager.Add(menu, idToolsConfigure, _("&Configure tools..."), _("Add/remove user-defined tools"));
}

int ToolsManager::Configure()
{
    SANITY_CHECK(0);
	ConfigureToolsDlg dlg(Manager::Get()->GetAppWindow());
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

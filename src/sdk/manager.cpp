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

#include <wx/xrc/xmlres.h>
#include <wx/fs_zip.h>
#include <wx/mdi.h>
#include <wx/notebook.h>

#include "manager.h" // class's header file
#include "projectmanager.h"
#include "editormanager.h"
#include "messagemanager.h"
#include "pluginmanager.h"
#include "toolsmanager.h"
#include "macrosmanager.h"
#include "configmanager.h"
#include "templatemanager.h"

Manager* g_Manager = 0L;

Manager* Manager::Get(wxMDIParentFrame* appWindow, wxNotebook* notebook)
{
    if (!g_Manager && appWindow)
	{
        g_Manager = new Manager(appWindow, notebook);
		g_Manager->GetMessageManager()->Log(_("Manager initialized"));
	}
    return g_Manager;
}

void Manager::Free()
{
	if (g_Manager)
	{
		MacrosManager::Free();
		ToolsManager::Free();
		TemplateManager::Free();
		PluginManager::Free();
		EditorManager::Free();
		//ProjectManager::Free();
		MessageManager::Free();
		delete g_Manager;
		g_Manager = 0L;
	}
}

// class constructor
Manager::Manager(wxMDIParentFrame* appWindow, wxNotebook* notebook)
	: m_pAppWindow(appWindow),
	m_pNotebook(notebook)
{
    // Basically, this is the very first place that will be called in the lib
    // (through Manager::Get()), so it's a very good place to load and initialize
    // any resources we 'll be using...
    wxFileSystem::AddHandler(new wxZipFSHandler);
    wxXmlResource::Get()->InitAllHandlers();
    wxString resPath = ConfigManager::Get()->Read("data_path", wxEmptyString);
    wxXmlResource::Get()->Load(ConfigManager::Get()->Read("data_path", wxEmptyString) + "/manager_resources.zip");
}

// class destructor
Manager::~Manager()
{
    //delete m_Notebook;
}

wxMDIParentFrame* Manager::GetAppWindow()
{
	return m_pAppWindow;
}

wxNotebook* Manager::GetNotebook()
{
	return m_pNotebook;
}

ProjectManager* Manager::GetProjectManager()
{
	return ProjectManager::Get(m_pNotebook);
}

EditorManager* Manager::GetEditorManager()
{
	return EditorManager::Get(m_pAppWindow);
}

MessageManager* Manager::GetMessageManager()
{
	return MessageManager::Get(m_pAppWindow);
}

PluginManager* Manager::GetPluginManager()
{
	return PluginManager::Get();
}

ToolsManager* Manager::GetToolsManager()
{
	return ToolsManager::Get();
}

MacrosManager* Manager::GetMacrosManager()
{
	return MacrosManager::Get();
}

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
#include <wx/menu.h>
#include <wx/toolbar.h>

#include "manager.h" // class's header file
#include "projectmanager.h"
#include "editormanager.h"
#include "messagemanager.h"
#include "pluginmanager.h"
#include "toolsmanager.h"
#include "macrosmanager.h"
#include "configmanager.h"
#include "templatemanager.h"
#include "managerproxy.h"
#include "xtra_res.h" // our new ToolBarAddOn handler


static bool appShutingDown = false;

Manager* Manager::Get(wxMDIParentFrame* appWindow, wxNotebook* notebook)
{
    if (!ManagerProxy::Get() && appWindow)
	{
        ManagerProxy::Set( new Manager(appWindow, notebook) );
		ManagerProxy::Get()->GetMessageManager()->Log(_("Manager initialized"));
	}
    return ManagerProxy::Get();
}

void Manager::Free()
{
    appShutingDown = true;
	if (ManagerProxy::Get())
	{
		/**
		@bug This is a dumb nasty bug. If ProjectManager is freed after ToolsManager, we get
		a crash. Seems like wxRemoveEventHandler doesn't work correctly (it doesn't NULL the
		prev event handler pointer)
		*/
		/*
            mandrav: This bug seems to be solved after cbPlugin::Release() was
                    changed to cbPlugin::Release(bool appShutDown). Please
                    test under MSVC too...
        */
		MacrosManager::Free();
		ToolsManager::Free();
		TemplateManager::Free();
		ProjectManager::Free();
		EditorManager::Free();
		PluginManager::Free();
		MessageManager::Free();

		delete ManagerProxy::Get();
		ManagerProxy::Set( 0L );
	}
}

bool Manager::isappShutingDown()
{
    return(appShutingDown);
}
// stupid typo ;-P		
bool Manager::isappShuttingDown()
{
    return(appShutingDown);
}

// class constructor
Manager::Manager(wxMDIParentFrame* appWindow, wxNotebook* notebook)
	: m_pAppWindow(appWindow),
	m_pNotebook(notebook)
{
    // Basically, this is the very first place that will be called in the lib
    // (through Manager::Get()), so it's a very good place to load and initialize
    // any resources we 'll be using...
    
    Initxrc(true);
    Loadxrc("/manager_resources.zip#zip:*.xrc");
}

// class destructor
Manager::~Manager()
{
    //delete m_Notebook;
}

void Manager::Initxrc(bool force)
{
    static bool xrcok = false;
    if(!xrcok || force)
    {
        wxFileSystem::AddHandler(new wxZipFSHandler);
        wxXmlResource::Get()->InitAllHandlers();
        wxXmlResource::Get()->InsertHandler(new wxToolBarAddOnXmlHandler);
        
        xrcok=true;
    }
}

void Manager::Loadxrc(wxString relpath)
{
    Manager::Initxrc();
    wxString resPath = ConfigManager::Get()->Read("data_path", wxEmptyString);
    wxXmlResource::Get()->Load(resPath + relpath);
}

wxMenuBar *Manager::LoadMenuBar(wxString resid,bool createonfailure)
{
    wxMenuBar *m = wxXmlResource::Get()->LoadMenuBar(resid);
    if(!m && createonfailure) m=new wxMenuBar();
    return m;
}

wxMenu *Manager::LoadMenu(wxString menu_id,bool createonfailure)
{
    wxMenu *m = wxXmlResource::Get()->LoadMenu(menu_id);
    if(!m && createonfailure) m=new wxMenu("");
    return m;
}

wxToolBar *Manager::LoadToolBar(wxFrame *parent,wxString resid,bool defaultsmall)
{
    if(!parent) 
        return 0L;
    wxToolBar *tb = wxXmlResource::Get()->LoadToolBar(parent,resid);
    if(!tb)
    {
        int flags = wxTB_HORIZONTAL;
        int major;
        int minor;
        // version==wxWINDOWS_NT && major==5 && minor==1 => windowsXP
        bool isXP = wxGetOsVersion(&major, &minor) == wxWINDOWS_NT && major == 5 && minor == 1;
        if (!isXP)
            flags |= wxTB_FLAT;
        tb = parent->CreateToolBar(flags, wxID_ANY);  
        tb->SetToolBitmapSize(defaultsmall ? wxSize(16, 16) : wxSize(22, 22));
    }
        
    return tb;
}

void Manager::AddonToolBar(wxToolBar* toolBar,wxString resid)
{
    if(!toolBar)
        return;
    wxXmlResource::Get()->LoadObject(toolBar,NULL,resid,"wxToolBarAddOn");    
}

bool Manager::isToolBar16x16(wxToolBar* toolBar)
{
    if(!toolBar) return true; // Small by default
    wxSize mysize=toolBar->GetToolBitmapSize();
    return (mysize.GetWidth()<=16 && mysize.GetHeight()<=16);
}

wxMDIParentFrame* Manager::GetAppWindow()
{
	if(!this) return 0; // Fixes early-shutdown segfault
	return m_pAppWindow;
}

wxNotebook* Manager::GetNotebook()
{
	if(!this) return 0; // Fixes early-shutdown segfault
	return m_pNotebook;
}

ProjectManager* Manager::GetProjectManager()
{
	return appShutingDown ? 0 : ProjectManager::Get(m_pNotebook);
}

EditorManager* Manager::GetEditorManager()
{
	return appShutingDown ? 0 : EditorManager::Get(m_pAppWindow);
}

MessageManager* Manager::GetMessageManager()
{
	return appShutingDown ? 0 : MessageManager::Get(m_pAppWindow);
}

PluginManager* Manager::GetPluginManager()
{
	return appShutingDown ? 0 : PluginManager::Get();
}

ToolsManager* Manager::GetToolsManager()
{
	return appShutingDown ? 0 : ToolsManager::Get();
}

MacrosManager* Manager::GetMacrosManager()
{
	return appShutingDown ? 0 : MacrosManager::Get();
}

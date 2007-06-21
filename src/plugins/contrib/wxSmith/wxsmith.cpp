/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "wxsmith.h"
#include "wxsproject.h"
#include "wxsmithmime.h"
#include "wxssettings.h"
#include "wxsextresmanager.h"
#include "wxsresourcefactory.h"
#include "properties/wxsproperties.h"

#include <wx/wxFlatNotebook/wxFlatNotebook.h>
#include <projectloader_hooks.h>
#include <projectmanager.h>

namespace
{
    int ConfigureId = wxNewId();
}

wxSmith* wxSmith::m_Singleton = 0;

BEGIN_EVENT_TABLE(wxSmith, cbPlugin)
    EVT_PROJECT_OPEN(wxSmith::OnProjectOpened)
	EVT_PROJECT_CLOSE(wxSmith::OnProjectClose)
	EVT_PROJECT_RENAMED(wxSmith::OnProjectRenamed)
	EVT_MENU(ConfigureId,wxSmith::OnConfigure)
	EVT_MENU(-1,wxSmith::OnMenu)
END_EVENT_TABLE()

wxSmith::wxSmith()
{
}

wxSmith::~wxSmith()
{
}

void wxSmith::OnAttach()
{
    // No more instances of wxSmith class can be found here,
    // even if it's on another dll/so, m_Singleton will point
    // to different memory locations
    wxASSERT(m_Singleton == 0);

    wxFlatNotebook* Notebook = Manager::Get()->GetProjectManager()->GetNotebook();
    wxASSERT(Notebook!=0);

    // Creating main splitting object
    m_Splitter = new wxsStoringSplitterWindow(Notebook);
    Notebook->AddPage(m_Splitter,_("Resources"));

    wxPanel* ResourcesContainer = new wxPanel(m_Splitter->GetSplitter(),-1,wxDefaultPosition,wxDefaultSize,0);
    wxPanel* PropertiesContainer = new wxPanel(m_Splitter->GetSplitter(),-1,wxDefaultPosition,wxDefaultSize,0);

    // Adding resource browser
    wxSizer* Sizer = new wxGridSizer(1);
    wxTreeCtrl* ResourceBrowser = new wxsResourceTree(ResourcesContainer);
    Sizer->Add(ResourceBrowser,1,wxGROW|wxALL);
    ResourcesContainer->SetSizer(Sizer);

    // Adding properties / events browser
    Sizer = new wxGridSizer(1);
    wxsPropertyGridManager* PGManager = new wxsPropertyGridManager(PropertiesContainer,-1,wxDefaultPosition,wxDefaultSize,wxPG_TOOLBAR|wxTAB_TRAVERSAL);
    PGManager->AddPage(_("Properties"));
    PGManager->AddPage(_("Events"));
    PGManager->SelectPage(0);
    Sizer->Add(PGManager,1,wxGROW);
    PropertiesContainer->SetSizer(Sizer);
    m_Splitter->Split(ResourcesContainer,PropertiesContainer);

    // Registering function for loading / saving extra XML configuration inside CBP files
    ProjectLoaderHooks::HookFunctorBase* wxSmithHook = new ProjectLoaderHooks::HookFunctor<wxSmith>(this, &wxSmith::OnProjectHook);
    m_HookId = ProjectLoaderHooks::RegisterHook(wxSmithHook);

    m_Singleton = this;

    wxsResourceFactory::OnAttachAll();
}

void wxSmith::OnRelease(bool appShutDown)
{
    ProjectLoaderHooks::UnregisterHook(m_HookId,true);
    for ( ProjectMapI i = m_ProjectMap.begin(); i!=m_ProjectMap.end(); ++i )
    {
        if ( i->second )
        {
            delete i->second;
            i->second = 0;
        }
    }
    wxsExtResManager::Get()->DeleteAll();

    m_ProjectMap.clear();

    wxsResourceFactory::OnReleaseAll();

    if ( m_Singleton == this )
    {
        m_Singleton = 0;
    }
}

cbConfigurationPanel* wxSmith::GetConfigurationPanel(wxWindow* parent)
{
	return new wxsSettings(parent);
}

cbConfigurationPanel* wxSmith::GetProjectConfigurationPanel(wxWindow* parent, cbProject* project)
{
    ProjectMapI i = m_ProjectMap.find(project);
    if ( i == m_ProjectMap.end() ) return 0;
    return i->second->GetProjectConfigurationPanel(parent);
}

void wxSmith::BuildMenu(wxMenuBar* menuBar)
{
	wxMenu* SmithMenu = new wxMenu;

	wxsResourceFactory::BuildSmithMenu(SmithMenu);

	SmithMenu->AppendSeparator();
	SmithMenu->Append(ConfigureId,_("&Configure wxSmith for current project"));

	int ToolsPos = menuBar->FindMenu(_("&Tools"));
	if  ( ToolsPos == wxNOT_FOUND )
	{
        menuBar->Append(SmithMenu,_("&wxSmith"));
	}
	else
	{
        menuBar->Insert(ToolsPos,SmithMenu,_("&wxSmith"));
	}
}

void wxSmith::BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data)
{
}

bool wxSmith::BuildToolBar(wxToolBar* toolBar)
{
	return false;
}

void wxSmith::OnProjectHook(cbProject* project,TiXmlElement* elem,bool loading)
{
    wxsProject* Proj = GetSmithProject(project);
    if ( loading ) Proj->ReadConfiguration(elem);
    else           Proj->WriteConfiguration(elem);
}

void wxSmith::OnProjectOpened(CodeBlocksEvent& event)
{
    event.Skip();
    cbProject* Proj = event.GetProject();
    ProjectMapI i = m_ProjectMap.find(Proj);
    if ( i == m_ProjectMap.end() ) return;
    wxsProject* wxsProj = i->second;
    Proj->SetModified(wxsProj->GetWasModifiedDuringLoad());
}

void wxSmith::OnProjectClose(CodeBlocksEvent& event)
{
    event.Skip();
    cbProject* Proj = event.GetProject();
    ProjectMapI i = m_ProjectMap.find(Proj);
    if ( i == m_ProjectMap.end() ) return;
    delete i->second;
    m_ProjectMap.erase(i);
}

void wxSmith::OnProjectRenamed(CodeBlocksEvent& event)
{
    event.Skip();
    cbProject* Proj = event.GetProject();
    ProjectMapI i = m_ProjectMap.find(Proj);
    if ( i == m_ProjectMap.end() ) return;
    i->second->UpdateName();
}

void wxSmith::OnConfigure(wxCommandEvent& event)
{
    cbProject* Proj = Manager::Get()->GetProjectManager()->GetActiveProject();
    if ( Proj )
    {
        GetSmithProject(Proj)->Configure();
    }
}

void wxSmith::OnMenu(wxCommandEvent& event)
{
    cbProject* Proj = Manager::Get()->GetProjectManager()->GetActiveProject();
    if ( Proj )
    {
        if ( wxsResourceFactory::NewResourceMenu(event.GetId(),GetSmithProject(Proj)) )
        {
            return;
        }
    }
    event.Skip();
}

cbProject* wxSmith::GetCBProject(wxsProject* Proj)
{
    return Proj->GetCBProject();
}

wxsProject* wxSmith::GetSmithProject(cbProject* Proj)
{
    ProjectMapI i = m_ProjectMap.find(Proj);
    if ( i == m_ProjectMap.end() )
    {
        wxsProject* NewProj = new wxsProject(Proj);
        m_ProjectMap[Proj] = NewProj;
        return NewProj;
    }
    return i->second;
}

// TODO: Move to resources\wxwidgets
/*

void wxSmith::OnImportXrc(wxCommandEvent& event)
{
    if ( !CheckIntegration() ) return;

	wxString FileName = ::wxFileSelector(
        _("Select XRC file"),
        _T(""),
        _T(""),
        _T("xrc"),
        _("XRC files (*.xrc)|*.xrc|All files (*)|*"),
        wxOPEN|wxFILE_MUST_EXIST|wxHIDE_READONLY);

    if ( FileName.empty() ) return;

    // Loading xrc file into xml document

    TiXmlDocument Doc(cbU2C(FileName));
    TiXmlElement* Resource;
    if ( !Doc.LoadFile() || !(Resource = Doc.FirstChildElement("resource")) )
    {
    	wxMessageBox(_("Couldn't load XRC file."));
        return;
    }

    // Generating list of objects
    wxArrayString Resources;
    TiXmlElement* Element = Resource->FirstChildElement("object");
    while ( Element )
    {
    	const char* Class = Element->Attribute("class");
    	const char* Name = Element->Attribute("name");
    	if ( !Class || !Name ) continue;

    	if ( !strcmp(Class,"wxDialog") ||
    	     !strcmp(Class,"wxPanel") ||
    	     !strcmp(Class,"wxFrame") )
        {
        	Resources.Add(cbC2U(Name));
        }

        Element = Element->NextSiblingElement("object");
    }

    if ( Resources.Count() == 0 )
    {
    	wxMessageBox(_("Didn't find any editable resources"));
    	return;
    }

    // Selecting resource to edit
    wxString Name;

    if ( Resources.Count() == 1 )
    {
    	Name = Resources[0];
    }
    else
    {
        int Index = ::wxGetSingleChoiceIndex(
            _("Select resource to import"),
            _("Importing XRC"),
            Resources );
        if ( Index == -1 ) return;
        Name = Resources[Index];
    }

    Element = Resource->FirstChildElement("object");
    while ( Element )
    {
        if ( !strcmp(Element->Attribute("name"),cbU2C(Name)) )
        {
            break;
        }
        Element = Element->NextSiblingElement("object");
    }
    if ( !Element ) return;

    // Creating fake resource and testing if xrc can be loaded without any errors
    wxsItem* Test = wxsGEN(cbC2U(Element->Attribute("class")),0);
    if ( !Test )
    {
    	// Something went wrong - default factory is not working ?
    	DBGLOG(_T("wxSmith: Internal error - did not found one of base items when importing XRC"));
    	return;
    }

    if ( !Test->XmlRead(Element,true,false) )
    {
		if ( wxMessageBox(_("Resource was not loaded properly. Some widgets may be\n"
                            "damaged or will be removed. Continue ?"),
                          _("XRC Load error"),
                          wxYES_NO|wxICON_QUESTION) == wxNO )
        {
        	delete Test;
        	return;
        }
    }
    delete Test;

    // Displaying configuration dialog - it will handle adding resource to project
    wxsImportXrcDlg Dlg(0,Element);
    Dlg.ShowModal();
}
*/

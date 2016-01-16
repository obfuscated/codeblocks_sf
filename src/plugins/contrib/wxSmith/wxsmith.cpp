/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
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

#include <prep.h>
#include <cbauibook.h>
#include <projectloader_hooks.h>
#include <manager.h>
#include <configmanager.h>
#include <projectmanager.h>
#include <logmanager.h>
#include <sqplus.h>
#include <sc_base_types.h>

namespace
{
    const int ConfigureId = wxNewId();
    const int ViewWxSmithId = wxNewId();
    const int ViewWxSmithResourceId = wxNewId();
    const int ViewWxSmithPropertyId = wxNewId();

    /* XPM */
    static const char * Events_xpm[] = {
    "16 16 2 1",
    "     c None",
    ".    c #000000",
    "                ",
    "    ..    ..    ",
    "   .        .   ",
    "   .        .   ",
    "   .        .   ",
    "   .        .   ",
    "   .        .   ",
    " ..          .. ",
    "   .        .   ",
    "   .        .   ",
    "   .        .   ",
    "   .        .   ",
    "   .        .   ",
    "    ..    ..    ",
    "                ",
    "                "};

    const int placementManagementPane = 0;
    const int placementOnePane = 1;
    const int placementTwoPanes = 2;

    inline int GetBrowserPlacements() { return Manager::Get()->GetConfigManager(_T("wxsmith"))->ReadInt(_T("/browserplacements"),0); }
}

wxSmith* wxSmith::m_Singleton = 0;

BEGIN_EVENT_TABLE(wxSmith, cbPlugin)
    EVT_UPDATE_UI(ViewWxSmithId,wxSmith::OnUpdateUI)
    EVT_UPDATE_UI(ViewWxSmithResourceId,wxSmith::OnUpdateUI)
    EVT_UPDATE_UI(ViewWxSmithPropertyId,wxSmith::OnUpdateUI)
    EVT_MENU(ConfigureId,wxSmith::OnConfigure)
    EVT_MENU(ViewWxSmithId,wxSmith::OnViewBrowsers)
    EVT_MENU(ViewWxSmithResourceId,wxSmith::OnViewResourceBrowser)
    EVT_MENU(ViewWxSmithPropertyId,wxSmith::OnViewPropertyBrowser)
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

    // Creating properties and resource browser
    BuildBrowsers();

    // Registering function for loading / saving extra XML configuration inside CBP files
    ProjectLoaderHooks::HookFunctorBase* wxSmithHook = new ProjectLoaderHooks::HookFunctor<wxSmith>(this, &wxSmith::OnProjectHook);
    m_HookId = ProjectLoaderHooks::RegisterHook(wxSmithHook);

    m_Singleton = this;

    wxsResourceFactory::OnAttachAll();

    // register event sink
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_OPEN, new cbEventFunctor<wxSmith, CodeBlocksEvent>(this, &wxSmith::OnProjectOpened));
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_CLOSE, new cbEventFunctor<wxSmith, CodeBlocksEvent>(this, &wxSmith::OnProjectClose));
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_RENAMED, new cbEventFunctor<wxSmith, CodeBlocksEvent>(this, &wxSmith::OnProjectRenamed));

    // register scripting stuff
    RegisterScripting();
}

void wxSmith::BuildBrowserParents()
{
    // Zero pointers to minimize segfault possibility ;)
    m_Splitter = 0;
    m_ResourceBrowserParent = 0;
    m_PropertyBrowserParent = 0;

    switch ( GetBrowserPlacements() )
    {
        case placementOnePane:
        {
            m_Splitter = new wxsStoringSplitterWindow(Manager::Get()->GetAppWindow());
            m_ResourceBrowserParent = new wxPanel(m_Splitter->GetSplitter(),-1,wxDefaultPosition,wxDefaultSize,0);
            m_PropertyBrowserParent = new wxPanel(m_Splitter->GetSplitter(),-1,wxDefaultPosition,wxDefaultSize,0);
            m_Splitter->Split(m_ResourceBrowserParent,m_PropertyBrowserParent);
            m_Splitter->SetSize(150,450);

            CodeBlocksDockEvent evt(cbEVT_ADD_DOCK_WINDOW);
            evt.name = _T("wxSmithOnePane");
            evt.title = _("wxSmith");
            evt.pWindow = m_Splitter;
            evt.dockSide = CodeBlocksDockEvent::dsFloating;
            evt.desiredSize.Set(150, 450);
            evt.floatingSize.Set(150, 450);
            evt.minimumSize.Set(50, 50);
            Manager::Get()->ProcessEvent(evt);
            break;
        }

        case placementTwoPanes:
        {
            m_ResourceBrowserParent = new wxPanel(Manager::Get()->GetAppWindow(),-1,wxDefaultPosition,wxDefaultSize,0);
            m_PropertyBrowserParent = new wxPanel(Manager::Get()->GetAppWindow(),-1,wxDefaultPosition,wxDefaultSize,0);

            CodeBlocksDockEvent evt(cbEVT_ADD_DOCK_WINDOW);
            evt.name = _T("wxSmithTwoPanes_ResourceBrowser");
            evt.title = _("wxSmith - Resource Browser");
            evt.pWindow = m_ResourceBrowserParent;
            evt.dockSide = CodeBlocksDockEvent::dsFloating;
            evt.desiredSize.Set(150, 450);
            evt.floatingSize.Set(150, 450);
            evt.minimumSize.Set(50, 50);
            Manager::Get()->ProcessEvent(evt);

            evt.name = _T("wxSmithTwoPanes_PropertyBrowser");
            evt.title = _("wxSmith - Property Browser");
            evt.pWindow = m_PropertyBrowserParent;
            Manager::Get()->ProcessEvent(evt);
            break;
        }

        default:
        {
            cbAuiNotebook* Notebook = Manager::Get()->GetProjectManager()->GetUI().GetNotebook();
            wxASSERT(Notebook!=0);

            // Creating main splitting object
            m_Splitter = new wxsStoringSplitterWindow(Notebook);
            Notebook->AddPage(m_Splitter,_("Resources"));

            m_ResourceBrowserParent = new wxPanel(m_Splitter->GetSplitter(),-1,wxDefaultPosition,wxDefaultSize,0);
            m_PropertyBrowserParent = new wxPanel(m_Splitter->GetSplitter(),-1,wxDefaultPosition,wxDefaultSize,0);
            m_Splitter->Split(m_ResourceBrowserParent,m_PropertyBrowserParent);
        }
    }
}


void wxSmith::BuildBrowsers()
{
    // Building background windows for panes
    BuildBrowserParents();

    // Adding resource browser
    wxSizer* Sizer = new wxGridSizer(1);
    wxTreeCtrl* ResourceBrowser = new wxsResourceTree(m_ResourceBrowserParent);
    Sizer->Add(ResourceBrowser,1,wxGROW|wxALL);
    m_ResourceBrowserParent->SetSizer(Sizer);

    // Adding properties / events browser
    Sizer = new wxGridSizer(1);
    wxsPropertyGridManager* PGManager = new wxsPropertyGridManager(m_PropertyBrowserParent,-1,wxDefaultPosition,wxDefaultSize,wxPG_TOOLBAR|wxTAB_TRAVERSAL|wxPG_SPLITTER_AUTO_CENTER);
    PGManager->AddPage(_("Properties"));
    PGManager->AddPage(_("Events"),wxBitmap(Events_xpm));
    PGManager->SelectPage(0);
    Sizer->Add(PGManager,1,wxGROW);
    m_PropertyBrowserParent->SetSizer(Sizer);
}

void wxSmith::OnRelease(cb_unused bool appShutDown)
{
    UnregisterScripting();

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
    // Generating separate wxSmith menu entry
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

    // Generate entries in "view" menu
    int idx = menuBar->FindMenu(_("&View"));
    if (idx != wxNOT_FOUND)
    {
        wxMenu* view = menuBar->GetMenu(idx);
        wxMenuItemList& items = view->GetMenuItems();
        // find the first separator and insert before it
        for (size_t i = 0; i < items.GetCount(); ++i)
        {
            if (items[i]->IsSeparator())
            {
                switch ( GetBrowserPlacements() )
                {
                    case placementOnePane:
                        view->InsertCheckItem(i,ViewWxSmithId, _("wxSmith browsers"), _("Toggle displaying the wxSmith resource and property browsers"));
                        break;

                    case placementTwoPanes:
                        view->InsertCheckItem(i,ViewWxSmithResourceId, _("wxSmith resource browser"), _("Toggle displaying the wxSmith resource browser"));
                        view->InsertCheckItem(i,ViewWxSmithPropertyId, _("wxSmith property browser"), _("Toggle displaying the wxSmith property browser"));
                        break;

                    default:
                        break;
                }
                return;
            }
        }

        // not found, just append
        switch ( GetBrowserPlacements() )
        {
            case placementOnePane:
                view->AppendCheckItem(ViewWxSmithId, _("wxSmith browsers"), _("Toggle displaying the wxSmith resource and property browsers"));
                break;

            case placementTwoPanes:
                view->AppendCheckItem(ViewWxSmithResourceId, _("wxSmith resource browser"), _("Toggle displaying the wxSmith resource browser"));
                view->AppendCheckItem(ViewWxSmithPropertyId, _("wxSmith property browser"), _("Toggle displaying the wxSmith property browser"));
                break;

            default:
                break;
        }
        return;
    }
}

void wxSmith::BuildModuleMenu(cb_unused const ModuleType type, cb_unused wxMenu* menu, const FileTreeData* /*data*/)
{
}

bool wxSmith::BuildToolBar(cb_unused wxToolBar* toolBar)
{
    return false;
}

void wxSmith::OnProjectHook(cbProject* project,TiXmlElement* elem,bool loading)
{
    assert(project);
    wxsProject* Proj = GetSmithProject(project);
    assert(Proj);
    if ( loading ) Proj->ReadConfiguration(elem);
    else           Proj->WriteConfiguration(elem);
}

void wxSmith::OnProjectOpened(CodeBlocksEvent& event)
{
    cbProject* Proj = event.GetProject();
    wxsProject* wxsProj = GetSmithProject(Proj);
    wxsProj->UpdateName();
    Proj->SetModified(wxsProj->GetWasModifiedDuringLoad());
    event.Skip();
}

void wxSmith::OnProjectClose(CodeBlocksEvent& event)
{
    cbProject* Proj = event.GetProject();
    ProjectMapI i = m_ProjectMap.find(Proj);
    if ( i == m_ProjectMap.end() ) return;
    delete i->second;
    m_ProjectMap.erase(i);
    event.Skip();
}

void wxSmith::OnProjectRenamed(cb_unused CodeBlocksEvent& event)
{
    cbProject* Proj = event.GetProject();
    ProjectMapI i = m_ProjectMap.find(Proj);
    if ( i == m_ProjectMap.end() ) return;
    i->second->UpdateName();
    event.Skip();
}

void wxSmith::OnConfigure(cb_unused wxCommandEvent& event)
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

void wxSmith::OnViewBrowsers(wxCommandEvent& event)
{
    CodeBlocksDockEvent evt(event.IsChecked() ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
    evt.pWindow = m_Splitter;
    Manager::Get()->ProcessEvent(evt);
}

void wxSmith::OnViewPropertyBrowser(wxCommandEvent& event)
{
    CodeBlocksDockEvent evt(event.IsChecked() ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
    evt.pWindow = m_PropertyBrowserParent;
    Manager::Get()->ProcessEvent(evt);
}

void wxSmith::OnViewResourceBrowser(cb_unused wxCommandEvent& event)
{
    CodeBlocksDockEvent evt(event.IsChecked() ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
    evt.pWindow = m_ResourceBrowserParent;
    Manager::Get()->ProcessEvent(evt);
}

void wxSmith::OnUpdateUI(cb_unused wxUpdateUIEvent& event)
{
    wxMenuBar* Bar = Manager::Get()->GetAppFrame()->GetMenuBar();

    Bar->Check(ViewWxSmithId,IsWindowReallyShown(m_Splitter));
    Bar->Check(ViewWxSmithResourceId,IsWindowReallyShown(m_ResourceBrowserParent));
    Bar->Check(ViewWxSmithPropertyId,IsWindowReallyShown(m_PropertyBrowserParent));
}

cbProject* wxSmith::GetCBProject(wxsProject* Proj)
{
    return Proj->GetCBProject();
}

wxsProject* wxSmith::GetSmithProject(cbProject* Proj)
{
    assert(Proj);
    ProjectMapI i = m_ProjectMap.find(Proj);
    if ( i == m_ProjectMap.end() )
    {
        wxsProject* NewProj = new wxsProject(Proj);
        m_ProjectMap[Proj] = NewProj;
        return NewProj;
    }
    return i->second;
}

void wxSmith::ShowResourcesTab()
{
    cbAuiNotebook* Notebook = Manager::Get()->GetProjectManager()->GetUI().GetNotebook();
    Notebook->SetSelection( Notebook->GetPageIndex(m_Splitter) );
}

void wxSmith::RegisterScripting()
{
    Manager::Get()->GetScriptingManager();
    if ( SquirrelVM::GetVMPtr() )
    {
        SqPlus::RegisterGlobal( &wxSmith::RecoverWxsFile, "WxsRecoverWxsFile" );
    }
}

void wxSmith::UnregisterScripting()
{
    Manager::Get()->GetScriptingManager();
    HSQUIRRELVM v = SquirrelVM::GetVMPtr();
    if ( v )
    {
        sq_pushroottable(v);
        sq_pushstring(v,"WxsRecoverWxsFile",-1);
        sq_deleteslot(v,-2,false);
        sq_poptop(v);
    }
}

bool wxSmith::RecoverWxsFile( const wxString& WxsResourceSettings )
{
    wxSmith* This = wxSmith::Get();
    if ( !This ) return false;
    if ( !ProjectManager::Get()->GetActiveProject() ) return false;

    wxsProject* project = This->GetSmithProject( ProjectManager::Get()->GetActiveProject() );
    if ( !project ) return false;

    return project->RecoverWxsFile( WxsResourceSettings );
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
        Manager::Get()->GetLogManager()->DebugLog(F(_T("wxSmith: Internal error - did not found one of base items when importing XRC")));
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

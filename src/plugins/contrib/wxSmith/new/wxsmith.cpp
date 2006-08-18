/***************************************************************
 * Name:      wxsmith.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    BYO<byo.spoon@gmail.com>
 * Created:   04/10/05 01:05:08
 * Copyright: (c) BYO
 * License:   GPL
 **************************************************************/

#include <licenses.h>
#include <manager.h>
#include <tinyxml/tinyxml.h>
#include <messagemanager.h>
#include <cbeditor.h>
#include <projectmanager.h>
#include <wxFlatNotebook/wxFlatNotebook.h>
#include <wx/sashwin.h>
#include <configmanager.h>
#include <cbexception.h>

#include "wxsmith.h"
#include "wxsproject.h"
#include "wxsresource.h"
#include "wxsitem.h"
#include "wxssettingsdlg.h"
#include "wxsmithmime.h"
#include "wxsmithwizard.h"
#include "wxsnewwindowdlg.h"
#include "wxsimportxrcdlg.h"
#include "wxsitemfactory.h"
#include "wxspropertybrowser.h"
#include "defwidgets/wxsstdmanager.h"
#include "editors/wxswindoweditor.h"

static int NewDialogId = wxNewId();
static int NewFrameId  = wxNewId();
static int NewPanelId  = wxNewId();
static int ImportXrcId = wxNewId();
static int ConfigureId = wxNewId();

CB_IMPLEMENT_PLUGINS_3( wxSmith, "wxSmith",
                        wxSmithMime, "wxSmith - MIME plugin",
                        wxSmithWizard, "wxSmith - Project Wizard plugin");

wxSmith* wxSmith::Singleton = NULL;

BEGIN_EVENT_TABLE(wxSmith, cbPlugin)
	EVT_PROJECT_CLOSE(wxSmith::OnProjectClose)
	EVT_PROJECT_OPEN(wxSmith::OnProjectOpen)
	EVT_PROJECT_ACTIVATE(wxSmith::OnProjectActivated)
	EVT_MENU(NewDialogId,wxSmith::OnNewWindow)
	EVT_MENU(NewFrameId,wxSmith::OnNewWindow)
	EVT_MENU(NewPanelId,wxSmith::OnNewWindow)
	EVT_MENU(ImportXrcId,wxSmith::OnImportXrc)
	EVT_MENU(ConfigureId,wxSmith::OnConfigure)
END_EVENT_TABLE()

wxSmith::wxSmith()
{
	m_PluginInfo.name = _("wxSmith");
	m_PluginInfo.title = _("wxSmith");
	m_PluginInfo.version = _("1.0");
	m_PluginInfo.description = _("RAD tool used to create wxWidgets forms");
	m_PluginInfo.author = _("BYO");
	m_PluginInfo.authorEmail = _("byo.spoon@gmail.com");
	m_PluginInfo.authorWebsite = _T("");
	m_PluginInfo.thanksTo =
        _("Ann for Being\n"
          "Anha for Smile\n"
          "Gigi for Faworki\n"
          "Jaakko Salli for wxPropertyGrid"
          "\n"
          "God for Love\n");
	m_PluginInfo.license = LICENSE_GPL;
}

wxSmith::~wxSmith()
{
	if ( Singleton == this )
	{
	    Singleton = NULL;
	}
}

void wxSmith::OnAttach()
{
    if ( Singleton != NULL )
    {
        // Can not have more than one wxSmith instance
        return;
    }

    wxFlatNotebook* Notebook = Manager::Get()->GetProjectManager()->GetNotebook();
	if ( !Notebook )
	{
        Splitter = NULL;
        return;
	}

    // Creating main splitting object
    Splitter = new wxsStoringSplitterWindow(Notebook);
    Notebook->AddPage(Splitter,_("Resources"));

    wxPanel* ResourcesContainer = new wxPanel(Splitter->GetSplitter(),-1,wxDefaultPosition,wxDefaultSize,0);
    wxPanel* PropertiesContainer = new wxPanel(Splitter->GetSplitter(),-1,wxDefaultPosition,wxDefaultSize,0);

    // Adding resource browser
    wxSizer* Sizer = new wxGridSizer(1);
    wxTreeCtrl* ResourceBrowser = new wxsResourceTree(ResourcesContainer);
    ResourceBrowser->Expand(ResourceBrowser->AddRoot(_("Resources")));
    Sizer->Add(ResourceBrowser,1,wxGROW|wxALL);
    ResourcesContainer->SetSizer(Sizer);

    // Adding properties / events browser
    Sizer = new wxGridSizer(1);
    Sizer->Add(new wxsPropertyBrowser(PropertiesContainer),1,wxGROW);
    PropertiesContainer->SetSizer(Sizer);
    Splitter->Split(ResourcesContainer,PropertiesContainer);

    Manager::Get()->Loadxrc(_T("/wxsmith.zip#zip:*"));

    // Initializing standard manager

    wxsStdManager.Initialize();
    if ( ! wxsStdManager.RegisterInFactory() )
    {
        DBGLOG(_T("wxSmith: Couldn't register standard widget's set - this plugin will be useless"));
    }

    // TODO (SpOoN#1#): Add other widgets from other managers

	if ( Singleton == NULL )
	{
	    Singleton = this;
	}
}

void wxSmith::OnRelease(bool appShutDown)
{
    // Deleting all projects
    for ( ProjectMapI i = ProjectMap.begin(); i!=ProjectMap.end(); ++i )
    {
        if ( i->second )
        {
            delete i->second;
            i->second = NULL;
        }
    }
    ProjectMap.clear();

    if ( Singleton == this )
    {
        Singleton = NULL;
    }
}

cbConfigurationPanel* wxSmith::GetConfigurationPanel(wxWindow* parent)
{
	wxsSettingsDlg* Dlg = new wxsSettingsDlg(parent);
	return Dlg;
}

void wxSmith::BuildMenu(wxMenuBar* menuBar)
{
	wxMenu* Menu = new wxMenu;
	Menu->Append(NewDialogId,_("Add &Dialog"));
	Menu->Append(NewFrameId,_("Add &Frame"));
	Menu->Append(NewPanelId,_("Add &Panel"));
	Menu->AppendSeparator();
	Menu->Append(ImportXrcId,_("Import &XRC file"));
	Menu->AppendSeparator();
	Menu->Append(ConfigureId,_("&Configure wxSmith for current project"));

	int ToolsPos = menuBar->FindMenu(_("&Tools"));

	if  ( ToolsPos == wxNOT_FOUND )
	{
        menuBar->Append(Menu,_("wxSmith"));
	}
	else
	{
        menuBar->Insert(ToolsPos,Menu,_("wxSmith"));
	}
}

void wxSmith::BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data)
{
}

bool wxSmith::BuildToolBar(wxToolBar* toolBar)
{
	return false;
}

void wxSmith::OnProjectClose(CodeBlocksEvent& event)
{
    cbProject* Proj = event.GetProject();
    ProjectMapI i = ProjectMap.find(Proj);
    if ( i == ProjectMap.end() ) return;
    delete i->second;
    ProjectMap.erase(i);
    event.Skip();
}

void wxSmith::OnProjectOpen(CodeBlocksEvent& event)
{
    wxsProject* NewProj = new wxsProject(event.GetProject());
    ProjectMap[event.GetProject()] = NewProj;
    event.Skip();
}

void wxSmith::OnProjectActivated(CodeBlocksEvent& event)
{
    event.Skip();
}

void wxSmith::SelectResource(wxsResource* Res)
{
    if ( Res )
    {
        Res->EditOpen();
    }
}

cbProject* wxSmith::GetCBProject(wxsProject* Proj)
{
    return Proj->GetCBProject();
}

wxsProject* wxSmith::GetSmithProject(cbProject* Proj)
{
    ProjectMapI i = ProjectMap.find(Proj);
    if ( i == ProjectMap.end() ) return NULL;
    return i->second;
}

void wxSmith::OnNewWindow(wxCommandEvent& event)
{
    if ( !CheckIntegration() )
    {
        return;
    }

    wxString ResType = _T("Dialog");
    if ( event.GetId() == NewDialogId ) ResType = _T("Dialog"); else
    if ( event.GetId() == NewFrameId  ) ResType = _T("Frame"); else
    if ( event.GetId() == NewPanelId  ) ResType = _T("Panel");
    else
    {
    	wxMessageBox(_T("Internal error - invalid resource type"));
    	return;
    }

    wxsNewWindowDlg Dlg(Manager::Get()->GetAppWindow(),ResType);
    Dlg.ShowModal();
}

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
    wxsItem* Test = wxsGEN(cbC2U(Element->Attribute("class")),NULL);
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
    wxsImportXrcDlg Dlg(NULL,Element);
    Dlg.ShowModal();
}

bool wxSmith::CheckIntegration()
{
    cbProject* Project = Manager::Get()->GetProjectManager()->GetActiveProject();

    if ( !Project )
    {
        wxMessageBox(_("Please open project first"),_("Error"),wxOK|wxICON_ERROR);
        return false;
    }

    wxsProject* Proj = GetSmithProject(Project);

    if ( !Proj )
    {
        DBGLOG(_T("Something wrong - couldn't find associated wxsProject"));
        return false;
    }

    switch ( Proj->GetIntegration() )
    {
        case wxsProject::NotBinded:
            return false;

        case wxsProject::NotWxsProject:
            if ( wxMessageBox(_("Active project doesn't use wxSmith.\nShould I change it ?"),
                              _("Not wxSmith project"),wxYES_NO|wxICON_EXCLAMATION ) == wxYES )
            {
                if ( !Proj->AddSmithConfig() ) return false;
            }
            else
            {
                return false;
            }
            break;

        default:
            break;
    }

    return true;
}

void wxSmith::OnConfigure(wxCommandEvent& event)
{
    cbProject* CP = Manager::Get()->GetProjectManager()->GetActiveProject();
    if ( !CP ) return;
    wxsProject* SP = GetSmithProject(CP);
    if ( !SP ) return;
    SP->Configure();
}

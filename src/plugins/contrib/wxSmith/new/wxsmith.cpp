/***************************************************************
 * Name:      wxsmith.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    BYO<byo.spoon@gmail.com>
 * Created:   04/10/05 01:05:08
 * Copyright: (c) BYO
 * License:   GPL
 **************************************************************/

#include "wxsmith.h"
#include "wxsproject.h"
#include "wxsmithmime.h"
#include "wxssettingsdlg.h"
#include "wxspropertybrowser.h"
#include "wxsresourcefactory.h"

#include <licenses.h>
#include <wxFlatNotebook/wxFlatNotebook.h>
#include <projectloader_hooks.h>

CB_IMPLEMENT_PLUGINS_2( wxSmith,     "wxSmith",
                        wxSmithMime, "wxSmith - MIME extension");

static int ConfigureId = wxNewId();

wxSmith* wxSmith::m_Singleton = NULL;

BEGIN_EVENT_TABLE(wxSmith, cbPlugin)
	EVT_PROJECT_CLOSE(wxSmith::OnProjectClose)
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
    if ( m_Singleton == this )
    {
        m_Singleton = NULL;
    }
}

void wxSmith::OnAttach()
{
    // No more instances of wxSmith class can be found here,
    // even if it's on another dll/so, m_Singleton will point
    // to different memory locations
    wxASSERT(m_Singleton == NULL);

    wxFlatNotebook* Notebook = Manager::Get()->GetProjectManager()->GetNotebook();
    wxASSERT(Notebook!=NULL);

    // Creating main splitting object
    m_Splitter = new wxsStoringSplitterWindow(Notebook);
    Notebook->AddPage(m_Splitter,_("Resources"));

    wxPanel* ResourcesContainer = new wxPanel(m_Splitter->GetSplitter(),-1,wxDefaultPosition,wxDefaultSize,0);
    wxPanel* PropertiesContainer = new wxPanel(m_Splitter->GetSplitter(),-1,wxDefaultPosition,wxDefaultSize,0);

    // Adding resource browser
    wxSizer* Sizer = new wxGridSizer(1);
    wxTreeCtrl* ResourceBrowser = new wxsResourceTree(ResourcesContainer);
    ResourceBrowser->Expand(ResourceBrowser->AddRoot(_("Resources")));
    Sizer->Add(ResourceBrowser,1,wxGROW|wxALL);
    ResourcesContainer->SetSizer(Sizer);

    // Adding properties / events browser
    Sizer = new wxGridSizer(1);
    // TODO: Uncomment when wxPropertyBrowser will be usable
//    Sizer->Add(new wxsPropertyBrowser(PropertiesContainer),1,wxGROW);
    PropertiesContainer->SetSizer(Sizer);
    m_Splitter->Split(ResourcesContainer,PropertiesContainer);

    // Loading resources
    Manager::Get()->Loadxrc(_T("/wxsmith.zip#zip:*"));

    // Registering function for loading / saving extra XML configuration inside CBP files
    ProjectLoaderHooks::HookFunctorBase* wxSmithHook = new ProjectLoaderHooks::HookFunctor<wxSmith>(this, &wxSmith::OnProjectHook);
    m_HookId = ProjectLoaderHooks::RegisterHook(wxSmithHook);

    m_Singleton = this;
}

void wxSmith::OnRelease(bool appShutDown)
{
    ProjectLoaderHooks::UnregisterHook(m_HookId,true);
    for ( ProjectMapI i = m_ProjectMap.begin(); i!=m_ProjectMap.end(); ++i )
    {
        if ( i->second )
        {
            delete i->second;
            i->second = NULL;
        }
    }

    m_ProjectMap.clear();
}

cbConfigurationPanel* wxSmith::GetConfigurationPanel(wxWindow* parent)
{
    return NULL;
//	return new wxsSettingsDlg(parent);
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
    wxsResourceFactory::BuildModuleMenu(type,menu,data);
}

bool wxSmith::BuildToolBar(wxToolBar* toolBar)
{
    wxsResourceFactory::BuildToolBar(toolBar);
	return false;
}

void wxSmith::OnProjectHook(cbProject* project,TiXmlElement* elem,bool loading)
{
    wxsProject* Proj = GetSmithProject(project);
    if ( loading ) Proj->ReadConfiguration(elem);
    else           Proj->WriteConfiguration(elem);
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

void wxSmith::OnConfigure(wxCommandEvent& event)
{
    cbProject* Proj = Manager::Get()->GetProjectManager()->GetActiveProject();
    if ( Proj )
    {
        GetSmithProject(Proj)->Configure();
    }
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
*/

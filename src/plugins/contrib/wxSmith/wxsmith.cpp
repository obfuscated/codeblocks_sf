/***************************************************************
 * Name:      wxsmith.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    BYO<byo.spoon@gmail.com>
 * Created:   04/10/05 01:05:08
 * Copyright: (c) BYO
 * License:   GPL
 **************************************************************/

#include "wxsheaders.h"
#include <manager.h>
#include <tinyxml/tinyxml.h>
#include <messagemanager.h>
#include <cbeditor.h>
#include <projectmanager.h>
#include <wx/wxFlatNotebook/wxFlatNotebook.h>
#include <wx/sashwin.h>
#include <configmanager.h>
#include <cbexception.h>
#include <projectloader_hooks.h>

#include "wxsmith.h"
#include "wxswindoweditor.h"
#include "defwidgets/wxsstdmanager.h"
#include "wxscodegen.h"
#include "wxspropertiesman.h"
#include "wxsproject.h"
#include "wxswidgetfactory.h"
#include "wxsevent.h"
#include "wxsnewwindowdlg.h"
#include "wxsimportxrcdlg.h"
#include "wxsresourcetree.h"
#include "wxssettingsdlg.h"
#include "wxsmithmime.h"
#include "wxsmithwizard.h"
#include "wxscoder.h"
#include "wxsextresmanager.h"

static int NewDialogId = wxNewId();
static int NewFrameId = wxNewId();
static int NewPanelId = wxNewId();
static int ImportXrcId = wxNewId();
static int ConfigureId = wxNewId();

// Register the 3 plugins contained herein
namespace
{
    PluginRegistrant<wxSmith> reg1(_T("wxSmith"));
    PluginRegistrant<wxSmithMime> reg2(_T("wxSmithMime"));
    PluginRegistrant<wxSmithWizard> reg3(_T("wxSmithWizard"));
};

wxSmith* wxSmith::Singleton = NULL;

BEGIN_EVENT_TABLE(wxSmith, cbPlugin)
	EVT_PROJECT_CLOSE(wxSmith::OnProjectClose)
	EVT_PROJECT_OPEN(wxSmith::OnProjectOpen)
	EVT_PROJECT_ACTIVATE(wxSmith::OnProjectActivated)
	EVT_SELECT_RES(wxSmith::OnSelectRes)
	EVT_UNSELECT_RES(wxSmith::OnSpreadEvent)
	EVT_SELECT_WIDGET(wxSmith::OnSpreadEvent)
	EVT_UNSELECT_WIDGET(wxSmith::OnSpreadEvent)
	EVT_MENU(NewDialogId,wxSmith::OnNewWindow)
	EVT_MENU(NewFrameId,wxSmith::OnNewWindow)
	EVT_MENU(NewPanelId,wxSmith::OnNewWindow)
	EVT_MENU(ImportXrcId,wxSmith::OnImportXrc)
	EVT_MENU(ConfigureId,wxSmith::OnConfigure)
END_EVENT_TABLE()

wxSmith::wxSmith()
{
}

wxSmith::~wxSmith()
{
}

void wxSmith::OnAttach()
{
	if ( Singleton == NULL ) Singleton = this;

    if(!Manager::LoadResource(_T("wxsmith.zip")))
    {
        NotifyMissingFile(_T("wxsmith.zip"));
    }

    Coder = new wxsCoder();
    ExtResManager = new wxsExtResManager();
    PropertiesManager = new wxsPropertiesMan();
    WidgetFactory = new wxsWidgetFactory();

    wxsStdManager.Initialize();
    wxsStdManager.RegisterInFactory();

    wxFlatNotebook* Notebook = Manager::Get()->GetProjectManager()->GetNotebook();
	if ( Notebook )
	{
        // Creating main splitting objects

        LeftSplitter = new wxsSplitterWindowEx(Notebook);
        Notebook->AddPage(LeftSplitter,_("Resources"));

        wxPanel* ResourcesContainer = new wxPanel(LeftSplitter->GetSplitter(),-1,wxDefaultPosition,wxDefaultSize,0);
        wxPanel* PropertiesContainer = new wxPanel(LeftSplitter->GetSplitter(),-1,wxDefaultPosition,wxDefaultSize,0);

        // Adding resource browser

        wxSizer* Sizer = new wxGridSizer(1);
        ResourceBrowser = new wxsResourceTree(ResourcesContainer);
        ResourceBrowser->Expand(ResourceBrowser->AddRoot(_("Resources")));
        Sizer->Add(ResourceBrowser,1,wxGROW|wxALL);
        ResourcesContainer->SetSizer(Sizer);

        // Adding new page into Manager
        Sizer = new wxGridSizer(1);
        wxNotebook* LDNotebook = new wxNotebook(PropertiesContainer,-1,wxDefaultPosition,wxDefaultSize,wxSUNKEN_BORDER);
        PropertiesPanel = new wxScrolledWindow(LDNotebook);
        PropertiesPanel->SetScrollRate(5,5);
        EventsPanel = new wxScrolledWindow(LDNotebook);
        EventsPanel->SetScrollRate(5,5);
        LDNotebook->AddPage(PropertiesPanel,_("Properties"));
        LDNotebook->AddPage(EventsPanel,_("Events"));
        Sizer->Add(LDNotebook,1,wxGROW);
        PropertiesContainer->SetSizer(Sizer);

        wxsPropertiesMan::Get()->PropertiesPanel = PropertiesPanel;
        wxsPropertiesMan::Get()->EventsPanel = EventsPanel;

        LeftSplitter->Split(ResourcesContainer,PropertiesContainer);

	}
	else
	{
        LeftSplitter = NULL;
        PropertiesPanel = NULL;
        EventsPanel = NULL;
        ResourceBrowser = NULL;
	}

    ProjectLoaderHooks::HookFunctorBase* myhook = new ProjectLoaderHooks::HookFunctor<wxSmith>(this, &wxSmith::OnProjectHook);
    HookId = ProjectLoaderHooks::RegisterHook(myhook);
}

void wxSmith::OnRelease(bool appShutDown)
{
    ProjectLoaderHooks::UnregisterHook(HookId,true);
    for ( ProjectMapI i = ProjectMap.begin(); i!=ProjectMap.end(); ++i )
    {
        if ( (*i).second )
        {
            delete (*i).second;
            (*i).second = NULL;
        }
    }

	if ( ExtResManager )
	{
	    delete ExtResManager;
	    ExtResManager = NULL;
	}

    ProjectMap.clear();

	if ( PropertiesManager )
	{
	    delete PropertiesManager;
	    PropertiesManager = NULL;
	}

    // Removing resources page
    wxFlatNotebook* Notebook = Manager::Get()->GetProjectManager()->GetNotebook();
	if ( Notebook )
	{
        int Index = Notebook->GetPageIndex(LeftSplitter);
        if ( Index >= 0 )
        {
            Notebook->RemovePage(Index,false);
        }
	}
	if ( LeftSplitter )
	{
	    delete LeftSplitter;
	    LeftSplitter = NULL;
	}

	if ( Coder )
	{
        delete Coder;
        Coder = NULL;
	}

	if ( WidgetFactory )
	{
	    delete WidgetFactory;
	    WidgetFactory = NULL;
	}

	wxsStdManager.Uninitialize();

	if ( Singleton == this ) Singleton = NULL;
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
    wxsProject* SmithProj = (*i).second;
    ProjectMap.erase(i);
    delete SmithProj;
    event.Skip();
}

void wxSmith::OnProjectOpen(CodeBlocksEvent& event)
{
    // Project should be loaded before using project loader hooks
    if ( !GetSmithProject(event.GetProject()) )
    {
        DBGLOG(_T("wxSmith: There's something wrong with C::B project extensions support, no \"Extensions\" node ?"));
        // Strange situation, cbp file doesn't contain "Extensions" node
        wxsProject* NewProj = new wxsProject;
        NewProj->BindProject(event.GetProject(),NULL);
        ProjectMap[event.GetProject()] = NewProj;
    }
    event.Skip();
}

void wxSmith::OnProjectActivated(CodeBlocksEvent& event)
{
    event.Skip();
}

void wxSmith::OnSpreadEvent(wxsEvent& event)
{
    // Adding blocking flag to prevent ping-pong spreads
    static bool Block = false;
    if ( Block ) return;
    Block = true;
    wxsPropertiesMan::Get()->ProcessEvent(event);
    wxsWindowEditor::SpreadEvent(event);
    Block = false;
}

void wxSmith::OnSelectRes(wxsEvent& event)
{
    wxsResource* Res = event.GetResource();
    if ( Res )
    {
        Res->EditOpen();
        Res->OnSelect();
    }
    OnSpreadEvent(event);
}

void wxSmith::OnSelectWidget(wxsEvent& event)
{
    wxsResource* Res = event.GetWidget()->GetResource();
    if ( Res )
    {
        Res->EditOpen();
    }
    OnSpreadEvent(event);
}

cbProject* wxSmith::GetCBProject(wxsProject* Proj)
{
    return Proj->GetCBProject();
}

wxsProject* wxSmith::GetSmithProject(cbProject* Proj)
{
    ProjectMapI i = ProjectMap.find(Proj);
    if ( i == ProjectMap.end() ) return NULL;
    return (*i).second;
}

void wxSmith::OnNewWindow(wxCommandEvent& event)
{
    if ( !CheckIntegration() ) return;

    wxString ResType = _T("Dialog");

    if ( event.GetId() == NewDialogId )      ResType = _T("Dialog");
    else if ( event.GetId() == NewFrameId  ) ResType = _T("Frame");
    else if ( event.GetId() == NewPanelId  ) ResType = _T("Panel");
    else
    {
    	wxMessageBox(_("Internal error - invalid resource type"));
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
        _("XRC files (*.xrc)|*.xrc"),
        wxOPEN|wxFILE_MUST_EXIST
#if (WXWIN_COMPATIBILITY_2_4)
        | wxHIDE_READONLY
#endif
        );

    if ( FileName.empty() ) return;

    // Loading xrc file into xml document

    TiXmlDocument Doc(cbU2C(FileName));
    TiXmlElement* Resource;     if (! Doc.LoadFile() ||
        ! (Resource = Doc.FirstChildElement("resource")) )
    {
    	wxMessageBox(_("Couldn't load XRC file."));
        return;
    }     // Generating list of objects
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
    }     if ( !Element ) return;

    // Creating fake resource and testing if xrc can be loaded without any errors
    wxsWidget* Test = wxsGEN(cbC2U(Element->Attribute("class")),NULL);
    if ( !Test )
    {
    	// Something went wrong - default factory is not working ?
    	wxMessageBox(_("Internal error"));
    	return;
    }

    if ( !Test->XmlLoad(Element) )
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
    wxsImportXrcDlg Dlg(Manager::Get()->GetAppWindow(),Element);
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
        DBGLOG(_("Something wrong - couldn't find associated wxsProject"));
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

void wxSmith::OnProjectHook(cbProject* project,TiXmlElement* elem,bool loading)
{
    if ( loading )
    {
        // Hook called when loading project file.
        // It should be right before sending open event

        wxsProject* Proj = GetSmithProject(project);
        if ( !Proj )
        {
            Proj = new wxsProject();
            ProjectMap[project] = Proj;
        }
        Proj->BindProject(project,elem);
    }
    else
    {
        wxsProject* Proj = GetSmithProject(project);
        if ( Proj )
        {
            Proj->XmlStore(elem);
        }
    }
}

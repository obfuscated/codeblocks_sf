/***************************************************************
 * Name:      wxsmith.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    BYO<byo@o2.pl>
 * Created:   04/10/05 01:05:08
 * Copyright: (c) BYO
 * License:   GPL
 **************************************************************/

#if defined(__GNUG__) && !defined(__APPLE__)
	#pragma implementation "wxsmith.h"
#endif

#include "wxsmith.h"
#include "wxswindoweditor.h"
#include <licenses.h> // defines some common licenses (like the GPL)
#include <manager.h>
#include <wx/notebook.h>
#include <tinyxml/tinyxml.h>
#include <messagemanager.h>
#include <cbeditor.h>
#include <projectmanager.h>
#include <wx/sashwin.h>

#include "resources/wxsdialogres.h"
#include "defwidgets/wxsstdmanager.h"
#include "wxscodegen.h"
#include "wxspropertiesman.h"
#include "wxsproject.h"
#include "wxswidgetfactory.h"
#include "wxspalette.h"
#include "wxsevent.h"
#include "wxsnewdialogdlg.h"

static int NewDialogId = wxNewId();

class wxsResourceTree: public wxTreeCtrl
{
    public:
    
        wxsResourceTree(wxWindow* Parent):
            wxTreeCtrl(Parent,-1)
        {}
        
        void OnSelectResource(wxTreeEvent& event)
        {
            wxsResourceTreeData* Data = dynamic_cast<wxsResourceTreeData*> (GetItemData(event.GetItem()));
            if ( Data )
            {
                switch ( Data->Type )
                {
                    case wxsResourceTreeData::tWidget:
                        {
                            wxsEvent SelectEvent(wxEVT_SELECT_WIDGET,0,NULL,Data->Widget);
                            wxPostEvent(wxSmith::Get(),SelectEvent);
                        }
                        break;
                        
                    case wxsResourceTreeData::tResource:
                        {
                            wxsEvent SelectEvent(wxEVT_SELECT_RES,0,Data->Resource);
                            wxPostEvent(wxSmith::Get(),SelectEvent);
                        }
                        break;
                        
                    default:;
                }
            }
        }
    
        DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxsResourceTree,wxTreeCtrl)
    EVT_TREE_SEL_CHANGED(wxID_ANY,wxsResourceTree::OnSelectResource)
END_EVENT_TABLE()


cbPlugin* GetPlugin()
{
	return new wxSmith;
}
wxSmith* wxSmith::Singleton = NULL;

BEGIN_EVENT_TABLE(wxSmith, cbPlugin)
	EVT_PROJECT_CLOSE(wxSmith::OnProjectClose)
	EVT_PROJECT_OPEN(wxSmith::OnProjectOpen)
	EVT_PROJECT_ACTIVATE(wxSmith::OnProjectActivated)
	EVT_SELECT_RES(wxSmith::OnSpreadEvent)
	EVT_UNSELECT_RES(wxSmith::OnSpreadEvent)
	EVT_SELECT_WIDGET(wxSmith::OnSpreadEvent)
	EVT_UNSELECT_WIDGET(wxSmith::OnSpreadEvent)
	EVT_MENU(NewDialogId,wxSmith::OnNewDialog)
END_EVENT_TABLE()

wxSmith::wxSmith()
{
	//ctor
	m_PluginInfo.name = "wxSmith";
	m_PluginInfo.title = "wxSmith";
	m_PluginInfo.version = "1.0";
	m_PluginInfo.description = "RAD tool used to create wxWidgets forms";
	m_PluginInfo.author = "BYO";
	m_PluginInfo.authorEmail = "byo.spoon@gmail.com";
	m_PluginInfo.authorWebsite = "";
	m_PluginInfo.thanksTo = "Ann for Being\nGigi for Faworki\n\nGod for Love";
	m_PluginInfo.license = LICENSE_GPL;
	m_PluginInfo.hasConfigure = true;
	
	if ( Singleton == NULL ) Singleton = this;
}

wxSmith::~wxSmith()
{
	if ( Singleton == this ) Singleton = NULL;
}

void FillTreeCtlReq(wxTreeCtrl* Tree,wxTreeItemId Parent,TiXmlElement* El)
{
    while ( El )
    {
        wxTreeItemId New = Tree->AppendItem(Parent,El->Value());
        FillTreeCtlReq(Tree,New,El->FirstChildElement());
        El = El->NextSiblingElement();
    }
}

void wxSmith::OnAttach()
{
    wxNotebook* Notebook = Manager::Get()->GetNotebook();
	if ( Notebook )
	{
        // Creating main splitting objects 
        LeftSplitter = new wxSplitterWindow(Notebook,-1,wxDefaultPosition,wxDefaultSize,0);
        wxPanel* ResourcesContainer = new wxPanel(LeftSplitter,-1,wxDefaultPosition,wxDefaultSize,0);
        wxPanel* PropertiesContainer = new wxPanel(LeftSplitter,-1,wxDefaultPosition,wxDefaultSize,wxSTATIC_BORDER);
        
        // TODO (SpOoN#1#): Find in configuration where to split
        LeftSplitter->SplitHorizontally(ResourcesContainer,PropertiesContainer,400);
        // Adding resource browser

        wxSizer* Sizer = new wxGridSizer(1);
        ResourceBrowser = new wxsResourceTree(ResourcesContainer);
        ResourceBrowser->AddRoot(wxT("Resources"));
        Sizer->Add(ResourceBrowser,0,wxGROW);
        ResourcesContainer->SetSizer(Sizer);
        // Adding notebook and two pages at the left-bottom part
        Sizer = new wxGridSizer(1);
        wxNotebook* LDNotebook = new wxNotebook(PropertiesContainer,-1,wxDefaultPosition,wxDefaultSize,wxSUNKEN_BORDER);
        PropertiesPanel = new wxScrolledWindow(LDNotebook);
        PropertiesPanel->SetScrollRate(5,5);
        EventsPanel = new wxScrolledWindow(LDNotebook);
        EventsPanel->SetScrollRate(5,5);
        LDNotebook->AddPage(PropertiesPanel,wxT("Properties"));
        LDNotebook->AddPage(EventsPanel,wxT("Events"));
        Sizer->Add(LDNotebook,0,wxGROW);
        PropertiesContainer->SetSizer(Sizer);
        Notebook->AddPage(LeftSplitter,wxT("Resources"));
        
        wxsPropertiesMan::Get()->PropertiesPanel = PropertiesPanel;
        
        // Adding palette at the bottom

        MessageManager* Messages = Manager::Get()->GetMessageManager();
        
        Manager::Get()->Loadxrc("/wxsmith.zip#zip:*");
        
        // Initializing standard manager
        
        wxsStdManager.Initialize();
        
        if ( ! wxsStdManager.RegisterInFactory() )
        {
            DebLog("Couldn't register standard widget's factory - this plugin will be useless");
        }
        // TODO (SpOoN#1#): Add other widgets
        
        if ( Messages )
        {
            wxWindow* Palette = new wxsPalette((wxWindow*)Messages,this,Messages->GetPageCount());
            Messages->AddPage(Palette,wxT("Widgets"));
        }
	}
	else
	{
        LeftSplitter = NULL;
        PropertiesPanel = NULL;
        EventsPanel = NULL;
        ResourceBrowser = NULL;
	}
	
}

void wxSmith::OnRelease(bool appShutDown)
{
    for ( ProjectMapI i = ProjectMap.begin(); i!=ProjectMap.end(); ++i )
    {
        if ( (*i).second )
        {
            delete (*i).second;
            (*i).second = NULL;
        }
    }
  
    ProjectMap.clear();
}

int wxSmith::Configure()
{
	//create and display the configuration dialog for your plugin
	NotImplemented("wxSmith::Configure()");
	return -1;
}

void wxSmith::BuildMenu(wxMenuBar* menuBar)
{
	wxMenu* Menu = new wxMenu;
	Menu->Append(NewDialogId,"Add Dialog");
	
	int ToolsPos = menuBar->FindMenu("&Tools");
	
	if  ( ToolsPos == wxNOT_FOUND )
	{
        menuBar->Append(Menu,wxT("wxSmith"));
	}
	else
	{
        menuBar->Insert(ToolsPos,Menu,wxT("wxSmith"));
	}
}

void wxSmith::BuildModuleMenu(const ModuleType type, wxMenu* menu, const wxString& arg)
{
	NotImplemented("wxSmith::OfferModuleMenuSpace()");
}

void wxSmith::BuildToolBar(wxToolBar* toolBar)
{
	//The application is offering its toolbar for your plugin,
	//to add any toolbar items you want...
	//Append any items you need on the toolbar...
	NotImplemented("wxSmith::BuildToolBar()");
	return;
}

void wxSmith::OnProjectClose(CodeBlocksEvent& event)
{
    cbProject* Proj = event.GetProject();
    ProjectMapI i = ProjectMap.find(Proj);
    if ( i == ProjectMap.end() ) return;
    
    if ( (*i).second )
    {
        (*i).second->SaveProject();
        delete (*i).second;
        (*i).second = NULL;
    }
    
    event.Skip();
}

void wxSmith::OnProjectOpen(CodeBlocksEvent& event)
{
    wxsProject* NewProj = new wxsProject;
    NewProj->BindProject(event.GetProject());
    ProjectMap[event.GetProject()] = NewProj;
    event.Skip();
}

void wxSmith::OnProjectActivated(CodeBlocksEvent& event)
{
    event.Skip();
}

void wxSmith::OnSpreadEvent(wxsEvent& event)
{
    wxPostEvent(wxsPropertiesMan::Get(),event);
    wxPostEvent(wxsPalette::Get(),event);
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

void wxSmith::OnNewDialog(wxCommandEvent& event)
{
    cbProject* Project = Manager::Get()->GetProjectManager()->GetActiveProject();
    
    if ( !Project )
    {
        wxMessageBox(wxT("Please open project first"),wxT("Error"),wxOK|wxICON_ERROR);
        return;
    }
    
    wxsProject* Proj = GetSmithProject(Project);
    
    if ( !Proj )
    {
        DebLog("Something wrong - couldn't find assciated wxsProject");
        return;
    }
    
    switch ( Proj->GetIntegration() )
    {
        case wxsProject::NotBinded:
            return;
            
        case wxsProject::NotWxsProject:
            if ( wxMessageBox(wxT("Active project doesn't use wxSmith.\nShould I change it ?"),
                              wxT("Not wxSmith project"),wxYES_NO|wxICON_EXCLAMATION ) == wxYES )
            {
                if ( !Proj->AddSmithConfig() ) return;
            }
            else
            {
                return;
            }
            break;
        
        default:
            break;
    }
    
    wxsNewDialogDlg Dlg(Manager::Get()->GetAppWindow());
    Dlg.ShowModal();
    event.Skip();
}


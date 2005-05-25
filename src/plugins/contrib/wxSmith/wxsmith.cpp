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
#include <wx/sashwin.h>

#include "resources/wxsdialogres.h"
#include "defwidgets/wxsstdmanager.h"
#include "wxscodegen.h"
#include "wxspropertiesman.h"
#include "wxsproject.h"
#include "wxswidgetfactory.h"
#include "wxspalette.h"


class wxsResourceTree: public wxTreeCtrl
{
    public:
    
        wxsResourceTree(wxWindow* Parent, wxSmith* _Smith):
            wxTreeCtrl(Parent,-1),
            Smith(_Smith)
        {}
        
        void OnSelectResource(wxTreeEvent& event)
        {
            wxsResourceTreeData* Data = dynamic_cast<wxsResourceTreeData*> (GetItemData(event.GetItem()));
            if ( Data && Smith )
            {
                Smith->OnSelectResource(Data);
            }
        }
    
    private:
        wxSmith* Smith;
        
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
	m_PluginInfo.thanksTo = "Gigi for Faworki";
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
        ResourceBrowser = new wxsResourceTree(ResourcesContainer,this);
        ResourceBrowser->AddRoot(wxT("Resources"));
        Sizer->Add(ResourceBrowser,0,wxGROW);
        ResourcesContainer->SetSizer(Sizer);
        // Adding notebook and two pages at the left-bottom part
        Sizer = new wxGridSizer(1);
        wxNotebook* LDNotebook = new wxNotebook(PropertiesContainer,-1,wxDefaultPosition,wxDefaultSize,wxSUNKEN_BORDER);
        PropertiesPanel = new wxScrolledWindow(LDNotebook);
        PropertiesPanel->SetScrollRate(0,1);
        EventsPanel = new wxScrolledWindow(LDNotebook);
        EventsPanel->SetScrollRate(0,1);
        LDNotebook->AddPage(PropertiesPanel,wxT("Properties"));
        LDNotebook->AddPage(EventsPanel,wxT("Events"));
        Sizer->Add(LDNotebook,0,wxGROW);
        PropertiesContainer->SetSizer(Sizer);
        Notebook->AddPage(LeftSplitter,wxT("Resources"));
        
        wxsPropertiesMan::Get()->PropertiesPanel = PropertiesPanel;
        
        // Adding palette at the bottom

        MessageManager* Messages = Manager::Get()->GetMessageManager();
        
        if ( ! wxsStdManager.RegisterInFactory() )
        {
            DebLog("Couldn't register standard widget's factory - this plugin will be useless");
        }
        // TODO (SpOoN#1#): Add other widgets
        
        if ( Messages )
        {
            wxWindow* Palette = new wxsPalette((wxWindow*)Messages,this);
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
	
    // Registering widget factories
}

void wxSmith::OnRelease(bool appShutDown)
{
    /*
    for ( ProjectMapI i = ProjectMap.begin(); i!=ProjectMap.end(); ++i )
    {
        delete (*i).second;
    }
    */
  
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
	NotImplemented("wxSmith::OfferMenuSpace()");
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
    
    (*i).second->SaveProject();
    
    delete (*i).second;
}

void wxSmith::OnProjectOpen(CodeBlocksEvent& event)
{
    wxsProject* NewProj = new wxsProject(this);
    NewProj->BindProject(event.GetProject());
    ProjectMap[event.GetProject()] = NewProj;
}

void wxSmith::OnProjectActivated(CodeBlocksEvent& event)
{}

void wxSmith::OnSelectResource(wxsResourceTreeData* Data)
{
    switch ( Data->Type )
    {
        case wxsResourceTreeData::tWidget:
            OnSelectWidget(Data);
            break;
            
        case wxsResourceTreeData::tDialog:
            OnSelectDialog(Data);
            break;
            
        default:;
    }
}

void wxSmith::OnSelectWidget(wxsResourceTreeData* Data)
{
    wxsPropertiesMan::Get()->SetActiveWidget(Data->Widget);
}

void wxSmith::OnSelectDialog(wxsResourceTreeData* Data)
{
    Data->Dialog->EditOpen();
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


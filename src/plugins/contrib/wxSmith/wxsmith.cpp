/***************************************************************
 * Name:      wxsmith.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    BYO<byo.spoon@gmail.com>
 * Created:   04/10/05 01:05:08
 * Copyright: (c) BYO
 * License:   GPL
 **************************************************************/

#if defined(__GNUG__) && !defined(__APPLE__) && !defined(FASTCOMPILE)
	#pragma implementation "wxsmith.h"
#endif

#include <licenses.h>
#include <manager.h>
#include <tinyxml/tinyxml.h>
#include <messagemanager.h>
#include <cbeditor.h>
#include <projectmanager.h>
#include <wx/notebook.h>
#include <wx/sashwin.h>

#include "wxsmith.h"
#include "wxswindoweditor.h"
//#include "resources/wxsdialogres.h"
#include "defwidgets/wxsstdmanager.h"
#include "wxscodegen.h"
#include "wxspropertiesman.h"
#include "wxsproject.h"
#include "wxswidgetfactory.h"
#include "wxspalette.h"
#include "wxsevent.h"
#include "wxsnewwindowdlg.h"

static int NewDialogId = wxNewId();
static int NewFrameId = wxNewId();
static int NewPanelId = wxNewId();

class wxsResourceTree: public wxTreeCtrl
{
    public:
    
        wxsResourceTree(wxWindow* Parent):
            wxTreeCtrl(Parent,-1)
        {}
        
        void OnSelectResource(wxTreeEvent& event)
        {
            wxsResourceTreeData* Data = ((wxsResourceTreeData*)GetItemData(event.GetItem()));
            if ( Data )
            {
                switch ( Data->Type )
                {
                    case wxsResourceTreeData::tWidget:
                        {
                        	wxsSelectWidget(Data->Widget);
                        }
                        break;
                        
                    case wxsResourceTreeData::tResource:
                        {
                        	wxsSelectRes(Data->Resource);
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
	EVT_MENU(NewDialogId,wxSmith::OnNewWindow)
	EVT_MENU(NewFrameId,wxSmith::OnNewWindow)
	EVT_MENU(NewPanelId,wxSmith::OnNewWindow)
END_EVENT_TABLE()

wxSmith::wxSmith()
{
	//ctor
	m_PluginInfo.name = _("wxSmith");
	m_PluginInfo.title = _("wxSmith");
	m_PluginInfo.version = _("1.0");
	m_PluginInfo.description = _("RAD tool used to create wxWidgets forms");
	m_PluginInfo.author = _("BYO");
	m_PluginInfo.authorEmail = _("byo.spoon@gmail.com");
	m_PluginInfo.authorWebsite = _("");
	m_PluginInfo.thanksTo = _("Ann for Being\nGigi for Faworki\n\nGod for Love\n\nJaakko Salli for wxPropertyGrid");
	m_PluginInfo.license = LICENSE_GPL;
	m_PluginInfo.hasConfigure = true;
	
	if ( Singleton == NULL ) Singleton = this;
}

wxSmith::~wxSmith()
{
	if ( Singleton == this ) Singleton = NULL;
}

void wxSmith::OnAttach()
{
    wxNotebook* Notebook = Manager::Get()->GetNotebook();
	if ( Notebook )
	{
        // Creating main splitting objects 
        
        LeftSplitter = new wxsSplitterWindow(Notebook);
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
        
        MessageManager* Messages = Manager::Get()->GetMessageManager();
        Manager::Get()->Loadxrc(_T("/wxsmith.zip#zip:*"));
        
        // Initializing standard manager
        
        wxsStdManager.Initialize();
        
        if ( ! wxsStdManager.RegisterInFactory() )
        {
            //DebLog("Couldn't register standard widget's factory - this plugin will be useless");
        }
        // TODO (SpOoN#1#): Add other widgets
        
        if ( Messages )
        {
            // Creating widgets palette ad the messages Notebook
            wxWindow* Palette = new wxsPalette((wxWindow*)Messages,this,Messages->GetPageCount());
            Messages->AddPage(Palette,_("Widgets"));
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
	return -1;
}

void wxSmith::BuildMenu(wxMenuBar* menuBar)
{
	wxMenu* Menu = new wxMenu;
	Menu->Append(NewDialogId,_("Add Dialog"));
	Menu->Append(NewFrameId,_("Add Frame"));
	Menu->Append(NewPanelId,_("Add Panel"));
	
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

void wxSmith::BuildModuleMenu(const ModuleType type, wxMenu* menu, const wxString& arg)
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
    if ( SmithProj )
    {
        SmithProj->SaveProject();
        delete SmithProj;
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
    wxsPropertiesMan::Get()->ProcessEvent(event);
    wxsPalette::Get()->ProcessEvent(event);
    for ( ProjectMapI i = ProjectMap.begin(); i != ProjectMap.end(); ++i )
    {
    	(*i).second->SendEventToEditors(event);
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
    
    return (*i).second;
}

void wxSmith::OnNewWindow(wxCommandEvent& event)
{
    cbProject* Project = Manager::Get()->GetProjectManager()->GetActiveProject();
    
    if ( !Project )
    {
        wxMessageBox(_("Please open project first"),_("Error"),wxOK|wxICON_ERROR);
        return;
    }
    
    wxsProject* Proj = GetSmithProject(Project);
    
    if ( !Proj )
    {
        DebLog(_("Something wrong - couldn't find assciated wxsProject"));
        return;
    }
    
    switch ( Proj->GetIntegration() )
    {
        case wxsProject::NotBinded:
            return;
            
        case wxsProject::NotWxsProject:
            if ( wxMessageBox(_("Active project doesn't use wxSmith.\nShould I change it ?"),
                              _("Not wxSmith project"),wxYES_NO|wxICON_EXCLAMATION ) == wxYES )
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


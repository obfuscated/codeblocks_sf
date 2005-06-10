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

#include <wx/imaglist.h>
#include <wx/menu.h>
#include <wx/utils.h>
#include <wx/dir.h>
#include <wx/textdlg.h>
#include <wx/splitter.h>

#include "projectmanager.h" // class's header file
#include "sdk_events.h"
#include "manager.h"
#include "configmanager.h"
#include "cbproject.h"
#include "incrementalselectlistdlg.h"
#include "messagemanager.h"
#include "pluginmanager.h"
#include "editormanager.h"
#include "workspaceloader.h"
#include "filegroupsandmasks.h"
#include "projectsfilemasksdlg.h"
#include "managerproxy.h"
#include "multiselectdlg.h"
#include "cbworkspace.h"
#include "xtra_classes.h"

// static
bool ProjectManager::s_CanShutdown = true;

ProjectManager* ProjectManager::Get(wxNotebook* parent)
{   
    if(Manager::isappShuttingDown()) // The mother of all sanity checks
        ProjectManager::Free();
    else if (!ProjectManagerProxy::Get())
	{
		ProjectManagerProxy::Set(new ProjectManager(parent));
		Manager::Get()->GetMessageManager()->Log(_("ProjectManager initialized"));
	}
    return ProjectManagerProxy::Get();
}

void ProjectManager::Free()
{
	if (ProjectManagerProxy::Get())
	{
		delete ProjectManagerProxy::Get();
		ProjectManagerProxy::Set( 0L );
	}
}

int ID_ProjectManager = wxNewId();
int idMenuSetActiveProject = wxNewId();
int idMenuOpenFile = wxNewId();
int idMenuCloseProject = wxNewId();
int idMenuCloseFile = wxNewId();
int idMenuAddFilePopup = wxNewId();
int idMenuAddFile = wxNewId();
int idMenuRemoveFilePopup = wxNewId();
int idMenuRemoveFile = wxNewId();
int idMenuProjectProperties = wxNewId();
int idMenuFileProperties = wxNewId();
int idMenuTreeProjectProperties = wxNewId();
int idMenuTreeFileProperties = wxNewId();
int idMenuGotoFile = wxNewId();
int idMenuExecParams = wxNewId();
int idMenuViewCategorize = wxNewId();
int idMenuViewUseFolders = wxNewId();
int idMenuViewFileMasks = wxNewId();
int idMenuNextProject = wxNewId();
int idMenuPriorProject = wxNewId();
int idMenuProjectTreeProps = wxNewId();
int idMenuProjectUp = wxNewId();
int idMenuProjectDown = wxNewId();
int idMenuViewCategorizePopup = wxNewId();
int idMenuViewUseFoldersPopup = wxNewId();
int idMenuTreeRenameWorkspace = wxNewId();
// TODO (mandrav#1#): Add "save workspace" context menu entry

#ifndef __WXMSW__
/*  
    Under wxGTK, I have noticed that wxTreeCtrl is not sending a EVT_COMMAND_RIGHT_CLICK
    event when right-clicking on the client area.
    This is a "proxy" wxTreeCtrl descendant that handles this for us...
*/

class PrjTree : public wxTreeCtrl
{
	public:
		PrjTree(wxWindow* parent, int id) : wxTreeCtrl(parent, id) {}
	protected:
		void OnRightClick(wxMouseEvent& event)
		{
            if(!this) return;
		    //Manager::Get()->GetMessageManager()->DebugLog("OnRightClick");
		    int flags;
		    HitTest(wxPoint(event.GetX(), event.GetY()), flags);
		    if (flags & (wxTREE_HITTEST_ABOVE | wxTREE_HITTEST_BELOW | wxTREE_HITTEST_NOWHERE))
		    {
		    	// "proxy" the call
			    wxCommandEvent e(wxEVT_COMMAND_RIGHT_CLICK, ID_ProjectManager);
				wxPostEvent(GetParent(), e);
			}
			else
		    	event.Skip();
		}
		DECLARE_EVENT_TABLE();
};
BEGIN_EVENT_TABLE(PrjTree, wxTreeCtrl)
	EVT_RIGHT_DOWN(PrjTree::OnRightClick)
END_EVENT_TABLE()
#endif // !__WXMSW__


BEGIN_EVENT_TABLE(ProjectManager, wxEvtHandler)
    EVT_TREE_ITEM_ACTIVATED(ID_ProjectManager, ProjectManager::OnProjectFileActivated)
    EVT_TREE_ITEM_RIGHT_CLICK(ID_ProjectManager, ProjectManager::OnTreeItemRightClick)
    EVT_COMMAND_RIGHT_CLICK(ID_ProjectManager, ProjectManager::OnRightClick)
    EVT_MENU(idMenuSetActiveProject, ProjectManager::OnSetActiveProject)
    EVT_MENU(idMenuNextProject, ProjectManager::OnSetActiveProject)
    EVT_MENU(idMenuPriorProject, ProjectManager::OnSetActiveProject)
    EVT_MENU(idMenuProjectUp, ProjectManager::OnSetActiveProject)
    EVT_MENU(idMenuProjectDown, ProjectManager::OnSetActiveProject)
    EVT_MENU(idMenuTreeRenameWorkspace, ProjectManager::OnRenameWorkspace)
    EVT_MENU(idMenuAddFile, ProjectManager::OnAddFileToProject)
    EVT_MENU(idMenuRemoveFile, ProjectManager::OnRemoveFileFromProject)
    EVT_MENU(idMenuAddFilePopup, ProjectManager::OnAddFileToProject)
    EVT_MENU(idMenuRemoveFilePopup, ProjectManager::OnRemoveFileFromProject)
    EVT_MENU(idMenuCloseProject, ProjectManager::OnCloseProject)
    EVT_MENU(idMenuCloseFile, ProjectManager::OnCloseFile)
    EVT_MENU(idMenuOpenFile, ProjectManager::OnOpenFile)
    EVT_MENU(idMenuProjectProperties, ProjectManager::OnProperties)
    EVT_MENU(idMenuFileProperties, ProjectManager::OnProperties)
    EVT_MENU(idMenuTreeProjectProperties, ProjectManager::OnProperties)
    EVT_MENU(idMenuTreeFileProperties, ProjectManager::OnProperties)
	EVT_MENU(idMenuGotoFile, ProjectManager::OnGotoFile)
    EVT_MENU(idMenuExecParams, ProjectManager::OnExecParameters)
    EVT_MENU(idMenuViewCategorize, ProjectManager::OnViewCategorize)
    EVT_MENU(idMenuViewCategorizePopup, ProjectManager::OnViewCategorize)
    EVT_MENU(idMenuViewUseFolders, ProjectManager::OnViewUseFolders)
    EVT_MENU(idMenuViewUseFoldersPopup, ProjectManager::OnViewUseFolders)
    EVT_MENU(idMenuViewFileMasks, ProjectManager::OnViewFileMasks)
END_EVENT_TABLE()

// class constructor
ProjectManager::ProjectManager(wxNotebook* parent)
	: m_pTree(0),
    m_pPanel(0),
    m_pWorkspace(0),
	m_pTopEditor(0),
    m_TreeCategorize(false),
    m_TreeUseFolders(true),
    m_TreeFreezeCounter(0),
    m_IsLoadingProject(false),
	m_IsLoadingWorkspace(false)
{
    SC_CONSTRUCTOR_BEGIN
    m_pParent = parent;
    m_pActiveProject = 0L;
    m_pProjects = new ProjectsArray;
    m_pProjects->Clear();
	// m_pPanel = new wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxCLIP_CHILDREN);
    ProjectManagerProxy::Set(this);
    InitPane();

	m_pFileGroups = new FilesGroupsAndMasks;
	m_TreeCategorize = ConfigManager::Get()->Read("/project_manager/categorize_tree", 1);
	m_TreeUseFolders = ConfigManager::Get()->Read("/project_manager/use_folders", 1);

    RebuildTree();

	ConfigManager::AddConfiguration(_("Project Manager"), "/project_manager");

	// Event handling. This must be THE LAST THING activated on startup.
	// Constructors and destructors must always follow the LIFO rule:
	// Last in, first out.
    Manager::Get()->GetAppWindow()->PushEventHandler(this);    
}

void ProjectManager::InitPane()
{
    SANITY_CHECK();
    if(Manager::isappShuttingDown())
        return;
    if(m_pTree)
        return;
    wxSplitPanel* mypanel = (wxSplitPanel*)(Manager::Get()->GetNotebookPage("Projects",wxTAB_TRAVERSAL | wxCLIP_CHILDREN,true));
    m_pPanel = mypanel;
    wxSplitterWindow* mysplitter = mypanel->GetSplitter();
    BuildTree(mysplitter);
    mypanel->SetAutoLayout(true);
    mypanel->RefreshSplitter(ID_EditorManager,ID_ProjectManager,200);
}

void ProjectManager::BuildTree(wxWindow* parent)
{
    #ifndef __WXMSW__
        m_pTree = new PrjTree(parent, ID_ProjectManager);
    #else
        m_pTree = new wxTreeCtrl(parent, ID_ProjectManager);
    #endif

    wxBitmap bmp;
    m_pImages = new wxImageList(16, 16);
    wxString prefix = ConfigManager::Get()->Read("data_path") + "/images/";
    bmp.LoadFile(prefix + "gohome.png", wxBITMAP_TYPE_PNG); // workspace
    m_pImages->Add(bmp);
    bmp.LoadFile(prefix + "codeblocks.png", wxBITMAP_TYPE_PNG); // project
    m_pImages->Add(bmp);
    bmp.LoadFile(prefix + "ascii.png", wxBITMAP_TYPE_PNG); // file
    m_pImages->Add(bmp);
    bmp.LoadFile(prefix + "folder_open.png", wxBITMAP_TYPE_PNG); // folder
    m_pImages->Add(bmp);
    m_pTree->SetImageList(m_pImages);
}
// class destructor
ProjectManager::~ProjectManager()
{
    SC_DESTRUCTOR_BEGIN

    // this is a core manager, so it is removed when the app is shutting down.
    // in this case, the app has already un-hooked us, so no need to do it ourselves...
//	Manager::Get()->GetAppWindow()->RemoveEventHandler(this);

    if (m_pWorkspace)
        delete m_pWorkspace;
    m_pWorkspace = 0;
    
    int count = m_pProjects->GetCount();
    for (int i = 0; i < count; ++i)
    {
        cbProject* project = m_pProjects->Item(i);
        if (project)
            delete project;
    }
    m_pProjects->Clear();
    
    delete m_pProjects;m_pProjects = 0;
    delete m_pImages;m_pImages = 0;
	delete m_pFileGroups;m_pFileGroups = 0;

	delete m_pTree; m_pTree = 0;

	SC_DESTRUCTOR_END
}

void ProjectManager::CreateMenu(wxMenuBar* menuBar)
{
/* TODO (mandrav#1#): Move menu items from main.cpp, here */
	if (menuBar)
	{
		int pos = menuBar->FindMenu(_("Search"));
		wxMenu* menu = menuBar->GetMenu(pos);
		if (menu)
			menu->Append(idMenuGotoFile, _("Goto file...\tAlt-G"));
		
		pos = menuBar->FindMenu(_("File"));
		menu = menuBar->GetMenu(pos);
		if (menu)
		{
            menu->Insert(menu->GetMenuItemCount() - 1, idMenuFileProperties, _("Properties"));
            menu->Insert(menu->GetMenuItemCount() - 1, wxID_SEPARATOR, ""); // instead of AppendSeparator();
        }

        pos = menuBar->FindMenu(_("Project"));
		menu = menuBar->GetMenu(pos);
		if (menu)
        {
            menu->AppendSeparator();
            menu->Append(idMenuAddFile, _("Add files..."), _("Add files to the project"));
            menu->Append(idMenuRemoveFile, _("Remove files..."), _("Remove files from the project"));

/* FIXME (mandrav#1#): Move this submenu creation in a function.
It is duplicated in ShowMenu() */
            wxMenu* treeprops = new wxMenu;
            treeprops->Append(idMenuProjectUp, _("Move project up\tCtrl-Shift-Up"), _("Move project up in project tree"));
            treeprops->Append(idMenuProjectDown, _("Move project down\tCtrl-Shift-Down"), _("Move project down in project tree"));
            treeprops->AppendSeparator();
            treeprops->Append(idMenuPriorProject, _("Activate prior project\tAlt-F5"), _("Activate prior project in open projects list"));
            treeprops->Append(idMenuNextProject, _("Activate next project\tAlt-F6"), _("Activate next project in open projects list"));
            treeprops->AppendSeparator();
            treeprops->AppendCheckItem(idMenuViewCategorize, _("Categorize by file types"));
            treeprops->AppendCheckItem(idMenuViewUseFolders, _("Display folders as on disk"));
            treeprops->Check(idMenuViewCategorize, ConfigManager::Get()->Read("/project_manager/categorize_tree", 1));
            treeprops->Check(idMenuViewUseFolders, ConfigManager::Get()->Read("/project_manager/use_folders", 1));
            treeprops->Append(idMenuViewFileMasks, _("Edit file types && categories..."));
            menu->AppendSeparator();
            menu->Append(idMenuProjectTreeProps, _("Project tree"), treeprops);

            menu->Append(idMenuExecParams, _("Set &programs' arguments..."), _("Set execution parameters for the targets of this project"));
            menu->Append(idMenuProjectProperties, _("Properties"));
        }
	}
}

void ProjectManager::ReleaseMenu(wxMenuBar* menuBar)
{
    SANITY_CHECK();
}

void ProjectManager::SetProject(cbProject* project, bool refresh)
{
    SANITY_CHECK();
	if (project != m_pActiveProject)
	{
        if (m_pWorkspace)
            m_pWorkspace->SetModified(true);
    }
    if (m_pActiveProject)
        m_pTree->SetItemBold(m_pActiveProject->GetProjectNode(), false);
    m_pActiveProject = project;
    if (m_pActiveProject)
        m_pTree->SetItemBold(m_pActiveProject->GetProjectNode(), true);
	if (refresh)
		RebuildTree();

    if (m_pActiveProject)
        m_pTree->EnsureVisible(m_pActiveProject->GetProjectNode());

	CodeBlocksEvent event(cbEVT_PROJECT_ACTIVATE);
	event.SetProject(m_pActiveProject);
	Manager::Get()->GetPluginManager()->NotifyPlugins(event);
}

void ProjectManager::ShowMenu(wxTreeItemId id, const wxPoint& pt)
{
    SANITY_CHECK();
    if ( !id.IsOk() )
        return;

	wxString caption;
    wxMenu menu;

    FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(id);
    // if it is not the workspace, add some more options
    if (ftd && id != m_TreeRoot)
    {
	    // if it is a project...
    	if (ftd->GetFileIndex() == -1)
        {
			if (ftd->GetProject() != m_pActiveProject)
				menu.Append(idMenuSetActiveProject, _("Activate project"));
    		menu.Append(idMenuCloseProject, _("Close project"));
    		menu.AppendSeparator();
    	    menu.Append(idMenuAddFilePopup, _("Add files to project..."));
            menu.Append(idMenuRemoveFile, _("Remove files from project..."), _("Remove files from the project"));
        }
        // if it is a file...
        else
        {
			// selected project file
			int idx = ftd->GetFileIndex();
			ProjectFile* pf = ftd->GetProject()->GetFile(idx);
			// is it already open in the editor?
            EditorBase* ed = Manager::Get()->GetEditorManager()->IsOpen(pf->file.GetFullPath());
			
			if (ed)
			{
				// is it already active?
				bool active = Manager::Get()->GetEditorManager()->GetActiveEditor() == ed;
				
				if (!active)
				{
					caption.Printf(_("Switch to %s"), m_pTree->GetItemText(id).c_str());
					menu.Append(idMenuOpenFile, caption);
				}
				caption.Printf(_("Close %s"), m_pTree->GetItemText(id).c_str());
				menu.Append(idMenuCloseFile, caption);
			}
			else
			{
				caption.Printf(_("Open %s"), m_pTree->GetItemText(id).c_str());
				menu.Append(idMenuOpenFile, caption);
			}
    		menu.AppendSeparator();
    	    menu.Append(idMenuRemoveFilePopup, _("Remove file from project"));
        }

        // ask any plugins to add items in this menu
        Manager::Get()->GetPluginManager()->AskPluginsForModuleMenu(mtProjectManager, &menu, m_pTree->GetItemText(id));

        menu.AppendSeparator();
        if (ftd->GetFileIndex() == -1)
        {
            // project
/* FIXME (mandrav#1#): Move this submenu creation in a function.
It is duplicated in CreateMenu() */
            wxMenu* treeprops = new wxMenu;
            treeprops->Append(idMenuProjectUp, _("Move project up\tCtrl-Shift-Up"), _("Move project up in project tree"));
            treeprops->Append(idMenuProjectDown, _("Move project down\tCtrl-Shift-Down"), _("Move project down in project tree"));
            treeprops->AppendSeparator();
            treeprops->Append(idMenuPriorProject, _("Activate prior project\tAlt-F5"), _("Activate prior project in open projects list"));
            treeprops->Append(idMenuNextProject, _("Activate next project\tAlt-F6"), _("Activate next project in open projects list"));
            treeprops->AppendSeparator();
            treeprops->AppendCheckItem(idMenuViewCategorize, _("Categorize by file types"));
            treeprops->AppendCheckItem(idMenuViewUseFolders, _("Display folders as on disk"));
            treeprops->Check(idMenuViewCategorize, ConfigManager::Get()->Read("/project_manager/categorize_tree", 1));
            treeprops->Check(idMenuViewUseFolders, ConfigManager::Get()->Read("/project_manager/use_folders", 1));
            treeprops->Append(idMenuViewFileMasks, _("Edit file types && categories..."));

            menu.Append(idMenuProjectTreeProps, _("Project tree"), treeprops);
            menu.Append(idMenuTreeProjectProperties, _("Properties"));
        }
        else
            menu.Append(idMenuTreeFileProperties, _("Properties"));
    }
    else if (id == m_TreeRoot)
    {
        menu.Append(idMenuTreeRenameWorkspace, _("Rename workspace"));
    }

    if (menu.GetMenuItemCount() != 0)
        m_pTree->PopupMenu(&menu, pt);
}

cbProject* ProjectManager::IsOpen(const wxString& filename)
{
    SANITY_CHECK(0L);
	if (filename.IsEmpty())
		return 0L;
    int count = m_pProjects->GetCount();
    for (int i = 0; i < count; ++i)
    {
        cbProject* project = m_pProjects->Item(i);
        if (project && project->GetFilename().Matches(filename))
            return project;
    }
	return 0L;
}

wxMenu* ProjectManager::GetProjectMenu()
{
    SANITY_CHECK(0L);
    wxMenu* result = 0L;
    do
    {
        wxFrame* frame = Manager::Get()->GetAppWindow();
        if(!frame)
            break;        
        wxMenuBar* mb = frame->GetMenuBar();
        if(!mb)
            break;
        result = mb->GetMenu(mb->FindMenu("Project"));
        break;
    }while(false);
    return result;    
}

cbProject* ProjectManager::LoadProject(const wxString& filename)
{
    SANITY_CHECK(0L);
    cbProject* result = 0;

    // disallow application shutdown while opening files
    // WARNING: remember to set it to true, when exiting this function!!!
    s_CanShutdown = false;
	cbProject* project = IsOpen(filename);
	
	// "Try" block (loop which only gets executed once)
	// These blocks are extremely useful in constructs that need 
	// premature exits. Instead of having multiple return points,
	// multiple return values and/or gotos, 
	// you just break out of the loop (which only gets executed once) to exit.
	do 
	{
        if (project)
        {
            if (m_pWorkspace)
                m_pWorkspace->SetModified(true);
            // we 're done
            result = project;
            break;
        }
        m_IsLoadingProject=true;
        project = new cbProject(filename);	
        if(!sanity_check()) 
            break; // sanity check
        // We need to do this because creating cbProject allows the app to be
        // closed in the middle of the operation. So the class destructor gets
        // called in the middle of a method call.
        
        if (!project->IsLoaded())
        {
            delete project;
            break;
        }
        
        if(!sanity_check()) 
            break; // sanity check
            
        m_pProjects->Add(project);

        // to avoid tree flickering, we 'll manually call here the project's BuildTree
        // but before we do it, remove bold from current active project (if any)
        if (m_pActiveProject)
            m_pTree->SetItemBold(m_pActiveProject->GetProjectNode(), false);
        // ok, set the new project
        SetProject(project, false);
        project->BuildTree(m_pTree, m_TreeRoot, m_TreeCategorize, m_TreeUseFolders, m_pFileGroups);
        m_pTree->SetItemBold(project->GetProjectNode(), true);
        m_pTree->Expand(m_TreeRoot); // make sure the root node is open

        if(!sanity_check()) 
            break; // sanity check
            
        project->LoadLayout();
        if(!sanity_check()) 
            break; // sanity check
        project->RestoreTreeState(m_pTree);
        if (m_pWorkspace)
            m_pWorkspace->SetModified(true);
        // we 're done
    
        result = project;
        break;
    }while(false);
    // we 're done

    m_IsLoadingProject=false;
    
    #ifdef USE_OPENFILES_TREE
    Manager::Get()->GetEditorManager()->RebuildOpenedFilesTree();
    #endif
	// Restore child windows' display
	// if(mywin)
    //    mywin->Show();
	
    s_CanShutdown = true;
    return result;
}

cbProject* ProjectManager::NewProject()
{
    return LoadProject(wxEmptyString);
}

bool ProjectManager::QueryCloseAllProjects()
{
    SANITY_CHECK(true);
    unsigned int i;

    if (!Manager::Get()->GetEditorManager()->QueryCloseAll())
        return false;

    for(i=0;i<m_pProjects->GetCount();i++)
    {
        // Ask for saving modified projects. However,
        // we already asked to save projects' files; 
        // do not ask again
        if(!QueryCloseProject(m_pProjects->Item(i),true))
            return false;
    }
    return true;
}

bool ProjectManager::QueryCloseProject(cbProject *proj,bool dontsavefiles)
{
    SANITY_CHECK(true);
    if(!proj) 
        return true;
    if(!dontsavefiles)
        if(!proj->QueryCloseAllFiles())
            return false;
    if (proj->GetModified())
    {
        wxString msg;
        msg.Printf(_("Project '%s' is modified...\nDo you want to save the changes?"), proj->GetTitle().c_str());
        switch (wxMessageBox(msg, _("Save project"), wxICON_QUESTION | wxYES_NO | wxCANCEL))
        {
            case wxYES:     if (!proj->Save()) return false; 
            case wxNO:      break;
            case wxCANCEL:  return false;
        }
    }
    return true;
}

bool ProjectManager::CloseAllProjects(bool dontsave)
{
    SANITY_CHECK(true);    
    if(!dontsave) 
        if(!QueryCloseAllProjects())
            return false;
    FreezeTree();
    while (m_pProjects->GetCount() != 0)
    {
        if (!CloseActiveProject(true))
            return false;
    }
    RebuildTree();
    UnfreezeTree(true);
    return true;
}

bool ProjectManager::CloseProject(cbProject* project,bool dontsave)
{
    SANITY_CHECK(true);
    if (!project)
        return true;
    if(!dontsave)
         if(!QueryCloseProject(project))
            return false;

    cbProject* tmp = 0L;
	bool same = (project == m_pActiveProject);

    if (!same)
    {
	    tmp = m_pActiveProject;
    	SetProject(project);
    }

    // The project and its files should have been saved by now.
    // It's safe to not save files now.
    bool ret = CloseActiveProject(true);

    if (!same)
	    SetProject(tmp);
	return ret;
}

bool ProjectManager::CloseActiveProject(bool dontsave)
{
    SANITY_CHECK(false);
    if (!m_pActiveProject)
        return true;
    if(m_sanitycheck_shutdown) // if shutdown, don't ask.
        dontsave=true;
    if(!dontsave)
         if(!QueryCloseProject(m_pActiveProject))
            return false;
    int index = m_pProjects->Index(m_pActiveProject);
    if (index == wxNOT_FOUND)
        return false;
	Manager::Get()->GetEditorManager()->UpdateProjectFiles(m_pActiveProject);
	m_pActiveProject->SaveTreeState(m_pTree);
    m_pActiveProject->SaveLayout();

    if (m_pWorkspace)
        m_pWorkspace->SetModified(true);

    m_pActiveProject->CloseAllFiles(true);
    m_pProjects->Remove(m_pActiveProject);
    delete m_pActiveProject;
	m_pActiveProject = 0L;
    if (m_pProjects->GetCount() > 0)
        SetProject(m_pProjects->Item(0));
    else
        SetProject(0L);
    return true;
}

bool ProjectManager::SaveProject(cbProject* project)
{
    SANITY_CHECK(false);
        
    if (!project)
        return false;
    if (project->Save())
    {
        RebuildTree();
        return true;
    }
    return false;
}

bool ProjectManager::SaveProjectAs(cbProject* project)
{
    SANITY_CHECK(false);
    if (!project)
        return false;
        
    if (project->SaveAs())
    {
        RebuildTree();
        return true;
    }
    return false;
}

bool ProjectManager::SaveActiveProject()
{
    SANITY_CHECK(false);
    return SaveProject(m_pActiveProject);
}

bool ProjectManager::SaveActiveProjectAs()
{
    SANITY_CHECK(false);
    return SaveProjectAs(m_pActiveProject);
}

bool ProjectManager::SaveAllProjects()
{
    SANITY_CHECK(false);
    FreezeTree();
    int prjCount = m_pProjects->GetCount();
    int count = 0;
    for (int i = 0; i < prjCount; ++i)
    {
        cbProject* project = m_pProjects->Item(i);
        if (SaveProject(project))
            ++count;
    }
    UnfreezeTree(true);
    return count == prjCount;
}

void ProjectManager::MoveProjectUp(cbProject* project, bool warpAround)
{
    SANITY_CHECK();
    if (!project)
        return;

    int idx = m_pProjects->Index(project);
    if (idx == wxNOT_FOUND)
        return; // project not opened in project manager???

    if (idx == 0)
    {
         if (!warpAround)
            return;
        else
            idx = m_pProjects->Count();
    }
    m_pProjects->RemoveAt(idx--);
    m_pProjects->Insert(project, idx);
    RebuildTree();
    if (m_pWorkspace)
        m_pWorkspace->SetModified(true);
}

void ProjectManager::MoveProjectDown(cbProject* project, bool warpAround)
{
    SANITY_CHECK();
    if (!project)
        return;

    int idx = m_pProjects->Index(project);
    if (idx == wxNOT_FOUND)
        return; // project not opened in project manager???

    if (idx == (int)m_pProjects->Count() - 1)
    {
         if (!warpAround)
            return;
        else
            idx = 0;
    }
    m_pProjects->RemoveAt(idx++);
    m_pProjects->Insert(project, idx);
    RebuildTree();
    if (m_pWorkspace)
        m_pWorkspace->SetModified(true);
}

cbWorkspace* ProjectManager::GetWorkspace()
{
    SANITY_CHECK(0L);
    if (!m_pWorkspace)
    {
        m_pWorkspace = new cbWorkspace("");
        m_pWorkspace->SetTitle(_("Workspace"));
        m_pWorkspace->SetModified(false);
    }
    return m_pWorkspace;
}

void ProjectManager::SetTopEditor(EditorBase* ed)
{
    SANITY_CHECK();
    m_pTopEditor = ed;
}

EditorBase* ProjectManager::GetTopEditor()
{
    SANITY_CHECK(0);
    return m_pTopEditor;
}

bool ProjectManager::LoadWorkspace(const wxString& filename)
{
    SANITY_CHECK(false);
    m_pTopEditor = 0;
    if (!CloseWorkspace())
        return false; // didn't close
    m_IsLoadingWorkspace=true;    
// TODO (mandrav#1#): Change default value for 'filename' to DEFAULT_WORKSPACE and
//                    remove the ternary operator in "new cbWorkspace()" below
    m_pWorkspace = new cbWorkspace(filename.IsEmpty() ? DEFAULT_WORKSPACE : filename);
    m_IsLoadingWorkspace=false;
    Manager::Get()->GetEditorManager()->RebuildOpenedFilesTree();
    SANITY_CHECK(false);
    m_pTree->SetItemText(m_TreeRoot, m_pWorkspace->GetTitle());
    if(m_pTopEditor)
        m_pTopEditor->Activate();
    Manager::Get()->GetEditorManager()->RefreshOpenedFilesTree(true);
    return m_pWorkspace->IsOK();
}

bool ProjectManager::SaveWorkspace()
{
    return GetWorkspace()->Save();
}

bool ProjectManager::SaveWorkspaceAs(const wxString& filename)
{
    return GetWorkspace()->SaveAs(filename);
}

bool ProjectManager::QueryCloseWorkspace()
{     
    SANITY_CHECK(true);
    if(!m_pWorkspace)
        return true;

    // don't ask to save the default workspace, if blank workspace is used on app startup
    if (m_pWorkspace->IsDefault() && ConfigManager::Get()->Read("/environment/blank_workspace", 0L) == 1)
        return true;

    if (m_pWorkspace->GetModified())
    {
        // workspace needs save
        wxString msg;
        msg.Printf(_("Workspace '%s' is modified. Do you want to save it?"), m_pWorkspace->GetTitle().c_str());
        switch (wxMessageBox(msg,
                        _("Save workspace"),
                        wxYES_NO | wxCANCEL | wxICON_QUESTION))
        {
            case wxYES: SaveWorkspace(); break;
            case wxCANCEL: return false;
            default: break;
        }
    }
    if(!QueryCloseAllProjects())
        return false;
    return true;
}        

bool ProjectManager::CloseWorkspace()
{
    SANITY_CHECK(false);
    if (m_pWorkspace)
    {
        if(!QueryCloseWorkspace())
            return false;
        if(!CloseAllProjects(true))
            return false;
        
        delete m_pWorkspace;
        m_pWorkspace = 0;
    }
    else
        return CloseAllProjects(false);
    return true;
}

bool ProjectManager::IsLoading()
{
    SANITY_CHECK(false);
    return (m_IsLoadingProject | m_IsLoadingWorkspace);
}

void ProjectManager::FreezeTree()
{
    SANITY_CHECK();
    if (!m_pTree)
        return;
    ++m_TreeFreezeCounter;
    m_pTree->Freeze();
}

void ProjectManager::UnfreezeTree(bool force)
{
    SANITY_CHECK();
    if (!m_pTree)
        return;
    --m_TreeFreezeCounter;
    if (force || m_TreeFreezeCounter <= 0)
    {
        m_pTree->Thaw();
        m_TreeFreezeCounter = 0;
    }
}

void ProjectManager::RebuildTree()
{
    SANITY_CHECK();
    FreezeTree();
    int count = m_pProjects->GetCount();
    for (int i = 0; i < count; ++i)
    {
        cbProject* project = m_pProjects->Item(i);
        if (project)
            project->SaveTreeState(m_pTree);
    }
    m_pTree->DeleteAllItems();
    wxString title="";
    if(m_pWorkspace)
        title=m_pWorkspace->GetTitle();
     if(title=="")
         title="Workspace";
    m_TreeRoot = m_pTree->AddRoot(title, 0, 0);
    for (int i = 0; i < count; ++i)
    {
        cbProject* project = m_pProjects->Item(i);
        if (project)
        {
            project->BuildTree(m_pTree, m_TreeRoot, m_TreeCategorize, m_TreeUseFolders, m_pFileGroups);
            m_pTree->SetItemBold(project->GetProjectNode(), project == m_pActiveProject);
        }
    }
    m_pTree->Expand(m_TreeRoot);

    for (int i = 0; i < count; ++i)
    {
        cbProject* project = m_pProjects->Item(i);
        if (project)
            project->RestoreTreeState(m_pTree);
    }
    UnfreezeTree();
}

int ProjectManager::AddFileToProject(const wxString& filename, cbProject* project, int target)
{
    SANITY_CHECK(-1);
	cbProject* prj = project;
	if (!prj)
		prj = GetActiveProject();
	if (!prj)
		return -1;

	// do we have to ask for target?
	if (target == -1)
	{
		// if project has only one target, use this
		if (prj->GetBuildTargetsCount() == 1)
			target = 0;
		// else display target selection dialog
		else
		{
			target = AskForBuildTargetIndex(prj);
			if (target == -1)
				return -1;
		}
	}
    SANITY_CHECK(-1); // In some dial
	prj->AddFile(target, filename);

	CodeBlocksEvent event(cbEVT_PROJECT_FILE_ADDED);
	event.SetProject(prj);
	event.SetString(filename);
	Manager::Get()->GetPluginManager()->NotifyPlugins(event);

	return target;
}

int ProjectManager::AddFileToProject(const wxString& filename, cbProject* project, wxArrayInt& targets)
{
    SANITY_CHECK(0);
	cbProject* prj = project;
	if (!prj)
		prj = GetActiveProject();
	if (!prj)
		return 0;

	// do we have to ask for target?
	if (targets.GetCount() == 0)
	{
		// if project has only one target, use this
		if (prj->GetBuildTargetsCount() == 1)
			targets.Add(0);
		// else display multiple target selection dialog
		else
		{
			targets = AskForMultiBuildTargetIndex(prj);
			if (targets.GetCount() == 0)
				return 0;
		}
	}

    // add the file to the first selected target
    SANITY_CHECK(0);
    ProjectFile* pf = prj->AddFile(targets[0], filename);
    if (pf)
    {
        // if the file was added succesfully,
        // add to this file the rest of the selected targets...
        for (size_t i = 0; i < targets.GetCount(); ++i)
        {
            ProjectBuildTarget* target = prj->GetBuildTarget(targets[i]);
            if (target)
                pf->AddBuildTarget(target->GetTitle());
        }
    }

    SANITY_CHECK(0);
	CodeBlocksEvent event(cbEVT_PROJECT_FILE_ADDED);
	event.SetProject(prj);
	event.SetString(filename);
	Manager::Get()->GetPluginManager()->NotifyPlugins(event);

	return targets.GetCount();
}

int ProjectManager::AskForBuildTargetIndex(cbProject* project)
{
    SANITY_CHECK(-1);
	cbProject* prj = project;
	if (!prj)
		prj = GetActiveProject();
	if (!prj)
		return -1;

	// ask for target
	wxArrayString array;
	int count = prj->GetBuildTargetsCount();
	for (int i = 0; i < count; ++i)
		array.Add(prj->GetBuildTarget(i)->GetTitle());
	int target = wxGetSingleChoiceIndex(_("Select the target:"), _("Project targets"), array);
	
	return target;
}

wxArrayInt ProjectManager::AskForMultiBuildTargetIndex(cbProject* project)
{
    wxArrayInt indices;
    SANITY_CHECK(indices);
	cbProject* prj = project;
	if (!prj)
		prj = GetActiveProject();
	if (!prj)
		return indices;

	// ask for target
	wxArrayString array;
	int count = prj->GetBuildTargetsCount();
	for (int i = 0; i < count; ++i)
		array.Add(prj->GetBuildTarget(i)->GetTitle());
    
    MultiSelectDlg dlg(0, array, false, _("Select the targets this file should belong to:"));
    if (dlg.ShowModal() == wxID_OK)
        indices = dlg.GetSelectedIndices();
	
	return indices;
}

void ProjectManager::DoOpenFile(ProjectFile* pf, const wxString& filename)
{
    SANITY_CHECK();
	FileType ft = FileTypeOf(filename);
	
	if (ft == ftHeader ||
		ft == ftSource)
	{
        // C/C++ header/source files, always get opened inside Code::Blocks
        cbEditor* ed = Manager::Get()->GetEditorManager()->Open(filename);
        if (ed)
        {
            ed->SetProjectFile(pf);
            ed->Show(true);
            
        }
        else
        {
            wxString msg;
            msg.Printf(_("Failed to open '%s'."), filename.c_str());
            Manager::Get()->GetMessageManager()->DebugLogError(msg);
        }
	}
	else
	{
        // first look for custom editors
        // if that fails, try MIME handlers
		EditorBase* eb = Manager::Get()->GetEditorManager()->IsOpen(filename);
		if (eb && !eb->IsBuiltinEditor())
		{
            // custom editors just get activated
            eb->Activate();
            return;
        }

		// not a recognized file type
        cbMimePlugin* plugin = Manager::Get()->GetPluginManager()->GetMIMEHandlerForFile(filename);
        if (!plugin || plugin->OpenFile(filename) != 0)
		{
            wxString msg;
            msg.Printf(_("Could not open file '%s'.\nNo handler registered for this type of file."), filename.c_str());
            Manager::Get()->GetMessageManager()->DebugLogError(msg);
		}
	}
}

void ProjectManager::DoOpenSelectedFile()
{
    SANITY_CHECK();
    wxTreeItemId sel = m_pTree->GetSelection();
    FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel);

    if (ftd)
    {
	    cbProject* project = ftd->GetProject();
	    ProjectFile* f = project->GetFile(ftd->GetFileIndex());
    	if (f)
        {
			DoOpenFile(f, f->file.GetFullPath());
        }
    }
}

// events

void ProjectManager::OnProjectFileActivated(wxTreeEvent& event)
{
    SANITY_CHECK();
    #ifdef USE_OPENFILES_TREE
    if(!MiscTreeItemData::OwnerCheck(event,m_pTree,this))
        return;
    #endif
	DoOpenSelectedFile();
}

void ProjectManager::OnExecParameters(wxCommandEvent& event)
{
    SANITY_CHECK();
    if (m_pActiveProject)
		m_pActiveProject->SelectTarget(0, true);
}

void ProjectManager::OnRightClick(wxCommandEvent& event)
{
    //Manager::Get()->GetMessageManager()->DebugLog("OnRightClick");
    SANITY_CHECK();

    wxMenu menu;

    // ask any plugins to add items in this menu
    Manager::Get()->GetPluginManager()->AskPluginsForModuleMenu(mtProjectManager, &menu, "");

    // if plugins added to this menu, add a separator
    if (menu.GetMenuItemCount() != 0)
        menu.AppendSeparator();

    menu.AppendCheckItem(idMenuViewCategorizePopup, _("Categorize by file types"));
    menu.AppendCheckItem(idMenuViewUseFoldersPopup, _("Display folders as on disk"));
    menu.AppendSeparator();
    menu.Append(idMenuViewFileMasks, _("Edit file types && categories..."));
    
    menu.Check(idMenuViewCategorizePopup, m_TreeCategorize);
    menu.Check(idMenuViewUseFoldersPopup, m_TreeUseFolders);

    wxPoint pt = wxGetMousePosition();
    pt = m_pPanel->ScreenToClient(pt);
    m_pPanel->PopupMenu(&menu, pt);
}

void ProjectManager::OnTreeItemRightClick(wxTreeEvent& event)
{
    SANITY_CHECK();
    #ifdef USE_OPENFILES_TREE
    if(!MiscTreeItemData::OwnerCheck(event,m_pTree,this))
        return;
    #endif
    if(m_IsLoadingProject)
    {
        wxBell();
        return;
    }

    //Manager::Get()->GetMessageManager()->DebugLog("OnTreeItemRightClick");
	m_pTree->SelectItem(event.GetItem());
    ShowMenu(event.GetItem(), event.GetPoint());
}

void ProjectManager::OnRenameWorkspace(wxCommandEvent& event)
{
    SANITY_CHECK();
    if (m_pWorkspace)
    {
        wxString text = wxGetTextFromUser(_("Please enter the new name for the workspace:"), _("Rename workspace"), m_pWorkspace->GetTitle());
        if (!text.IsEmpty())
        {
            m_pWorkspace->SetTitle(text);
            m_pTree->SetItemText(m_TreeRoot, m_pWorkspace->GetTitle());
        }
    }
}

void ProjectManager::OnSetActiveProject(wxCommandEvent& event)
{
    SANITY_CHECK();
    if (event.GetId() == idMenuSetActiveProject)
    {
        wxTreeItemId sel = m_pTree->GetSelection();
        FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel);
        if (!ftd)
            return;
    
        SetProject(ftd->GetProject(), false);
	}
    else if (event.GetId() == idMenuPriorProject)
    {
        int index = m_pProjects->Index(m_pActiveProject);
        if (index == wxNOT_FOUND)
            return;
        --index;
        if (index < 0)
            index = m_pProjects->GetCount() - 1;
        SetProject(m_pProjects->Item(index), false);
    }
    else if (event.GetId() == idMenuNextProject)
    {
        int index = m_pProjects->Index(m_pActiveProject);
        if (index == wxNOT_FOUND)
            return;
        ++index;
        if (index == (int)m_pProjects->GetCount())
            index = 0;
        SetProject(m_pProjects->Item(index), false);
    }
    else if (event.GetId() == idMenuProjectUp)
    {
        MoveProjectUp(m_pActiveProject);
    }
    else if (event.GetId() == idMenuProjectDown)
    {
        MoveProjectDown(m_pActiveProject);
    }
}

void ProjectManager::OnAddFileToProject(wxCommandEvent& event)
{
    SANITY_CHECK();
    cbProject* prj = 0;
    if (event.GetId() == idMenuAddFile)
        prj = GetActiveProject();
    else
    {
        wxTreeItemId sel = m_pTree->GetSelection();
        FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel);
        if (ftd)
            prj = ftd->GetProject();
    }
    if (!prj)
        return;

    wxFileDialog dlg(m_pPanel,
                    _("Add files to project..."),
                    prj->GetBasePath(),
                    wxEmptyString,
                    KNOWN_SOURCES_DIALOG_FILTER,
                    wxOPEN | wxMULTIPLE | wxFILE_MUST_EXIST);
    dlg.SetFilterIndex(KNOWN_SOURCES_FILTER_INDEX);
    
    if (dlg.ShowModal() == wxID_OK)
    {
		wxArrayInt targets;
        // ask for target only if more than one
		if (prj->GetBuildTargetsCount() == 1)
             targets.Add(0);
		
		wxArrayString array;
        dlg.GetPaths(array);
        for (unsigned int i = 0; i < array.GetCount(); ++i)
		{
            AddFileToProject(array[i], prj, targets);
			if (targets.GetCount() == 0)
				break;
		}
        RebuildTree();
    }
}

void ProjectManager::OnRemoveFileFromProject(wxCommandEvent& event)
{
    SANITY_CHECK();
    if (event.GetId() == idMenuRemoveFile)
    {
        // remove multiple-files
        cbProject* prj = GetActiveProject();
        if (!prj)
            return;
        wxArrayString files;
        for (int i = 0; i < prj->GetFilesCount(); ++i)
        {
            files.Add(prj->GetFile(i)->relativeFilename);
        }
        wxString msg;
        msg.Printf(_("Select files to remove from %s:"), prj->GetTitle().c_str());
        MultiSelectDlg dlg(0, files, false, msg);
        if (dlg.ShowModal() == wxID_OK)
        {
            wxArrayInt indices = dlg.GetSelectedIndices();
            if (indices.GetCount() == 0)
                return;
            if (wxMessageBox(_("Are you sure you want to remove these files from the project?"),
                            _("Confirmation"),
                            wxICON_QUESTION | wxYES_NO | wxNO_DEFAULT) != wxYES)
            {
                return;
            }
            // we iterate the arry backwards, because if we iterate it normally,
            // when we remove the first index, the rest become invalid...
            for (int i = (int)indices.GetCount() - 1; i >= 0; --i)
            {
                ProjectFile* pf = prj->GetFile(indices[i]);
                if (!pf)
                {
                    Manager::Get()->GetMessageManager()->DebugLog("Invalid project file: Index %d", indices[i]);
                    continue;
                }
                wxString filename = pf->file.GetFullPath();
                Manager::Get()->GetMessageManager()->DebugLog("Removing index %d, %s", indices[i], filename.c_str());
                prj->RemoveFile(indices[i]);
                CodeBlocksEvent evt(cbEVT_PROJECT_FILE_REMOVED);
                evt.SetProject(prj);
                evt.SetString(filename);
                Manager::Get()->GetPluginManager()->NotifyPlugins(evt);
            }
            prj->CalculateCommonTopLevelPath();
            RebuildTree();
        }
    }
    else
    {
        // remove single file
        wxTreeItemId sel = m_pTree->GetSelection();
        FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel);
        if (ftd)
        {
            cbProject* prj = ftd->GetProject();
            if (!prj)
                return;
            int fileindex = ftd->GetFileIndex();
            wxString filename = prj->GetFile(fileindex)->file.GetFullPath();
            prj->RemoveFile(fileindex);
            prj->CalculateCommonTopLevelPath();
            RebuildTree();
            CodeBlocksEvent evt(cbEVT_PROJECT_FILE_REMOVED);
            evt.SetProject(prj);
            evt.SetString(filename);
            Manager::Get()->GetPluginManager()->NotifyPlugins(evt);
        }
    }
}

void ProjectManager::OnCloseProject(wxCommandEvent& event)
{
    SANITY_CHECK();
    wxTreeItemId sel = m_pTree->GetSelection();
    FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel);
    cbProject *proj;
    if (ftd)
        proj = ftd->GetProject();
    if(proj)
    {
        if(m_IsLoadingProject)
        {
            wxBell();
        }
        else
            CloseProject(proj);
    }
}

void ProjectManager::OnCloseFile(wxCommandEvent& event)
{
    SANITY_CHECK();
    wxTreeItemId sel = m_pTree->GetSelection();
    FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel);

    if (ftd)
    {
	    cbProject* project = ftd->GetProject();
	    ProjectFile* f = project->GetFile(ftd->GetFileIndex());
    	if (f)
        	Manager::Get()->GetEditorManager()->Close(f->file.GetFullPath());
    }
}

void ProjectManager::OnOpenFile(wxCommandEvent& event)
{
	DoOpenSelectedFile();
}

void ProjectManager::OnProperties(wxCommandEvent& event)
{
    SANITY_CHECK();
    if (event.GetId() == idMenuProjectProperties)
    {
        wxString backupTitle = m_pActiveProject ? m_pActiveProject->GetTitle() : "";
        if (m_pActiveProject && m_pActiveProject->ShowOptions())
        {
            // make sure that cbEVT_PROJECT_ACTIVATE
            // is sent (maybe targets have changed)...
            // rebuild tree  only if title has changed
            SetProject(m_pActiveProject, backupTitle != m_pActiveProject->GetTitle());
        }
    }
    else if (event.GetId() == idMenuTreeProjectProperties)
    {
        wxTreeItemId sel = m_pTree->GetSelection();
        FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel);
    
        cbProject* project = ftd ? ftd->GetProject() : m_pActiveProject;
        wxString backupTitle = project ? project->GetTitle() : "";
        if (project && project->ShowOptions() && project == m_pActiveProject)
        {
            // rebuild tree and make sure that cbEVT_PROJECT_ACTIVATE
            // is sent (maybe targets have changed)...
            // rebuild tree  only if title has changed
            SetProject(project, backupTitle != project->GetTitle());
        }
    }
    else if (event.GetId() == idMenuTreeFileProperties)
    {
        wxTreeItemId sel = m_pTree->GetSelection();
        FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel);
    
        cbProject* project = ftd ? ftd->GetProject() : m_pActiveProject;
        if (project)
        {
            if (ftd && ftd->GetFileIndex() != -1)
            {
                ProjectFile* pf = project->GetFile(ftd->GetFileIndex());
                if (pf)
                    pf->ShowOptions(m_pPanel);
            }
        }
    }
    else // active editor properties
    {
        cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
        if (ed)
        {
            ProjectFile* pf = ed->GetProjectFile();
            if (pf)
                pf->ShowOptions(m_pPanel);
        }
    }
}

void ProjectManager::OnGotoFile(wxCommandEvent& event)
{
    SANITY_CHECK();
	if (!m_pActiveProject)
	{
		Manager::Get()->GetMessageManager()->DebugLog(_("No active project!"));
		return;
	}

	wxArrayString files;
	for (int i = 0; i < m_pActiveProject->GetFilesCount(); ++i)
		files.Add(m_pActiveProject->GetFile(i)->relativeFilename);

	IncrementalSelectListDlg dlg(m_pPanel, files, _("Select file..."), _("Please select file to open:"));
	if (dlg.ShowModal() == wxID_OK)
	{
		ProjectFile* pf = m_pActiveProject->GetFileByFilename(dlg.GetStringSelection(), true);
		if (pf)
		{
			DoOpenFile(pf, pf->file.GetFullPath());
		}
	}
}

void ProjectManager::OnViewCategorize(wxCommandEvent& event)
{
    SANITY_CHECK();
    bool isChecked = event.IsChecked();
    if (event.GetId() == idMenuViewCategorizePopup)
        isChecked = !isChecked;
    m_TreeCategorize = isChecked;
    Manager::Get()->GetAppWindow()->GetMenuBar()->Check(idMenuViewCategorize, m_TreeCategorize);
	ConfigManager::Get()->Write("/project_manager/categorize_tree", m_TreeCategorize);
    RebuildTree();
}

void ProjectManager::OnViewUseFolders(wxCommandEvent& event)
{
    SANITY_CHECK();
    bool isChecked = event.IsChecked();
    if (event.GetId() == idMenuViewUseFoldersPopup)
        isChecked = !isChecked;
    m_TreeUseFolders = isChecked;
    Manager::Get()->GetAppWindow()->GetMenuBar()->Check(idMenuViewUseFolders, m_TreeUseFolders);
	ConfigManager::Get()->Write("/project_manager/use_folders", m_TreeUseFolders);
    RebuildTree();
}

void ProjectManager::OnViewFileMasks(wxCommandEvent& event)
{
    SANITY_CHECK();
	ProjectsFileMasksDlg dlg(Manager::Get()->GetAppWindow(), m_pFileGroups);
	if (dlg.ShowModal() == wxID_OK)
	{
		m_pFileGroups->Save();
		RebuildTree();
	}
}

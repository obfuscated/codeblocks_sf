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

ProjectManager* ProjectManager::Get(wxNotebook* parent)
{
    if (!ProjectManagerProxy::Get())
	{
		ProjectManagerProxy::Set( new ProjectManager(parent) );
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
int idMenuAddFile = wxNewId();
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

#ifndef __WXMSW
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
    EVT_MENU(idMenuAddFile, ProjectManager::OnAddFileToProject)
    EVT_MENU(idMenuRemoveFile, ProjectManager::OnRemoveFileFromProject)
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
    EVT_MENU(idMenuViewUseFolders, ProjectManager::OnViewUseFolders)
    EVT_MENU(idMenuViewFileMasks, ProjectManager::OnViewFileMasks)
END_EVENT_TABLE()

// class constructor
ProjectManager::ProjectManager(wxNotebook* parent)
	: m_Modified(false),
    m_TreeCategorize(false),
    m_TreeUseFolders(true)
{
	Manager::Get()->GetAppWindow()->PushEventHandler(this);
    m_pParent = parent;
    m_pActiveProject = 0L;
    m_pProjects = new ProjectsArray;
    m_pProjects->Clear();
	m_pPanel = new wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxCLIP_CHILDREN);

    wxBoxSizer* bs = new wxBoxSizer(wxVERTICAL);
#ifndef __WXMSW
	m_pTree = new PrjTree(m_pPanel, ID_ProjectManager);
#else
	m_pTree = new wxTreeCtrl(m_pPanel, ID_ProjectManager);
#endif
    bs->Add(m_pTree, 1, wxEXPAND | wxALL);
    m_pPanel->SetAutoLayout(true);
    m_pPanel->SetSizer(bs);

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

    m_pParent->AddPage(m_pPanel, _("Projects"));
	
	m_pFileGroups = new FilesGroupsAndMasks;
	m_TreeCategorize = ConfigManager::Get()->Read("/project_manager/categorize_tree", 1);
	m_TreeUseFolders = ConfigManager::Get()->Read("/project_manager/use_folders", 1);
    RebuildTree();
}

// class destructor
ProjectManager::~ProjectManager()
{
	Manager::Get()->GetAppWindow()->RemoveEventHandler(this);
    int count = m_pProjects->GetCount();
    for (int i = 0; i < count; ++i)
    {
        cbProject* project = m_pProjects->Item(i);
        if (project)
            delete project;
    }
    m_pProjects->Clear();
    delete m_pProjects;
    delete m_pImages;
	delete m_pFileGroups;
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
            menu->Insert(menu->GetMenuItemCount() - 1, -1, ""); // instead of AppendSeparator();
        }

        pos = menuBar->FindMenu(_("Project"));
		menu = menuBar->GetMenu(pos);
		if (menu)
        {
            menu->AppendSeparator();
            menu->Append(idMenuPriorProject, _("Activate prior\tAlt-F5"), _("Activate prior project in open projects list"));
            menu->Append(idMenuNextProject, _("Activate next\tAlt-F6"), _("Activate next project in open projects list"));
            menu->AppendSeparator();

            wxMenu* treeprops = new wxMenu;
            treeprops->AppendCheckItem(idMenuViewCategorize, _("Categorize by file types"));
            treeprops->AppendCheckItem(idMenuViewUseFolders, _("Display folders as on disk"));
            treeprops->Check(idMenuViewCategorize, ConfigManager::Get()->Read("/project_manager/categorize_tree", 1));
            treeprops->Check(idMenuViewUseFolders, ConfigManager::Get()->Read("/project_manager/use_folders", 1));
            treeprops->AppendSeparator();
            treeprops->Append(idMenuViewFileMasks, _("Edit file types && categories..."));
            menu->Append(idMenuProjectTreeProps, _("Project tree properties"), treeprops);

            menu->Append(idMenuExecParams, _("Set execution &parameters..."), _("Set execution parameters for the targets of this project"));
            menu->Append(idMenuProjectProperties, _("Properties"));
        }
	}
}

void ProjectManager::ReleaseMenu(wxMenuBar* menuBar)
{
}

void ProjectManager::SetProject(cbProject* project, bool refresh)
{
	if (project != m_pActiveProject)
        SetModified();
    if (m_pActiveProject)
        m_pTree->SetItemBold(m_pActiveProject->GetProjectNode(), false);
    m_pActiveProject = project;
    if (m_pActiveProject)
        m_pTree->SetItemBold(m_pActiveProject->GetProjectNode(), true);
	if (refresh)
		RebuildTree();

	CodeBlocksEvent event(cbEVT_PROJECT_ACTIVATE);
	event.SetProject(m_pActiveProject);
	Manager::Get()->GetPluginManager()->NotifyPlugins(event);
}

void ProjectManager::ShowMenu(wxTreeItemId id, const wxPoint& pt)
{
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
    	    menu.Append(idMenuAddFile, _("Add files to project..."));
        }
        // if it is a file...
        else
        {
			// selected project file
			int idx = ftd->GetFileIndex();
			ProjectFile* pf = ftd->GetProject()->GetFile(idx);
			// is it already open in the editor?
			cbEditor* ed = Manager::Get()->GetEditorManager()->IsOpen(pf->file.GetFullPath());
			
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
    	    menu.Append(idMenuRemoveFile, _("Remove file from project"));
        }

        // ask any plugins to add items in this menu
        Manager::Get()->GetPluginManager()->AskPluginsForModuleMenu(mtProjectManager, &menu, m_pTree->GetItemText(id));

        menu.AppendSeparator();
        if (ftd->GetFileIndex() == -1)
            menu.Append(idMenuTreeProjectProperties, _("Properties"));
        else
            menu.Append(idMenuTreeFileProperties, _("Properties"));
    }

    if (menu.GetMenuItemCount() != 0)
        m_pPanel->PopupMenu(&menu, pt);
}

cbProject* ProjectManager::IsOpen(const wxString& filename)
{
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

cbProject* ProjectManager::LoadProject(const wxString& filename)
{
	cbProject* project = IsOpen(filename);
	if (project)
	{
		SetModified();
		return project;
	}
	
	project = new cbProject(filename);
	if (!project->IsLoaded())
		return 0L;
	
    m_pProjects->Add(project);
	SetProject(project);
	project->LoadLayout();
	project->RestoreTreeState(m_pTree);

	SetModified();
    return project;
}

cbProject* ProjectManager::NewProject()
{
    return LoadProject(wxEmptyString);
}

bool ProjectManager::CloseAllProjects()
{
    while (m_pProjects->GetCount() != 0)
    {
        if (!CloseActiveProject())
            return false;
    }
    RebuildTree();
    return true;
}

bool ProjectManager::CloseProject(cbProject* project)
{
    if (!project)
        return true;

    cbProject* tmp = 0L;
	bool same = (project == m_pActiveProject);

    if (!same)
    {
	    tmp = m_pActiveProject;
    	SetProject(project);
    }

    CloseActiveProject();

    if (!same)
	    SetProject(tmp);
	return true;
}

bool ProjectManager::CloseActiveProject()
{
    if (!m_pActiveProject)
        return true;

    int index = m_pProjects->Index(m_pActiveProject);
    if (index == wxNOT_FOUND)
        return false;

	Manager::Get()->GetEditorManager()->UpdateProjectFiles(m_pActiveProject);
	m_pActiveProject->SaveTreeState(m_pTree);
	m_pActiveProject->SaveLayout();

    if (m_pActiveProject->GetModified())
    {
        wxString msg;
        msg.Printf(_("Project '%s' is modified...\nDo you want to save the changes?"), m_pActiveProject->GetTitle().c_str());
        switch (wxMessageBox(msg, _("Save project"), wxICON_QUESTION | wxYES_NO | wxCANCEL))
        {
            case wxYES:     if (!m_pActiveProject->Save()) return false; 
            case wxNO:      break;
            case wxCANCEL:  return false;
        }
    }
	SetModified();

    if (!m_pActiveProject->CloseAllFiles())
        return false;
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
    if (!project)
        return true;
    return project->Save();
}

bool ProjectManager::SaveActiveProject()
{
    if (SaveProject(m_pActiveProject))
    {
        RebuildTree();
        return true;
    }
    return false;
}

bool ProjectManager::SaveActiveProjectAs()
{
    if (!m_pActiveProject)
        return true;
    return m_pActiveProject->SaveAs();
}

bool ProjectManager::SaveAllProjects()
{
    int prjCount = m_pProjects->GetCount();
    int count = 0;
    for (int i = 0; i < prjCount; ++i)
    {
        cbProject* project = m_pProjects->Item(i);
        if (SaveProject(project))
            ++count;
    }
    return count == prjCount;
}

const wxString& ProjectManager::GetWorkspace()
{
    if (m_Workspace.IsEmpty())
	{
        // USERPROFILE is used under windows; all other OSes use HOME,
        // so if HOME is not defined, we 'll try USERPROFILE...
		if (wxGetEnv("HOME", &m_Workspace) || wxGetEnv("USERPROFILE", &m_Workspace))
		{
			m_Workspace << "/.CodeBlocks";
			if (!wxDirExists(m_Workspace))
				wxMkdir(m_Workspace, 0755);
			m_Workspace << "/" << DEFAULT_WORKSPACE;
		}
		//m_Workspace = ConfigManager::Get()->Read("/app_path", ".") + "/" + DEFAULT_WORKSPACE;
	}
    return m_Workspace;
}

bool ProjectManager::LoadWorkspace(const wxString& filename)
{
	m_Workspace = filename;
	Manager::Get()->GetMessageManager()->DebugLog("Loading workspace \"%s\"", GetWorkspace().c_str());
	WorkspaceLoader wsp;
	bool ret = wsp.Open(GetWorkspace());
	if (ret)
		SetModified(false);
	return ret;
}

bool ProjectManager::SaveWorkspace(const wxString& filename)
{
    if (!(!m_Workspace.IsEmpty() && filename.IsEmpty()))
        m_Workspace = filename;
	Manager::Get()->GetMessageManager()->DebugLog("Saving workspace \"%s\"", GetWorkspace().c_str());
	WorkspaceLoader wsp;
	bool ret = wsp.Save(GetWorkspace());
	if (ret)
		SetModified(false);
	return ret;
}

void ProjectManager::RebuildTree()
{
    int count = m_pProjects->GetCount();
    for (int i = 0; i < count; ++i)
    {
        cbProject* project = m_pProjects->Item(i);
        if (project)
            project->SaveTreeState(m_pTree);
    }

    m_pTree->DeleteAllItems();
    m_TreeRoot = m_pTree->AddRoot(_("Workspace"), 0, 0);

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
}

int ProjectManager::AddFileToProject(const wxString& filename, cbProject* project, int target)
{
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

	prj->AddFile(target, filename);

	CodeBlocksEvent event(cbEVT_PROJECT_FILE_ADDED);
	event.SetProject(prj);
	event.SetString(filename);
	Manager::Get()->GetPluginManager()->NotifyPlugins(event);

	return target;
}

int ProjectManager::AskForBuildTargetIndex(cbProject* project)
{
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

void ProjectManager::DoOpenFile(ProjectFile* pf, const wxString& filename)
{
	FileType ft = FileTypeOf(filename);
	if (ft == ftHeader ||
		ft == ftSource ||
		ft == ftResource ||
		ft == ftXRCResource ||
		ft == ftLua)
	{
		cbEditor* ed = Manager::Get()->GetEditorManager()->Open(filename);
		if (ed)
		{
			ed->SetProjectFile(pf);
			ed->Show(true);
		}
		else
            Manager::Get()->GetMessageManager()->DebugLog("Failed to open '%s'", filename.c_str());
	}
	else
	{
		// not a recognized file type
		bool opened = false;
		PluginsArray mimes = Manager::Get()->GetPluginManager()->GetMimeOffers();
		for (unsigned int i = 0; i < mimes.GetCount(); ++i)
		{
			cbMimePlugin* plugin = dynamic_cast<cbMimePlugin*>(mimes[i]);
			if (plugin && plugin->CanOpenFile(filename))
			{
				opened = plugin->OpenFile(filename);
				break;
			}
		}
		if (!opened)
		{
            Manager::Get()->GetMessageManager()->DebugLog("Could not open file '%s' (no registered handler)", filename.c_str());
		}
	}
}

void ProjectManager::DoOpenSelectedFile()
{
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
	DoOpenSelectedFile();
}

void ProjectManager::OnExecParameters(wxCommandEvent& event)
{
    if (m_pActiveProject)
		m_pActiveProject->SelectTarget(0, true);
}

void ProjectManager::OnRightClick(wxCommandEvent& event)
{
    //Manager::Get()->GetMessageManager()->DebugLog("OnRightClick");

    wxMenu menu;

    // ask any plugins to add items in this menu
    Manager::Get()->GetPluginManager()->AskPluginsForModuleMenu(mtProjectManager, &menu, "");

    // if plugins added to this menu, add a separator
    if (menu.GetMenuItemCount() != 0)
        menu.AppendSeparator();

    menu.AppendCheckItem(idMenuViewCategorize, _("Categorize by file types"));
    menu.AppendCheckItem(idMenuViewUseFolders, _("Display folders as on disk"));
    menu.AppendSeparator();
    menu.Append(idMenuViewFileMasks, _("Edit file types && categories..."));
    
    menu.Check(idMenuViewCategorize, m_TreeCategorize);
    menu.Check(idMenuViewUseFolders, m_TreeUseFolders);

    wxPoint pt = wxGetMousePosition();
    pt = m_pTree->ScreenToClient(pt);
    m_pPanel->PopupMenu(&menu, pt);
}

void ProjectManager::OnTreeItemRightClick(wxTreeEvent& event)
{
    //Manager::Get()->GetMessageManager()->DebugLog("OnTreeItemRightClick");
	m_pTree->SelectItem(event.GetItem());
    ShowMenu(event.GetItem(), event.GetPoint());
}

void ProjectManager::OnSetActiveProject(wxCommandEvent& event)
{
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
    else // activate next project
    {
        int index = m_pProjects->Index(m_pActiveProject);
        if (index == wxNOT_FOUND)
            return;
        ++index;
        if (index == (int)m_pProjects->GetCount())
            index = 0;
        SetProject(m_pProjects->Item(index), false);
    }
}

void ProjectManager::OnAddFileToProject(wxCommandEvent& event)
{
    wxTreeItemId sel = m_pTree->GetSelection();
    FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel);

    if (!ftd)
        return;

    wxFileDialog dlg(m_pPanel,
                    _("Add files to project..."),
                    ftd->GetProject()->GetBasePath(),
                    wxEmptyString,
                    KNOWN_SOURCES_DIALOG_FILTER,
                    wxOPEN | wxMULTIPLE | wxFILE_MUST_EXIST);
    dlg.SetFilterIndex(KNOWN_SOURCES_FILTER_INDEX);
    
    if (dlg.ShowModal() == wxID_OK)
    {
        cbProject* prj = ftd->GetProject();
		int target = prj->GetBuildTargetsCount() == 1 ? 0 : -1; // ask for target only if more than one
		
		wxArrayString array;
        dlg.GetPaths(array);
        for (unsigned int i = 0; i < array.GetCount(); ++i)
		{
            target = AddFileToProject(array[i], prj, target);
			if (target == -1)
				break;
		}
        RebuildTree();
    }
}

void ProjectManager::OnRemoveFileFromProject(wxCommandEvent& event)
{
    wxTreeItemId sel = m_pTree->GetSelection();
    FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel);

    if (!ftd)
        return;

	cbProject* prj = ftd->GetProject();
	wxString filename = prj->GetFile(ftd->GetFileIndex())->file.GetFullPath();
    prj->RemoveFile(ftd->GetFileIndex());
    RebuildTree();

	CodeBlocksEvent evt(cbEVT_PROJECT_FILE_REMOVED);
	evt.SetProject(prj);
	evt.SetString(filename);
	Manager::Get()->GetPluginManager()->NotifyPlugins(evt);
}

void ProjectManager::OnCloseProject(wxCommandEvent& event)
{
    wxTreeItemId sel = m_pTree->GetSelection();
    FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel);

    if (ftd)
	    CloseProject(ftd->GetProject());
}

void ProjectManager::OnCloseFile(wxCommandEvent& event)
{
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
    if (event.GetId() == idMenuProjectProperties)
    {
        if (m_pActiveProject && m_pActiveProject->ShowOptions())
        {
            // rebuild tree and make sure that cbEVT_PROJECT_ACTIVATE
            // is sent (maybe targets have changed)...
            SetProject(m_pActiveProject, true);
        }
    }
    else if (event.GetId() == idMenuTreeProjectProperties)
    {
        wxTreeItemId sel = m_pTree->GetSelection();
        FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel);
    
        cbProject* project = ftd ? ftd->GetProject() : m_pActiveProject;
        if (project && project->ShowOptions() && project == m_pActiveProject)
        {
            // rebuild tree and make sure that cbEVT_PROJECT_ACTIVATE
            // is sent (maybe targets have changed)...
            SetProject(project, true);
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
        cbEditor* ed = Manager::Get()->GetEditorManager()->GetActiveEditor();
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
    bool isChecked = event.IsChecked();
#ifdef __WXMSW__
	// it seems that wxMSW checkable menus behave differently than wxGTK (others?)
	isChecked = !isChecked;
#endif
    m_TreeCategorize = isChecked;
    Manager::Get()->GetAppWindow()->GetMenuBar()->Check(idMenuViewCategorize, m_TreeCategorize);
	ConfigManager::Get()->Write("/project_manager/categorize_tree", m_TreeCategorize);
    RebuildTree();
}

void ProjectManager::OnViewUseFolders(wxCommandEvent& event)
{
    bool isChecked = event.IsChecked();
#ifdef __WXMSW__
	// it seems that wxMSW checkable menus behave differently than wxGTK (others?)
	isChecked = !isChecked;
#endif
    m_TreeUseFolders = isChecked;
    Manager::Get()->GetAppWindow()->GetMenuBar()->Check(idMenuViewUseFolders, m_TreeUseFolders);
	ConfigManager::Get()->Write("/project_manager/use_folders", m_TreeUseFolders);
    RebuildTree();
}

void ProjectManager::OnViewFileMasks(wxCommandEvent& event)
{
	ProjectsFileMasksDlg dlg(Manager::Get()->GetAppWindow(), m_pFileGroups);
	if (dlg.ShowModal() == wxID_OK)
	{
		m_pFileGroups->Save();
		RebuildTree();
	}
}

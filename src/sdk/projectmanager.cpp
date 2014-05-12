/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/datetime.h>
    #include <wx/dir.h>
    #include <wx/filename.h>

    #include "projectmanager.h" // class's header file
    #include "sdk_events.h"
    #include "manager.h"
    #include "configmanager.h"
    #include "cbproject.h"
    #include "logmanager.h"
    #include "pluginmanager.h"
    #include "editormanager.h"
    #include "uservarmanager.h"
    #include "workspaceloader.h"
    #include "cbworkspace.h"
    #include "cbeditor.h"
    #include "globals.h"
    #include "cbexception.h"  // for cbassert
#endif

#include <wx/progdlg.h>

#include "cbauibook.h"
//#include "filefilters.h"
#include "filegroupsandmasks.h"

template<> ProjectManager* Mgr<ProjectManager>::instance = nullptr;
template<> bool  Mgr<ProjectManager>::isShutdown = false;

// static
bool ProjectManager::s_CanShutdown = true;


BEGIN_EVENT_TABLE(ProjectManager, wxEvtHandler)
END_EVENT_TABLE()

class NullProjectManagerUI : public cbProjectManagerUI
{
    public:
        cbAuiNotebook* GetNotebook() { return nullptr; }
        cbTreeCtrl* GetTree() { return nullptr; }
        void RebuildTree() {}
        void FreezeTree() {}
        void UnfreezeTree(bool force = false) { (void)force; }
        wxTreeItemId GetTreeSelection() { return wxTreeItemId(); }
        void UpdateActiveProject(cbProject* WXUNUSED(oldProject), cbProject* WXUNUSED(newProject), bool WXUNUSED(refresh)) {}
        void RemoveProject(cbProject* WXUNUSED(project)) {}
        void BeginLoadingWorkspace() {}
        void CloseWorkspace() {}
        void FinishLoadingProject(cbProject* WXUNUSED(project), bool WXUNUSED(newAddition), FilesGroupsAndMasks* WXUNUSED(fileGroups)) {}
        void FinishLoadingWorkspace(cbProject* WXUNUSED(activeProject), const wxString& WXUNUSED(workspaceTitle)) {}
        void ShowFileInTree(ProjectFile& WXUNUSED(projectFile)) {}
        bool QueryCloseAllProjects() { return true; }
        bool QueryCloseProject(cbProject* WXUNUSED(proj), bool dontsavefiles = false)  { (void)dontsavefiles; return true; }
        bool QueryCloseWorkspace()  { return true; }
        int AskForBuildTargetIndex(cbProject* project = nullptr) { (void)project; return -1; }
        wxArrayInt AskForMultiBuildTargetIndex(cbProject* project = nullptr) { (void)project; return wxArrayInt(); }
        void ConfigureProjectDependencies(cbProject* base = nullptr) { (void)base; }
        void SwitchToProjectsPage() {}
};

// class constructor
ProjectManager::ProjectManager() :
    m_ui(new NullProjectManagerUI),
    m_pWorkspace(nullptr),
    m_IsLoadingProject(false),
    m_IsLoadingWorkspace(false),
    m_IsClosingProject(false),
    m_IsClosingWorkspace(false),
    m_InitialDir(_T("")),
    m_CanSendWorkspaceChanged(false),
    m_RunningPlugin(nullptr)
{
    m_InitialDir = wxFileName::GetCwd();
    m_pActiveProject = nullptr;
    m_pProjectToActivate = nullptr;
    m_pProjects = new ProjectsArray;
    m_pProjects->Clear();

    m_pFileGroups = new FilesGroupsAndMasks;

    // register event sinks
    Manager::Get()->RegisterEventSink(cbEVT_APP_STARTUP_DONE, new cbEventFunctor<ProjectManager, CodeBlocksEvent>(this, &ProjectManager::OnAppDoneStartup));

    // Event handling. This must be THE LAST THING activated on startup.
    // Constructors and destructors must always follow the LIFO rule:
    // Last in, first out.
    Manager::Get()->GetAppWindow()->PushEventHandler(this);
}

// class destructor
ProjectManager::~ProjectManager()
{
    // this is a core manager, so it is removed when the app is shutting down.
    // in this case, the app has already un-hooked us, so no need to do it ourselves...
//    Manager::Get()->GetAppWindow()->RemoveEventHandler(this);

    delete m_pWorkspace;
    m_pWorkspace = nullptr;

    int count = m_pProjects->GetCount();
    for (int i = 0; i < count; ++i)
    {
        cbProject* project = m_pProjects->Item(i);
        if (project)
            delete project;
    }
    m_pProjects->Clear();

    delete m_pProjects;m_pProjects = nullptr;
    delete m_pFileGroups;m_pFileGroups = nullptr;

    delete m_ui;
    m_ui = nullptr;
}

void ProjectManager::SetUI(cbProjectManagerUI *ui)
{
    delete m_ui;
    m_ui = ui;
}

wxString ProjectManager::GetDefaultPath()
{
    wxString path = Manager::Get()->GetConfigManager(_T("project_manager"))->Read(_T("default_path"), wxEmptyString);
    if (!path.IsEmpty() && path.Last() != _T('/') && path.Last() != _T('\\'))
        path.Append(wxFILE_SEP_PATH);
    return path;
}

void ProjectManager::SetDefaultPath(const wxString& path)
{
    Manager::Get()->GetConfigManager(_T("project_manager"))->Write(_T("default_path"), path);
}

bool ProjectManager::IsProjectStillOpen(cbProject* project)
{
    int count = m_pProjects->GetCount();
    for (int i = 0; i < count; ++i)
    {
        if (m_pProjects->Item(i) == project)
            return true;
    }
    return false;
}

void ProjectManager::SetProject(cbProject* project, bool refresh)
{
    bool activeProjectChanged = false;
    if (project != m_pActiveProject)
    {
        // Only set workspace as modified, if there was an active project before
        if (m_pWorkspace && m_pActiveProject)
            activeProjectChanged = true;
    }
    else
        return; // already active

    cbProject *oldActiveProject = m_pActiveProject;
    m_pActiveProject = project;

    m_ui->UpdateActiveProject(oldActiveProject, m_pActiveProject, refresh);

    if (activeProjectChanged)
        m_pWorkspace->ActiveProjectChanged();

    CodeBlocksEvent event(cbEVT_PROJECT_ACTIVATE);
    event.SetProject(m_pActiveProject);
    Manager::Get()->GetPluginManager()->NotifyPlugins(event);
}

cbProject* ProjectManager::IsOpen(const wxString& filename)
{
    if (filename.IsEmpty())
        return nullptr;
    wxString realFile = realpath(filename);
    int count = m_pProjects->GetCount();
    for (int i = 0; i < count; ++i)
    {
        cbProject* project = m_pProjects->Item(i);
        if (project)
        {
#ifdef __WXMSW__
            // MS Windows Filenames are case-insensitive, we have to
            // avoid opening the same project if the files are only
            // different in upper/lowercase.
            if (project->GetFilename().Lower().Matches(realFile.Lower()))
                return project;
#else
            if (project->GetFilename().Matches(realFile))
                return project;
#endif
        }
    }
    return nullptr;
}

cbProject* ProjectManager::LoadProject(const wxString& filename, bool activateIt)
{
    cbProject* result = nullptr;
    if (!wxFileExists(filename) || !BeginLoadingProject())
    {
        return nullptr;
    }

    // "Try" block (loop which only gets executed once)
    // These blocks are extremely useful in constructs that need
    // premature exits. Instead of having multiple return points,
    // multiple return values and/or gotos,
    // you just break out of the loop (which only gets executed once) to exit.
    do
    {
        cbProject* project = IsOpen(filename);
        if (project)
        {
            // already open
            result = project;
            break;
        }

        if (FileTypeOf(filename) == ftCodeBlocksProject)
        {
            project = new cbProject(filename);

            // We need to do this because creating cbProject allows the app to be
            // closed in the middle of the operation. So the class destructor gets
            // called in the middle of a method call.

            if (!project->IsLoaded())
            {
                delete project;
                break;
            }

            result = project;
        }
        else // !ftCodeBlocksProject
        {
            // the plugin handler should call begin/end on its own...
            EndLoadingProject(nullptr);

            cbMimePlugin* plugin = Manager::Get()->GetPluginManager()->GetMIMEHandlerForFile(filename);
            if (plugin)
                plugin->OpenFile(filename);
        }

        break;
    }  while (false);
    // we 're done

    EndLoadingProject(result);
    if (activateIt)
    {
        if (m_IsLoadingWorkspace)
            // postpone the call to SetProject() until EndLoadingWorkspace() is called
            // (we must call RebuildTree() before SetProject() is called)
            m_pProjectToActivate = result;
        else
            SetProject(result, true);
    }

    return result;
}

void ProjectManager::ReloadProject(cbProject *project)
{
    m_ui->FreezeTree();

    bool workspaceModified = m_pWorkspace ? m_pWorkspace->GetModified() : false;
    wxString name = project->GetFilename();
    wxString activeProjectName = m_pActiveProject ? m_pActiveProject->GetFilename() : wxString(wxEmptyString);
    ProjectsArray projectDependencies; // all projects that the reloaded project depends on
    ProjectsArray projectsDependingOnReloaded; // all projects that depend on the reloaded project

    for (DepsMap::iterator it = m_ProjectDeps.begin(); it != m_ProjectDeps.end(); ++it)
    {
        if (!it->second)
            continue;

        if (it->first == project)
            projectDependencies = *(it->second);
        else
        {
            if (it->second->Index(project) != wxNOT_FOUND)
                projectsDependingOnReloaded.push_back(it->first);
        }
    }


    int originalPosition = m_pProjects->Index(project);

    CloseProject(project);
    cbProject *loadedProject = LoadProject(name);

    if (loadedProject)
    {
        if (!projectDependencies.empty())
        {
            for (ProjectsArray::iterator it = projectDependencies.begin(); it != projectDependencies.end(); ++it)
                AddProjectDependency(loadedProject, *it);
        }
        if (!projectsDependingOnReloaded.empty())
        {
            for (ProjectsArray::iterator it = projectsDependingOnReloaded.begin();
                 it != projectsDependingOnReloaded.end(); ++it)
            {
                AddProjectDependency(*it, loadedProject);
            }
        }

        int loadedPosition = -1;
        int index = 0;
        cbProject *projectToActivate = nullptr;
        for (ProjectsArray::iterator it = m_pProjects->begin(); it != m_pProjects->end(); ++it, ++index)
        {
            if (*it == loadedProject)
                loadedPosition = index;

            if ((*it)->GetFilename() == activeProjectName)
                projectToActivate = *it;
        }

        m_pProjects->RemoveAt(loadedPosition);
        m_pProjects->Insert(loadedProject, originalPosition);

        if (projectToActivate)
            m_pActiveProject = projectToActivate;

        m_ui->RebuildTree();

        if (m_pWorkspace)
            m_pWorkspace->SetModified(workspaceModified);
    }

    m_ui->UnfreezeTree();
}

cbProject* ProjectManager::NewProject(const wxString& filename)
{
    if (!filename.IsEmpty() && wxFileExists(filename))
    {
        if (cbMessageBox(_("Project file already exists.\nAre you really sure you want to OVERWRITE it?"),
                         _("Confirmation"), wxYES_NO | wxICON_QUESTION) == wxID_YES)
        {
            if (!wxRemoveFile(filename))
            {
                cbMessageBox(_("Couldn't remove the old project file to create the new one.\nThe file might be read-only?!"),
                             _("Error"), wxICON_WARNING);
                return nullptr;
            }
        }
        else
            return nullptr;
    }

    cbProject* prj = IsOpen(filename);
    if (!prj && BeginLoadingProject())
    {
        prj = new cbProject(filename);
        EndLoadingProject(prj);
        SetProject(prj, !m_IsLoadingWorkspace);
    }
    return prj;
}

bool ProjectManager::CloseAllProjects(bool dontsave)
{
    if (!dontsave)
    {
        if (!m_ui->QueryCloseAllProjects())
            return false;
    }

    m_ui->FreezeTree();
    m_IsClosingProject = true;
    while (m_pProjects->GetCount() != 0)
    {
// Commented it by Heromyth
//        if (!CloseActiveProject(true))
        if (!CloseProject(m_pProjects->Item(0), true, false))
        {
            m_ui->UnfreezeTree(true);
            m_IsClosingProject = false;
            return false;
        }
    }

    if (!Manager::IsAppShuttingDown())
        m_ui->RebuildTree();
    m_ui->UnfreezeTree(true);

    if (!m_InitialDir.IsEmpty())
        wxFileName::SetCwd(m_InitialDir);
    m_IsClosingProject = false;
    WorkspaceChanged();

    return true;
}

bool ProjectManager::CloseProject(cbProject* project, bool dontsave, bool refresh)
{
    if (!project)
        return true;
    if (project->GetCurrentlyCompilingTarget())
        return false;
    if (!dontsave)
    {
         if (!m_ui->QueryCloseProject(project))
            return false;
    }

    bool wasActive = project == m_pActiveProject;
    if (wasActive)
        m_pActiveProject = nullptr;

    int index = m_pProjects->Index(project);
    if (index == wxNOT_FOUND)
        return false;

    // CloseProject is also called by CloseAllProjects, so we need to save
    // the state of m_IsClosingProject.
    bool isClosingOtherProjects = m_IsClosingProject;
    m_IsClosingProject = true;
    Manager::Get()->GetEditorManager()->UpdateProjectFiles(project);
    project->SaveLayout();

    if (m_pWorkspace)
        m_pWorkspace->SetModified(true);

    RemoveProjectFromAllDependencies(project);
    m_pProjects->Remove(project);

    // moved here from cbProject's destructor, because by then
    // the list of project files was already emptied...
    CodeBlocksEvent event(cbEVT_PROJECT_CLOSE);
    event.SetProject(project);
    Manager::Get()->GetPluginManager()->NotifyPlugins(event);

    project->CloseAllFiles(true);
    if (refresh)
        m_ui->RemoveProject(project);
    if (wasActive && m_pProjects->GetCount())
        SetProject(m_pProjects->Item(0), refresh);
    delete project;
    if (!m_InitialDir.IsEmpty()) // Restore the working directory
        wxFileName::SetCwd(m_InitialDir);
    m_IsClosingProject = isClosingOtherProjects;
    WorkspaceChanged();
    return true;
}

bool ProjectManager::CloseActiveProject(bool dontsave)
{
    if (!CloseProject(m_pActiveProject, dontsave))
        return false;
    if (m_pProjects->GetCount() > 0)
        SetProject(m_pProjects->Item(0));
    return true;
}

bool ProjectManager::SaveProject(cbProject* project)
{
    if (!project)
        return false;
    if (project->Save())
    {
        m_ui->RebuildTree();
        return true;
    }
    return false;
}

bool ProjectManager::SaveProjectAs(cbProject* project)
{
    if (!project)
        return false;

    if (project->SaveAs())
    {
        m_ui->RebuildTree();
        return true;
    }
    return false;
}

bool ProjectManager::SaveActiveProject()
{
    return SaveProject(m_pActiveProject);
}

bool ProjectManager::SaveActiveProjectAs()
{
    return SaveProjectAs(m_pActiveProject);
}

bool ProjectManager::SaveAllProjects()
{
    m_ui->FreezeTree();
    int prjCount = m_pProjects->GetCount();
    int count = 0;
    for (int i = 0; i < prjCount; ++i)
    {
        cbProject* project = m_pProjects->Item(i);
        if (project)
        {
            bool isModified = project->GetModified();
            if (isModified && SaveProject(project))
                ++count;
        }
    }
    m_ui->UnfreezeTree(true);
    return count == prjCount;
}

cbWorkspace* ProjectManager::GetWorkspace()
{
    if (!m_pWorkspace)
    {
        m_pWorkspace = new cbWorkspace(_T(""));
        m_pWorkspace->SetTitle(_("Workspace"));
        m_pWorkspace->SetModified(false);
    }
    return m_pWorkspace;
}


bool ProjectManager::LoadWorkspace(const wxString& filename)
{
    if ( !BeginLoadingWorkspace() )
        return false;

    m_pWorkspace = new cbWorkspace(filename);
    EndLoadingWorkspace();

    if (m_pProjects->GetCount() > 0 && !m_pActiveProject)
        SetProject(m_pProjects->Item(0), false);

    if ( m_pWorkspace && m_pWorkspace->IsOK() )
    {
        // Fire-up event here, where we're sure there's an active project
        CodeBlocksEvent event(cbEVT_WORKSPACE_LOADING_COMPLETE);
        Manager::Get()->GetPluginManager()->NotifyPlugins(event);

        return true;
    }

    return false;
}

bool ProjectManager::SaveWorkspace()
{
    return GetWorkspace()->Save();
}

bool ProjectManager::SaveWorkspaceAs(const wxString& filename)
{
    return GetWorkspace()->SaveAs(filename);
}

bool ProjectManager::CloseWorkspace()
{
    bool result = false;
    m_IsClosingWorkspace = true;
    if (m_pWorkspace)
    {
        if (!m_ui->QueryCloseWorkspace())
        {
            m_IsClosingWorkspace = false;
            return false;
        }
        // m_ui->QueryCloseWorkspace asked for saving workspace AND projects, no need to do again
        if (!CloseAllProjects(true))
        {
            m_IsClosingWorkspace = false;
            return false;
        }

        delete m_pWorkspace;
        m_pWorkspace = nullptr;

        m_ui->CloseWorkspace();
        result = true;
    }
    else
        result = CloseAllProjects(false);
    m_IsClosingWorkspace = false;
    WorkspaceChanged();
    return result;
}

// This function is static for your convenience :)
bool ProjectManager::IsBusy()
{
    if (Manager::IsAppShuttingDown())
        return true;

    ProjectManager* projman = Manager::Get()->GetProjectManager();
    if (!projman)
        return true;

    return projman->IsLoadingOrClosing();
}

bool ProjectManager::IsLoadingOrClosing()
{
    return (m_IsLoadingProject || m_IsLoadingWorkspace || m_IsClosingProject || m_IsClosingWorkspace);
}

bool ProjectManager::IsLoadingProject()
{
    return m_IsLoadingProject;
}

bool ProjectManager::IsLoadingWorkspace()
{
    return m_IsLoadingWorkspace;
}

bool ProjectManager::IsLoading()
{
    return (m_IsLoadingProject || m_IsLoadingWorkspace);
}

bool ProjectManager::IsClosingProject()
{
    return m_IsClosingProject;
}

bool ProjectManager::IsClosingWorkspace()
{
    return m_IsClosingWorkspace;
}


int ProjectManager::DoAddFileToProject(const wxString& filename, cbProject* project, wxArrayInt& targets)
{
    if (!project)
        return 0;

    // do we have to ask for target?
    if (targets.GetCount() == 0)
    {
        // if project has only one target, use this
        if (project->GetBuildTargetsCount() == 1)
            targets.Add(0);
        // else display multiple target selection dialog
        else
        {
            targets = m_ui->AskForMultiBuildTargetIndex(project);
            if (targets.GetCount() == 0)
                return 0;
        }
    }

    // make sure filename is relative to project path
    wxFileName fname(filename);
    fname.Normalize(wxPATH_NORM_DOTS | wxPATH_NORM_ABSOLUTE, project->GetBasePath());
    fname.MakeRelativeTo(project->GetBasePath());

    // add the file to the first selected target
    ProjectFile* pf = project->AddFile(targets[0], fname.GetFullPath());
    if (pf)
    {
        // if the file was added successfully,
        // add to this file the rest of the selected targets...
        for (size_t i = 0; i < targets.GetCount(); ++i)
        {
            ProjectBuildTarget* target = project->GetBuildTarget(targets[i]);
            if (target)
                pf->AddBuildTarget(target->GetTitle());
        }
    }
    return targets.GetCount();
}

int ProjectManager::AddFileToProject(const wxString& filename, cbProject* project, int target)
{
    if (!project)
        project = GetActiveProject();

    wxArrayInt targets;
    targets.Add(target);
    if (AddFileToProject(filename, project, targets) == 1)
        return targets[0];
    return -1;
}

int ProjectManager::AddFileToProject(const wxString& filename, cbProject* project, wxArrayInt& targets)
{
    if (!project)
        project = GetActiveProject();

    int ret = DoAddFileToProject(filename, project, targets);
    if (ret > 0)
    {
        CodeBlocksEvent event(cbEVT_PROJECT_FILE_ADDED);
        event.SetProject(project);
        event.SetString(filename);
        Manager::Get()->GetPluginManager()->NotifyPlugins(event);
    }
    return ret;
}

int ProjectManager::AddMultipleFilesToProject(const wxArrayString& filelist, cbProject* project, int target)
{
    if (!project)
        project = GetActiveProject();

    wxArrayInt targets;
    targets.Add(target);
    if (AddMultipleFilesToProject(filelist, project, targets) == 1)
        return targets[0];
    return -1;
}

int ProjectManager::AddMultipleFilesToProject(const wxArrayString& filelist, cbProject* project, wxArrayInt& targets)
{
    wxProgressDialog progress(_("Project Manager"), _("Please wait while adding files to project..."), filelist.GetCount(), Manager::Get()->GetAppFrame());

    if (!project)
        project = GetActiveProject();

    if (project)
    {
        project->BeginAddFiles();

        wxArrayString addedFiles; // to know which files were added successfully
        for (unsigned int i = 0; i < filelist.GetCount(); ++i)
        {
            if (DoAddFileToProject(filelist[i], project, targets) != 0)
                addedFiles.Add(filelist[i]);
            progress.Update(i);
        }

        if (addedFiles.GetCount() != 0)
        {
            for (unsigned int i = 0; i < addedFiles.GetCount(); ++i)
            {
                CodeBlocksEvent event(cbEVT_PROJECT_FILE_ADDED);
                event.SetProject(project);
                event.SetString(addedFiles[i]);
                Manager::Get()->GetPluginManager()->NotifyPlugins(event);
            }
        }

        project->EndAddFiles();
    }

    return targets.GetCount();
}

bool ProjectManager::CausesCircularDependency(cbProject* base, cbProject* dependsOn)
{
    if (!base || !dependsOn)
        return false;

    // 1st check is both projects are the same one
    if (base == dependsOn)
        return true;

    const ProjectsArray* arr = GetDependenciesForProject(dependsOn);
    if (arr)
    {
        // now check deeper
        for (size_t i = 0; i < arr->GetCount(); ++i)
        {
            if (CausesCircularDependency(base, arr->Item(i)))
                return true;
        }
    }

    // if we reached here, no possibility of circular dependency :)
    return false;
}

bool ProjectManager::AddProjectDependency(cbProject* base, cbProject* dependsOn)
{
    if (!base || !dependsOn)
        return false;

    // avoid circular dependencies
    if ( CausesCircularDependency(base, dependsOn) )
        return false;

    ProjectsArray* arr = nullptr;
    DepsMap::iterator it = m_ProjectDeps.find(base);
    if (it == m_ProjectDeps.end())
    {
        // create a ProjectsArray* to hold the dependencies for base
        arr = new ProjectsArray;
        m_ProjectDeps[base] = arr;
    }
    else
        arr = it->second;

    // add dependency only if not already there
    if (arr && arr->Index(dependsOn) == wxNOT_FOUND)
    {
        arr->Add(dependsOn);
        if (m_pWorkspace)
            m_pWorkspace->SetModified(true);
        Manager::Get()->GetLogManager()->DebugLog(F(_T("%s now depends on %s (%lu deps)"), base->GetTitle().wx_str(), dependsOn->GetTitle().wx_str(), static_cast<unsigned long>(arr->GetCount())));
    }
    return true;
}

void ProjectManager::RemoveProjectDependency(cbProject* base, cbProject* doesNotDependOn)
{
    if (!base || !doesNotDependOn)
        return;

    DepsMap::iterator it = m_ProjectDeps.find(base);
    if (it == m_ProjectDeps.end())
        return; // nothing to remove

    ProjectsArray* arr = it->second;
    arr->Remove(doesNotDependOn);

    Manager::Get()->GetLogManager()->DebugLog(F(_T("%s now does not depend on %s (%lu deps)"), base->GetTitle().wx_str(), doesNotDependOn->GetTitle().wx_str(), static_cast<unsigned long>(arr->GetCount())));
    // if it was the last dependency, delete the array
    if (!arr->GetCount())
    {
        m_ProjectDeps.erase(it);
        delete arr;
    }
    if (m_pWorkspace)
        m_pWorkspace->SetModified(true);
}

void ProjectManager::ClearProjectDependencies(cbProject* base)
{
    if (!base)
        return;
    DepsMap::iterator it = m_ProjectDeps.find(base);
    if (it == m_ProjectDeps.end())
        return; // nothing to remove

    delete it->second;
    m_ProjectDeps.erase(it);
    if (m_pWorkspace)
        m_pWorkspace->SetModified(true);

    Manager::Get()->GetLogManager()->DebugLog(_T("Removed all deps from ") + base->GetTitle());
}

void ProjectManager::RemoveProjectFromAllDependencies(cbProject* base)
{
    if (!base)
        return;
    DepsMap::iterator it = m_ProjectDeps.begin();
    while (it != m_ProjectDeps.end())
    {
        if (it->first == base)
        {
            ++it;
            continue;
        }

        ProjectsArray* arr = it->second;
        // only check projects that do have a dependencies array
        if (!arr)
        {
            ++it;
            continue;
        }

        int index = arr->Index(base);
        if (index != wxNOT_FOUND)
            arr->RemoveAt(index);

        if (m_pWorkspace)
            m_pWorkspace->SetModified(true);

        // if it was the last dependency, delete the array
        if (!arr->GetCount())
        {
            DepsMap::iterator it2 = it++;
            m_ProjectDeps.erase(it2);
            delete arr;
        }
        else
            ++it;
    }
    Manager::Get()->GetLogManager()->DebugLog(F(_T("Removed %s from all deps"), base->GetTitle().wx_str()));
}

const ProjectsArray* ProjectManager::GetDependenciesForProject(cbProject* base)
{
    DepsMap::iterator it = m_ProjectDeps.find(base);
    if (it != m_ProjectDeps.end())
        return it->second;
    return nullptr;
}

// events

void ProjectManager::OnAppDoneStartup(CodeBlocksEvent& event)
{
    // we do not send the workspace loaded event yet because: a) We don't know
    // if there's a workspace yet, and b) app.cpp hasn't finished init'ing yet.
    // We'll let app.cpp send the workspace changed for us when it's done.
    m_CanSendWorkspaceChanged = true;
    event.Skip();
}

void ProjectManager::WorkspaceChanged()
{
    // We use IsBusy() to check *ALL* the conditions: If we're in the process of
    // opening or closing a project, we cannot send the event yet.
    // Specifically, *DO NOT* send the event if the application hasn't been
    // initialized yet!!
    if (!IsBusy() && m_CanSendWorkspaceChanged)
    {
        CodeBlocksEvent event(cbEVT_WORKSPACE_CHANGED);
        Manager::Get()->GetPluginManager()->NotifyPlugins(event);
        Manager::Get()->GetEditorManager()->GetNotebook()->MinimizeFreeSpace();
    }
}

void ProjectManager::RemoveFileFromProject(ProjectFile *pfile, cbProject* project)
{
    if (!pfile)
    {
        Manager::Get()->GetLogManager()->DebugLog(_T("Invalid project file!"));
        return;
    }

    if (pfile->AutoGeneratedBy())
    {
        cbMessageBox(_("Can't remove file because it is auto-generated..."), _("Error"));
        return;
    }

    if (!project)
        project = pfile->GetParentProject(); // should actually not be necessary

    wxString filename = pfile->file.GetFullPath();
    project->RemoveFile(pfile);

    CodeBlocksEvent evt(cbEVT_PROJECT_FILE_REMOVED);
    evt.SetProject(project);
    evt.SetString(filename);
    Manager::Get()->GetPluginManager()->NotifyPlugins(evt);

    Manager::Get()->GetLogManager()->DebugLog(_T("Removed ") + filename + _T(" from ") + project->GetTitle());
}

bool ProjectManager::BeginLoadingProject()
{
    if (m_IsLoadingProject)
        return false;

    if (!Manager::Get()->GetPluginManager()->FindPluginByName(_T("Compiler")))
    {
        cbMessageBox(_("Deactivating the compiler plugin is most unwise.\n\nIf you intend to open a project, you have to re-activate the compiler plugin first."), _("Error"));
        return false;
    }

    // disallow application shutdown while opening files
    s_CanShutdown = false;
    // flag project loading
    m_IsLoadingProject = true;

    return true;
}

void ProjectManager::EndLoadingProject(cbProject* project)
{
    s_CanShutdown = true;
    if (!m_IsLoadingProject)
        return;

    if (project)
    {
        bool newAddition = m_pProjects->Index(project) == -1;
        if (newAddition)
        {
            m_pProjects->Add(project);
            project->LoadLayout();
        }

        if (!m_IsLoadingWorkspace)
            m_ui->FinishLoadingProject(project, newAddition, m_pFileGroups);

        if (m_pWorkspace)
            m_pWorkspace->SetModified(true);

        // if loading a workspace, avoid sending the event now
        // we 'll send them after all projects have been loaded
        // (look in LoadWorkspace)
        if (!m_IsLoadingWorkspace)
        {
            // notify plugins that the project is loaded
            // moved here from cbProject::Open() because code-completion
            // kicks in too early and the perceived loading time is long...
            CodeBlocksEvent event(cbEVT_PROJECT_OPEN);
            event.SetProject(project);
            Manager::Get()->ProcessEvent(event);

            // finally, display project notes (if appropriate)
            if (project->GetShowNotesOnLoad())
                project->ShowNotes(true);
        }
    }

    /* While loading the project layout, the ProjectManager is still working.
       Thus it should be set to Not Busy only at the end.*/
    m_IsLoadingProject = false;

    // sort out any global user vars that need to be defined now (in a batch) :)
    // but only if not loading workspace (else LoadWorkspace() will handle this)
    if (!m_IsLoadingWorkspace)
        Manager::Get()->GetUserVariableManager()->Arrogate();

    WorkspaceChanged();
}

bool ProjectManager::BeginLoadingWorkspace()
{
    if (m_IsLoadingWorkspace)
        return false;

    m_IsLoadingWorkspace = true;
    if (!CloseWorkspace())
    {
        m_IsLoadingWorkspace = false;
        return false; // didn't close
    }

    m_ui->BeginLoadingWorkspace();

    return true;
}

void ProjectManager::EndLoadingWorkspace()
{
    if (!m_IsLoadingWorkspace)
        return;

    m_IsLoadingWorkspace = false;
    if (!m_pWorkspace)
        return;

    if (m_pWorkspace->IsOK())
    {
        if (m_pProjectToActivate)
        {
            SetProject(m_pProjectToActivate, true);
            m_pProjectToActivate = nullptr;
        }

        m_ui->FinishLoadingWorkspace(m_pActiveProject, m_pWorkspace->GetTitle());

        // sort out any global user vars that need to be defined now (in a batch) :)
        Manager::Get()->GetUserVariableManager()->Arrogate();

        int numNotes = 0;

        // and now send the project loaded events
        // since we were loading a workspace, these events were not sent before
        for (size_t i = 0; i < m_pProjects->GetCount(); ++i)
        {
            cbProject* project = m_pProjects->Item(i);

            // notify plugins that the project is loaded
            // moved here from cbProject::Open() because code-completion
            // kicks in too early and the perceived loading time is long...
            CodeBlocksEvent event(cbEVT_PROJECT_OPEN);
            event.SetProject(project);
            Manager::Get()->GetPluginManager()->NotifyPlugins(event);

            // since we 're iterating anyway, let's count the project notes that should be displayed
            if (project->GetShowNotesOnLoad() && !project->GetNotes().IsEmpty())
                ++numNotes;
        }

        // finally, display projects notes (if appropriate)
        if (numNotes)
        {
            if (numNotes == 1 || // if only one project has notes, don't bother asking
                cbMessageBox(wxString::Format(_("%d projects contain notes that should be displayed on-load.\n"
                                                "Do you want to display them now, one after the other?"),
                                                numNotes),
                                                _("Display project notes?"),
                                                wxICON_QUESTION | wxYES_NO) == wxID_YES)
            {
                for (size_t i = 0; i < m_pProjects->GetCount(); ++i)
                {
                    cbProject* project = m_pProjects->Item(i);
                    if (project->GetShowNotesOnLoad())
                        project->ShowNotes(true);
                }
            }
        }

        WorkspaceChanged();
    }
    else
        CloseWorkspace();
}

void ProjectManager::SetIsRunning(cbPlugin *plugin)
{
    m_RunningPlugin = plugin;
}

cbPlugin* ProjectManager::GetIsRunning() const
{
    return m_RunningPlugin;
}

cbProject* ProjectManager::FindProjectForFile(const wxString& file, ProjectFile **resultFile,
                                              bool isRelative, bool isUnixFilename)
{
    for (size_t i = 0; i < m_pProjects->GetCount(); ++i)
    {
        cbProject* prj = m_pProjects->Item(i);
        ProjectFile *temp = prj->GetFileByFilename(file, isRelative, isUnixFilename);
        if (temp)
        {
            if (resultFile)
                *resultFile = temp;
            return prj;
        }
    }
    if (resultFile)
        *resultFile = nullptr;
    return nullptr;
}

/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk.h"
#include "projectmanagerui.h"

#ifndef CB_PRECOMP
    #include <algorithm>

    #include <wx/checkbox.h>
    #include <wx/choicdlg.h>
    #include <wx/dir.h>
    #include <wx/filedlg.h>
    #include <wx/imaglist.h>
    #include <wx/menu.h>
    #include <wx/settings.h>
    #include <wx/textdlg.h>
    #include <wx/xrc/xmlres.h>

    #include "cbeditor.h"
    #include "cbproject.h"
    #include "cbworkspace.h"
    #include "configmanager.h"
    #include "editormanager.h"
    #include "logmanager.h"
#endif

#include "cbauibook.h"
#include "cbcolourmanager.h"
#include "confirmreplacedlg.h"
#include "filefilters.h"
#include "filegroupsandmasks.h"
#include "incrementalselectlistdlg.h"
#include "multiselectdlg.h"
#include "projectdepsdlg.h"
#include "projectfileoptionsdlg.h"
#include "projectsfilemasksdlg.h"

namespace
{

// maximum number of items in "Open with" context menu
static const unsigned int MAX_OPEN_WITH_ITEMS = 20; // keep it in sync with below array!
static const int idOpenWith[] =
{
    static_cast<int>(wxNewId()), static_cast<int>(wxNewId()), static_cast<int>(wxNewId()), static_cast<int>(wxNewId()), static_cast<int>(wxNewId()),
    static_cast<int>(wxNewId()), static_cast<int>(wxNewId()), static_cast<int>(wxNewId()), static_cast<int>(wxNewId()), static_cast<int>(wxNewId()),
    static_cast<int>(wxNewId()), static_cast<int>(wxNewId()), static_cast<int>(wxNewId()), static_cast<int>(wxNewId()), static_cast<int>(wxNewId()),
    static_cast<int>(wxNewId()), static_cast<int>(wxNewId()), static_cast<int>(wxNewId()), static_cast<int>(wxNewId()), static_cast<int>(wxNewId()),
};
// special entry: force open with internal editor
static const int idOpenWithInternal = wxNewId();

const int ID_ProjectManager              = wxNewId();
const int idMenuSetActiveProject         = wxNewId();
const int idMenuOpenFile                 = wxNewId();
const int idMenuSaveProject              = wxNewId();
const int idMenuSaveFile                 = wxNewId();
const int idMenuCloseProject             = wxNewId();
const int idMenuCloseFile                = wxNewId();
const int idMenuAddFilePopup             = wxNewId();
const int idMenuAddFilesRecursivelyPopup = wxNewId();
const int idMenuAddFile                  = wxNewId();
const int idMenuAddFilesRecursively      = wxNewId();
const int idMenuRemoveFolderFilesPopup   = wxNewId();
const int idMenuOpenFolderFilesPopup     = wxNewId();
const int idMenuRemoveFilePopup          = wxNewId();
const int idMenuRemoveFile               = wxNewId();
const int idMenuRenameFile               = wxNewId();
const int idMenuProjectNotes             = wxNewId();
const int idMenuProjectProperties        = wxNewId();
const int idMenuFileProperties           = wxNewId();
const int idMenuTreeProjectProperties    = wxNewId();
const int idMenuTreeFileProperties       = wxNewId();
const int idMenuGotoFile                 = wxNewId();
const int idMenuExecParams               = wxNewId();
const int idMenuViewCategorize           = wxNewId();
const int idMenuViewUseFolders           = wxNewId();
const int idMenuViewHideFolderName       = wxNewId();
const int idMenuViewFileMasks            = wxNewId();
const int idMenuNextProject              = wxNewId();
const int idMenuPriorProject             = wxNewId();
const int idMenuProjectTreeProps         = wxNewId();
const int idMenuProjectUp                = wxNewId();
const int idMenuProjectDown              = wxNewId();
const int idMenuViewCategorizePopup      = wxNewId();
const int idMenuViewUseFoldersPopup      = wxNewId();
const int idMenuViewHideFolderNamePopup  = wxNewId();
const int idMenuTreeRenameWorkspace      = wxNewId();
const int idMenuTreeSaveWorkspace        = wxNewId();
const int idMenuTreeSaveAsWorkspace      = wxNewId();
const int idMenuTreeCloseWorkspace       = wxNewId();
const int idMenuAddVirtualFolder         = wxNewId();
const int idMenuDeleteVirtualFolder      = wxNewId();
const int idMenuFindFile = wxNewId();
const int idNB           = wxNewId();
const int idNB_TabTop    = wxNewId();
const int idNB_TabBottom = wxNewId();
} // anonymous namespace


BEGIN_EVENT_TABLE(ProjectManagerUI, wxEvtHandler)
    EVT_TREE_BEGIN_DRAG(ID_ProjectManager,       ProjectManagerUI::OnTreeBeginDrag)
    EVT_TREE_END_DRAG(ID_ProjectManager,         ProjectManagerUI::OnTreeEndDrag)

    EVT_TREE_BEGIN_LABEL_EDIT(ID_ProjectManager, ProjectManagerUI::OnBeginEditNode)
    EVT_TREE_END_LABEL_EDIT(ID_ProjectManager,   ProjectManagerUI::OnEndEditNode)

    EVT_TREE_ITEM_ACTIVATED(ID_ProjectManager,   ProjectManagerUI::OnProjectFileActivated)
    EVT_TREE_ITEM_RIGHT_CLICK(ID_ProjectManager, ProjectManagerUI::OnTreeItemRightClick)
    EVT_TREE_KEY_DOWN(ID_ProjectManager,         ProjectManagerUI::OnKeyDown)
    EVT_COMMAND_RIGHT_CLICK(ID_ProjectManager,   ProjectManagerUI::OnRightClick)

    EVT_AUINOTEBOOK_TAB_RIGHT_UP(idNB, ProjectManagerUI::OnTabContextMenu)

    EVT_MENU_RANGE(idOpenWith[0], idOpenWith[MAX_OPEN_WITH_ITEMS - 1], ProjectManagerUI::OnOpenWith)
    EVT_MENU(idOpenWithInternal,             ProjectManagerUI::OnOpenWith)
    EVT_MENU(idNB_TabTop,                    ProjectManagerUI::OnTabPosition)
    EVT_MENU(idNB_TabBottom,                 ProjectManagerUI::OnTabPosition)
    EVT_MENU(idMenuSetActiveProject,         ProjectManagerUI::OnSetActiveProject)
    EVT_MENU(idMenuNextProject,              ProjectManagerUI::OnSetActiveProject)
    EVT_MENU(idMenuPriorProject,             ProjectManagerUI::OnSetActiveProject)
    EVT_MENU(idMenuProjectUp,                ProjectManagerUI::OnSetActiveProject)
    EVT_MENU(idMenuProjectDown,              ProjectManagerUI::OnSetActiveProject)
    EVT_MENU(idMenuTreeRenameWorkspace,      ProjectManagerUI::OnRenameWorkspace)
    EVT_MENU(idMenuTreeSaveWorkspace,        ProjectManagerUI::OnSaveWorkspace)
    EVT_MENU(idMenuTreeSaveAsWorkspace,      ProjectManagerUI::OnSaveAsWorkspace)
    EVT_MENU(idMenuTreeCloseWorkspace,       ProjectManagerUI::OnCloseWorkspace)
    EVT_MENU(idMenuAddVirtualFolder,         ProjectManagerUI::OnAddVirtualFolder)
    EVT_MENU(idMenuDeleteVirtualFolder,      ProjectManagerUI::OnDeleteVirtualFolder)
    EVT_MENU(idMenuAddFile,                  ProjectManagerUI::OnAddFileToProject)
    EVT_MENU(idMenuAddFilesRecursively,      ProjectManagerUI::OnAddFilesToProjectRecursively)
    EVT_MENU(idMenuRemoveFile,               ProjectManagerUI::OnRemoveFileFromProject)
    EVT_MENU(idMenuAddFilePopup,             ProjectManagerUI::OnAddFileToProject)
    EVT_MENU(idMenuAddFilesRecursivelyPopup, ProjectManagerUI::OnAddFilesToProjectRecursively)
    EVT_MENU(idMenuRemoveFolderFilesPopup,   ProjectManagerUI::OnRemoveFileFromProject)
    EVT_MENU(idMenuOpenFolderFilesPopup,     ProjectManagerUI::OnOpenFolderFiles)
    EVT_MENU(idMenuRemoveFilePopup,          ProjectManagerUI::OnRemoveFileFromProject)
    EVT_MENU(idMenuRenameFile,               ProjectManagerUI::OnRenameFile)
    EVT_MENU(idMenuSaveProject,              ProjectManagerUI::OnSaveProject)
    EVT_MENU(idMenuSaveFile,                 ProjectManagerUI::OnSaveFile)
    EVT_MENU(idMenuCloseProject,             ProjectManagerUI::OnCloseProject)
    EVT_MENU(idMenuCloseFile,                ProjectManagerUI::OnCloseFile)
    EVT_MENU(idMenuOpenFile,                 ProjectManagerUI::OnOpenFile)
    EVT_MENU(idMenuProjectNotes,             ProjectManagerUI::OnNotes)
    EVT_MENU(idMenuProjectProperties,        ProjectManagerUI::OnProperties)
    EVT_MENU(idMenuFileProperties,           ProjectManagerUI::OnProperties)
    EVT_MENU(idMenuTreeProjectProperties,    ProjectManagerUI::OnProperties)
    EVT_MENU(idMenuTreeFileProperties,       ProjectManagerUI::OnProperties)
    EVT_MENU(idMenuGotoFile,                 ProjectManagerUI::OnGotoFile)
    EVT_MENU(idMenuExecParams,               ProjectManagerUI::OnExecParameters)
    EVT_MENU(idMenuViewCategorize,           ProjectManagerUI::OnViewCategorize)
    EVT_MENU(idMenuViewUseFolders,           ProjectManagerUI::OnViewUseFolders)
    EVT_MENU(idMenuViewHideFolderName,       ProjectManagerUI::OnViewHideFolderName)
    EVT_MENU(idMenuViewCategorizePopup,      ProjectManagerUI::OnViewCategorize)
    EVT_MENU(idMenuViewUseFoldersPopup,      ProjectManagerUI::OnViewUseFolders)
    EVT_MENU(idMenuViewHideFolderNamePopup,  ProjectManagerUI::OnViewHideFolderName)
    EVT_MENU(idMenuViewFileMasks,            ProjectManagerUI::OnViewFileMasks)
    EVT_MENU(idMenuFindFile,                 ProjectManagerUI::OnFindFile)
    EVT_IDLE(                                ProjectManagerUI::OnIdle)
END_EVENT_TABLE()

ProjectManagerUI::ProjectManagerUI() :
    m_pTree(nullptr),
    m_TreeFreezeCounter(0),
    m_isCheckingForExternallyModifiedProjects(false)
{
    m_pNotebook = new cbAuiNotebook(Manager::Get()->GetAppWindow(), idNB,
                                    wxDefaultPosition, wxDefaultSize, wxAUI_NB_WINDOWLIST_BUTTON);
    if (Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/project_tabs_bottom"), false))
        m_pNotebook->SetWindowStyleFlag(m_pNotebook->GetWindowStyleFlag() | wxAUI_NB_BOTTOM);

    InitPane();

    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("project_manager"));
    m_TreeVisualState  = ptvsNone;
    m_TreeVisualState |= (cfg->ReadBool(_T("/categorize_tree"),  true)  ? ptvsCategorize     : ptvsNone);
    m_TreeVisualState |= (cfg->ReadBool(_T("/use_folders"),      true)  ? ptvsUseFolders     : ptvsNone);
    m_TreeVisualState |= (cfg->ReadBool(_T("/hide_folder_name"), false) ? ptvsHideFolderName : ptvsNone);
    // fix invalid combination, "use folders" has precedence
    if ( (m_TreeVisualState&ptvsUseFolders) && (m_TreeVisualState&ptvsHideFolderName) )
    {
        m_TreeVisualState &= ~ptvsHideFolderName;
        cfg->Write(_T("/hide_folder_name"), false);
    }

    RebuildTree();

    Manager::Get()->GetColourManager()->RegisterColour(_("Project Tree"), _("Not-compiled files (headers/resources)"),
                                                       wxT("project_tree_non_source_files"),
                                                       wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));

    // Event handling. This must be THE LAST THING activated on startup.
    // Constructors and destructors must always follow the LIFO rule:
    // Last in, first out.
    Manager::Get()->GetAppWindow()->PushEventHandler(this);
}

ProjectManagerUI::~ProjectManagerUI()
{
    delete m_pImages;
    m_pImages = nullptr;
    m_pNotebook->Destroy();
}

void ProjectManagerUI::InitPane()
{
    if (Manager::IsAppShuttingDown())
        return;
    if (m_pTree)
        return;
    BuildTree();
    m_pNotebook->AddPage(m_pTree, _("Projects"));
}

void ProjectManagerUI::BuildTree()
{
    m_pTree = new cbTreeCtrl(m_pNotebook, ID_ProjectManager);

    static const wxString imgs[] =
    {
        // NOTE: Keep in sync with FileVisualState in globals.h!

        // The following are related to (editable, source-) file states
        _T("file.png"),                  // fvsNormal
        _T("file-missing.png"),          // fvsMissing,
        _T("file-modified.png"),         // fvsModified,
        _T("file-readonly.png"),         // fvsReadOnly,

        // The following are related to version control systems (vc)
        _T("rc-file-added.png"),         // fvsVcAdded,
        _T("rc-file-conflict.png"),      // fvsVcConflict,
        _T("rc-file-missing.png"),       // fvsVcMissing,
        _T("rc-file-modified.png"),      // fvsVcModified,
        _T("rc-file-outofdate.png"),     // fvsVcOutOfDate,
        _T("rc-file-uptodate.png"),      // fvsVcUpToDate,
        _T("rc-file-requireslock.png"),  // fvsVcRequiresLock,
        _T("rc-file-external.png"),      // fvsVcExternal,
        _T("rc-file-gotlock.png"),       // fvsVcGotLock,
        _T("rc-file-lockstolen.png"),    // fvsVcLockStolen,
        _T("rc-file-mismatch.png"),      // fvsVcMismatch,
        _T("rc-file-noncontrolled.png"), // fvsVcNonControlled,

        // The following are related to C::B workspace/project/folder/virtual
        _T("workspace.png"),             // fvsWorkspace,         WorkspaceIconIndex()
        _T("workspace-readonly.png"),    // fvsWorkspaceReadOnly, WorkspaceIconIndex(true)
        _T("project.png"),               // fvsProject,           ProjectIconIndex()
        _T("project-readonly.png"),      // fvsProjectReadOnly,   ProjectIconIndex(true)
        _T("folder_open.png"),           // fvsFolder,            FolderIconIndex()
        _T("vfolder_open.png"),          // fvsVirtualFolder,     VirtualFolderIconIndex()

        wxEmptyString
    };
    wxBitmap bmp;
    m_pImages = new wxImageList(16, 16);
    wxString prefix = ConfigManager::ReadDataPath() + _T("/images/");

    int i = 0;
    while (!imgs[i].IsEmpty())
    {
        bmp = cbLoadBitmap(prefix + imgs[i], wxBITMAP_TYPE_PNG); // workspace
        m_pImages->Add(bmp);
        ++i;
    }
    m_pTree->SetImageList(m_pImages);
}

void ProjectManagerUI::RebuildTree()
{
    if (Manager::IsAppShuttingDown() || Manager::IsBatchBuild()) // saves a lot of time at startup for large projects
        return;

    FreezeTree();
    ProjectManager *prjManager = Manager::Get()->GetProjectManager();
    ProjectsArray *projects = prjManager->GetProjects();
    int count = projects->GetCount();
    for (int i = 0; i < count; ++i)
    {
        cbProject* project = projects->Item(i);
        if (project)
            project->SaveTreeState(m_pTree);
    }
    m_pTree->DeleteAllItems();
    wxString title;
    bool read_only = false;
    cbWorkspace *workspace = prjManager->GetWorkspace();
    if (workspace)
    {
        title = workspace->GetTitle();
        wxString ws_file = workspace->GetFilename();
        read_only = (   !ws_file.IsEmpty()
                     &&  wxFile::Exists(ws_file.c_str())
                     && !wxFile::Access(ws_file.c_str(), wxFile::write) );
    }
    if (title.IsEmpty())
        title = _("Workspace");
    m_TreeRoot = m_pTree->AddRoot(title, prjManager->WorkspaceIconIndex(read_only), prjManager->WorkspaceIconIndex(read_only));
    for (int i = 0; i < count; ++i)
    {
        cbProject* project = projects->Item(i);
        if (project)
        {
            project->BuildTree(m_pTree, m_TreeRoot, m_TreeVisualState, prjManager->GetFilesGroupsAndMasks());
            m_pTree->SetItemBold(project->GetProjectNode(), project == prjManager->GetActiveProject());
        }
    }
    m_pTree->Expand(m_TreeRoot);

    for (int i = 0; i < count; ++i)
    {
        cbProject* project = projects->Item(i);
        if (project)
            project->RestoreTreeState(m_pTree);
    }
    UnfreezeTree();
}

void ProjectManagerUI::FreezeTree()
{
    if (!m_pTree)
        return;

    ++m_TreeFreezeCounter;
    m_pTree->Freeze();
}

void ProjectManagerUI::UnfreezeTree(cb_unused bool force)
{
    if (!m_pTree)
        return;

    if (m_TreeFreezeCounter)
    {
        --m_TreeFreezeCounter;
        m_pTree->Thaw();
    }
}

void ProjectManagerUI::UpdateActiveProject(cbProject *oldProject, cbProject *newProject, bool refresh)
{
    if (oldProject)
        m_pTree->SetItemBold(oldProject->GetProjectNode(), false);
    if (newProject)
    {
        wxTreeItemId tid = newProject->GetProjectNode();
        if (tid)
            m_pTree->SetItemBold(newProject->GetProjectNode(), true);
    }

    if (refresh)
        RebuildTree();
    if (newProject)
        m_pTree->EnsureVisible(newProject->GetProjectNode());

    m_pTree->Refresh();
}

void ProjectManagerUI::RemoveProject(cbProject *project)
{
    m_pTree->Delete(project->GetProjectNode());
}

wxTreeItemId ProjectManagerUI::GetTreeSelection()
{
    // User may have selected several items and right-clicked on one,
    // so return the right-click item instead in that case.
    if (m_RightClickItem.IsOk())
        return m_RightClickItem;

    wxArrayTreeItemIds selections;
    unsigned int sel = m_pTree->GetSelections(selections);

    if (sel)
        // Usually return the first item in the selection list.
        return selections[0];

    return wxTreeItemId();
}

void ProjectManagerUI::FinishLoadingProject(cbProject *project, bool newAddition, FilesGroupsAndMasks* fileGroups)
{
    if (newAddition)
    {
        ProjectManager *prjManager = Manager::Get()->GetProjectManager();
        project->BuildTree(m_pTree, m_TreeRoot, m_TreeVisualState, prjManager->GetFilesGroupsAndMasks());
    }
    else
        RebuildTree();
    m_pTree->Expand(project->GetProjectNode());
    m_pTree->Expand(m_TreeRoot); // make sure the root node is open
}

void ProjectManagerUI::FinishLoadingWorkspace(cbProject *activeProject, const wxString &workspaceTitle)
{
    RebuildTree();
    if (activeProject)
        m_pTree->Expand(activeProject->GetProjectNode());
    m_pTree->Expand(m_TreeRoot); // make sure the root node is open
    m_pTree->SetItemText(m_TreeRoot, workspaceTitle);

    UnfreezeTree(true);
}

void ProjectManagerUI::ShowFileInTree(ProjectFile &projectFile)
{
    // first unselect previous selected item if any, needed because of wxTR_MULTIPLE flag
    wxTreeItemId sel = GetTreeSelection();
    if (sel.IsOk())
        m_pTree->SelectItem(sel, false);

    const wxTreeItemId &itemId = projectFile.GetTreeItemId();
    if (itemId.IsOk())
    {
        m_pTree->EnsureVisible(itemId);
        m_pTree->SelectItem(itemId, true);
    }
}

void ProjectManagerUI::CreateMenu(wxMenuBar* menuBar)
{
/* TODO (mandrav#1#): Move menu items from main.cpp, here */
    if (menuBar)
    {
        int pos = menuBar->FindMenu(_("Sea&rch"));
        wxMenu* menu = menuBar->GetMenu(pos);
        if (menu)
            menu->Append(idMenuGotoFile, _("Goto file...\tAlt-G"));

        pos = menuBar->FindMenu(_("&File"));
        menu = menuBar->GetMenu(pos);
        if (menu)
        {
            menu->Insert(menu->GetMenuItemCount() - 1, idMenuFileProperties, _("Properties..."));
            menu->Insert(menu->GetMenuItemCount() - 1, wxID_SEPARATOR, _T("")); // instead of AppendSeparator();
        }

        pos = menuBar->FindMenu(_("&Project"));
        menu = menuBar->GetMenu(pos);
        if (menu)
        {
            if (menu->GetMenuItemCount())
                menu->AppendSeparator();
            menu->Append(idMenuAddFile,             _("Add files..."),             _("Add files to the project"));
            menu->Append(idMenuAddFilesRecursively, _("Add files recursively..."), _("Add files recursively to the project"));
            menu->Append(idMenuRemoveFile,          _("Remove files..."),          _("Remove files from the project"));

            menu->AppendSeparator();
            CreateMenuTreeProps(menu, false);

            menu->Append(idMenuExecParams,        _("Set &programs' arguments..."), _("Set execution parameters for the targets of this project"));
            menu->Append(idMenuProjectNotes,      _("Notes..."));
            menu->Append(idMenuProjectProperties, _("Properties..."));
        }
    }
}

void ProjectManagerUI::CreateMenuTreeProps(wxMenu* menu, bool popup)
{
    wxMenu* treeprops = new wxMenu;
    treeprops->Append(idMenuProjectUp,   _("Move project up\tCtrl-Shift-Up"),
                     _("Move project up in project tree"));
    treeprops->Append(idMenuProjectDown, _("Move project down\tCtrl-Shift-Down"),
                     _("Move project down in project tree"));

    treeprops->AppendSeparator();

    treeprops->Append(idMenuPriorProject, _("Activate prior project\tAlt-F5"),
                     _("Activate prior project in open projects list"));
    treeprops->Append(idMenuNextProject,  _("Activate next project\tAlt-F6"),
                     _("Activate next project in open projects list"));

    treeprops->AppendSeparator();

    treeprops->AppendCheckItem((popup ? idMenuViewCategorizePopup     : idMenuViewCategorize),
                              _("Categorize by file types"));
    treeprops->AppendCheckItem((popup ? idMenuViewUseFoldersPopup     : idMenuViewUseFolders),
                              _("Display folders as on disk"));
    treeprops->AppendCheckItem((popup ? idMenuViewHideFolderNamePopup : idMenuViewHideFolderName),
                              _("Hide folder name"));

    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("project_manager"));
    bool do_categorise       = cfg->ReadBool(_T("/categorize_tree"),  true);
    bool do_use_folders      = cfg->ReadBool(_T("/use_folders"),      true);
    bool do_hide_folder_name = !do_use_folders && cfg->ReadBool(_T("/hide_folder_name"), false); // "use folders" has precedence
    cfg->Write(_T("/hide_folder_name"), do_hide_folder_name); // make sure that configuration is consistent

    treeprops->Check((popup ? idMenuViewCategorizePopup     : idMenuViewCategorize),     do_categorise);
    treeprops->Check((popup ? idMenuViewUseFoldersPopup     : idMenuViewUseFolders),     do_use_folders);
    treeprops->Check((popup ? idMenuViewHideFolderNamePopup : idMenuViewHideFolderName), do_hide_folder_name);

    treeprops->Enable((popup ? idMenuViewUseFoldersPopup     : idMenuViewUseFolders),     !do_hide_folder_name);
    treeprops->Enable((popup ? idMenuViewHideFolderNamePopup : idMenuViewHideFolderName), !do_use_folders);

    treeprops->Append(idMenuViewFileMasks, _("Edit file types && categories..."));

    menu->Append(idMenuProjectTreeProps, _("Project tree"), treeprops);
}

void ProjectManagerUI::ShowMenu(wxTreeItemId id, const wxPoint& pt)
{
    if ( !id.IsOk() )
        return;
    ProjectManager *prjManager = Manager::Get()->GetProjectManager();

    wxString caption;
    wxMenu menu;

    FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(id);
    bool is_vfolder = ftd && ftd->GetKind() == FileTreeData::ftdkVirtualFolder;
    /* Following code will check for currently compiling project.
    *  If it finds the selected is project is currently compiling,
    *  then it will disable some of the options */
    bool PopUpMenuOption = true;
    ProjectsArray* Projects = Manager::Get()->GetProjectManager()->GetProjects();
    if (   Projects && ftd
        && (   ftd->GetKind() == FileTreeData::ftdkProject
            || ftd->GetKind() == FileTreeData::ftdkFile
            || ftd->GetKind() == FileTreeData::ftdkFolder ) )
    {
        int Count = Projects->GetCount();
        cbProject* ProjInTree = ftd->GetProject();
        for (int i = 0; i < Count; ++i)
        {
            cbProject* CurProject = Projects->Item(i);
            if (ProjInTree->GetTitle().IsSameAs(CurProject->GetTitle()))
            {
                if (CurProject->GetCurrentlyCompilingTarget())
                {
                    PopUpMenuOption = false;
                    break;
                }
            }
        }
    }

    // if it is not the workspace, add some more options
    if (ftd)
    {
        // if it is a project...
        if (ftd->GetKind() == FileTreeData::ftdkProject)
        {
            if (ftd->GetProject() != prjManager->GetActiveProject())
                menu.Append(idMenuSetActiveProject,     _("Activate project"));
            menu.Append(idMenuSaveProject,              _("Save project"));
            menu.Enable(idMenuSaveProject, PopUpMenuOption);
            menu.Append(idMenuCloseProject,             _("Close project"));
            menu.Enable(idMenuCloseProject, PopUpMenuOption);
            menu.AppendSeparator();
            menu.Append(idMenuAddFilePopup,             _("Add files..."));
            menu.Enable(idMenuAddFilePopup, PopUpMenuOption);
            menu.Append(idMenuAddFilesRecursivelyPopup, _("Add files recursively..."));
            menu.Enable(idMenuAddFilesRecursivelyPopup, PopUpMenuOption);
            menu.Append(idMenuRemoveFile,               _("Remove files..."));
            menu.Enable(idMenuRemoveFile, PopUpMenuOption);
            menu.AppendSeparator();
            menu.Append(idMenuFindFile,                 _("Find file..."));
            menu.Enable(idMenuFindFile, PopUpMenuOption);
            menu.AppendSeparator();
            CreateMenuTreeProps(&menu, true);
            menu.Append(idMenuAddVirtualFolder,         _("Add new virtual folder..."));
            if (is_vfolder)
                menu.Append(idMenuDeleteVirtualFolder,  _("Delete this virtual folder..."));
        }

        // if it is a file...
        else if (ftd->GetKind() == FileTreeData::ftdkFile)
        {
            // selected project file
            ProjectFile* pf = ftd->GetProjectFile();
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
                caption.Printf(_("Save %s"), m_pTree->GetItemText(id).c_str());
                menu.Append(idMenuSaveFile, caption);
                caption.Printf(_("Close %s"), m_pTree->GetItemText(id).c_str());
                menu.Append(idMenuCloseFile, caption);
            }
            else
            {
                caption.Printf(_("Open %s"), m_pTree->GetItemText(id).c_str());
                menu.Append(idMenuOpenFile, caption);
            }

            // add "Open with" menu
            wxMenu* openWith = new wxMenu;
            PluginsArray mimes = Manager::Get()->GetPluginManager()->GetMimeOffers();
            for (unsigned int i = 0; i < mimes.GetCount() && i < MAX_OPEN_WITH_ITEMS; ++i)
            {
                cbMimePlugin* plugin = (cbMimePlugin*)mimes[i];
                if (plugin && plugin->CanHandleFile(m_pTree->GetItemText(id)))
                {
                    const PluginInfo* info = Manager::Get()->GetPluginManager()->GetPluginInfo(plugin);
                    openWith->Append(idOpenWith[i], info ? info->title : wxString(_("<Unknown plugin>")));
                }
            }
            openWith->AppendSeparator();
            openWith->Append(idOpenWithInternal, _("Internal editor"));
            menu.Append(wxID_ANY, _("Open with"), openWith);

            if (pf->GetFileState() == fvsNormal &&  !Manager::Get()->GetEditorManager()->IsOpen(pf->file.GetFullPath()))
            {
                menu.AppendSeparator();
                menu.Append(idMenuRenameFile,  _("Rename file..."));
                menu.Enable(idMenuRenameFile, PopUpMenuOption);
            }
            menu.AppendSeparator();
            menu.Append(idMenuRemoveFilePopup, _("Remove file from project"));
            menu.Enable(idMenuRemoveFilePopup, PopUpMenuOption);
        }

        // if it is a folder...
        else if (ftd->GetKind() == FileTreeData::ftdkFolder)
        {
            menu.Append(idMenuAddFilePopup,             _("Add files..."));
            menu.Enable(idMenuAddFilePopup, PopUpMenuOption);
            menu.Append(idMenuAddFilesRecursivelyPopup, _("Add files recursively..."));
            menu.Enable(idMenuAddFilesRecursivelyPopup, PopUpMenuOption);
            menu.AppendSeparator();
            menu.Append(idMenuRemoveFile,               _("Remove files..."));
            menu.Enable(idMenuRemoveFile, PopUpMenuOption);
            menu.AppendSeparator();
            menu.Append(idMenuFindFile,                 _("Find file..."));
            menu.Enable(idMenuFindFile, PopUpMenuOption);
            menu.AppendSeparator();
            wxFileName f(ftd->GetFolder());
            f.MakeRelativeTo(ftd->GetProject()->GetCommonTopLevelPath());
            menu.Append(idMenuRemoveFolderFilesPopup, wxString::Format(_("Remove %s*"), f.GetFullPath().c_str()));
            menu.Enable(idMenuRemoveFolderFilesPopup, PopUpMenuOption);
            menu.Append(idMenuOpenFolderFilesPopup, wxString::Format(_("Open %s*"), f.GetFullPath().c_str()));
            menu.Enable(idMenuOpenFolderFilesPopup, PopUpMenuOption);
        }

        // if it is a virtual folder
        else if (is_vfolder)
        {
            menu.Append(idMenuAddVirtualFolder,    _("Add new virtual folder..."));
            menu.Append(idMenuDeleteVirtualFolder, _("Delete this virtual folder"));
            menu.AppendSeparator();
            menu.Append(idMenuRemoveFile, _("Remove files..."));
            menu.Append(idMenuRemoveFolderFilesPopup, wxString::Format(_("Remove %s*"), ftd->GetFolder().c_str()));
            menu.Append(idMenuOpenFolderFilesPopup, wxString::Format(_("Open %s*"), ftd->GetFolder().c_str()));
            menu.AppendSeparator();
            menu.Append(idMenuFindFile, _("Find file..."));
            menu.Enable(idMenuFindFile, PopUpMenuOption);
        }

        // if it is a virtual group (wild-card matching)
        else if (ftd->GetKind() == FileTreeData::ftdkVirtualGroup)
        {
            menu.Append(idMenuFindFile, _("Find file..."));
        }

        // ask any plugins to add items in this menu
        Manager::Get()->GetPluginManager()->AskPluginsForModuleMenu(mtProjectManager, &menu, ftd);

        // more project options
        if (ftd->GetKind() == FileTreeData::ftdkProject)
        {
            menu.Append(idMenuTreeProjectProperties, _("Properties..."));
            menu.Enable(idMenuTreeProjectProperties, PopUpMenuOption);
        }

        // more file options
        else if (ftd->GetKind() == FileTreeData::ftdkFile)
        {
            menu.AppendSeparator();
            menu.Append(idMenuTreeFileProperties, _("Properties..."));
        }
    }
    else if (!ftd && prjManager->GetWorkspace())
    {
        wxCommandEvent event;
        OnRightClick(event);
        return;
    }

    if (menu.GetMenuItemCount() != 0)
        m_pTree->PopupMenu(&menu, pt);
}

void ProjectManagerUI::DoOpenFile(ProjectFile* pf, const wxString& filename)
{
    // Basic stuff: We can only open files that are still present
    wxFileName the_file(filename);
    if (!the_file.FileExists())
    {
        wxString msg;
        msg.Printf(_("Could not open the file '%s'.\nThe file does not exist."), filename.c_str());
        cbMessageBox(msg, _("Error"));
        pf->SetFileState(fvsMissing);
        Manager::Get()->GetLogManager()->LogError(msg);
        return;
    }

    FileType ft = FileTypeOf(filename);
    if (ft == ftHeader || ft == ftSource)
    {
        // C/C++ header/source files, always get opened inside Code::Blocks
        cbEditor* ed = Manager::Get()->GetEditorManager()->Open(filename);
        if (ed)
        {
            ed->SetProjectFile(pf);
            ed->Activate();
        }
        else
        {
            wxString msg;
            msg.Printf(_("Failed to open '%s'."), filename.c_str());
            Manager::Get()->GetLogManager()->LogError(msg);
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
        if (!plugin)
        {
            wxString msg;
            msg.Printf(_("Could not open file '%s'.\nNo handler registered for this type of file."), filename.c_str());
            Manager::Get()->GetLogManager()->LogError(msg);
        }
        else if (plugin->OpenFile(filename) != 0)
        {
            const PluginInfo* info = Manager::Get()->GetPluginManager()->GetPluginInfo(plugin);
            wxString msg;
            msg.Printf(_("Could not open file '%s'.\nThe registered handler (%s) could not open it."), filename.c_str(), info ? info->title.c_str() : wxString(_("<Unknown plugin>")).c_str());
            Manager::Get()->GetLogManager()->LogError(msg);
        }
    }
}

void ProjectManagerUI::DoOpenSelectedFile()
{
    wxTreeItemId sel = GetTreeSelection();
    if (!sel.IsOk())
        return;

    FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel);
    if (ftd)
    {
        ProjectFile* f = ftd->GetProjectFile();
        if (f)
            DoOpenFile(f, f->file.GetFullPath());
    }
}

void ProjectManagerUI::RemoveFilesRecursively(wxTreeItemId& sel_id)
{
    wxTreeItemIdValue cookie;
    wxTreeItemId child;
    wxString filename;
    size_t i = 0;
    while (i < m_pTree->GetChildrenCount(sel_id))
    {
        if (i == 0)
            child = m_pTree->GetFirstChild(sel_id, cookie);
        else
            child = m_pTree->GetNextChild(sel_id, cookie);
        if (child.IsOk())
        {
            FileTreeData* data = (FileTreeData*)m_pTree->GetItemData(child);
            if (data)
            {
                cbProject* prj = data->GetProject();
                if (prj && data->GetKind() == FileTreeData::ftdkFile)
                {
                    ProjectFile* pf = data->GetProjectFile();

                    Manager::Get()->GetProjectManager()->RemoveFileFromProject(pf, prj);
                }
                else if (  data->GetKind() == FileTreeData::ftdkFolder
                        || data->GetKind() == FileTreeData::ftdkVirtualFolder)
                {
                    RemoveFilesRecursively(child);
                }
            }
            ++i;
        }
        else
            break;
    }
}

void ProjectManagerUI::OpenFilesRecursively(wxTreeItemId& sel_id)
{
    wxTreeItemIdValue cookie;
    wxTreeItemId child;
    wxString filename;
    size_t i = 0;
    while (i < m_pTree->GetChildrenCount(sel_id))
    {
        if (i == 0)
            child = m_pTree->GetFirstChild(sel_id, cookie);
        else
            child = m_pTree->GetNextChild(sel_id, cookie);
        if (child.IsOk())
        {
            FileTreeData* data = (FileTreeData*)m_pTree->GetItemData(child);
            if (data)
            {
                cbProject* prj = data->GetProject();
                if (prj && data->GetKind() == FileTreeData::ftdkFile)
                {
                    ProjectFile* pf = data->GetProjectFile();
                    DoOpenFile(pf, pf->file.GetFullPath());
                }
                else if (   data->GetKind() == FileTreeData::ftdkFolder
                         || data->GetKind() == FileTreeData::ftdkVirtualFolder )
                {
                    OpenFilesRecursively(child);
                }
            }
            ++i;
        }
        else
            break;
    }
}


// events

void ProjectManagerUI::OnTabContextMenu(cb_unused wxAuiNotebookEvent& event)
{
    wxMenu* NBmenu = new wxMenu();
    if (Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/project_tabs_bottom"), false))
        NBmenu->Append(idNB_TabTop, _("Tabs at top"));
    else
        NBmenu->Append(idNB_TabBottom, _("Tabs at bottom"));
    m_pNotebook->PopupMenu(NBmenu);
    delete NBmenu;
}

void ProjectManagerUI::OnTabPosition(wxCommandEvent& event)
{
    long style = m_pNotebook->GetWindowStyleFlag();
    style &= ~wxAUI_NB_BOTTOM;

    if (event.GetId() == idNB_TabBottom)
        style |= wxAUI_NB_BOTTOM;
    m_pNotebook->SetWindowStyleFlag(style);
    m_pNotebook->Refresh();
    // (style & wxAUI_NB_BOTTOM) saves info only about the the tabs position
    Manager::Get()->GetConfigManager(_T("app"))->Write(_T("/environment/project_tabs_bottom"), (bool)(style & wxAUI_NB_BOTTOM));
}

void ProjectManagerUI::OnTreeBeginDrag(wxTreeEvent& event)
{
    size_t count = m_pTree->GetSelections(m_DraggingSelection);
    for (size_t i = 0; i < count; i++)
    {
        //what item do we start dragging?
        wxTreeItemId id = m_DraggingSelection[i];

        if (!id.IsOk())
            return;

        // if no data associated with it, disallow
        FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(id);
        if (!ftd)
            return;

        // if no project, disallow
        cbProject* prj = ftd->GetProject();
        if (!prj)
            return;

        // allow only if the project approves
        if (!prj->CanDragNode(m_pTree, id))
            return;

    }

    // allowed
    event.Allow();
}

void ProjectManagerUI::OnTreeEndDrag(wxTreeEvent& event)
{
    wxTreeItemId to = event.GetItem();

    // is the drag target valid?
    if (!to.IsOk())
        return;

    // if no data associated with any of them, disallow
    FileTreeData* ftdTo = (FileTreeData*)m_pTree->GetItemData(to);
    if (!ftdTo)
        return;

    // if no project or different projects, disallow
    cbProject* prjTo = ftdTo->GetProject();
    if (!prjTo)
        return;

    size_t count = m_DraggingSelection.Count();
    for (size_t i = 0; i < count; i++)
    {
        wxTreeItemId from = m_DraggingSelection[i];

        // is the item valid?
        if (!from.IsOk())
            return;

        // if no data associated with any of them, disallow
        FileTreeData* ftdFrom = (FileTreeData*)m_pTree->GetItemData(from);
        if (!ftdFrom)
            return;

        // if no project or different projects, disallow
        cbProject* prjFrom = ftdTo->GetProject();
        if (prjFrom != prjTo)
            return;
    }

    // allow only if the project approves
    if (!prjTo->NodeDragged(m_pTree, m_DraggingSelection, to))
        return;

    event.Allow();
}

void ProjectManagerUI::OnProjectFileActivated(wxTreeEvent& event)
{
    wxTreeItemId id = event.GetItem();
    FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(id);
    ProjectManager *prjManager = Manager::Get()->GetProjectManager();

    if (ftd && ftd->GetKind() == FileTreeData::ftdkProject)
    {
        if (ftd->GetProject() != prjManager->GetActiveProject())
            prjManager->SetProject(ftd->GetProject(), false);

        // prevent item expand state toggle when project is activated
        // toggle it one time so that it is toggled back by wx
        m_pTree->IsExpanded(id) ? m_pTree->Collapse(id) : m_pTree->Expand(id);
    }
    else if (   ftd
             && (   (ftd->GetKind() == FileTreeData::ftdkVirtualGroup)
                 || (ftd->GetKind() == FileTreeData::ftdkVirtualFolder)
                 || (ftd->GetKind() == FileTreeData::ftdkFolder) ) )
        m_pTree->IsExpanded(id) ? m_pTree->Collapse(id) : m_pTree->Expand(id);
    else if (!ftd && prjManager->GetWorkspace())
        m_pTree->IsExpanded(m_TreeRoot) ? m_pTree->Collapse(m_TreeRoot) : m_pTree->Expand(m_TreeRoot);
    else
        DoOpenSelectedFile();
}

void ProjectManagerUI::OnExecParameters(cb_unused wxCommandEvent& event)
{
    if (Manager::Get()->GetProjectManager()->GetActiveProject())
        Manager::Get()->GetProjectManager()->GetActiveProject()->SelectTarget(-1, true);
}

void ProjectManagerUI::OnRightClick(cb_unused wxCommandEvent& event)
{
    ProjectManager *prjManager = Manager::Get()->GetProjectManager();

    wxMenu menu;
    if (prjManager->GetWorkspace())
    {
        menu.Append(idMenuTreeRenameWorkspace, _("Rename workspace..."));
        menu.AppendSeparator();
        menu.Append(idMenuTreeSaveWorkspace,   _("Save workspace"));
        menu.Append(idMenuTreeSaveAsWorkspace, _("Save workspace as..."));
        menu.AppendSeparator();
        menu.Append(idMenuFindFile,            _("Find file..."));
    }

    // ask any plugins to add items in this menu
    Manager::Get()->GetPluginManager()->AskPluginsForModuleMenu(mtProjectManager, &menu);

    // if plugins added to this menu, add a separator
    if (menu.GetMenuItemCount() != 0)
        menu.AppendSeparator();

    menu.AppendCheckItem(idMenuViewCategorizePopup,     _("Categorize by file types"));
    menu.AppendCheckItem(idMenuViewUseFoldersPopup,     _("Display folders as on disk"));
    menu.AppendCheckItem(idMenuViewHideFolderNamePopup, _("Hide folder name"));

    bool do_categorise       = (m_TreeVisualState&ptvsCategorize);
    bool do_use_folders      = (m_TreeVisualState&ptvsUseFolders);
    bool do_hide_folder_name = !do_use_folders && (m_TreeVisualState&ptvsHideFolderName); // "use folders" has precedence

    menu.Check(idMenuViewCategorizePopup,     do_categorise);
    menu.Check(idMenuViewUseFoldersPopup,     do_use_folders);
    menu.Check(idMenuViewHideFolderNamePopup, do_hide_folder_name);

    menu.Enable(idMenuViewUseFoldersPopup,     !do_hide_folder_name);
    menu.Enable(idMenuViewHideFolderNamePopup, !do_use_folders);

    menu.AppendSeparator();
    menu.Append(idMenuViewFileMasks, _("Edit file types && categories..."));

    if (prjManager->GetWorkspace())
    {
        // this menu items should be always the last one
        menu.AppendSeparator();
        menu.Append(idMenuTreeCloseWorkspace,  _("Close workspace"));
    }

    wxPoint pt = wxGetMousePosition();
    pt = m_pTree->ScreenToClient(pt);
    m_pTree->PopupMenu(&menu, pt);
}

void ProjectManagerUI::OnTreeItemRightClick(wxTreeEvent& event)
{
    if (Manager::Get()->GetProjectManager()->IsLoadingProject())
    {
        wxBell();
        return;
    }

    // We have a popup menu, so we will use the right-click item instead of the first tree selection.
    m_RightClickItem = event.GetItem();

    m_pTree->SelectItem(event.GetItem());
    ShowMenu(event.GetItem(), event.GetPoint());

    // Unset it so that we go back to using the first tree selection again.
    m_RightClickItem.Unset();
}

void ProjectManagerUI::OnRenameWorkspace(cb_unused wxCommandEvent& event)
{
    cbWorkspace *workspace = Manager::Get()->GetProjectManager()->GetWorkspace();
    if (workspace)
    {
        wxString text = wxGetTextFromUser(_("Please enter the new name for the workspace:"),
                                          _("Rename workspace"),
                                          workspace->GetTitle());
        if (!text.IsEmpty())
        {
            workspace->SetTitle(text);
            m_pTree->SetItemText(m_TreeRoot, workspace->GetTitle());
        }
    }
}

void ProjectManagerUI::OnSaveWorkspace(cb_unused wxCommandEvent& event)
{
    ProjectManager *prjManager = Manager::Get()->GetProjectManager();
    if (prjManager->GetWorkspace())
        prjManager->SaveWorkspace();
}

void ProjectManagerUI::OnSaveAsWorkspace(cb_unused wxCommandEvent& event)
{
    ProjectManager *prjManager = Manager::Get()->GetProjectManager();
    if (prjManager->GetWorkspace())
        prjManager->SaveWorkspaceAs(_T(""));
}

void ProjectManagerUI::OnCloseWorkspace(cb_unused wxCommandEvent& event)
{
    ProjectManager *prjManager = Manager::Get()->GetProjectManager();
    if (prjManager->GetWorkspace())
        prjManager->CloseWorkspace();
}

void ProjectManagerUI::OnSetActiveProject(wxCommandEvent& event)
{
    ProjectManager *prjManager = Manager::Get()->GetProjectManager();
    ProjectsArray *projects = prjManager->GetProjects();

    if (event.GetId() == idMenuSetActiveProject)
    {
        wxTreeItemId sel = GetTreeSelection();
        if (!sel.IsOk())
            return;

        FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel);
        if (!ftd)
            return;

        prjManager->SetProject(ftd->GetProject(), false);
    }
    else if (event.GetId() == idMenuPriorProject)
    {
        int index = projects->Index(prjManager->GetActiveProject());
        if (index == wxNOT_FOUND)
            return;
        --index;
        if (index < 0)
            index = projects->GetCount() - 1;
        prjManager->SetProject(projects->Item(index), false);
    }
    else if (event.GetId() == idMenuNextProject)
    {
        int index = projects->Index(prjManager->GetActiveProject());
        if (index == wxNOT_FOUND)
            return;
        ++index;
        if (index == (int)projects->GetCount())
            index = 0;
        prjManager->SetProject(projects->Item(index), false);
    }
    else if (event.GetId() == idMenuProjectUp)
    {
        wxTreeItemId sel = GetTreeSelection();
        if (!sel.IsOk())
            return;
        FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel);
        if (ftd)
            MoveProjectUp(ftd->GetProject());
    }
    else if (event.GetId() == idMenuProjectDown)
    {
        wxTreeItemId sel = GetTreeSelection();
        if (!sel.IsOk())
            return;

        FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel);
        if (ftd)
            MoveProjectDown(ftd->GetProject());
    }
}

void ProjectManagerUI::OnAddFilesToProjectRecursively(wxCommandEvent& event)
{
    ProjectManager *prjManager = Manager::Get()->GetProjectManager();
    cbProject* prj = nullptr;
    wxString basePath;

    if (event.GetId() == idMenuAddFilesRecursively)
    {
        prj = prjManager->GetActiveProject();
        if (prj)
            basePath = prj->GetBasePath();
    }
    else
    {
        wxTreeItemId sel = GetTreeSelection();
        if (!sel.IsOk())
            return;
        FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel);
        if (ftd)
        {
             prj = ftd->GetProject();
             if (prj)
             {
                 basePath = ftd->GetFolder();
                 if (!wxDirExists(basePath))
                     basePath = prj->GetBasePath();
             }
        }
    }
    if (!prj)
        return;

    wxString dir = ChooseDirectory(m_pTree,
                                   _("Add files recursively..."),
                                   basePath,
                                   wxEmptyString,
                                   false,
                                   false);
    if (dir.IsEmpty())
        return;

    wxArrayInt targets;
    // ask for target only if more than one
    if (prj->GetBuildTargetsCount() == 1)
         targets.Add(0);

    // generate list of files to add
    wxArrayString array;
    wxDir::GetAllFiles(dir, &array, wxEmptyString, wxDIR_FILES | wxDIR_DIRS);
    if (array.GetCount() == 0)
        return;

    // for usability reasons, remove any directory entries from the list...
    unsigned int i = 0;
    while (i < array.GetCount())
    {
        // discard directories, as well as some well known SCMs control folders ;)
        // also discard C::B project files
        if (wxDirExists(array[i]) ||
            array[i].Contains(_T("\\.svn\\")) ||
            array[i].Contains(_T("/.svn/")) ||
            array[i].Contains(_T("\\CVS\\")) ||
            array[i].Contains(_T("/CVS/")) ||
            array[i].Lower().Matches(_T("*.cbp")))
        {
            array.RemoveAt(i);
        }
        else
            ++i;
    }

    wxString wild;
    const FilesGroupsAndMasks *fileGroups = prjManager->GetFilesGroupsAndMasks();
    for (unsigned fm_idx = 0; fm_idx < fileGroups->GetGroupsCount(); fm_idx++)
        wild += fileGroups->GetFileMasks(fm_idx);

    MultiSelectDlg dlg(0, array, wild, _("Select the files to add to the project:"));
    PlaceWindow(&dlg);
    if (dlg.ShowModal() != wxID_OK)
        return;
    array = dlg.GetSelectedStrings();

    // finally add the files
    prjManager->AddMultipleFilesToProject(array, prj, targets);
    RebuildTree();
}

void ProjectManagerUI::OnAddFileToProject(wxCommandEvent& event)
{
    ProjectManager *prjManager = Manager::Get()->GetProjectManager();
    cbProject* prj = 0;
    wxString basePath;

    if (event.GetId() == idMenuAddFile)
    {
        prj = prjManager->GetActiveProject();
        if (prj)
            basePath = prj->GetBasePath();
    }
    else
    {
        wxTreeItemId sel = GetTreeSelection();
        if (!sel.IsOk())
            return;

        FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel);
        if (ftd)
        {
            prj = ftd->GetProject();
            if (prj)
            {
                basePath = ftd->GetFolder();
                if (!wxDirExists(basePath))
                    basePath = prj->GetBasePath();
            }
        }
    }
    if (!prj)
        return;

    wxFileDialog dlg(Manager::Get()->GetAppWindow(),
                    _("Add files to project..."),
                    basePath,
                    wxEmptyString,
                    FileFilters::GetFilterString(),
                    wxFD_OPEN | wxFD_MULTIPLE | wxFD_FILE_MUST_EXIST | compatibility::wxHideReadonly);
    dlg.SetFilterIndex(FileFilters::GetIndexForFilterAll());

    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxArrayInt targets;
        // ask for target only if more than one
        if (prj->GetBuildTargetsCount() == 1)
             targets.Add(0);

        wxArrayString array;
        dlg.GetPaths(array);
        prjManager->AddMultipleFilesToProject(array, prj, targets);
        RebuildTree();
    }
}

namespace
{
void FindFiles(wxArrayString &resultFiles, wxTreeCtrl &tree, wxTreeItemId item)
{
    FileTreeData* ftd = static_cast<FileTreeData*>(tree.GetItemData(item));
    switch (ftd->GetKind())
    {
        case FileTreeData::ftdkFile:
            resultFiles.Add(ftd->GetProjectFile()->relativeFilename);
            break;
        case FileTreeData::ftdkFolder:
            {
                wxTreeItemIdValue cookie;
                wxTreeItemId i = tree.GetFirstChild(item, cookie);
                while (i.IsOk())
                {
                    FindFiles(resultFiles, tree, i);
                    i = tree.GetNextChild(item, cookie);
                }
            }
            break;
        case FileTreeData::ftdkUndefined:     // fall-through
        case FileTreeData::ftdkProject:       // fall-through
        case FileTreeData::ftdkVirtualGroup:  // fall-through
        case FileTreeData::ftdkVirtualFolder: // fall-through
        default:
            for (FilesList::iterator it = ftd->GetProject()->GetFilesList().begin(); it != ftd->GetProject()->GetFilesList().end(); ++it)
                resultFiles.Add(((ProjectFile*)*it)->relativeFilename);
    }
}
} // namespace

void ProjectManagerUI::OnRemoveFileFromProject(wxCommandEvent& event)
{
    ProjectManager *prjManager = Manager::Get()->GetProjectManager();
    wxTreeItemId sel = GetTreeSelection();
    if (!sel.IsOk())
        return;

    FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel);
    if (!ftd)
        return;

    cbProject* prj = ftd->GetProject();
    if (!prj)
        return;

    wxString oldpath = prj->GetCommonTopLevelPath();

    if (event.GetId() == idMenuRemoveFile)
    {
        // remove multiple-files
        wxArrayString files;
        FindFiles(files, *m_pTree, sel);

        if (files.Count()==0)
        {
            cbMessageBox(_("This project does not contain any files to remove."),
                         _("Error"), wxICON_WARNING);
            return;
        }
        wxString msg;
        msg.Printf(_("Select files to remove from %s:"), prj->GetTitle().c_str());
        MultiSelectDlg dlg(0, files, true, msg);
        PlaceWindow(&dlg);
        if (dlg.ShowModal() == wxID_OK)
        {
            wxArrayInt indices = dlg.GetSelectedIndices();
            if (indices.GetCount() == 0)
                return;
            if (cbMessageBox(_("Are you sure you want to remove these files from the project?"),
                             _("Confirmation"),
                             wxICON_QUESTION | wxYES_NO | wxNO_DEFAULT) != wxID_YES)
            {
                return;
            }
            prj->BeginRemoveFiles();
            // we iterate the array backwards, because if we iterate it normally,
            // when we remove the first index, the rest becomes invalid...
            for (int i = (int)indices.GetCount() - 1; i >= 0; --i)
            {
                Manager::Get()->GetLogManager()->DebugLog(F(_T("Removing index %d"), indices[i]));

                ProjectFile* pf = prj->GetFileByFilename(files[indices[i]]);
                prjManager->RemoveFileFromProject(pf, prj);
            }
            prj->CalculateCommonTopLevelPath();
            prj->EndRemoveFiles();
            RebuildTree();
        }
    }
    else if (event.GetId() == idMenuRemoveFilePopup)
    {
        ProjectFile *prjfile = ftd->GetProjectFile();
        if (prjfile)
        {
            // remove single file
            prj->BeginRemoveFiles();
            prjManager->RemoveFileFromProject(prjfile, prj);
            prj->CalculateCommonTopLevelPath();
            if (prj->GetCommonTopLevelPath() == oldpath)
                m_pTree->Delete(sel);
            prj->EndRemoveFiles();
            RebuildTree();
        }
    }
    else if (event.GetId() == idMenuRemoveFolderFilesPopup)
    {
        // remove all files from a folder
        if (cbMessageBox(_("Are you sure you want to recursively remove from the project all the files under this folder?"),
                         _("Confirmation"),
                         wxICON_QUESTION | wxYES_NO | wxNO_DEFAULT) != wxID_YES)
        {
            return;
        }
        bool is_virtual = ftd->GetKind() == FileTreeData::ftdkVirtualFolder;
        if (is_virtual || ftd->GetKind() == FileTreeData::ftdkFolder)
        {
            prj->BeginRemoveFiles();
            RemoveFilesRecursively(sel);
            prj->EndRemoveFiles();
        }
        prj->CalculateCommonTopLevelPath();
        if (prj->GetCommonTopLevelPath() == oldpath && !is_virtual)
            m_pTree->Delete(sel);
        else if (is_virtual)
            prj->VirtualFolderDeleted(m_pTree, sel);
        RebuildTree();
    }
}

void ProjectManagerUI::OnSaveProject(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId sel = GetTreeSelection();
    if (!sel.IsOk())
        return;

    ProjectManager *prjManager = Manager::Get()->GetProjectManager();

    if (FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel))
    {
        if (cbProject* Project = ftd->GetProject())
        {
            // TODO : does it make sense TO not save project file while compiling ??
            if (prjManager->IsLoadingProject() || Project->GetCurrentlyCompilingTarget())
                wxBell();
            else
                prjManager->SaveProject(Project);
        }
    }
}

void ProjectManagerUI::OnCloseProject(wxCommandEvent& WXUNUSED(event))
{
    ProjectManager *prjManager = Manager::Get()->GetProjectManager();
    if (prjManager->IsLoadingProject())
    {
        wxBell();
        return;
    }

    wxArrayTreeItemIds selections;
    int count = m_pTree->GetSelections(selections);
    if (count == 0)
        return;
    std::set<cbProject*> projectsToClose;

    for (size_t ii = 0; ii < selections.GetCount(); ++ii)
    {
        FileTreeData *ftd = reinterpret_cast<FileTreeData*>(m_pTree->GetItemData(selections[ii]));
        if (ftd->GetKind() != FileTreeData::ftdkProject)
            continue;

        cbProject *project = ftd->GetProject();
        if (project)
        {
            if (project->GetCurrentlyCompilingTarget())
                wxBell();
            else
                projectsToClose.insert(project);
        }
    }

    for (std::set<cbProject*>::iterator it = projectsToClose.begin(); it != projectsToClose.end(); ++it)
        prjManager->CloseProject(*it);

    if (prjManager->GetProjects()->GetCount() > 0 && !prjManager->GetActiveProject())
        prjManager->SetProject(prjManager->GetProjects()->Item(0), false);

    Manager::Get()->GetAppWindow()->Refresh();
}

void ProjectManagerUI::OnSaveFile(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId sel = GetTreeSelection();
    if (!sel.IsOk())
        return;

    if (FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel))
    {
        if (ProjectFile* File = ftd->GetProjectFile())
            Manager::Get()->GetEditorManager()->Save(File->file.GetFullPath());
    }
}

void ProjectManagerUI::OnCloseFile(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId sel = GetTreeSelection();
    if (!sel.IsOk())
        return;

    if (FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel))
    {
        if (ProjectFile* File = ftd->GetProjectFile())
            Manager::Get()->GetEditorManager()->Close(File->file.GetFullPath());
    }
}

void ProjectManagerUI::OnOpenFile(wxCommandEvent& WXUNUSED(event))
{
    DoOpenSelectedFile();
}

void ProjectManagerUI::OnOpenFolderFiles(wxCommandEvent& event)
{
    wxTreeItemId sel = GetTreeSelection();
    FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel);
    if (!ftd)
        return;

    // open all files from a folder
    if (cbMessageBox(_("Are you sure you want to recursively open from the project all the files under this folder?"),
                     _("Confirmation"),
                     wxICON_QUESTION | wxYES_NO | wxNO_DEFAULT) != wxID_YES)
    {
        return;
    }

    if (   ftd->GetKind() == FileTreeData::ftdkFolder
        || ftd->GetKind() == FileTreeData::ftdkVirtualFolder )
    {
        OpenFilesRecursively(sel);
    }

    event.Skip();
}

void ProjectManagerUI::OnOpenWith(wxCommandEvent& event)
{
    wxTreeItemId sel = GetTreeSelection();
    if (!sel.IsOk())
        return;

    FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel);
    if (ftd)
    {
        ProjectFile* f = ftd->GetProjectFile();
        if (f)
        {
            wxString filename = f->file.GetFullPath();
            if (event.GetId() == idOpenWithInternal)
            {
                cbEditor* ed = Manager::Get()->GetEditorManager()->Open(filename);
                if (ed)
                {
                    ed->SetProjectFile(f);
                    ed->Show(true);
                    return;
                }
            }
            else
            {
                PluginsArray mimes = Manager::Get()->GetPluginManager()->GetMimeOffers();
                cbMimePlugin* plugin = (cbMimePlugin*)mimes[event.GetId() - idOpenWith[0]];
                if (plugin && plugin->OpenFile(filename) == 0)
                    return;
            }
            wxString msg;
            msg.Printf(_("Failed to open '%s'."), filename.c_str());
            Manager::Get()->GetLogManager()->LogError(msg);
        }
    }
}

void ProjectManagerUI::OnNotes(wxCommandEvent& WXUNUSED(event))
{
    cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
    if (project)
        project->ShowNotes(false, true);
}

void ProjectManagerUI::OnProperties(wxCommandEvent& event)
{
    ProjectManager *prjManager = Manager::Get()->GetProjectManager();
    cbProject *activeProject = prjManager->GetActiveProject();
    if (event.GetId() == idMenuProjectProperties)
    {
        wxString backupTitle = activeProject ? activeProject->GetTitle() : _T("");
        if (activeProject && activeProject->ShowOptions())
        {
            // make sure that cbEVT_PROJECT_ACTIVATE
            // is sent (maybe targets have changed)...
            // rebuild tree  only if title has changed
            prjManager->SetProject(activeProject, backupTitle != activeProject->GetTitle());
        }
    }
    else if (event.GetId() == idMenuTreeProjectProperties)
    {
        wxTreeItemId sel = GetTreeSelection();
        if (!sel.IsOk())
            return;

        FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel);

        cbProject* project = ftd ? ftd->GetProject() : activeProject;
        wxString backupTitle = project ? project->GetTitle() : _T("");
        if (project && project->ShowOptions() && project == activeProject)
        {
            // rebuild tree and make sure that cbEVT_PROJECT_ACTIVATE
            // is sent (maybe targets have changed)...
            // rebuild tree  only if title has changed
            prjManager->SetProject(project, backupTitle != project->GetTitle());
        }
        // if project title has changed, update the appropriate tab tooltips
        wxString newTitle = project->GetTitle();
        if (backupTitle != newTitle)
        {
            cbAuiNotebook* nb = Manager::Get()->GetEditorManager()->GetNotebook();
            if (nb)
            {
                wxString toolTip;
                for (size_t i = 0; i < nb->GetPageCount(); ++i)
                {
                    toolTip = nb->GetPageToolTip(i);
                    if (toolTip.EndsWith(_("Project: ") + backupTitle))
                    {
                        toolTip.Replace(_("Project: ") + backupTitle,_("Project: ") + newTitle);
                        nb->SetPageToolTip(i, toolTip);
                    }
                }
            }
        }
    }
    else if (event.GetId() == idMenuTreeFileProperties)
    {
        wxTreeItemId sel = GetTreeSelection();
        if (!sel.IsOk())
            return;

        FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel);

        cbProject* project = ftd ? ftd->GetProject() : activeProject;
        if (project)
        {
            if (ftd && ftd->GetFileIndex() != -1)
            {
                ProjectFile* pf = ftd->GetProjectFile();
                if (pf)
                    pf->ShowOptions(Manager::Get()->GetAppWindow());
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
                pf->ShowOptions(Manager::Get()->GetAppWindow());
            else
            {
                // active editor not-in-project
                ProjectFileOptionsDlg dlg(Manager::Get()->GetAppWindow(), ed->GetFilename());
                PlaceWindow(&dlg);
                dlg.ShowModal();
            }
        }
    }
}

struct ProjectFileRelativePathCmp
{
    ProjectFileRelativePathCmp(cbProject *pActiveProject) : m_pActiveProject(pActiveProject) {}
    bool operator()(ProjectFile* f1, ProjectFile* f2)
    {
        if (f1->GetParentProject() == m_pActiveProject && f2->GetParentProject() != m_pActiveProject)
            return true;
        else if (f1->GetParentProject() != m_pActiveProject && f2->GetParentProject() == m_pActiveProject)
            return false;
        else
            return f1->relativeFilename.Cmp(f2->relativeFilename) < 0;
    }
private:
    cbProject *m_pActiveProject;
};

struct ProjectFileAbsolutePathCmp
{
    bool operator()(ProjectFile* f1, ProjectFile* f2)
    { return f1->file.GetFullPath().Cmp(f2->file.GetFullPath()) < 0; }
};

struct ProjectFileAbsolutePathEqual
{
    bool operator()(ProjectFile* f1, ProjectFile* f2)
    { return f1->file.GetFullPath() == f2->file.GetFullPath(); }
};

void ProjectManagerUI::OnGotoFile(cb_unused wxCommandEvent& event)
{
    ProjectManager *prjManager = Manager::Get()->GetProjectManager();
    cbProject *activeProject = prjManager->GetActiveProject();

    if (!activeProject)
    {
        Manager::Get()->GetLogManager()->DebugLog(_T("No active project!"));
        return;
    }

    ProjectsArray *projects = prjManager->GetProjects();

    typedef std::vector<ProjectFile*> VProjectFiles;
    VProjectFiles pfiles;
    for (size_t prjIdx = 0; prjIdx < projects->GetCount(); ++prjIdx)
    {
        cbProject* project = (*projects)[prjIdx];
        if (!project) continue;

        for (FilesList::iterator it = project->GetFilesList().begin(); it != project->GetFilesList().end(); ++it)
            pfiles.push_back(*it);
    }

    if (!pfiles.empty())
    {
        std::sort(pfiles.begin(), pfiles.end(), ProjectFileAbsolutePathCmp());
        VProjectFiles::iterator last = std::unique(pfiles.begin(), pfiles.end(), ProjectFileAbsolutePathEqual());

        if (last != pfiles.end())
            pfiles.erase(last, pfiles.end());

        std::sort(pfiles.begin(), pfiles.end(), ProjectFileRelativePathCmp(activeProject));
    }

    class Iterator : public IncrementalSelectIterator
    {
        public:
            Iterator(VProjectFiles& pfiles, bool showProject) : m_PFiles(pfiles), m_ShowProject(showProject) {}
            virtual long GetCount() const              { return m_PFiles.size();                    }
            virtual wxString GetItem(long index) const { return m_PFiles[index]->relativeFilename; }
            virtual wxString GetDisplayItem(long index) const
            {
                if (m_ShowProject)
                {
                    ProjectFile *f = m_PFiles[index];
                    return f->relativeFilename + wxT(" (") + f->GetParentProject()->GetTitle() + wxT(")");
                }
                else
                    return m_PFiles[index]->relativeFilename;
            }
        private:
            VProjectFiles& m_PFiles;
            bool           m_ShowProject;
    };

    Iterator iterator(pfiles, projects->GetCount() > 1);
    IncrementalSelectListDlg dlg(Manager::Get()->GetAppWindow(), iterator,
                                 _("Select file..."), _("Please select file to open:"));
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        long selection = dlg.GetSelection();
        if (selection != -1)
            DoOpenFile(pfiles[selection], pfiles[selection]->file.GetFullPath());
    }
}

void ProjectManagerUI::OnViewCategorize(wxCommandEvent& event)
{
    bool do_categorise = event.IsChecked();

    if (do_categorise)
        m_TreeVisualState |=  ptvsCategorize;
    else
        m_TreeVisualState &= ~ptvsCategorize;

    Manager::Get()->GetAppFrame()->GetMenuBar()->Check(idMenuViewCategorize, do_categorise);
    Manager::Get()->GetConfigManager(_T("project_manager"))->Write(_T("/categorize_tree"), do_categorise);

    RebuildTree();
}

void ProjectManagerUI::OnViewUseFolders(wxCommandEvent& event)
{
    bool do_use_folders = event.IsChecked();

    if (do_use_folders)
        m_TreeVisualState |=  ptvsUseFolders;
    else
        m_TreeVisualState &= ~ptvsUseFolders;

    Manager::Get()->GetAppFrame()->GetMenuBar()->Check(idMenuViewUseFolders, do_use_folders);
    Manager::Get()->GetAppFrame()->GetMenuBar()->Enable(idMenuViewHideFolderName, !do_use_folders);
    Manager::Get()->GetConfigManager(_T("project_manager"))->Write(_T("/use_folders"), do_use_folders);

    // Do not create an invalid state
    if (do_use_folders)
    {
        m_TreeVisualState &= ~ptvsHideFolderName;
        Manager::Get()->GetAppFrame()->GetMenuBar()->Check(idMenuViewHideFolderName, false);
        Manager::Get()->GetConfigManager(_T("project_manager"))->Write(_T("/hide_folder_name"), false);
    }

    RebuildTree();
}

void ProjectManagerUI::OnViewHideFolderName(wxCommandEvent& event)
{
    bool do_hide_folder_name = event.IsChecked();

    if (do_hide_folder_name)
        m_TreeVisualState |=  ptvsHideFolderName;
    else
        m_TreeVisualState &= ~ptvsHideFolderName;

    Manager::Get()->GetAppFrame()->GetMenuBar()->Check(idMenuViewHideFolderName, do_hide_folder_name);
    Manager::Get()->GetAppFrame()->GetMenuBar()->Enable(idMenuViewUseFolders, !do_hide_folder_name);
    Manager::Get()->GetConfigManager(_T("project_manager"))->Write(_T("/hide_folder_name"), do_hide_folder_name);

    // Do not create an invalid state
    if (do_hide_folder_name)
    {
        m_TreeVisualState &= ~ptvsUseFolders;
        Manager::Get()->GetAppFrame()->GetMenuBar()->Check(idMenuViewUseFolders, false);
        Manager::Get()->GetConfigManager(_T("project_manager"))->Write(_T("/use_folders"), false);
    }

    RebuildTree();
}

void ProjectManagerUI::OnViewFileMasks(cb_unused wxCommandEvent& event)
{
    FilesGroupsAndMasks *fileGroups = Manager::Get()->GetProjectManager()->GetFilesGroupsAndMasks();
    ProjectsFileMasksDlg dlg(Manager::Get()->GetAppWindow(), fileGroups);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        fileGroups->Save();
        RebuildTree();
    }
}

wxArrayString ProjectManagerUI::ListNodes(wxTreeItemId node) const
{
    wxArrayString nodes;
    wxTreeItemIdValue cookie;
    wxTreeItemId item = m_pTree->GetFirstChild(node, cookie);
    while (item.IsOk())
    {
        nodes.Add(m_pTree->GetItemText(item));
        if (m_pTree->ItemHasChildren(item))
        {
            const wxArrayString& children = ListNodes(item);
            const wxString parent = nodes.Last();
            for (size_t i = 0; i < children.GetCount(); ++i)
                nodes.Add(parent + wxT("/") + children[i]);
        }
        item = m_pTree->GetNextChild(node, cookie);
    }
    return nodes;
}

void ProjectManagerUI::OnFindFile(cb_unused wxCommandEvent& event)
{
    wxTreeItemId sel = GetTreeSelection();
    if (!sel.IsOk())
        return;
    wxArrayString files = ListNodes(sel);
    if (files.IsEmpty())
        return;

    ProjectManager *prjManager = Manager::Get()->GetProjectManager();

    // workspace selected, add *.cbp filenames
    ConfigManagerContainer::StringToStringMap fileNameMap;
    if ( prjManager->GetWorkspace() && !(FileTreeData*)m_pTree->GetItemData(sel) )
    {
        for (size_t i = 0; i < prjManager->GetProjects()->GetCount(); ++i)
        {
            const cbProject* prj = prjManager->GetProjects()->Item(i);
            const wxFileName file(prj->GetFilename());
            files.Add(file.GetFullName());
            fileNameMap[file.GetFullName()] = prj->GetTitle();
        }
    }
    IncrementalSelectIteratorStringArray iter(files);
    IncrementalSelectListDlg dlg(Manager::Get()->GetAppWindow(), iter, _("Find file..."),
                                 _("Please enter the name of the file you are searching:"));
    wxListBox*     listBx  = XRCCTRL(dlg, "lstItems", wxListBox);
    wxCheckBox*    chkOpen = new wxCheckBox(&dlg, wxID_ANY, wxT("Open file"));
    ConfigManager* cfg     = Manager::Get()->GetConfigManager(wxT("project_manager"));
    chkOpen->SetValue(cfg->ReadBool(wxT("/find_file_open"), false));
    // insert the check box into the dialogue
    listBx->GetParent()->GetSizer()->Add(chkOpen, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_RIGHT, 8);
    dlg.Fit();
    dlg.SetMinSize(dlg.GetSize());
    PlaceWindow(&dlg);
    if (dlg.ShowModal() != wxID_OK)
        return;
    const long selection = dlg.GetSelection();
    if (selection == wxNOT_FOUND)
        return;

    wxString file = files[selection];
    ConfigManagerContainer::StringToStringMap::iterator it = fileNameMap.find(file);
    if (it != fileNameMap.end())
        file = it->second; // resolve .cbp project filename
    wxTreeItemIdValue cookie;
    wxTreeItemId item = m_pTree->GetFirstChild(sel, cookie);
    while (item.IsOk())
    {
        if (m_pTree->GetItemText(item) == file)
            break; // found it, exit
        else if (file.StartsWith(m_pTree->GetItemText(item) + wxT("/")))
        {
            // expand node
            file = file.Mid(m_pTree->GetItemText(item).Length() + 1);
            sel  = item;
            item = m_pTree->GetFirstChild(sel, cookie);
        }
        else // try next node
            item = m_pTree->GetNextChild(sel, cookie);
    }
    if (item.IsOk())
    {
        m_pTree->UnselectAll();
        m_pTree->SelectItem(item);
        const FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(item);
        if (ftd && chkOpen->IsChecked())
        {
            ProjectFile* pf = ftd->GetProjectFile();
            if (pf) // open the file
                DoOpenFile(pf, pf->file.GetFullPath());
            else if (   ftd->GetKind() == FileTreeData::ftdkProject
                     && ftd->GetProject() )
            {
                // change active project
                prjManager->SetProject(ftd->GetProject(), false);
            }
        }
        cfg->Write(wxT("/find_file_open"), chkOpen->IsChecked());
    }
    else
    {
        // error ?!
        // ... this should not fail (unless the tree was modified during selection)
    }
}

void ProjectManagerUI::OnAddVirtualFolder(cb_unused wxCommandEvent& event)
{
    wxString fld = wxGetTextFromUser(_("Please enter the new virtual folder path:"), _("New virtual folder"));
    if (fld.IsEmpty())
        return;

    wxTreeItemId sel = GetTreeSelection();
    if (!sel.IsOk())
        return;

    FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel);
    if (!ftd)
        return;

    cbProject* prj = ftd->GetProject();
    if (!prj)
        return;

    prj->VirtualFolderAdded(m_pTree, sel, fld);
//    RebuildTree();
}

void ProjectManagerUI::OnDeleteVirtualFolder(cb_unused wxCommandEvent& event)
{
    wxTreeItemId sel = GetTreeSelection();
    if (!sel.IsOk())
        return;

    FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel);
    if (!ftd)
        return;

    cbProject* prj = ftd->GetProject();
    if (!prj)
        return;

    prj->VirtualFolderDeleted(m_pTree, sel);
    RebuildTree();
}

void ProjectManagerUI::OnBeginEditNode(wxTreeEvent& event)
{
    // what item do we start editing?
    wxTreeItemId id = event.GetItem();
    if (!id.IsOk())
    {
        event.Veto();
        return;
    }

    // if no data associated with it, disallow
    FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(id);
    if (!ftd)
    {
        event.Veto();
        return;
    }

    // only allow editing virtual folders
    if (ftd->GetKind() != FileTreeData::ftdkVirtualFolder)
        event.Veto();
}

void ProjectManagerUI::OnEndEditNode(wxTreeEvent& event)
{
    FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(event.GetItem());
    if (!ftd)
    {
        event.Veto();
        return;
    }
    cbProject* prj = ftd->GetProject();
    if (!prj)
    {
        event.Veto();
        return;
    }

    if (!prj->VirtualFolderRenamed(m_pTree, event.GetItem(), event.GetLabel()))
        event.Veto();
//    RebuildTree();
}

void ProjectManagerUI::OnUpdateUI(wxUpdateUIEvent& event)
{
    event.Skip();
}

void ProjectManagerUI::OnIdle(wxIdleEvent& event)
{
    event.Skip();
}

void ProjectManagerUI::OnRenameFile(cb_unused wxCommandEvent& event)
{
    wxTreeItemId sel = GetTreeSelection();
    if (!sel.IsOk())
        return;

    FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel);
    if (!ftd)
        return;

    cbProject* prj = ftd->GetProject();
    if (!prj)
        return;

    if (ftd->GetProjectFile()->AutoGeneratedBy())
    {
        cbMessageBox(_("Can't rename file because it is auto-generated..."), _("Error"));
        return;
    }

    wxString path = ftd->GetProjectFile()->file.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    wxString name = ftd->GetProjectFile()->file.GetFullName();

    wxTextEntryDialog dlg(Manager::Get()->GetAppWindow(), _T("Please enter the new name:"), _T("Rename file"), name, wxOK | wxCANCEL | wxCENTRE);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxFileName fn(dlg.GetValue());
        wxString new_name = fn.GetFullName();

        if (name != new_name)
        {
        #ifdef __WXMSW__
            // only overwrite files, if the names are the same, but with different cases
            if (!wxRenameFile(path + name, path + new_name, (name.Lower() == new_name.Lower())))
        #else
            if (!wxRenameFile(path + name, path + new_name, false))
        #endif
            {
                wxBell();
                return;
            }
            ProjectFile *pf = ftd->GetProjectFile();
            pf->Rename(new_name);
            RebuildTree();
        }
    }
}

void ProjectManagerUI::OnKeyDown(wxTreeEvent& event)
{
    const wxKeyEvent& key_event = event.GetKeyEvent();

    cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
    if (    project
        && (project->GetCurrentlyCompilingTarget() == 0)
        && (   key_event.GetKeyCode() == WXK_DELETE
            || key_event.GetKeyCode() == WXK_NUMPAD_DELETE ) )
    {
        wxCommandEvent command(0, idMenuRemoveFilePopup);
        OnRemoveFileFromProject(command);
    }
    else
        event.Skip();
}

void ProjectManagerUI::MoveProjectUp(cbProject* project, bool warpAround)
{
    if (!project)
        return;
    ProjectManager *prjManager = Manager::Get()->GetProjectManager();
    ProjectsArray *projects = prjManager->GetProjects();

    int idx = projects->Index(project);
    if (idx == wxNOT_FOUND)
        return; // project not opened in project manager???

    if (idx == 0)
    {
         if (!warpAround)
            return;
        else
            idx = projects->Count();
    }
    projects->RemoveAt(idx--);
    projects->Insert(project, idx);
    RebuildTree();
    if (prjManager->GetWorkspace())
        prjManager->GetWorkspace()->SetModified(true);

    // re-select the project
    wxTreeItemId itemId = project->GetProjectNode();
    cbAssert(itemId.IsOk());
    m_pTree->SelectItem(itemId);
}

void ProjectManagerUI::MoveProjectDown(cbProject* project, bool warpAround)
{
    if (!project)
        return;
    ProjectManager *prjManager = Manager::Get()->GetProjectManager();
    ProjectsArray *projects = prjManager->GetProjects();

    int idx = projects->Index(project);
    if (idx == wxNOT_FOUND)
        return; // project not opened in project manager???

    if (idx == (int)projects->Count() - 1)
    {
         if (!warpAround)
            return;
        else
            idx = 0;
    }
    projects->RemoveAt(idx++);
    projects->Insert(project, idx);
    RebuildTree();
    if (prjManager->GetWorkspace())
        prjManager->GetWorkspace()->SetModified(true);

    // re-select the project
    wxTreeItemId itemId = project->GetProjectNode();
    cbAssert(itemId.IsOk());
    m_pTree->SelectItem(itemId);
}


bool ProjectManagerUI::QueryCloseAllProjects()
{
    if (!Manager::Get()->GetEditorManager()->QueryCloseAll())
        return false;
    ProjectsArray *projects = Manager::Get()->GetProjectManager()->GetProjects();
    for (size_t i = 0; i < projects->GetCount(); ++i)
    {
        // Ask for saving modified projects. However,
        // we already asked to save projects' files;
        // do not ask again
        if (!QueryCloseProject((*projects)[i], true))
            return false;
    }
    return true;
}

bool ProjectManagerUI::QueryCloseProject(cbProject *proj, bool dontsavefiles)
{
    if (!proj)
        return true;
    if (proj->GetCurrentlyCompilingTarget())
        return false;
    if (!dontsavefiles)
    {
        if (!proj->QueryCloseAllFiles())
            return false;
    }
    if (proj->GetModified() && !Manager::IsBatchBuild())
    {
        wxString msg;
        msg.Printf(_("Project '%s' is modified...\nDo you want to save the changes?"), proj->GetTitle().c_str());
        switch (cbMessageBox(msg, _("Save project"), wxICON_QUESTION | wxYES_NO | wxCANCEL))
        {
            case wxID_YES:
                if (!proj->Save())
                    return false;
            case wxID_NO:
                break;
            case wxID_CANCEL: // fall-through
            default:
                return false;
        }
    }
    return true;
}

bool ProjectManagerUI::QueryCloseWorkspace()
{
    cbWorkspace *workspace = Manager::Get()->GetProjectManager()->GetWorkspace();
    if (!workspace)
        return true;

    // don't ask to save the default workspace, if blank workspace is used on app startup
    if (workspace->IsDefault() && Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/blank_workspace"), true) == true)
        return true;

    if (workspace->GetModified())
    {
        // workspace needs save
        wxString msg;
        msg.Printf(_("Workspace '%s' is modified. Do you want to save it?"), workspace->GetTitle().c_str());
        switch (cbMessageBox(msg, _("Save workspace"),
                             wxYES_NO | wxCANCEL | wxICON_QUESTION))
        {
            case wxID_YES:
                Manager::Get()->GetProjectManager()->SaveWorkspace();
                break;
            case wxID_CANCEL:
                return false;
            default:
                break;
        }
    }
    if (!QueryCloseAllProjects())
        return false;
    return true;
}

int ProjectManagerUI::AskForBuildTargetIndex(cbProject* project)
{
    cbProject* prj = project;
    if (!prj)
        prj = Manager::Get()->GetProjectManager()->GetActiveProject();
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

wxArrayInt ProjectManagerUI::AskForMultiBuildTargetIndex(cbProject* project)
{
    wxArrayInt indices;
    cbProject* prj = project;
    if (!prj)
        prj = Manager::Get()->GetProjectManager()->GetActiveProject();
    if (!prj)
        return indices;

    // ask for target
    wxArrayString array;
    int count = prj->GetBuildTargetsCount();
    for (int i = 0; i < count; ++i)
        array.Add(prj->GetBuildTarget(i)->GetTitle());

    MultiSelectDlg dlg(0, array, true, _("Select the targets this file should belong to:"));
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
        indices = dlg.GetSelectedIndices();

    return indices;
}

void ProjectManagerUI::ConfigureProjectDependencies(cbProject* base)
{
    ProjectDepsDlg dlg(Manager::Get()->GetAppWindow(), base);
    PlaceWindow(&dlg);
    dlg.ShowModal();
}

void ProjectManagerUI::CheckForExternallyModifiedProjects()
{
    if (Manager::IsBatchBuild())
        return;
    if (m_isCheckingForExternallyModifiedProjects) // for some reason, a mutex locker does not work???
        return;
    m_isCheckingForExternallyModifiedProjects = true;

    // check also the projects (TO DO : what if we gonna reload while compiling/debugging)
    // TODO : make sure the same project is the active one again
    ProjectManager* ProjectMgr = Manager::Get()->GetProjectManager();
    if ( ProjectsArray* Projects = ProjectMgr->GetProjects())
    {
        bool reloadAll = false;
        // make a copy of all the pointers before we start messing with closing and opening projects
        // the hash (Projects) could change the order
        std::vector<cbProject*> ProjectPointers;
        for (unsigned int idxProject = 0; idxProject < Projects->Count(); ++idxProject)
            ProjectPointers.push_back(Projects->Item(idxProject));

        for (unsigned int idxProject = 0; idxProject < ProjectPointers.size(); ++idxProject)
        {
            cbProject* pProject = ProjectPointers[idxProject];
            wxFileName fname(pProject->GetFilename());
            wxDateTime last = fname.GetModificationTime();
            if (last.IsLaterThan(pProject->GetLastModificationTime()))
            {    // was modified -> reload
                int ret = -1;
                if (!reloadAll)
                {
                    Manager::Get()->GetLogManager()->Log(pProject->GetFilename());
                    wxString msg;
                    msg.Printf(_("Project %s is modified outside the IDE...\nDo you want to reload it (you will lose any unsaved work)?"),
                               pProject->GetFilename().c_str());
                    ConfirmReplaceDlg dlg(Manager::Get()->GetAppWindow(), false, msg);
                    dlg.SetTitle(_("Reload Project?"));
                    PlaceWindow(&dlg);
                    ret = dlg.ShowModal();
                    reloadAll = ret == crAll;
                }
                if (reloadAll || ret == crYes)
                    ProjectMgr->ReloadProject(pProject);
                else if (ret == crCancel)
                    break;
                else if (ret == crNo)
                    pProject->Touch();
            }
        } // end for : idx : idxProject
    }
    m_isCheckingForExternallyModifiedProjects = false;
}

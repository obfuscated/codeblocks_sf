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
    #include <wx/imaglist.h>
    #include <wx/frame.h>
    #include <wx/menu.h>
    #include <wx/splitter.h>
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
    #include <wx/dir.h>
    #include "globals.h"
    #include "cbexception.h"  // for cbassert
#endif

#include <vector>
#include <algorithm>

#include "cbauibook.h"
#include <wx/busyinfo.h>
#include <wx/choicdlg.h>
#include <wx/filedlg.h>
#include <wx/progdlg.h>
#include <wx/textdlg.h>
#include <wx/tokenzr.h>
#include <wx/utils.h>

#include "incrementalselectlistdlg.h"
#include "filegroupsandmasks.h"
#include "projectsfilemasksdlg.h"
#include "projectdepsdlg.h"
#include "multiselectdlg.h"
#include "filefilters.h"
#include "confirmreplacedlg.h"
#include "projectfileoptionsdlg.h"

template<> ProjectManager* Mgr<ProjectManager>::instance = 0;
template<> bool  Mgr<ProjectManager>::isShutdown = false;

// maximum number of items in "Open with" context menu
static const unsigned int MAX_OPEN_WITH_ITEMS = 20; // keep it in sync with below array!
static const int idOpenWith[] =
{
    wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(),
    wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(),
    wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(),
    wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId(),
};
// special entry: force open with internal editor
static const int idOpenWithInternal = wxNewId();

// static
bool ProjectManager::s_CanShutdown = true;

int ID_ProjectManager = wxNewId();
int idMenuSetActiveProject = wxNewId();
int idMenuOpenFile = wxNewId();
int idMenuSaveProject = wxNewId();
int idMenuSaveFile = wxNewId();
int idMenuCloseProject = wxNewId();
int idMenuCloseFile = wxNewId();
int idMenuAddFilePopup = wxNewId();
int idMenuAddFilesRecursivelyPopup = wxNewId();
int idMenuAddFile = wxNewId();
int idMenuAddFilesRecursively = wxNewId();
int idMenuRemoveFolderFilesPopup = wxNewId();
int idMenuOpenFolderFilesPopup = wxNewId();
int idMenuRemoveFilePopup = wxNewId();
int idMenuRemoveFile = wxNewId();
int idMenuRenameFile = wxNewId();
int idMenuProjectNotes = wxNewId();
int idMenuProjectProperties = wxNewId();
int idMenuFileProperties = wxNewId();
int idMenuTreeProjectProperties = wxNewId();
int idMenuTreeFileProperties = wxNewId();
int idMenuGotoFile = wxNewId();
int idMenuExecParams = wxNewId();
int idMenuViewCategorize = wxNewId();
int idMenuViewUseFolders = wxNewId();
int idMenuViewHideFolderName = wxNewId();
int idMenuViewFileMasks = wxNewId();
int idMenuNextProject = wxNewId();
int idMenuPriorProject = wxNewId();
int idMenuProjectTreeProps = wxNewId();
int idMenuProjectUp = wxNewId();
int idMenuProjectDown = wxNewId();
int idMenuViewCategorizePopup = wxNewId();
int idMenuViewUseFoldersPopup = wxNewId();
int idMenuViewHideFolderNamePopup = wxNewId();
int idMenuTreeRenameWorkspace = wxNewId();
int idMenuTreeSaveWorkspace = wxNewId();
int idMenuTreeSaveAsWorkspace = wxNewId();
int idMenuTreeCloseWorkspace = wxNewId();
int idMenuAddVirtualFolder = wxNewId();
int idMenuDeleteVirtualFolder = wxNewId();

static const int idMenuFindFile = wxNewId();
static const int idNB = wxNewId();
static const int idNB_TabTop = wxNewId();
static const int idNB_TabBottom = wxNewId();

cbTreeCtrl::cbTreeCtrl() : wxTreeCtrl()
{
    Compare = &filesSort;
}

cbTreeCtrl::cbTreeCtrl(wxWindow* parent, int id) :
    wxTreeCtrl(parent, id, wxDefaultPosition, wxDefaultSize,
               wxTR_EDIT_LABELS | wxTR_HAS_BUTTONS | wxTR_MULTIPLE | wxBORDER_NONE)
{
    Compare = &filesSort;
}

void cbTreeCtrl::SetCompareFunction(const int ptvs)
{
    // sort list of files
    if ( !(ptvs & ptvsUseFolders) && (ptvs & ptvsHideFolderName) )
        Compare = &filesSortNameOnly;
    else
        Compare = &filesSort;
}

#ifndef __WXMSW__
/*
    Under wxGTK, wxTreeCtrl is not sending an EVT_COMMAND_RIGHT_CLICK
    event when right-clicking on the client area.
*/
void cbTreeCtrl::OnRightClick(wxMouseEvent& event)
{
    if (!this) return;

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
#endif // !__WXMSW__
/*
    Under all platforms there is no reaction when pressing "ENTER".
    Expected would be e.g. to open the file in an editor.
*/
void cbTreeCtrl::OnKeyDown(wxKeyEvent& event)
{
    // Don't care about special key combinations
    if ( !this || (event.GetModifiers()!=wxMOD_NONE) )
    {
        event.Skip();
        return;
    }

    wxArrayTreeItemIds selections;
    unsigned int num = GetSelections(selections);
    // Don't care if no selection has been made
    if ( GetSelections(selections)<1 )
    {
        event.Skip();
        return;
    }

    long         keycode = event.GetKeyCode();
    wxTreeItemId itemId  = selections[0];
    // Don't care if item is invalid
    if ( !itemId.IsOk() )
    {
        event.Skip();
        return;
    }

    switch (keycode)
    {
        case WXK_RETURN:
        {
            wxTreeEvent te = wxTreeEvent(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, this, itemId);
            wxPostEvent(this, te);
            break;
        }
#ifndef __WXMSW__
/*
        Under wxGTK, there is no navigation possible using cursor keys.
*/
        case WXK_UP:
        {
            // GetPrevVisible - not implemented... strange...
            wxTreeItemId itemIdPrev = GetPrevSibling(itemId);
            if (itemIdPrev.IsOk())
            {
                SelectItem(itemId, false);
                SelectItem(itemIdPrev, true);
            }
            else
            {
                wxTreeItemId itemIdParent = GetItemParent(itemId);
                if (itemIdParent.IsOk())
                {
                    SelectItem(itemId, false);
                    SelectItem(itemIdParent, true);
                }
            }
            break;
        }
        case WXK_DOWN:
        {
            wxTreeItemId itemIdNext = GetNextVisible(itemId);
            if (itemIdNext.IsOk())
            {
                SelectItem(itemId, false);
                SelectItem(itemIdNext, true);
            }
            break;
        }
        case WXK_LEFT:
        {
            if ( ItemHasChildren(itemId) && IsExpanded(itemId) )
                Collapse(itemId);
            else
            {
                wxTreeItemId itemIdParent = GetItemParent(itemId);
                if (itemIdParent.IsOk())
                {
                    SelectItem(itemId, false);
                    SelectItem(itemIdParent, true);
                }
            }
            break;
        }
        case WXK_RIGHT:
        {
            if ( ItemHasChildren(itemId) && !IsExpanded(itemId) )
                Expand(itemId);
            else
            {
                wxTreeItemIdValue cookie;
                wxTreeItemId itemIdChild = GetFirstChild(itemId, cookie);
                if (itemIdChild.IsOk())
                {
                    SelectItem(itemId, false);
                    SelectItem(itemIdChild, true);
                }
            }
            break;
        }
#endif // !__WXMSW__
        default:
            event.Skip();
    }
}

int cbTreeCtrl::filesSort(const ProjectFile* arg1, const ProjectFile* arg2)
{
    if (arg1 && arg2)
        return wxStricmp(arg1->file.GetFullPath(), arg2->file.GetFullPath());
    return 0;
}

int cbTreeCtrl::filesSortNameOnly(const ProjectFile* arg1, const ProjectFile* arg2)
{
    if (arg1 && arg2)
        return wxStricmp(arg1->file.GetFullName(), arg2->file.GetFullName());
    return 0;
}

int cbTreeCtrl::OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2)
{
    return Compare(((FileTreeData*)GetItemData(item1))->GetProjectFile(), ((FileTreeData*)GetItemData(item2))->GetProjectFile());
}
IMPLEMENT_DYNAMIC_CLASS(cbTreeCtrl, wxTreeCtrl)
BEGIN_EVENT_TABLE(cbTreeCtrl, wxTreeCtrl)
#ifndef __WXMSW__
    EVT_RIGHT_DOWN(cbTreeCtrl::OnRightClick)
#endif // !__WXMSW__
    EVT_KEY_DOWN(cbTreeCtrl::OnKeyDown)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(ProjectManager, wxEvtHandler)
    EVT_TREE_BEGIN_DRAG(ID_ProjectManager, ProjectManager::OnTreeBeginDrag)
    EVT_TREE_END_DRAG(ID_ProjectManager, ProjectManager::OnTreeEndDrag)

    EVT_TREE_BEGIN_LABEL_EDIT(ID_ProjectManager, ProjectManager::OnBeginEditNode)
    EVT_TREE_END_LABEL_EDIT(ID_ProjectManager, ProjectManager::OnEndEditNode)

    EVT_TREE_ITEM_ACTIVATED(ID_ProjectManager, ProjectManager::OnProjectFileActivated)
    EVT_TREE_ITEM_RIGHT_CLICK(ID_ProjectManager, ProjectManager::OnTreeItemRightClick)
    EVT_TREE_KEY_DOWN(ID_ProjectManager, ProjectManager::OnKeyDown)
    EVT_COMMAND_RIGHT_CLICK(ID_ProjectManager, ProjectManager::OnRightClick)

    EVT_AUINOTEBOOK_TAB_RIGHT_UP(idNB, ProjectManager::OnTabContextMenu)

    EVT_MENU_RANGE(idOpenWith[0], idOpenWith[MAX_OPEN_WITH_ITEMS - 1], ProjectManager::OnOpenWith)
    EVT_MENU(idOpenWithInternal, ProjectManager::OnOpenWith)
    EVT_MENU(idNB_TabTop, ProjectManager::OnTabPosition)
    EVT_MENU(idNB_TabBottom, ProjectManager::OnTabPosition)
    EVT_MENU(idMenuSetActiveProject, ProjectManager::OnSetActiveProject)
    EVT_MENU(idMenuNextProject, ProjectManager::OnSetActiveProject)
    EVT_MENU(idMenuPriorProject, ProjectManager::OnSetActiveProject)
    EVT_MENU(idMenuProjectUp, ProjectManager::OnSetActiveProject)
    EVT_MENU(idMenuProjectDown, ProjectManager::OnSetActiveProject)
    EVT_MENU(idMenuTreeRenameWorkspace, ProjectManager::OnRenameWorkspace)
    EVT_MENU(idMenuTreeSaveWorkspace, ProjectManager::OnSaveWorkspace)
    EVT_MENU(idMenuTreeSaveAsWorkspace, ProjectManager::OnSaveAsWorkspace)
    EVT_MENU(idMenuTreeCloseWorkspace, ProjectManager::OnCloseWorkspace)
    EVT_MENU(idMenuAddVirtualFolder, ProjectManager::OnAddVirtualFolder)
    EVT_MENU(idMenuDeleteVirtualFolder, ProjectManager::OnDeleteVirtualFolder)
    EVT_MENU(idMenuAddFile, ProjectManager::OnAddFileToProject)
    EVT_MENU(idMenuAddFilesRecursively, ProjectManager::OnAddFilesToProjectRecursively)
    EVT_MENU(idMenuRemoveFile, ProjectManager::OnRemoveFileFromProject)
    EVT_MENU(idMenuAddFilePopup, ProjectManager::OnAddFileToProject)
    EVT_MENU(idMenuAddFilesRecursivelyPopup, ProjectManager::OnAddFilesToProjectRecursively)
    EVT_MENU(idMenuRemoveFolderFilesPopup, ProjectManager::OnRemoveFileFromProject)
    EVT_MENU(idMenuOpenFolderFilesPopup, ProjectManager::OnOpenFolderFiles)
    EVT_MENU(idMenuRemoveFilePopup, ProjectManager::OnRemoveFileFromProject)
    EVT_MENU(idMenuRenameFile, ProjectManager::OnRenameFile)
    EVT_MENU(idMenuSaveProject, ProjectManager::OnSaveProject)
    EVT_MENU(idMenuSaveFile, ProjectManager::OnSaveFile)
    EVT_MENU(idMenuCloseProject, ProjectManager::OnCloseProject)
    EVT_MENU(idMenuCloseFile, ProjectManager::OnCloseFile)
    EVT_MENU(idMenuOpenFile, ProjectManager::OnOpenFile)
    EVT_MENU(idMenuProjectNotes, ProjectManager::OnNotes)
    EVT_MENU(idMenuProjectProperties, ProjectManager::OnProperties)
    EVT_MENU(idMenuFileProperties, ProjectManager::OnProperties)
    EVT_MENU(idMenuTreeProjectProperties, ProjectManager::OnProperties)
    EVT_MENU(idMenuTreeFileProperties, ProjectManager::OnProperties)
    EVT_MENU(idMenuGotoFile, ProjectManager::OnGotoFile)
    EVT_MENU(idMenuExecParams, ProjectManager::OnExecParameters)
    EVT_MENU(idMenuViewCategorize, ProjectManager::OnViewCategorize)
    EVT_MENU(idMenuViewUseFolders, ProjectManager::OnViewUseFolders)
    EVT_MENU(idMenuViewHideFolderName, ProjectManager::OnViewHideFolderName)
    EVT_MENU(idMenuViewCategorizePopup, ProjectManager::OnViewCategorize)
    EVT_MENU(idMenuViewUseFoldersPopup, ProjectManager::OnViewUseFolders)
    EVT_MENU(idMenuViewHideFolderNamePopup, ProjectManager::OnViewHideFolderName)
    EVT_MENU(idMenuViewFileMasks, ProjectManager::OnViewFileMasks)
    EVT_MENU(idMenuFindFile, ProjectManager::OnFindFile)
    EVT_IDLE(ProjectManager::OnIdle)
END_EVENT_TABLE()

// class constructor
ProjectManager::ProjectManager()
    : m_pTree(0),
    m_pWorkspace(0),
    m_TreeFreezeCounter(0),
    m_IsLoadingProject(false),
    m_IsLoadingWorkspace(false),
    m_IsClosingProject(false),
    m_IsClosingWorkspace(false),
    m_InitialDir(_T("")),
    m_isCheckingForExternallyModifiedProjects(false),
    m_CanSendWorkspaceChanged(false),
    m_RunningPlugin(NULL)
{
    m_pNotebook = new cbAuiNotebook(Manager::Get()->GetAppWindow(), idNB, wxDefaultPosition, wxDefaultSize, wxAUI_NB_WINDOWLIST_BUTTON);
    if (Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/project_tabs_bottom"), false))
        m_pNotebook->SetWindowStyleFlag(m_pNotebook->GetWindowStyleFlag() | wxAUI_NB_BOTTOM);

    m_InitialDir = wxFileName::GetCwd();
    m_pActiveProject = 0L;
    m_pProjectToActivate = 0L;
    m_pProjects = new ProjectsArray;
    m_pProjects->Clear();
    InitPane();

    m_pFileGroups = new FilesGroupsAndMasks;

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

    m_pNotebook->Destroy();
}

void ProjectManager::InitPane()
{
    if (Manager::isappShuttingDown())
        return;
    if (m_pTree)
        return;
    BuildTree();
    m_pNotebook->AddPage(m_pTree, _("Projects"));
}

int ProjectManager::WorkspaceIconIndex(bool read_only)
{
    if (read_only)
        return (int)fvsWorkspaceReadOnly;

    return (int)fvsWorkspace;
}

int ProjectManager::ProjectIconIndex(bool read_only)
{
    if (read_only)
        return (int)fvsProjectReadOnly;

    return (int)fvsProject;
}

int ProjectManager::FolderIconIndex()
{
    return (int)fvsFolder;
}

int ProjectManager::VirtualFolderIconIndex()
{
    return (int)fvsVirtualFolder;
}

void ProjectManager::BuildTree()
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

void ProjectManager::CreateMenu(wxMenuBar* menuBar)
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

void ProjectManager::CreateMenuTreeProps(wxMenu* menu, bool popup)
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

void ProjectManager::ReleaseMenu(wxMenuBar* /*menuBar*/)
{
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
        {
            m_pWorkspace->SetModified(true);
            activeProjectChanged = true;
        }
    }
    else
        return; // already active

    if (m_pActiveProject)
        m_pTree->SetItemBold(m_pActiveProject->GetProjectNode(), false);

    m_pActiveProject = project;
    if (m_pActiveProject)
    {
        wxTreeItemId tid = m_pActiveProject->GetProjectNode();
        if (tid)
            m_pTree->SetItemBold(m_pActiveProject->GetProjectNode(), true);
    }

    if(activeProjectChanged)
    {
        m_pWorkspace->ActiveProjectChanged();
    }

    if (refresh)
        RebuildTree();

    if (m_pActiveProject)
        m_pTree->EnsureVisible(m_pActiveProject->GetProjectNode());

    m_pTree->Refresh();

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
            if (ftd->GetProject() != m_pActiveProject)
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
    else if (!ftd && m_pWorkspace)
    {
        menu.Append(idMenuTreeRenameWorkspace, _("Rename workspace..."));
        menu.AppendSeparator();
        menu.Append(idMenuTreeSaveWorkspace,   _("Save workspace"));
        menu.Append(idMenuTreeSaveAsWorkspace, _("Save workspace as..."));
        menu.AppendSeparator();
        menu.Append(idMenuFindFile,            _("Find file..."));

        // ask any plugins to add items in this menu
        Manager::Get()->GetPluginManager()->AskPluginsForModuleMenu(mtProjectManager, &menu, NULL);

        // this menu items should be always the last one
        menu.AppendSeparator();
        menu.Append(idMenuTreeCloseWorkspace,  _("Close workspace"));
    }

    if (menu.GetMenuItemCount() != 0)
        m_pTree->PopupMenu(&menu, pt);
}

cbProject* ProjectManager::IsOpen(const wxString& filename)
{
    if (filename.IsEmpty())
        return 0L;
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
    return 0L;
}

wxMenu* ProjectManager::GetProjectMenu()
{
    wxMenu* result = 0L;
    do
    {
        wxFrame* frame = Manager::Get()->GetAppFrame();
        if (!frame)
            break;
        wxMenuBar* mb = frame->GetMenuBar();
        if (!mb)
            break;
        result = mb->GetMenu(mb->FindMenu(_("&Project")));
        break;
    } while (false);
    return result;
}

cbProject* ProjectManager::LoadProject(const wxString& filename, bool activateIt)
{
    cbProject* result = 0;
    if (!wxFileExists(filename) || !BeginLoadingProject())
    {
        return 0;
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
            EndLoadingProject(0);

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
    FreezeTree();

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

        RebuildTree();

        if (m_pWorkspace)
            m_pWorkspace->SetModified(workspaceModified);
    }

    UnfreezeTree();
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
                return 0;
            }
        }
        else
            return 0;
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

bool ProjectManager::QueryCloseAllProjects()
{
    unsigned int i;

    if (!Manager::Get()->GetEditorManager()->QueryCloseAll())
        return false;

    for (i=0; i<m_pProjects->GetCount(); i++)
    {
        // Ask for saving modified projects. However,
        // we already asked to save projects' files;
        // do not ask again
        if (!QueryCloseProject(m_pProjects->Item(i),true))
            return false;
    }
    return true;
}

bool ProjectManager::QueryCloseProject(cbProject *proj,bool dontsavefiles)
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
            case wxID_YES:    if (!proj->Save()) return false;
            case wxID_NO:     break;
            case wxID_CANCEL: return false;
        }
    }
    return true;
}

bool ProjectManager::CloseAllProjects(bool dontsave)
{
    if (!dontsave)
    {
        if (!QueryCloseAllProjects())
            return false;
    }

    FreezeTree();
    m_IsClosingProject = true;
    while (m_pProjects->GetCount() != 0)
    {
// Commented it by Heromyth
//        if (!CloseActiveProject(true))
        if (!CloseProject(m_pProjects->Item(0), true, false))
        {
            UnfreezeTree(true);
            m_IsClosingProject = false;
            return false;
        }
    }

    if (!Manager::IsAppShuttingDown())
        RebuildTree();
    UnfreezeTree(true);

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
         if (!QueryCloseProject(project))
            return false;
    }

    bool wasActive = project == m_pActiveProject;
    if (wasActive)
        m_pActiveProject = 0L;

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
        m_pTree->Delete(project->GetProjectNode());
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
        RebuildTree();
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
        RebuildTree();
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
    FreezeTree();
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
    UnfreezeTree(true);
    return count == prjCount;
}

void ProjectManager::MoveProjectUp(cbProject* project, bool warpAround)
{
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

    // re-select the project
    wxTreeItemId node = project->GetProjectNode();
    cbAssert(node.IsOk());
    m_pTree->SelectItem(node, true);
}

void ProjectManager::MoveProjectDown(cbProject* project, bool warpAround)
{
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

    // re-select the project
    wxTreeItemId node = project->GetProjectNode();
    cbAssert(node.IsOk());
    m_pTree->SelectItem(node, true);
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

bool ProjectManager::QueryCloseWorkspace()
{
    if (!m_pWorkspace)
        return true;

    // don't ask to save the default workspace, if blank workspace is used on app startup
    if (m_pWorkspace->IsDefault() && Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/blank_workspace"), true) == true)
        return true;

    if (m_pWorkspace->GetModified())
    {
        // workspace needs save
        wxString msg;
        msg.Printf(_("Workspace '%s' is modified. Do you want to save it?"), m_pWorkspace->GetTitle().c_str());
        switch (cbMessageBox(msg, _("Save workspace"),
                             wxYES_NO | wxCANCEL | wxICON_QUESTION))
        {
            case wxID_YES: SaveWorkspace(); break;
            case wxID_CANCEL: return false;
            default: break;
        }
    }
    if (!QueryCloseAllProjects())
        return false;
    return true;
}

bool ProjectManager::CloseWorkspace()
{

    bool result = false;
    m_IsClosingWorkspace = true;
    if (m_pWorkspace)
    {
        if (!QueryCloseWorkspace())
        {
            m_IsClosingWorkspace = false;
            return false;
        }
        if (!CloseAllProjects(false))
        {
            m_IsClosingWorkspace = false;
            return false;
        }

        delete m_pWorkspace;
        m_pWorkspace = 0;

        if (m_pTree)
        {
            m_pTree->SetItemText(m_TreeRoot, _("Workspace"));
            if (!Manager::IsAppShuttingDown())
                RebuildTree(); // update the workspace icon if required
        }
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

void ProjectManager::FreezeTree()
{
    if (!m_pTree)
        return;

    ++m_TreeFreezeCounter;
    m_pTree->Freeze();
}

void ProjectManager::UnfreezeTree(bool /*force*/)
{
    if (!m_pTree)
        return;

    if (m_TreeFreezeCounter)
    {
        --m_TreeFreezeCounter;
        m_pTree->Thaw();
    }
}

void ProjectManager::RebuildTree()
{
    if (Manager::isappShuttingDown() || Manager::IsBatchBuild()) // saves a lot of time at startup for large projects
        return;

    FreezeTree();
    int count = m_pProjects->GetCount();
    for (int i = 0; i < count; ++i)
    {
        cbProject* project = m_pProjects->Item(i);
        if (project)
            project->SaveTreeState(m_pTree);
    }
    m_pTree->DeleteAllItems();
    wxString title;
    bool read_only = false;
    if (m_pWorkspace)
    {
        title = m_pWorkspace->GetTitle();
        wxString ws_file = m_pWorkspace->GetFilename();
        read_only = (   !ws_file.IsEmpty()
                     &&  wxFile::Exists(ws_file.c_str())
                     && !wxFile::Access(ws_file.c_str(), wxFile::write) );
    }
    if (title.IsEmpty())
        title = _("Workspace");
    m_TreeRoot = m_pTree->AddRoot(title, WorkspaceIconIndex(read_only), WorkspaceIconIndex(read_only));
    for (int i = 0; i < count; ++i)
    {
        cbProject* project = m_pProjects->Item(i);
        if (project)
        {
            project->BuildTree(m_pTree, m_TreeRoot, m_TreeVisualState, m_pFileGroups);
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
            targets = AskForMultiBuildTargetIndex(project);
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

wxArrayInt ProjectManager::AskForMultiBuildTargetIndex(cbProject* project)
{
    wxArrayInt indices;
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

    MultiSelectDlg dlg(0, array, true, _("Select the targets this file should belong to:"));
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
        indices = dlg.GetSelectedIndices();

    return indices;
}

void ProjectManager::DoOpenFile(ProjectFile* pf, const wxString& filename)
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

void ProjectManager::DoOpenSelectedFile()
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

    ProjectsArray* arr = 0;
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
        #if wxCHECK_VERSION(2, 9, 0)
        Manager::Get()->GetLogManager()->DebugLog(F(_T("%s now depends on %s (%d deps)"), base->GetTitle().wx_str(), dependsOn->GetTitle().wx_str(), arr->GetCount()));
        #else
        Manager::Get()->GetLogManager()->DebugLog(F(_T("%s now depends on %s (%d deps)"), base->GetTitle().c_str(), dependsOn->GetTitle().c_str(), arr->GetCount()));
        #endif
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

    #if wxCHECK_VERSION(2, 9, 0)
    Manager::Get()->GetLogManager()->DebugLog(F(_T("%s now does not depend on %s (%d deps)"), base->GetTitle().wx_str(), doesNotDependOn->GetTitle().wx_str(), arr->GetCount()));
    #else
    Manager::Get()->GetLogManager()->DebugLog(F(_T("%s now does not depend on %s (%d deps)"), base->GetTitle().c_str(), doesNotDependOn->GetTitle().c_str(), arr->GetCount()));
    #endif
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
    #if wxCHECK_VERSION(2, 9, 0)
    Manager::Get()->GetLogManager()->DebugLog(F(_T("Removed %s from all deps"), base->GetTitle().wx_str()));
    #else
    Manager::Get()->GetLogManager()->DebugLog(F(_T("Removed %s from all deps"), base->GetTitle().c_str()));
    #endif
}

const ProjectsArray* ProjectManager::GetDependenciesForProject(cbProject* base)
{
    DepsMap::iterator it = m_ProjectDeps.find(base);
    if (it != m_ProjectDeps.end())
        return it->second;
    return 0;
}

void ProjectManager::ConfigureProjectDependencies(cbProject* base)
{
    ProjectDepsDlg dlg(Manager::Get()->GetAppWindow(), base);
    PlaceWindow(&dlg);
    dlg.ShowModal();
}

// events

void ProjectManager::OnTabContextMenu(wxAuiNotebookEvent& /*event*/)
{
    wxMenu* NBmenu = new wxMenu();
    NBmenu->Append(idNB_TabTop, _("Tabs at top"));
    NBmenu->Append(idNB_TabBottom, _("Tabs at bottom"));
    if (Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/project_tabs_bottom"), false))
        NBmenu->FindItem(idNB_TabBottom)->Enable(false);
    else
        NBmenu->FindItem(idNB_TabTop)->Enable(false);
    m_pNotebook->PopupMenu(NBmenu);
    delete NBmenu;
}

void ProjectManager::OnTabPosition(wxCommandEvent& event)
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

void ProjectManager::OnTreeBeginDrag(wxTreeEvent& event)
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

void ProjectManager::OnTreeEndDrag(wxTreeEvent& event)
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

void ProjectManager::OnProjectFileActivated(wxTreeEvent& event)
{
    wxTreeItemId id = event.GetItem();
    FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(id);
    if (ftd && ftd->GetKind() == FileTreeData::ftdkProject)
    {
        if (ftd->GetProject() != m_pActiveProject)
            SetProject(ftd->GetProject(), false);

        // prevent item expand state toggle when project is activated
        // toggle it one time so that it is toggled back by wx
        m_pTree->IsExpanded(id) ? m_pTree->Collapse(id) : m_pTree->Expand(id);
    }
    else if (   ftd
             && (   (ftd->GetKind() == FileTreeData::ftdkVirtualGroup)
                 || (ftd->GetKind() == FileTreeData::ftdkVirtualFolder)
                 || (ftd->GetKind() == FileTreeData::ftdkFolder) ) )
        m_pTree->IsExpanded(id) ? m_pTree->Collapse(id) : m_pTree->Expand(id);
    else if (!ftd && m_pWorkspace)
        m_pTree->IsExpanded(m_TreeRoot) ? m_pTree->Collapse(m_TreeRoot) : m_pTree->Expand(m_TreeRoot);
    else
        DoOpenSelectedFile();
}

void ProjectManager::OnExecParameters(wxCommandEvent& /*event*/)
{
    if (m_pActiveProject)
        m_pActiveProject->SelectTarget(-1, true);
}

void ProjectManager::OnRightClick(wxCommandEvent& /*event*/)
{
    //Manager::Get()->GetLogManager()->DebugLog("OnRightClick");

    wxMenu menu;

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

    wxPoint pt = wxGetMousePosition();
    pt = m_pTree->ScreenToClient(pt);
    m_pTree->PopupMenu(&menu, pt);
}

void ProjectManager::OnTreeItemRightClick(wxTreeEvent& event)
{
    if (m_IsLoadingProject)
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

wxTreeItemId ProjectManager::GetTreeSelection()
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

void ProjectManager::OnRenameWorkspace(wxCommandEvent& /*event*/)
{
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

void ProjectManager::OnSaveWorkspace(wxCommandEvent& /*event*/)
{
    if (m_pWorkspace)
        SaveWorkspace();
}

void ProjectManager::OnSaveAsWorkspace(wxCommandEvent& /*event*/)
{
    if (m_pWorkspace)
        SaveWorkspaceAs(_T(""));
}

void ProjectManager::OnCloseWorkspace(wxCommandEvent& /*event*/)
{
    if (m_pWorkspace)
        CloseWorkspace();
}

void ProjectManager::OnSetActiveProject(wxCommandEvent& event)
{
    if (event.GetId() == idMenuSetActiveProject)
    {
        wxTreeItemId sel = GetTreeSelection();
        if (!sel.IsOk())
            return;

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

void ProjectManager::OnAddFilesToProjectRecursively(wxCommandEvent& event)
{
    cbProject* prj = 0;
    wxString basePath;

    if (event.GetId() == idMenuAddFilesRecursively)
    {
        prj = GetActiveProject();
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
    for (unsigned i = 0; i < m_pFileGroups->GetGroupsCount(); i++)
        wild += m_pFileGroups->GetFileMasks(i);

    MultiSelectDlg dlg(0, array, wild, _("Select the files to add to the project:"));
    PlaceWindow(&dlg);
    if (dlg.ShowModal() != wxID_OK)
        return;
    array = dlg.GetSelectedStrings();

    // finally add the files
    AddMultipleFilesToProject(array, prj, targets);
    RebuildTree();
}

void ProjectManager::OnAddFileToProject(wxCommandEvent& event)
{
    cbProject* prj = 0;
    wxString basePath;

    if (event.GetId() == idMenuAddFile)
    {
        prj = GetActiveProject();
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
        AddMultipleFilesToProject(array, prj, targets);
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
        default:
            for (FilesList::iterator it = ftd->GetProject()->GetFilesList().begin(); it != ftd->GetProject()->GetFilesList().end(); ++it)
                resultFiles.Add(((ProjectFile*)*it)->relativeFilename);
    }
}
} // namespace

void ProjectManager::OnRemoveFileFromProject(wxCommandEvent& event)
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
                RemoveFileFromProject(pf, prj);
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
            RemoveFileFromProject(prjfile, prj);
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

void ProjectManager::OnSaveProject(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId sel = GetTreeSelection();
    if (!sel.IsOk())
        return;

    if (FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel))
    {
        if (cbProject* Project = ftd->GetProject())
        {
            // TODO : does it make sense TO not save project file while compiling ??
            if (m_IsLoadingProject || Project->GetCurrentlyCompilingTarget())
                wxBell();
            else
                SaveProject(Project);
        }
    }
}

void ProjectManager::OnCloseProject(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId sel = GetTreeSelection();
    if (!sel.IsOk())
        return;

    FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel);
    cbProject *proj = 0;
    if (ftd)
        proj = ftd->GetProject();

    if (proj)
    {
        if (m_IsLoadingProject || proj->GetCurrentlyCompilingTarget())
            wxBell();
        else
            CloseProject(proj);
    }

    if (m_pProjects->GetCount() > 0 && !m_pActiveProject)
        SetProject(m_pProjects->Item(0), false);

    Manager::Get()->GetAppWindow()->Refresh();
}

void ProjectManager::OnSaveFile(wxCommandEvent& WXUNUSED(event))
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

void ProjectManager::OnCloseFile(wxCommandEvent& WXUNUSED(event))
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

void ProjectManager::OnOpenFile(wxCommandEvent& WXUNUSED(event))
{
    DoOpenSelectedFile();
}

void ProjectManager::OnOpenFolderFiles(wxCommandEvent& event)
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

void ProjectManager::OnOpenWith(wxCommandEvent& event)
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

void ProjectManager::OnNotes(wxCommandEvent& WXUNUSED(event))
{
    cbProject* project = GetActiveProject();
    if (project)
        project->ShowNotes(false, true);
}

void ProjectManager::OnProperties(wxCommandEvent& event)
{
    if (event.GetId() == idMenuProjectProperties)
    {
        wxString backupTitle = m_pActiveProject ? m_pActiveProject->GetTitle() : _T("");
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
        wxTreeItemId sel = GetTreeSelection();
        if (!sel.IsOk())
            return;

        FileTreeData* ftd = (FileTreeData*)m_pTree->GetItemData(sel);

        cbProject* project = ftd ? ftd->GetProject() : m_pActiveProject;
        wxString backupTitle = project ? project->GetTitle() : _T("");
        if (project && project->ShowOptions() && project == m_pActiveProject)
        {
            // rebuild tree and make sure that cbEVT_PROJECT_ACTIVATE
            // is sent (maybe targets have changed)...
            // rebuild tree  only if title has changed
            SetProject(project, backupTitle != project->GetTitle());
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
                    toolTip = nb->GetPage(i)->GetName();
                    if (toolTip.EndsWith(_("Project: ") + backupTitle))
                    {
                        toolTip.Replace(_("Project: ") + backupTitle,_("Project: ") + newTitle);
                        nb->SetTabToolTip(nb->GetPage(i), toolTip);
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

        cbProject* project = ftd ? ftd->GetProject() : m_pActiveProject;
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

void ProjectManager::OnGotoFile(wxCommandEvent& /*event*/)
{
    if (!m_pActiveProject)
    {
        Manager::Get()->GetLogManager()->DebugLog(_T("No active project!"));
        return;
    }

    typedef std::vector<ProjectFile*> ProjectFiles;
    ProjectFiles files;
    for (size_t proj_index = 0; proj_index < m_pProjects->GetCount(); ++proj_index)
    {
        cbProject *project = (*m_pProjects)[proj_index];

        for (FilesList::iterator it = project->GetFilesList().begin(); it != project->GetFilesList().end(); ++it)
            files.push_back(*it);
    }

    if (!files.empty())
    {
        std::sort(files.begin(), files.end(), ProjectFileAbsolutePathCmp());
        ProjectFiles::iterator last = std::unique(files.begin(), files.end(), ProjectFileAbsolutePathEqual());

        if (last != files.end())
        {
            files.erase(last, files.end());
        }

        std::sort(files.begin(), files.end(), ProjectFileRelativePathCmp(m_pActiveProject));
    }

    class Iterator : public IncrementalSelectIterator
    {
        public:
            Iterator(ProjectFiles &files, bool showProject) : m_Files(files), m_ShowProject(showProject) {}
            virtual long GetCount() const { return m_Files.size(); }
            virtual wxString GetItem(long index) const
            {
                return m_Files[index]->relativeFilename;
            }
            virtual wxString GetDisplayItem(long index) const
            {
                if (m_ShowProject)
                {
                    ProjectFile *f = m_Files[index];
                    return f->relativeFilename + wxT(" (") + f->GetParentProject()->GetTitle() + wxT(")");
                }
                else
                    return m_Files[index]->relativeFilename;
            }
        private:
            ProjectFiles &m_Files;
            bool m_ShowProject;
    };

    Iterator iterator(files, m_pProjects->GetCount() > 1);
    IncrementalSelectListDlg dlg(Manager::Get()->GetAppWindow(), iterator,
                                 _("Select file..."), _("Please select file to open:"));
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        long selection = dlg.GetSelection();
        if (selection != -1)
            DoOpenFile(files[selection], files[selection]->file.GetFullPath());
    }
}

void ProjectManager::OnViewCategorize(wxCommandEvent& event)
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

void ProjectManager::OnViewUseFolders(wxCommandEvent& event)
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

void ProjectManager::OnViewHideFolderName(wxCommandEvent& event)
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

void ProjectManager::OnViewFileMasks(wxCommandEvent& /*event*/)
{
    ProjectsFileMasksDlg dlg(Manager::Get()->GetAppWindow(), m_pFileGroups);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        m_pFileGroups->Save();
        RebuildTree();
    }
}

wxTreeItemId ProjectManager::FindItem( wxTreeItemId Node, const wxString& Search) const
{
    wxTreeItemIdValue cookie;
    wxTreeItemId item = m_pTree->GetFirstChild(Node, cookie );
    while ( item.IsOk() )
    {
        if ( Search.IsSameAs( m_pTree->GetItemText( item ) ) )
            return item;
        else if ( m_pTree->ItemHasChildren( item ) )
        {
            wxTreeItemId SearchId = FindItem( item, Search );
            if ( SearchId.IsOk() )
                return SearchId;
        }

        item = m_pTree->GetNextChild(Node, cookie);
    } // end while
    wxTreeItemId notFound;
    return notFound;
}

void ProjectManager::OnFindFile(wxCommandEvent& /*event*/)
{
    const wxString text = wxGetTextFromUser(_("Please enter the name of the file you are searching:"), _("Find file..."));
    if ( !text.IsEmpty() )
    {
        wxTreeItemId sel = GetTreeSelection();
        if (!sel.IsOk())
            return;

        wxTreeItemId found = FindItem( sel, text );

        if ( found.IsOk() )
        {
            m_pTree->UnselectAll();
            m_pTree->SelectItem( found );
        }
        else
            cbMessageBox(_("The file couldn't be found!"), _(""), wxICON_INFORMATION);
    }
}

void ProjectManager::OnAddVirtualFolder(wxCommandEvent& /*event*/)
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

void ProjectManager::OnDeleteVirtualFolder(wxCommandEvent& /*event*/)
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

void ProjectManager::OnBeginEditNode(wxTreeEvent& event)
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

void ProjectManager::OnEndEditNode(wxTreeEvent& event)
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

void ProjectManager::OnUpdateUI(wxUpdateUIEvent& event)
{
    event.Skip();
}

void ProjectManager::OnIdle(wxIdleEvent& event)
{
    event.Skip();
}

void ProjectManager::OnAppDoneStartup(CodeBlocksEvent& event)
{
    // we do not send the workspace loaded event yet because: a) We don't know
    // if there's a workspace yet, and b) app.cpp hasn't finished init'ing yet.
    // We'll let app.cpp send the workspace changed for us when it's done.
    m_CanSendWorkspaceChanged = true;
    event.Skip();
}

void ProjectManager::OnRenameFile(wxCommandEvent& /*event*/)
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

void ProjectManager::CheckForExternallyModifiedProjects()
{
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
        {
            ProjectPointers.push_back(Projects->Item(idxProject));
        }
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

void ProjectManager::RemoveFilesRecursively(wxTreeItemId& sel_id)
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
                    RemoveFileFromProject(pf, prj);
                }
                else if (data->GetKind() == FileTreeData::ftdkFolder
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

void ProjectManager::OpenFilesRecursively(wxTreeItemId& sel_id)
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
        {
            if (newAddition)
                project->BuildTree(m_pTree, m_TreeRoot, m_TreeVisualState, m_pFileGroups);
            else
                RebuildTree();
            m_pTree->Expand(project->GetProjectNode());
            m_pTree->Expand(m_TreeRoot); // make sure the root node is open
        }

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

    FreezeTree();
    m_pTree->AppendItem(m_pTree->GetRootItem(), _("Loading workspace..."));
    m_pTree->Expand(m_pTree->GetRootItem());
    UnfreezeTree();

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
        RebuildTree();
        if (m_pActiveProject || m_pProjectToActivate)
        {
            if (m_pProjectToActivate)
            {
                SetProject(m_pProjectToActivate, true);
                m_pProjectToActivate = 0L;
            }
            m_pTree->Expand(m_pActiveProject->GetProjectNode());
        }
        m_pTree->Expand(m_TreeRoot); // make sure the root node is open
        m_pTree->SetItemText(m_TreeRoot, m_pWorkspace->GetTitle());

        UnfreezeTree(true);
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

void ProjectManager::OnKeyDown(wxTreeEvent& event)
{
    const wxKeyEvent& key_event = event.GetKeyEvent();

    cbProject* project = GetActiveProject();
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

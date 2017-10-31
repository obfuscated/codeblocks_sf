/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk.h"

#ifndef CB_PRECOMP
    #include <manager.h>
    #include <configmanager.h>
    #include <editormanager.h>
    #include <projectmanager.h>
    #include <logmanager.h>
    #include <cbeditor.h>
    #include <cbproject.h>
    #include <editorbase.h>
    #include <sdk_events.h>
    #include <misctreeitemdata.h>

    #include <wx/window.h>
    #include <wx/treectrl.h>
    #include <wx/bitmap.h>
    #include <wx/imaglist.h>
    #include <wx/menu.h>
#endif

#include "openfileslistplugin.h"
#include "tinyxml.h"
#include "tinywxuni.h"

namespace
{
    // this auto-registers the plugin
    PluginRegistrant<OpenFilesListPlugin> reg(_T("OpenFilesList"));

    const int idOpenFilesTree = wxNewId();
    const int idViewOpenFilesTree = wxNewId();
    const int idViewPreserveOpenEditors = wxNewId();

    class OpenFilesListData : public wxTreeItemData
    {
        public:
            OpenFilesListData(EditorBase* ed_in) : ed(ed_in) {}
            EditorBase* GetEditor() const { return ed; }
        private:
            EditorBase* ed;
    };
}

BEGIN_EVENT_TABLE(OpenFilesListPlugin, cbPlugin)
    EVT_UPDATE_UI_RANGE(idViewOpenFilesTree, idViewPreserveOpenEditors, OpenFilesListPlugin::OnUpdateUI)
    EVT_MENU(idViewOpenFilesTree, OpenFilesListPlugin::OnViewOpenFilesTree)
    EVT_MENU(idViewPreserveOpenEditors, OpenFilesListPlugin::OnViewPreserveOpenEditors)
    EVT_TREE_ITEM_ACTIVATED(idOpenFilesTree, OpenFilesListPlugin::OnTreeItemActivated)
    EVT_TREE_ITEM_RIGHT_CLICK(idOpenFilesTree, OpenFilesListPlugin::OnTreeItemRightClick)
END_EVENT_TABLE()

OpenFilesListPlugin::OpenFilesListPlugin()
{
    //ctor
}

OpenFilesListPlugin::~OpenFilesListPlugin()
{
    //dtor
}

void OpenFilesListPlugin::OnAttach()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("open_files_list"));
    if (cfg)
        m_PreserveOpenEditors = cfg->ReadBool(_T("preserve_open_editors"), false);
    m_ProjectLoading = false;
    m_pActiveProject = nullptr;
    m_ViewMenu = 0;

    m_EditorArray.Clear();

    // create tree
    m_pTree = new wxTreeCtrl(Manager::Get()->GetAppWindow(), idOpenFilesTree,wxDefaultPosition,wxSize(150, 100),
                            wxTR_HAS_BUTTONS | wxNO_BORDER | wxTR_HIDE_ROOT);

    // load bitmaps
    wxBitmap bmp;
    m_pImages = new wxImageList(16, 16);
    wxString prefix = ConfigManager::GetDataFolder() + _T("/images/");

    bmp = cbLoadBitmap(prefix + _T("folder_open.png"), wxBITMAP_TYPE_PNG); // folder
    m_pImages->Add(bmp);

    bmp = cbLoadBitmap(prefix + _T("ascii.png"), wxBITMAP_TYPE_PNG); // file
    m_pImages->Add(bmp);

    bmp = cbLoadBitmap(prefix + _T("modified_file.png"), wxBITMAP_TYPE_PNG); // modified file
    m_pImages->Add(bmp);

    bmp = cbLoadBitmap(prefix + _T("file-readonly.png"), wxBITMAP_TYPE_PNG); // read only file
    m_pImages->Add(bmp);

    m_pTree->SetImageList(m_pImages);
    m_pTree->AddRoot(_T("Opened Files"), 0, 0);

    // first build of the tree
    RebuildOpenFilesTree();

    // add the tree to the docking system
    CodeBlocksDockEvent evt(cbEVT_ADD_DOCK_WINDOW);
    evt.name = _T("OpenFilesPane");
    evt.title = _("Open files list");
    evt.pWindow = m_pTree;
    evt.minimumSize.Set(50, 50);
    evt.desiredSize.Set(150, 100);
    evt.floatingSize.Set(100, 150);
    evt.dockSide = CodeBlocksDockEvent::dsLeft;
    evt.stretch = true;
    Manager::Get()->ProcessEvent(evt);

    // register event sinks
    Manager* pm = Manager::Get();

    // basically everything editor related
    pm->RegisterEventSink(cbEVT_EDITOR_ACTIVATED, new cbEventFunctor<OpenFilesListPlugin, CodeBlocksEvent>(this, &OpenFilesListPlugin::OnEditorActivated));
    pm->RegisterEventSink(cbEVT_EDITOR_CLOSE, new cbEventFunctor<OpenFilesListPlugin, CodeBlocksEvent>(this, &OpenFilesListPlugin::OnEditorClosed));
    pm->RegisterEventSink(cbEVT_EDITOR_DEACTIVATED, new cbEventFunctor<OpenFilesListPlugin, CodeBlocksEvent>(this, &OpenFilesListPlugin::OnEditorDeactivated));
    pm->RegisterEventSink(cbEVT_EDITOR_MODIFIED, new cbEventFunctor<OpenFilesListPlugin, CodeBlocksEvent>(this, &OpenFilesListPlugin::OnEditorModified));
    pm->RegisterEventSink(cbEVT_EDITOR_OPEN, new cbEventFunctor<OpenFilesListPlugin, CodeBlocksEvent>(this, &OpenFilesListPlugin::OnEditorOpened));
    pm->RegisterEventSink(cbEVT_EDITOR_SAVE, new cbEventFunctor<OpenFilesListPlugin, CodeBlocksEvent>(this, &OpenFilesListPlugin::OnEditorSaved));
    pm->RegisterEventSink(cbEVT_PROJECT_ACTIVATE, new cbEventFunctor<OpenFilesListPlugin, CodeBlocksEvent>(this, &OpenFilesListPlugin::OnProjectActivated));
    pm->RegisterEventSink(cbEVT_PROJECT_OPEN, new cbEventFunctor<OpenFilesListPlugin, CodeBlocksEvent>(this, &OpenFilesListPlugin::OnProjectOpened));
    pm->RegisterEventSink(cbEVT_PROJECT_CLOSE, new cbEventFunctor<OpenFilesListPlugin, CodeBlocksEvent>(this, &OpenFilesListPlugin::OnProjectClosed));
    pm->RegisterEventSink(cbEVT_BUILDTARGET_SELECTED, new cbEventFunctor<OpenFilesListPlugin, CodeBlocksEvent>(this, &OpenFilesListPlugin::OnBuildTargetSelected));
}

void OpenFilesListPlugin::OnRelease(bool appShutDown)
{
    // Write config
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("open_files_list"));
    if (cfg)
        cfg->Write(_T("preserve_open_editors"), m_PreserveOpenEditors);

    if (appShutDown)
        return;
    // remove registered event sinks
    Manager::Get()->RemoveAllEventSinksFor(this);

    // remove tree from docking system
    CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
    evt.pWindow = m_pTree;
    Manager::Get()->ProcessEvent(evt);

    // finally destroy the tree
    m_pTree->Destroy();
    m_pTree = nullptr;
}

void OpenFilesListPlugin::BuildMenu(wxMenuBar* menuBar)
{
    // if not attached, exit
    if (!IsAttached())
        return;

    // add the open files list in the "View" menu
    int idx = menuBar->FindMenu(_("&View"));
    if (idx != wxNOT_FOUND)
    {
        m_ViewMenu = menuBar->GetMenu(idx);
        wxMenuItemList& items = m_ViewMenu->GetMenuItems();
        // find the first separator and insert before it
        for (size_t i = 0; i < items.GetCount(); ++i)
        {
            if (items[i]->IsSeparator())
            {
                m_ViewMenu->InsertCheckItem(i, idViewOpenFilesTree, _("&Open files list"), _("Toggle displaying the open files list"));
                m_ViewPreserveChk = m_ViewMenu->InsertCheckItem(i + 2, idViewPreserveOpenEditors, _("&Preserve open editors"), _("Preserve open editors per target/project.\n"
                                                                                                                                 "Only available the when the workspace is empty."));
                m_ViewPreserveChk->Check(m_PreserveOpenEditors);
                m_ViewMenu->InsertSeparator(i + 3);
                return;
            }
        }
        // not found, just append
        m_ViewMenu->AppendCheckItem(idViewOpenFilesTree, _("&Open files list"), _("Toggle displaying the open files list"));
        m_ViewMenu->AppendSeparator();
        m_ViewPreserveChk = m_ViewMenu->AppendCheckItem(idViewPreserveOpenEditors, _("&Preserve open editors"), _("Preserve open editors per target/project\n"
                                                                                                                  "Only available the when workspace is empty."));
        m_ViewPreserveChk->Check(m_PreserveOpenEditors);
        m_ViewMenu->AppendSeparator();
    }
}

// return the appropriate icon for an editor, based on its state
int OpenFilesListPlugin::GetOpenFilesListIcon(EditorBase* ed)
{
    int mod = 1;          // ascii
    if (ed->IsReadOnly())
        mod = 3;          // read only
    else if (ed->GetModified())
        mod =2;           // modified
    return mod;
}

void OpenFilesListPlugin::RebuildOpenFilesTree()
{
    if (Manager::IsAppShuttingDown())
        return;

    EditorManager* mgr = Manager::Get()->GetEditorManager();

    m_pTree->Freeze();
    m_pTree->DeleteChildren(m_pTree->GetRootItem());
    if (!mgr->GetEditorsCount())
    {
        m_pTree->Thaw();
        return;
    }
    // loop all open editors
    for (int i = 0; i < mgr->GetEditorsCount(); ++i)
    {
        EditorBase* ed = mgr->GetEditor(i);
        if (!ed || !ed->VisibleToTree())
            continue;

        wxString shortname = ed->GetShortName();
        int mod = GetOpenFilesListIcon(ed);
        wxTreeItemId item = m_pTree->AppendItem(m_pTree->GetRootItem(), shortname, mod, mod, new OpenFilesListData(ed));
        if (mgr->GetActiveEditor() == ed)
            m_pTree->SelectItem(item);
    }

    m_pTree->SortChildren(m_pTree->GetRootItem());
    m_pTree->Thaw();
}

void OpenFilesListPlugin::RefreshOpenFilesTree(EditorBase* ed, bool remove)
{
    if (Manager::IsAppShuttingDown() || !ed)
        return;

    EditorManager* mgr = Manager::Get()->GetEditorManager();
    EditorBase* aed = mgr->GetActiveEditor();

    m_pTree->Freeze();

    wxTreeItemIdValue cookie = 0;
    wxTreeItemId item = m_pTree->GetFirstChild(m_pTree->GetRootItem(), cookie);
    wxString shortname = ed->GetShortName();
    bool found = false;
    // loop all tree items
    while (item)
    {
        EditorBase* data = static_cast<EditorBase*>(static_cast<OpenFilesListData*>(m_pTree->GetItemData(item))->GetEditor());
        if (data && ed == data)
        {
            // and apply changes to current item
            if (!remove)
            {
                found = true;
                int mod = GetOpenFilesListIcon(ed);
                if (m_pTree->GetItemText(item) != shortname)
                    m_pTree->SetItemText(item, shortname);
                if (m_pTree->GetItemImage(item) != mod)
                {
                    m_pTree->SetItemImage(item, mod, wxTreeItemIcon_Normal);
                    m_pTree->SetItemImage(item, mod, wxTreeItemIcon_Selected);
                }
                if (ed == aed)
                    m_pTree->SelectItem(item);
            }
            else
                m_pTree->Delete(item);
             break;
        }
        item = m_pTree->GetNextChild(m_pTree->GetRootItem(), cookie);
    }

    // not found, not to-remove and valid name: add new item
    if (!found && !remove && ed->VisibleToTree() && !shortname.IsEmpty())
    {
        int mod = GetOpenFilesListIcon(ed);
        item = m_pTree->AppendItem(m_pTree->GetRootItem(), shortname, mod, mod, new OpenFilesListData(ed));
        if (mgr->GetActiveEditor() == ed)
            m_pTree->SelectItem(item);
        m_pTree->SortChildren(m_pTree->GetRootItem());
    }

    m_pTree->Thaw();
}

// tree item double-clicked
void OpenFilesListPlugin::OnTreeItemActivated(wxTreeEvent& event)
{
    if (Manager::IsAppShuttingDown())
        return;

    EditorBase* ed = static_cast<EditorBase*>( static_cast<OpenFilesListData*>(m_pTree->GetItemData(event.GetItem()))->GetEditor() );
    if (ed)
        Manager::Get()->GetEditorManager()->SetActiveEditor(ed);
}

// tree item right-clicked
void OpenFilesListPlugin::OnTreeItemRightClick(wxTreeEvent& event)
{
    if (Manager::IsAppShuttingDown())
        return;

    EditorBase* ed = static_cast<EditorBase*>( static_cast<OpenFilesListData*>(m_pTree->GetItemData(event.GetItem()))->GetEditor() );
    if (ed)
    {
        wxPoint pt = m_pTree->ClientToScreen(event.GetPoint());
        ed->DisplayContextMenu(pt,mtOpenFilesList);
    }
}

// view menu toggle tree
void OpenFilesListPlugin::OnViewOpenFilesTree(wxCommandEvent& event)
{
    CodeBlocksDockEvent evt(event.IsChecked() ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
    evt.pWindow = m_pTree;
    Manager::Get()->ProcessEvent(evt);
}

void OpenFilesListPlugin::OnViewPreserveOpenEditors(wxCommandEvent& event)
{
    m_PreserveOpenEditors = event.IsChecked();
}

void OpenFilesListPlugin::OnUpdateUI(wxUpdateUIEvent& event)
{
    if (m_ViewMenu)
    {
        bool isVis = IsWindowReallyShown((wxWindow*)m_pTree);
        m_ViewMenu->Check(idViewOpenFilesTree, isVis);
    }

    m_ViewPreserveChk->Enable(Manager::Get()->GetProjectManager()->GetProjects()->IsEmpty());

    // must do...
    event.Skip();
}

void OpenFilesListPlugin::OnEditorActivated(CodeBlocksEvent& event)
{
//  Manager::Get()->GetLogManager()->Log(_T("OnEditorActivated: ") + event.GetEditor()->GetFilename());
    RefreshOpenFilesTree(event.GetEditor());
}

void OpenFilesListPlugin::OnEditorClosed(CodeBlocksEvent& event)
{
//  Manager::Get()->GetLogManager()->Log(_T("OnEditorClosed: ") + event.GetEditor()->GetFilename());
    RefreshOpenFilesTree(event.GetEditor(), true);
}

void OpenFilesListPlugin::OnEditorDeactivated(CodeBlocksEvent& event)
{
//  Manager::Get()->GetLogManager()->Log(_T("OnEditorDeactivated: ") + event.GetEditor()->GetFilename());
    RefreshOpenFilesTree(event.GetEditor());
}

void OpenFilesListPlugin::OnEditorModified(CodeBlocksEvent& event)
{
//  Manager::Get()->GetLogManager()->Log(_T("OnEditorModified: ") + event.GetEditor()->GetFilename());
    RefreshOpenFilesTree(event.GetEditor());
}

void OpenFilesListPlugin::OnEditorOpened(CodeBlocksEvent& event)
{
    EditorBase* eb = event.GetEditor();
    if (Manager::Get()->GetProjectManager()->IsBusy() && eb && (m_EditorArray.Index(eb) == wxNOT_FOUND))
    {
        m_EditorArray.Add(eb);
    }
    else
    {
        RefreshOpenFilesTree(eb);
    }
}

void OpenFilesListPlugin::OnEditorSaved(CodeBlocksEvent& event)
{
//  Manager::Get()->GetLogManager()->Log(_T("OnEditorSaved: ") + event.GetEditor()->GetFilename());
    RefreshOpenFilesTree(event.GetEditor());
}

void OpenFilesListPlugin::OnProjectOpened(CodeBlocksEvent& event)
{
    // Load open editors list whether or not the preserve option selected not to lose any previous data
    cbProject* prj = event.GetProject();
    ProjectFilesMap pfm;
    wxFileName fname(prj->GetFilename());
    fname.SetExt(_T("layout"));
    TiXmlDocument doc;
    if (TinyXML::LoadDocument(fname.GetFullPath(), &doc))
    {
        TiXmlElement* root = doc.FirstChildElement("CodeBlocks_layout_file");
        if (!root)
            root = doc.FirstChildElement("Code::Blocks_layout_file");
        if (root)
        {
            TiXmlElement* target = root->FirstChildElement("Target");
            while (target)
            {
                wxString targetName = cbC2U(target->Attribute("name"));
                wxString topFile = cbC2U(target->Attribute("top"));
                if (topFile != wxEmptyString)
                {
                    ProjectFile* pf = prj->GetFileByFilename(topFile);
                    if (pf)
                        pfm[targetName].activeFile = pf;
                }

                int tp = 0;
                TiXmlElement* file = target->FirstChildElement("File");
                while (file)
                {
                    wxString fn = cbC2U(file->Attribute("name"));
                    // Tab order may differ than the original, 'tp' is used as a cheap hack to restore the tab order
                    ProjectFile* pf = prj->GetFileByFilename(fn);
                    if (pf)
                    {
                        pf->editorTabPos = tp++;
                        pfm[targetName].openFiles.insert(pf);
                    }
                    file = file->NextSiblingElement();
                }

                target = target->NextSiblingElement();
            }
        }
    }

    m_WorkspaceFilesMap.insert(std::pair<cbProject*, ProjectFilesMap>(prj, pfm));

    if (m_PreserveOpenEditors)
        m_ProjectLoading = true;

    if (m_EditorArray.IsEmpty())
        return;

    for (size_t i = 0; i < m_EditorArray.GetCount(); ++i)
    {
        EditorBase* eb = m_EditorArray[i];
        if (eb)
        {
            RefreshOpenFilesTree(eb);
        }
    }
    m_EditorArray.Clear();
}

void OpenFilesListPlugin::OnProjectActivated(CodeBlocksEvent& event)
{
    if (m_PreserveOpenEditors)
    {
        cbProject* np = event.GetProject();
        wxString nt = np->GetActiveBuildTarget();
        if (m_ProjectLoading)
            m_ProjectLoading = false;
        else if (!m_pActiveProject) // Active project closed
        {
            if (m_WorkspaceFilesMap.find(np) != m_WorkspaceFilesMap.end() && m_WorkspaceFilesMap[np].find(nt) != m_WorkspaceFilesMap[np].end())
                LoadEditors(event.GetProject(), event.GetProject()->GetActiveBuildTarget());
        }
        else // An already open project activated
        {
            SaveEditors(m_pActiveProject, m_ActiveTargetName);
            CloseEditors(m_pActiveProject, m_ActiveTargetName);
            if (m_WorkspaceFilesMap.find(np) != m_WorkspaceFilesMap.end() && m_WorkspaceFilesMap[np].find(nt) != m_WorkspaceFilesMap[np].end())
                LoadEditors(event.GetProject(), event.GetProject()->GetActiveBuildTarget());
        }

        m_pActiveProject = event.GetProject();
        m_ActiveTargetName = m_pActiveProject->GetActiveBuildTarget();
    }

    if (m_EditorArray.IsEmpty())
        return;

    for (size_t i = 0; i < m_EditorArray.GetCount(); ++i)
    {
        EditorBase* eb = m_EditorArray[i];
        if (eb)
        {
            RefreshOpenFilesTree(eb);
        }
    }
    m_EditorArray.Clear();
}

void OpenFilesListPlugin::OnProjectClosed(CodeBlocksEvent& event)
{
    cbProject* prj = event.GetProject();

    if (m_PreserveOpenEditors)
    {
        if (event.GetProject() == m_pActiveProject) // Active project is closed
        {
            SaveEditors(m_pActiveProject, m_ActiveTargetName);
            m_pActiveProject = nullptr;
            m_ActiveTargetName = wxEmptyString;
        }
    }

    // Save open editors list whether or not preserve option selected not to lose any previous data
    if (!m_WorkspaceFilesMap[prj].empty())
    {
        ProjectFilesMap& pfm = m_WorkspaceFilesMap[prj];
        wxFileName fname(prj->GetFilename());
        fname.SetExt(_T("layout"));
        TiXmlDocument doc;
        doc.SetCondenseWhiteSpace(false);
        if (TinyXML::LoadDocument(fname.GetFullPath(), &doc))
        {
            TiXmlElement* root = doc.FirstChildElement("CodeBlocks_layout_file");
            for (ProjectFilesMap::iterator itt = pfm.begin(); itt != pfm.end(); itt++)
            {
                // Do not save renamed or deleted targets
                if (prj->GetBuildTarget((*itt).first))
                {
                    TiXmlElement* target = static_cast<TiXmlElement*>(root->InsertEndChild(TiXmlElement("Target")));
                    target->SetAttribute("name", cbU2C((*itt).first));
                    if ((*itt).second.activeFile)
                        target->SetAttribute("top", cbU2C((*itt).second.activeFile->relativeFilename));
                    else
                        target->SetAttribute("top", cbU2C(wxEmptyString));
                    for (TargetFilesData::OpenFilesSet::iterator itf = (*itt).second.openFiles.begin(); itf != (*itt).second.openFiles.end(); ++itf)
                    {
                        TiXmlElement* file = static_cast<TiXmlElement*>(target->InsertEndChild(TiXmlElement("File")));
                        file->SetAttribute("name", cbU2C((*itf)->relativeFilename));
                    }
                }
            }
            cbSaveTinyXMLDocument(&doc, fname.GetFullPath());
        }
    }

    m_WorkspaceFilesMap.erase(event.GetProject());
}

void OpenFilesListPlugin::OnBuildTargetSelected(CodeBlocksEvent& event)
{
    if (m_PreserveOpenEditors && event.GetBuildTargetName() != wxEmptyString) // Make sure project is not closing
    {
        wxString nt = event.GetBuildTargetName();
        if (event.GetProject() != m_pActiveProject) // Another project is opened or activated
        {
            if (!m_pActiveProject) // Workspace is empty, nothing to do
                return;
            else if (m_WorkspaceFilesMap.find(event.GetProject()) == m_WorkspaceFilesMap.end()) // New project is opened
            {
                SaveEditors(m_pActiveProject, m_ActiveTargetName);
                CloseEditors(m_pActiveProject, m_ActiveTargetName);
            }
        }
        else // Project's targets switched
        {
            SaveEditors(m_pActiveProject, m_ActiveTargetName);

            // Load new target's editors if there is any, keep the open editors otherwise
            if (m_WorkspaceFilesMap[m_pActiveProject].find(nt) != m_WorkspaceFilesMap[m_pActiveProject].end())
            {
                CloseEditors(m_pActiveProject, m_ActiveTargetName);
                LoadEditors(m_pActiveProject, nt);
            }
            m_ActiveTargetName = nt;
        }
    }
}

void OpenFilesListPlugin::SaveEditors(cbProject* project, const wxString& targetName)
{
    EditorManager* em = Manager::Get()->GetEditorManager();
    if (em)
    {
        // Save target's open editors sorted by their tab order
        m_WorkspaceFilesMap[project][targetName].openFiles.clear();
        em->UpdateProjectFiles(project);
        for (int i = 0; i < em->GetEditorsCount(); i++)
        {
            EditorBase* eb = em->GetEditor(i);
            if (eb->IsBuiltinEditor())
            {
                ProjectFile* pf = project->GetFileByFilename(eb->GetFilename(), false);
                if (pf)
                    m_WorkspaceFilesMap[project][targetName].openFiles.insert(pf);
            }
        }
        // Save active editor
        EditorBase* eb = em->GetActiveEditor();
        m_WorkspaceFilesMap[project][targetName].activeFile = (eb && eb->IsBuiltinEditor() ? project->GetFileByFilename(eb->GetFilename(), false) : nullptr);
    }
}

void OpenFilesListPlugin::CloseEditors(cbProject* project, const wxString& targetName)
{
    EditorManager* em = Manager::Get()->GetEditorManager();
    if (em)
    {
        // Close old target's open editors, CloseAll() won't do good here because other non-project files may already be opened
        TargetFilesData::OpenFilesSet::iterator it;
        for (it = m_WorkspaceFilesMap[project][targetName].openFiles.begin(); it != m_WorkspaceFilesMap[project][targetName].openFiles.end(); ++it)
            em->Close((*it)->file.GetFullPath());
    }
}

void OpenFilesListPlugin::LoadEditors(cbProject* project, const wxString& targetName)
{
    EditorManager* em = Manager::Get()->GetEditorManager();
    if (em)
    {
        // Save and close non-project built-in editors
        std::set<wxString> otherEditors;
        for (int i = 0; i < em->GetEditorsCount(); i++)
        {
            EditorBase* eb = em->GetEditor(i);
            if (eb->IsBuiltinEditor())
            {
                wxString fn = em->GetBuiltinEditor(eb)->GetFilename();
                if (fn != wxEmptyString)
                {
                    otherEditors.insert(fn);
                    em->Close(fn);
                }
            }
        }

        TargetFilesData::OpenFilesSet::iterator it;
        // Load target's open editors
        for (it = m_WorkspaceFilesMap[project][targetName].openFiles.begin(); it != m_WorkspaceFilesMap[project][targetName].openFiles.end(); ++it)
            em->Open((*it)->file.GetFullPath());

        // Reopen non-project built-in editors to maintain consistency with initial project startup
        std::set<wxString>::iterator its;
        for (its = otherEditors.begin(); its != otherEditors.end(); ++its)
            em->Open(*its);

        // Activate target's active editor
        ProjectFile* pf = m_WorkspaceFilesMap[project][targetName].activeFile;
        if (pf)
            em->Open(pf->file.GetFullPath());
    }
}

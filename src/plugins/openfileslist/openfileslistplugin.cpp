#include "openfileslistplugin.h"

#include <manager.h>
#include <configmanager.h>
#include <editormanager.h>
#include <logmanager.h>
#include <editorbase.h>
#include <sdk_events.h>
#include <misctreeitemdata.h>

#include <wx/window.h>
#include <wx/treectrl.h>
#include <wx/bitmap.h>
#include <wx/imaglist.h>
#include <wx/menu.h>

namespace compatibility
{
    typedef TernaryCondTypedef<wxMinimumVersion<2,5>::eval, wxTreeItemIdValue, long int>::eval tree_cookie_t;
}

namespace
{
    // this auto-registers the plugin
    PluginRegistrant<OpenFilesListPlugin> reg(_T("OpenFilesList"));

    const int idOpenFilesTree = wxNewId();
    const int idViewOpenFilesTree = wxNewId();

    class OpenFilesListData : public wxTreeItemData
    {
        public:
            OpenFilesListData(EditorBase* ed) : ed(ed) {}
            EditorBase* GetEditor() const { return ed; }
        private:
            EditorBase* ed;
    };
}

BEGIN_EVENT_TABLE(OpenFilesListPlugin, cbPlugin)
    EVT_UPDATE_UI(idViewOpenFilesTree, OpenFilesListPlugin::OnUpdateUI)
    EVT_MENU(idViewOpenFilesTree, OpenFilesListPlugin::OnViewOpenFilesTree)
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
    pm->RegisterEventSink(cbEVT_PROJECT_ACTIVATE, new cbEventFunctor<OpenFilesListPlugin, CodeBlocksEvent>(this, &OpenFilesListPlugin::OnProjectOpened));
    pm->RegisterEventSink(cbEVT_PROJECT_OPEN, new cbEventFunctor<OpenFilesListPlugin, CodeBlocksEvent>(this, &OpenFilesListPlugin::OnProjectOpened));
}

void OpenFilesListPlugin::OnRelease()
{
    // remove registered event sinks
    Manager::Get()->RemoveAllEventSinksFor(this);

    // remove tree from docking system
    CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
    evt.pWindow = m_pTree;
    Manager::Get()->ProcessEvent(evt);

    // finally destroy the tree
    m_pTree->Destroy();
}

void OpenFilesListPlugin::BuildMenu(wxMenuBar* menuBar)
{
    // if not attached, exit
    if (!IsAttached())
        return;

    // add the open files list in the "View" menu
    int idx = menuBar->FindMenu(_("View"));
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
                return;
            }
        }
        // not found, just append
        m_ViewMenu->AppendCheckItem(idViewOpenFilesTree, _("&Open files list"), _("Toggle displaying the open files list"));
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
    if(Manager::IsAppShuttingDown())
        return;

    EditorManager* mgr = Manager::Get()->GetEditorManager();

    m_pTree->Freeze();
    m_pTree->DeleteChildren(m_pTree->GetRootItem());
    if(!mgr->GetEditorsCount())
    {
        m_pTree->Thaw();
        return;
    }
    // loop all open editors
    for (int i = 0; i < mgr->GetEditorsCount(); ++i)
    {
        EditorBase* ed = mgr->GetEditor(i);
        if(!ed || !ed->VisibleToTree())
            continue;

        wxString shortname = ed->GetShortName();
        int mod = GetOpenFilesListIcon(ed);
        wxTreeItemId item = m_pTree->AppendItem(m_pTree->GetRootItem(), shortname, mod, mod, new OpenFilesListData(ed));
        if(mgr->GetActiveEditor() == ed)
            m_pTree->SelectItem(item);
    }

    m_pTree->SortChildren(m_pTree->GetRootItem());
    m_pTree->Expand(m_pTree->GetRootItem());
    m_pTree->Thaw();
}

void OpenFilesListPlugin::RefreshOpenFilesTree(EditorBase* ed, bool remove)
{
    if(Manager::IsAppShuttingDown())
        return;

    EditorManager* mgr = Manager::Get()->GetEditorManager();
    EditorBase* aed = mgr->GetActiveEditor();

    m_pTree->Freeze();

    compatibility::tree_cookie_t cookie = 0;
    wxTreeItemId item = m_pTree->GetFirstChild(m_pTree->GetRootItem(), cookie);
    wxString shortname = ed->GetShortName();
    bool found = false;
    // loop all tree items
    while (item)
    {
        EditorBase* data = static_cast<EditorBase*>(static_cast<OpenFilesListData*>(m_pTree->GetItemData(item))->GetEditor());
        if(data && ed == data)
        {
            // and apply changes to current item
            if (!remove)
            {
                found = true;
                int mod = GetOpenFilesListIcon(ed);
                if(m_pTree->GetItemText(item) != shortname)
                    m_pTree->SetItemText(item, shortname);
                if (m_pTree->GetItemImage(item) != mod)
                {
                    m_pTree->SetItemImage(item, mod, wxTreeItemIcon_Normal);
                    m_pTree->SetItemImage(item, mod, wxTreeItemIcon_Selected);
                }
                if(ed == aed)
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
        wxTreeItemId item = m_pTree->AppendItem(m_pTree->GetRootItem(), shortname, mod, mod, new OpenFilesListData(ed));
        if(mgr->GetActiveEditor() == ed)
            m_pTree->SelectItem(item);
        m_pTree->SortChildren(m_pTree->GetRootItem());
        m_pTree->Expand(m_pTree->GetRootItem());
    }

    m_pTree->Thaw();
}

// tree item double-clicked
void OpenFilesListPlugin::OnTreeItemActivated(wxTreeEvent &event)
{
    if(Manager::IsAppShuttingDown())
        return;
    EditorBase* ed = static_cast<EditorBase*>(static_cast<OpenFilesListData*>(m_pTree->GetItemData(event.GetItem()))->GetEditor());
    if(ed)
        Manager::Get()->GetEditorManager()->SetActiveEditor(ed);
}

// tree item right-clicked
void OpenFilesListPlugin::OnTreeItemRightClick(wxTreeEvent &event)
{
    if(Manager::IsAppShuttingDown())
        return;
    EditorBase* ed = static_cast<EditorBase*>(static_cast<OpenFilesListData*>(m_pTree->GetItemData(event.GetItem()))->GetEditor());
    if(ed)
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

void OpenFilesListPlugin::OnUpdateUI(wxUpdateUIEvent& event)
{
    if (m_ViewMenu)
    {
        bool isVis = IsWindowReallyShown((wxWindow*)m_pTree);
        m_ViewMenu->Check(idViewOpenFilesTree, isVis);
    }

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

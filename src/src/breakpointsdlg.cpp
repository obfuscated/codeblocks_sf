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
#   include <algorithm>
#   include <set>

#   include "globals.h"
#   include "manager.h"
#   include "editormanager.h"
#   include "cbeditor.h"
#   include "cbplugin.h"

#   include <wx/button.h>
#   include <wx/checkbox.h>
#   include <wx/intl.h>
#   include <wx/listbox.h>
#   include <wx/listctrl.h>
#   include <wx/menu.h>
#   include <wx/textctrl.h>
#   include <wx/spinctrl.h>
#   include <wx/sizer.h>
#endif

#include "debuggermanager.h"

#include "breakpointsdlg.h"
#include "cbstyledtextctrl.h"

namespace
{
    const long idList = wxNewId();
    // menu
    const long idRemove = wxNewId();
    const long idRemoveAll = wxNewId();
    const long idProperties = wxNewId();
    const long idOpen = wxNewId();
    const long idEnable = wxNewId();
    const long idDisable = wxNewId();
    const long idShowTemp = wxNewId();
};

BEGIN_EVENT_TABLE(BreakpointsDlg, wxPanel)
    EVT_MENU(idRemove, BreakpointsDlg::OnRemove)
    EVT_MENU(idRemoveAll, BreakpointsDlg::OnRemoveAll)
    EVT_MENU(idProperties, BreakpointsDlg::OnProperties)
    EVT_MENU(idOpen, BreakpointsDlg::OnOpen)
    EVT_MENU(idEnable, BreakpointsDlg::OnEnable)
    EVT_MENU(idDisable, BreakpointsDlg::OnEnable)
    EVT_MENU(idShowTemp, BreakpointsDlg::OnShowTemp)

    EVT_KEY_UP(BreakpointsDlg::OnKeyUp)

    EVT_UPDATE_UI(idRemove, BreakpointsDlg::OnUpdateUI)
    EVT_UPDATE_UI(idRemoveAll, BreakpointsDlg::OnUpdateUI)
    EVT_UPDATE_UI(idProperties, BreakpointsDlg::OnUpdateUI)
    EVT_UPDATE_UI(idOpen, BreakpointsDlg::OnUpdateUI)
    EVT_UPDATE_UI(idShowTemp, BreakpointsDlg::OnUpdateUI)
END_EVENT_TABLE()

BreakpointsDlg::BreakpointsDlg() :
    wxPanel(Manager::Get()->GetAppWindow(), -1),
    m_icons(16, 16, true)
{
    wxBoxSizer* bs = new wxBoxSizer(wxVERTICAL);
    m_pList = new wxListCtrl(this, idList, wxDefaultPosition, wxDefaultSize,
                             wxLC_REPORT | wxLC_HRULES | wxLC_VRULES);
    bs->Add(m_pList, 1, wxEXPAND | wxALL);
    SetAutoLayout(TRUE);
    SetSizer(bs);

    // Setup the image list for the enabled/disabled icons.
    const wxString &basepath = ConfigManager::GetDataFolder() + wxT("/manager_resources.zip#zip:/images/16x16/");
    wxBitmap icon = cbLoadBitmap(basepath + wxT("breakpoint.png"), wxBITMAP_TYPE_PNG);
    if (icon.IsOk())
        m_icons.Add(icon);
    icon = cbLoadBitmap(basepath + wxT("breakpoint_disabled.png"), wxBITMAP_TYPE_PNG);
    if (icon.IsOk())
        m_icons.Add(icon);
    icon = cbLoadBitmap(basepath + wxT("breakpoint_other.png"), wxBITMAP_TYPE_PNG);
    if (icon.IsOk())
        m_icons.Add(icon);
    m_pList->SetImageList(&m_icons, wxIMAGE_LIST_SMALL);

    m_pList->InsertColumn(Type, _("Type"), wxLIST_FORMAT_LEFT, 128);
    m_pList->InsertColumn(FilenameAddress, _("Filename/Address"), wxLIST_FORMAT_LEFT, 128);
    m_pList->InsertColumn(Line, _("Line"), wxLIST_FORMAT_LEFT, 44);
    m_pList->InsertColumn(Info, _("Info"), wxLIST_FORMAT_LEFT, 120);
    m_pList->InsertColumn(Debugger, _("Debugger"), wxLIST_FORMAT_LEFT, 60);

    Connect(idList, -1, wxEVT_COMMAND_LIST_ITEM_ACTIVATED,
            (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction)
            &BreakpointsDlg::OnDoubleClick);

    Connect(idList, -1, wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK,
            (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction)
            &BreakpointsDlg::OnRightClick);

    typedef cbEventFunctor<BreakpointsDlg, CodeBlocksEvent> CBEvent;

    Reload();
}

void BreakpointsDlg::Reload()
{
    m_pList->Freeze();
    m_pList->DeleteAllItems();
    m_breakpoints.clear();

    bool showTemp = cbDebuggerCommonConfig::GetFlag(cbDebuggerCommonConfig::ShowTemporaryBreakpoints);

    DebuggerManager::RegisteredPlugins const &debuggers = Manager::Get()->GetDebuggerManager()->GetAllDebuggers();
    for (DebuggerManager::RegisteredPlugins::const_iterator dbg = debuggers.begin(); dbg != debuggers.end(); ++dbg)
    {
        int count = dbg->first->GetBreakpointsCount();
        for (int ii = 0; ii < count; ++ii)
        {
            cb::shared_ptr<cbBreakpoint> bp = dbg->first->GetBreakpoint(ii);
            if (showTemp || (!showTemp && !bp->IsTemporary()))
                m_breakpoints.push_back(Item(bp, dbg->first, dbg->first->GetGUIName()));
        }
    }

    cbDebuggerPlugin *activeDebugger = Manager::Get()->GetDebuggerManager()->GetActiveDebugger();

    for (Items::const_iterator it = m_breakpoints.begin(); it != m_breakpoints.end(); ++it)
    {
        m_pList->InsertItem(m_pList->GetItemCount(), _T(""));
        long item = m_pList->GetItemCount() - 1;
        int imageId;
        if (it->plugin != activeDebugger)
            imageId = 2;
        else if (it->breakpoint->IsEnabled())
            imageId = 0;
        else
            imageId = 1;
        m_pList->SetItem(item, Type, it->breakpoint->GetType(), imageId);
        m_pList->SetItem(item, FilenameAddress, it->breakpoint->GetLocation());
        m_pList->SetItem(item, Line, it->breakpoint->GetLineString());
        m_pList->SetItem(item, Info, it->breakpoint->GetInfo());
        m_pList->SetItem(item, Debugger, it->pluginName);
    }

    if (!m_breakpoints.empty())
    {
        for (int column = 0; column < m_pList->GetColumnCount(); ++column)
            m_pList->SetColumnWidth(column, wxLIST_AUTOSIZE);
    }
    m_pList->Thaw();
}

BreakpointsDlg::Items::iterator BreakpointsDlg::FindBreakpoint(const wxString &filename, int line)
{
    for (Items::iterator it = m_breakpoints.begin(); it != m_breakpoints.end(); ++it)
    {
        if (it->breakpoint->IsVisibleInEditor())
        {
            if (it->breakpoint->GetLocation() == filename && it->breakpoint->GetLine() == line)
                return it;
        }
    }
    return m_breakpoints.end();
}

bool BreakpointsDlg::AddBreakpoint(cbDebuggerPlugin *plugin, const wxString& filename, int line)
{
    if (plugin && plugin->AddBreakpoint(filename, line))
    {
        Reload();
        return true;
    }
    else
        return false;
}

struct FindBreakpointPred
{
    FindBreakpointPred(cbDebuggerPlugin *plugin_in, const wxString &filename_in, int line_in) :
        plugin(plugin_in),
        filename(filename_in),
        line(line_in)
    {}
    bool operator()(const BreakpointsDlg::Item &item) const
    {
        return plugin == item.plugin
                && item.breakpoint->IsVisibleInEditor()
                && item.breakpoint->GetLocation() == filename
                && item.breakpoint->GetLine() == line;
    }
private:
    cbDebuggerPlugin *plugin;
    const wxString &filename;
    int line;
};

bool BreakpointsDlg::RemoveBreakpoint(cbDebuggerPlugin *plugin, const wxString& filename, int line)
{
    if (!plugin)
        return false;
    Items::iterator it;
    it = std::find_if(m_breakpoints.begin(), m_breakpoints.end(), FindBreakpointPred(plugin, filename, line));
    if (it != m_breakpoints.end())
    {
        it->plugin->DeleteBreakpoint(it->breakpoint);
        Reload();
        return true;
    }
    else
        return false;
}

void BreakpointsDlg::RemoveAllBreakpoints()
{
    std::set<cbDebuggerPlugin*> plugins;
    std::set<cbEditor*> editors;

    for (Items::iterator it = m_breakpoints.begin(); it != m_breakpoints.end(); ++it)
    {
        cbBreakpoint& b = *it->breakpoint;

        if (b.IsVisibleInEditor())
        {
            cbEditor* ed = Manager::Get()->GetEditorManager()->IsBuiltinOpen(b.GetLocation());
            if (ed)
            {
                plugins.insert(it->plugin);
                editors.insert(ed);
            }
        }
        else
            plugins.insert(it->plugin);
    }
    for (std::set<cbDebuggerPlugin*>::iterator it = plugins.begin(); it != plugins.end(); ++it)
        (*it)->DeleteAllBreakpoints();
    for (std::set<cbEditor*>::iterator it = editors.begin(); it != editors.end(); ++it)
        (*it)->RefreshBreakpointMarkers();

    Reload();

}

void BreakpointsDlg::EditBreakpoint(const wxString& filename, int line)
{
    Items::iterator it = FindBreakpoint(filename, line);
    if (it != m_breakpoints.end())
        BreakpointProperties(*it);
}

void BreakpointsDlg::EnableBreakpoint(const wxString& filename, int line, bool enable)
{
    Items::iterator it = FindBreakpoint(filename, line);
    if (it != m_breakpoints.end())
    {
        it->plugin->EnableBreakpoint(it->breakpoint, enable);
        if (it->breakpoint->IsVisibleInEditor())
        {
            cbEditor *ed = Manager::Get()->GetEditorManager()->IsBuiltinOpen(it->breakpoint->GetLocation());
            if (ed)
                ed->RefreshBreakpointMarkers();
        }
        Reload();
    }
}

void BreakpointsDlg::OnRemove(cb_unused wxCommandEvent& event)
{
    long item = -1;
    bool reload = false;
    std::set<cbEditor*> editors;

    while ((item = m_pList->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED)) != -1)
    {
        if (item >= 0 || item < static_cast<int>(m_breakpoints.size()))
        {
            Item const &data = m_breakpoints[item];
            if (data.breakpoint->IsVisibleInEditor())
            {
                cbEditor *ed = Manager::Get()->GetEditorManager()->IsBuiltinOpen(data.breakpoint->GetLocation());
                if (ed)
                    editors.insert(ed);
            }

            data.plugin->DeleteBreakpoint(data.breakpoint);
            reload = true;
        }
    }
    if (reload)
        Reload();
    for (std::set<cbEditor*>::iterator it = editors.begin(); it != editors.end(); ++it)
        (*it)->RefreshBreakpointMarkers();
}

void BreakpointsDlg::OnRemoveAll(cb_unused wxCommandEvent& event)
{
    RemoveAllBreakpoints();
}

void BreakpointsDlg::OnProperties(cb_unused wxCommandEvent& event)
{
    int sel = m_pList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (sel < 0 || sel >= static_cast<int>(m_breakpoints.size()))
        return;
    BreakpointProperties(m_breakpoints[sel]);
}

void BreakpointsDlg::BreakpointProperties(const Item &item)
{
    item.plugin->UpdateBreakpoint(item.breakpoint);
    if (item.breakpoint->IsVisibleInEditor())
    {
        cbEditor *ed = Manager::Get()->GetEditorManager()->IsBuiltinOpen(item.breakpoint->GetLocation());
        if (ed)
            ed->RefreshBreakpointMarkers();
    }
    Reload();
}

void BreakpointsDlg::OnOpen(cb_unused wxCommandEvent& event)
{
    long item_index = m_pList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (item_index < 0 || item_index >= static_cast<int>(m_breakpoints.size()))
        return;

    Item const &item = m_breakpoints[item_index];

    if (item.breakpoint->IsVisibleInEditor())
    {
        cbEditor* ed = Manager::Get()->GetEditorManager()->Open(item.breakpoint->GetLocation());
        if (ed)
        {
            ed->GotoLine(item.breakpoint->GetLine() - 1, true);
            ed->Activate();
        }
    }
}

void BreakpointsDlg::OnRightClick(cb_unused wxListEvent& event)
{
    long itemIndex = -1;
    bool hasEnabled = false, hasDisabled = false;
    bool found = false;
    while ((itemIndex = m_pList->GetNextItem(itemIndex, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED)) != -1)
    {
        found = true;
        bool enabled = m_breakpoints[itemIndex].breakpoint->IsEnabled();
        if (enabled)
            hasEnabled = true;
        else
            hasDisabled = true;
    }

    wxMenu menu;
    menu.Append(idOpen, _("Open in editor"));
    menu.Append(idProperties, _("Edit"));
    menu.AppendSeparator();
    if (found)
    {
        if (hasDisabled)
            menu.Append(idEnable, _("Enable"));
        if (hasEnabled)
            menu.Append(idDisable, _("Disable"));
        menu.AppendSeparator();
    }
    menu.Append(idRemove, _("Remove"));
    menu.Append(idRemoveAll, _("Remove all"));
    menu.AppendSeparator();
    menu.AppendCheckItem(idShowTemp, _("Show temporary"));

    if (!found)
    {
        menu.Enable(idOpen, false);
        menu.Enable(idProperties, false);
        menu.Enable(idRemove, false);
    }
    if (m_pList->GetItemCount() == 0)
        menu.Enable(idRemoveAll, false);
    PopupMenu(&menu);
}

void BreakpointsDlg::OnDoubleClick(cb_unused wxListEvent& event)
{
    wxCommandEvent evt;
    OnOpen(evt);
}

void BreakpointsDlg::OnKeyUp(wxKeyEvent& event)
{
    if (event.GetKeyCode() == WXK_DELETE || event.GetKeyCode() == WXK_NUMPAD_DELETE)
    {
        wxCommandEvent empty_event;
        OnRemove(empty_event);
    }
}

void BreakpointsDlg::OnBreakpointAdd(CodeBlocksEvent& event)
{
// TODO (obfuscated#) add the line text to the breakpoint
    DebuggerManager *manager = Manager::Get()->GetDebuggerManager();
    if(manager->GetActiveDebugger())
        manager->GetActiveDebugger()->AddBreakpoint(event.GetString(), event.GetInt());

    Reload();
}

void BreakpointsDlg::OnBreakpointEdit(CodeBlocksEvent& event)
{
    const wxString& filename = event.GetString();
    int line = event.GetInt();

    for (Items::iterator it = m_breakpoints.begin(); it != m_breakpoints.end(); ++it)
    {
        if (it->breakpoint->GetLocation() == filename && it->breakpoint->GetLine() == line)
        {
            it->plugin->UpdateBreakpoint(it->breakpoint);
            if (it->breakpoint->IsVisibleInEditor())
            {
                EditorBase *ed = Manager::Get()->GetEditorManager()->GetEditor(filename);
                if (ed)
                    ed->RefreshBreakpointMarkers();
            }
            break;
        }
    }
    Reload();
}

void BreakpointsDlg::OnShowTemp(cb_unused wxCommandEvent& event)
{
    bool old = cbDebuggerCommonConfig::GetFlag(cbDebuggerCommonConfig::ShowTemporaryBreakpoints);
    cbDebuggerCommonConfig::SetFlag(cbDebuggerCommonConfig::ShowTemporaryBreakpoints, !old);
    Reload();
}

void BreakpointsDlg::OnUpdateUI(wxUpdateUIEvent &event)
{
    if (event.GetId() == idShowTemp)
        event.Check(cbDebuggerCommonConfig::GetFlag(cbDebuggerCommonConfig::ShowTemporaryBreakpoints));
}

void BreakpointsDlg::OnEnable(wxCommandEvent &event)
{
    bool enable = (event.GetId() == idEnable);
    typedef std::pair<cbEditor*, cbDebuggerPlugin*> EditorPair;
    std::set<EditorPair> editorsToRefresh;
    long itemIndex = -1;
    bool reload = false;

    while ((itemIndex = m_pList->GetNextItem(itemIndex, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED)) != -1)
    {
        const Item &item = m_breakpoints[itemIndex];
        if (item.breakpoint->IsEnabled() != enable)
        {
            item.plugin->EnableBreakpoint(item.breakpoint, enable);
            reload = true;
            if (item.breakpoint->IsVisibleInEditor())
            {
                cbEditor *editor = Manager::Get()->GetEditorManager()->IsBuiltinOpen(item.breakpoint->GetLocation());
                if (editor)
                    editorsToRefresh.insert(EditorPair(editor, item.plugin));
            }
        }
    }

    for (std::set<EditorPair>::iterator it = editorsToRefresh.begin(); it != editorsToRefresh.end(); ++it)
        it->first->RefreshBreakpointMarkers();

    if (reload)
        Reload();
}

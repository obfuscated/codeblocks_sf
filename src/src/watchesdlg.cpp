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
    #include <wx/app.h>
    #include <wx/dnd.h>
    #include <wx/fontutil.h>
    #include <wx/menu.h>
    #include <wx/settings.h>
    #include <wx/sizer.h>

    #include "cbexception.h"
    #include "cbplugin.h"
    #include "logmanager.h"
    #include "scrollingdialog.h"
#endif

#include <numeric>
#include <map>

#include <wx/propgrid/propgrid.h>

#include "watchesdlg.h"

#include "cbcolourmanager.h"
#include "debuggermanager.h"

namespace
{
    const long idGrid = wxNewId();
    const long idTooltipGrid = wxNewId();
    const long idTooltipTimer = wxNewId();

    const long idMenuRename = wxNewId();
    const long idMenuProperties = wxNewId();
    const long idMenuDelete = wxNewId();
    const long idMenuDeleteAll = wxNewId();
    const long idMenuAddDataBreak = wxNewId();
    const long idMenuAutoUpdate = wxNewId();
    const long idMenuUpdate = wxNewId();
}

BEGIN_EVENT_TABLE(WatchesDlg, wxPanel)
    EVT_PG_ITEM_EXPANDED(idGrid, WatchesDlg::OnExpand)
    EVT_PG_ITEM_COLLAPSED(idGrid, WatchesDlg::OnCollapse)
    EVT_PG_SELECTED(idGrid, WatchesDlg::OnPropertySelected)
    EVT_PG_CHANGED(idGrid, WatchesDlg::OnPropertyChanged)
    EVT_PG_CHANGING(idGrid, WatchesDlg::OnPropertyChanging)
    EVT_PG_LABEL_EDIT_BEGIN(idGrid, WatchesDlg::OnPropertyLableEditBegin)
    EVT_PG_LABEL_EDIT_ENDING(idGrid, WatchesDlg::OnPropertyLableEditEnd)
    EVT_PG_RIGHT_CLICK(idGrid, WatchesDlg::OnPropertyRightClick)
    EVT_IDLE(WatchesDlg::OnIdle)

    EVT_MENU(idMenuRename, WatchesDlg::OnMenuRename)
    EVT_MENU(idMenuProperties, WatchesDlg::OnMenuProperties)
    EVT_MENU(idMenuDelete, WatchesDlg::OnMenuDelete)
    EVT_MENU(idMenuDeleteAll, WatchesDlg::OnMenuDeleteAll)
    EVT_MENU(idMenuAddDataBreak, WatchesDlg::OnMenuAddDataBreak)
    EVT_MENU(idMenuAutoUpdate, WatchesDlg::OnMenuAutoUpdate)
    EVT_MENU(idMenuUpdate, WatchesDlg::OnMenuUpdate)
END_EVENT_TABLE()

#if wxCHECK_VERSION(2,9,0)
typedef wxString wxPG_CONST_WXCHAR_PTR;
#endif

struct WatchesDlg::WatchItemPredicate
{
    WatchItemPredicate(cb::shared_ptr<cbWatch> watch) : m_watch(watch) {}

    bool operator()(const WatchItem& item) const
    {
        return item.watch == m_watch;
    }
private:
    cb::shared_ptr<cbWatch> m_watch;
};

class cbTextCtrlAndButtonTooltipEditor : public wxPGTextCtrlAndButtonEditor
{
    DECLARE_DYNAMIC_CLASS(cbTextCtrlAndButtonTooltipEditor)
public:
    virtual wxPG_CONST_WXCHAR_PTR GetName() const
    {
        return wxT("cbTextCtrlAndButtonTooltipEditor");
    }

    virtual wxPGWindowList CreateControls(wxPropertyGrid* propgrid, wxPGProperty* property,
                                          const wxPoint& pos, const wxSize& sz) const
    {
        wxPGWindowList const &list = wxPGTextCtrlAndButtonEditor::CreateControls(propgrid, property, pos, sz);

        list.m_secondary->SetToolTip(_("Click the button to see the value.\n"
                                       "Hold CONTROL to see the raw output string returned by the debugger.\n"
                                       "Hold SHIFT to see debugging representation of the cbWatch object."));
        return list;
    }

};

IMPLEMENT_DYNAMIC_CLASS(cbTextCtrlAndButtonTooltipEditor, wxPGTextCtrlAndButtonEditor);
static wxPGEditor *watchesPropertyEditor = nullptr;

class WatchesProperty : public wxStringProperty
{
        DECLARE_DYNAMIC_CLASS(WatchesProperty)

        WatchesProperty(){}
    public:
        WatchesProperty(const wxString& label, const wxString& value, cb::shared_ptr<cbWatch> watch, bool readonly) :
            wxStringProperty(label, wxPG_LABEL, value),
            m_watch(watch),
            m_readonly(readonly)
        {
        }

        // Set editor to have button
        virtual const wxPGEditor* DoGetEditorClass() const
        {
            return m_readonly ? nullptr : watchesPropertyEditor;
        }

        // Set what happens on button click
        virtual wxPGEditorDialogAdapter* GetEditorDialog() const;

        cb::shared_ptr<cbWatch> GetWatch() { return m_watch; }
        cb::shared_ptr<const cbWatch> GetWatch() const { return m_watch; }
        void SetWatch(cb::shared_ptr<cbWatch> watch) { m_watch = watch; }

    protected:
        cb::shared_ptr<cbWatch> m_watch;
        bool m_readonly;
};

class WatchRawDialogAdapter : public wxPGEditorDialogAdapter
{
    public:

        WatchRawDialogAdapter()
        {
        }

        virtual bool DoShowDialog(wxPropertyGrid* WXUNUSED(propGrid), wxPGProperty* property);

    protected:
};

IMPLEMENT_DYNAMIC_CLASS(WatchesProperty, wxStringProperty);

/// @breif dialog to show the value of a watch
class WatchRawDialog : public wxScrollingDialog
{
    private:
        enum Type
        {
            TypeNormal,
            TypeDebug,
            TypeWatchTree
        };
    public:
        static WatchRawDialog* Create(const WatchesProperty* watch)
        {
            cbAssert(watch->GetWatch());

            WatchRawDialog *dlg;
            const cbWatch *watchPtr = watch->GetWatch().get();
            Map::iterator it = s_dialogs.find(watchPtr);
            if (it != s_dialogs.end())
                dlg = it->second;
            else
            {
                dlg = new WatchRawDialog;
                s_dialogs[watchPtr] = dlg;
            }

            dlg->m_type = TypeNormal;

            if (wxGetKeyState(WXK_CONTROL))
                dlg->m_type = TypeDebug;
            else if (wxGetKeyState(WXK_SHIFT))
                dlg->m_type = TypeWatchTree;

            dlg->SetTitle(wxString::Format(wxT("Watch '%s' raw value"), watch->GetName().c_str()));
            dlg->SetValue(watch);
            dlg->Raise();

            return dlg;
        }

        static void UpdateValue(const WatchesProperty* watch)
        {
            Map::iterator it = s_dialogs.find(watch->GetWatch().get());
            if (it != s_dialogs.end())
                it->second->SetValue(watch);
        }
    private:
        WatchRawDialog() :
            wxScrollingDialog(Manager::Get()->GetAppWindow(),
                              wxID_ANY,
                              wxEmptyString,
                              wxDefaultPosition,
                              wxSize(400, 400),
                              wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
            m_type(TypeNormal)
        {
            wxBoxSizer *bs = new wxBoxSizer(wxVERTICAL);
            m_text = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                    wxTE_MULTILINE | wxTE_READONLY);
            bs->Add(m_text, 1, wxEXPAND | wxALL, 5);
            SetAutoLayout(TRUE);
            SetSizer(bs);
        }

        void OnClose(cb_unused wxCloseEvent &event)
        {
            for (Map::iterator it = s_dialogs.begin(); it != s_dialogs.end(); ++it)
            {
                if (it->second == this)
                {
                    s_dialogs.erase(it);
                    break;
                }
            }
            Destroy();
        }

        void SetValue(const WatchesProperty* watch)
        {
            switch (m_type)
            {
                case TypeNormal:
                    m_text->SetValue(watch->GetValueAsString(wxPG_FULL_VALUE));
                    break;

                case TypeDebug:
                    m_text->SetValue(watch->GetWatch()->GetDebugString());
                    break;

                case TypeWatchTree:
                    {
                        wxString value;
                        WatchToString(value, *watch->GetWatch());
                        m_text->SetValue(value);
                    }
                    break;
                default:
                    break;
            }
        }

        static void WatchToString(wxString &result, const cbWatch &watch, const wxString &indent = wxEmptyString)
        {
            wxString sym, value;
            watch.GetSymbol(sym);
            watch.GetValue(value);

            result += indent + wxT("[symbol = ") + sym + wxT("]\n");
            result += indent + wxT("[value = ") + value + wxT("]\n");
            result += indent + wxString::Format(wxT("[children = %d]\n"), watch.GetChildCount());

            for(int child_index = 0; child_index < watch.GetChildCount(); ++child_index)
            {
                cb::shared_ptr<const cbWatch> child = watch.GetChild(child_index);

                result += indent + wxString::Format(wxT("[child %d]\n"), child_index);
                WatchToString(result, *child, indent + wxT("    "));
            }
        }
    private:
        DECLARE_EVENT_TABLE()
    private:
        typedef std::map<cbWatch const*, WatchRawDialog*> Map;

        static Map s_dialogs;

        wxTextCtrl *m_text;
        Type        m_type;
};

WatchRawDialog::Map WatchRawDialog::s_dialogs;

BEGIN_EVENT_TABLE(WatchRawDialog, wxScrollingDialog)
    EVT_CLOSE(WatchRawDialog::OnClose)
END_EVENT_TABLE()


bool WatchRawDialogAdapter::DoShowDialog(wxPropertyGrid* WXUNUSED(propGrid), wxPGProperty* property)
{
    WatchesProperty *watch = static_cast<WatchesProperty*>(property);
    if (watch->GetWatch())
    {
        WatchRawDialog *dlg = WatchRawDialog::Create(watch);
        dlg->Show();
    }
    return false;
}

wxPGEditorDialogAdapter* WatchesProperty::GetEditorDialog() const
{
    return new WatchRawDialogAdapter();
}

class WatchesDropTarget : public wxTextDropTarget
{
public:
    virtual bool OnDropText(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y), const wxString& text)
    {
        cbDebuggerPlugin *activeDebugger = Manager::Get()->GetDebuggerManager()->GetActiveDebugger();
        if (!activeDebugger->SupportsFeature(cbDebuggerFeature::Watches))
            return false;
        cb::shared_ptr<cbWatch> watch = activeDebugger->AddWatch(text);
        if (watch.get())
            Manager::Get()->GetDebuggerManager()->GetWatchesDialog()->AddWatch(watch);
        // we return false here to veto the operation, otherwise the dragged text might get cut,
        // because we use wxDrag_DefaultMove in ScintillaWX::StartDrag (seems to happen only on windows)
        return false;
    }
    virtual wxDragResult OnDragOver(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y), wxDragResult WXUNUSED(def))
    {
        return wxDragCopy;
    }
private:
};

WatchesDlg::WatchesDlg() :
    wxPanel(Manager::Get()->GetAppWindow(), -1),
    m_append_empty_watch(false)
{
    wxBoxSizer *bs = new wxBoxSizer(wxVERTICAL);
    m_grid = new wxPropertyGrid(this, idGrid, wxDefaultPosition, wxDefaultSize,
                                wxPG_SPLITTER_AUTO_CENTER | wxTAB_TRAVERSAL /*| wxWANTS_CHARS*/);

#if wxCHECK_VERSION(2, 9, 0)
    #define wxPG_EX_DISABLE_TLP_TRACKING 0x00000000
#endif
    m_grid->SetExtraStyle(wxPG_EX_DISABLE_TLP_TRACKING | wxPG_EX_HELP_AS_TOOLTIPS);
    m_grid->SetDropTarget(new WatchesDropTarget);
    m_grid->SetColumnCount(3);
    bs->Add(m_grid, 1, wxEXPAND | wxALL);
    SetAutoLayout(TRUE);
    SetSizer(bs);

    if (!watchesPropertyEditor)
    {
#if wxCHECK_VERSION(2, 9, 0)
        watchesPropertyEditor = wxPropertyGrid::RegisterEditorClass(new cbTextCtrlAndButtonTooltipEditor, true);
#else
        watchesPropertyEditor = wxPropertyGrid::RegisterEditorClass(new cbTextCtrlAndButtonTooltipEditor,
                                                                    wxT("cbTextCtrlAndButtonTooltipEditor"),
                                                                    true);
#endif
    }

    m_grid->SetColumnProportion(0, 40);
    m_grid->SetColumnProportion(1, 40);
    m_grid->SetColumnProportion(2, 20);

    wxPGProperty *prop = m_grid->Append(new WatchesProperty(wxEmptyString, wxEmptyString,
                                                            cb::shared_ptr<cbWatch>(), false));
    m_grid->SetPropertyAttribute(prop, wxT("Units"), wxEmptyString);

    m_grid->Connect(idGrid, wxEVT_KEY_DOWN, wxKeyEventHandler(WatchesDlg::OnKeyDown), NULL, this);

    ColourManager *colours = Manager::Get()->GetColourManager();
    colours->RegisterColour(_("Debugger"), _("Watches changed value"), wxT("dbg_watches_changed"), *wxRED);
}

inline void AppendChildren(wxPropertyGrid &grid, wxPGProperty &property, cbWatch &watch,
                           bool readonly, const wxColour &changedColour)
{
    for(int ii = 0; ii < watch.GetChildCount(); ++ii)
    {
        cb::shared_ptr<cbWatch> child = watch.GetChild(ii);

        wxString symbol, value, type;
        child->GetSymbol(symbol);
        child->GetValue(value);
        child->GetType(type);

        wxPGProperty *prop = new WatchesProperty(symbol, value, child, readonly);
        prop->SetExpanded(child->IsExpanded());
        wxPGProperty *new_prop = grid.AppendIn(&property, prop);
        grid.SetPropertyAttribute(new_prop, wxT("Units"), type);
        if (value.empty())
            grid.SetPropertyHelpString(new_prop, wxEmptyString);
        else
            grid.SetPropertyHelpString(new_prop, symbol + wxT("=") + value);
        grid.EnableProperty(new_prop, grid.IsPropertyEnabled(&property));

        if (child->IsChanged())
        {
            grid.SetPropertyTextColour(prop, changedColour);
            WatchRawDialog::UpdateValue(static_cast<const WatchesProperty*>(prop));
        }
        else
        {
#if wxCHECK_VERSION(2, 9, 0)
            grid.SetPropertyColoursToDefault(prop);
#else
            grid.SetPropertyColourToDefault(prop);
#endif
        }

        AppendChildren(grid, *prop, *child.get(), readonly, changedColour);
    }
}

inline void UpdateWatch(wxPropertyGrid *grid, wxPGProperty *property, cb::shared_ptr<cbWatch> watch, bool readonly)
{
    if (!property)
        return;
    const wxColour &changedColour = Manager::Get()->GetColourManager()->GetColour(wxT("dbg_watches_changed"));

    wxString value, symbol, type;
    watch->GetSymbol(symbol);
    watch->GetValue(value);
    property->SetLabel(symbol);
    property->SetValue(value);
    property->SetExpanded(watch->IsExpanded());
    watch->GetType(type);
    if (watch->IsChanged())
        grid->SetPropertyTextColour(property, changedColour);
    else
    {
#if wxCHECK_VERSION(2, 9, 0)
        grid->SetPropertyColoursToDefault(property);
#else
        grid->SetPropertyColourToDefault(property);
#endif
    }
    grid->SetPropertyAttribute(property, wxT("Units"), type);
    if (value.empty())
        grid->SetPropertyHelpString(property, wxEmptyString);
    else
        grid->SetPropertyHelpString(property, symbol + wxT("=") + value);

    property->DeleteChildren();

    if (property->GetName() != symbol)
    {
        grid->SetPropertyName(property, symbol);
        grid->SetPropertyLabel(property, symbol);
    }

    AppendChildren(*grid, *property, *watch, readonly, changedColour);

    WatchRawDialog::UpdateValue(static_cast<const WatchesProperty*>(property));
}

inline void SetValue(WatchesProperty *prop)
{
    if (prop)
    {
        cb::shared_ptr<cbWatch> watch = prop->GetWatch();
        if (watch)
        {
            cbDebuggerPlugin *plugin = Manager::Get()->GetDebuggerManager()->GetDebuggerHavingWatch(watch);
            if (plugin)
                plugin->SetWatchValue(watch, prop->GetValue());
        }
    }
}

void WatchesDlg::UpdateWatches()
{
    for (WatchItems::iterator it = m_watches.begin(); it != m_watches.end(); ++it)
        UpdateWatch(m_grid, it->property, it->watch, it->readonly);
    m_grid->Refresh();
}

void WatchesDlg::AddWatch(cb::shared_ptr<cbWatch> watch)
{
    WatchesProperty *last_prop = static_cast<WatchesProperty*>(m_grid->GetLastItem(wxPG_ITERATE_DEFAULT));

    WatchItem item;
    wxString symbol, value;
    watch->GetSymbol(symbol);

    if (last_prop && last_prop->GetLabel() == wxEmptyString)
    {
        item.property = last_prop;

        // if we are editing the label the calls SetPropertyLabel and SetPropertyName don't work,
        // so we stop the edit operations
        if (m_grid->GetLabelEditor())
            m_grid->EndLabelEdit(0);
        m_grid->SetPropertyLabel(item.property, symbol);
        m_grid->SetPropertyName(item.property, symbol);

        WatchesProperty *watches_prop = static_cast<WatchesProperty*>(last_prop);
        watches_prop->SetWatch(watch);
        m_grid->Append(new WatchesProperty(wxEmptyString, wxEmptyString, cb::shared_ptr<cbWatch>(), false));
    }
    else
    {
        item.property = static_cast<WatchesProperty*>(m_grid->Append(new WatchesProperty(symbol, value, watch, false)));
    }

    item.property->SetExpanded(watch->IsExpanded());
    item.watch = watch;
    m_watches.push_back(item);
    m_grid->Refresh();
}

void WatchesDlg::AddSpecialWatch(cb::shared_ptr<cbWatch> watch, bool readonly)
{
    WatchItems::iterator it = std::find_if(m_watches.begin(), m_watches.end(), WatchItemPredicate(watch));
    if (it != m_watches.end())
        return;
    wxPGProperty *first_prop = m_grid->GetFirst();

    WatchItem item;
    wxString symbol, value;
    watch->GetSymbol(symbol);

    item.property = static_cast<WatchesProperty*>(m_grid->Insert(first_prop, new WatchesProperty(symbol, value, watch, true)));

    item.property->SetExpanded(watch->IsExpanded());
    item.watch = watch;
    item.readonly = readonly;
    item.special = true;
    m_watches.push_back(item);
    m_grid->Refresh();
}

void WatchesDlg::RemoveWatch(cb::shared_ptr<cbWatch> watch)
{
    WatchItems::iterator it = std::find_if(m_watches.begin(), m_watches.end(), WatchItemPredicate(watch));
    if (it != m_watches.end())
    {
        DeleteProperty(*it->property);
    }
}

void WatchesDlg::OnExpand(wxPropertyGridEvent &event)
{
    WatchesProperty *prop = static_cast<WatchesProperty*>(event.GetProperty());
    cb::shared_ptr<cbWatch> watch = prop->GetWatch();
    watch->Expand(true);

    cbDebuggerPlugin *plugin = Manager::Get()->GetDebuggerManager()->GetDebuggerHavingWatch(watch);
    if (plugin)
        plugin->ExpandWatch(watch);
}

void WatchesDlg::OnCollapse(wxPropertyGridEvent &event)
{
    WatchesProperty *prop = static_cast<WatchesProperty*>(event.GetProperty());
    cb::shared_ptr<cbWatch> watch = prop->GetWatch();
    watch->Expand(false);

    cbDebuggerPlugin *plugin = Manager::Get()->GetDebuggerManager()->GetDebuggerHavingWatch(watch);
    if (plugin)
        plugin->CollapseWatch(watch);
}

void WatchesDlg::OnPropertyChanged(wxPropertyGridEvent &event)
{
    WatchesProperty *prop = static_cast<WatchesProperty*>(event.GetProperty());
    SetValue(prop);
}

void WatchesDlg::OnPropertyChanging(wxPropertyGridEvent &event)
{
    if (event.GetProperty()->GetChildCount() > 0)
        event.Veto(true);
}

void WatchesDlg::OnPropertyLableEditBegin(wxPropertyGridEvent &event)
{
    wxPGProperty *prop = event.GetProperty();

    if (prop)
    {
        wxPGProperty *prop_parent = prop->GetParent();
        if (prop_parent && !prop_parent->IsRoot())
            event.Veto(true);
    }
}

void WatchesDlg::OnPropertyLableEditEnd(wxPropertyGridEvent &event)
{
    const wxString& label = m_grid->GetLabelEditor()->GetValue();
    RenameWatch(event.GetProperty(), label);
}

void WatchesDlg::OnIdle(cb_unused wxIdleEvent &event)
{
    if (m_append_empty_watch)
    {
        wxPGProperty *new_prop = m_grid->Append(new WatchesProperty(wxEmptyString, wxEmptyString,
                                                                    cb::shared_ptr<cbWatch>(), false));
        m_grid->SelectProperty(new_prop, true);
        m_grid->Refresh();
        m_append_empty_watch = false;
    }
}

void WatchesDlg::OnPropertySelected(wxPropertyGridEvent &event)
{
    cbDebuggerPlugin *plugin = Manager::Get()->GetDebuggerManager()->GetActiveDebugger();
    if (plugin && !plugin->SupportsFeature(cbDebuggerFeature::Watches))
        return;

    wxPGProperty *property = event.GetProperty();
    if (property && property->GetLabel() == wxEmptyString)
        m_grid->BeginLabelEdit(0);
}

void WatchesDlg::DeleteProperty(WatchesProperty &prop)
{
    cb::shared_ptr<cbWatch> watch = prop.GetWatch();
    if (!watch)
        return;

    cbDebuggerPlugin *debugger = Manager::Get()->GetDebuggerManager()->GetDebuggerHavingWatch(watch);
    if (debugger)
        debugger->DeleteWatch(watch);

    wxPGProperty *parent = prop.GetParent();
    if (parent && parent->IsRoot())
    {
        for (WatchItems::iterator it = m_watches.begin(); it != m_watches.end(); ++it)
        {
            if (!it->property)
                continue;
            if (it->property == &prop)
            {
                m_watches.erase(it);
                break;
            }
        }
        prop.DeleteChildren();
        m_grid->DeleteProperty(&prop);
    }
}

void WatchesDlg::OnKeyDown(wxKeyEvent &event)
{
    wxPGProperty *prop = m_grid->GetSelection();
    WatchesProperty *watches_prop = static_cast<WatchesProperty*>(prop);

    // don't delete the watch when editing the value or the label
    if (m_grid->IsEditorFocused() || m_grid->GetLabelEditor())
        return;

    if (!prop || !prop->GetParent() || !prop->GetParent()->IsRoot())
        return;

    switch (event.GetKeyCode())
    {
        case WXK_DELETE:
            {
                cb::shared_ptr<cbWatch> watch = watches_prop->GetWatch();
                cbDebuggerPlugin *plugin = Manager::Get()->GetDebuggerManager()->GetDebuggerHavingWatch(watch);
                if (plugin && plugin->SupportsFeature(cbDebuggerFeature::Watches))
                {
                    unsigned int index = watches_prop->GetIndexInParent();

                    DeleteProperty(*watches_prop);

                    wxPGProperty *root = m_grid->GetRoot();
                    if (index < root->GetChildCount())
                        m_grid->SelectProperty(root->Item(index), false);
                    else if (root->GetChildCount() > 0)
                        m_grid->SelectProperty(root->Item(root->GetChildCount() - 1), false);
                }
            }
            break;
        case WXK_INSERT:
            {
                cb::shared_ptr<cbWatch> watch = watches_prop->GetWatch();
                WatchItems::const_iterator it = std::find_if(m_watches.begin(), m_watches.end(),
                                                             WatchItemPredicate(watch));
                if (!(it != m_watches.end() && it->special))
                {
                    cbDebuggerPlugin *plugin = Manager::Get()->GetDebuggerManager()->GetDebuggerHavingWatch(watch);
                    if (plugin && plugin->SupportsFeature(cbDebuggerFeature::Watches))
                        m_grid->BeginLabelEdit(0);
                }
            }
            break;
        default:
            break;
    }
}

void WatchesDlg::OnPropertyRightClick(wxPropertyGridEvent &event)
{
    WatchesProperty *prop = static_cast<WatchesProperty*>(event.GetProperty());
    if (prop && prop->GetLabel()!=wxEmptyString)
    {
        wxMenu m;
        m.Append(idMenuRename, _("Rename"), _("Rename the watch"));
        m.Append(idMenuAddDataBreak, _("Add Data breakpoint"), _("Add Data breakpoing"));
        m.AppendSeparator();
        m.AppendCheckItem(idMenuAutoUpdate, _("Auto update"),
                          _("Flag which controls if this watch should be auto updated."));
        m.Append(idMenuUpdate, _("Update"), _("Manually update the selected watch."));
        m.AppendSeparator();
        m.Append(idMenuProperties, _("Properties"), _("Show the properties for the watch"));
        m.Append(idMenuDelete, _("Delete"), _("Delete the currently selected watch"));
        m.Append(idMenuDeleteAll, _("Delete All"), _("Delete all watches"));

        if (prop->GetLabel()==wxEmptyString)
            return;
        cb::shared_ptr<cbWatch> watch = prop->GetWatch();
        if (watch)
        {
            int disabled = cbDebuggerPlugin::WatchesDisabledMenuItems::Empty;
            DebuggerManager *dbgManager = Manager::Get()->GetDebuggerManager();
            cb::shared_ptr<cbWatch> rootWatch = cbGetRootWatch(watch);
            cbDebuggerPlugin *plugin = dbgManager->GetDebuggerHavingWatch(watch);

            WatchItems::const_iterator itItem = std::find_if(m_watches.begin(), m_watches.end(),
                                                             WatchItemPredicate(rootWatch));
            if (itItem != m_watches.end() && itItem->special)
            {
                disabled = cbDebuggerPlugin::WatchesDisabledMenuItems::Rename |
                           cbDebuggerPlugin::WatchesDisabledMenuItems::Properties |
                           cbDebuggerPlugin::WatchesDisabledMenuItems::Delete |
                           cbDebuggerPlugin::WatchesDisabledMenuItems::AddDataBreak;
            }
            else
            {

                if (plugin && plugin->SupportsFeature(cbDebuggerFeature::Watches))
                    plugin->OnWatchesContextMenu(m, *watch, event.GetProperty(), disabled);
                else
                {
                    disabled = cbDebuggerPlugin::WatchesDisabledMenuItems::Rename |
                               cbDebuggerPlugin::WatchesDisabledMenuItems::Properties |
                               cbDebuggerPlugin::WatchesDisabledMenuItems::Delete;
                }

                if (plugin != dbgManager->GetActiveDebugger())
                    disabled = cbDebuggerPlugin::WatchesDisabledMenuItems::Properties;
            }

            if (disabled & cbDebuggerPlugin::WatchesDisabledMenuItems::Rename)
                m.Enable(idMenuRename, false);
            if (disabled & cbDebuggerPlugin::WatchesDisabledMenuItems::Properties)
                m.Enable(idMenuProperties, false);
            if (disabled & cbDebuggerPlugin::WatchesDisabledMenuItems::Delete)
                m.Enable(idMenuDelete, false);
            if (disabled & cbDebuggerPlugin::WatchesDisabledMenuItems::DeleteAll)
                m.Enable(idMenuDeleteAll, false);
            if (disabled & cbDebuggerPlugin::WatchesDisabledMenuItems::AddDataBreak)
                m.Enable(idMenuAddDataBreak, false);

            if (rootWatch != watch)
            {
                m.Enable(idMenuAutoUpdate, false);
                m.Enable(idMenuUpdate, false);
            }
            else
            {
                m.Check(idMenuAutoUpdate, watch->IsAutoUpdateEnabled());
                if (plugin != dbgManager->GetActiveDebugger())
                    m.Enable(idMenuUpdate, false);
            }
        }
        PopupMenu(&m);
    }
    else
    {
        wxMenu m;
        m.Append(idMenuDelete, _("Delete"), _("Delete the currently selected watch"));
        m.Append(idMenuDeleteAll, _("Delete All"), _("Delete all watches"));
        PopupMenu(&m);
    }
}

void WatchesDlg::OnMenuRename(cb_unused wxCommandEvent &event)
{
    if (!m_grid->GetLabelEditor())
    {
        m_grid->SetFocus();
        m_grid->BeginLabelEdit(0);
    }
}

void WatchesDlg::OnMenuProperties(cb_unused wxCommandEvent &event)
{
    wxPGProperty *selected = m_grid->GetSelection();
    if (selected)
    {
        WatchesProperty *prop = static_cast<WatchesProperty*>(selected);
        cb::shared_ptr<cbWatch> watch = prop->GetWatch();
        if (watch)
        {
            cbDebuggerPlugin *debugger = Manager::Get()->GetDebuggerManager()->GetDebuggerHavingWatch(watch);
            if (debugger)
                debugger->ShowWatchProperties(watch);
        }
    }
}

void WatchesDlg::OnMenuDelete(cb_unused wxCommandEvent &event)
{
    wxPGProperty *selected = m_grid->GetSelection();
    if (selected)
    {
        WatchesProperty *prop = static_cast<WatchesProperty*>(selected);
        DeleteProperty(*prop);
    }
}

void WatchesDlg::OnMenuDeleteAll(cb_unused wxCommandEvent &event)
{
    WatchItems specialWatches;

    for (WatchItems::iterator it = m_watches.begin(); it != m_watches.end(); ++it)
    {
        if (it->special)
            specialWatches.push_back(*it);
        else
        {
            cbDebuggerPlugin *debugger = Manager::Get()->GetDebuggerManager()->GetDebuggerHavingWatch(it->watch);
            debugger->DeleteWatch(it->watch);
            m_grid->DeleteProperty(it->property);
        }
    }

    m_watches.swap(specialWatches);
}

void WatchesDlg::OnMenuAddDataBreak(cb_unused wxCommandEvent &event)
{
    wxPGProperty *selected = m_grid->GetSelection();
    if (!selected)
        return;
    WatchesProperty *prop = static_cast<WatchesProperty*>(selected);

    wxString expression;
    prop->GetWatch()->GetSymbol(expression);

    cbDebuggerPlugin *plugin = Manager::Get()->GetDebuggerManager()->GetActiveDebugger();
    if (plugin && !expression.empty())
    {
        if (plugin->AddDataBreakpoint(expression))
            Manager::Get()->GetDebuggerManager()->GetBreakpointDialog()->Reload();
    }
}

void WatchesDlg::OnMenuAutoUpdate(cb_unused wxCommandEvent &event)
{
    WatchesProperty *selected = static_cast<WatchesProperty*>(m_grid->GetSelection());
    if (!selected)
        return;
    cb::shared_ptr<cbWatch> watch = selected->GetWatch();
    if (!watch)
        return;
    watch->AutoUpdate(!watch->IsAutoUpdateEnabled());
}

// Must not be called on non-active debuggers!
void WatchesDlg::OnMenuUpdate(cb_unused wxCommandEvent &event)
{
    WatchesProperty *selected = static_cast<WatchesProperty*>(m_grid->GetSelection());
    if (!selected)
        return;
    cb::shared_ptr<cbWatch> watch = selected->GetWatch();
    if (!watch)
        return;
    watch = cbGetRootWatch(watch);
    cbDebuggerPlugin *plugin = Manager::Get()->GetDebuggerManager()->GetActiveDebugger();
    if (plugin)
        plugin->UpdateWatch(watch);
}

void WatchesDlg::RenameWatch(wxObject *prop, const wxString &newSymbol)
{
    cbDebuggerPlugin *active_plugin = Manager::Get()->GetDebuggerManager()->GetActiveDebugger();
    if (!active_plugin)
        return;
    WatchesProperty *watchesProp = static_cast<WatchesProperty*>(prop);
    if (newSymbol == wxEmptyString || !watchesProp)
        return;

    // if the user have edited existing watch, we replace it. The new watch is added to the active plugin.
    if (watchesProp->GetWatch())
    {
        cb::shared_ptr<cbWatch> old_watch = watchesProp->GetWatch();
        cbDebuggerPlugin *old_plugin = Manager::Get()->GetDebuggerManager()->GetDebuggerHavingWatch(old_watch);
        watchesProp->SetWatch(cb::shared_ptr<cbWatch>());
        old_plugin->DeleteWatch(old_watch);
        cb::shared_ptr<cbWatch> new_watch = active_plugin->AddWatch(newSymbol);
        watchesProp->SetWatch(new_watch);

        for (WatchItems::iterator it = m_watches.begin(); it != m_watches.end(); ++it)
        {
            if (it->property == watchesProp)
                it->watch = new_watch;
        }
        watchesProp->SetExpanded(new_watch->IsExpanded());
        ::UpdateWatch(m_grid, watchesProp, new_watch, false);
        m_grid->Refresh();
    }
    else
    {
        WatchItem item;
        item.property = watchesProp;
        item.watch = active_plugin->AddWatch(newSymbol);
        watchesProp->SetWatch(item.watch);
        m_watches.push_back(item);
        watchesProp->SetExpanded(item.watch->IsExpanded());

        m_append_empty_watch = true;
    }
}

void WatchesDlg::RefreshUI()
{
    DebuggerManager *manager = Manager::Get()->GetDebuggerManager();
    cbDebuggerPlugin *active = manager->GetActiveDebugger();

    for (WatchItems::iterator it = m_watches.begin(); it != m_watches.end(); )
    {
        cbDebuggerPlugin *plugin = manager->GetDebuggerHavingWatch(it->watch);
        if (plugin)
        {
            bool supports = plugin->SupportsFeature(cbDebuggerFeature::Watches);
            WatchesProperty *prop = static_cast<WatchesProperty*>(it->property);

            if (!supports || plugin != active)
                m_grid->DisableProperty(prop);
            else
                m_grid->EnableProperty(prop);
            ++it;
        }
        else
        {
            m_grid->RemoveProperty(it->property);
            it = m_watches.erase(it);
        }
    }
}

//////////////////////////////////////////////////////////////////
//////////// ValueTooltip implementation /////////////////////////
//////////////////////////////////////////////////////////////////

#ifndef __WXMAC__
IMPLEMENT_CLASS(ValueTooltip, wxPopupWindow)
BEGIN_EVENT_TABLE(ValueTooltip, wxPopupWindow)
#else
IMPLEMENT_CLASS(ValueTooltip, wxWindow)
BEGIN_EVENT_TABLE(ValueTooltip, wxWindow)
#endif
    EVT_PG_ITEM_COLLAPSED(idTooltipGrid, ValueTooltip::OnCollapse)
    EVT_PG_ITEM_EXPANDED(idTooltipGrid, ValueTooltip::OnExpand)
    EVT_TIMER(idTooltipTimer, ValueTooltip::OnTimer)
END_EVENT_TABLE()

inline wxPGProperty* GetRealRoot(wxPropertyGrid *grid)
{
    wxPGProperty *property = grid->GetRoot();
    return property ? grid->GetFirstChild(property) : nullptr;
}

inline void GetColumnWidths(wxClientDC &dc, wxPropertyGrid *grid, wxPGProperty *root, int width[3])
{
#if wxCHECK_VERSION(2, 9, 0)
    wxPropertyGridPageState *state = grid->GetState();
#else
    wxPropertyGridState *state = grid->GetState();
#endif

    width[0] = width[1] = width[2] = 0;
    int minWidths[3] = { state->GetColumnMinWidth(0),
                         state->GetColumnMinWidth(1),
                         state->GetColumnMinWidth(2) };

#if !wxCHECK_VERSION(2,9,0)
    for (unsigned ii = 0; ii < root->GetChildCount(); ++ii)
    {
        wxPGProperty* p = root->Item(ii);

        width[0] = std::max(width[0], state->GetColumnFullWidth(dc, p, 0));
        width[1] = std::max(width[1], state->GetColumnFullWidth(dc, p, 1));
        width[2] = std::max(width[2], state->GetColumnFullWidth(dc, p, 2));
    }
    for (unsigned ii = 0; ii < root->GetCount(); ++ii)
    {
        wxPGProperty* p = root->Item(ii);
        if (p->IsExpanded())
        {
            int w[3];
            GetColumnWidths(dc, grid, p, w);
            width[0] = std::max(width[0], w[0]);
            width[1] = std::max(width[1], w[1]);
            width[2] = std::max(width[2], w[2]);
        }
    }
#endif

    width[0] = std::max(width[0], minWidths[0]);
    width[1] = std::max(width[1], minWidths[1]);
    width[2] = std::max(width[2], minWidths[2]);
}

inline void GetColumnWidths(wxPropertyGrid *grid, wxPGProperty *root, int width[3])
{
    wxClientDC dc(grid);
    dc.SetFont(grid->GetFont());
    GetColumnWidths(dc, grid, root, width);
}

inline void SetMinSize(wxPropertyGrid *grid)
{
    wxPGProperty *p = GetRealRoot(grid);
    wxPGProperty *first = grid->wxPropertyGridInterface::GetFirst(wxPG_ITERATE_ALL);
    wxPGProperty *last = grid->GetLastItem(wxPG_ITERATE_DEFAULT);
    wxRect rect = grid->GetPropertyRect(first, last);
    int height = rect.height + 2 * grid->GetVerticalSpacing();

    // add some height when the root item is collapsed,
    // this is needed to prevent the vertical scroll from showing
    if (!grid->IsPropertyExpanded(p))
        height += 2 * grid->GetVerticalSpacing();

    int width[3];
    GetColumnWidths(grid, grid->GetRoot(), width);
    rect.width = std::accumulate(width, width+3, 0);

    int minWidth = (wxSystemSettings::GetMetric(wxSYS_SCREEN_X, grid->GetParent())*3)/2;
    int minHeight = (wxSystemSettings::GetMetric(wxSYS_SCREEN_Y, grid->GetParent())*3)/2;

#if wxCHECK_VERSION(2, 9, 0)
    wxSize size(std::min(minWidth, rect.width), std::min(minHeight, height));
#else
    wxSize size(std::min(minWidth, rect.width + grid->GetMarginWidth()), std::min(minHeight, height));
#endif
    grid->SetMinSize(size);

    int proportions[3];
    proportions[0] = static_cast<int>(floor((double)width[0]/size.x*100.0+0.5));
    proportions[1] = static_cast<int>(floor((double)width[1]/size.x*100.0+0.5));
    proportions[2]= std::max(100 - proportions[0] - proportions[1], 0);
    grid->SetColumnProportion(0, proportions[0]);
    grid->SetColumnProportion(1, proportions[1]);
    grid->SetColumnProportion(2, proportions[2]);
    grid->ResetColumnSizes(true);
}

ValueTooltip::ValueTooltip(const cb::shared_ptr<cbWatch> &watch, wxWindow *parent) :
#ifndef __WXMAC__
    wxPopupWindow(parent, wxBORDER_NONE|wxWANTS_CHARS),
#else
    wxWindow(parent, -1),
#endif
    m_timer(this, idTooltipTimer),
    m_outsideCount(0),
    m_watch(watch)
{
    m_panel = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxSize(200, 200));
    m_grid = new wxPropertyGrid(m_panel, idTooltipGrid, wxDefaultPosition, wxSize(400,400), wxPG_SPLITTER_AUTO_CENTER);

    m_grid->SetExtraStyle(wxPG_EX_DISABLE_TLP_TRACKING /*| wxPG_EX_HELP_AS_TOOLTIPS*/);
    m_grid->SetDropTarget(new WatchesDropTarget);

    wxNativeFontInfo fontInfo;
    fontInfo.FromString(cbDebuggerCommonConfig::GetValueTooltipFont());
    wxFont font(fontInfo);
    m_grid->SetFont(font);

    m_grid->SetColumnCount(3);

    wxString symbol, value;
    m_watch->GetSymbol(symbol);
    m_watch->GetValue(value);
    wxPGProperty *root = m_grid->Append(new WatchesProperty(symbol, value, m_watch, true));
    m_watch->MarkAsChangedRecursive(false);
    ::UpdateWatch(m_grid, root, m_watch, true);

    ::SetMinSize(m_grid);

    m_sizer = new wxBoxSizer( wxVERTICAL );
    m_sizer->Add(m_grid, 0, wxALL | wxEXPAND, 0);

    m_panel->SetAutoLayout(true);
    m_panel->SetSizer(m_sizer);
    m_sizer->Fit(m_panel);
    m_sizer->Fit(this);

    m_timer.Start(100);
}

ValueTooltip::~ValueTooltip()
{
    ClearWatch();
}

void ValueTooltip::UpdateWatch()
{
    m_watch->MarkAsChangedRecursive(false);
    ::UpdateWatch(m_grid, GetRealRoot(m_grid), m_watch, true);
    m_grid->Refresh();
    Fit();
}

void ValueTooltip::ClearWatch()
{
    if (m_watch)
    {
        cbDebuggerPlugin *plugin = Manager::Get()->GetDebuggerManager()->GetDebuggerHavingWatch(m_watch);
        if (plugin)
            plugin->DeleteWatch(m_watch);
        m_watch.reset();
    }
}

void ValueTooltip::Dismiss()
{
    Hide();
    m_timer.Stop();
    ClearWatch();
}

void ValueTooltip::OnDismiss()
{
    ClearWatch();
}

void ValueTooltip::Fit()
{
    ::SetMinSize(m_grid);
    m_sizer->Fit(m_panel);
    wxPoint pos = GetScreenPosition();
    wxSize size = m_panel->GetScreenRect().GetSize();
    SetSize(pos.x, pos.y, size.x, size.y);
}

void ValueTooltip::OnCollapse(wxPropertyGridEvent &event)
{
    WatchesProperty *prop = static_cast<WatchesProperty*>(event.GetProperty());
    prop->GetWatch()->Expand(false);

    cbDebuggerPlugin *plugin = Manager::Get()->GetDebuggerManager()->GetActiveDebugger();
    if (plugin)
        plugin->CollapseWatch(prop->GetWatch());
    Fit();
}

void ValueTooltip::OnExpand(wxPropertyGridEvent &event)
{
    WatchesProperty *prop = static_cast<WatchesProperty*>(event.GetProperty());
    prop->GetWatch()->Expand(true);

    cbDebuggerPlugin *plugin = Manager::Get()->GetDebuggerManager()->GetActiveDebugger();
    if (plugin)
        plugin->ExpandWatch(prop->GetWatch());
    Fit();
}

void ValueTooltip::OnTimer(cb_unused wxTimerEvent &event)
{
    if (!wxTheApp->IsActive())
        Dismiss();

    wxPoint mouse = wxGetMousePosition();
    wxRect rect = GetScreenRect();
    rect.Inflate(5);

    if (!rect.Contains(mouse))
    {
        if (++m_outsideCount > 5)
            Dismiss();
    }
    else
        m_outsideCount = 0;
}

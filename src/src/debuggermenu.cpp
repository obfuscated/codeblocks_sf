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
    #include <wx/menu.h>
    #include <wx/textdlg.h>
    #include <wx/toolbar.h>
    #include <wx/xrc/xmlres.h>

    #include "cbeditor.h"
    #include "cbproject.h"
    #include "editormanager.h"
    #include "logmanager.h"
    #include "projectmanager.h"
#endif

#include "debuggermenu.h"

#include <algorithm>
#include <wx/aui/aui.h> // wxAuiManager

#include "cbdebugger_interfaces.h"
#include "cbstyledtextctrl.h"
#include "debuggermanager.h"

namespace
{
    const int idMenuDebug = XRCID("idDebuggerMenuDebug");
    const int idMenuDebugActive = XRCID("idDebuggerMenuActive");
    const int idMenuDebugActiveTargetsDefault = wxNewId();
    const int idMenuRunToCursor = XRCID("idDebuggerMenuRunToCursor");
    const int idMenuSetNextStatement = XRCID("idDebuggerMenuSetNextStatement");
    const int idMenuNext = XRCID("idDebuggerMenuNext");
    const int idMenuStep = XRCID("idDebuggerMenuStep");
    const int idMenuNextInstr = XRCID("idDebuggerMenuNextInstr");
    const int idMenuStepIntoInstr = XRCID("idDebuggerMenuStepIntoInstr");
    const int idMenuStepOut = XRCID("idDebuggerMenuStepOut");
    const int idMenuBreak = XRCID("idDebuggerMenuBreak");
    const int idMenuStop = XRCID("idDebuggerMenuStop");
    const int idToolbarStop = XRCID("idDebuggerToolbarStop");
    const int idMenuToggleBreakpoint = XRCID("idDebuggerMenuToggleBreakpoint");
    const int idMenuRemoveAllBreakpoints = XRCID("idDebuggerMenuRemoveAllBreakpoints");
    const int idMenuAddDataBreakpoint = XRCID("idMenuAddDataBreakpoint");
    const int idMenuSendCommand = XRCID("idDebuggerMenuSendCommand");
    const int idMenuAddSymbolFile = XRCID("idDebuggerMenuAddSymbolFile");
    const int idMenuAttachToProcess = XRCID("idDebuggerMenuAttachToProcess");
    const int idMenuDetach = XRCID("idDebuggerMenuDetach");

    const long idMenuDebuggingWindows = XRCID("idDebuggingWindows");
    const long idMenuTools = XRCID("idDebuggerInfo");

    const int idDebuggerToolInfo = XRCID("idDebuggerToolInfo");
    const int idDebuggerToolWindows = XRCID("idDebuggerToolWindows");

    const int idMenuDebuggerAddWatch = wxNewId();

    inline void HideValueTooltip() { Manager::Get()->GetDebuggerManager()->GetInterfaceFactory()->HideValueTooltip(); }

    bool Support(cbDebuggerPlugin *plugin, cbDebuggerFeature::Flags flag)
    {
        return plugin && plugin->SupportsFeature(flag);
    }

    wxMenu* GetMenuById(long menuId, bool recreate = false)
    {
        wxMenuBar* mbar = Manager::Get()->GetAppFrame()->GetMenuBar();
        if (!mbar)
            return nullptr;
        wxMenuItem *item = mbar->FindItem(menuId);
        if (!item)
            return nullptr;
        if (recreate)
        {
            wxMenu *menu = item->GetMenu();
            int pos = wxNOT_FOUND;
            for (size_t ii = 0; ii < menu->GetMenuItemCount(); ++ii)
            {
                if (item == menu->FindItemByPosition(ii))
                {
                    pos = ii;
                    break;
                }
            }
            if (pos != wxNOT_FOUND)
            {
                wxMenu *newSubMenu = new wxMenu;
                wxMenuItem *newItem = new wxMenuItem(menu, item->GetId(), item->GetItemLabelText(), item->GetHelp(),
                                                     item->GetKind(), newSubMenu);
                menu->Insert(pos, newItem);

                menu->Destroy(item);
                return newItem->GetSubMenu();
            }
        }
        return item ? item->GetSubMenu() : nullptr;
    }
}

BEGIN_EVENT_TABLE(DebuggerMenuHandler, wxEvtHandler)
    // these are different because they are loaded from the XRC
    EVT_UPDATE_UI(idMenuDebug, DebuggerMenuHandler::OnUpdateUI)
    EVT_UPDATE_UI(idMenuRunToCursor, DebuggerMenuHandler::OnUpdateUI)
    EVT_UPDATE_UI(idMenuSetNextStatement, DebuggerMenuHandler::OnUpdateUI)
    EVT_UPDATE_UI(idMenuNext, DebuggerMenuHandler::OnUpdateUI)
    EVT_UPDATE_UI(idMenuNextInstr, DebuggerMenuHandler::OnUpdateUI)
    EVT_UPDATE_UI(idMenuStepIntoInstr, DebuggerMenuHandler::OnUpdateUI)
    EVT_UPDATE_UI(idMenuStep, DebuggerMenuHandler::OnUpdateUI)
    EVT_UPDATE_UI(idMenuStepOut, DebuggerMenuHandler::OnUpdateUI)
    EVT_UPDATE_UI(idMenuBreak, DebuggerMenuHandler::OnUpdateUI)
    EVT_UPDATE_UI(idMenuStop, DebuggerMenuHandler::OnUpdateUI)

    EVT_UPDATE_UI(idMenuAttachToProcess, DebuggerMenuHandler::OnUpdateUI)
    EVT_UPDATE_UI(idMenuDetach, DebuggerMenuHandler::OnUpdateUI)

    EVT_UPDATE_UI(idMenuTools, DebuggerMenuHandler::OnUpdateUI)

    EVT_MENU(idMenuDebug, DebuggerMenuHandler::OnStart)
    EVT_MENU(idMenuBreak, DebuggerMenuHandler::OnBreak)
    EVT_MENU(idMenuStop, DebuggerMenuHandler::OnStop)
    EVT_MENU(idMenuNext, DebuggerMenuHandler::OnNext)
    EVT_MENU(idMenuStep, DebuggerMenuHandler::OnStep)
    EVT_MENU(idMenuNextInstr, DebuggerMenuHandler::OnNextInstr)
    EVT_MENU(idMenuStepIntoInstr, DebuggerMenuHandler::OnStepIntoInstr)
    EVT_MENU(idMenuStepOut, DebuggerMenuHandler::OnStepOut)
    EVT_MENU(idMenuRunToCursor, DebuggerMenuHandler::OnRunToCursor)
    EVT_MENU(idMenuSetNextStatement, DebuggerMenuHandler::OnSetNextStatement)
    EVT_MENU(idMenuToggleBreakpoint, DebuggerMenuHandler::OnToggleBreakpoint)
    EVT_MENU(idMenuRemoveAllBreakpoints, DebuggerMenuHandler::OnRemoveAllBreakpoints)
    EVT_MENU(idMenuAddDataBreakpoint, DebuggerMenuHandler::OnAddDataBreakpoint)
    EVT_MENU(idMenuSendCommand, DebuggerMenuHandler::OnSendCommand)

    EVT_MENU(idMenuDebuggerAddWatch, DebuggerMenuHandler::OnAddWatch)
    EVT_MENU(idMenuAttachToProcess, DebuggerMenuHandler::OnAttachToProcess)
    EVT_MENU(idMenuDetach, DebuggerMenuHandler::OnDetachFromProcess)
    EVT_MENU(idMenuDebugActiveTargetsDefault, DebuggerMenuHandler::OnActiveDebuggerTargetsDefaultClick)
END_EVENT_TABLE()


DebuggerMenuHandler::DebuggerMenuHandler() :
    m_activeDebugger(NULL),
    m_disableContinue(false)
{
}

namespace
{
template<typename DlgType>
struct CommonItem : cbDebuggerWindowMenuItem
{
    typedef DlgType* (DebuggerManager::*GetWindowFunc)();
    CommonItem(cbDebuggerFeature::Flags enableFeature, cbDebuggerPlugin::DebugWindows requestUpdate, GetWindowFunc func) :
        m_enableFeature(enableFeature),
        m_requestUpdate(requestUpdate),
        m_getWindowFunc(func)
    {}

    void OnClick(bool enable)
    {
        CodeBlocksDockEvent evt(enable ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
        DebuggerManager *manager = Manager::Get()->GetDebuggerManager();
        DlgType *dialog = (manager->*(m_getWindowFunc))();
        if (dialog)
        {
            evt.pWindow = dialog->GetWindow();
            Manager::Get()->ProcessEvent(evt);
        }

        if (enable && manager->GetActiveDebugger())
            manager->GetActiveDebugger()->RequestUpdate(m_requestUpdate);
    }
    virtual bool IsEnabled()
    {
        return Support(Manager::Get()->GetDebuggerManager()->GetActiveDebugger(), m_enableFeature);
    }
    virtual bool IsChecked()
    {
        DlgType *dialog = (Manager::Get()->GetDebuggerManager()->*(m_getWindowFunc))();
        return dialog && IsWindowReallyShown(dialog->GetWindow());
    }
private:
    cbDebuggerFeature::Flags m_enableFeature;
    cbDebuggerPlugin::DebugWindows m_requestUpdate;
    GetWindowFunc m_getWindowFunc;
};

template<typename DlgType>
CommonItem<DlgType>* MakeItem(cbDebuggerFeature::Flags enableFeature,
                              cbDebuggerPlugin::DebugWindows requestUpdate,
                              DlgType* (DebuggerManager::*func)())
{
    return new CommonItem<DlgType>(enableFeature, requestUpdate, func);
}

}

void DebuggerMenuHandler::RegisterDefaultWindowItems()
{
    struct Breakpoints : cbDebuggerWindowMenuItem
    {
        virtual void OnClick(bool enable)
        {
            CodeBlocksDockEvent evt(enable ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
            cbBreakpointsDlg *dialog = Manager::Get()->GetDebuggerManager()->GetBreakpointDialog();
            if (dialog)
            {
                evt.pWindow = dialog->GetWindow();
                Manager::Get()->ProcessEvent(evt);
            }
        }
        virtual bool IsEnabled()
        {
            return Manager::Get()->GetDebuggerManager()->GetBreakpointDialog();
        }
        virtual bool IsChecked()
        {
            cbBreakpointsDlg *dialog = Manager::Get()->GetDebuggerManager()->GetBreakpointDialog();
            return dialog && IsWindowReallyShown(dialog->GetWindow());
        }
    };
    struct Watches : CommonItem<cbWatchesDlg>
    {
        Watches() :
            CommonItem<cbWatchesDlg>(cbDebuggerFeature::Watches, cbDebuggerPlugin::Watches, &DebuggerManager::GetWatchesDialog)
        {
        }
        virtual bool IsEnabled()
        {
            return Manager::Get()->GetDebuggerManager()->GetWatchesDialog();
        }
    };

    RegisterWindowMenu(_("Breakpoints"), _("Edit breakpoints"), new Breakpoints);
    RegisterWindowMenu(_("Watches"), _("Watch variables"), new Watches);
    RegisterWindowMenu(_("Call stack"), _("Displays the current call stack"),
                       MakeItem(cbDebuggerFeature::Callstack, cbDebuggerPlugin::Backtrace,
                                &DebuggerManager::GetBacktraceDialog));
    RegisterWindowMenu(_("CPU Registers"), _("Display the CPU registers"),
                       MakeItem(cbDebuggerFeature::CPURegisters, cbDebuggerPlugin::CPURegisters,
                                &DebuggerManager::GetCPURegistersDialog));
    RegisterWindowMenu(_("Disassembly"), _("Disassembles the current stack frame"),
                       MakeItem(cbDebuggerFeature::Disassembly, cbDebuggerPlugin::Disassembly,
                                &DebuggerManager::GetDisassemblyDialog));
    RegisterWindowMenu(_("Memory dump"), _("Displays the contents of a memory location"),
                       MakeItem(cbDebuggerFeature::ExamineMemory, cbDebuggerPlugin::ExamineMemory,
                                &DebuggerManager::GetExamineMemoryDialog));
    RegisterWindowMenu(_("Running threads"),
                       _("Displays the currently running threads and allows switching between them"),
                       MakeItem(cbDebuggerFeature::Threads, cbDebuggerPlugin::Threads,
                                &DebuggerManager::GetThreadsDialog));
}

bool DebuggerMenuHandler::RegisterWindowMenu(const wxString &name, const wxString &help, cbDebuggerWindowMenuItem *item)
{
    for (WindowMenuItemsMap::iterator it = m_windowMenuItems.begin(); it != m_windowMenuItems.end(); ++it)
    {
        if (it->second.name == name)
        {
            wxString msg = wxString::Format(_("Duplicate debugger window name '%s'. Igrnoring it."), name.wx_str());
            Manager::Get()->GetLogManager()->DebugLog(msg, Logger::error);
            delete item;
            return false;
        }
    }

    WindowMenuItem i;
    i.item = cb::shared_ptr<cbDebuggerWindowMenuItem>(item);
    i.name = name;
    i.help = help;
    long id = wxNewId();

    m_windowMenuItems[id] = i;

    Connect(id, wxEVT_UPDATE_UI, wxObjectEventFunction(&DebuggerMenuHandler::OnWindowMenuItemUpdateUI));
    Connect(id, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(DebuggerMenuHandler::OnWindowMenuItemClicked));
    return true;
}

void DebuggerMenuHandler::UnregisterWindowMenu(const wxString &name)
{
    for (WindowMenuItemsMap::iterator it = m_windowMenuItems.begin(); it != m_windowMenuItems.end(); ++it)
    {
        if (it->second.name == name)
        {
            Disconnect(it->first, wxEVT_UPDATE_UI);
            Disconnect(it->first, wxEVT_COMMAND_MENU_SELECTED);

            m_windowMenuItems.erase(it);
            return;
        }
    }
}

void DebuggerMenuHandler::OnWindowMenuItemUpdateUI(wxUpdateUIEvent& event)
{
    WindowMenuItemsMap::iterator it = m_windowMenuItems.find(event.GetId());
    if (it != m_windowMenuItems.end())
    {
        event.Check(it->second.item->IsChecked());
        event.Enable(it->second.item->IsEnabled());
    }
}

void DebuggerMenuHandler::OnWindowMenuItemClicked(wxCommandEvent &event)
{
    WindowMenuItemsMap::iterator it = m_windowMenuItems.find(event.GetId());
    if (it != m_windowMenuItems.end())
        it->second.item->OnClick(event.IsChecked());
}

void DebuggerMenuHandler::AppendWindowMenuItems(wxMenu &menu)
{
    std::map<wxString, long> sortedNames;

    for (WindowMenuItemsMap::iterator it = m_windowMenuItems.begin(); it != m_windowMenuItems.end(); ++it)
        sortedNames[it->second.name] = it->first;

    for (std::map<wxString, long>::iterator it = sortedNames.begin(); it != sortedNames.end(); ++it)
        menu.AppendCheckItem(it->second, it->first, m_windowMenuItems[it->second].help);
}

void DebuggerMenuHandler::SetActiveDebugger(cbDebuggerPlugin *active)
{
    m_activeDebugger = active;
}

void DebuggerMenuHandler::MarkActiveTargetAsValid(bool valid)
{
    wxMenu *menu = GetMenuById(idMenuDebugActive);
    if (!menu)
        return;
    wxMenuItem *item = menu->FindItem(idMenuDebugActiveTargetsDefault);
    if (item)
#if wxCHECK_VERSION(2, 9, 0)
        item->SetItemLabel(valid ? _("Target's default") : _("Target's default (invalid)"));
#else
        item->SetText(valid ? _("Target's default") : _("Target's default (invalid)"));
#endif
}

void DebuggerMenuHandler::RebuildMenus()
{
    wxMenu *menuWindows = GetMenuById(idMenuDebuggingWindows, true);
    if (menuWindows)
        AppendWindowMenuItems(*menuWindows);
    if (m_activeDebugger)
    {
        wxMenu *menuTools = GetMenuById(idMenuTools, true);
        if (menuTools)
            m_activeDebugger->SetupToolsMenu(*menuTools);
    }

    DebuggerManager *dbgManager = Manager::Get()->GetDebuggerManager();
    wxMenu *menu = GetMenuById(idMenuDebugActive, true);
    if (!menu)
        return;

    menu->AppendRadioItem(idMenuDebugActiveTargetsDefault, _("Target's default"),
                          _("Use the debugger associated with the compiler for the active target"));

    DebuggerManager::RegisteredPlugins &allDebugger = dbgManager->GetAllDebuggers();
    for (DebuggerManager::RegisteredPlugins::iterator it = allDebugger.begin(); it != allDebugger.end(); ++it)
    {
        DebuggerManager::ConfigurationVector &configs = it->second.GetConfigurations();
        for (DebuggerManager::ConfigurationVector::iterator itConf = configs.begin(); itConf != configs.end(); ++itConf)
        {
            long id = (*itConf)->GetMenuId();
            if (id == wxID_ANY)
            {
                id = wxNewId();
                (*itConf)->SetMenuId(id);
            }

            menu->AppendRadioItem(id, it->first->GetGUIName() + wxT(": ") + (*itConf)->GetName());
            Connect(id, -1, wxEVT_COMMAND_MENU_SELECTED,
                    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                    &DebuggerMenuHandler::OnActiveDebuggerClick);
        }
    }

    if (m_activeDebugger && !dbgManager->IsActiveDebuggerTargetsDefault())
    {
        DebuggerManager::RegisteredPlugins::const_iterator it = allDebugger.find(m_activeDebugger);
        cbAssert(it != allDebugger.end());

        const DebuggerManager::ConfigurationVector &configs = it->second.GetConfigurations();

        DebuggerManager::ConfigurationVector::const_iterator itConf = configs.begin();
        std::advance(itConf, m_activeDebugger->GetIndexOfActiveConfig());

        if (itConf != configs.end())
            menu->Check((*itConf)->GetMenuId(), true);
        else
            menu->Check(configs.front()->GetMenuId(), true);
    }
    else
        menu->Check(idMenuDebugActiveTargetsDefault, true);
}

void DebuggerMenuHandler::BuildContextMenu(wxMenu &menu, const wxString& word_at_caret, bool is_running)
{
    cbDebuggerPlugin *plugin = Manager::Get()->GetDebuggerManager()->GetActiveDebugger();
    if (!plugin)
        return;

    int item = 0;
    // Insert Run to Cursor
    if (plugin->SupportsFeature(cbDebuggerFeature::RunToCursor))
        menu.Insert(item++, idMenuRunToCursor, _("Run to cursor"));
    if (is_running)
    {
        if (plugin->SupportsFeature(cbDebuggerFeature::SetNextStatement))
            menu.Insert(item++, idMenuSetNextStatement, _("Set next statement"));
        if (item > 0)
            menu.InsertSeparator(item++);
        if (!word_at_caret.empty())
        {
            if (plugin->SupportsFeature(cbDebuggerFeature::Watches))
                menu.Insert(item++, idMenuDebuggerAddWatch, wxString::Format(_("Watch '%s'"), word_at_caret.c_str()));
            // data breakpoint
            if (plugin->SupportsFeature(cbDebuggerFeature::Breakpoints))
            {
                menu.Insert(item++, idMenuAddDataBreakpoint,
                            wxString::Format(_("Add data breakpoint for '%s'"), word_at_caret.c_str()));
            }
        }
    }
    // Insert toggle breakpoint
    if (plugin->SupportsFeature(cbDebuggerFeature::Breakpoints))
        menu.Insert(item++, idMenuToggleBreakpoint, _("Toggle breakpoint"));
    if (item > 0)
        menu.InsertSeparator(item++);
}


void DebuggerMenuHandler::OnUpdateUI(wxUpdateUIEvent& event)
{
    cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
    bool en = false, stopped = false, isRunning = false, isAttached = false;

    if (m_activeDebugger)
    {
        isAttached = m_activeDebugger->IsAttachedToProcess();
        en = (prj && !prj->GetCurrentlyCompilingTarget()) || isAttached;
        stopped = m_activeDebugger->IsStopped() && !m_activeDebugger->IsBusy();
        isRunning = m_activeDebugger->IsRunning();
    }

    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    wxMenuBar* mbar = Manager::Get()->GetAppFrame()->GetMenuBar();
    cbPlugin *runningPlugin = Manager::Get()->GetProjectManager()->GetIsRunning();

    bool otherPlugin = false;
    if (runningPlugin != NULL && runningPlugin != m_activeDebugger)
    {
        en = false;
        otherPlugin = true;
    }

    if (mbar)
    {
        bool hasBreaks = Support(m_activeDebugger, cbDebuggerFeature::Breakpoints);

        mbar->Enable(idMenuDebug, (!isRunning || stopped) && en);
        mbar->Enable(idMenuNext, isRunning && en && stopped);
        mbar->Enable(idMenuNextInstr, isRunning && en && stopped);
        mbar->Enable(idMenuStepIntoInstr, isRunning && en && stopped);
        mbar->Enable(idMenuStep, en && stopped);
        mbar->Enable(idMenuStepOut, isRunning && en && stopped);
        mbar->Enable(idMenuRunToCursor,
                     en && ed && stopped && Support(m_activeDebugger, cbDebuggerFeature::RunToCursor));
        mbar->Enable(idMenuSetNextStatement,
                     en && ed && stopped && isRunning && Support(m_activeDebugger, cbDebuggerFeature::SetNextStatement));
        mbar->Enable(idMenuToggleBreakpoint, ed && m_activeDebugger && hasBreaks);
        mbar->Enable(idMenuRemoveAllBreakpoints, m_activeDebugger && hasBreaks);
        mbar->Enable(idMenuSendCommand, isRunning && stopped);
        mbar->Enable(idMenuAddSymbolFile, isRunning && stopped);
        mbar->Enable(idMenuStop, isRunning && en);
        mbar->Enable(idMenuBreak, isRunning && !stopped && en);
        mbar->Enable(idMenuAttachToProcess, !isRunning && !otherPlugin && m_activeDebugger);
        mbar->Enable(idMenuDetach, isRunning && stopped && isAttached);

        wxMenu *activeMenu = GetMenuById(idMenuDebugActive);
        if (activeMenu)
        {
            for (size_t ii = 0; ii < activeMenu->GetMenuItemCount(); ++ii)
                activeMenu->Enable(activeMenu->FindItemByPosition(ii)->GetId(), !isRunning);
        }

        mbar->Enable(idMenuTools, m_activeDebugger && m_activeDebugger->ToolMenuEnabled());
    }

    // allow other UpdateUI handlers to process this event
    // *very* important! don't forget it...
    event.Skip();
}

void DebuggerMenuHandler::LogActiveConfig()
{
    DebuggerManager *dbgManager = Manager::Get()->GetDebuggerManager();
    const DebuggerManager::RegisteredPlugins &allDebuggers = dbgManager->GetAllDebuggers();
    DebuggerManager::RegisteredPlugins::const_iterator it = allDebuggers.find(m_activeDebugger);
    wxString configName;
    if (it != allDebuggers.end())
    {
        cbDebuggerConfiguration &config = m_activeDebugger->GetActiveConfig();
        configName = it->first->GetGUIName() + wxT(":") + config.GetName();
    }
    m_activeDebugger->Log(_("Active debugger config: ") + configName);
}

void DebuggerMenuHandler::OnStart(cb_unused wxCommandEvent& event)
{
    cbAssert(m_activeDebugger);
    if (!m_activeDebugger->IsRunning())
    {
        m_disableContinue = true;

        ProjectManager *manager = Manager::Get()->GetProjectManager();
        if (manager->GetIsRunning() == NULL)
        {
            manager->SetIsRunning(m_activeDebugger);

            m_activeDebugger->ClearLog();
            LogActiveConfig();

            if (!m_activeDebugger->Debug(false))
                manager->SetIsRunning(NULL);
        }
        m_disableContinue = false;
    }
    else if (m_activeDebugger->IsStopped() && !m_disableContinue)
    {
        HideValueTooltip();
        m_activeDebugger->Continue();
    }
}

void DebuggerMenuHandler::OnBreak(cb_unused wxCommandEvent& event)
{
    cbAssert(m_activeDebugger);
    HideValueTooltip();
    m_activeDebugger->Break();
}

void DebuggerMenuHandler::OnStop(cb_unused wxCommandEvent& event)
{
    cbAssert(m_activeDebugger);
    HideValueTooltip();
    m_activeDebugger->Stop();
}

void DebuggerMenuHandler::OnContinue(cb_unused wxCommandEvent& event)
{
    cbAssert(m_activeDebugger);
    if(!m_disableContinue)
    {
        HideValueTooltip();
        m_activeDebugger->Continue();
    }
}

void DebuggerMenuHandler::OnNext(cb_unused wxCommandEvent& event)
{
    cbAssert(m_activeDebugger);
    HideValueTooltip();
    m_activeDebugger->Next();
}

void DebuggerMenuHandler::OnNextInstr(cb_unused wxCommandEvent& event)
{
    cbAssert(m_activeDebugger);
    HideValueTooltip();
    m_activeDebugger->NextInstruction();
}

void DebuggerMenuHandler::OnStepIntoInstr(cb_unused wxCommandEvent& event)
{
    cbAssert(m_activeDebugger);
    HideValueTooltip();
    m_activeDebugger->StepIntoInstruction();
}

void DebuggerMenuHandler::OnStep(cb_unused wxCommandEvent& event)
{
    cbAssert(m_activeDebugger);
    if (m_activeDebugger->IsRunning())
    {
        if(!m_disableContinue)
        {
            HideValueTooltip();
            m_activeDebugger->Step();
        }
    }
    else
    {
        m_disableContinue = true;
        ProjectManager *manager = Manager::Get()->GetProjectManager();
        if (manager->GetIsRunning() == NULL)
        {
            manager->SetIsRunning(m_activeDebugger);
            m_activeDebugger->ClearLog();
            LogActiveConfig();

            if (!m_activeDebugger->Debug(true))
                manager->SetIsRunning(NULL);
        }
        m_disableContinue = false;
    }
}

void DebuggerMenuHandler::OnStepOut(cb_unused wxCommandEvent& event)
{
    cbAssert(m_activeDebugger);
    HideValueTooltip();
    m_activeDebugger->StepOut();
}

void DebuggerMenuHandler::OnRunToCursor(cb_unused wxCommandEvent& event)
{
    cbAssert(m_activeDebugger);
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed)
        return;
    const wxString &line_text = ed->GetControl()->GetLine(ed->GetControl()->GetCurrentLine());

    ProjectManager *manager = Manager::Get()->GetProjectManager();
    if (manager->GetIsRunning() == NULL || manager->GetIsRunning() == m_activeDebugger)
    {
        manager->SetIsRunning(m_activeDebugger);
        if (!m_activeDebugger->IsRunning())
        {
            m_activeDebugger->ClearLog();
            LogActiveConfig();
        }
        HideValueTooltip();
        if (!m_activeDebugger->RunToCursor(ed->GetFilename(), ed->GetControl()->GetCurrentLine() + 1, line_text))
            manager->SetIsRunning(NULL);
    }
}

void DebuggerMenuHandler::OnSetNextStatement(cb_unused wxCommandEvent& event)
{
    cbAssert(m_activeDebugger);
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed)
        return;
    HideValueTooltip();
    m_activeDebugger->SetNextStatement(ed->GetFilename(), ed->GetControl()->GetCurrentLine() + 1);
}

void DebuggerMenuHandler::OnToggleBreakpoint(cb_unused wxCommandEvent& event)
{
    cbAssert(m_activeDebugger);
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed)
        return;
    ed->ToggleBreakpoint();
}

void DebuggerMenuHandler::OnRemoveAllBreakpoints(cb_unused wxCommandEvent& event)
{
    cbAssert(m_activeDebugger);
    Manager::Get()->GetDebuggerManager()->GetBreakpointDialog()->RemoveAllBreakpoints();
}

void DebuggerMenuHandler::OnAddDataBreakpoint(cb_unused wxCommandEvent& event)
{
    cbAssert(m_activeDebugger);
    const wxString& word_at_caret = m_activeDebugger->GetEditorWordAtCaret();
    if (!word_at_caret.empty())
    {
        if (m_activeDebugger->AddDataBreakpoint(word_at_caret))
            Manager::Get()->GetDebuggerManager()->GetBreakpointDialog()->Reload();
    }
}

void DebuggerMenuHandler::OnAttachToProcess(cb_unused wxCommandEvent& event)
{
    cbAssert(m_activeDebugger);
    wxString pidStr = wxGetTextFromUser(_("PID to attach to:"));
    if (!pidStr.empty())
    {
        m_activeDebugger->AttachToProcess(pidStr);
    }
}

void DebuggerMenuHandler::OnDetachFromProcess(cb_unused wxCommandEvent& event)
{
    cbAssert(m_activeDebugger);
    m_activeDebugger->DetachFromProcess();
}

void DebuggerMenuHandler::OnSendCommand(cb_unused wxCommandEvent& event)
{
    cbAssert(m_activeDebugger);
    wxString cmd = wxGetTextFromUser(_("Enter command for Debugger:"), _("Send command to Debugger:"), m_lastCommand);
    if (cmd.IsEmpty())
        return;

    m_activeDebugger->SendCommand(cmd, false);
    m_lastCommand = cmd;
}

void DebuggerMenuHandler::OnAddWatch(cb_unused wxCommandEvent& event)
{
    if (!m_activeDebugger)
        return;

    wxString const &src = m_activeDebugger->GetEditorWordAtCaret();
    if (!src.empty())
    {
        cb::shared_ptr<cbWatch> watch = m_activeDebugger->AddWatch(src);
        if (watch.get())
        {
            cbWatchesDlg *dialog = Manager::Get()->GetDebuggerManager()->GetWatchesDialog();
            dialog->AddWatch(watch);
            if (!IsWindowReallyShown(dialog->GetWindow()))
            {
                CodeBlocksDockEvent evt(cbEVT_SHOW_DOCK_WINDOW);
                evt.pWindow = dialog->GetWindow();
                Manager::Get()->ProcessEvent(evt);
            }
        }
    }
}

void DebuggerMenuHandler::OnActiveDebuggerClick(wxCommandEvent& event)
{
    DebuggerManager *manager = Manager::Get()->GetDebuggerManager();
    DebuggerManager::RegisteredPlugins &plugins = manager->GetAllDebuggers();

    for(DebuggerManager::RegisteredPlugins::iterator it = plugins.begin(); it != plugins.end(); ++it)
    {
        DebuggerManager::ConfigurationVector &configs = it->second.GetConfigurations();
        for (DebuggerManager::ConfigurationVector::iterator itConf = configs.begin(); itConf != configs.end(); ++itConf)
        {
            if((*itConf)->GetMenuId() == event.GetId())
            {
                manager->SetActiveDebugger(it->first, itConf);
                return;
            }
        }
    }
}

void  DebuggerMenuHandler::OnActiveDebuggerTargetsDefaultClick(cb_unused wxCommandEvent& event)
{
    Manager::Get()->GetDebuggerManager()->SetTargetsDefaultAsActiveDebugger();
}


BEGIN_EVENT_TABLE(DebuggerToolbarHandler, wxEvtHandler)
    // these are different because they are loaded from the XRC
    EVT_UPDATE_UI(idMenuDebug, DebuggerToolbarHandler::OnUpdateUI)
    EVT_UPDATE_UI(idMenuRunToCursor, DebuggerToolbarHandler::OnUpdateUI)
    EVT_UPDATE_UI(idMenuNext, DebuggerToolbarHandler::OnUpdateUI)
    EVT_UPDATE_UI(idMenuNextInstr, DebuggerToolbarHandler::OnUpdateUI)
    EVT_UPDATE_UI(idMenuStepIntoInstr, DebuggerToolbarHandler::OnUpdateUI)
    EVT_UPDATE_UI(idMenuStep, DebuggerToolbarHandler::OnUpdateUI)
    EVT_UPDATE_UI(idMenuStepOut, DebuggerToolbarHandler::OnUpdateUI)
    EVT_UPDATE_UI(idMenuBreak, DebuggerToolbarHandler::OnUpdateUI)
    EVT_UPDATE_UI(idToolbarStop, DebuggerToolbarHandler::OnUpdateUI)

    EVT_MENU(idDebuggerToolInfo, DebuggerToolbarHandler::OnToolInfo)
    EVT_MENU(idDebuggerToolWindows, DebuggerToolbarHandler::OnDebugWindows)
    EVT_MENU(idToolbarStop, DebuggerToolbarHandler::OnStop)
END_EVENT_TABLE()

DebuggerToolbarHandler::DebuggerToolbarHandler(DebuggerMenuHandler *menuHandler) :
    m_Toolbar(NULL),
    m_menuHandler(menuHandler)
{
}

wxToolBar* DebuggerToolbarHandler::GetToolbar(bool create)
{
    if (!m_Toolbar)
    {
        if (!create)
            return NULL;

        m_Toolbar = Manager::Get()->CreateEmptyToolbar();
        wxString my_16x16 = Manager::isToolBar16x16(m_Toolbar) ? _T("_16x16") : _T("");
        Manager::AddonToolBar(m_Toolbar, wxString(_T("debugger_toolbar")) + my_16x16);

        m_Toolbar->Realize();
        m_Toolbar->SetInitialSize();
    }
    return m_Toolbar;
}

void DebuggerToolbarHandler::OnUpdateUI(wxUpdateUIEvent& event)
{
    cbDebuggerPlugin *plugin = Manager::Get()->GetDebuggerManager()->GetActiveDebugger();
    ProjectManager *manager = Manager::Get()->GetProjectManager();

    bool en = false;
    bool stopped = false, isRunning = false;

    if (plugin)
    {
        cbProject* prj = manager->GetActiveProject();
        en = (prj && !prj->GetCurrentlyCompilingTarget()) || plugin->IsAttachedToProcess();
        stopped = plugin->IsStopped();
        isRunning = plugin->IsRunning();
    }

    if (m_Toolbar)
    {
        cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();

        cbPlugin *runningPlugin = manager->GetIsRunning();
        if (runningPlugin != NULL && runningPlugin != plugin)
            en = false;

        m_Toolbar->EnableTool(idMenuDebug, (!isRunning || stopped) && en);
        m_Toolbar->EnableTool(idMenuRunToCursor, en && ed && stopped);
        m_Toolbar->EnableTool(idMenuNext, isRunning && en && stopped);
        m_Toolbar->EnableTool(idMenuNextInstr, isRunning && en && stopped);
        m_Toolbar->EnableTool(idMenuStepIntoInstr, isRunning && en && stopped);
        m_Toolbar->EnableTool(idMenuStep, en && stopped);
        m_Toolbar->EnableTool(idMenuStepOut, isRunning && en && stopped);
        m_Toolbar->EnableTool(idToolbarStop, isRunning && en);
        m_Toolbar->EnableTool(idMenuBreak, isRunning && !stopped && en);
        m_Toolbar->EnableTool(idDebuggerToolInfo, plugin && plugin->ToolMenuEnabled());
    }

    // allow other UpdateUI handlers to process this event
    // *very* important! don't forget it...
    event.Skip();
}

void DebuggerToolbarHandler::OnToolInfo(cb_unused wxCommandEvent& event)
{
    cbDebuggerPlugin *plugin = Manager::Get()->GetDebuggerManager()->GetActiveDebugger();
    if (plugin)
    {
        wxMenu menu;
        plugin->SetupToolsMenu(menu);
        Manager::Get()->GetAppWindow()->PopupMenu(&menu);
    }
}

void DebuggerToolbarHandler::OnDebugWindows(cb_unused wxCommandEvent& event)
{
    wxMenu m;
    m_menuHandler->AppendWindowMenuItems(m);
    Manager::Get()->GetAppWindow()->PopupMenu(&m);
}

void DebuggerToolbarHandler::OnStop(cb_unused wxCommandEvent& event)
{
    DebuggerManager *manager = Manager::Get()->GetDebuggerManager();
    cbDebuggerPlugin *plugin = manager->GetActiveDebugger();
    if (!plugin)
        return;

    if (plugin->IsAttachedToProcess())
    {
        wxMenu m;

        if (plugin->IsStopped())
            m.Append(idMenuDetach, _("Detach"));
        else
        {
            wxMenuItem *detach_item = m.Append(idMenuDetach, _("Detach (debugger is running)"));
            detach_item->Enable(false);
        }

        m.Append(idMenuStop, _("Stop debugger (kills the debuggee)"));

        Manager::Get()->GetAppWindow()->PopupMenu(&m);
    }
    else
    {
        wxCommandEvent event2(wxEVT_COMMAND_TOOL_CLICKED, idMenuStop);
        m_Toolbar->GetEventHandler()->ProcessEvent(event2);
    }
}

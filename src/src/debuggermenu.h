/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef X_DEBUGGER_MENU_H
#define X_DEBUGGER_MENU_H

#include <wx/event.h>
#include <cbdebugger_interfaces.h>

class cbDebuggerPlugin;
class wxToolBar;

class DebuggerMenuHandler : public wxEvtHandler, public cbDebuggerMenuHandler
{
    public:
        DebuggerMenuHandler();

        void RegisterDefaultWindowItems();
        void AppendWindowMenuItems(wxMenu &menu);

    public: // derived from cbDebuggerMenuHandler
        virtual void SetActiveDebugger(cbDebuggerPlugin *active);
        virtual void MarkActiveTargetAsValid(bool valid);
        virtual void RebuildMenus();
        virtual void BuildContextMenu(wxMenu &menu, const wxString& word_at_caret, bool is_running);

        virtual bool RegisterWindowMenu(const wxString &name, const wxString &help, cbDebuggerWindowMenuItem *item);
        virtual void UnregisterWindowMenu(const wxString &name);
    private:
        void OnUpdateUI(wxUpdateUIEvent& event);
        void OnStart(wxCommandEvent& event);
        void OnBreak(wxCommandEvent& event);
        void OnStop(wxCommandEvent& event);

        void OnContinue(wxCommandEvent& event);
        void OnNext(wxCommandEvent& event);
        void OnNextInstr(wxCommandEvent& event);
        void OnStepIntoInstr(wxCommandEvent& event);
        void OnStep(wxCommandEvent& event);
        void OnStepOut(wxCommandEvent& event);
        void OnRunToCursor(wxCommandEvent& event);
        void OnSetNextStatement(wxCommandEvent& event);

        void OnToggleBreakpoint(wxCommandEvent& event);
        void OnRemoveAllBreakpoints(wxCommandEvent& event);
        void OnAddDataBreakpoint(wxCommandEvent& event);
        void OnSendCommand(wxCommandEvent& event);
        void OnAttachToProcess(wxCommandEvent& event);
        void OnDetachFromProcess(wxCommandEvent& event);
        void OnAddWatch(wxCommandEvent& event);

        void OnBacktrace(wxCommandEvent& event);
        void OnBreakpoints(wxCommandEvent& event);
        void OnCPURegisters(wxCommandEvent& event);
        void OnDisassembly(wxCommandEvent& event);
        void OnExamineMemory(wxCommandEvent& event);
        void OnThreads(wxCommandEvent& event);
        void OnWatches(wxCommandEvent& event);

        void OnActiveDebuggerClick(wxCommandEvent& event);
        void OnActiveDebuggerTargetsDefaultClick(wxCommandEvent& event);

        void LogActiveConfig();

        void OnWindowMenuItemUpdateUI(wxUpdateUIEvent &event);
        void OnWindowMenuItemClicked(wxCommandEvent &event);

        DECLARE_EVENT_TABLE();
    private:
        struct WindowMenuItem
        {
            cb::shared_ptr<cbDebuggerWindowMenuItem> item;
            wxString name, help;
        };
        typedef std::map<long, WindowMenuItem> WindowMenuItemsMap;
    private:
        WindowMenuItemsMap m_windowMenuItems;
        cbDebuggerPlugin* m_activeDebugger;
        wxString m_lastCommand;
        bool m_disableContinue;
};

class DebuggerToolbarHandler : public wxEvtHandler
{
        friend class DebuggerManager;
    public:
        DebuggerToolbarHandler(DebuggerMenuHandler *menuHandler);
        wxToolBar* GetToolbar(bool create = true);
    private:
        void OnUpdateUI(wxUpdateUIEvent& event);
        void OnToolInfo(wxCommandEvent& event);
        void OnDebugWindows(wxCommandEvent& event);
        void OnStop(wxCommandEvent& event);

        wxToolBar *m_Toolbar;
        DebuggerMenuHandler *m_menuHandler;

        DECLARE_EVENT_TABLE();
};

#endif // X_DEBUGGER_MENU_H


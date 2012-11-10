/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */
#include "debugger_interface_creator.h"

#ifndef CB_PRECOMP
    #include "cbeditor.h"
    #include "editormanager.h"
#endif

#include "backtracedlg.h"
#include "breakpointsdlg.h"
#include "cbstyledtextctrl.h"
#include "cpuregistersdlg.h"
#include "disassemblydlg.h"
#include "examinememorydlg.h"
#include "threadsdlg.h"
#include "watchesdlg.h"

void RemoveDockWindow(wxWindow *window)
{
    if (window)
    {
        CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
        evt.pWindow = window;
        Manager::Get()->ProcessEvent(evt);
        window->Destroy();
    }
}

DebugInterfaceFactory::DebugInterfaceFactory() : m_tooltip(nullptr)
{
    typedef cbEventFunctor<DebugInterfaceFactory, CodeBlocksEvent> Event;
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_DEACTIVATED,
                                      new Event(this, &DebugInterfaceFactory::OnEditorDeactivate));
}

DebugInterfaceFactory::~DebugInterfaceFactory()
{
    Manager::Get()->RemoveAllEventSinksFor(this);
}

cbBacktraceDlg* DebugInterfaceFactory::CreateBacktrace()
{
    BacktraceDlg *dialog = new BacktraceDlg(Manager::Get()->GetAppWindow());
    CodeBlocksDockEvent evt(cbEVT_ADD_DOCK_WINDOW);
    evt.name = _T("CallStackPane");
    evt.title = _("Call stack");
    evt.pWindow = dialog;
    evt.dockSide = CodeBlocksDockEvent::dsFloating;
    evt.desiredSize.Set(150, 150);
    evt.floatingSize.Set(450, 150);
    evt.minimumSize.Set(150, 150);
    Manager::Get()->ProcessEvent(evt);
    return dialog;
}

void DebugInterfaceFactory::DeleteBacktrace(cbBacktraceDlg *dialog)
{
    if (dialog)
        RemoveDockWindow(dialog->GetWindow());
}

cbBreakpointsDlg* DebugInterfaceFactory::CreateBreapoints()
{
    BreakpointsDlg *dialog = new BreakpointsDlg;

    CodeBlocksDockEvent evt(cbEVT_ADD_DOCK_WINDOW);
    evt.name = _T("BreakpointsPane");
    evt.title = _("Breakpoints");
    evt.pWindow = dialog;
    evt.dockSide = CodeBlocksDockEvent::dsFloating;
    evt.desiredSize.Set(350, 250);
    evt.floatingSize.Set(350, 250);
    evt.minimumSize.Set(150, 150);
    Manager::Get()->ProcessEvent(evt);
    return dialog;
}

void DebugInterfaceFactory::DeleteBreakpoints(cbBreakpointsDlg *dialog)
{
    if (dialog)
        RemoveDockWindow(dialog->GetWindow());
}

cbCPURegistersDlg* DebugInterfaceFactory::CreateCPURegisters()
{
    CPURegistersDlg *dialog = new CPURegistersDlg(Manager::Get()->GetAppWindow());

    CodeBlocksDockEvent evt(cbEVT_ADD_DOCK_WINDOW);
    evt.name = _T("CPURegistersPane");
    evt.title = _("CPU Registers");
    evt.pWindow = dialog;
    evt.dockSide = CodeBlocksDockEvent::dsFloating;
    evt.desiredSize.Set(350, 250);
    evt.floatingSize.Set(350, 250);
    evt.minimumSize.Set(150, 150);
    Manager::Get()->ProcessEvent(evt);
    return dialog;
}

void DebugInterfaceFactory::DeleteCPURegisters(cbCPURegistersDlg *dialog)
{
    if (dialog)
        RemoveDockWindow(dialog->GetWindow());
}

cbDisassemblyDlg* DebugInterfaceFactory::CreateDisassembly()
{
    DisassemblyDlg *dialog = new DisassemblyDlg(Manager::Get()->GetAppWindow());
    CodeBlocksDockEvent evt(cbEVT_ADD_DOCK_WINDOW);

    evt.name = _T("DisassemblyPane");
    evt.title = _("Disassembly");
    evt.pWindow = dialog;
    evt.dockSide = CodeBlocksDockEvent::dsFloating;
    evt.desiredSize.Set(350, 250);
    evt.floatingSize.Set(350, 250);
    evt.minimumSize.Set(150, 150);
    Manager::Get()->ProcessEvent(evt);
    return dialog;
}

void DebugInterfaceFactory::DeleteDisassembly(cbDisassemblyDlg *dialog)
{
    if (dialog)
        RemoveDockWindow(dialog->GetWindow());
}

cbExamineMemoryDlg* DebugInterfaceFactory::CreateMemory()
{
    ExamineMemoryDlg *dialog = new ExamineMemoryDlg(Manager::Get()->GetAppWindow());
    CodeBlocksDockEvent evt(cbEVT_ADD_DOCK_WINDOW);
    evt.name = _T("ExamineMemoryPane");
    evt.title = _("Memory");
    evt.pWindow = dialog;
    evt.dockSide = CodeBlocksDockEvent::dsFloating;
    evt.desiredSize.Set(450, 250);
    evt.floatingSize.Set(450, 250);
    evt.minimumSize.Set(350, 150);
    Manager::Get()->ProcessEvent(evt);
    return dialog;
}

void DebugInterfaceFactory::DeleteMemory(cbExamineMemoryDlg *dialog)
{
    if (dialog)
        RemoveDockWindow(dialog->GetWindow());
}

cbThreadsDlg* DebugInterfaceFactory::CreateThreads()
{
    ThreadsDlg *dialog = new ThreadsDlg(Manager::Get()->GetAppWindow());

    CodeBlocksDockEvent evt(cbEVT_ADD_DOCK_WINDOW);
    evt.name = _T("ThreadsPane");
    evt.title = _("Running threads");
    evt.pWindow = dialog;
    evt.dockSide = CodeBlocksDockEvent::dsFloating;
    evt.desiredSize.Set(350, 75);
    evt.floatingSize.Set(450, 75);
    evt.minimumSize.Set(250, 75);
    Manager::Get()->ProcessEvent(evt);
    return dialog;
}

void DebugInterfaceFactory::DeleteThreads(cbThreadsDlg *dialog)
{
    if (dialog)
        RemoveDockWindow(dialog->GetWindow());
}

cbWatchesDlg* DebugInterfaceFactory::CreateWatches()
{
    WatchesDlg *dialog = new WatchesDlg;
    CodeBlocksDockEvent evt(cbEVT_ADD_DOCK_WINDOW);
    evt.name = _T("WatchesPane");
    evt.title = _("Watches (new)");
    evt.pWindow = dialog;
    evt.dockSide = CodeBlocksDockEvent::dsFloating;
    evt.desiredSize.Set(150, 250);
    evt.floatingSize.Set(150, 250);
    evt.minimumSize.Set(150, 150);
    Manager::Get()->ProcessEvent(evt);
    return dialog;
}

void DebugInterfaceFactory::DeleteWatches(cbWatchesDlg *dialog)
{
    if (dialog)
        RemoveDockWindow(dialog->GetWindow());
}

bool DebugInterfaceFactory::ShowValueTooltip(const cb::shared_ptr<cbWatch> &watch, const wxRect &rect)
{
    delete m_tooltip;
    m_tooltip = nullptr;

    wxPoint pt = wxGetMousePosition();
    if (!rect.Contains(pt))
        return false;
    else
    {
        m_tooltip = new ValueTooltip(watch, Manager::Get()->GetAppWindow());
#ifndef __WXMAC__
        m_tooltip->Position(pt, wxSize(0, 0));
#endif
        // hide any other tooltips
        EditorBase *base = Manager::Get()->GetEditorManager()->GetActiveEditor();
        cbEditor *ed = base && base->IsBuiltinEditor() ? static_cast<cbEditor*>(base) : nullptr;
        if (ed && ed->GetControl()->CallTipActive())
            ed->GetControl()->CallTipCancel();

        m_tooltip->Show();
        return true;
    }
}

void DebugInterfaceFactory::HideValueTooltip()
{
    if (m_tooltip)
        m_tooltip->Dismiss();
}

void DebugInterfaceFactory::UpdateValueTooltip()
{
    if (m_tooltip)
        m_tooltip->UpdateWatch();
}

bool DebugInterfaceFactory::IsValueTooltipShown()
{
    return m_tooltip && m_tooltip->IsShown();
}

void DebugInterfaceFactory::OnEditorDeactivate(cb_unused CodeBlocksEvent &event)
{
    HideValueTooltip();
}

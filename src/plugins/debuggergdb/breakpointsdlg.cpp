#include <sdk.h>
#include "breakpointsdlg.h"
#include <manager.h>
#include <editormanager.h>
#include <cbeditor.h>
#include "editbreakpointdlg.h"
#include "debuggerstate.h"
#include "debuggerdriver.h"
#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/listbox.h>
#include <wx/checkbox.h>
#include <wx/spinctrl.h>
#include <globals.h>

static const int idRemove = wxNewId();
static const int idRemoveAll = wxNewId();
static const int idProperties = wxNewId();
static const int idOpen = wxNewId();

BEGIN_EVENT_TABLE(BreakpointsDlg, wxPanel)
    EVT_MENU(idRemove, BreakpointsDlg::OnRemove)
    EVT_MENU(idRemoveAll, BreakpointsDlg::OnRemoveAll)
    EVT_MENU(idProperties, BreakpointsDlg::OnProperties)
    EVT_MENU(idOpen, BreakpointsDlg::OnOpen)
END_EVENT_TABLE()

BreakpointsDlg::BreakpointsDlg(DebuggerState& state)
    : m_State(state),
    m_BreakpointsList(state.GetBreakpoints())
{
    //ctor
    long style = m_pList->GetWindowStyleFlag();
    m_pList->SetWindowStyleFlag(style | wxLC_HRULES | wxLC_VRULES);
    int widths[] = { 128, 44 };
    wxArrayString titles;
    titles.Add(_("Filename"));
    titles.Add(_("Line"));
    SetColumns(2, widths, titles);

    Connect(m_pList->GetId(), -1, wxEVT_COMMAND_LIST_ITEM_ACTIVATED,
            (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction)
            &BreakpointsDlg::OnDoubleClick);

    Connect(m_pList->GetId(), -1, wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK,
            (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction)
            &BreakpointsDlg::OnRightClick);

    FillBreakpoints();
}

BreakpointsDlg::~BreakpointsDlg()
{
    //dtor
}

void BreakpointsDlg::Refresh()
{
    FillBreakpoints();
}

void BreakpointsDlg::FillBreakpoints()
{
    m_pList->Freeze();
    Clear();
    for (unsigned int i = 0; i < m_BreakpointsList.GetCount(); ++i)
    {
        DebuggerBreakpoint* bp = m_State.GetBreakpoints()[i];
        if (bp->temporary)
            continue;
        wxArrayString entry;
        entry.Add(bp->filename);
        entry.Add(wxString::Format(_T("%d"), bp->line + 1));
        AddLog(entry);
    }
    m_pList->SetColumnWidth(0, wxLIST_AUTOSIZE);
    m_pList->Thaw();
}

void BreakpointsDlg::RemoveBreakpoint(int sel)
{
    // if debugger is running and is not paused, return
    if (m_State.GetDriver() && !m_State.GetDriver()->IsStopped())
        return;
    // if index is out of range, return
    if (sel < 0 || sel >= (int)m_State.GetBreakpoints().GetCount())
        return;
    // if not valid breakpoint, return
    DebuggerBreakpoint* bp = m_State.GetBreakpoints()[sel];
    if (!bp)
        return;
    cbEditor* ed = Manager::Get()->GetEditorManager()->IsBuiltinOpen(bp->filenameAsPassed);
    if (ed)
        ed->RemoveBreakpoint(bp->line);
}

void BreakpointsDlg::OnRemove(wxCommandEvent& event)
{
    long item = m_pList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (item == -1)
        return;
    RemoveBreakpoint(item);
}

void BreakpointsDlg::OnRemoveAll(wxCommandEvent& event)
{
    // if debugger is running and is not paused, return
    if (m_State.GetDriver() && !m_State.GetDriver()->IsStopped())
        return;
    while (m_State.GetBreakpoints().GetCount())
    {
        // if not valid breakpoint, continue with the next one
        DebuggerBreakpoint* bp = m_State.GetBreakpoints()[0];
        if (!bp)
            continue;
        cbEditor* ed = Manager::Get()->GetEditorManager()->IsBuiltinOpen(bp->filenameAsPassed);
        if (ed)
            ed->RemoveBreakpoint(bp->line, false);
        m_State.RemoveBreakpoint(0);
    }
    FillBreakpoints();
}

void BreakpointsDlg::OnProperties(wxCommandEvent& event)
{
    long item = m_pList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (item == -1 || item > (int)m_State.GetBreakpoints().GetCount())
        return;
    DebuggerBreakpoint* bp = m_State.GetBreakpoints()[item];
    if (!bp)
        return;
    int idx = m_State.HasBreakpoint(bp->filename, bp->line);
    bp = m_State.GetBreakpoint(idx);
    EditBreakpointDlg dlg(bp);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        m_State.ResetBreakpoint(idx);
    }
}

void BreakpointsDlg::OnOpen(wxCommandEvent& event)
{
    long item = m_pList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (item < 0 || item >= (int)m_State.GetBreakpoints().GetCount())
        return;
    DebuggerBreakpoint* bp = m_State.GetBreakpoints()[item];
    cbEditor* ed = Manager::Get()->GetEditorManager()->Open(bp->filename);
    if (ed)
    {
        ed->GotoLine(bp->line, true);
        ed->Activate();
    }
}

void BreakpointsDlg::OnRightClick(wxListEvent& event)
{
    wxMenu menu;
    menu.Append(idOpen, _("Open in editor"));
    menu.Append(idProperties, _("Breakpoint properties"));
    menu.AppendSeparator();
    menu.Append(idRemove, _("Remove breakpoint"));
    menu.Append(idRemoveAll, _("Remove all breakpoints"));
    PopupMenu(&menu);
}

void BreakpointsDlg::OnDoubleClick(wxListEvent& event)
{
    wxCommandEvent evt;
    OnOpen(evt);
}

#include <sdk.h>
#include <wx/listctrl.h>
#include "breakpointsdlg.h"
#include <manager.h>
#include <editormanager.h>
#include <cbeditor.h>
#include "editbreakpointdlg.h"
#include "databreakpointdlg.h"
#include "debuggerstate.h"
#include "debuggerdriver.h"
#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/listbox.h>
#include <wx/checkbox.h>
#include <wx/spinctrl.h>
#include <wx/menu.h>
#include <globals.h>
namespace
{
	const int idList = wxNewId();
	const int idRemove = wxNewId();
	const int idRemoveAll = wxNewId();
	const int idProperties = wxNewId();
	const int idOpen = wxNewId();
};

BEGIN_EVENT_TABLE(BreakpointsDlg, wxPanel)
    EVT_MENU(idRemove, BreakpointsDlg::OnRemove)
    EVT_MENU(idRemoveAll, BreakpointsDlg::OnRemoveAll)
    EVT_MENU(idProperties, BreakpointsDlg::OnProperties)
    EVT_MENU(idOpen, BreakpointsDlg::OnOpen)
END_EVENT_TABLE()

BreakpointsDlg::BreakpointsDlg(DebuggerState& state)
    : wxPanel(Manager::Get()->GetAppWindow(), -1),
    m_State(state),
    m_BreakpointsList(state.GetBreakpoints())
{
    //ctor
    wxBoxSizer* bs = new wxBoxSizer(wxVERTICAL);
	m_pList = new wxListCtrl(this, idList, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES | wxLC_VRULES);
    bs->Add(m_pList, 1, wxEXPAND | wxALL);
    SetAutoLayout(TRUE);
    SetSizer(bs);

	m_pList->InsertColumn(0, _("Type"), wxLIST_FORMAT_LEFT, 128);
	m_pList->InsertColumn(1, _("Filename/Address"), wxLIST_FORMAT_LEFT, 128);
	m_pList->InsertColumn(2, _("Line"), wxLIST_FORMAT_LEFT, 44);

    Connect(idList, -1, wxEVT_COMMAND_LIST_ITEM_ACTIVATED,
            (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction)
            &BreakpointsDlg::OnDoubleClick);

    Connect(idList, -1, wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK,
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
    m_pList->DeleteAllItems();
    for (unsigned int i = 0; i < m_BreakpointsList.GetCount(); ++i)
    {
        DebuggerBreakpoint* bp = m_State.GetBreakpoints()[i];
        if (bp->temporary)
            continue;
        if (bp->type == DebuggerBreakpoint::bptCode)
        {
			m_pList->InsertItem(m_pList->GetItemCount(), _("Code"));
			m_pList->SetItem(m_pList->GetItemCount() - 1, 1, bp->filename);
			m_pList->SetItem(m_pList->GetItemCount() - 1, 2, wxString::Format(_T("%d"), bp->line + 1));
        }
        else if (bp->type == DebuggerBreakpoint::bptData)
        {
			m_pList->InsertItem(m_pList->GetItemCount(), _("Data"));
			m_pList->SetItem(m_pList->GetItemCount() - 1, 1, wxString::Format(_T("%s (read: %s, write: %s)"),
																				bp->breakAddress.c_str(),
																				bp->breakOnRead ? _T("yes") : _T("no"),
																				bp->breakOnWrite ? _T("yes") : _T("no")));
			m_pList->SetItem(m_pList->GetItemCount() - 1, 2, wxEmptyString);
        }
        else if (bp->type == DebuggerBreakpoint::bptFunction)
        {
			m_pList->InsertItem(m_pList->GetItemCount(), _("Function"));
			m_pList->SetItem(m_pList->GetItemCount() - 1, 1, bp->filename);
			m_pList->SetItem(m_pList->GetItemCount() - 1, 2, wxString::Format(_T("%d"), bp->line + 1));
        }
        m_pList->SetItemData(m_pList->GetItemCount() - 1, (long)bp);
    }
    m_pList->SetColumnWidth(0, wxLIST_AUTOSIZE);
    m_pList->SetColumnWidth(1, wxLIST_AUTOSIZE);
    m_pList->SetColumnWidth(2, wxLIST_AUTOSIZE);
    m_pList->Thaw();
}

void BreakpointsDlg::RemoveBreakpoint(int sel)
{
    // if debugger is running and is not paused, return
    if (m_State.HasDriver() && !m_State.GetDriver()->IsStopped())
        return;
    // if index is out of range, return
    if (sel < 0 || sel >= (int)m_State.GetBreakpoints().GetCount())
        return;
    // if not valid breakpoint, return
    DebuggerBreakpoint* bp = (DebuggerBreakpoint*)m_pList->GetItemData(sel);//m_State.GetBreakpoints()[sel];
    if (!bp)
        return;
	if (bp->type == DebuggerBreakpoint::bptData)
	{
		m_State.RemoveBreakpoint(bp);
		Refresh();
	}
	else
	{
		cbEditor* ed = Manager::Get()->GetEditorManager()->IsBuiltinOpen(bp->filenameAsPassed);
		if (ed)
			ed->RemoveBreakpoint(bp->line);
	}
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
    if (m_State.HasDriver() && !m_State.GetDriver()->IsStopped())
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
    DebuggerBreakpoint* bp = (DebuggerBreakpoint*)m_pList->GetItemData(item);//m_State.GetBreakpoints()[item];
    if (!bp)
        return;

    if (bp->type == DebuggerBreakpoint::bptData)
    {
    	int sel = 0;
    	if (bp->breakOnRead && bp->breakOnWrite)
			sel = 2;
    	else if (!bp->breakOnRead && bp->breakOnWrite)
			sel = 1;
    	DataBreakpointDlg dlg(this, -1, bp->enabled, sel);
    	if (dlg.ShowModal() == wxID_OK)
    	{
    		bp->enabled = dlg.IsEnabled();
    		bp->breakOnRead = dlg.GetSelection() != 1;
    		bp->breakOnWrite = dlg.GetSelection() != 0;
    		m_State.ResetBreakpoint(bp);
    	}
    }
    else
    {
		int idx = m_State.HasBreakpoint(bp->filename, bp->line);
		bp = m_State.GetBreakpoint(idx);

		EditBreakpointDlg dlg(bp);
		PlaceWindow(&dlg);
		if (dlg.ShowModal() == wxID_OK)
		{
			m_State.ResetBreakpoint(idx);
		}
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

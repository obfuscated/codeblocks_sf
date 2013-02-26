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
    #include <wx/listctrl.h>
    #include <wx/menu.h>
    #include <wx/sizer.h>

    #include "cbplugin.h"
    #include "debuggermanager.h"
#endif

#include "threadsdlg.h"
#include "cbcolourmanager.h"
#include "debuggermanager.h"

namespace
{
    const int idList = wxNewId();
    const int idSwitch = wxNewId();
}

BEGIN_EVENT_TABLE(ThreadsDlg, wxPanel)
    EVT_LIST_ITEM_RIGHT_CLICK(idList, ThreadsDlg::OnListRightClick)
    EVT_LIST_ITEM_ACTIVATED(idList, ThreadsDlg::OnListDoubleClick)
    EVT_MENU(idSwitch, ThreadsDlg::OnSwitchThread)
END_EVENT_TABLE()


ThreadsDlg::ThreadsDlg(wxWindow* parent) : wxPanel(parent)
{
    m_list = new wxListCtrl(this, idList, wxDefaultPosition, wxDefaultSize,
                            wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES | wxLC_VRULES);
    wxBoxSizer* bs = new wxBoxSizer(wxVERTICAL);
    bs->Add(m_list, 1, wxEXPAND | wxALL);
    SetAutoLayout(true);
    SetSizer(bs);

    wxFont font(8, wxMODERN, wxNORMAL, wxNORMAL);
    m_list->SetFont(font);
    m_list->InsertColumn(0, _("Active"), wxLIST_FORMAT_LEFT, 64);
    m_list->InsertColumn(1, _("Number"), wxLIST_FORMAT_RIGHT, 64);
    m_list->InsertColumn(2, _("Info"), wxLIST_FORMAT_LEFT);

    Manager::Get()->GetColourManager()->RegisterColour(_("Debugger"), _("Active thread text"),
                                                       wxT("dbg_threads_active_text"), *wxWHITE);
    Manager::Get()->GetColourManager()->RegisterColour(_("Debugger"), _("Active thread background"),
                                                       wxT("dbg_threads_active_back"), *wxRED);
}

void ThreadsDlg::Reload()
{
    cbDebuggerPlugin *plugin = Manager::Get()->GetDebuggerManager()->GetActiveDebugger();
    if (!plugin)
        return;

    ColourManager *colours = Manager::Get()->GetColourManager();

    m_list->Freeze();
    m_list->DeleteAllItems();
    long active_index = -1;
    for (int ii = 0; ii < plugin->GetThreadsCount(); ++ii)
    {
        cb::shared_ptr<const cbThread> thread = plugin->GetThread(ii);

        long index = m_list->InsertItem(m_list->GetItemCount(), thread->IsActive() ? wxT("-->") : wxT(""));

        m_list->SetItem(index, 1, wxString::Format(wxT("%d"), thread->GetNumber()));
        m_list->SetItem(index, 2, thread->GetInfo());
        if (thread->IsActive())
        {
            m_list->SetItemTextColour(index, colours->GetColour(wxT("dbg_threads_active_text")));
            m_list->SetItemBackgroundColour(index, colours->GetColour(wxT("dbg_threads_active_back")));
            active_index = index;
        }
    }
    if (active_index != -1)
        m_list->EnsureVisible(active_index);
    m_list->Thaw();
    for (int ii = 0; ii < m_list->GetColumnCount(); ++ii)
    {
        m_list->SetColumnWidth(ii, wxLIST_AUTOSIZE);
    }
}

//void ThreadsDlg::AddThread(const wxString& active_mark, const wxString& thread_num, const wxString& thread_info)
//{
//    m_list->Freeze();
//    m_list->InsertItem(m_list->GetItemCount(), active_mark);
//    int idx = m_list->GetItemCount() - 1;
//    m_list->SetItem(idx, 1, thread_num);
//    m_list->SetItem(idx, 2, thread_info);
//
//    m_list->SetColumnWidth(2, wxLIST_AUTOSIZE);
//    m_list->Thaw();
//}

void ThreadsDlg::OnListRightClick(cb_unused wxListEvent& event)
{
    if (m_list->GetSelectedItemCount() == 0)
        return;
    wxMenu m;
    m.Append(idSwitch, _("Switch to this thread"));
    m_list->PopupMenu(&m);
}

void ThreadsDlg::OnListDoubleClick(cb_unused wxListEvent& /* event */) // TODO: verify this please
{
    wxCommandEvent event; // is ignoring the original event and using an empty event really what is intended?
    OnSwitchThread(event);
}

void ThreadsDlg::OnSwitchThread(cb_unused wxCommandEvent& event)
{
    if (m_list->GetSelectedItemCount() == 0)
        return;

    // find selected item index
    int index = m_list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    wxString thread;
    wxListItem info;
    info.m_itemId = index;
    info.m_col = 1;
    info.m_mask = wxLIST_MASK_TEXT;
    if (m_list->GetItem(info))
        thread = info.m_text;
    else
        return;

    unsigned long thread_num;
    if (thread.ToULong(&thread_num, 10))
    {
        cbDebuggerPlugin *plugin = Manager::Get()->GetDebuggerManager()->GetActiveDebugger();
        if(plugin)
            plugin->SwitchToThread(thread_num);
    }
}

void ThreadsDlg::EnableWindow(bool enable)
{
    m_list->Enable(enable);
}

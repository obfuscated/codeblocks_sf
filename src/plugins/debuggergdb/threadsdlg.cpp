#include <sdk.h>
#include "threadsdlg.h"
#include "debuggergdb.h"
#include "debuggerdriver.h"
#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/menu.h>
#include <globals.h>

static const int idSwitch = wxNewId();

BEGIN_EVENT_TABLE(ThreadsDlg, wxPanel)
    EVT_LIST_ITEM_RIGHT_CLICK(XRCID("lstThreads"), ThreadsDlg::OnListRightClick)
    EVT_MENU(idSwitch, ThreadsDlg::OnSwitchThread)
END_EVENT_TABLE()

ThreadsDlg::ThreadsDlg(wxWindow* parent, DebuggerGDB* debugger)
    : m_pDbg(debugger)
{
    //ctor
    wxXmlResource::Get()->LoadPanel(this, parent, _T("dlgThreads"));
//    SetWindowStyle(GetWindowStyle() | wxFRAME_FLOAT_ON_PARENT);

    wxListCtrl* lst = XRCCTRL(*this, "lstThreads", wxListCtrl);

    wxFont font(8, wxMODERN, wxNORMAL, wxNORMAL);
    lst->SetFont(font);
    Clear();
}

ThreadsDlg::~ThreadsDlg()
{
    //dtor
}

void ThreadsDlg::Clear()
{
    wxListCtrl* lst = XRCCTRL(*this, "lstThreads", wxListCtrl);
    lst->ClearAll();
    lst->Freeze();
    lst->DeleteAllItems();
    lst->InsertColumn(0, _("Active"), wxLIST_FORMAT_LEFT, 64);
    lst->InsertColumn(1, _("Number"), wxLIST_FORMAT_RIGHT, 64);
    lst->InsertColumn(2, _("Info"), wxLIST_FORMAT_LEFT);
    lst->Thaw();
}

void ThreadsDlg::AddThread(const wxString& active_mark, const wxString& thread_num, const wxString& thread_info)
{
//    Manager::Get()->GetLogManager()->DebugLog(_T("Add: mark=%s, num=%s, info=%s"), active_mark.c_str(), thread_num.c_str(), thread_info.c_str());
    wxListCtrl* lst = XRCCTRL(*this, "lstThreads", wxListCtrl);
    lst->Freeze();
    lst->InsertItem(lst->GetItemCount(), active_mark);
    int idx = lst->GetItemCount() - 1;
    lst->SetItem(idx, 1, thread_num);
    lst->SetItem(idx, 2, thread_info);

    lst->SetColumnWidth(2, wxLIST_AUTOSIZE);
    lst->Thaw();
}

void ThreadsDlg::OnListRightClick(wxListEvent& event)
{
    wxListCtrl* lst = XRCCTRL(*this, "lstThreads", wxListCtrl);

    wxMenu m;
    m.Append(idSwitch, _("Switch to this thread"));
    lst->PopupMenu(&m);
}

void ThreadsDlg::OnSwitchThread(wxCommandEvent& event)
{
    wxListCtrl* lst = XRCCTRL(*this, "lstThreads", wxListCtrl);
    if (lst->GetSelectedItemCount() == 0)
        return;

    // find selected item index
    int index = lst->GetNextItem(-1,
                                 wxLIST_NEXT_ALL,
                                 wxLIST_STATE_SELECTED);

    wxString active = lst->GetItemText(index);
    if (active == _T("*"))
        return; // same thread

    wxString thread;
    wxListItem info;
    info.m_itemId = index;
    info.m_col = 1;
    info.m_mask = wxLIST_MASK_TEXT;
    if (lst->GetItem(info))
        thread = info.m_text;
    else
        return;

    unsigned long thread_num;
    if (thread.ToULong(&thread_num, 10))
        if (m_pDbg->GetState().HasDriver())
            m_pDbg->GetState().GetDriver()->SwitchThread(thread_num);
}

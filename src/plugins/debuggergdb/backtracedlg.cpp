#include <sdk.h>
#include "backtracedlg.h"
#include "debuggergdb.h"
#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/button.h>
#include <wx/wfstream.h>

BEGIN_EVENT_TABLE(BacktraceDlg, wxDialog)
    EVT_BUTTON(XRCID("btnSave"), BacktraceDlg::OnSave)
    EVT_BUTTON(XRCID("btnRefresh"), BacktraceDlg::OnRefresh)
    EVT_LIST_ITEM_ACTIVATED(XRCID("lstTrace"), BacktraceDlg::OnDblClick)
END_EVENT_TABLE()

BacktraceDlg::BacktraceDlg(wxWindow* parent, DebuggerGDB* debugger)
    : m_pDbg(debugger)
{
	//ctor
	wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgBacktrace"));
	SetWindowStyle(GetWindowStyle() | wxFRAME_FLOAT_ON_PARENT);

    wxListCtrl* lst = XRCCTRL(*this, "lstTrace", wxListCtrl);

	wxFont font(8, wxMODERN, wxNORMAL, wxNORMAL);
    lst->SetFont(font);
    Clear();
}

BacktraceDlg::~BacktraceDlg()
{
	//dtor
}

void BacktraceDlg::Clear()
{
    wxListCtrl* lst = XRCCTRL(*this, "lstTrace", wxListCtrl);
    lst->ClearAll();
	lst->Freeze();
	lst->DeleteAllItems();
    lst->InsertColumn(0, _("Nr"), wxLIST_FORMAT_LEFT);
    lst->InsertColumn(1, _("Address"), wxLIST_FORMAT_LEFT);
    lst->InsertColumn(2, _("Function"), wxLIST_FORMAT_LEFT);
    lst->InsertColumn(3, _("File"), wxLIST_FORMAT_LEFT, 128);
    lst->InsertColumn(4, _("Line"), wxLIST_FORMAT_RIGHT, 64);
	lst->Thaw();
}

void BacktraceDlg::AddFrame(const StackFrame& frame)
{
    wxListCtrl* lst = XRCCTRL(*this, "lstTrace", wxListCtrl);

    wxString addr = _T("??");
    if (frame.valid)
        addr.Printf(_T("0x%8.8x"), frame.address);

    wxString num = _T("??");
    if (frame.valid)
        num.Printf(_T("%d"), frame.number);

	lst->Freeze();
	lst->InsertItem(lst->GetItemCount(), num);
	int idx = lst->GetItemCount() - 1;
    lst->SetItem(idx, 1, addr);
    lst->SetItem(idx, 2, frame.valid ? frame.function : _T("??"));
    lst->SetItem(idx, 3, frame.valid ? frame.file : _T("??"));
    lst->SetItem(idx, 4, frame.valid ? frame.line : _T("??"));
	lst->Thaw();

	for (int i = 0; i < 3; ++i)
	{
        lst->SetColumnWidth(i, wxLIST_AUTOSIZE);
	}
}

void BacktraceDlg::OnDblClick(wxListEvent& event)
{
    wxListCtrl* lst = XRCCTRL(*this, "lstTrace", wxListCtrl);
    if (lst->GetSelectedItemCount() == 0)
        return;

    // find selected item index
    int index = lst->GetNextItem(-1,
                                 wxLIST_NEXT_ALL,
                                 wxLIST_STATE_SELECTED);

    wxListItem info;
    info.m_itemId = index;
    info.m_mask = wxLIST_MASK_TEXT;
    info.m_col = 3;
    wxString file = lst->GetItem(info) ? info.m_text : _T("");
    info.m_col = 4;
    wxString line = lst->GetItem(info) ? info.m_text : _T("");
    if (!file.IsEmpty() && !line.IsEmpty())
        m_pDbg->SyncEditor(file, atoi(line.mb_str()));
}

void BacktraceDlg::OnSave(wxCommandEvent& event)
{
    wxFileDialog dlg(this,
                        _("Save as text file"),
                        wxEmptyString,
                        wxEmptyString,
                        ALL_FILES_FILTER,
                        wxSAVE | wxOVERWRITE_PROMPT);
    if (dlg.ShowModal() != wxID_OK)
        return;

    wxFFileOutputStream output(dlg.GetPath());
    wxTextOutputStream text(output);

    wxListCtrl* lst = XRCCTRL(*this, "lstTrace", wxListCtrl);
    for (int i = 0; i < lst->GetItemCount(); ++i)
    {
        wxListItem info;
        info.m_itemId = i;
        info.m_col = 1;
        info.m_mask = wxLIST_MASK_TEXT;
        wxString addr = lst->GetItem(info) && !info.m_text.IsEmpty() ? info.m_text : _T("??");
        info.m_col = 2;
        wxString func = lst->GetItem(info) && !info.m_text.IsEmpty() ? info.m_text : _T("??");
        info.m_col = 3;
        wxString file = lst->GetItem(info) && !info.m_text.IsEmpty() ? info.m_text : _T("??");
        info.m_col = 4;
        wxString line = lst->GetItem(info) && !info.m_text.IsEmpty() ? info.m_text : _T("??");

        text << _T('#') << lst->GetItemText(i) << _T(' ')
            << addr << _T('\t')
            << func << _T(' ')
            << _T('(') << file << _T(':') << line << _T(')')
            << _T('\n');
    }
    wxMessageBox(_("File saved"), _("Result"), wxICON_INFORMATION);
}

void BacktraceDlg::OnRefresh(wxCommandEvent& event)
{
    m_pDbg->CmdBacktrace();
}

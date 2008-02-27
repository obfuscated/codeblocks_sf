/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#ifndef CB_PRECOMP
#include <wx/filedlg.h>
#endif
#include "backtracedlg.h"
#include "debuggergdb.h"
#include "debuggerdriver.h"
#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/button.h>
#include <wx/wfstream.h>
#include <wx/menu.h>
#include <globals.h>
#include <filefilters.h>

static const int idSwitch = wxNewId();
static const int idSave = wxNewId();
static const int idJump = wxNewId();

BEGIN_EVENT_TABLE(BacktraceDlg, wxPanel)
    EVT_LIST_ITEM_RIGHT_CLICK(XRCID("lstTrace"), BacktraceDlg::OnListRightClick)
    EVT_MENU(idSwitch, BacktraceDlg::OnSwitchFrame)
    EVT_MENU(idSave, BacktraceDlg::OnSave)
    EVT_MENU(idJump, BacktraceDlg::OnJump)
    EVT_LIST_ITEM_ACTIVATED(XRCID("lstTrace"), BacktraceDlg::OnDblClick)
END_EVENT_TABLE()

BacktraceDlg::BacktraceDlg(wxWindow* parent, DebuggerGDB* debugger)
    : m_pDbg(debugger)
{
    //ctor
    wxXmlResource::Get()->LoadPanel(this, parent, _T("dlgBacktrace"));
//    SetWindowStyle(GetWindowStyle() | wxFRAME_FLOAT_ON_PARENT);

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
    lst->InsertColumn(0, _("Nr"), wxLIST_FORMAT_RIGHT);
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
        addr.Printf(_T("%p"), (void*)frame.address);

    wxString num = _T("??");
    if (frame.valid)
        num.Printf(_T("%ld"), frame.number);

    lst->Freeze();
    lst->InsertItem(lst->GetItemCount(), num);
    int idx = lst->GetItemCount() - 1;
    lst->SetItem(idx, 1, addr);
    lst->SetItem(idx, 2, frame.valid ? frame.function : _T("??"));
    lst->SetItem(idx, 3, frame.valid ? frame.file : _T("??"));
    lst->SetItem(idx, 4, frame.valid ? frame.line : _T("??"));
    lst->Thaw();

    lst->SetColumnWidth(0, 32);
    for (int i = 1; i < 3; ++i)
    {
        lst->SetColumnWidth(i, wxLIST_AUTOSIZE);
    }
}

void BacktraceDlg::OnListRightClick(wxListEvent& event)
{
    wxListCtrl* lst = XRCCTRL(*this, "lstTrace", wxListCtrl);

    wxMenu m;
    m.Append(idJump, _("Jump to this file/line"));
    m.Append(idSwitch, _("Switch to this frame"));
    m.AppendSeparator();
    m.Append(idSave, _("Save to file..."));

    lst->PopupMenu(&m);
}

void BacktraceDlg::OnJump(wxCommandEvent& event)
{
    wxListEvent evt;
    OnDblClick(evt);
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
        m_pDbg->SyncEditor(file, atoi(line.mb_str()), false);
}

void BacktraceDlg::OnSave(wxCommandEvent& event)
{
    wxFileDialog dlg(this,
                        _("Save as text file"),
                        wxEmptyString,
                        wxEmptyString,
                        FileFilters::GetFilterAll(),
                        wxSAVE | wxOVERWRITE_PROMPT);
    PlaceWindow(&dlg);
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
    cbMessageBox(_("File saved"), _("Result"), wxICON_INFORMATION);
}

void BacktraceDlg::OnSwitchFrame(wxCommandEvent& event)
{
    wxListCtrl* lst = XRCCTRL(*this, "lstTrace", wxListCtrl);
    if (lst->GetSelectedItemCount() == 0)
        return;

    // find selected item index
    int index = lst->GetNextItem(-1,
                                 wxLIST_NEXT_ALL,
                                 wxLIST_STATE_SELECTED);
    // read the frame number from the first column
    long realFrameNr;
    if (lst->GetItemText(index).ToLong(&realFrameNr))
    {
        // switch to this frame
        if (m_pDbg->GetState().HasDriver())
            m_pDbg->GetState().GetDriver()->SwitchToFrame(realFrameNr);
    }
    else
        cbMessageBox(_("Couldn't find out the frame number!"), _("Error"), wxICON_ERROR);
}

/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#include "editwatchesdlg.h"
#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/listbox.h>
#include <wx/checkbox.h>
#include <wx/radiobox.h>
#include <wx/spinctrl.h>
#include <wx/textdlg.h>

BEGIN_EVENT_TABLE(EditWatchesDlg, wxScrollingDialog)
    EVT_BUTTON(XRCID("btnAdd"), EditWatchesDlg::OnAdd)
    EVT_BUTTON(XRCID("btnRemove"), EditWatchesDlg::OnRemove)
    EVT_LISTBOX(XRCID("lstWatches"), EditWatchesDlg::OnListboxClick)
    EVT_UPDATE_UI(-1, EditWatchesDlg::OnUpdateUI)
END_EVENT_TABLE()

EditWatchesDlg::EditWatchesDlg(WatchesArray& arr, wxWindow* parent)
    : m_LastSel(-1),
    m_Watches(arr)
{
    //ctor
    wxXmlResource::Get()->LoadObject(this, parent, _T("dlgEditWatches"),_T("wxScrollingDialog"));
    FillWatches();
    FillRecord(0);
}

EditWatchesDlg::~EditWatchesDlg()
{
    //dtor
}

void EditWatchesDlg::FillWatches()
{
    wxListBox* lst = XRCCTRL(*this, "lstWatches", wxListBox);
    lst->Clear();

    for (size_t i = 0; i < m_Watches.GetCount(); ++i)
    {
        Watch& w = m_Watches[i];
        lst->Append(w.keyword);
    }
}

void EditWatchesDlg::FillRecord(int sel)
{
    if (m_LastSel != -1)
    {
        // save old record
        m_Watches[m_LastSel].keyword = XRCCTRL(*this, "txtKeyword", wxTextCtrl)->GetValue();
        m_Watches[m_LastSel].format = (WatchFormat)XRCCTRL(*this, "rbFormat", wxRadioBox)->GetSelection();
        m_Watches[m_LastSel].is_array = XRCCTRL(*this, "chkArray", wxCheckBox)->GetValue();
        m_Watches[m_LastSel].array_start = XRCCTRL(*this, "spnArrStart", wxSpinCtrl)->GetValue();
        m_Watches[m_LastSel].array_count = XRCCTRL(*this, "spnArrCount", wxSpinCtrl)->GetValue();
        XRCCTRL(*this, "lstWatches", wxListBox)->SetString(m_LastSel, m_Watches[m_LastSel].keyword);
    }

    if (sel >= 0 && sel < (int)m_Watches.GetCount())
    {
        m_LastSel = sel;
        XRCCTRL(*this, "txtKeyword", wxTextCtrl)->SetValue(m_Watches[sel].keyword);
        XRCCTRL(*this, "rbFormat", wxRadioBox)->SetSelection((int)m_Watches[sel].format);
        XRCCTRL(*this, "chkArray", wxCheckBox)->SetValue(m_Watches[sel].is_array);
        XRCCTRL(*this, "spnArrStart", wxSpinCtrl)->SetValue(m_Watches[sel].array_start);
        XRCCTRL(*this, "spnArrCount", wxSpinCtrl)->SetValue(m_Watches[sel].array_count);
        XRCCTRL(*this, "lstWatches", wxListBox)->SetSelection(sel);
    }
}

void EditWatchesDlg::EndModal(int retCode)
{
    FillRecord(XRCCTRL(*this, "lstWatches", wxListBox)->GetSelection());
    wxScrollingDialog::EndModal(retCode);
}

void EditWatchesDlg::OnAdd(wxCommandEvent& WXUNUSED(event))
{
    wxString item = wxGetTextFromUser(_("Please enter the new watch keyword:"), _("Add watch"));
    if (!item.IsEmpty())
    {
        m_Watches.Add(Watch(item));
        XRCCTRL(*this, "lstWatches", wxListBox)->Append(item);
        FillRecord(XRCCTRL(*this, "lstWatches", wxListBox)->GetCount() - 1);
    }
}

void EditWatchesDlg::OnRemove(wxCommandEvent& WXUNUSED(event))
{
    int sel = XRCCTRL(*this, "lstWatches", wxListBox)->GetSelection();
    m_Watches.RemoveAt(sel);
    // m_LastSel must be reset before calling Delete, because Delete sends a ListBoxSelect event on linux and
    // therefore leads to a crash in FillRecord, if m_LastSel is not -1, but the last item was removed
    m_LastSel = -1;
    XRCCTRL(*this, "lstWatches", wxListBox)->Delete(sel);
    FillWatches();
    sel = sel == (int)XRCCTRL(*this, "lstWatches", wxListBox)->GetCount() ? sel - 1 : sel;
    FillRecord(sel);
}

void EditWatchesDlg::OnListboxClick(wxCommandEvent& WXUNUSED(event))
{
    FillRecord(XRCCTRL(*this, "lstWatches", wxListBox)->GetSelection());
}

void EditWatchesDlg::OnUpdateUI(wxUpdateUIEvent& WXUNUSED(event))
{
    bool en = XRCCTRL(*this, "lstWatches", wxListBox)->GetSelection() != -1;
    XRCCTRL(*this, "btnRemove", wxButton)->Enable(en);
    XRCCTRL(*this, "txtKeyword", wxTextCtrl)->Enable(en);
    XRCCTRL(*this, "rbFormat", wxRadioBox)->Enable(en);
}

#include <sdk.h>
#include "editwatchesdlg.h"
#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/listbox.h>
#include <wx/radiobox.h>
#include <wx/spinctrl.h>
#include <wx/textdlg.h>

BEGIN_EVENT_TABLE(EditWatchesDlg, wxDialog)
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
    wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgEditWatches"));
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
        m_Watches[sel].array_start = XRCCTRL(*this, "spnArrStart", wxSpinCtrl)->GetValue();
        m_Watches[sel].array_count = XRCCTRL(*this, "spnArrCount", wxSpinCtrl)->GetValue();
        XRCCTRL(*this, "lstWatches", wxListBox)->SetString(m_LastSel, m_Watches[m_LastSel].keyword);
    }

    if (sel >= 0 && sel < (int)m_Watches.GetCount())
    {
        m_LastSel = sel;
        XRCCTRL(*this, "txtKeyword", wxTextCtrl)->SetValue(m_Watches[sel].keyword);
        XRCCTRL(*this, "rbFormat", wxRadioBox)->SetSelection((int)m_Watches[sel].format);
        XRCCTRL(*this, "spnArrStart", wxSpinCtrl)->SetValue(m_Watches[sel].array_start);
        XRCCTRL(*this, "spnArrCount", wxSpinCtrl)->SetValue(m_Watches[sel].array_count);
        XRCCTRL(*this, "lstWatches", wxListBox)->SetSelection(sel);
    }
}

void EditWatchesDlg::EndModal(int retCode)
{
    FillRecord(XRCCTRL(*this, "lstWatches", wxListBox)->GetSelection());
    wxDialog::EndModal(retCode);
}

void EditWatchesDlg::OnAdd(wxCommandEvent& event)
{
    wxString item = wxGetTextFromUser(_("Please enter the new watch keyword:"), _("Add watch"));
    if (!item.IsEmpty())
    {
        m_Watches.Add(Watch(item));
        XRCCTRL(*this, "lstWatches", wxListBox)->Append(item);
        FillRecord(XRCCTRL(*this, "lstWatches", wxListBox)->GetCount() - 1);
    }
}

void EditWatchesDlg::OnRemove(wxCommandEvent& event)
{
    int sel = XRCCTRL(*this, "lstWatches", wxListBox)->GetSelection();
    m_Watches.RemoveAt(sel);
    XRCCTRL(*this, "lstWatches", wxListBox)->Delete(sel);
    m_LastSel = -1;
    FillWatches();
    sel = sel == XRCCTRL(*this, "lstWatches", wxListBox)->GetCount() - 1 ? sel - 1 : sel;
    FillRecord(sel);
}

void EditWatchesDlg::OnListboxClick(wxCommandEvent& event)
{
    FillRecord(XRCCTRL(*this, "lstWatches", wxListBox)->GetSelection());
}

void EditWatchesDlg::OnUpdateUI(wxUpdateUIEvent& event)
{
    bool en = XRCCTRL(*this, "lstWatches", wxListBox)->GetSelection() != -1;
    XRCCTRL(*this, "btnRemove", wxButton)->Enable(en);
    XRCCTRL(*this, "txtKeyword", wxTextCtrl)->Enable(en);
    XRCCTRL(*this, "rbFormat", wxRadioBox)->Enable(en);
}

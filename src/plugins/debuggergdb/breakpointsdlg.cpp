#include <sdk.h>
#include "breakpointsdlg.h"
#include <manager.h>
#include <editormanager.h>
#include <cbeditor.h>
#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/listbox.h>
#include <wx/checkbox.h>
#include <wx/spinctrl.h>

BEGIN_EVENT_TABLE(BreakpointsDlg, wxDialog)
    EVT_BUTTON(XRCID("btnRemove"), BreakpointsDlg::OnRemove)
    EVT_BUTTON(XRCID("btnRemoveAll"), BreakpointsDlg::OnRemoveAll)
    EVT_LISTBOX(XRCID("lstBreakpoints"), BreakpointsDlg::OnListboxClick)
    EVT_UPDATE_UI(-1, BreakpointsDlg::OnUpdateUI)
END_EVENT_TABLE()

BreakpointsDlg::BreakpointsDlg(BreakpointsList& list, wxWindow* parent)
    : m_LastSel(-1),
    m_List(list)
{
    //ctor
	wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgBreakpoints"));
    wxFont font(8, wxMODERN, wxNORMAL, wxNORMAL);
	XRCCTRL(*this, "lstBreakpoints", wxListBox)->SetFont(font);
	FillBreakpoints();
	FillRecord(0);
}

BreakpointsDlg::~BreakpointsDlg()
{
    //dtor
}

void BreakpointsDlg::FillBreakpoints()
{
    wxListBox* lst = XRCCTRL(*this, "lstBreakpoints", wxListBox);
    lst->Clear();

    for (unsigned int i = 0; i < m_List.GetCount(); ++i)
    {
        DebuggerBreakpoint* bp = m_List[i];
        if (bp->temporary)
            continue;
        wxString filename = bp->filename;
        filename << _T(" : ") << wxString::Format(_T("%d"), bp->line + 1);
        lst->Append(filename, bp);
    }
}

void BreakpointsDlg::FillRecord(int sel)
{
    wxListBox* lst = XRCCTRL(*this, "lstBreakpoints", wxListBox);
    if (m_LastSel != -1)
    {
        // save old record
        DebuggerBreakpoint* bp = static_cast<DebuggerBreakpoint*>(lst->GetClientData(m_LastSel));
        if (bp)
        {
            bp->enabled = XRCCTRL(*this, "chkEnabled", wxCheckBox)->GetValue();
            bp->useIgnoreCount = XRCCTRL(*this, "chkIgnore", wxCheckBox)->IsChecked();
            bp->ignoreCount = XRCCTRL(*this, "spnIgnoreCount", wxSpinCtrl)->GetValue();
            bp->useCondition = XRCCTRL(*this, "chkExpr", wxCheckBox)->IsChecked();
            bp->condition = XRCCTRL(*this, "txtExpr", wxTextCtrl)->GetValue();
        }
    }

    if (sel >= 0 && sel < (int)lst->GetCount())
    {
        m_LastSel = sel;
        DebuggerBreakpoint* bp = static_cast<DebuggerBreakpoint*>(lst->GetClientData(sel));
        if (bp)
        {
            XRCCTRL(*this, "chkEnabled", wxCheckBox)->SetValue(bp->enabled);
            XRCCTRL(*this, "chkIgnore", wxCheckBox)->SetValue(bp->useIgnoreCount);
            XRCCTRL(*this, "spnIgnoreCount", wxSpinCtrl)->SetValue(bp->ignoreCount);
            XRCCTRL(*this, "chkExpr", wxCheckBox)->SetValue(bp->useCondition);
            XRCCTRL(*this, "txtExpr", wxTextCtrl)->SetValue(bp->condition);
        }
        lst->SetSelection(sel);
    }
    else
    {
        XRCCTRL(*this, "chkEnabled", wxCheckBox)->SetValue(false);
        XRCCTRL(*this, "chkIgnore", wxCheckBox)->SetValue(false);
        XRCCTRL(*this, "spnIgnoreCount", wxSpinCtrl)->SetValue(0);
        XRCCTRL(*this, "chkExpr", wxCheckBox)->SetValue(false);
        XRCCTRL(*this, "txtExpr", wxTextCtrl)->SetValue(_T(""));
    }
}

void BreakpointsDlg::EndModal(int retCode)
{
    FillRecord(XRCCTRL(*this, "lstBreakpoints", wxListBox)->GetSelection());
    wxDialog::EndModal(retCode);
}

void BreakpointsDlg::RemoveBreakpoint(int sel)
{
    wxListBox* lst = XRCCTRL(*this, "lstBreakpoints", wxListBox);
    if (sel >= 0 && sel < (int)lst->GetCount())
    {
        DebuggerBreakpoint* bp = m_List[sel];
        cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(bp->filename);
        if (ed)
        {
            ed->ToggleBreakpoint(bp->line);
        }
    }
}

void BreakpointsDlg::OnRemove(wxCommandEvent& event)
{
    RemoveBreakpoint(XRCCTRL(*this, "lstBreakpoints", wxListBox)->GetSelection());
    m_LastSel = -1;
    FillBreakpoints();
    FillRecord(0);
}

void BreakpointsDlg::OnRemoveAll(wxCommandEvent& event)
{
    wxListBox* lst = XRCCTRL(*this, "lstBreakpoints", wxListBox);
    for (int i = 0; i < lst->GetCount(); ++i)
        RemoveBreakpoint(i);
    m_LastSel = -1;
    FillBreakpoints();
    FillRecord(0);
}

void BreakpointsDlg::OnListboxClick(wxCommandEvent& event)
{
    FillRecord(XRCCTRL(*this, "lstBreakpoints", wxListBox)->GetSelection());
}

void BreakpointsDlg::OnUpdateUI(wxUpdateUIEvent& event)
{
    bool en = XRCCTRL(*this, "chkEnabled", wxCheckBox)->IsChecked();
    bool sel = XRCCTRL(*this, "lstBreakpoints", wxListBox)->GetSelection() != -1;
    bool cnt = XRCCTRL(*this, "lstBreakpoints", wxListBox)->GetCount() != 0;
    XRCCTRL(*this, "chkEnabled", wxCheckBox)->Enable(sel);
    XRCCTRL(*this, "btnRemove", wxButton)->Enable(sel);
    XRCCTRL(*this, "btnRemoveAll", wxButton)->Enable(cnt);
    XRCCTRL(*this, "chkIgnore", wxCheckBox)->Enable(sel && en && !XRCCTRL(*this, "chkExpr", wxCheckBox)->IsChecked());
    XRCCTRL(*this, "spnIgnoreCount", wxSpinCtrl)->Enable(sel && en && XRCCTRL(*this, "chkIgnore", wxCheckBox)->IsChecked());
    XRCCTRL(*this, "chkExpr", wxCheckBox)->Enable(sel && en && !XRCCTRL(*this, "chkIgnore", wxCheckBox)->IsChecked());
    XRCCTRL(*this, "txtExpr", wxTextCtrl)->Enable(sel && en && XRCCTRL(*this, "chkExpr", wxCheckBox)->IsChecked());
}

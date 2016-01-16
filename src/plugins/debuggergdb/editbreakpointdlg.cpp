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
    #include <wx/checkbox.h>
    #include <wx/intl.h>
    #include <wx/button.h>
    #include <wx/listbox.h>
    #include <wx/spinctrl.h>
    #include <wx/textctrl.h>
    #include <wx/xrc/xmlres.h>

    #include <debuggermanager.h>
#endif

#include "editbreakpointdlg.h"

BEGIN_EVENT_TABLE(EditBreakpointDlg, wxScrollingDialog)
    EVT_UPDATE_UI(-1, EditBreakpointDlg::OnUpdateUI)
END_EVENT_TABLE()

EditBreakpointDlg::EditBreakpointDlg(const DebuggerBreakpoint &breakpoint, wxWindow* parent)
    : m_breakpoint(breakpoint)
{
    //ctor
    wxXmlResource::Get()->LoadObject(this, parent, _T("dlgEditBreakpoint"),_T("wxScrollingDialog"));

    XRCCTRL(*this, "chkEnabled", wxCheckBox)->SetValue(m_breakpoint.enabled);
    XRCCTRL(*this, "chkIgnore", wxCheckBox)->SetValue(m_breakpoint.useIgnoreCount);
    XRCCTRL(*this, "spnIgnoreCount", wxSpinCtrl)->SetValue(m_breakpoint.ignoreCount);
    XRCCTRL(*this, "chkExpr", wxCheckBox)->SetValue(m_breakpoint.useCondition);
    XRCCTRL(*this, "txtExpr", wxTextCtrl)->SetValue(m_breakpoint.condition);

    // Limit vertical resizing.
    SetMaxSize(wxSize(-1, GetMinHeight()));
}

EditBreakpointDlg::~EditBreakpointDlg()
{
    //dtor
}

void EditBreakpointDlg::EndModal(int retCode)
{
    if (retCode == wxID_OK)
    {
        m_breakpoint.enabled = XRCCTRL(*this, "chkEnabled", wxCheckBox)->GetValue();
        m_breakpoint.useIgnoreCount = XRCCTRL(*this, "chkIgnore", wxCheckBox)->IsChecked();
        m_breakpoint.ignoreCount = XRCCTRL(*this, "spnIgnoreCount", wxSpinCtrl)->GetValue();
        m_breakpoint.useCondition = XRCCTRL(*this, "chkExpr", wxCheckBox)->IsChecked();
        m_breakpoint.condition = CleanStringValue(XRCCTRL(*this, "txtExpr", wxTextCtrl)->GetValue());
    }
    wxScrollingDialog::EndModal(retCode);
}

void EditBreakpointDlg::OnUpdateUI(cb_unused wxUpdateUIEvent& event)
{
    bool en = XRCCTRL(*this, "chkEnabled", wxCheckBox)->IsChecked();
    XRCCTRL(*this, "chkIgnore", wxCheckBox)->Enable(en && !XRCCTRL(*this, "chkExpr", wxCheckBox)->IsChecked());
    XRCCTRL(*this, "spnIgnoreCount", wxSpinCtrl)->Enable(en && XRCCTRL(*this, "chkIgnore", wxCheckBox)->IsChecked());
    XRCCTRL(*this, "chkExpr", wxCheckBox)->Enable(en && !XRCCTRL(*this, "chkIgnore", wxCheckBox)->IsChecked());
    XRCCTRL(*this, "txtExpr", wxTextCtrl)->Enable(en && XRCCTRL(*this, "chkExpr", wxCheckBox)->IsChecked());
}

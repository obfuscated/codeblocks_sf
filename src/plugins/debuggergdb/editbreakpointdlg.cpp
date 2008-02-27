/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#include "editbreakpointdlg.h"
#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/listbox.h>
#include <wx/checkbox.h>
#include <wx/spinctrl.h>

BEGIN_EVENT_TABLE(EditBreakpointDlg, wxDialog)
    EVT_UPDATE_UI(-1, EditBreakpointDlg::OnUpdateUI)
END_EVENT_TABLE()

EditBreakpointDlg::EditBreakpointDlg(DebuggerBreakpoint* bp, wxWindow* parent)
    : m_Bp(bp)
{
    //ctor
    wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgEditBreakpoint"));

    XRCCTRL(*this, "chkEnabled", wxCheckBox)->SetValue(bp->enabled);
    XRCCTRL(*this, "chkIgnore", wxCheckBox)->SetValue(bp->useIgnoreCount);
    XRCCTRL(*this, "spnIgnoreCount", wxSpinCtrl)->SetValue(bp->ignoreCount);
    XRCCTRL(*this, "chkExpr", wxCheckBox)->SetValue(bp->useCondition);
    XRCCTRL(*this, "txtExpr", wxTextCtrl)->SetValue(bp->condition);
}

EditBreakpointDlg::~EditBreakpointDlg()
{
    //dtor
}

void EditBreakpointDlg::EndModal(int retCode)
{
    if (retCode == wxID_OK)
    {
        m_Bp->enabled = XRCCTRL(*this, "chkEnabled", wxCheckBox)->GetValue();
        m_Bp->useIgnoreCount = XRCCTRL(*this, "chkIgnore", wxCheckBox)->IsChecked();
        m_Bp->ignoreCount = XRCCTRL(*this, "spnIgnoreCount", wxSpinCtrl)->GetValue();
        m_Bp->useCondition = XRCCTRL(*this, "chkExpr", wxCheckBox)->IsChecked();
        m_Bp->condition = XRCCTRL(*this, "txtExpr", wxTextCtrl)->GetValue();
    }
    wxDialog::EndModal(retCode);
}

void EditBreakpointDlg::OnUpdateUI(wxUpdateUIEvent& event)
{
    bool en = XRCCTRL(*this, "chkEnabled", wxCheckBox)->IsChecked();
    XRCCTRL(*this, "chkIgnore", wxCheckBox)->Enable(en && !XRCCTRL(*this, "chkExpr", wxCheckBox)->IsChecked());
    XRCCTRL(*this, "spnIgnoreCount", wxSpinCtrl)->Enable(en && XRCCTRL(*this, "chkIgnore", wxCheckBox)->IsChecked());
    XRCCTRL(*this, "chkExpr", wxCheckBox)->Enable(en && !XRCCTRL(*this, "chkIgnore", wxCheckBox)->IsChecked());
    XRCCTRL(*this, "txtExpr", wxTextCtrl)->Enable(en && XRCCTRL(*this, "chkExpr", wxCheckBox)->IsChecked());
}

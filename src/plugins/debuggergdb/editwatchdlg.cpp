/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#include "editwatchdlg.h"

#ifndef CB_PRECOMP
    #include <wx/button.h>
    #include <wx/checkbox.h>
    #include <wx/defs.h>
    #include <wx/intl.h>
    #include <wx/radiobox.h>
    #include <wx/sizer.h>
    #include <wx/spinctrl.h>
    #include <wx/textctrl.h>
    #include <wx/xrc/xmlres.h>
#endif // CB_PRECOMP

#include "debugger_defs.h"

EditWatchDlg::EditWatchDlg(cb::shared_ptr<GDBWatch> w, wxWindow* parent)
    : m_watch(w)
{
    //ctor
    wxXmlResource::Get()->LoadObject(this, parent, _T("dlgEditWatch"),_T("wxScrollingDialog"));

    if (m_watch)
    {
        wxString symbol;
        m_watch->GetSymbol(symbol);

        XRCCTRL(*this, "txtKeyword", wxTextCtrl)->SetValue(symbol);
        XRCCTRL(*this, "rbFormat", wxRadioBox)->SetSelection((int)m_watch->GetFormat());
        XRCCTRL(*this, "chkArray", wxCheckBox)->SetValue(m_watch->IsArray());
        XRCCTRL(*this, "spnArrStart", wxSpinCtrl)->SetValue(m_watch->GetArrayStart());
        XRCCTRL(*this, "spnArrCount", wxSpinCtrl)->SetValue(m_watch->GetArrayCount());
    }
    XRCCTRL(*this, "txtKeyword", wxTextCtrl)->SetFocus();
    XRCCTRL(*this, "wxID_OK", wxButton)->SetDefault();
}

EditWatchDlg::~EditWatchDlg()
{
    //dtor
}

void EditWatchDlg::EndModal(int retCode)
{
    if (retCode == wxID_OK && m_watch)
    {
        m_watch->SetSymbol(CleanStringValue(XRCCTRL(*this, "txtKeyword", wxTextCtrl)->GetValue()));
        m_watch->SetFormat((WatchFormat)XRCCTRL(*this, "rbFormat", wxRadioBox)->GetSelection());
        m_watch->SetArray(XRCCTRL(*this, "chkArray", wxCheckBox)->GetValue());
        m_watch->SetArrayParams(XRCCTRL(*this, "spnArrStart", wxSpinCtrl)->GetValue(),
                                XRCCTRL(*this, "spnArrCount", wxSpinCtrl)->GetValue());
    }
    wxScrollingDialog::EndModal(retCode);
}

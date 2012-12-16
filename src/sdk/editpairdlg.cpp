/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/xrc/xmlres.h>
    #include <wx/button.h>
    #include <wx/textctrl.h>
    #include "globals.h"
#endif

#include <wx/filedlg.h>
#include "editpairdlg.h"
#include "filefilters.h"

BEGIN_EVENT_TABLE(EditPairDlg, wxScrollingDialog)
    EVT_BUTTON(XRCID("btnBrowse"), EditPairDlg::OnBrowse)
    EVT_UPDATE_UI(-1, EditPairDlg::OnUpdateUI)
END_EVENT_TABLE()

EditPairDlg::EditPairDlg(wxWindow* parent, wxString& key, wxString& value, const wxString& title, BrowseMode allowBrowse)
    : m_Key(key),
    m_Value(value),
    m_BrowseMode(allowBrowse)
{
    //ctor
    wxXmlResource::Get()->LoadObject(this, parent, _T("dlgEditPair"),_T("wxScrollingDialog"));
    SetTitle(title);
    XRCCTRL(*this, "btnBrowse", wxButton)->Enable(m_BrowseMode != bmDisable);
    XRCCTRL(*this, "txtKey", wxTextCtrl)->SetValue(key);
    XRCCTRL(*this, "txtValue", wxTextCtrl)->SetValue(value);
}

EditPairDlg::~EditPairDlg()
{
    //dtor
}

void EditPairDlg::OnUpdateUI(cb_unused wxUpdateUIEvent& event)
{
    XRCCTRL(*this, "wxID_OK", wxButton)->Enable(!XRCCTRL(*this, "txtKey", wxTextCtrl)->GetValue().IsEmpty());
}

void EditPairDlg::OnBrowse(cb_unused wxCommandEvent& event)
{
    switch (m_BrowseMode)
    {
        case bmBrowseForFile:
        {
            wxFileDialog dlg(this,
                            _("Select file"),
                            XRCCTRL(*this, "txtValue", wxTextCtrl)->GetValue(),
                            _T(""),
                            FileFilters::GetFilterAll(),
                            wxFD_OPEN | compatibility::wxHideReadonly);
            PlaceWindow(&dlg);
            if (dlg.ShowModal() == wxID_OK)
                XRCCTRL(*this, "txtValue", wxTextCtrl)->SetValue(dlg.GetPath());
            break;
        }
        case bmBrowseForDirectory:
        {
            wxString dir = ChooseDirectory(this,
                                            _("Select directory"),
                                            XRCCTRL(*this, "txtValue", wxTextCtrl)->GetValue(),
                                            _T(""),
                                            false,
                                            true);
            if (!dir.IsEmpty())
                XRCCTRL(*this, "txtValue", wxTextCtrl)->SetValue(dir);
            break;
        }
        case bmDisable: // fall through
        default: break;
    }
}

void EditPairDlg::EndModal(int retCode)
{
    if (retCode == wxID_OK)
    {
        m_Key = XRCCTRL(*this, "txtKey", wxTextCtrl)->GetValue();
        m_Value = XRCCTRL(*this, "txtValue", wxTextCtrl)->GetValue();
    }
    wxScrollingDialog::EndModal(retCode);
}

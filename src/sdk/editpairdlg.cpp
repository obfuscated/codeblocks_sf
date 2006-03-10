/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is distributed under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
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

BEGIN_EVENT_TABLE(EditPairDlg, wxDialog)
    EVT_BUTTON(XRCID("btnBrowse"), EditPairDlg::OnBrowse)
    EVT_UPDATE_UI(-1, EditPairDlg::OnUpdateUI)
END_EVENT_TABLE()

EditPairDlg::EditPairDlg(wxWindow* parent, wxString& key, wxString& value, const wxString& title, BrowseMode allowBrowse)
    : m_Key(key),
    m_Value(value),
    m_BrowseMode(allowBrowse)
{
    //ctor
	wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgEditPair"));
	SetTitle(title);
    XRCCTRL(*this, "btnBrowse", wxButton)->Enable(m_BrowseMode != bmDisable);
    XRCCTRL(*this, "txtKey", wxTextCtrl)->SetValue(key);
    XRCCTRL(*this, "txtValue", wxTextCtrl)->SetValue(value);
}

EditPairDlg::~EditPairDlg()
{
    //dtor
}

void EditPairDlg::OnUpdateUI(wxUpdateUIEvent& event)
{
    XRCCTRL(*this, "wxID_OK", wxButton)->Enable(!XRCCTRL(*this, "txtKey", wxTextCtrl)->GetValue().IsEmpty());
}

void EditPairDlg::OnBrowse(wxCommandEvent& event)
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
                            wxOPEN);
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
    wxDialog::EndModal(retCode);
}

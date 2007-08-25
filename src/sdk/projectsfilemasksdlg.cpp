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
    #include <wx/intl.h>
    #include <wx/button.h>
    #include <wx/textctrl.h>
    #include <wx/msgdlg.h>
    #include <wx/listbox.h>
    #include "globals.h"
#endif

#include <wx/textdlg.h>
#include "projectsfilemasksdlg.h"

BEGIN_EVENT_TABLE(ProjectsFileMasksDlg, wxDialog)
    EVT_UPDATE_UI( -1, ProjectsFileMasksDlg::OnUpdateUI)
    EVT_BUTTON(XRCID("btnAdd"), ProjectsFileMasksDlg::OnAdd)
    EVT_BUTTON(XRCID("btnEdit"), ProjectsFileMasksDlg::OnEdit)
    EVT_BUTTON(XRCID("btnDelete"), ProjectsFileMasksDlg::OnDelete)
    EVT_BUTTON(XRCID("btnSetDefault"), ProjectsFileMasksDlg::OnSetDefault)
    EVT_LISTBOX(XRCID("lstCategories"), ProjectsFileMasksDlg::OnListChanged)
END_EVENT_TABLE()

ProjectsFileMasksDlg::ProjectsFileMasksDlg(wxWindow* parent, FilesGroupsAndMasks* fgam)
    : m_FileGroups(*fgam), // store a local copy, so if we press "Cancel", we can revert to the original...
    m_pOrigFileGroups(fgam),
    m_LastListSelection(0)
{
    wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgProjectsFileMasks"));

    RebuildList();
}

ProjectsFileMasksDlg::~ProjectsFileMasksDlg()
{
    //dtor
}

void ProjectsFileMasksDlg::RebuildList()
{
    wxListBox* pList = XRCCTRL(*this, "lstCategories", wxListBox);
    pList->Clear();
    for (unsigned int i = 0; i < m_FileGroups.GetGroupsCount(); ++i)
    {
        pList->Append(m_FileGroups.GetGroupName(i));
    }
    if (pList->GetCount() != 0)
    {
        pList->SetSelection(m_LastListSelection);
        ListChange();
    }
}

void ProjectsFileMasksDlg::ListChange()
{
    wxTextCtrl* pText = XRCCTRL(*this, "txtFileMasks", wxTextCtrl);
    int sel = XRCCTRL(*this, "lstCategories", wxListBox)->GetSelection();

    if (sel != m_LastListSelection)
    {
        // switching group; see if the user changed the masks...
        if (pText->GetValue() != m_FileGroups.GetFileMasks(m_LastListSelection))
            m_FileGroups.SetFileMasks(m_LastListSelection, pText->GetValue());
    }

    pText->SetValue(m_FileGroups.GetFileMasks(sel));
    m_LastListSelection = sel;
}

void ProjectsFileMasksDlg::OnUpdateUI(wxUpdateUIEvent& event)
{
    int sel = XRCCTRL(*this, "lstCategories", wxListBox)->GetSelection();

    XRCCTRL(*this, "btnEdit", wxButton)->Enable(sel >= 0);
    XRCCTRL(*this, "btnDelete", wxButton)->Enable(sel >= 0);
}

void ProjectsFileMasksDlg::OnAdd(wxCommandEvent& event)
{
    wxString groupName = wxGetTextFromUser(_("Enter the new group name:"),
                                            _("New group"));
    if (groupName.IsEmpty())
        return;
    m_FileGroups.AddGroup(groupName);
    wxListBox* pList = XRCCTRL(*this, "lstCategories", wxListBox);
    pList->Append(groupName);
    pList->SetSelection(pList->GetCount() - 1);
    ListChange();
    XRCCTRL(*this, "txtFileMasks", wxTextCtrl)->SetFocus();
}

void ProjectsFileMasksDlg::OnEdit(wxCommandEvent& event)
{
    wxListBox* pList = XRCCTRL(*this, "lstCategories", wxListBox);
    wxString oldName = pList->GetStringSelection();
    wxString groupName = wxGetTextFromUser(_("Rename the group:"),
                                            _("Edit group"), oldName);
    if (!groupName.IsEmpty() && groupName != oldName)
    {
        m_FileGroups.RenameGroup(pList->GetSelection(), groupName);
        pList->SetString(pList->GetSelection(), groupName);
    }
}

void ProjectsFileMasksDlg::OnDelete(wxCommandEvent& event)
{
    wxListBox* pList = XRCCTRL(*this, "lstCategories", wxListBox);
    wxString name = pList->GetStringSelection();
    wxString caption;
    caption.Printf(_("Are you sure you want to delete the group \"%s\"?"), name.c_str());
    wxMessageDialog dlg(this,
                        caption,
                        _("Confirmation"),
                        wxYES_NO | wxNO_DEFAULT | wxCENTRE | wxICON_QUESTION);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_NO)
        return;
    m_FileGroups.DeleteGroup(pList->GetSelection());
    RebuildList();
}

void ProjectsFileMasksDlg::OnSetDefault(wxCommandEvent& event)
{
    m_FileGroups.SetDefault();
    RebuildList();
}

void ProjectsFileMasksDlg::OnListChanged(wxCommandEvent& event)
{
    ListChange();
}

void ProjectsFileMasksDlg::EndModal(int retCode)
{
    if (retCode == wxID_OK)
    {
        wxTextCtrl* pText = XRCCTRL(*this, "txtFileMasks", wxTextCtrl);
        if (pText->GetValue() != m_FileGroups.GetFileMasks(m_LastListSelection))
            m_FileGroups.SetFileMasks(m_LastListSelection, pText->GetValue());

        m_pOrigFileGroups->CopyFrom(m_FileGroups);
    }

    wxDialog::EndModal(retCode);
}

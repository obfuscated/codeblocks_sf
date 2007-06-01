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
    #include "cbproject.h"
    #include "manager.h"
    #include "projectmanager.h"
    #include <wx/intl.h>
    #include <wx/xrc/xmlres.h>
    #include <wx/combobox.h>
    #include <wx/msgdlg.h>
    #include <wx/checklst.h>
#endif

#include "projectdepsdlg.h"


BEGIN_EVENT_TABLE(ProjectDepsDlg, wxDialog)
    EVT_COMBOBOX(XRCID("cmbProject"), ProjectDepsDlg::OnProjectChange)
END_EVENT_TABLE()

ProjectDepsDlg::ProjectDepsDlg(wxWindow* parent, cbProject* sel)
    : m_LastSel(-1)
{
    //ctor
    wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgConfigureProjectDeps"));

    wxComboBox* cmb = XRCCTRL(*this, "cmbProject", wxComboBox);

    int idx = 0;
    ProjectsArray* mainarr = Manager::Get()->GetProjectManager()->GetProjects();
    for (size_t i = 0; i < mainarr->GetCount(); ++i)
    {
        cbProject* prj = mainarr->Item(i);
        cmb->Append(prj->GetTitle(), prj);
        if (prj == sel)
            idx = i;
    }
    cmb->SetSelection(idx);
    m_LastSel = idx;
    FillList();
}

ProjectDepsDlg::~ProjectDepsDlg()
{
    //dtor
}

bool ProjectDepsDlg::SaveList()
{
    wxComboBox* cmb = XRCCTRL(*this, "cmbProject", wxComboBox);
    wxCheckListBox* lst = XRCCTRL(*this, "lstDeps", wxCheckListBox);

    if (m_LastSel == -1)
        return true;

    cbProject* thisprj = static_cast<cbProject*>(cmb->GetClientData(m_LastSel));
    if (!thisprj)
        return true;

    // first clear all deps for this project
    Manager::Get()->GetProjectManager()->ClearProjectDependencies(thisprj);

    // now set the the new deps
    for (int i = 0; i < (int)lst->GetCount(); ++i)
    {
        if (!lst->IsChecked(i))
            continue;

        cbProject* prj = 0;

        ProjectsArray* mainarr = Manager::Get()->GetProjectManager()->GetProjects();
        for (size_t x = 0; x < mainarr->GetCount(); ++x)
        {
            if (mainarr->Item(x)->GetTitle() == lst->GetString(i))
            {
                prj = mainarr->Item(x);
                break;
            }
        }
        if (!prj)
            continue;

        if (!Manager::Get()->GetProjectManager()->AddProjectDependency(thisprj, prj))
        {
            cbMessageBox(wxString::Format(_("Cannot add project '%s' as a dependency to '%s' because this "
                                            "would cause a circular dependency error..."),
                                            thisprj->GetTitle().c_str(), prj->GetTitle().c_str()),
                        _("Error"), wxICON_ERROR);
            return false;
        }
    }
    return true;
}

void ProjectDepsDlg::FillList()
{
    wxComboBox* cmb = XRCCTRL(*this, "cmbProject", wxComboBox);
    wxCheckListBox* lst = XRCCTRL(*this, "lstDeps", wxCheckListBox);

    int idx = cmb->GetSelection();
    if (m_LastSel != idx && m_LastSel != -1)
    {
        // save old list
        SaveList();
    }
    m_LastSel = idx;
    if (idx == -1)
        return;

    cbProject* thisprj = static_cast<cbProject*>(cmb->GetClientData(idx));
    if (!thisprj)
        return;
    const ProjectsArray* arr = Manager::Get()->GetProjectManager()->GetDependenciesForProject(thisprj);

    lst->Clear();
    ProjectsArray* mainarr = Manager::Get()->GetProjectManager()->GetProjects();
    for (size_t i = 0; i < mainarr->GetCount(); ++i)
    {
        cbProject* prj = mainarr->Item(i);
        if (prj == thisprj)
            continue;
        lst->Append(prj->GetTitle());

        // check dependency
        lst->Check(lst->GetCount() - 1, arr && arr->Index(prj) != wxNOT_FOUND);
    }
}

void ProjectDepsDlg::OnProjectChange(wxCommandEvent& event)
{
    FillList();
}

void ProjectDepsDlg::EndModal(int retCode)
{
    if (SaveList())
        return wxDialog::EndModal(retCode);
}

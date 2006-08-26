#include "sdk_precomp.h"
#include "virtualbuildtargetsdlg.h"

#ifndef CB_PRECOMP
    #include "cbproject.h"
    #include "globals.h"
    #include "wx/textdlg.h"
    #include "wx/utils.h"
#endif

BEGIN_EVENT_TABLE(VirtualBuildTargetsDlg,wxDialog)
	//(*EventTable(VirtualBuildTargetsDlg)
	EVT_LISTBOX(XRCID("ID_LISTBOX1"),VirtualBuildTargetsDlg::OnAliasesSelect)
	EVT_BUTTON(XRCID("ID_BUTTON1"),VirtualBuildTargetsDlg::OnAddClick)
	EVT_BUTTON(XRCID("ID_BUTTON2"),VirtualBuildTargetsDlg::OnEditClick)
	EVT_BUTTON(XRCID("ID_BUTTON3"),VirtualBuildTargetsDlg::OnRemoveClick)
	EVT_CHECKLISTBOX(XRCID("ID_CHECKLISTBOX1"),VirtualBuildTargetsDlg::OnTargetsToggled)
	//*)

	EVT_UPDATE_UI(-1, VirtualBuildTargetsDlg::OnUpdateUI)
END_EVENT_TABLE()

VirtualBuildTargetsDlg::VirtualBuildTargetsDlg(wxWindow* parent,wxWindowID id, cbProject* project)
    : lstAliases(0),
    btnAdd(0),
    btnEdit(0),
    btnRemove(0),
    lstTargets(0),
    m_pProject(project)
{
	//(*Initialize(VirtualBuildTargetsDlg)
	wxXmlResource::Get()->LoadDialog(this,parent,_T("VirtualBuildTargetsDlg"));
	lstAliases = XRCCTRL(*this,"ID_LISTBOX1",wxListBox);
	btnAdd = XRCCTRL(*this,"ID_BUTTON1",wxButton);
	btnEdit = XRCCTRL(*this,"ID_BUTTON2",wxButton);
	btnRemove = XRCCTRL(*this,"ID_BUTTON3",wxButton);
	lstTargets = XRCCTRL(*this,"ID_CHECKLISTBOX1",wxCheckListBox);
	//*)

	// fill aliases
	wxArrayString virtuals = m_pProject->GetVirtualBuildTargets();
	lstAliases->Set(virtuals);
	lstAliases->SetSelection(0);

	// fill build targets
	for (int i = 0; i < m_pProject->GetBuildTargetsCount(); ++i)
        lstTargets->Append(m_pProject->GetBuildTarget(i)->GetTitle());
    CheckTargets();
}

VirtualBuildTargetsDlg::~VirtualBuildTargetsDlg()
{
}

void VirtualBuildTargetsDlg::SetVirtualTarget(const wxString& targetName)
{
    if (!lstTargets->IsEnabled())
        return;
    wxArrayString checked;
    for (int i = 0; i < lstTargets->GetCount(); ++i)
    {
        if (lstTargets->IsChecked(i))
            checked.Add(lstTargets->GetString(i));
    }

    if (checked.GetCount() > 0)
    {
        if (!m_pProject->DefineVirtualBuildTarget(targetName, checked))
            cbMessageBox(_("Failed to setup this virtual build target.\n"
                            "Check the debug log for more info..."), _("Error"), wxICON_ERROR);
    }
}

void VirtualBuildTargetsDlg::CheckTargets()
{
    const wxArrayString& group = m_pProject->GetVirtualBuildTargetGroup(lstAliases->GetStringSelection());
	for (int i = 0; i < m_pProject->GetBuildTargetsCount(); ++i)
	{
	    wxString tgtName = m_pProject->GetBuildTarget(i)->GetTitle();
	    bool check = group.Index(tgtName) != wxNOT_FOUND;
        lstTargets->Check(i, check);
	}
}

void VirtualBuildTargetsDlg::OnUpdateUI(wxUpdateUIEvent& event)
{
    bool hasSel = lstAliases->GetSelection() != -1;
    btnEdit->Enable(hasSel);
    btnRemove->Enable(hasSel);
    lstTargets->Enable(hasSel);
}

void VirtualBuildTargetsDlg::OnAddClick(wxCommandEvent& event)
{
    wxString targetName = wxGetTextFromUser(_("Enter the new virtual build target name:"),
                                            _("New virtual build target"));
    if (targetName.IsEmpty())
        return;

    if (lstAliases->FindString(targetName) != wxNOT_FOUND)
    {
        cbMessageBox(_("A virtual build target with this name already exists in this project!"),
                        _("Error"),
                        wxOK | wxCENTRE | wxICON_ERROR);
        return;
    }

    if (m_pProject->GetBuildTarget(targetName))
    {
        cbMessageBox(_("A real build target with this name already exists in this project!"),
                        _("Error"),
                        wxOK | wxCENTRE | wxICON_ERROR);
        return;
    }

    // add it with an empty group
    lstAliases->Append(targetName);
    lstAliases->SetSelection(lstAliases->GetCount() - 1);
    CheckTargets();
}

void VirtualBuildTargetsDlg::OnEditClick(wxCommandEvent& event)
{
    wxString targetName = wxGetTextFromUser(_("Enter the new virtual build target name:"),
                                            _("Edit virtual build target"),
                                            lstAliases->GetStringSelection());
    
    // is name unchanged, or user cancelled?
    if (targetName.IsEmpty() || targetName == lstAliases->GetStringSelection())
        return;

    if (lstAliases->FindString(targetName) != wxNOT_FOUND)
    {
        cbMessageBox(_("A virtual build target with this name already exists in this project!"),
                        _("Error"),
                        wxOK | wxCENTRE | wxICON_ERROR);
        return;
    }

    if (m_pProject->GetBuildTarget(targetName))
    {
        cbMessageBox(_("A real build target with this name already exists in this project!"),
                        _("Error"),
                        wxOK | wxCENTRE | wxICON_ERROR);
        return;
    }

    m_pProject->RemoveVirtualBuildTarget(lstAliases->GetStringSelection());
    lstAliases->SetString(lstAliases->GetSelection(), targetName);
    SetVirtualTarget(targetName);
}

void VirtualBuildTargetsDlg::OnRemoveClick(wxCommandEvent& event)
{
    if (cbMessageBox(_("Are you sure you want to remove this virtual build target?"), _("Confirmation"), wxYES_NO | wxICON_QUESTION) == wxID_NO)
        return;
    m_pProject->RemoveVirtualBuildTarget(lstAliases->GetStringSelection());
    lstAliases->Delete(lstAliases->GetSelection());
    lstAliases->SetSelection(0);
    CheckTargets();
}

void VirtualBuildTargetsDlg::OnAliasesSelect(wxCommandEvent& event)
{
    CheckTargets();
}

void VirtualBuildTargetsDlg::OnTargetsToggled(wxCommandEvent& event)
{
    SetVirtualTarget(lstAliases->GetStringSelection());
}

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
    #include "cbproject.h"
    #include "projectbuildtarget.h"
    #include <wx/xrc/xmlres.h>
    #include <wx/button.h>
    #include <wx/listbox.h>
    #include "globals.h"
#endif

#include "externaldepsdlg.h"
#include <wx/msgdlg.h>
#include "editpathdlg.h"

BEGIN_EVENT_TABLE(ExternalDepsDlg, wxScrollingDialog)
    EVT_UPDATE_UI(-1, ExternalDepsDlg::OnUpdateUI)
    EVT_LISTBOX_DCLICK(XRCID("lstAdditionalFiles"), ExternalDepsDlg::OnEditAdditional)
    EVT_LISTBOX_DCLICK(XRCID("lstExternalFiles"), ExternalDepsDlg::OnEditExternal)
    EVT_BUTTON(XRCID("btnAddAdditional"), ExternalDepsDlg::OnAddAdditional)
    EVT_BUTTON(XRCID("btnEditAdditional"), ExternalDepsDlg::OnEditAdditional)
    EVT_BUTTON(XRCID("btnDelAdditional"), ExternalDepsDlg::OnDelAdditional)
    EVT_BUTTON(XRCID("btnAddExternal"), ExternalDepsDlg::OnAddExternal)
    EVT_BUTTON(XRCID("btnEditExternal"), ExternalDepsDlg::OnEditExternal)
    EVT_BUTTON(XRCID("btnDelExternal"), ExternalDepsDlg::OnDelExternal)
END_EVENT_TABLE()

ExternalDepsDlg::ExternalDepsDlg(wxWindow* parent, cbProject* project, ProjectBuildTarget* target)
    : m_pProject(project),
    m_pTarget(target)
{
    //ctor
    wxXmlResource::Get()->LoadObject(this, parent, _T("dlgExternalDeps"),_T("wxScrollingDialog"));
    FillAdditional();
    FillExternal();
}

ExternalDepsDlg::~ExternalDepsDlg()
{
    //dtor
}

void ExternalDepsDlg::FillAdditional()
{
    wxListBox* lst = XRCCTRL(*this, "lstAdditionalFiles", wxListBox);
    lst->Clear();
    wxArrayString array = GetArrayFromString(m_pTarget->GetAdditionalOutputFiles());
    for (unsigned int i = 0; i < array.GetCount(); ++i)
    {
        lst->Append(array[i]);
    }
}

void ExternalDepsDlg::FillExternal()
{
    wxListBox* lst = XRCCTRL(*this, "lstExternalFiles", wxListBox);
    lst->Clear();
    wxArrayString array = GetArrayFromString(m_pTarget->GetExternalDeps());
    for (unsigned int i = 0; i < array.GetCount(); ++i)
    {
        lst->Append(array[i]);
    }
}

void ExternalDepsDlg::EndModal(int retCode)
{
    wxString deps;
    wxListBox* lst = XRCCTRL(*this, "lstExternalFiles", wxListBox);
    for (unsigned int i = 0; i < (unsigned int)lst->GetCount(); ++i)
    {
        deps << lst->GetString(i) << _T(';');
    }
    m_pTarget->SetExternalDeps(deps);

    wxString files;
    lst = XRCCTRL(*this, "lstAdditionalFiles", wxListBox);
    for (unsigned int i = 0; i < (unsigned int)lst->GetCount(); ++i)
    {
        files << lst->GetString(i) << _T(';');
    }
    m_pTarget->SetAdditionalOutputFiles(files);

    return wxScrollingDialog::EndModal(retCode);
}

//void ExternalDepsDlg::DoAdd(const wxString& listbox, const wxString& message)
#define DoAdd(listbox,message)                              \
{                                                           \
    wxListBox* lst = XRCCTRL(*this, listbox, wxListBox);    \
    EditPathDlg dlg(this,                                   \
                    m_pProject->GetBasePath(),              \
                    m_pProject->GetBasePath(),              \
                    message,                                \
                    wxEmptyString,                          \
                    false);                                 \
    PlaceWindow(&dlg);                                      \
    if (dlg.ShowModal() == wxID_OK)                         \
        lst->Append(dlg.GetPath());                         \
}

//void ExternalDepsDlg::DoEdit(const wxString& listbox, const wxString& message)
#define DoEdit(listbox,message)                             \
{                                                           \
    wxListBox* lst = XRCCTRL(*this, listbox, wxListBox);    \
    int sel = lst->GetSelection();                          \
    if (sel == -1)                                          \
        return;                                             \
    EditPathDlg dlg(this,                                   \
                    lst->GetStringSelection(),              \
                    m_pProject->GetBasePath(),              \
                    message,                                \
                    wxEmptyString,                          \
                    false);                                 \
    PlaceWindow(&dlg);                                      \
    if (dlg.ShowModal() == wxID_OK)                         \
        lst->SetString(sel, dlg.GetPath());                 \
}

//void ExternalDepsDlg::DoDel(const wxString& listbox)
#define DoDel(listbox)                                      \
{                                                           \
    wxListBox* lst = XRCCTRL(*this, listbox, wxListBox);    \
    int sel = lst->GetSelection();                          \
    if (sel == -1)                                          \
        return;                                             \
    if (cbMessageBox(_("Are you sure you want to remove this file?"), _("Remove file"), wxYES_NO | wxNO_DEFAULT, this) == wxID_NO)   \
        return;                                             \
    lst->Delete(sel);                                       \
}

void ExternalDepsDlg::OnAddAdditional(cb_unused wxCommandEvent& event)
{
    DoAdd("lstAdditionalFiles", _("Add additional output file"));
}

void ExternalDepsDlg::OnEditAdditional(cb_unused wxCommandEvent& event)
{
    DoEdit("lstAdditionalFiles", _("Edit additional output file"));
}

void ExternalDepsDlg::OnDelAdditional(cb_unused wxCommandEvent& event)
{
    DoDel("lstAdditionalFiles");
}

void ExternalDepsDlg::OnAddExternal(cb_unused wxCommandEvent& event)
{
    DoAdd("lstExternalFiles", _("Add external dependency file"));
}

void ExternalDepsDlg::OnEditExternal(cb_unused wxCommandEvent& event)
{
    DoEdit("lstExternalFiles", _("Edit external dependency file"));
}

void ExternalDepsDlg::OnDelExternal(cb_unused wxCommandEvent& event)
{
    DoDel("lstExternalFiles");
}

void ExternalDepsDlg::OnUpdateUI(cb_unused wxUpdateUIEvent& event)
{
    int selAdd = XRCCTRL(*this, "lstAdditionalFiles", wxListBox)->GetSelection();
    int selExt = XRCCTRL(*this, "lstExternalFiles", wxListBox)->GetSelection();

    XRCCTRL(*this, "btnEditAdditional", wxButton)->Enable(selAdd != -1);
    XRCCTRL(*this, "btnDelAdditional", wxButton)->Enable(selAdd != -1);
    XRCCTRL(*this, "btnEditExternal", wxButton)->Enable(selExt != -1);
    XRCCTRL(*this, "btnDelExternal", wxButton)->Enable(selExt != -1);
}

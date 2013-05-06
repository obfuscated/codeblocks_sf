/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk.h"
#ifndef CB_PRECOMP
  #include <wx/arrstr.h>
  #include <wx/choice.h>
  #include <wx/intl.h>
  #include <wx/spinctrl.h>
  #include <wx/textctrl.h>
  #include <wx/xrc/xmlres.h>
  #include "manager.h"
  #include "configmanager.h"
#endif
#include <wx/textdlg.h>
#include "addtododlg.h"

BEGIN_EVENT_TABLE(AddTodoDlg, wxScrollingDialog)
    EVT_BUTTON(XRCID("btAddUser"), AddTodoDlg::OnAddUser)
    EVT_BUTTON(XRCID("btDelUser"), AddTodoDlg::OnDelUser)
    EVT_BUTTON(XRCID("btAddType"), AddTodoDlg::OnAddType)
    EVT_BUTTON(XRCID("btDelType"), AddTodoDlg::OnDelType)
END_EVENT_TABLE()

AddTodoDlg::AddTodoDlg(wxWindow* parent, wxArrayString users, wxArrayString types, std::bitset<(int)tdctError+1> supportedTdcts) :
    m_Users(users),
    m_Types(types),
    m_supportedTdcts(supportedTdcts)
{
    wxXmlResource::Get()->LoadObject(this, parent, _T("dlgAddToDo"),_T("wxScrollingDialog"));

    wxString lastUser  = Manager::Get()->GetConfigManager(_T("todo_list"))->Read(_T("last_used_user"));
    wxString lastType  = Manager::Get()->GetConfigManager(_T("todo_list"))->Read(_T("last_used_type"));
    wxString lastStyle = Manager::Get()->GetConfigManager(_T("todo_list"))->Read(_T("last_used_style"));
    wxString lastPos   = Manager::Get()->GetConfigManager(_T("todo_list"))->Read(_T("last_used_position"));
    bool lastDateReq   = Manager::Get()->GetConfigManager(_T("todo_list"))->ReadBool(_T("last_date_req"), false);

    // load users
    wxChoice* cmb = XRCCTRL(*this, "chcUser", wxChoice);
    cmb->Clear();
    for (unsigned int i = 0; i < m_Users.GetCount(); ++i)
        cmb->Append(m_Users[i]);

    if (m_Users.GetCount() == 0)
        cmb->Append(wxGetUserId());

    if (!lastUser.IsEmpty())
    {
        int sel = cmb->FindString(lastUser, true);
        if (sel != -1)
            cmb->SetSelection(sel);
    }
    else
        cmb->SetSelection(0);

    // load types
    cmb = XRCCTRL(*this, "chcType", wxChoice);
    cmb->Clear();
    for (unsigned int i = 0; i < m_Types.GetCount(); ++i)
        cmb->Append(m_Types[i]);

    if (m_Types.GetCount() == 0)
    {
        cmb->Append(_T("TODO"));
        cmb->Append(_T("@todo"));
        cmb->Append(_T("\\todo"));

        cmb->Append(_T("FIXME"));
        cmb->Append(_T("@fixme"));
        cmb->Append(_T("\\fixme"));

        cmb->Append(_T("NOTE"));
        cmb->Append(_T("@note"));
        cmb->Append(_T("\\note"));
    }

    if (!lastType.IsEmpty())
    {
        int sel = cmb->FindString(lastType, true);
        if (sel != -1)
            cmb->SetSelection(sel);
    }
    else
        cmb->SetSelection(0);

    cmb = XRCCTRL(*this, "chcStyle", wxChoice);
    cmb->Clear();
    if (m_supportedTdcts[(int)tdctLine])
        cmb->Append(_T("Line comment"));
    if (m_supportedTdcts[(int)tdctStream])
        cmb->Append(_T("Stream comment"));
    if (m_supportedTdcts[(int)tdctDoxygenLine])
        cmb->Append(_T("Doxygen line comment"));
    if (m_supportedTdcts[(int)tdctDoxygenStream])
        cmb->Append(_T("Doxygen stream comment"));
    if (m_supportedTdcts[(int)tdctWarning])
        cmb->Append(_T("Compiler warning"));
    if (m_supportedTdcts[(int)tdctError])
            cmb->Append(_T("Compiler error"));

    if (!lastStyle.IsEmpty())
    {
        int sel = cmb->FindString(lastStyle, true);
        if (sel != -1)
            cmb->SetSelection(sel);
    }
    else
        cmb->SetSelection(0);

    cmb = XRCCTRL(*this, "chcPosition", wxChoice);
    if (!lastPos.IsEmpty())
    {
        int sel = cmb->FindString(lastPos, true);
        if (sel != -1)
            cmb->SetSelection(sel);
    }

    XRCCTRL(*this, "ID_CHECKBOX1", wxCheckBox)->SetValue(lastDateReq);
}

AddTodoDlg::~AddTodoDlg()
{
    //dtor
}

wxString AddTodoDlg::GetText() const
{
    return XRCCTRL(*this, "txtText", wxTextCtrl)->GetValue();
}

wxString AddTodoDlg::GetUser() const
{
    return XRCCTRL(*this, "chcUser", wxChoice)->GetStringSelection();
}

int AddTodoDlg::GetPriority() const
{
    int prio = XRCCTRL(*this, "spnPriority", wxSpinCtrl)->GetValue();
    if (prio < 1)
        prio = 1;
    else if (prio > 9)
        prio = 9;
    return prio;
}

ToDoPosition AddTodoDlg::GetPosition() const
{
    return (ToDoPosition)(XRCCTRL(*this, "chcPosition", wxChoice)->GetSelection());
}

wxString AddTodoDlg::GetType() const
{
    return XRCCTRL(*this, "chcType", wxChoice)->GetStringSelection();
}

ToDoCommentType AddTodoDlg::GetCommentType() const
{
    wxChoice* cmb = XRCCTRL(*this, "chcStyle", wxChoice);

    int sel = cmb->GetSelection();
    for ( int i = 0; i < (int)tdctError ; i++)
    {
        if (!m_supportedTdcts[i] && sel >= i)
            sel++;
    }

    return (ToDoCommentType)(sel);
}

void AddTodoDlg::EndModal(int retVal)
{
    if (retVal == wxID_OK)
    {
        // "save" users
        wxChoice* cmb = XRCCTRL(*this, "chcUser", wxChoice);
        m_Users.Clear();
        if (cmb->FindString(cmb->GetStringSelection(), true) == wxNOT_FOUND)
            m_Users.Add(cmb->GetStringSelection());
        for (unsigned int i = 0; i < cmb->GetCount(); ++i)
            m_Users.Add(cmb->GetString(i));
        Manager::Get()->GetConfigManager(_T("todo_list"))->Write(_T("users"), m_Users);
        Manager::Get()->GetConfigManager(_T("todo_list"))->Write(_T("last_used_user"), cmb->GetStringSelection());

        // "save" types
        cmb = XRCCTRL(*this, "chcType", wxChoice);
        m_Types.Clear();
        if (cmb->FindString(cmb->GetStringSelection(), true) == wxNOT_FOUND)
            m_Types.Add(cmb->GetStringSelection());
        for (unsigned int i = 0; i < cmb->GetCount(); ++i)
            m_Types.Add(cmb->GetString(i));
        Manager::Get()->GetConfigManager(_T("todo_list"))->Write(_T("types"), m_Types);
        Manager::Get()->GetConfigManager(_T("todo_list"))->Write(_T("last_used_type"), cmb->GetStringSelection());

        cmb = XRCCTRL(*this, "chcStyle", wxChoice);
        Manager::Get()->GetConfigManager(_T("todo_list"))->Write(_T("last_used_style"), cmb->GetStringSelection());

        cmb = XRCCTRL(*this, "chcPosition", wxChoice);
        Manager::Get()->GetConfigManager(_T("todo_list"))->Write(_T("last_used_position"), cmb->GetStringSelection());

        wxCheckBox *checkAddDate = XRCCTRL(*this, "ID_CHECKBOX1", wxCheckBox);
        Manager::Get()->GetConfigManager(_T("todo_list"))->Write(_T("last_date_req"), checkAddDate->GetValue() );
    }

    wxScrollingDialog::EndModal(retVal);
}

void AddTodoDlg::OnAddUser(wxCommandEvent&)
{
    // ask for the new user to be added to the "choice" list
    wxTextEntryDialog dlg(this, _T("Enter the user you wish to add"), _T("Add user"), _T(""), wxOK|wxCANCEL);
    if(dlg.ShowModal() == wxID_OK)
    {
        wxString User = dlg.GetValue();
        if(!User.IsEmpty())
            XRCCTRL(*this, "chcUser", wxChoice)->Append(User);
    }
}

void AddTodoDlg::OnDelUser(wxCommandEvent&)
{
    wxChoice* cmb = XRCCTRL(*this, "chcUser", wxChoice);
    int sel = cmb->GetCurrentSelection();
    if (sel == wxNOT_FOUND)
        return;

    wxString msg; msg.Printf(_T("Are you sure you want to delete the user '%s'?"), cmb->GetString(sel).c_str());
    if (cbMessageBox(msg, _T("Confirmation"), wxICON_QUESTION | wxYES_NO, this) == wxID_NO)
       return;

    cmb->Delete(sel);
    if (cmb->GetCount()>0)
      cmb->SetSelection(0);
}


void AddTodoDlg::OnAddType(wxCommandEvent&)
{
    // ask for the new type to be added to the "choice" list
    wxTextEntryDialog dlg(this, _T("Enter the type you wish to add"), _T("Add type"), _T(""), wxOK|wxCANCEL);
    if(dlg.ShowModal() == wxID_OK)
    {
        wxString Type = dlg.GetValue();
        if(!Type.IsEmpty())
            XRCCTRL(*this, "chcType", wxChoice)->Append(Type);
    }
}

void AddTodoDlg::OnDelType(wxCommandEvent&)
{
    wxChoice* cmb = XRCCTRL(*this, "chcType", wxChoice);
    int sel = cmb->GetCurrentSelection();
    if (sel == wxNOT_FOUND)
        return;

    wxString msg; msg.Printf(_T("Are you sure you want to delete the type '%s'?"), cmb->GetString(sel).c_str());
    if (cbMessageBox(msg, _T("Confirmation"), wxICON_QUESTION | wxYES_NO, this) == wxID_NO)
       return;

    cmb->Delete(sel);
    if (cmb->GetCount()>0)
      cmb->SetSelection(0);
}
bool AddTodoDlg::DateRequested() const
{
    return XRCCTRL(*this, "ID_CHECKBOX1", wxCheckBox)->IsChecked();
}

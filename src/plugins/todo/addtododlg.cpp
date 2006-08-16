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
#include "addtododlg.h"

AddTodoDlg::AddTodoDlg(wxWindow* parent, wxArrayString& types)
    : m_Types(types)
{
	wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgAddToDo"));
	LoadUsers();

    // load types
    wxChoice* cmb = XRCCTRL(*this, "chcType", wxChoice);
    cmb->Clear();
    for (unsigned int i = 0; i < m_Types.GetCount(); ++i)
    {
        cmb->Append(m_Types[i]);
    }
    if (m_Types.Index(_T("TODO")) == wxNOT_FOUND)
        cmb->Append(_T("TODO"));
    if (m_Types.Index(_T("FIXME")) == wxNOT_FOUND)
        cmb->Append(_T("FIXME"));
    if (m_Types.Index(_T("NOTE")) == wxNOT_FOUND)
        cmb->Append(_T("NOTE"));

    wxString lastType = Manager::Get()->GetConfigManager(_T("todo_list"))->Read(_T("last_used_type"));
    wxString lastStyle = Manager::Get()->GetConfigManager(_T("todo_list"))->Read(_T("last_used_style"));
    wxString lastPos = Manager::Get()->GetConfigManager(_T("todo_list"))->Read(_T("last_used_position"));
    if (!lastType.IsEmpty())
    {
        int sel = cmb->FindString(lastType);
        if (sel != -1)
            cmb->SetSelection(sel);
    }
    else
        cmb->SetSelection(0);


    cmb = XRCCTRL(*this, "chcStyle", wxChoice);
    if (!lastStyle.IsEmpty())
    {
        int sel = cmb->FindString(lastStyle);
        if (sel != -1)
            cmb->SetSelection(sel);
    }

    cmb = XRCCTRL(*this, "chcPosition", wxChoice);
    if (!lastPos.IsEmpty())
    {
        int sel = cmb->FindString(lastPos);
        if (sel != -1)
            cmb->SetSelection(sel);
    }
}

AddTodoDlg::~AddTodoDlg()
{
	//dtor
}

void AddTodoDlg::LoadUsers() const
{
	wxChoice* cmb = XRCCTRL(*this, "chcUser", wxChoice);

	wxArrayString users;
	Manager::Get()->GetConfigManager(_T("todo_list"))->Read(_T("users"), &users);

	cmb->Clear();
    cmb->Append(users);

	if (cmb->GetCount() == 0)
		cmb->Append(wxGetUserId());
	cmb->SetSelection(0);
}

void AddTodoDlg::SaveUsers() const
{
	wxChoice* cmb = XRCCTRL(*this, "chcUser", wxChoice);
	wxArrayString users;

	for (int i = 0; i < cmb->GetCount(); ++i)
	{
		users.Add(cmb->GetString(i));
	}
	Manager::Get()->GetConfigManager(_T("todo_list"))->Write(_T("users"), users);
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
    return (ToDoCommentType)(XRCCTRL(*this, "chcStyle", wxChoice)->GetSelection());
}

void AddTodoDlg::EndModal(int retVal)
{
	if (retVal == wxID_OK)
	{
		SaveUsers();

        // "save" types
        wxChoice* cmb = XRCCTRL(*this, "chcType", wxChoice);
        m_Types.Clear();
        if (cmb->FindString(cmb->GetStringSelection()) == wxNOT_FOUND)
            m_Types.Add(cmb->GetStringSelection());
        for (int i = 0; i < cmb->GetCount(); ++i)
        {
            m_Types.Add(cmb->GetString(i));
        }

        Manager::Get()->GetConfigManager(_T("todo_list"))->Write(_T("last_used_type"), cmb->GetStringSelection());
        cmb = XRCCTRL(*this, "chcStyle", wxChoice);
        Manager::Get()->GetConfigManager(_T("todo_list"))->Write(_T("last_used_style"), cmb->GetStringSelection());
        cmb = XRCCTRL(*this, "chcPosition", wxChoice);
        Manager::Get()->GetConfigManager(_T("todo_list"))->Write(_T("last_used_position"), cmb->GetStringSelection());
	}

	wxDialog::EndModal(retVal);
}

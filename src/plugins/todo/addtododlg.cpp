#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/spinctrl.h>
#include <wx/utils.h>
#include <configmanager.h>
#include "addtododlg.h"

#define CONF_GROUP "/todo/users"

AddTodoDlg::AddTodoDlg(wxWindow* parent, wxArrayString& types)
    : m_Types(types)
{
	wxXmlResource::Get()->LoadDialog(this, parent, "dlgAddToDo");
	LoadUsers();

    // load types
    wxComboBox* cmb = XRCCTRL(*this, "cmbType", wxComboBox);
    cmb->Clear();
    if (m_Types.GetCount() == 0)
    {
        cmb->Append("TODO");
        cmb->Append("FIXME");
        cmb->Append("NOTE");
    }
    else
    {
        for (unsigned int i = 0; i < m_Types.GetCount(); ++i)
        {
            cmb->Append(m_Types[i]);
        }
    }
    
    wxString sels = ConfigManager::Get()->Read("/todo/last_used_type", "");
    if (!sels.IsEmpty())
    {
        int sel = cmb->FindString(sels);
        if (sel != -1)
            cmb->SetSelection(sel);
    }
    else
        cmb->SetSelection(0);
}

AddTodoDlg::~AddTodoDlg()
{
	//dtor
}

void AddTodoDlg::LoadUsers()
{
	wxComboBox* cmb = XRCCTRL(*this, "cmbUser", wxComboBox);
	cmb->Clear();
	long cookie;
	wxString entry;
	wxConfigBase* conf = ConfigManager::Get();
	wxString oldPath = conf->GetPath();
	conf->SetPath(CONF_GROUP);
	bool cont = conf->GetFirstEntry(entry, cookie);
	while (cont)
	{
		cmb->Append(entry);
		cont = conf->GetNextEntry(entry, cookie);
	}
	conf->SetPath(oldPath);
	
	if (cmb->GetCount() == 0)
		cmb->Append(wxGetUserId());
	cmb->SetSelection(0);
}

void AddTodoDlg::SaveUsers()
{
	wxComboBox* cmb = XRCCTRL(*this, "cmbUser", wxComboBox);
	wxConfigBase* conf = ConfigManager::Get();
	conf->DeleteGroup(CONF_GROUP);
	wxString oldPath = conf->GetPath();
	conf->SetPath(CONF_GROUP);
	if (cmb->FindString(cmb->GetValue()) == wxNOT_FOUND)
		conf->Write(cmb->GetValue(), wxEmptyString);
	for (int i = 0; i < cmb->GetCount(); ++i)
	{
		conf->Write(cmb->GetString(i), wxEmptyString);
	}
	conf->SetPath(oldPath);
}

wxString AddTodoDlg::GetText()
{
    return XRCCTRL(*this, "txtText", wxTextCtrl)->GetValue();
}

wxString AddTodoDlg::GetUser()
{
    return XRCCTRL(*this, "cmbUser", wxComboBox)->GetValue();
}

int AddTodoDlg::GetPriority()
{
    int prio = XRCCTRL(*this, "spnPriority", wxSpinCtrl)->GetValue();
    if (prio < 1)
        prio = 1;
    else if (prio > 9)
        prio = 9;
    return prio;
}

ToDoPosition AddTodoDlg::GetPosition()
{
    return (ToDoPosition)(XRCCTRL(*this, "cmbPosition", wxComboBox)->GetSelection());
}

wxString AddTodoDlg::GetType()
{
    return XRCCTRL(*this, "cmbType", wxComboBox)->GetValue();
}

ToDoCommentType AddTodoDlg::GetCommentType()
{
    return (ToDoCommentType)(XRCCTRL(*this, "cmbStyle", wxComboBox)->GetSelection());
}

void AddTodoDlg::EndModal(int retVal)
{
	if (retVal == wxID_OK)
	{
		SaveUsers();

        // "save" types
        wxComboBox* cmb = XRCCTRL(*this, "cmbType", wxComboBox);
        m_Types.Clear();
        if (cmb->FindString(cmb->GetValue()) == wxNOT_FOUND)
            m_Types.Add(cmb->GetValue());
        for (int i = 0; i < cmb->GetCount(); ++i)
        {
            m_Types.Add(cmb->GetString(i));
        }

        ConfigManager::Get()->Write("/todo/last_used_type", cmb->GetValue());
	}

	wxDialog::EndModal(retVal);
}

#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/spinctrl.h>
#include <wx/utils.h>
#include <configmanager.h>
#include "addtododlg.h"

#define CONF_GROUP "/todo/users"

AddTodoDlg::AddTodoDlg(wxWindow* parent)
{
	wxXmlResource::Get()->LoadDialog(this, parent, "dlgAddToDo");
	LoadUsers();
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
    return XRCCTRL(*this, "spnPriority", wxSpinCtrl)->GetValue();
}

ToDoType AddTodoDlg::GetType()
{
    return (ToDoType)(XRCCTRL(*this, "cmbType", wxComboBox)->GetSelection());
}

ToDoPosition AddTodoDlg::GetPosition()
{
    return (ToDoPosition)(XRCCTRL(*this, "cmbPosition", wxComboBox)->GetSelection());
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
	}

	wxDialog::EndModal(retVal);
}

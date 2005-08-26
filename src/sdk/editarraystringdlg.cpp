#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/button.h>
#include <wx/listbox.h>
#include <wx/textdlg.h>
#include <wx/msgdlg.h>
#include "editarraystringdlg.h"

BEGIN_EVENT_TABLE(EditArrayStringDlg, wxDialog)
	EVT_LISTBOX_DCLICK(XRCID("lstItems"), EditArrayStringDlg::OnEdit)
	EVT_BUTTON(XRCID("btnAdd"), EditArrayStringDlg::OnAdd)
	EVT_BUTTON(XRCID("btnEdit"), EditArrayStringDlg::OnEdit)
	EVT_BUTTON(XRCID("btnDelete"), EditArrayStringDlg::OnDelete)
	EVT_UPDATE_UI(-1, EditArrayStringDlg::OnUpdateUI)
END_EVENT_TABLE()

EditArrayStringDlg::EditArrayStringDlg(wxWindow* parent, wxArrayString& array)
	: m_Array(array)
{
	//ctor
	wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgEditArrayString"));

	wxListBox* list = XRCCTRL(*this, "lstItems", wxListBox);
	list->Clear();
	for (unsigned int i = 0; i < m_Array.GetCount(); ++i)
	{
		list->Append(m_Array[i]);
	}
}

EditArrayStringDlg::~EditArrayStringDlg()
{
	//dtor
}

void EditArrayStringDlg::EndModal(int retCode)
{
	if (retCode == wxID_OK)
	{
		wxListBox* list = XRCCTRL(*this, "lstItems", wxListBox);
		m_Array.Clear();
		for (int i = 0; i < list->GetCount(); ++i)
		{
			m_Array.Add(list->GetString(i));
		}
	}
	wxDialog::EndModal(retCode);
}

// events

void EditArrayStringDlg::OnAdd(wxCommandEvent& event)
{
	wxString w = wxGetTextFromUser(_("Add item"), _("Enter the new item:"));
	if (!w.IsEmpty())
		XRCCTRL(*this, "lstItems", wxListBox)->Append(w);
}

void EditArrayStringDlg::OnEdit(wxCommandEvent& event)
{
	wxListBox* list = XRCCTRL(*this, "lstItems", wxListBox);
	
	wxString w = list->GetStringSelection();
	w = wxGetTextFromUser(_("Edit item"), _("Edit the item:"), w);
	if (!w.IsEmpty())
		list->SetString(list->GetSelection(), w);
}

void EditArrayStringDlg::OnDelete(wxCommandEvent& event)
{
	if (wxMessageBox(_("Delete this item?"), _("Confirm"), wxYES_NO) == wxYES)
	{
		wxListBox* list = XRCCTRL(*this, "lstItems", wxListBox);
		list->Delete(list->GetSelection());
	}
}

void EditArrayStringDlg::OnUpdateUI(wxUpdateUIEvent& event)
{
	bool en = XRCCTRL(*this, "lstItems", wxListBox)->GetSelection() != -1;
	XRCCTRL(*this, "btnEdit", wxButton)->Enable(en);
	XRCCTRL(*this, "btnDelete", wxButton)->Enable(en);
}

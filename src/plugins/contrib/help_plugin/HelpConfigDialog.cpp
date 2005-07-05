#include "HelpConfigDialog.h"
#include <wx/xrc/xmlres.h>
#include <manager.h>
#include <configmanager.h>
#include <editormanager.h>
#include <messagemanager.h>
#include <projectmanager.h>

#include <wx/filedlg.h>
#include <wx/textdlg.h>
#include <wx/msgdlg.h>
#include <wx/listbox.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/button.h>

BEGIN_EVENT_TABLE(HelpConfigDialog, wxDialog)
    EVT_UPDATE_UI(-1, HelpConfigDialog::UpdateUI)
    EVT_BUTTON(XRCID("btnAdd"), HelpConfigDialog::Add)
    EVT_BUTTON(XRCID("btnRename"), HelpConfigDialog::Rename)
    EVT_BUTTON(XRCID("btnDelete"), HelpConfigDialog::Delete)
    EVT_BUTTON(XRCID("btnBrowse"), HelpConfigDialog::Browse)
    EVT_BUTTON(XRCID("btnOK"), HelpConfigDialog::Ok)
    EVT_LISTBOX(XRCID("lstHelp"), HelpConfigDialog::ListChange)
END_EVENT_TABLE()

HelpConfigDialog::HelpConfigDialog()
    : m_LastSel(0)
{
    wxXmlResource::Get()->LoadDialog(this, 0L, "HelpConfigDialog");
    LoadHelpFilesMap(m_Map);

    wxListBox* lst = XRCCTRL(*this, "lstHelp", wxListBox);
    lst->Clear();
	HelpFilesMap::iterator it;
	for (it = m_Map.begin(); it != m_Map.end(); ++it)
	{
        lst->Append(it->first);
	}
	if (m_Map.size() != 0)
	{
        lst->SetSelection(0);
        m_LastSel = 0;
        it = m_Map.begin();
        XRCCTRL(*this, "txtHelp", wxTextCtrl)->SetValue(it->second);
        XRCCTRL(*this, "chkDefault", wxCheckBox)->SetValue(g_DefaultHelpIndex == 0);
	}
}

HelpConfigDialog::~HelpConfigDialog()
{
    //dtor
}

void HelpConfigDialog::UpdateEntry(int index)
{
    if (index == -1)
        return;
    wxListBox* lst = XRCCTRL(*this, "lstHelp", wxListBox);
    m_Map[lst->GetString(index)] = XRCCTRL(*this, "txtHelp", wxTextCtrl)->GetValue();
    if (XRCCTRL(*this, "chkDefault", wxCheckBox)->GetValue())
        g_DefaultHelpIndex = index;
}

void HelpConfigDialog::ChooseFile()
{
    wxString filename = wxFileSelector("Choose a help file",NULL,NULL,NULL,"Help files (*.chm;*.hlp)|*.hlp;*.chm");
    if ( ! filename.empty() )
    {
        XRCCTRL(*this, "txtHelp", wxTextCtrl)->SetValue(filename.GetData());
    }
}

void HelpConfigDialog::ListChange(wxCommandEvent& event)
{
    wxListBox* lst = XRCCTRL(*this, "lstHelp", wxListBox);
    if (lst->GetSelection() != m_LastSel)
        UpdateEntry(m_LastSel);
    m_LastSel = lst->GetSelection();
    XRCCTRL(*this, "txtHelp", wxTextCtrl)->SetValue(m_Map[lst->GetString(lst->GetSelection())]);
    XRCCTRL(*this, "chkDefault", wxCheckBox)->SetValue(g_DefaultHelpIndex == lst->GetSelection());
}

void HelpConfigDialog::Browse(wxCommandEvent& event)
{
    ChooseFile();
}

void HelpConfigDialog::Add(wxCommandEvent& event)
{
    wxListBox* lst = XRCCTRL(*this, "lstHelp", wxListBox);
    wxString orig = lst->GetString(lst->GetSelection());
    UpdateEntry(lst->GetSelection());
    wxString text = wxGetTextFromUser(_("Please enter new help file title:"), _("Add title"));
    if (!text.IsEmpty())
    {
        HelpFilesMap::iterator it = m_Map.find(text);
        if (it != m_Map.end())
        {
            wxMessageBox(_("This title is already in use"), _("Warning"), wxICON_WARNING);
            return;
        }
        lst->Append(text);
        lst->SetSelection(lst->GetCount() - 1);
        XRCCTRL(*this, "txtHelp", wxTextCtrl)->SetValue(_(""));
        ChooseFile();
        UpdateEntry(lst->GetCount() - 1);
    }
}

void HelpConfigDialog::Rename(wxCommandEvent& event)
{
    wxListBox* lst = XRCCTRL(*this, "lstHelp", wxListBox);
    wxString orig = lst->GetString(lst->GetSelection());
    wxString text = wxGetTextFromUser(_("Rename this help file title:"), _("Rename title"), orig);
    if (!text.IsEmpty())
    {
        HelpFilesMap::iterator it = m_Map.find(text);
        if (it != m_Map.end())
        {
            wxMessageBox(_("This title is already in use"), _("Warning"), wxICON_WARNING);
            return;
        }
        it = m_Map.find(orig);
        if (it != m_Map.end())
        {
            it->first = text;
            lst->SetString(lst->GetSelection(), text);
        }
    }
}

void HelpConfigDialog::Delete(wxCommandEvent& event)
{
    if (wxMessageBox(_("Are you sure you want to remove this help file?"), _("Remove"), wxICON_QUESTION | wxYES_NO) == wxNO)
        return;
    wxListBox* lst = XRCCTRL(*this, "lstHelp", wxListBox);
    wxString orig = lst->GetString(lst->GetSelection());
    HelpFilesMap::iterator it = m_Map.find(orig);
    if (it != m_Map.end())
    {
        lst->Delete(lst->GetSelection());
        m_Map.erase(it);
        if (lst->GetSelection() != -1)
            XRCCTRL(*this, "txtHelp", wxTextCtrl)->SetValue(m_Map[lst->GetString(lst->GetSelection())]);
        else
            XRCCTRL(*this, "txtHelp", wxTextCtrl)->SetValue(_(""));
    }
}

void HelpConfigDialog::UpdateUI(wxUpdateUIEvent& event)
{
    int sel = XRCCTRL(*this, "lstHelp", wxListBox)->GetSelection();
    XRCCTRL(*this, "btnRename", wxButton)->Enable(sel != -1);
    XRCCTRL(*this, "btnDelete", wxButton)->Enable(sel != -1);
}

void HelpConfigDialog::Ok(wxCommandEvent& event)
{
    wxListBox* lst = XRCCTRL(*this, "lstHelp", wxListBox);
    UpdateEntry(lst->GetSelection());
    SaveHelpFilesMap(m_Map);
    wxDialog::EndModal(wxID_OK);
}

void HelpConfigDialog::Cancel(wxCommandEvent& event)
{
}

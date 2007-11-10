#include "HelpConfigDialog.h"
#include <wx/xrc/xmlres.h>
#include <globals.h> // cbMessageBox
#include <manager.h>
#include <editormanager.h>
#include <logmanager.h>
#include <projectmanager.h>

#include <wx/filedlg.h>
#include <wx/textdlg.h>
#include <wx/msgdlg.h>
#include <wx/listbox.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/button.h>

#include <algorithm>

#include "help_plugin.h"

using std::find;
using std::make_pair;
using std::swap;

BEGIN_EVENT_TABLE(HelpConfigDialog, wxPanel)
  EVT_UPDATE_UI( -1, HelpConfigDialog::UpdateUI)
  EVT_BUTTON(XRCID("btnAdd"), HelpConfigDialog::Add)
  EVT_BUTTON(XRCID("btnRename"), HelpConfigDialog::Rename)
  EVT_BUTTON(XRCID("btnDelete"), HelpConfigDialog::Delete)
  EVT_BUTTON(XRCID("btnBrowse"), HelpConfigDialog::Browse)
  EVT_BUTTON(XRCID("btnUp"), HelpConfigDialog::OnUp)
  EVT_BUTTON(XRCID("btnDown"), HelpConfigDialog::OnDown)
  EVT_LISTBOX(XRCID("lstHelp"), HelpConfigDialog::ListChange)
  EVT_CHECKBOX(XRCID("chkDefault"), HelpConfigDialog::OnCheckbox)
  EVT_CHECKBOX(XRCID("chkExecute"), HelpConfigDialog::OnCheckboxExecute)
END_EVENT_TABLE()


HelpConfigDialog::HelpConfigDialog(wxWindow* parent, HelpPlugin* plugin)
: m_LastSel(0),
m_pPlugin(plugin)
{
  wxXmlResource::Get()->LoadPanel(this, parent, _T("HelpConfigDialog"));
  HelpCommon::LoadHelpFilesVector(m_Vector);

  wxListBox *lst = XRCCTRL(*this, "lstHelp", wxListBox);
  lst->Clear();
  HelpCommon::HelpFilesVector::iterator it;

  for (it = m_Vector.begin(); it != m_Vector.end(); ++it)
  {
    lst->Append(it->first);
  }

  if (m_Vector.size() != 0)
  {
    lst->SetSelection(0);
    m_LastSel = 0;
    XRCCTRL(*this, "txtHelp", wxTextCtrl)->SetValue(m_Vector[0].second.name);
    XRCCTRL(*this, "chkExecute", wxCheckBox)->SetValue(m_Vector[0].second.isExecutable);
    XRCCTRL(*this, "chkDefault", wxCheckBox)->SetValue(HelpCommon::getDefaultHelpIndex() == 0);
  }
}

HelpConfigDialog::~HelpConfigDialog()
{
  //dtor
}

void HelpConfigDialog::UpdateEntry(int index)
{
  if (index == -1)
  {
    return;
  }

  wxListBox *lst = XRCCTRL(*this, "lstHelp", wxListBox);

  if (index < static_cast<int>(m_Vector.size()))
  {
  	m_Vector[index].second.name = XRCCTRL(*this, "txtHelp", wxTextCtrl)->GetValue();
  	m_Vector[index].second.isExecutable = XRCCTRL(*this, "chkExecute", wxCheckBox)->IsChecked();
  }
  else
  {
    HelpCommon::HelpFileAttrib hfa;
    hfa.name = XRCCTRL(*this, "txtHelp", wxTextCtrl)->GetValue();
    hfa.isExecutable = XRCCTRL(*this, "chkExecute", wxCheckBox)->IsChecked();
  	m_Vector.push_back(make_pair(lst->GetString(index), hfa));
  }
}

void HelpConfigDialog::ChooseFile()
{
  wxString filename = wxFileSelector
  (
    _T("Choose a help file"),
    wxEmptyString,
    wxEmptyString,
    wxEmptyString,
#ifdef __WXMSW__
    _T(
      "Windows help files (*.chm;*.hlp)|*.hlp;*.chm|"
      "All files (*.*)|*.*"
    )
#else
    _T("All files (*.*)|*.*")
#endif
  );

  if (!filename.IsEmpty())
  {
    XRCCTRL(*this, "txtHelp", wxTextCtrl)->SetValue(filename);
  }
}

void HelpConfigDialog::ListChange(wxCommandEvent& event)
{
  wxListBox *lst = XRCCTRL(*this, "lstHelp", wxListBox);

  if (lst->GetSelection() != m_LastSel)
  {
    UpdateEntry(m_LastSel);
  }

  if ((m_LastSel = lst->GetSelection()) != -1)
  {
    XRCCTRL(*this, "txtHelp", wxTextCtrl)->SetValue(m_Vector[lst->GetSelection()].second.name);
    XRCCTRL(*this, "chkExecute", wxCheckBox)->SetValue(m_Vector[lst->GetSelection()].second.isExecutable);
    XRCCTRL(*this, "chkDefault", wxCheckBox)->SetValue(HelpCommon::getDefaultHelpIndex() == lst->GetSelection());
  }
  else
  {
  	XRCCTRL(*this, "chkDefault", wxCheckBox)->SetValue(false);
  	XRCCTRL(*this, "chkExecute", wxCheckBox)->SetValue(false);
  }
}

void HelpConfigDialog::Browse(wxCommandEvent &event)
{
  ChooseFile();
}

void HelpConfigDialog::Add(wxCommandEvent &event)
{
  wxListBox *lst = XRCCTRL(*this, "lstHelp", wxListBox);
  UpdateEntry(lst->GetSelection());
  wxString text = wxGetTextFromUser(_("Please enter new help file title:"), _("Add title"));

  if (!text.IsEmpty())
  {
    HelpCommon::HelpFilesVector::iterator it = find(m_Vector.begin(), m_Vector.end(), text);

    if (it != m_Vector.end())
    {
      cbMessageBox(_("This title is already in use"), _("Warning"), wxICON_WARNING);
      return;
    }

    if (text.Find(_T('/')) != -1 || text.Find(_T('\\')) != -1)
    {
      cbMessageBox(_("Slashes and backslashes cannot be used to name a help file"), _("Warning"), wxICON_WARNING);
      return;
    }

    lst->Append(text);
    lst->SetSelection(lst->GetCount() - 1);
    XRCCTRL(*this, "chkDefault", wxCheckBox)->SetValue(false);
    XRCCTRL(*this, "chkExecute", wxCheckBox)->SetValue(false);
    XRCCTRL(*this, "txtHelp", wxTextCtrl)->SetValue(_T(""));

    if (cbMessageBox(_("Would you like to browse for the help file?\n(Check \"Help->Plugins->Help plugin\" for a reason you would like to choose No)"), _("Browse"), wxICON_QUESTION | wxYES_NO) == wxID_YES)
    {
      ChooseFile();
    }

    UpdateEntry(lst->GetSelection());
    m_LastSel = lst->GetSelection();
  }
}

void HelpConfigDialog::Rename(wxCommandEvent &event)
{
  wxListBox *lst = XRCCTRL(*this, "lstHelp", wxListBox);
  wxString orig = lst->GetString(lst->GetSelection());
  wxString text = wxGetTextFromUser(_("Rename this help file title:"), _("Rename title"), orig);

  if (!text.IsEmpty())
  {
    HelpCommon::HelpFilesVector::iterator it = find(m_Vector.begin(), m_Vector.end(), text);

    if (it != m_Vector.end())
    {
      cbMessageBox(_("This title is already in use."), _("Warning"), wxICON_WARNING);
      return;
    }

    if (text.Find(_T('/')) != -1 || text.Find(_T('\\')) != -1)
    {
      cbMessageBox(_("Slashes and backslashes cannot be used to name a help file."), _("Warning"), wxICON_WARNING);
      return;
    }

    m_Vector[lst->GetSelection()].first = text;
    lst->SetString(lst->GetSelection(), text);
  }
}

void HelpConfigDialog::Delete(wxCommandEvent &event)
{
  if (cbMessageBox(_("Are you sure you want to remove this help file?"), _("Remove"), wxICON_QUESTION | wxYES_NO) == wxID_NO)
  {
    return;
  }

  wxListBox *lst = XRCCTRL(*this, "lstHelp", wxListBox);

  if (HelpCommon::getDefaultHelpIndex() >= lst->GetSelection())
  {
  	HelpCommon::setDefaultHelpIndex(HelpCommon::getDefaultHelpIndex() - 1);
  }

  m_Vector.erase(m_Vector.begin() + lst->GetSelection());
  lst->Delete(lst->GetSelection());

  if (lst->GetSelection() != -1)
  {
    XRCCTRL(*this, "txtHelp", wxTextCtrl)->SetValue(m_Vector[lst->GetSelection()].first);
    XRCCTRL(*this, "chkExecute", wxCheckBox)->SetValue(m_Vector[lst->GetSelection()].second.isExecutable);
  }
  else
  {
    XRCCTRL(*this, "txtHelp", wxTextCtrl)->SetValue(_T(""));
    XRCCTRL(*this, "chkExecute", wxCheckBox)->SetValue(false);
    XRCCTRL(*this, "chkDefault", wxCheckBox)->SetValue(false);
  }

  m_LastSel = lst->GetSelection();
}

void HelpConfigDialog::OnUp(wxCommandEvent &event)
{
	wxListBox *lst = XRCCTRL(*this, "lstHelp", wxListBox);
	int helpIndex = HelpCommon::getDefaultHelpIndex();
	int current = lst->GetSelection();

	if (helpIndex == current)
	{
		helpIndex = current - 1;
	}
	else if (helpIndex == current - 1)
	{
		helpIndex = current;
	}

	wxString temp(lst->GetString(current));
  lst->SetString(current, lst->GetString(current - 1));
	lst->SetSelection(--current);
	lst->SetString(current, temp);
	HelpCommon::setDefaultHelpIndex(helpIndex);
	swap(m_Vector[current], m_Vector[current + 1]);
	m_LastSel = current;
}

void HelpConfigDialog::OnDown(wxCommandEvent &event)
{
	wxListBox *lst = XRCCTRL(*this, "lstHelp", wxListBox);
	int helpIndex = HelpCommon::getDefaultHelpIndex();
	int current = lst->GetSelection();

	if (helpIndex == current)
	{
		helpIndex = current + 1;
	}
	else if (helpIndex == current + 1)
	{
		helpIndex = current;
	}

	wxString temp(lst->GetString(current));
  lst->SetString(current, lst->GetString(current + 1));
	lst->SetSelection(++current);
	lst->SetString(current, temp);
  HelpCommon::setDefaultHelpIndex(helpIndex);
	swap(m_Vector[current], m_Vector[current - 1]);
	m_LastSel = current;
}

void HelpConfigDialog::OnCheckbox(wxCommandEvent &event)
{
	if (event.IsChecked())
	{
		HelpCommon::setDefaultHelpIndex(XRCCTRL(*this, "lstHelp", wxListBox)->GetSelection());
	}
	else
	{
		HelpCommon::setDefaultHelpIndex(-1);
	}
}

void HelpConfigDialog::OnCheckboxExecute(wxCommandEvent &event)
{
  int current = XRCCTRL(*this, "lstHelp", wxListBox)->GetSelection();

  if (event.IsChecked())
  {
    m_Vector[current].second.isExecutable = true;
  }
  else
  {
    m_Vector[current].second.isExecutable = false;
  }
}

void HelpConfigDialog::UpdateUI(wxUpdateUIEvent &event)
{
  int sel = XRCCTRL(*this, "lstHelp", wxListBox)->GetSelection();
  int count = XRCCTRL(*this, "lstHelp", wxListBox)->GetCount();
  XRCCTRL(*this, "btnRename", wxButton)->Enable(sel != -1);
  XRCCTRL(*this, "btnDelete", wxButton)->Enable(sel != -1);
  XRCCTRL(*this, "btnBrowse", wxButton)->Enable(sel != -1);
  XRCCTRL(*this, "txtHelp", wxTextCtrl)->Enable(sel != -1);
  XRCCTRL(*this, "chkDefault", wxCheckBox)->Enable(sel != -1);
  XRCCTRL(*this, "chkExecute", wxCheckBox)->Enable(sel != -1);

  if (sel == -1 || count == 1)
  {
  	XRCCTRL(*this, "btnUp", wxButton)->Disable();
  	XRCCTRL(*this, "btnDown", wxButton)->Disable();
  }
  else if (sel == 0)
  {
  	XRCCTRL(*this, "btnUp", wxButton)->Disable();
  	XRCCTRL(*this, "btnDown", wxButton)->Enable();
  }
  else if (sel == count - 1)
  {
  	XRCCTRL(*this, "btnUp", wxButton)->Enable();
  	XRCCTRL(*this, "btnDown", wxButton)->Disable();
  }
  else
  {
  	XRCCTRL(*this, "btnUp", wxButton)->Enable();
  	XRCCTRL(*this, "btnDown", wxButton)->Enable();
  }
}

void HelpConfigDialog::OnApply()
{
  wxListBox *lst = XRCCTRL(*this, "lstHelp", wxListBox);
  UpdateEntry(lst->GetSelection());
  HelpCommon::SaveHelpFilesVector(m_Vector);
  m_pPlugin->Reload();
}

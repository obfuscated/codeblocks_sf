/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include "globals.h"
    #include "configmanager.h"
    #include "manager.h"
    #include "projectmanager.h"
    #include "cbproject.h"
    #include <wx/xrc/xmlres.h>
    #include <wx/intl.h>
    #include <wx/combobox.h>
    #include <wx/checkbox.h>
    #include <wx/button.h>
    #include <wx/radiobox.h>
    #include <wx/textctrl.h>
    #include <wx/notebook.h>
#endif

#include "finddlg.h"

#define CONF_GROUP _T("/find_options")

// flag to know when to update the search-in-files custom path
cbProject* g_LastUsedProject = 0;

BEGIN_EVENT_TABLE(FindDlg, wxDialog)
    EVT_NOTEBOOK_PAGE_CHANGED(XRCID("nbFind"), FindDlg::OnFindChange)
    EVT_CHECKBOX(XRCID("chkRegEx1"),    FindDlg::OnRegEx)
    EVT_BUTTON(XRCID("btnBrowsePath"),  FindDlg::OnBrowsePath)
    EVT_RADIOBOX(XRCID("rbScope2"),     FindDlg::OnRadioBox)
    EVT_ACTIVATE(                       FindDlg::OnActivate)
END_EVENT_TABLE()

FindDlg::FindDlg(wxWindow* parent, const wxString& initial, bool hasSelection, bool findInFilesOnly, bool findInFilesActive)
	: FindReplaceBase(parent, initial, hasSelection),
	m_Complete(!findInFilesOnly)
{
	wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgFind"));
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));

	// load last searches
	wxArrayString previous = GetArrayFromString(cfg->Read(CONF_GROUP _T("/last"), wxEmptyString), DEFAULT_ARRAY_SEP, false);
	for (unsigned int i = 0; i < previous.GetCount(); ++i)
	{
		if (!previous[i].IsEmpty())
		{
			XRCCTRL(*this, "cmbFind1", wxComboBox)->Append(previous[i]);
			XRCCTRL(*this, "cmbFind2", wxComboBox)->Append(previous[i]);
        }
	}

	// find options
	XRCCTRL(*this, "cmbFind1", wxComboBox)->SetValue(initial);
	XRCCTRL(*this, "chkWholeWord1", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/match_word1"), false));
	XRCCTRL(*this, "chkStartWord1", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/start_word1"), false));
	XRCCTRL(*this, "chkMatchCase1", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/match_case1"), false));
	XRCCTRL(*this, "chkRegEx1", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/regex1"), false));
	XRCCTRL(*this, "rbDirection", wxRadioBox)->SetSelection(cfg->ReadInt(CONF_GROUP _T("/direction"), 1));
	XRCCTRL(*this, "rbDirection", wxRadioBox)->Enable(!XRCCTRL(*this, "chkRegEx1", wxCheckBox)->GetValue()); // if regex, only forward searches
	XRCCTRL(*this, "rbOrigin", wxRadioBox)->SetSelection(cfg->ReadInt(CONF_GROUP _T("/origin"), 0));
	XRCCTRL(*this, "rbScope1", wxRadioBox)->SetSelection(hasSelection);
	XRCCTRL(*this, "rbScope1", wxRadioBox)->Enable(hasSelection);

	// find in files options
	XRCCTRL(*this, "cmbFind2", wxComboBox)->SetValue(initial);
	XRCCTRL(*this, "chkWholeWord2", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/match_word2"), false));
	XRCCTRL(*this, "chkStartWord2", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/start_word2"), false));
	XRCCTRL(*this, "chkMatchCase2", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/match_case2"), false));
	XRCCTRL(*this, "chkRegEx2", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/regex2"), false));
	XRCCTRL(*this, "chkDelOldSearchRes2", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/delete_old_searches2"), true));
	XRCCTRL(*this, "rbScope2", wxRadioBox)->SetSelection(cfg->ReadInt(CONF_GROUP _T("/scope2"), 0));
    UpdateUI();

	// find in files search path options
    cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
    if (prj && g_LastUsedProject != prj)
    {
        XRCCTRL(*this, "txtSearchPath", wxTextCtrl)->SetValue(prj->GetBasePath());
        g_LastUsedProject = prj;
    }
    else
        XRCCTRL(*this, "txtSearchPath", wxTextCtrl)->SetValue(cfg->Read(CONF_GROUP _T("/search_path")));

    XRCCTRL(*this, "txtSearchMask", wxTextCtrl)->SetValue(cfg->Read(CONF_GROUP _T("/search_mask")));
    XRCCTRL(*this, "chkSearchRecursively", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/search_recursive"), false));
    XRCCTRL(*this, "chkSearchHidden", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/search_hidden"), false));

	if (!m_Complete)
        XRCCTRL(*this, "nbFind", wxNotebook)->DeletePage(0); // no active editor, so only find-in-files
	else if (findInFilesActive)
	{
		XRCCTRL(*this, "nbFind", wxNotebook)->SetSelection(1); // Search->Find in Files was selected
		XRCCTRL(*this, "cmbFind2", wxComboBox)->SetFocus();
	}
}

FindDlg::~FindDlg()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));

    cfg->Write(CONF_GROUP _T("/search_path"), XRCCTRL(*this, "txtSearchPath", wxTextCtrl)->GetValue());
    cfg->Write(CONF_GROUP _T("/search_mask"), XRCCTRL(*this, "txtSearchMask", wxTextCtrl)->GetValue());
    cfg->Write(CONF_GROUP _T("/search_recursive"), XRCCTRL(*this, "chkSearchRecursively", wxCheckBox)->GetValue());
    cfg->Write(CONF_GROUP _T("/search_hidden"), XRCCTRL(*this, "chkSearchHidden", wxCheckBox)->GetValue());

	// save last searches (up to 10)
    wxComboBox* combo;
	if (IsFindInFiles())
        combo = XRCCTRL(*this, "cmbFind2", wxComboBox);
	else
		combo = XRCCTRL(*this, "cmbFind1", wxComboBox);
    wxArrayString previous;
    for (int i = 0; (i < combo->GetCount()) && (i < 10); ++i)
    {
        if (!combo->GetString(i).IsEmpty())
            previous.Add(combo->GetString(i));
    }
    wxString find = combo->GetValue();
    int prev_pos = combo->FindString(find);
    if (prev_pos > 0)
    {
        // if it's already in the list at a position other than the top, we'll bump it to the
        // top by deleting the duplicate item before we add the current search item to the top
        combo->Delete(prev_pos);
    }
    if (prev_pos != 0)
        previous.Insert(find, 0);
    wxString last = GetStringFromArray(previous);
    cfg->Write(CONF_GROUP _T("/last"), last);

	if (m_Complete)
	{
        // find options
        cfg->Write(CONF_GROUP _T("/match_word1"), XRCCTRL(*this, "chkWholeWord1", wxCheckBox)->GetValue());
        cfg->Write(CONF_GROUP _T("/start_word1"), XRCCTRL(*this, "chkStartWord1", wxCheckBox)->GetValue());
        cfg->Write(CONF_GROUP _T("/match_case1"), XRCCTRL(*this, "chkMatchCase1", wxCheckBox)->GetValue());
        cfg->Write(CONF_GROUP _T("/regex1"), XRCCTRL(*this, "chkRegEx1", wxCheckBox)->GetValue());
        cfg->Write(CONF_GROUP _T("/direction"), XRCCTRL(*this, "rbDirection", wxRadioBox)->GetSelection());
        cfg->Write(CONF_GROUP _T("/origin"), XRCCTRL(*this, "rbOrigin", wxRadioBox)->GetSelection());
	}

	// find in files options
	cfg->Write(CONF_GROUP _T("/match_word2"), XRCCTRL(*this, "chkWholeWord2", wxCheckBox)->GetValue());
	cfg->Write(CONF_GROUP _T("/start_word2"), XRCCTRL(*this, "chkStartWord2", wxCheckBox)->GetValue());
	cfg->Write(CONF_GROUP _T("/match_case2"), XRCCTRL(*this, "chkMatchCase2", wxCheckBox)->GetValue());
	cfg->Write(CONF_GROUP _T("/regex2"), XRCCTRL(*this, "chkRegEx2", wxCheckBox)->GetValue());
	cfg->Write(CONF_GROUP _T("/delete_old_searches2"), XRCCTRL(*this, "chkDelOldSearchRes2", wxCheckBox)->GetValue());
	cfg->Write(CONF_GROUP _T("/scope2"), XRCCTRL(*this, "rbScope2", wxRadioBox)->GetSelection());
}

wxString FindDlg::GetFindString() const
{
	if (IsFindInFiles())
        return XRCCTRL(*this, "cmbFind2", wxComboBox)->GetValue();
    else
        return XRCCTRL(*this, "cmbFind1", wxComboBox)->GetValue();
}

bool FindDlg::IsFindInFiles() const
{
	return !m_Complete || XRCCTRL(*this, "nbFind", wxNotebook)->GetSelection() == 1;
}

bool FindDlg::GetDeleteOldSearches() const
{
	if (IsFindInFiles())
        return XRCCTRL(*this, "chkDelOldSearchRes2", wxCheckBox)->GetValue();
    else
        return true;  // checkbox doesn't exist in Find dialog
}

bool FindDlg::GetMatchWord() const
{
	if (IsFindInFiles())
		return XRCCTRL(*this, "chkWholeWord2", wxCheckBox)->GetValue();
	else
		return XRCCTRL(*this, "chkWholeWord1", wxCheckBox)->GetValue();
}

bool FindDlg::GetStartWord() const
{
	if (IsFindInFiles())
		return XRCCTRL(*this, "chkStartWord2", wxCheckBox)->GetValue();
	else
		return XRCCTRL(*this, "chkStartWord1", wxCheckBox)->GetValue();
}

bool FindDlg::GetMatchCase() const
{
	if (IsFindInFiles())
		return XRCCTRL(*this, "chkMatchCase2", wxCheckBox)->GetValue();
	else
		return XRCCTRL(*this, "chkMatchCase1", wxCheckBox)->GetValue();
}

bool FindDlg::GetRegEx() const
{
	if (IsFindInFiles())
		return XRCCTRL(*this, "chkRegEx2", wxCheckBox)->GetValue();
	else
		return XRCCTRL(*this, "chkRegEx1", wxCheckBox)->GetValue();
}

int FindDlg::GetDirection() const
{
	if (IsFindInFiles())
        return 1;
    else
        return XRCCTRL(*this, "rbDirection", wxRadioBox)->GetSelection();
}

int FindDlg::GetOrigin() const
{
	if (IsFindInFiles())
        return 1;
    else
        return XRCCTRL(*this, "rbOrigin", wxRadioBox)->GetSelection();
}

int FindDlg::GetScope() const
{
	if (IsFindInFiles())
		return XRCCTRL(*this, "rbScope2", wxRadioBox)->GetSelection();
	else
		return XRCCTRL(*this, "rbScope1", wxRadioBox)->GetSelection();
}

bool FindDlg::GetRecursive() const
{
    return XRCCTRL(*this, "chkSearchRecursively", wxCheckBox)->IsChecked();
}

bool FindDlg::GetHidden() const
{
    return XRCCTRL(*this, "chkSearchHidden", wxCheckBox)->IsChecked();
}

wxString FindDlg::GetSearchPath() const
{
    return XRCCTRL(*this, "txtSearchPath", wxTextCtrl)->GetValue();
}

wxString FindDlg::GetSearchMask() const
{
    return XRCCTRL(*this, "txtSearchMask", wxTextCtrl)->GetValue();
}

void FindDlg::UpdateUI()
{
    bool on = XRCCTRL(*this, "rbScope2", wxRadioBox)->GetSelection() == 2; // find in search path
    XRCCTRL(*this, "txtSearchPath", wxTextCtrl)->Enable(on);
    XRCCTRL(*this, "txtSearchMask", wxTextCtrl)->Enable(on);
    XRCCTRL(*this, "btnBrowsePath", wxButton)->Enable(on);
}

// events

void FindDlg::OnFindChange(wxNotebookEvent& event)
{
    wxComboBox* cmbFind1 = XRCCTRL(*this, "cmbFind1", wxComboBox);
    wxComboBox* cmbFind2 = XRCCTRL(*this, "cmbFind2", wxComboBox);

    if (cmbFind1 && cmbFind2)
    {
        if (XRCCTRL(*this, "nbFind", wxNotebook)->GetSelection() == 1)
            cmbFind2->SetValue(cmbFind1->GetValue());
        else
            cmbFind1->SetValue(cmbFind2->GetValue());
    }
    event.Skip();
}

void FindDlg::OnRegEx(wxCommandEvent& event)
{
	if (m_Complete)
        XRCCTRL(*this, "rbDirection", wxRadioBox)->Enable(!XRCCTRL(*this, "chkRegEx1", wxCheckBox)->GetValue());
}

void FindDlg::OnBrowsePath(wxCommandEvent& event)
{
    wxString txtSearchPath = XRCCTRL(*this, "txtSearchPath", wxTextCtrl)->GetValue();
    wxString dir = ChooseDirectory(0, _("Select search path"), txtSearchPath);
    if (!dir.IsEmpty())
        XRCCTRL(*this, "txtSearchPath", wxTextCtrl)->SetValue(dir);
}

void FindDlg::OnRadioBox(wxCommandEvent& event)
{
    UpdateUI();
    event.Skip();
}

void FindDlg::OnActivate(wxActivateEvent& event)
{
    wxComboBox* cbp = 0;
    if (IsFindInFiles())
        cbp = XRCCTRL(*this, "cmbFind2", wxComboBox);
    else
        cbp = XRCCTRL(*this, "cmbFind1", wxComboBox);

    if (cbp != 0) cbp->SetFocus();
    event.Skip();
}


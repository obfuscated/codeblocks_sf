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

    #include <wx/xrc/xmlres.h>
    #include <wx/intl.h>
    #include <wx/combobox.h>
    #include <wx/checkbox.h>
    #include <wx/button.h>
#endif

#include "replacedlg.h"

#include <wx/radiobox.h>

#define CONF_GROUP _T("/replace_options")

BEGIN_EVENT_TABLE(ReplaceDlg, wxDialog)
    EVT_NOTEBOOK_PAGE_CHANGED(XRCID("nbReplace"), ReplaceDlg::OnFindChange)
	EVT_CHECKBOX(XRCID("chkRegEx1"), 	ReplaceDlg::OnRegEx)
	EVT_ACTIVATE(                       ReplaceDlg::OnActivate)
END_EVENT_TABLE()

ReplaceDlg::ReplaceDlg(wxWindow* parent, const wxString& initial, bool hasSelection,
    bool findInFilesOnly, bool replaceInFilesActive)
	: FindReplaceBase(parent, initial, hasSelection),
	m_Complete(!replaceInFilesActive)
{
	wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgReplace"));
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));

	// load last searches
	FillComboWithLastValues(XRCCTRL(*this, "cmbFind1", wxComboBox), CONF_GROUP _T("/last"));
	FillComboWithLastValues(XRCCTRL(*this, "cmbReplace1", wxComboBox), CONF_GROUP _T("/lastReplace"));
	FillComboWithLastValues(XRCCTRL(*this, "cmbFind2", wxComboBox), CONF_GROUP _T("/last"));
	FillComboWithLastValues(XRCCTRL(*this, "cmbReplace2", wxComboBox), CONF_GROUP _T("/lastReplace"));

	// replace options
	XRCCTRL(*this, "cmbFind1", wxComboBox)->SetValue(initial);
	XRCCTRL(*this, "chkWholeWord1", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/match_word"), false));
	XRCCTRL(*this, "chkStartWord1", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/start_word"), false));
	XRCCTRL(*this, "chkMatchCase1", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/match_case"), false));
	XRCCTRL(*this, "chkRegEx1", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/regex"), false));
	XRCCTRL(*this, "rbDirection", wxRadioBox)->SetSelection(cfg->ReadInt(CONF_GROUP _T("/direction"), 1));
	XRCCTRL(*this, "rbDirection", wxRadioBox)->Enable(!XRCCTRL(*this, "chkRegEx1", wxCheckBox)->GetValue()); // if regex, only forward searches
	XRCCTRL(*this, "rbOrigin", wxRadioBox)->SetSelection(cfg->ReadInt(CONF_GROUP _T("/origin"), 0));
	XRCCTRL(*this, "rbScope1", wxRadioBox)->SetSelection(hasSelection);
	XRCCTRL(*this, "rbScope1", wxRadioBox)->Enable(hasSelection);

	// replace in files options
	XRCCTRL(*this, "cmbFind2", wxComboBox)->SetValue(initial);
	XRCCTRL(*this, "chkWholeWord2", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/match_word2"), false));
	XRCCTRL(*this, "chkStartWord2", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/start_word2"), false));
	XRCCTRL(*this, "chkMatchCase2", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/match_case2"), false));
	XRCCTRL(*this, "chkRegEx2", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/regex2"), false));
	XRCCTRL(*this, "rbScope2", wxRadioBox)->SetSelection(cfg->ReadInt(CONF_GROUP _T("/scope2"), 0));

    if (findInFilesOnly)
    {
        XRCCTRL(*this, "nbReplace", wxNotebook)->DeletePage(0); // no active editor, so only replace-in-files
		XRCCTRL(*this, "cmbFind2", wxComboBox)->SetFocus();
    }
	else if (replaceInFilesActive)
	{
		XRCCTRL(*this, "nbReplace", wxNotebook)->SetSelection(1); // Search->Replace in Files was selected
		XRCCTRL(*this, "cmbFind2", wxComboBox)->SetFocus();
	}
}

ReplaceDlg::~ReplaceDlg()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));

	// save last searches (up to 10)
	if (IsFindInFiles())
	{
	    SaveComboValues(XRCCTRL(*this, "cmbFind2", wxComboBox), CONF_GROUP _T("/last"));
        SaveComboValues(XRCCTRL(*this, "cmbReplace2", wxComboBox), CONF_GROUP _T("/lastReplace"));
	}
	else
	{
	    SaveComboValues(XRCCTRL(*this, "cmbFind1", wxComboBox), CONF_GROUP _T("/last"));
        SaveComboValues(XRCCTRL(*this, "cmbReplace1", wxComboBox), CONF_GROUP _T("/lastReplace"));
	}

    if (m_Complete)
	{
        // find(replace) options
        cfg->Write(CONF_GROUP _T("/match_word"), XRCCTRL(*this, "chkWholeWord1", wxCheckBox)->GetValue());
        cfg->Write(CONF_GROUP _T("/start_word"), XRCCTRL(*this, "chkStartWord1", wxCheckBox)->GetValue());
        cfg->Write(CONF_GROUP _T("/match_case"), XRCCTRL(*this, "chkMatchCase1", wxCheckBox)->GetValue());
        cfg->Write(CONF_GROUP _T("/regex"), XRCCTRL(*this, "chkRegEx1", wxCheckBox)->GetValue());
        cfg->Write(CONF_GROUP _T("/direction"), XRCCTRL(*this, "rbDirection", wxRadioBox)->GetSelection());
        cfg->Write(CONF_GROUP _T("/origin"), XRCCTRL(*this, "rbOrigin", wxRadioBox)->GetSelection());
	}

	// find(replace) in files options
	cfg->Write(CONF_GROUP _T("/match_word2"), XRCCTRL(*this, "chkWholeWord2", wxCheckBox)->GetValue());
	cfg->Write(CONF_GROUP _T("/start_word2"), XRCCTRL(*this, "chkStartWord2", wxCheckBox)->GetValue());
	cfg->Write(CONF_GROUP _T("/match_case2"), XRCCTRL(*this, "chkMatchCase2", wxCheckBox)->GetValue());
	cfg->Write(CONF_GROUP _T("/regex2"), XRCCTRL(*this, "chkRegEx2", wxCheckBox)->GetValue());
	cfg->Write(CONF_GROUP _T("/scope2"), XRCCTRL(*this, "rbScope2", wxRadioBox)->GetSelection());
}

void ReplaceDlg::FillComboWithLastValues(wxComboBox* combo, const wxString& configKey)
{
	wxArrayString values;
	Manager::Get()->GetConfigManager(_T("editor"))->Read(configKey, &values);
	for (unsigned int i = 0; i < values.GetCount(); ++i)
	{
		if (!values[i].IsEmpty())
			combo->Append(values[i]);
	}
}

void ReplaceDlg::SaveComboValues(wxComboBox* combo, const wxString& configKey)
{
	wxArrayString values;
	for (int i = 0; (i < combo->GetCount()) && (i < 10); ++i)
	{
		if (!combo->GetString(i).IsEmpty())
			values.Add(combo->GetString(i));
	}
	wxString find = combo->GetValue();
	if (combo->FindString(find) == -1)
		values.Insert(find, 0);
	Manager::Get()->GetConfigManager(_T("editor"))->Write(configKey, values);
}

wxString ReplaceDlg::GetFindString() const
{
    if (IsFindInFiles())
        return XRCCTRL(*this, "cmbFind2", wxComboBox)->GetValue();
    else
        return XRCCTRL(*this, "cmbFind1", wxComboBox)->GetValue();
}

wxString ReplaceDlg::GetReplaceString() const
{
    if (IsFindInFiles())
        return XRCCTRL(*this, "cmbReplace2", wxComboBox)->GetValue();
    else
        return XRCCTRL(*this, "cmbReplace1", wxComboBox)->GetValue();
}

bool ReplaceDlg::IsFindInFiles() const
{
	return !m_Complete || XRCCTRL(*this, "nbReplace", wxNotebook)->GetSelection() == 1;
}

bool ReplaceDlg::GetDeleteOldSearches() const
{
    return false; // checkbox doesn't exist in both dialogs
}

bool ReplaceDlg::GetMatchWord() const
{
    if (IsFindInFiles())
		return XRCCTRL(*this, "chkWholeWord2", wxCheckBox)->GetValue();
	else
        return XRCCTRL(*this, "chkWholeWord1", wxCheckBox)->GetValue();
}

bool ReplaceDlg::GetStartWord() const
{
    if (IsFindInFiles())
		return XRCCTRL(*this, "chkStartWord2", wxCheckBox)->GetValue();
	else
        return XRCCTRL(*this, "chkStartWord1", wxCheckBox)->GetValue();
}

bool ReplaceDlg::GetMatchCase() const
{
    if (IsFindInFiles())
		return XRCCTRL(*this, "chkMatchCase2", wxCheckBox)->GetValue();
	else
        return XRCCTRL(*this, "chkMatchCase1", wxCheckBox)->GetValue();
}

bool ReplaceDlg::GetRegEx() const
{
    if (IsFindInFiles())
		return XRCCTRL(*this, "chkRegEx2", wxCheckBox)->GetValue();
	else
		return XRCCTRL(*this, "chkRegEx1", wxCheckBox)->GetValue();
}

bool ReplaceDlg::GetAutoWrapSearch() const
{
    if (IsFindInFiles())
		return XRCCTRL(*this, "chkAutoWrapSearch", wxCheckBox)->GetValue();
	else
		return XRCCTRL(*this, "chkAutoSrapSearch", wxCheckBox)->GetValue();
}

bool ReplaceDlg::GetFindUsesSelectedText() const
{
    if (IsFindInFiles())
		return XRCCTRL(*this, "chkFindUsesSelectedText", wxCheckBox)->GetValue();
	else
		return XRCCTRL(*this, "chkFindUsesSelectedText", wxCheckBox)->GetValue();
}

int ReplaceDlg::GetDirection() const
{
    if (IsFindInFiles())
        return 1;
    else
        return XRCCTRL(*this, "rbDirection", wxRadioBox)->GetSelection();
}

int ReplaceDlg::GetOrigin() const
{
    if (IsFindInFiles())
        return 1;
    else
        return XRCCTRL(*this, "rbOrigin", wxRadioBox)->GetSelection();
}

int ReplaceDlg::GetScope() const
{
    if (IsFindInFiles())
        return XRCCTRL(*this, "rbScope2", wxRadioBox)->GetSelection();
    else
        return XRCCTRL(*this, "rbScope1", wxRadioBox)->GetSelection();
}

// events

void ReplaceDlg::OnFindChange(wxNotebookEvent& event)
{
    wxComboBox* cmbFind1 = XRCCTRL(*this, "cmbFind1", wxComboBox);
    wxComboBox* cmbFind2 = XRCCTRL(*this, "cmbFind2", wxComboBox);

    if (cmbFind1 && cmbFind2)
    {
        if (XRCCTRL(*this, "nbReplace", wxNotebook)->GetSelection() == 1)
        {
            cmbFind2->SetValue(cmbFind1->GetValue());
            cmbFind2->SetFocus();
        }
        else
        {
            cmbFind1->SetValue(cmbFind2->GetValue());
            cmbFind1->SetFocus();
        }
    }
    event.Skip();
}

void ReplaceDlg::OnRegEx(wxCommandEvent& event)
{
    if (m_Complete)
        XRCCTRL(*this, "rbDirection", wxRadioBox)->Enable(!XRCCTRL(*this, "chkRegEx1", wxCheckBox)->GetValue());
}

void ReplaceDlg::OnActivate(wxActivateEvent& event)
{
    wxComboBox* cbp = 0;
    if (IsFindInFiles())
        cbp = XRCCTRL(*this, "cmbFind2", wxComboBox);
    else
        cbp = XRCCTRL(*this, "cmbFind1", wxComboBox);

    if (cbp != 0) cbp->SetFocus();
    event.Skip();
}

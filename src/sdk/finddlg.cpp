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
* $Id$
* $Date$
*/

#include "finddlg.h"
#include "globals.h"
#include "configmanager.h"
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include <wx/radiobox.h>
#include <wx/button.h>
#include <wx/textctrl.h>
#include <wx/notebook.h>

#define CONF_GROUP _T("/editor/find_options")

BEGIN_EVENT_TABLE(FindDlg, wxDialog)
	EVT_TEXT(XRCID("cmbFind1"),			FindDlg::OnFindChange)
	EVT_TEXT(XRCID("cmbFind2"),			FindDlg::OnFindChange)
	EVT_CHECKBOX(XRCID("chkRegEx1"), 	FindDlg::OnRegEx)
END_EVENT_TABLE()

FindDlg::FindDlg(wxWindow* parent, const wxString& initial, bool hasSelection, bool findInFilesOnly)
	: FindReplaceBase(parent, initial, hasSelection),
	m_Complete(!findInFilesOnly)
{
	wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgFind"));

	// load last searches
	wxArrayString previous = GetArrayFromString(ConfigManager::Get()->Read(CONF_GROUP _T("/last"), wxEmptyString));
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
	XRCCTRL(*this, "chkWholeWord1", wxCheckBox)->SetValue(ConfigManager::Get()->Read(CONF_GROUP _T("/match_word1"), 0L));
	XRCCTRL(*this, "chkStartWord1", wxCheckBox)->SetValue(ConfigManager::Get()->Read(CONF_GROUP _T("/start_word1"), 0L));
	XRCCTRL(*this, "chkMatchCase1", wxCheckBox)->SetValue(ConfigManager::Get()->Read(CONF_GROUP _T("/match_case1"), 0L));
	XRCCTRL(*this, "chkRegEx1", wxCheckBox)->SetValue(ConfigManager::Get()->Read(CONF_GROUP _T("/regex1"), 0L));
	XRCCTRL(*this, "rbDirection", wxRadioBox)->SetSelection(ConfigManager::Get()->Read(CONF_GROUP _T("/direction"), 1));
	XRCCTRL(*this, "rbDirection", wxRadioBox)->Enable(!XRCCTRL(*this, "chkRegEx1", wxCheckBox)->GetValue()); // if regex, only forward searches
	XRCCTRL(*this, "rbOrigin", wxRadioBox)->SetSelection(ConfigManager::Get()->Read(CONF_GROUP _T("/origin"), 0L));
	XRCCTRL(*this, "rbScope1", wxRadioBox)->SetSelection(hasSelection);
	XRCCTRL(*this, "rbScope1", wxRadioBox)->Enable(hasSelection);

	// find in files options
	XRCCTRL(*this, "cmbFind2", wxComboBox)->SetValue(initial);
	XRCCTRL(*this, "chkWholeWord2", wxCheckBox)->SetValue(ConfigManager::Get()->Read(CONF_GROUP _T("/match_word2"), 0L));
	XRCCTRL(*this, "chkStartWord2", wxCheckBox)->SetValue(ConfigManager::Get()->Read(CONF_GROUP _T("/start_word2"), 0L));
	XRCCTRL(*this, "chkMatchCase2", wxCheckBox)->SetValue(ConfigManager::Get()->Read(CONF_GROUP _T("/match_case2"), 0L));
	XRCCTRL(*this, "chkRegEx2", wxCheckBox)->SetValue(ConfigManager::Get()->Read(CONF_GROUP _T("/regex2"), 0L));
	XRCCTRL(*this, "rbScope2", wxRadioBox)->SetSelection(ConfigManager::Get()->Read(CONF_GROUP _T("/scope2"), 0L));

	if (!m_Complete)
    {
        XRCCTRL(*this, "nbFind", wxNotebook)->DeletePage(0); // no active editor, so only find-in-files
        XRCCTRL(*this, "cmbFind2", wxComboBox)->SetFocus();
    }
    else
        XRCCTRL(*this, "cmbFind1", wxComboBox)->SetFocus();
}

FindDlg::~FindDlg()
{
	// save last searches (up to 10)
    wxComboBox* combo = XRCCTRL(*this, "cmbFind1", wxComboBox);
	if (!m_Complete)
        combo = XRCCTRL(*this, "cmbFind2", wxComboBox);
    wxArrayString previous;
    for (int i = 0; (i < combo->GetCount()) && (i < 10); ++i)
    {
        if (!combo->GetString(i).IsEmpty())
            previous.Add(combo->GetString(i));
    }
    wxString find = combo->GetValue();
    if (combo->FindString(find) == -1)
        previous.Insert(find, 0);
    wxString last = GetStringFromArray(previous);
    ConfigManager::Get()->Write(CONF_GROUP _T("/last"), last);

	if (m_Complete)
	{
        // find options
        ConfigManager::Get()->Write(CONF_GROUP _T("/match_word1"), XRCCTRL(*this, "chkWholeWord1", wxCheckBox)->GetValue());
        ConfigManager::Get()->Write(CONF_GROUP _T("/start_word1"), XRCCTRL(*this, "chkStartWord1", wxCheckBox)->GetValue());
        ConfigManager::Get()->Write(CONF_GROUP _T("/match_case1"), XRCCTRL(*this, "chkMatchCase1", wxCheckBox)->GetValue());
        ConfigManager::Get()->Write(CONF_GROUP _T("/regex1"), XRCCTRL(*this, "chkRegEx1", wxCheckBox)->GetValue());
        ConfigManager::Get()->Write(CONF_GROUP _T("/direction"), XRCCTRL(*this, "rbDirection", wxRadioBox)->GetSelection());
        ConfigManager::Get()->Write(CONF_GROUP _T("/origin"), XRCCTRL(*this, "rbOrigin", wxRadioBox)->GetSelection());
	}

	// find in files options
	ConfigManager::Get()->Write(CONF_GROUP _T("/match_word2"), XRCCTRL(*this, "chkWholeWord2", wxCheckBox)->GetValue());
	ConfigManager::Get()->Write(CONF_GROUP _T("/start_word2"), XRCCTRL(*this, "chkStartWord2", wxCheckBox)->GetValue());
	ConfigManager::Get()->Write(CONF_GROUP _T("/match_case2"), XRCCTRL(*this, "chkMatchCase2", wxCheckBox)->GetValue());
	ConfigManager::Get()->Write(CONF_GROUP _T("/regex2"), XRCCTRL(*this, "chkRegEx2", wxCheckBox)->GetValue());
	ConfigManager::Get()->Write(CONF_GROUP _T("/scope2"), XRCCTRL(*this, "rbScope2", wxRadioBox)->GetSelection());
}

wxString FindDlg::GetFindString()
{
	if (IsFindInFiles())
        return XRCCTRL(*this, "cmbFind2", wxComboBox)->GetValue();
    else
        return XRCCTRL(*this, "cmbFind1", wxComboBox)->GetValue();
}

bool FindDlg::IsFindInFiles()
{
	return !m_Complete || XRCCTRL(*this, "nbFind", wxNotebook)->GetSelection() == 1;
}

bool FindDlg::GetMatchWord()
{
	if (IsFindInFiles())
		return XRCCTRL(*this, "chkWholeWord2", wxCheckBox)->GetValue();
	else
		return XRCCTRL(*this, "chkWholeWord1", wxCheckBox)->GetValue();
}

bool FindDlg::GetStartWord()
{
	if (IsFindInFiles())
		return XRCCTRL(*this, "chkStartWord2", wxCheckBox)->GetValue();
	else
		return XRCCTRL(*this, "chkStartWord1", wxCheckBox)->GetValue();
}

bool FindDlg::GetMatchCase()
{
	if (IsFindInFiles())
		return XRCCTRL(*this, "chkMatchCase2", wxCheckBox)->GetValue();
	else
		return XRCCTRL(*this, "chkMatchCase1", wxCheckBox)->GetValue();
}

bool FindDlg::GetRegEx()
{
	if (IsFindInFiles())
		return XRCCTRL(*this, "chkRegEx2", wxCheckBox)->GetValue();
	else
		return XRCCTRL(*this, "chkRegEx1", wxCheckBox)->GetValue();
}

int FindDlg::GetDirection()
{
	if (IsFindInFiles())
        return 1;
    else
        return XRCCTRL(*this, "rbDirection", wxRadioBox)->GetSelection();
}

int FindDlg::GetOrigin()
{
	if (IsFindInFiles())
        return 1;
    else
        return XRCCTRL(*this, "rbOrigin", wxRadioBox)->GetSelection();
}

int FindDlg::GetScope()
{
	if (IsFindInFiles())
		return XRCCTRL(*this, "rbScope2", wxRadioBox)->GetSelection();
	else
		return XRCCTRL(*this, "rbScope1", wxRadioBox)->GetSelection();
}

// events

void FindDlg::OnFindChange(wxCommandEvent& event)
{
    wxComboBox* cmbFind1 = XRCCTRL(*this, "cmbFind1", wxComboBox);
    wxComboBox* cmbFind2 = XRCCTRL(*this, "cmbFind2", wxComboBox);

	if (cmbFind2 && event.GetId() == XRCID("cmbFind1"))
		cmbFind2->SetValue(cmbFind1->GetValue());
	else if (cmbFind1)
		cmbFind1->SetValue(cmbFind2->GetValue());
}

void FindDlg::OnRegEx(wxCommandEvent& event)
{
	if (m_Complete)
        XRCCTRL(*this, "rbDirection", wxRadioBox)->Enable(!XRCCTRL(*this, "chkRegEx1", wxCheckBox)->GetValue());
}

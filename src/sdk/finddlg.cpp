/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
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

BEGIN_EVENT_TABLE(FindDlg, wxScrollingDialog)
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
    wxXmlResource::Get()->LoadObject(this, parent, _T("dlgFind"),_T("wxScrollingDialog"));
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));

    // load last searches
    wxArrayString previous;
    cfg->Read(CONF_GROUP _T("/last"), &previous);
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
    XRCCTRL(*this, "chkAutoWrapSearch", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/auto_wrap_search"), true));
    XRCCTRL(*this, "chkFindUsesSelectedText", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/findUsesSelectedText"), false));

    XRCCTRL(*this, "rbDirection", wxRadioBox)->SetSelection(cfg->ReadInt(CONF_GROUP _T("/direction"), 1));
    XRCCTRL(*this, "rbDirection", wxRadioBox)->Enable(!XRCCTRL(*this, "chkRegEx1", wxCheckBox)->GetValue()); // if regex, only forward searches
    XRCCTRL(*this, "rbOrigin", wxRadioBox)->SetSelection(cfg->ReadInt(CONF_GROUP _T("/origin"), 0));
    XRCCTRL(*this, "rbScope1", wxRadioBox)->SetSelection(hasSelection);
    //XRCCTRL(*this, "rbScope1", wxRadioBox)->Enable(hasSelection);

    // find in files options
    XRCCTRL(*this, "cmbFind2", wxComboBox)->SetValue(initial);
    XRCCTRL(*this, "chkWholeWord2", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/match_word2"), false));
    XRCCTRL(*this, "chkStartWord2", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/start_word2"), false));
    XRCCTRL(*this, "chkMatchCase2", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/match_case2"), false));
    XRCCTRL(*this, "chkRegEx2", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/regex2"), false));
    XRCCTRL(*this, "chkDelOldSearchRes2", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/delete_old_searches2"), true));
    XRCCTRL(*this, "chkSortSearchResult2", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/sort_search_results2"), true));
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
    {
        // NOTE (jens#1#): Do not delete, just hide the page, to avoid asserts in debug-mode
        (XRCCTRL(*this, "nbFind", wxNotebook)->GetPage(0))->Hide(); // no active editor, so only find-in-files
        XRCCTRL(*this, "cmbFind2", wxComboBox)->SetFocus();
    }

    if (findInFilesActive)
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
    for (int i = 0; (i < (int)combo->GetCount()) && (i < 10); ++i)
    {
        if (!combo->GetString(i).IsEmpty() && (previous.Index(combo->GetString(i)) == wxNOT_FOUND))
            previous.Add(combo->GetString(i));
    }
    // Now bump the latest search string to top
    wxString find = combo->GetValue();
    int prev_pos = previous.Index(find);
    if (prev_pos != wxNOT_FOUND)
        previous.RemoveAt(prev_pos);
    previous.Insert(find, 0);

    cfg->Write(CONF_GROUP _T("/last"), previous);

    if (m_Complete)
    {
        // find options
        cfg->Write(CONF_GROUP _T("/match_word1"), XRCCTRL(*this, "chkWholeWord1", wxCheckBox)->GetValue());
        cfg->Write(CONF_GROUP _T("/start_word1"), XRCCTRL(*this, "chkStartWord1", wxCheckBox)->GetValue());
        cfg->Write(CONF_GROUP _T("/match_case1"), XRCCTRL(*this, "chkMatchCase1", wxCheckBox)->GetValue());
        cfg->Write(CONF_GROUP _T("/regex1"), XRCCTRL(*this, "chkRegEx1", wxCheckBox)->GetValue());
        cfg->Write(CONF_GROUP _T("/auto_wrap_search"), XRCCTRL(*this, "chkAutoWrapSearch", wxCheckBox)->GetValue());
        cfg->Write(CONF_GROUP _T("/findUsesSelectedText"), XRCCTRL(*this, "chkFindUsesSelectedText", wxCheckBox)->GetValue());

        cfg->Write(CONF_GROUP _T("/direction"), XRCCTRL(*this, "rbDirection", wxRadioBox)->GetSelection());
        cfg->Write(CONF_GROUP _T("/origin"), XRCCTRL(*this, "rbOrigin", wxRadioBox)->GetSelection());
    }

    // find in files options
    cfg->Write(CONF_GROUP _T("/match_word2"), XRCCTRL(*this, "chkWholeWord2", wxCheckBox)->GetValue());
    cfg->Write(CONF_GROUP _T("/start_word2"), XRCCTRL(*this, "chkStartWord2", wxCheckBox)->GetValue());
    cfg->Write(CONF_GROUP _T("/match_case2"), XRCCTRL(*this, "chkMatchCase2", wxCheckBox)->GetValue());
    cfg->Write(CONF_GROUP _T("/regex2"), XRCCTRL(*this, "chkRegEx2", wxCheckBox)->GetValue());
    cfg->Write(CONF_GROUP _T("/delete_old_searches2"), XRCCTRL(*this, "chkDelOldSearchRes2", wxCheckBox)->GetValue());
    cfg->Write(CONF_GROUP _T("/sort_search_results2"), XRCCTRL(*this, "chkSortSearchResult2", wxCheckBox)->GetValue());
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
    if ( IsFindInFiles() )
        return XRCCTRL(*this, "chkDelOldSearchRes2", wxCheckBox)->GetValue();

    return true;  // checkbox doesn't exist in Find dialog
}

bool FindDlg::GetSortSearchResult() const
{
    if (IsFindInFiles())
        return XRCCTRL(*this, "chkSortSearchResult2", wxCheckBox)->GetValue();

    return true;  // checkbox doesn't exist in Find dialog
}

bool FindDlg::GetMatchWord() const
{
    if ( IsFindInFiles() )
        return XRCCTRL(*this, "chkWholeWord2", wxCheckBox)->GetValue();

    return XRCCTRL(*this, "chkWholeWord1", wxCheckBox)->GetValue();
}

bool FindDlg::GetStartWord() const
{
    if ( IsFindInFiles() )
        return XRCCTRL(*this, "chkStartWord2", wxCheckBox)->GetValue();

    return XRCCTRL(*this, "chkStartWord1", wxCheckBox)->GetValue();
}

bool FindDlg::GetMatchCase() const
{
    if ( IsFindInFiles() )
        return XRCCTRL(*this, "chkMatchCase2", wxCheckBox)->GetValue();

    return XRCCTRL(*this, "chkMatchCase1", wxCheckBox)->GetValue();
}

bool FindDlg::GetRegEx() const
{
    if ( IsFindInFiles() )
        return XRCCTRL(*this, "chkRegEx2", wxCheckBox)->GetValue();

    return XRCCTRL(*this, "chkRegEx1", wxCheckBox)->GetValue();
}
bool FindDlg::GetAutoWrapSearch() const
{
    if ( IsFindInFiles() )
        return false; // not for search in files

    return XRCCTRL(*this, "chkAutoWrapSearch", wxCheckBox)->GetValue();
}

bool FindDlg::GetFindUsesSelectedText() const
{
     if ( IsFindInFiles() )
        return false;

    return XRCCTRL(*this, "chkFindUsesSelectedText", wxCheckBox)->GetValue();
}

int FindDlg::GetDirection() const
{
    if ( IsFindInFiles() )
        return 1;

    return XRCCTRL(*this, "rbDirection", wxRadioBox)->GetSelection();
}

int FindDlg::GetOrigin() const
{
    if ( IsFindInFiles() )
        return 1;

    return XRCCTRL(*this, "rbOrigin", wxRadioBox)->GetSelection();
}

int FindDlg::GetScope() const
{
    if ( IsFindInFiles() )
        return XRCCTRL(*this, "rbScope2", wxRadioBox)->GetSelection();

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

bool FindDlg::GetStartFile() const
{
// TODO (rick#1#): Implement FindDlg::GetStartFile()
    return false;
}

bool FindDlg::GetFixEOLs() const
{
    // Fixing EOLs only applicable in "Replace"
    return false;
}

void FindDlg::UpdateUI()
{
    bool on = XRCCTRL(*this, "rbScope2", wxRadioBox)->GetSelection() == 3; // find in search path
    XRCCTRL(*this, "txtSearchPath", wxTextCtrl)->Enable(on);
    XRCCTRL(*this, "txtSearchMask", wxTextCtrl)->Enable(on);
    XRCCTRL(*this, "btnBrowsePath", wxButton)->Enable(on);
    XRCCTRL(*this, "chkSearchRecursively", wxCheckBox)->Enable(on);
    XRCCTRL(*this, "chkSearchHidden", wxCheckBox)->Enable(on);
} // end of UpdateUI

// events

void FindDlg::OnFindChange(wxNotebookEvent& event)
{
    if (    (event.GetOldSelection() == 0 && event.GetSelection() == 1)
         || (event.GetOldSelection() == 1 && event.GetSelection() == 0))
    {
        wxComboBox* cmbFind1 = XRCCTRL(*this, "cmbFind1", wxComboBox);
        wxComboBox* cmbFind2 = XRCCTRL(*this, "cmbFind2", wxComboBox);

        if (event.GetSelection() == 0)
        {
            cmbFind1->SetValue(cmbFind2->GetValue());
            cmbFind1->SetFocus();
        }
        else if (event.GetSelection() == 1)
        {
            cmbFind2->SetValue(cmbFind1->GetValue());
            cmbFind2->SetFocus();
        }
    }
    event.Skip();
}

void FindDlg::OnRegEx(wxCommandEvent& /*event*/)
{
    if (m_Complete)
        XRCCTRL(*this, "rbDirection", wxRadioBox)->Enable(!XRCCTRL(*this, "chkRegEx1", wxCheckBox)->GetValue());
}

void FindDlg::OnBrowsePath(wxCommandEvent& /*event*/)
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



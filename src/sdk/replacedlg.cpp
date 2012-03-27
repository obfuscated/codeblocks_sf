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

    #include <wx/xrc/xmlres.h>
    #include <wx/intl.h>
    #include <wx/combobox.h>
    #include <wx/checkbox.h>
    #include <wx/button.h>
    #include <wx/notebook.h>
    #include <wx/radiobox.h>
    #include <wx/sizer.h>
#endif

#include "replacedlg.h"


#define CONF_GROUP _T("/replace_options")

BEGIN_EVENT_TABLE(ReplaceDlg, wxScrollingDialog)
    EVT_CHECKBOX(XRCID("chkMultiLine1"), ReplaceDlg::OnMultiChange)
    EVT_CHECKBOX(XRCID("chkMultiLine2"), ReplaceDlg::OnMultiChange)
    EVT_CHECKBOX(XRCID("chkLimitTo1"), ReplaceDlg::OnLimitToChange)
    EVT_CHECKBOX(XRCID("chkLimitTo2"), ReplaceDlg::OnLimitToChange)

    EVT_CHECKBOX(XRCID("chkRegEx1"),    ReplaceDlg::OnRegEx)
    EVT_ACTIVATE(                       ReplaceDlg::OnActivate)
END_EVENT_TABLE()

ReplaceDlg::ReplaceDlg(wxWindow* parent, const wxString& initial, bool hasSelection,
                       bool findInFilesOnly, bool replaceInFilesActive)
    : FindReplaceBase(parent, initial, hasSelection),
    m_Complete(!replaceInFilesActive)
{
    wxXmlResource::Get()->LoadObject(this, parent, _T("dlgReplace"),_T("wxScrollingDialog"));
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));

    // Get minimum size. This must be done before loading the values, or our dialog could get REALLY fat.
    wxSize sizeSingle = XRCCTRL(*this, "tabReplaceSingle", wxPanel)->GetEffectiveMinSize();
    wxSize sizeMulti  = XRCCTRL(*this, "tabReplaceMulti", wxPanel)->GetEffectiveMinSize();
    wxSize sizeFilesSingle = XRCCTRL(*this, "tabReplaceInFilesSingle", wxPanel)->GetEffectiveMinSize();
    wxSize sizeFilesMulti  = XRCCTRL(*this, "tabReplaceInFilesMulti", wxPanel)->GetEffectiveMinSize();

    sizeSingle.IncTo(sizeMulti);
    sizeFilesSingle.IncTo(sizeFilesMulti);

    wxSize tmpsize = XRCCTRL(*this, "nbReplaceInFilesSingleMulti", wxPanel)->GetClientSize();
    XRCCTRL(*this, "tabReplaceInFilesMulti", wxPanel)->SetSize(tmpsize);
    XRCCTRL(*this, "tabReplaceInFilesSingle", wxPanel)->SetSize(tmpsize);

    tmpsize = XRCCTRL(*this, "nbReplaceSingleMulti", wxPanel)->GetClientSize();
    XRCCTRL(*this, "tabReplaceMulti", wxPanel)->SetSize(tmpsize);
    XRCCTRL(*this, "tabReplaceSingle", wxPanel)->SetSize(tmpsize);

    // load last searches
    FillComboWithLastValues(XRCCTRL(*this, "cmbFind1", wxComboBox), CONF_GROUP _T("/last"));
    FillComboWithLastValues(XRCCTRL(*this, "cmbReplace1", wxComboBox), CONF_GROUP _T("/lastReplace"));
    FillComboWithLastValues(XRCCTRL(*this, "cmbFind2", wxComboBox), CONF_GROUP _T("/last"));
    FillComboWithLastValues(XRCCTRL(*this, "cmbReplace2", wxComboBox), CONF_GROUP _T("/lastReplace"));

    // load last multiline searches
    XRCCTRL(*this, "txtMultiLineFind1", wxTextCtrl)->SetValue(cfg->Read(CONF_GROUP _T("/lastMultiLineFind"), _T("")));
    XRCCTRL(*this, "txtMultiLineReplace1", wxTextCtrl)->SetValue(cfg->Read(CONF_GROUP _T("/lastMultiLineReplace"), _T("")));
    XRCCTRL(*this, "txtMultiLineFind2", wxTextCtrl)->SetValue(cfg->Read(CONF_GROUP _T("/lastMultiLineFind"), _T("")));
    XRCCTRL(*this, "txtMultiLineReplace2", wxTextCtrl)->SetValue(cfg->Read(CONF_GROUP _T("/lastMultiLineReplace"), _T("")));

    // replace options
    XRCCTRL(*this, "cmbFind1", wxComboBox)->SetValue(initial);

    bool flgWholeWord = cfg->ReadBool(CONF_GROUP _T("/match_word"), false);
    bool flgStartWord = cfg->ReadBool(CONF_GROUP _T("/start_word"), false);
    bool flgStartFile = cfg->ReadBool(CONF_GROUP _T("/start_file"), false);
    XRCCTRL(*this, "chkLimitTo1", wxCheckBox)->SetValue(flgWholeWord | flgStartWord | flgStartFile);
    XRCCTRL(*this, "rbLimitTo1", wxRadioBox)->Enable((bool)(flgWholeWord | flgStartWord | flgStartFile));
    XRCCTRL(*this, "rbLimitTo1", wxRadioBox)->SetSelection(flgStartFile ? 2 : (flgStartWord ? 1 : 0));

    XRCCTRL(*this, "chkMatchCase1", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/match_case"), false));
    XRCCTRL(*this, "chkRegEx1", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/regex"), false));
    XRCCTRL(*this, "rbDirection", wxRadioBox)->SetSelection(cfg->ReadInt(CONF_GROUP _T("/direction"), 1));
    XRCCTRL(*this, "rbDirection", wxRadioBox)->Enable(!XRCCTRL(*this, "chkRegEx1", wxCheckBox)->GetValue()); // if regex, only forward searches
    XRCCTRL(*this, "rbOrigin", wxRadioBox)->SetSelection(cfg->ReadInt(CONF_GROUP _T("/origin"), 0));
    XRCCTRL(*this, "rbScope1", wxRadioBox)->SetSelection(hasSelection);
    //XRCCTRL(*this, "rbScope1", wxRadioBox)->Enable(hasSelection);
    // special key, uses same config for both find & replace options
    XRCCTRL(*this, "chkAutoWrapSearch", wxCheckBox)->SetValue(cfg->ReadBool(_T("/find_options/auto_wrap_search"), true));

    // replace in files options
    flgWholeWord = cfg->ReadBool(CONF_GROUP _T("/match_word2"), false);
    flgStartWord = cfg->ReadBool(CONF_GROUP _T("/start_word2"), false);
    flgStartFile = cfg->ReadBool(CONF_GROUP _T("/start_file2"), false);
    XRCCTRL(*this, "chkLimitTo2", wxCheckBox)->SetValue(flgWholeWord | flgStartWord | flgStartFile);
    XRCCTRL(*this, "rbLimitTo2", wxRadioBox)->Enable((bool)(flgWholeWord | flgStartWord | flgStartFile));
    XRCCTRL(*this, "rbLimitTo2", wxRadioBox)->SetSelection(flgStartFile ? 2 : (flgStartWord ? 1 : 0));

    XRCCTRL(*this, "cmbFind2", wxComboBox)->SetValue(initial);
    XRCCTRL(*this, "chkMatchCase2", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/match_case2"), false));
    XRCCTRL(*this, "chkRegEx2", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/regex2"), false));
    XRCCTRL(*this, "rbScope2", wxRadioBox)->SetSelection(cfg->ReadInt(CONF_GROUP _T("/scope2"), 0));

    XRCCTRL(*this, "nbReplaceSingleMulti", wxPanel)->SetMinSize(sizeSingle);
    XRCCTRL(*this, "nbReplaceInFilesSingleMulti", wxPanel)->SetMinSize(sizeFilesSingle);

    XRCCTRL(*this, "chkMultiLine1", wxCheckBox)->SetValue(false);
    XRCCTRL(*this, "chkFixEOLs1", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/fix_eols1"), false));
    XRCCTRL(*this, "chkFixEOLs1", wxCheckBox)->Enable(XRCCTRL(*this, "chkMultiLine1", wxCheckBox)->GetValue());

    XRCCTRL(*this, "chkMultiLine2", wxCheckBox)->SetValue(false);
    XRCCTRL(*this, "chkFixEOLs2", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/fix_eols2"), false));
    XRCCTRL(*this, "chkFixEOLs2", wxCheckBox)->Enable(XRCCTRL(*this, "chkMultiLine2", wxCheckBox)->GetValue());

    XRCCTRL(*this, "tabReplaceSingle", wxPanel)->Show();
    XRCCTRL(*this, "tabReplaceMulti", wxPanel)->Hide();
    XRCCTRL(*this, "tabReplaceMulti", wxPanel)->Disable();
    XRCCTRL(*this, "tabReplaceInFilesSingle", wxPanel)->Show();
    XRCCTRL(*this, "tabReplaceInFilesMulti", wxPanel)->Hide();
    XRCCTRL(*this, "tabReplaceInFilesMulti", wxPanel)->Disable();

    if (findInFilesOnly)
    {
// NOTE (jens#1#): Do not delete, just hide the page, to avoid asserts in debug-mode
        (XRCCTRL(*this, "nbReplace", wxNotebook)->GetPage(0))->Hide(); // no active editor, so only replace-in-files
        XRCCTRL(*this, "cmbFind2", wxComboBox)->SetFocus();
    }
    else if (replaceInFilesActive)
    {
        XRCCTRL(*this, "nbReplace", wxNotebook)->SetSelection(1); // Search->Replace in Files was selected
        XRCCTRL(*this, "cmbFind2", wxComboBox)->SetFocus();
    }

    GetSizer()->SetSizeHints(this);

// NOTE (jens#1#): Dynamically connect these events, to avoid asserts in debug-mode
    Connect(XRCID("nbReplace"), wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler(ReplaceDlg::OnFindChange));
    Connect(wxEVT_SIZE, wxSizeEventHandler(ReplaceDlg::OnSize));

}

ReplaceDlg::~ReplaceDlg()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));

    // save last searches (up to 10)
    if (IsFindInFiles())
    {
        SaveComboValues(XRCCTRL(*this, "cmbFind2", wxComboBox), CONF_GROUP _T("/last"));
        SaveComboValues(XRCCTRL(*this, "cmbReplace2", wxComboBox), CONF_GROUP _T("/lastReplace"));

        // Save last multi-line search & replace
        cfg->Write(CONF_GROUP _T("/lastMultiLineFind"), XRCCTRL(*this, "txtMultiLineFind2", wxTextCtrl)->GetValue());
        cfg->Write(CONF_GROUP _T("/lastMultiLineReplace"), XRCCTRL(*this, "txtMultiLineReplace2", wxTextCtrl)->GetValue());
    }
    else
    {
        SaveComboValues(XRCCTRL(*this, "cmbFind1", wxComboBox), CONF_GROUP _T("/last"));
        SaveComboValues(XRCCTRL(*this, "cmbReplace1", wxComboBox), CONF_GROUP _T("/lastReplace"));

        // Save last multi-line search & replace
        cfg->Write(CONF_GROUP _T("/lastMultiLineFind"), XRCCTRL(*this, "txtMultiLineFind1", wxTextCtrl)->GetValue());
        cfg->Write(CONF_GROUP _T("/lastMultiLineReplace"), XRCCTRL(*this, "txtMultiLineReplace1", wxTextCtrl)->GetValue());
    }

    if (m_Complete)
    {
        // find(replace) options
        cfg->Write(CONF_GROUP _T("/fix_eols1"), XRCCTRL(*this, "chkFixEOLs1", wxCheckBox)->GetValue());
        cfg->Write(CONF_GROUP _T("/match_case"), XRCCTRL(*this, "chkMatchCase1", wxCheckBox)->GetValue());
        cfg->Write(CONF_GROUP _T("/regex"), XRCCTRL(*this, "chkRegEx1", wxCheckBox)->GetValue());
        cfg->Write(CONF_GROUP _T("/direction"), XRCCTRL(*this, "rbDirection", wxRadioBox)->GetSelection());
        cfg->Write(CONF_GROUP _T("/origin"), XRCCTRL(*this, "rbOrigin", wxRadioBox)->GetSelection());

        bool flgLimitTo = XRCCTRL(*this, "chkLimitTo1", wxCheckBox)->GetValue();
        int  valLimitTo = XRCCTRL(*this, "rbLimitTo1", wxRadioBox)->GetSelection();

        cfg->Write(CONF_GROUP _T("/match_word"), flgLimitTo && valLimitTo == 0);
        cfg->Write(CONF_GROUP _T("/start_word"), flgLimitTo && valLimitTo == 1);
        cfg->Write(CONF_GROUP _T("/start_file"), flgLimitTo && valLimitTo == 2);

        // special key, uses same config for both find & replace options
        cfg->Write(_T("/find_options/auto_wrap_search"), XRCCTRL(*this, "chkAutoWrapSearch", wxCheckBox)->GetValue());
    }

    // find(replace) in files options
    bool flgLimitTo = XRCCTRL(*this, "chkLimitTo2", wxCheckBox)->GetValue();
    int  valLimitTo = XRCCTRL(*this, "rbLimitTo2", wxRadioBox)->GetSelection();

    cfg->Write(CONF_GROUP _T("/match_word2"), flgLimitTo && valLimitTo == 0);
    cfg->Write(CONF_GROUP _T("/start_word2"), flgLimitTo && valLimitTo == 1);
    cfg->Write(CONF_GROUP _T("/start_file2"), flgLimitTo && valLimitTo == 2);

    cfg->Write(CONF_GROUP _T("/fix_eols2"), XRCCTRL(*this, "chkFixEOLs2", wxCheckBox)->GetValue());
    cfg->Write(CONF_GROUP _T("/match_case2"), XRCCTRL(*this, "chkMatchCase2", wxCheckBox)->GetValue());
    cfg->Write(CONF_GROUP _T("/regex2"), XRCCTRL(*this, "chkRegEx2", wxCheckBox)->GetValue());
    cfg->Write(CONF_GROUP _T("/scope2"), XRCCTRL(*this, "rbScope2", wxRadioBox)->GetSelection());

    Disconnect(XRCID("nbReplace"), wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler(ReplaceDlg::OnFindChange));
    Disconnect(wxEVT_SIZE, wxSizeEventHandler(ReplaceDlg::OnSize));
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
    for (int i = 0; (i < (int)combo->GetCount()) && (i < 10); ++i)
    {
        if (!combo->GetString(i).IsEmpty() && (values.Index(combo->GetString(i)) == wxNOT_FOUND))
            values.Add(combo->GetString(i));
    }
    wxString find = combo->GetValue();
    int prev_pos = values.Index(find);
    if (prev_pos != wxNOT_FOUND)
        values.RemoveAt(prev_pos);
    values.Insert(find, 0);
    Manager::Get()->GetConfigManager(_T("editor"))->Write(configKey, values);
}

wxString ReplaceDlg::GetFindString() const
{
    if (IsMultiLine())
    {
        if (IsFindInFiles())
            return XRCCTRL(*this, "txtMultiLineFind2", wxTextCtrl)->GetValue();
        else
            return XRCCTRL(*this, "txtMultiLineFind1", wxTextCtrl)->GetValue();
    }
    else
    {
        if (IsFindInFiles())
            return XRCCTRL(*this, "cmbFind2", wxComboBox)->GetValue();
        else
            return XRCCTRL(*this, "cmbFind1", wxComboBox)->GetValue();
    }
}

wxString ReplaceDlg::GetReplaceString() const
{
    if (IsMultiLine())
    {
        wxString tmpString;

        if (IsFindInFiles())
            return XRCCTRL(*this, "txtMultiLineReplace2", wxTextCtrl)->GetValue();
        else
            return XRCCTRL(*this, "txtMultiLineReplace1", wxTextCtrl)->GetValue();
    }
    else
    {
        if (IsFindInFiles())
            return XRCCTRL(*this, "cmbReplace2", wxComboBox)->GetValue();
        else
            return XRCCTRL(*this, "cmbReplace1", wxComboBox)->GetValue();
    }
}

bool ReplaceDlg::IsFindInFiles() const
{
    return !m_Complete || XRCCTRL(*this, "nbReplace", wxNotebook)->GetSelection() == 1;
}

bool ReplaceDlg::IsMultiLine() const
{
    if(IsFindInFiles())
        return XRCCTRL(*this, "chkMultiLine2", wxCheckBox)->GetValue();
    else
        return XRCCTRL(*this, "chkMultiLine1", wxCheckBox)->GetValue();
}

bool ReplaceDlg::GetDeleteOldSearches() const
{
    return false; // checkbox doesn't exist in both dialogs
}

bool ReplaceDlg::GetMatchWord() const
{
    if (IsFindInFiles())
    {
        bool flgLimitTo = XRCCTRL(*this, "chkLimitTo2", wxCheckBox)->GetValue();
        return flgLimitTo && XRCCTRL(*this, "rbLimitTo2", wxRadioBox)->GetSelection() == 0;
    }
    else
    {
        bool flgLimitTo = XRCCTRL(*this, "chkLimitTo1", wxCheckBox)->GetValue();
        return flgLimitTo && XRCCTRL(*this, "rbLimitTo1", wxRadioBox)->GetSelection() == 0;
    }
}

bool ReplaceDlg::GetStartWord() const
{
    if (IsFindInFiles())
    {
        bool flgLimitTo = XRCCTRL(*this, "chkLimitTo2", wxCheckBox)->GetValue();
        return flgLimitTo && XRCCTRL(*this, "rbLimitTo2", wxRadioBox)->GetSelection() == 1;
    }
    else
    {
        bool flgLimitTo = XRCCTRL(*this, "chkLimitTo1", wxCheckBox)->GetValue();
        return flgLimitTo && XRCCTRL(*this, "rbLimitTo1", wxRadioBox)->GetSelection() == 1;
    }
}

bool ReplaceDlg::GetStartFile() const
{
    if (IsFindInFiles())
    {
        bool flgLimitTo = XRCCTRL(*this, "chkLimitTo2", wxCheckBox)->GetValue();
        return flgLimitTo && XRCCTRL(*this, "rbLimitTo2", wxRadioBox)->GetSelection() == 2;
    }
    else
    {
        bool flgLimitTo = XRCCTRL(*this, "chkLimitTo1", wxCheckBox)->GetValue();
        return flgLimitTo && XRCCTRL(*this, "rbLimitTo1", wxRadioBox)->GetSelection() == 2;
    }
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
        return false; // not for replace in files
    else
        return XRCCTRL(*this, "chkAutoWrapSearch", wxCheckBox)->GetValue();
}

bool ReplaceDlg::GetFindUsesSelectedText() const
{
    return false; // not for replace
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

bool ReplaceDlg::GetFixEOLs() const
{
    if (IsFindInFiles())
        return XRCCTRL(*this, "chkFixEOLs2", wxCheckBox)->GetValue();
    else
        return XRCCTRL(*this, "chkFixEOLs1", wxCheckBox)->GetValue();
}

// events

void ReplaceDlg::OnSize(wxSizeEvent& event)
{
    wxPanel* tmpPanel = XRCCTRL(*this, "nbReplaceInFilesSingleMulti", wxPanel);
    if(tmpPanel)
    {
        wxSize tmpsize = tmpPanel->GetClientSize();
        tmpPanel = XRCCTRL(*this, "tabReplaceInFilesMulti", wxPanel);
        if(tmpPanel) tmpPanel->SetSize(tmpsize);
        tmpPanel = XRCCTRL(*this, "tabReplaceInFilesSingle", wxPanel);
        if(tmpPanel) tmpPanel->SetSize(tmpsize);
    }

    tmpPanel = XRCCTRL(*this, "nbReplaceSingleMulti", wxPanel);
    if(tmpPanel)
    {
        wxSize tmpsize = tmpPanel->GetClientSize();
        tmpPanel = XRCCTRL(*this, "tabReplaceMulti", wxPanel);
        if(tmpPanel) tmpPanel->SetSize(tmpsize);
        tmpPanel = XRCCTRL(*this, "tabReplaceSingle", wxPanel);
        if(tmpPanel) tmpPanel->SetSize(tmpsize);
    }
    event.Skip();
}

void ReplaceDlg::OnMultiChange(wxCommandEvent& event)
{
    wxComboBox* cmbFind1 = XRCCTRL(*this, "cmbFind1", wxComboBox);
    wxComboBox* cmbFind2 = XRCCTRL(*this, "cmbFind2", wxComboBox);
    wxTextCtrl* txtFind1 = XRCCTRL(*this, "txtMultiLineFind1", wxTextCtrl);
    wxTextCtrl* txtFind2 = XRCCTRL(*this, "txtMultiLineFind2", wxTextCtrl);
    wxCheckBox* chkMultiLine1 = XRCCTRL(*this, "chkMultiLine1", wxCheckBox);
    wxCheckBox* chkMultiLine2 = XRCCTRL(*this, "chkMultiLine2", wxCheckBox);
    wxCheckBox* chkFixEOLs1 = XRCCTRL(*this, "chkFixEOLs1", wxCheckBox);
    wxCheckBox* chkFixEOLs2 = XRCCTRL(*this, "chkFixEOLs2", wxCheckBox);

    wxPanel* tabReplaceSingle = XRCCTRL(*this, "tabReplaceSingle", wxPanel);
    wxPanel* tabReplaceMulti = XRCCTRL(*this, "tabReplaceMulti", wxPanel);
    wxPanel* tabReplaceInFilesSingle = XRCCTRL(*this, "tabReplaceInFilesSingle", wxPanel);
    wxPanel* tabReplaceInFilesMulti = XRCCTRL(*this, "tabReplaceInFilesMulti", wxPanel);
    wxWindow* controlToFocus = 0;

    bool enabledMultiLine = false;
    if (event.GetId() == XRCID("chkMultiLine1"))
    {
        enabledMultiLine = chkMultiLine1->GetValue();
        if(chkMultiLine2) chkMultiLine2->SetValue(enabledMultiLine);
        controlToFocus = enabledMultiLine ? dynamic_cast<wxWindow*>(txtFind1) : dynamic_cast<wxWindow*>(cmbFind1);
    }
    else if (event.GetId() == XRCID("chkMultiLine2"))
    {
        enabledMultiLine = chkMultiLine2->GetValue();
        if(chkMultiLine1) chkMultiLine1->SetValue(enabledMultiLine);
        controlToFocus = enabledMultiLine ? dynamic_cast<wxWindow*>(txtFind2) : dynamic_cast<wxWindow*>(cmbFind2);
    }
    else
        return;

    if(tabReplaceSingle)
    {
        tabReplaceSingle->Enable(!enabledMultiLine);
        tabReplaceSingle->Show(!enabledMultiLine);
    }
    if(tabReplaceInFilesSingle)
    {
        tabReplaceInFilesSingle->Enable(!enabledMultiLine);
        tabReplaceInFilesSingle->Show(!enabledMultiLine);
    }
    if(tabReplaceMulti)
    {
        tabReplaceMulti->Enable(enabledMultiLine);
        tabReplaceMulti->Show(enabledMultiLine);
    }
    if(tabReplaceInFilesMulti)
    {
        tabReplaceInFilesMulti->Show(enabledMultiLine);
        tabReplaceInFilesMulti->Enable(enabledMultiLine);
    }
    if(chkFixEOLs1)
    {
        chkFixEOLs1->Enable(enabledMultiLine);
    }
    chkFixEOLs2->Enable(enabledMultiLine);
    if(controlToFocus) controlToFocus->SetFocus();

    wxSizeEvent event1;
    OnSize(event1);
    Refresh();
}

void ReplaceDlg::OnFindChange(wxNotebookEvent& event)
{
    wxComboBox* cmbFind1  = XRCCTRL(*this, "cmbFind1", wxComboBox);
    wxComboBox* cmbFind2  = XRCCTRL(*this, "cmbFind2", wxComboBox);
    wxTextCtrl* txtFind1  = XRCCTRL(*this, "txtMultiLineFind1", wxTextCtrl);
    wxTextCtrl* txtFind2  = XRCCTRL(*this, "txtMultiLineFind2", wxTextCtrl);
    wxComboBox* cmbReplace1 =XRCCTRL(*this, "cmbReplace1", wxComboBox); //
    wxComboBox* cmbReplace2 =XRCCTRL(*this, "cmbReplace2", wxComboBox); //

    if (txtFind1 && txtFind2 && cmbFind1 && cmbFind2)
    {
        if (event.GetSelection() == 0)
        {
            // Switched from "Find in files" to "Find"

            txtFind1->SetValue(txtFind2->GetValue());

            cmbFind1->SetValue(cmbFind2->GetValue());
            cmbReplace1->SetValue(cmbReplace2->GetValue());
        }
        else if (event.GetSelection() == 1)
        {
            // Switched from "Find" to "Find in files"

            txtFind2->SetValue(txtFind1->GetValue());

            cmbFind2->SetValue(cmbFind1->GetValue());
            cmbReplace2->SetValue(cmbReplace1->GetValue());
        }
    }
    wxSizeEvent event1;
    OnSize(event1);
    event.Skip();
}

void ReplaceDlg::OnRegEx(wxCommandEvent& /*event*/)
{
    if (m_Complete)
        XRCCTRL(*this, "rbDirection", wxRadioBox)->Enable(!XRCCTRL(*this, "chkRegEx1", wxCheckBox)->GetValue());
}

void ReplaceDlg::OnLimitToChange(wxCommandEvent& event)
{
    if (event.GetId() == XRCID("chkLimitTo1"))
        XRCCTRL(*this, "rbLimitTo1", wxRadioBox)->Enable(XRCCTRL(*this, "chkLimitTo1", wxCheckBox)->GetValue());
    else
        XRCCTRL(*this, "rbLimitTo2", wxRadioBox)->Enable(XRCCTRL(*this, "chkLimitTo2", wxCheckBox)->GetValue());
}

void ReplaceDlg::OnActivate(wxActivateEvent& event)
{
    if (IsMultiLine())
    {
        wxTextCtrl* tcp = 0;
        if (IsFindInFiles())
            tcp = XRCCTRL(*this, "txtMultiLineFind2", wxTextCtrl);
        else
            tcp = XRCCTRL(*this, "txtMultiLineFind1", wxTextCtrl);

        if (tcp != 0) tcp->SetFocus();
    }
    else
    {
        wxComboBox* cbp = 0;
        if (IsFindInFiles())
            cbp = XRCCTRL(*this, "cmbFind2", wxComboBox);
        else
            cbp = XRCCTRL(*this, "cmbFind1", wxComboBox);

        if (cbp != 0) cbp->SetFocus();

    }

    event.Skip();
}

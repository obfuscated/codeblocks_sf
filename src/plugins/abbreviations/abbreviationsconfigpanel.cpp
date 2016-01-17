/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#include "abbreviationsconfigpanel.h"

#ifndef CB_PRECOMP
    #include <wx/xrc/xmlres.h>
    #include <wx/fontutil.h>
    #include <wx/choicdlg.h>

    #include <configmanager.h>
    #include <editorcolourset.h>
    #include <editormanager.h>
    #include <logmanager.h>
#endif

#include "cbcolourmanager.h"

BEGIN_EVENT_TABLE(AbbreviationsConfigPanel, cbConfigurationPanel)
END_EVENT_TABLE()

AbbreviationsConfigPanel::AbbreviationsConfigPanel(wxWindow* parent, Abbreviations* plugin) :
    m_AutoCompTextControl(nullptr),
    m_Plugin(plugin)
{
    wxXmlResource::Get()->LoadObject(this, parent, _T("AbbreviationsConfigPanel"), _T("wxPanel"));

    m_LanguageCmb = XRCCTRL(*this, "cmbAutoCompLanguage", wxComboBox);
    FillLangugages();
    m_LanguageCmb->SetSelection(0);
    m_LastAutoCompLanguage = defaultLanguageStr;

    InitCompText();
    m_Keyword = XRCCTRL(*this, "lstAutoCompKeyword", wxListBox);
    m_Keyword->Clear();
    m_pCurrentAutoCompMap = m_Plugin->m_AutoCompLanguageMap[defaultLanguageStr];

    FillKeywords();

    Connect(XRCID("lstAutoCompKeyword"), wxEVT_COMMAND_LISTBOX_SELECTED, (wxObjectEventFunction)&AbbreviationsConfigPanel::OnAutoCompKeyword);
    Connect(XRCID("btnAutoCompAdd"), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&AbbreviationsConfigPanel::OnAutoCompAdd);
    Connect(XRCID("btnAutoCompDelete"), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&AbbreviationsConfigPanel::OnAutoCompDelete);
    Connect(XRCID("cmbAutoCompLanguage"), wxEVT_COMMAND_COMBOBOX_SELECTED, (wxObjectEventFunction)&AbbreviationsConfigPanel::OnLanguageSelect);
    Connect(XRCID("btnAutoCompAddLanguage"), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&AbbreviationsConfigPanel::OnLanguageAdd);
    Connect(XRCID("btnAutoCompCopyLanguage"), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&AbbreviationsConfigPanel::OnLanguageCopy);
    Connect(XRCID("btnAutoCompDelLanguage"), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&AbbreviationsConfigPanel::OnLanguageDelete);
}

AbbreviationsConfigPanel::~AbbreviationsConfigPanel()
{
    Disconnect(XRCID("lstAutoCompKeyword"), wxEVT_COMMAND_LISTBOX_SELECTED, (wxObjectEventFunction)&AbbreviationsConfigPanel::OnAutoCompKeyword);
    Disconnect(XRCID("btnAutoCompAdd"), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&AbbreviationsConfigPanel::OnAutoCompAdd);
    Disconnect(XRCID("btnAutoCompDelete"), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&AbbreviationsConfigPanel::OnAutoCompDelete);
    Disconnect(XRCID("cmbAutoCompLanguage"), wxEVT_COMMAND_COMBOBOX_SELECTED, (wxObjectEventFunction)&AbbreviationsConfigPanel::OnLanguageSelect);
    Disconnect(XRCID("btnAutoCompAddLanguage"), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&AbbreviationsConfigPanel::OnLanguageAdd);
    Disconnect(XRCID("btnAutoCompCopyLanguage"), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&AbbreviationsConfigPanel::OnLanguageCopy);
    Disconnect(XRCID("btnAutoCompDelLanguage"), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&AbbreviationsConfigPanel::OnLanguageDelete);

    if (m_AutoCompTextControl)
        delete m_AutoCompTextControl;
}

void AbbreviationsConfigPanel::InitCompText()
{
    if (m_AutoCompTextControl)
        delete m_AutoCompTextControl;
    m_AutoCompTextControl = new cbStyledTextCtrl(this, wxID_ANY);
    m_AutoCompTextControl->SetTabWidth(4);
    m_AutoCompTextControl->SetMarginType(0, wxSCI_MARGIN_NUMBER);
    m_AutoCompTextControl->SetMarginWidth(0, 32);
    m_AutoCompTextControl->SetViewWhiteSpace(1);
    m_AutoCompTextControl->SetMinSize(wxSize(50, 50));

    wxColor ccolor = Manager::Get()->GetColourManager()->GetColour(wxT("editor_caret"));
    m_AutoCompTextControl->SetCaretForeground( ccolor );

    ApplyColours();
    wxXmlResource::Get()->AttachUnknownControl(_T("txtAutoCompCode"), m_AutoCompTextControl);
}

void AbbreviationsConfigPanel::ApplyColours()
{
    EditorColourSet* colour_set = Manager::Get()->GetEditorManager()->GetColourSet();
    if (colour_set)
    {
        wxString FontString = Manager::Get()->GetConfigManager(_T("editor"))->Read(_T("/font"), wxEmptyString);
        wxFont tmpFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

        if (!FontString.IsEmpty())
        {
            wxNativeFontInfo nfi;
            nfi.FromString(FontString);
            tmpFont.SetNativeFontInfo(nfi);
        }
        if (m_AutoCompTextControl)
        {
            m_AutoCompTextControl->StyleSetFont(wxSCI_STYLE_DEFAULT, tmpFont);
            colour_set->Apply(colour_set->GetHighlightLanguage(_T("C/C++")), m_AutoCompTextControl);
        }
    }
}

void AbbreviationsConfigPanel::AutoCompUpdate(const wxString& key, const wxString& lang)
{
    if (!key.IsEmpty())
    {
        AutoCompleteMap* compMap = m_Plugin->m_AutoCompLanguageMap[lang];
        wxString code = m_AutoCompTextControl->GetText();
        code.Replace(wxT("\r\n"), wxT("\n"));
        code.Replace(wxT("\r"),   wxT("\n"));
        (*compMap)[key] = code;
    }
}

void AbbreviationsConfigPanel::OnAutoCompAdd(cb_unused wxCommandEvent& event)
{
    wxString key = wxGetTextFromUser(_("Please enter the new keyword"), _("Add keyword"));
    if (!key.IsEmpty())
    {
        AutoCompleteMap::iterator it = m_pCurrentAutoCompMap->find(key);
        if (it != m_pCurrentAutoCompMap->end())
        {
            cbMessageBox(_("This keyword already exists!"), _("Error"), wxICON_ERROR, this);
            return;
        }
        (*m_pCurrentAutoCompMap)[key] = wxEmptyString;
        int sel = m_Keyword->Append(key);
        m_Keyword->SetSelection(sel);
        m_LastAutoCompKeyword = m_Keyword->GetStringSelection();
        AutoCompUpdate(m_LastAutoCompKeyword,m_LanguageCmb->GetValue());
        m_AutoCompTextControl->SetText(wxEmptyString);
    }
}

void AbbreviationsConfigPanel::OnAutoCompDelete(cb_unused wxCommandEvent& event)
{
    if (m_Keyword->GetSelection() == -1)
        return;

    if (cbMessageBox(_("Are you sure you want to delete this keyword?"), _("Confirmation"),
                     wxICON_QUESTION | wxYES_NO, this) == wxID_NO)
        return;

    int delsel = m_Keyword->GetSelection();
    AutoCompleteMap::iterator it = m_pCurrentAutoCompMap->find(m_Keyword->GetString(delsel));
    if (it != m_pCurrentAutoCompMap->end())
    {
        int sel = delsel + 1;
        if (sel >= (int)(m_Keyword->GetCount()))
            sel = m_Keyword->GetCount() - 2;
        m_Keyword->SetSelection(sel);
        m_LastAutoCompKeyword = m_Keyword->GetStringSelection();
        if (sel >= 0)
            m_AutoCompTextControl->SetText((*m_pCurrentAutoCompMap)[m_Keyword->GetString(sel)]);
        else
            m_AutoCompTextControl->SetText(wxEmptyString);
        m_Keyword->Delete(delsel);
        m_pCurrentAutoCompMap->erase(it);
    }
}

void AbbreviationsConfigPanel::OnAutoCompKeyword(cb_unused wxCommandEvent& event)
{
    if (   m_LastAutoCompKeyword.IsSameAs(m_Keyword->GetStringSelection())
        && m_LastAutoCompLanguage.IsSameAs(m_LanguageCmb->GetValue()))
        return;

    AutoCompUpdate(m_LastAutoCompKeyword, m_LastAutoCompLanguage);

    // list new keyword's code
    m_LastAutoCompKeyword  = m_Keyword->GetStringSelection();
    if (!m_LastAutoCompKeyword.IsEmpty())
        m_AutoCompTextControl->SetText((*m_pCurrentAutoCompMap)[m_LastAutoCompKeyword]);
    else
        m_AutoCompTextControl->SetText(wxEmptyString);
    m_LastAutoCompLanguage = m_LanguageCmb->GetValue();
}

void AbbreviationsConfigPanel::OnApply()
{
    // save any changes in auto-completion
    AutoCompUpdate(m_Keyword->GetStringSelection(),m_LanguageCmb->GetValue());

    for (AutoCompLanguageMap::iterator it = m_Plugin->m_AutoCompLanguageMap.begin(); it != m_Plugin->m_AutoCompLanguageMap.end(); ++it)
    {
        Abbreviations::ExchangeTabAndSpaces(*it->second);
    }
    //*(m_Plugin->m_AutoCompLanguageMap[defaultLanguageStr]) = m_AutoCompMap;
}

void AbbreviationsConfigPanel::OnCancel()
{}

wxString AbbreviationsConfigPanel::GetTitle() const
{ return _("Abbreviations"); }

wxString AbbreviationsConfigPanel::GetBitmapBaseName() const
{ return _T("abbrev"); }

void AbbreviationsConfigPanel::FillLangugages()
{
    m_LanguageCmb->Clear();

    wxSortedArrayString langs;
    AutoCompLanguageMap::iterator it;
    for (it = m_Plugin->m_AutoCompLanguageMap.begin(); it != m_Plugin->m_AutoCompLanguageMap.end(); ++it)
    {
        langs.Add(it->first);
    }
    m_LanguageCmb->Append(langs);
}

void AbbreviationsConfigPanel::FillKeywords()
{
    m_Keyword->Clear();

    for (AutoCompleteMap::iterator it = m_pCurrentAutoCompMap->begin(); it != m_pCurrentAutoCompMap->end(); ++it)
        m_Keyword->Append(it->first);

    if (!m_pCurrentAutoCompMap->empty())
    {
        m_Keyword->SetSelection(0);
        m_LastAutoCompKeyword = m_Keyword->GetStringSelection();
        m_AutoCompTextControl->SetText((*m_pCurrentAutoCompMap)[m_Keyword->GetString(m_Keyword->GetSelection())]);
    }
    else
    {
        m_LastAutoCompKeyword = wxEmptyString;
        m_AutoCompTextControl->SetText(wxEmptyString);
    }
}

void AbbreviationsConfigPanel::OnLanguageSelect(cb_unused wxCommandEvent& event)
{
    LanguageSelected();
}

void AbbreviationsConfigPanel::LanguageSelected()
{
    AutoCompUpdate(m_LastAutoCompKeyword, m_LastAutoCompLanguage);
    Abbreviations::ExchangeTabAndSpaces(*m_pCurrentAutoCompMap);
    wxString lang = m_LanguageCmb->GetValue();
    m_pCurrentAutoCompMap = m_Plugin->m_AutoCompLanguageMap[lang];
    FillKeywords();
    m_LastAutoCompLanguage = lang;
}

void AbbreviationsConfigPanel::OnLanguageAdd(cb_unused wxCommandEvent& event)
{
    LanguageAdd();
}

int AbbreviationsConfigPanel::LanguageAdd()
{
    EditorColourSet* colSet = Manager::Get()->GetEditorManager()->GetColourSet();
    if (colSet)
        colSet = new EditorColourSet(*colSet);
    else
        colSet = new EditorColourSet();

    wxSortedArrayString newLangs;
    wxArrayString langs = colSet->GetAllHighlightLanguages();
    for (unsigned int i = 0; i < langs.GetCount(); ++i)
    {
        if (m_Plugin->m_AutoCompLanguageMap.find(langs[i]) == m_Plugin->m_AutoCompLanguageMap.end() &&
            !langs[i].IsSameAs(_T("Fortran77")))
            newLangs.Add(langs[i]);
    }

    int sel = cbGetSingleChoiceIndex(_("Select language:"), _("Languages"), newLangs, this);
    if (sel == -1)
        return sel;

    m_Plugin->m_AutoCompLanguageMap[newLangs[sel]] = new AutoCompleteMap();
    FillLangugages();
    m_LanguageCmb->SetValue(newLangs[sel]);
    LanguageSelected();
    return 0;
}

void AbbreviationsConfigPanel::OnLanguageCopy(cb_unused wxCommandEvent& event)
{
    wxString langCopy = m_LanguageCmb->GetValue();
    if (LanguageAdd() == -1)
        return;
    wxString langNew = m_LanguageCmb->GetValue();
    AutoCompleteMap* pMapCopy = m_Plugin->m_AutoCompLanguageMap[langCopy];
    AutoCompleteMap* pMapNew  = m_Plugin->m_AutoCompLanguageMap[langNew];
    for (AutoCompleteMap::iterator it = pMapCopy->begin(); it != pMapCopy->end(); ++it)
    {
        (*pMapNew)[it->first] = it->second;
    }
    FillKeywords();
}

void AbbreviationsConfigPanel::OnLanguageDelete(cb_unused wxCommandEvent& event)
{
    wxString lang = m_LanguageCmb->GetValue();
    if (lang.IsSameAs(defaultLanguageStr) || lang.IsSameAs(_T("Fortran")))
    {
        cbMessageBox(_("Abbreviations for ") + lang + _(" language can't be deleted!"), _("Error"),
                     wxICON_ERROR | wxOK, this);
        return;
    }

    if (cbMessageBox(_("Are you sure you want to delete abbreviations for ") + lang + _(" language?"), _("Confirmation"),
                     wxICON_QUESTION | wxYES_NO | wxNO_DEFAULT, this) == wxID_NO)
        return;

    m_LanguageCmb->SetSelection(0);
    LanguageSelected();

    AutoCompleteMap* pAutoCompleteMap = m_Plugin->m_AutoCompLanguageMap[lang];
    pAutoCompleteMap->clear();
    delete pAutoCompleteMap;
    m_Plugin->m_AutoCompLanguageMap.erase(lang);
    FillLangugages();
    m_LanguageCmb->SetSelection(0);
}

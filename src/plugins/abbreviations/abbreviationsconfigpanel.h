#ifndef ABBREVIATIONSCONFIGPANEL_H
#define ABBREVIATIONSCONFIGPANEL_H

#include "abbreviations.h"
#include <cbstyledtextctrl.h>

#include <configurationpanel.h>

#include <wx/stattext.h>
#include <wx/listbox.h>
#include <wx/panel.h>
#include <wx/button.h>

class AbbreviationsConfigPanel: public cbConfigurationPanel
{
public:
    AbbreviationsConfigPanel(wxWindow* parent, Abbreviations* plugin);
    virtual ~AbbreviationsConfigPanel();

    /// @return the panel's title.
    virtual wxString GetTitle() const;
    /// @return the panel's bitmap base name. You must supply two bitmaps: \<basename\>.png and \<basename\>-off.png...
    virtual wxString GetBitmapBaseName() const;
    /// Called when the user chooses to apply the configuration.
    virtual void OnApply();
    /// Called when the user chooses to cancel the configuration.
    virtual void OnCancel();

private:
    void InitCompText();
    void AutoCompUpdate(const wxString& key, const wxString& lang);
    void ApplyColours();

    void OnAutoCompKeyword(wxCommandEvent& event);
    void OnAutoCompAdd(wxCommandEvent& event);
    void OnAutoCompDelete(wxCommandEvent& event);
    void OnLanguageSelect(wxCommandEvent& event);
    void OnLanguageAdd(wxCommandEvent& event);
    void OnLanguageCopy(wxCommandEvent& event);
    void OnLanguageDelete(wxCommandEvent& event);

    void FillLangugages();
    void FillKeywords();
    void LanguageSelected();
    int  LanguageAdd();

private:
    cbStyledTextCtrl* m_AutoCompTextControl;
    wxListBox*        m_Keyword;
    wxString          m_LastAutoCompKeyword;
    wxString          m_LastAutoCompLanguage;
    AutoCompleteMap*  m_pCurrentAutoCompMap;
    Abbreviations*    m_Plugin;

    wxComboBox*       m_LanguageCmb;

    DECLARE_EVENT_TABLE()
};

#endif

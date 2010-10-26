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
    void AutoCompUpdate(int index);
    void ApplyColours();

    void OnAutoCompKeyword(wxCommandEvent& event);
    void OnAutoCompAdd(wxCommandEvent& event);
    void OnAutoCompDelete(wxCommandEvent& event);

private:
    cbStyledTextCtrl* m_AutoCompTextControl;
    wxListBox*        m_Keyword;
    int               m_LastAutoCompKeyword;
    AutoCompleteMap   m_AutoCompMap;
    Abbreviations*    m_Plugin;

    DECLARE_EVENT_TABLE()
};

#endif

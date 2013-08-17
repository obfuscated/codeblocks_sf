#ifndef OCCURRENCESHIGHLIGHTINGCONFIGURATIONPANEL_H
#define OCCURRENCESHIGHLIGHTINGCONFIGURATIONPANEL_H

#include <cbstyledtextctrl.h>
#include <configurationpanel.h>

#include <wx/string.h>

class wxCommandEvent;

class OccurrencesHighlightingConfigurationPanel : public cbConfigurationPanel
{
public:
    OccurrencesHighlightingConfigurationPanel(wxWindow* parent);
    virtual ~OccurrencesHighlightingConfigurationPanel();

    /// @return the panel's title.
    virtual wxString GetTitle() const;
    /// @return the panel's bitmap base name. You must supply two bitmaps: \<basename\>.png and \<basename\>-off.png...
    virtual wxString GetBitmapBaseName() const;
    /// Called when the user chooses to apply the configuration.
    virtual void OnApply();
    /// Called when the user chooses to cancel the configuration.
    virtual void OnCancel();

private:
    void OnChooseColour(wxCommandEvent& event);
    void OnCheckHighlightOccurrences(wxCommandEvent& event);

private:

    DECLARE_EVENT_TABLE()
};

#endif

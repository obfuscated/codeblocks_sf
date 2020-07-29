/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

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
    ~OccurrencesHighlightingConfigurationPanel() override;

    /// @return the panel's title.
    wxString GetTitle() const override;
    /// @return the panel's bitmap base name. You must supply two bitmaps: \<basename\>.png and \<basename\>-off.png...
    wxString GetBitmapBaseName() const override;
    /// Called when the user chooses to apply the configuration.
    void OnApply() override;
    /// Called when the user chooses to cancel the configuration.
    void OnCancel() override;

private:
    void OnChooseColour(wxCommandEvent& event);
    void OnCheck(wxCommandEvent& event);

    void UpdateUI();
private:

    DECLARE_EVENT_TABLE()
};

#endif

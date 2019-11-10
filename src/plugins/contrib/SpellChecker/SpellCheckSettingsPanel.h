/*
* This file is part of SpellChecker plugin for Code::Blocks Studio
* Copyright (C) 2009 Daniel Anselmi
*
* SpellChecker plugin is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* SpellChecker plugin is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with SpellChecker. If not, see <http://www.gnu.org/licenses/>.
*
*/
#ifndef SPELLCHECKSETTINGSPANEL_H
#define SPELLCHECKSETTINGSPANEL_H

#include <configurationpanel.h>

#include <map>
#include <vector>


//(*Headers(SpellCheckSettingsPanel)
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/hyperlink.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
//*)

class SpellCheckerConfig;

class SpellCheckSettingsPanel: public cbConfigurationPanel
{
public:

    //SpellCheckSettingsPanel(wxWindow* parent);
    SpellCheckSettingsPanel(wxWindow* parent, SpellCheckerConfig *cfg);
    virtual ~SpellCheckSettingsPanel();

    /// @return the panel's title.
    virtual wxString GetTitle() const;
    /// @return the panel's bitmap base name. You must supply two bitmaps: \<basename\>.png and \<basename\>-off.png...
    virtual wxString GetBitmapBaseName() const;
    /// Called when the user chooses to apply the configuration.
    virtual void OnApply();
    /// Called when the user chooses to cancel the configuration.
    virtual void OnCancel();

private:

    //(*Declarations(SpellCheckSettingsPanel)
    wxButton* Button1;
    wxButton* Button2;
    wxButton* Button3;
    wxChoice* m_choiceDictionary;
    wxStaticText* StaticText1;
    wxHyperlinkCtrl* HyperlinkCtrl1;
    wxTextCtrl* m_TextThPath;
    wxStaticText* StaticText3;
    wxCheckBox* m_checkSpellTooltips;
    wxStaticText* StaticText4;
    wxStaticText* StaticText2;
    wxTextCtrl* m_TextBitmapPath;
    wxCheckBox* m_checkEnableOnlineSpellChecker;
    wxTextCtrl* m_TextDictPath;
    wxCheckBox* m_checkThesaurusTooltips;
    //*)

    SpellCheckerConfig *m_sccfg;



private:
    void InitDictionaryChoice(const wxString &path = wxEmptyString);
    void PostConfig();

protected:

    //(*Identifiers(SpellCheckSettingsPanel)
    //*)

private:

    //(*Handlers(SpellCheckSettingsPanel)
    void OnChooseDirectory(wxCommandEvent& event);
    //*)

    void OnChangeDictPathText( wxCommandEvent &event);


private:
    DECLARE_EVENT_TABLE()
};

#endif

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
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/hyperlink.h>
#include <wx/choice.h>
#include <wx/button.h>
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
		wxTextCtrl* m_TextThPath;
		wxChoice* m_choiceDictionary;
		wxStaticText* StaticText2;
		wxTextCtrl* m_TextDictPath;
		wxButton* Button1;
		wxStaticText* StaticText1;
		wxStaticText* StaticText3;
		wxHyperlinkCtrl* HyperlinkCtrl1;
		wxButton* Button2;
		wxButton* Button3;
		wxTextCtrl* m_TextBitmapPath;
		wxStaticText* StaticText4;
		wxCheckBox* m_checkEnableOnlineSpellChecker;
		//*)

        SpellCheckerConfig *m_sccfg;



    private:
        void InitDictionaryChoice();
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

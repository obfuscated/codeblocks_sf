/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef EDITORCONFIGURATION_H
#define EDITORCONFIGURATION_H

#include <wx/dialog.h>
#include <wx/arrstr.h>
#include "settings.h"

// forward decls
class cbStyledTextCtrl;
class EditorColourSet;
class wxListbookEvent;

class EditorConfigurationDlg : public wxDialog
{
	public:
		EditorConfigurationDlg(wxWindow* parent);
    	~EditorConfigurationDlg();

		void OnChooseFont(wxCommandEvent& event);
        void OnChooseColour(wxCommandEvent& event);
        void OnBoldItalicUline(wxCommandEvent& event);
		void OnColourTheme(wxCommandEvent& event);
		void OnAddColourTheme(wxCommandEvent& event);
		void OnDeleteColourTheme(wxCommandEvent& event);
		void OnRenameColourTheme(wxCommandEvent& event);
		void OnColourComponent(wxCommandEvent& event);
		void OnColoursReset(wxCommandEvent& event);
		void OnEditKeywords(wxCommandEvent& event);
		void OnEditFilemasks(wxCommandEvent& event);
		void OnChangeLang(wxCommandEvent& event);
		void OnChangeDefCodeFileType(wxCommandEvent& event);
		void OnAutoCompKeyword(wxCommandEvent& event);
		void OnAutoCompAdd(wxCommandEvent& event);
		void OnAutoCompDelete(wxCommandEvent& event);
		void OnDynamicCheck(wxCommandEvent& event);
		void UpdateSampleFont(bool askForNewFont);

		void EndModal(int retCode);
    private:
        void OnPageChanged(wxListbookEvent& event);
        void AddPluginPanels();
        void UpdateListbookImages();
		void CreateColoursSample();
		void CreateAutoCompText();
		void AutoCompUpdate(int index);
		void ApplyColours();
		void FillColourComponents();
		void ReadColours();
		void WriteColours();
		void LoadThemes();
		void ChangeTheme();
		bool AskToSaveTheme();
		cbStyledTextCtrl* m_TextColourControl;
		cbStyledTextCtrl* m_AutoCompTextControl;
		EditorColourSet* m_Theme;
		HighlightLanguage m_Lang;
		int m_DefCodeFileType;
		wxArrayString m_DefaultCode;  //!< will store the default code during configuration for each type
		bool m_ThemeModified;
		int m_LastAutoCompKeyword;
		AutoCompleteMap m_AutoCompMap;
        ConfigurationPanelsArray m_PluginPanels;
        wxString m_FontString; // This is to store font data temporarily

    	DECLARE_EVENT_TABLE()
};

#endif // EDITORCONFIGURATION_H

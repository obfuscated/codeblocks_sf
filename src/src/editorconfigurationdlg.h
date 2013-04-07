/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef EDITORCONFIGURATION_H
#define EDITORCONFIGURATION_H

#include <map>
#include "scrollingdialog.h"
#include <wx/arrstr.h>
#include "settings.h"

// forward decls
class cbStyledTextCtrl;
class EditorColourSet;
class wxListbookEvent;

class EditorConfigurationDlg : public wxScrollingDialog
{
    public:
        EditorConfigurationDlg(wxWindow* parent);
        ~EditorConfigurationDlg();

    private:
        void OnChooseFont(wxCommandEvent& event);
        void OnChooseColour(wxCommandEvent& event);
        void OnSetDefaultColour(wxCommandEvent& event);
        void OnBoldItalicUline(wxCommandEvent& event);
        void OnColourTheme(wxCommandEvent& event);
        void OnAddColourTheme(wxCommandEvent& event);
        void OnDeleteColourTheme(wxCommandEvent& event);
        void OnRenameColourTheme(wxCommandEvent& event);
        void OnColourComponent(wxCommandEvent& event);
        void OnColoursReset(wxCommandEvent& event);
        void OnColoursCopyFrom(wxCommandEvent& event);
        void OnMenuColoursCopyFrom(wxCommandEvent& event);
        void OnColoursCopyAllFrom(wxCommandEvent& event);
        void OnMenuColoursCopyAllFrom(wxCommandEvent &event);
        void OnEditKeywords(wxCommandEvent& event);
        void OnEditFilemasks(wxCommandEvent& event);
        void OnChangeLang(wxCommandEvent& event);
        void OnChangeDefCodeFileType(wxCommandEvent& event);
        void OnDynamicCheck(wxCommandEvent& event);
        void OnHighlightOccurrences(wxCommandEvent& event);
        void OnMultipleSelections(wxCommandEvent& event);
        void UpdateSampleFont(bool askForNewFont);
        void OnCaretStyle(wxCommandEvent& event);

        void EndModal(int retCode);
    private:
        void OnPageChanged(wxListbookEvent& event);
        void AddPluginPanels();
        void UpdateListbookImages();
        void CreateColoursSample();
        void ApplyColours();
        void FillColourComponents();
        void ReadColours();
        void WriteColours();
        void LoadThemes();
        void ChangeTheme();
        bool AskToSaveTheme();
        void UpdateColourControls(const OptionColour *opt);
    private:
        struct MenuItemLanguageOptionID
        {
            wxString langID;
            int index;
        };
        typedef std::map<long, MenuItemLanguageOptionID> MenuIDToLanguageOption;
    private:
        cbStyledTextCtrl* m_TextColourControl;
        EditorColourSet* m_Theme;
        HighlightLanguage m_Lang;
        int m_DefCodeFileType;
        wxArrayString m_DefaultCode;  //!< will store the default code during configuration for each type
        bool m_ThemeModified;
        ConfigurationPanelsArray m_PluginPanels;
        wxString m_FontString; // This is to store font data temporarily
        bool m_EnableChangebar; // remember whether changebar was enabled or not
        bool m_EnableScrollWidthTracking; // remember whether scrollbarwidthtracking was enabled or not
        MenuIDToLanguageOption m_MenuIDToLanguageOption;

        DECLARE_EVENT_TABLE()
};

#endif // EDITORCONFIGURATION_H

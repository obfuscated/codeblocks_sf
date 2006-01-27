#ifndef EDITORCONFIGURATION_H
#define EDITORCONFIGURATION_H

#include <wx/dialog.h>
#include "settings.h"
#include "editormanager.h"

// forward decls
class cbStyledTextCtrl;
class EditorColorSet;
class wxListbookEvent;

class EditorConfigurationDlg : public wxDialog
{
	public:
		EditorConfigurationDlg(wxWindow* parent);
    	~EditorConfigurationDlg();

		void OnChooseFont(wxCommandEvent& event);
        void OnChooseColor(wxCommandEvent& event);
        void OnBoldItalicUline(wxCommandEvent& event);
		void OnColorTheme(wxCommandEvent& event);
		void OnAddColorTheme(wxCommandEvent& event);
		void OnDeleteColorTheme(wxCommandEvent& event);
		void OnRenameColorTheme(wxCommandEvent& event);
		void OnColorComponent(wxCommandEvent& event);
		void OnColorsReset(wxCommandEvent& event);
		void OnEditKeywords(wxCommandEvent& event);
		void OnEditFilemasks(wxCommandEvent& event);
		void OnChangeLang(wxCommandEvent& event);
		void OnChangeDefCodeFileType(wxCommandEvent& event);
		void OnAutoCompKeyword(wxCommandEvent& event);
		void OnAutoCompAdd(wxCommandEvent& event);
		void OnAutoCompDelete(wxCommandEvent& event);
		void UpdateSampleFont(bool askForNewFont);

		void EndModal(int retCode);
    private:
        void OnPageChanged(wxListbookEvent& event);
        void AddPluginPanels();
        void UpdateListbookImages();
		void CreateColorsSample();
		void CreateAutoCompText();
		void AutoCompUpdate(int index);
		void ApplyColors();
		void FillColorComponents();
		void ReadColors();
		void WriteColors();
		void LoadThemes();
		void ChangeTheme();
		bool AskToSaveTheme();
		cbStyledTextCtrl* m_TextColorControl;
		cbStyledTextCtrl* m_AutoCompTextControl;
		EditorColorSet* m_Theme;
		HighlightLanguage m_Lang;
		int m_DefCodeFileType;
		bool m_ThemeModified;
		int m_LastAutoCompKeyword;
		AutoCompleteMap m_AutoCompMap;
        ConfigurationPanelsArray m_PluginPanels;

    	DECLARE_EVENT_TABLE()
};

#endif // EDITORCONFIGURATION_H

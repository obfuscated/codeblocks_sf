#ifndef EDITORCONFIGURATION_H
#define EDITORCONFIGURATION_H

#include <wx/dialog.h>
#include "settings.h"

// forward decls
class wxStyledTextCtrl;
class EditorColorSet;

class EditorConfigurationDlg : public wxDialog
{
	public:
		EditorConfigurationDlg(wxWindow* parent);
    	~EditorConfigurationDlg();

		void OnOK(wxCommandEvent& event);
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
		void OnChangeLang(wxCommandEvent& event);
		void OnChangeDefCodeFileType(wxCommandEvent& event);
		void UpdateSampleFont(bool askForNewFont);
    private:
		void CreateColorsSample();
		void ApplyColors();
		void FillColorComponents();
		void ReadColors();
		void WriteColors();
		void LoadThemes();
		void ChangeTheme();
		bool AskToSaveTheme();
		wxStyledTextCtrl* m_TextColorControl;
		EditorColorSet* m_Theme;
		HighlightLanguage m_Lang;
		int m_DefCodeFileType;
		bool m_ThemeModified;
    	DECLARE_EVENT_TABLE()
};

#endif // EDITORCONFIGURATION_H

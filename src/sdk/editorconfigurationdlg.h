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
		void OnColorComponent(wxCommandEvent& event);
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
		wxStyledTextCtrl* m_TextColorControl;
		EditorColorSet* m_Theme;
		HighlightLanguage m_Lang;
		int m_DefCodeFileType;
    	DECLARE_EVENT_TABLE()
};

#endif // EDITORCONFIGURATION_H

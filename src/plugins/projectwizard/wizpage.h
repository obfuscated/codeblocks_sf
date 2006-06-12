#ifndef WIZPAGE_H
#define WIZPAGE_H

#include <wx/string.h>
#include <wx/wizard.h>
#include <wx/panel.h>
#include <wx/xrc/xmlres.h>

class ProjectPathPanel;
class CompilerPanel;
class LanguagePanel;

class WizPage : public wxWizardPageSimple
{
	public:
		WizPage(const wxString& panelName, wxWizard* parent, const wxBitmap& bitmap = wxNullBitmap);
		~WizPage();
	    void OnButton(wxCommandEvent& event);
	    void OnPageChanging(wxWizardEvent& event);
	    void OnPageChanged(wxWizardEvent& event);
    protected:
        wxString m_PanelName;
    private:
		DECLARE_EVENT_TABLE()
};

class WizIntroPanel : public wxWizardPageSimple
{
	public:
		WizIntroPanel(const wxString& intro_msg, wxWizard* parent, const wxBitmap& bitmap = wxNullBitmap);
		~WizIntroPanel();
    protected:
    private:
};

class WizProjectPathPanel : public wxWizardPageSimple
{
	public:
		WizProjectPathPanel(wxWizard* parent, const wxBitmap& bitmap = wxNullBitmap);
		~WizProjectPathPanel();

		wxString GetPath();
		wxString GetName();

	    void OnButton(wxCommandEvent& event);
	    void OnPageChanging(wxWizardEvent& event);
	    void OnPageChanged(wxWizardEvent& event);
    protected:
        ProjectPathPanel* m_pProjectPathPanel;
    private:
		DECLARE_EVENT_TABLE()
};

class WizCompilerPanel : public wxWizardPageSimple
{
	public:
		WizCompilerPanel(const wxString& compilerID, const wxString& validCompilerIDs, wxWizard* parent, const wxBitmap& bitmap = wxNullBitmap,
                        bool allowCompilerChange = true, bool allowConfigChange = true);
		~WizCompilerPanel();

		wxString GetCompilerID();
		bool GetWantDebug();
        wxString GetDebugName();
        wxString GetDebugOutputDir();
        wxString GetDebugObjectOutputDir();
		bool GetWantRelease();
        wxString GetReleaseName();
        wxString GetReleaseOutputDir();
        wxString GetReleaseObjectOutputDir();

	    void OnPageChanging(wxWizardEvent& event);
    protected:
        CompilerPanel* m_pCompilerPanel;
    private:
		DECLARE_EVENT_TABLE()
};

class WizLanguagePanel : public wxWizardPageSimple
{
	public:
		WizLanguagePanel(const wxArrayString& langs, int defLang, wxWizard* parent, const wxBitmap& bitmap = wxNullBitmap);
		~WizLanguagePanel();

        int GetLanguage() const;
        void SetLanguage(int lang);
    protected:
        LanguagePanel* m_pLanguagePanel;
    private:
};

#endif // WIZPAGE_H

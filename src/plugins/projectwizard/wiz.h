/***************************************************************
 * Name:      wiz.h
 * Purpose:   Code::Blocks plugin
 * Author:    <>
 * Copyright: (c)
 * License:   GPL
 **************************************************************/

#ifndef WIZ_H
#define WIZ_H

#if defined(__GNUG__) && !defined(__APPLE__)
	#pragma interface "wiz.h"
#endif
// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <cbplugin.h> // the base class we 're inheriting
#include <settings.h> // needed to use the Code::Blocks SDK
#include <wx/bitmap.h>

class wxWizard;
class wxWizardPageSimple;
class WizProjectPathPanel;
class WizCompilerPanel;
class WizLanguagePanel;

struct WizardInfo
{
    wxString title;
    wxString cat;
    wxString script;
    wxBitmap templatePNG;
    wxBitmap wizardPNG;
    wxString xrc;
};

WX_DECLARE_OBJARRAY(WizardInfo, Wizards);
WX_DEFINE_ARRAY(wxWizardPageSimple*, WizPages);

class Wiz : public cbProjectWizardPlugin
{
	public:
		Wiz();
		~Wiz();
		int Configure(){ return 0; }
		int GetCount() const;
		wxString GetTitle(int index) const;
		wxString GetDescription(int index) const;
		wxString GetCategory(int index) const;
		const wxBitmap& GetBitmap(int index) const;
		int Launch(int index);

        // Scripting support
        void AddWizard(const wxString& title,
                        const wxString& cat,
                        const wxString& script,
                        const wxString& templatePNG,
                        const wxString& wizardPNG,
                        const wxString& xrc);

        void CheckCheckbox(const wxString& name, bool check);
        bool IsCheckboxChecked(const wxString& name);

        void FillComboboxWithCompilers(const wxString& name);
        wxString GetCompilerFromCombobox(const wxString& name);

        wxString GetComboboxStringSelection(const wxString& name);
        int GetComboboxSelection(const wxString& name);
        void SetComboboxSelection(const wxString& name, int sel);

        int GetRadioboxSelection(const wxString& name);
        void SetRadioboxSelection(const wxString& name, int sel);

        void SetTextControlValue(const wxString& name, const wxString& value);
        wxString GetTextControlValue(const wxString& name);

        wxString GetProjectPath();
        wxString GetProjectName();
        wxString GetCompilerID();

        bool GetWantDebug();
        wxString GetDebugName();
        wxString GetDebugOutputDir();
        wxString GetDebugObjectOutputDir();

        bool GetWantRelease();
        wxString GetReleaseName();
        wxString GetReleaseOutputDir();
        wxString GetReleaseObjectOutputDir();

        int GetLanguageIndex() const;

        // pre-defined pages
        void AddIntroPage(const wxString& intro_msg);
        void AddProjectPathPage();
        void AddCompilerPage(const wxString& compilerID, const wxString& validCompilerIDs, bool allowCompilerChange = true, bool allowConfigChange = true);
        void AddLanguagePage(const wxString& langs, int defLang);
        // XRC pages
        void AddPage(const wxString& panelName);

        void Finalize();
        void RegisterWizard();
        const wxString& GetTemplatePath() { return m_TemplatePath; }
	protected:
        void OnAttach();
        void Clear();
        void CopyFiles(cbProject* theproject, const wxString&  prjdir, const wxString& srcdir);

        Wizards m_Wizards;
        wxWizard* m_pWizard;
        WizPages m_Pages;
        WizProjectPathPanel* m_pWizProjectPathPanel;
        WizCompilerPanel* m_pWizCompilerPanel;
        WizLanguagePanel* m_pWizLanguagePanel;
        int m_LaunchIndex;
        wxString m_TemplatePath;
        wxString m_LastXRC;
	private:
};

// Declare the plugin's hooks
CB_DECLARE_PLUGIN();

#endif // WIZ_H


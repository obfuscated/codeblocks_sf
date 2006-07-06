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
#include <cbexception.h>
#include <wx/bitmap.h>

class wxWizard;
class wxWizardPageSimple;
class WizProjectPathPanel;
class WizFilePathPanel;
class WizCompilerPanel;
class WizBuildTargetPanel;
class WizLanguagePanel;
class WizGenericSelectPathPanel;

struct WizardInfo
{
    TemplateOutputType output_type;
    wxString title;
    wxString cat;
    wxString script;
    wxBitmap templatePNG;
    wxBitmap wizardPNG;
    wxString xrc;
};

WX_DECLARE_OBJARRAY(WizardInfo, Wizards);
WX_DEFINE_ARRAY(wxWizardPageSimple*, WizPages);

class Wiz : public cbWizardPlugin
{
	public:
		Wiz();
		~Wiz();

        Wiz(const Wiz& rhs) { cbThrow(_T("Can't call Wiz's copy ctor!!!")); }
        virtual void operator=(const Wiz& rhs){ cbThrow(_T("Can't assign an Wiz* !!!")); }

		int Configure(){ return 0; }
		int GetCount() const;
        TemplateOutputType GetOutputType(int index) const;
		wxString GetTitle(int index) const;
		wxString GetDescription(int index) const;
		wxString GetCategory(int index) const;
		const wxBitmap& GetBitmap(int index) const;
        wxString GetScriptFilename(int index) const;
		CompileTargetBase* Launch(int index, wxString* pFilename = 0);

		CompileTargetBase* RunProjectWizard(wxString* pFilename); // called by Launch() for otProject wizards
		CompileTargetBase* RunTargetWizard(wxString* pFilename); // called by Launch() for otTarget wizards (always returns NULL)
		CompileTargetBase* RunFilesWizard(wxString* pFilename); // called by Launch() for otFiles wizards (always returns NULL)
		CompileTargetBase* RunCustomWizard(wxString* pFilename); // called by Launch() for otCustom wizards (always returns NULL)

        // Scripting support
        void AddWizard(TemplateOutputType otype,
                        const wxString& title,
                        const wxString& cat,
                        const wxString& script,
                        const wxString& templatePNG,
                        const wxString& wizardPNG,
                        const wxString& xrc);

        TemplateOutputType GetWizardType();

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

        // project path page
        wxString GetProjectPath();
        wxString GetProjectName();
        wxString GetProjectFullFilename();
        wxString GetProjectTitle();

        // compiler page
        wxString GetCompilerID();
        bool GetWantDebug();
        wxString GetDebugName();
        wxString GetDebugOutputDir();
        wxString GetDebugObjectOutputDir();
        bool GetWantRelease();
        wxString GetReleaseName();
        wxString GetReleaseOutputDir();
        wxString GetReleaseObjectOutputDir();

        // build target page
        wxString GetTargetCompilerID();
        bool GetTargetEnableDebug();
        wxString GetTargetName();
        wxString GetTargetOutputDir();
        wxString GetTargetObjectOutputDir();

        // language page
        int GetLanguageIndex();

        // file path page
        wxString GetFileName();
        wxString GetFileHeaderGuard();
        bool GetFileAddToProject();
        int GetFileTargetIndex();
        void SetFilePathSelectionFilter(const wxString& filter);

        // pre-defined pages
        void AddIntroPage(const wxString& intro_msg);
        void AddFilePathPage(bool showHeaderGuard);
        void AddProjectPathPage();
        void AddCompilerPage(const wxString& compilerID, const wxString& validCompilerIDs, bool allowCompilerChange = true, bool allowConfigChange = true);
        void AddBuildTargetPage(const wxString& targetName, bool isDebug, bool showCompiler = false, const wxString& compilerID = wxEmptyString, const wxString& validCompilerIDs = _T("*"), bool allowCompilerChange = true);
        void AddLanguagePage(const wxString& langs, int defLang);
        void AddGenericSelectPathPage(const wxString& pageId, const wxString& descr, const wxString& label, const wxString& defValue);
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
        WizFilePathPanel* m_pWizFilePathPanel;
        WizCompilerPanel* m_pWizCompilerPanel;
        WizBuildTargetPanel* m_pWizBuildTargetPanel;
        WizLanguagePanel* m_pWizLanguagePanel;
        int m_LaunchIndex;
        wxString m_TemplatePath;
        wxString m_LastXRC;
	private:
};

// Declare the plugin's hooks
CB_DECLARE_PLUGIN();

#endif // WIZ_H


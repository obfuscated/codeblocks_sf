/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

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
class WizGenericSingleChoiceList;
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

        void EnableWindow(const wxString& name, bool enable);

        void CheckCheckbox(const wxString& name, bool check);
        bool IsCheckboxChecked(const wxString& name);

        void FillComboboxWithCompilers(const wxString& name);
        wxString GetCompilerFromCombobox(const wxString& name);

        wxString GetComboboxStringSelection(const wxString& name);
        int GetComboboxSelection(const wxString& name);
        void SetComboboxSelection(const wxString& name, int sel);

        int GetRadioboxSelection(const wxString& name);
        void SetRadioboxSelection(const wxString& name, int sel);

        int GetListboxSelection(const wxString& name);
        wxString GetListboxSelections(const wxString& name);
        wxString GetListboxStringSelections(const wxString& name);
        void SetListboxSelection(const wxString& name, int sel);

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

        // file path page
        wxString GetFileName();
        wxString GetFileHeaderGuard();
        bool GetFileAddToProject();
        int GetFileTargetIndex();
        void SetFilePathSelectionFilter(const wxString& filter);

        // compiler defaults
        void SetCompilerDefault(const wxString& defCompilerID);
        void SetDebugTargetDefaults(bool wantDebug,
                                        const wxString& debugName,
                                        const wxString& debugOut,
                                        const wxString& debugObjOut);
        void SetReleaseTargetDefaults(bool wantRelease,
                                        const wxString& releaseName,
                                        const wxString& releaseOut,
                                        const wxString& releaseObjOut);

        // pre-defined pages
        void AddInfoPage(const wxString& pageId, const wxString& intro_msg);
        void AddFilePathPage(bool showHeaderGuard);
        void AddProjectPathPage();
        void AddCompilerPage(const wxString& compilerID, const wxString& validCompilerIDs, bool allowCompilerChange = true, bool allowConfigChange = true);
        void AddBuildTargetPage(const wxString& targetName, bool isDebug, bool showCompiler = false, const wxString& compilerID = wxEmptyString, const wxString& validCompilerIDs = _T("*"), bool allowCompilerChange = true);
        void AddGenericSingleChoiceListPage(const wxString& pageName, const wxString& descr, const wxString& choices, int defChoice);
        void AddGenericSelectPathPage(const wxString& pageId, const wxString& descr, const wxString& label, const wxString& defValue);
        // XRC pages
        void AddPage(const wxString& panelName);

        void Finalize();
        void RegisterWizard();
        wxString FindTemplateFile(const wxString& filename);
	protected:
        void OnAttach();
        void Clear();
        void CopyFiles(cbProject* theproject, const wxString&  prjdir, const wxString& srcdir);
        wxString GenerateFile(const wxString& basePath, const wxString& filename, const wxString& contents);

        Wizards m_Wizards;
        wxWizard* m_pWizard;
        WizPages m_Pages;
        WizProjectPathPanel* m_pWizProjectPathPanel;
        WizFilePathPanel* m_pWizFilePathPanel;
        WizCompilerPanel* m_pWizCompilerPanel;
        WizBuildTargetPanel* m_pWizBuildTargetPanel;
        int m_LaunchIndex;
        wxString m_LastXRC;

        // default compiler settings (returned if no compiler page is added in the wizard)
        wxString m_DefCompilerID;
        bool m_WantDebug;
        wxString m_DebugName;
        wxString m_DebugOutputDir;
        wxString m_DebugObjOutputDir;
        bool m_WantRelease;
        wxString m_ReleaseName;
        wxString m_ReleaseOutputDir;
        wxString m_ReleaseObjOutputDir;
	private:
};

#endif // WIZ_H


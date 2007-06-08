#ifndef WIZPAGE_H
#define WIZPAGE_H

#include <map>

#include <wx/string.h>
#include <wx/wizard.h>
#include <wx/panel.h>
#include <wx/xrc/xmlres.h>

class ProjectPathPanel;
class CompilerPanel;
class BuildTargetPanel;
class GenericSingleChoiceList;
class FilePathPanel;
class GenericSelectPath;
class WizPageBase;
class InfoPanel;

typedef std::map<wxString, WizPageBase*> PagesByName;

class WizPageBase : public wxWizardPageSimple
{
    public:
        WizPageBase(const wxString& pageName, wxWizard* parent, const wxBitmap& bitmap = wxNullBitmap);
        ~WizPageBase();

        virtual bool SkipPage(){ return m_SkipPage; } // default: false

        virtual void OnPageChanging(wxWizardEvent& event);
        virtual void OnPageChanged(wxWizardEvent& event);

        virtual wxWizardPage* GetPrev() const;
        virtual wxWizardPage* GetNext() const;
    protected:
        static PagesByName s_PagesByName;
        wxString m_PageName;
        bool m_SkipPage;
    private:
        DECLARE_EVENT_TABLE()
};

class WizPage : public WizPageBase
{
    public:
        WizPage(const wxString& panelName, wxWizard* parent, const wxBitmap& bitmap = wxNullBitmap);
        ~WizPage();
        void OnButton(wxCommandEvent& event);
    protected:
    private:
        DECLARE_EVENT_TABLE()
};

class WizInfoPanel : public WizPageBase
{
    public:
        WizInfoPanel(const wxString& pageId, const wxString& intro_msg, wxWizard* parent, const wxBitmap& bitmap = wxNullBitmap);
        ~WizInfoPanel();

        virtual void OnPageChanging(wxWizardEvent& event);
    protected:
        InfoPanel* m_InfoPanel;
    private:
};

class WizFilePathPanel : public WizPageBase
{
    public:
        WizFilePathPanel(bool showHeaderGuard, wxWizard* parent, const wxBitmap& bitmap = wxNullBitmap);
        ~WizFilePathPanel();

        wxString GetFilename(){ return m_Filename; }
        wxString GetHeaderGuard(){ return m_HeaderGuard; }
        bool GetAddToProject(){ return m_AddToProject; }
        int GetTargetIndex();
        void SetFilePathSelectionFilter(const wxString& filter);

        void OnPageChanging(wxWizardEvent& event);
    protected:
        FilePathPanel* m_pFilePathPanel;
        wxString m_Filename;
        wxString m_HeaderGuard;
        bool m_AddToProject;
        int m_TargetIndex;
    private:
};

class WizProjectPathPanel : public WizPageBase
{
    public:
        WizProjectPathPanel(wxWizard* parent, const wxBitmap& bitmap = wxNullBitmap);
        ~WizProjectPathPanel();

        wxString GetPath();
        wxString GetName();
        wxString GetFullFileName();
        wxString GetTitle();

        void OnButton(wxCommandEvent& event);
        virtual void OnPageChanging(wxWizardEvent& event);
        virtual void OnPageChanged(wxWizardEvent& event);
    protected:
        ProjectPathPanel* m_pProjectPathPanel;
    private:
        DECLARE_EVENT_TABLE()
};

class WizGenericSelectPathPanel : public WizPageBase
{
    public:
        WizGenericSelectPathPanel(const wxString& pageId, const wxString& descr, const wxString& label, const wxString& defValue,
                            wxWizard* parent, const wxBitmap& bitmap = wxNullBitmap);
        ~WizGenericSelectPathPanel();

        void OnButton(wxCommandEvent& event);
        virtual void OnPageChanging(wxWizardEvent& event);
    protected:
        GenericSelectPath* m_pGenericSelectPath;
    private:
        DECLARE_EVENT_TABLE()
};

class WizCompilerPanel : public WizPageBase
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
        bool m_AllowConfigChange;
    private:
};

class WizBuildTargetPanel : public WizPageBase
{
    public:
        WizBuildTargetPanel(const wxString& targetName, bool isDebug,
                        wxWizard* parent, const wxBitmap& bitmap = wxNullBitmap,
                        bool showCompiler = false,
                        const wxString& compilerID = wxEmptyString, const wxString& validCompilerIDs = _T("*"),
                        bool allowCompilerChange = true);
        ~WizBuildTargetPanel();

        wxString GetCompilerID();
        bool GetEnableDebug();
        wxString GetTargetName();
        wxString GetTargetOutputDir();
        wxString GetTargetObjectOutputDir();

        void OnPageChanging(wxWizardEvent& event);
    protected:
        BuildTargetPanel* m_pBuildTargetPanel;
    private:
};

class WizGenericSingleChoiceList : public WizPageBase
{
    public:
        WizGenericSingleChoiceList(const wxString& pageId, const wxString& descr, const wxArrayString& choices, int defChoice, wxWizard* parent, const wxBitmap& bitmap = wxNullBitmap);
        ~WizGenericSingleChoiceList();

        int GetChoice() const;
        void SetChoice(int choice);

        void OnPageChanging(wxWizardEvent& event);
    protected:
        GenericSingleChoiceList* m_pGenericSingleChoiceList;
    private:
};

#endif // WIZPAGE_H

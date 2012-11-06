/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

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

        virtual bool SkipPage() const { return m_SkipPage; } // default: false

        virtual void OnPageChanging(wxWizardEvent& event);
        virtual void OnPageChanged(wxWizardEvent& event);

        virtual wxWizardPage* GetPrev() const;
        virtual wxWizardPage* GetNext() const;
        wxString GetPageName() const {return m_PageName;}
        bool GetSkipPage() const {return m_SkipPage;}
        void SetSkipPage(bool skip) {m_SkipPage = skip;}

    private:
        static PagesByName s_PagesByName;
        wxString m_PageName;
        bool m_SkipPage;

        DECLARE_EVENT_TABLE()
};

class WizPage : public WizPageBase
{
    public:
        WizPage(const wxString& panelName, wxWizard* parent, const wxBitmap& bitmap = wxNullBitmap);
        ~WizPage();
        void OnButton(wxCommandEvent& event);
    private:
        DECLARE_EVENT_TABLE()
};

class WizInfoPanel : public WizPageBase
{
    public:
        WizInfoPanel(const wxString& pageId, const wxString& intro_msg, wxWizard* parent, const wxBitmap& bitmap = wxNullBitmap);
        ~WizInfoPanel();

        virtual void OnPageChanging(wxWizardEvent& event);
    private:
        InfoPanel* m_InfoPanel;
};

class WizFilePathPanel : public WizPageBase
{
    public:
        WizFilePathPanel(bool showHeaderGuard, wxWizard* parent, const wxBitmap& bitmap = wxNullBitmap);
        ~WizFilePathPanel();

        wxString GetFilename() const { return m_Filename; }
        wxString GetHeaderGuard() const { return m_HeaderGuard; }
        bool GetAddToProject() const { return m_AddToProject; }
        int GetTargetIndex() const;
        void SetFilePathSelectionFilter(const wxString& filter);

        void OnPageChanging(wxWizardEvent& event);
    private:
        FilePathPanel* m_pFilePathPanel;
        wxString m_Filename;
        wxString m_HeaderGuard;
        bool m_AddToProject;
        int m_TargetIndex;
};

class WizProjectPathPanel : public WizPageBase
{
    public:
        WizProjectPathPanel(wxWizard* parent, const wxBitmap& bitmap = wxNullBitmap);
        ~WizProjectPathPanel();

        wxString GetPath() const;
        wxString GetName() const;
        wxString GetFullFileName() const;
        wxString GetTitle() const;

        void OnButton(wxCommandEvent& event);
        virtual void OnPageChanging(wxWizardEvent& event);
        virtual void OnPageChanged(wxWizardEvent& event);
    private:
        ProjectPathPanel* m_pProjectPathPanel;

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
    private:
        GenericSelectPath* m_pGenericSelectPath;

        DECLARE_EVENT_TABLE()
};

class WizCompilerPanel : public WizPageBase
{
    public:
        WizCompilerPanel(const wxString& compilerID, const wxString& validCompilerIDs, wxWizard* parent, const wxBitmap& bitmap = wxNullBitmap,
                        bool allowCompilerChange = true, bool allowConfigChange = true);
        ~WizCompilerPanel();

        wxString GetCompilerID() const;
        bool GetWantDebug() const;
        wxString GetDebugName() const;
        wxString GetDebugOutputDir() const;
        wxString GetDebugObjectOutputDir() const;
        bool GetWantRelease() const;
        wxString GetReleaseName() const;
        wxString GetReleaseOutputDir() const;
        wxString GetReleaseObjectOutputDir() const;

        void OnPageChanging(wxWizardEvent& event);
    private:
        CompilerPanel* m_pCompilerPanel;
        bool m_AllowConfigChange;
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

        wxString GetCompilerID() const;
        bool GetEnableDebug() const;
        wxString GetTargetName() const;
        wxString GetTargetOutputDir() const;
        wxString GetTargetObjectOutputDir() const;

        void OnPageChanging(wxWizardEvent& event);
    private:
        BuildTargetPanel* m_pBuildTargetPanel;
};

class WizGenericSingleChoiceList : public WizPageBase
{
    public:
        WizGenericSingleChoiceList(const wxString& pageId, const wxString& descr, const wxArrayString& choices, int defChoice, wxWizard* parent, const wxBitmap& bitmap = wxNullBitmap);
        ~WizGenericSingleChoiceList();

        int GetChoice() const;
        void SetChoice(int choice);

        void OnPageChanging(wxWizardEvent& event);
    private:
        GenericSingleChoiceList* m_pGenericSingleChoiceList;
};

#endif // WIZPAGE_H

#ifndef VALGRIND_CONFIG_H
#define VALGRIND_CONFIG_H

//(*Headers(ValgrindConfigurationPanel)
#include <wx/panel.h>
class wxTextCtrl;
class wxStaticBoxSizer;
class wxButton;
class wxBoxSizer;
class wxStaticText;
class wxCheckBox;
//*)

#include <configurationpanel.h>

class ValgrindConfigurationPanel: public cbConfigurationPanel
{
    public:

        ValgrindConfigurationPanel(wxWindow *parent);
        virtual ~ValgrindConfigurationPanel();

        virtual wxString GetTitle() const { return _("Valgrind settings"); }
        virtual wxString GetBitmapBaseName() const { return _T("valgrind"); }
        virtual void OnApply();
        virtual void OnCancel() {}

    private:
        void LoadSettings();
    private:

        //(*Declarations(ValgrindConfigurationPanel)
        wxTextCtrl* m_CachegrindArgs;
        wxTextCtrl* m_MemCheckArgs;
        wxCheckBox* m_ShowReachable;
        wxCheckBox* m_FullMemCheck;
        wxTextCtrl* m_ExecutablePath;
        wxCheckBox* m_TrackOrigins;
        //*)

        //(*Identifiers(ValgrindConfigurationPanel)
        static const long IdExecutablePath;
        static const long IdBrowseButton;
        static const long IdMemCheckArgs;
        static const long IdFullMemCheck;
        static const long IdTrackOrigins;
        static const long IdShowReachable;
        static const long IdCachegrindArgs;
        //*)

        //(*Handlers(ValgrindConfigurationPanel)
        void OnBrowseButtonClick(wxCommandEvent& event);
        //*)

    private:

        void BuildContent(wxWindow *parent);

        DECLARE_EVENT_TABLE()
};

#endif

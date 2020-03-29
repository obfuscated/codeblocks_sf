#ifndef CONFIGPANEL_H
#define CONFIGPANEL_H

//(*Headers(ConfigPanel)
#include <wx/panel.h>
class wxHyperlinkCtrl;
class wxTextCtrl;
class wxStaticLine;
class wxStaticText;
class wxFlexGridSizer;
class wxBoxSizer;
class wxButton;
class wxChoice;
//*)

#include <wx/string.h>

#include <configurationpanel.h>

class CppCheck;

class ConfigPanel : public cbConfigurationPanel
{
public:

    ConfigPanel(wxWindow* parent);
    virtual ~ConfigPanel();

    //(*Declarations(ConfigPanel)
    wxTextCtrl* txtCppCheckApp;
    wxTextCtrl* txtCppCheckArgs;
    wxChoice* choOperation;
    wxStaticLine* StaticLine2;
    wxStaticLine* StaticLine1;
    wxTextCtrl* txtVeraArgs;
    wxTextCtrl* txtVeraApp;
    //*)

    /// @return the panel's title.
    virtual wxString GetTitle() const { return _("CppCheck/Vera++"); }
    /// @return the panel's bitmap base name. You must supply two bitmaps: \<basename\>.png and \<basename\>-off.png...
    virtual wxString GetBitmapBaseName() const { return wxT("CppCheck"); }
    /// Called when the user chooses to apply the configuration.
    virtual void OnApply();
    /// Called when the user chooses to cancel the configuration.
    virtual void OnCancel() { ; }

    static wxString GetDefaultCppCheckExecutableName();
    static wxString GetDefaultVeraExecutableName();
protected:

    //(*Identifiers(ConfigPanel)
    static const long ID_TXT_CPP_CHECK_APP;
    static const long ID_BTN_CPPCHECK_APP;
    static const long ID_TXT_CPP_CHECK_ARGS;
    static const long ID_HYC_CPP_CHECK_WWW;
    static const long ID_TXT_VERA_APP;
    static const long ID_BTN_VERA;
    static const long ID_TXT_VERA_ARGS;
    static const long ID_HYC_VERA_WWW;
    static const long ID_STATICLINE1;
    static const long ID_STATICLINE2;
    static const long ID_CHO_OPERATION;
    //*)

private:

    //(*Handlers(ConfigPanel)
    void OnCppCheckApp(wxCommandEvent& event);
    void OnVeraApp(wxCommandEvent& event);
    //*)

    DECLARE_EVENT_TABLE()
};

#endif

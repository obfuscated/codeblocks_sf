#ifndef CONFIGPANEL_H
#define CONFIGPANEL_H

//(*Headers(ConfigPanel)
#include <wx/panel.h>
class wxTextCtrl;
class wxFlexGridSizer;
class wxButton;
class wxBoxSizer;
class wxStaticText;
//*)

#include <wx/string.h>

#include <configurationpanel.h>

class CppCheck;

class ConfigPanel : public cbConfigurationPanel
{
public:

    ConfigPanel(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
    virtual ~ConfigPanel();

    //(*Declarations(ConfigPanel)
    wxTextCtrl* txtCppCheckArgs;
    wxTextCtrl* txtCppCheckApp;
    //*)

    /// @return the panel's title.
    virtual wxString GetTitle() const  { return _("CppCheck"); }
    /// @return the panel's bitmap base name. You must supply two bitmaps: \<basename\>.png and \<basename\>-off.png...
    virtual wxString GetBitmapBaseName() const { return wxT("CppCheck"); }
    /// Called when the user chooses to apply the configuration.
    virtual void OnApply();
    /// Called when the user chooses to cancel the configuration.
    virtual void OnCancel() { ; }

    static wxString GetDefaultExecutableName();
protected:

    //(*Identifiers(ConfigPanel)
    static const long ID_TXT_CPP_CHECK_APP;
    static const long ID_BTN_CPPCHECK_APP;
    static const long ID_TXT_CPP_CHECK_ARGS;
    //*)

private:

    //(*Handlers(ConfigPanel)
    void OnCppCheckApp(wxCommandEvent& event);
    //*)


    DECLARE_EVENT_TABLE()
};

#endif

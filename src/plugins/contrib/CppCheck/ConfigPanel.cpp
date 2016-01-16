#include "ConfigPanel.h"

#include <wx/filedlg.h>

#include <manager.h>
#include <configmanager.h>

//(*InternalHeaders(ConfigPanel)
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/string.h>
#include <wx/intl.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

//(*IdInit(ConfigPanel)
const long ConfigPanel::ID_TXT_CPP_CHECK_APP = wxNewId();
const long ConfigPanel::ID_BTN_CPPCHECK_APP = wxNewId();
const long ConfigPanel::ID_TXT_CPP_CHECK_ARGS = wxNewId();
//*)

BEGIN_EVENT_TABLE(ConfigPanel,wxPanel)
    //(*EventTable(ConfigPanel)
    //*)
END_EVENT_TABLE()

ConfigPanel::ConfigPanel(wxWindow* parent,wxWindowID id,const wxPoint& /*pos*/,const wxSize& /*size*/)
{
    //(*Initialize(ConfigPanel)
    wxFlexGridSizer* flsMain;
    wxStaticText* lblCppCheckArgs;
    wxButton* btnCppCheckApp;
    wxBoxSizer* bszMain;
    wxBoxSizer* bszCppCheckApp;
    wxStaticText* lblCppCheckArgsComment;
    wxStaticText* lblCppCheckApp;

    Create(parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("id"));
    bszMain = new wxBoxSizer(wxHORIZONTAL);
    flsMain = new wxFlexGridSizer(0, 2, 0, 0);
    flsMain->AddGrowableCol(1);
    lblCppCheckApp = new wxStaticText(this, wxID_ANY, _("CppCheck application:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    flsMain->Add(lblCppCheckApp, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    bszCppCheckApp = new wxBoxSizer(wxHORIZONTAL);
    txtCppCheckApp = new wxTextCtrl(this, ID_TXT_CPP_CHECK_APP, _("cppcheck"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TXT_CPP_CHECK_APP"));
    bszCppCheckApp->Add(txtCppCheckApp, 1, wxLEFT|wxEXPAND, 5);
    btnCppCheckApp = new wxButton(this, ID_BTN_CPPCHECK_APP, _("..."), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BTN_CPPCHECK_APP"));
    btnCppCheckApp->SetMinSize(wxSize(30,-1));
    bszCppCheckApp->Add(btnCppCheckApp, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    flsMain->Add(bszCppCheckApp, 1, wxEXPAND, 5);
    lblCppCheckArgs = new wxStaticText(this, wxID_ANY, _("CppCheck arguments:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    flsMain->Add(lblCppCheckArgs, 0, wxTOP|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    txtCppCheckArgs = new wxTextCtrl(this, ID_TXT_CPP_CHECK_ARGS, _("--verbose --enable=all --enable=style --xml"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TXT_CPP_CHECK_ARGS"));
    flsMain->Add(txtCppCheckArgs, 1, wxTOP|wxLEFT|wxEXPAND, 5);
    lblCppCheckArgsComment = new wxStaticText(this, wxID_ANY, _("(before \"--file-list\")"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    flsMain->Add(lblCppCheckArgsComment, 0, wxLEFT|wxALIGN_TOP, 5);
    flsMain->Add(-1,-1,1, wxEXPAND, 5);
    bszMain->Add(flsMain, 1, wxEXPAND, 5);
    SetSizer(bszMain);
    bszMain->Fit(this);
    bszMain->SetSizeHints(this);

    Connect(ID_BTN_CPPCHECK_APP,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ConfigPanel::OnCppCheckApp);
    //*)

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("cppcheck"));
    if (cfg)
    {
        txtCppCheckApp->SetValue( cfg->Read(_T("cppcheck_app"),
#ifdef __WXMSW__
                                  _T("cppcheck.exe")) );
#else
                                  _T("cppcheck")) );
#endif
        txtCppCheckArgs->SetValue( cfg->Read(_T("cppcheck_args"),
                                   _T("--verbose --enable=all --enable=style --xml")) );
    }
}

ConfigPanel::~ConfigPanel()
{
    //(*Destroy(ConfigPanel)
    //*)
}

void ConfigPanel::OnCppCheckApp(wxCommandEvent& /*event*/)
{
    wxFileName initialFile(txtCppCheckApp->GetValue());
    wxFileDialog dialog (this, _("Select CppCheck application"),
                         initialFile.GetPath(),
                         GetDefaultExecutableName(),
#ifdef __WXMSW__
                         _("Executable files (*.exe)|*.exe"),
#else
                         _("Executable files (*)|*"),
#endif
                        wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (dialog.ShowModal() == wxID_OK)
        txtCppCheckApp->SetValue(dialog.GetPath());
}

void ConfigPanel::OnApply()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("cppcheck"));
    if (cfg)
    {
        wxString app = txtCppCheckApp->GetValue();
        if (!app.IsEmpty())
            cfg->Write(_T("cppcheck_app"), app);
        if (!txtCppCheckArgs->GetValue().IsEmpty())
            cfg->Write(_T("cppcheck_args"), txtCppCheckArgs->GetValue());
    }
}

wxString ConfigPanel::GetDefaultExecutableName()
{
#ifdef __WXMSW__
    return _T("cppcheck.exe");
#else
    return _T("cppcheck");
#endif
}

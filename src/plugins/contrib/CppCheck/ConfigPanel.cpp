#include "ConfigPanel.h"

#include <wx/filedlg.h>

#include <manager.h>
#include <configmanager.h>

//(*InternalHeaders(ConfigPanel)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/statline.h>
#include <wx/hyperlink.h>
#include <wx/choice.h>
#include <wx/intl.h>
#include <wx/button.h>
#include <wx/string.h>
//*)

//(*IdInit(ConfigPanel)
const long ConfigPanel::ID_TXT_CPP_CHECK_APP = wxNewId();
const long ConfigPanel::ID_BTN_CPPCHECK_APP = wxNewId();
const long ConfigPanel::ID_TXT_CPP_CHECK_ARGS = wxNewId();
const long ConfigPanel::ID_HYC_CPP_CHECK_WWW = wxNewId();
const long ConfigPanel::ID_TXT_VERA_APP = wxNewId();
const long ConfigPanel::ID_BTN_VERA = wxNewId();
const long ConfigPanel::ID_TXT_VERA_ARGS = wxNewId();
const long ConfigPanel::ID_HYC_VERA_WWW = wxNewId();
const long ConfigPanel::ID_STATICLINE1 = wxNewId();
const long ConfigPanel::ID_STATICLINE2 = wxNewId();
const long ConfigPanel::ID_CHO_OPERATION = wxNewId();
//*)

BEGIN_EVENT_TABLE(ConfigPanel,wxPanel)
    //(*EventTable(ConfigPanel)
    //*)
END_EVENT_TABLE()

ConfigPanel::ConfigPanel(wxWindow* parent)
{
    //(*Initialize(ConfigPanel)
    wxBoxSizer* bszMain;
    wxStaticText* lblOperation;
    wxButton* btnVeraApp;
    wxStaticText* lblCppCheckWWW;
    wxHyperlinkCtrl* hycVeraWWW;
    wxStaticText* lblCppCheckArgsComment;
    wxHyperlinkCtrl* hycCppCheckWWW;
    wxBoxSizer* bszVeraApp;
    wxStaticLine* slSeparatorRight;
    wxFlexGridSizer* flsMain;
    wxStaticLine* slSeparatorLeft;
    wxStaticText* lblVeraArgsComment;
    wxButton* btnCppCheckApp;
    wxBoxSizer* bszCppCheckApp;
    wxStaticText* lblVeraArgs;
    wxStaticText* lblCppCheckApp;
    wxStaticText* lblVeraWWW;
    wxStaticText* lblVeraApp;
    wxStaticText* lblCppCheckArgs;

    Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("wxID_ANY"));
    bszMain = new wxBoxSizer(wxHORIZONTAL);
    flsMain = new wxFlexGridSizer(0, 2, 0, 0);
    flsMain->AddGrowableCol(1);
    lblCppCheckApp = new wxStaticText(this, wxID_ANY, _("CppCheck application:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    flsMain->Add(lblCppCheckApp, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    bszCppCheckApp = new wxBoxSizer(wxHORIZONTAL);
    txtCppCheckApp = new wxTextCtrl(this, ID_TXT_CPP_CHECK_APP, _("cppcheck"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TXT_CPP_CHECK_APP"));
    bszCppCheckApp->Add(txtCppCheckApp, 1, wxEXPAND, 5);
    btnCppCheckApp = new wxButton(this, ID_BTN_CPPCHECK_APP, _("..."), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BTN_CPPCHECK_APP"));
    btnCppCheckApp->SetMinSize(wxSize(30,-1));
    bszCppCheckApp->Add(btnCppCheckApp, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    flsMain->Add(bszCppCheckApp, 1, wxLEFT|wxEXPAND, 5);
    lblCppCheckArgs = new wxStaticText(this, wxID_ANY, _("CppCheck arguments:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    flsMain->Add(lblCppCheckArgs, 0, wxTOP|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    txtCppCheckArgs = new wxTextCtrl(this, ID_TXT_CPP_CHECK_ARGS, _("--verbose --enable=all --enable=style --xml"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TXT_CPP_CHECK_ARGS"));
    txtCppCheckArgs->SetMinSize(wxSize(240,-1));
    flsMain->Add(txtCppCheckArgs, 1, wxTOP|wxLEFT|wxEXPAND, 5);
    lblCppCheckArgsComment = new wxStaticText(this, wxID_ANY, _("(before \"--file-list\")"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    flsMain->Add(lblCppCheckArgsComment, 0, wxLEFT|wxALIGN_TOP, 5);
    flsMain->Add(-1,-1,1, wxLEFT|wxEXPAND, 5);
    lblCppCheckWWW = new wxStaticText(this, wxID_ANY, _("CppCheck homepage:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    flsMain->Add(lblCppCheckWWW, 0, wxTOP|wxLEFT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    hycCppCheckWWW = new wxHyperlinkCtrl(this, ID_HYC_CPP_CHECK_WWW, _("http://cppcheck.sourceforge.net"), wxEmptyString, wxDefaultPosition, wxDefaultSize, wxHL_CONTEXTMENU|wxHL_ALIGN_CENTRE|wxNO_BORDER, _T("ID_HYC_CPP_CHECK_WWW"));
    flsMain->Add(hycCppCheckWWW, 1, wxTOP|wxLEFT|wxEXPAND, 5);
    slSeparatorLeft = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(10,-1), wxLI_HORIZONTAL, _T("wxID_ANY"));
    flsMain->Add(slSeparatorLeft, 0, wxTOP|wxBOTTOM|wxLEFT|wxEXPAND, 5);
    slSeparatorRight = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(10,-1), wxLI_HORIZONTAL, _T("wxID_ANY"));
    flsMain->Add(slSeparatorRight, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
    lblVeraApp = new wxStaticText(this, wxID_ANY, _("Vera++ application:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    flsMain->Add(lblVeraApp, 0, wxLEFT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    bszVeraApp = new wxBoxSizer(wxHORIZONTAL);
    txtVeraApp = new wxTextCtrl(this, ID_TXT_VERA_APP, _("vera++"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TXT_VERA_APP"));
    bszVeraApp->Add(txtVeraApp, 1, wxEXPAND, 5);
    btnVeraApp = new wxButton(this, ID_BTN_VERA, _("..."), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BTN_VERA"));
    btnVeraApp->SetMinSize(wxSize(30,-1));
    bszVeraApp->Add(btnVeraApp, 0, wxLEFT|wxALIGN_TOP, 5);
    flsMain->Add(bszVeraApp, 1, wxLEFT|wxEXPAND, 5);
    lblVeraArgs = new wxStaticText(this, wxID_ANY, _("Vera++ arguments:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    flsMain->Add(lblVeraArgs, 0, wxTOP|wxLEFT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    txtVeraArgs = new wxTextCtrl(this, ID_TXT_VERA_ARGS, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TXT_VERA_ARGS"));
    flsMain->Add(txtVeraArgs, 1, wxTOP|wxLEFT|wxEXPAND, 5);
    lblVeraArgsComment = new wxStaticText(this, wxID_ANY, _("(before file list)"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    flsMain->Add(lblVeraArgsComment, 0, wxLEFT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    flsMain->Add(-1,-1,1, wxLEFT|wxEXPAND, 5);
    lblVeraWWW = new wxStaticText(this, wxID_ANY, _("Vera++  homepage:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    flsMain->Add(lblVeraWWW, 0, wxTOP|wxLEFT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    hycVeraWWW = new wxHyperlinkCtrl(this, ID_HYC_VERA_WWW, _("https://bitbucket.org/verateam/vera"), wxEmptyString, wxDefaultPosition, wxDefaultSize, wxHL_CONTEXTMENU|wxHL_ALIGN_CENTRE|wxNO_BORDER, _T("ID_HYC_VERA_WWW"));
    flsMain->Add(hycVeraWWW, 1, wxTOP|wxLEFT|wxEXPAND, 5);
    StaticLine1 = new wxStaticLine(this, ID_STATICLINE1, wxDefaultPosition, wxSize(10,-1), wxLI_HORIZONTAL, _T("ID_STATICLINE1"));
    flsMain->Add(StaticLine1, 0, wxTOP|wxBOTTOM|wxLEFT|wxEXPAND, 5);
    StaticLine2 = new wxStaticLine(this, ID_STATICLINE2, wxDefaultPosition, wxSize(10,-1), wxLI_HORIZONTAL, _T("ID_STATICLINE2"));
    flsMain->Add(StaticLine2, 1, wxTOP|wxBOTTOM|wxEXPAND, 5);
    lblOperation = new wxStaticText(this, wxID_ANY, _("Select operation:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    flsMain->Add(lblOperation, 0, wxLEFT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    choOperation = new wxChoice(this, ID_CHO_OPERATION, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHO_OPERATION"));
    choOperation->SetSelection( choOperation->Append(_("CppCheck only")) );
    choOperation->Append(_("Vera++ only"));
    choOperation->Append(_("CppCheck and Vera++"));
    flsMain->Add(choOperation, 1, wxLEFT|wxEXPAND, 5);
    bszMain->Add(flsMain, 1, wxEXPAND, 5);
    SetSizer(bszMain);
    bszMain->Fit(this);
    bszMain->SetSizeHints(this);

    Connect(ID_BTN_CPPCHECK_APP,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(ConfigPanel::OnCppCheckApp));
    Connect(ID_BTN_VERA,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(ConfigPanel::OnVeraApp));
    //*)

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("cppcheck"));
    if (cfg)
    {
        txtCppCheckApp->SetValue( cfg->Read(_T("cppcheck_app"),
                                  GetDefaultCppCheckExecutableName()) );
        txtCppCheckArgs->SetValue( cfg->Read(_T("cppcheck_args"),
                                   _T("--verbose --enable=all --enable=style --xml")) );

        txtVeraApp->SetValue( cfg->Read(_T("vera_app"),
                                  GetDefaultVeraExecutableName()) );
        txtVeraArgs->SetValue( cfg->Read(_T("vera_args"), wxEmptyString) );

        choOperation->SetSelection( cfg->ReadInt(_T("operation"), 0) );
    }
}

ConfigPanel::~ConfigPanel()
{
    //(*Destroy(ConfigPanel)
    //*)
}

void ConfigPanel::OnCppCheckApp(cb_unused wxCommandEvent& event)
{
    wxFileName initialFile(txtCppCheckApp->GetValue());
    wxFileDialog dialog (this, _("Select CppCheck application"),
                         initialFile.GetPath(),
                         GetDefaultCppCheckExecutableName(),
#ifdef __WXMSW__
                         _("Executable files (*.exe)|*.exe"),
#else
                         _("Executable files (*)|*"),
#endif
                        wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (dialog.ShowModal() == wxID_OK)
        txtCppCheckApp->SetValue(dialog.GetPath());
}


void ConfigPanel::OnVeraApp(cb_unused wxCommandEvent& event)
{
    wxFileName initialFile(txtVeraApp->GetValue());
    wxFileDialog dialog (this, _("Select Vera++ application"),
                         initialFile.GetPath(),
                         GetDefaultVeraExecutableName(),
#ifdef __WXMSW__
                         _("Executable files (*.exe)|*.exe"),
#else
                         _("Executable files (*)|*"),
#endif
                        wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (dialog.ShowModal() == wxID_OK)
        txtVeraApp->SetValue(dialog.GetPath());
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

        app = txtVeraApp->GetValue();
        if (!app.IsEmpty())
            cfg->Write(_T("vera_app"), app);
        if (!txtVeraArgs->GetValue().IsEmpty())
            cfg->Write(_T("vera_args"), txtVeraArgs->GetValue());

        cfg->Write(_T("operation"), choOperation->GetSelection());
    }
}

wxString ConfigPanel::GetDefaultCppCheckExecutableName()
{
#ifdef __WXMSW__
    return _T("cppcheck.exe");
#else
    return _T("cppcheck");
#endif
}

wxString ConfigPanel::GetDefaultVeraExecutableName()
{
#ifdef __WXMSW__
    return _T("vera++.exe");
#else
    return _T("vera++");
#endif
}


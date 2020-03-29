#include "CscopeConfigPanel.h"
#include "configmanager.h"
#include <wx/filedlg.h>

//(*InternalHeaders(CscopeConfigPanel)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(CscopeConfigPanel)
const long CscopeConfigPanel::ID_TXT_CPP_CHECK_APP = wxNewId();
const long CscopeConfigPanel::ID_BTN_CPPCHECK_APP = wxNewId();
const long CscopeConfigPanel::ID_HYC_CPP_CHECK_WWW = wxNewId();
//*)

BEGIN_EVENT_TABLE(CscopeConfigPanel,wxPanel)
	//(*EventTable(CscopeConfigPanel)
	//*)
END_EVENT_TABLE()

CscopeConfigPanel::CscopeConfigPanel(wxWindow* parent)
{
	//(*Initialize(CscopeConfigPanel)
	wxBoxSizer* bszCppCheckApp;
	wxBoxSizer* bszMain;
	wxButton* btnCscopeApp;
	wxFlexGridSizer* flsMain;
	wxHyperlinkCtrl* hycCscopeWWW;
	wxStaticText* lblCscopeApp;
	wxStaticText* lblCscopeWWW;

	Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("wxID_ANY"));
	bszMain = new wxBoxSizer(wxHORIZONTAL);
	flsMain = new wxFlexGridSizer(0, 2, 0, 0);
	flsMain->AddGrowableCol(1);
	lblCscopeApp = new wxStaticText(this, wxID_ANY, _("Cscope application:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
	flsMain->Add(lblCscopeApp, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
	bszCppCheckApp = new wxBoxSizer(wxHORIZONTAL);
	txtCscopeApp = new wxTextCtrl(this, ID_TXT_CPP_CHECK_APP, _("cscope"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TXT_CPP_CHECK_APP"));
	bszCppCheckApp->Add(txtCscopeApp, 1, wxEXPAND, 5);
	btnCscopeApp = new wxButton(this, ID_BTN_CPPCHECK_APP, _("..."), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BTN_CPPCHECK_APP"));
	btnCscopeApp->SetMinSize(wxSize(30,-1));
	bszCppCheckApp->Add(btnCscopeApp, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
	flsMain->Add(bszCppCheckApp, 1, wxLEFT|wxEXPAND, 5);
	lblCscopeWWW = new wxStaticText(this, wxID_ANY, _("CScope Install instruction"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
	flsMain->Add(lblCscopeWWW, 0, wxTOP|wxLEFT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	hycCscopeWWW = new wxHyperlinkCtrl(this, ID_HYC_CPP_CHECK_WWW, _("http://wiki.codeblocks.org/index.php/Cscope_plugin"), wxEmptyString, wxDefaultPosition, wxDefaultSize, wxHL_CONTEXTMENU|wxHL_ALIGN_CENTRE, _T("ID_HYC_CPP_CHECK_WWW"));
	flsMain->Add(hycCscopeWWW, 1, wxTOP|wxLEFT|wxEXPAND, 5);
	bszMain->Add(flsMain, 1, wxEXPAND, 5);
	SetSizer(bszMain);
	bszMain->Fit(this);
	bszMain->SetSizeHints(this);

	Connect(ID_BTN_CPPCHECK_APP,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CscopeConfigPanel::OnbtnCscopeAppClick);
	//*)


    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("cscope"));
    if (cfg)
    {
        txtCscopeApp->SetValue( cfg->Read(_T("cscope_app"),
                                  GetDefaultCscopeExecutableName()) );
    }
}

CscopeConfigPanel::~CscopeConfigPanel()
{
	//(*Destroy(CscopeConfigPanel)
	//*)
}

void CscopeConfigPanel::OnApply()
{
ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("cscope"));
    if (cfg)
    {
        wxString app = txtCscopeApp->GetValue();
        if (!app.IsEmpty())
            cfg->Write(_T("cscope_app"), app);
    }
}


void CscopeConfigPanel::OnbtnCscopeAppClick(wxCommandEvent& event)
{
    wxFileName initialFile(txtCscopeApp->GetValue());
    wxFileDialog dialog (this, _("Select CScope application"),
                         initialFile.GetPath(),
                         GetDefaultCscopeExecutableName(),
#ifdef __WXMSW__
                         _("Executable files (*.exe)|*.exe"),
#else
                         _("Executable files (*)|*"),
#endif
                        wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (dialog.ShowModal() == wxID_OK)
        txtCscopeApp->SetValue(dialog.GetPath());
}

wxString CscopeConfigPanel::GetDefaultCscopeExecutableName()
{
#ifdef __WXMSW__
    return _T("cscope.exe");
#else
    return _T("cscope");
#endif
}

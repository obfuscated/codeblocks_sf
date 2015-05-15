#include "valgrind_config.h"

//(*InternalHeaders(ValgrindConfigurationPanel)
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/string.h>
#include <wx/intl.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)
#include <wx/filedlg.h>

#include <configmanager.h>

//(*IdInit(ValgrindConfigurationPanel)
const long ValgrindConfigurationPanel::IdExecutablePath = wxNewId();
const long ValgrindConfigurationPanel::IdBrowseButton = wxNewId();
const long ValgrindConfigurationPanel::IdMemCheckArgs = wxNewId();
const long ValgrindConfigurationPanel::IdFullMemCheck = wxNewId();
const long ValgrindConfigurationPanel::IdTrackOrigins = wxNewId();
const long ValgrindConfigurationPanel::IdShowReachable = wxNewId();
const long ValgrindConfigurationPanel::IdCachegrindArgs = wxNewId();
//*)

BEGIN_EVENT_TABLE(ValgrindConfigurationPanel,cbConfigurationPanel)
	//(*EventTable(ValgrindConfigurationPanel)
	//*)
END_EVENT_TABLE()

ValgrindConfigurationPanel::ValgrindConfigurationPanel(wxWindow *parent)
{
	BuildContent(parent);
	LoadSettings();
}

void ValgrindConfigurationPanel::BuildContent(wxWindow *parent)
{
	//(*Initialize(ValgrindConfigurationPanel)
	wxStaticText* CacheGrindArgs;
	wxButton* BrowseButton;
	wxStaticText* MemCheckArgs;
	wxStaticText* StaticText1;
	wxBoxSizer* MemCheckArgSizer;
	wxStaticBoxSizer* MemCheckOptSizer;
	wxBoxSizer* MainSizer;
	wxStaticBoxSizer* CachegrindOptSizer;
	wxBoxSizer* ExecSizer;

	Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("wxID_ANY"));
	MainSizer = new wxBoxSizer(wxVERTICAL);
	ExecSizer = new wxBoxSizer(wxHORIZONTAL);
	StaticText1 = new wxStaticText(this, wxID_ANY, _("Executable:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
	ExecSizer->Add(StaticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
	m_ExecutablePath = new wxTextCtrl(this, IdExecutablePath, _("valgrind"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("IdExecutablePath"));
	ExecSizer->Add(m_ExecutablePath, 1, wxALL|wxEXPAND, 5);
	BrowseButton = new wxButton(this, IdBrowseButton, _("..."), wxDefaultPosition, wxSize(29,28), 0, wxDefaultValidator, _T("IdBrowseButton"));
	ExecSizer->Add(BrowseButton, 0, wxALL|wxALIGN_BOTTOM, 5);
	MainSizer->Add(ExecSizer, 0, wxALL|wxEXPAND, 0);
	MemCheckOptSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("MemCheck options"));
	MemCheckArgSizer = new wxBoxSizer(wxHORIZONTAL);
	MemCheckArgs = new wxStaticText(this, wxID_ANY, _("Args:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
	MemCheckArgSizer->Add(MemCheckArgs, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
	m_MemCheckArgs = new wxTextCtrl(this, IdMemCheckArgs, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("IdMemCheckArgs"));
	MemCheckArgSizer->Add(m_MemCheckArgs, 1, wxALL|wxEXPAND, 5);
	MemCheckOptSizer->Add(MemCheckArgSizer, 0, wxALL|wxEXPAND, 0);
	m_FullMemCheck = new wxCheckBox(this, IdFullMemCheck, _("Full MemCheck"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("IdFullMemCheck"));
	m_FullMemCheck->SetValue(true);
	MemCheckOptSizer->Add(m_FullMemCheck, 1, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5);
	m_TrackOrigins = new wxCheckBox(this, IdTrackOrigins, _("Track Origins"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("IdTrackOrigins"));
	m_TrackOrigins->SetValue(true);
	MemCheckOptSizer->Add(m_TrackOrigins, 1, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5);
	m_ShowReachable = new wxCheckBox(this, IdShowReachable, _("Show reachable"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("IdShowReachable"));
	m_ShowReachable->SetValue(false);
	MemCheckOptSizer->Add(m_ShowReachable, 1, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5);
	MainSizer->Add(MemCheckOptSizer, 0, wxALL|wxEXPAND, 5);
	CachegrindOptSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Cachegrind options"));
	CacheGrindArgs = new wxStaticText(this, wxID_ANY, _("Args:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
	CachegrindOptSizer->Add(CacheGrindArgs, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
	m_CachegrindArgs = new wxTextCtrl(this, IdCachegrindArgs, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("IdCachegrindArgs"));
	CachegrindOptSizer->Add(m_CachegrindArgs, 1, wxALL|wxEXPAND, 5);
	MainSizer->Add(CachegrindOptSizer, 0, wxALL|wxEXPAND, 5);
	SetSizer(MainSizer);
	MainSizer->Fit(this);
	MainSizer->SetSizeHints(this);

	Connect(IdBrowseButton,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ValgrindConfigurationPanel::OnBrowseButtonClick);
	//*)
}

ValgrindConfigurationPanel::~ValgrindConfigurationPanel()
{
	//(*Destroy(ValgrindConfigurationPanel)
	//*)
}

void ValgrindConfigurationPanel::LoadSettings()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("valgrind"));

    m_ExecutablePath->SetValue(cfg->Read(wxT("/exec_path"), wxT("valgrind")));

    m_MemCheckArgs->SetValue(cfg->Read(wxT("/memcheck_args"), wxEmptyString));
    m_FullMemCheck->SetValue(cfg->ReadBool(wxT("/memcheck_full"), true));
    m_TrackOrigins->SetValue(cfg->ReadBool(wxT("/memcheck_track_origins"), true));
    m_ShowReachable->SetValue(cfg->ReadBool(wxT("/memcheck_reachable"), false));

    m_CachegrindArgs->SetValue(cfg->Read(wxT("/cachegrind_args"), wxEmptyString));
}

void ValgrindConfigurationPanel::OnApply()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("valgrind"));

    cfg->Write(wxT("/exec_path"), m_ExecutablePath->GetValue());

    cfg->Write(wxT("/memcheck_args"), m_MemCheckArgs->GetValue());
    cfg->Write(wxT("/memcheck_full"), m_FullMemCheck->GetValue());
    cfg->Write(wxT("/memcheck_track_origins"), m_TrackOrigins->GetValue());
    cfg->Write(wxT("/memcheck_reachable"), m_ShowReachable->GetValue());

    cfg->Write(wxT("/cachegrind_args"), m_CachegrindArgs->GetValue());
}

void ValgrindConfigurationPanel::OnBrowseButtonClick(wxCommandEvent& /*event*/)
{
    wxFileDialog dialog(this, wxT("Choose path"));
    if (dialog.ShowModal() == wxID_OK)
    {
        m_ExecutablePath->SetValue(dialog.GetPath());
    }
}

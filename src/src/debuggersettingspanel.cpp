#include "sdk.h"
#include "debuggersettingspanel.h"

#ifndef CB_PRECOMP
	//(*InternalHeadersPCH(DebuggerSettingsPanel)
	#include <wx/string.h>
	#include <wx/intl.h>
	//*)

	#include <wx/textdlg.h>

	#include "cbplugin.h"
	#include "debuggermanager.h"
#endif
//(*InternalHeaders(DebuggerSettingsPanel)
//*)

#include "debuggersettingsdlg.h"

//(*IdInit(DebuggerSettingsPanel)
const long DebuggerSettingsPanel::ID_BUTTON_CREATE = wxNewId();
const long DebuggerSettingsPanel::ID_BUTTON_DELETE = wxNewId();
const long DebuggerSettingsPanel::ID_BUTTON_RESET = wxNewId();
const long DebuggerSettingsPanel::ID_TEXTCTRL_INFO = wxNewId();
//*)

BEGIN_EVENT_TABLE(DebuggerSettingsPanel,wxPanel)
	//(*EventTable(DebuggerSettingsPanel)
	//*)
END_EVENT_TABLE()

DebuggerSettingsPanel::DebuggerSettingsPanel(wxWindow* parent, DebuggerSettingsDlg *dialog, cbDebuggerPlugin *plugin) :
    m_dialog(dialog),
    m_plugin(plugin)
{
	//(*Initialize(DebuggerSettingsPanel)
	wxBoxSizer* buttonSizer;
	wxTextCtrl* textInfo;
	wxButton* butReset;
	wxButton* butCreate;
	wxBoxSizer* mainSizer;
	wxStaticBoxSizer* infoSizer;
	wxButton* butDelete;

	Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("wxID_ANY"));
	mainSizer = new wxBoxSizer(wxVERTICAL);
	buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	butCreate = new wxButton(this, ID_BUTTON_CREATE, _("Create Config"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_CREATE"));
	buttonSizer->Add(butCreate, 1, wxALIGN_BOTTOM, 5);
	butDelete = new wxButton(this, ID_BUTTON_DELETE, _("Delete Config"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_DELETE"));
	buttonSizer->Add(butDelete, 1, wxLEFT|wxALIGN_BOTTOM, 5);
	butReset = new wxButton(this, ID_BUTTON_RESET, _("Reset defaults"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_RESET"));
	buttonSizer->Add(butReset, 1, wxLEFT|wxALIGN_BOTTOM, 5);
	mainSizer->Add(buttonSizer, 0, wxALL|wxEXPAND|wxSHAPED, 0);
	infoSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Info"));
	textInfo = new wxTextCtrl(this, ID_TEXTCTRL_INFO, wxEmptyString, wxDefaultPosition, wxSize(186,243), 0, wxDefaultValidator, _T("ID_TEXTCTRL_INFO"));
	textInfo->Disable();
	infoSizer->Add(textInfo, 1, wxEXPAND, 5);
	mainSizer->Add(infoSizer, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 0);
	SetSizer(mainSizer);
	mainSizer->Fit(this);
	mainSizer->SetSizeHints(this);

	Connect(ID_BUTTON_CREATE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DebuggerSettingsPanel::OnButtonCreate);
	Connect(ID_BUTTON_DELETE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DebuggerSettingsPanel::OnButtonDelete);
	Connect(ID_BUTTON_RESET,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DebuggerSettingsPanel::OnButtonReset);
	//*)

	// If this debugger plugin is used to debug some program at the moment
	// - disable the buttons for managing its configurations.
	if (plugin && plugin->IsRunning())
    {
        butCreate->Disable();
        butDelete->Disable();
        butReset->Disable();
    }
}

DebuggerSettingsPanel::~DebuggerSettingsPanel()
{
	//(*Destroy(DebuggerSettingsPanel)
	//*)
}

void DebuggerSettingsPanel::OnButtonCreate(cb_unused wxCommandEvent& event)
{
    wxTextEntryDialog dialog(this, _("Please specify a name for the configuration"), _("Create config"));
    PlaceWindow(&dialog);
    while (dialog.ShowModal() == wxID_OK)
    {
        wxString name = dialog.GetValue();
        if (!name.empty())
        {
            if (m_dialog->CreateConfig(this, m_plugin, name))
                break;
            else
                cbMessageBox(_("Name is not unique! Choose different one."), _("Error"), wxICON_ERROR, this);
        }
    }
}

void DebuggerSettingsPanel::OnButtonDelete(cb_unused wxCommandEvent& event)
{
    m_dialog->DeleteConfig(this, m_plugin);
}

void DebuggerSettingsPanel::OnButtonReset(cb_unused wxCommandEvent& event)
{
    if (cbMessageBox(_("Are you sure you want to reset the settings for the selected plugin?"), _("Reset settings"),
                     wxICON_QUESTION | wxYES) == wxID_NO)
    {
        return;
    }
    m_dialog->ResetConfig(this, m_plugin);
}

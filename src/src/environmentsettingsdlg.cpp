#include <wx/xrc/xmlres.h>
#include <configmanager.h>
#include <wx/intl.h>
#include <wx/checkbox.h>
#include <wx/radiobox.h>
#include <wx/msgdlg.h>
#include <wx/filedlg.h>

#include "environmentsettingsdlg.h"

BEGIN_EVENT_TABLE(EnvironmentSettingsDlg, wxDialog)
    EVT_BUTTON(XRCID("btnExportConfig"), EnvironmentSettingsDlg::OnExportConfig)
END_EVENT_TABLE()

EnvironmentSettingsDlg::EnvironmentSettingsDlg(wxWindow* parent)
{
	wxXmlResource::Get()->LoadDialog(this, parent, _("dlgEnvironmentSettings"));
	
	// tab "General"
	XRCCTRL(*this, "chkShowSplash", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/environment/show_splash", 1));
	XRCCTRL(*this, "chkDDE", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/environment/use_dde", 1));
	XRCCTRL(*this, "chkAssociations", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/environment/check_associations", 1));
	XRCCTRL(*this, "chkDebugLog", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/message_manager/has_debug_log", (long int)0));
	XRCCTRL(*this, "rbProjectOpen", wxRadioBox)->SetSelection(ConfigManager::Get()->Read("/project_manager/open_files", (long int)1));
	XRCCTRL(*this, "rbEditorInterface", wxRadioBox)->Enable(false); // not implemented
}

EnvironmentSettingsDlg::~EnvironmentSettingsDlg()
{
	//dtor
}

void EnvironmentSettingsDlg::OnExportConfig(wxCommandEvent& event)
{
//    wxFileDialog dlg(this, _("Choose a file"), "", "", "*.*", wxSAVE | wxOVERWRITE_PROMPT | wxHIDE_READONLY);
//    if (dlg.ShowModal() == wxID_OK)
//        ConfigManager::ExportToFile(dlg.GetPath());
    ConfigManager::ExportToFile("c:/Devel/codeblocks/src/devel/test.conf");
}

void EnvironmentSettingsDlg::EndModal(int retCode)
{
	if (retCode == wxID_OK)
	{
		// tab "General"
		ConfigManager::Get()->Write("/environment/show_splash", XRCCTRL(*this, "chkShowSplash", wxCheckBox)->GetValue());
		ConfigManager::Get()->Write("/environment/use_dde", XRCCTRL(*this, "chkDDE", wxCheckBox)->GetValue());
		ConfigManager::Get()->Write("/environment/check_associations", XRCCTRL(*this, "chkAssociations", wxCheckBox)->GetValue());
        ConfigManager::Get()->Write("/message_manager/has_debug_log", XRCCTRL(*this, "chkDebugLog", wxCheckBox)->GetValue());
        ConfigManager::Get()->Write("/project_manager/open_files", XRCCTRL(*this, "rbProjectOpen", wxRadioBox)->GetSelection());
	}

	wxDialog::EndModal(retCode);
}

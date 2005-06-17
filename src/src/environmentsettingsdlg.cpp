#include <wx/xrc/xmlres.h>
#include <configmanager.h>
#include <wx/intl.h>
#include <wx/checkbox.h>
#include <wx/radiobox.h>
#include <wx/msgdlg.h>

#include "environmentsettingsdlg.h"

EnvironmentSettingsDlg::EnvironmentSettingsDlg(wxWindow* parent)
{
	wxXmlResource::Get()->LoadDialog(this, parent, _("dlgEnvironmentSettings"));
	
	// tab "General"
	XRCCTRL(*this, "chkShowSplash", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/environment/show_splash", 1));
	XRCCTRL(*this, "chkDDE", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/environment/use_dde", 1));
	XRCCTRL(*this, "chkSingleInstance", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/environment/single_instance", 1));
	XRCCTRL(*this, "chkAssociations", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/environment/check_associations", 1));
	XRCCTRL(*this, "chkModifiedFiles", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/environment/check_modified_files", 1));
	XRCCTRL(*this, "chkDebugLog", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/message_manager/has_debug_log", (long int)0));
	XRCCTRL(*this, "rbAppStart", wxRadioBox)->SetSelection(ConfigManager::Get()->Read("/environment/blank_workspace", (long int)0));
	XRCCTRL(*this, "rbProjectOpen", wxRadioBox)->SetSelection(ConfigManager::Get()->Read("/project_manager/open_files", (long int)1));
	XRCCTRL(*this, "rbToolbarSize", wxRadioBox)->SetSelection(ConfigManager::Get()->Read("/environment/toolbar_size", (long int)0));
	XRCCTRL(*this, "chkAutoHideMessages", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/message_manager/auto_hide", 0L));
	XRCCTRL(*this, "chkShowStartPage", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/environment/start_here_page", 1));
}

EnvironmentSettingsDlg::~EnvironmentSettingsDlg()
{
	//dtor
}

void EnvironmentSettingsDlg::EndModal(int retCode)
{
	if (retCode == wxID_OK)
	{
		// tab "General"
		ConfigManager::Get()->Write("/environment/show_splash", XRCCTRL(*this, "chkShowSplash", wxCheckBox)->GetValue());
		ConfigManager::Get()->Write("/environment/use_dde", XRCCTRL(*this, "chkDDE", wxCheckBox)->GetValue());
		ConfigManager::Get()->Write("/environment/single_instance", XRCCTRL(*this, "chkSingleInstance", wxCheckBox)->GetValue());
		ConfigManager::Get()->Write("/environment/check_associations", XRCCTRL(*this, "chkAssociations", wxCheckBox)->GetValue());
		ConfigManager::Get()->Write("/environment/check_modified_files", XRCCTRL(*this, "chkModifiedFiles", wxCheckBox)->GetValue());
        ConfigManager::Get()->Write("/message_manager/has_debug_log", XRCCTRL(*this, "chkDebugLog", wxCheckBox)->GetValue());
        ConfigManager::Get()->Write("/environment/blank_workspace", XRCCTRL(*this, "rbAppStart", wxRadioBox)->GetSelection());
        ConfigManager::Get()->Write("/project_manager/open_files", XRCCTRL(*this, "rbProjectOpen", wxRadioBox)->GetSelection());
        ConfigManager::Get()->Write("/environment/toolbar_size", XRCCTRL(*this, "rbToolbarSize", wxRadioBox)->GetSelection());
        ConfigManager::Get()->Write("/message_manager/auto_hide", XRCCTRL(*this, "chkAutoHideMessages", wxCheckBox)->GetValue());
        ConfigManager::Get()->Write("/environment/start_here_page", XRCCTRL(*this, "chkShowStartPage", wxCheckBox)->GetValue());
	}

	wxDialog::EndModal(retCode);
}

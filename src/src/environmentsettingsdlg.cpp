#include <wx/xrc/xmlres.h>
#include "../sdk/configmanager.h"
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
	XRCCTRL(*this, "chkAssociations", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/environment/check_associations", 1));
	XRCCTRL(*this, "chkDebugLog", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/message_manager/has_debug_log", (long int)0));
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
		ConfigManager::Get()->Write("/environment/check_associations", XRCCTRL(*this, "chkAssociations", wxCheckBox)->GetValue());
        ConfigManager::Get()->Write("/message_manager/has_debug_log", XRCCTRL(*this, "chkDebugLog", wxCheckBox)->GetValue());
	}

	wxDialog::EndModal(retCode);
}

#include <sdk.h>
#include <wx/xrc/xmlres.h>
#include <configmanager.h>
#include <wx/intl.h>
#include <wx/checkbox.h>
#include <wx/radiobox.h>
#include <wx/msgdlg.h>

#include "environmentsettingsdlg.h"

EnvironmentSettingsDlg::EnvironmentSettingsDlg(wxWindow* parent)
{
	wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgEnvironmentSettings"));

	// tab "General"
	XRCCTRL(*this, "chkShowSplash", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/environment/show_splash"), 1));
	XRCCTRL(*this, "chkDDE", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/environment/use_dde"), 1));
	XRCCTRL(*this, "chkSingleInstance", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/environment/single_instance"), 1));
	XRCCTRL(*this, "chkAssociations", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/environment/check_associations"), 1));
	XRCCTRL(*this, "chkModifiedFiles", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/environment/check_modified_files"), 1));
	XRCCTRL(*this, "chkDebugLog", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/message_manager/has_debug_log"), (long int)0));
	XRCCTRL(*this, "rbAppStart", wxRadioBox)->SetSelection(ConfigManager::Get()->Read(_T("/environment/blank_workspace"), (long int)0));
	XRCCTRL(*this, "rbProjectOpen", wxRadioBox)->SetSelection(ConfigManager::Get()->Read(_T("/project_manager/open_files"), (long int)1));
	XRCCTRL(*this, "rbToolbarSize", wxRadioBox)->SetSelection(ConfigManager::Get()->Read(_T("/environment/toolbar_size"), (long int)1));
	XRCCTRL(*this, "chkAutoHideMessages", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/message_manager/auto_hide"), 0L));
	XRCCTRL(*this, "chkShowEditorCloseButton", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/editor/show_close_button"), 0L));
	XRCCTRL(*this, "chkShowStartPage", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/environment/start_here_page"), 1));
	XRCCTRL(*this, "chkSafebutSlow", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/message_manager/safe_but_slow"), 0L));
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
		ConfigManager::Get()->Write(_T("/environment/show_splash"), XRCCTRL(*this, "chkShowSplash", wxCheckBox)->GetValue());
		ConfigManager::Get()->Write(_T("/environment/use_dde"), XRCCTRL(*this, "chkDDE", wxCheckBox)->GetValue());
		ConfigManager::Get()->Write(_T("/environment/single_instance"), XRCCTRL(*this, "chkSingleInstance", wxCheckBox)->GetValue());
		ConfigManager::Get()->Write(_T("/environment/check_associations"), XRCCTRL(*this, "chkAssociations", wxCheckBox)->GetValue());
		ConfigManager::Get()->Write(_T("/environment/check_modified_files"), XRCCTRL(*this, "chkModifiedFiles", wxCheckBox)->GetValue());
        ConfigManager::Get()->Write(_T("/message_manager/has_debug_log"), XRCCTRL(*this, "chkDebugLog", wxCheckBox)->GetValue());
        ConfigManager::Get()->Write(_T("/environment/blank_workspace"), XRCCTRL(*this, "rbAppStart", wxRadioBox)->GetSelection());
        ConfigManager::Get()->Write(_T("/project_manager/open_files"), XRCCTRL(*this, "rbProjectOpen", wxRadioBox)->GetSelection());
        ConfigManager::Get()->Write(_T("/environment/toolbar_size"), XRCCTRL(*this, "rbToolbarSize", wxRadioBox)->GetSelection());
        ConfigManager::Get()->Write(_T("/message_manager/auto_hide"), XRCCTRL(*this, "chkAutoHideMessages", wxCheckBox)->GetValue());
        ConfigManager::Get()->Write(_T("/editor/show_close_button"), XRCCTRL(*this, "chkShowEditorCloseButton", wxCheckBox)->GetValue());
        ConfigManager::Get()->Write(_T("/environment/start_here_page"), XRCCTRL(*this, "chkShowStartPage", wxCheckBox)->GetValue());
        ConfigManager::Get()->Write(_T("/message_manager/safe_but_slow"), XRCCTRL(*this, "chkSafebutSlow", wxCheckBox)->GetValue());
	}

	wxDialog::EndModal(retCode);
}

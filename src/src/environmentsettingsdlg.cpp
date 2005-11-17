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

    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("app"));
    ConfigManager *pcfg = Manager::Get()->GetConfigManager(_T("project_manager"));
    ConfigManager *mcfg = Manager::Get()->GetConfigManager(_T("message_manager"));
    ConfigManager *ecfg = Manager::Get()->GetConfigManager(_T("editor"));

    // tab "General"
    XRCCTRL(*this, "chkShowSplash", wxCheckBox)->SetValue(cfg->ReadBool(_T("/environment/show_splash"), true));
    XRCCTRL(*this, "chkDDE", wxCheckBox)->SetValue(cfg->ReadBool(_T("/environment/use_dde"), true));
    XRCCTRL(*this, "chkSingleInstance", wxCheckBox)->SetValue(cfg->ReadBool(_T("/environment/single_instance"), true));
    XRCCTRL(*this, "chkAssociations", wxCheckBox)->SetValue(cfg->ReadBool(_T("/environment/check_associations"), true));
    XRCCTRL(*this, "chkModifiedFiles", wxCheckBox)->SetValue(cfg->ReadBool(_T("/environment/check_modified_files"), true));
    XRCCTRL(*this, "chkDebugLog", wxCheckBox)->SetValue(mcfg->ReadBool(_T("/has_debug_log"), false));
    XRCCTRL(*this, "rbAppStart", wxRadioBox)->SetSelection(cfg->ReadBool(_T("/environment/blank_workspace"), true) ? 1 : 0);

    XRCCTRL(*this, "rbProjectOpen", wxRadioBox)->SetSelection(pcfg->ReadInt(_T("/open_files"), 1));

    XRCCTRL(*this, "rbToolbarSize", wxRadioBox)->SetSelection(cfg->ReadInt(_T("/environment/toolbar_size"), 1));
    XRCCTRL(*this, "chkAutoHideMessages", wxCheckBox)->SetValue(mcfg->ReadBool(_T("/auto_hide"), false));

    XRCCTRL(*this, "chkShowEditorCloseButton", wxCheckBox)->SetValue(ecfg->ReadBool(_T("/show_close_button"), false));

    XRCCTRL(*this, "chkShowStartPage", wxCheckBox)->SetValue(cfg->ReadBool(_T("/environment/start_here_page"), true));
    XRCCTRL(*this, "chkSafebutSlow", wxCheckBox)->SetValue(mcfg->ReadBool(_T("/safe_but_slow"), false));
}

EnvironmentSettingsDlg::~EnvironmentSettingsDlg()
{
    //dtor
}

void EnvironmentSettingsDlg::EndModal(int retCode)
{
    if (retCode == wxID_OK)
    {
        ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("app"));
        ConfigManager *pcfg = Manager::Get()->GetConfigManager(_T("project_manager"));
        ConfigManager *mcfg = Manager::Get()->GetConfigManager(_T("message_manager"));
        ConfigManager *ecfg = Manager::Get()->GetConfigManager(_T("editor"));

        // tab "General"
        cfg->Write(_T("/environment/show_splash"),           (bool) XRCCTRL(*this, "chkShowSplash", wxCheckBox)->GetValue());
        cfg->Write(_T("/environment/use_dde"),               (bool) XRCCTRL(*this, "chkDDE", wxCheckBox)->GetValue());
        cfg->Write(_T("/environment/single_instance"),       (bool) XRCCTRL(*this, "chkSingleInstance", wxCheckBox)->GetValue());
        cfg->Write(_T("/environment/check_associations"),    (bool) XRCCTRL(*this, "chkAssociations", wxCheckBox)->GetValue());
        cfg->Write(_T("/environment/check_modified_files"),  (bool) XRCCTRL(*this, "chkModifiedFiles", wxCheckBox)->GetValue());
        mcfg->Write(_T("/has_debug_log"),                    (bool) XRCCTRL(*this, "chkDebugLog", wxCheckBox)->GetValue());
        cfg->Write(_T("/environment/blank_workspace"),       (bool) XRCCTRL(*this, "rbAppStart", wxRadioBox)->GetSelection() ? true : false);

        pcfg->Write(_T("/open_files"),                       (int) XRCCTRL(*this, "rbProjectOpen", wxRadioBox)->GetSelection());

        cfg->Write(_T("/environment/toolbar_size"),          (int)  XRCCTRL(*this, "rbToolbarSize", wxRadioBox)->GetSelection());
        mcfg->Write(_T("/auto_hide"),                        (bool) XRCCTRL(*this, "chkAutoHideMessages", wxCheckBox)->GetValue());

        ecfg->Write(_T("/show_close_button"),                (bool) XRCCTRL(*this, "chkShowEditorCloseButton", wxCheckBox)->GetValue());

        cfg->Write(_T("/environment/start_here_page"),       (bool) XRCCTRL(*this, "chkShowStartPage", wxCheckBox)->GetValue());
        mcfg->Write(_T("/safe_but_slow"),                    (bool) XRCCTRL(*this, "chkSafebutSlow", wxCheckBox)->GetValue());
    }

    wxDialog::EndModal(retCode);
}

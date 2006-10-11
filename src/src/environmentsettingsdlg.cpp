/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is distributed under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include <sdk.h>
#include <wx/xrc/xmlres.h>
#include <manager.h>
#include <configmanager.h>
#include <pluginmanager.h>
#include <wx/intl.h>
#include <wx/listbook.h>
#include <wx/listctrl.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include <wx/checklst.h>
#include <wx/radiobox.h>
#include <wx/spinctrl.h>
#include <wx/colordlg.h>
#include <wx/msgdlg.h>
#include "wxAUI/manager.h"
#include "appglobals.h"
#include "../sdk/globals.h"
#include "associations.h"

#include "configurationpanel.h"
#include "environmentsettingsdlg.h"
#ifdef __WXMSW__
    #include "associations.h"
#endif

#ifndef CB_PRECOMP
    #include "cbplugin.h" // cgCompiler, cgDebugger...
#endif

// images by order of pages
const wxString base_imgs[] =
{
    _T("general-prefs"),
    _T("view"),
    _T("notebook-appearance"),
    _T("colours"),
    _T("dialogs"),
    _T("net"),
};
const int IMAGES_COUNT = sizeof(base_imgs) / sizeof(wxString);

BEGIN_EVENT_TABLE(EnvironmentSettingsDlg, wxDialog)
    EVT_UPDATE_UI(-1, EnvironmentSettingsDlg::OnUpdateUI)
    EVT_BUTTON(XRCID("btnSetAssocs"), EnvironmentSettingsDlg::OnSetAssocs)
    EVT_BUTTON(XRCID("btnManageAssocs"), EnvironmentSettingsDlg::OnManageAssocs)
    EVT_BUTTON(XRCID("btnFNBorder"), EnvironmentSettingsDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnFNFrom"), EnvironmentSettingsDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnFNTo"), EnvironmentSettingsDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnAuiBgColour"), EnvironmentSettingsDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnAuiSashColour"), EnvironmentSettingsDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnAuiCaptionColour"), EnvironmentSettingsDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnAuiCaptionTextColour"), EnvironmentSettingsDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnAuiBorderColour"), EnvironmentSettingsDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnAuiGripperColour"), EnvironmentSettingsDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnAuiActiveCaptionColour"), EnvironmentSettingsDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnAuiActiveCaptionGradientColour"), EnvironmentSettingsDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnAuiActiveCaptionTextColour"), EnvironmentSettingsDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnAuiInactiveCaptionColour"), EnvironmentSettingsDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnAuiInactiveCaptionGradientColour"), EnvironmentSettingsDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnAuiInactiveCaptionTextColour"), EnvironmentSettingsDlg::OnChooseColour)
    EVT_CHECKBOX(XRCID("chkDoPlace"), EnvironmentSettingsDlg::OnPlaceCheck)
    EVT_CHECKBOX(XRCID("chkPlaceHead"), EnvironmentSettingsDlg::OnHeadCheck)
    EVT_CHECKBOX(XRCID("chkAutoHideMessages"), EnvironmentSettingsDlg::OnAutoHide)
    EVT_CHECKBOX(XRCID("chkI18N"), EnvironmentSettingsDlg::OnI18NCheck)
    EVT_RADIOBOX(XRCID("rbSettingsIconsSize"), EnvironmentSettingsDlg::OnSettingsIconsSize)

    EVT_LISTBOOK_PAGE_CHANGING(XRCID("nbMain"), EnvironmentSettingsDlg::OnPageChanging)
    EVT_LISTBOOK_PAGE_CHANGED(XRCID("nbMain"), EnvironmentSettingsDlg::OnPageChanged)
END_EVENT_TABLE()

EnvironmentSettingsDlg::EnvironmentSettingsDlg(wxWindow* parent, wxDockArt* art)
    : m_pArt(art)
{
    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("app"));
    ConfigManager *pcfg = Manager::Get()->GetConfigManager(_T("project_manager"));
    ConfigManager *mcfg = Manager::Get()->GetConfigManager(_T("message_manager"));
    ConfigManager *acfg = Manager::Get()->GetConfigManager(_T("an_dlg"));

    wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgEnvironmentSettings"));
    int sel = cfg->ReadInt(_T("/environment/settings_size"), 0);
    wxListbook* lb = XRCCTRL(*this, "nbMain", wxListbook);
    SetSettingsIconsStyle(lb->GetListView(), (SettingsIconsStyle)sel);
    LoadListbookImages();

    // this setting is not available under wxGTK
    #ifndef __WXMSW__
    XRCCTRL(*this, "rbSettingsIconsSize", wxRadioBox)->Enable(false);
    #endif

    // tab "General"
    XRCCTRL(*this, "chkShowSplash", wxCheckBox)->SetValue(cfg->ReadBool(_T("/environment/show_splash"), true));
    XRCCTRL(*this, "chkSingleInstance", wxCheckBox)->SetValue(cfg->ReadBool(_T("/environment/single_instance"), true));
    XRCCTRL(*this, "chkAssociations", wxCheckBox)->SetValue(cfg->ReadBool(_T("/environment/check_associations"), true));
    XRCCTRL(*this, "chkModifiedFiles", wxCheckBox)->SetValue(cfg->ReadBool(_T("/environment/check_modified_files"), true));
    XRCCTRL(*this, "rbAppStart", wxRadioBox)->SetSelection(cfg->ReadBool(_T("/environment/blank_workspace"), true) ? 1 : 0);
    wxTextCtrl* txt = XRCCTRL(*this, "txtConsoleTerm", wxTextCtrl);
    txt->SetValue(cfg->Read(_T("/console_terminal"), DEFAULT_CONSOLE_TERM));
#ifdef __WXMSW__
    // under win32, this option is not needed, so disable it
    txt->Enable(false);
#endif
    txt = XRCCTRL(*this, "txtConsoleShell", wxTextCtrl);
    txt->SetValue(cfg->Read(_T("/console_shell"), DEFAULT_CONSOLE_SHELL));
#ifdef __WXMSW__
    // under win32, this option is not needed, so disable it
    txt->Enable(false);
#endif

    // tab "View"
    bool do_place = cfg->ReadBool(_T("/dialog_placement/do_place"), false);
    XRCCTRL(*this, "chkDoPlace", wxCheckBox)->SetValue(do_place);
    XRCCTRL(*this, "chkPlaceHead", wxCheckBox)->SetValue(cfg->ReadInt(_T("/dialog_placement/dialog_position"), 0) == pdlHead ? 1 : 0);
    XRCCTRL(*this, "chkPlaceHead", wxCheckBox)->Enable(do_place);

    XRCCTRL(*this, "rbProjectOpen", wxRadioBox)->SetSelection(pcfg->ReadInt(_T("/open_files"), 1));
    XRCCTRL(*this, "rbToolbarSize", wxRadioBox)->SetSelection(cfg->ReadBool(_T("/environment/toolbar_size"), true) ? 1 : 0);
    XRCCTRL(*this, "rbSettingsIconsSize", wxRadioBox)->SetSelection(cfg->ReadInt(_T("/environment/settings_size"), 0));
    XRCCTRL(*this, "chkShowStartPage", wxCheckBox)->SetValue(cfg->ReadBool(_T("/environment/start_here_page"), true));
    XRCCTRL(*this, "spnLogFontSize", wxSpinCtrl)->SetValue(mcfg->ReadInt(_T("/log_font_size"), 8));

    bool en = mcfg->ReadBool(_T("/auto_hide"), false);
    XRCCTRL(*this, "chkAutoHideMessages", wxCheckBox)->SetValue(en);
    XRCCTRL(*this, "chkAutoShowMessagesOnSearch", wxCheckBox)->SetValue(mcfg->ReadBool(_T("/auto_show_search"), true));
    XRCCTRL(*this, "chkAutoShowMessagesOnWarn", wxCheckBox)->SetValue(mcfg->ReadBool(_T("/auto_show_build_warnings"), true));
    XRCCTRL(*this, "chkAutoShowMessagesOnErr", wxCheckBox)->SetValue(mcfg->ReadBool(_T("/auto_show_build_errors"), true));
    XRCCTRL(*this, "chkAutoShowMessagesOnSearch", wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkAutoShowMessagesOnWarn", wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkAutoShowMessagesOnErr", wxCheckBox)->Enable(en);

    bool i18n=cfg->ReadBool(_T("/environment/I18N"), false);
        XRCCTRL(*this, "chkI18N", wxCheckBox)->SetValue(i18n);
    for(size_t i=0; i < LANGUAGES_SIZE; i++)
    {
        XRCCTRL(*this, "cbxLanguage", wxComboBox)->Append(langs[i]);
    }
    XRCCTRL(*this, "cbxLanguage", wxComboBox)->Enable(i18n);
    if(i18n)
    {
        int lng = cfg->ReadInt(_T("/locale/language"),-1);

        if (lng >= 0 && static_cast<unsigned int>(lng) < LANGUAGES_SIZE)
        {
            XRCCTRL(*this, "cbxLanguage", wxComboBox)->SetSelection(lng+1);
        }
    }

    // tab "Notebook"
    XRCCTRL(*this, "cmbEditorTabs", wxComboBox)->SetSelection(cfg->ReadInt(_T("/environment/tabs_style"), 0));
    XRCCTRL(*this, "chkSmartTabs", wxCheckBox)->SetValue(cfg->ReadBool(_T("/environment/tabs_smart"), 0));
    XRCCTRL(*this, "btnFNBorder", wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/gradient_border"), wxColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW))));
    XRCCTRL(*this, "btnFNFrom", wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/gradient_from"), wxColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE))));
    XRCCTRL(*this, "btnFNTo", wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/gradient_to"), *wxWHITE));

    // tab "Docking"
    XRCCTRL(*this, "spnAuiBorder", wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/environment/aui/border_size"), m_pArt->GetMetric(wxAUI_ART_PANE_BORDER_SIZE)));
    XRCCTRL(*this, "spnAuiSash", wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/environment/aui/sash_size"), m_pArt->GetMetric(wxAUI_ART_SASH_SIZE)));
    XRCCTRL(*this, "spnAuiCaption", wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/environment/aui/caption_size"), m_pArt->GetMetric(wxAUI_ART_CAPTION_SIZE)));
    XRCCTRL(*this, "btnAuiBgColour", wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/aui/bg_colour"), m_pArt->GetColour(wxAUI_ART_BACKGROUND_COLOUR)));
    XRCCTRL(*this, "btnAuiSashColour", wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/aui/sash_colour"), m_pArt->GetColour(wxAUI_ART_SASH_COLOUR)));
    XRCCTRL(*this, "btnAuiActiveCaptionColour", wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/aui/active_caption_colour"), m_pArt->GetColour(wxAUI_ART_ACTIVE_CAPTION_COLOUR)));
    XRCCTRL(*this, "btnAuiActiveCaptionGradientColour", wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/aui/active_caption_gradient_colour"), m_pArt->GetColour(wxAUI_ART_ACTIVE_CAPTION_GRADIENT_COLOUR)));
    XRCCTRL(*this, "btnAuiActiveCaptionTextColour", wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/aui/active_caption_text_colour"), m_pArt->GetColour(wxAUI_ART_ACTIVE_CAPTION_TEXT_COLOUR)));
    XRCCTRL(*this, "btnAuiInactiveCaptionColour", wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/aui/inactive_caption_colour"), m_pArt->GetColour(wxAUI_ART_INACTIVE_CAPTION_COLOUR)));
    XRCCTRL(*this, "btnAuiInactiveCaptionGradientColour", wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/aui/inactive_caption_gradient_colour"), m_pArt->GetColour(wxAUI_ART_INACTIVE_CAPTION_GRADIENT_COLOUR)));
    XRCCTRL(*this, "btnAuiInactiveCaptionTextColour", wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/aui/inactive_caption_text_colour"), m_pArt->GetColour(wxAUI_ART_INACTIVE_CAPTION_TEXT_COLOUR)));
    XRCCTRL(*this, "btnAuiBorderColour", wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/aui/border_colour"), m_pArt->GetColour(wxAUI_ART_BORDER_COLOUR)));
    XRCCTRL(*this, "btnAuiGripperColour", wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/aui/gripper_colour"), m_pArt->GetColour(wxAUI_ART_GRIPPER_COLOUR)));

    // tab "Dialogs"
    wxCheckListBox* clb = XRCCTRL(*this, "chkDialogs", wxCheckListBox);
    clb->Clear();

    ConfigManagerContainer::StringSet dialogs = acfg->ReadSSet(_T("/disabled"));
    for (ConfigManagerContainer::StringSet::iterator i = dialogs.begin(); i != dialogs.end(); ++i)
            clb->Append(*i);

    // tab "Network"
    XRCCTRL(*this, "txtProxy", wxTextCtrl)->SetValue(cfg->Read(_T("/network_proxy")));

    // disable some windows-only settings, in other platforms
#ifndef __WXMSW__
    XRCCTRL(*this, "chkAssociations", wxCheckBox)->Enable(false);
    XRCCTRL(*this, "btnSetAssocs", wxButton)->Enable(false);
    XRCCTRL(*this, "btnManageAssocs", wxButton)->Enable(false);
#endif

    // add all plugins configuration panels
    AddPluginPanels();

    // make sure everything is laid out properly
    GetSizer()->SetSizeHints(this);
    CentreOnParent();
}

EnvironmentSettingsDlg::~EnvironmentSettingsDlg()
{
    //dtor
}

void EnvironmentSettingsDlg::AddPluginPanels()
{
    const wxString base = ConfigManager::GetDataFolder() + _T("/images/settings/");

    wxListbook* lb = XRCCTRL(*this, "nbMain", wxListbook);
    // get all configuration panels which are *not* about compiler, debugger and editor.
    Manager::Get()->GetPluginManager()->GetConfigurationPanels(~(cgCompiler | cgDebugger | cgEditor), lb, m_PluginPanels);

    for (size_t i = 0; i < m_PluginPanels.GetCount(); ++i)
    {
        cbConfigurationPanel* panel = m_PluginPanels[i];
        lb->AddPage(panel, panel->GetTitle());

        lb->GetImageList()->Add(cbLoadBitmap(base + panel->GetBitmapBaseName() + _T(".png")));
        lb->GetImageList()->Add(cbLoadBitmap(base + panel->GetBitmapBaseName() + _T("-off.png")));
        lb->SetPageImage(lb->GetPageCount() - 1, lb->GetImageList()->GetImageCount() - 2);
    }

    UpdateListbookImages();
}

void EnvironmentSettingsDlg::LoadListbookImages()
{
    const wxString base = ConfigManager::GetDataFolder() + _T("/images/settings/");

    wxImageList* images = new wxImageList(80, 80);
    wxBitmap bmp;
    for (int i = 0; i < IMAGES_COUNT; ++i)
    {
        bmp = cbLoadBitmap(base + base_imgs[i] + _T(".png"));
        images->Add(bmp);
        bmp = cbLoadBitmap(base + base_imgs[i] + _T("-off.png"));
        images->Add(bmp);
    }
    wxListbook* lb = XRCCTRL(*this, "nbMain", wxListbook);
    lb->AssignImageList(images);

    UpdateListbookImages();
}

void EnvironmentSettingsDlg::UpdateListbookImages()
{
    wxListbook* lb = XRCCTRL(*this, "nbMain", wxListbook);
    int sel = lb->GetSelection();
    // set page images according to their on/off status
    for (size_t i = 0; i < IMAGES_COUNT + m_PluginPanels.GetCount(); ++i)
    {
        lb->SetPageImage(i, (i * 2) + (sel == (int)i ? 0 : 1));
    }

    // the selection colour is ruining the on/off effect,
    // so make sure no item is selected ;)
    // (only if we have icons showing)
    if (GetSettingsIconsStyle(lb->GetListView()) != sisNoIcons)
        lb->GetListView()->Select(sel, false);

    // update the page title
    wxString label = lb->GetPageText(sel);
    // replace any stray & with && because label makes it an underscore
    while (label.Replace(_T(" & "), _T(" && ")))
        ;
    XRCCTRL(*this, "lblBigTitle", wxStaticText)->SetLabel(label);
}

void EnvironmentSettingsDlg::OnPageChanging(wxListbookEvent& event)
{
}

void EnvironmentSettingsDlg::OnPageChanged(wxListbookEvent& event)
{
    UpdateListbookImages();
}

void EnvironmentSettingsDlg::OnSetAssocs(wxCommandEvent& event)
{
#ifdef __WXMSW__
    Associations::SetCore();
    //cbMessageBox(_("Code::Blocks associated with C/C++ files."), _("Information"), wxICON_INFORMATION);
#endif
}

void EnvironmentSettingsDlg::OnManageAssocs(wxCommandEvent& event)
{
#ifdef __WXMSW__
    ManageAssocsDialog dlg(this);
    PlaceWindow(&dlg);
    dlg.ShowModal();
#endif
}

void EnvironmentSettingsDlg::OnChooseColour(wxCommandEvent& event)
{
    wxColourData data;
    wxWindow* sender = FindWindowById(event.GetId());
    data.SetColour(sender->GetBackgroundColour());

    wxColourDialog dlg(this, &data);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxColour colour = dlg.GetColourData().GetColour();
        sender->SetBackgroundColour(colour);
    }
}

void EnvironmentSettingsDlg::OnAutoHide(wxCommandEvent& event)
{
    bool en = XRCCTRL(*this, "chkAutoHideMessages", wxCheckBox)->GetValue();
    XRCCTRL(*this, "chkAutoShowMessagesOnSearch", wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkAutoShowMessagesOnWarn", wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkAutoShowMessagesOnErr", wxCheckBox)->Enable(en);
}

void EnvironmentSettingsDlg::OnPlaceCheck(wxCommandEvent& event)
{
    XRCCTRL(*this, "chkPlaceHead", wxCheckBox)->Enable(event.IsChecked());
}

void EnvironmentSettingsDlg::OnHeadCheck(wxCommandEvent& event)
{
    PlaceWindow(this, event.IsChecked() ? pdlHead : pdlCentre, true);
}

void EnvironmentSettingsDlg::OnI18NCheck(wxCommandEvent& event)
{
    XRCCTRL(*this, "cbxLanguage", wxComboBox)->Enable(event.IsChecked());
}

void EnvironmentSettingsDlg::OnSettingsIconsSize(wxCommandEvent& event)
{
    wxListbook* lb = XRCCTRL(*this, "nbMain", wxListbook);
    SetSettingsIconsStyle(lb->GetListView(), (SettingsIconsStyle)event.GetSelection());
}

void EnvironmentSettingsDlg::OnUpdateUI(wxUpdateUIEvent& event)
{
    bool en = XRCCTRL(*this, "cmbEditorTabs", wxComboBox)->GetSelection() == 1;
    XRCCTRL(*this, "btnFNBorder", wxButton)->Enable(en);
    XRCCTRL(*this, "btnFNFrom", wxButton)->Enable(en);
    XRCCTRL(*this, "btnFNTo", wxButton)->Enable(en);
}

void EnvironmentSettingsDlg::EndModal(int retCode)
{
    if (retCode == wxID_OK)
    {
        ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("app"));
        ConfigManager *pcfg = Manager::Get()->GetConfigManager(_T("project_manager"));
        ConfigManager *mcfg = Manager::Get()->GetConfigManager(_T("message_manager"));
        ConfigManager *acfg = Manager::Get()->GetConfigManager(_T("an_dlg"));

        // tab "General"
        cfg->Write(_T("/environment/show_splash"),           (bool) XRCCTRL(*this, "chkShowSplash", wxCheckBox)->GetValue());
        cfg->Write(_T("/environment/single_instance"),       (bool) XRCCTRL(*this, "chkSingleInstance", wxCheckBox)->GetValue());
        cfg->Write(_T("/environment/check_associations"),    (bool) XRCCTRL(*this, "chkAssociations", wxCheckBox)->GetValue());
        cfg->Write(_T("/environment/check_modified_files"),  (bool) XRCCTRL(*this, "chkModifiedFiles", wxCheckBox)->GetValue());
        cfg->Write(_T("/console_shell"),                            XRCCTRL(*this, "txtConsoleShell", wxTextCtrl)->GetValue());
        cfg->Write(_T("/console_terminal"),                         XRCCTRL(*this, "txtConsoleTerm", wxTextCtrl)->GetValue());

        // tab "View"
        cfg->Write(_T("/environment/blank_workspace"),       (bool) XRCCTRL(*this, "rbAppStart", wxRadioBox)->GetSelection() ? true : false);
        pcfg->Write(_T("/open_files"),                       (int)  XRCCTRL(*this, "rbProjectOpen", wxRadioBox)->GetSelection());
        cfg->Write(_T("/environment/toolbar_size"),          (bool) XRCCTRL(*this, "rbToolbarSize", wxRadioBox)->GetSelection() == 1);
        cfg->Write(_T("/environment/settings_size"),         (int)  XRCCTRL(*this, "rbSettingsIconsSize", wxRadioBox)->GetSelection());
        mcfg->Write(_T("/auto_hide"),                        (bool) XRCCTRL(*this, "chkAutoHideMessages", wxCheckBox)->GetValue());
        mcfg->Write(_T("/auto_show_search"),                 (bool) XRCCTRL(*this, "chkAutoShowMessagesOnSearch", wxCheckBox)->GetValue());
        mcfg->Write(_T("/auto_show_build_warnings"),         (bool) XRCCTRL(*this, "chkAutoShowMessagesOnWarn", wxCheckBox)->GetValue());
        mcfg->Write(_T("/auto_show_build_errors"),           (bool) XRCCTRL(*this, "chkAutoShowMessagesOnErr", wxCheckBox)->GetValue());
        cfg->Write(_T("/environment/start_here_page"),       (bool) XRCCTRL(*this, "chkShowStartPage", wxCheckBox)->GetValue());
        cfg->Write(_T("/environment/I18N"),                  (bool) XRCCTRL(*this, "chkI18N", wxCheckBox)->GetValue());
        cfg->Write(_T("/locale/language"),                   (int)  XRCCTRL(*this, "cbxLanguage", wxComboBox)->GetSelection()-1);
        mcfg->Write(_T("/log_font_size"),                    (int)  XRCCTRL(*this, "spnLogFontSize", wxSpinCtrl)->GetValue());
        cfg->Write(_T("/dialog_placement/do_place"),         (bool) XRCCTRL(*this, "chkDoPlace", wxCheckBox)->GetValue());
        cfg->Write(_T("/dialog_placement/dialog_position"),  (int)  XRCCTRL(*this, "chkPlaceHead", wxCheckBox)->GetValue() ? pdlHead : pdlCentre);

        // tab "Appearence"
        cfg->Write(_T("/environment/tabs_style"),           (int)XRCCTRL(*this, "cmbEditorTabs", wxComboBox)->GetSelection());
        cfg->Write(_T("/environment/tabs_smart"),           (bool)XRCCTRL(*this, "chkSmartTabs", wxCheckBox)->GetValue());
        cfg->Write(_T("/environment/gradient_border"),      XRCCTRL(*this, "btnFNBorder", wxButton)->GetBackgroundColour());
        cfg->Write(_T("/environment/gradient_from"),        XRCCTRL(*this, "btnFNFrom", wxButton)->GetBackgroundColour());
        cfg->Write(_T("/environment/gradient_to"),          XRCCTRL(*this, "btnFNTo", wxButton)->GetBackgroundColour());
        cfg->Write(_T("/environment/aui/border_size"),          (int)XRCCTRL(*this, "spnAuiBorder", wxSpinCtrl)->GetValue());
        cfg->Write(_T("/environment/aui/sash_size"),            (int)XRCCTRL(*this, "spnAuiSash", wxSpinCtrl)->GetValue());
        cfg->Write(_T("/environment/aui/caption_size"),         (int)XRCCTRL(*this, "spnAuiCaption", wxSpinCtrl)->GetValue());
        cfg->Write(_T("/environment/aui/bg_colour"),             XRCCTRL(*this, "btnAuiBgColour", wxButton)->GetBackgroundColour());
        cfg->Write(_T("/environment/aui/sash_colour"),           XRCCTRL(*this, "btnAuiSashColour", wxButton)->GetBackgroundColour());
        cfg->Write(_T("/environment/aui/active_caption_colour"),             XRCCTRL(*this, "btnAuiActiveCaptionColour", wxButton)->GetBackgroundColour());
        cfg->Write(_T("/environment/aui/active_caption_gradient_colour"),    XRCCTRL(*this, "btnAuiActiveCaptionGradientColour", wxButton)->GetBackgroundColour());
        cfg->Write(_T("/environment/aui/active_caption_text_colour"),        XRCCTRL(*this, "btnAuiActiveCaptionTextColour", wxButton)->GetBackgroundColour());
        cfg->Write(_T("/environment/aui/inactive_caption_colour"),           XRCCTRL(*this, "btnAuiInactiveCaptionColour", wxButton)->GetBackgroundColour());
        cfg->Write(_T("/environment/aui/inactive_caption_gradient_colour"),  XRCCTRL(*this, "btnAuiInactiveCaptionGradientColour", wxButton)->GetBackgroundColour());
        cfg->Write(_T("/environment/aui/inactive_caption_text_colour"),      XRCCTRL(*this, "btnAuiInactiveCaptionTextColour", wxButton)->GetBackgroundColour());
        cfg->Write(_T("/environment/aui/border_colour"),                     XRCCTRL(*this, "btnAuiBorderColour", wxButton)->GetBackgroundColour());
        cfg->Write(_T("/environment/aui/gripper_colour"),                    XRCCTRL(*this, "btnAuiGripperColour", wxButton)->GetBackgroundColour());

        m_pArt->SetMetric(wxAUI_ART_PANE_BORDER_SIZE,   XRCCTRL(*this, "spnAuiBorder", wxSpinCtrl)->GetValue());
        m_pArt->SetMetric(wxAUI_ART_SASH_SIZE,          XRCCTRL(*this, "spnAuiSash", wxSpinCtrl)->GetValue());
        m_pArt->SetMetric(wxAUI_ART_CAPTION_SIZE,       XRCCTRL(*this, "spnAuiCaption", wxSpinCtrl)->GetValue());
        m_pArt->SetColour(wxAUI_ART_BACKGROUND_COLOUR,   XRCCTRL(*this, "btnAuiBgColour", wxButton)->GetBackgroundColour());
        m_pArt->SetColour(wxAUI_ART_SASH_COLOUR,         XRCCTRL(*this, "btnAuiSashColour", wxButton)->GetBackgroundColour());
        m_pArt->SetColour(wxAUI_ART_ACTIVE_CAPTION_COLOUR,           XRCCTRL(*this, "btnAuiActiveCaptionColour", wxButton)->GetBackgroundColour());
        m_pArt->SetColour(wxAUI_ART_ACTIVE_CAPTION_GRADIENT_COLOUR,  XRCCTRL(*this, "btnAuiActiveCaptionGradientColour", wxButton)->GetBackgroundColour());
        m_pArt->SetColour(wxAUI_ART_ACTIVE_CAPTION_TEXT_COLOUR,      XRCCTRL(*this, "btnAuiActiveCaptionTextColour", wxButton)->GetBackgroundColour());
        m_pArt->SetColour(wxAUI_ART_INACTIVE_CAPTION_COLOUR,         XRCCTRL(*this, "btnAuiInactiveCaptionColour", wxButton)->GetBackgroundColour());
        m_pArt->SetColour(wxAUI_ART_INACTIVE_CAPTION_GRADIENT_COLOUR,XRCCTRL(*this, "btnAuiInactiveCaptionGradientColour", wxButton)->GetBackgroundColour());
        m_pArt->SetColour(wxAUI_ART_INACTIVE_CAPTION_TEXT_COLOUR,    XRCCTRL(*this, "btnAuiInactiveCaptionTextColour", wxButton)->GetBackgroundColour());
        m_pArt->SetColour(wxAUI_ART_BORDER_COLOUR,       XRCCTRL(*this, "btnAuiBorderColour", wxButton)->GetBackgroundColour());
        m_pArt->SetColour(wxAUI_ART_GRIPPER_COLOUR,      XRCCTRL(*this, "btnAuiGripperColour", wxButton)->GetBackgroundColour());

        // tab "Dialogs"
        wxCheckListBox* lb = XRCCTRL(*this, "chkDialogs", wxCheckListBox);

        ConfigManagerContainer::StringSet dialogs = acfg->ReadSSet(_T("/disabled"));

        for (int i = 0; i < lb->GetCount(); ++i)
        {
            if (lb->IsChecked(i))
                dialogs.erase(lb->GetString(i));
        }

        acfg->Write(_T("/disabled"), dialogs);

        // tab "Network"
        cfg->Write(_T("/network_proxy"),    XRCCTRL(*this, "txtProxy", wxTextCtrl)->GetValue());

        // finally, apply settings in all plugins' panels
        for (size_t i = 0; i < m_PluginPanels.GetCount(); ++i)
        {
            cbConfigurationPanel* panel = m_PluginPanels[i];
            panel->OnApply();
        }
    }
    else
    {
        // finally, cancel settings in all plugins' panels
        for (size_t i = 0; i < m_PluginPanels.GetCount(); ++i)
        {
            cbConfigurationPanel* panel = m_PluginPanels[i];
            panel->OnCancel();
        }
    }

    wxDialog::EndModal(retCode);
}

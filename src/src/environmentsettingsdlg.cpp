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
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include <wx/checklst.h>
#include <wx/radiobox.h>
#include <wx/spinctrl.h>
#include <wx/colordlg.h>
#include <wx/msgdlg.h>
#include "wxAUI/manager.h"
#include "appglobals.h"

#include "environmentsettingsdlg.h"
#ifdef __WXMSW__
    #include "associations.h"
#endif

// images by order of pages
const wxString base_imgs[] =
{
    _T("general-prefs"),
    _T("notebook-appearance"),
    _T("colours"),
    _T("dialogs"),
    _T("net"),
};
const int IMAGES_COUNT = sizeof(base_imgs) / sizeof(wxString);

BEGIN_EVENT_TABLE(EnvironmentSettingsDlg, wxDialog)
    EVT_UPDATE_UI(-1, EnvironmentSettingsDlg::OnUpdateUI)
    EVT_BUTTON(XRCID("btnSetAssocs"), EnvironmentSettingsDlg::OnSetAssocs)
    EVT_BUTTON(XRCID("btnFNBorder"), EnvironmentSettingsDlg::OnChooseColor)
    EVT_BUTTON(XRCID("btnFNFrom"), EnvironmentSettingsDlg::OnChooseColor)
    EVT_BUTTON(XRCID("btnFNTo"), EnvironmentSettingsDlg::OnChooseColor)
    EVT_BUTTON(XRCID("btnAuiBgColor"), EnvironmentSettingsDlg::OnChooseColor)
    EVT_BUTTON(XRCID("btnAuiSashColor"), EnvironmentSettingsDlg::OnChooseColor)
    EVT_BUTTON(XRCID("btnAuiCaptionColor"), EnvironmentSettingsDlg::OnChooseColor)
    EVT_BUTTON(XRCID("btnAuiCaptionTextColor"), EnvironmentSettingsDlg::OnChooseColor)
    EVT_BUTTON(XRCID("btnAuiBorderColor"), EnvironmentSettingsDlg::OnChooseColor)
    EVT_BUTTON(XRCID("btnAuiGripperColor"), EnvironmentSettingsDlg::OnChooseColor)
    EVT_BUTTON(XRCID("btnAuiActiveCaptionColor"), EnvironmentSettingsDlg::OnChooseColor)
    EVT_BUTTON(XRCID("btnAuiActiveCaptionGradientColor"), EnvironmentSettingsDlg::OnChooseColor)
    EVT_BUTTON(XRCID("btnAuiActiveCaptionTextColor"), EnvironmentSettingsDlg::OnChooseColor)
    EVT_BUTTON(XRCID("btnAuiInactiveCaptionColor"), EnvironmentSettingsDlg::OnChooseColor)
    EVT_BUTTON(XRCID("btnAuiInactiveCaptionGradientColor"), EnvironmentSettingsDlg::OnChooseColor)
    EVT_BUTTON(XRCID("btnAuiInactiveCaptionTextColor"), EnvironmentSettingsDlg::OnChooseColor)

    EVT_LISTBOOK_PAGE_CHANGING(XRCID("nbMain"), EnvironmentSettingsDlg::OnPageChanging)
    EVT_LISTBOOK_PAGE_CHANGED(XRCID("nbMain"), EnvironmentSettingsDlg::OnPageChanged)
END_EVENT_TABLE()

EnvironmentSettingsDlg::EnvironmentSettingsDlg(wxWindow* parent, wxDockArt* art)
    : m_pArt(art)
{
    wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgEnvironmentSettings"));
    LoadListbookImages();

    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("app"));
    ConfigManager *pcfg = Manager::Get()->GetConfigManager(_T("project_manager"));
    ConfigManager *mcfg = Manager::Get()->GetConfigManager(_T("message_manager"));
    ConfigManager *acfg = Manager::Get()->GetConfigManager(_T("an_dlg"));

    // tab "General"
    XRCCTRL(*this, "chkShowSplash", wxCheckBox)->SetValue(cfg->ReadBool(_T("/environment/show_splash"), true));
    XRCCTRL(*this, "chkDDE", wxCheckBox)->SetValue(cfg->ReadBool(_T("/environment/use_dde"), true));
    XRCCTRL(*this, "chkSingleInstance", wxCheckBox)->SetValue(cfg->ReadBool(_T("/environment/single_instance"), true));
    XRCCTRL(*this, "chkAssociations", wxCheckBox)->SetValue(cfg->ReadBool(_T("/environment/check_associations"), true));
    XRCCTRL(*this, "chkModifiedFiles", wxCheckBox)->SetValue(cfg->ReadBool(_T("/environment/check_modified_files"), true));
    XRCCTRL(*this, "chkDebugLog", wxCheckBox)->SetValue(mcfg->ReadBool(_T("/has_debug_log"), false));
    XRCCTRL(*this, "rbAppStart", wxRadioBox)->SetSelection(cfg->ReadBool(_T("/environment/blank_workspace"), true) ? 1 : 0);

    // tab "View"
    XRCCTRL(*this, "rbProjectOpen", wxRadioBox)->SetSelection(pcfg->ReadInt(_T("/open_files"), 1));
    XRCCTRL(*this, "rbToolbarSize", wxRadioBox)->SetSelection(cfg->ReadBool(_T("/environment/toolbar_size"), true) ? 1 : 0);
    XRCCTRL(*this, "chkAutoHideMessages", wxCheckBox)->SetValue(mcfg->ReadBool(_T("/auto_hide"), false));
    XRCCTRL(*this, "chkShowStartPage", wxCheckBox)->SetValue(cfg->ReadBool(_T("/environment/start_here_page"), true));

    // tab "Notebook"
    XRCCTRL(*this, "cmbEditorTabs", wxComboBox)->SetSelection(cfg->ReadInt(_T("/environment/tabs_style"), 0));
    XRCCTRL(*this, "btnFNBorder", wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/gradient_border"), wxColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW))));
    XRCCTRL(*this, "btnFNFrom", wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/gradient_from"), wxColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE))));
    XRCCTRL(*this, "btnFNTo", wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/gradient_to"), *wxWHITE));

    // tab "Docking"
    XRCCTRL(*this, "spnAuiBorder", wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/environment/aui/border_size"), m_pArt->GetMetric(wxAUI_ART_PANE_BORDER_SIZE)));
    XRCCTRL(*this, "spnAuiSash", wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/environment/aui/sash_size"), m_pArt->GetMetric(wxAUI_ART_SASH_SIZE)));
    XRCCTRL(*this, "spnAuiCaption", wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/environment/aui/caption_size"), m_pArt->GetMetric(wxAUI_ART_CAPTION_SIZE)));
    XRCCTRL(*this, "btnAuiBgColor", wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/aui/bg_color"), m_pArt->GetColor(wxAUI_ART_BACKGROUND_COLOUR)));
    XRCCTRL(*this, "btnAuiSashColor", wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/aui/sash_color"), m_pArt->GetColor(wxAUI_ART_SASH_COLOUR)));
    XRCCTRL(*this, "btnAuiActiveCaptionColor", wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/aui/active_caption_color"), m_pArt->GetColor(wxAUI_ART_ACTIVE_CAPTION_COLOUR)));
    XRCCTRL(*this, "btnAuiActiveCaptionGradientColor", wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/aui/active_caption_gradient_color"), m_pArt->GetColor(wxAUI_ART_ACTIVE_CAPTION_GRADIENT_COLOUR)));
    XRCCTRL(*this, "btnAuiActiveCaptionTextColor", wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/aui/active_caption_text_color"), m_pArt->GetColor(wxAUI_ART_ACTIVE_CAPTION_TEXT_COLOUR)));
    XRCCTRL(*this, "btnAuiInactiveCaptionColor", wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/aui/inactive_caption_color"), m_pArt->GetColor(wxAUI_ART_INACTIVE_CAPTION_COLOUR)));
    XRCCTRL(*this, "btnAuiInactiveCaptionGradientColor", wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/aui/inactive_caption_gradient_color"), m_pArt->GetColor(wxAUI_ART_INACTIVE_CAPTION_GRADIENT_COLOUR)));
    XRCCTRL(*this, "btnAuiInactiveCaptionTextColor", wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/aui/inactive_caption_text_color"), m_pArt->GetColor(wxAUI_ART_INACTIVE_CAPTION_TEXT_COLOUR)));
    XRCCTRL(*this, "btnAuiBorderColor", wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/aui/border_color"), m_pArt->GetColor(wxAUI_ART_BORDER_COLOUR)));
    XRCCTRL(*this, "btnAuiGripperColor", wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/aui/gripper_color"), m_pArt->GetColor(wxAUI_ART_GRIPPER_COLOUR)));

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
    XRCCTRL(*this, "chkDDE", wxCheckBox)->Enable(false);
    XRCCTRL(*this, "chkAssociations", wxCheckBox)->Enable(false);
    XRCCTRL(*this, "btnSetAssocs", wxButton)->Enable(false);
#endif

    // add all plugins configuration panels
    AddPluginPanels();

    // make sure everything is laid out properly
    GetSizer()->SetSizeHints(this);
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

        lb->GetImageList()->Add(LoadPNGWindows2000Hack(base + panel->GetBitmapBaseName() + _T(".png")));
        lb->GetImageList()->Add(LoadPNGWindows2000Hack(base + panel->GetBitmapBaseName() + _T("-off.png")));
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
        bmp.LoadFile(base + base_imgs[i] + _T(".png"), wxBITMAP_TYPE_PNG);
        images->Add(bmp);
        bmp.LoadFile(base + base_imgs[i] + _T("-off.png"), wxBITMAP_TYPE_PNG);
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

    // the selection color is ruining the on/off effect,
    // so make sure no item is selected ;)
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
    Associations::Set();
#endif
    wxMessageBox(_("Code::Blocks associated with C/C++ files."), _("Information"), wxICON_INFORMATION);
}

void EnvironmentSettingsDlg::OnChooseColor(wxCommandEvent& event)
{
	wxColourData data;
	wxWindow* sender = FindWindowById(event.GetId());
    data.SetColour(sender->GetBackgroundColour());

	wxColourDialog dlg(this, &data);
    if (dlg.ShowModal() == wxID_OK)
    {
    	wxColour color = dlg.GetColourData().GetColour();
	    sender->SetBackgroundColour(color);
    }
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
        cfg->Write(_T("/environment/use_dde"),               (bool) XRCCTRL(*this, "chkDDE", wxCheckBox)->GetValue());
        cfg->Write(_T("/environment/single_instance"),       (bool) XRCCTRL(*this, "chkSingleInstance", wxCheckBox)->GetValue());
        cfg->Write(_T("/environment/check_associations"),    (bool) XRCCTRL(*this, "chkAssociations", wxCheckBox)->GetValue());
        cfg->Write(_T("/environment/check_modified_files"),  (bool) XRCCTRL(*this, "chkModifiedFiles", wxCheckBox)->GetValue());
        mcfg->Write(_T("/has_debug_log"),                    (bool) XRCCTRL(*this, "chkDebugLog", wxCheckBox)->GetValue());

        // tab "View"
        cfg->Write(_T("/environment/blank_workspace"),       (bool) XRCCTRL(*this, "rbAppStart", wxRadioBox)->GetSelection() ? true : false);
        pcfg->Write(_T("/open_files"),                       (int)  XRCCTRL(*this, "rbProjectOpen", wxRadioBox)->GetSelection());
        cfg->Write(_T("/environment/toolbar_size"),          (bool) XRCCTRL(*this, "rbToolbarSize", wxRadioBox)->GetSelection() == 1);
        mcfg->Write(_T("/auto_hide"),                        (bool) XRCCTRL(*this, "chkAutoHideMessages", wxCheckBox)->GetValue());
        cfg->Write(_T("/environment/start_here_page"),       (bool) XRCCTRL(*this, "chkShowStartPage", wxCheckBox)->GetValue());

        // tab "Appearence"
        cfg->Write(_T("/environment/tabs_style"),           (int)XRCCTRL(*this, "cmbEditorTabs", wxComboBox)->GetSelection());
        cfg->Write(_T("/environment/gradient_border"),      XRCCTRL(*this, "btnFNBorder", wxButton)->GetBackgroundColour());
        cfg->Write(_T("/environment/gradient_from"),        XRCCTRL(*this, "btnFNFrom", wxButton)->GetBackgroundColour());
        cfg->Write(_T("/environment/gradient_to"),          XRCCTRL(*this, "btnFNTo", wxButton)->GetBackgroundColour());
        cfg->Write(_T("/environment/aui/border_size"),          (int)XRCCTRL(*this, "spnAuiBorder", wxSpinCtrl)->GetValue());
        cfg->Write(_T("/environment/aui/sash_size"),            (int)XRCCTRL(*this, "spnAuiSash", wxSpinCtrl)->GetValue());
        cfg->Write(_T("/environment/aui/caption_size"),         (int)XRCCTRL(*this, "spnAuiCaption", wxSpinCtrl)->GetValue());
        cfg->Write(_T("/environment/aui/bg_color"),             XRCCTRL(*this, "btnAuiBgColor", wxButton)->GetBackgroundColour());
        cfg->Write(_T("/environment/aui/sash_color"),           XRCCTRL(*this, "btnAuiSashColor", wxButton)->GetBackgroundColour());
        cfg->Write(_T("/environment/aui/active_caption_color"),             XRCCTRL(*this, "btnAuiActiveCaptionColor", wxButton)->GetBackgroundColour());
        cfg->Write(_T("/environment/aui/active_caption_gradient_color"),    XRCCTRL(*this, "btnAuiActiveCaptionGradientColor", wxButton)->GetBackgroundColour());
        cfg->Write(_T("/environment/aui/active_caption_text_color"),        XRCCTRL(*this, "btnAuiActiveCaptionTextColor", wxButton)->GetBackgroundColour());
        cfg->Write(_T("/environment/aui/inactive_caption_color"),           XRCCTRL(*this, "btnAuiInactiveCaptionColor", wxButton)->GetBackgroundColour());
        cfg->Write(_T("/environment/aui/inactive_caption_gradient_color"),  XRCCTRL(*this, "btnAuiInactiveCaptionGradientColor", wxButton)->GetBackgroundColour());
        cfg->Write(_T("/environment/aui/inactive_caption_text_color"),      XRCCTRL(*this, "btnAuiInactiveCaptionTextColor", wxButton)->GetBackgroundColour());
        cfg->Write(_T("/environment/aui/border_color"),                     XRCCTRL(*this, "btnAuiBorderColor", wxButton)->GetBackgroundColour());
        cfg->Write(_T("/environment/aui/gripper_color"),                    XRCCTRL(*this, "btnAuiGripperColor", wxButton)->GetBackgroundColour());

        m_pArt->SetMetric(wxAUI_ART_PANE_BORDER_SIZE,   XRCCTRL(*this, "spnAuiBorder", wxSpinCtrl)->GetValue());
        m_pArt->SetMetric(wxAUI_ART_SASH_SIZE,          XRCCTRL(*this, "spnAuiSash", wxSpinCtrl)->GetValue());
        m_pArt->SetMetric(wxAUI_ART_CAPTION_SIZE,       XRCCTRL(*this, "spnAuiCaption", wxSpinCtrl)->GetValue());
        m_pArt->SetColor(wxAUI_ART_BACKGROUND_COLOUR,   XRCCTRL(*this, "btnAuiBgColor", wxButton)->GetBackgroundColour());
        m_pArt->SetColor(wxAUI_ART_SASH_COLOUR,         XRCCTRL(*this, "btnAuiSashColor", wxButton)->GetBackgroundColour());
        m_pArt->SetColor(wxAUI_ART_ACTIVE_CAPTION_COLOUR,           XRCCTRL(*this, "btnAuiActiveCaptionColor", wxButton)->GetBackgroundColour());
        m_pArt->SetColor(wxAUI_ART_ACTIVE_CAPTION_GRADIENT_COLOUR,  XRCCTRL(*this, "btnAuiActiveCaptionGradientColor", wxButton)->GetBackgroundColour());
        m_pArt->SetColor(wxAUI_ART_ACTIVE_CAPTION_TEXT_COLOUR,      XRCCTRL(*this, "btnAuiActiveCaptionTextColor", wxButton)->GetBackgroundColour());
        m_pArt->SetColor(wxAUI_ART_INACTIVE_CAPTION_COLOUR,         XRCCTRL(*this, "btnAuiInactiveCaptionColor", wxButton)->GetBackgroundColour());
        m_pArt->SetColor(wxAUI_ART_INACTIVE_CAPTION_GRADIENT_COLOUR,XRCCTRL(*this, "btnAuiInactiveCaptionGradientColor", wxButton)->GetBackgroundColour());
        m_pArt->SetColor(wxAUI_ART_INACTIVE_CAPTION_TEXT_COLOUR,    XRCCTRL(*this, "btnAuiInactiveCaptionTextColor", wxButton)->GetBackgroundColour());
        m_pArt->SetColor(wxAUI_ART_BORDER_COLOUR,       XRCCTRL(*this, "btnAuiBorderColor", wxButton)->GetBackgroundColour());
        m_pArt->SetColor(wxAUI_ART_GRIPPER_COLOUR,      XRCCTRL(*this, "btnAuiGripperColor", wxButton)->GetBackgroundColour());

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

/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>

#ifndef CB_PRECOMP
    #include <wx/button.h>
    #include <wx/menu.h>
    #include <wx/radiobut.h>
    #include <wx/xrc/xmlres.h>
    #include <wx/intl.h>
    #include <wx/listctrl.h>
    #include <wx/combobox.h>
    #include <wx/choice.h>
    #include <wx/checkbox.h>
    #include <wx/checklst.h>
    #include <wx/radiobox.h>
    #include <wx/spinctrl.h>
    #include <wx/colordlg.h>
    #include <wx/msgdlg.h>
    #include <wx/imaglist.h>
    #include <wx/settings.h>
    #include <wx/stattext.h>

    #include <manager.h>
    #include <configmanager.h>
    #include <editormanager.h>
    #include <pluginmanager.h>
    #include "appglobals.h"
    #include "globals.h"
    #include "associations.h"
    #include "cbauibook.h"
#endif

#include <wx/aui/aui.h>
#include <wx/listbook.h>

#include "annoyingdialog.h"
#include "configurationpanel.h"
#include "environmentsettingsdlg.h"
#include "cbcolourmanager.h"

#ifdef __WXMSW__
    #include "associations.h"
#endif

#ifndef CB_PRECOMP
    #include <wx/dir.h>
    #include "cbplugin.h" // cgCompiler...
#endif

// images by order of pages
const wxString base_imgs[] =
{
    _T("general-prefs"),
    _T("view"),
    _T("notebook-appearance"),
    _T("colours"),
    _T("colours"),
    _T("dialogs"),
    _T("net")
};
const int IMAGES_COUNT = sizeof(base_imgs) / sizeof(wxString);

BEGIN_EVENT_TABLE(EnvironmentSettingsDlg, wxScrollingDialog)
    EVT_BUTTON(XRCID("btnSetAssocs"), EnvironmentSettingsDlg::OnSetAssocs)
    EVT_BUTTON(XRCID("btnManageAssocs"), EnvironmentSettingsDlg::OnManageAssocs)
    EVT_BUTTON(XRCID("btnAuiCaptionColour"), EnvironmentSettingsDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnAuiCaptionTextColour"), EnvironmentSettingsDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnAuiActiveCaptionColour"), EnvironmentSettingsDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnAuiActiveCaptionGradientColour"), EnvironmentSettingsDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnAuiActiveCaptionTextColour"), EnvironmentSettingsDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnAuiInactiveCaptionColour"), EnvironmentSettingsDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnAuiInactiveCaptionGradientColour"), EnvironmentSettingsDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnAuiInactiveCaptionTextColour"), EnvironmentSettingsDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnResetDefaultColours"), EnvironmentSettingsDlg::OnResetDefaultColours)
    EVT_CHECKBOX(XRCID("chkUseIPC"), EnvironmentSettingsDlg::OnUseIpcCheck)
    EVT_CHECKBOX(XRCID("chkDoPlace"), EnvironmentSettingsDlg::OnPlaceCheck)
    EVT_CHECKBOX(XRCID("chkPlaceHead"), EnvironmentSettingsDlg::OnHeadCheck)
    EVT_CHECKBOX(XRCID("chkAutoHideMessages"), EnvironmentSettingsDlg::OnAutoHide)
    EVT_CHECKBOX(XRCID("chkI18N"), EnvironmentSettingsDlg::OnI18NCheck)
    EVT_RADIOBOX(XRCID("rbSettingsIconsSize"), EnvironmentSettingsDlg::OnSettingsIconsSize)
    EVT_CHECKBOX(XRCID("chkDblClkMaximizes"), EnvironmentSettingsDlg::OnDblClickMaximizes)
    EVT_CHECKBOX(XRCID("chkNBUseMousewheel"), EnvironmentSettingsDlg::OnUseTabMousewheel)

    EVT_CHOICE(XRCID("chCategory"), EnvironmentSettingsDlg::OnChooseAppColourCategory)
    EVT_LISTBOX(XRCID("lstColours"), EnvironmentSettingsDlg::OnChooseAppColourItem)
    EVT_BUTTON(XRCID("btnColour"), EnvironmentSettingsDlg::OnClickAppColour)
    EVT_BUTTON(XRCID("btnDefaultColour"), EnvironmentSettingsDlg::OnClickAppColour)
END_EVENT_TABLE()

EnvironmentSettingsDlg::EnvironmentSettingsDlg(wxWindow* parent, wxAuiDockArt* art)
    : m_pArt(art)
{
    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("app"));
    ConfigManager *pcfg = Manager::Get()->GetConfigManager(_T("project_manager"));
    ConfigManager *mcfg = Manager::Get()->GetConfigManager(_T("message_manager"));
    ConfigManager *acfg = Manager::Get()->GetConfigManager(_T("an_dlg"));

    wxXmlResource::Get()->LoadObject(this, parent, _T("dlgEnvironmentSettings"),_T("wxScrollingDialog"));
    int sel = cfg->ReadInt(_T("/environment/settings_size"), 0);
    wxListbook* lb = XRCCTRL(*this, "nbMain", wxListbook);
    SetSettingsIconsStyle(lb->GetListView(), (SettingsIconsStyle)sel);
    LoadListbookImages();

    Connect(XRCID("nbMain"),wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING,wxListbookEventHandler(EnvironmentSettingsDlg::OnPageChanging));
    Connect(XRCID("nbMain"),wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED, wxListbookEventHandler(EnvironmentSettingsDlg::OnPageChanged ));

    // this setting is not available under wxGTK
    #ifndef __WXMSW__
    XRCCTRL(*this, "rbSettingsIconsSize", wxRadioBox)->Enable(false);
    #endif

    // tab "General"
    XRCCTRL(*this, "chkShowSplash", wxCheckBox)->SetValue(cfg->ReadBool(_T("/environment/show_splash"), true));
    XRCCTRL(*this, "chkSingleInstance", wxCheckBox)->SetValue(cfg->ReadBool(_T("/environment/single_instance"), true));
#ifdef __WXMSW__
    static_cast<wxStaticBoxSizer*>(XRCCTRL(*this, "chkUseIPC", wxCheckBox)->GetContainingSizer())->GetStaticBox()->SetLabel(_("Dynamic Data Exchange (will take place after restart)"));
#endif
    bool useIpc = cfg->ReadBool(_T("/environment/use_ipc"), true);
    XRCCTRL(*this, "chkUseIPC",      wxCheckBox)->SetValue(useIpc);
    XRCCTRL(*this, "chkRaiseViaIPC", wxCheckBox)->SetValue(cfg->ReadBool(_T("/environment/raise_via_ipc"), true));
    XRCCTRL(*this, "chkRaiseViaIPC", wxCheckBox)->Enable(useIpc);

    XRCCTRL(*this, "chkAssociations",       wxCheckBox)->SetValue(cfg->ReadBool(_T("/environment/check_associations"),          true));
    XRCCTRL(*this, "chkModifiedFiles",      wxCheckBox)->SetValue(cfg->ReadBool(_T("/environment/check_modified_files"),        true));
    XRCCTRL(*this, "chkInvalidTargets",     wxCheckBox)->SetValue(cfg->ReadBool(_T("/environment/ignore_invalid_targets"),      true));
    XRCCTRL(*this, "rbAppStart", wxRadioBox)->SetSelection(cfg->ReadBool(_T("/environment/blank_workspace"), true) ? 1 : 0);

    wxTextCtrl* txt = XRCCTRL(*this, "txtConsoleShell", wxTextCtrl);
    txt->SetValue(cfg->Read(_T("/console_shell"), DEFAULT_CONSOLE_SHELL));
#ifdef __WXMSW__
    // under win32, this option is not needed, so disable it
    txt->Enable(false);
#endif

    wxComboBox *combo = XRCCTRL(*this, "cbConsoleTerm", wxComboBox);
    combo->Append(DEFAULT_CONSOLE_TERM);

    if (platform::id == platform::windows)
        combo->Enable(false);
    else
    {
        if (platform::id != platform::macosx && platform::id != platform::darwin)
        {
            combo->Append(wxT("gnome-terminal --disable-factory -t $TITLE -x "));
            combo->Append(wxT("konsole -e "));
            combo->Append(wxT("xfce4-terminal -T $TITLE -x "));
            combo->Append(wxT("terminology -M -T $TITLE -e "));
        }
        wxString terminal = cfg->Read(wxT("/console_terminal"), DEFAULT_CONSOLE_TERM);
        if (!combo->SetStringSelection(terminal))
        {
            combo->Insert(terminal, 0);
            combo->SetStringSelection(terminal);
        }
    }

    // tab "View"
    bool do_place = cfg->ReadBool(_T("/dialog_placement/do_place"), false);
    XRCCTRL(*this, "chkDoPlace", wxCheckBox)->SetValue(do_place);
    XRCCTRL(*this, "chkPlaceHead", wxCheckBox)->SetValue(cfg->ReadInt(_T("/dialog_placement/dialog_position"), 0) == pdlHead ? 1 : 0);
    XRCCTRL(*this, "chkPlaceHead", wxCheckBox)->Enable(do_place);

    XRCCTRL(*this, "rbProjectOpen",           wxRadioBox)->SetSelection(pcfg->ReadInt(_T("/open_files"), 1));
    XRCCTRL(*this, "rbToolbarSize",           wxRadioBox)->SetSelection(cfg->ReadBool(_T("/environment/toolbar_size"), true) ? 1 : 0);
    XRCCTRL(*this, "rbSettingsIconsSize",     wxRadioBox)->SetSelection(cfg->ReadInt(_T("/environment/settings_size"), 0));
    XRCCTRL(*this, "chkShowStartPage",        wxCheckBox)->SetValue(cfg->ReadBool(_T("/environment/start_here_page"), true));
    XRCCTRL(*this, "spnLogFontSize",          wxSpinCtrl)->SetValue(mcfg->ReadInt(_T("/log_font_size"), 8));

    bool en = mcfg->ReadBool(_T("/auto_hide"), false);
    XRCCTRL(*this, "chkAutoHideMessages",         wxCheckBox)->SetValue(en);
    XRCCTRL(*this, "chkAutoShowMessagesOnSearch", wxCheckBox)->SetValue(mcfg->ReadBool(_T("/auto_show_search"), true));
    XRCCTRL(*this, "chkAutoShowMessagesOnWarn",   wxCheckBox)->SetValue(mcfg->ReadBool(_T("/auto_show_build_warnings"), true));
    XRCCTRL(*this, "chkAutoShowMessagesOnErr",    wxCheckBox)->SetValue(mcfg->ReadBool(_T("/auto_show_build_errors"), true));
    XRCCTRL(*this, "chkAutoShowMessagesOnSearch", wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkAutoShowMessagesOnWarn",   wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkAutoShowMessagesOnErr",    wxCheckBox)->Enable(en);

    XRCCTRL(*this, "chkSaveSelectionChangeInMP", wxCheckBox)->SetValue(mcfg->ReadBool(_T("/save_selection_change_in_mp"), true));

    en = cfg->ReadBool(_T("/environment/view/dbl_clk_maximize"), true);
     XRCCTRL(*this, "chkDblClkMaximizes", wxCheckBox)->SetValue(en);
    int idx = Manager::Get()->GetAppFrame()->GetMenuBar()->FindMenu(_("&View"));
    if (idx != wxNOT_FOUND)
    {
        wxMenu* menuView = Manager::Get()->GetAppFrame()->GetMenuBar()->GetMenu(idx);
        int sub_idx = menuView->FindItem(_("Perspectives"));
        if (sub_idx != wxNOT_FOUND)
        {
            wxMenu* menuLayouts = menuView->FindItem(sub_idx)->GetSubMenu();
            if (menuLayouts)
            {
                wxMenuItemList& items = menuLayouts->GetMenuItems();
                for (size_t i = 0; i < items.GetCount() && ! items[i]->IsSeparator() ; ++i)
                {
#if wxCHECK_VERSION(2,8,5)
                    XRCCTRL(*this, "choLayoutToToggle", wxChoice)->Append(items[i]->GetLabelText(items[i]->GetItemLabelText()));
#else
                    XRCCTRL(*this, "choLayoutToToggle", wxChoice)->Append(items[i]->GetLabelFromText(items[i]->GetLabel()));
#endif
                }
            }
        }
    }

    sel = XRCCTRL(*this, "choLayoutToToggle", wxChoice)->FindString( cfg->Read(_T("/environment/view/layout_to_toggle"),cfg->Read(_T("/main_frame/layout/default"))));
    XRCCTRL(*this, "choLayoutToToggle", wxChoice)->SetSelection(sel != wxNOT_FOUND ? sel : 0);
    XRCCTRL(*this, "choLayoutToToggle", wxChoice)->Enable(en);

    bool i18n = cfg->ReadBool(_T("/locale/enable"), false);
        XRCCTRL(*this, "chkI18N", wxCheckBox)->SetValue(i18n);

    wxString locPath = ConfigManager::GetDataFolder() + _T("/locale");
    if ( wxDirExists(locPath) )
    {
        wxString locFName;
        wxDir    locDir(locPath);
        if ( locDir.IsOpened() && locDir.GetFirst(&locFName/*, wxEmptyString, wxDIR_DIRS*/) )
        {
            do
            {
                const wxLanguageInfo* info = wxLocale::FindLanguageInfo(locFName);
                if (info)
                    XRCCTRL(*this, "cbxLanguage", wxComboBox)->Append(info->Description);
            } while ( locDir.GetNext(&locFName) );
        }
    }

    XRCCTRL(*this, "cbxLanguage", wxComboBox)->Enable(i18n);

    const wxLanguageInfo* info = wxLocale::FindLanguageInfo(cfg->Read(_T("/locale/language")));
    if (info)
        XRCCTRL(*this, "cbxLanguage", wxComboBox)->SetStringSelection(info->Description);


    // tab "Notebook"
    XRCCTRL(*this, "cmbEditorTabs",               wxChoice)->SetSelection(cfg->ReadInt(_T("/environment/tabs_style"), 0));
    XRCCTRL(*this, "cmbTabCloseStyle",            wxChoice)->SetSelection(cfg->ReadInt(_T("/environment/tabs_closestyle"), 0));
    XRCCTRL(*this, "chkListTabs",                 wxCheckBox)->SetValue(cfg->ReadBool(_T("/environment/tabs_list"), 0));
    XRCCTRL(*this, "chkStackedBasedTabSwitching", wxCheckBox)->SetValue(cfg->ReadBool(_T("/environment/tabs_stacked_based_switching"), 0));
    bool enableTabMousewheel = cfg->ReadBool(_T("/environment/tabs_use_mousewheel"),true);
    bool modToAdvance = cfg->ReadBool(_T("/environment/tabs_mousewheel_advance"),false);
    XRCCTRL(*this, "chkNBUseMousewheel",          wxCheckBox)->SetValue(enableTabMousewheel);
    XRCCTRL(*this, "rbNBModToAdvance",            wxRadioButton)->SetValue(modToAdvance);
    XRCCTRL(*this, "rbNBModToMove",               wxRadioButton)->SetValue(!modToAdvance);
    XRCCTRL(*this, "chkNBInvertAdvance",          wxCheckBox)->SetValue(cfg->ReadBool(_T("/environment/tabs_invert_advance"),false));
    XRCCTRL(*this, "chkNBInvertMove",             wxCheckBox)->SetValue(cfg->ReadBool(_T("/environment/tabs_invert_move"),false));
    XRCCTRL(*this, "txtMousewheelModifier",       wxTextCtrl)->SetValue(cfg->Read(_T("/environment/tabs_mousewheel_modifier"),_T("Ctrl")));
    XRCCTRL(*this, "txtMousewheelModifier",       wxTextCtrl)->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(EnvironmentSettingsDlg::OnMousewheelModifier));
    XRCCTRL(*this, "rbNBModToAdvance",            wxRadioButton)->Enable(enableTabMousewheel);
    XRCCTRL(*this, "rbNBModToMove",               wxRadioButton)->Enable(enableTabMousewheel);
    XRCCTRL(*this, "chkNBInvertAdvance",          wxCheckBox)->Enable(enableTabMousewheel);
    XRCCTRL(*this, "chkNBInvertMove",             wxCheckBox)->Enable(enableTabMousewheel);
    XRCCTRL(*this, "txtMousewheelModifier",       wxTextCtrl)->Enable(enableTabMousewheel);

    // tab "Docking"
    XRCCTRL(*this, "spnAuiBorder",                        wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/environment/aui/border_size"), m_pArt->GetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE)));
    XRCCTRL(*this, "spnAuiSash",                          wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/environment/aui/sash_size"), m_pArt->GetMetric(wxAUI_DOCKART_SASH_SIZE)));
    XRCCTRL(*this, "spnAuiCaption",                       wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/environment/aui/caption_size"), m_pArt->GetMetric(wxAUI_DOCKART_CAPTION_SIZE)));
    XRCCTRL(*this, "btnAuiActiveCaptionColour",           wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/aui/active_caption_colour"), m_pArt->GetColour(wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR)));
    XRCCTRL(*this, "btnAuiActiveCaptionGradientColour",   wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/aui/active_caption_gradient_colour"), m_pArt->GetColour(wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR)));
    XRCCTRL(*this, "btnAuiActiveCaptionTextColour",       wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/aui/active_caption_text_colour"), m_pArt->GetColour(wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR)));
    XRCCTRL(*this, "btnAuiInactiveCaptionColour",         wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/aui/inactive_caption_colour"), m_pArt->GetColour(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR)));
    XRCCTRL(*this, "btnAuiInactiveCaptionGradientColour", wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/aui/inactive_caption_gradient_colour"), m_pArt->GetColour(wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR)));
    XRCCTRL(*this, "btnAuiInactiveCaptionTextColour",     wxButton)->SetBackgroundColour(cfg->ReadColour(_T("/environment/aui/inactive_caption_text_colour"), m_pArt->GetColour(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR)));

    // tab "Dialogs"
    wxCheckListBox* clb = XRCCTRL(*this, "chkDialogs", wxCheckListBox);
    clb->Clear();

    m_AnnoyingDlgReturnMap[F(wxT("%d"), AnnoyingDialog::rtOK)]     = _("OK");
    m_AnnoyingDlgReturnMap[F(wxT("%d"), AnnoyingDialog::rtCANCEL)] = _("Cancel");
    m_AnnoyingDlgReturnMap[F(wxT("%d"), AnnoyingDialog::rtYES)]    = _("Yes");
    m_AnnoyingDlgReturnMap[F(wxT("%d"), AnnoyingDialog::rtNO)]     = _("No");
    ConfigManagerContainer::StringSet dialogs;
    if (acfg->Exists(wxT("/disabled_ret")))
    {
        // new config style
        dialogs = acfg->ReadSSet(wxT("/disabled_ret"));
    }
    else
    {
        // if the new config key does not exist, read from the old one
        dialogs = acfg->ReadSSet(wxT("/disabled"));
        // and copy it to the new one
        acfg->Write(wxT("/disabled_ret"), dialogs);
        // we do not do an in place upgrade of the format to maintain
        // compatibility with previous versions
    }
    for (ConfigManagerContainer::StringSet::iterator i = dialogs.begin(); i != dialogs.end(); ++i)
        clb->Append(AnnoyingDlgReturnToString(*i));

    // tab "Network"
    XRCCTRL(*this, "txtProxy", wxTextCtrl)->SetValue(cfg->Read(_T("/network_proxy")));

    FillApplicationColours();

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
    const wxString base = _T("images/settings/");
    // for plugins who do not supply icons, use common generic icons
    const wxString noimg = _T("images/settings/generic-plugin");

    wxListbook* lb = XRCCTRL(*this, "nbMain", wxListbook);
    // get all configuration panels which are *not* about compiler and editor.
    Manager::Get()->GetPluginManager()->GetConfigurationPanels(~(cgCompiler | cgEditor), lb, m_PluginPanels);

    for (size_t i = 0; i < m_PluginPanels.GetCount(); ++i)
    {
        cbConfigurationPanel* panel = m_PluginPanels[i];
        panel->SetParentDialog(this);
        lb->AddPage(panel, panel->GetTitle());

        wxString onFile = ConfigManager::LocateDataFile(base + panel->GetBitmapBaseName() + _T(".png"), sdDataGlobal | sdDataUser);
        if (onFile.IsEmpty())
            onFile = ConfigManager::LocateDataFile(noimg + _T(".png"), sdDataGlobal | sdDataUser);
        wxString offFile = ConfigManager::LocateDataFile(base + panel->GetBitmapBaseName() + _T("-off.png"), sdDataGlobal | sdDataUser);
        if (offFile.IsEmpty())
            offFile = ConfigManager::LocateDataFile(noimg + _T("-off.png"), sdDataGlobal | sdDataUser);

        lb->GetImageList()->Add(cbLoadBitmap(onFile));
        lb->GetImageList()->Add(cbLoadBitmap(offFile));
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
        lb->SetPageImage(i, (i * 2) + (sel == (int)i ? 0 : 1));

    // update the page title
    wxString label = lb->GetPageText(sel);
    // replace any stray & with && because label makes it an underscore
    while (label.Replace(_T(" & "), _T(" && ")))
        ;
    XRCCTRL(*this, "lblBigTitle", wxStaticText)->SetLabel(label);
    XRCCTRL(*this, "pnlTitleInfo", wxPanel)->Layout();
}

void EnvironmentSettingsDlg::OnPageChanging(cb_unused wxListbookEvent& event)
{
}

void EnvironmentSettingsDlg::OnPageChanged(wxListbookEvent& event)
{
    // update only on real change, not on dialog creation
    if (event.GetOldSelection() != -1 && event.GetSelection() != -1)
        UpdateListbookImages();
}

void EnvironmentSettingsDlg::OnSetAssocs(cb_unused wxCommandEvent& event)
{
#ifdef __WXMSW__
    Associations::SetCore();
    //cbMessageBox(_("Code::Blocks associated with C/C++ files."), _("Information"), wxICON_INFORMATION, this);
#endif
}

void EnvironmentSettingsDlg::OnManageAssocs(cb_unused wxCommandEvent& event)
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

void EnvironmentSettingsDlg::OnResetDefaultColours(cb_unused wxCommandEvent& event)
{
    wxAuiDockArt* art = new wxAuiDefaultDockArt;

    XRCCTRL(*this, "spnAuiBorder", wxSpinCtrl)->SetValue(art->GetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE));
    XRCCTRL(*this, "spnAuiSash", wxSpinCtrl)->SetValue(art->GetMetric(wxAUI_DOCKART_SASH_SIZE));
    XRCCTRL(*this, "spnAuiCaption", wxSpinCtrl)->SetValue(art->GetMetric(wxAUI_DOCKART_CAPTION_SIZE));
    XRCCTRL(*this, "btnAuiActiveCaptionColour", wxButton)->SetBackgroundColour(art->GetColour(wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR));
    XRCCTRL(*this, "btnAuiActiveCaptionGradientColour", wxButton)->SetBackgroundColour(art->GetColour(wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR));
    XRCCTRL(*this, "btnAuiActiveCaptionTextColour", wxButton)->SetBackgroundColour(art->GetColour(wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR));
    XRCCTRL(*this, "btnAuiInactiveCaptionColour", wxButton)->SetBackgroundColour(art->GetColour(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR));
    XRCCTRL(*this, "btnAuiInactiveCaptionGradientColour", wxButton)->SetBackgroundColour(art->GetColour(wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR));
    XRCCTRL(*this, "btnAuiInactiveCaptionTextColour", wxButton)->SetBackgroundColour(art->GetColour(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR));

    delete art;
}

void EnvironmentSettingsDlg::OnAutoHide(cb_unused wxCommandEvent& event)
{
    bool en = XRCCTRL(*this, "chkAutoHideMessages", wxCheckBox)->GetValue();
    XRCCTRL(*this, "chkAutoShowMessagesOnSearch",   wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkAutoShowMessagesOnWarn",     wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkAutoShowMessagesOnErr",      wxCheckBox)->Enable(en);
}

void EnvironmentSettingsDlg::OnUseIpcCheck(wxCommandEvent& event)
{
    XRCCTRL(*this, "chkRaiseViaIPC", wxCheckBox)->Enable(event.IsChecked());
}

void EnvironmentSettingsDlg::OnDblClickMaximizes(cb_unused wxCommandEvent& event)
{
    bool en = XRCCTRL(*this, "chkDblClkMaximizes", wxCheckBox)->GetValue();
    XRCCTRL(*this, "choLayoutToToggle", wxCheckBox)->Enable(en);
}

void EnvironmentSettingsDlg::OnMousewheelModifier(cb_unused wxKeyEvent& event)
{
    wxString keys;

    if (wxGetKeyState(WXK_SHIFT))
        keys += keys.IsEmpty()?wxT("Shift"):wxT("+Shift");

    if (wxGetKeyState(WXK_CONTROL))
        keys += keys.IsEmpty()?wxT("Ctrl"):wxT("+Ctrl");

#if defined(__WXMAC__) || defined(__WXCOCOA__)
    if (wxGetKeyState(WXK_COMMAND))
        keys += keys.IsEmpty()?wxT("XCtrl"):wxT("+XCtrl");
#endif

    if (wxGetKeyState(WXK_ALT))
        keys += keys.IsEmpty()?wxT("Alt"):wxT("+Alt");

    if (!keys.IsEmpty())
        XRCCTRL(*this, "txtMousewheelModifier", wxTextCtrl)->SetValue(keys);
}

void EnvironmentSettingsDlg::OnUseTabMousewheel(cb_unused wxCommandEvent& event)
{
    bool en = (bool)XRCCTRL(*this, "chkNBUseMousewheel",wxCheckBox)->GetValue();
    XRCCTRL(*this, "rbNBModToAdvance", wxRadioButton)->Enable(en);
    XRCCTRL(*this, "rbNBModToMove", wxRadioButton)->Enable(en);
    XRCCTRL(*this, "chkNBInvertAdvance", wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkNBInvertMove", wxCheckBox)->Enable(en);
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

void EnvironmentSettingsDlg::EndModal(int retCode)
{
    if (retCode == wxID_OK)
    {
        ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("app"));
        ConfigManager *pcfg = Manager::Get()->GetConfigManager(_T("project_manager"));
        ConfigManager *mcfg = Manager::Get()->GetConfigManager(_T("message_manager"));
        ConfigManager *acfg = Manager::Get()->GetConfigManager(_T("an_dlg"));

        // tab "General"
        cfg->Write(_T("/environment/show_splash"),                 (bool) XRCCTRL(*this, "chkShowSplash",         wxCheckBox)->GetValue());
        cfg->Write(_T("/environment/single_instance"),             (bool) XRCCTRL(*this, "chkSingleInstance",     wxCheckBox)->GetValue());
        cfg->Write(_T("/environment/use_ipc"),                     (bool) XRCCTRL(*this, "chkUseIPC",             wxCheckBox)->GetValue());
        cfg->Write(_T("/environment/raise_via_ipc"),               (bool) XRCCTRL(*this, "chkRaiseViaIPC",        wxCheckBox)->GetValue());
        cfg->Write(_T("/environment/check_associations"),          (bool) XRCCTRL(*this, "chkAssociations",       wxCheckBox)->GetValue());
        cfg->Write(_T("/environment/check_modified_files"),        (bool) XRCCTRL(*this, "chkModifiedFiles",      wxCheckBox)->GetValue());
        cfg->Write(_T("/environment/ignore_invalid_targets"),      (bool) XRCCTRL(*this, "chkInvalidTargets",     wxCheckBox)->GetValue());
        cfg->Write(_T("/console_shell"),                                  XRCCTRL(*this, "txtConsoleShell",       wxTextCtrl)->GetValue());
        cfg->Write(_T("/console_terminal"),                               XRCCTRL(*this, "cbConsoleTerm",         wxComboBox)->GetValue());

        // tab "View"
        cfg->Write(_T("/environment/blank_workspace"),       (bool) XRCCTRL(*this, "rbAppStart", wxRadioBox)->GetSelection() ? true : false);
        pcfg->Write(_T("/open_files"),                       (int)  XRCCTRL(*this, "rbProjectOpen", wxRadioBox)->GetSelection());
        cfg->Write(_T("/environment/toolbar_size"),          (bool) XRCCTRL(*this, "rbToolbarSize", wxRadioBox)->GetSelection() == 1);
        cfg->Write(_T("/environment/settings_size"),         (int)  XRCCTRL(*this, "rbSettingsIconsSize", wxRadioBox)->GetSelection());
        mcfg->Write(_T("/auto_hide"),                        (bool) XRCCTRL(*this, "chkAutoHideMessages", wxCheckBox)->GetValue());
        mcfg->Write(_T("/auto_show_search"),                 (bool) XRCCTRL(*this, "chkAutoShowMessagesOnSearch", wxCheckBox)->GetValue());
        mcfg->Write(_T("/auto_show_build_warnings"),         (bool) XRCCTRL(*this, "chkAutoShowMessagesOnWarn", wxCheckBox)->GetValue());
        mcfg->Write(_T("/auto_show_build_errors"),           (bool) XRCCTRL(*this, "chkAutoShowMessagesOnErr", wxCheckBox)->GetValue());
        mcfg->Write(_T("/save_selection_change_in_mp"),       (bool) XRCCTRL(*this, "chkSaveSelectionChangeInMP", wxCheckBox)->GetValue());

        cfg->Write(_T("/environment/start_here_page"),       (bool) XRCCTRL(*this, "chkShowStartPage", wxCheckBox)->GetValue());

        cfg->Write(_T("/environment/view/dbl_clk_maximize"),    (bool)XRCCTRL(*this, "chkDblClkMaximizes", wxCheckBox)->GetValue());
        cfg->Write(_T("/environment/view/layout_to_toggle"),    XRCCTRL(*this, "choLayoutToToggle", wxChoice)->GetStringSelection());

        cfg->Write(_T("/locale/enable"),                     (bool) XRCCTRL(*this, "chkI18N", wxCheckBox)->GetValue());
        const wxLanguageInfo *info = wxLocale::FindLanguageInfo(XRCCTRL(*this, "cbxLanguage", wxComboBox)->GetStringSelection());
        if (info)
            cfg->Write(_T("/locale/language"), info->CanonicalName);
        else
            cfg->Write(_T("/locale/language"), wxEmptyString);

        mcfg->Write(_T("/log_font_size"),                    (int)  XRCCTRL(*this, "spnLogFontSize",          wxSpinCtrl)->GetValue());

        cfg->Write(_T("/dialog_placement/do_place"),         (bool) XRCCTRL(*this, "chkDoPlace",     wxCheckBox)->GetValue());
        cfg->Write(_T("/dialog_placement/dialog_position"),  (int)  XRCCTRL(*this, "chkPlaceHead",   wxCheckBox)->GetValue() ? pdlHead : pdlCentre);

        // tab "Appearence"
        cfg->Write(_T("/environment/tabs_style"),            (int)  XRCCTRL(*this, "cmbEditorTabs",               wxChoice)->GetSelection());
        cfg->Write(_T("/environment/tabs_closestyle"),       (int)  XRCCTRL(*this, "cmbTabCloseStyle",            wxChoice)->GetSelection());
        cfg->Write(_T("/environment/tabs_list"),             (bool) XRCCTRL(*this, "chkListTabs",                 wxCheckBox)->GetValue());
        bool tab_switcher_mode =                             (bool) XRCCTRL(*this, "chkStackedBasedTabSwitching", wxCheckBox)->GetValue();
        if (Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/tabs_stacked_based_switching")) != tab_switcher_mode)
        {
            if (tab_switcher_mode)
                Manager::Get()->GetEditorManager()->RebuildNotebookStack();
            else
                Manager::Get()->GetEditorManager()->DeleteNotebookStack();
        }
        cfg->Write(_T("/environment/tabs_stacked_based_switching"),          tab_switcher_mode);

        bool enableMousewheel = (bool) XRCCTRL(*this, "chkNBUseMousewheel",wxCheckBox)->GetValue();
        cfg->Write(_T("/environment/tabs_use_mousewheel"),           enableMousewheel);
        wxString key = XRCCTRL(*this, "txtMousewheelModifier", wxTextCtrl)->GetValue();
        cfg->Write(_T("/environment/tabs_mousewheel_modifier"),      key.IsEmpty()?_T("Ctrl"):key);
        cfg->Write(_T("/environment/tabs_mousewheel_advance"),       (bool) XRCCTRL(*this, "rbNBModToAdvance", wxRadioButton)->GetValue());
        cfg->Write(_T("/environment/tabs_invert_advance"),           (bool) XRCCTRL(*this, "chkNBInvertAdvance", wxCheckBox)->GetValue());
        cfg->Write(_T("/environment/tabs_invert_move"),              (bool) XRCCTRL(*this, "chkNBInvertMove", wxCheckBox)->GetValue());
        cbAuiNotebook::AllowScrolling(enableMousewheel);

        cbAuiNotebook::SetModKeys(cfg->Read(_T("/environment/tabs_mousewheel_modifier"),_T("Ctrl")));
        cbAuiNotebook::UseModToAdvance(cfg->ReadBool(_T("/environment/tabs_mousewheel_advance"),false));
        cbAuiNotebook::InvertAdvanceDirection(cfg->ReadBool(_T("/environment/tabs_invert_advance"),false));
        cbAuiNotebook::InvertMoveDirection(cfg->ReadBool(_T("/environment/tabs_invert_move"),false));

        cfg->Write(_T("/environment/aui/border_size"),                (int)  XRCCTRL(*this, "spnAuiBorder", wxSpinCtrl)->GetValue());
        cfg->Write(_T("/environment/aui/sash_size"),                  (int)  XRCCTRL(*this, "spnAuiSash", wxSpinCtrl)->GetValue());
        cfg->Write(_T("/environment/aui/caption_size"),               (int)  XRCCTRL(*this, "spnAuiCaption", wxSpinCtrl)->GetValue());
        cfg->Write(_T("/environment/aui/active_caption_colour"),             XRCCTRL(*this, "btnAuiActiveCaptionColour", wxButton)->GetBackgroundColour());
        cfg->Write(_T("/environment/aui/active_caption_gradient_colour"),    XRCCTRL(*this, "btnAuiActiveCaptionGradientColour", wxButton)->GetBackgroundColour());
        cfg->Write(_T("/environment/aui/active_caption_text_colour"),        XRCCTRL(*this, "btnAuiActiveCaptionTextColour", wxButton)->GetBackgroundColour());
        cfg->Write(_T("/environment/aui/inactive_caption_colour"),           XRCCTRL(*this, "btnAuiInactiveCaptionColour", wxButton)->GetBackgroundColour());
        cfg->Write(_T("/environment/aui/inactive_caption_gradient_colour"),  XRCCTRL(*this, "btnAuiInactiveCaptionGradientColour", wxButton)->GetBackgroundColour());
        cfg->Write(_T("/environment/aui/inactive_caption_text_colour"),      XRCCTRL(*this, "btnAuiInactiveCaptionTextColour", wxButton)->GetBackgroundColour());

        m_pArt->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE,                XRCCTRL(*this, "spnAuiBorder", wxSpinCtrl)->GetValue());
        m_pArt->SetMetric(wxAUI_DOCKART_SASH_SIZE,                       XRCCTRL(*this, "spnAuiSash", wxSpinCtrl)->GetValue());
        m_pArt->SetMetric(wxAUI_DOCKART_CAPTION_SIZE,                    XRCCTRL(*this, "spnAuiCaption", wxSpinCtrl)->GetValue());
        m_pArt->SetColour(wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR,           XRCCTRL(*this, "btnAuiActiveCaptionColour", wxButton)->GetBackgroundColour());
        m_pArt->SetColour(wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR,  XRCCTRL(*this, "btnAuiActiveCaptionGradientColour", wxButton)->GetBackgroundColour());
        m_pArt->SetColour(wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR,      XRCCTRL(*this, "btnAuiActiveCaptionTextColour", wxButton)->GetBackgroundColour());
        m_pArt->SetColour(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR,         XRCCTRL(*this, "btnAuiInactiveCaptionColour", wxButton)->GetBackgroundColour());
        m_pArt->SetColour(wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR,XRCCTRL(*this, "btnAuiInactiveCaptionGradientColour", wxButton)->GetBackgroundColour());
        m_pArt->SetColour(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR,    XRCCTRL(*this, "btnAuiInactiveCaptionTextColour", wxButton)->GetBackgroundColour());

        // tab "Dialogs"
        wxCheckListBox* lb = XRCCTRL(*this, "chkDialogs", wxCheckListBox);

        ConfigManagerContainer::StringSet dialogs = acfg->ReadSSet(_T("/disabled_ret"));

        for (size_t i = 0; i < lb->GetCount(); ++i)
        {
            if (lb->IsChecked(i))
                dialogs.erase(StringToAnnoyingDlgReturn(lb->GetString(i)));
        }

        acfg->Write(_T("/disabled_ret"), dialogs);

        // tab "Network"
        cfg->Write(_T("/network_proxy"),    XRCCTRL(*this, "txtProxy", wxTextCtrl)->GetValue());

        WriteApplicationColours();

        // finally, apply settings in all plugins' panels
        for (size_t i = 0; i < m_PluginPanels.GetCount(); ++i)
        {
            cbConfigurationPanel* panel = m_PluginPanels[i];
            panel->OnApply();
        }

        // save the colours manager here, just in case there are duplicate colour controls
        Manager::Get()->GetColourManager()->Save();
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

    wxScrollingDialog::EndModal(retCode);
}

namespace
{
struct AppColoursClientData : wxClientData
{
    AppColoursClientData(const wxString &id_) : id(id_) {}

    wxString id;
};

} // anonymous namespace

void EnvironmentSettingsDlg::FillApplicationColours()
{
    wxListBox *list = XRCCTRL(*this, "lstColours", wxListBox);
    wxChoice *categories = XRCCTRL(*this, "chCategory", wxChoice);

    bool fillCategories = (categories->GetCount() == 0);
    std::set<wxString> setCategories;

    wxString category = categories->GetStringSelection();
    if (categories->GetSelection() == 0)
        category = wxEmptyString;

    list->Clear();
    const ColourManager::ColourDefMap &colours = Manager::Get()->GetColourManager()->GetColourDefinitions();
    for (ColourManager::ColourDefMap::const_iterator it = colours.begin(); it != colours.end(); ++it)
    {
        if (!it->second.IsValid())
            continue;
        if (category.empty())
            list->Append(it->second.category + wxT(" : ") + it->second.name, new AppColoursClientData(it->first));
        else if (category == it->second.category)
            list->Append(it->second.name, new AppColoursClientData(it->first));
        if (fillCategories)
            setCategories.insert(it->second.category);
    }

    if (fillCategories)
    {
        categories->Append(_("All"));
        categories->Select(0);
        for (std::set<wxString>::const_iterator it = setCategories.begin(); it != setCategories.end(); ++it)
            categories->Append(*it);
    }

    wxCommandEvent tempEvent;
    OnChooseAppColourItem(tempEvent);
}

void EnvironmentSettingsDlg::OnChooseAppColourCategory(cb_unused wxCommandEvent &event)
{
    FillApplicationColours();
}

void EnvironmentSettingsDlg::OnChooseAppColourItem(wxCommandEvent &event)
{
    wxButton *btnColour = XRCCTRL(*this, "btnColour", wxButton);
    wxButton *btnDefault = XRCCTRL(*this, "btnDefaultColour", wxButton);

    const AppColoursClientData *data = static_cast<AppColoursClientData*>(event.GetClientObject());
    if (!data)
    {
        btnColour->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
        btnDefault->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
        btnColour->Enable(false);
        btnDefault->Enable(false);
        return;
    }

    const ColourManager::ColourDefMap &colours = Manager::Get()->GetColourManager()->GetColourDefinitions();
    const ColourManager::ColourDefMap::const_iterator it = colours.find(data->id);
    if (it != colours.end())
    {
        std::map<wxString, wxColour>::const_iterator colourIt = m_ChangedAppColours.find(data->id);
        if (colourIt != m_ChangedAppColours.end())
            btnColour->SetBackgroundColour(colourIt->second);
        else
            btnColour->SetBackgroundColour(it->second.value);

        btnDefault->SetBackgroundColour(it->second.defaultValue);
        btnColour->Enable(true);
        btnDefault->Enable(true);
    }
    else
    {
        btnColour->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
        btnDefault->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
        btnColour->Enable(false);
        btnDefault->Enable(false);
    }
}

void EnvironmentSettingsDlg::OnClickAppColour(wxCommandEvent &event)
{
    wxListBox *list = XRCCTRL(*this, "lstColours", wxListBox);
    if (list->GetSelection() == wxNOT_FOUND)
        return;
    const AppColoursClientData *data;
    data = static_cast<AppColoursClientData*>(list->GetClientObject(list->GetSelection()));
    if (!data)
        return;
    const ColourManager::ColourDefMap &colours = Manager::Get()->GetColourManager()->GetColourDefinitions();
    const ColourManager::ColourDefMap::const_iterator it = colours.find(data->id);
    if (it == colours.end())
        return;

    wxButton *btnColour = XRCCTRL(*this, "btnColour", wxButton);

    if (event.GetId() == XRCID("btnColour"))
    {
        wxColourData colour;
        colour.SetColour(it->second.value);
        wxColourDialog dlg(this, &colour);
        if (dlg.ShowModal() == wxID_OK)
        {
            m_ChangedAppColours[data->id] = dlg.GetColourData().GetColour();
            btnColour->SetBackgroundColour(dlg.GetColourData().GetColour());
        }
    }
    else if (event.GetId() == XRCID("btnDefaultColour"))
    {
        m_ChangedAppColours[data->id] = it->second.defaultValue;
        btnColour->SetBackgroundColour(it->second.defaultValue);
    }
}

void EnvironmentSettingsDlg::WriteApplicationColours()
{
    if (m_ChangedAppColours.empty())
        return;

    ColourManager *manager = Manager::Get()->GetColourManager();
    for (std::map<wxString, wxColour>::const_iterator it = m_ChangedAppColours.begin();
         it != m_ChangedAppColours.end();
         ++it)
    {
        manager->SetColour(it->first, it->second);
    }
}

/*
  AnnoyingDialog captions are in the form of
  "Question xyz?:4"
  where '4' corresponds to an AnnoyingDialog::dReturnType enum value
  The following two methods translate to and from the human readable form of
  "Question xyz?:Yes
 */

wxString EnvironmentSettingsDlg::AnnoyingDlgReturnToString(const wxString& caption)
{
    std::map<wxString, wxString>::const_iterator it = m_AnnoyingDlgReturnMap.find(caption.AfterLast(wxT(':')));
    if (it != m_AnnoyingDlgReturnMap.end())
        return caption.BeforeLast(wxT(':')) + wxT(':') + it->second;
    return caption;
}

wxString EnvironmentSettingsDlg::StringToAnnoyingDlgReturn(const wxString& caption)
{
    for (std::map<wxString, wxString>::const_iterator it = m_AnnoyingDlgReturnMap.begin();
         it != m_AnnoyingDlgReturnMap.end(); ++it)
    {
        if (caption.AfterLast(wxT(':')) == it->second)
            return caption.BeforeLast(wxT(':')) + wxT(':') + it->first;
    }
    return caption;
}

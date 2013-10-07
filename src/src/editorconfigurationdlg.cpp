/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk.h"

#ifndef CB_PRECOMP
    #include <wx/button.h>
    #include <wx/checkbox.h>
    #include <wx/choice.h>
    #include <wx/colordlg.h>
    #include <wx/imaglist.h>
    #include <wx/listbox.h>
    #include <wx/listctrl.h>
    #include <wx/menu.h>
    #include <wx/radiobox.h>
    #include <wx/regex.h>
    #include <wx/settings.h>
    #include <wx/slider.h>
    #include <wx/spinctrl.h>
    #include <wx/stattext.h>
    #include <wx/textdlg.h>
    #include <wx/xrc/xmlres.h>

    #include "manager.h"
    #include "cbauibook.h"
    #include "cbeditor.h"
    #include "cbplugin.h" // cgEditor
    #include "configmanager.h"
    #include "editormanager.h"
    #include "globals.h"
    #include "pluginmanager.h"
#endif
#include "cbstyledtextctrl.h"
#include "cbcolourmanager.h"

#include <wx/fontdlg.h>
#include <wx/fontutil.h>
#include <wx/fontmap.h>
#include <wx/listbook.h>

#include "configurationpanel.h"
#include "editkeywordsdlg.h"
#include "editorcolourset.h"
#include "editorconfigurationdlg.h"

// images by order of pages
const wxString base_imgs[] =
{
    _T("editor"),
    _T("folding"),
    _T("gutter-margin"),
    _T("syntax-highlight"),
    _T("default-code"),
};
const int IMAGES_COUNT = sizeof(base_imgs) / sizeof(wxString);

// map cmbDefCodeFileType indexes to FileType values
// if more entries are added to cmbDefCodeFileType, edit the mapping here
const FileType IdxToFileType[] = { ftSource, ftHeader };

BEGIN_EVENT_TABLE(EditorConfigurationDlg, wxScrollingDialog)
    EVT_BUTTON(XRCID("btnChooseEditorFont"),           EditorConfigurationDlg::OnChooseFont)
    EVT_BUTTON(XRCID("btnKeywords"),                   EditorConfigurationDlg::OnEditKeywords)
    EVT_BUTTON(XRCID("btnFilemasks"),                  EditorConfigurationDlg::OnEditFilemasks)
    EVT_BUTTON(XRCID("btnColoursReset"),               EditorConfigurationDlg::OnColoursReset)
    EVT_BUTTON(XRCID("btnColoursCopy"),                EditorConfigurationDlg::OnColoursCopyFrom)
    EVT_BUTTON(XRCID("btnColoursCopyAll"),             EditorConfigurationDlg::OnColoursCopyAllFrom)
    EVT_BUTTON(XRCID("btnCaretColour"),                EditorConfigurationDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnGutterColour"),               EditorConfigurationDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnColoursFore"),                EditorConfigurationDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnColoursBack"),                EditorConfigurationDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnForeSetDefault"),             EditorConfigurationDlg::OnSetDefaultColour)
    EVT_BUTTON(XRCID("btnBackSetDefault"),             EditorConfigurationDlg::OnSetDefaultColour)
    EVT_BUTTON(XRCID("btnColoursAddTheme"),            EditorConfigurationDlg::OnAddColourTheme)
    EVT_BUTTON(XRCID("btnColoursDeleteTheme"),         EditorConfigurationDlg::OnDeleteColourTheme)
    EVT_BUTTON(XRCID("btnColoursRenameTheme"),         EditorConfigurationDlg::OnRenameColourTheme)
    EVT_CHECKBOX(XRCID("chkColoursBold"),              EditorConfigurationDlg::OnBoldItalicUline)
    EVT_CHECKBOX(XRCID("chkColoursItalics"),           EditorConfigurationDlg::OnBoldItalicUline)
    EVT_CHECKBOX(XRCID("chkColoursUnderlined"),        EditorConfigurationDlg::OnBoldItalicUline)
    EVT_LISTBOX(XRCID("lstComponents"),                EditorConfigurationDlg::OnColourComponent)
    EVT_CHOICE(XRCID("cmbLangs"),                      EditorConfigurationDlg::OnChangeLang)
    EVT_CHOICE(XRCID("cmbDefCodeFileType"),            EditorConfigurationDlg::OnChangeDefCodeFileType)
    EVT_CHOICE(XRCID("cmbThemes"),                     EditorConfigurationDlg::OnColourTheme)
    EVT_CHECKBOX(XRCID("chkDynamicWidth"),             EditorConfigurationDlg::OnDynamicCheck)
    EVT_CHECKBOX(XRCID("chkEnableMultipleSelections"), EditorConfigurationDlg::OnMultipleSelections)
    EVT_CHOICE(XRCID("lstCaretStyle"),                 EditorConfigurationDlg::OnCaretStyle)

    EVT_LISTBOOK_PAGE_CHANGED(XRCID("nbMain"), EditorConfigurationDlg::OnPageChanged)
END_EVENT_TABLE()

EditorConfigurationDlg::EditorConfigurationDlg(wxWindow* parent)
    : m_TextColourControl(0L),
    m_Theme(0L),
    m_Lang(HL_NONE),
    m_DefCodeFileType(0),
    m_ThemeModified(false),
    m_EnableChangebar(false),
    m_pImageList(nullptr)
{
    wxXmlResource::Get()->LoadObject(this, parent, _T("dlgConfigureEditor"),_T("wxScrollingDialog"));

    XRCCTRL(*this, "lblEditorFont", wxStaticText)->SetLabel(_("This is sample text"));
    m_FontString = Manager::Get()->GetConfigManager(_T("editor"))->Read(_T("/font"), wxEmptyString);
    UpdateSampleFont(false);

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));

    XRCCTRL(*this, "chkAutoIndent",               wxCheckBox)->SetValue(cfg->ReadBool(_T("/auto_indent"),                true));
    XRCCTRL(*this, "chkSmartIndent",              wxCheckBox)->SetValue(cfg->ReadBool(_T("/smart_indent"),               true));
    XRCCTRL(*this, "chkBraceCompletion",          wxCheckBox)->SetValue(cfg->ReadBool(_T("/brace_completion"),           true));
    XRCCTRL(*this, "chkDetectIndent",             wxCheckBox)->SetValue(cfg->ReadBool(_T("/detect_indent"),              false));
    XRCCTRL(*this, "chkUseTab",                   wxCheckBox)->SetValue(cfg->ReadBool(_T("/use_tab"),                    false));
    m_EnableScrollWidthTracking = cfg->ReadBool(_T("/margin/scroll_width_tracking"), false);
    XRCCTRL(*this, "chkScrollWidthTracking",      wxCheckBox)->SetValue(m_EnableScrollWidthTracking);
    m_EnableChangebar = cfg->ReadBool(_T("/margin/use_changebar"), true);
    XRCCTRL(*this, "chkUseChangebar",             wxCheckBox)->SetValue(m_EnableChangebar);
    XRCCTRL(*this, "chkShowIndentGuides",         wxCheckBox)->SetValue(cfg->ReadBool(_T("/show_indent_guides"),         false));
    XRCCTRL(*this, "chkBraceSmartIndent",         wxCheckBox)->SetValue(cfg->ReadBool(_T("/brace_smart_indent"),         true));
    XRCCTRL(*this, "chkSelectionBraceCompletion", wxCheckBox)->SetValue(cfg->ReadBool(_T("/selection_brace_completion"), false));
    XRCCTRL(*this, "chkTabIndents",               wxCheckBox)->SetValue(cfg->ReadBool(_T("/tab_indents"),                true));
    XRCCTRL(*this, "chkBackspaceUnindents",       wxCheckBox)->SetValue(cfg->ReadBool(_T("/backspace_unindents"),        true));
    XRCCTRL(*this, "chkWordWrap",                 wxCheckBox)->SetValue(cfg->ReadBool(_T("/word_wrap"),                  false));
    XRCCTRL(*this, "chkWordWrapStyleHomeEnd",     wxCheckBox)->SetValue(cfg->ReadBool(_T("/word_wrap_style_home_end"),   true));
    XRCCTRL(*this, "chkPosixRegex",               wxCheckBox)->SetValue(cfg->ReadBool(_T("/use_posix_style_regexes"),    false));
    #ifdef wxHAS_REGEX_ADVANCED
    XRCCTRL(*this, "chkAdvancedRegex",            wxCheckBox)->SetValue(cfg->ReadBool(_T("/use_advanced_regexes"),       false));
    #else
    XRCCTRL(*this, "chkAdvancedRegex",            wxCheckBox)->SetValue(false);
    XRCCTRL(*this, "chkAdvancedRegex",            wxCheckBox)->Enable(false);
    #endif
    XRCCTRL(*this, "chkShowLineNumbers",          wxCheckBox)->SetValue(cfg->ReadBool(_T("/show_line_numbers"),          true));
    XRCCTRL(*this, "chkHighlightCaretLine",       wxCheckBox)->SetValue(cfg->ReadBool(_T("/highlight_caret_line"),       false));
    XRCCTRL(*this, "chkSimplifiedHome",           wxCheckBox)->SetValue(cfg->ReadBool(_T("/simplified_home"),            false));
    XRCCTRL(*this, "chkCamelCase",                wxCheckBox)->SetValue(cfg->ReadBool(_T("/camel_case"),                 false));
    XRCCTRL(*this, "chkResetZoom",                wxCheckBox)->SetValue(cfg->ReadBool(_T("/reset_zoom"),                 false));
    XRCCTRL(*this, "chkZoomAll",                  wxCheckBox)->SetValue(cfg->ReadBool(_T("/zoom_all"),                   false));
    XRCCTRL(*this, "spnTabSize",                  wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/tab_size"),                    4));
    XRCCTRL(*this, "cmbViewWS",                   wxChoice)->SetSelection(cfg->ReadInt(_T("/view_whitespace"),           0));
    XRCCTRL(*this, "rbTabText",                   wxRadioBox)->SetSelection(cfg->ReadBool(_T("/tab_text_relative"),      true)? 1 : 0);

    XRCCTRL(*this, "chkTrackPreprocessor",        wxCheckBox)->SetValue(cfg->ReadBool(_T("/track_preprocessor"),         true));
    XRCCTRL(*this, "chkCollectPrjDefines",        wxCheckBox)->SetValue(cfg->ReadBool(_T("/collect_prj_defines"),        true));
    XRCCTRL(*this, "chkPlatDefines",              wxCheckBox)->SetValue(cfg->ReadBool(_T("/platform_defines"),           false));
    XRCCTRL(*this, "chkColoursWxSmith",           wxCheckBox)->SetValue(cfg->ReadBool(_T("/highlight_wxsmith"),          true));
    XRCCTRL(*this, "chkNoStlC",                   wxCheckBox)->SetValue(cfg->ReadBool(_T("/no_stl_in_c"),                true));

#if defined __WXMSW__
    const wxString openFolderCmds = _T("explorer.exe /select,");
#elif defined __WXMAC__
    const wxString openFolderCmds = _T("open -R");
#else
    const wxString openFolderCmds = _T("xdg-open");
#endif
    XRCCTRL(*this, "txtOpenFolder", wxTextCtrl)->SetValue(cfg->Read(_T("/open_containing_folder"), openFolderCmds));

    XRCCTRL(*this, "chkShowEOL",             wxCheckBox)->SetValue(cfg->ReadBool(_T("/show_eol"),                        false));
    XRCCTRL(*this, "chkStripTrailings",      wxCheckBox)->SetValue(cfg->ReadBool(_T("/eol/strip_trailing_spaces"),       true));
    XRCCTRL(*this, "chkEnsureFinalEOL",      wxCheckBox)->SetValue(cfg->ReadBool(_T("/eol/ensure_final_line_end"),       true));
    XRCCTRL(*this, "chkEnsureConsistentEOL", wxCheckBox)->SetValue(cfg->ReadBool(_T("/eol/ensure_consistent_line_ends"), false));
    // NOTE: duplicate line in cbeditor.cpp (CreateEditor)
    XRCCTRL(*this, "cmbEOLMode",             wxChoice)->SetSelection(cfg->ReadInt(_T("/eol/eolmode"),                  platform::windows ? wxSCI_EOL_CRLF : wxSCI_EOL_LF)); // Windows takes CR+LF, other platforms LF only

    //caret
    wxColour caretColour = Manager::Get()->GetColourManager()->GetColour(wxT("editor_caret"));
    int caretStyle = cfg->ReadInt(_T("/caret/style"), wxSCI_CARETSTYLE_LINE);
    XRCCTRL(*this, "lstCaretStyle",  wxChoice)->SetSelection(caretStyle);
    XRCCTRL(*this, "spnCaretWidth",  wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/caret/width"), 1));
    XRCCTRL(*this, "spnCaretWidth",  wxSpinCtrl)->Enable(caretStyle == wxSCI_CARETSTYLE_LINE);
    XRCCTRL(*this, "btnCaretColour", wxButton)->SetBackgroundColour(caretColour);
    XRCCTRL(*this, "slCaretPeriod",  wxSlider)->SetValue(cfg->ReadInt(_T("/caret/period"), 500));

    //selections
    XRCCTRL(*this, "chkEnableVirtualSpace", wxCheckBox)->SetValue(cfg->ReadBool(_T("/selection/use_vspace"), false));
    bool multiSelectEnabled = cfg->ReadBool(_T("/selection/multi_select"), false);
    XRCCTRL(*this, "chkEnableMultipleSelections",        wxCheckBox)->SetValue(multiSelectEnabled);
    XRCCTRL(*this, "chkEnableAdditionalSelectionTyping", wxCheckBox)->SetValue(cfg->ReadBool(_T("/selection/multi_typing"), false));
    XRCCTRL(*this, "chkEnableAdditionalSelectionTyping", wxCheckBox)->Enable(multiSelectEnabled);

    //folding
    XRCCTRL(*this, "chkEnableFolding",       wxCheckBox)->SetValue(cfg->ReadBool(_T("/folding/show_folds"),            true));
    XRCCTRL(*this, "chkFoldOnOpen",          wxCheckBox)->SetValue(cfg->ReadBool(_T("/folding/fold_all_on_open"),      false));
    XRCCTRL(*this, "chkFoldPreprocessor",    wxCheckBox)->SetValue(cfg->ReadBool(_T("/folding/fold_preprocessor"),     false));
    XRCCTRL(*this, "chkFoldComments",        wxCheckBox)->SetValue(cfg->ReadBool(_T("/folding/fold_comments"),         true));
    XRCCTRL(*this, "chkFoldXml",             wxCheckBox)->SetValue(cfg->ReadBool(_T("/folding/fold_xml"),              true));
    XRCCTRL(*this, "chkUnderlineFoldedLine", wxCheckBox)->SetValue(cfg->ReadBool(_T("/folding/underline_folded_line"), true));
    XRCCTRL(*this, "lstIndicators",          wxChoice)->SetSelection(cfg->ReadInt(_T("/folding/indicator"),            2));
    XRCCTRL(*this, "chkFoldLimit",           wxCheckBox)->SetValue(cfg->ReadBool(_T("/folding/limit"),                 false));
    XRCCTRL(*this, "spnFoldLimitLevel",      wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/folding/limit_level"),            1));

    //gutter
    wxColour gutterColour = Manager::Get()->GetColourManager()->GetColour(wxT("editor_gutter"));
    XRCCTRL(*this, "lstGutterMode",   wxChoice)->SetSelection(cfg->ReadInt(_T("/gutter/mode"), 0));
    XRCCTRL(*this, "btnGutterColour", wxButton)->SetBackgroundColour(gutterColour);
    XRCCTRL(*this, "spnGutterColumn", wxSpinCtrl)->SetRange(1, 500);
    XRCCTRL(*this, "spnGutterColumn", wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/gutter/column"), 80));

    //margin
    XRCCTRL(*this, "spnMarginWidth",      wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/margin/width_chars"),    6));
    XRCCTRL(*this, "chkDynamicWidth",     wxCheckBox)->SetValue(cfg->ReadBool(_T("/margin/dynamic_width"), false));
    XRCCTRL(*this, "spnMarginWidth",      wxSpinCtrl)->Enable(!cfg->ReadBool(_T("/margin/dynamic_width"),  false));
    XRCCTRL(*this, "chkAddBPByLeftClick", wxCheckBox)->SetValue(cfg->ReadBool(_T("/margin_1_sensitive"),   true));
    XRCCTRL(*this, "chkImageBP",          wxCheckBox)->SetValue(cfg->ReadBool(_T("/margin_1_image_bp"),    true));

    // colour set
    LoadThemes();

    // fill encodings
    wxChoice* cmbEnc = XRCCTRL(*this, "cmbEncoding", wxChoice);
    if (cmbEnc)
    {
        cmbEnc->Clear();
        wxString def_enc_name = cfg->Read(_T("/default_encoding"), wxLocale::GetSystemEncodingName());
        int sel = 0;
        size_t count = wxFontMapper::GetSupportedEncodingsCount();
        for (size_t i = 0; i < count; ++i)
        {
            wxFontEncoding enc = wxFontMapper::GetEncoding(i);
            wxString enc_name = wxFontMapper::GetEncodingName(enc);
            cmbEnc->Append(enc_name);
            if (enc_name.CmpNoCase(def_enc_name) == 0)
                sel = i;
        }
        cmbEnc->SetSelection(sel);
    }
    XRCCTRL(*this, "rbEncodingUseOption",   wxRadioBox)->SetSelection(cfg->ReadInt(_T("/default_encoding/use_option"), 0));
    XRCCTRL(*this, "chkEncodingFindLatin2", wxCheckBox)->SetValue(cfg->ReadBool(_T("/default_encoding/find_latin2"),   false));
    XRCCTRL(*this, "chkEncodingUseSystem",  wxCheckBox)->SetValue(cfg->ReadBool(_T("/default_encoding/use_system"),    true));

    // default code
    XRCCTRL(*this, "cmbDefCodeFileType", wxChoice)->SetSelection(m_DefCodeFileType);
    wxString key;
    key.Printf(_T("/default_code/set%d"), IdxToFileType[m_DefCodeFileType]);
    XRCCTRL(*this, "txtDefCode", wxTextCtrl)->SetValue(cfg->Read(key, wxEmptyString));

    // setting the default editor font size to 10 point
    wxFont tmpFont(10, wxMODERN, wxNORMAL, wxNORMAL);

    XRCCTRL(*this, "txtDefCode", wxTextCtrl)->SetFont(tmpFont);
    // read them all in the array
    for(size_t idx = 0; idx < sizeof(IdxToFileType)/sizeof(*IdxToFileType); ++ idx)
    {
        key.Printf(_T("/default_code/set%d"), IdxToFileType[idx]);
        m_DefaultCode.Add(cfg->Read(key, wxEmptyString));
    }// end for : idx

    LoadListbookImages();

    // add all plugins configuration panels
    AddPluginPanels();

    // the following code causes a huge dialog to be created with wx2.8.4
    // commenting it out fixes the problem (along with some XRC surgery)
    // if this causes problems with earlier wx versions we might need to
    // add a platform/version #ifdef...
    // the former commented out code leads to problems with wxGTK (parts of long static texts not shown)
    // seems to work without the comments on wx2.8.9 on linux and windows

    // make sure everything is laid out properly
    GetSizer()->SetSizeHints(this);
    CentreOnParent();
}

EditorConfigurationDlg::~EditorConfigurationDlg()
{
    if (m_Theme)
        delete m_Theme;

    if (m_TextColourControl)
        delete m_TextColourControl;

    delete m_pImageList;
}

void EditorConfigurationDlg::AddPluginPanels()
{
    const wxString base = _T("images/settings/");
    // for plugins who do not supply icons, use common generic icons
    const wxString noimg = _T("images/settings/generic-plugin");

    wxListbook* lb = XRCCTRL(*this, "nbMain", wxListbook);
    // get all configuration panels which are about the editor.
    Manager::Get()->GetPluginManager()->GetConfigurationPanels(cgEditor, lb, m_PluginPanels);

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

        m_pImageList->Add(cbLoadBitmap(onFile));
        m_pImageList->Add(cbLoadBitmap(offFile));
        lb->SetPageImage(lb->GetPageCount() - 1, m_pImageList->GetImageCount() - 2);
    }

    UpdateListbookImages();
}

void EditorConfigurationDlg::LoadListbookImages()
{
    const wxString base = ConfigManager::GetDataFolder() + _T("/images/settings/");

    m_pImageList = new wxImageList(80, 80);
    wxBitmap bmp;
    for (int i = 0; i < IMAGES_COUNT; ++i)
    {
        bmp = cbLoadBitmap(base + base_imgs[i] + _T(".png"));
        m_pImageList->Add(bmp);
        bmp = cbLoadBitmap(base + base_imgs[i] + _T("-off.png"));
        m_pImageList->Add(bmp);
    }
    UpdateListbookImages();
}

void EditorConfigurationDlg::UpdateListbookImages()
{
    wxListbook* lb = XRCCTRL(*this, "nbMain", wxListbook);
    int sel = lb->GetSelection();

    if (SettingsIconsStyle(Manager::Get()->GetConfigManager(_T("app"))->ReadInt(_T("/environment/settings_size"), 0)))
    {
        SetSettingsIconsStyle(lb->GetListView(), sisNoIcons);
        lb->SetImageList(nullptr);
    }
    else
    {
        lb->SetImageList(m_pImageList);
        // set page images according to their on/off status
        for (size_t i = 0; i < IMAGES_COUNT + m_PluginPanels.GetCount(); ++i)
            lb->SetPageImage(i, (i * 2) + (sel == (int)i ? 0 : 1));
        SetSettingsIconsStyle(lb->GetListView(), sisLargeIcons);
    }

    // update the page title
    wxString label = lb->GetPageText(sel);
    // replace any stray & with && because label makes it an underscore
    while (label.Replace(_T(" & "), _T(" && ")))
        ;
    XRCCTRL(*this, "lblBigTitle", wxStaticText)->SetLabel(label);
    XRCCTRL(*this, "pnlTitleInfo", wxPanel)->Layout();
}

void EditorConfigurationDlg::OnPageChanged(wxListbookEvent& event)
{
    // update only on real change, not on dialog creation
    if (event.GetOldSelection() != -1 && event.GetSelection() != -1)
        UpdateListbookImages();
}

void EditorConfigurationDlg::CreateColoursSample()
{
    if (!m_TextColourControl)
    {
        m_TextColourControl = new cbStyledTextCtrl(this, wxID_ANY);

        m_TextColourControl->SetTabWidth(4);
        m_TextColourControl->SetCaretWidth(0);
        m_TextColourControl->SetMarginType(0, wxSCI_MARGIN_NUMBER);
        m_TextColourControl->SetMarginWidth(0, 32);
        m_TextColourControl->SetMinSize(wxSize(50,50));
        m_TextColourControl->SetMarginWidth(1, 0);

        wxXmlResource::Get()->AttachUnknownControl(_T("txtColoursSample"), m_TextColourControl);
    }

    int breakLine = -1;
    int debugLine = -1;
    int errorLine = -1;
    wxString code = m_Theme->GetSampleCode(m_Lang, &breakLine, &debugLine, &errorLine);
    if (!code.IsEmpty())
    {
        m_TextColourControl->SetReadOnly(false);
        m_TextColourControl->LoadFile(code);
        m_TextColourControl->SetReadOnly(true);
    }

    m_TextColourControl->MarkerDeleteAll(2);
    m_TextColourControl->MarkerDeleteAll(3);
    m_TextColourControl->MarkerDeleteAll(4);
    if (breakLine != -1) m_TextColourControl->MarkerAdd(breakLine, 2); // breakpoint line
    if (debugLine != -1) m_TextColourControl->MarkerAdd(debugLine, 3); // active line
    if (errorLine != -1) m_TextColourControl->MarkerAdd(errorLine, 4); // error line

    ApplyColours();
    FillColourComponents();
}

void EditorConfigurationDlg::FillColourComponents()
{
    wxListBox* colours = XRCCTRL(*this, "lstComponents", wxListBox);
    colours->Clear();
    for (int i = 0; i < m_Theme->GetOptionCount(m_Lang); ++i)
    {
        OptionColour* opt = m_Theme->GetOptionByIndex(m_Lang, i);
        if (colours->FindString(opt->name) == -1)
            colours->Append(opt->name);
    }
    colours->SetSelection(0);
    ReadColours();
}

void EditorConfigurationDlg::ApplyColours()
{
    if (m_TextColourControl && m_Theme)
    {
        wxFont fnt = XRCCTRL(*this, "lblEditorFont", wxStaticText)->GetFont();
        if (m_TextColourControl)
        {
            m_TextColourControl->StyleSetFont(wxSCI_STYLE_DEFAULT,fnt);
            m_Theme->Apply(m_Lang, m_TextColourControl);
        }
    }
}

void EditorConfigurationDlg::ReadColours()
{
    if (m_Theme)
    {
        wxListBox* colours = XRCCTRL(*this, "lstComponents", wxListBox);
/* TODO (mandrav#1#): FIXME!!! */
        OptionColour* opt = m_Theme->GetOptionByName(m_Lang, colours->GetStringSelection());
        UpdateColourControls(opt);
    }
}

void EditorConfigurationDlg::UpdateColourControls(const OptionColour *opt)
{
    if (opt)
    {
        wxColour c = opt->fore;
        if (c == wxNullColour)
        {
            XRCCTRL(*this, "btnColoursFore", wxButton)->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
            XRCCTRL(*this, "btnColoursFore", wxButton)->SetLabel(_("\"Default\""));
        }
        else
        {
            XRCCTRL(*this, "btnColoursFore", wxButton)->SetBackgroundColour(c);
            XRCCTRL(*this, "btnColoursFore", wxButton)->SetLabel(_T(""));
        }

        c = opt->back;
        if (c == wxNullColour)
        {
            XRCCTRL(*this, "btnColoursBack", wxButton)->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
            XRCCTRL(*this, "btnColoursBack", wxButton)->SetLabel(_("\"Default\""));
        }
        else
        {
            XRCCTRL(*this, "btnColoursBack", wxButton)->SetBackgroundColour(c);
            XRCCTRL(*this, "btnColoursBack", wxButton)->SetLabel(_T(""));
        }

        XRCCTRL(*this, "chkColoursBold", wxCheckBox)->SetValue(opt->bold);
        XRCCTRL(*this, "chkColoursItalics", wxCheckBox)->SetValue(opt->italics);
        XRCCTRL(*this, "chkColoursUnderlined", wxCheckBox)->SetValue(opt->underlined);

//          XRCCTRL(*this, "btnColorsFore", wxButton)->Enable(opt->isStyle);
        XRCCTRL(*this, "chkColoursBold", wxCheckBox)->Enable(opt->isStyle);
        XRCCTRL(*this, "chkColoursItalics", wxCheckBox)->Enable(opt->isStyle);
        XRCCTRL(*this, "chkColoursUnderlined", wxCheckBox)->Enable(opt->isStyle);
        bool isDefault = (opt->name == _("Default"));
        XRCCTRL(*this, "btnForeSetDefault", wxButton)->Enable(!isDefault);
        XRCCTRL(*this, "btnBackSetDefault", wxButton)->Enable(!isDefault);
    }
}

void EditorConfigurationDlg::WriteColours()
{
    if (m_Theme)
    {
        wxListBox* colours = XRCCTRL(*this, "lstComponents", wxListBox);
/* TODO (mandrav#1#): FIXME!!! */
        OptionColour* opt = m_Theme->GetOptionByName(m_Lang, colours->GetStringSelection());
        if (opt)
        {
            wxColour c = XRCCTRL(*this, "btnColoursFore", wxButton)->GetBackgroundColour();
            if (c != wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE))
                opt->fore = c;
            else
                opt->fore = wxNullColour;
            c = XRCCTRL(*this, "btnColoursBack", wxButton)->GetBackgroundColour();
            if (c != wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE))
                opt->back = c;
            else
                opt->back = wxNullColour;
            opt->bold = XRCCTRL(*this, "chkColoursBold", wxCheckBox)->GetValue();
            opt->italics = XRCCTRL(*this, "chkColoursItalics", wxCheckBox)->GetValue();
            opt->underlined = XRCCTRL(*this, "chkColoursUnderlined", wxCheckBox)->GetValue();
            m_Theme->UpdateOptionsWithSameName(m_Lang, opt);
        }
    }
    ApplyColours();
    m_ThemeModified = true;
}

void EditorConfigurationDlg::UpdateSampleFont(bool askForNewFont)
{
    // setting the default editor font size to 10 point
    wxFont tmpFont(10, wxMODERN, wxNORMAL, wxNORMAL);

    if (!m_FontString.IsEmpty())
    {
        wxNativeFontInfo nfi;
        nfi.FromString(m_FontString);
        tmpFont.SetNativeFontInfo(nfi);
    }

    XRCCTRL(*this, "lblEditorFont", wxStaticText)->SetFont(tmpFont);
    if (!askForNewFont)
        return;

    wxFontData data;
    data.SetInitialFont(tmpFont);

    wxFontDialog dlg(this, data);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxFont font = dlg.GetFontData().GetChosenFont();
        XRCCTRL(*this, "lblEditorFont", wxStaticText)->SetFont(font);
        m_FontString = font.GetNativeFontInfoDesc();
        ApplyColours();
    }
}

void EditorConfigurationDlg::OnCaretStyle(cb_unused wxCommandEvent& event)
{
    XRCCTRL(*this, "spnCaretWidth", wxSpinCtrl)->Enable(XRCCTRL(*this, "lstCaretStyle", wxChoice)->GetSelection() == wxSCI_CARETSTYLE_LINE);
}

void EditorConfigurationDlg::LoadThemes()
{
    wxChoice* cmbThemes = XRCCTRL(*this, "cmbThemes", wxChoice);
    cmbThemes->Clear();
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));
    wxArrayString list = cfg->EnumerateSubPaths(_T("/colour_sets"));
    list.Sort();
    for (unsigned int i = 0; i < list.GetCount(); ++i)
    {
        cmbThemes->Append(cfg->Read(_T("/colour_sets/") + list[i] + _T("/name"), list[i]));
    }
    if (cmbThemes->GetCount() == 0)
        cmbThemes->Append(COLORSET_DEFAULT);
    wxString group = cfg->Read(_T("/colour_sets/active_colour_set"), COLORSET_DEFAULT);
    long int cookie = cmbThemes->FindString(group);
    if (cookie == wxNOT_FOUND)
        cookie = 0;
    cmbThemes->SetSelection(cookie);
    ChangeTheme();
}

bool EditorConfigurationDlg::AskToSaveTheme()
{
    wxChoice* cmbThemes = XRCCTRL(*this, "cmbThemes", wxChoice);
    if (m_Theme && m_ThemeModified)
    {
        wxString msg;
        msg.Printf(_("The colour theme \"%s\" is modified.\nDo you want to save the changes?"), m_Theme->GetName().c_str());
        int ret = cbMessageBox(msg, _("Save"), wxYES_NO | wxCANCEL, this);
        switch (ret)
        {
            case wxID_YES: m_Theme->Save(); break;
            case wxID_CANCEL:
            {
                int idx = cmbThemes->FindString(m_Theme->GetName());
                cmbThemes->SetSelection(idx);
                return false;
            }
            default: break;
        }
    }
    return true;
}

void EditorConfigurationDlg::ChangeTheme()
{
    wxChoice* cmbThemes = XRCCTRL(*this, "cmbThemes", wxChoice);
    if (cmbThemes->GetSelection() == wxNOT_FOUND)
        cmbThemes->SetSelection(0);
    wxString key = cmbThemes->GetStringSelection();
    XRCCTRL(*this, "btnColoursRenameTheme", wxButton)->Enable(key != COLORSET_DEFAULT);
    XRCCTRL(*this, "btnColoursDeleteTheme", wxButton)->Enable(key != COLORSET_DEFAULT);

    if (m_Theme)
        delete m_Theme;

    // If the theme is the same one used by EditorManager,
    // skip the creation of new EditorColourSet class to avoid lengthy loading times.
    // Instead, use the copy constructor...
    EditorColourSet* manSet = Manager::Get()->GetEditorManager()->GetColourSet();
    if (manSet && key == manSet->GetName())
        m_Theme = new EditorColourSet(*manSet);
    else
        m_Theme = new EditorColourSet(key);

    XRCCTRL(*this, "btnKeywords", wxButton)->Enable(m_Theme);
    XRCCTRL(*this, "btnFilemasks", wxButton)->Enable(m_Theme);

    wxChoice* cmbLangs = XRCCTRL(*this, "cmbLangs", wxChoice);
    int sel = cmbLangs->GetSelection();
    cmbLangs->Clear();
    wxArrayString langs = m_Theme->GetAllHighlightLanguages();
    for (unsigned int i = 0; i < langs.GetCount(); ++i)
    {
        cmbLangs->Append(langs[i]);
    }
    if (sel == -1)
    {
        wxString lang = Manager::Get()->GetConfigManager(_T("editor"))->Read(_T("/colour_sets/active_lang"), _T("C/C++"));
        sel = cmbLangs->FindString(lang);
    }
    cmbLangs->SetSelection(sel != -1 ? sel : 0);
    cmbLangs->Enable(langs.GetCount() != 0);
    if (m_Theme)
    {
        wxString str_sel = cmbLangs->GetStringSelection();
        m_Lang = m_Theme->GetHighlightLanguage(str_sel);
    }

    CreateColoursSample();
    m_ThemeModified = false;
}

// events

void EditorConfigurationDlg::OnColourTheme(cb_unused wxCommandEvent& event)
{
    // theme has changed
    wxChoice* cmbThemes = XRCCTRL(*this, "cmbThemes", wxChoice);
    if (m_Theme && m_Theme->GetName() != cmbThemes->GetStringSelection())
    {
        if (AskToSaveTheme())
            ChangeTheme();
    }
}

namespace
{
bool CheckColourThemeName(const wxString &name, wxWindow *parent)
{
    wxRegEx regex(wxT("^[A-Za-z][A-Za-z_0-9]*$"));
    if (regex.Matches(name))
        return true;
    else
    {
        cbMessageBox(_("You've entered invalid characters for the name of the theme. "
                       "Only alphanumeric characters and '_' are allowed! The first character should be a letter. "
                       "Please try again."),
                     _("Error"),
                     wxOK,
                     parent);
        return false;
    }
}

} // anonymous namespace

void EditorConfigurationDlg::OnAddColourTheme(cb_unused wxCommandEvent& event)
{
    wxTextEntryDialog dlg(this, _("Please enter the name of the new colour theme:"), _("New theme name"));
    PlaceWindow(&dlg);
    if (dlg.ShowModal() != wxID_OK)
        return;

    wxString name = dlg.GetValue();
    if (!CheckColourThemeName(name, this))
        return;

    wxChoice* cmbThemes = XRCCTRL(*this, "cmbThemes", wxChoice);
    cmbThemes->Append(name);
    cmbThemes->SetSelection(cmbThemes->GetCount() - 1);
    ChangeTheme();
}

void EditorConfigurationDlg::OnDeleteColourTheme(cb_unused wxCommandEvent& event)
{
    if (cbMessageBox(_("Are you sure you want to delete this theme?"), _("Confirmation"), wxYES_NO, this) == wxID_YES)
    {
        Manager::Get()->GetConfigManager(_T("editor"))->DeleteSubPath(_T("/colour_sets/") + m_Theme->GetName());
        wxChoice* cmbThemes = XRCCTRL(*this, "cmbThemes", wxChoice);
        int idx = cmbThemes->FindString(m_Theme->GetName());
        if (idx != wxNOT_FOUND)
            cmbThemes->Delete(idx);
        cmbThemes->SetSelection(wxNOT_FOUND);
        ChangeTheme();
    }
}

void EditorConfigurationDlg::OnRenameColourTheme(cb_unused wxCommandEvent& event)
{
    wxTextEntryDialog dlg(this, _("Please enter the new name of the new colour theme:"), _("New theme name"), m_Theme->GetName());
    PlaceWindow(&dlg);
    if (dlg.ShowModal() != wxID_OK)
        return;

    wxString name = dlg.GetValue();
    if (!CheckColourThemeName(name, this))
        return;

    wxString oldName = m_Theme->GetName();
    wxChoice* cmbThemes = XRCCTRL(*this, "cmbThemes", wxChoice);
    int idx = cmbThemes->GetSelection();
    if (idx != wxNOT_FOUND)
        cmbThemes->SetString(idx, name);
    m_Theme->SetName(name);
    m_Theme->Save();
    Manager::Get()->GetConfigManager(_T("editor"))->DeleteSubPath(_T("/colour_sets/") + oldName);

    cmbThemes->SetSelection(cmbThemes->FindString(name));
    ChangeTheme();
}

void EditorConfigurationDlg::OnEditKeywords(cb_unused wxCommandEvent& event)
{
    if (m_Theme && m_Lang != HL_NONE)
    {
        wxArrayString descriptions;
        if (m_TextColourControl)
            descriptions = GetArrayFromString(m_TextColourControl->DescribeKeyWordSets(), wxT("\n"));
        EditKeywordsDlg dlg(0, m_Theme, m_Lang, descriptions);
        PlaceWindow(&dlg);
        dlg.ShowModal();

        dlg.m_pTheme->SetKeywords(dlg.m_Lang, dlg.m_LastSet, dlg.txtKeywords->GetValue());
    }
}

void EditorConfigurationDlg::OnEditFilemasks(cb_unused wxCommandEvent& event)
{
    if (m_Theme && m_Lang != HL_NONE)
    {
        wxString masks = wxGetTextFromUser(_("Edit filemasks (use commas to separate them - case insensitive):"),
                                        m_Theme->GetLanguageName(m_Lang),
                                        GetStringFromArray(m_Theme->GetFileMasks(m_Lang), _T(",")));
        if (!masks.IsEmpty())
            m_Theme->SetFileMasks(m_Lang, masks);
    }
}

void EditorConfigurationDlg::OnColoursReset(cb_unused wxCommandEvent& event)
{
    if (m_Theme && m_Lang != HL_NONE)
    {
        wxString tmp;
        tmp.Printf(_("Are you sure you want to reset all settings to defaults for \"%s\"?"),
                    m_Theme->GetLanguageName(m_Lang).c_str());
        if (cbMessageBox(tmp, _("Confirmation"), wxICON_QUESTION | wxYES_NO, this) == wxID_YES)
        {
            m_Theme->Reset(m_Lang);
            ApplyColours();
            ReadColours();
            m_ThemeModified = true;
        }
    }
}

void EditorConfigurationDlg::OnColoursCopyFrom(cb_unused wxCommandEvent &event)
{
    const wxArrayString &listLang = m_Theme->GetAllHighlightLanguages();

    wxMenu menu;
    for (size_t ii = 0; ii < listLang.GetCount(); ++ii)
    {
        wxMenu *optionsMenu = new wxMenu;
        const wxString &langID = m_Theme->GetHighlightLanguage(listLang[ii]);
        for (int optIndex = 0; optIndex < m_Theme->GetOptionCount(langID); ++optIndex)
        {
            const OptionColour* opt = m_Theme->GetOptionByIndex(langID, optIndex);
            if (optionsMenu->FindItem(opt->name) == wxNOT_FOUND)
            {
                MenuItemLanguageOptionID id;
                id.index = optIndex;
                id.langID = langID;

                long menuID = wxNewId();

                optionsMenu->Append(menuID, opt->name);
                Connect(menuID, wxEVT_COMMAND_MENU_SELECTED,
                        wxCommandEventHandler(EditorConfigurationDlg::OnMenuColoursCopyFrom));
                m_MenuIDToLanguageOption.insert(MenuIDToLanguageOption::value_type(menuID, id));
            }
        }
        menu.AppendSubMenu(optionsMenu, listLang[ii]);
    }

    PopupMenu(&menu);

    for (MenuIDToLanguageOption::const_iterator it = m_MenuIDToLanguageOption.begin();
         it != m_MenuIDToLanguageOption.end();
         ++it)
    {
        Disconnect(it->first, wxEVT_COMMAND_MENU_SELECTED,
                   wxCommandEventHandler(EditorConfigurationDlg::OnMenuColoursCopyFrom));
    }
    m_MenuIDToLanguageOption.clear();
}

void EditorConfigurationDlg::OnMenuColoursCopyFrom(wxCommandEvent &event)
{
    if (!m_Theme)
        return;
    long id = event.GetId();
    MenuIDToLanguageOption::const_iterator it = m_MenuIDToLanguageOption.find(id);
    if (it == m_MenuIDToLanguageOption.end())
        return;

    MenuItemLanguageOptionID option = it->second;
    const OptionColour* optSource = m_Theme->GetOptionByIndex(option.langID, option.index);

    wxListBox* colours = XRCCTRL(*this, "lstComponents", wxListBox);
    OptionColour* optDest = m_Theme->GetOptionByName(m_Lang, colours->GetStringSelection());
    if (optSource && optDest)
    {
        optDest->back = optSource->back;
        optDest->fore = optSource->fore;
        optDest->bold = optSource->bold;
        optDest->italics = optSource->italics;
        optDest->underlined = optSource->underlined;

        UpdateColourControls(optDest);
        ApplyColours();
    }
}

void EditorConfigurationDlg::OnColoursCopyAllFrom(cb_unused wxCommandEvent &event)
{
    const wxArrayString &listLang = m_Theme->GetAllHighlightLanguages();

    wxMenu menu;
    for (size_t ii = 0; ii < listLang.GetCount(); ++ii)
    {
        MenuItemLanguageOptionID id;
        id.index = -1;
        id.langID = m_Theme->GetHighlightLanguage(listLang[ii]);;

        long menuID = wxNewId();

        menu.Append(menuID, listLang[ii]);
        Connect(menuID, wxEVT_COMMAND_MENU_SELECTED,
                wxCommandEventHandler(EditorConfigurationDlg::OnMenuColoursCopyAllFrom));
        m_MenuIDToLanguageOption.insert(MenuIDToLanguageOption::value_type(menuID, id));
    }

    PopupMenu(&menu);

    for (MenuIDToLanguageOption::const_iterator it = m_MenuIDToLanguageOption.begin();
         it != m_MenuIDToLanguageOption.end();
         ++it)
    {
        Disconnect(it->first, wxEVT_COMMAND_MENU_SELECTED,
                   wxCommandEventHandler(EditorConfigurationDlg::OnMenuColoursCopyAllFrom));
    }
    m_MenuIDToLanguageOption.clear();
}

void EditorConfigurationDlg::OnMenuColoursCopyAllFrom(wxCommandEvent &event)
{
    if (!m_Theme)
        return;
    long id = event.GetId();
    MenuIDToLanguageOption::const_iterator it = m_MenuIDToLanguageOption.find(id);
    if (it == m_MenuIDToLanguageOption.end())
        return;
    const wxString &srcLang = it->second.langID;
    for (int destIndex = 0; destIndex < m_Theme->GetOptionCount(m_Lang); ++destIndex)
    {
        OptionColour *optDest = m_Theme->GetOptionByIndex(m_Lang, destIndex);
        if (!optDest)
            continue;
        OptionColour *optSource = m_Theme->GetOptionByName(srcLang, optDest->name);
        // if the option is not found and we are looking for comment,
        // then try to get the C/C++ option for comments.
        if (!optSource && optDest->name == wxT("Comment"))
            optSource = m_Theme->GetOptionByName(srcLang, wxT("Comment (normal)"));
        if (optSource)
        {
            optDest->back = optSource->back;
            optDest->fore = optSource->fore;
            optDest->bold = optSource->bold;
            optDest->italics = optSource->italics;
            optDest->underlined = optSource->underlined;
        }
    }
    ApplyColours();
    ReadColours();
}

void EditorConfigurationDlg::OnChangeLang(cb_unused wxCommandEvent& event)
{
    if (m_Theme)
    {
        wxString sel = XRCCTRL(*this, "cmbLangs", wxChoice)->GetStringSelection();
        m_Lang = m_Theme->GetHighlightLanguage(sel);
    }
    FillColourComponents();
    CreateColoursSample();
}

void EditorConfigurationDlg::OnChangeDefCodeFileType(cb_unused wxCommandEvent& event)
{
    int sel = XRCCTRL(*this, "cmbDefCodeFileType", wxChoice)->GetSelection();
    if (sel != m_DefCodeFileType)
    {   // update array for previous selected and show the code for the newly selected
        m_DefaultCode[m_DefCodeFileType] = XRCCTRL(*this, "txtDefCode", wxTextCtrl)->GetValue();
        m_DefCodeFileType = sel;
        XRCCTRL(*this, "txtDefCode", wxTextCtrl)->SetValue(m_DefaultCode[m_DefCodeFileType]);
    }
}

void EditorConfigurationDlg::OnChooseColour(wxCommandEvent& event)
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
        sender->SetLabel(wxEmptyString);
    }

    if (event.GetId() == XRCID("btnColoursFore") ||
        event.GetId() == XRCID("btnColoursBack"))
        WriteColours();
}

void EditorConfigurationDlg::OnSetDefaultColour(wxCommandEvent& event)
{
    wxWindow *button = nullptr;
    if (event.GetId() == XRCID("btnForeSetDefault"))
        button = FindWindowById(XRCID("btnColoursFore"));
    else if (event.GetId() == XRCID("btnBackSetDefault"))
        button = FindWindowById(XRCID("btnColoursBack"));
    if (!button)
        return;
    button->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    button->SetLabel(_("\"Default\""));
    WriteColours();
}

void EditorConfigurationDlg::OnChooseFont(cb_unused wxCommandEvent& event)
{
    UpdateSampleFont(true);
}

void EditorConfigurationDlg::OnColourComponent(cb_unused wxCommandEvent& event)
{
    ReadColours();
}

void EditorConfigurationDlg::OnBoldItalicUline(cb_unused wxCommandEvent& event)
{
    WriteColours();
}

void EditorConfigurationDlg::OnDynamicCheck(wxCommandEvent& event)
{
    XRCCTRL(*this, "spnMarginWidth", wxSpinCtrl)->Enable(!event.IsChecked());
}

void EditorConfigurationDlg::EndModal(int retCode)
{
    if (retCode == wxID_OK)
    {
        ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));

        cfg->Write(_T("/font"), XRCCTRL(*this, "lblEditorFont", wxStaticText)->GetFont().GetNativeFontInfoDesc());

        cfg->Write(_T("/auto_indent"),                         XRCCTRL(*this, "chkAutoIndent",               wxCheckBox)->GetValue());
        cfg->Write(_T("/smart_indent"),                        XRCCTRL(*this, "chkSmartIndent",              wxCheckBox)->GetValue());
        cfg->Write(_T("/brace_completion"),                    XRCCTRL(*this, "chkBraceCompletion",          wxCheckBox)->GetValue());
        cfg->Write(_T("/detect_indent"),                       XRCCTRL(*this, "chkDetectIndent",             wxCheckBox)->GetValue());
        cfg->Write(_T("/use_tab"),                             XRCCTRL(*this, "chkUseTab",                   wxCheckBox)->GetValue());
        cfg->Write(_T("/show_indent_guides"),                  XRCCTRL(*this, "chkShowIndentGuides",         wxCheckBox)->GetValue());
        cfg->Write(_T("/brace_smart_indent"),                  XRCCTRL(*this, "chkBraceSmartIndent",         wxCheckBox)->GetValue());
        cfg->Write(_T("/selection_brace_completion"),          XRCCTRL(*this, "chkSelectionBraceCompletion", wxCheckBox)->GetValue());
        cfg->Write(_T("/tab_indents"),                         XRCCTRL(*this, "chkTabIndents",               wxCheckBox)->GetValue());
        cfg->Write(_T("/backspace_unindents"),                 XRCCTRL(*this, "chkBackspaceUnindents",       wxCheckBox)->GetValue());
        cfg->Write(_T("/word_wrap"),                           XRCCTRL(*this, "chkWordWrap",                 wxCheckBox)->GetValue());
        cfg->Write(_T("/word_wrap_style_home_end"),            XRCCTRL(*this, "chkWordWrapStyleHomeEnd",     wxCheckBox)->GetValue());
        cfg->Write(_T("/use_posix_style_regexes"),             XRCCTRL(*this, "chkPosixRegex",               wxCheckBox)->GetValue());
        #ifdef wxHAS_REGEX_ADVANCED
        cfg->Write(_T("/use_advanced_regexes"),                XRCCTRL(*this, "chkAdvancedRegex",            wxCheckBox)->GetValue());
        #endif

        cfg->Write(_T("/show_line_numbers"),                   XRCCTRL(*this, "chkShowLineNumbers",    wxCheckBox)->GetValue());
        cfg->Write(_T("/highlight_caret_line"),                XRCCTRL(*this, "chkHighlightCaretLine", wxCheckBox)->GetValue());
        cfg->Write(_T("/simplified_home"),                     XRCCTRL(*this, "chkSimplifiedHome",     wxCheckBox)->GetValue());
        cfg->Write(_T("/camel_case"),                          XRCCTRL(*this, "chkCamelCase",          wxCheckBox)->GetValue());

        cfg->Write(_T("/track_preprocessor"),                  XRCCTRL(*this, "chkTrackPreprocessor",  wxCheckBox)->GetValue());
        cfg->Write(_T("/collect_prj_defines"),                 XRCCTRL(*this, "chkCollectPrjDefines",  wxCheckBox)->GetValue());
        cfg->Write(_T("/platform_defines"),                    XRCCTRL(*this, "chkPlatDefines",        wxCheckBox)->GetValue());
        cfg->Write(_T("/highlight_wxsmith"),                   XRCCTRL(*this, "chkColoursWxSmith",     wxCheckBox)->GetValue());
        cfg->Write(_T("/no_stl_in_c"),                         XRCCTRL(*this, "chkNoStlC",             wxCheckBox)->GetValue());

        bool resetZoom = XRCCTRL(*this, "chkResetZoom", wxCheckBox)->GetValue();
        bool zoomAll = XRCCTRL(*this, "chkZoomAll", wxCheckBox)->GetValue();
        if (zoomAll || resetZoom)
        {
            EditorManager* em = Manager::Get()->GetEditorManager();
            if (resetZoom)
                em->SetZoom(0);
            em->GetNotebook()->SetZoom(em->GetZoom());
        }
        cfg->Write(_T("/reset_zoom"),                          resetZoom);
        cfg->Write(_T("/zoom_all"),                            zoomAll);

        cfg->Write(_T("/tab_size"),                            XRCCTRL(*this, "spnTabSize",                           wxSpinCtrl)->GetValue());
        cfg->Write(_T("/view_whitespace"),                     XRCCTRL(*this, "cmbViewWS",                            wxChoice)->GetSelection());
        cfg->Write(_T("/open_containing_folder"),              XRCCTRL(*this, "txtOpenFolder",                        wxTextCtrl)->GetValue());
        cfg->Write(_T("/tab_text_relative"),                   XRCCTRL(*this, "rbTabText",                            wxRadioBox)->GetSelection() ? true : false);
        // find & replace, regex searches

        //caret
        cfg->Write(_T("/caret/style"),                         XRCCTRL(*this, "lstCaretStyle",  wxChoice)->GetSelection());
        cfg->Write(_T("/caret/width"),                         XRCCTRL(*this, "spnCaretWidth",  wxSpinCtrl)->GetValue());
        wxColour caretColour = XRCCTRL(*this, "btnCaretColour", wxButton)->GetBackgroundColour();
        Manager::Get()->GetColourManager()->SetColour(wxT("editor_caret"), caretColour);
        cfg->Write(_T("/caret/period"),                        XRCCTRL(*this, "slCaretPeriod",  wxSlider)->GetValue());

        //folding
        bool enableFolding = XRCCTRL(*this, "chkEnableFolding", wxCheckBox)->GetValue();
        if (!enableFolding)
        {
            //if the folding has been disabled, first unfold
            //all blocks in all editors
            EditorManager *em = Manager::Get()->GetEditorManager();
            for (int idx = 0; idx<em->GetEditorsCount(); ++idx)
            {
                cbEditor *ed = em->GetBuiltinEditor(em->GetEditor(idx));
                if(ed)
                    ed->UnfoldAll();
            }
        }

        cfg->Write(_T("/folding/show_folds"), enableFolding);
        cfg->Write(_T("/folding/fold_all_on_open"),        XRCCTRL(*this, "chkFoldOnOpen",          wxCheckBox)->GetValue());
        cfg->Write(_T("/folding/fold_preprocessor"),       XRCCTRL(*this, "chkFoldPreprocessor",    wxCheckBox)->GetValue());
        cfg->Write(_T("/folding/fold_comments"),           XRCCTRL(*this, "chkFoldComments",        wxCheckBox)->GetValue());
        cfg->Write(_T("/folding/fold_xml"),                XRCCTRL(*this, "chkFoldXml",             wxCheckBox)->GetValue());
        cfg->Write(_T("/folding/underline_folded_line"),   XRCCTRL(*this, "chkUnderlineFoldedLine", wxCheckBox)->GetValue());
        cfg->Write(_T("/folding/indicator"),               XRCCTRL(*this, "lstIndicators",          wxChoice)->GetSelection());
        cfg->Write(_T("/folding/limit"),                   XRCCTRL(*this, "chkFoldLimit",           wxCheckBox)->GetValue());
        cfg->Write(_T("/folding/limit_level"),             XRCCTRL(*this, "spnFoldLimitLevel",      wxSpinCtrl)->GetValue());

        //eol
        cfg->Write(_T("/show_eol"),                        XRCCTRL(*this, "chkShowEOL",             wxCheckBox)->GetValue());
        cfg->Write(_T("/eol/strip_trailing_spaces"),       XRCCTRL(*this, "chkStripTrailings",      wxCheckBox)->GetValue());
        cfg->Write(_T("/eol/ensure_final_line_end"),       XRCCTRL(*this, "chkEnsureFinalEOL",      wxCheckBox)->GetValue());
        cfg->Write(_T("/eol/ensure_consistent_line_ends"), XRCCTRL(*this, "chkEnsureConsistentEOL", wxCheckBox)->GetValue());
        cfg->Write(_T("/eol/eolmode"),                (int)XRCCTRL(*this, "cmbEOLMode",             wxChoice)->GetSelection());

        //gutter
        cfg->Write(_T("/gutter/mode"),                     XRCCTRL(*this, "lstGutterMode",   wxChoice)->GetSelection());
        wxColour gutterColour = XRCCTRL(*this, "btnGutterColour", wxButton)->GetBackgroundColour();
        Manager::Get()->GetColourManager()->SetColour(wxT("editor_gutter"), gutterColour);
        cfg->Write(_T("/gutter/column"),                   XRCCTRL(*this, "spnGutterColumn", wxSpinCtrl)->GetValue());

        //margin
        cfg->Write(_T("/margin/width_chars"),              XRCCTRL(*this, "spnMarginWidth",      wxSpinCtrl)->GetValue());
        cfg->Write(_T("/margin/dynamic_width"),            XRCCTRL(*this, "chkDynamicWidth",     wxCheckBox)->GetValue());
        cfg->Write(_T("/margin_1_sensitive"),        (bool)XRCCTRL(*this, "chkAddBPByLeftClick", wxCheckBox)->GetValue());
        cfg->Write(_T("/margin_1_image_bp"),         (bool)XRCCTRL(*this, "chkImageBP",          wxCheckBox)->GetValue());

        //selections
        cfg->Write(_T("/selection/use_vspace"),      (bool)XRCCTRL(*this, "chkEnableVirtualSpace",              wxCheckBox)->GetValue());
        cfg->Write(_T("/selection/multi_select"),    (bool)XRCCTRL(*this, "chkEnableMultipleSelections",        wxCheckBox)->GetValue());
        cfg->Write(_T("/selection/multi_typing"),    (bool)XRCCTRL(*this, "chkEnableAdditionalSelectionTyping", wxCheckBox)->GetValue());

        //scrollbar
        cfg->Write(_T("/margin/scroll_width_tracking"),    XRCCTRL(*this, "chkScrollWidthTracking", wxCheckBox)->GetValue());

        //changebar
        bool enableChangebar = XRCCTRL(*this, "chkUseChangebar", wxCheckBox)->GetValue();
        cfg->Write(_T("/margin/use_changebar"), enableChangebar);
        if (enableChangebar != m_EnableChangebar)
        {
            EditorManager *em = Manager::Get()->GetEditorManager();
            for (int idx = 0; idx<em->GetEditorsCount(); ++idx)
            {
                cbEditor *ed = em->GetBuiltinEditor(em->GetEditor(idx));
                if(ed)
                {
                    // if we enable changeCollection, we also have to empty Undo-Buffer, to avoid inconsistences,
                    // if we disable it, there is no need to do that
                    enableChangebar?
                        ed->ClearHistory():
                        ed->SetChangeCollection(false);
                }
            }
        }
        // default code : first update what's in the current txtCtrl,
        // and then write them all to the config file (even if unmodified)
        int sel = XRCCTRL(*this, "cmbDefCodeFileType", wxChoice)->GetSelection();
        m_DefaultCode[sel] = XRCCTRL(*this, "txtDefCode", wxTextCtrl)->GetValue();
        for(size_t idx = 0; idx < sizeof(IdxToFileType)/sizeof(*IdxToFileType); ++ idx)
        {
            wxString key;
            key.Printf(_T("/default_code/set%d"), IdxToFileType[idx]);
            m_DefaultCode.Add(cfg->Read(key, wxEmptyString));
            cfg->Write(key, m_DefaultCode[idx]);
        }// end for : idx


        if (m_Theme)
        {
            m_Theme->Save();
            Manager::Get()->GetEditorManager()->SetColourSet(m_Theme);
            cfg->Write(_T("/colour_sets/active_colour_set"), m_Theme->GetName());
        }
        cfg->Write(_T("/colour_sets/active_lang"), XRCCTRL(*this, "cmbLangs", wxChoice)->GetStringSelection());

        // encoding
        wxChoice* cmbEnc = XRCCTRL(*this, "cmbEncoding", wxChoice);
        if (cmbEnc)
        {
            cfg->Write(_T("/default_encoding"), cmbEnc->GetStringSelection());
        }
        cfg->Write(_T("/default_encoding/use_option"),  XRCCTRL(*this, "rbEncodingUseOption", wxRadioBox)->GetSelection());
        cfg->Write(_T("/default_encoding/find_latin2"), XRCCTRL(*this, "chkEncodingFindLatin2", wxCheckBox)->GetValue());
        cfg->Write(_T("/default_encoding/use_system"),  XRCCTRL(*this, "chkEncodingUseSystem", wxCheckBox)->GetValue());

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

void EditorConfigurationDlg::OnMultipleSelections(wxCommandEvent& event)
{
    XRCCTRL(*this, "chkEnableAdditionalSelectionTyping", wxCheckBox)->Enable( event.IsChecked() );
}

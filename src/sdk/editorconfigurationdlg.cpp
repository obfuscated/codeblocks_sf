/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/xrc/xmlres.h>
    #include <wx/spinctrl.h>
    #include <wx/combobox.h>

    #include "manager.h"
    #include "configmanager.h"
    #include "pluginmanager.h"
    #include "editormanager.h"
    #include "cbeditor.h"
    #include "cbplugin.h" // cgEditor
    #include "globals.h"
    #include <wx/listbook.h>
    #include <wx/listbox.h>
    #include <wx/colordlg.h>
    #include <wx/radiobox.h>
    #include <wx/imaglist.h>
#endif

#include <wx/fontdlg.h>
#include <wx/fontutil.h>
#include <wx/fontmap.h>
#include <wx/textctrl.h>
#include <wx/listbook.h>
#include <wx/listctrl.h>

#include "configurationpanel.h"
#include "editorcolourset.h"
#include "editorconfigurationdlg.h"
#include "editkeywordsdlg.h"

// images by order of pages
const wxString base_imgs[] =
{
    _T("editor"),
    _T("folding"),
    _T("gutter-margin"),
    _T("syntax-highlight"),
    _T("abbrev"),
    _T("default-code"),
};
const int IMAGES_COUNT = sizeof(base_imgs) / sizeof(wxString);

// map cmbDefCodeFileType indexes to FileType values
// if more entries are added to cmbDefCodeFileType, edit the mapping here
const FileType IdxToFileType[] = { ftSource, ftHeader };

BEGIN_EVENT_TABLE(EditorConfigurationDlg, wxDialog)
	EVT_BUTTON(XRCID("btnChooseEditorFont"), 	EditorConfigurationDlg::OnChooseFont)
	EVT_BUTTON(XRCID("btnKeywords"), 			EditorConfigurationDlg::OnEditKeywords)
	EVT_BUTTON(XRCID("btnFilemasks"), 			EditorConfigurationDlg::OnEditFilemasks)
	EVT_BUTTON(XRCID("btnColoursReset"), 		EditorConfigurationDlg::OnColoursReset)
	EVT_BUTTON(XRCID("btnCaretColour"), 		    EditorConfigurationDlg::OnChooseColour)
	EVT_BUTTON(XRCID("btnGutterColour"), 		EditorConfigurationDlg::OnChooseColour)
	EVT_BUTTON(XRCID("btnColoursFore"), 			EditorConfigurationDlg::OnChooseColour)
	EVT_BUTTON(XRCID("btnColoursBack"), 			EditorConfigurationDlg::OnChooseColour)
	EVT_BUTTON(XRCID("btnColoursAddTheme"), 		EditorConfigurationDlg::OnAddColourTheme)
	EVT_BUTTON(XRCID("btnColoursDeleteTheme"), 	EditorConfigurationDlg::OnDeleteColourTheme)
	EVT_BUTTON(XRCID("btnColoursRenameTheme"), 	EditorConfigurationDlg::OnRenameColourTheme)
	EVT_CHECKBOX(XRCID("chkColoursBold"),		EditorConfigurationDlg::OnBoldItalicUline)
	EVT_CHECKBOX(XRCID("chkColoursItalics"),		EditorConfigurationDlg::OnBoldItalicUline)
	EVT_CHECKBOX(XRCID("chkColoursUnderlined"),	EditorConfigurationDlg::OnBoldItalicUline)
	EVT_LISTBOX(XRCID("lstComponents"),			EditorConfigurationDlg::OnColourComponent)
	EVT_COMBOBOX(XRCID("cmbLangs"),				EditorConfigurationDlg::OnChangeLang)
	EVT_COMBOBOX(XRCID("cmbDefCodeFileType"),	EditorConfigurationDlg::OnChangeDefCodeFileType)
	EVT_COMBOBOX(XRCID("cmbThemes"),	        EditorConfigurationDlg::OnColourTheme)
	EVT_LISTBOX(XRCID("lstAutoCompKeyword"),	EditorConfigurationDlg::OnAutoCompKeyword)
	EVT_BUTTON(XRCID("btnAutoCompAdd"),	        EditorConfigurationDlg::OnAutoCompAdd)
	EVT_BUTTON(XRCID("btnAutoCompDelete"),	    EditorConfigurationDlg::OnAutoCompDelete)
	EVT_CHECKBOX(XRCID("chkDynamicWidth"),      EditorConfigurationDlg::OnDynamicCheck)

    EVT_LISTBOOK_PAGE_CHANGED(XRCID("nbMain"), EditorConfigurationDlg::OnPageChanged)
END_EVENT_TABLE()

EditorConfigurationDlg::EditorConfigurationDlg(wxWindow* parent)
	: m_TextColourControl(0L),
	m_AutoCompTextControl(0L),
	m_Theme(0L),
	m_Lang(HL_NONE),
	m_DefCodeFileType(0),
	m_ThemeModified(false),
	m_LastAutoCompKeyword(-1)
{
	wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgConfigureEditor"));

	XRCCTRL(*this, "lblEditorFont", wxStaticText)->SetLabel(_("This is sample text"));
	UpdateSampleFont(false);

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));

   	XRCCTRL(*this, "chkAutoIndent", wxCheckBox)->SetValue(cfg->ReadBool(_T("/auto_indent"), true));
   	XRCCTRL(*this, "chkSmartIndent", wxCheckBox)->SetValue(cfg->ReadBool(_T("/smart_indent"), true));
   	XRCCTRL(*this, "chkUseTab", wxCheckBox)->SetValue(cfg->ReadBool(_T("/use_tab"), false));
   	XRCCTRL(*this, "chkShowIndentGuides", wxCheckBox)->SetValue(cfg->ReadBool(_T("/show_indent_guides"), false));
   	XRCCTRL(*this, "chkTabIndents", wxCheckBox)->SetValue(cfg->ReadBool(_T("/tab_indents"), true));
   	XRCCTRL(*this, "chkBackspaceUnindents", wxCheckBox)->SetValue(cfg->ReadBool(_T("/backspace_unindents"), true));
   	XRCCTRL(*this, "chkWordWrap", wxCheckBox)->SetValue(cfg->ReadBool(_T("/word_wrap"), false));
   	XRCCTRL(*this, "chkPosixRegex", wxCheckBox)->SetValue(cfg->ReadBool(_T("/use_posix_style_regexes"), false));
   	XRCCTRL(*this, "chkAdvancedRegex", wxCheckBox)->SetValue(cfg->ReadBool(_T("/use_advanced_regexes"), false));
   	XRCCTRL(*this, "chkShowLineNumbers", wxCheckBox)->SetValue(cfg->ReadBool(_T("/show_line_numbers"), true));
   	XRCCTRL(*this, "chkHighlightCaretLine", wxCheckBox)->SetValue(cfg->ReadBool(_T("/highlight_caret_line"), false));
   	XRCCTRL(*this, "spnTabSize", wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/tab_size"), 4));
   	XRCCTRL(*this, "cmbViewWS", wxComboBox)->SetSelection(cfg->ReadInt(_T("/view_whitespace"), 0));
   	XRCCTRL(*this, "rbTabText", wxRadioBox)->SetSelection(cfg->ReadBool(_T("/tab_text_relative"), true) ? 1 : 0);

    // NOTE: duplicate line in cbeditor.cpp (CreateEditor)
    const int default_eol = platform::windows ? wxSCI_EOL_CRLF : wxSCI_EOL_LF; // Windows takes CR+LF, other platforms LF only

   	XRCCTRL(*this, "chkShowEOL", wxCheckBox)->SetValue(cfg->ReadBool(_T("/show_eol"), false));
   	XRCCTRL(*this, "chkStripTrailings", wxCheckBox)->SetValue(cfg->ReadBool(_T("/eol/strip_trailing_spaces"), true));
   	XRCCTRL(*this, "chkEnsureFinalEOL", wxCheckBox)->SetValue(cfg->ReadBool(_T("/eol/ensure_final_line_end"), true));
   	XRCCTRL(*this, "chkEnsureConsistentEOL", wxCheckBox)->SetValue(cfg->ReadBool(_T("/eol/ensure_consistent_line_ends"), false));
    XRCCTRL(*this, "cmbEOLMode", wxComboBox)->SetSelection(cfg->ReadInt(_T("/eol/eolmode"), default_eol));

    //caret
    wxColour caretColour = cfg->ReadColour(_T("/caret/colour"), *wxBLACK);
    XRCCTRL(*this, "spnCaretWidth", wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/caret/width"), 1));
    XRCCTRL(*this, "btnCaretColour", wxButton)->SetBackgroundColour(caretColour);
    XRCCTRL(*this, "slCaretPeriod", wxSlider)->SetValue(cfg->ReadInt(_T("/caret/period"), 500));

	//folding
   	XRCCTRL(*this, "chkEnableFolding", wxCheckBox)->SetValue(cfg->ReadBool(_T("/folding/show_folds"), true));
   	XRCCTRL(*this, "chkFoldOnOpen", wxCheckBox)->SetValue(cfg->ReadBool(_T("/folding/fold_all_on_open"), false));
   	XRCCTRL(*this, "chkFoldPreprocessor", wxCheckBox)->SetValue(cfg->ReadBool(_T("/folding/fold_preprocessor"), false));
   	XRCCTRL(*this, "chkFoldComments", wxCheckBox)->SetValue(cfg->ReadBool(_T("/folding/fold_comments"), true));
   	XRCCTRL(*this, "chkFoldXml", wxCheckBox)->SetValue(cfg->ReadBool(_T("/folding/fold_xml"), true));

	//gutter
    wxColour gutterColour = cfg->ReadColour(_T("/gutter/colour"), *wxLIGHT_GREY);
    XRCCTRL(*this, "lstGutterMode", wxChoice)->SetSelection(cfg->ReadInt(_T("/gutter/mode"), 0));
    XRCCTRL(*this, "btnGutterColour", wxButton)->SetBackgroundColour(gutterColour);
    XRCCTRL(*this, "spnGutterColumn", wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/gutter/column"), 80));
    XRCCTRL(*this, "spnGutterColumn", wxSpinCtrl)->SetRange(1, 500);

    //margin
    XRCCTRL(*this, "spnMarginWidth", wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/margin/width_chars"), 6));
    XRCCTRL(*this, "chkDynamicWidth", wxCheckBox)->SetValue(cfg->ReadBool(_T("/margin/dynamic_width"), false));
    XRCCTRL(*this, "spnMarginWidth", wxSpinCtrl)->Enable(!cfg->ReadBool(_T("/margin/dynamic_width"), false));
    XRCCTRL(*this, "chkAddBPByLeftClick", wxCheckBox)->SetValue(cfg->ReadBool(_T("/margin_1_sensitive"), true));

	// colour set
	LoadThemes();

    // fill encodings
    wxComboBox* cmbEnc = XRCCTRL(*this, "cmbEncoding", wxComboBox);
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

    // auto-complete
    CreateAutoCompText();
    wxListBox* lstKeyword = XRCCTRL(*this, "lstAutoCompKeyword", wxListBox);
    lstKeyword->Clear();
    m_AutoCompMap = Manager::Get()->GetEditorManager()->GetAutoCompleteMap();
    AutoCompleteMap::iterator it;
    for (it = m_AutoCompMap.begin(); it != m_AutoCompMap.end(); ++it)
    {
    	lstKeyword->Append(it->first);
    }
    if (m_AutoCompMap.size() != 0)
    {
        lstKeyword->SetSelection(0);
        m_LastAutoCompKeyword = 0;
        it = m_AutoCompMap.begin();
        m_AutoCompTextControl->SetText(it->second);
    }

	// default code
    wxString key;
    key.Printf(_T("/default_code/set%d"), IdxToFileType[m_DefCodeFileType]);
    XRCCTRL(*this, "txtDefCode", wxTextCtrl)->SetValue(cfg->Read(key, wxEmptyString));

    // 8 point is not readable on Mac OS X, increase font size:
    wxFont tmpFont(platform::macosx ? 10 : 8, wxMODERN, wxNORMAL, wxNORMAL);

    XRCCTRL(*this, "txtDefCode", wxTextCtrl)->SetFont(tmpFont);
	// read them all in the array
	for(size_t idx = 0; idx < sizeof(IdxToFileType)/sizeof(*IdxToFileType); ++ idx)
	{
		wxString key;
		key.Printf(_T("/default_code/set%d"), IdxToFileType[idx]);
		m_DefaultCode.Add(cfg->Read(key, wxEmptyString));
	}// end for : idx

    // load listbook images
    const wxString base = ConfigManager::GetDataFolder() + _T("/images/settings/");

    wxImageList* images = new wxImageList(80, 80);
    wxBitmap bmp;
    for (int i = 0; i < IMAGES_COUNT; ++i)
    {
        bmp = cbLoadBitmap(base + base_imgs[i] + _T(".png"), wxBITMAP_TYPE_PNG);
        images->Add(bmp);
        bmp = cbLoadBitmap(base + base_imgs[i] + _T("-off.png"), wxBITMAP_TYPE_PNG);
        images->Add(bmp);
    }
    wxListbook* lb = XRCCTRL(*this, "nbMain", wxListbook);
    lb->AssignImageList(images);
    int sel = Manager::Get()->GetConfigManager(_T("app"))->ReadInt(_T("/environment/settings_size"), 0);
    SetSettingsIconsStyle(lb->GetListView(), (SettingsIconsStyle)sel);

    // add all plugins configuration panels
    AddPluginPanels();

	// the following code causes a huge dialog to be created with wx2.8.4
	// commenting it out fixes the problem (along with some XRC surgery)
	// if this causes problems with earlier wx versions we might need to
	// add a platform/version #ifdef...
	
    // make sure everything is laid out properly
//    GetSizer()->SetSizeHints(this);
}

EditorConfigurationDlg::~EditorConfigurationDlg()
{
	if (m_Theme)
		delete m_Theme;

	if (m_TextColourControl)
		delete m_TextColourControl;

    if (m_AutoCompTextControl)
        delete m_AutoCompTextControl;
}

void EditorConfigurationDlg::AddPluginPanels()
{
    const wxString base = ConfigManager::GetDataFolder() + _T("/images/settings/");

    wxListbook* lb = XRCCTRL(*this, "nbMain", wxListbook);
    // get all configuration panels which are about the editor.
    Manager::Get()->GetPluginManager()->GetConfigurationPanels(cgEditor, lb, m_PluginPanels);

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

void EditorConfigurationDlg::UpdateListbookImages()
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

void EditorConfigurationDlg::OnPageChanged(wxListbookEvent& event)
{
    UpdateListbookImages();
}

void EditorConfigurationDlg::CreateColoursSample()
{
    Freeze();
	if (m_TextColourControl)
		delete m_TextColourControl;
	m_TextColourControl = new cbStyledTextCtrl(this, wxID_ANY);
	m_TextColourControl->SetTabWidth(4);

    int breakLine = -1;
    int debugLine = -1;
    int errorLine = -1;
    wxString code = m_Theme->GetSampleCode(m_Lang, &breakLine, &debugLine, &errorLine);
    if (!code.IsEmpty())
        m_TextColourControl->LoadFile(code);

	m_TextColourControl->SetReadOnly(true);
	m_TextColourControl->SetCaretWidth(0);
    m_TextColourControl->SetMarginType(0, wxSCI_MARGIN_NUMBER);
    m_TextColourControl->SetMarginWidth(0, 32);
	ApplyColours();

    m_TextColourControl->SetMarginWidth(1, 0);
	if (breakLine != -1) m_TextColourControl->MarkerAdd(breakLine, 2); // breakpoint line
	if (debugLine != -1) m_TextColourControl->MarkerAdd(debugLine, 3); // active line
	if (errorLine != -1) m_TextColourControl->MarkerAdd(errorLine, 4); // error line

	FillColourComponents();
    wxXmlResource::Get()->AttachUnknownControl(_T("txtColoursSample"), m_TextColourControl);
    Thaw();
}

void EditorConfigurationDlg::CreateAutoCompText()
{
	if (m_AutoCompTextControl)
		delete m_AutoCompTextControl;
	m_AutoCompTextControl = new cbStyledTextCtrl(this, wxID_ANY);
	m_AutoCompTextControl->SetTabWidth(4);
    m_AutoCompTextControl->SetMarginType(0, wxSCI_MARGIN_NUMBER);
    m_AutoCompTextControl->SetMarginWidth(0, 32);
    m_AutoCompTextControl->SetViewWhiteSpace(1);
	ApplyColours();
    wxXmlResource::Get()->AttachUnknownControl(_T("txtAutoCompCode"), m_AutoCompTextControl);
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
		if (m_AutoCompTextControl)
		{
            m_AutoCompTextControl->StyleSetFont(wxSCI_STYLE_DEFAULT,fnt);
            m_Theme->Apply(m_Theme->GetHighlightLanguage(_T("C/C++")), m_AutoCompTextControl);
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

//			XRCCTRL(*this, "btnColorsFore", wxButton)->Enable(opt->isStyle);
			XRCCTRL(*this, "chkColoursBold", wxCheckBox)->Enable(opt->isStyle);
			XRCCTRL(*this, "chkColoursItalics", wxCheckBox)->Enable(opt->isStyle);
			XRCCTRL(*this, "chkColoursUnderlined", wxCheckBox)->Enable(opt->isStyle);
		}
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
			c = XRCCTRL(*this, "btnColoursBack", wxButton)->GetBackgroundColour();
            if (c != wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE))
                opt->back = c;
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
    // 8 point is not readable on Mac OS X, increase font size:
    wxFont tmpFont(platform::macosx ? 10 : 8, wxMODERN, wxNORMAL, wxNORMAL);
    wxString fontstring = Manager::Get()->GetConfigManager(_T("editor"))->Read(_T("/font"), wxEmptyString);

    if (!fontstring.IsEmpty())
    {
        wxNativeFontInfo nfi;
        nfi.FromString(fontstring);
        tmpFont.SetNativeFontInfo(nfi);
    }

	XRCCTRL(*this, "lblEditorFont", wxStaticText)->SetFont(tmpFont);
	if (!askForNewFont)
		return;

	wxFontData data;
    data.SetInitialFont(tmpFont);

	#if wxCHECK_VERSION(2, 8, 0)
	wxFontDialog dlg(this, data);
	#else
	wxFontDialog dlg(this, &data);
	#endif
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
    	wxFont font = dlg.GetFontData().GetChosenFont();
		XRCCTRL(*this, "lblEditorFont", wxStaticText)->SetFont(font);
		ApplyColours();
    }
}

void EditorConfigurationDlg::LoadThemes()
{
	wxComboBox* cmbThemes = XRCCTRL(*this, "cmbThemes", wxComboBox);
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
    wxComboBox* cmbThemes = XRCCTRL(*this, "cmbThemes", wxComboBox);
    if (m_Theme && m_ThemeModified)
    {
        wxString msg;
        msg.Printf(_("The colour theme \"%s\" is modified.\nDo you want to save the changes?"), m_Theme->GetName().c_str());
        int ret = cbMessageBox(msg, _("Save"), wxYES_NO | wxCANCEL);
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
    wxComboBox* cmbThemes = XRCCTRL(*this, "cmbThemes", wxComboBox);
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

	wxComboBox* cmbLangs = XRCCTRL(*this, "cmbLangs", wxComboBox);
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
		wxString sel = cmbLangs->GetStringSelection();
		m_Lang = m_Theme->GetHighlightLanguage(sel);
	}

	CreateColoursSample();
	m_ThemeModified = false;
}

// events

void EditorConfigurationDlg::OnColourTheme(wxCommandEvent& event)
{
    // theme has changed
    wxComboBox* cmbThemes = XRCCTRL(*this, "cmbThemes", wxComboBox);
    if (m_Theme && m_Theme->GetName() != cmbThemes->GetStringSelection())
    {
        if (AskToSaveTheme())
            ChangeTheme();
    }
}

void EditorConfigurationDlg::OnAddColourTheme(wxCommandEvent& event)
{
    wxTextEntryDialog dlg(this, _("Please enter the name of the new colour theme:"), _("New theme name"));
    PlaceWindow(&dlg);
    if (dlg.ShowModal() != wxID_OK)
        return;

    wxString name = dlg.GetValue();
    wxComboBox* cmbThemes = XRCCTRL(*this, "cmbThemes", wxComboBox);
    cmbThemes->Append(name);
    cmbThemes->SetSelection(cmbThemes->GetCount() - 1);
    ChangeTheme();
}

void EditorConfigurationDlg::OnDeleteColourTheme(wxCommandEvent& event)
{
    if (cbMessageBox(_("Are you sure you want to delete this theme?"), _("Confirmation"), wxYES_NO) == wxID_YES)
    {
        Manager::Get()->GetConfigManager(_T("editor"))->DeleteSubPath(_T("/colour_sets/") + m_Theme->GetName());
        wxComboBox* cmbThemes = XRCCTRL(*this, "cmbThemes", wxComboBox);
        int idx = cmbThemes->FindString(m_Theme->GetName());
        if (idx != wxNOT_FOUND)
            cmbThemes->Delete(idx);
        cmbThemes->SetSelection(wxNOT_FOUND);
        ChangeTheme();
    }
}

void EditorConfigurationDlg::OnRenameColourTheme(wxCommandEvent& event)
{
    wxTextEntryDialog dlg(this, _("Please enter the new name of the new colour theme:"), _("New theme name"), m_Theme->GetName());
    PlaceWindow(&dlg);
    if (dlg.ShowModal() != wxID_OK)
        return;

    wxString name = dlg.GetValue();
    wxString oldName = m_Theme->GetName();
    wxComboBox* cmbThemes = XRCCTRL(*this, "cmbThemes", wxComboBox);
    int idx = cmbThemes->GetSelection();
    if (idx != wxNOT_FOUND)
        cmbThemes->SetString(idx, name);
    m_Theme->SetName(name);
    m_Theme->Save();
    Manager::Get()->GetConfigManager(_T("editor"))->DeleteSubPath(_T("/colour_sets/") + oldName);

    cmbThemes->SetSelection(cmbThemes->FindString(name));
    ChangeTheme();
}

void EditorConfigurationDlg::OnEditKeywords(wxCommandEvent& event)
{
	if (m_Theme && m_Lang != HL_NONE)
	{
	    EditKeywordsDlg dlg(0, m_Theme, m_Lang);
        PlaceWindow(&dlg);
	    dlg.ShowModal();

        dlg.m_pTheme->SetKeywords(dlg.m_Lang, dlg.m_LastSet, dlg.txtKeywords->GetValue());
	}
}

void EditorConfigurationDlg::OnEditFilemasks(wxCommandEvent& event)
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

void EditorConfigurationDlg::OnColoursReset(wxCommandEvent& event)
{
	if (m_Theme && m_Lang != HL_NONE)
	{
	    wxString tmp;
	    tmp.Printf(_("Are you sure you want to reset all settings to defaults for \"%s\"?"),
                    m_Theme->GetLanguageName(m_Lang).c_str());
        if (cbMessageBox(tmp, _("Confirmation"), wxICON_QUESTION | wxYES_NO) == wxID_YES)
        {
            m_Theme->Reset(m_Lang);
            ApplyColours();
            ReadColours();
            m_ThemeModified = true;
        }
	}
}

void EditorConfigurationDlg::OnChangeLang(wxCommandEvent& event)
{
	if (m_Theme)
	{
		wxString sel = XRCCTRL(*this, "cmbLangs", wxComboBox)->GetStringSelection();
		m_Lang = m_Theme->GetHighlightLanguage(sel);
	}
	FillColourComponents();
	CreateColoursSample();
}

void EditorConfigurationDlg::OnChangeDefCodeFileType(wxCommandEvent& event)
{
	int sel = XRCCTRL(*this, "cmbDefCodeFileType", wxComboBox)->GetSelection();
	if (sel != m_DefCodeFileType)
	{	// update array for previous selected and show the code for the newly selected
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
    }

	if (event.GetId() == XRCID("btnColoursFore") ||
		event.GetId() == XRCID("btnColoursBack"))
		WriteColours();
}

void EditorConfigurationDlg::OnChooseFont(wxCommandEvent& event)
{
	UpdateSampleFont(true);
}

void EditorConfigurationDlg::OnColourComponent(wxCommandEvent& event)
{
	ReadColours();
}

void EditorConfigurationDlg::OnBoldItalicUline(wxCommandEvent& event)
{
	WriteColours();
}

void EditorConfigurationDlg::AutoCompUpdate(int index)
{
    if (index != -1)
    {
        wxListBox* lstKeyword = XRCCTRL(*this, "lstAutoCompKeyword", wxListBox);
        wxString lastSel = lstKeyword->GetString(index);
        if (m_AutoCompTextControl->GetText() != m_AutoCompMap[lastSel])
            m_AutoCompMap[lastSel] = m_AutoCompTextControl->GetText();
    }
}

void EditorConfigurationDlg::OnAutoCompAdd(wxCommandEvent& event)
{
    wxString key = wxGetTextFromUser(_("Please enter the new keyword"), _("Add keyword"));
    if (!key.IsEmpty())
    {
        AutoCompleteMap::iterator it = m_AutoCompMap.find(key);
        if (it != m_AutoCompMap.end())
        {
            cbMessageBox(_("This keyword already exists!"), _("Error"), wxICON_ERROR);
            return;
        }
        m_AutoCompMap[key] = _T("");
        wxListBox* lstKeyword = XRCCTRL(*this, "lstAutoCompKeyword", wxListBox);
        lstKeyword->Append(key);
        AutoCompUpdate(lstKeyword->GetSelection());
        m_AutoCompTextControl->SetText(_T(""));
        m_LastAutoCompKeyword = lstKeyword->GetCount() - 1;
        lstKeyword->SetSelection(m_LastAutoCompKeyword);
    }
}

void EditorConfigurationDlg::OnAutoCompDelete(wxCommandEvent& event)
{
    wxListBox* lstKeyword = XRCCTRL(*this, "lstAutoCompKeyword", wxListBox);
    if (lstKeyword->GetSelection() == -1)
        return;

    if (cbMessageBox(_("Are you sure you want to delete this keyword?"), _("Confirmation"), wxICON_QUESTION | wxYES_NO) == wxID_NO)
        return;

    int sel = lstKeyword->GetSelection();
    AutoCompleteMap::iterator it = m_AutoCompMap.find(lstKeyword->GetString(sel));
    if (it != m_AutoCompMap.end())
    {
        m_AutoCompMap.erase(it);
        lstKeyword->Delete(sel);
        if (sel >= lstKeyword->GetCount())
            sel = lstKeyword->GetCount() - 1;
        lstKeyword->SetSelection(sel);
        if (sel != -1)
        {
            m_AutoCompTextControl->SetText(m_AutoCompMap[lstKeyword->GetString(sel)]);
            m_LastAutoCompKeyword = sel;
        }
        else
            m_AutoCompTextControl->SetText(_T(""));
    }
}

void EditorConfigurationDlg::OnAutoCompKeyword(wxCommandEvent& event)
{
    wxListBox* lstKeyword = XRCCTRL(*this, "lstAutoCompKeyword", wxListBox);
    if (lstKeyword->GetSelection() == m_LastAutoCompKeyword)
        return;

    AutoCompUpdate(m_LastAutoCompKeyword);
    // list new keyword's code
    m_AutoCompTextControl->SetText(m_AutoCompMap[lstKeyword->GetString(lstKeyword->GetSelection())]);
    m_LastAutoCompKeyword = lstKeyword->GetSelection();
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

        cfg->Write(_T("/auto_indent"),			XRCCTRL(*this, "chkAutoIndent", wxCheckBox)->GetValue());
        cfg->Write(_T("/smart_indent"),			XRCCTRL(*this, "chkSmartIndent", wxCheckBox)->GetValue());
        cfg->Write(_T("/use_tab"), 				XRCCTRL(*this, "chkUseTab", wxCheckBox)->GetValue());
        cfg->Write(_T("/show_indent_guides"), 	XRCCTRL(*this, "chkShowIndentGuides", wxCheckBox)->GetValue());
        cfg->Write(_T("/tab_indents"), 			XRCCTRL(*this, "chkTabIndents", wxCheckBox)->GetValue());
        cfg->Write(_T("/backspace_unindents"), 	XRCCTRL(*this, "chkBackspaceUnindents", wxCheckBox)->GetValue());
        cfg->Write(_T("/word_wrap"), 			XRCCTRL(*this, "chkWordWrap", wxCheckBox)->GetValue());
        cfg->Write(_T("/use_posix_style_regexes"),XRCCTRL(*this, "chkPosixRegex", wxCheckBox)->GetValue());
        cfg->Write(_T("/use_advanced_regexes"), XRCCTRL(*this, "chkAdvancedRegex", wxCheckBox)->GetValue());

        cfg->Write(_T("/show_line_numbers"), 	XRCCTRL(*this, "chkShowLineNumbers", wxCheckBox)->GetValue());
        cfg->Write(_T("/highlight_caret_line"), XRCCTRL(*this, "chkHighlightCaretLine", wxCheckBox)->GetValue());
        cfg->Write(_T("/tab_size"),             XRCCTRL(*this, "spnTabSize", wxSpinCtrl)->GetValue());
        cfg->Write(_T("/view_whitespace"),      XRCCTRL(*this, "cmbViewWS", wxComboBox)->GetSelection());
        cfg->Write(_T("/tab_text_relative"),    XRCCTRL(*this, "rbTabText", wxRadioBox)->GetSelection() ? true : false);

        // find & replace, regex searches

        //caret
        cfg->Write(_T("/caret/width"), XRCCTRL(*this, "spnCaretWidth", wxSpinCtrl)->GetValue());
        cfg->Write(_T("/caret/colour"), XRCCTRL(*this, "btnCaretColour", wxButton)->GetBackgroundColour());
        cfg->Write(_T("/caret/period"), XRCCTRL(*this, "slCaretPeriod", wxSlider)->GetValue());

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

        cfg->Write(_T("/folding/show_folds"), 			enableFolding);
        cfg->Write(_T("/folding/fold_all_on_open"), 	XRCCTRL(*this, "chkFoldOnOpen", wxCheckBox)->GetValue());
        cfg->Write(_T("/folding/fold_preprocessor"), 	XRCCTRL(*this, "chkFoldPreprocessor", wxCheckBox)->GetValue());
        cfg->Write(_T("/folding/fold_comments"), 		XRCCTRL(*this, "chkFoldComments", wxCheckBox)->GetValue());
        cfg->Write(_T("/folding/fold_xml"), 		    XRCCTRL(*this, "chkFoldXml", wxCheckBox)->GetValue());

        //eol
        cfg->Write(_T("/show_eol"), 			        XRCCTRL(*this, "chkShowEOL", wxCheckBox)->GetValue());
        cfg->Write(_T("/eol/strip_trailing_spaces"),    XRCCTRL(*this, "chkStripTrailings", wxCheckBox)->GetValue());
        cfg->Write(_T("/eol/ensure_final_line_end"),    XRCCTRL(*this, "chkEnsureFinalEOL", wxCheckBox)->GetValue());
        cfg->Write(_T("/eol/ensure_consistent_line_ends"), XRCCTRL(*this, "chkEnsureConsistentEOL", wxCheckBox)->GetValue());
        cfg->Write(_T("/eol/eolmode"),                  (int)XRCCTRL(*this, "cmbEOLMode", wxComboBox)->GetSelection());

        //gutter
        cfg->Write(_T("/gutter/mode"), 			XRCCTRL(*this, "lstGutterMode", wxChoice)->GetSelection());
        cfg->Write(_T("/gutter/colour"),		    XRCCTRL(*this, "btnGutterColour", wxButton)->GetBackgroundColour());
        cfg->Write(_T("/gutter/column"), 		XRCCTRL(*this, "spnGutterColumn", wxSpinCtrl)->GetValue());

        //margin
        cfg->Write(_T("/margin/width_chars"), XRCCTRL(*this, "spnMarginWidth", wxSpinCtrl)->GetValue());
        cfg->Write(_T("/margin/dynamic_width"), XRCCTRL(*this, "chkDynamicWidth", wxCheckBox)->GetValue());
        cfg->Write(_T("/margin_1_sensitive"), (bool)XRCCTRL(*this, "chkAddBPByLeftClick", wxCheckBox)->GetValue());

		// default code : first update what's in the current txtCtrl,
		// and then write them all to the config file (even if unmodified)
		int sel = XRCCTRL(*this, "cmbDefCodeFileType", wxComboBox)->GetSelection();
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
        cfg->Write(_T("/colour_sets/active_lang"), XRCCTRL(*this, "cmbLangs", wxComboBox)->GetStringSelection());

        // encoding
        wxComboBox* cmbEnc = XRCCTRL(*this, "cmbEncoding", wxComboBox);
        if (cmbEnc)
        {
            cfg->Write(_T("/default_encoding"), cmbEnc->GetStringSelection());
        }

        // save any changes in auto-completion
        wxListBox* lstKeyword = XRCCTRL(*this, "lstAutoCompKeyword", wxListBox);
        AutoCompUpdate(lstKeyword->GetSelection());
        AutoCompleteMap& map = Manager::Get()->GetEditorManager()->GetAutoCompleteMap();
        map = m_AutoCompMap;

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

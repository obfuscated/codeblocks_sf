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
* $Id$
* $Date$
*/

#include <wx/stc/stc.h>
#include <wx/xrc/xmlres.h>
#include <wx/colordlg.h>
#include <wx/fontdlg.h>
#include <wx/fontutil.h>
#include <wx/fontmap.h>
#include <wx/spinctrl.h>
#include <wx/combobox.h>
#include <wx/radiobox.h>
#include <wx/listbox.h>
#include <wx/textctrl.h>

#include "editorcolorset.h"
#include "editorconfigurationdlg.h"
#include "manager.h"
#include "configmanager.h"
#include "editormanager.h"
#include "cbeditor.h"
#include "globals.h"

// map cmbDefCodeFileType indexes to FileType values
// if more entries are added to cmbDefCodeFileType, edit the mapping here
const FileType IdxToFileType[] = { ftSource, ftHeader };

BEGIN_EVENT_TABLE(EditorConfigurationDlg, wxDialog)
	EVT_BUTTON(XRCID("btnChooseEditorFont"), 	EditorConfigurationDlg::OnChooseFont)
	EVT_BUTTON(XRCID("btnKeywords"), 			EditorConfigurationDlg::OnEditKeywords)
	EVT_BUTTON(XRCID("btnColorsReset"), 		EditorConfigurationDlg::OnColorsReset)
	EVT_BUTTON(XRCID("btnGutterColor"), 		EditorConfigurationDlg::OnChooseColor)
	EVT_BUTTON(XRCID("btnColorsFore"), 			EditorConfigurationDlg::OnChooseColor)
	EVT_BUTTON(XRCID("btnColorsBack"), 			EditorConfigurationDlg::OnChooseColor)
	EVT_BUTTON(XRCID("btnColorsAddTheme"), 		EditorConfigurationDlg::OnAddColorTheme)
	EVT_BUTTON(XRCID("btnColorsDeleteTheme"), 	EditorConfigurationDlg::OnDeleteColorTheme)
	EVT_BUTTON(XRCID("btnColorsRenameTheme"), 	EditorConfigurationDlg::OnRenameColorTheme)
	EVT_CHECKBOX(XRCID("chkColorsBold"),		EditorConfigurationDlg::OnBoldItalicUline)
	EVT_CHECKBOX(XRCID("chkColorsItalics"),		EditorConfigurationDlg::OnBoldItalicUline)
	EVT_CHECKBOX(XRCID("chkColorsUnderlined"),	EditorConfigurationDlg::OnBoldItalicUline)
	EVT_BUTTON(XRCID("btnOK"), 					EditorConfigurationDlg::OnOK)
	EVT_LISTBOX(XRCID("lstComponents"),			EditorConfigurationDlg::OnColorComponent)
	EVT_COMBOBOX(XRCID("cmbLangs"),				EditorConfigurationDlg::OnChangeLang)
	EVT_COMBOBOX(XRCID("cmbDefCodeFileType"),	EditorConfigurationDlg::OnChangeDefCodeFileType)
	EVT_COMBOBOX(XRCID("cmbThemes"),	        EditorConfigurationDlg::OnColorTheme)
	EVT_LISTBOX(XRCID("lstAutoCompKeyword"),	EditorConfigurationDlg::OnAutoCompKeyword)
	EVT_BUTTON(XRCID("btnAutoCompAdd"),	        EditorConfigurationDlg::OnAutoCompAdd)
	EVT_BUTTON(XRCID("btnAutoCompDelete"),	    EditorConfigurationDlg::OnAutoCompDelete)
END_EVENT_TABLE()

EditorConfigurationDlg::EditorConfigurationDlg(wxWindow* parent)
	: m_TextColorControl(0L),
	m_AutoCompTextControl(0L),
	m_Theme(0L),
	m_Lang(wxSTC_LEX_CPP),
	m_DefCodeFileType(0),
	m_ThemeModified(false),
	m_LastAutoCompKeyword(-1)
{
	wxXmlResource::Get()->LoadDialog(this, parent, _("dlgConfigureEditor"));

	XRCCTRL(*this, "lblEditorFont", wxStaticText)->SetLabel(_("This is sample text"));
	UpdateSampleFont(false);

   	XRCCTRL(*this, "chkAutoIndent", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/editor/auto_indent", true));
   	XRCCTRL(*this, "chkSmartIndent", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/editor/smart_indent", true));
   	XRCCTRL(*this, "chkUseTab", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/editor/use_tab", 0l));
   	XRCCTRL(*this, "chkShowIndentGuides", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/editor/show_indent_guides", 0l));
   	XRCCTRL(*this, "chkTabIndents", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/editor/tab_indents", 1));
   	XRCCTRL(*this, "chkBackspaceUnindents", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/editor/backspace_unindents", 1));
   	XRCCTRL(*this, "chkWordWrap", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/editor/word_wrap", 0l));
   	XRCCTRL(*this, "chkShowEOL", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/editor/show_eol", 0l));
   	XRCCTRL(*this, "chkShowLineNumbers", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/editor/show_line_numbers", 0l));
   	XRCCTRL(*this, "chkHighlightCaretLine", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/editor/highlight_caret_line", 1));
   	XRCCTRL(*this, "spnTabSize", wxSpinCtrl)->SetValue(ConfigManager::Get()->Read("/editor/tab_size", 4));
   	XRCCTRL(*this, "cmbViewWS", wxComboBox)->SetSelection(ConfigManager::Get()->Read("/editor/view_whitespace", 0l));
   	XRCCTRL(*this, "rbTabText", wxRadioBox)->SetSelection(ConfigManager::Get()->Read("/editor/tab_text_relative", 1));
	//folding
   	XRCCTRL(*this, "chkEnableFolding", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/editor/folding/show_folds", 1));
   	XRCCTRL(*this, "chkFoldOnOpen", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/editor/folding/fold_all_on_open", 0L));
   	XRCCTRL(*this, "chkFoldPreprocessor", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/editor/folding/fold_preprocessor", 0L));
   	XRCCTRL(*this, "chkFoldComments", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/editor/folding/fold_comments", 1));
	//gutter
    wxColour color(ConfigManager::Get()->Read("/editor/gutter/color/red", 0l),
    				ConfigManager::Get()->Read("/editor/gutter/color/green", 0l),
    				ConfigManager::Get()->Read("/editor/gutter/color/blue", 0l)
    );
    XRCCTRL(*this, "lstGutterMode", wxChoice)->SetSelection(ConfigManager::Get()->Read("/editor/gutter/mode", 1));
    XRCCTRL(*this, "btnGutterColor", wxButton)->SetBackgroundColour(color);
    XRCCTRL(*this, "spnGutterColumn", wxSpinCtrl)->SetValue(ConfigManager::Get()->Read("/editor/gutter/column", 80));

	// color set
	LoadThemes();

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
    key.Printf("/editor/default_code/%d", IdxToFileType[m_DefCodeFileType]);
    XRCCTRL(*this, "txtDefCode", wxTextCtrl)->SetValue(ConfigManager::Get()->Read(key, wxEmptyString));
    wxFont tmpFont(8, wxMODERN, wxNORMAL, wxNORMAL);
    XRCCTRL(*this, "txtDefCode", wxTextCtrl)->SetFont(tmpFont);
}

EditorConfigurationDlg::~EditorConfigurationDlg()
{
	if (m_Theme)
		delete m_Theme;
		
	if (m_TextColorControl)
		delete m_TextColorControl;
    
    if (m_AutoCompTextControl)
        delete m_AutoCompTextControl;
}

void EditorConfigurationDlg::CreateColorsSample()
{
	if (m_TextColorControl)
		delete m_TextColorControl;
	m_TextColorControl = new wxStyledTextCtrl(this, wxID_ANY);
	m_TextColorControl->SetTabWidth(4);
	wxString buffer;
	if (m_Lang == wxSTC_LEX_CPP)
	{
		buffer << "/*" << '\n';
		buffer << " * Sample preview code" << '\n';
		buffer << " * This is a block comment" << '\n';
		buffer << " */" << '\n';
		buffer << "" << '\n';
		buffer << "#include <iostream> // this is a line comment" << '\n';
		buffer << "#include <cstdio>" << '\n';
		buffer << "" << '\n';
		buffer << "/**" << '\n';
		buffer << "  * This is a documentation comment block" << '\n';
		buffer << "  * @param xxx does this (this is the documentation keyword)" << '\n';
		buffer << "  * @author some user (this is the documentation keyword error)" << '\n';
		buffer << "  */" << '\n';
		buffer << "" << '\n';
		buffer << "int main(int argc, char **argv)" << '\n';
		buffer << "{" << '\n';
		buffer << '\t' << "/// This is a documentation comment line" << '\n';
		buffer << '\t' << "int numbers[20];" << '\n';
		buffer << '\t' << "int average = 0;" << '\n';
		buffer << '\t' << "char ch = '\\n';" << '\n';
		buffer << '\t' << "for (int i = 0; i < 20; ++i) // a breakpoint is set" << '\n';
		buffer << '\t' << "{" << '\n';
		buffer << '\t' << '\t' << "numbers[i] = i; // active line (during debugging)" << '\n';
		buffer << '\t' << '\t' << "total += i; // error line" << '\n';
		buffer << '\t' << "}" << '\n';
		buffer << '\t' << "average = total / 20;" << '\n';
		buffer << '\t' << "std::cout << numbers[0] << '\\n' << numbers[19] << '\\n';" << '\n';
		buffer << '\t' << "std::cout << \"total:\" << total << \"average:\" << average << '\\n';" << '\n';
		buffer << '\t' << "std::cout << \"Press any key...\" << '\\n';" << '\n';
		buffer << '\t' << "getch();" << '\n';
		buffer << "}" << '\n';
	}
	else if (m_Lang == wxSTC_LEX_LUA)
	{
		buffer << "-- LUA sample script" << '\n';
		buffer << "-- comments start with --" << '\n';
		buffer << "--[[ and this is a multi-line" << '\n';
		buffer << "     comment]]" << '\n';
		buffer << "" << '\n';
		buffer << "print(\"Hello world\")" << '\n';
		buffer << "" << '\n';
		buffer << "function SomeFunction(a, b)" << '\n';
		buffer << '\t' << "local x = a" << '\n';
		buffer << '\t' << "x = x + 1" << '\n';
		buffer << '\t' << "print(a,b,'x='..x)" << '\n';
		buffer << "end" << '\n';
		buffer << "" << '\n';
		buffer << "SomeFunction(1, 2)" << '\n';
		buffer << "var = {x = 0, y = 0)" << '\n';
		buffer << "print(var, var.x, var.y)" << '\n';
	}
	m_TextColorControl->SetText(buffer);
	m_TextColorControl->SetReadOnly(true);
	m_TextColorControl->SetCaretWidth(0);
    m_TextColorControl->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    m_TextColorControl->SetMarginWidth(0, 32);
	ApplyColors();

    m_TextColorControl->SetMarginWidth(1, 0);
	m_TextColorControl->MarkerAdd(20, 2); // breakpoint line
	m_TextColorControl->MarkerAdd(22, 3); // active line
	m_TextColorControl->MarkerAdd(23, 4); // error line

	FillColorComponents();
    wxXmlResource::Get()->AttachUnknownControl("txtColorsSample", m_TextColorControl);
}

void EditorConfigurationDlg::CreateAutoCompText()
{
	if (m_AutoCompTextControl)
		delete m_AutoCompTextControl;
	m_AutoCompTextControl = new wxStyledTextCtrl(this, wxID_ANY);
	m_AutoCompTextControl->SetTabWidth(4);
    m_AutoCompTextControl->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    m_AutoCompTextControl->SetMarginWidth(0, 32);
    m_AutoCompTextControl->SetViewWhiteSpace(1);
	ApplyColors();
    wxXmlResource::Get()->AttachUnknownControl("txtAutoCompCode", m_AutoCompTextControl);
}

void EditorConfigurationDlg::FillColorComponents()
{
	wxListBox* colors = XRCCTRL(*this, "lstComponents", wxListBox);
	colors->Clear();
	for (int i = 0; i < m_Theme->GetOptionCount(m_Lang); ++i)
	{
		OptionColor* opt = m_Theme->GetOptionByIndex(m_Lang, i);
		if (colors->FindString(opt->name) == -1)
            colors->Append(opt->name);
	}
	colors->SetSelection(0);
	ReadColors();
}

void EditorConfigurationDlg::ApplyColors()
{
	if (m_TextColorControl && m_Theme)
	{
		wxFont fnt = XRCCTRL(*this, "lblEditorFont", wxStaticText)->GetFont();
		if (m_TextColorControl)
		{
            m_TextColorControl->StyleSetFont(wxSTC_STYLE_DEFAULT,fnt); 
            m_Theme->Apply(m_Lang, m_TextColorControl);
        }
		if (m_AutoCompTextControl)
		{
            m_AutoCompTextControl->StyleSetFont(wxSTC_STYLE_DEFAULT,fnt); 
            m_Theme->Apply(wxSTC_LEX_CPP, m_AutoCompTextControl);
        }
	}
}

void EditorConfigurationDlg::ReadColors()
{
	if (m_Theme)
	{
		wxListBox* colors = XRCCTRL(*this, "lstComponents", wxListBox);
/* TODO (mandrav#1#): FIXME!!! */
		OptionColor* opt = m_Theme->GetOptionByName(m_Lang, colors->GetStringSelection());
		if (opt)
		{
			wxColour c = opt->fore;
			if (c == wxNullColour)
			{
                XRCCTRL(*this, "btnColorsFore", wxButton)->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
                XRCCTRL(*this, "btnColorsFore", wxButton)->SetLabel(_("\"Default\""));
            }
            else
            {
                XRCCTRL(*this, "btnColorsFore", wxButton)->SetBackgroundColour(c);
                XRCCTRL(*this, "btnColorsFore", wxButton)->SetLabel("");
            }

			c = opt->back;
			if (c == wxNullColour)
			{
                XRCCTRL(*this, "btnColorsBack", wxButton)->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
                XRCCTRL(*this, "btnColorsBack", wxButton)->SetLabel(_("\"Default\""));
            }
            else
            {
                XRCCTRL(*this, "btnColorsBack", wxButton)->SetBackgroundColour(c);
                XRCCTRL(*this, "btnColorsBack", wxButton)->SetLabel("");
            }
			
			XRCCTRL(*this, "chkColorsBold", wxCheckBox)->SetValue(opt->bold);
			XRCCTRL(*this, "chkColorsItalics", wxCheckBox)->SetValue(opt->italics);
			XRCCTRL(*this, "chkColorsUnderlined", wxCheckBox)->SetValue(opt->underlined);

			XRCCTRL(*this, "btnColorsFore", wxButton)->Enable(opt->isStyle);
			XRCCTRL(*this, "chkColorsBold", wxCheckBox)->Enable(opt->isStyle);
			XRCCTRL(*this, "chkColorsItalics", wxCheckBox)->Enable(opt->isStyle);
			XRCCTRL(*this, "chkColorsUnderlined", wxCheckBox)->Enable(opt->isStyle);
		}
	}
}

void EditorConfigurationDlg::WriteColors()
{
	if (m_Theme)
	{
		wxListBox* colors = XRCCTRL(*this, "lstComponents", wxListBox);
/* TODO (mandrav#1#): FIXME!!! */
		OptionColor* opt = m_Theme->GetOptionByName(m_Lang, colors->GetStringSelection());
		if (opt)
		{
            wxColour c = XRCCTRL(*this, "btnColorsFore", wxButton)->GetBackgroundColour();
            if (c != wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE))
                opt->fore = c;
			c = XRCCTRL(*this, "btnColorsBack", wxButton)->GetBackgroundColour();
            if (c != wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE))
                opt->back = c;
			opt->bold = XRCCTRL(*this, "chkColorsBold", wxCheckBox)->GetValue();
			opt->italics = XRCCTRL(*this, "chkColorsItalics", wxCheckBox)->GetValue();
			opt->underlined = XRCCTRL(*this, "chkColorsUnderlined", wxCheckBox)->GetValue();
			m_Theme->UpdateOptionsWithSameName(m_Lang, opt);
		}
	}
	ApplyColors();
	m_ThemeModified = true;
}

void EditorConfigurationDlg::UpdateSampleFont(bool askForNewFont)
{
    wxFont tmpFont(8, wxMODERN, wxNORMAL, wxNORMAL);
    wxString fontstring = ConfigManager::Get()->Read("/editor/font", wxEmptyString);

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

	wxFontDialog dlg(this, &data);
    if (dlg.ShowModal() == wxID_OK)
    {
    	wxFont font = dlg.GetFontData().GetChosenFont();
		XRCCTRL(*this, "lblEditorFont", wxStaticText)->SetFont(font);
		ApplyColors();
    }
}

void EditorConfigurationDlg::LoadThemes()
{
	wxComboBox* cmbThemes = XRCCTRL(*this, "cmbThemes", wxComboBox);
	cmbThemes->Clear();
	wxString group;
	long cookie;
	wxConfigBase* conf = ConfigManager::Get();
	wxString oldPath = conf->GetPath();
	conf->SetPath("/editor/color_sets");
	bool cont = conf->GetFirstGroup(group, cookie);
	while (cont)
	{
        cmbThemes->Append(group);
        cont = conf->GetNextGroup(group, cookie);
    }
	conf->SetPath(oldPath);
    if (cmbThemes->GetCount() == 0)
        cmbThemes->Append(COLORSET_DEFAULT);
    group = ConfigManager::Get()->Read("/editor/color_sets/active_color_set", COLORSET_DEFAULT);
    cookie = cmbThemes->FindString(group);
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
        msg.Printf(_("The color theme \"%s\" is modified.\nDo you want to save the changes?"), m_Theme->GetName().c_str());
        int ret = wxMessageBox(msg, _("Save"), wxYES_NO | wxCANCEL);
        switch (ret)
        {
            case wxYES: m_Theme->Save(); break;
            case wxCANCEL:
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
    XRCCTRL(*this, "btnColorsRenameTheme", wxButton)->Enable(key != COLORSET_DEFAULT);
    XRCCTRL(*this, "btnColorsDeleteTheme", wxButton)->Enable(key != COLORSET_DEFAULT);
    
    if (m_Theme)
        delete m_Theme;
    m_Theme = new EditorColorSet(key);

   	XRCCTRL(*this, "btnKeywords", wxButton)->Enable(m_Theme);

	wxComboBox* cmbLangs = XRCCTRL(*this, "cmbLangs", wxComboBox);
    cmbLangs->Clear();
    wxArrayString langs = m_Theme->GetAllHighlightLanguages();
    for (unsigned int i = 0; i < langs.GetCount(); ++i)
    {
    	cmbLangs->Append(langs[i]);
    }
    cmbLangs->SetSelection(0);
    cmbLangs->Enable(langs.GetCount() != 0);
	if (m_Theme)
	{
		wxString sel = cmbLangs->GetStringSelection();
		m_Lang = m_Theme->GetHighlightLanguage(sel);
	}

	CreateColorsSample();
	m_ThemeModified = false;
}

// events

void EditorConfigurationDlg::OnColorTheme(wxCommandEvent& event)
{
    // theme has changed
    wxComboBox* cmbThemes = XRCCTRL(*this, "cmbThemes", wxComboBox);
    if (m_Theme && m_Theme->GetName() != cmbThemes->GetStringSelection())
    {
        if (AskToSaveTheme())
            ChangeTheme();
    }
}

void EditorConfigurationDlg::OnAddColorTheme(wxCommandEvent& event)
{
    wxTextEntryDialog dlg(this, _("Please enter the name of the new color theme:"), _("New theme name"));
    if (dlg.ShowModal() != wxID_OK)
        return;

    wxString name = dlg.GetValue();
    wxComboBox* cmbThemes = XRCCTRL(*this, "cmbThemes", wxComboBox);
    cmbThemes->Append(name);
    cmbThemes->SetSelection(cmbThemes->GetCount() - 1);
    ChangeTheme();
}

void EditorConfigurationDlg::OnDeleteColorTheme(wxCommandEvent& event)
{
    if (wxMessageBox(_("Are you sure you want to delete this theme?"), _("Confirmation"), wxYES_NO) == wxYES)
    {
        ConfigManager::Get()->DeleteGroup("/editor/color_sets/" + m_Theme->GetName());
        wxComboBox* cmbThemes = XRCCTRL(*this, "cmbThemes", wxComboBox);
        int idx = cmbThemes->FindString(m_Theme->GetName());
        if (idx != wxNOT_FOUND)
            cmbThemes->Delete(idx);
        cmbThemes->SetSelection(wxNOT_FOUND);
        ChangeTheme();
    }
}

void EditorConfigurationDlg::OnRenameColorTheme(wxCommandEvent& event)
{
#ifdef __WXMSW__
    wxTextEntryDialog dlg(this, _("Please enter the new name of the new color theme:"), _("New theme name"), m_Theme->GetName());
    if (dlg.ShowModal() != wxID_OK)
        return;

    wxString name = dlg.GetValue();
    wxComboBox* cmbThemes = XRCCTRL(*this, "cmbThemes", wxComboBox);
    int idx = cmbThemes->FindString(m_Theme->GetName());
    if (idx != wxNOT_FOUND)
        cmbThemes->SetString(idx, name);
    ConfigManager::Get()->RenameGroup("/editor/color_sets/" + m_Theme->GetName(), "/editor/color_sets/" + name);
    m_Theme->SetName(name);
#else
	#warning "wxComboBox::SetString() doesn't work under non-win32 platforms"
#endif
}

void EditorConfigurationDlg::OnEditKeywords(wxCommandEvent& event)
{
	if (m_Theme && m_Lang != HL_NONE)
	{
		wxString keyw = wxGetTextFromUser(_("Edit keywords:"),
										m_Theme->GetLanguageName(m_Lang),
										m_Theme->GetKeywords(m_Lang));
		if (!keyw.IsEmpty())
			m_Theme->SetKeywords(m_Lang, keyw);
	}
}

void EditorConfigurationDlg::OnColorsReset(wxCommandEvent& event)
{
    if (wxMessageBox(_("Are you sure you want to reset all colors to defaults?"),
                    _("Confirmation"),
                    wxICON_QUESTION | wxYES_NO) == wxYES)
    {
        m_Theme->Reset(m_Lang);
        ApplyColors();
        m_ThemeModified = true;
    }
}

void EditorConfigurationDlg::OnChangeLang(wxCommandEvent& event)
{
	if (m_Theme)
	{
		wxString sel = XRCCTRL(*this, "cmbLangs", wxComboBox)->GetStringSelection();
		m_Lang = m_Theme->GetHighlightLanguage(sel);
	}
	FillColorComponents();
	CreateColorsSample();
}

void EditorConfigurationDlg::OnChangeDefCodeFileType(wxCommandEvent& event)
{
    wxString key;
	int sel = XRCCTRL(*this, "cmbDefCodeFileType", wxComboBox)->GetSelection();
	if (sel != m_DefCodeFileType)
	{
        key.Printf("/editor/default_code/%d", IdxToFileType[m_DefCodeFileType]);
        ConfigManager::Get()->Write(key, XRCCTRL(*this, "txtDefCode", wxTextCtrl)->GetValue());
	}
	m_DefCodeFileType = sel;
    key.Printf("/editor/default_code/%d", IdxToFileType[m_DefCodeFileType]);
    XRCCTRL(*this, "txtDefCode", wxTextCtrl)->SetValue(ConfigManager::Get()->Read(key, wxEmptyString));
}

void EditorConfigurationDlg::OnChooseColor(wxCommandEvent& event)
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
	
	if (event.GetId() == XRCID("btnColorsFore") ||
		event.GetId() == XRCID("btnColorsBack"))
		WriteColors();
}

void EditorConfigurationDlg::OnChooseFont(wxCommandEvent& event)
{
	UpdateSampleFont(true);
}

void EditorConfigurationDlg::OnColorComponent(wxCommandEvent& event)
{
	ReadColors();
}

void EditorConfigurationDlg::OnBoldItalicUline(wxCommandEvent& event)
{
	WriteColors();
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
            wxMessageBox(_("This keyword already exists!"), _("Error"), wxICON_ERROR);
            return;
        }
        m_AutoCompMap[key] = _("");
        wxListBox* lstKeyword = XRCCTRL(*this, "lstAutoCompKeyword", wxListBox);
        lstKeyword->Append(key);
        AutoCompUpdate(lstKeyword->GetSelection());
        m_AutoCompTextControl->SetText(_(""));
        m_LastAutoCompKeyword = lstKeyword->GetCount() - 1;
        lstKeyword->SetSelection(m_LastAutoCompKeyword);
    }
}

void EditorConfigurationDlg::OnAutoCompDelete(wxCommandEvent& event)
{
    wxListBox* lstKeyword = XRCCTRL(*this, "lstAutoCompKeyword", wxListBox);
    if (lstKeyword->GetSelection() == -1)
        return;
    
    if (wxMessageBox(_("Are you sure you want to delete this keyword?"), _("Confirmation"), wxICON_QUESTION | wxYES_NO) == wxNO)
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
            m_AutoCompTextControl->SetText(_(""));
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

void EditorConfigurationDlg::OnOK(wxCommandEvent& event)
{
    ConfigManager::Get()->Write("/editor/font", XRCCTRL(*this, "lblEditorFont", wxStaticText)->GetFont().GetNativeFontInfoDesc());

    ConfigManager::Get()->Write("/editor/auto_indent",			XRCCTRL(*this, "chkAutoIndent", wxCheckBox)->GetValue());
    ConfigManager::Get()->Write("/editor/smart_indent",			XRCCTRL(*this, "chkSmartIndent", wxCheckBox)->GetValue());
    ConfigManager::Get()->Write("/editor/use_tab", 				XRCCTRL(*this, "chkUseTab", wxCheckBox)->GetValue());
    ConfigManager::Get()->Write("/editor/show_indent_guides", 	XRCCTRL(*this, "chkShowIndentGuides", wxCheckBox)->GetValue());
   	ConfigManager::Get()->Write("/editor/tab_indents", 			XRCCTRL(*this, "chkTabIndents", wxCheckBox)->GetValue());
   	ConfigManager::Get()->Write("/editor/backspace_unindents", 	XRCCTRL(*this, "chkBackspaceUnindents", wxCheckBox)->GetValue());
   	ConfigManager::Get()->Write("/editor/word_wrap", 			XRCCTRL(*this, "chkWordWrap", wxCheckBox)->GetValue());
   	ConfigManager::Get()->Write("/editor/show_eol", 			XRCCTRL(*this, "chkShowEOL", wxCheckBox)->GetValue());
   	ConfigManager::Get()->Write("/editor/show_line_numbers", 	XRCCTRL(*this, "chkShowLineNumbers", wxCheckBox)->GetValue());
   	ConfigManager::Get()->Write("/editor/highlight_caret_line", XRCCTRL(*this, "chkHighlightCaretLine", wxCheckBox)->GetValue());
   	ConfigManager::Get()->Write("/editor/tab_size",             XRCCTRL(*this, "spnTabSize", wxSpinCtrl)->GetValue());
   	ConfigManager::Get()->Write("/editor/view_whitespace",      XRCCTRL(*this, "cmbViewWS", wxComboBox)->GetSelection());
   	ConfigManager::Get()->Write("/editor/tab_text_relative",    XRCCTRL(*this, "rbTabText", wxRadioBox)->GetSelection());
	//folding
   	ConfigManager::Get()->Write("/editor/folding/show_folds", 			XRCCTRL(*this, "chkEnableFolding", wxCheckBox)->GetValue());
   	ConfigManager::Get()->Write("/editor/folding/fold_all_on_open", 	XRCCTRL(*this, "chkFoldOnOpen", wxCheckBox)->GetValue());
   	ConfigManager::Get()->Write("/editor/folding/fold_preprocessor", 	XRCCTRL(*this, "chkFoldPreprocessor", wxCheckBox)->GetValue());
   	ConfigManager::Get()->Write("/editor/folding/fold_comments", 		XRCCTRL(*this, "chkFoldComments", wxCheckBox)->GetValue());
	//gutter
    ConfigManager::Get()->Write("/editor/gutter/mode", 			XRCCTRL(*this, "lstGutterMode", wxChoice)->GetSelection());
    ConfigManager::Get()->Write("/editor/gutter/color/red",		XRCCTRL(*this, "btnGutterColor", wxButton)->GetBackgroundColour().Red());
    ConfigManager::Get()->Write("/editor/gutter/color/green",	XRCCTRL(*this, "btnGutterColor", wxButton)->GetBackgroundColour().Green());
    ConfigManager::Get()->Write("/editor/gutter/color/blue",	XRCCTRL(*this, "btnGutterColor", wxButton)->GetBackgroundColour().Blue());
    ConfigManager::Get()->Write("/editor/gutter/column", 		XRCCTRL(*this, "spnGutterColumn", wxSpinCtrl)->GetValue());

	int sel = XRCCTRL(*this, "cmbDefCodeFileType", wxComboBox)->GetSelection();
    wxString key;
    key.Printf("/editor/default_code/%d", IdxToFileType[sel]);
    ConfigManager::Get()->Write(key, XRCCTRL(*this, "txtDefCode", wxTextCtrl)->GetValue());

	if (m_Theme)
	{
		m_Theme->Save();
		Manager::Get()->GetEditorManager()->SetColorSet(m_Theme);
        ConfigManager::Get()->Write("/editor/color_sets/active_color_set", m_Theme->GetName());
	}

    // save any changes in auto-completion
    wxListBox* lstKeyword = XRCCTRL(*this, "lstAutoCompKeyword", wxListBox);
    AutoCompUpdate(lstKeyword->GetSelection());
    AutoCompleteMap& map = Manager::Get()->GetEditorManager()->GetAutoCompleteMap();
    map = m_AutoCompMap;

    EndModal(wxID_OK);
}

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
	EVT_BUTTON(XRCID("btnGutterColor"), 		EditorConfigurationDlg::OnChooseColor)
	EVT_BUTTON(XRCID("btnColorsFore"), 			EditorConfigurationDlg::OnChooseColor)
	EVT_BUTTON(XRCID("btnColorsBack"), 			EditorConfigurationDlg::OnChooseColor)
	EVT_CHECKBOX(XRCID("chkColorsBold"),		EditorConfigurationDlg::OnBoldItalicUline)
	EVT_CHECKBOX(XRCID("chkColorsItalics"),		EditorConfigurationDlg::OnBoldItalicUline)
	EVT_CHECKBOX(XRCID("chkColorsUnderlined"),	EditorConfigurationDlg::OnBoldItalicUline)
	EVT_BUTTON(XRCID("btnOK"), 					EditorConfigurationDlg::OnOK)
	EVT_LISTBOX(XRCID("lstComponents"),			EditorConfigurationDlg::OnColorComponent)
	EVT_COMBOBOX(XRCID("cmbLangs"),				EditorConfigurationDlg::OnChangeLang)
	EVT_COMBOBOX(XRCID("cmbDefCodeFileType"),	EditorConfigurationDlg::OnChangeDefCodeFileType)
END_EVENT_TABLE()

EditorConfigurationDlg::EditorConfigurationDlg(wxWindow* parent)
	: m_TextColorControl(0L),
	m_Theme(0L),
	m_Lang(hlCpp),
	m_DefCodeFileType(0)
{
	wxXmlResource::Get()->LoadDialog(this, parent, _("dlgConfigureEditor"));

	XRCCTRL(*this, "lblEditorFont", wxStaticText)->SetLabel(_("This is sample text"));
	UpdateSampleFont(false);

   	XRCCTRL(*this, "chkAutoIndent", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/editor/auto_indent", true));
   	XRCCTRL(*this, "chkUseTab", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/editor/use_tab", 0l));
   	XRCCTRL(*this, "chkShowIndentGuides", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/editor/show_indent_guides", 0l));
   	XRCCTRL(*this, "chkTabIndents", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/editor/tab_indents", 1));
   	XRCCTRL(*this, "chkBackspaceUnindents", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/editor/backspace_unindents", 1));
   	XRCCTRL(*this, "chkWordWrap", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/editor/word_wrap", 0l));
   	XRCCTRL(*this, "chkShowEOL", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/editor/show_eol", 0l));
   	XRCCTRL(*this, "chkShowLineNumbers", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/editor/show_line_numbers", 0l));
   	XRCCTRL(*this, "chkHighlightCaretLine", wxCheckBox)->SetValue(ConfigManager::Get()->Read("/editor/highlight_caret_line", 1));
   	XRCCTRL(*this, "spnTabSize", wxSpinCtrl)->SetValue(ConfigManager::Get()->Read("/editor/tab_size", 4));
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
	m_Theme = new EditorColorSet(ConfigManager::Get()->Read("/editor/color_sets", COLORSET_DEFAULT));

	//keywords
   	XRCCTRL(*this, "btnKeywords", wxButton)->Enable(m_Theme);
   	XRCCTRL(*this, "cmbLangs", wxComboBox)->Enable(m_Theme);
	if (m_Theme)
		m_Lang = HighlightLanguage(XRCCTRL(*this, "cmbLangs", wxComboBox)->GetSelection() + 1); // +1 because 0 would mean hlNone
	CreateColorsSample();
	
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
}

void EditorConfigurationDlg::CreateColorsSample()
{
	if (m_TextColorControl)
		delete m_TextColorControl;
	m_TextColorControl = new wxStyledTextCtrl(this, wxID_ANY);
	m_TextColorControl->SetTabWidth(2);
	wxString buffer;
	if (m_Lang == hlCpp)
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
	else if (m_Lang == hlLua)
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
	ApplyColors();

    m_TextColorControl->SetMarginWidth(1, 0);
	m_TextColorControl->MarkerAdd(20, 2); // breakpoint line
	m_TextColorControl->MarkerAdd(22, 3); // active line
	m_TextColorControl->MarkerAdd(23, 4); // error line

	FillColorComponents();
    wxXmlResource::Get()->AttachUnknownControl("txtColorsSample", m_TextColorControl);
}

void EditorConfigurationDlg::FillColorComponents()
{
	wxListBox* colors = XRCCTRL(*this, "lstComponents", wxListBox);
	colors->Clear();
/* TODO (mandrav#1#): FIXME!!! */
	for (int i = 0; i < m_Theme->GetOptionCount(m_Lang); ++i)
	{
		OptionColor* opt = m_Theme->GetOptionByIndex(m_Lang, i);
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
		m_TextColorControl->StyleSetFont(wxSTC_STYLE_DEFAULT,fnt); 
		m_Theme->Apply(m_Lang, m_TextColorControl);
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
				c = *wxBLACK;
			XRCCTRL(*this, "btnColorsFore", wxButton)->SetBackgroundColour(c);
			c = opt->back;
			if (c == wxNullColour)
				c = *wxWHITE;
			XRCCTRL(*this, "btnColorsBack", wxButton)->SetBackgroundColour(c);
			
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
			opt->fore = XRCCTRL(*this, "btnColorsFore", wxButton)->GetBackgroundColour();
			opt->back = XRCCTRL(*this, "btnColorsBack", wxButton)->GetBackgroundColour();
			opt->bold = XRCCTRL(*this, "chkColorsBold", wxCheckBox)->GetValue();
			opt->italics = XRCCTRL(*this, "chkColorsItalics", wxCheckBox)->GetValue();
			opt->underlined = XRCCTRL(*this, "chkColorsUnderlined", wxCheckBox)->GetValue();
		}
	}
	ApplyColors();
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

// events

void EditorConfigurationDlg::OnEditKeywords(wxCommandEvent& event)
{
	if (m_Theme && m_Lang != hlNone)
	{
		wxString keyw = wxGetTextFromUser(_("Edit keywords:"),
										m_Theme->GetLanguageName(m_Lang),
										m_Theme->GetKeywords(m_Lang));
		if (!keyw.IsEmpty())
			m_Theme->SetKeywords(m_Lang, keyw);
	}
}

void EditorConfigurationDlg::OnChangeLang(wxCommandEvent& event)
{
	m_Lang = HighlightLanguage(XRCCTRL(*this, "cmbLangs", wxComboBox)->GetSelection() + 1); // +1 because 0 would mean hlNone
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

void EditorConfigurationDlg::OnOK(wxCommandEvent& event)
{
    ConfigManager::Get()->Write("/editor/font", XRCCTRL(*this, "lblEditorFont", wxStaticText)->GetFont().GetNativeFontInfoDesc());

    ConfigManager::Get()->Write("/editor/auto_indent",			XRCCTRL(*this, "chkAutoIndent", wxCheckBox)->GetValue());
    ConfigManager::Get()->Write("/editor/use_tab", 				XRCCTRL(*this, "chkUseTab", wxCheckBox)->GetValue());
    ConfigManager::Get()->Write("/editor/show_indent_guides", 	XRCCTRL(*this, "chkShowIndentGuides", wxCheckBox)->GetValue());
   	ConfigManager::Get()->Write("/editor/tab_indents", 			XRCCTRL(*this, "chkTabIndents", wxCheckBox)->GetValue());
   	ConfigManager::Get()->Write("/editor/backspace_unindents", 	XRCCTRL(*this, "chkBackspaceUnindents", wxCheckBox)->GetValue());
   	ConfigManager::Get()->Write("/editor/word_wrap", 			XRCCTRL(*this, "chkWordWrap", wxCheckBox)->GetValue());
   	ConfigManager::Get()->Write("/editor/show_eol", 			XRCCTRL(*this, "chkShowEOL", wxCheckBox)->GetValue());
   	ConfigManager::Get()->Write("/editor/show_line_numbers", 	XRCCTRL(*this, "chkShowLineNumbers", wxCheckBox)->GetValue());
   	ConfigManager::Get()->Write("/editor/highlight_caret_line", XRCCTRL(*this, "chkHighlightCaretLine", wxCheckBox)->GetValue());
   	ConfigManager::Get()->Write("/editor/tab_size",             XRCCTRL(*this, "spnTabSize", wxSpinCtrl)->GetValue());
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
	}

    EndModal(wxID_OK);
}

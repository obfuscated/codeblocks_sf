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
#include <wx/log.h>
#include <wx/dir.h>

#include "globals.h"
#include "cbeditor.h"
#include "editorcolorset.h"
#include "editorlexerloader.h"
#include "configmanager.h"
#include "messagemanager.h"
#include "manager.h"

#define cbHIGHLIGHT_LINE -98 // highlight line under caret virtual style
#define cbSELECTION      -99 // selection virtual style

EditorColorSet::EditorColorSet(const wxString& setName)
	: m_LanguageID(0),
	m_Name(setName)
{
	LoadAvailableSets();

	if (setName.IsEmpty())
		m_Name = COLORSET_DEFAULT;
	else
		Load();
}

EditorColorSet::EditorColorSet(const EditorColorSet& other) // copy ctor
{
	m_Name = other.m_Name;
	
	for (int x = 0; x < HL_LAST; ++x)
	{
		m_Langs[x] = other.m_Langs[x];
		m_Lexers[x] = other.m_Lexers[x];
		m_Keywords[x] = other.m_Keywords[x];
		m_FileMasks[x] = other.m_FileMasks[x];
		m_SampleCode[x] = other.m_SampleCode[x];
		m_BreakLine[x] = other.m_BreakLine[x];
		m_DebugLine[x] = other.m_DebugLine[x];
		m_ErrorLine[x] = other.m_ErrorLine[x];
		const OptionColors& value = other.m_Colors[x];
		for (unsigned int i = 0; i < value.GetCount(); ++i)
		{
			AddOption(x, value[i]);
        }
	}
}

EditorColorSet::~EditorColorSet()
{
	ClearAllOptionColors();
}

void EditorColorSet::ClearAllOptionColors()
{
	for (int i = 0; i < HL_LAST; ++i)
	{
        WX_CLEAR_ARRAY(m_Colors[i]);
		m_Colors[i].Clear();
		m_Langs[i].Clear();
		m_Lexers[i] = wxSTC_LEX_NULL;
		m_Keywords[i].Clear();
		m_FileMasks[i].Clear();
		m_SampleCode[i].Clear();
		m_BreakLine[i] = -1;
		m_DebugLine[i] = -1;
		m_ErrorLine[i] = -1;
	}
}

void EditorColorSet::LoadAvailableSets()
{
    m_LanguageID = 0;
	wxString path = ConfigManager::Get()->Read("data_path") + "/lexers";
    wxDir dir(path);

    if (!dir.IsOpened())
        return;

	EditorLexerLoader lex(this);
    wxString filename;
    bool ok = dir.GetFirst(&filename, "lexer_*.xml", wxDIR_FILES);
    while (ok)
    {
        lex.Load(path + "/" + filename);
        ok = dir.GetNext(&filename);
    }
}

HighlightLanguage EditorColorSet::AddHighlightLanguage(int lexer, const wxString& name)
{
	if (m_LanguageID == HL_LAST ||
        lexer <= wxSTC_LEX_NULL ||
        lexer > wxSTC_LEX_MMIXAL ||
        name.IsEmpty() ||
        GetHighlightLanguage(name) != HL_NONE)
    {
        return HL_NONE;
    }
    m_Langs[m_LanguageID] = name;
    m_Lexers[m_LanguageID] = lexer;
    ++m_LanguageID;
    return m_LanguageID - 1;
}

HighlightLanguage EditorColorSet::GetHighlightLanguage(const wxString& name)
{
	for (int i = 0; i < HL_LAST; ++i)
	{
		if (m_Langs[i].Matches(name))
            return i;
	}
	return HL_NONE;
}

wxArrayString EditorColorSet::GetAllHighlightLanguages()
{
	wxArrayString ret;
	for (int i = 0; i < HL_LAST; ++i)
	{
		if (!m_Langs[i].IsEmpty())
            ret.Add(m_Langs[i]);
	}
	return ret;
}

void EditorColorSet::UpdateOptionsWithSameName(HighlightLanguage lang, OptionColor* base)
{
    if (!base)
        return;
    // first find the index of this option
    int idx = -1;
	for (unsigned int i = 0; i < m_Colors[lang].GetCount(); ++i)
	{
		OptionColor* opt = m_Colors[lang].Item(i);
		if (opt == base)
		{
			idx = i;
			break;
		}
	}
	if (idx == -1)
        return;
    
    // now loop again, but update the other options with the same name
	for (unsigned int i = 0; i < m_Colors[lang].GetCount(); ++i)
	{
		if ((int)i == idx)
            continue; // skip the base option
		OptionColor* opt = m_Colors[lang].Item(i);
		if (!opt->name.Matches(base->name))
            continue;
        opt->fore = base->fore;
        opt->back = base->back;
        opt->bold = base->bold;
        opt->italics = base->italics;
        opt->underlined = base->underlined;
	}
}

bool EditorColorSet::AddOption(HighlightLanguage lang, OptionColor* option, bool checkIfExists)
{
	if (checkIfExists && GetOptionByValue(lang, option->value))
        return false;

	OptionColors& colors =  m_Colors[lang];
	colors.Add(new OptionColor(*option));
	return true;
}

void EditorColorSet::AddOption(HighlightLanguage lang,
								const wxString& name,
								int value,
								wxColour fore,
								wxColour back,
								bool bold,
								bool italics,
								bool underlined,
								bool isStyle)
{
	OptionColor* opt = new OptionColor;
	opt->name = name;
	opt->value = value;
	opt->fore = fore;
	opt->back = back;
	opt->bold = bold;
	opt->italics = italics;
	opt->underlined = underlined;
	opt->isStyle = isStyle;

	if (!AddOption(lang, opt))
        delete opt;
}

OptionColor* EditorColorSet::GetOptionByName(HighlightLanguage lang, const wxString& name)
{
	for (unsigned int i = 0; i < m_Colors[lang].GetCount(); ++i)
	{
		OptionColor* opt = m_Colors[lang].Item(i);
		if (opt->name == name)
			return opt;
	}
	return 0L;
}

OptionColor* EditorColorSet::GetOptionByValue(HighlightLanguage lang, int value)
{
	for (unsigned int i = 0; i < m_Colors[lang].GetCount(); ++i)
	{
		OptionColor* opt = m_Colors[lang].Item(i);
		if (opt->value == value)
			return opt;
	}
	return 0L;
}

OptionColor* EditorColorSet::GetOptionByIndex(HighlightLanguage lang, int index)
{
	return m_Colors[lang].Item(index);
}

HighlightLanguage EditorColorSet::GetLanguageForFilename(const wxString& filename)
{
	// first search in filemasks
	for (int i = 0; i < HL_LAST; ++i)
	{
		for (unsigned int x = 0; x < m_FileMasks[i].GetCount(); ++x)
		{
			if (filename.Matches(m_FileMasks[i].Item(x)))
                return i;
		}
	}
	
	// if nothing found, check a couple of common-types
//	FileType ft = FileTypeOf(filename);
//	switch (ft)
//	{
//		case ftResource: // resource files are highlighted like C/C++ files (rfe #1184765)
//		case ftSource:
//		case ftHeader: return wxSTC_LEX_CPP;
//		
//		case ftLua: return wxSTC_LEX_LUA;
//		
//		default: break;
//	}
    return HL_NONE;
}

wxString EditorColorSet::GetLanguageName(HighlightLanguage lang)
{
	wxString name = m_Langs[lang];
    if (!name.IsEmpty())
        return name;
    return _("Unknown");
}

void EditorColorSet::DoApplyStyle(wxStyledTextCtrl* control, int value, OptionColor* option)
{
	// option->value is ignored here...
	// value is used instead
	if (option->fore != wxNullColour)
        control->StyleSetForeground(value, option->fore);
	if (option->back != wxNullColour)
        control->StyleSetBackground(value, option->back);
	control->StyleSetBold(value, option->bold);
	control->StyleSetItalic(value, option->italics);
	control->StyleSetUnderline(value, option->underlined);
}

void EditorColorSet::Apply(cbEditor* editor)
{
	if (!editor)
		return;
	HighlightLanguage lang = GetLanguageForFilename(editor->GetFilename());
	if (lang == HL_NONE)
		return;
	Apply(lang, editor->GetControl());
}


void EditorColorSet::Apply(HighlightLanguage lang, wxStyledTextCtrl* control)
{
	control->StyleClearAll();

    // first load the default colors to all styles (ignoring some built-in styles)
    OptionColor* defaults = GetOptionByName(lang, _("Default"));
    if (defaults)
    {
        for (int i = 0; i < wxSTC_STYLE_MAX; ++i)
        {
            if (i < 32 || i > 39)
                DoApplyStyle(control, i, defaults);
        }
        // also set the caret color, same as the default foreground
        control->SetCaretForeground(defaults->fore);
    }
	// for some strange reason, when switching styles, the line numbering changes color
	// too, though we didn't ask it to...
	// this makes sure it stays the correct color
    control->StyleSetForeground(wxSTC_STYLE_LINENUMBER, wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));

	for (unsigned int i = 0; i < m_Colors[lang].GetCount(); ++i)
	{
		OptionColor* opt = m_Colors[lang].Item(i);
		
		if (opt->isStyle)
		{
			DoApplyStyle(control, opt->value, opt);
		}
		else
		{
            if (opt->value == cbHIGHLIGHT_LINE)
            {
                control->SetCaretLineBack(opt->back);
                ConfigManager::Get()->Write("/editor/highlight_caret_line_color/red",	  opt->back.Red());
                ConfigManager::Get()->Write("/editor/highlight_caret_line_color/green", opt->back.Green());
                ConfigManager::Get()->Write("/editor/highlight_caret_line_color/blue",  opt->back.Blue());
            }
            else if (opt->value == cbSELECTION)
            {
                if (opt->back != wxNullColour)
                {
                    control->SetSelBackground(true, opt->back);
                    ConfigManager::Get()->Write("/editor/selection_color/red",	 opt->back.Red());
                    ConfigManager::Get()->Write("/editor/selection_color/green", opt->back.Green());
                    ConfigManager::Get()->Write("/editor/selection_color/blue",  opt->back.Blue());
                }
                else
                    control->SetSelBackground(false, wxColour(0xC0, 0xC0, 0xC0));
            }
            else
                control->MarkerDefine(-opt->value, 1, wxNullColour, opt->back);
		}
	}
	control->SetLexer(m_Lexers[lang]);
    control->SetKeyWords(0, m_Keywords[lang]);
    control->Colourise(0, -1); // the *most* important part!
}

void EditorColorSet::Save()
{
	wxString key;
	
	for (int x = 0; x < HL_LAST; ++x)
	{
		wxString lang = m_Langs[x];
		lang.Replace("/", "_");
		lang.Replace("\\", "_");
		if (lang.IsEmpty())
            continue;
        key.Clear();
		key << "/editor/color_sets/" << m_Name << "/" << lang;
		for (unsigned int i = 0; i < m_Colors[x].GetCount(); ++i)
		{
			OptionColor* opt = m_Colors[x].Item(i);
			wxString tmpKey;
			tmpKey << key << "/" << opt->name;
			
			if (opt->fore != wxNullColour)
			{
                ConfigManager::Get()->Write(tmpKey + "/fore/red", opt->fore.Red());
                ConfigManager::Get()->Write(tmpKey + "/fore/green", opt->fore.Green());
                ConfigManager::Get()->Write(tmpKey + "/fore/blue", opt->fore.Blue());
			}
			if (opt->back != wxNullColour)
			{
                ConfigManager::Get()->Write(tmpKey + "/back/red", opt->back.Red());
                ConfigManager::Get()->Write(tmpKey + "/back/green", opt->back.Green());
                ConfigManager::Get()->Write(tmpKey + "/back/blue", opt->back.Blue());
            }
	
			ConfigManager::Get()->Write(tmpKey + "/bold", opt->bold);
			ConfigManager::Get()->Write(tmpKey + "/italics", opt->italics);
			ConfigManager::Get()->Write(tmpKey + "/underlined", opt->underlined);
	
			ConfigManager::Get()->Write(tmpKey + "/isStyle", opt->isStyle);
		}
		ConfigManager::Get()->Write(key + "/editor/keywords", m_Keywords[x]);
	}
}

void EditorColorSet::Load()
{
	wxString key;
	
	for (int x = 0; x < HL_LAST; ++x)
	{
		wxString lang = m_Langs[x];
		lang.Replace("/", "_");
		lang.Replace("\\", "_");
		if (lang.IsEmpty())
            continue;
		key.Clear();
		key << "/editor/color_sets/" << m_Name << "/" << lang;
		if (!ConfigManager::Get()->HasGroup(key))
			continue;
		for (unsigned int i = 0; i < m_Colors[x].GetCount(); ++i)
		{
			OptionColor* opt = m_Colors[x].Item(i);
			wxString tmpKey;
			tmpKey << key << "/" << opt->name;
			
			if (ConfigManager::Get()->HasGroup(tmpKey + "/fore"))
			{
                opt->fore = wxColour(ConfigManager::Get()->Read(tmpKey + "/fore/red", opt->fore.Red()),
                                    ConfigManager::Get()->Read(tmpKey + "/fore/green", opt->fore.Green()),
                                    ConfigManager::Get()->Read(tmpKey + "/fore/blue", opt->fore.Blue())
                                    );
            }
			if (ConfigManager::Get()->HasGroup(tmpKey + "/back"))
			{
                opt->back = wxColour(ConfigManager::Get()->Read(tmpKey+ "/back/red", opt->back.Red()),
                                    ConfigManager::Get()->Read(tmpKey + "/back/green", opt->back.Green()),
                                    ConfigManager::Get()->Read(tmpKey + "/back/blue", opt->back.Blue())
                                    );
            }
			opt->bold = ConfigManager::Get()->Read(tmpKey + "/bold", opt->bold);
			opt->italics = ConfigManager::Get()->Read(tmpKey + "/italics", opt->italics);
			opt->underlined = ConfigManager::Get()->Read(tmpKey + "/underlined", opt->underlined);
	
			opt->isStyle = ConfigManager::Get()->Read(tmpKey + "/isStyle", opt->underlined);
		}
		m_Keywords[x] = ConfigManager::Get()->Read(key + "/editor/keywords", "");
	}
}

void EditorColorSet::Reset(HighlightLanguage lang)
{
    wxLogNull ln;
    wxString key;
    key << "/editor/color_sets/" << m_Name << "/" << GetLanguageName(lang);
    if (ConfigManager::Get()->HasGroup(key))
        ConfigManager::Get()->DeleteGroup(key);

    ClearAllOptionColors();
    LoadAvailableSets();
}

wxString& EditorColorSet::GetKeywords(HighlightLanguage lang)
{
    return m_Keywords[lang];
}

void EditorColorSet::SetKeywords(HighlightLanguage lang, const wxString& keywords)
{
    if (lang != HL_NONE) m_Keywords[lang] = keywords;
}

const wxArrayString& EditorColorSet::GetFileMasks(HighlightLanguage lang)
{
	return m_FileMasks[lang];
}

void EditorColorSet::SetFileMasks(HighlightLanguage lang, const wxString& masks, const wxString& separator)
{
	m_FileMasks[lang] = GetArrayFromString(masks.Lower(), separator);
}

wxString EditorColorSet::GetSampleCode(HighlightLanguage lang, int* breakLine, int* debugLine, int* errorLine)
{
    if (breakLine)
        *breakLine = m_BreakLine[lang];
    if (debugLine)
        *debugLine = m_DebugLine[lang];
    if (errorLine)
        *errorLine = m_ErrorLine[lang];
	wxString path = ConfigManager::Get()->Read("data_path") + "/lexers/";
    if (!m_SampleCode[lang].IsEmpty())
        return path + m_SampleCode[lang];
    return wxEmptyString;
}

void EditorColorSet::SetSampleCode(HighlightLanguage lang, const wxString& sample, int breakLine, int debugLine, int errorLine)
{
    m_SampleCode[lang] = sample;
    m_BreakLine[lang] = breakLine;
    m_DebugLine[lang] = debugLine;
    m_ErrorLine[lang] = errorLine;
}

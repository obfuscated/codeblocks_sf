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
		m_Sets[x].m_Langs = other.m_Sets[x].m_Langs;
		m_Sets[x].m_Lexers = other.m_Sets[x].m_Lexers;
		m_Sets[x].m_Keywords[0] = other.m_Sets[x].m_Keywords[0];
		m_Sets[x].m_Keywords[1] = other.m_Sets[x].m_Keywords[1];
		m_Sets[x].m_Keywords[2] = other.m_Sets[x].m_Keywords[2];
		m_Sets[x].m_FileMasks = other.m_Sets[x].m_FileMasks;
		m_Sets[x].m_SampleCode = other.m_Sets[x].m_SampleCode;
		m_Sets[x].m_BreakLine = other.m_Sets[x].m_BreakLine;
		m_Sets[x].m_DebugLine = other.m_Sets[x].m_DebugLine;
		m_Sets[x].m_ErrorLine = other.m_Sets[x].m_ErrorLine;
		const OptionColors& value = other.m_Sets[x].m_Colors;
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
        WX_CLEAR_ARRAY(m_Sets[i].m_Colors);
		m_Sets[i].m_Colors.Clear();
		m_Sets[i].m_Langs.Clear();
		m_Sets[i].m_Lexers = wxSTC_LEX_NULL;
		m_Sets[i].m_Keywords[0].Clear();
		m_Sets[i].m_Keywords[1].Clear();
		m_Sets[i].m_Keywords[2].Clear();
		m_Sets[i].m_FileMasks.Clear();
		m_Sets[i].m_SampleCode.Clear();
		m_Sets[i].m_BreakLine = -1;
		m_Sets[i].m_DebugLine = -1;
		m_Sets[i].m_ErrorLine = -1;
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
    m_Sets[m_LanguageID].m_Langs = name;
    m_Sets[m_LanguageID].m_Lexers = lexer;
    ++m_LanguageID;
    return m_LanguageID - 1;
}

HighlightLanguage EditorColorSet::GetHighlightLanguage(const wxString& name)
{
	for (int i = 0; i < HL_LAST; ++i)
	{
		if (m_Sets[i].m_Langs.Matches(name))
            return i;
	}
	return HL_NONE;
}

wxArrayString EditorColorSet::GetAllHighlightLanguages()
{
	wxArrayString ret;
	for (int i = 0; i < HL_LAST; ++i)
	{
		if (!m_Sets[i].m_Langs.IsEmpty())
            ret.Add(m_Sets[i].m_Langs);
	}
	return ret;
}

void EditorColorSet::UpdateOptionsWithSameName(HighlightLanguage lang, OptionColor* base)
{
    if (!base)
        return;
    // first find the index of this option
    int idx = -1;
	for (unsigned int i = 0; i < m_Sets[lang].m_Colors.GetCount(); ++i)
	{
		OptionColor* opt = m_Sets[lang].m_Colors.Item(i);
		if (opt == base)
		{
			idx = i;
			break;
		}
	}
	if (idx == -1)
        return;
    
    // now loop again, but update the other options with the same name
	for (unsigned int i = 0; i < m_Sets[lang].m_Colors.GetCount(); ++i)
	{
		if ((int)i == idx)
            continue; // skip the base option
		OptionColor* opt = m_Sets[lang].m_Colors.Item(i);
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

	OptionColors& colors =  m_Sets[lang].m_Colors;
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
	for (unsigned int i = 0; i < m_Sets[lang].m_Colors.GetCount(); ++i)
	{
		OptionColor* opt = m_Sets[lang].m_Colors.Item(i);
		if (opt->name == name)
			return opt;
	}
	return 0L;
}

OptionColor* EditorColorSet::GetOptionByValue(HighlightLanguage lang, int value)
{
	for (unsigned int i = 0; i < m_Sets[lang].m_Colors.GetCount(); ++i)
	{
		OptionColor* opt = m_Sets[lang].m_Colors.Item(i);
		if (opt->value == value)
			return opt;
	}
	return 0L;
}

OptionColor* EditorColorSet::GetOptionByIndex(HighlightLanguage lang, int index)
{
	return m_Sets[lang].m_Colors.Item(index);
}

int EditorColorSet::GetOptionCount(HighlightLanguage lang)
{
    return m_Sets[lang].m_Colors.GetCount();
}

HighlightLanguage EditorColorSet::GetLanguageForFilename(const wxString& filename)
{
	// first search in filemasks
	for (int i = 0; i < HL_LAST; ++i)
	{
		for (unsigned int x = 0; x < m_Sets[i].m_FileMasks.GetCount(); ++x)
		{
			if (filename.Matches(m_Sets[i].m_FileMasks.Item(x)))
                return i;
		}
	}
    return HL_NONE;
}

wxString EditorColorSet::GetLanguageName(HighlightLanguage lang)
{
	wxString name = m_Sets[lang].m_Langs;
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

	for (unsigned int i = 0; i < m_Sets[lang].m_Colors.GetCount(); ++i)
	{
		OptionColor* opt = m_Sets[lang].m_Colors.Item(i);
		
		if (opt->isStyle)
		{
			DoApplyStyle(control, opt->value, opt);
		}
		else
		{
            if (opt->value == cbHIGHLIGHT_LINE)
            {
                control->SetCaretLineBack(opt->back);
                ConfigManager::Get()->Write("/editor/highlight_caret_line_color/red",	opt->back.Red());
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
	control->SetLexer(m_Sets[lang].m_Lexers);
	for (int i = 0; i < 3; ++i)
        control->SetKeyWords(i, m_Sets[lang].m_Keywords[i]);
    control->Colourise(0, -1); // the *most* important part!
}

void EditorColorSet::Save()
{
	wxString key;
	
	for (int x = 0; x < HL_LAST; ++x)
	{
		wxString lang = m_Sets[x].m_Langs;
		lang.Replace("/", "_");
		lang.Replace("\\", "_");
		if (lang.IsEmpty())
            continue;
        key.Clear();
		key << "/editor/color_sets/" << m_Name << "/" << lang;
		for (unsigned int i = 0; i < m_Sets[x].m_Colors.GetCount(); ++i)
		{
			OptionColor* opt = m_Sets[x].m_Colors.Item(i);
			wxString tmpKey;
			tmpKey << key << "/" << opt->name;
			
			if (opt->fore != wxNullColour)
			{
                ConfigManager::Get()->Write(tmpKey + "/fore/red",   opt->fore.Red());
                ConfigManager::Get()->Write(tmpKey + "/fore/green", opt->fore.Green());
                ConfigManager::Get()->Write(tmpKey + "/fore/blue",  opt->fore.Blue());
			}
			if (opt->back != wxNullColour)
			{
                ConfigManager::Get()->Write(tmpKey + "/back/red",   opt->back.Red());
                ConfigManager::Get()->Write(tmpKey + "/back/green", opt->back.Green());
                ConfigManager::Get()->Write(tmpKey + "/back/blue",  opt->back.Blue());
            }
	
			ConfigManager::Get()->Write(tmpKey + "/bold",       opt->bold);
			ConfigManager::Get()->Write(tmpKey + "/italics",    opt->italics);
			ConfigManager::Get()->Write(tmpKey + "/underlined", opt->underlined);
	
			ConfigManager::Get()->Write(tmpKey + "/isStyle", opt->isStyle);
		}
		for (int i = 0; i < 3; ++i)
		{
			wxString tmpkey;
			tmpkey.Printf("%s/editor/keywords/%d", key.c_str(), i);
            ConfigManager::Get()->Write(tmpkey, m_Sets[x].m_Keywords[i]);
		}
	}
}

void EditorColorSet::Load()
{
	wxString key;
	
	for (int x = 0; x < HL_LAST; ++x)
	{
		wxString lang = m_Sets[x].m_Langs;
		lang.Replace("/", "_");
		lang.Replace("\\", "_");
		if (lang.IsEmpty())
            continue;
		key.Clear();
		key << "/editor/color_sets/" << m_Name << "/" << lang;
		if (!ConfigManager::Get()->HasGroup(key))
			continue;
		for (unsigned int i = 0; i < m_Sets[x].m_Colors.GetCount(); ++i)
		{
			OptionColor* opt = m_Sets[x].m_Colors.Item(i);
			wxString tmpKey;
			tmpKey << key << "/" << opt->name;
			
			if (ConfigManager::Get()->HasGroup(tmpKey + "/fore"))
			{
                opt->fore = wxColour(ConfigManager::Get()->Read(tmpKey + "/fore/red",  opt->fore.Red()),
                                    ConfigManager::Get()->Read(tmpKey + "/fore/green", opt->fore.Green()),
                                    ConfigManager::Get()->Read(tmpKey + "/fore/blue",  opt->fore.Blue())
                                    );
            }
			if (ConfigManager::Get()->HasGroup(tmpKey + "/back"))
			{
                opt->back = wxColour(ConfigManager::Get()->Read(tmpKey+ "/back/red",   opt->back.Red()),
                                    ConfigManager::Get()->Read(tmpKey + "/back/green", opt->back.Green()),
                                    ConfigManager::Get()->Read(tmpKey + "/back/blue",  opt->back.Blue())
                                    );
            }
			opt->bold = ConfigManager::Get()->Read(tmpKey + "/bold", opt->bold);
			opt->italics = ConfigManager::Get()->Read(tmpKey + "/italics", opt->italics);
			opt->underlined = ConfigManager::Get()->Read(tmpKey + "/underlined", opt->underlined);
	
			opt->isStyle = ConfigManager::Get()->Read(tmpKey + "/isStyle", opt->underlined);
		}
        for (int i = 0; i < 3; ++i)
        {
            wxString tmpkey;
            tmpkey.Printf("%s/editor/keywords/%d", key.c_str(), i);
            m_Sets[x].m_Keywords[i] = ConfigManager::Get()->Read(tmpkey, "");
        }
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

wxString& EditorColorSet::GetKeywords(HighlightLanguage lang, int idx)
{
    return m_Sets[lang].m_Keywords[idx];
}

void EditorColorSet::SetKeywords(HighlightLanguage lang, int idx, const wxString& keywords)
{
    if (lang != HL_NONE && idx >=0 && idx < 3)
    {
        m_Sets[lang].m_Keywords[idx] = keywords;
        m_Sets[lang].m_Keywords[idx].Replace("\r", " ");
        m_Sets[lang].m_Keywords[idx].Replace("\n", " ");
        m_Sets[lang].m_Keywords[idx].Replace("\t", " ");
        while (m_Sets[lang].m_Keywords[idx].Replace("  ", " "))
            ;
    }
}

const wxArrayString& EditorColorSet::GetFileMasks(HighlightLanguage lang)
{
	return m_Sets[lang].m_FileMasks;
}

void EditorColorSet::SetFileMasks(HighlightLanguage lang, const wxString& masks, const wxString& separator)
{
	m_Sets[lang].m_FileMasks = GetArrayFromString(masks.Lower(), separator);
}

wxString EditorColorSet::GetSampleCode(HighlightLanguage lang, int* breakLine, int* debugLine, int* errorLine)
{
    if (breakLine)
        *breakLine = m_Sets[lang].m_BreakLine;
    if (debugLine)
        *debugLine = m_Sets[lang].m_DebugLine;
    if (errorLine)
        *errorLine = m_Sets[lang].m_ErrorLine;
	wxString path = ConfigManager::Get()->Read("data_path") + "/lexers/";
    if (!m_Sets[lang].m_SampleCode.IsEmpty())
        return path + m_Sets[lang].m_SampleCode;
    return wxEmptyString;
}

void EditorColorSet::SetSampleCode(HighlightLanguage lang, const wxString& sample, int breakLine, int debugLine, int errorLine)
{
    m_Sets[lang].m_SampleCode = sample;
    m_Sets[lang].m_BreakLine = breakLine;
    m_Sets[lang].m_DebugLine = debugLine;
    m_Sets[lang].m_ErrorLine = errorLine;
}

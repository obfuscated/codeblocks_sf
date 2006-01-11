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

#include "sdk_precomp.h"
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
		m_Sets[i].m_Lexers = wxSCI_LEX_NULL;
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
	wxString path = ConfigManager::GetDataFolder() + _T("/lexers");
    wxDir dir(path);

    if (!dir.IsOpened())
        return;

	EditorLexerLoader lex(this);
    wxString filename;
    bool ok = dir.GetFirst(&filename, _T("lexer_*.xml"), wxDIR_FILES);
    while (ok)
    {
        lex.Load(path + _T("/") + filename);
        ok = dir.GetNext(&filename);
    }

    // remove old settings, no longer used
	for (int x = 0; x < HL_LAST; ++x)
	{
		wxString lang = m_Sets[x].m_Langs;
		if (lang.IsEmpty())
            continue;
		unsigned int i = 0;
		while (i < m_Sets[x].m_Colors.GetCount())
		{
			OptionColor* opt = m_Sets[x].m_Colors.Item(i);
			// valid values are:
			if (opt->value < 0 &&               // styles >= 0
                opt->value != cbSELECTION &&    // cbSELECTION
                opt->value != cbHIGHLIGHT_LINE) // cbHIGHLIGHT_LINE
            {
                m_Sets[x].m_Colors.Remove(opt);
                delete opt;
            }
            else
                ++i;
		}
	}
}

HighlightLanguage EditorColorSet::AddHighlightLanguage(int lexer, const wxString& name)
{
	if (m_LanguageID == HL_LAST ||
        lexer <= wxSCI_LEX_NULL ||
        lexer > wxSCI_LEX_FREEBASIC ||
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
		if (m_Sets[i].m_Langs.CmpNoCase(name) == 0)
            return i;
	}
	return HL_NONE;
}

// from scintilla lexer (wxSCI_LEX_*)
// Warning: the first one found is returned!
HighlightLanguage EditorColorSet::GetHighlightLanguage(int lexer)
{
	for (int i = 0; i < HL_LAST; ++i)
	{
		if (m_Sets[i].m_Lexers == lexer)
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
    if (lang == HL_NONE)
        return false;
	if (checkIfExists && GetOptionByValue(lang, option->value))
        return false;

    option->originalfore = option->fore;
    option->originalback = option->back;

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
    if (lang == HL_NONE)
        return;
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
    if (lang == HL_NONE)
        return 0L;
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
    if (lang == HL_NONE)
        return 0L;
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
    if (lang == HL_NONE)
        return 0L;
	return m_Sets[lang].m_Colors.Item(index);
}

int EditorColorSet::GetOptionCount(HighlightLanguage lang)
{
    return m_Sets[lang].m_Colors.GetCount();
}

HighlightLanguage EditorColorSet::GetLanguageForFilename(const wxString& filename)
{
    // convert filename to lowercase first (m_FileMasks already contains
    // lowercase-only strings)
    wxString lfname = filename.Lower();

	// first search in filemasks
	for (int i = 0; i < HL_LAST; ++i)
	{
		for (unsigned int x = 0; x < m_Sets[i].m_FileMasks.GetCount(); ++x)
		{
			if (lfname.Matches(m_Sets[i].m_FileMasks.Item(x)))
                return i;
		}
	}
    return HL_NONE;
}

wxString EditorColorSet::GetLanguageName(HighlightLanguage lang)
{
    if (lang == HL_NONE)
        return _("Unknown");
	wxString name = m_Sets[lang].m_Langs;
    if (!name.IsEmpty())
        return name;
    return _("Unknown");
}

void EditorColorSet::DoApplyStyle(cbStyledTextCtrl* control, int value, OptionColor* option)
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


void EditorColorSet::Apply(HighlightLanguage lang, cbStyledTextCtrl* control)
{
	control->StyleClearAll();

    // first load the default colors to all styles (ignoring some built-in styles)
    OptionColor* defaults = GetOptionByName(lang, _("Default"));
    if (defaults)
    {
        for (int i = 0; i < wxSCI_STYLE_MAX; ++i)
        {
            if (i < 33 || i > 39)
                DoApplyStyle(control, i, defaults);
        }
        // also set the caret color, same as the default foreground
        control->SetCaretForeground(defaults->fore);
    }
	// for some strange reason, when switching styles, the line numbering changes color
	// too, though we didn't ask it to...
	// this makes sure it stays the correct color
    control->StyleSetForeground(wxSCI_STYLE_LINENUMBER, wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));

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
                control->SetCaretLineBackground(opt->back);
                Manager::Get()->GetConfigManager(_T("editor"))->Write(_T("/highlight_caret_line_color"), opt->back);
            }
            else if (opt->value == cbSELECTION)
            {
                if (opt->back != wxNullColour)
                {
                    control->SetSelBackground(true, opt->back);
                    Manager::Get()->GetConfigManager(_T("editor"))->Write(_T("/selection_color"), opt->back);
                }
                else
                    control->SetSelBackground(false, wxColour(0xC0, 0xC0, 0xC0));
            }
//            else
//            {
//                control->MarkerDefine(-opt->value, 1);
//                control->MarkerSetBackground(-opt->value, opt->back);
//            }
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
	ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));
	cfg->DeleteSubPath(_T("/color_sets"));

	for (int x = 0; x < HL_LAST; ++x)
	{
		wxString lang = m_Sets[x].m_Langs;
		lang.Replace(_T("/"), _T("_"));
		lang.Replace(_T("\\"), _T("_"));
		if (lang.IsEmpty())
            continue;
        key.Clear();
		key << _T("/color_sets/") << m_Name << _T("/set") << wxString::Format(_T("%d"), x);
        cfg->Write(key + _T("/name"), lang);
		for (unsigned int i = 0; i < m_Sets[x].m_Colors.GetCount(); ++i)
		{
			OptionColor* opt = m_Sets[x].m_Colors.Item(i);
			wxString tmpKey;
			tmpKey << key << _T("/style") << wxString::Format(_T("%d"), i);

			if (opt->fore != opt->originalfore && opt->fore != wxNullColour)
                cfg->Write(tmpKey + _T("/fore"), opt->fore);
			if (opt->back != opt->originalback && opt->back != wxNullColour)
                cfg->Write(tmpKey + _T("/back"), opt->back);

            cfg->Write(tmpKey + _T("/bold"),       opt->bold);
            cfg->Write(tmpKey + _T("/italics"),    opt->italics);
            cfg->Write(tmpKey + _T("/underlined"), opt->underlined);
            cfg->Write(tmpKey + _T("/isStyle"),    opt->isStyle);

            cfg->Write(tmpKey + _T("/name"), opt->name, true);
		}
        wxString tmpkey;
		for (int i = 0; i < 3; ++i)
		{
		    if (!m_Sets[x].m_Keywords[i].IsEmpty())
		    {
                tmpkey.Printf(_T("%s/editor/keywords/set%d"), key.c_str(), i);
                cfg->Write(tmpkey, m_Sets[x].m_Keywords[i]);
		    }
		}
        tmpkey.Printf(_T("%s/editor/filemasks"), key.c_str());
        wxString tmparr = GetStringFromArray(m_Sets[x].m_FileMasks, _T(","));
        if (!tmparr.IsEmpty())
            cfg->Write(tmpkey, tmparr);
	}
}

void EditorColorSet::Load()
{
	wxString key;
	ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));

	for (int x = 0; x < HL_LAST; ++x)
	{
		wxString lang = m_Sets[x].m_Langs;
		lang.Replace(_T("/"), _T("_"));
		lang.Replace(_T("\\"), _T("_"));
		if (lang.IsEmpty())
            continue;
		key.Clear();
		key << _T("/color_sets/") << m_Name << _T("/set") << wxString::Format(_T("%d"), x);
		if (cfg->EnumerateSubPaths(key).GetCount() == 0)
			continue;
		for (unsigned int i = 0; i < m_Sets[x].m_Colors.GetCount(); ++i)
		{
			OptionColor* opt = m_Sets[x].m_Colors.Item(i);
			if (!opt)
                continue;
			wxString tmpKey;
			tmpKey << key << _T("/style") << wxString::Format(_T("%d"), i);

			if (cfg->Exists(tmpKey + _T("/name")))
                opt->name = cfg->Read(tmpKey + _T("/name"));

			if (cfg->Exists(tmpKey + _T("/fore")))
                opt->fore = cfg->ReadColour(tmpKey + _T("/fore"), opt->fore);
			if (cfg->Exists(tmpKey + _T("/back")))
                opt->back = cfg->ReadColour(tmpKey + _T("/back"), opt->back);
			opt->bold = cfg->ReadBool(tmpKey + _T("/bold"), opt->bold);
			opt->italics = cfg->ReadBool(tmpKey + _T("/italics"), opt->italics);
			opt->underlined = cfg->ReadBool(tmpKey + _T("/underlined"), opt->underlined);

			opt->isStyle = cfg->ReadBool(tmpKey + _T("/isStyle"), opt->isStyle);
		}
        wxString tmpkey;
        for (int i = 0; i < 3; ++i)
        {
            tmpkey.Printf(_T("%s/editor/keywords/set%d"), key.c_str(), i);
            m_Sets[x].m_Keywords[i] = cfg->Read(tmpkey, wxEmptyString);
        }
        tmpkey.Printf(_T("%s/editor/filemasks"), key.c_str());
        if (cfg->Exists(tmpkey))
            m_Sets[x].m_FileMasks = GetArrayFromString(cfg->Read(tmpkey, wxEmptyString), _T(","));
	}
}

void EditorColorSet::Reset(HighlightLanguage lang)
{
    wxLogNull ln;
    wxString key;
    key << _T("/color_sets/") << m_Name << _T("/set") << wxString::Format(_T("%d"), lang);
    if (Manager::Get()->GetConfigManager(_T("editor"))->EnumerateSubPaths(key).GetCount() != 0)
        Manager::Get()->GetConfigManager(_T("editor"))->DeleteSubPath(key);

    ClearAllOptionColors();
    LoadAvailableSets();
    Load();
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
        m_Sets[lang].m_Keywords[idx].Replace(_T("\r"), _T(" "));
        m_Sets[lang].m_Keywords[idx].Replace(_T("\n"), _T(" "));
        m_Sets[lang].m_Keywords[idx].Replace(_T("\t"), _T(" "));
        while (m_Sets[lang].m_Keywords[idx].Replace(_T("  "), _T(" ")))
            ;
    }
}

const wxArrayString& EditorColorSet::GetFileMasks(HighlightLanguage lang)
{
	return m_Sets[lang].m_FileMasks;
}

void EditorColorSet::SetFileMasks(HighlightLanguage lang, const wxString& masks, const wxString& separator)
{
    if (lang != HL_NONE)
        m_Sets[lang].m_FileMasks = GetArrayFromString(masks.Lower(), separator);
}

wxString EditorColorSet::GetSampleCode(HighlightLanguage lang, int* breakLine, int* debugLine, int* errorLine)
{
	if (lang == HL_NONE)
        return wxEmptyString;
    if (breakLine)
        *breakLine = m_Sets[lang].m_BreakLine;
    if (debugLine)
        *debugLine = m_Sets[lang].m_DebugLine;
    if (errorLine)
        *errorLine = m_Sets[lang].m_ErrorLine;
	wxString path = ConfigManager::GetDataFolder() + _T("/lexers/");
    if (!m_Sets[lang].m_SampleCode.IsEmpty())
        return path + m_Sets[lang].m_SampleCode;
    return wxEmptyString;
}

void EditorColorSet::SetSampleCode(HighlightLanguage lang, const wxString& sample, int breakLine, int debugLine, int errorLine)
{
	if (lang == HL_NONE)
        return;
    m_Sets[lang].m_SampleCode = sample;
    m_Sets[lang].m_BreakLine = breakLine;
    m_Sets[lang].m_DebugLine = debugLine;
    m_Sets[lang].m_ErrorLine = errorLine;
}

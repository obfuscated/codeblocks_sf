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

#include "globals.h"
#include "cbeditor.h"
#include "editorcolorset.h"
#include "configmanager.h"
#include "messagemanager.h"
#include "manager.h"

#define cbHIGHLIGHT_LINE -99 // highlight line under caret virtual style

#define EDITOR_KEYWORDS_CPP "asm auto bool break case catch char class const " \
						"const_cast continue default delete do double " \
						"dynamic_cast else enum explicit export extern " \
						"false float for friend goto if inline int long " \
						"mutable namespace new operator private protected " \
						"public register reinterpret_cast return short signed " \
						"sizeof static static_cast struct switch template this " \
						"throw true try typedef typeid typename union unsigned " \
						"using virtual void volatile wchar_t while "

#define EDITOR_KEYWORDS_LUA "if elseif else then end do while nil true false " \
						"in for and or function local not repeat return until "

EditorColorSet::EditorColorSet(const wxString& setName)
	: m_Name(setName)
{
	m_Keywords[hlCpp] = EDITOR_KEYWORDS_CPP;
	m_Keywords[hlLua] = EDITOR_KEYWORDS_LUA;

	LoadBuiltInSet(hlCpp);
	LoadBuiltInSet(hlLua);

	if (setName.IsEmpty())
		m_Name = COLORSET_DEFAULT;
	else
		Load();
}

EditorColorSet::EditorColorSet(const EditorColorSet& other) // copy ctor
{
	m_Name = other.m_Name;
	
	m_Keywords.clear();
	for (KeywordsMap::const_iterator it = other.m_Keywords.begin(); it != other.m_Keywords.end(); ++it)
		m_Keywords[it->first] = it->second;
	
	m_Colors.clear();
	for (ColorsMap::const_iterator it = other.m_Colors.begin(); it != other.m_Colors.end(); ++it)
	{
		HighlightLanguage key = it->first;
		OptionColors value = it->second;
		for (unsigned int i = 0; i < value.GetCount(); ++i)
		{
			AddOption(key, value[i]);
        }
	}
}

EditorColorSet::~EditorColorSet()
{
	ClearAllOptionColors();
}

void EditorColorSet::ClearAllOptionColors()
{
	for (ColorsMap::iterator it = m_Colors.begin(); it != m_Colors.end(); ++it)
	{
        WX_CLEAR_ARRAY(it->second);
		it->second.Clear();
	}
	m_Colors.clear();
}

bool EditorColorSet::AddOption(HighlightLanguage lang, OptionColor* option, bool checkIfExists)
{
	if (checkIfExists && (GetOptionByName(lang, option->name) || GetOptionByValue(lang, option->value)))
		return false;
    if (m_Colors.find(lang) == m_Colors.end())
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
	FileType ft = FileTypeOf(filename);
	switch (ft)
	{
		case ftSource:
		case ftHeader: return hlCpp;
		
		case ftLua: return hlLua;
		
		default: return hlNone;
	}
}

wxString EditorColorSet::GetLanguageName(HighlightLanguage lang)
{
	switch (lang)
	{
		case hlCpp: return "C++";
		case hlLua: return "LUA";
		default: return _("Unknown");
	}
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
	if (lang == hlNone)
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
    }
	// also set the caret color, same as the default foreground
	control->SetCaretForeground(defaults->fore);
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
			/*if (opt->value == wxSTC_C_COMMENT)
			{
				// for all kinds of C/C++ comments, use the same style
				DoApplyStyle(control, wxSTC_C_COMMENTDOC, opt);
				DoApplyStyle(control, wxSTC_C_COMMENTLINE, opt);
				DoApplyStyle(control, wxSTC_C_COMMENTLINEDOC, opt);
				DoApplyStyle(control, wxSTC_C_COMMENTDOCKEYWORD, opt);
				DoApplyStyle(control, wxSTC_C_COMMENTDOCKEYWORDERROR, opt);
			}
			else */if (opt->value == wxSTC_LUA_COMMENT)
			{
				// for all kinds of LUA comments, use the same style
				DoApplyStyle(control, wxSTC_LUA_COMMENTDOC, opt);
			}
			else if (opt->value == wxSTC_LUA_STRING)
			{
				// for all kinds of LUA strings, use the same style
				DoApplyStyle(control, wxSTC_LUA_LITERALSTRING, opt);
				DoApplyStyle(control, wxSTC_LUA_CHARACTER, opt);
			}
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
            else
                control->MarkerDefine(-opt->value, 1, wxNullColour, opt->back);
		}
	}
	switch (lang)
	{
		case hlCpp: control->SetLexer(wxSTC_LEX_CPP); break;
		case hlLua: control->SetLexer(wxSTC_LEX_LUA); break;
		default: break; // stop compiler warning
	}
    control->SetKeyWords(0, m_Keywords[lang]);
    control->Colourise(0, -1); // the *most* important part!
}

void EditorColorSet::LoadBuiltInSet(HighlightLanguage lang)
{
    // common options to all languages
    AddOption(lang, _("Default"), wxSTC_STYLE_DEFAULT, wxColour(0x00, 0x00, 0x00), wxColour(0xFF, 0xFF, 0xFF));

	if (lang == hlCpp)
	{
		Manager::Get()->GetMessageManager()->DebugLog(_("Loading C/C++ highlighter"));
		AddOption(lang, _("Character"), 				            wxSTC_C_CHARACTER,				wxColour(0xE0, 0xA0, 0x00), wxNullColour);
		AddOption(lang, _("Comment (block)"), 					    wxSTC_C_COMMENT, 				wxColour(0xA0, 0xA0, 0xA0), wxNullColour);
		AddOption(lang, _("Comment (line)"), 					    wxSTC_C_COMMENTLINE,			wxColour(0xA0, 0xA0, 0xA0), wxNullColour);
		AddOption(lang, _("Documentation comment (block)"), 		wxSTC_C_COMMENTDOC,				wxColour(0xA0, 0xA0, 0xA0), wxNullColour);
		AddOption(lang, _("Documentation comment (line)"), 	        wxSTC_C_COMMENTLINEDOC, 		wxColour(0xA0, 0xA0, 0xA0), wxNullColour);
		AddOption(lang, _("Documentation comment keyword"), 	    wxSTC_C_COMMENTDOCKEYWORD, 		wxColour(0xA0, 0xA0, 0xA0), wxNullColour);
		AddOption(lang, _("Documentation comment keyword error"),   wxSTC_C_COMMENTDOCKEYWORDERROR,	wxColour(0xA0, 0xA0, 0xA0), wxNullColour);
		AddOption(lang, _("Keyword"),					            wxSTC_C_WORD,					wxColour(0x00, 0x00, 0xA0), wxNullColour, true);
		AddOption(lang, _("Number"), 					            wxSTC_C_NUMBER,					wxColour(0xF0, 0x00, 0xF0), wxNullColour);
		AddOption(lang, _("Operator"), 					            wxSTC_C_OPERATOR,				wxColour(0xFF, 0x00, 0x00), wxNullColour);
		AddOption(lang, _("Preprocessor"), 				            wxSTC_C_PREPROCESSOR,			wxColour(0x00, 0xA0, 0x00), wxNullColour);
		AddOption(lang, _("String"), 					            wxSTC_C_STRING,					wxColour(0x00, 0x00, 0xFF), wxNullColour);
		AddOption(lang, _("Breakpoint line"),			            -2,								wxNullColour, wxColour(0xFF, 0xA0, 0xA0), false, false, false, false);
		AddOption(lang, _("Debugger active line"),		            -3,								wxNullColour, wxColour(0xA0, 0xA0, 0xFF), false, false, false, false);
		AddOption(lang, _("Compiler error line"),				    -4,								wxNullColour, wxColour(0xFF, 0x80, 0x00), false, false, false, false);
	}
	else if (lang == hlLua)
	{
		Manager::Get()->GetMessageManager()->DebugLog(_("Loading LUA highlighter"));
		AddOption(lang, _("Comment (block)"), 			wxSTC_LUA_COMMENT, 				wxColour(0xA0, 0xA0, 0xA0), wxNullColour);
		AddOption(lang, _("Comment (line)"), 			wxSTC_LUA_COMMENTLINE,			wxColour(0xA0, 0xA0, 0xA0), wxNullColour);
		AddOption(lang, _("Keyword"),					wxSTC_LUA_WORD,					wxColour(0x00, 0x00, 0xA0), wxNullColour, true);
		AddOption(lang, _("Number"), 					wxSTC_LUA_NUMBER,				wxColour(0xF0, 0x00, 0xF0), wxNullColour);
		AddOption(lang, _("Operator"), 					wxSTC_LUA_OPERATOR,				wxColour(0xFF, 0x00, 0x00), wxNullColour);
		AddOption(lang, _("String"), 					wxSTC_LUA_STRING,				wxColour(0x00, 0x00, 0xFF), wxNullColour);
	}
	else
		Manager::Get()->GetMessageManager()->DebugLog(_("EditorColorSet::LoadBuiltInSet() : Unknown language..."));

    // common options to all languages
    AddOption(lang, _("Editor active line"), 	                cbHIGHLIGHT_LINE,               wxNullColour, wxColour(0xFF, 0xFF, 0xA0), false, false, false, false);
    AddOption(lang, _("Brace match highlight"), 	            wxSTC_STYLE_BRACELIGHT,			wxColour(0xFF, 0x00, 0x00), wxNullColour, true, false, true);
    AddOption(lang, _("Brace no match highlight"),	            wxSTC_STYLE_BRACEBAD,			wxColour(0xFF, 0x00, 0x00), wxNullColour, true);
}

void EditorColorSet::Save()
{
	wxString key;
	
	for (ColorsMap::iterator it = m_Colors.begin(); it != m_Colors.end(); ++it)
	{
		key << "/editor/color_sets/" << m_Name << "/" << GetLanguageName(it->first);
		for (unsigned int i = 0; i < it->second.GetCount(); ++i)
		{
			OptionColor* opt = it->second[i];
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
		ConfigManager::Get()->Write(key + "/editor/keywords", m_Keywords[it->first]);
	}
}

void EditorColorSet::Load()
{
	wxString key;
	
	for (ColorsMap::iterator it = m_Colors.begin(); it != m_Colors.end(); ++it)
	{
		key = "";
		key << "/editor/color_sets/" << m_Name << "/" << GetLanguageName(it->first);
		if (!ConfigManager::Get()->HasGroup(key))
		{
			LoadBuiltInSet(it->first);
			continue;
		}
		for (unsigned int i = 0; i < it->second.GetCount(); ++i)
		{
			OptionColor* opt = it->second[i];
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
		m_Keywords[it->first] = ConfigManager::Get()->Read(key + "/editor/keywords", EDITOR_KEYWORDS_CPP);
	}
}

void EditorColorSet::Reset(HighlightLanguage lang)
{
    WX_CLEAR_ARRAY(m_Colors[lang]);
    m_Colors[lang].Clear();

    wxLogNull ln;
    wxString key;
    key << "/editor/color_sets/" << m_Name << "/" << GetLanguageName(lang);
    if (ConfigManager::Get()->HasGroup(key))
        ConfigManager::Get()->DeleteGroup(key);

    LoadBuiltInSet(lang);
}

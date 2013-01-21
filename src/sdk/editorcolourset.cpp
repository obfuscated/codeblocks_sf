/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/dir.h>
    #include <wx/settings.h>

    #include "globals.h"
    #include "cbeditor.h"
    #include "configmanager.h"
    #include "logmanager.h"
    #include "filemanager.h"
    #include "manager.h"
#endif

#include <wx/regex.h>
#include <wx/txtstrm.h>  // wxTextInputStream
#include <wx/wfstream.h> // wxFileInputStream

#include "cbstyledtextctrl.h"

#include "editorcolourset.h"
#include "editorlexerloader.h"
#include "filefilters.h"

const int cbHIGHLIGHT_LINE = -98; // highlight line under caret virtual style
const int cbSELECTION      = -99; // selection virtual style

EditorColourSet::EditorColourSet(const wxString& setName)
    : m_Name(setName)
{
    LoadAvailableSets();

    if (setName.IsEmpty())
        m_Name = COLORSET_DEFAULT;
    else
        Load();
}

EditorColourSet::EditorColourSet(const EditorColourSet& other) // copy ctor
{
    m_Name = other.m_Name;
    m_Sets.clear();

    for (OptionSetsMap::const_iterator it = other.m_Sets.begin(); it != other.m_Sets.end(); ++it)
    {
        OptionSet& mset = m_Sets[it->first];

        mset.m_Langs = it->second.m_Langs;
        mset.m_Lexers = it->second.m_Lexers;
        for (int i = 0; i <= wxSCI_KEYWORDSET_MAX; ++i)
        {
            mset.m_Keywords[i] = it->second.m_Keywords[i];
            mset.m_originalKeywords[i] = it->second.m_originalKeywords[i];
        }
        mset.m_FileMasks = it->second.m_FileMasks;
        mset.m_originalFileMasks = it->second.m_originalFileMasks;
        mset.m_SampleCode = it->second.m_SampleCode;
        mset.m_BreakLine = it->second.m_BreakLine;
        mset.m_DebugLine = it->second.m_DebugLine;
        mset.m_ErrorLine = it->second.m_ErrorLine;
        mset.comment = it->second.comment;
        mset.m_CaseSensitive = it->second.m_CaseSensitive;
        const OptionColours& value = it->second.m_Colours;
        for (unsigned int i = 0; i < value.GetCount(); ++i)
        {
            AddOption(it->first, value[i]);
        }
    }
}

EditorColourSet::~EditorColourSet()
{
    ClearAllOptionColours();
}

void EditorColourSet::ClearAllOptionColours()
{
   for (OptionSetsMap::iterator map_it = m_Sets.begin();
                                                   map_it != m_Sets.end(); ++map_it)
    {
        for (OptionColours::iterator vec_it = (*map_it).second.m_Colours.begin();
                            vec_it != (*map_it).second.m_Colours.end(); ++vec_it)
        {
            delete (*vec_it);
        }
    }
    m_Sets.clear();
}

void EditorColourSet::LoadAvailableSets()
{
    // no need for syntax highlighting if batch building
    if (Manager::IsBatchBuild())
        return;

    wxDir dir;
    wxString filename;
    FileManager *fm = FileManager::Get();
    std::list<LoaderBase*> loaders;
    int count = 0;

    // user paths first
    wxString path = ConfigManager::GetFolder(sdDataUser) + _T("/lexers/");
    if (wxDirExists(path) && dir.Open(path))
    {
        Manager::Get()->GetLogManager()->Log(F(_("Scanning for lexers in %s..."), path.wx_str()));
        bool ok = dir.GetFirst(&filename, _T("lexer_*.xml"), wxDIR_FILES);
        while (ok)
        {
            loaders.push_back(fm->Load(path + filename));
            ok = dir.GetNext(&filename);
            ++count;
        }
        Manager::Get()->GetLogManager()->Log(F(_("Found %d lexers"), count));
        count = 0;
    }

    // global paths next
    path = ConfigManager::GetFolder(sdDataGlobal) + _T("/lexers/");
    if (wxDirExists(path) && dir.Open(path))
    {
        Manager::Get()->GetLogManager()->Log(F(_("Scanning for lexers in %s..."), path.wx_str()));
        bool ok = dir.GetFirst(&filename, _T("lexer_*.xml"), wxDIR_FILES);
        while (ok)
        {
            loaders.push_back(fm->Load(path + filename));
            ok = dir.GetNext(&filename);
            ++count;
        }
        Manager::Get()->GetLogManager()->Log(F(_("Found %d lexers"), count));
    }

    EditorLexerLoader lex(this);
    for (std::list<LoaderBase*>::iterator it = loaders.begin(); it != loaders.end(); ++it)
        lex.Load(*it);

    ::Delete(loaders);


    for (OptionSetsMap::iterator it = m_Sets.begin(); it != m_Sets.end(); ++it)
    {
        wxString lang = it->second.m_Langs;
        if (lang.IsEmpty())
            continue;

        // keep the original filemasks and keywords, so we know what needs saving later
        for (int i = 0; i <= wxSCI_KEYWORDSET_MAX; ++i)
        {
            it->second.m_originalKeywords[i] = it->second.m_Keywords[i];
        }
        it->second.m_originalFileMasks = it->second.m_FileMasks;

        // remove old settings, no longer used
        unsigned int i = 0;
        while (i < it->second.m_Colours.GetCount())
        {
            OptionColour* opt = it->second.m_Colours.Item(i);
            // valid values are:
            if (opt->value < 0 &&               // styles >= 0
                opt->value != cbSELECTION &&    // cbSELECTION
                opt->value != cbHIGHLIGHT_LINE) // cbHIGHLIGHT_LINE
            {
                it->second.m_Colours.Remove(opt);
                delete opt;
            }
            else
                ++i;
        }
    }
}

HighlightLanguage EditorColourSet::AddHighlightLanguage(int lexer, const wxString& name)
{
    if (   lexer <= wxSCI_LEX_NULL
        || lexer >  wxSCI_LEX_LAST // this is a C::B extension to wxscintilla.h
        || name.IsEmpty() )
    {
        return HL_NONE;
    }

    // fix name to be XML compliant
    wxString newID;
    size_t pos = 0;
    while (pos < name.Length())
    {
        wxChar ch = name[pos];
        if      (wxIsalnum(ch) || ch == _T('_'))
            newID.Append(ch); // valid character
        else if (wxIsspace(ch))
            newID.Append(_T('_')); // convert spaces to underscores
        ++pos;
    }
    // make sure it's not starting with a number or underscore.
    // if it is, prepend an 'A'
    if (wxIsdigit(newID.GetChar(0)) || newID.GetChar(0) == _T('_'))
        newID.Prepend(_T('A'));

    if (GetHighlightLanguage(newID) != HL_NONE)
        return HL_NONE;

    m_Sets[newID].m_Langs = name;
    m_Sets[newID].m_Lexers = lexer;
    return newID;
}

HighlightLanguage EditorColourSet::GetHighlightLanguage(const wxString& name)
{
    for (OptionSetsMap::iterator it = m_Sets.begin(); it != m_Sets.end(); ++it)
    {
        if (it->second.m_Langs.CmpNoCase(name) == 0)
            return it->first;
    }
    return HL_NONE;
}

// from scintilla lexer (wxSCI_LEX_*)
// Warning: the first one found is returned!
HighlightLanguage EditorColourSet::GetHighlightLanguage(int lexer)
{
    for (OptionSetsMap::iterator it = m_Sets.begin(); it != m_Sets.end(); ++it)
    {
        if (it->second.m_Lexers == lexer)
            return it->first;
    }
    return HL_NONE;
}

// sorting helper function
static int CompareStringNoCase(const wxString& first, const wxString& second)
{
    return first.CmpNoCase(second);
}

wxArrayString EditorColourSet::GetAllHighlightLanguages()
{
    wxArrayString ret;
    for (OptionSetsMap::iterator it = m_Sets.begin(); it != m_Sets.end(); ++it)
    {
        if (!it->second.m_Langs.IsEmpty())
            ret.Add(it->second.m_Langs);
    }
    ret.Sort(CompareStringNoCase);
    return ret;
}

void EditorColourSet::UpdateOptionsWithSameName(HighlightLanguage lang, OptionColour* base)
{
    if (!base)
        return;
    // first find the index of this option
    int idx = -1;
    OptionSet& mset = m_Sets[lang];
    for (unsigned int i = 0; i < mset.m_Colours.GetCount(); ++i)
    {
        OptionColour* opt = mset.m_Colours.Item(i);
        if (opt == base)
        {
            idx = i;
            break;
        }
    }
    if (idx == -1)
        return;

    // now loop again, but update the other options with the same name
    for (unsigned int i = 0; i < mset.m_Colours.GetCount(); ++i)
    {
        if ((int)i == idx)
            continue; // skip the base option
        OptionColour* opt = mset.m_Colours.Item(i);
        if (!opt->name.Matches(base->name))
            continue;
        opt->fore       = base->fore;
        opt->back       = base->back;
        opt->bold       = base->bold;
        opt->italics    = base->italics;
        opt->underlined = base->underlined;
    }
}

bool EditorColourSet::AddOption(HighlightLanguage lang, OptionColour* option, bool checkIfExists)
{
    if (lang == HL_NONE)
        return false;

    if (checkIfExists && GetOptionByValue(lang, option->value))
        return false;

    OptionColours& colours =  m_Sets[lang].m_Colours;
    colours.Add(new OptionColour(*option));
    return true;
}

void EditorColourSet::AddOption(HighlightLanguage lang,
                                const wxString&   name,
                                int               value,
                                wxColour          fore,
                                wxColour          back,
                                bool              bold,
                                bool              italics,
                                bool              underlined,
                                bool              isStyle)
{
    if (lang == HL_NONE)
        return;

    OptionColour* opt = new OptionColour;
    opt->name = name;
    opt->value = value;
    opt->fore = fore;
    opt->back = back;
    opt->bold = bold;
    opt->italics = italics;
    opt->underlined = underlined;
    opt->isStyle = isStyle;

    opt->originalfore = fore;
    opt->originalback = back;
    opt->originalbold = bold;
    opt->originalitalics = italics;
    opt->originalunderlined = underlined;
    opt->originalisStyle = isStyle;

    AddOption(lang, opt);
    delete opt;
}

OptionColour* EditorColourSet::GetOptionByName(HighlightLanguage lang, const wxString& name)
{
    if (lang == HL_NONE)
        return 0L;

    OptionSet& mset = m_Sets[lang];
    for (unsigned int i = 0; i < mset.m_Colours.GetCount(); ++i)
    {
        OptionColour* opt = mset.m_Colours.Item(i);
        if (opt->name == name)
            return opt;
    }
    return 0L;
}

OptionColour* EditorColourSet::GetOptionByValue(HighlightLanguage lang, int value)
{
    if (lang == HL_NONE)
        return 0L;

    OptionSet& mset = m_Sets[lang];
    for (unsigned int i = 0; i < mset.m_Colours.GetCount(); ++i)
    {
        OptionColour* opt = mset.m_Colours.Item(i);
        if (opt->value == value)
            return opt;
    }
    return 0L;
}

OptionColour* EditorColourSet::GetOptionByIndex(HighlightLanguage lang, int index)
{
    if (lang == HL_NONE)
        return 0L;
    return m_Sets[lang].m_Colours.Item(index);
}

int EditorColourSet::GetOptionCount(HighlightLanguage lang)
{
    return m_Sets[lang].m_Colours.GetCount();
}

HighlightLanguage EditorColourSet::GetLanguageForFilename(const wxString& filename)
{
    // convert filename to lowercase first (m_FileMasks already contains
    // lowercase-only strings) and allow for filemasks like Makefile.*:
    wxString lfname = wxFileName(filename.Lower()).GetFullName();

    // first search in filemasks
    for (OptionSetsMap::iterator it = m_Sets.begin(); it != m_Sets.end(); ++it)
    {
        for (unsigned int x = 0; x < it->second.m_FileMasks.GetCount(); ++x)
        {
            if (lfname.Matches(it->second.m_FileMasks.Item(x)))
                return it->first;
        }
    }
    // parse #! directive
    if ( wxFileExists(filename) )
    {
        wxFileInputStream input(filename);
        wxTextInputStream text(input);
        wxString line;
        if (input.IsOk() && !input.Eof() )
            line = text.ReadLine();
        if (!line.IsEmpty())
        {
            wxRegEx reSheBang(wxT("#![ \t]*([a-zA-Z/]+)[ \t]*([a-zA-Z/]*)"));
            if (reSheBang.Matches(line))
            {
                wxString prog = reSheBang.GetMatch(line, 1);
                if (prog.EndsWith(wxT("env")))
                    prog = reSheBang.GetMatch(line, 2);
                if (prog.Find(wxT('/')) != wxNOT_FOUND)
                    prog = prog.AfterLast(wxT('/'));
                if (prog == wxT("sh"))
                    prog = wxT("bash");
                HighlightLanguage lang = GetHighlightLanguage(prog);
                if (lang !=  HL_NONE)
                    return lang;
            }
            else if (line.Trim().StartsWith(wxT("<?xml")))
                return GetHighlightLanguage(wxT("XML"));
        }
    }
    // standard headers
    const wxString cppNames = wxT("|"
            "algorithm|"        "array|"         "bitset|"        "chrono|"
            "complex|"          "deque|"         "exception|"     "fstream|"
            "forward_list|"     "functional|"    "hash_map|"      "hash_set|"
            "initializer_list|" "iomanip|"       "ios|"           "iostream|"
            "istream|"          "iterator|"      "limits|"        "list|"
            "locale|"           "map|"           "memory|"        "new|"
            "numeric|"          "ostream|"       "queue|"         "random|"
            "ratio|"            "regex|"         "set|"           "sstream|"
            "stack|"            "stdexcept|"     "streambuf|"     "string|"
            "strstream|"        "system_error|"  "tuple|"         "typeinfo|"
            "type_traits|"      "unordered_map|" "unordered_set|" "utility|"
            "valarray|"         "vector|"

            "cassert|" "cctype|"  "cerrno|"  "cfloat|"
            "ciso646|" "climits|" "clocale|" "cmath|"
            "csetjmp|" "csignal|" "cstdarg|" "cstdbool|"
            "cstddef|" "cstdint|" "cstdio|"  "cstdlib|"
            "cstring|" "ctime|"   "cwchar|"  "cwctype|"  );
    if (cppNames.Find(wxT("|") + lfname + wxT("|")) != wxNOT_FOUND)
        return GetHighlightLanguage(wxT("C/C++"));
    return HL_NONE;
}

wxString EditorColourSet::GetLanguageName(HighlightLanguage lang)
{
    if (lang == HL_NONE)
        return _("Plain text");

    wxString name = m_Sets[lang].m_Langs;
    if (!name.IsEmpty())
        return name;

    return _("Plain text");
}

void EditorColourSet::DoApplyStyle(cbStyledTextCtrl* control, int value, OptionColour* option)
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

HighlightLanguage EditorColourSet::Apply(cbEditor* editor, HighlightLanguage lang)
{
    if (!editor)
        return HL_NONE;

    if (lang == HL_AUTO)
        lang = GetLanguageForFilename(editor->GetFilename());

    const bool isC = (   Manager::Get()->GetConfigManager(wxT("editor"))->ReadBool(wxT("no_stl_in_c"), true)
                      && lang == GetHighlightLanguage(wxT("C/C++"))
                      && editor->GetFilename().Lower().EndsWith(wxT(".c")) );

    Apply(lang, editor->GetLeftSplitViewControl(),  isC);
    Apply(lang, editor->GetRightSplitViewControl(), isC);

    return lang;
}


void EditorColourSet::Apply(HighlightLanguage lang, cbStyledTextCtrl* control, bool isC)
{
    if (!control)
        return;
    control->StyleClearAll();

    if (lang == HL_NONE)
        return;

    // first load the default colours to all styles used by the actual lexer (ignoring some built-in styles)
    OptionColour* defaults = GetOptionByName(lang, _T("Default"));
    OptionSet& mset = m_Sets[lang];
    control->SetLexer(mset.m_Lexers);
    control->SetStyleBits(control->GetStyleBitsNeeded());
    if (defaults)
    {
        int countStyles = 1 << control->GetStyleBits();
        // walk until countStyles, otherwise the background-colour is only set for characters,
        // not for empty background
        for (int i = 0; i <= countStyles; ++i)
        {
            if (i < 33 || (i > 39 && i < wxSCI_STYLE_MAX))
                DoApplyStyle(control, i, defaults);
        }
    }
    // for some strange reason, when switching styles, the line numbering changes colour
    // too, though we didn't ask it to...
    // this makes sure it stays the correct colour
    control->StyleSetForeground(wxSCI_STYLE_LINENUMBER, wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));

    for (unsigned int i = 0; i < mset.m_Colours.GetCount(); ++i)
    {
        OptionColour* opt = mset.m_Colours.Item(i);

        if (opt->isStyle)
        {
            DoApplyStyle(control, opt->value, opt);
        }
        else
        {
            if (opt->value == cbHIGHLIGHT_LINE)
            {
                control->SetCaretLineBackground(opt->back);
                Manager::Get()->GetConfigManager(_T("editor"))->Write(_T("/highlight_caret_line_colour"), opt->back);
            }
            else if (opt->value == cbSELECTION)
            {
                if (opt->back != wxNullColour)
                {
                    control->SetSelBackground(true, opt->back);
//                    Manager::Get()->GetConfigManager(_T("editor"))->Write(_T("/selection_colour"), opt->back);
                }
                else
                    control->SetSelBackground(false, wxColour(0xC0, 0xC0, 0xC0));

                if (opt->fore != wxNullColour)
                {
                    control->SetSelForeground(true, opt->fore);
//                    Manager::Get()->GetConfigManager(_T("editor"))->Write(_T("/selection_fgcolour"), opt->fore);
                }
                else
                    control->SetSelForeground(false, *wxBLACK);
            }
//            else
//            {
//                control->MarkerDefine(-opt->value, 1);
//                control->MarkerSetBackground(-opt->value, opt->back);
//            }
        }
    }
    for (int i = 0; i <= wxSCI_KEYWORDSET_MAX; ++i)
    {
        if (!isC || i != 1) // exclude stl highlights for C
            control->SetKeyWords(i, mset.m_Keywords[i]);
    }

    control->Colourise(0, -1); // the *most* important part!
}

void EditorColourSet::Save()
{
    // no need for syntax highlighting if batch building
    if (Manager::IsBatchBuild())
        return;

    wxString key;
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));

    //FIXME: Commenting out the following line is no definite cure, but it hides the annoying disappearing colourset for now
    //NOTE (mandrav): uncommenting it doesn't seem to cause any trouble (at least now). What was the problem?
    cfg->DeleteSubPath(_T("/colour_sets/") + m_Name);

    // write the theme name
    cfg->Write(_T("/colour_sets/") + m_Name + _T("/name"), m_Name);

    for (OptionSetsMap::iterator it = m_Sets.begin(); it != m_Sets.end(); ++it)
    {
        if (it->first == HL_NONE || it->first == HL_AUTO)
            continue;
        wxString lang = it->first;

        bool gsaved = false;

        key.Clear();
        key << _T("/colour_sets/") << m_Name << _T('/') << lang;
        for (unsigned int i = 0; i < it->second.m_Colours.GetCount(); ++i)
        {
            OptionColour* opt = it->second.m_Colours.Item(i);
            wxString tmpKey;
            tmpKey << key << _T("/style") << wxString::Format(_T("%u"), i);

            bool saved = false;

            if (opt->fore != opt->originalfore && opt->fore != wxNullColour)
            {
                cfg->Write(tmpKey + _T("/fore"), opt->fore);
                saved = true;
            }
            if (opt->back != opt->originalback && opt->back != wxNullColour)
            {
                cfg->Write(tmpKey + _T("/back"), opt->back);
                saved = true;
            }
            if (opt->bold != opt->originalbold)
            {
                cfg->Write(tmpKey + _T("/bold"),       opt->bold);
                saved = true;
            }
            if (opt->italics != opt->originalitalics)
            {
                cfg->Write(tmpKey + _T("/italics"),    opt->italics);
                saved = true;
            }
            if (opt->underlined != opt->originalunderlined)
            {
                cfg->Write(tmpKey + _T("/underlined"), opt->underlined);
                saved = true;
            }
            if (opt->isStyle != opt->originalisStyle)
            {
                cfg->Write(tmpKey + _T("/isStyle"),    opt->isStyle);
                saved = true;
            }

            if (saved)
            {
                cfg->Write(tmpKey + _T("/name"), opt->name, true);
                gsaved = true;
            }
        }
        wxString tmpkey;
        for (int i = 0; i <= wxSCI_KEYWORDSET_MAX; ++i)
        {
            if (it->second.m_Keywords[i] != it->second.m_originalKeywords[i])
            {
                tmpkey.Printf(_T("%s/editor/keywords/set%d"), key.c_str(), i);
                cfg->Write(tmpkey, it->second.m_Keywords[i]);
                gsaved = true;
            }
        }
        tmpkey.Printf(_T("%s/editor/filemasks"), key.c_str());
        wxString tmparr = GetStringFromArray(it->second.m_FileMasks, _T(","));
        wxString tmparrorig = GetStringFromArray(it->second.m_originalFileMasks, _T(","));
        if (tmparr != tmparrorig)
        {
            cfg->Write(tmpkey, tmparr);
            gsaved = true;
        }

        if (gsaved)
            cfg->Write(key + _T("/name"), it->second.m_Langs);
    }
}

void EditorColourSet::Load()
{
    // no need for syntax highlighting if batch building
    if (Manager::IsBatchBuild())
        return;

    static bool s_notifiedUser = false;

    wxString key;
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));

    // read the theme name
    m_Name = cfg->Read(_T("/colour_sets/") + m_Name + _T("/name"), m_Name);

    int x = 0;
    for (OptionSetsMap::iterator it = m_Sets.begin(); it != m_Sets.end(); ++it)
    {
        if (it->first == HL_NONE || it->first == HL_AUTO)
            continue;

        // look for old-style configuration
        key.Clear();
        key << _T("/colour_sets/") << m_Name << _T("/set") << wxString::Format(_T("%d"), x++);
        if (cfg->Exists(key + _T("/name")))
        {
            // old-style configuration
            // delete it and tell the user about it
            cfg->DeleteSubPath(key);
            if (!s_notifiedUser)
            {
                cbMessageBox(_("The way editor syntax highlighting configuration is saved, has changed.\n"
                                "Syntax highlighting for all supported languages will revert to defaults now.\n"
                                "We 're sorry for the inconvenience..."),
                                _("Information"),
                                wxICON_INFORMATION);
                s_notifiedUser = true;
            }
            continue;
        }
        // make sure we didn't create it accidentally
        cfg->DeleteSubPath(key);

        // new-style configuration key
        key.Clear();
        key << _T("/colour_sets/") << m_Name << _T('/') << it->first;
        if (!cfg->Exists(key + _T("/name")))
        {
            // make sure we didn't create it accidentally
            cfg->DeleteSubPath(key);
            continue;
        }

        for (unsigned int i = 0; i < it->second.m_Colours.GetCount(); ++i)
        {
            wxString tmpKey;
            tmpKey << key << _T("/style") << wxString::Format(_T("%u"), i);
            if (!cfg->Exists(tmpKey + _T("/name")))
            {
                // make sure we didn't create it accidentally
                cfg->DeleteSubPath(tmpKey);
                continue;
            }
            wxString name = cfg->Read(tmpKey + _T("/name"));
            for (size_t j = 0; j < it->second.m_Colours.GetCount(); ++j)
            {
                OptionColour* opt = it->second.m_Colours.Item(j);
                if (!opt || opt->name != name)
                    continue;

                if (cfg->Exists(tmpKey + _T("/fore")))
                    opt->fore = cfg->ReadColour(tmpKey     + _T("/fore"),       opt->fore);
                if (cfg->Exists(tmpKey + _T("/back")))
                    opt->back = cfg->ReadColour(tmpKey     + _T("/back"),       opt->back);
                if (cfg->Exists(tmpKey + _T("/bold")))
                    opt->bold = cfg->ReadBool(tmpKey       + _T("/bold"),       opt->bold);
                if (cfg->Exists(tmpKey + _T("/italics")))
                    opt->italics = cfg->ReadBool(tmpKey    + _T("/italics"),    opt->italics);
                if (cfg->Exists(tmpKey + _T("/underlined")))
                    opt->underlined = cfg->ReadBool(tmpKey + _T("/underlined"), opt->underlined);
                if (cfg->Exists(tmpKey + _T("/isStyle")))
                    opt->isStyle = cfg->ReadBool(tmpKey    + _T("/isStyle"),    opt->isStyle);
            }
        }
        wxString tmpkey;
        for (int i = 0; i <= wxSCI_KEYWORDSET_MAX; ++i)
        {
            tmpkey.Printf(_T("%s/editor/keywords/set%d"), key.c_str(), i);
            if (cfg->Exists(tmpkey))
                it->second.m_Keywords[i] = cfg->Read(tmpkey, wxEmptyString);
        }
        tmpkey.Printf(_T("%s/editor/filemasks"), key.c_str());
        if (cfg->Exists(tmpkey))
            it->second.m_FileMasks = GetArrayFromString(cfg->Read(tmpkey, wxEmptyString), _T(","));
    }
}

void EditorColourSet::Reset(HighlightLanguage lang)
{
    // no need for syntax highlighting if batch building
    if (Manager::IsBatchBuild())
        return;

    wxString key;
    key << _T("/colour_sets/") << m_Name << _T('/') << lang;
    if (Manager::Get()->GetConfigManager(_T("editor"))->Exists(key + _T("/name")))
        Manager::Get()->GetConfigManager(_T("editor"))->DeleteSubPath(key);

    ClearAllOptionColours();
    LoadAvailableSets();
    Load();
}

wxString& EditorColourSet::GetKeywords(HighlightLanguage lang, int idx)
{
    if (idx < 0 || idx > wxSCI_KEYWORDSET_MAX)
        idx = 0;
    return m_Sets[lang].m_Keywords[idx];
}

void EditorColourSet::SetKeywords(HighlightLanguage lang, int idx, const wxString& keywords)
{
    if (lang != HL_NONE && idx >=0 && idx <= wxSCI_KEYWORDSET_MAX)
    {
        wxString tmp(_T(' '), keywords.length()); // faster than using Alloc()

        const wxChar *src = keywords.c_str();
        #if wxCHECK_VERSION(2, 9, 0)
        wxStringCharType *dst = const_cast<wxStringCharType*>(tmp.wx_str());
        #else
        wxChar *dst = (wxChar *) tmp.c_str();
        #endif
        wxChar c;
        size_t len = 0;

        while ((c = *src))
        {
            ++src;
            if (c > _T(' '))
                *dst = c;
            else // white space
            {
                *dst = _T(' ');
                while (*src && *src < _T(' '))
                    ++src;
            }

            ++dst;
            ++len;
        }

        tmp.Truncate(len);

        OptionSet& mset = m_Sets[lang];
        mset.m_Keywords[idx] = tmp;
    }
}

const wxArrayString& EditorColourSet::GetFileMasks(HighlightLanguage lang)
{
    return m_Sets[lang].m_FileMasks;
}

void EditorColourSet::SetFileMasks(HighlightLanguage lang, const wxString& masks, const wxString& separator)
{
    if (lang != HL_NONE)
    {
        m_Sets[lang].m_FileMasks = GetArrayFromString(masks.Lower(), separator);

        // let's add these filemasks in the file filters master list ;)
        FileFilters::Add(wxString::Format(_("%s files"), m_Sets[lang].m_Langs.c_str()), masks);
    }
}

wxString EditorColourSet::GetSampleCode(HighlightLanguage lang, int* breakLine, int* debugLine, int* errorLine)
{
    if (lang == HL_NONE)
        return wxEmptyString;
    OptionSet& mset = m_Sets[lang];
    if (breakLine)
        *breakLine = mset.m_BreakLine;
    if (debugLine)
        *debugLine = mset.m_DebugLine;
    if (errorLine)
        *errorLine = mset.m_ErrorLine;

    const wxString shortname = _T("lexer_") + lang + _T(".sample");
    // user path first
    wxString path = ConfigManager::GetFolder(sdDataUser) + _T("/lexers/");
    wxFileName fullname( path + shortname );
    if ( !fullname.FileExists(path + shortname) )
    {
        // global path next
        path = ConfigManager::GetFolder(sdDataGlobal) + _T("/lexers/");
    }
    if ( !mset.m_SampleCode.IsEmpty() )
        return path + mset.m_SampleCode;
    return wxEmptyString;
}

void EditorColourSet::SetSampleCode(HighlightLanguage lang, const wxString& sample, int breakLine, int debugLine, int errorLine)
{
    if (lang == HL_NONE)
        return;
    OptionSet& mset = m_Sets[lang];
    mset.m_SampleCode = sample;
    mset.m_BreakLine = breakLine;
    mset.m_DebugLine = debugLine;
    mset.m_ErrorLine = errorLine;
}

void EditorColourSet::SetCommentToken(HighlightLanguage lang, CommentToken token)
{
    if (lang == HL_NONE)
        return;
    m_Sets[lang].comment = token;
}

CommentToken EditorColourSet::GetCommentToken(HighlightLanguage lang)
{
    CommentToken com;
    com.lineComment               = _T("");
    com.doxygenLineComment        = _T("");
    com.streamCommentStart        = _T("");
    com.streamCommentEnd          = _T("");
    com.doxygenStreamCommentStart = _T("");
    com.doxygenStreamCommentEnd   = _T("");
    com.boxCommentStart           = _T("");
    com.boxCommentMid             = _T("");
    com.boxCommentEnd             = _T("");

    if (lang != HL_NONE)
        com = m_Sets[lang].comment;

    return com;
}

void EditorColourSet::SetCaseSensitivity(HighlightLanguage lang, bool CaseSensitive)
{
    if ( lang == HL_NONE )
        return;

    m_Sets[lang].m_CaseSensitive = CaseSensitive;
}

bool EditorColourSet::GetCaseSensitivity(HighlightLanguage lang)
{
    if ( lang == HL_NONE )
        return false;

    return m_Sets[lang].m_CaseSensitive;
}

void EditorColourSet::SetStringLexerStyles(HighlightLanguage lang, const std::set<int> &styles)
{
    if ( lang == HL_NONE )
        return;

   cbStyledTextCtrl::GetStringLexerStyles()[m_Sets[lang].m_Lexers] = styles;
}

void EditorColourSet::SetCommentLexerStyles(HighlightLanguage lang, const std::set<int> &styles)
{
    if ( lang == HL_NONE )
        return;

   cbStyledTextCtrl::GetCommentLexerStyles()[m_Sets[lang].m_Lexers] = styles;
}

void EditorColourSet::SetCharacterLexerStyles(HighlightLanguage lang, const std::set<int> &styles)
{
    if ( lang == HL_NONE )
        return;

   cbStyledTextCtrl::GetCharacterLexerStyles()[m_Sets[lang].m_Lexers] = styles;
}

void EditorColourSet::SetPreprocessorLexerStyles(HighlightLanguage lang, const std::set<int> &styles)
{
    if ( lang == HL_NONE )
        return;

   cbStyledTextCtrl::GetPreprocessorLexerStyles()[m_Sets[lang].m_Lexers] = styles;
}

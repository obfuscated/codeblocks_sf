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
    #include <configurationpanel.h>
    #include <cbstyledtextctrl.h>
    #include <editorcolourset.h>
#endif

#include "abbreviations.h"
#include "abbreviationsconfigpanel.h"

#include <sqplus.h>
#include <sc_base_types.h>
#include <editor_hooks.h>


// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
    PluginRegistrant<Abbreviations> reg(_T("Abbreviations"));
    const int idEditAutoComplete = wxNewId();
}

Abbreviations* Abbreviations::m_Singleton = nullptr;

/* XPM */
static const char* abbrev_xpm[] = {
"16 16 7 1",
"   c None",
".  c #B00000",
"+  c #F70000",
"@  c #DD3F3F",
"#  c #FF3A39",
"$  c #D56D6C",
"%  c #F5A8A8",
"                ",
"                ",
"          +.    ",
"   %%#++++++    ",
"  %%#++++++++   ",
" $%@+++++++++   ",
" %@++. +++++++  ",
" #++.  +++####  ",
" +++   .@$$%%$@ ",
"++++            ",
"+++.            ",
"+++             ",
"++.             ",
"+..             ",
"                ",
"                "};

// events handling
BEGIN_EVENT_TABLE(Abbreviations, cbPlugin)
    // add any events you want to handle here
    EVT_MENU(idEditAutoComplete,      Abbreviations::OnEditAutoComplete)
    EVT_UPDATE_UI(idEditAutoComplete, Abbreviations::OnEditMenuUpdateUI)
END_EVENT_TABLE()

wxString defaultLanguageStr = _T("--default--");

// constructor
Abbreviations::Abbreviations()
{
    // Make sure our resources are available.
    // In the generated boilerplate code we have no resources but when
    // we add some, it will be nice that this code is in place already ;)
    if (!Manager::LoadResource(_T("abbreviations.zip")))
        NotifyMissingFile(_T("abbreviations.zip"));

    m_IsAutoCompVisible = false;
}

// destructor
Abbreviations::~Abbreviations()
{
}

void Abbreviations::OnAttach()
{
    // do whatever initialization you need for your plugin
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be TRUE...
    // You should check for it in other functions, because if it
    // is FALSE, it means that the application did *not* "load"
    // (see: does not need) this plugin...

    //wxASSERT(m_Singleton == 0);
    m_Singleton = this;

    LoadAutoCompleteConfig();
    RegisterScripting();

    // hook to editors
    EditorHooks::HookFunctorBase* myhook = new EditorHooks::HookFunctor<Abbreviations>(this, &Abbreviations::EditorEventHook);
    m_EditorHookId = EditorHooks::RegisterHook(myhook);
}

void Abbreviations::OnRelease(cb_unused bool appShutDown)
{
    // do de-initialization for your plugin
    // if appShutDown is true, the plugin is unloaded because Code::Blocks is being shut down,
    // which means you must not use any of the SDK Managers
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be FALSE...

    UnregisterScripting();
    SaveAutoCompleteConfig();

    if (m_Singleton == this)
        m_Singleton = nullptr;

    // unregister hook
    // 'true' will delete the functor too
    EditorHooks::UnregisterHook(m_EditorHookId, true);

    ClearAutoCompLanguageMap();
}

void Abbreviations::RegisterScripting()
{
    Manager::Get()->GetScriptingManager();
    if (SquirrelVM::GetVMPtr())
        SqPlus::RegisterGlobal(&Abbreviations::AutoComplete, "AutoComplete");
}

void Abbreviations::UnregisterScripting()
{
    Manager::Get()->GetScriptingManager();
    HSQUIRRELVM v = SquirrelVM::GetVMPtr();
    if (v)
    {
        sq_pushstring(v, "AutoComplete", -1);
        sq_pushroottable(v);
        sq_deleteslot(v, -2, false);
        sq_poptop(v);
    }
}

void Abbreviations::BuildMenu(wxMenuBar* menuBar)
{
    //NOTE: Be careful in here... The application's menubar is at your disposal.

    // if not attached, exit
    if ( !IsAttached() )
        return;

    int editmenuPos = menuBar->FindMenu(_("&Edit"));
    if (editmenuPos == wxNOT_FOUND) return;
    wxMenu* editMenu = menuBar->GetMenu(editmenuPos);

    if (editMenu)
    {
        editMenu->AppendSeparator();
        editMenu->Append(idEditAutoComplete, _T("Auto-complete\tCtrl-J"), _T("Auto-completes the word under the caret (nothing to do with code-completion plugins)"));
    }
}

void Abbreviations::OnEditAutoComplete(cb_unused wxCommandEvent& event)
{
    cbEditor* editor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    cbStyledTextCtrl* control = editor->GetControl();

    const AutoCompleteMap& acm = *GetCurrentACMap(editor);

    int curPos = control->GetCurrentPos();
    int startPos = control->WordStartPosition(curPos, true);
    const int endPos = control->WordEndPosition(curPos, true);

    const wxString keyword = control->GetTextRange(startPos, endPos);
    AutoCompleteMap::const_iterator acm_it = acm.find(keyword);

    if (acm_it != acm.end())
    {
        DoAutoComplete(editor);
	}
    else
    {
        wxArrayString items;
        for (acm_it = acm.begin(); acm_it != acm.end(); ++acm_it)
        {
            if (acm_it->first.Lower().StartsWith(keyword))
                items.Add(acm_it->first + _T("?0"));
        }

        if (!items.IsEmpty())
        {
            control->ClearRegisteredImages();
            control->RegisterImage(0, wxBitmap(abbrev_xpm));
            items.Sort();
            wxString itemsStr = GetStringFromArray(items, _T(" "));
            control->AutoCompSetSeparator(_T(' '));
            control->AutoCompShow(endPos-startPos, itemsStr);
        }
        m_IsAutoCompVisible = control->AutoCompActive();
    }

}

void Abbreviations::OnEditMenuUpdateUI(wxUpdateUIEvent& event)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    event.Enable(ed != NULL);
}

void Abbreviations::AutoComplete(cbEditor& ed)
{
    if (Abbreviations::Get())
        Abbreviations::Get()->DoAutoComplete(&ed);
}

void Abbreviations::DoAutoComplete(cbEditor* ed)
{
    if (!ed)
        return;

    cbStyledTextCtrl* control = ed->GetControl();
    if (!control)
        return;

    if (control->AutoCompActive())
        control->AutoCompCancel();

    if (control->CallTipActive())
        control->CallTipCancel();

    m_IsAutoCompVisible = false;

    LogManager* msgMan = Manager::Get()->GetLogManager();
    int curPos = control->GetCurrentPos();
    int wordStartPos = control->WordStartPosition(curPos, true);
    const int endPos = control->WordEndPosition(curPos, true);
    wxString keyword = control->GetTextRange(wordStartPos, endPos);
    wxString lineIndent = ed->GetLineIndentString(control->GetCurrentLine());
    msgMan->DebugLog(_T("Auto-complete keyword: ") + keyword);

    AutoCompleteMap* pAutoCompleteMap = GetCurrentACMap(ed);
    AutoCompleteMap::iterator it = pAutoCompleteMap->find(keyword);
    if (it != pAutoCompleteMap->end() )
    {
        // found; auto-complete it
        msgMan->DebugLog(_T("Auto-complete match for keyword found."));

        // indent code accordingly
        wxString code = it->second;
        code.Replace(_T("\n"), _T('\n') + lineIndent);

        // look for and replace macros
        int macroPos = code.Find(_T("$("));
        while (macroPos != -1)
        {
            // locate ending parenthesis
            int macroPosEnd = macroPos + 2;
            int len = (int)code.Length();

            while (macroPosEnd < len && code.GetChar(macroPosEnd) != _T(')'))
                ++macroPosEnd;

            if (macroPosEnd == len)
                return; // no ending parenthesis

            wxString macroName = code.SubString(macroPos + 2, macroPosEnd - 1);
            msgMan->DebugLog(_T("Found macro: ") + macroName);
            wxString macro = wxGetTextFromUser(_("Please enter the text for \"") + macroName + _T("\":"),
                                               _("Macro substitution"));
            if (macro.IsEmpty())
                return;

            code.Replace(_T("$(") + macroName + _T(")"), macro);
            macroPos = code.Find(_T("$("));
        }

        control->BeginUndoAction();

        // delete keyword
        control->SetSelectionVoid(wordStartPos, endPos);
        control->ReplaceSelection(wxEmptyString);
        curPos = wordStartPos;

        // replace any other macros in the generated code
        Manager::Get()->GetMacrosManager()->ReplaceMacros(code);
        // match current EOL mode
        if (control->GetEOLMode() == wxSCI_EOL_CRLF)
            code.Replace(wxT("\n"), wxT("\r\n"));
        else if (control->GetEOLMode() == wxSCI_EOL_CR)
            code.Replace(wxT("\n"), wxT("\r"));
        // add the text
        control->InsertText(curPos, code);

        // put cursor where "|" appears in code (if it appears)
        int caretPos = code.Find(_T('|'));
        if (caretPos != -1)
        {
            control->SetCurrentPos(curPos + caretPos);
            control->SetSelectionVoid(curPos + caretPos, curPos + caretPos + 1);
            control->ReplaceSelection(wxEmptyString);
        }
        control->ChooseCaretX();

        control->EndUndoAction();
    }
}

void Abbreviations::LoadAutoCompleteConfig()
{
    ClearAutoCompLanguageMap();
    AutoCompleteMap* pAutoCompleteMap;
    wxArrayString list = Manager::Get()->GetConfigManager(_T("editor"))->EnumerateSubPaths(_T("/auto_complete"));
    for (unsigned int i = 0; i < list.GetCount(); ++i)
    {
        wxString langStr = Manager::Get()->GetConfigManager(_T("editor"))->Read(_T("/auto_complete/") + list[i] + _T("/language"), defaultLanguageStr);
        wxString name = Manager::Get()->GetConfigManager(_T("editor"))->Read(_T("/auto_complete/") + list[i] + _T("/name"), wxEmptyString);
        wxString code = Manager::Get()->GetConfigManager(_T("editor"))->Read(_T("/auto_complete/") + list[i] + _T("/code"), wxEmptyString);
        if (m_AutoCompLanguageMap.find(langStr) == m_AutoCompLanguageMap.end())
            m_AutoCompLanguageMap[langStr] = new AutoCompleteMap();

        pAutoCompleteMap = m_AutoCompLanguageMap[langStr];

        if (name.IsEmpty())
            continue;
        // convert non-printable chars to printable
        wxString resolved;
        resolved.Alloc(code.Length());
        for (size_t pos = 0; pos < code.Length(); ++pos)
        {
            if (code[pos] == wxT('\\') && pos < code.Length() - 1)
            {
                ++pos;
                if (code[pos] == wxT('n'))
                    resolved += wxT("\n");
                else if (code[pos] == wxT('r')) // should not exist, remove in next step
                    resolved += wxT("\r");
                else if (code[pos] == wxT('t'))
                    resolved += wxT("\t");
                else if (code[pos] == wxT('\\'))
                    resolved += wxT("\\");
                else // ?!
                {
                    resolved += wxT("\\");
                    resolved += code[pos];
                }
            }
            else
                resolved += code[pos];
        }

        // should not exist, but remove if it does (EOL style is matched just before code generation)
        resolved.Replace(wxT("\r\n"), wxT("\n"));
        resolved.Replace(wxT("\r"),   wxT("\n"));
        (*pAutoCompleteMap)[name] = resolved;
    }

    if (m_AutoCompLanguageMap.find(defaultLanguageStr) == m_AutoCompLanguageMap.end())
        m_AutoCompLanguageMap[defaultLanguageStr] = new AutoCompleteMap();

    pAutoCompleteMap = m_AutoCompLanguageMap[defaultLanguageStr];
    if (pAutoCompleteMap->empty())
    {
        // default auto-complete items
        (*pAutoCompleteMap)[_T("if")]     = _T("if (|)\n\t;");
        (*pAutoCompleteMap)[_T("ifb")]    = _T("if (|)\n{\n\t\n}");
        (*pAutoCompleteMap)[_T("ife")]    = _T("if (|)\n{\n\t\n}\nelse\n{\n\t\n}");
        (*pAutoCompleteMap)[_T("ifei")]   = _T("if (|)\n{\n\t\n}\nelse if ()\n{\n\t\n}\nelse\n{\n\t\n}");
        (*pAutoCompleteMap)[_T("guard")]  = _T("#ifndef $(Guard token)\n#define $(Guard token)\n\n|\n\n#endif // $(Guard token)\n");
        (*pAutoCompleteMap)[_T("while")]  = _T("while (|)\n\t;");
        (*pAutoCompleteMap)[_T("whileb")] = _T("while (|)\n{\n\t\n}");
        (*pAutoCompleteMap)[_T("do")]     = _T("do\n{\n\t\n} while (|);");
        (*pAutoCompleteMap)[_T("switch")] = _T("switch (|)\n{\ncase :\n\tbreak;\n\ndefault:\n\tbreak;\n}\n");
        (*pAutoCompleteMap)[_T("for")]    = _T("for (|; ; )\n\t;");
        (*pAutoCompleteMap)[_T("forb")]   = _T("for (|; ; )\n{\n\t\n}");
        (*pAutoCompleteMap)[_T("class")]  = _T("class $(Class name)|\n{\npublic:\n\t$(Class name)();\n\t~$(Class name)();\nprotected:\nprivate:\n};\n");
        (*pAutoCompleteMap)[_T("struct")] = _T("struct |\n{\n\t\n};\n");
    }
    ExchangeTabAndSpaces(*pAutoCompleteMap);
    // date and time macros
    // these are auto-added if they 're found to be missing
    const wxString timeAndDate[9][2] =
    {
        { _T("tday"),   _T("$TDAY") },
        { _T("tdayu"),  _T("$TDAY_UTC") },
        { _T("today"),  _T("$TODAY") },
        { _T("todayu"), _T("$TODAY_UTC") },
        { _T("now"),    _T("$NOW") },
        { _T("nowl"),   _T("$NOW_L") },
        { _T("nowu"),   _T("$NOW_UTC") },
        { _T("nowlu"),  _T("$NOW_L_UTC") },
        { _T("wdu"),    _T("$WEEKDAY_UTC") },
    };

    for (int i = 0; i < 9; ++i)
    {
        if (pAutoCompleteMap->find(timeAndDate[i][0]) == pAutoCompleteMap->end())
            (*pAutoCompleteMap)[timeAndDate[i][0]] = timeAndDate[i][1];
    }

    wxString langFortran = _T("Fortran");
    if (m_AutoCompLanguageMap.find(langFortran) == m_AutoCompLanguageMap.end())
        m_AutoCompLanguageMap[langFortran] = new AutoCompleteMap();

    pAutoCompleteMap = m_AutoCompLanguageMap[langFortran];
    if (pAutoCompleteMap->empty())
    {
        // default auto-complete items for Fortran
        (*pAutoCompleteMap)[_T("if")]  = _T("if (|) then\n\t\nend if\n");
        (*pAutoCompleteMap)[_T("do")]  = _T("do |\n\t\nend do\n");
        (*pAutoCompleteMap)[_T("dw")]  = _T("do while (|)\n\t\nend do\n");
        (*pAutoCompleteMap)[_T("sc")]  = _T("select case (|)\n\tcase ()\n\t\t\n\tcase default\n\t\t\nend select\n");
        (*pAutoCompleteMap)[_T("fun")] = _T("function |()\n\t\nend function\n");
        (*pAutoCompleteMap)[_T("sub")] = _T("subroutine |()\n\t\nend subroutine\n");
        (*pAutoCompleteMap)[_T("mod")] = _T("module |\n\t\nend module\n");
        (*pAutoCompleteMap)[_T("ty")]  = _T("type |\n\t\nend type\n");
    }
    ExchangeTabAndSpaces(*pAutoCompleteMap);
}

void Abbreviations::ExchangeTabAndSpaces(AutoCompleteMap& map)
{
    const bool useTabs = Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/use_tab"), false);
    const int tabSize = Manager::Get()->GetConfigManager(_T("editor"))->ReadInt(_T("/tab_size"), 4);
    const wxString tabSpace = wxString(_T(' '), tabSize);
    for (AutoCompleteMap::iterator it = map.begin(); it != map.end(); ++it)
    {
        wxString& item = it->second;
        if (useTabs)
            item.Replace(tabSpace, _T("\t"), true);
        else
            item.Replace(_T("\t"), tabSpace, true);
    }
}

void Abbreviations::SaveAutoCompleteConfig()
{
    Manager::Get()->GetConfigManager(_T("editor"))->DeleteSubPath(_T("/auto_complete"));
    AutoCompLanguageMap::iterator itlan;
    AutoCompleteMap::iterator it;
    int count = 0;
    for (itlan = m_AutoCompLanguageMap.begin(); itlan != m_AutoCompLanguageMap.end(); ++itlan)
    {
        wxString langStr = itlan->first;
        wxString langStrLw = langStr.Lower();
        AutoCompleteMap* pAutoCompleteMap = itlan->second;
        for (it = pAutoCompleteMap->begin(); it != pAutoCompleteMap->end(); ++it)
        {
            wxString code = it->second;
            // convert non-printable chars to printable
            code.Replace(_T("\\"),   _T("\\\\"));
            code.Replace(_T("\r\n"), _T("\\n")); // EOL style will be matched just before code generation
            code.Replace(_T("\n"),   _T("\\n"));
            code.Replace(_T("\r"),   _T("\\n"));
            code.Replace(_T("\t"),   _T("\\t"));

            ++count;
            wxString key;
            if (!langStr.IsSameAs(defaultLanguageStr))
            {
                key.Printf(_T("/auto_complete/entry%d/language"), count);
                Manager::Get()->GetConfigManager(_T("editor"))->Write(key, langStr);
            }
            key.Printf(_T("/auto_complete/entry%d/name"), count);
            Manager::Get()->GetConfigManager(_T("editor"))->Write(key, it->first);
            key.Printf(_T("/auto_complete/entry%d/code"), count);
            Manager::Get()->GetConfigManager(_T("editor"))->Write(key, code);
        }
    }
}

void Abbreviations::EditorEventHook(cbEditor* editor, wxScintillaEvent& event)
{
    cbStyledTextCtrl* control = editor->GetControl();

    if ( !IsAttached() || !m_IsAutoCompVisible || !control )
    {
        event.Skip();
        return;
    }

    if (event.GetEventType() == wxEVT_SCI_AUTOCOMP_SELECTION)
    {
        const wxString& itemText = event.GetText();
        int curPos = control->GetCurrentPos();
        int startPos = control->WordStartPosition(curPos, true);
        const int endPos = control->WordEndPosition(curPos, true);

        control->BeginUndoAction();
        control->SetTargetStart(startPos);
        control->SetTargetEnd(endPos);
        control->ReplaceTarget(itemText);
        control->GotoPos(startPos + itemText.size() );
        control->EndUndoAction();

        DoAutoComplete(editor);

        // prevent other plugins from insertion this keyword
        event.SetText(wxEmptyString);
        event.SetEventType(wxEVT_NULL);
    }
    else // here should be: else if (event.GetEventType() == wxEVT_SCI_AUTOCOMP_CANCELLED)
    {    // but is this event doesn't occur.
        m_IsAutoCompVisible = control->AutoCompActive();
    }

    if (!m_IsAutoCompVisible)
      event.Skip(); // allow others to handle this event
}

cbConfigurationPanel* Abbreviations::GetConfigurationPanel(wxWindow* parent)
{
    return new AbbreviationsConfigPanel(parent, this);
}

void Abbreviations::ClearAutoCompLanguageMap()
{
    AutoCompLanguageMap::iterator it;
    for (it=m_AutoCompLanguageMap.begin(); it!=m_AutoCompLanguageMap.end(); ++it)
    {
        it->second->clear();
        delete it->second;
        it->second = 0;
    }
    m_AutoCompLanguageMap.clear();
}

AutoCompleteMap* Abbreviations::GetCurrentACMap(cbEditor* ed)
{
    AutoCompleteMap* pAutoCompleteMap;
    EditorColourSet* theme = ed->GetColourSet();
    if (theme)
    {
        wxString strLang = theme->GetLanguageName(ed->GetLanguage());

        if (strLang == _T("Fortran77")) // the same abbreviations for Fortran and Fortran77
            strLang = _T("Fortran");

        if (m_AutoCompLanguageMap.find(strLang) == m_AutoCompLanguageMap.end())
            pAutoCompleteMap = m_AutoCompLanguageMap[defaultLanguageStr];
        else
            pAutoCompleteMap = m_AutoCompLanguageMap[strLang];
    }
    else
        pAutoCompleteMap = m_AutoCompLanguageMap[defaultLanguageStr];

    return pAutoCompleteMap;
}

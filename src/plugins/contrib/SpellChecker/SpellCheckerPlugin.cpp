/*
* This file is part of SpellChecker plugin for Code::Blocks Studio
* Copyright (C) 2009 Daniel Anselmi
*
* SpellChecker plugin is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* SpellChecker plugin is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with SpellChecker. If not, see <http://www.gnu.org/licenses/>.
*
*/
#include <sdk.h> // Code::Blocks SDK
#ifndef CB_PRECOMP
    #include <cbeditor.h>
    #include <configmanager.h>
    #include <editorcolourset.h>
    #include <editormanager.h>
#endif
#include <configurationpanel.h>
#include <editor_hooks.h>
#include <cbstyledtextctrl.h>
#ifdef wxUSE_STATUSBAR
#include <cbstatusbar.h>
#endif

#include <wx/dir.h>

#include "SpellCheckerPlugin.h"
#include "SpellCheckSettingsPanel.h"
#include "SpellCheckerConfig.h"
#include "OnlineSpellChecker.h"
#include "Thesaurus.h"

#include "HunspellInterface.h"
#include "SpellCheckHelper.h"
#include "MySpellingDialog.h"
#include "StatusField.h"
#include "DictionariesNeededDialog.h"

// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
    PluginRegistrant<SpellCheckerPlugin> reg(_T("SpellChecker"));

    const int idSpellCheck                 = wxNewId();
    const int idThesaurus                  = wxNewId();
    const int idCamelCase                  = wxNewId();

    const unsigned int MaxSuggestEntries = 5;
    const int idSuggest[MaxSuggestEntries] =
        {static_cast<int>(wxNewId()), static_cast<int>(wxNewId()), static_cast<int>(wxNewId()), static_cast<int>(wxNewId()), static_cast<int>(wxNewId())};
    const int idAddToDictionary            = wxNewId();
    const int idMoreSuggestions            = wxNewId();
}


// events handling
BEGIN_EVENT_TABLE(SpellCheckerPlugin, cbPlugin)
    // add any events you want to handle here
END_EVENT_TABLE()

SpellCheckerPlugin::SpellCheckerPlugin():
    m_pSpellChecker(NULL),
    m_pSpellingDialog(NULL),
    m_pSpellHelper(NULL),
    m_pOnlineChecker(NULL),
    m_pThesaurus(NULL),
    m_sccfg(NULL)
#ifdef wxUSE_STATUSBAR
    ,m_fld(NULL)
#endif
{
    // Make sure our resources are available.
    // In the generated boilerplate code we have no resources but when
    // we add some, it will be nice that this code is in place already ;)
    if(!Manager::LoadResource(_T("SpellChecker.zip")))
    {
        NotifyMissingFile(_T("SpellChecker.zip"));
    }
}

SpellCheckerPlugin::~SpellCheckerPlugin()
{
}


void SpellCheckerPlugin::OnAttach()
{
    // do whatever initialization you need for your plugin
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be TRUE...
    // You should check for it in other functions, because if it
    // is FALSE, it means that the application did *not* "load"
    // (see: does not need) this plugin...

    // load configuration
    m_sccfg = new SpellCheckerConfig(this);

    DictionariesNeededDialog dlg;
    if (m_sccfg->GetPossibleDictionaries().empty())
        dlg.ShowModal();


    //initialize spell checker
    if ( !m_pSpellingDialog )
        m_pSpellingDialog = new MySpellingDialog( Manager::Get()->GetAppFrame() );
    m_pSpellChecker = new HunspellInterface(m_pSpellingDialog);
    ConfigureHunspellSpellCheckEngine();
    m_pSpellChecker->InitializeSpellCheckEngine();

    // initialize Helper and online checker
    m_pSpellHelper = new SpellCheckHelper();
    m_pOnlineChecker = new OnlineSpellChecker(m_pSpellChecker, m_pSpellHelper);
    m_FunctorId = EditorHooks::RegisterHook( m_pOnlineChecker );
    m_pOnlineChecker->EnableOnlineChecks( m_sccfg->GetEnableOnlineChecker() );

    // initialize thesaurus
    m_pThesaurus = new Thesaurus(Manager::Get()->GetAppFrame());
    ConfigureThesaurus();



    // connect events
    Connect(idSpellCheck,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SpellCheckerPlugin::OnSpelling));
    Connect(idSpellCheck,      wxEVT_UPDATE_UI,             wxUpdateUIEventHandler(SpellCheckerPlugin::OnUpdateSpelling));
    for ( unsigned int i = 0 ; i < MaxSuggestEntries ; i++ )
        Connect(idSuggest[i],  wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SpellCheckerPlugin::OnReplaceBySuggestion), NULL, this);
    Connect(idMoreSuggestions, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SpellCheckerPlugin::OnMoreSuggestions));
    Connect(idAddToDictionary, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SpellCheckerPlugin::OnAddToPersonalDictionary), NULL, this);
    Connect(idThesaurus,       wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SpellCheckerPlugin::OnThesaurus));
    Connect(idThesaurus,       wxEVT_UPDATE_UI,             wxUpdateUIEventHandler(SpellCheckerPlugin::OnUpdateThesaurus));
    Connect(idCamelCase,       wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SpellCheckerPlugin::OnCamelCase));

    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_SAVE, new cbEventFunctor<SpellCheckerPlugin, CodeBlocksEvent>(this, &SpellCheckerPlugin::OnEditorSaved));
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_TOOLTIP, new cbEventFunctor<SpellCheckerPlugin, CodeBlocksEvent>(this, &SpellCheckerPlugin::OnEditorTooltip));
}
#ifdef wxUSE_STATUSBAR
void SpellCheckerPlugin::CreateStatusField(cbStatusBar *bar)
{
    m_fld = new SpellCheckerStatusField(bar, this, m_sccfg);
    bar->AddField(this, m_fld, 60);
}
#endif
void SpellCheckerPlugin::ConfigureThesaurus()
{
    m_pThesaurus->SetFiles(
        m_sccfg->GetThesaurusPath() + wxFILE_SEP_PATH + _T("th_") + m_sccfg->GetDictionaryName() + _T(".idx"),
        m_sccfg->GetThesaurusPath() + wxFILE_SEP_PATH + _T("th_") + m_sccfg->GetDictionaryName() + _T(".dat")
    );
}
wxString SpellCheckerPlugin::GetOnlineCheckerConfigPath()
{
    return ConfigManager::GetDataFolder() + wxFileName::GetPathSeparator() + _T("SpellChecker") ;
}
void SpellCheckerPlugin::ConfigureHunspellSpellCheckEngine()
{
    SpellCheckEngineOption DictionaryFileOption(
        _T("dict-file"), _T("Dictionary File"),
        m_sccfg->GetDictionaryPath() + wxFILE_SEP_PATH + m_sccfg->GetDictionaryName() + _T(".dic"), SpellCheckEngineOption::FILE
    );
    m_pSpellChecker->AddOptionToMap(DictionaryFileOption);
    SpellCheckEngineOption AffixFileOption(
        _T("affix-file"), _T("Affix File"),
        m_sccfg->GetDictionaryPath() + wxFILE_SEP_PATH + m_sccfg->GetDictionaryName() + _T(".aff"), SpellCheckEngineOption::FILE
    );
    m_pSpellChecker->AddOptionToMap(AffixFileOption);
    m_pSpellChecker->ApplyOptions();

    ConfigurePersonalDictionary();
}
void SpellCheckerPlugin::ConfigurePersonalDictionary()
{
    // Set the personal dictionary file
    HunspellInterface *hsi = dynamic_cast<HunspellInterface *>(m_pSpellChecker);
    if (hsi)
        hsi->OpenPersonalDictionary(m_sccfg->GetPersonalDictionaryFilename());
}

void SpellCheckerPlugin::OnRelease(bool appShutDown)
{
    // do de-initialization for your plugin
    // if appShutDown is true, the plugin is unloaded because Code::Blocks is being shut down,
    // which means you must not use any of the SDK Managers
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be FALSE...
    EditorHooks::UnregisterHook(m_FunctorId);

    SavePersonalDictionary();

    m_pSpellChecker->UninitializeSpellCheckEngine();
    delete m_pSpellChecker;
    m_pSpellChecker = NULL;
    //delete m_pSpellingDialog; // gets deleted in wxSpellCheckEngineInterface::~wxSpellCheckEngineInterface()
    m_pSpellingDialog = NULL;
    delete m_pSpellHelper;
    m_pSpellHelper = NULL;
    //delete m_pOnlineChecker;
    m_pOnlineChecker = NULL;

    delete m_pThesaurus;
    m_pThesaurus = NULL;

    delete m_sccfg;
    m_sccfg = NULL;


    Disconnect(idSpellCheck, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SpellCheckerPlugin::OnSpelling));
    Disconnect(idSpellCheck, wxEVT_UPDATE_UI,             wxUpdateUIEventHandler(SpellCheckerPlugin::OnUpdateSpelling) );
    for ( unsigned int i = 0 ; i < MaxSuggestEntries ; i++ )
        Disconnect(idSuggest[i], wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SpellCheckerPlugin::OnReplaceBySuggestion), NULL, this);
    Disconnect(idMoreSuggestions, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SpellCheckerPlugin::OnMoreSuggestions));
    Disconnect(idAddToDictionary, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SpellCheckerPlugin::OnAddToPersonalDictionary), NULL, this);
    Disconnect(idThesaurus,  wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SpellCheckerPlugin::OnThesaurus));
    Disconnect(idThesaurus,  wxEVT_UPDATE_UI,             wxUpdateUIEventHandler(SpellCheckerPlugin::OnUpdateThesaurus));
    Disconnect(idCamelCase,  wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SpellCheckerPlugin::OnCamelCase));
}
void SpellCheckerPlugin::SavePersonalDictionary()
{
    HunspellInterface *hsi = dynamic_cast<HunspellInterface *>(m_pSpellChecker);
    if (hsi) hsi->GetPersonalDictionary()->SavePersonalDictionary();
}
int SpellCheckerPlugin::Configure()
{
    //create and display the configuration dialog for your plugin
    cbConfigurationDialog dlg(Manager::Get()->GetAppWindow(), wxID_ANY, _("Your dialog title"));
    cbConfigurationPanel* panel = GetConfigurationPanel(&dlg);
    if (panel)
    {
        dlg.AttachConfigurationPanel(panel);
        PlaceWindow(&dlg);
        return dlg.ShowModal() == wxID_OK ? 0 : -1;
    }
    return -1;
}
void SpellCheckerPlugin::BuildMenu(wxMenuBar* menuBar)
{
    //NOTE: Be careful in here... The application's menubar is at your disposal.
    // if not attached, exit
    if (!IsAttached()) return;

    // insert entry in the Edit menu
    int EditPos = menuBar->FindMenu(_("&Edit"));
    if (EditPos != wxNOT_FOUND)
    {
        // just append
        wxMenu* EditMenu = menuBar->GetMenu(EditPos);
        EditMenu->AppendSeparator();
        EditMenu->Append(idSpellCheck, _("Spelling..."), _("Spell check the selected text"));
        EditMenu->Append(idThesaurus,  _("Thesaurus..."), _T(""));

        // find menu - Edit/Special commands/Case
        int id = EditMenu->FindItem(_("Special commands"));
        if (id == wxNOT_FOUND) return;
        wxMenuItem* subMenuItem = EditMenu->FindItem(id, 0);
        if (!subMenuItem)      return;
        wxMenu* subMenu = subMenuItem->GetSubMenu();
        if (!subMenu)          return;
        id = EditMenu->FindItem(_("Case"));
        if (id == wxNOT_FOUND) return;
        subMenuItem = EditMenu->FindItem(id, 0);
        if (!subMenuItem)      return;
        subMenu = subMenuItem->GetSubMenu();
        if (!subMenu)          return;
        // and append
        subMenu->Append(idCamelCase, _("CamelCase"), _("Make selection CamelCase"));
    }
}
void SpellCheckerPlugin::BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data)
{
    //Some library module is ready to display a pop-up menu.
    //Check the parameter \"type\" and see which module it is
    //and append any items you need in the menu...
    //TIP: for consistency, add a separator as the first item...
    if ( !IsAttached() ) return;
    if(type != mtEditorManager || !menu ) return;
    cbEditor *ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if ( !ed ) return;
    cbStyledTextCtrl *stc = ed->GetControl();
    if ( !stc ) return;

    const int id = menu->FindItem(_("Edit"));
    if (id != wxNOT_FOUND)
    {
        wxMenuItem* subMenuItem = menu->FindItem(id, 0);
        wxMenu* subMenu;
        if (subMenuItem)
            subMenu = subMenuItem->GetSubMenu();
        if (subMenu)
        {
            int insertPos = wxNOT_FOUND;
            const wxMenuItemList itemsList = subMenu->GetMenuItems();
            for (size_t i = 0; i < itemsList.GetCount(); ++i)
            {
                #if wxCHECK_VERSION(2, 9, 0)
                if (itemsList[i]->GetItemLabelText() == _("lowercase"))
                #else
                if (itemsList[i]->GetLabel() == _("lowercase"))
                #endif
                {
                    insertPos = i + 1;
                    break;
                }
            }
            if (insertPos != wxNOT_FOUND)
                subMenu->Insert(insertPos, idCamelCase, _("CamelCase"));
            else
                subMenu->Append(idCamelCase, _("CamelCase"));
        }
    }

    int pos = stc->GetCurrentPos();
    stc->GetIndicatorValue();
    m_wordstart= -1;
    m_wordend = -1;
    m_suggestions.Empty();
    //Manager::Get()->GetLogManager()->Log( wxString::Format(_T("SpellChecker indicator: %d"), indic) );
    if ( !stc->GetSelectedText().IsEmpty() )
    {
        menu->AppendSeparator();
        menu->Append(idSpellCheck, _T("Spelling..."));
    }
    else if ( stc->IndicatorValueAt( m_pOnlineChecker->GetIndicator(), pos) )
    {
        // indicator is on -> check if we can find a suggestion or show that there are no suggestions
        menu->AppendSeparator();
        wxString misspelledWord;

        int wordstart = pos, wordend = pos;
        while ( wordstart )
        {
            if ( m_pSpellHelper->IsWhiteSpace( stc->GetCharAt(wordstart-1) ) )
                break;
            wordstart--;
        }
        while ( wordend < stc->GetLength() )
        {
            if ( m_pSpellHelper->IsWhiteSpace( stc->GetCharAt(++wordend) ) )
                break;
        }
        misspelledWord = stc->GetTextRange(wordstart, wordend);
        m_wordstart = wordstart;
        m_wordend   = wordend;

        m_suggestions = m_pSpellChecker->GetSuggestions( misspelledWord );
        if ( m_suggestions.size() )
        {
            wxMenu *SuggestionsMenu = new wxMenu();
            for ( unsigned int i = 0 ; i < MaxSuggestEntries && i < m_suggestions.size() ; i++ )
                SuggestionsMenu->Append(idSuggest[i], m_suggestions[i] );
            SuggestionsMenu->AppendSeparator();
            if ( m_suggestions.size() > MaxSuggestEntries )
                SuggestionsMenu->Append(idMoreSuggestions, _("more..."));
            SuggestionsMenu->Append(idAddToDictionary, _("Add '") + misspelledWord + _("' to dictionary"));
            menu->AppendSubMenu(SuggestionsMenu, _("Spelling suggestions for '") + misspelledWord + _T("'") );
        }
        else
        {
            //menu->Append(idMoreSuggestions, _("No spelling suggestions for '") + misspelledWord + _T("'"))->Enable(false);
            menu->Append(idAddToDictionary, _("Add '") + misspelledWord + _("' to dictionary"));
        }
    }
}

bool SpellCheckerPlugin::BuildToolBar(wxToolBar* toolBar)
{
    //The application is offering its toolbar for your plugin,
    //to add any toolbar items you want...
    //Append any items you need on the toolbar...
    //NotImplemented(_T("SpellChecker::BuildToolBar()"));

    // return true if you add toolbar items
    return false;
}

void SpellCheckerPlugin::OnSpelling(wxCommandEvent &event)
{
    cbEditor *ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if ( !ed ) return;
    cbStyledTextCtrl *stc = ed->GetControl();
    if ( !stc ) return;
    if ( m_pSpellingDialog )
        PlaceWindow(m_pSpellingDialog, pdlBest, true);
    stc->ReplaceSelection(m_pSpellChecker->CheckSpelling(stc->GetSelectedText()));
}
void SpellCheckerPlugin::OnUpdateSpelling(wxUpdateUIEvent &event)
{
    if ( ActiveEditorHasTextSelected() )
        event.Enable(true);
    else
        event.Enable(false);
}
void SpellCheckerPlugin::OnThesaurus(wxCommandEvent &event)
{
    cbEditor *ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if ( !ed ) return;
    cbStyledTextCtrl *stc = ed->GetControl();
    if ( !stc ) return;


    // take only the first word from the selection
    int selstart = stc->GetSelectionStart();
    while ( selstart < stc->GetLength() )
    {
        if ( !m_pSpellHelper->IsWhiteSpace( stc->GetCharAt(selstart) ))
            break;
        selstart++;
    }
    int selend = selstart;
    while ( selend < stc->GetLength() )
    {
        if ( m_pSpellHelper->IsWhiteSpace( stc->GetCharAt(++selend) ) )
            break;
    }

    wxString word = stc->GetTextRange(selstart, selend);
    if ( word.IsEmpty() ) return;

    wxString Synonym;
    bool hasEntry = m_pThesaurus->GetSynonym(word, Synonym);
    if ( hasEntry )
    {
        if ( !Synonym.IsEmpty() )
        {
            stc->SetSelectionVoid(selstart, selend);
            stc->ReplaceSelection(Synonym);
        }
    }
    else
    {
        AnnoyingDialog dlg(_T("Thesaurus"), _T("No entry found!"), wxART_INFORMATION, AnnoyingDialog::OK);
        dlg.ShowModal();
    }
}
void SpellCheckerPlugin::OnUpdateThesaurus(wxUpdateUIEvent &event)
{
    if ( ActiveEditorHasTextSelected() && m_pThesaurus->IsOk() )
        event.Enable(true);
    else
        event.Enable(false);
}
void SpellCheckerPlugin::OnCamelCase(wxCommandEvent &event)
{
    cbEditor *ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed || !m_pSpellChecker->IsInitialized()) return;
    cbStyledTextCtrl *stc = ed->GetControl();
    if (!stc) return;

    // take only the first word from the selection
    int selstart = stc->GetSelectionStart();
    while (selstart < stc->GetLength())
    {
        if ( !m_pSpellHelper->IsWhiteSpace(stc->GetCharAt(selstart)) )
            break;
        ++selstart;
    }
    // and scan back for the actual word start
    while (selstart > 0)
    {
        if ( m_pSpellHelper->IsWhiteSpace(stc->GetCharAt(selstart - 1)) )
            break;
        --selstart;
    }
    if (selstart > stc->GetSelectionEnd())
        return;
    int selend = selstart;
    while (selend < stc->GetLength())
    {
        if ( m_pSpellHelper->IsWhiteSpace(stc->GetCharAt(++selend)) )
            break;
    }

    if (selend - selstart < 4) // too small
        return;
    else if (selend - selstart > 30) // max limit (DoGetWordStarts() is recursive, so watch out)
        selend = selstart + 30;

    wxString text = stc->GetTextRange(selstart, selend);
    wxArrayString prefixes = GetArrayFromString(wxT("Get;Set;Do;On;Is;wx"));
    prefixes.Add(wxEmptyString); // check without prefix
    prefixes.Add(wxT("p")); // less common prefix, check last
    for (size_t i = 0; i < prefixes.GetCount(); ++i)
    {
        wxString word = text;
        if (!prefixes[i].IsEmpty()) // try with prefix
        {
            if (!text.Lower().StartsWith(prefixes[i].Lower(), &word))
                continue; // no, try next prefix
        }
        wxString camelWord;
        for (size_t j = 0; j < word.Length() / 2; ++j) // 0 index number of words to break into
        {
            wxArrayInt wordStarts;
            if (DoGetWordStarts(word.Lower(), wordStarts, j))
            {
                for (size_t k = 0; k < word.Length(); ++k) // make CamelCase
                {
                    if (wordStarts.Index(k) == wxNOT_FOUND)
                        camelWord << word.Lower()[k];
                    else
                        camelWord << word.Upper()[k];
                }
                break;
            }
        }
        if (!camelWord.IsEmpty())
        {
            if (i != prefixes.GetCount())
                camelWord.Prepend(prefixes[i]);
            if (text == camelWord)
                return; // already formed, so do nothing
            stc->BeginUndoAction();
            stc->DeleteRange(selstart, text.Length());
            stc->InsertText(selstart, camelWord);
            stc->SetSelectionStart(selstart);
            stc->SetSelectionEnd(selend);
            stc->EndUndoAction();
            return; // exit
        }
    }
}
bool SpellCheckerPlugin::DoGetWordStarts(const wxString& word, wxArrayInt& wordStarts, int numWords)
{
    if (numWords <= 0) // finish split
    {
        wordStarts.Add(0); // first word
        wxString currWord;
        for (int i = wordStarts.GetCount() - 1; i > 0; --i) // reverse iteration (so numbers are checked lowest to highest)
        {
            currWord = word(wordStarts[i], wordStarts[i - 1] - wordStarts[i]);
            if (currWord.Length() > 3) // capitalize medium/long words so proper nouns work
                currWord = currWord(0, 1).Upper() + currWord.Mid(1);
            if (!m_pSpellChecker->IsWordInDictionary(currWord))
            {
                wordStarts.RemoveAt(wordStarts.GetCount() - 1);
                return false; // no, fall back a level
            }
        }
        currWord = word.Mid(wordStarts[0]);
        if (currWord.Length() > 3) // capitalize
            currWord = currWord(0, 1).Upper() + currWord.Mid(1);
        if (!m_pSpellChecker->IsWordInDictionary(currWord)) // last word (wordStarts[] is reverse sorted)
        {
            wordStarts.RemoveAt(wordStarts.GetCount() - 1);
            return false; // no, fall back a level
        }
        return true; // all parts are correctly spelled
    }

    // iterate through possibilities of the current word start
    for (int i = (wordStarts.IsEmpty() ? word.Length() : wordStarts[wordStarts.GetCount() - 1]) - 2;
         i >= numWords * 2; --i)
    {
        wordStarts.Add(i);
        if (DoGetWordStarts(word, wordStarts, numWords - 1))
        {
            return true; // yes, fall through and return
        }
        wordStarts.RemoveAt(wordStarts.GetCount() - 1);
    }
    return false; // no, fall back an iteration
}

bool SpellCheckerPlugin::ActiveEditorHasTextSelected(void)
{
    cbEditor *ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if ( ed )
    {
        cbStyledTextCtrl *stc = ed->GetControl();
        if ( stc )
        {
            wxString str = stc->GetSelectedText();
            if ( !str.IsEmpty() )
                return true;
        }
    }
    return false;
}

void SpellCheckerPlugin::OnReplaceBySuggestion(wxCommandEvent &event)
{
    if ( m_wordstart == -1 || m_wordend == -1 ) return;
    cbEditor *ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if ( ed )
    {
        cbStyledTextCtrl *stc = ed->GetControl();
        if ( stc )
        {
            for ( unsigned int i = 0 ; i < MaxSuggestEntries ; i++)
            {
                if ( idSuggest[i] == event.GetId() )
                {
                    stc->SetAnchor(m_wordstart);
                    stc->SetCurrentPos(m_wordend);
                    stc->ReplaceSelection(m_suggestions[i]);
                    break;
                }
            }
        }
    }

    m_wordend = -1;
    m_wordstart = -1;
    m_suggestions.Empty();
}
void SpellCheckerPlugin::OnMoreSuggestions(wxCommandEvent &event)
{
    if ( m_wordstart == -1 || m_wordend == -1 ) return;
    cbEditor *ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if ( ed )
    {
        cbStyledTextCtrl *stc = ed->GetControl();
        if ( stc )
        {
            stc->SetAnchor(m_wordstart);
            stc->SetCurrentPos(m_wordend);
            if ( m_pSpellingDialog )
                PlaceWindow(m_pSpellingDialog, pdlBest, true);
            stc->ReplaceSelection(m_pSpellChecker->CheckSpelling(stc->GetSelectedText()));
        }
    }

    m_wordend = -1;
    m_wordstart = -1;
    m_suggestions.Empty();
}

void SpellCheckerPlugin::ReloadSettings()
{
    SavePersonalDictionary();
    ConfigureHunspellSpellCheckEngine();
    m_pOnlineChecker->EnableOnlineChecks(m_sccfg->GetEnableOnlineChecker());
    ConfigureThesaurus();
#ifdef wxUSE_STATUSBAR
    if (m_fld)
        m_fld->Update();
#endif
}


cbConfigurationPanel *SpellCheckerPlugin::GetConfigurationPanel(wxWindow* parent)
{
    return new SpellCheckSettingsPanel(parent, m_sccfg);
}

void SpellCheckerPlugin::OnAddToPersonalDictionary(wxCommandEvent &event)
{
    if ( m_wordstart == -1 || m_wordend == -1 ) return;
    cbEditor *ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if ( ed )
    {
        cbStyledTextCtrl *stc = ed->GetControl();
        if ( stc )
        {
            stc->SetAnchor(m_wordstart);
            stc->SetCurrentPos(m_wordend);
            m_pSpellChecker->AddWordToDictionary(stc->GetSelectedText());
        }
    }
    m_wordend = -1;
    m_wordstart = -1;
    m_suggestions.Empty();

    if ( ed )
    {
        m_pOnlineChecker->OnEditorChange(ed);
        m_pOnlineChecker->DoSetIndications(ed);
    }
}
void SpellCheckerPlugin::EditPersonalDictionary()
{
    SavePersonalDictionary();
    Manager::Get()->GetEditorManager()->Open(m_sccfg->GetPersonalDictionaryFilename());
}
void SpellCheckerPlugin::OnEditorSaved(CodeBlocksEvent& event)
{
    EditorBase *eb = event.GetEditor();
    if ( !eb )
        return;

    // the personal dictionary (with the current language) gets saved ->
    // reload its content into SpellChecker
    if ( eb->GetFilename() == m_sccfg->GetPersonalDictionaryFilename() )
    {
        ConfigurePersonalDictionary();

        // redo on line checks:
        m_pOnlineChecker->EnableOnlineChecks(m_sccfg->GetEnableOnlineChecker());
    }


}

void SpellCheckerPlugin::OnEditorTooltip(CodeBlocksEvent& event)
{
    if (   !IsAttached() || wxGetKeyState(WXK_CONTROL)
        || !(m_sccfg->GetEnableSpellTooltips() || m_sccfg->GetEnableThesaurusTooltips()))
    {
        event.Skip();
        return;
    }
    EditorBase* base = event.GetEditor();
    cbEditor* ed = base && base->IsBuiltinEditor() ? static_cast<cbEditor*>(base) : 0;
    if (   !ed || ed->IsContextMenuOpened()
        || wxWindow::FindFocus() != static_cast<wxWindow*>(ed->GetControl()) )
    {
        event.Skip();
        return;
    }
    cbStyledTextCtrl* stc = ed->GetControl();
    if (!stc)
        return;
    int pos = stc->PositionFromPointClose(event.GetX(), event.GetY());
    if (pos < 0 || pos >= stc->GetLength())
    {
        event.Skip();
        return;
    }

    wxString tip;
    int wordstart = pos, wordend = pos;
    while (wordstart)
    {
        if ( m_pSpellHelper->IsWhiteSpace( stc->GetCharAt(wordstart - 1) ) )
            break;
        --wordstart;
    }
    while ( wordend < stc->GetLength() )
    {
        if ( m_pSpellHelper->IsWhiteSpace( stc->GetCharAt(++wordend) ) )
            break;
    }
    int tipWidth = 0;
    if (   m_sccfg->GetEnableSpellTooltips()
        && m_pSpellChecker->IsInitialized()
        && stc->IndicatorValueAt(m_pOnlineChecker->GetIndicator(), pos))
    {
        // indicator is on -> check if we can find a suggestion
        wxString misspelledWord = stc->GetTextRange(wordstart, wordend);
        m_suggestions = m_pSpellChecker->GetSuggestions(misspelledWord);
        if (!m_suggestions.IsEmpty())
        {
            // allow maximum 12 entries in 3 rows
            int lineWidth = 0;
            for (size_t i = 0; i < 12 && i < m_suggestions.size(); ++i)
            {
                tip << m_suggestions[i];
                lineWidth += m_suggestions[i].Length();
                if (i % 4 == 3)
                {
                    tip << wxT(",\n");
                    if (lineWidth > tipWidth)
                        tipWidth = lineWidth;
                    lineWidth = 0;
                }
                else
                {
                    tip << wxT(", ");
                    lineWidth += 2;
                }
            }
            tip.RemoveLast(2);
            lineWidth -= 2;
            if (lineWidth > tipWidth) // in case the last line was not full, and thereby not checked
                tipWidth = lineWidth;
        }
    }
    else if (   m_sccfg->GetEnableThesaurusTooltips()
             && m_pThesaurus->IsOk()
             && m_pSpellHelper->HasStyleToBeChecked(ed->GetColourSet()->GetLanguageName(ed->GetLanguage()), event.GetInt()))
    {
        wxString word = stc->GetTextRange(wordstart, wordend);
        synonyms syn = m_pThesaurus->GetSynonyms(word);
        if (!syn.size()) // if not found, try lower case
            syn = m_pThesaurus->GetSynonyms(word.Lower());
        if (syn.size())
        {
            wxArrayString usedSyns; // avoid duplicate synonyms
            // allow maximum 12 entries in 4 rows
            synonyms::iterator it = syn.begin();
            for (size_t i = 0; i < 4 && it != syn.end(); ++i, ++it)
            {
                wxString tipLine(it->first + wxT(": "));
                std::vector< wxString > syns = syn[it->first];
                size_t j = 0;
                for (size_t k = 0; k < 3 && j < syns.size(); ++j, ++k)
                {
                    if (usedSyns.Index(syns[j]) == wxNOT_FOUND)
                    {
                        tipLine << syns[j] << wxT(", ");
                        usedSyns.Add(syns[j]);
                    }
                    else
                        --k; // synonym already listed, look for another word
                }
                tipLine.RemoveLast(2);
                if (tipLine.Length() > static_cast<size_t>(tipWidth))
                    tipWidth = tipLine.Length();
                tip << tipLine << wxT("\n");
            }
            tip.RemoveLast();
        }
    }

    if (tip.IsEmpty())
    {
        event.Skip();
        return;
    }

    if (stc->CallTipActive())
        stc->CallTipCancel();
    // calculation from CC
    const int lnStart = stc->PositionFromLine(stc->LineFromPosition(pos));
                  // pos - lnStart   == distance from start of line
                  //  + tipWidth + 1 == projected virtual position of tip end (with a 1 character buffer) from start of line
                  //  - (width_of_editor_in_pixels / width_of_character) == distance tip extends past window edge
                  //       horizontal scrolling is accounted for by PointFromPosition().x
    const int offset = tipWidth + pos + 1 - lnStart -
                       (stc->GetSize().x - stc->PointFromPosition(lnStart).x) /
                       stc->TextWidth(wxSCI_STYLE_LINENUMBER, _T("W"));
    if (offset > 0)
        pos -= offset;
    if (pos < lnStart) // do not go to previous line if tip is wider than editor
        pos = lnStart;

    stc->CallTipShow(pos, tip);
    event.SetExtraLong(1); // notify CC not to cancel this tooltip
    event.Skip();
}

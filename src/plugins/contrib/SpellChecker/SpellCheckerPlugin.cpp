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
    #include <configmanager.h>
    #include <cbeditor.h>
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

    const unsigned int MaxSuggestEntries = 5;
    const int idSuggest[MaxSuggestEntries] =
        {wxNewId(), wxNewId(), wxNewId(), wxNewId(), wxNewId()};
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

    // initialze Helper and online checker
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

    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_SAVE, new cbEventFunctor<SpellCheckerPlugin, CodeBlocksEvent>(this, &SpellCheckerPlugin::OnEditorSaved));
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
    Manager::Get()->GetLogManager()->DebugLogError(F(_T("in %s::%s:%d"), cbC2U(__FILE__).c_str(),cbC2U(__PRETTY_FUNCTION__).c_str(), __LINE__));
    EditorHooks::UnregisterHook(m_FunctorId);

    Manager::Get()->GetLogManager()->DebugLogError(F(_T("in %s::%s:%d"), cbC2U(__FILE__).c_str(),cbC2U(__PRETTY_FUNCTION__).c_str(), __LINE__));
    SavePersonalDictionary();

    Manager::Get()->GetLogManager()->DebugLogError(F(_T("in %s::%s:%d"), cbC2U(__FILE__).c_str(),cbC2U(__PRETTY_FUNCTION__).c_str(), __LINE__));
    m_pSpellChecker->UninitializeSpellCheckEngine();
    Manager::Get()->GetLogManager()->DebugLogError(F(_T("in %s::%s:%d"), cbC2U(__FILE__).c_str(),cbC2U(__PRETTY_FUNCTION__).c_str(), __LINE__));
    delete m_pSpellChecker;
    Manager::Get()->GetLogManager()->DebugLogError(F(_T("in %s::%s:%d"), cbC2U(__FILE__).c_str(),cbC2U(__PRETTY_FUNCTION__).c_str(), __LINE__));
    m_pSpellChecker = NULL;
    Manager::Get()->GetLogManager()->DebugLogError(F(_T("in %s::%s:%d"), cbC2U(__FILE__).c_str(),cbC2U(__PRETTY_FUNCTION__).c_str(), __LINE__));
    delete m_pSpellHelper;
    Manager::Get()->GetLogManager()->DebugLogError(F(_T("in %s::%s:%d"), cbC2U(__FILE__).c_str(),cbC2U(__PRETTY_FUNCTION__).c_str(), __LINE__));
    m_pSpellHelper = NULL;
    //delete m_pOnlineChecker;
    Manager::Get()->GetLogManager()->DebugLogError(F(_T("in %s::%s:%d"), cbC2U(__FILE__).c_str(),cbC2U(__PRETTY_FUNCTION__).c_str(), __LINE__));
    m_pOnlineChecker = NULL;

    Manager::Get()->GetLogManager()->DebugLogError(F(_T("in %s::%s:%d"), cbC2U(__FILE__).c_str(),cbC2U(__PRETTY_FUNCTION__).c_str(), __LINE__));
    if ( m_pThesaurus )
    {
        Manager::Get()->GetLogManager()->DebugLogError(F(_T("in %s::%s:%d"), cbC2U(__FILE__).c_str(),cbC2U(__PRETTY_FUNCTION__).c_str(), __LINE__));
        delete m_pThesaurus;
    }
    Manager::Get()->GetLogManager()->DebugLogError(F(_T("in %s::%s:%d"), cbC2U(__FILE__).c_str(),cbC2U(__PRETTY_FUNCTION__).c_str(), __LINE__));
    m_pThesaurus = NULL;
    Manager::Get()->GetLogManager()->DebugLogError(F(_T("in %s::%s:%d"), cbC2U(__FILE__).c_str(),cbC2U(__PRETTY_FUNCTION__).c_str(), __LINE__));

    Manager::Get()->GetLogManager()->DebugLogError(F(_T("in %s::%s:%d"), cbC2U(__FILE__).c_str(),cbC2U(__PRETTY_FUNCTION__).c_str(), __LINE__));
    if ( m_sccfg )
    {
        Manager::Get()->GetLogManager()->DebugLogError(F(_T("in %s::%s:%d"), cbC2U(__FILE__).c_str(),cbC2U(__PRETTY_FUNCTION__).c_str(), __LINE__));
        delete m_sccfg;
    }
    m_sccfg = NULL;
    Manager::Get()->GetLogManager()->DebugLogError(F(_T("in %s::%s:%d"), cbC2U(__FILE__).c_str(),cbC2U(__PRETTY_FUNCTION__).c_str(), __LINE__));

    Manager::Get()->GetLogManager()->DebugLogError(F(_T("in %s::%s:%d"), cbC2U(__FILE__).c_str(),cbC2U(__PRETTY_FUNCTION__).c_str(), __LINE__));

    Disconnect(idSpellCheck, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SpellCheckerPlugin::OnSpelling));
    Manager::Get()->GetLogManager()->DebugLogError(F(_T("in %s::%s:%d"), cbC2U(__FILE__).c_str(),cbC2U(__PRETTY_FUNCTION__).c_str(), __LINE__));
    Disconnect(idSpellCheck, wxEVT_UPDATE_UI,             wxUpdateUIEventHandler(SpellCheckerPlugin::OnUpdateSpelling) );
    Manager::Get()->GetLogManager()->DebugLogError(F(_T("in %s::%s:%d"), cbC2U(__FILE__).c_str(),cbC2U(__PRETTY_FUNCTION__).c_str(), __LINE__));
    for ( unsigned int i = 0 ; i < MaxSuggestEntries ; i++ )
    {
        Manager::Get()->GetLogManager()->DebugLogError(F(_T("in %s::%s:%d"), cbC2U(__FILE__).c_str(),cbC2U(__PRETTY_FUNCTION__).c_str(), __LINE__));
        Disconnect(idSuggest[i], wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SpellCheckerPlugin::OnReplaceBySuggestion), NULL, this);
    }
    Manager::Get()->GetLogManager()->DebugLogError(F(_T("in %s::%s:%d"), cbC2U(__FILE__).c_str(),cbC2U(__PRETTY_FUNCTION__).c_str(), __LINE__));
    Disconnect(idMoreSuggestions, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SpellCheckerPlugin::OnMoreSuggestions));
    Manager::Get()->GetLogManager()->DebugLogError(F(_T("in %s::%s:%d"), cbC2U(__FILE__).c_str(),cbC2U(__PRETTY_FUNCTION__).c_str(), __LINE__));
    Disconnect(idAddToDictionary, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SpellCheckerPlugin::OnAddToPersonalDictionary), NULL, this);
    Manager::Get()->GetLogManager()->DebugLogError(F(_T("in %s::%s:%d"), cbC2U(__FILE__).c_str(),cbC2U(__PRETTY_FUNCTION__).c_str(), __LINE__));
    Manager::Get()->GetLogManager()->DebugLogError(F(_T("in %s::%s:%d"), cbC2U(__FILE__).c_str(),cbC2U(__PRETTY_FUNCTION__).c_str(), __LINE__));
    Disconnect(idThesaurus,  wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SpellCheckerPlugin::OnThesaurus));
    Manager::Get()->GetLogManager()->DebugLogError(F(_T("in %s::%s:%d"), cbC2U(__FILE__).c_str(),cbC2U(__PRETTY_FUNCTION__).c_str(), __LINE__));
    Disconnect(idThesaurus,  wxEVT_UPDATE_UI,             wxUpdateUIEventHandler(SpellCheckerPlugin::OnUpdateThesaurus));
    Manager::Get()->GetLogManager()->DebugLogError(F(_T("in %s::%s:%d"), cbC2U(__FILE__).c_str(),cbC2U(__PRETTY_FUNCTION__).c_str(), __LINE__));

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

    // insert entry in the View menu
    int EditPos = menuBar->FindMenu(_("&Edit"));
    if (EditPos != wxNOT_FOUND)
    {
        // just append
        wxMenu* EditMenu = menuBar->GetMenu(EditPos);
        EditMenu->AppendSeparator();
        EditMenu->Append(idSpellCheck, _T("Spelling..."), _T("Spell check the selected text"));
        EditMenu->Append(idThesaurus,  _T("Thesaurus..."), _T(""));
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
            SuggestionsMenu->Append(idAddToDictionary, _T("Add '") + misspelledWord + _T("' to dictionary"));
            menu->AppendSubMenu(SuggestionsMenu, _("Spelling suggestions for '") + misspelledWord + _T("'") );
        }
        else
        {
            //menu->Append(idMoreSuggestions, _T("No spelling suggestions for '") + misspelledWord + _T("'"))->Enable(false);
            menu->Append(idAddToDictionary, _T("Add '") + misspelledWord + _T("' to dictionary"));
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
    PlaceWindow(m_pSpellingDialog, pdlBest, true);
    stc->ReplaceSelection(m_pSpellChecker->CheckSpelling(stc->GetSelectedText()));
}
void SpellCheckerPlugin::OnUpdateSpelling(wxUpdateUIEvent &event)
{
    cbEditor *ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if ( ed )
    {
        cbStyledTextCtrl *stc = ed->GetControl();
        if ( stc )
        {
            wxString str = stc->GetSelectedText();
            if ( !str.IsEmpty() )
            {
                event.Enable(true);
                return;
            }
        }
    }
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
    cbEditor *ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if ( ed && m_pThesaurus->IsOk() )
    {
        cbStyledTextCtrl *stc = ed->GetControl();
        if ( stc )
        {
            wxString str = stc->GetSelectedText();
            if ( !str.IsEmpty() )
            {
                event.Enable(true);
                return;
            }
        }
    }
    event.Enable(false);
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

    // the personal dictionary (with the current language gets saved ->
    // reload its content into SpellChecker
    if ( eb->GetFilename() == m_sccfg->GetPersonalDictionaryFilename() )
    {
        ConfigurePersonalDictionary();

        // redo on line checks:
        m_pOnlineChecker->EnableOnlineChecks(m_sccfg->GetEnableOnlineChecker());
    }


}

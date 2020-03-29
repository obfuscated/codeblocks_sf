// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/filename.h"
#include "wx/config.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows and OS/2 it is in resources)
#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)
    #include "mondrian.xpm"
#endif

#include "wxSpellCheckerDemo.h"

#include "MySpellCheckDialog.h"
#include "XmlSpellCheckDialog.h"
#include "XmlPersonalDictionaryDialog.h"
#include "AspellInterface.h"
#include "MySpellInterface.h"
#include "SpellCheckerOptionsDialog.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    FileNew = wxID_NEW,
    FileOpen = wxID_OPEN,
    FileSave = wxID_SAVE,
    FileQuit = wxID_EXIT,
    FileSpellCheckMozillaUI = 10000,
    FileSpellCheckOutlookUI = 10001,
    FileSpellCheckSentryUI = 10002,
    FileSpellCheckAbiwordUI = 10003,
    FileUseAspell = 10004,
    FileUseMySpell = 10005,
    EditOptions = 10006,
    EditPersonalDictionary = 10007,
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(FileNew, MyFrame::OnNew)
    EVT_MENU(FileOpen, MyFrame::OnOpen)
    EVT_MENU(FileSave, MyFrame::OnSave)
    EVT_MENU(FileQuit,  MyFrame::OnQuit)
    EVT_MENU(FileUseAspell, MyFrame::OnUseAspell)
    EVT_UPDATE_UI(FileUseAspell, MyFrame::OnUpdateUseAspell)
    EVT_MENU(FileUseMySpell, MyFrame::OnUseMySpell)
    EVT_UPDATE_UI(FileUseMySpell, MyFrame::OnUpdateUseMySpell)
    EVT_MENU(FileSpellCheckMozillaUI, MyFrame::OnSpellCheckMozillaUI)
    EVT_MENU(FileSpellCheckOutlookUI, MyFrame::OnSpellCheckOutlookUI)
    EVT_MENU(FileSpellCheckSentryUI, MyFrame::OnSpellCheckSentryUI)
    EVT_MENU(FileSpellCheckAbiwordUI, MyFrame::OnSpellCheckAbiwordUI)
    EVT_MENU(EditOptions, MyFrame::OnEditOptions)
    EVT_MENU(EditPersonalDictionary, MyFrame::OnEditPersonalDictionary)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // create the main application window
    MyFrame *frame = new MyFrame(_T("wxSpellChecker Demo"));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title)
{
    m_nSelectedSpellCheckEngine = MyFrame::USE_ASPELL;
    
    // set the frame icon
    SetIcon(wxICON(mondrian));

    textCtrl = new wxTextCtrl(this, -1, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    textCtrl->SetFocus();
    
    // create a menu bar
    wxMenu* menuFile = new wxMenu;

    menuFile->Append(FileNew, _T("New\tCtrl+N"), _T("Clear out the existing text"));
    menuFile->Append(FileOpen, _T("Open\tCtrl+O"), _T("Open an existing document"));
    menuFile->Append(FileSave, _T("Save\tCtrl+S"), _T("Save the cuurent document"));
    menuFile->AppendSeparator();
    menuFile->AppendCheckItem(FileUseAspell, _T("Use Aspell"), _T("Spell check using the Aspell engine"));
    menuFile->AppendCheckItem(FileUseMySpell, _T("Use MySpell"), _T("Spell check using the MySpell engine"));
    menuFile->AppendSeparator();
    menuFile->Append(FileSpellCheckMozillaUI, _T("SpellCheck Mozilla-style\tF5"), _T("SpellCheck the current document using the Mozilla spellchecker look-and-feel"));
    menuFile->Append(FileSpellCheckOutlookUI, _T("SpellCheck Outlook-style\tF6"), _T("SpellCheck the current document using the Outlook 97 spellchecker look-and-feel"));
    menuFile->Append(FileSpellCheckSentryUI, _T("SpellCheck Sentry-style\tF7"), _T("SpellCheck the current document using the Wintertree Sentry spellchecker look-and-feel"));
    menuFile->Append(FileSpellCheckAbiwordUI, _T("SpellCheck Abiword-style\tF8"), _T("SpellCheck the current document using the Abiword dialog interface"));
    menuFile->AppendSeparator();
    menuFile->Append(FileQuit, _T("E&xit\tCtrl+Q"), _T("Quit this program"));
    
    wxMenu* menuEdit = new wxMenu;
    menuEdit->Append(EditOptions, _T("&Options"), _T("Edit the options for the currently selected spell checking engine"));
    menuEdit->Append(EditPersonalDictionary, _T("Personal Dictionary\tCtrl+P"), _T("Edit the personal dictionary for the selected spell check engine"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(menuEdit, _T("&Edit"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    // create a status bar
    CreateStatusBar();
    SetStatusText(_T("Please type something in the text area or load a file to spell check"));
    
    m_bSpellCheckOnRightClick = false;

    m_pAspellInterface = new AspellInterface();
    SetDefaultAspellOptions();
    m_pMySpellInterface = new MySpellInterface();
    SetDefaultMySpellOptions();

    m_pMySpellInterface->InitializeSpellCheckEngine();
    m_pAspellInterface->InitializeSpellCheckEngine();
}



MyFrame::~MyFrame()
{
  SavePersonalDictionaries();

  wxDELETE(m_pAspellInterface);
  wxDELETE(m_pMySpellInterface);
}

// event handlers
void MyFrame::OnUseAspell(wxCommandEvent& event)
{
  m_nSelectedSpellCheckEngine = MyFrame::USE_ASPELL;
}

void MyFrame::OnUpdateUseAspell(wxUpdateUIEvent& event)
{
  event.Enable(m_pAspellInterface != NULL);
  event.Check(m_nSelectedSpellCheckEngine == MyFrame::USE_ASPELL);
}

void MyFrame::OnUseMySpell(wxCommandEvent& event)
{
  m_nSelectedSpellCheckEngine = MyFrame::USE_MYSPELL;
}

void MyFrame::OnUpdateUseMySpell(wxUpdateUIEvent& event)
{
  event.Enable(m_pMySpellInterface != NULL);
  event.Check(m_nSelectedSpellCheckEngine == MyFrame::USE_MYSPELL);
}

void MyFrame::OnNew(wxCommandEvent& WXUNUSED(event))
{
  if (textCtrl)
    textCtrl->Clear();
}

void MyFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{
  wxString filename = ::wxFileSelector(_T("Please select a file"));
  if (!filename.empty())
  {
    if (textCtrl)
      textCtrl->LoadFile(filename);
  }
}

void MyFrame::OnSave(wxCommandEvent& WXUNUSED(event))
{
  wxString filename = ::wxFileSelector(_T("Please select a filename to save as"), _T(""), _T(""), _T(""), _T("*.*"), wxSAVE|wxOVERWRITE_PROMPT);
  if (!filename.empty())
  {
    if (textCtrl)
      textCtrl->SaveFile(filename);
  }
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnSpellCheckMozillaUI(wxCommandEvent& event)
{
  wxSpellCheckEngineInterface* pSpellChecker = ReturnSelectedSpellCheckEngine();
  if (pSpellChecker)
  {
    if (pSpellChecker->IsInitialized())
    {
      pSpellChecker->SetSpellCheckUserInterface(new MySpellingDialog(NULL));
      SpellCheck(pSpellChecker);
    }
    else
    {
      ::wxMessageBox(wxString::Format(_("Current options prevent proper %s spell check engine initialization.  Spell check is not possible."), pSpellChecker->GetSpellCheckEngineName().c_str()));
    }
  }
}

void MyFrame::OnSpellCheckOutlookUI(wxCommandEvent& event)
{
  XmlSpellCheck(_T("OutlookLike"));
}

void MyFrame::OnSpellCheckSentryUI(wxCommandEvent& event)
{
  XmlSpellCheck(_T("SentryLike"));
}

void MyFrame::OnSpellCheckAbiwordUI(wxCommandEvent& event)
{
  XmlSpellCheck(_T("AbiwordLike"));
}

void MyFrame::OnEditOptions(wxCommandEvent& event)
{
  // Create a really basic dialog that gets dynamically populated
  // with controls based on the m_pSpellCheckEngine->GetOptions();
  SpellCheckerOptionsDialog OptionsDialog(this, ReturnSelectedSpellCheckEngine()->GetSpellCheckEngineName() + _T(" Options"), ReturnSelectedSpellCheckEngine());
  if (OptionsDialog.ShowModal() == wxID_OK)
  {
    // Set the modified options
    OptionsMap* pOptionsMap = OptionsDialog.GetModifiedOptions();
    if (pOptionsMap)
    {
      for (OptionsMap::iterator it = pOptionsMap->begin(); it != pOptionsMap->end(); it++)
        ReturnSelectedSpellCheckEngine()->AddOptionToMap(it->second);
    }
    ReturnSelectedSpellCheckEngine()->ApplyOptions();
  }
}

void MyFrame::OnEditPersonalDictionary(wxCommandEvent& event)
{
  XmlPersonalDictionaryDialog PersonalDictionaryDialog(NULL, _T("resource.xrc"), _T("PersonalDictionary"), ReturnSelectedSpellCheckEngine());
  PersonalDictionaryDialog.ShowModal();
}

void MyFrame::SpellCheck(wxSpellCheckEngineInterface* pSpellChecker)
{
  long selectionFrom = -1, selectionTo = -1;
  wxString strText = textCtrl->GetStringSelection();
  if (!strText.empty())
  {
    // Get the range of the text controls's string value to replace
    textCtrl->GetSelection(&selectionFrom, &selectionTo);
  }
  else
  {
    // Since there was no text selected, grab the entire contents of the text control
    strText = textCtrl->GetValue();
  }
  
  wxString strNewText = pSpellChecker->CheckSpelling(strText);

  if (!strNewText.empty())  // If the new text string is empty, then assume that the user cancelled
  {
    if ((selectionFrom != -1) && (selectionTo != -1))
    {
      textCtrl->Replace(selectionFrom, selectionTo, strNewText);
    }
    else
    {
      textCtrl->SetValue(strNewText);
    }
  }
  ::wxMessageBox(_T("Spell Check Completed"));
}

wxSpellCheckEngineInterface* MyFrame::ReturnSelectedSpellCheckEngine()
{
  wxSpellCheckEngineInterface* pEngine = NULL;
  if (m_nSelectedSpellCheckEngine == MyFrame::USE_ASPELL)
    pEngine = m_pAspellInterface;
  else if (m_nSelectedSpellCheckEngine == MyFrame::USE_MYSPELL)
    pEngine = m_pMySpellInterface;

  return pEngine;
}

void MyFrame::XmlSpellCheck(wxString strDialogResource)
{
  wxSpellCheckEngineInterface* pSpellChecker = ReturnSelectedSpellCheckEngine();
  if (pSpellChecker)
  {
    if (pSpellChecker->IsInitialized())
    {
      pSpellChecker->SetSpellCheckUserInterface(new XmlSpellCheckDialog(NULL, _T("resource.xrc"), strDialogResource, _T("PersonalDictionary")));
      SpellCheck(pSpellChecker);
    }
    else
    {
      ::wxMessageBox(wxString::Format(_("Current options prevent proper %s spell check engine initialization.  Spell check is not possible."), pSpellChecker->GetSpellCheckEngineName().c_str()));
    }
  }
}

void MyFrame::SetDefaultAspellOptions()
{
  if (m_pAspellInterface)
  {
    SpellCheckEngineOption LanguageOption(_T("lang"), _T("Language Code"), _("en"), SpellCheckEngineOption::STRING); // A list of possible values would be good here
    LanguageOption.SetDependency(_T("dict-dir"));
    m_pAspellInterface->AddOptionToMap(LanguageOption);
    SpellCheckEngineOption DataDirOption(_T("data-dir"), _T("Language Data File Directory"), _("data"), SpellCheckEngineOption::DIR);
    m_pAspellInterface->AddOptionToMap(DataDirOption);
    SpellCheckEngineOption DictDirOption(_T("dict-dir"), _T("Language Word List Directory"), _("dict"), SpellCheckEngineOption::DIR);
    m_pAspellInterface->AddOptionToMap(DictDirOption);
  
    SpellCheckEngineOption SuggestionModeOption(_T("sug-mode"), _T("Suggestion Mode"), _T("normal"), SpellCheckEngineOption::STRING);
    SuggestionModeOption.AddPossibleValue(wxString(_T("ultra")));
    SuggestionModeOption.AddPossibleValue(wxString(_T("fast")));
    SuggestionModeOption.AddPossibleValue(wxString(_T("normal")));
    SuggestionModeOption.AddPossibleValue(wxString(_T("bad-spellers")));
    m_pAspellInterface->AddOptionToMap(SuggestionModeOption);

    /* - Not compatible with Aspell 0.60 library
    SpellCheckEngineOption FilterModeOption(_T("mode"), _T("Filter Mode"), pConfig->Read(_T("mode"), _T("none")));
    FilterModeOption.AddPossibleValue(wxString(_T("none")));
    FilterModeOption.AddPossibleValue(wxString(_T("url")));
    FilterModeOption.AddPossibleValue(wxString(_T("email")));
    FilterModeOption.AddPossibleValue(wxString(_T("sgml")));
    FilterModeOption.AddPossibleValue(wxString(_T("tex")));
    m_pAspellInterface->AddOptionToMap(FilterModeOption);
    */

    bool bIgnoreCase = false;
    SpellCheckEngineOption IgnoreCaseOption(_T("ignore-case"), _T("Ignore Case"), bIgnoreCase);
    m_pAspellInterface->AddOptionToMap(IgnoreCaseOption);
    m_pAspellInterface->ApplyOptions();

    // Set the personal dictionary file
    m_pAspellInterface->OpenPersonalDictionary(_("personaldictionary.dic"));
  }
}

void MyFrame::SetDefaultMySpellOptions()
{
  if (m_pMySpellInterface)
  {
    SpellCheckEngineOption DictionaryPathOption(_T("dictionary-path"), _T("Dictionary Path"), wxFileName::GetCwd(), SpellCheckEngineOption::DIR);
    // If you need to set an option but don't want the option to be user editable, use SetShowOption as follows:
    //  DictionaryPathOption.SetShowOption(false);
    m_pMySpellInterface->AddOptionToMap(DictionaryPathOption);
    SpellCheckEngineOption LanguageOption(_T("language"), _T("Language"), _("English (United States)"), SpellCheckEngineOption::STRING);
    LanguageOption.SetDependency(_T("dictionary-path"));
    m_pMySpellInterface->AddOptionToMap(LanguageOption);

    // Add custom dictionary entries
    m_pMySpellInterface->AddCustomMySpellDictionary(_("Michigan, USA"), _("en_US"));

    // An alternative to using the "dictionary-path" and "language" options is to use the dict-file and affix-file options
    //  and set the dictionary and affix file directory with these options.
    // The "dict-file" and "affix-file" options go together and are mutually exclusive from using the "dictionary-path" and "language" options
    //SpellCheckEngineOption DictionaryFileOption(_T("dict-file"), _T("Dictionary File"), wxFileName::GetCwd() + wxFILE_SEP_PATH + _T("en_US.dic"), SpellCheckEngineOption::FILE);
    //m_pMySpellInterface->AddOptionToMap(DictionaryFileOption);
    //SpellCheckEngineOption AffixFileOption(_T("affix-file"), _T("Affix File"), wxFileName::GetCwd() + wxFILE_SEP_PATH + _T("en_US.aff"), SpellCheckEngineOption::FILE);
    //m_pMySpellInterface->AddOptionToMap(AffixFileOption);

    m_pMySpellInterface->ApplyOptions();

    // Set the personal dictionary file
    m_pMySpellInterface->OpenPersonalDictionary(_("personaldictionary.dic"));
  }
}

void MyFrame::SavePersonalDictionaries()
{
  if (m_pAspellInterface)
  {
    m_pAspellInterface->GetPersonalDictionary()->SavePersonalDictionary();
  }

  if (m_pMySpellInterface)
  {
    m_pMySpellInterface->GetPersonalDictionary()->SavePersonalDictionary();
  }
}

void MyFrame::SaveOptions()
{
  // This function isn't actually used in the demo, but here is some sample code as to how you might save
  //  the spell checker options
  OptionsMap* pAspellOptions = m_pAspellInterface->GetOptions();
  wxConfigBase* pConfig = wxConfigBase::Get();
  if (pConfig)
  {
    wxString strPath = m_pAspellInterface->GetSpellCheckEngineName();
    pConfig->SetPath(strPath);
    wxString strOption = wxEmptyString;

    // Iterate through ALL the options
    for (OptionsMap::iterator it = pAspellOptions->begin(); it != pAspellOptions->end(); it++)
    {
      strOption = _T("/") + strPath + _T("/") + it->second.GetName();
      switch (it->second.GetOptionType())
      {
        case SpellCheckEngineOption::STRING:
        case SpellCheckEngineOption::DIR:
        case SpellCheckEngineOption::FILE:
          // For DIR and FILE options, GetStringValue() returns the absolute path
          pConfig->Write(strOption, it->second.GetStringValue());
          break;
        case SpellCheckEngineOption::LONG:
          pConfig->Write(strOption, it->second.GetLongValue());
          break;
        case SpellCheckEngineOption::DOUBLE:
          pConfig->Write(strOption, it->second.GetDoubleValue());
          break;
        case SpellCheckEngineOption::BOOLEAN:
          pConfig->Write(strOption, it->second.GetBoolValue());
          break;
        default:
          pConfig->Write(strOption, it->second.GetStringValue());
          break;
      };
    }
    // End of code saving ALL options


    // Save only selected options ("language" in this case)
    OptionsMap* pMySpellOptions = m_pMySpellInterface->GetOptions();
    strPath = m_pMySpellInterface->GetSpellCheckEngineName();
    pConfig->SetPath(strPath);
    strOption = wxEmptyString;
    OptionsMap::iterator finder = pMySpellOptions->find(_T("language"));
    if (finder != pMySpellOptions->end())
    {
      strOption = _T("/") + strPath + _T("/") + finder->second.GetName();
      pConfig->Write(strOption, finder->second.GetValueAsString());
    }
  }
}


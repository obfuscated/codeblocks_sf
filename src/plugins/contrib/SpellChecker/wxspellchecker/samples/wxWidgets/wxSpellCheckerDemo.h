#ifndef __WX_SPELLCHECKER_DEMO__
#define __WX_SPELLCHECKER_DEMO__

class wxSpellCheckEngineInterface;
class wxSpellCheckUserInterface;

class AspellInterface;
class MySpellInterface;

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title);
    virtual ~MyFrame();

    // event handlers (these functions should _not_ be virtual)
    void OnNew(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnUseAspell(wxCommandEvent& event);
    void OnUpdateUseAspell(wxUpdateUIEvent& event);
    void OnUseMySpell(wxCommandEvent& event);
    void OnUpdateUseMySpell(wxUpdateUIEvent& event);
    void OnSpellCheckMozillaUI(wxCommandEvent& event);
    void OnSpellCheckOutlookUI(wxCommandEvent& event);
    void OnSpellCheckSentryUI(wxCommandEvent& event);
    void OnSpellCheckAbiwordUI(wxCommandEvent& event);
    void OnEditOptions(wxCommandEvent& event);
    void OnEditPersonalDictionary(wxCommandEvent& event);
    void SpellCheck(wxSpellCheckEngineInterface* pSpellChecker);
    wxSpellCheckEngineInterface* ReturnSelectedSpellCheckEngine();
    void XmlSpellCheck(wxString strDialogResource);
    
    void SetDefaultAspellOptions();
    void SetDefaultMySpellOptions();
    void SavePersonalDictionaries();
    void SaveOptions();

private:
    wxTextCtrl* textCtrl;
    bool m_bSpellCheckOnRightClick;
    int m_nSelectedSpellCheckEngine;
    AspellInterface* m_pAspellInterface;
    MySpellInterface* m_pMySpellInterface;
    
    enum
    {
      USE_ASPELL = 0,
      USE_MYSPELL
    };
    
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

#endif // __WX_SPELLCHECKER_DEMO__

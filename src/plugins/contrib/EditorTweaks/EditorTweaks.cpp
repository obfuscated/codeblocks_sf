#include <sdk.h> // Code::Blocks SDK
#include <algorithm> // std::sort
#include <configurationpanel.h>
#include "EditorTweaks.h"

#ifndef CB_PRECOMP
    #include <wx/menu.h>
    #include <wx/textdlg.h>
    #include <wx/toolbar.h>
#endif

#include "EditorTweaksConfDlg.h"


#include <manager.h>
#include <configmanager.h>
#include <logmanager.h>
#include <editor_hooks.h>
#include <cbeditor.h>
#include <wx/wxscintilla.h>
#include <editormanager.h>
#include "cbstyledtextctrl.h"


// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
    PluginRegistrant<EditorTweaks> reg(_T("EditorTweaks"));

    struct CompareAlignerMenuEntry
    {
        bool operator() (AlignerMenuEntry i, AlignerMenuEntry j) { return (i.UsageCount<=j.UsageCount);}
    } CompareAlignerMenuEntryObject;

    const unsigned int defaultStoredAlignerEntries = 4;

    wxString defaultNames[defaultStoredAlignerEntries] = { _T("Equality Operator"), _T("C/C++ line Comment "), _T("VHDL Signal Assignment"), _T("VHDL named association")};
    wxString defaultStrings[defaultStoredAlignerEntries] = { _T("="), _T("//"), _T("<="), _T("=>") };

}

int id_et= wxNewId();
int id_et_WordWrap= wxNewId();
int id_et_ShowLineNumbers= wxNewId();
int id_et_TabChar = wxNewId();
int id_et_TabIndent = wxNewId();
int id_et_TabSize2 = wxNewId();
int id_et_TabSize4 = wxNewId();
int id_et_TabSize6 = wxNewId();
int id_et_TabSize8 = wxNewId();
int id_et_ShowEOL = wxNewId();
int id_et_StripTrailingBlanks = wxNewId();
int id_et_EnsureConsistentEOL = wxNewId();
int id_et_EOLCRLF = wxNewId();
int id_et_EOLCR = wxNewId();
int id_et_EOLLF = wxNewId();
int id_et_Fold1= wxNewId();
int id_et_Fold2= wxNewId();
int id_et_Fold3= wxNewId();
int id_et_Fold4= wxNewId();
int id_et_Fold5= wxNewId();
int id_et_Unfold1= wxNewId();
int id_et_Unfold2= wxNewId();
int id_et_Unfold3= wxNewId();
int id_et_Unfold4= wxNewId();
int id_et_Unfold5= wxNewId();
int id_et_align_others= wxNewId();
int id_et_SuppressInsertKey= wxNewId();

// events handling
BEGIN_EVENT_TABLE(EditorTweaks, cbPlugin)
    EVT_UPDATE_UI(id_et_WordWrap, EditorTweaks::OnUpdateUI)
    EVT_UPDATE_UI(id_et_ShowLineNumbers, EditorTweaks::OnUpdateUI)
    EVT_UPDATE_UI(id_et_TabChar, EditorTweaks::OnUpdateUI)
    EVT_UPDATE_UI(id_et_TabIndent, EditorTweaks::OnUpdateUI)
    EVT_UPDATE_UI(id_et_TabSize2, EditorTweaks::OnUpdateUI)
    EVT_UPDATE_UI(id_et_TabSize4, EditorTweaks::OnUpdateUI)
    EVT_UPDATE_UI(id_et_TabSize6, EditorTweaks::OnUpdateUI)
    EVT_UPDATE_UI(id_et_TabSize8, EditorTweaks::OnUpdateUI)
    EVT_UPDATE_UI(id_et_ShowEOL, EditorTweaks::OnUpdateUI)
    EVT_UPDATE_UI(id_et_StripTrailingBlanks, EditorTweaks::OnUpdateUI)
    EVT_UPDATE_UI(id_et_EnsureConsistentEOL, EditorTweaks::OnUpdateUI)
    EVT_UPDATE_UI(id_et_EOLCRLF, EditorTweaks::OnUpdateUI)
    EVT_UPDATE_UI(id_et_EOLCR, EditorTweaks::OnUpdateUI)
    EVT_UPDATE_UI(id_et_EOLLF, EditorTweaks::OnUpdateUI)


    EVT_MENU(id_et_WordWrap, EditorTweaks::OnWordWrap)
    EVT_MENU(id_et_ShowLineNumbers, EditorTweaks::OnShowLineNumbers)
    EVT_MENU(id_et_TabChar, EditorTweaks::OnTabChar)
    EVT_MENU(id_et_TabIndent, EditorTweaks::OnTabIndent)
    EVT_MENU(id_et_TabSize2, EditorTweaks::OnTabSize2)
    EVT_MENU(id_et_TabSize4, EditorTweaks::OnTabSize4)
    EVT_MENU(id_et_TabSize6, EditorTweaks::OnTabSize6)
    EVT_MENU(id_et_TabSize8, EditorTweaks::OnTabSize8)
    EVT_MENU(id_et_ShowEOL, EditorTweaks::OnShowEOL)
    EVT_MENU(id_et_StripTrailingBlanks, EditorTweaks::OnStripTrailingBlanks)
    EVT_MENU(id_et_EnsureConsistentEOL, EditorTweaks::OnEnsureConsistentEOL)
    EVT_MENU(id_et_EOLCRLF, EditorTweaks::OnEOLCRLF)
    EVT_MENU(id_et_EOLCR, EditorTweaks::OnEOLCR)
    EVT_MENU(id_et_EOLLF, EditorTweaks::OnEOLLF)
    EVT_MENU(id_et_Fold1, EditorTweaks::OnFold)
    EVT_MENU(id_et_Fold2, EditorTweaks::OnFold)
    EVT_MENU(id_et_Fold3, EditorTweaks::OnFold)
    EVT_MENU(id_et_Fold4, EditorTweaks::OnFold)
    EVT_MENU(id_et_Fold5, EditorTweaks::OnFold)
    EVT_MENU(id_et_Unfold1, EditorTweaks::OnUnfold)
    EVT_MENU(id_et_Unfold2, EditorTweaks::OnUnfold)
    EVT_MENU(id_et_Unfold3, EditorTweaks::OnUnfold)
    EVT_MENU(id_et_Unfold4, EditorTweaks::OnUnfold)
    EVT_MENU(id_et_Unfold5, EditorTweaks::OnUnfold)

    EVT_MENU(id_et_SuppressInsertKey, EditorTweaks::OnSuppressInsert)
    EVT_MENU(id_et_align_others, EditorTweaks::OnAlignOthers)
END_EVENT_TABLE()

// constructor
EditorTweaks::EditorTweaks()
{
    // Make sure our resources are available.
    // In the generated boilerplate code we have no resources but when
    // we add some, it will be nice that this code is in place already ;)
    if (!Manager::LoadResource(_T("EditorTweaks.zip")))
        NotifyMissingFile(_T("EditorTweaks.zip"));
}

// destructor
EditorTweaks::~EditorTweaks()
{
}

void EditorTweaks::OnAttach()
{
    // do whatever initialization you need for your plugin
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be TRUE...
    // You should check for it in other functions, because if it
    // is FALSE, it means that the application did *not* "load"
    // (see: does not need) this plugin...

//    EditorHooks::HookFunctorBase* myhook = new EditorHooks::HookFunctor<EditorTweaks>(this, &EditorTweaks::EditorEventHook);
//    m_EditorHookId = EditorHooks::RegisterHook(myhook);
    Manager* pm = Manager::Get();
    pm->RegisterEventSink(cbEVT_EDITOR_OPEN, new cbEventFunctor<EditorTweaks, CodeBlocksEvent>(this, &EditorTweaks::OnEditorOpen));
//    pm->RegisterEventSink(cbEVT_EDITOR_CLOSE, new cbEventFunctor<EditorTweaks, CodeBlocksEvent>(this, &EditorTweaks::OnEditorClose));
//    pm->RegisterEventSink(cbEVT_EDITOR_UPDATE_UI, new cbEventFunctor<EditorTweaks, CodeBlocksEvent>(this, &EditorTweaks::OnEditorUpdateUI));
//    pm->RegisterEventSink(cbEVT_EDITOR_SWITCHED, new cbEventFunctor<EditorTweaks, CodeBlocksEvent>(this, &EditorTweaks::OnEditorActivate));
//    pm->RegisterEventSink(cbEVT_EDITOR_DEACTIVATED, new cbEventFunctor<EditorTweaks, CodeBlocksEvent>(this, &EditorTweaks::OnEditorDeactivate));

    m_tweakmenu=NULL;

    EditorManager* em = Manager::Get()->GetEditorManager();
    for (int i=0;i<em->GetEditorsCount();i++)
    {
        cbEditor* ed=em->GetBuiltinEditor(i);
        if (ed && ed->GetControl())
        {
            ed->GetControl()->SetOvertype(false);
            ed->GetControl()->Connect(wxEVT_KEY_DOWN,(wxObjectEventFunction) (wxEventFunction) (wxCharEventFunction)&EditorTweaks::OnKeyPress,NULL,this);
        }
    }


    AlignerMenuEntry e;

    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("EditorTweaks"));

    for (int i = 0 ; i < cfg->ReadInt(_T("AlingerSavedEntries"),defaultStoredAlignerEntries) ; ++i)
    {
        e.MenuName = cfg->Read(wxString::Format(_T("AlignerFirstName%d"),i),defaultNames[i]);
        e.ArgumentString = cfg->Read(wxString::Format(_T("AlignerFirstArgumentString%d"),i) ,defaultStrings[i]);
        e.UsageCount = 0;
        e.id = wxNewId();
        AlignerMenuEntries.push_back(e);
        Connect(e.id, wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(EditorTweaks::OnAlign) );
    }
    m_suppress_insert=cfg->ReadBool(_("SuppressInsertKey"),false);
}

void EditorTweaks::OnRelease(bool /*appShutDown*/)
{
    m_tweakmenu = 0;

//    EditorHooks::UnregisterHook(m_EditorHookId, true);
    EditorManager* em = Manager::Get()->GetEditorManager();
    for (int i=0;i<em->GetEditorsCount();i++)
    {
        cbEditor* ed=em->GetBuiltinEditor(i);
        if (ed && ed->GetControl())
            ed->GetControl()->Disconnect(wxEVT_NULL,(wxObjectEventFunction) (wxEventFunction) (wxCharEventFunction)&EditorTweaks::OnKeyPress);
    }

    AlignerMenuEntry e;

    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("EditorTweaks"));
    std::sort (AlignerMenuEntries.begin(), AlignerMenuEntries.end(),CompareAlignerMenuEntryObject);
    std::reverse( AlignerMenuEntries.begin(), AlignerMenuEntries.end());
    int i = 0;
    for (; i < cfg->ReadInt(_T("AlingerMaxSavedEntries"),defaultStoredAlignerEntries) && i < AlignerMenuEntries.size() ; ++i)
    {
        cfg->Write(wxString::Format(_T("AlignerFirstName%d"),i),AlignerMenuEntries[i].MenuName);
        cfg->Write(wxString::Format(_T("AlignerFirstArgumentString%d"),i) ,AlignerMenuEntries[i].ArgumentString);

        Disconnect(AlignerMenuEntries[i].id, wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(EditorTweaks::OnAlign) );
    }
    cfg->Write(_T("AlingerSavedEntries"),i);
    for (; i < static_cast<int>(AlignerMenuEntries.size()) ; ++i)
        Disconnect(AlignerMenuEntries[i].id, wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(EditorTweaks::OnAlign) );
    cfg->Write(_("SuppressInsertKey"),m_suppress_insert);
}

cbConfigurationPanel* EditorTweaks::GetConfigurationPanel(wxWindow* parent)
{
    if ( !IsAttached() )
        return NULL;

    EditorTweaksConfDlg* cfg = new EditorTweaksConfDlg(parent);
    return cfg;
}

void EditorTweaks::BuildMenu(wxMenuBar* menuBar)
{
    Manager::Get()->GetLogManager()->DebugLog(_("Editor Tweaks plugin: Building menu"));
    int i=menuBar->FindMenu(_("&Edit"));
    if (i==wxNOT_FOUND)
    {
        Manager::Get()->GetLogManager()->DebugLog(_("Editor Tweaks plugin: edit menu not found"));
        return;
    }
    wxMenu *menu=menuBar->GetMenu(i);
    for (i = 0; i < static_cast<int>(menu->GetMenuItemCount()); ++i)
    {
        wxMenuItem *mm=menu->FindItemByPosition(i);
        #if wxCHECK_VERSION(2, 9, 0)
        if (mm->GetItemLabel()==_("End-of-line mode"))
        #else
        if (mm->GetLabel()==_("End-of-line mode"))
        #endif
            menu->Remove(mm);
        #if wxCHECK_VERSION(2, 9, 0)
        if (mm->GetItemLabel()==_("Special commands"))
        #else
        if (mm->GetLabel()==_("Special commands"))
        #endif
            break;
    }
    if (i == static_cast<int>(menu->GetMenuItemCount()))
    {
        Manager::Get()->GetLogManager()->DebugLog(_("Editor Tweaks plugin: Special commands menu not found"));
        return;
    }
    Manager::Get()->GetLogManager()->DebugLog(wxString::Format(_("Editor Tweaks plugin: making the menu %i"),i));
    m_tweakmenu=new wxMenu();
    m_tweakmenuitem=new wxMenuItem(menu, id_et, _("Editor Tweaks"), _("Tweak the settings of the active editor"), wxITEM_NORMAL, m_tweakmenu);
    menu->Insert(i+1,m_tweakmenuitem);

    wxMenu *submenu=m_tweakmenu; //_("Editor Tweaks")

    submenu->AppendCheckItem( id_et_WordWrap, _( "Word wrap" ), _( "Wrap text" ) );
    submenu->AppendCheckItem( id_et_ShowLineNumbers, _( "Show Line Numbers" ), _( "Show Line Numbers" ) );
    submenu->AppendSeparator();
    submenu->AppendCheckItem( id_et_TabChar, _( "Use Tab Character" ), _( "Use Tab Character" ) );
    submenu->AppendCheckItem( id_et_TabIndent, _( "Tab Indents" ), _( "Tab Indents" ) );
    wxMenu *tabsizemenu=new wxMenu();
    tabsizemenu->AppendRadioItem( id_et_TabSize2, _( "2" ), _( "Tab Width of 2" ) );
    tabsizemenu->AppendRadioItem( id_et_TabSize4, _( "4" ), _( "Tab Width of 4" ) );
    tabsizemenu->AppendRadioItem( id_et_TabSize6, _( "6" ), _( "Tab Width of 6" ) );
    tabsizemenu->AppendRadioItem( id_et_TabSize8, _( "8" ), _( "Tab Width of 8" ) );
    submenu->Append(wxID_ANY,_("Tab Size"),tabsizemenu);
    submenu->AppendSeparator();
    wxMenu *eolmenu=new wxMenu();
    eolmenu->AppendRadioItem( id_et_EOLCRLF, _( "CR LF" ), _( "Carriage Return - Line Feed (Windows Default)" ) );
    eolmenu->AppendRadioItem( id_et_EOLCR, _( "CR" ), _( "Carriage Return (Mac Default)" ) );
    eolmenu->AppendRadioItem( id_et_EOLLF, _( "LF" ), _( "Line Feed (Unix Default)" ) );
    submenu->Append(wxID_ANY,_("End-of-Line Mode"),eolmenu);
    submenu->AppendCheckItem( id_et_ShowEOL, _( "Show EOL Chars" ), _( "Show End-of-Line Characters" ) );
    submenu->Append( id_et_StripTrailingBlanks, _( "Strip Trailing Blanks Now" ), _( "Strip trailing blanks from each line" ) );
    submenu->Append( id_et_EnsureConsistentEOL, _( "Make EOLs Consistent Now" ), _( "Convert End-of-Line Characters to the Active Setting" ) );
    submenu->AppendSeparator();
    submenu->AppendCheckItem( id_et_SuppressInsertKey, _("Suppress Insert Key"), _("Disable toggle between insert and overwrite mode using the insert key") );


    wxMenu *foldmenu = 0;
    for (i = 0; i < static_cast<int>(menu->GetMenuItemCount()); ++i)
    {
        wxMenuItem *mm = menu->FindItemByPosition(i);
        #if wxCHECK_VERSION(2, 9, 0)
        if (mm->GetItemLabel()==_("Folding"))
        #else
        if (mm->GetLabel()==_("Folding"))
        #endif
        {
            foldmenu=mm->GetSubMenu();
            break;
        }
    }
    if (!foldmenu)
    {
        Manager::Get()->GetLogManager()->DebugLog(_("Editor Tweaks plugin: Folding menu"));
        return;
    }

    foldmenu->AppendSeparator();
    wxMenu *foldlevelmenu=new wxMenu();
    foldlevelmenu->Append( id_et_Fold1, _( "1" ), _( "Fold all code to the first level" ) );
    foldlevelmenu->Append( id_et_Fold2, _( "2" ), _( "Fold all code to the second level" ) );
    foldlevelmenu->Append( id_et_Fold3, _( "3" ), _( "Fold all code to the third level" ) );
    foldlevelmenu->Append( id_et_Fold4, _( "4" ), _( "Fold all code to the fourth level" ) );
    foldlevelmenu->Append( id_et_Fold5, _( "5" ), _( "Fold all code to the fifth level" ) );
    foldmenu->Append(wxID_ANY,_("Fold all above level"),foldlevelmenu);

    wxMenu *unfoldlevelmenu=new wxMenu();
    unfoldlevelmenu->Append( id_et_Unfold1, _( "1" ), _( "Unfold all code to the first level" ) );
    unfoldlevelmenu->Append( id_et_Unfold2, _( "2" ), _( "Unfold all code to the second level" ) );
    unfoldlevelmenu->Append( id_et_Unfold3, _( "3" ), _( "Unfold all code to the third level" ) );
    unfoldlevelmenu->Append( id_et_Unfold4, _( "4" ), _( "Unfold all code to the fourth level" ) );
    unfoldlevelmenu->Append( id_et_Unfold5, _( "5" ), _( "Unfold all code to the fifth level" ) );
    foldmenu->Append(wxID_ANY,_("Unfold all above level"),unfoldlevelmenu);

    UpdateUI();
}


void EditorTweaks::UpdateUI()
{
    if (!m_tweakmenu)
    	return;

    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed || !ed->GetControl())
    {
        m_tweakmenuitem->Enable(false);
        return;
    }
    m_tweakmenuitem->Enable(true);

    wxMenu *submenu = m_tweakmenu; //_("Editor Tweaks") TODO: Retrieve actual menu

    submenu->Check(id_et_WordWrap,ed->GetControl()->GetWrapMode()>0);
    submenu->Check(id_et_ShowLineNumbers,ed->GetControl()->GetMarginWidth(0)>0);
    submenu->Check(id_et_TabChar,ed->GetControl()->GetUseTabs());
    submenu->Check(id_et_TabIndent,ed->GetControl()->GetTabIndents());
    submenu->Check(id_et_TabSize2,ed->GetControl()->GetTabWidth()==2);
    submenu->Check(id_et_TabSize4,ed->GetControl()->GetTabWidth()==4);
    submenu->Check(id_et_TabSize6,ed->GetControl()->GetTabWidth()==6);
    submenu->Check(id_et_TabSize8,ed->GetControl()->GetTabWidth()==8);
    submenu->Check(id_et_EOLCRLF,ed->GetControl()->GetEOLMode()==wxSCI_EOL_CRLF);
    submenu->Check(id_et_EOLCR,ed->GetControl()->GetEOLMode()==wxSCI_EOL_CR);
    submenu->Check(id_et_EOLLF,ed->GetControl()->GetEOLMode()==wxSCI_EOL_LF);
    submenu->Check(id_et_ShowEOL,ed->GetControl()->GetViewEOL());
    submenu->Check(id_et_SuppressInsertKey,m_suppress_insert);
}

void EditorTweaks::OnEditorUpdateUI(CodeBlocksEvent& /*event*/)
{
    Manager::Get()->GetLogManager()->DebugLog(wxString::Format(_("Editor Update UI")));
    if (!m_IsAttached || !m_tweakmenu)
        return;
    return;
    UpdateUI();
}

void EditorTweaks::OnUpdateUI(wxUpdateUIEvent &/*event*/)
{
    UpdateUI();
}

void EditorTweaks::OnEditorActivate(CodeBlocksEvent& event)
{
    Manager::Get()->GetLogManager()->DebugLog(wxString::Format(_("Editor Activate")));
    if (!m_IsAttached || !m_tweakmenu)
        return;
    if (event.GetEditor() && event.GetEditor()->IsBuiltinEditor())
    {
        m_tweakmenuitem->Enable(true);
        UpdateUI();
    }
    else
        m_tweakmenuitem->Enable(false);
}

void EditorTweaks::OnEditorDeactivate(CodeBlocksEvent& /*event*/)
{
    Manager::Get()->GetLogManager()->DebugLog(wxString::Format(_("Editor Deactivate")));
    if (!m_IsAttached || !m_tweakmenu)
        return;
    m_tweakmenuitem->Enable(false);
}


void EditorTweaks::OnEditorOpen(CodeBlocksEvent& /*event*/)
{
    Manager::Get()->GetLogManager()->DebugLog(wxString::Format(_("Editor Open")));
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed && ed->GetControl())
    {
        ed->GetControl()->SetOvertype(false);
        ed->GetControl()->Connect(wxEVT_KEY_DOWN,(wxObjectEventFunction) (wxEventFunction) (wxCharEventFunction)&EditorTweaks::OnKeyPress,NULL,this);
    }
}


void EditorTweaks::OnEditorClose(CodeBlocksEvent& /*event*/)
{
    Manager::Get()->GetLogManager()->DebugLog(wxString::Format(_("Editor Close")));
    if (!m_IsAttached || !m_tweakmenu)
        return;
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed && ed->GetControl())
        return;
    m_tweakmenuitem->Enable(false);
}

void EditorTweaks::OnKeyPress(wxKeyEvent& event)
{
    if (m_suppress_insert && event.GetKeyCode()==WXK_INSERT && event.GetModifiers()==wxMOD_NONE)
        event.Skip(false);
    else
        event.Skip(true);
}

void EditorTweaks::OnSuppressInsert(wxCommandEvent& event)
{
    m_suppress_insert = event.IsChecked();
}

//void EditorTweaks::EditorEventHook(cbEditor* editor, wxScintillaEvent& event)
//{
//}

void EditorTweaks::BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* /*data*/)
{
    //Some library module is ready to display a pop-up menu.
    //Check the parameter \"type\" and see which module it is
    //and append any items you need in the menu...
    //TIP: for consistency, add a separator as the first item...

    //make sure we have an editor
    if (type != mtEditorManager || !m_tweakmenu)
        return;

    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed || !ed->GetControl())
    {
        m_tweakmenuitem->Enable(false);
        return;
    }
    m_tweakmenuitem->Enable(true);

    // build aligner menu and items
    wxMenu* alignerMenu = new wxMenu();

    std::sort (AlignerMenuEntries.begin(), AlignerMenuEntries.end(),CompareAlignerMenuEntryObject);
    std::reverse( AlignerMenuEntries.begin(), AlignerMenuEntries.end());

    for ( unsigned int i = 0; i < AlignerMenuEntries.size() ; i++ )
        alignerMenu->Append(AlignerMenuEntries[i].id, AlignerMenuEntries[i].MenuName + _T("\t")  + _T("[") + AlignerMenuEntries[i].ArgumentString + _T("]"));
    alignerMenu->AppendSeparator();
    alignerMenu->Append(id_et_align_others, _T("more ..."));

    // attach aligner menu
    menu->AppendSeparator();
    menu->Append(wxID_ANY, _T("Aligner"), alignerMenu);

    return; // DISABLED ALL OF THE STUFF BELOW (IT IS ALREADY IN THE MAIN MENU BAR)

    // build "editor tweaks" menu
    wxMenu *submenu=new wxMenu(); //_("Editor Tweaks")

    menu->Append(id_et,_("Editor Tweaks"),submenu);

    submenu->AppendCheckItem( id_et_WordWrap, _( "Word wrap" ), _( "Wrap text" ) );
    if (ed->GetControl()->GetWrapMode()>0)
        submenu->Check(id_et_WordWrap,true);

    submenu->AppendCheckItem( id_et_ShowLineNumbers, _( "Show Line Numbers" ), _( "Show Line Numbers" ) );
    if (ed->GetControl()->GetMarginWidth(0)>0)
        submenu->Check(id_et_ShowLineNumbers,true);

    submenu->AppendSeparator();

    submenu->AppendCheckItem( id_et_TabChar, _( "Use Tab Character" ), _( "Use Tab Character" ) );
    if (ed->GetControl()->GetUseTabs())
        submenu->Check(id_et_TabChar,true);

    submenu->AppendCheckItem( id_et_TabIndent, _( "Tab Indents" ), _( "Tab Indents" ) );
    if (ed->GetControl()->GetTabIndents())
        submenu->Check(id_et_TabIndent,true);

    wxMenu *tabsizemenu=new wxMenu();
    tabsizemenu->AppendRadioItem( id_et_TabSize2, _( "2" ), _( "Tab Width of 2" ) );
    if (ed->GetControl()->GetTabWidth()==2)
        tabsizemenu->Check(id_et_TabSize2,true);
    tabsizemenu->AppendRadioItem( id_et_TabSize4, _( "4" ), _( "Tab Width of 4" ) );
    if (ed->GetControl()->GetTabWidth()==4)
        tabsizemenu->Check(id_et_TabSize4,true);
    tabsizemenu->AppendRadioItem( id_et_TabSize6, _( "6" ), _( "Tab Width of 6" ) );
    if (ed->GetControl()->GetTabWidth()==6)
        tabsizemenu->Check(id_et_TabSize6,true);
    tabsizemenu->AppendRadioItem( id_et_TabSize8, _( "8" ), _( "Tab Width of 8" ) );
    if (ed->GetControl()->GetTabWidth()==8)
        tabsizemenu->Check(id_et_TabSize8,true);
    submenu->Append(wxID_ANY,_("Tab Size"),tabsizemenu);

    submenu->AppendSeparator();

    wxMenu *eolmenu=new wxMenu();
    eolmenu->AppendRadioItem( id_et_EOLCRLF, _( "CR LF" ), _( "Carriage Return - Line Feed (Windows Default)" ) );
    if (ed->GetControl()->GetEOLMode()==wxSCI_EOL_CRLF)
        eolmenu->Check(id_et_EOLCRLF,true);
    eolmenu->AppendRadioItem( id_et_EOLCR, _( "CR" ), _( "Carriage Return (Mac Default)" ) );
    if (ed->GetControl()->GetEOLMode()==wxSCI_EOL_CR)
        eolmenu->Check(id_et_EOLCR,true);
    eolmenu->AppendRadioItem( id_et_EOLLF, _( "LF" ), _( "Line Feed (Unix Default)" ) );
    if (ed->GetControl()->GetEOLMode()==wxSCI_EOL_LF)
        eolmenu->Check(id_et_EOLLF,true);
    submenu->Append(wxID_ANY,_("End-of-Line Mode"),eolmenu);

    submenu->AppendCheckItem( id_et_ShowEOL, _( "Show EOL Chars" ), _( "Show End-of-Line Characters" ) );
    if (ed->GetControl()->GetViewEOL())
        submenu->Check(id_et_ShowEOL,true);

    submenu->Append( id_et_StripTrailingBlanks, _( "Strip Trailing Blanks Now" ), _( "Strip trailing blanks from each line" ) );

    submenu->Append( id_et_EnsureConsistentEOL, _( "Make EOLs Consistent Now" ), _( "Convert End-of-Line Characters to the Active Setting" ) );

    menu->Append(wxID_ANY, _T("Editor Tweaks"), submenu);

}

void EditorTweaks::OnWordWrap(wxCommandEvent &/*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed || !ed->GetControl())
        return;

    bool enabled=ed->GetControl()->GetWrapMode()>0;

    if (enabled)
        ed->GetControl()->SetWrapMode(wxSCI_WRAP_NONE);
    else
        ed->GetControl()->SetWrapMode(wxSCI_WRAP_WORD);


}

void EditorTweaks::OnShowLineNumbers(wxCommandEvent &/*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed || !ed->GetControl())
        return;

    bool enabled=ed->GetControl()->GetMarginWidth(0)>0;

//    bool old_state=mgr->ReadBool(_T("/show_line_numbers"), true);
//    mgr->Write(_T("/show_line_numbers"), !enabled);
//    ed->SetEditorStyleAfterFileOpen();
//    mgr->Write(_T("/show_line_numbers"), old_state);

    if (enabled)
        ed->GetControl()->SetMarginWidth(0, 0);
    else
    {
        ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));
        int pixelWidth = ed->GetControl()->TextWidth(wxSCI_STYLE_LINENUMBER, _T("9"));

        if (cfg->ReadBool(_T("/margin/dynamic_width"), false))
        {
            int lineNumWidth = 1;
            int lineCount = ed->GetControl()->GetLineCount();

            while (lineCount >= 10)
            {
                lineCount /= 10;
                ++lineNumWidth;
            }

            ed->GetControl()->SetMarginWidth(0, 6 + lineNumWidth * pixelWidth);
        }
        else
            ed->GetControl()->SetMarginWidth(0, 6 + cfg->ReadInt(_T("/margin/width_chars"), 6) * pixelWidth);
    }
}

void EditorTweaks::OnTabChar(wxCommandEvent &/*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed || !ed->GetControl())
        return;

    ed->GetControl()->SetUseTabs(!ed->GetControl()->GetUseTabs());
}

void EditorTweaks::OnTabIndent(wxCommandEvent &/*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed || !ed->GetControl())
        return;

    ed->GetControl()->SetTabIndents(!ed->GetControl()->GetTabIndents());
}

void EditorTweaks::OnTabSize2(wxCommandEvent &/*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed || !ed->GetControl())
        return;

    ed->GetControl()->SetTabWidth(2);
}

void EditorTweaks::OnTabSize4(wxCommandEvent &/*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed || !ed->GetControl())
        return;

    ed->GetControl()->SetTabWidth(4);
}

void EditorTweaks::OnTabSize6(wxCommandEvent &/*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed || !ed->GetControl())
        return;

    ed->GetControl()->SetTabWidth(6);
}

void EditorTweaks::OnTabSize8(wxCommandEvent &/*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed || !ed->GetControl())
        return;

    ed->GetControl()->SetTabWidth(8);
}

void EditorTweaks::OnShowEOL(wxCommandEvent &/*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed || !ed->GetControl())
        return;

    ed->GetControl()->SetViewEOL(!ed->GetControl()->GetViewEOL());
}

void EditorTweaks::OnStripTrailingBlanks(wxCommandEvent &/*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed || !ed->GetControl())
        return;

    cbStyledTextCtrl* control = ed->GetControl();
    int maxLines = control->GetLineCount();
    control->BeginUndoAction();
    for (int line = 0; line < maxLines; line++)
    {
        int lineStart = control->PositionFromLine(line);
        int lineEnd = control->GetLineEndPosition(line);
        int i = lineEnd-1;
        wxChar ch = (wxChar)(control->GetCharAt(i));
        while ((i >= lineStart) && ((ch == _T(' ')) || (ch == _T('\t'))))
        {
            i--;
            ch = (wxChar)(control->GetCharAt(i));
        }
        if (i < (lineEnd-1))
        {
            control->SetTargetStart(i+1);
            control->SetTargetEnd(lineEnd);
            control->ReplaceTarget(_T(""));
        }
    }
    control->EndUndoAction();
}

void EditorTweaks::OnEnsureConsistentEOL(wxCommandEvent &/*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed || !ed->GetControl())
        return;

    ed->GetControl()->ConvertEOLs(ed->GetControl()->GetEOLMode());
}

void EditorTweaks::OnEOLCRLF(wxCommandEvent &/*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed || !ed->GetControl())
        return;

    ed->GetControl()->SetEOLMode(wxSCI_EOL_CRLF);
}

void EditorTweaks::OnEOLCR(wxCommandEvent &/*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed || !ed->GetControl())
        return;

    ed->GetControl()->SetEOLMode(wxSCI_EOL_CR);
}

void EditorTweaks::OnEOLLF(wxCommandEvent &/*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed || !ed->GetControl())
        return;

    ed->GetControl()->SetEOLMode(wxSCI_EOL_LF);
}

void EditorTweaks::OnFold(wxCommandEvent &event)
{
    int level=event.GetId()-id_et_Fold1;
    Manager::Get()->GetLogManager()->DebugLog(wxString::Format(_("Fold at level %i"),level));
    DoFoldAboveLevel(level,1);
}

void EditorTweaks::OnUnfold(wxCommandEvent &event)
{
    int level=event.GetId()-id_et_Unfold1;
    Manager::Get()->GetLogManager()->DebugLog(wxString::Format(_("Unfold at level %i"),level));
    DoFoldAboveLevel(level,0);
}


/**	Fold/Unfold/Toggle all folds in the givel level.
	\param	level	Level number of folding, starting from 0.
	\param	fold	Type of folding action requested:	\n
	-	0 = Unfold.
	-	1 = Fold.
*/
void EditorTweaks::DoFoldAboveLevel(int level, int fold)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed || !ed->GetControl())
        return;

    level+=wxSCI_FOLDLEVELBASE;

    ed->GetControl()->Colourise(0, -1); // the *most* important part!

	// Scan all file lines searching for the specified folding level.
    int count = ed->GetControl()->GetLineCount();
    for (int line = 0; line <= count; ++line)
    {
        int line_level_data = ed->GetControl()->GetFoldLevel(line);
        if (!(line_level_data & wxSCI_FOLDLEVELHEADERFLAG))
            continue;
        const int line_level = line_level_data & wxSCI_FOLDLEVELNUMBERMASK;

        const bool IsExpanded = ed->GetControl()->GetFoldExpanded(line);

        // If a fold/unfold request is issued when the block is already
        // folded/unfolded, ignore the request.
        if (line_level<=level)
        {
            if (IsExpanded)
                continue;
        }
        else
        {
            if ((fold==0 && IsExpanded) || (fold ==1 && !IsExpanded))
                continue;
        }
        ed->GetControl()->ToggleFold(line);
    }
}

void EditorTweaks::OnAlign(wxCommandEvent& event)
{
    int id = event.GetId();
    for ( unsigned int i = 0 ; i < AlignerMenuEntries.size(); i++)
    {
        if ( AlignerMenuEntries[i].id == id )
        {
            AlignToString(AlignerMenuEntries[i].ArgumentString);
            AlignerMenuEntries[i].UsageCount ++;
            break;
        }
    }
}

void EditorTweaks::OnAlignOthers(wxCommandEvent& /*event*/)
{
    wxString NewAlignmentString;
    wxString NewAlignmentStringName;
    bool NewCharacter = true;

    // create the name and call the first DialogBox
    const wxString MessageArgumentString = _("Insert a new character");
    const wxString CaptionArgumentString = _("New character");
    NewAlignmentString = wxGetTextFromUser( MessageArgumentString, CaptionArgumentString );
    if (NewAlignmentString !=_T(""))
    {
        // check if the new character is equal as an exist
        unsigned int i;
        for ( i = 0 ; i < AlignerMenuEntries.size(); i++)
        {
            if (AlignerMenuEntries[i].ArgumentString == NewAlignmentString)
            {
                NewCharacter = false;
                break;
            }
        }

        if (NewCharacter)
        {
            AlignerMenuEntry e;
            e.UsageCount = 0;
            e.id = wxNewId();
            e.ArgumentString = NewAlignmentString;
            AlignerMenuEntries.push_back(e);
            Connect(e.id, wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(EditorTweaks::OnAlign) );
        }

        // create the name and call the second DialogBox
        const wxString MessageName = _("Insert a name for the (new) character");
        const wxString CaptionName = NewAlignmentString;
        NewAlignmentStringName = wxGetTextFromUser( MessageName, CaptionName , AlignerMenuEntries[i].MenuName);
        if (NewAlignmentStringName != _T(""))
            AlignerMenuEntries[i].MenuName = NewAlignmentStringName;

        AlignToString(AlignerMenuEntries[i].ArgumentString);
        AlignerMenuEntries[i].UsageCount++;
    }
}

void EditorTweaks::AlignToString(const wxString AlignmentString)
{
    cbStyledTextCtrl* control = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor()->GetControl();
    if (control)
    {
        int line_start = wxSCI_INVALID_POSITION;
        int line_end   = wxSCI_INVALID_POSITION;
        if (GetSelectionLines(line_start, line_end))
        {
            // get furthest position of alignmentstring
            size_t find_position  = wxString::npos;
            size_t max_position   = wxString::npos;
            int matches           = 0;
            for (int i=line_start;i<=line_end;i++)
            {
            	// look for string
            	find_position = control->GetLine(i).Find(AlignmentString);

                // store max position
                if (find_position != wxString::npos)
                {
                	matches++;
					if ((int) find_position > (int) max_position)
						max_position = find_position;
                }
			}

            // if string has been found more than once
            if (matches > 1)
            {
				// loop through lines
				wxString replacement_text = _T("");
				wxString current_line     = _T("");
				int spacing_diff          = 0;
				for (int i=line_start;i<=line_end;i++)
				{
					// get line
					current_line = control->GetLine(i);
					if ( i == line_end )
                        current_line = current_line.Trim();


					// look for string
					find_position = current_line.Find(AlignmentString);

					// insert spacing
					if (find_position != wxString::npos)
					{
						spacing_diff = (int) max_position - (int) find_position;
						if (spacing_diff > 0)
						{
							// assemble next part of replacement string
							current_line = current_line.insert(find_position, GetPadding(_T(" "), spacing_diff));
						}
					}

					// tack on line
					replacement_text += current_line;
				}

				// start undo
				control->BeginUndoAction();

				// get character positions of true selection start and end
				int pos_start = control->PositionFromLine(line_start);
				int pos_end   = control->GetLineEndPosition(line_end);

				// select all lines properly
				control->SetSelectionVoid(pos_start, pos_end);

				// replace with replacement string
				control->ReplaceSelection(replacement_text);

				// finish undo
				control->EndUndoAction();
            }
        }
    }
}

wxString EditorTweaks::GetPadding(const wxString& Padding, const int Count)
{
	wxString padding = _T("");
	for (int i=0;i<Count;i++)
		padding += Padding;
	return padding;
}

bool EditorTweaks::GetSelectionLines(int& LineStart, int& LineEnd)
{
    bool found_lines = false;

    cbEditor* editor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if ( (editor) && (editor->HasSelection()) )
    {
        cbStyledTextCtrl* control = editor->GetControl();

        if (control)
        {
            int line_start = control->GetSelectionStart();
            int line_end   = control->GetSelectionEnd();

			if ( (line_start != wxSCI_INVALID_POSITION) && (line_end != wxSCI_INVALID_POSITION) )
			{
				LineStart   = control->LineFromPosition(line_start);
				LineEnd     = control->LineFromPosition(line_end);
				found_lines = (line_end > line_start);
			}
        }
    }

    // done
    return found_lines;
}


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

int id_et                     = wxNewId();
int id_et_WordWrap            = wxNewId();
int id_et_CharWrap            = wxNewId();
int id_et_ShowLineNumbers     = wxNewId();
int id_et_TabChar             = wxNewId();
int id_et_TabIndent           = wxNewId();
int id_et_TabSize2            = wxNewId();
int id_et_TabSize4            = wxNewId();
int id_et_TabSize6            = wxNewId();
int id_et_TabSize8            = wxNewId();
int id_et_ConsistentIndent    = wxNewId();
int id_et_ShowEOL             = wxNewId();
int id_et_StripTrailingBlanks = wxNewId();
int id_et_EnsureConsistentEOL = wxNewId();
int id_et_EOLCRLF             = wxNewId();
int id_et_EOLCR               = wxNewId();
int id_et_EOLLF               = wxNewId();
int id_et_Fold1               = wxNewId();
int id_et_Fold2               = wxNewId();
int id_et_Fold3               = wxNewId();
int id_et_Fold4               = wxNewId();
int id_et_Fold5               = wxNewId();
int id_et_Unfold1             = wxNewId();
int id_et_Unfold2             = wxNewId();
int id_et_Unfold3             = wxNewId();
int id_et_Unfold4             = wxNewId();
int id_et_Unfold5             = wxNewId();
int id_et_align_others        = wxNewId();
int id_et_align_auto          = wxNewId();
int id_et_align_last          = wxNewId();
int id_et_SuppressInsertKey   = wxNewId();
int id_et_ConvertBraces       = wxNewId();
int id_et_ScrollTimer         = wxNewId();

// events handling
BEGIN_EVENT_TABLE(EditorTweaks, cbPlugin)
    EVT_UPDATE_UI(id_et_WordWrap, EditorTweaks::OnUpdateUI)
    EVT_UPDATE_UI(id_et_CharWrap, EditorTweaks::OnUpdateUI)
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
    EVT_MENU(id_et_CharWrap, EditorTweaks::OnCharWrap)
    EVT_MENU(id_et_ShowLineNumbers, EditorTweaks::OnShowLineNumbers)
    EVT_MENU(id_et_TabChar, EditorTweaks::OnTabChar)
    EVT_MENU(id_et_TabIndent, EditorTweaks::OnTabIndent)
    EVT_MENU(id_et_TabSize2, EditorTweaks::OnTabSize2)
    EVT_MENU(id_et_TabSize4, EditorTweaks::OnTabSize4)
    EVT_MENU(id_et_TabSize6, EditorTweaks::OnTabSize6)
    EVT_MENU(id_et_TabSize8, EditorTweaks::OnTabSize8)
    EVT_MENU(id_et_ConsistentIndent, EditorTweaks::OnMakeIndentsConsistent)
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
    EVT_MENU(id_et_ConvertBraces,     EditorTweaks::OnConvertBraces)
    EVT_MENU(id_et_align_others,      EditorTweaks::OnAlignOthers)
    EVT_MENU(id_et_align_auto,        EditorTweaks::OnAlignAuto)
    EVT_MENU(id_et_align_last,        EditorTweaks::OnAlignLast)

    EVT_TIMER(id_et_ScrollTimer, EditorTweaks::OnScrollTimer)
END_EVENT_TABLE()

// constructor
EditorTweaks::EditorTweaks() :
    AlignerLastUsedIdx(0),
    AlignerLastUsedAuto(false),
    AlignerLastUsed(false),
    m_scrollTimer(this, id_et_ScrollTimer)
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

    Manager* pm = Manager::Get();
    pm->RegisterEventSink(cbEVT_EDITOR_OPEN, new cbEventFunctor<EditorTweaks, CodeBlocksEvent>(this, &EditorTweaks::OnEditorOpen));

    m_tweakmenu=NULL;

    EditorManager* em = Manager::Get()->GetEditorManager();
    for (int i=0;i<em->GetEditorsCount();i++)
    {
        cbEditor* ed=em->GetBuiltinEditor(i);
        if (ed && ed->GetControl())
        {
            ed->GetControl()->SetOvertype(false);
            ed->GetControl()->Connect(wxEVT_KEY_DOWN,(wxObjectEventFunction) (wxEventFunction) (wxCharEventFunction)&EditorTweaks::OnKeyPress,NULL,this);
            ed->GetControl()->Connect(wxEVT_CHAR,(wxObjectEventFunction) (wxEventFunction) (wxCharEventFunction)&EditorTweaks::OnChar,NULL,this);
        }
    }


    AlignerMenuEntry e;

    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("EditorTweaks"));

    for (int i = 0 ; i < cfg->ReadInt(_T("/aligner/saved_entries"),defaultStoredAlignerEntries) ; ++i)
    {
        e.MenuName = cfg->Read(wxString::Format(_T("/aligner/first_name_%d"),i),defaultNames[i]);
        e.ArgumentString = cfg->Read(wxString::Format(_T("/aligner/first_argument_string_%d"),i) ,defaultStrings[i]);
        e.UsageCount = 0;
        e.id = wxNewId();
        AlignerMenuEntries.push_back(e);
        Connect(e.id, wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(EditorTweaks::OnAlign) );
    }
    m_suppress_insert = cfg->ReadBool(wxT("/suppress_insert_key"), false);
    m_convert_braces  = cfg->ReadBool(wxT("/convert_braces"),      false);
    m_buffer_caret    = -1;
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
        {
            ed->GetControl()->Disconnect(wxEVT_NULL,(wxObjectEventFunction) (wxEventFunction) (wxCharEventFunction)&EditorTweaks::OnKeyPress);
            ed->GetControl()->Disconnect(wxEVT_NULL,(wxObjectEventFunction) (wxEventFunction) (wxCharEventFunction)&EditorTweaks::OnChar);
        }
    }

    AlignerMenuEntry e;

    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("EditorTweaks"));
    std::sort (AlignerMenuEntries.begin(), AlignerMenuEntries.end(),CompareAlignerMenuEntryObject);
    std::reverse( AlignerMenuEntries.begin(), AlignerMenuEntries.end());
    int i = 0;
    for (; i < cfg->ReadInt(_T("/aligner/max_saved_entries"),defaultStoredAlignerEntries) && i < static_cast<int>(AlignerMenuEntries.size()) ; ++i)
    {
        cfg->Write(wxString::Format(_T("/aligner/first_name_%d"),i),AlignerMenuEntries[i].MenuName);
        cfg->Write(wxString::Format(_T("/aligner/first_argument_string_%d"),i) ,AlignerMenuEntries[i].ArgumentString);

        Disconnect(AlignerMenuEntries[i].id, wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(EditorTweaks::OnAlign) );
    }
    cfg->Write(_T("/aligner/saved_entries"),i);
    for (; i < static_cast<int>(AlignerMenuEntries.size()) ; ++i)
        Disconnect(AlignerMenuEntries[i].id, wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(EditorTweaks::OnAlign) );
    cfg->Write(wxT("/suppress_insert_key"), m_suppress_insert);
    cfg->Write(wxT("/convert_braces"),      m_convert_braces);
}

cbConfigurationPanel* EditorTweaks::GetConfigurationPanel(wxWindow* parent)
{
    if ( !IsAttached() )
        return NULL;

    m_buffer_caret = -1; // invalidate so value will be read from configuration on next use
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

    submenu->AppendCheckItem( id_et_WordWrap, _( "Word wrap" ), _( "Wrap word" ) );
    submenu->AppendCheckItem( id_et_CharWrap, _( "Char wrap" ), _( "Wrap char" ) );
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
    submenu->Append( id_et_ConsistentIndent, _( "Make Indents Consistent" ),  _( "Convert leading tabs/spaces to the active setting" ) );
    submenu->AppendSeparator();
    wxMenu *eolmenu=new wxMenu();
    eolmenu->AppendRadioItem( id_et_EOLCRLF, _( "CR LF" ), _( "Carriage Return - Line Feed (Windows Default)" ) );
    eolmenu->AppendRadioItem( id_et_EOLCR,   _( "CR" ),    _( "Carriage Return (Mac Default)" ) );
    eolmenu->AppendRadioItem( id_et_EOLLF,   _( "LF" ),    _( "Line Feed (Unix Default)" ) );
    submenu->Append(wxID_ANY,_("End-of-Line Mode"),eolmenu);
    submenu->AppendCheckItem( id_et_ShowEOL,    _( "Show EOL Chars" ),            _( "Show End-of-Line Characters" ) );
    submenu->Append( id_et_StripTrailingBlanks, _( "Strip Trailing Blanks Now" ), _( "Strip trailing blanks from each line" ) );
    submenu->Append( id_et_EnsureConsistentEOL, _( "Make EOLs Consistent Now" ),  _( "Convert End-of-Line Characters to the Active Setting" ) );
    submenu->AppendSeparator();
    submenu->AppendCheckItem( id_et_SuppressInsertKey, _("Suppress Insert Key"),     _("Disable toggle between insert and overwrite mode using the insert key") );
    submenu->AppendCheckItem( id_et_ConvertBraces,     _("Convert Matching Braces"), _("Selecting a brace and typing a new brace character will change the matching brace appropriately") );
    submenu->AppendSeparator();
    submenu->AppendCheckItem( id_et_align_last,        _("Last Align"), _("repeat last Align command") );
    submenu->AppendCheckItem( id_et_align_auto,        _("Auto Align"), _("Align lines automatically") );


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
    if ( !IsAttached() )
        return;

    if (!m_tweakmenu)
    	return;

    cbStyledTextCtrl* control = GetSafeControl();
    if (!control)
    {
        m_tweakmenuitem->Enable(false);
        return;
    }

    m_tweakmenuitem->Enable(true);

    wxMenu *submenu = m_tweakmenu; //_("Editor Tweaks") TODO: Retrieve actual menu

    submenu->Check(id_et_WordWrap,control->GetWrapMode()==wxSCI_WRAP_WORD);
    submenu->Check(id_et_CharWrap,control->GetWrapMode()==wxSCI_WRAP_CHAR);
    submenu->Check(id_et_ShowLineNumbers,control->GetMarginWidth(0)>0);
    submenu->Check(id_et_TabChar,control->GetUseTabs());
    submenu->Check(id_et_TabIndent,control->GetTabIndents());
    submenu->Check(id_et_TabSize2,control->GetTabWidth()==2);
    submenu->Check(id_et_TabSize4,control->GetTabWidth()==4);
    submenu->Check(id_et_TabSize6,control->GetTabWidth()==6);
    submenu->Check(id_et_TabSize8,control->GetTabWidth()==8);
    submenu->Check(id_et_EOLCRLF,control->GetEOLMode()==wxSCI_EOL_CRLF);
    submenu->Check(id_et_EOLCR,control->GetEOLMode()==wxSCI_EOL_CR);
    submenu->Check(id_et_EOLLF,control->GetEOLMode()==wxSCI_EOL_LF);
    submenu->Check(id_et_ShowEOL,control->GetViewEOL());
    submenu->Check(id_et_SuppressInsertKey, m_suppress_insert);
    submenu->Check(id_et_ConvertBraces,     m_convert_braces);

}

void EditorTweaks::OnUpdateUI(wxUpdateUIEvent &/*event*/)
{
    UpdateUI();
}

void EditorTweaks::OnEditorOpen(CodeBlocksEvent& /*event*/)
{
    Manager::Get()->GetLogManager()->DebugLog(wxString::Format(_("Editor Open")));
    cbStyledTextCtrl* control = GetSafeControl();
    if (!control)
        return;

    control->SetOvertype(false);
    control->Connect(wxEVT_KEY_DOWN,(wxObjectEventFunction) (wxEventFunction) (wxCharEventFunction)&EditorTweaks::OnKeyPress,NULL,this);
    control->Connect(wxEVT_CHAR,(wxObjectEventFunction) (wxEventFunction) (wxCharEventFunction)&EditorTweaks::OnChar,NULL,this);

}

void EditorTweaks::OnKeyPress(wxKeyEvent& event)
{
    const int keyCode = event.GetKeyCode();
    switch (keyCode)
    {
    case WXK_NUMPAD_UP:      case WXK_UP:
        if (event.GetModifiers() != wxMOD_CONTROL)
            DoBufferEditorPos(-1);
        break;

    case WXK_NUMPAD_DOWN:    case WXK_DOWN:
        if (event.GetModifiers() == wxMOD_CONTROL)
            break;
        // fall through
    case WXK_NUMPAD_ENTER:   case WXK_RETURN:
        DoBufferEditorPos(1);
        break;

    case WXK_NUMPAD_TAB:     case WXK_TAB:
        if (event.GetModifiers() != wxMOD_NONE)
            break;
        // fall through
    case WXK_BACK:
    case WXK_NUMPAD_DELETE:  case WXK_DELETE:
    case WXK_NUMPAD_LEFT:    case WXK_LEFT:
    case WXK_NUMPAD_RIGHT:   case WXK_RIGHT:
        if (event.GetModifiers() == wxMOD_ALT)
            break;
        // fall through
    case WXK_NUMPAD_HOME:    case WXK_HOME:
    case WXK_NUMPAD_END:     case WXK_END:
        DoBufferEditorPos();
        break;

    default:
        break;
    }
    if (m_suppress_insert && keyCode == WXK_INSERT && event.GetModifiers() == wxMOD_NONE)
        event.Skip(false);
    else
        event.Skip(true);
    if (m_convert_braces && keyCode == WXK_DELETE && (event.GetModifiers() == wxMOD_NONE || event.GetModifiers() == wxMOD_SHIFT))
    {
        event.Skip(true);

        cbStyledTextCtrl* control = GetSafeControl();
        if (!control)
            return;

        int p = control->GetCurrentPos();
        int a = control->GetAnchor();
        if (abs(p-a) != 1)
            return;
        int l = a<p? a: p;
        int m = control->BraceMatch(l);
        if (m == wxSCI_INVALID_POSITION)
            return;
        control->BeginUndoAction();
        if(l<m)
        {
            control->DeleteRange(m, 1);
            control->DeleteRange(l, 1);
        }
        else
        {
            control->DeleteRange(l, 1);
            control->DeleteRange(m, 1);
            l--;
        }
        control->SetCurrentPos(l);
        control->SetAnchor(l);
        control->EndUndoAction();
        event.Skip(false);
    }
}

void EditorTweaks::OnChar(wxKeyEvent& event)
{
    event.Skip(true);
    DoBufferEditorPos();
    wxChar ch = event.GetKeyCode();
    if (m_convert_braces &&
            (ch == _T('(') ||
             ch == _T(')') ||
             ch == _T('[') ||
             ch == _T(']') ||
             ch == _T('<') ||
             ch == _T('>') ||
             ch == _T('{') ||
             ch == _T('}')
             ))
    {
        event.Skip(true);

        cbStyledTextCtrl* control = GetSafeControl();
        if (!control)
            return;

        int p = control->GetCurrentPos();
        int a = control->GetAnchor();
        if (abs(p-a) != 1)
            return;
        int l = a<p? a: p;
        wxString opch;
        switch (ch)
        {
            case _T('('):
                opch = _T(")");
                break;
            case _T(')'):
                opch = _T("(");
                break;
            case _T('['):
                opch = _T("]");
                break;
            case _T(']'):
                opch = _T("[");
                break;
            case _T('<'):
                opch = _T(">");
                break;
            case _T('>'):
                opch = _T("<");
                break;
            case _T('{'):
                opch = _T("}");
                break;
            case _T('}'):
                opch = _T("{");
                break;
            default:
              return;
        }
        int m = control->BraceMatch(l);
        if (m == wxSCI_INVALID_POSITION)
            return;
        control->BeginUndoAction();
        control->InsertText(l, wxString(ch,1));
        control->DeleteRange(l+1, 1);
        control->InsertText(m, opch);
        control->DeleteRange(m+1, 1);
        control->SetCurrentPos(p);
        control->SetAnchor(a);
        control->EndUndoAction();
        event.Skip(false);
    }
}

void EditorTweaks::OnSuppressInsert(wxCommandEvent& event)
{
    m_suppress_insert = event.IsChecked();
}

void EditorTweaks::OnConvertBraces(wxCommandEvent& event)
{
    m_convert_braces = event.IsChecked();
}


void EditorTweaks::BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* /*data*/)
{
    //Some library module is ready to display a pop-up menu.
    //Check the parameter \"type\" and see which module it is
    //and append any items you need in the menu...
    //TIP: for consistency, add a separator as the first item...

    //make sure we have an editor
    if (type != mtEditorManager || !m_tweakmenu)
        return;

    cbStyledTextCtrl* control = GetSafeControl();
    if (!control)
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
    alignerMenu->Append(id_et_align_auto,   _("Auto"));
    alignerMenu->Append(id_et_align_others, _("More ..."));

    // attach aligner menu
    menu->AppendSeparator();
    menu->Append(wxID_ANY, _T("Aligner"), alignerMenu);

    return;

// DISABLED ALL OF THE STUFF BELOW (IT IS ALREADY IN THE MAIN MENU BAR)
#if 0
    // build "editor tweaks" menu
    wxMenu *submenu=new wxMenu(); //_("Editor Tweaks")

    menu->Append(id_et,_("Editor Tweaks"),submenu);

    submenu->AppendCheckItem( id_et_WordWrap, _( "Word wrap" ), _( "Wrap word" ) );
    if (control->GetWrapMode()==wxSCI_WRAP_WORD)
        submenu->Check(id_et_WordWrap,true);

    submenu->AppendCheckItem( id_et_CharWrap, _( "Char wrap" ), _( "Wrap char" ) );
    if (control->GetWrapMode()==wxSCI_WRAP_CHAR)
        submenu->Check(id_et_CharWrap,true);

    submenu->AppendCheckItem( id_et_ShowLineNumbers, _( "Show Line Numbers" ), _( "Show Line Numbers" ) );
    if (control->GetMarginWidth(0)>0)
        submenu->Check(id_et_ShowLineNumbers,true);

    submenu->AppendSeparator();

    submenu->AppendCheckItem( id_et_TabChar, _( "Use Tab Character" ), _( "Use Tab Character" ) );
    if (control->GetUseTabs())
        submenu->Check(id_et_TabChar,true);

    submenu->AppendCheckItem( id_et_TabIndent, _( "Tab Indents" ), _( "Tab Indents" ) );
    if (control->GetTabIndents())
        submenu->Check(id_et_TabIndent,true);

    wxMenu *tabsizemenu=new wxMenu();
    tabsizemenu->AppendRadioItem( id_et_TabSize2, _( "2" ), _( "Tab Width of 2" ) );
    if (control->GetTabWidth()==2)
        tabsizemenu->Check(id_et_TabSize2,true);
    tabsizemenu->AppendRadioItem( id_et_TabSize4, _( "4" ), _( "Tab Width of 4" ) );
    if (control->GetTabWidth()==4)
        tabsizemenu->Check(id_et_TabSize4,true);
    tabsizemenu->AppendRadioItem( id_et_TabSize6, _( "6" ), _( "Tab Width of 6" ) );
    if (control->GetTabWidth()==6)
        tabsizemenu->Check(id_et_TabSize6,true);
    tabsizemenu->AppendRadioItem( id_et_TabSize8, _( "8" ), _( "Tab Width of 8" ) );
    if (control->GetTabWidth()==8)
        tabsizemenu->Check(id_et_TabSize8,true);
    submenu->Append(wxID_ANY,_("Tab Size"),tabsizemenu);

    submenu->AppendSeparator();

    wxMenu *eolmenu=new wxMenu();
    eolmenu->AppendRadioItem( id_et_EOLCRLF, _( "CR LF" ), _( "Carriage Return - Line Feed (Windows Default)" ) );
    if (control->GetEOLMode()==wxSCI_EOL_CRLF)
        eolmenu->Check(id_et_EOLCRLF,true);
    eolmenu->AppendRadioItem( id_et_EOLCR, _( "CR" ), _( "Carriage Return (Mac Default)" ) );
    if (control->GetEOLMode()==wxSCI_EOL_CR)
        eolmenu->Check(id_et_EOLCR,true);
    eolmenu->AppendRadioItem( id_et_EOLLF, _( "LF" ), _( "Line Feed (Unix Default)" ) );
    if (control->GetEOLMode()==wxSCI_EOL_LF)
        eolmenu->Check(id_et_EOLLF,true);
    submenu->Append(wxID_ANY,_("End-of-Line Mode"),eolmenu);

    submenu->AppendCheckItem( id_et_ShowEOL, _( "Show EOL Chars" ), _( "Show End-of-Line Characters" ) );
    if (control->GetViewEOL())
        submenu->Check(id_et_ShowEOL,true);

    submenu->Append( id_et_StripTrailingBlanks, _( "Strip Trailing Blanks Now" ), _( "Strip trailing blanks from each line" ) );

    submenu->Append( id_et_EnsureConsistentEOL, _( "Make EOLs Consistent Now" ), _( "Convert End-of-Line Characters to the Active Setting" ) );

    menu->Append(wxID_ANY, _T("Editor Tweaks"), submenu);
#endif
}

void EditorTweaks::OnWordWrap(wxCommandEvent &/*event*/)
{
    cbStyledTextCtrl* control = GetSafeControl();
    if (!control)
        return;

    bool enabled = control->GetWrapMode() == wxSCI_WRAP_WORD;

    if (enabled)
        control->SetWrapMode(wxSCI_WRAP_NONE);
    else
        control->SetWrapMode(wxSCI_WRAP_WORD);
}

void EditorTweaks::OnCharWrap(wxCommandEvent &/*event*/)
{
    cbStyledTextCtrl* control = GetSafeControl();
    if (!control)
        return;

    bool enabled = control->GetWrapMode() == wxSCI_WRAP_CHAR;

    if (enabled)
        control->SetWrapMode(wxSCI_WRAP_NONE);
    else
        control->SetWrapMode(wxSCI_WRAP_CHAR);
}

void EditorTweaks::OnShowLineNumbers(wxCommandEvent &/*event*/)
{
    cbStyledTextCtrl* control = GetSafeControl();

    bool enabled=control->GetMarginWidth(0)>0;

//    bool old_state=mgr->ReadBool(_T("/show_line_numbers"), true);
//    mgr->Write(_T("/show_line_numbers"), !enabled);
//    ed->SetEditorStyleAfterFileOpen();
//    mgr->Write(_T("/show_line_numbers"), old_state);

    if (enabled)
        control->SetMarginWidth(0, 0);
    else
    {
        ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));
        int pixelWidth = control->TextWidth(wxSCI_STYLE_LINENUMBER, _T("9"));

        if (cfg->ReadBool(_T("/margin/dynamic_width"), false))
        {
            int lineNumWidth = 1;
            int lineCount = control->GetLineCount();

            while (lineCount >= 10)
            {
                lineCount /= 10;
                ++lineNumWidth;
            }

            control->SetMarginWidth(0, 6 + lineNumWidth * pixelWidth);
        }
        else
            control->SetMarginWidth(0, 6 + cfg->ReadInt(_T("/margin/width_chars"), 6) * pixelWidth);
    }
}

void EditorTweaks::OnTabChar(wxCommandEvent &/*event*/)
{
    cbStyledTextCtrl* control = GetSafeControl();
    if (!control)
        return;

    control->SetUseTabs(!control->GetUseTabs());
}

void EditorTweaks::OnTabIndent(wxCommandEvent &/*event*/)
{
    cbStyledTextCtrl* control = GetSafeControl();
    if (!control)
        return;

    control->SetTabIndents(!control->GetTabIndents());
}

void EditorTweaks::OnTabSize2(wxCommandEvent &/*event*/)
{
    cbStyledTextCtrl* control = GetSafeControl();
    if (!control)
        return;

    control->SetTabWidth(2);
}

void EditorTweaks::OnTabSize4(wxCommandEvent &/*event*/)
{
    cbStyledTextCtrl* control = GetSafeControl();
    if (!control)
        return;

    control->SetTabWidth(4);
}

void EditorTweaks::OnTabSize6(wxCommandEvent &/*event*/)
{
    cbStyledTextCtrl* control = GetSafeControl();
    if (!control)
        return;

    control->SetTabWidth(6);
}

void EditorTweaks::OnTabSize8(wxCommandEvent &/*event*/)
{
    cbStyledTextCtrl* control = GetSafeControl();
    if (!control)
        return;

    control->SetTabWidth(8);
}

void EditorTweaks::OnMakeIndentsConsistent(wxCommandEvent& /*event*/)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed)
        return;

    MakeIndentsConsistent(ed);
}
void EditorTweaks::MakeIndentsConsistent(cbEditor* ed)
{
    cbStyledTextCtrl* stc = ed->GetControl();

    const bool useTab     = stc->GetUseTabs();
    const int  tabWidth   = stc->GetTabWidth();
    const int  maxLines   = stc->GetLineCount();
    bool  changed         = false;
    for (int curLine = 0; curLine < maxLines; ++curLine)
    {
        const wxString curInd = ed->GetLineIndentString(curLine);
        wxString indent = curInd;
        if (useTab)
            indent.Replace(wxString(wxT(' '), tabWidth), wxT("\t"));
        else
            indent.Replace(wxT("\t"), wxString(wxT(' '), tabWidth));
        if (indent != curInd)
        {
            if (!changed) // all changes in a single undo step
            {
                stc->BeginUndoAction();
                changed = true;
            }
            stc->SetTargetStart(stc->PositionFromLine(curLine));
            stc->SetTargetEnd(stc->PositionFromLine(curLine) + curInd.Length());
            stc->ReplaceTarget(indent);
        }
    }
    if (changed)
        stc->EndUndoAction();
}

void EditorTweaks::OnShowEOL(wxCommandEvent &/*event*/)
{
    cbStyledTextCtrl* control = GetSafeControl();
    if (!control)
        return;

    control->SetViewEOL(!control->GetViewEOL());
}

void EditorTweaks::OnStripTrailingBlanks(wxCommandEvent &/*event*/)
{
    cbStyledTextCtrl* control = GetSafeControl();
    if (!control)
            return;

    StripTrailingBlanks(control);
}

void EditorTweaks::StripTrailingBlanks(cbStyledTextCtrl* control)
{
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
    cbStyledTextCtrl* control = GetSafeControl();
    if (!control)
            return;

    control->ConvertEOLs(control->GetEOLMode());
}

void EditorTweaks::OnEOLCRLF(wxCommandEvent &/*event*/)
{
    cbStyledTextCtrl* control = GetSafeControl();
    if (!control)
        return;;

    control->SetEOLMode(wxSCI_EOL_CRLF);
}

void EditorTweaks::OnEOLCR(wxCommandEvent &/*event*/)
{
    cbStyledTextCtrl* control = GetSafeControl();
    if (!control)
        return;

    control->SetEOLMode(wxSCI_EOL_CR);
}

void EditorTweaks::OnEOLLF(wxCommandEvent &/*event*/)
{
    cbStyledTextCtrl* control = GetSafeControl();
    if (!control)
        return;

    control->SetEOLMode(wxSCI_EOL_LF);
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
    cbStyledTextCtrl* control = GetSafeControl();
    if (!control)
        return;

    level+=wxSCI_FOLDLEVELBASE;

    control->Colourise(0, -1); // the *most* important part!

	// Scan all file lines searching for the specified folding level.
    int count = control->GetLineCount();
    for (int line = 0; line <= count; ++line)
    {
        int line_level_data = control->GetFoldLevel(line);
        if (!(line_level_data & wxSCI_FOLDLEVELHEADERFLAG))
            continue;
        const int line_level = line_level_data & wxSCI_FOLDLEVELNUMBERMASK;

        const bool IsExpanded = control->GetFoldExpanded(line);

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
        control->ToggleFold(line);
    }
}

void EditorTweaks::OnAlign(wxCommandEvent& event)
{
    int id = event.GetId();
    for ( unsigned int i = 0 ; i < AlignerMenuEntries.size(); i++)
    {
        if ( AlignerMenuEntries[i].id == id )
        {
            OnAlign(i);
            break;
        }
    }
}
void EditorTweaks::OnAlign(unsigned int idx)
{
    if (idx >= AlignerMenuEntries.size())
        return;
    AlignToString(AlignerMenuEntries[idx].ArgumentString);
    AlignerMenuEntries[idx].UsageCount ++;

    AlignerLastUsedIdx = idx;
    AlignerLastUsedAuto = false;
    AlignerLastUsed = true;
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

void EditorTweaks::OnAlignLast(wxCommandEvent& event)
{
    if(!AlignerLastUsed)
        return;

    if (AlignerLastUsedAuto)
        OnAlignAuto();
    else
        OnAlign(AlignerLastUsedIdx);
}

void EditorTweaks::OnAlignAuto(wxCommandEvent& WXUNUSED(event))
{
    OnAlignAuto();
}
void EditorTweaks::OnAlignAuto()
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed)
        return;
    cbStyledTextCtrl* stc = ed->GetControl();
    if (!stc)
        return;

    int line_start = wxSCI_INVALID_POSITION;
    int line_end   = wxSCI_INVALID_POSITION;
    if (!GetSelectionLines(line_start, line_end))
        return;
    wxArrayString lines;
    for (int i = line_start; i <= line_end; ++i)
        lines.Add(stc->GetLine(i));
    if (lines.GetCount() < 2)
        return;
    int lexer = stc->GetLexer();
    wxArrayString out;
    for (size_t i = 0; i < lines.GetCount(); ++i)
    {
        lines[i].Replace(wxT("\t"), wxT(" "));
        // buffer assignment operators and commas in C++
        if (lexer == wxSCI_LEX_CPP)
        {
            const wxString op = wxT("=<>!+-*/%&^| "); // do not split compound operators
            for (int j = lines[i].Length() - 2; j >= 0; --j)
            {
                if (   lines[i][j] == wxT(',')
                    || (lines[i][j] == wxT('=') && lines[i][j + 1] != wxT('='))
                    || (lines[i][j + 1] == wxT('=') && op.Find(lines[i][j]) == wxNOT_FOUND) )
                {
                    lines[i].insert(j + 1, wxT(' '));
                }
            }
        }
        // initialize output strings with their current indentation
        out.Add(ed->GetLineIndentString(line_start + i));
    }
    // loop through number of columns
    size_t numCols = 1;
    for (size_t i = 0; i < numCols; ++i)
    {
        // add the next token
        for (size_t j = 0; j < lines.GetCount(); ++j)
        {
            wxArrayString lnParts = GetArrayFromString(lines[j], wxT(" "));
            if (i < lnParts.GetCount())
                out[j].Append(lnParts[i]);
            // set actual number of columns
            if (lnParts.GetCount() > numCols)
                numCols = lnParts.GetCount();
        }
        // find the column size
        size_t colPos = 0;
        for (size_t j = 0; j < out.GetCount(); ++j)
        {
            if (out[j].Length() > colPos)
                colPos = out[j].Length();
        }
        // buffer output lines to column size + 1
        for (size_t j = 0; j < out.GetCount(); ++j)
        {
            while (out[j].Length() <= colPos)
                out[j].Append(wxT(' '));
        }
    }
    // replace only the lines that have been modified
    stc->BeginUndoAction();
    for (size_t i = 0; i < out.GetCount(); ++i)
    {
        stc->SetSelectionVoid(stc->PositionFromLine(line_start + i),
                              stc->GetLineEndPosition(line_start + i));
        if (stc->GetSelectedText() != out[i].Trim())
            stc->ReplaceSelection(out[i]);
    }
    stc->LineEnd(); // remove selection (if last line was not replaced)
    stc->EndUndoAction();

    AlignerLastUsedAuto = true;
    AlignerLastUsed = true;
}

void EditorTweaks::AlignToString(const wxString AlignmentString)
{
    cbStyledTextCtrl* control = GetSafeControl();
    if (!control)
        return;

    int line_start = wxSCI_INVALID_POSITION;
    int line_end   = wxSCI_INVALID_POSITION;
    if (GetSelectionLines(line_start, line_end))
    {
        // get furthest position of alignmentstring
        size_t find_position  = wxString::npos;
        size_t max_position   = wxString::npos;
        int matches           = 0;
        for (int i=line_start; i<=line_end; i++)
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
            for (int i=line_start; i<=line_end; i++)
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

void EditorTweaks::DoBufferEditorPos(int delta, bool isScrollTimer)
{
    if (m_buffer_caret == -1)
        m_buffer_caret = Manager::Get()->GetConfigManager(wxT("EditorTweaks"))->ReadInt(wxT("/buffer_caret"), 1);
    if (m_buffer_caret < 1) // feature disabled (selected "None" in settings)
        return;

    cbStyledTextCtrl* stc = GetSafeControl();
    if (!stc)
        return;

    if (!stc || stc->AutoCompActive() || stc->LinesOnScreen() < 10) // ignore small editors
        return;
    const int firstVisibleLine = stc->GetFirstVisibleLine();
    const int dist = stc->VisibleFromDocLine(stc->GetCurrentLine()) + delta - firstVisibleLine;
    if (dist < 0 || dist > stc->LinesOnScreen()) // caret is off screen (see bug #18795)
    {
        if (!isScrollTimer && !m_scrollTimer.IsRunning())
            m_scrollTimer.Start(5, wxTIMER_ONE_SHOT); // check to see if we moved into place
        return;
    }
    const int buffer = (m_buffer_caret > 4 ? (stc->LinesOnScreen() >> 1) - 2 : m_buffer_caret);
    int remaining = 0;
    if (dist < buffer)
    {
        remaining = buffer - dist - 1;
        stc->LineScroll(0, (remaining > 3 ? -2 : -1)); // scroll up
    }
    else if (dist >= stc->LinesOnScreen() - buffer)
    {
        remaining = dist + buffer - stc->LinesOnScreen();
        stc->LineScroll(0, (remaining > 3 ? 2 : 1)); // scroll down
    }
    if (!m_scrollTimer.IsRunning() && remaining > 0 && firstVisibleLine != stc->GetFirstVisibleLine())
        m_scrollTimer.Start(4 + (30 / remaining), wxTIMER_ONE_SHOT); // smooth scroll required lines
}

void EditorTweaks::OnScrollTimer(wxTimerEvent& WXUNUSED(event))
{
    DoBufferEditorPos(0, true);
}

cbStyledTextCtrl* EditorTweaks::GetSafeControl()
{
    if ( !IsAttached() )
        return nullptr;

    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    return ed ? ed->GetControl() : nullptr;
}

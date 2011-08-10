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
    #include <algorithm>
    #include <iterator>
    #include <set> // for handling unique items in some places

    #include <wx/choicdlg.h>
    #include <wx/choice.h>
    #include <wx/dir.h>
    #include <wx/filename.h>
    #include <wx/fs_zip.h>
    #include <wx/menu.h>
    #include <wx/mimetype.h>
    #include <wx/msgdlg.h>
    #include <wx/regex.h>
    #include <wx/tipwin.h>
    #include <wx/toolbar.h>
    #include <wx/utils.h>
    #include <wx/xrc/xmlres.h>
    #include <wx/wxscintilla.h>

    #include <cbeditor.h>
    #include <configmanager.h>
    #include <editorcolourset.h>
    #include <editormanager.h>
    #include <globals.h>
    #include <logmanager.h>
    #include <macrosmanager.h>
    #include <manager.h>
    #include <projectmanager.h>
    #include <sdk_events.h>
#endif

#include <wx/tokenzr.h>

#include <cbstyledtextctrl.h>
#include <editor_hooks.h>
#include <incrementalselectlistdlg.h>
#include <multiselectdlg.h>

#include "codecompletion.h"

#include "insertclassmethoddlg.h"
#include "ccoptionsdlg.h"
#include "ccoptionsprjdlg.h"
#include "parser/parser.h"
#include "parser/tokenizer.h"
#include "selectincludefile.h"

#define CC_CODECOMPLETION_DEBUG_OUTPUT 0

#if (CC_GLOBAL_DEBUG_OUTPUT)
    #undef CC_CODECOMPLETION_DEBUG_OUTPUT
    #define CC_CODECOMPLETION_DEBUG_OUTPUT 1
#endif

#if CC_CODECOMPLETION_DEBUG_OUTPUT == 1
    #define TRACE(format, args...) \
        CCLogger::Get()->DebugLog(F(format, ##args))
    #define TRACE2(format, args...)
#elif CC_CODECOMPLETION_DEBUG_OUTPUT == 2
    #define TRACE(format, args...)                                              \
        do                                                                      \
        {                                                                       \
            if (g_EnableDebugTrace)                                             \
                CCLogger::Get()->DebugLog(F(format, ##args));                   \
        }                                                                       \
        while (false)
    #define TRACE2(format, args...) \
        CCLogger::Get()->DebugLog(F(format, ##args))
#else
    #define TRACE(format, args...)
    #define TRACE2(format, args...)
#endif

static wxCriticalSection s_HeadersCriticalSection;
static wxString g_GlobalScope(_T("<global>"));
const int g_EditorActivatedDelay = 300;

// this auto-registers the plugin
namespace
{
    PluginRegistrant<CodeCompletion> reg(_T("CodeCompletion"));
}

// empty bitmap for use as C++ keywords icon in code-completion list
/* XPM */
static const char * cpp_keyword_xpm[] = {
"16 16 2 1",
"     c None",
".    c #04049B",
"                ",
"  .......       ",
" .........      ",
" ..     ..      ",
"..              ",
"..   ..     ..  ",
"..   ..     ..  ",
".. ...... ......",
".. ...... ......",
"..   ..     ..  ",
"..   ..     ..  ",
"..      ..      ",
"...     ..      ",
" .........      ",
"  .......       ",
"                "};

// bitmap for use as D keywords icon in code-completion list
/* XPM */
static const char *d_keyword_xpm[] = {
/* width height num_colors chars_per_pixel */
"    14    14      6            1",
/* colors */
"  c none",
". c #fefefe",
"# c #e43a3a",
"a c #e40000",
"b c #e48f8f",
"c c #8f0000",
/* pixels */
"              ",
"              ",
"  .#aaaa#b.   ",
"  baabb#aa#   ",
"  ba#   baa#  ",
"  ba#    bcab ",
"  ba#     #a# ",
"  ba#     bac ",
"  ba#     ba# ",
"  ba#     bc# ",
"  ba#     #cb ",
"  bcc    ac#  ",
"  #aa###ac#   ",
"  cccccc#b    "
};


// bitmap for other-than-C++ keywords
// it's pretty nice actually :)
/* XPM */
static const char * unknown_keyword_xpm[] = {
"16 16 7 1",
"     c None",
".    c #FF8800",
"+    c #FF8D0B",
"@    c #FF9115",
"#    c #FFA948",
"$    c #FFC686",
"%    c #FFFFFF",
"                ",
"                ",
"      ....      ",
"    ........    ",
"   ..+@+.....   ",
"   .+#$#+....   ",
"  ..@$%$@.....  ",
"  ..+#$#+.....  ",
"  ...+@+......  ",
"  ............  ",
"   ..........   ",
"   ..........   ",
"    ........    ",
"      ....      ",
"                ",
"                "};

// menu IDS
// just because we don't know other plugins' used identifiers,
// we use wxNewId() to generate a guaranteed unique ID ;), instead of enum
// (don't forget that, especially in a plugin)
int idMenuCodeComplete          = wxNewId();
int idMenuShowCallTip           = wxNewId();
int idMenuGotoFunction          = wxNewId();
int idMenuGotoPrevFunction      = wxNewId();
int idMenuGotoNextFunction      = wxNewId();
int idMenuGotoDeclaration       = wxNewId();
int idMenuGotoImplementation    = wxNewId();
int idMenuOpenIncludeFile       = wxNewId();
int idMenuFindReferences        = wxNewId();
int idMenuRenameSymbols         = wxNewId();
int idViewClassBrowser          = wxNewId();
int idCurrentProjectReparse     = wxNewId();
int idSelectedProjectReparse    = wxNewId();
int idSelectedFileReparse       = wxNewId();
int idEditorSubMenu             = wxNewId();
int idClassMethod               = wxNewId();
int idUnimplementedClassMethods = wxNewId();
int idGotoDeclaration           = wxNewId();
int idGotoImplementation        = wxNewId();
int idOpenIncludeFile           = wxNewId();
int idStartParsingProjects      = wxNewId();
int idCodeCompleteTimer         = wxNewId();
int idFunctionsParsingTimer     = wxNewId();
int idRealtimeParsingTimer      = wxNewId();
int idToolbarTimer              = wxNewId();
int idProjectSavedTimer         = wxNewId();
int idReparsingTimer            = wxNewId();
int idTimerEditorActivated      = wxNewId();

int idThreadUpdate              = wxNewId();
int idThreadCompleted           = wxNewId();
int idThreadError               = wxNewId();
int idCCLogger                  = wxNewId();
int idCCDebugLogger             = wxNewId();

// milliseconds
#define REALTIME_PARSING_DELAY      500
#define EDITOR_AND_LINE_INTERVAL    150

BEGIN_EVENT_TABLE(CodeCompletion, cbCodeCompletionPlugin)
    EVT_UPDATE_UI_RANGE(idMenuCodeComplete, idCurrentProjectReparse, CodeCompletion::OnUpdateUI)

    EVT_MENU(idMenuCodeComplete,          CodeCompletion::OnCodeComplete             )
    EVT_MENU(idMenuShowCallTip,           CodeCompletion::OnShowCallTip              )
    EVT_MENU(idMenuGotoFunction,          CodeCompletion::OnGotoFunction             )
    EVT_MENU(idMenuGotoPrevFunction,      CodeCompletion::OnGotoPrevFunction         )
    EVT_MENU(idMenuGotoNextFunction,      CodeCompletion::OnGotoNextFunction         )
    EVT_MENU(idMenuGotoDeclaration,       CodeCompletion::OnGotoDeclaration          )
    EVT_MENU(idMenuGotoImplementation,    CodeCompletion::OnGotoDeclaration          )
    EVT_MENU(idMenuFindReferences,        CodeCompletion::OnFindReferences           )
    EVT_MENU(idMenuRenameSymbols,         CodeCompletion::OnRenameSymbols            )
    EVT_MENU(idClassMethod,               CodeCompletion::OnClassMethod              )
    EVT_MENU(idUnimplementedClassMethods, CodeCompletion::OnUnimplementedClassMethods)
    EVT_MENU(idGotoDeclaration,           CodeCompletion::OnGotoDeclaration          )
    EVT_MENU(idGotoImplementation,        CodeCompletion::OnGotoDeclaration          )
    EVT_MENU(idOpenIncludeFile,           CodeCompletion::OnOpenIncludeFile          )
    EVT_MENU(idMenuOpenIncludeFile,       CodeCompletion::OnOpenIncludeFile          )

    EVT_MENU(idViewClassBrowser,       CodeCompletion::OnViewClassBrowser      )
    EVT_MENU(idCurrentProjectReparse,  CodeCompletion::OnCurrentProjectReparse )
    EVT_MENU(idSelectedProjectReparse, CodeCompletion::OnSelectedProjectReparse)
    EVT_MENU(idSelectedFileReparse,    CodeCompletion::OnSelectedFileReparse   )

    EVT_TIMER(idCodeCompleteTimer,     CodeCompletion::OnCodeCompleteTimer    )
    EVT_TIMER(idFunctionsParsingTimer, CodeCompletion::OnStartParsingFunctions)
    EVT_TIMER(idRealtimeParsingTimer,  CodeCompletion::OnRealtimeParsing      )
    EVT_TIMER(idToolbarTimer,          CodeCompletion::OnStartParsingFunctions)
    EVT_TIMER(idProjectSavedTimer,     CodeCompletion::OnProjectSavedTimer    )
    EVT_TIMER(idReparsingTimer,        CodeCompletion::OnReparsingTimer       )
    EVT_TIMER(idTimerEditorActivated,  CodeCompletion::OnEditorActivatedTimer )

    EVT_CHOICE(XRCID("chcCodeCompletionScope"),    CodeCompletion::OnScope   )
    EVT_CHOICE(XRCID("chcCodeCompletionFunction"), CodeCompletion::OnFunction)

    EVT_MENU(idThreadUpdate,    CodeCompletion::OnThreadUpdate    )
    EVT_MENU(idThreadCompleted, CodeCompletion::OnThreadCompletion)
    EVT_MENU(idThreadError,     CodeCompletion::OnThreadError     )
END_EVENT_TABLE()

class SystemHeadersThread : public wxThread
{
public:
    SystemHeadersThread(CodeCompletion* parent, SystemHeadersMap& headersMap, const wxArrayString& incDirs) :
        m_Parent(parent),
        m_SystemHeadersMap(headersMap),
        m_IncludeDirs(incDirs)
    {
        Create();
        SetPriority(60u);
    }

    virtual void* Entry()
    {
        wxArrayString dirs;
        {
            wxCriticalSectionLocker locker(s_HeadersCriticalSection);
            for (size_t i = 0; i < m_IncludeDirs.GetCount(); ++i)
            {
                if (m_SystemHeadersMap.find(m_IncludeDirs[i]) == m_SystemHeadersMap.end())
                {
                    dirs.Add(m_IncludeDirs[i]);
                    m_SystemHeadersMap[m_IncludeDirs[i]] = StringSet();
                }
            }
        }

        for (size_t i = 0; i < dirs.GetCount(); ++i)
        {
            wxDir dir(dirs[i]);
            if ( !dir.IsOpened() )
            {
                wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, idThreadError);
                evt.SetClientData(this);
                evt.SetString(wxString::Format(_T("SystemHeadersThread: Unable to open: %s"), dirs[i].wx_str()));
                wxPostEvent(m_Parent, evt);
                continue;
            }

            HeaderDirTraverser traverser(this, m_SystemHeadersMap, dirs[i]);
            dir.Traverse(traverser, wxEmptyString, wxDIR_FILES | wxDIR_DIRS);
            if ( TestDestroy() )
                break;

            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, idThreadUpdate);
            evt.SetClientData(this);
            evt.SetString(wxString::Format(_T("SystemHeadersThread: %s , %d"), dirs[i].wx_str(),
                                           m_SystemHeadersMap[dirs[i]].size()));
            wxPostEvent(m_Parent, evt);
        }

        if ( !TestDestroy() )
        {
            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, idThreadCompleted);
            evt.SetClientData(this);
            if (!dirs.IsEmpty())
                evt.SetString(wxString::Format(_T("SystemHeadersThread: Total number of paths: %d"), dirs.GetCount()));
            wxPostEvent(m_Parent, evt);
        }

        return NULL;
    }

private:
    CodeCompletion*   m_Parent;
    SystemHeadersMap& m_SystemHeadersMap;
    wxArrayString     m_IncludeDirs;

private:
    class HeaderDirTraverser : public wxDirTraverser
    {
    public:
        HeaderDirTraverser(wxThread* thread, SystemHeadersMap& headersMap, const wxString& searchDir) :
            m_Thread(thread),
            m_SystemHeadersMap(headersMap),
            m_SearchDir(searchDir),
            m_Headers(headersMap[searchDir]),
            m_Locked(false),
            m_Count(0)
        {}

        virtual ~HeaderDirTraverser()
        {
            if (m_Locked)
                 s_HeadersCriticalSection.Leave();
        }

        virtual wxDirTraverseResult OnFile(const wxString& filename)
        {
            if (m_Thread->TestDestroy())
                return wxDIR_STOP;

            AddLock();
            wxFileName fn(filename);
            if (!fn.HasExt() || fn.GetExt().GetChar(0) == _T('h'))
            {
                fn.MakeRelativeTo(m_SearchDir);
                wxString final(fn.GetFullPath());
                final.Replace(_T("\\"), _T("/"), true);
                m_Headers.insert(final);
            }

            return wxDIR_CONTINUE;
        }

        virtual wxDirTraverseResult OnDir(const wxString& dirname)
        {
            if (m_Thread->TestDestroy())
                return wxDIR_STOP;

            AddLock();
            wxString path(dirname);
            if (path.Last() != wxFILE_SEP_PATH)
                path.Append(wxFILE_SEP_PATH);

            if (m_SystemHeadersMap.find(path) != m_SystemHeadersMap.end())
                return wxDIR_IGNORE;
            return wxDIR_CONTINUE;
        }

        void AddLock()
        {
            if (++m_Count % 100 == 1)
            {
                if (m_Locked)
                {
                    s_HeadersCriticalSection.Leave();
                    m_Locked = false;
                }
                if (!m_Locked)
                {
                    s_HeadersCriticalSection.Enter();
                    m_Locked = true;
                }
            }
        }

    private:
        wxThread*               m_Thread;
        const SystemHeadersMap& m_SystemHeadersMap;
        const wxString&         m_SearchDir;
        StringSet&              m_Headers;
        bool                    m_Locked;
        size_t                  m_Count;
    };
};

CodeCompletion::CodeCompletion() :
    m_InitDone(false),
    m_CodeRefactoring(m_NativeParser),
    m_EditorHookId(0),
    m_TimerCodeCompletion(this, idCodeCompleteTimer),
    m_TimerFunctionsParsing(this, idFunctionsParsingTimer),
    m_TimerRealtimeParsing(this, idRealtimeParsingTimer),
    m_TimerToolbar(this, idToolbarTimer),
    m_TimerProjectSaved(this, idProjectSavedTimer),
    m_TimerReparsing(this, idReparsingTimer),
    m_TimerEditorActivated(this, idTimerEditorActivated),
    m_LastEditor(0),
    m_ActiveCalltipsNest(0),
    m_IsAutoPopup(false),
    m_ToolBar(0),
    m_Function(0),
    m_Scope(0),
    m_ToolbarChanged(true),
    m_CurrentLine(0),
    m_NeedReparse(false),
    m_CurrentLength(-1),
    m_UseCodeCompletion(true),
    m_CCAutoLaunchChars(3),
    m_CCAutoLaunch(true),
    m_CCLaunchDelay(300),
    m_CCMaxMatches(16384),
    m_CCAutoAddParentheses(true),
    m_CCAutoSelectOne(false),
    m_CCEnableHeaders(false)
{
    CCLogger::Get()->Init(this, idCCLogger, idCCDebugLogger);

    if (!Manager::LoadResource(_T("codecompletion.zip")))
        NotifyMissingFile(_T("codecompletion.zip"));

    Connect(idCCLogger, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CodeCompletion::OnCCLogger));
    Connect(idCCDebugLogger, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CodeCompletion::OnCCDebugLogger));
    Connect(idParserStart, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CodeCompletion::OnParserStart));
    Connect(idParserEnd, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CodeCompletion::OnParserEnd));
}

CodeCompletion::~CodeCompletion()
{
    Disconnect(idCCLogger, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CodeCompletion::OnCCLogger));
    Disconnect(idCCDebugLogger, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CodeCompletion::OnCCDebugLogger));
    Disconnect(idParserStart, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CodeCompletion::OnParserStart));
    Disconnect(idParserEnd, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CodeCompletion::OnParserEnd));

    while (!m_SystemHeadersThread.empty())
    {
        SystemHeadersThread* thread = m_SystemHeadersThread.front();
        m_SystemHeadersThread.pop_front();
        if (thread->IsAlive() && thread->IsRunning())
            thread->Delete();
    }
}

void CodeCompletion::LoadTokenReplacements()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("code_completion"));
    ConfigManagerContainer::StringToStringMap repl;
    cfg->Read(_T("token_replacements"), &repl);

    // for GCC
    repl[_T("_GLIBCXX_STD")]                    = _T("std");
    repl[_T("_GLIBCXX_STD_D")]                  = _T("std");
    repl[_T("_GLIBCXX_STD_P")]                  = _T("std");
    repl[_T("_GLIBCXX_BEGIN_NESTED_NAMESPACE")] = _T("+namespace std {");
    repl[_T("_GLIBCXX_END_NESTED_NAMESPACE")]   = _T("}");
    repl[_T("_GLIBCXX_BEGIN_NAMESPACE")]        = _T("+namespace std {");
    repl[_T("_GLIBCXX_END_NAMESPACE")]          = _T("}");
    repl[_T("_GLIBCXX_BEGIN_NAMESPACE_TR1")]    = _T("namespace tr1 {");
    repl[_T("_GLIBCXX_END_NAMESPACE_TR1")]      = _T("}");

    // for VC
    repl[_T("_STD_BEGIN")]                      = _T("namespace std {");
    repl[_T("_STD_END")]                        = _T("}");
    repl[_T("_STDEXT_BEGIN")]                   = _T("namespace std {");
    repl[_T("_STDEXT_END")]                     = _T("}");

    // for wxWidgets
    repl[_T("BEGIN_EVENT_TABLE")]               = _T("-END_EVENT_TABLE");
    repl[_T("WXDLLEXPORT")]                     = _T("");
    repl[_T("WXEXPORT")]                        = _T("");
    repl[_T("WXIMPORT")]                        = _T("");

    // apply
    Tokenizer::ConvertToHashReplacementMap(repl);
}

void CodeCompletion::SaveTokenReplacements()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("code_completion"));

    const wxStringHashMap& hashRepl = Tokenizer::GetTokenReplacementsMap();
    ConfigManagerContainer::StringToStringMap repl;
    wxStringHashMap::const_iterator it = hashRepl.begin();
    for (; it != hashRepl.end(); it++)
    {
        repl[it->first] = it->second;
    }
    cfg->Write(_T("token_replacements"), repl);
}

cbConfigurationPanel* CodeCompletion::GetConfigurationPanel(wxWindow* parent)
{
    CCOptionsDlg* dlg = new CCOptionsDlg(parent, &m_NativeParser, this);
    return dlg;
}

cbConfigurationPanel* CodeCompletion::GetProjectConfigurationPanel(wxWindow* parent, cbProject* project)
{
    return new CCOptionsProjectDlg(parent, project, &m_NativeParser);
}

int CodeCompletion::Configure()
{
    return 0;
}

void CodeCompletion::RereadOptions()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("code_completion"));

    m_LexerKeywordsToInclude[0] = cfg->ReadBool(_T("/lexer_keywords_set1"), true);
    m_LexerKeywordsToInclude[1] = cfg->ReadBool(_T("/lexer_keywords_set2"), true);
    m_LexerKeywordsToInclude[2] = cfg->ReadBool(_T("/lexer_keywords_set3"), false);
    m_LexerKeywordsToInclude[3] = cfg->ReadBool(_T("/lexer_keywords_set4"), false);
    m_LexerKeywordsToInclude[4] = cfg->ReadBool(_T("/lexer_keywords_set5"), false);
    m_LexerKeywordsToInclude[5] = cfg->ReadBool(_T("/lexer_keywords_set6"), false);
    m_LexerKeywordsToInclude[6] = cfg->ReadBool(_T("/lexer_keywords_set7"), false);
    m_LexerKeywordsToInclude[7] = cfg->ReadBool(_T("/lexer_keywords_set8"), false);
    m_LexerKeywordsToInclude[8] = cfg->ReadBool(_T("/lexer_keywords_set9"), false);

    // for CC
    m_UseCodeCompletion    = cfg->ReadBool(_T("/use_code_completion"), true);
    m_CCAutoLaunchChars    = cfg->ReadInt(_T("/auto_launch_chars"), 3);
    m_CCAutoLaunch         = cfg->ReadBool(_T("/auto_launch"), true);
    m_CCLaunchDelay        = cfg->ReadInt(_T("/cc_delay"), 300);
    m_CCMaxMatches         = cfg->ReadInt(_T("/max/matches"), 16384);
    m_CCAutoAddParentheses = cfg->ReadBool(_T("/auto_add_parentheses"), true);
    m_CCFillupChars        = cfg->Read(_T("/fillup_chars"), wxEmptyString);
    m_CCAutoSelectOne      = cfg->ReadBool(_T("/auto_select_one"), false);
    m_CCEnableHeaders      = cfg->ReadBool(_T("/enable_headers"), false);

    if (m_ToolBar)
    {
        UpdateToolBar();
        CodeBlocksLayoutEvent evt(cbEVT_UPDATE_VIEW_LAYOUT);
        Manager::Get()->ProcessEvent(evt);
        ParseFunctionsAndFillToolbar(true);
    }
}

void CodeCompletion::UpdateToolBar()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("code_completion"));
    bool showScope = cfg->ReadBool(_T("/scope_filter"), true);

    if (showScope && !m_Scope)
    {
        m_Scope = new wxChoice(m_ToolBar, wxNewId(), wxPoint(0, 0), wxSize(280, -1), 0, 0);
        m_ToolBar->InsertControl(0, m_Scope);
    }
    else if (!showScope && m_Scope)
    {
        m_ToolBar->DeleteTool(m_Scope->GetId());
        m_Scope = NULL;
    }
    else
        return;

    m_ToolBar->Realize();
    m_ToolBar->SetInitialSize();
}

void CodeCompletion::BuildMenu(wxMenuBar* menuBar)
{
    // if not attached, exit
    if (!IsAttached())
        return;

    int pos = menuBar->FindMenu(_("&Edit"));
    if (pos != wxNOT_FOUND)
    {
        m_EditMenu = menuBar->GetMenu(pos);
        m_EditMenu->AppendSeparator();

        const wxLanguageInfo* info = wxLocale::GetLanguageInfo(wxLANGUAGE_DEFAULT);
        if ( info && ( (   info->Language >= wxLANGUAGE_CHINESE
                && info->Language <= wxLANGUAGE_CHINESE_TAIWAN )
            || info->Language == wxLANGUAGE_JAPANESE
            || info->Language == wxLANGUAGE_KOREAN ) )
        {
            m_EditMenu->Append(idMenuCodeComplete, _("Complete code\tShift-Space"));
        }
        else
            m_EditMenu->Append(idMenuCodeComplete, _("Complete code\tCtrl-Space"));

        m_EditMenu->Append(idMenuShowCallTip, _("Show call tip\tCtrl-Shift-Space"));
        m_EditMenu->AppendSeparator();
        m_EditMenu->Append(idMenuRenameSymbols, _("Rename symbols\tAlt-N"));
    }
    else
        CCLogger::Get()->DebugLog(_T("Could not find Edit menu!"));

    pos = menuBar->FindMenu(_("Sea&rch"));
    if (pos != wxNOT_FOUND)
    {
        m_SearchMenu = menuBar->GetMenu(pos);
        m_SearchMenu->Append(idMenuGotoFunction, _("Goto function...\tCtrl-Alt-G"));
        m_SearchMenu->Append(idMenuGotoPrevFunction, _("Goto previous function\tCtrl-PgUp"));
        m_SearchMenu->Append(idMenuGotoNextFunction, _("Goto next function\tCtrl-PgDn"));
        m_SearchMenu->Append(idMenuGotoDeclaration, _("Goto declaration\tCtrl-Shift-."));
        m_SearchMenu->Append(idMenuGotoImplementation, _("Goto implementation\tCtrl-."));
        m_SearchMenu->Append(idMenuFindReferences, _("Find references\tAlt-."));
        m_SearchMenu->Append(idMenuOpenIncludeFile, _("Open include file\tCtrl-Alt-."));
    }
    else
        CCLogger::Get()->DebugLog(_T("Could not find Search menu!"));

    // add the classbrowser window in the "View" menu
    int idx = menuBar->FindMenu(_("&View"));
    if (idx != wxNOT_FOUND)
    {
        m_ViewMenu = menuBar->GetMenu(idx);
        wxMenuItemList& items = m_ViewMenu->GetMenuItems();
        bool inserted = false;

        // find the first separator and insert before it
        for (size_t i = 0; i < items.GetCount(); ++i)
        {
            if (items[i]->IsSeparator())
            {
                m_ViewMenu->InsertCheckItem(i, idViewClassBrowser, _("Symbols browser"), _("Toggle displaying the symbols browser"));
                inserted = true;
                break;
            }
        }

        // not found, just append
        if (!inserted)
            m_ViewMenu->AppendCheckItem(idViewClassBrowser, _("Symbols browser"), _("Toggle displaying the symbols browser"));
    }
    else
        CCLogger::Get()->DebugLog(_T("Could not find View menu!"));

    // add Reparse item in the "Project" menu
    idx = menuBar->FindMenu(_("&Project"));
    if (idx != wxNOT_FOUND)
    {
        m_ProjectMenu = menuBar->GetMenu(idx);
        wxMenuItemList& items = m_ProjectMenu->GetMenuItems();
        bool inserted = false;

        // find the first separator and insert before it
        for (size_t i = items.GetCount() - 1; i > 0; --i)
        {
            if (items[i]->IsSeparator())
            {
                m_ProjectMenu->InsertSeparator(i);
                m_ProjectMenu->Insert(i + 1, idCurrentProjectReparse, _("Reparse current project"), _("Reparse of the final switched project"));
                inserted = true;
                break;
            }
        }

        // not found, just append
        if (!inserted)
        {
            m_ProjectMenu->AppendSeparator();
            m_ProjectMenu->Append(idCurrentProjectReparse, _("Reparse current project"), _("Reparse of the final switched project"));
        }
    }
    else
        CCLogger::Get()->DebugLog(_T("Could not find Project menu!"));
}

wxChar GetLastNonWhitespaceChar(cbStyledTextCtrl* control, int position)
{
    if (!control)
        return 0;

    while (--position > 0)
    {
        const int style = control->GetStyleAt(position);
        if (control->IsComment(style))
            continue;

        const wxChar ch = control->GetCharAt(position);
        if (ch <= _T(' '))
            continue;

        return ch;
    }

    return 0;
}

wxChar GetNextNonWhitespaceChar(cbStyledTextCtrl* control, int position)
{
    if (!control)
        return 0;

    const int totalLength = control->GetLength();
    --position;
    while (++position < totalLength)
    {
        const int style = control->GetStyleAt(position);
        if (control->IsComment(style))
            continue;

        const wxChar ch = control->GetCharAt(position);
        if (ch <= _T(' '))
            continue;

        return ch;
    }

    return 0;
}

// invariant : on return true : NameUnderCursor is NOT empty
bool EditorHasNameUnderCursor(wxString& NameUnderCursor, bool& IsInclude)
{
    bool ReturnValue = false;
    if (cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor())
    {
        cbStyledTextCtrl* control = ed->GetControl();
        const int pos = control->GetCurrentPos();
        const wxString line = control->GetLine(control->LineFromPosition(pos));
        const wxRegEx reg(_T("^[ \t]*#[ \t]*include[ \t]+[\"<]([^\">]+)[\">]"));
        wxString inc;
        if (reg.Matches(line))
            inc = reg.GetMatch(line, 1);

        if (!inc.IsEmpty())
        {
            NameUnderCursor = inc;
            ReturnValue = true;
            IsInclude = true;
        }
        else
        {
            const int start = control->WordStartPosition(pos, true);
            const int end = control->WordEndPosition(pos, true);
            const wxString word = control->GetTextRange(start, end);
            if (!word.IsEmpty())
            {
                NameUnderCursor.Clear();
                if (GetLastNonWhitespaceChar(control, start) == _T('~'))
                    NameUnderCursor << _T('~');
                NameUnderCursor << word;
                ReturnValue = true;
                IsInclude = false;
            }
        }
    }
    return ReturnValue;
}

void CodeCompletion::BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data)
{
    // if not attached, exit
    if (!menu || !IsAttached() || !m_InitDone)
        return;

    if (type == mtEditorManager)
    {
        wxString NameUnderCursor;
        bool IsInclude = false;
        const bool nameUnderCursor = EditorHasNameUnderCursor(NameUnderCursor, IsInclude);
        if (nameUnderCursor)
        {
            if (IsInclude)
            {
                wxString msg;
                msg.Printf(_("Open #include file: '%s'"), NameUnderCursor.wx_str());
                menu->Insert(0, idOpenIncludeFile, msg);
                menu->Insert(1, wxID_SEPARATOR, wxEmptyString);
            }
            else
            {
                wxString msg;
                size_t pos = 0;
                msg.Printf(_("Find declaration of: '%s'"), NameUnderCursor.wx_str());
                menu->Insert(pos, idGotoDeclaration, msg);
                ++pos;

                msg.Printf(_("Find implementation of: '%s'"), NameUnderCursor.wx_str());
                menu->Insert(pos, idGotoImplementation, msg);
                ++pos;

                if (m_NativeParser.GetParser().Done())
                {
                    msg.Printf(_("Find references of: '%s'"), NameUnderCursor.wx_str());
                    menu->Insert(pos, idMenuFindReferences, msg);
                    ++pos;
                }

                menu->Insert(pos, wxID_SEPARATOR, wxEmptyString);
            }
        }

        const int insertId = menu->FindItem(_("Insert"));
        if (insertId != wxNOT_FOUND)
        {
            if (wxMenuItem* insertMenu = menu->FindItem(insertId, 0))
            {
                if (wxMenu* subMenu = insertMenu->GetSubMenu())
                {
                    subMenu->Append(idClassMethod, _("Class method declaration/implementation..."));
                    subMenu->Append(idUnimplementedClassMethods, _("All class methods without implementation..."));
                }
                else
                    CCLogger::Get()->DebugLog(_T("Could not find Insert menu 3!"));
            }
            else
                CCLogger::Get()->DebugLog(_T("Could not find Insert menu 2!"));
        }
        else
            CCLogger::Get()->DebugLog(_T("Could not find Insert menu!"));

        if (m_NativeParser.GetParser().Done() && nameUnderCursor && !IsInclude)
        {
            wxMenu* refactorMenu = new wxMenu();
            refactorMenu->Append(idMenuRenameSymbols, _("Rename symbols"), _("Rename symbols under cursor"));
            menu->AppendSeparator();
            menu->Append(wxID_ANY, _T("Code Refactoring"), refactorMenu);
        }
    }
    else if (type == mtProjectManager)
    {
        if (data)
        {
            if (data->GetKind() == FileTreeData::ftdkProject)
            {
                size_t position = menu->GetMenuItemCount();
                int id = menu->FindItem(_("Build"));
                if (id != wxNOT_FOUND)
                    menu->FindChildItem(id, &position);
                menu->Insert(position, idSelectedProjectReparse, _("Reparse this project"),
                             _("Reparse current actived project"));
                menu->InsertSeparator(position + 1);
            }
            else if (data->GetKind() == FileTreeData::ftdkFile)
                menu->Append(idSelectedFileReparse, _("Reparse this file"), _("Reparse current selected file"));
        }
    }
}

bool CodeCompletion::BuildToolBar(wxToolBar* toolBar)
{
    Manager::Get()->AddonToolBar(toolBar,_T("codecompletion_toolbar"));

    m_Function = XRCCTRL(*toolBar, "chcCodeCompletionFunction", wxChoice);
    m_Scope = XRCCTRL(*toolBar, "chcCodeCompletionScope", wxChoice);

    m_ToolBar = toolBar;
    UpdateToolBar();
    EnableToolbarTools(false);

    return true;
}

void CodeCompletion::OnAttach()
{
    m_PageIndex   = -1;
    m_EditMenu    = 0;
    m_SearchMenu  = 0;
    m_ViewMenu    = 0;
    m_ProjectMenu = 0;
    m_ToolBar     = 0;
    m_Function    = 0;
    m_Scope       = 0;
    m_FunctionsScope.clear();
    m_NameSpaces.clear();
    m_AllFunctionsScopes.clear();
    m_ToolbarChanged = true; // by default

    m_LastFile.clear();

    LoadTokenReplacements();
    RereadOptions();

    m_LastPosForCodeCompletion = -1;
    m_NativeParser.SetNextHandler(this);

    m_NativeParser.CreateClassBrowser();

    // hook to editors
    EditorHooks::HookFunctorBase* myhook = new EditorHooks::HookFunctor<CodeCompletion>(this, &CodeCompletion::EditorEventHook);
    m_EditorHookId = EditorHooks::RegisterHook(myhook);

    // register event sinks
    Manager* pm = Manager::Get();

    pm->RegisterEventSink(cbEVT_EDITOR_SAVE,          new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnEditorSaveOrModified));
    pm->RegisterEventSink(cbEVT_EDITOR_MODIFIED,      new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnEditorSaveOrModified));
    pm->RegisterEventSink(cbEVT_EDITOR_OPEN,          new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnEditorOpen));
    pm->RegisterEventSink(cbEVT_EDITOR_ACTIVATED,     new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnEditorActivated));
    pm->RegisterEventSink(cbEVT_EDITOR_TOOLTIP,       new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnValueTooltip));
    pm->RegisterEventSink(cbEVT_EDITOR_CLOSE,         new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnEditorClosed));

    pm->RegisterEventSink(cbEVT_APP_STARTUP_DONE,     new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnAppDoneStartup));
    pm->RegisterEventSink(cbEVT_WORKSPACE_CHANGED,    new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnWorkspaceChanged));
    pm->RegisterEventSink(cbEVT_PROJECT_ACTIVATE,     new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnProjectActivated));
    pm->RegisterEventSink(cbEVT_PROJECT_CLOSE,        new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnProjectClosed));
    pm->RegisterEventSink(cbEVT_PROJECT_SAVE,         new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnProjectSaved));
    pm->RegisterEventSink(cbEVT_PROJECT_FILE_ADDED,   new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnProjectFileAdded));
    pm->RegisterEventSink(cbEVT_PROJECT_FILE_REMOVED, new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnProjectFileRemoved));
    pm->RegisterEventSink(cbEVT_PROJECT_FILE_CHANGED, new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnProjectFileChanged));
}

void CodeCompletion::OnRelease(bool appShutDown)
{
    SaveTokenReplacements();

    m_NativeParser.RemoveClassBrowser(appShutDown);
    m_NativeParser.ClearParsers();

    // unregister hook
    // 'true' will delete the functor too
    EditorHooks::UnregisterHook(m_EditorHookId, true);

    // remove registered event sinks
    Manager::Get()->RemoveAllEventSinksFor(this);

    m_FunctionsScope.clear();
    m_NameSpaces.clear();
    m_AllFunctionsScopes.clear();
    m_ToolbarChanged = false;

/* TODO (mandrav#1#): Delete separator line too... */
    if (m_EditMenu)
    {
        m_EditMenu->Delete(idMenuCodeComplete);
        m_EditMenu->Delete(idMenuShowCallTip);
        m_EditMenu->Delete(idMenuRenameSymbols);
    }
    if (m_SearchMenu)
    {
        m_SearchMenu->Delete(idMenuGotoFunction);
        m_SearchMenu->Delete(idMenuGotoPrevFunction);
        m_SearchMenu->Delete(idMenuGotoNextFunction);
        m_SearchMenu->Delete(idMenuGotoDeclaration);
        m_SearchMenu->Delete(idMenuGotoImplementation);
        m_SearchMenu->Delete(idMenuFindReferences);
        m_SearchMenu->Delete(idMenuOpenIncludeFile);
    }
}

static int SortCCList(const wxString& first, const wxString& second)
{
    const wxChar* a = first.c_str();
    const wxChar* b = second.c_str();
    while (*a && *b)
    {
        if (*a != *b)
        {
            if      ((*a == _T('?')) && (*b != _T('?')))
                return -1;
            else if ((*a != _T('?')) && (*b == _T('?')))
                return 1;
            else if ((*a == _T('?')) && (*b == _T('?')))
                return 0;

            if      ((*a == _T('_')) && (*b != _T('_')))
                return 1;
            else if ((*a != _T('_')) && (*b == _T('_')))
                return -1;

            wxChar lowerA = wxTolower(*a);
            wxChar lowerB = wxTolower(*b);

            if (lowerA != lowerB)
                return lowerA - lowerB;
        }
        a++;
        b++;
    }
    // Either *a or *b is null
    return *a - *b;
}

int CodeCompletion::CodeComplete()
{
    if (!IsAttached() || !m_InitDone)
        return -1;

    EditorManager* edMan = Manager::Get()->GetEditorManager();
    cbEditor* ed = edMan->GetBuiltinActiveEditor();
    if (!ed)
        return -3;

    FileType ft = FileTypeOf(ed->GetShortName());
    const bool caseSens = m_NativeParser.GetParser().Options().caseSensitive;

    TokenIdxSet result;
    if (   m_NativeParser.MarkItemsByAI(result, m_NativeParser.GetParser().Options().useSmartSense, true, caseSens)
        || m_NativeParser.LastAISearchWasGlobal() ) // enter even if no match (code-complete C++ keywords)
    {
        if (s_DebugSmartSense)
            CCLogger::Get()->DebugLog(F(_T("%d results"), result.size()));

        if (result.size() <= m_CCMaxMatches)
        {
            if (s_DebugSmartSense)
                CCLogger::Get()->DebugLog(_T("Generating tokens list..."));

            wxImageList* ilist = m_NativeParser.GetImageList();
            ed->GetControl()->ClearRegisteredImages();

            wxArrayString items;
            items.Alloc(result.size());
            int pos   = ed->GetControl()->GetCurrentPos();
            int start = ed->GetControl()->WordStartPosition(pos, true);
            wxArrayInt already_registered;
            std::set< wxString, std::less<wxString> > unique_strings; // check against this before inserting a new string in the list

            m_SearchItem.clear();
            {
                wxCriticalSectionLocker locker(s_TokensTreeCritical);
                TokensTree* tokens = m_NativeParser.GetParser().GetTokensTree();
                for (TokenIdxSet::iterator it = result.begin(); it != result.end(); ++it)
                {
                    Token* token = tokens->at(*it);
                    if (!token || token->m_Name.IsEmpty())
                        continue;

                    // check hashmap for unique_strings
                    if (unique_strings.find(token->m_Name) != unique_strings.end())
                        continue;

                    unique_strings.insert(token->m_Name);
                    int iidx = m_NativeParser.GetTokenKindImage(token);
                    if (already_registered.Index(iidx) == wxNOT_FOUND)
                    {
                        ed->GetControl()->RegisterImage(iidx, ilist->GetBitmap(iidx));
                        already_registered.Add(iidx);
                    }
                    wxString tmp;
                    tmp << token->m_Name << wxString::Format(_T("?%d"), iidx);
                    items.Add(tmp);
                    if (m_CCAutoAddParentheses && token->m_TokenKind == tkFunction)
                    {
                        m_SearchItem[token->m_Name] = token->GetFormattedArgs().size() - 2;
                    }
                    if (token->m_TokenKind == tkNamespace && token->m_Aliases.size())
                    {
                        for (size_t i = 0; i < token->m_Aliases.size(); ++i)
                        {
                            if (unique_strings.find(token->m_Aliases[i]) != unique_strings.end())
                                continue;

                            unique_strings.insert(token->m_Aliases[i]);
                            wxString tmp;
                            tmp << token->m_Aliases[i] << wxString::Format(_T("?%d"), iidx);
                            items.Add(tmp);
                        }
                    }
                }
            }

            if (m_NativeParser.LastAISearchWasGlobal())
            {
                // empty or partial search phrase: add theme keywords in search list
                if (s_DebugSmartSense)
                    CCLogger::Get()->DebugLog(_T("Last AI search was global: adding theme keywords in list"));

                EditorColourSet* theme = ed->GetColourSet();
                if (theme)
                {
                    wxString lastSearch = m_NativeParser.LastAIGlobalSearch().Lower();
                    int iidx = ilist->GetImageCount();
                    bool isC = ft == ftHeader || ft == ftSource;
                    // theme keywords
                    HighlightLanguage lang = theme->GetLanguageForFilename(_T(".")+wxFileName(ed->GetFilename()).GetExt());
                    wxString strLang = theme->GetLanguageName(lang);
                    // if its sourcecode/header file and a known fileformat, show the corresponding icon
                    if (isC && strLang==_T("C/C++"))
                        ed->GetControl()->RegisterImage(iidx, wxBitmap(cpp_keyword_xpm));
                    else if (isC && strLang==_T("D"))
                        ed->GetControl()->RegisterImage(iidx, wxBitmap(d_keyword_xpm));
                    else
                        ed->GetControl()->RegisterImage(iidx, wxBitmap(unknown_keyword_xpm));
                    // the first two keyword sets are the primary and secondary keywords (for most lexers at least)
                    // but this is now configurable in global settings
                    for (int i = 0; i <= wxSCI_KEYWORDSET_MAX; ++i)
                    {
                        if (!m_LexerKeywordsToInclude[i])
                            continue;

                        wxString keywords = theme->GetKeywords(lang, i);
                        wxStringTokenizer tkz(keywords, _T(" \t\r\n"), wxTOKEN_STRTOK);
                        while (tkz.HasMoreTokens())
                        {
                            wxString kw = tkz.GetNextToken() + wxString::Format(_T("?%d"), iidx);
                            if (kw.Lower().StartsWith(lastSearch))
                                items.Add(kw);
                        }
                    }
                }
            }

            if (items.IsEmpty())
            {
                if (s_DebugSmartSense)
                    CCLogger::Get()->DebugLog(_T("No items found."));
                return -2;
            }

            if (caseSens)
                items.Sort();
            else
                items.Sort(SortCCList);

            if (s_DebugSmartSense)
                CCLogger::Get()->DebugLog(_T("Done generating tokens list"));

            ed->GetControl()->AutoCompSetIgnoreCase(!caseSens);
            ed->GetControl()->AutoCompSetCancelAtStart(true);
            ed->GetControl()->AutoCompSetFillUps(m_CCFillupChars);
            ed->GetControl()->AutoCompSetChooseSingle(m_IsAutoPopup ? false : m_CCAutoSelectOne);
            ed->GetControl()->AutoCompSetAutoHide(true);
            ed->GetControl()->AutoCompSetDropRestOfWord(m_IsAutoPopup ? false : true);
            wxString final = GetStringFromArray(items, _T(" "));
            final.RemoveLast(); // remove last space

            ed->GetControl()->AutoCompShow(pos - start, final);
            return 0;
        }
        else if (!ed->GetControl()->CallTipActive())
        {
            wxString msg = _("Too many results.\n"
                             "Please edit results' limit in code-completion options,\n"
                             "or type at least one more character to narrow the scope down.");
            ed->GetControl()->CallTipShow(ed->GetControl()->GetCurrentPos(), msg);
            return -2;
        }
    }
    else if (!ed->GetControl()->CallTipActive())
    {
        if (s_DebugSmartSense)
            CCLogger::Get()->DebugLog(_T("0 results"));

        if (!m_NativeParser.GetParser().Done())
        {
            wxString msg = _("The Parser is still parsing files.");
            ed->GetControl()->CallTipShow(ed->GetControl()->GetCurrentPos(), msg);
            msg += m_NativeParser.GetParser().NotDoneReason();
            CCLogger::Get()->DebugLog(msg);
        }
    }

    return -5;
}

bool TestIncludeLine(wxString const &line)
{
    size_t index = line.find(_T('#'));
    if (index == wxString::npos)
        return false;
    ++index;

    for(; index < line.length(); ++index)
    {
        if (line[index] != _T(' ') && line[index] != _T('\t'))
        {
            if (line.Mid(index, 7) == _T("include"))
                return true;
            break;
        }
    }
    return false;
}

int CompareStringLen(const wxString& first, const wxString& second)
{
    return second.Len() - first.Len();
}

void GetStringFromSet(wxString& str, const StringSet& s, const wxString& separator)
{
    size_t totalLen = 0;
    for (StringSet::iterator it = s.begin(); it != s.end(); ++it)
        totalLen += (*it).Len();
    str.Clear();
    str.Alloc(totalLen + s.size() * separator.Len() + 1);
    for (StringSet::iterator it = s.begin(); it != s.end(); ++it)
        str << *it << separator;
}

wxArrayString& CodeCompletion::GetSystemIncludeDirs(cbProject* project, bool force)
{
    static cbProject* lastProject = nullptr;
    static wxArrayString incDirs;

    if (!force && project == lastProject)
        return incDirs;
    else
    {
        incDirs.Clear();
        lastProject = project;
    }

    wxString prjPath;
    if (project)
        prjPath = project->GetCommonTopLevelPath();

    ParserBase* parser = m_NativeParser.GetParserByProject(project);
    if (!parser)
        return incDirs;

    incDirs = parser->GetIncludeDirs();
    for (size_t i = 0; i < incDirs.GetCount();)
    {
        if (incDirs[i].Last() != wxFILE_SEP_PATH)
            incDirs[i].Append(wxFILE_SEP_PATH);
        if (project && incDirs[i].StartsWith(prjPath))
            incDirs.RemoveAt(i);
        else
            ++i;
    }

    return incDirs;
}

void CodeCompletion::GetAbsolutePath(const wxString& basePath, const wxArrayString& targets, wxArrayString& dirs)
{
    for (size_t i = 0; i < targets.GetCount(); ++i)
    {
        wxString includePath = targets[i];
        Manager::Get()->GetMacrosManager()->ReplaceMacros(includePath);
        wxFileName fn(includePath, wxEmptyString);
        if (fn.IsRelative())
        {
            const wxArrayString oldDirs = fn.GetDirs();
            fn.SetPath(basePath);
            for (size_t j = 0; j < oldDirs.GetCount(); ++j)
                fn.AppendDir(oldDirs[j]);
        }

        const wxString path = fn.GetFullPath();
        if (dirs.Index(path) == wxNOT_FOUND)
            dirs.Add(path);
    }
}

wxArrayString CodeCompletion::GetLocalIncludeDirs(cbProject* project, const wxArrayString& buildTargets)
{
    wxArrayString dirs;
    const wxString prjPath = project->GetCommonTopLevelPath();
    GetAbsolutePath(prjPath, project->GetIncludeDirs(), dirs);

    for (size_t i = 0; i < buildTargets.GetCount(); ++i)
        GetAbsolutePath(prjPath, project->GetBuildTarget(buildTargets[i])->GetIncludeDirs(), dirs);

    wxArrayString sysDirs;
    for (size_t i = 0; i < dirs.GetCount();)
    {
        if (dirs[i].StartsWith(prjPath))
            ++i;
        else
        {
            wxCriticalSectionLocker locker(s_HeadersCriticalSection);
            if (m_SystemHeadersMap.find(dirs[i]) == m_SystemHeadersMap.end())
                sysDirs.Add(dirs[i]);
            dirs.RemoveAt(i);
        }
    }

    if (!sysDirs.IsEmpty())
    {
        SystemHeadersThread* thread = new SystemHeadersThread(this, m_SystemHeadersMap, sysDirs);
        m_SystemHeadersThread.push_back(thread);
        if (!m_SystemHeadersThread.front()->IsRunning() && m_NativeParser.Done())
            thread->Run();
    }

    dirs.Sort(CompareStringLen);
    return dirs;
}

// Do the code completion when we enter:
// #include "| or #include <|
void CodeCompletion::CodeCompleteIncludes()
{
    if (!m_CCEnableHeaders || !IsAttached() || !m_InitDone)
        return;

    wxStopWatch sw;

    cbEditor* editor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!editor)
        return;

    const wxString curFile(editor->GetFilename());
    const wxString curPath(wxFileName(curFile).GetPath());
    wxArrayString buildTargets;

    cbProject* project = m_NativeParser.GetProjectByEditor(editor);
    ProjectFile* pf = project ? project->GetFileByFilename(curFile, false) : 0;
    if (pf)
        buildTargets = pf->buildTargets;

    FileType ft = FileTypeOf(editor->GetShortName());
    if ( ft != ftHeader && ft != ftSource) // only parse source/header files
        return;

    cbStyledTextCtrl* control = editor->GetControl();
    const int pos = control->GetCurrentPos();
    const int lineStartPos = control->PositionFromLine(control->GetCurrentLine());
    wxString line = control->GetLine(control->GetCurrentLine());
    line.Trim();
    if (line.IsEmpty() || !TestIncludeLine(line))
        return;

    int keyPos = line.Find(_T('"'));
    if (keyPos == wxNOT_FOUND)
        keyPos = line.Find(_T('<'));
    if (keyPos == wxNOT_FOUND || keyPos > pos - lineStartPos)
        return;
    ++keyPos;

    // now, we are after the quote prompt
    wxString filename = line.SubString(keyPos, pos - lineStartPos - 1);
    filename.Replace(_T("\\"), _T("/"), true);
    if (filename.Last() == _T('"') || filename.Last() == _T('>'))
        filename.RemoveLast();

    // fill a list of matching files
    StringSet files;

    // #include < or #include "
    {
        wxCriticalSectionLocker locker(s_HeadersCriticalSection);
        wxArrayString& incDirs = GetSystemIncludeDirs(project, project ? project->GetModified() : true);
        for (size_t i = 0; i < incDirs.GetCount(); ++i)
        {
            SystemHeadersMap::iterator it = m_SystemHeadersMap.find(incDirs[i]);
            if (it != m_SystemHeadersMap.end())
            {
                const StringSet& headers = it->second;
                for (StringSet::iterator it = headers.begin(); it != headers.end(); ++it)
                {
                    const wxString& file = *it;
                    if (file.StartsWith(filename))
                        files.insert(file);
                }
            }
        }
    }

    // #include "
    if (project)
    {
        const wxArrayString localIncludeDirs = GetLocalIncludeDirs(project, buildTargets);
        for (int i = 0; i < project->GetFilesCount(); ++i)
        {
            ProjectFile* pf = project->GetFile(i);
            if (pf && FileTypeOf(pf->relativeFilename) == ftHeader)
            {
                wxString file = pf->file.GetFullPath();
                wxString header;
                for (size_t j = 0; j < localIncludeDirs.GetCount(); ++j)
                {
                    const wxString& dir = localIncludeDirs[j];
                    if (file.StartsWith(dir))
                    {
                        header = file.Mid(dir.Len());
                        break;
                    }
                }

                if (header.IsEmpty())
                {
                    if (pf->buildTargets != buildTargets)
                        continue;

                    wxFileName fn(file);
                    fn.MakeRelativeTo(curPath);
                    header = fn.GetFullPath();
                }

                if (header.StartsWith(filename))
                {
                    header.Replace(_T("\\"), _T("/"), true);
                    files.insert(header);
                }
            }
        }
    }

    // popup the auto completion window
    if (!files.empty())
    {
        control->ClearRegisteredImages();
        control->AutoCompSetIgnoreCase(false);
        control->AutoCompSetCancelAtStart(true);
        control->AutoCompSetFillUps(m_CCFillupChars);
        control->AutoCompSetChooseSingle(false);
        control->AutoCompSetAutoHide(true);
        control->AutoCompSetDropRestOfWord(m_IsAutoPopup ? false : true);
        wxString final;
        GetStringFromSet(final, files, _T(" "));
        final.RemoveLast(); // remove last space
        control->AutoCompShow(pos - lineStartPos - keyPos, final);
        CCLogger::Get()->DebugLog(F(_T("Get include file count is %d, use time is %d"),
                                    files.size(), sw.Time()));
    }
}

void CodeCompletion::CodeCompletePreprocessor()
{
    if (!IsAttached() || !m_InitDone)
        return;

    EditorManager* edMan = Manager::Get()->GetEditorManager();
    cbEditor* ed = edMan->GetBuiltinActiveEditor();
    if (!ed)
        return;

    cbStyledTextCtrl* control = ed->GetControl();
    const int curPos = control->GetCurrentPos();
    const int start = control->WordStartPosition(curPos, true);
    const int end = control->WordEndPosition(curPos, true);

    wxArrayString tokens;
    tokens.Add(_T("include"));
    tokens.Add(_T("if"));
    tokens.Add(_T("ifdef"));
    tokens.Add(_T("ifndef"));
    tokens.Add(_T("elif"));
    tokens.Add(_T("elifdef"));
    tokens.Add(_T("elifndef"));
    tokens.Add(_T("else"));
    tokens.Add(_T("endif"));
    tokens.Add(_T("define"));
    tokens.Add(_T("undef"));
    tokens.Add(_T("pragma"));
    tokens.Add(_T("error"));
    tokens.Add(_T("line"));
    tokens.Sort();
    ed->GetControl()->ClearRegisteredImages();
    ed->GetControl()->AutoCompSetIgnoreCase(false);
    ed->GetControl()->AutoCompShow(end - start, GetStringFromArray(tokens, _T(" ")));
}

void CodeCompletion::ShowCallTip()
{
    if (!IsAttached() || !m_InitDone)
        return;

    if (!Manager::Get()->GetEditorManager())
        return;

    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed)
        return;

    // calculate the size of the calltips window
    int pos = ed->GetControl()->GetCurrentPos();
    wxPoint p = ed->GetControl()->PointFromPosition(pos); // relative point
    int pixelWidthPerChar = ed->GetControl()->TextWidth(wxSCI_STYLE_LINENUMBER, _T("W"));
    int maxCalltipLineSizeInChars = (ed->GetSize().x - p.x) / pixelWidthPerChar;
    if (maxCalltipLineSizeInChars < 64)
    {
        // if less than a threshold in chars, recalculate the starting position (instead of shrinking it even more)
        p.x -= (64 - maxCalltipLineSizeInChars) * pixelWidthPerChar;
        // but if it goes out of range, continue shrinking
        if (p.x >= 0)
        {
            maxCalltipLineSizeInChars = 64;
            pos = ed->GetControl()->PositionFromPoint(p);
        }
        // else, out of range
    }

    int start = 0, end = 0, count = 0, typedCommas = 0;

    wxArrayString items;
    m_NativeParser.GetCallTips(maxCalltipLineSizeInChars, items, typedCommas);
    std::set< wxString, std::less<wxString> > unique_tips; // check against this before inserting a new tip in the list
    wxString definition;
    for (unsigned int i = 0; i < items.GetCount(); ++i)
    {
        // allow only unique, non-empty items with equal or more commas than what the user has already typed
        if (unique_tips.find(items[i]) == unique_tips.end() && // unique
            !items[i].IsEmpty() && // non-empty
            typedCommas <= m_NativeParser.CountCommas(items[i], 0)) // commas satisfied
        {
            unique_tips.insert(items[i]);
            if (count != 0)
                definition << _T('\n'); // add new-line, except for the first line
            definition << items[i];
            if (start == 0)
                m_NativeParser.GetCallTipHighlight(items[i], &start, &end, typedCommas);
            ++count;
        }
    }

    if (definition.empty())
        return;

    ed->GetControl()->CallTipShow(pos, definition);
    if (start != 0 && end > start)
        ed->GetControl()->CallTipSetHighlight(start, end);
}

int CodeCompletion::DoClassMethodDeclImpl()
{
    if (!IsAttached() || !m_InitDone)
        return -1;

    EditorManager* edMan = Manager::Get()->GetEditorManager();
    cbEditor* ed = edMan->GetBuiltinActiveEditor();
    if (!ed)
        return -3;

    FileType ft = FileTypeOf(ed->GetShortName());
    if ( ft != ftHeader && ft != ftSource) // only parse source/header files
        return -4;

    wxString filename = ed->GetFilename();

    // open the insert class dialog
    InsertClassMethodDlg dlg(Manager::Get()->GetAppWindow(), &m_NativeParser.GetParser(), filename);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        int pos = ed->GetControl()->GetCurrentPos();
        int line = ed->GetControl()->LineFromPosition(pos);
        ed->GetControl()->GotoPos(ed->GetControl()->PositionFromLine(line));

        wxArrayString result = dlg.GetCode();
        for (unsigned int i = 0; i < result.GetCount(); ++i)
        {
            pos = ed->GetControl()->GetCurrentPos();
            line = ed->GetControl()->LineFromPosition(pos);
            wxString str = ed->GetLineIndentString(line - 1) + result[i];
            ed->GetControl()->SetTargetStart(pos);
            ed->GetControl()->SetTargetEnd(pos);
            ed->GetControl()->ReplaceTarget(str);
            ed->GetControl()->GotoPos(pos + str.Length());// - 3);
        }
        return 0;
    }

    return -5;
}

int CodeCompletion::DoAllMethodsImpl()
{
    if (!IsAttached() || !m_InitDone)
        return -1;

    EditorManager* edMan = Manager::Get()->GetEditorManager();
    cbEditor* ed = edMan->GetBuiltinActiveEditor();
    if (!ed)
        return -3;

    FileType ft = FileTypeOf(ed->GetShortName());
    if ( ft != ftHeader && ft != ftSource) // only parse source/header files
        return -4;

    wxCriticalSectionLocker locker(s_TokensTreeCritical);
    TokensTree* tree = m_NativeParser.GetParser().GetTokensTree();

    // get all filenames' indices matching our mask
    TokenFilesSet result;
    TokenFilesSet tmp;
    wxArrayString paths = m_NativeParser.GetAllPathsByFilename(ed->GetFilename());
    for (size_t i = 0; i < paths.GetCount(); ++i)
    {
        tree->m_FilenamesMap.FindMatches(paths[i], tmp, true, true);
        for (TokenFilesSet::iterator it = tmp.begin(); it != tmp.end(); ++it)
            result.insert(*it);
    }

    if (result.empty())
    {
        cbMessageBox(_("Can not find any file in parser's database."), _("Warning"), wxICON_WARNING);
        return -5;
    }

    // loop matching files, loop tokens in file and get list of un-implemented functions
    wxArrayString arr; // for selection (keeps strings)
    wxArrayInt arrint; // for selection (keeps indices)
    typedef std::map<int, std::pair<int, wxString> > ImplMap;
    ImplMap im;
    for (TokenFilesSet::iterator itf = result.begin(); itf != result.end(); ++itf)
    {
        TokenIdxSet& tokens = tree->m_FilesMap[*itf];
        // loop tokens in file
        for (TokenIdxSet::iterator its = tokens.begin(); its != tokens.end(); ++its)
        {
            Token* token = tree->at(*its);
            if (token && // valid token
                (token->m_TokenKind & (tkFunction | tkConstructor | tkDestructor)) && // is method
                token->m_ImplLine == 0) // is un-implemented
            {
                im[token->m_Line] = std::make_pair(*its, token->DisplayName());
            }
        }
    }

    for (ImplMap::iterator it = im.begin(); it != im.end(); ++it)
    {
        arrint.Add(it->second.first);
        arr.Add(it->second.second);
    }

    if (arr.empty())
    {
        cbMessageBox(_("No classes declared or no un-implemented class methods found."), _("Warning"), wxICON_WARNING);
        return -5;
    }

    // select tokens
    MultiSelectDlg dlg(Manager::Get()->GetAppWindow(), arr, true);
    if (dlg.ShowModal() == wxID_OK)
    {
        cbStyledTextCtrl* control = ed->GetControl();
        int pos = control->GetCurrentPos();
        int line = control->LineFromPosition(pos);
        control->GotoPos(control->PositionFromLine(line));

        ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("code_completion"));
        bool addDoxgenComment = cfg->ReadBool(_T("/add_doxgen_comment"), false);

        wxArrayInt indices = dlg.GetSelectedIndices();
        for (size_t i = 0; i < indices.GetCount(); ++i)
        {
            Token* token = tree->at(arrint[indices[i]]);
            if (!token)
                continue;

            pos = control->GetCurrentPos();
            line = control->LineFromPosition(pos);

            // actual code generation
            wxString str;
            if (i > 0)
                str << _T("\n");
            str << ed->GetLineIndentString(line - 1);
            if (addDoxgenComment)
                str << _T("/** @brief ") << token->m_Name << _T("\n  *\n  * @todo: document this function\n  */\n");
            wxString type = token->m_Type;
            if ((type.Last() == _T('&') || type.Last() == _T('*')) && type[type.Len() - 2] == _T(' '))
            {
                type[type.Len() - 2] = type.Last();
                type.RemoveLast();
            }
            if (!type.IsEmpty())
                str << type << _T(" ");
            if (token->m_ParentIndex != -1)
                str << token->GetParentName() << _T("::");
            str << token->m_Name << token->GetStrippedArgs();
            if (token->m_IsConst)
                str << _T(" const");
            str << _T("\n{\n}\n");

            // add code in editor
            control->SetTargetStart(pos);
            control->SetTargetEnd(pos);
            control->ReplaceTarget(str);
            control->GotoPos(pos + str.Length());
        }
        return 0;
    }

    return -5;
}

void CodeCompletion::DoCodeComplete()
{
    EditorManager* edMan = Manager::Get()->GetEditorManager();
    cbEditor* ed = edMan->GetBuiltinActiveEditor();
    if (!ed)
        return;

    cbStyledTextCtrl* control = ed->GetControl();
    const int pos = control->GetCurrentPos();
    const int style = control->GetStyleAt(pos);
    const int lineIndentPos = control->GetLineIndentPosition(control->GetCurrentLine());

    const wxChar lineFirstChar = ed->GetControl()->GetCharAt(lineIndentPos);
    const wxChar curChar = ed->GetControl()->GetCharAt(pos - 1);

    if (lineFirstChar == _T('#'))
    {
        const int start = control->WordStartPosition(lineIndentPos + 1, true);
        const int end = control->WordEndPosition(lineIndentPos + 1, true);
        const wxString str = control->GetTextRange(start, end);

        if (str == _T("include"))
            CodeCompleteIncludes();
        else if (end >= pos)
            CodeCompletePreprocessor();
        return;
    }
    else if (curChar == _T('#'))
        return;
    else if (lineFirstChar == _T(':') && curChar == _T(':'))
        return;

    if (style != wxSCI_C_DEFAULT && style != wxSCI_C_OPERATOR && style != wxSCI_C_IDENTIFIER)
        return;

    CodeComplete();
}

void CodeCompletion::DoInsertCodeCompleteToken(wxString tokName)
{
    // remove arguments
    int pos = tokName.Find(_T("("));
    if (pos != wxNOT_FOUND)
        tokName.Remove(pos);

    EditorManager* edMan = Manager::Get()->GetEditorManager();
    cbEditor* ed = edMan->GetBuiltinActiveEditor();
    if (!ed)
        return;

    int end = ed->GetControl()->GetCurrentPos() > m_NativeParser.GetEditorEndWord() ? ed->GetControl()->GetCurrentPos() : m_NativeParser.GetEditorEndWord();
    ed->GetControl()->SetSelectionVoid(m_NativeParser.GetEditorStartWord(), end);
    ed->GetControl()->ReplaceSelection(_T(""));
    ed->GetControl()->InsertText(m_NativeParser.GetEditorStartWord(), tokName);
    ed->GetControl()->GotoPos(m_NativeParser.GetEditorStartWord() + tokName.Length());
}

// events

void CodeCompletion::OnViewClassBrowser(wxCommandEvent& event)
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("code_completion"));
    if (!cfg->ReadBool(_T("/use_symbols_browser"), true))
    {
        cbMessageBox(_("The symbols browser is disabled in code-completion options.\n"
                        "Please enable it there first..."), _("Information"), wxICON_INFORMATION);
        return;
    }
    CodeBlocksDockEvent evt(event.IsChecked() ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
    evt.pWindow = (wxWindow*)m_NativeParser.GetClassBrowser();
    Manager::Get()->ProcessEvent(evt);
}

void CodeCompletion::OnAppDoneStartup(CodeBlocksEvent& event)
{
    // Let the app startup before parsing
    // This is to prevent the Splash Screen from delaying so much. By adding the
    // timer, the splash screen is closed and Code::Blocks doesn't take so long
    // in starting.
    m_InitDone = true;

    // Dreaded DDE-open bug related: do not touch the following lines unless for a good reason

    // parse any projects opened through DDE or the command-line
    cbProject* curProject = Manager::Get()->GetProjectManager()->GetActiveProject();
    if (curProject && !m_NativeParser.GetParserByProject(curProject))
        m_NativeParser.CreateParser(curProject);

    // parse any files opened through DDE or the command-line
    EditorBase* editor = Manager::Get()->GetEditorManager()->GetActiveEditor();
    if (editor)
        m_NativeParser.OnEditorActivated(editor);

    event.Skip();
}

void CodeCompletion::OnCodeCompleteTimer(wxTimerEvent& event)
{
    if (Manager::Get()->GetEditorManager()->FindPageFromEditor(m_LastEditor) == -1)
        return; // editor is invalid (probably closed already)

    // ask for code-completion *only* if the editor is still after the "." or "->" operator
    if (m_LastEditor && m_LastEditor->GetControl()->GetCurrentPos() == m_LastPosForCodeCompletion)
    {
        DoCodeComplete();
        m_LastPosForCodeCompletion = -1; // reset it
    }
}

void CodeCompletion::OnWorkspaceChanged(CodeBlocksEvent& event)
{
    // EVT_WORKSPACE_CHANGED is a powerful event, it's sent after any project
    // has finished loading or closing. It's the *LAST* event to be sent when
    // the workspace has been changed, and it's not sent if the application is
    // shutting down. So it's the ideal time to parse files and update your
    // widgets.
    if (IsAttached() && m_InitDone)
    {
        cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
        if (project && !m_NativeParser.GetParserByProject(project))
            m_NativeParser.CreateParser(project);

        // Update the Function toolbar
        m_TimerFunctionsParsing.Start(g_EditorActivatedDelay + 100, wxTIMER_ONE_SHOT);

        // Update the class browser
        if (m_NativeParser.GetParser().ClassBrowserOptions().displayFilter == bdfProject)
            m_NativeParser.UpdateClassBrowser();
    }
    event.Skip();
}

void CodeCompletion::OnProjectActivated(CodeBlocksEvent& event)
{
    // The Class browser shouldn't be updated if we're in the middle of loading/closing
    // a project/workspace, because the class browser would need to be updated again.
    // So we need to update it with the EVT_WORKSPACE_CHANGED event, which gets
    // triggered after everything's finished loading/closing.
    if (!ProjectManager::IsBusy() && IsAttached() && m_InitDone)
    {
        cbProject* project = event.GetProject();
        if (project && !m_NativeParser.GetParserByProject(project) && project->GetFilesCount() > 0)
            m_NativeParser.CreateParser(project);

        if (m_NativeParser.GetParser().ClassBrowserOptions().displayFilter == bdfProject)
            m_NativeParser.UpdateClassBrowser();
    }

    event.Skip();
}

void CodeCompletion::OnProjectClosed(CodeBlocksEvent& event)
{
    // After this, the Class Browser needs to be updated. It will happen
    // when we receive the next EVT_PROJECT_ACTIVATED event.
    if (IsAttached() && m_InitDone)
    {
        cbProject* project = event.GetProject();
        if (project && m_NativeParser.GetParserByProject(project))
        {
            ReparsingMap::iterator it = m_ReparsingMap.find(project);
            if (it != m_ReparsingMap.end())
                m_ReparsingMap.erase(it);

            m_NativeParser.DeleteParser(project);
        }
    }
    event.Skip();
}

void CodeCompletion::OnProjectSaved(CodeBlocksEvent& event)
{
    // reparse project (compiler search dirs might have changed)
    m_TimerProjectSaved.SetClientData(event.GetProject());
    // we need more time for waiting wxExecute in NativeParser::AddCompilerPredefinedMacros
    m_TimerProjectSaved.Start(200, wxTIMER_ONE_SHOT);

    event.Skip();
}

void CodeCompletion::OnProjectSavedTimer(wxTimerEvent& event)
{
    cbProject* project = static_cast<cbProject*>(m_TimerProjectSaved.GetClientData());
    m_TimerProjectSaved.SetClientData(NULL);

    ProjectsArray* projs = Manager::Get()->GetProjectManager()->GetProjects();
    if (projs->Index(project) == wxNOT_FOUND)
        return;

    if (IsAttached() && m_InitDone && project)
    {
        if (project &&  m_NativeParser.GetParserByProject(project))
        {
            ReparsingMap::iterator it = m_ReparsingMap.find(project);
            if (it != m_ReparsingMap.end())
                m_ReparsingMap.erase(it);
            if (m_NativeParser.DeleteParser(project))
            {
                CCLogger::Get()->DebugLog(_T("Reparsing project."));
                m_NativeParser.CreateParser(project);
            }
        }
    }
}

void CodeCompletion::OnReparsingTimer(wxTimerEvent& event)
{
    if (!ProjectManager::IsBusy() && IsAttached() && m_InitDone)
    {
        ReparsingMap::iterator it = m_ReparsingMap.begin();
        if (it != m_ReparsingMap.end() && m_NativeParser.Done())
        {
            cbProject* project = it->first;
            wxArrayString& files = it->second;
            if (!project)
                project = m_NativeParser.GetProjectByFilename(files[0]);

            if (project && Manager::Get()->GetProjectManager()->IsProjectStillOpen(project))
            {
                for (size_t i = 0; i < files.GetCount(); ++i)
                {
                    const wxString& filename = files[i];
                    if (m_NativeParser.ReparseFile(project, filename))
                    {
                        CCLogger::Get()->DebugLog(_T("Reparsing file : ") + filename);
                        Manager::Yield();
                        EditorBase* editor = Manager::Get()->GetEditorManager()->GetActiveEditor();
                        if (editor && editor->GetFilename() == filename)
                            ParseFunctionsAndFillToolbar(true);
                    }
                    else
                    {
                        m_TimerReparsing.Start(g_EditorActivatedDelay, wxTIMER_ONE_SHOT);
                        return;
                    }
                }
                CCLogger::Get()->DebugLog(F(_T("Be re-parse file number: %d"), files.GetCount()));
            }

            m_ReparsingMap.erase(it);
        }

        if (!m_ReparsingMap.empty())
            m_TimerReparsing.Start(g_EditorActivatedDelay, wxTIMER_ONE_SHOT);
    }
    else
    {
        m_ReparsingMap.clear();
        CCLogger::Get()->DebugLog(_T("Reparsing files failed!"));
    }
}

void CodeCompletion::OnProjectFileAdded(CodeBlocksEvent& event)
{
    if (IsAttached() && m_InitDone)
        m_NativeParser.AddFileToParser(event.GetProject(), event.GetString());
    event.Skip();
}

void CodeCompletion::OnProjectFileRemoved(CodeBlocksEvent& event)
{
    if (IsAttached() && m_InitDone)
        m_NativeParser.RemoveFileFromParser(event.GetProject(), event.GetString());
    event.Skip();
}

void CodeCompletion::OnProjectFileChanged(CodeBlocksEvent& event)
{
    if (IsAttached() && m_InitDone)
    {
        // TODO (Loaden#5#) make sure the event.GetProject() is valid.
        cbProject* project = event.GetProject();
        wxString filename = event.GetString();
        if (!project)
            project = m_NativeParser.GetProjectByFilename(filename);
        if (project && m_NativeParser.ReparseFile(project, filename))
            CCLogger::Get()->DebugLog(_T("Reparsing when file changed: ") + filename);
    }
    event.Skip();
}

void CodeCompletion::OnUserListSelection(CodeBlocksEvent& event)
{
    if (IsAttached() && m_InitDone)
    {
        wxString tokName = event.GetString();
        DoInsertCodeCompleteToken(event.GetString());
    }

    event.Skip();
}

void CodeCompletion::OnThreadUpdate(wxCommandEvent& event)
{
    if (!m_SystemHeadersThread.empty())
    {
        SystemHeadersThread* thread = static_cast<SystemHeadersThread*>(event.GetClientData());
        if (thread == m_SystemHeadersThread.front())
            CCLogger::Get()->DebugLog(event.GetString());
    }
}

void CodeCompletion::OnThreadCompletion(wxCommandEvent& event)
{
    if (!m_SystemHeadersThread.empty())
    {
        SystemHeadersThread* thread = static_cast<SystemHeadersThread*>(event.GetClientData());
        if (thread == m_SystemHeadersThread.front())
        {
            if (!event.GetString().IsEmpty())
                CCLogger::Get()->DebugLog(event.GetString());
            m_SystemHeadersThread.pop_front();
        }

        if (   m_CCEnableHeaders
            && !m_SystemHeadersThread.empty()
            && !m_SystemHeadersThread.front()->IsRunning()
            && m_NativeParser.Done() )
        {
            m_SystemHeadersThread.front()->Run();
        }
    }
}

void CodeCompletion::OnThreadError(wxCommandEvent& event)
{
    if (!m_SystemHeadersThread.empty())
    {
        SystemHeadersThread* thread = static_cast<SystemHeadersThread*>(event.GetClientData());
        if (thread == m_SystemHeadersThread.front())
            CCLogger::Get()->DebugLog(event.GetString());
    }
}

// compare method for the sort algorithm for our FunctionScope struct
bool LessFunctionScope(const CodeCompletion::FunctionScope& fs1, const CodeCompletion::FunctionScope& fs2)
{
    int result = wxStricmp(fs1.Scope, fs2.Scope);
    if (result == 0)
    {
        result = wxStricmp(fs1.Name, fs2.Name);
        if (result == 0)
        {
            result = fs1.StartLine - fs2.StartLine;
        }
    }

    return result < 0;
}

bool EqualFunctionScope(const CodeCompletion::FunctionScope& fs1, const CodeCompletion::FunctionScope& fs2)
{
    int result = wxStricmp(fs1.Scope, fs2.Scope);
    if (result == 0)
    {
        result = wxStricmp(fs1.Name, fs2.Name);
    }

    return result == 0;
}

bool LessNameSpace(const NameSpace& ns1, const NameSpace& ns2)
{
    return ns1.Name < ns2.Name;
}

bool EqualNameSpace(const NameSpace& ns1, const NameSpace& ns2)
{
    return ns1.Name == ns2.Name;
}

// help method in finding the namespace position in the vector for the namespace containing the current line
int CodeCompletion::NameSpacePosition() const
{
    int retValue = -1;
    int startLine = -1;
    for (unsigned int idxNs = 0; idxNs < m_NameSpaces.size(); ++idxNs)
    {
        const NameSpace& Ns = m_NameSpaces[idxNs];
        if (m_CurrentLine >= Ns.StartLine && m_CurrentLine <= Ns.EndLine && Ns.StartLine > startLine)
        {
            // got one, maybe there might be a btter fitting namespace (embedded namespaces)
            // so keep on looking
            retValue = static_cast<int>(idxNs);
            startLine = Ns.StartLine;
        }
    }

    return retValue;
}

// help method in finding the function position in the vector for the function containing the current line
void CodeCompletion::FunctionPosition(int &scopeItem, int &functionItem) const
{
    scopeItem = -1;
    functionItem = -1;

    for (unsigned int idxSc = 0; idxSc < m_ScopeMarks.size(); ++idxSc)
    {
        unsigned int start = m_ScopeMarks[idxSc];
        unsigned int end = (idxSc + 1 < m_ScopeMarks.size()) ? m_ScopeMarks[idxSc + 1] : m_FunctionsScope.size();
        for (int idxFn = 0; start + idxFn < end; ++idxFn)
        {
            const FunctionScope fs = m_FunctionsScope[start + idxFn];
            if (m_CurrentLine >= fs.StartLine && m_CurrentLine <= fs.EndLine)
            {
                scopeItem = idxSc;
                functionItem = idxFn;
            }
        }
    }
}

void CodeCompletion::GotoFunctionPrevNext(bool next /* = false */)
{
    EditorManager* edMan = Manager::Get()->GetEditorManager();
    cbEditor* ed = edMan->GetBuiltinActiveEditor();
    if (!ed)
        return;

    int current_line = ed->GetControl()->GetCurrentLine();

    // search previous/next function from current line, default: previous
    int line = -1;
    if (m_FunctionsScope.size())
    {
        unsigned int best_func = 0;
        bool found_best_func   = false;
        for (unsigned int idx_func=0; idx_func<m_FunctionsScope.size(); ++idx_func)
        {
            int best_func_line  = m_FunctionsScope[best_func].StartLine;
            int func_start_line = m_FunctionsScope[idx_func].StartLine;
            if (next)
            {
                if         (best_func_line  > current_line)     // candidate: is after current line
                {
                    if (   (func_start_line > current_line  )   // another candidate
                        && (func_start_line < best_func_line) ) // decide which is more near
                    { best_func = idx_func; found_best_func = true; }
                }
                else if    (func_start_line > current_line)     // candidate: is after current line
                {     best_func = idx_func; found_best_func = true; }
            }
            else // prev
            {
                if         (best_func_line  < current_line)     // candidate: is before current line
                {
                    if (   (func_start_line < current_line  )   // another candidate
                        && (func_start_line > best_func_line) ) // decide which is more near
                    { best_func = idx_func; found_best_func = true; }
                }
                else if    (func_start_line < current_line)     // candidate: is before current line
                {     best_func = idx_func; found_best_func = true; }
            }
        }

        if      (found_best_func)
        { line = m_FunctionsScope[best_func].StartLine; }
        else if ( next && m_FunctionsScope[best_func].StartLine>current_line)
        { line = m_FunctionsScope[best_func].StartLine; }
        else if (!next && m_FunctionsScope[best_func].StartLine<current_line)
        { line = m_FunctionsScope[best_func].StartLine; }
    }

    if (line != -1)
    {
        ed->GotoLine(line);
        ed->SetFocus();
    }
}

void CodeCompletion::ParseFunctionsAndFillToolbar(bool force)
{
    EditorManager* edMan = Manager::Get()->GetEditorManager();
    if (!edMan) // Closing the app?
        return;

    cbEditor* ed = edMan->GetBuiltinActiveEditor();
    if (!ed || !ed->GetControl())
    {
        if (m_Function)
            m_Function->Clear();
        if (m_Scope)
            m_Scope->Clear();

        EnableToolbarTools(false);
        m_LastFile.Clear();
        return;
    }

    const wxString filename = ed->GetFilename();
    if (filename.IsEmpty())
        return;

    bool fileParseFinished = m_NativeParser.GetParser().IsFileParsed(filename);
    FunctionsScopePerFile* funcdata = &(m_AllFunctionsScopes[filename]);

    // *** Part 1: Parse the file (if needed) ***
    if (force || !funcdata->parsed)
    {
        m_TimerFunctionsParsing.Stop();
        funcdata->m_FunctionsScope.clear();
        funcdata->m_NameSpaces.clear();

        wxCriticalSectionLocker locker(s_TokensTreeCritical);

        TokensTree* tree = m_NativeParser.GetParser().GetTokensTree();
        TokenIdxSet result;
        m_NativeParser.GetParser().FindTokensInFile(filename, result, tkAnyFunction | tkEnum | tkClass | tkNamespace);

        if (!result.empty())
            funcdata->parsed = true;
        else
            fileParseFinished = false;

        for (TokenIdxSet::iterator it = result.begin(); it != result.end(); ++it)
        {
            const Token* token = tree->at(*it);
            if (token && token->m_ImplLine != 0)
            {
                FunctionScope fs;
                fs.StartLine = token->m_ImplLine - 1;
                fs.EndLine = token->m_ImplLineEnd - 1;
                const size_t fileIdx = tree->GetFileIndex(filename);
                if (token->m_TokenKind & tkAnyFunction && fileIdx == token->m_ImplFileIdx)
                {
                    fs.Scope = token->GetNamespace();
                    if (fs.Scope.IsEmpty())
                        fs.Scope = g_GlobalScope;
                    wxString result = token->m_Name;
                    result << token->GetFormattedArgs();
                    if (!token->m_Type.IsEmpty())
                        result << _T(" : ") << token->m_Type;
                    fs.Name = result;
                    funcdata->m_FunctionsScope.push_back(fs);
                }
                else if (token->m_TokenKind & (tkEnum | tkClass | tkNamespace))
                {
                    fs.Scope = token->GetNamespace() + token->m_Name + _T("::");
                    funcdata->m_FunctionsScope.push_back(fs);
                }
            }
        }

        FunctionsScopeVec& functionsScopes = funcdata->m_FunctionsScope;
        NameSpaceVec& nameSpaces = funcdata->m_NameSpaces;

        m_NativeParser.GetParser().ParseBufferForNamespaces(ed->GetControl()->GetText(), nameSpaces);
        std::sort(nameSpaces.begin(), nameSpaces.end(), LessNameSpace);

        std::copy(nameSpaces.begin(), nameSpaces.end(), back_inserter(functionsScopes));
        std::sort(functionsScopes.begin(), functionsScopes.end(), LessFunctionScope);

        // remove consecutive duplicates
        FunctionsScopeVec::iterator it;
        it = unique(functionsScopes.begin(), functionsScopes.end(), EqualFunctionScope);
        functionsScopes.resize(it - functionsScopes.begin());

        /*
        CCLogger::Get()->DebugLog(F(_T("Found %d namespace locations"), nameSpaces.size()));
        for (unsigned int i = 0; i < nameSpaces.size(); ++i)
            CCLogger::Get()->DebugLog(F(_T("\t%s (%d:%d)"),
                nameSpaces[i].Name.wx_str(), nameSpaces[i].StartLine, nameSpaces[i].EndLine));
        */

       m_ToolbarChanged = true;
    }

    // *** Part 2: Fill the toolbar ***
    m_FunctionsScope = funcdata->m_FunctionsScope;
    m_NameSpaces     = funcdata->m_NameSpaces;

    m_ScopeMarks.clear();
    unsigned int fsSize = m_FunctionsScope.size();
    if (!m_FunctionsScope.empty())
    {
        m_ScopeMarks.push_back(0);
        if (m_Scope)
        {
            wxString lastScope = m_FunctionsScope[0].Scope;
            for (unsigned int idx = 1; idx < fsSize; ++idx)
            {
                const wxString& currentScope = m_FunctionsScope[idx].Scope;
                if (lastScope != currentScope)
                {
                    m_ScopeMarks.push_back(idx);
                    lastScope = currentScope;
                }
            }
        }
    }

    /*
    CCLogger::Get()->DebugLog(F(_T("Parsed %d functionscope items"), m_FunctionsScope.size()));
    for (unsigned int i = 0; i < m_FunctionsScope.size(); ++i)
        CCLogger::Get()->DebugLog(F(_T("\t%s%s (%d:%d)"),
            m_FunctionsScope[i].Scope.wx_str(), m_FunctionsScope[i].Name.wx_str(),
            m_FunctionsScope[i].StartLine, m_FunctionsScope[i].EndLine));
    */

    // Does the toolbar need a refresh?
    if (m_ToolbarChanged || m_LastFile != filename)
    {
        // Update the last editor and changed flag...
        m_ToolbarChanged = false;
        m_LastFile = filename;
        TRACE(_T("ParseFunctionsAndFillToolbar() : Update last file is %s"), filename.wx_str());

        // ...and refresh the toolbars.
        m_Function->Clear();

        if (m_Scope)
        {
            m_Scope->Freeze();
            m_Scope->Clear();

            // add to the choice controls
            for (unsigned int idxSc = 0; idxSc < m_ScopeMarks.size(); ++idxSc)
            {
                int idxFn = m_ScopeMarks[idxSc];
                const FunctionScope& fs = m_FunctionsScope[idxFn];
                m_Scope->Append(fs.Scope);
            }

            m_Scope->Thaw();
        }
        else
        {
            m_Function->Freeze();

            for (unsigned int idxFn = 0; idxFn < m_FunctionsScope.size(); ++idxFn)
            {
                const FunctionScope& fs = m_FunctionsScope[idxFn];
                m_Function->Append(fs.Scope + fs.Name);
            }

            m_Function->Thaw();
        }
    }

    // Find the current function and update
    FindFunctionAndUpdate(ed->GetControl()->GetCurrentLine());

    // Control the toolbar state
    EnableToolbarTools(fileParseFinished);
}

void CodeCompletion::FindFunctionAndUpdate(int currentLine)
{
    if (currentLine == -1)
        return;

    m_CurrentLine = currentLine;

    int selSc, selFn;
    FunctionPosition(selSc, selFn);

    if (m_Scope)
    {
        if (selSc != -1 && selSc != m_Scope->GetSelection())
        {
            m_Scope->SetSelection(selSc);
            UpdateFunctions(selSc);
        }
        else if (selSc == -1)
        {
            m_Scope->SetSelection(-1);
        }
    }

    if (selFn != -1 && selFn != m_Function->GetSelection())
    {
        m_Function->SetSelection(selFn);
    }
    else if (selFn == -1)
    {
        m_Function->SetSelection(-1);

        wxChoice* choice = (m_Scope) ? m_Scope : m_Function;

        int NsSel = NameSpacePosition();
        if (NsSel != -1)
            choice->SetStringSelection(m_NameSpaces[NsSel].Name);
        else if (!m_Scope)
            choice->SetSelection(-1);
        else
        {
            choice->SetStringSelection(g_GlobalScope);
            wxCommandEvent evt(wxEVT_COMMAND_CHOICE_SELECTED, XRCID("chcCodeCompletionScope"));
            wxPostEvent(this, evt);
        }
    }
}

void CodeCompletion::UpdateFunctions(unsigned int scopeItem)
{
    m_Function->Freeze();
    m_Function->Clear();

    unsigned int idxEnd = (scopeItem + 1 < m_ScopeMarks.size()) ? m_ScopeMarks[scopeItem + 1] : m_FunctionsScope.size();
    for (unsigned int idxFn = m_ScopeMarks[scopeItem]; idxFn < idxEnd; ++idxFn)
    {
        const wxString &name = m_FunctionsScope[idxFn].Name;
        m_Function->Append(name);
    }

    m_Function->Thaw();
}

void CodeCompletion::OnEditorSaveOrModified(CodeBlocksEvent& event)
{
    if (!ProjectManager::IsBusy() && IsAttached() && m_InitDone && event.GetEditor())
    {
        cbProject* project = event.GetProject();
        ReparsingMap::iterator it = m_ReparsingMap.find(project);
        if (it == m_ReparsingMap.end())
            it = m_ReparsingMap.insert(std::make_pair(project, wxArrayString())).first;

        const wxString& filename = event.GetEditor()->GetFilename();
        if (it->second.Index(filename) == wxNOT_FOUND)
            it->second.Add(filename);

        m_TimerReparsing.Start(g_EditorActivatedDelay + it->second.GetCount(), wxTIMER_ONE_SHOT);
    }

    event.Skip();
}

void CodeCompletion::OnEditorOpen(CodeBlocksEvent& event)
{
    if (!Manager::IsAppShuttingDown() && IsAttached() && m_InitDone)
    {
        cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(event.GetEditor());
        if (ed)
        {
            FunctionsScopePerFile* funcdata = &(m_AllFunctionsScopes[ed->GetFilename()]);
            funcdata->parsed = false;
        }
    }

    event.Skip();
}

void CodeCompletion::OnEditorActivatedTimer(wxTimerEvent& event)
{
    EditorBase* editor = Manager::Get()->GetEditorManager()->GetActiveEditor();
    if (!editor || editor != m_LastEditor || editor->GetFilename().IsEmpty())
    {
        m_LastEditor = nullptr;
        return;
    }

    if (   !m_LastFile.IsEmpty()
        && m_LastFile != g_StartHereTitle
        && m_LastFile == editor->GetFilename() )
    {
        return;
    }

    m_NativeParser.OnEditorActivated(editor);
    m_TimerFunctionsParsing.Start(g_EditorActivatedDelay + 100, wxTIMER_ONE_SHOT);
}

void CodeCompletion::OnEditorActivated(CodeBlocksEvent& event)
{
    TRACE(_T("CodeCompletion::OnEditorActivated() : %d, %s, %s"), ProjectManager::IsBusy(),
          m_LastFile.wx_str(), event.GetEditor() ? event.GetEditor()->GetFilename().wx_str() : _T("NULL"));

    if (!ProjectManager::IsBusy() && IsAttached() && m_InitDone && event.GetEditor())
    {
        if (event.GetEditor()->GetFilename() == g_StartHereTitle)
            m_LastEditor = nullptr;
        else
            m_LastEditor = Manager::Get()->GetEditorManager()->GetBuiltinEditor(event.GetEditor());

        m_TimerEditorActivated.Start(g_EditorActivatedDelay, wxTIMER_ONE_SHOT);
    }

    event.Skip();
}

void CodeCompletion::OnEditorClosed(CodeBlocksEvent& event)
{
    if (m_LastEditor == event.GetEditor())
    {
        m_LastEditor = nullptr;
        if (m_TimerEditorActivated.IsRunning())
            m_TimerEditorActivated.Stop();
    }

    m_NativeParser.OnEditorClosed(event.GetEditor());
    m_LastFile.Clear();

    EditorManager* edm = Manager::Get()->GetEditorManager();
    if (!edm)
    {
        event.Skip();
        return;
    }

    wxString activeFile;
    EditorBase* eb = edm->GetActiveEditor();
    if (eb)
        activeFile = eb->GetFilename();

    // we need to clear CC toolbar only when we are closing last editor
    // in other situations OnEditorActivated does this job
    if (edm->GetEditorsCount() == 0 || activeFile == g_StartHereTitle)
    {
        EnableToolbarTools(false);

        // clear toolbar when closing last editor
        if (m_Scope)
            m_Scope->Clear();
        if (m_Function)
            m_Function->Clear();

        cbEditor* ed = edm->GetBuiltinEditor(event.GetEditor());
        wxString filename;
        if (ed)
            filename = ed->GetFilename();

        m_AllFunctionsScopes[filename].m_FunctionsScope.clear();
        m_AllFunctionsScopes[filename].m_NameSpaces.clear();
        m_AllFunctionsScopes[filename].parsed = false;
        if (m_NativeParser.GetParser().ClassBrowserOptions().displayFilter == bdfFile)
            m_NativeParser.UpdateClassBrowser();
    }

    event.Skip();
}

void CodeCompletion::OnStartParsingFunctions(wxTimerEvent& event)
{
    if (!ProjectManager::IsBusy())
    {
        ParseFunctionsAndFillToolbar();
    }
}

void CodeCompletion::OnFindFunctionAndUpdate(wxTimerEvent& event)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed)
        return;

    FindFunctionAndUpdate(ed->GetControl()->GetCurrentLine());
}

void CodeCompletion::OnValueTooltip(CodeBlocksEvent& event)
{
    event.Skip();

    if (IsAttached() && m_InitDone)
    {
        if (!Manager::Get()->GetConfigManager(_T("code_completion"))->ReadBool(_T("eval_tooltip"), true))
            return;

        EditorBase* base = event.GetEditor();
        cbEditor* ed = base && base->IsBuiltinEditor() ? static_cast<cbEditor*>(base) : 0;
        if (!ed || ed->IsContextMenuOpened())
            return;

        if (ed->GetControl()->CallTipActive())
            ed->GetControl()->CallTipCancel();
//        CCLogger::Get()->DebugLog(F(_T("CodeCompletion::OnValueTooltip: %p"), ed));
        /* NOTE: The following 2 lines of codes can fix [Bug #11785].
        *       The solution may not the best one and it requires the editor
        *       to have the focus (even if C::B has the focus) in order to pop-up the tooltip. */
        if (wxWindow::FindFocus() != static_cast<wxWindow*>(ed->GetControl()))
            return;

        // ignore comments, strings, preprocesor, etc
        int style = event.GetInt();
        if (   (style != wxSCI_C_DEFAULT)
            && (style != wxSCI_C_OPERATOR)
            && (style != wxSCI_C_IDENTIFIER) )
            return;

        int pos = ed->GetControl()->PositionFromPointClose(event.GetX(), event.GetY());
        if (pos < 0 || pos >= ed->GetControl()->GetLength())
            return;

        TokenIdxSet result;
        int endOfWord = ed->GetControl()->WordEndPosition(pos, true);
        if (m_NativeParser.MarkItemsByAI(result, true, false, true, endOfWord))
        {
            wxCriticalSectionLocker locker(s_TokensTreeCritical);
            wxString msg;
            int count = 0;
            for (TokenIdxSet::iterator it = result.begin(); it != result.end(); ++it)
            {
                Token* token = m_NativeParser.GetParser().GetTokensTree()->at(*it);
                if (token)
                {
                    msg << token->DisplayName() << _T("\n");
                    ++count;
                    if (count > 32) // allow max 32 matches (else something is definitely wrong)
                    {
                        msg.Clear();
                        break;
                    }
                }
            }
            if (!msg.IsEmpty())
            {
                msg.RemoveLast(); // last \n
                ed->GetControl()->CallTipShow(pos, msg);
//                    CCLogger::Get()->DebugLog(F(msg));
            }
        }
    }
}

void CodeCompletion::OnUpdateUI(wxUpdateUIEvent& event)
{
    wxString NameUnderCursor;
    bool IsInclude = false;
    const bool HasNameUnderCursor = EditorHasNameUnderCursor(NameUnderCursor, IsInclude);

    const bool HasEd = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor() != 0;
    if (m_EditMenu)
    {
        m_EditMenu->Enable(idMenuCodeComplete, HasEd);
        m_EditMenu->Enable(idMenuShowCallTip, HasEd);
        const bool RenameEnable = HasNameUnderCursor && !IsInclude && m_NativeParser.GetParser().Done();
        m_EditMenu->Enable(idMenuRenameSymbols, RenameEnable);
    }

    if (m_SearchMenu)
    {
        m_SearchMenu->Enable(idMenuGotoFunction,       HasEd);
        m_SearchMenu->Enable(idMenuGotoPrevFunction,   HasEd);
        m_SearchMenu->Enable(idMenuGotoNextFunction,   HasEd);

        const bool GotoEnable = HasNameUnderCursor && !IsInclude;
        m_SearchMenu->Enable(idMenuGotoDeclaration,    GotoEnable);
        m_SearchMenu->Enable(idMenuGotoImplementation, GotoEnable);
        const bool FindEnable = HasNameUnderCursor && !IsInclude && m_NativeParser.GetParser().Done();
        m_SearchMenu->Enable(idMenuFindReferences, FindEnable);
        const bool IncludeEnable = HasNameUnderCursor && IsInclude;
        m_SearchMenu->Enable(idMenuOpenIncludeFile, IncludeEnable);
    }

    if (m_ViewMenu)
    {
        bool isVis = IsWindowReallyShown((wxWindow*)m_NativeParser.GetClassBrowser());
        m_ViewMenu->Check(idViewClassBrowser, isVis);
    }

    if (m_ProjectMenu)
    {
        cbProject* project = m_NativeParser.GetCurrentProject();
        m_ProjectMenu->Enable(idCurrentProjectReparse, project);
    }

    // must do...
    event.Skip();
}

void CodeCompletion::OnCodeComplete(wxCommandEvent& event)
{
    if (!Manager::Get()->GetConfigManager(_T("code_completion"))->ReadBool(_T("/use_code_completion"), true))
        return;
    if (IsAttached() && m_InitDone)
        DoCodeComplete();
    event.Skip();
}

void CodeCompletion::OnShowCallTip(wxCommandEvent& event)
{
    if (IsAttached() && m_InitDone)
        ShowCallTip();
    event.Skip();
}

void CodeCompletion::OnGotoFunction(wxCommandEvent& event)
{
    EditorManager* edMan = Manager::Get()->GetEditorManager();
    cbEditor* ed = edMan->GetBuiltinActiveEditor();
    if (!ed)
        return;

    wxCriticalSectionLocker locker(s_TokensTreeCritical);
    m_NativeParser.GetTempParser().ParseBufferForFunctions(ed->GetControl()->GetText());

    wxArrayString funcs;
    TokensTree* tmptree = m_NativeParser.GetTempParser().GetTempTokensTree();
    if (tmptree->empty())
    {
        cbMessageBox(_("No functions parsed in this file..."));
        return;
    }

    wxArrayString tokens;
    SearchTree<Token*> tmpsearch;
    tokens.Clear();
    for (size_t i = 0; i < tmptree->size(); i++)
    {
        Token* token = tmptree->at(i);
        if (token && token->m_TokenKind & tkAnyFunction)
        {
            tokens.Add(token->DisplayName());
            tmpsearch.AddItem(token->DisplayName(), token);
        }
    }

    IncrementalSelectIteratorStringArray iterator(tokens);
    IncrementalSelectListDlg dlg(Manager::Get()->GetAppWindow(), iterator,
                                 _("Select function..."), _("Please select function to go to:"));
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxString sel = dlg.GetStringSelection();
        Token* token = tmpsearch.GetItem(sel);
        if (token)
        {
            CCLogger::Get()->DebugLog(F(_T("Token found at line %d"), token->m_Line));
            ed->GotoLine(token->m_Line - 1);
        }
    }

    tmptree->clear();
}

void CodeCompletion::OnGotoPrevFunction(wxCommandEvent& event)
{
    GotoFunctionPrevNext(); // prev function
}

void CodeCompletion::OnGotoNextFunction(wxCommandEvent& event)
{
    GotoFunctionPrevNext(true); // next function
}

void CodeCompletion::OnClassMethod(wxCommandEvent& event)
{
    DoClassMethodDeclImpl();
}

void CodeCompletion::OnUnimplementedClassMethods(wxCommandEvent& event)
{
    DoAllMethodsImpl();
}

void CodeCompletion::OnGotoDeclaration(wxCommandEvent& event)
{
    EditorManager* edMan = Manager::Get()->GetEditorManager();
    cbEditor* editor = edMan->GetBuiltinActiveEditor();
    if (!editor)
        return;

    const int pos = editor->GetControl()->GetCurrentPos();
    const int start = editor->GetControl()->WordStartPosition(pos, true);
    const int end = editor->GetControl()->WordEndPosition(pos, true);
    wxString target;
    if (GetLastNonWhitespaceChar(editor->GetControl(), start) == _T('~'))
        target << _T('~');
    target << editor->GetControl()->GetTextRange(start, end);
    if (target.IsEmpty())
        return;

    // prepare a boolean filter for declaration/implementation
    bool isDecl = event.GetId() == idGotoDeclaration || event.GetId() == idMenuGotoDeclaration;
    bool isImpl = event.GetId() == idGotoImplementation || event.GetId() == idMenuGotoImplementation;

    // get the matching set
    TokenIdxSet result;
    m_NativeParser.MarkItemsByAI(result, true, false, true, end);

    wxCriticalSectionLocker locker(s_TokensTreeCritical);
    TokensTree* tokens = m_NativeParser.GetParser().GetTokensTree();

    // special handle destructor function
    if (target[0] == _T('~'))
    {
        TokenIdxSet tmp = result;
        result.clear();

        for (TokenIdxSet::iterator it = tmp.begin(); it != tmp.end(); ++it)
        {
            Token* tk = tokens->at(*it);
            if (tk && tk->m_TokenKind == tkClass)
            {
                tk = tokens->at(tokens->TokenExists(target, tk->GetSelf(), tkDestructor));
                if (tk)
                    result.insert(tk->GetSelf());
            }
        }
    }

    // special handle constructor function
    else
    {
        bool isClassOrConstructor = false;
        for (TokenIdxSet::iterator it = result.begin(); it != result.end(); ++it)
        {
            Token* tk = tokens->at(*it);
            if (tk && (tk->m_TokenKind == tkClass || tk->m_TokenKind == tkConstructor))
            {
                isClassOrConstructor = true;
                break;
            }
        }
        if (isClassOrConstructor)
        {
            const bool isConstructor = (GetNextNonWhitespaceChar(editor->GetControl(), end) == _T('('));
            for (TokenIdxSet::iterator it = result.begin(); it != result.end();)
            {
                Token* tk = tokens->at(*it);
                if (isConstructor && tk && tk->m_TokenKind == tkClass)
                    result.erase(it++);
                else if (!isConstructor && tk && tk->m_TokenKind == tkConstructor)
                    result.erase(it++);
                else
                    ++it;
            }
        }
    }

    // special handle for function overloading
    if (result.size() > 1)
    {
        const size_t curLine = editor->GetControl()->GetCurrentLine() + 1;
        for (TokenIdxSet::iterator it = result.begin(); it != result.end(); ++it)
        {
            Token* tk = tokens->at(*it);
            if (tk)
            {
                if (tk->m_Line == curLine || tk->m_ImplLine == curLine)
                {
                    const int theOnlyOne = *it;
                    result.clear();
                    result.insert(theOnlyOne);
                    break;
                }
            }
        }
    }

    // one match
    Token* token = NULL;
    if (result.size() == 1)
    {
        Token* sel = tokens->at(*(result.begin()));
        if (   (isImpl && !sel->GetImplFilename().IsEmpty())
            || (isDecl && !sel->GetFilename().IsEmpty()) )
        {
            token = sel;
        }
    }
    // if more than one match, display a selection dialog
    else if (result.size() > 1)
    {
        // TODO: we could parse the line containing the text so
        // if namespaces were included, we could limit the results (and be more accurate)
        wxArrayString selections;
        wxArrayInt int_selections;
        for (TokenIdxSet::iterator it = result.begin(); it != result.end(); ++it)
        {
            Token* sel = tokens->at(*it);
            if (sel)
            {
                // only match tokens that have filename info
                if (   (isImpl && !sel->GetImplFilename().IsEmpty())
                    || (isDecl && !sel->GetFilename().IsEmpty()) )
                {
                    selections.Add(sel->DisplayName());
                    int_selections.Add(*it);
                }
            }
        }
        if (selections.GetCount() > 1)
        {
            int sel = wxGetSingleChoiceIndex(_("Please make a selection:"), _("Multiple matches"), selections);
            if (sel == -1)
                return;
            token = tokens->at(int_selections[sel]);
        }
        else if (selections.GetCount() == 1)
        {    // number of selections can be < result.size() due to the if tests, so in case we fall
            // back on 1 entry no need to show a selection
            token = tokens->at(int_selections[0]);
        }
    }

    // do we have a token?
    if (token)
    {
        if (isImpl)
        {
            if (cbEditor* ed = edMan->Open(token->GetImplFilename()))
                GotoTokenPosition(ed, target, token->m_ImplLine - 1);
            else
                cbMessageBox(wxString::Format(_("Implementation not found: %s"), target.wx_str()),
                             _("Warning"), wxICON_WARNING);
        }
        else
        {
            if (cbEditor* ed = edMan->Open(token->GetFilename()))
                GotoTokenPosition(ed, target, token->m_Line - 1);
            else
                cbMessageBox(wxString::Format(_("Declaration not found: %s"), target.wx_str()),
                             _("Warning"), wxICON_WARNING);
        }
    }
    else
    {
        cbMessageBox(wxString::Format(_("Not found: %s"), target.wx_str()), _("Warning"), wxICON_WARNING);
    }
}

void CodeCompletion::OnFindReferences(wxCommandEvent& event)
{
    m_CodeRefactoring.FindReferences();
}

void CodeCompletion::OnRenameSymbols(wxCommandEvent& event)
{
    m_CodeRefactoring.RenameSymbols();
}

void CodeCompletion::OnOpenIncludeFile(wxCommandEvent& event)
{
    wxString lastIncludeFileFrom;
    cbEditor* editor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (editor)
        lastIncludeFileFrom = editor->GetFilename();

    // check one more time because menu entries are enabled only when it makes sense
    // but the shortcut accelerator can always be executed
    bool MoveOn = false;
    wxString NameUnderCursor;
    bool IsInclude = false;
    if (EditorHasNameUnderCursor(NameUnderCursor, IsInclude))
    {
        if (IsInclude)
            MoveOn = true;
    }

    if (!MoveOn)
        return; // nothing under cursor or thing under cursor is not an include

    wxArrayString foundSet = m_NativeParser.GetParser().FindFileInIncludeDirs(NameUnderCursor); // search in all parser's include dirs

    // look in the same dir as the source file
    wxFileName fname = NameUnderCursor;
    NormalizePath(fname, lastIncludeFileFrom);
    if (wxFileExists(fname.GetFullPath()) )
        foundSet.Add(fname.GetFullPath());

    // search for the file in project files
    cbProject* project = m_NativeParser.GetProjectByEditor(editor);
    if (project)
    {
        for (int i = 0; i < project->GetFilesCount(); ++i)
        {
            ProjectFile* pf = project->GetFile(i);
            if (!pf)
                continue;

            if (IsSuffixOfPath(NameUnderCursor, pf->file.GetFullPath()))
                foundSet.Add(pf->file.GetFullPath());
        }
    }

    // Remove duplicates
    for (int i = 0; i < (int)foundSet.Count() - 1; i++)
    {
        for (int j = i + 1; j < (int)foundSet.Count(); )
        {
            if (foundSet.Item(i) == foundSet.Item(j))
                foundSet.RemoveAt(j);
            else
                j++;
        }
    }

    wxString selectedFile;
    if (foundSet.GetCount() > 1)
    {    // more than 1 hit : let the user choose
        SelectIncludeFile Dialog(Manager::Get()->GetAppWindow());
        Dialog.AddListEntries(foundSet);
        PlaceWindow(&Dialog);
        if(Dialog.ShowModal() == wxID_OK)
            selectedFile = Dialog.GetIncludeFile();
        else
            return; // user cancelled the dialog...
    }
    else if (foundSet.GetCount() == 1)
        selectedFile = foundSet[0];

    if (!selectedFile.IsEmpty())
    {
        EditorManager* edMan = Manager::Get()->GetEditorManager();
        edMan->Open(selectedFile);
        return;
    }

    cbMessageBox(wxString::Format(_("Not found: %s"), NameUnderCursor.c_str()), _("Warning"), wxICON_WARNING);
}

void CodeCompletion::OnCurrentProjectReparse(wxCommandEvent& event)
{
    m_NativeParser.ReparseCurrentProject();
    event.Skip();
}

void CodeCompletion::OnSelectedProjectReparse(wxCommandEvent& event)
{
    m_NativeParser.ReparseSelectedProject();
    event.Skip();
}

void CodeCompletion::OnSelectedFileReparse(wxCommandEvent& event)
{
    wxTreeCtrl* tree = Manager::Get()->GetProjectManager()->GetTree();
    if (!tree)
        return;

    wxTreeItemId treeItem =  tree->GetSelection();
    if (!treeItem.IsOk())
        return;

    const FileTreeData* data = static_cast<FileTreeData*>(tree->GetItemData(treeItem));
    if (!data)
        return;

    if (data->GetKind() == FileTreeData::ftdkFile)
    {
        cbProject* project = data->GetProject();
        ProjectFile* pf = project->GetFile(data->GetFileIndex());
        if (pf && m_NativeParser.ReparseFile(project, pf->file.GetFullPath()))
        {
             CCLogger::Get()->DebugLog(_T("Reparsing the selected file ") +
                                                          pf->file.GetFullPath());
        }
    }

    event.Skip();
}

void CodeCompletion::EditorEventHook(cbEditor* editor, wxScintillaEvent& event)
{
    if (!IsAttached() || !m_InitDone || !m_UseCodeCompletion)
    {
        event.Skip();
        return;
    }

    cbStyledTextCtrl* control = editor->GetControl();

//    if (event.GetEventType() == wxEVT_SCI_CHARADDED)
//        CCLogger::Get()->DebugLog(_T("wxEVT_SCI_CHARADDED"));
//    else if (event.GetEventType() == wxEVT_SCI_CHANGE)
//        CCLogger::Get()->DebugLog(_T("wxEVT_SCI_CHANGE"));
//    else if (event.GetEventType() == wxEVT_SCI_KEY)
//        CCLogger::Get()->DebugLog(_T("wxEVT_SCI_KEY"));
//    else if (event.GetEventType() == wxEVT_SCI_MODIFIED)
//        CCLogger::Get()->DebugLog(_T("wxEVT_SCI_MODIFIED"));

    if ((event.GetKey() == '.') && control->AutoCompActive())
        control->AutoCompCancel();

    if (event.GetEventType() == wxEVT_SCI_AUTOCOMP_SELECTION)
    {
        wxString itemText = event.GetText();
        int curPos = control->GetCurrentPos();
        int startPos = control->WordStartPosition(curPos, true);
        if (   itemText.GetChar(0) == _T('~') // special handle for dtor
            && startPos > 0
            && control->GetCharAt(startPos - 1) == _T('~'))
        {
            --startPos;
        }
        const int endPos = control->WordEndPosition(curPos, true);
        bool needReparse = false;

        if (control->IsPreprocessor(control->GetStyleAt(curPos)))
        {
            control->DelLineRight();
            needReparse = true;

            int pos = startPos;
            wxChar ch = control->GetCharAt(pos);
            while (ch != _T('<') && ch != _T('"') && ch != _T('#'))
                ch = control->GetCharAt(--pos);
            if (ch == _T('<') || ch == _T('"'))
                startPos = pos + 1;
        }

        const wxString alreadyText = control->GetTextRange(curPos, endPos);
        if (!alreadyText.IsEmpty() && itemText.EndsWith(alreadyText))
            curPos = endPos;

        control->AutoCompCancel();
        control->SetTargetStart(startPos);
        control->SetTargetEnd(curPos);

        std::map<wxString, int>::const_iterator it = m_SearchItem.find(itemText);
        if (it != m_SearchItem.end())
        {
            //Check if there are brace behind the target
            wxString addString(itemText);
            if (control->GetCharAt(curPos) != _T('('))
                addString += _T("()");

            control->ReplaceTarget(addString);
            control->GotoPos(control->GetCurrentPos() + itemText.size() + 2);
            if ((*it).second != 0)
            {
                control->GotoPos(control->GetCurrentPos() - 1);
                control->EnableTabSmartJump();
                ShowCallTip();
            }
        }
        else
        {
            if (control->IsPreprocessor(control->GetStyleAt(curPos)))
            {
                const wxChar start = control->GetCharAt(startPos - 1);
                if (start == _T('"'))
                    itemText << _T('"');
                else if (start == _T('<'))
                    itemText << _T('>');
            }

            control->ReplaceTarget(itemText);
            control->GotoPos(startPos + itemText.Length());

            if (needReparse)
                m_TimerRealtimeParsing.Start(1, wxTIMER_ONE_SHOT);
        }
    }

    if (event.GetEventType() == wxEVT_SCI_CHARADDED)
    {
        // a character was just added in the editor
        m_TimerCodeCompletion.Stop();
        const wxChar ch = event.GetKey();
        const int pos = control->GetCurrentPos();
        const int wordStartPos = control->WordStartPosition(pos, true);

        // -2 is used next because the char has already been added and Pos is ahead of it...
        const wxChar prevChar = control->GetCharAt(pos - 2);

        // if more than two chars have been typed, invoke CC
        const bool autoCC = m_CCAutoLaunch && (pos - wordStartPos >= m_CCAutoLaunchChars);

        // update calltip highlight while we type
        if (control->CallTipActive())
            ShowCallTip();

        // start calltip
        if (ch == _T('(') || ch == _T(','))
        {
            int style = control->GetStyleAt(control->GetCurrentPos() - 1);
            if (!(control->IsString(style) || control->IsCharacter(style) || control->IsComment(style)))
            {
                if (control->CallTipActive())
                    ++m_ActiveCalltipsNest;
                ShowCallTip();
            }
        }

        // support multi-line call tips
        else if (ch == _T('\n') && m_ActiveCalltipsNest > 0)
            ShowCallTip();

        // end calltip
        else if (ch == _T(')') || ch == _T(';'))
        {
            int style = control->GetStyleAt(control->GetCurrentPos() - 1);
            if (!(control->IsString(style) || control->IsCharacter(style) || control->IsComment(style)))
            {
                // cancel any active calltip
                control->CallTipCancel();
                if (m_ActiveCalltipsNest > 0)
                {
                    --m_ActiveCalltipsNest;
                    ShowCallTip();
                }
            }
        }

        // code completion
        else if (   (autoCC && !control->AutoCompActive()) // not already active autocompletion
                 || (prevChar == _T('"')) // #include "
                 || (prevChar == _T('<')) // #include <
                 || (ch == _T('.'))
                 || (ch == _T('#'))
                 || (   (ch == _T('>')) // ->
                     && (prevChar == _T('-')) )
                 || (   (ch == _T(':')) // ::
                     && (prevChar == _T(':')) ) )
        {
            int style = control->GetStyleAt(pos);
            //CCLogger::Get()->DebugLog(_T("Style at %d is %d (char '%c')"), pos, style, ch);
            if (prevChar == _T('"') || prevChar == _T('<'))
            {
                if (style != wxSCI_C_PREPROCESSOR)
                {
                    event.Skip();
                    return;
                }
            }
            else
            {
                if (style != wxSCI_C_DEFAULT && style != wxSCI_C_OPERATOR && style != wxSCI_C_IDENTIFIER)
                {
                    event.Skip();
                    return;
                }
            }

            if (autoCC || m_CCLaunchDelay == 0)
            {
                if (autoCC)
                    m_IsAutoPopup = true;
                DoCodeComplete();
                if (autoCC)
                    m_IsAutoPopup = false;
            }
            else
            {
                m_LastPosForCodeCompletion = pos;
                m_LastEditor = editor;
                m_TimerCodeCompletion.Start(m_CCLaunchDelay, wxTIMER_ONE_SHOT);
            }
        }
    }

    if (   m_NativeParser.GetParser().Options().whileTyping
        && (   (event.GetModificationType() & wxSCI_MOD_INSERTTEXT)
            || (event.GetModificationType() & wxSCI_MOD_DELETETEXT) ) )
    {
        m_NeedReparse = true;
    }
    if (control->GetCurrentLine() != m_CurrentLine)
    {
        if (m_NeedReparse)
        {
            m_TimerRealtimeParsing.Start(REALTIME_PARSING_DELAY, wxTIMER_ONE_SHOT);
            m_CurrentLength = control->GetLength();
            m_NeedReparse = false;
        }

        if (event.GetEventType() == wxEVT_SCI_UPDATEUI)
            m_TimerToolbar.Start(EDITOR_AND_LINE_INTERVAL, wxTIMER_ONE_SHOT);
    }

    // allow others to handle this event
    event.Skip();
}

void CodeCompletion::OnScope(wxCommandEvent&)
{
    int sel = m_Scope->GetSelection();
    if (sel != -1 && sel < static_cast<int>(m_ScopeMarks.size()))
    {
        UpdateFunctions(sel);
    }
}

void CodeCompletion::OnFunction(wxCommandEvent& /*event*/)
{
    int selSc = (m_Scope) ? m_Scope->GetSelection() : 0;
    if (selSc != -1 && selSc < static_cast<int>(m_ScopeMarks.size()))
    {
        int idxFn = m_ScopeMarks[selSc] + m_Function->GetSelection();
        if (idxFn != -1 && idxFn < static_cast<int>(m_FunctionsScope.size()))
        {
            int Line = m_FunctionsScope[idxFn].StartLine;
            cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
            if (!ed)
                return;
            ed->GotoLine(Line);
            ed->SetFocus();
        }
    }
}

void CodeCompletion::OnCCLogger(wxCommandEvent& event)
{
    if (!Manager::IsAppShuttingDown())
        Manager::Get()->GetLogManager()->Log(event.GetString());
}

void CodeCompletion::OnCCDebugLogger(wxCommandEvent& event)
{
    if (!Manager::IsAppShuttingDown())
        Manager::Get()->GetLogManager()->DebugLog(event.GetString());
}

void CodeCompletion::OnParserStart(wxCommandEvent& event)
{
    cbProject* project = static_cast<cbProject*>(event.GetClientData());
    ParsingType type = static_cast<ParsingType>(event.GetInt());

    if (type == ptCreateParser)
    {
        if (m_CCEnableHeaders)
        {
            wxArrayString& dirs = GetSystemIncludeDirs(project, true);
            SystemHeadersThread* thread = new SystemHeadersThread(this, m_SystemHeadersMap, dirs);
            m_SystemHeadersThread.push_back(thread);
        }

        cbEditor* editor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
        if (m_NativeParser.GetProjectByEditor(editor) == project)
            EnableToolbarTools(false);
    }
}

void CodeCompletion::OnParserEnd(wxCommandEvent& event)
{
    ParsingType type = static_cast<ParsingType>(event.GetInt());
    if (type == ptCreateParser)
    {
        if (   m_CCEnableHeaders
            && !m_SystemHeadersThread.empty()
            && !m_SystemHeadersThread.front()->IsRunning()
            && m_NativeParser.Done() )
        {
            m_SystemHeadersThread.front()->Run();
        }
    }

    cbEditor* editor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (editor)
        ParseFunctionsAndFillToolbar(true);

    event.Skip();
}

void CodeCompletion::EnableToolbarTools(bool enable)
{
    if (m_Scope)
        m_Scope->Enable(enable);
    if (m_Function)
        m_Function->Enable(enable);
}

void CodeCompletion::OnRealtimeParsing(wxTimerEvent& event)
{
    cbEditor* editor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!editor)
        return;
    const int curLen = editor->GetControl()->GetLength();
    if (curLen != m_CurrentLength)
    {
        m_CurrentLength = curLen;
        m_TimerRealtimeParsing.Start(REALTIME_PARSING_DELAY, wxTIMER_ONE_SHOT);
        return;
    }

    cbProject* project = m_NativeParser.GetProjectByEditor(editor);
    if (project && !project->GetFileByFilename(m_LastFile, false, true))
        return;
    if (m_NativeParser.ReparseFile(project, m_LastFile))
        CCLogger::Get()->DebugLog(_T("Reparsing when typing for editor ") + m_LastFile);
}

void CodeCompletion::GotoTokenPosition(cbEditor* editor, const wxString& target, size_t line)
{
    if (!editor)
        return;
    cbStyledTextCtrl* control = editor->GetControl();
    control->GotoLine(line);
    const int start = control->GetCurrentPos();
    const int end = start + control->LineLength(line);
    int tokenPos = control->FindText(start, end, target, wxSCI_FIND_WHOLEWORD | wxSCI_FIND_MATCHCASE, nullptr);
    if (tokenPos == wxSCI_INVALID_POSITION)
        tokenPos = start;
    control->GotoPos(tokenPos);
}

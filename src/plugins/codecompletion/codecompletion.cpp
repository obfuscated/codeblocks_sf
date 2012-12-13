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
#include <filegroupsandmasks.h>
#include <incrementalselectlistdlg.h>
#include <multiselectdlg.h>

#include "codecompletion.h"

#include "ccoptionsdlg.h"
#include "ccoptionsprjdlg.h"
#include "insertclassmethoddlg.h"
#include "selectincludefile.h"
#include "parser/ccdebuginfo.h"
#include "parser/cclogger.h"
#include "parser/parser.h"
#include "parser/tokenizer.h"

#define CC_CODECOMPLETION_DEBUG_OUTPUT 0

#if defined(CC_GLOBAL_DEBUG_OUTPUT)
    #if CC_GLOBAL_DEBUG_OUTPUT == 1
        #undef CC_CODECOMPLETION_DEBUG_OUTPUT
        #define CC_CODECOMPLETION_DEBUG_OUTPUT 1
    #elif CC_GLOBAL_DEBUG_OUTPUT == 2
        #undef CC_CODECOMPLETION_DEBUG_OUTPUT
        #define CC_CODECOMPLETION_DEBUG_OUTPUT 2
    #endif
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

static wxString g_GlobalScope(_T("<global>"));

// this auto-registers the plugin
namespace
{
    PluginRegistrant<CodeCompletion> reg(_T("CodeCompletion"));
}

namespace CodeCompletionHelper
{
    // compare method for the sort algorithm for our FunctionScope struct
    inline bool LessFunctionScope(const CodeCompletion::FunctionScope& fs1, const CodeCompletion::FunctionScope& fs2)
    {
        int result = wxStricmp(fs1.Scope, fs2.Scope);
        if (result == 0)
        {
            result = wxStricmp(fs1.Name, fs2.Name);
            if (result == 0)
                result = fs1.StartLine - fs2.StartLine;
        }

        return result < 0;
    }

    inline bool EqualFunctionScope(const CodeCompletion::FunctionScope& fs1, const CodeCompletion::FunctionScope& fs2)
    {
        int result = wxStricmp(fs1.Scope, fs2.Scope);
        if (result == 0)
            result = wxStricmp(fs1.Name, fs2.Name);

        return result == 0;
    }

    inline bool LessNameSpace(const NameSpace& ns1, const NameSpace& ns2)
    {
        return ns1.Name < ns2.Name;
    }

    inline bool EqualNameSpace(const NameSpace& ns1, const NameSpace& ns2)
    {
        return ns1.Name == ns2.Name;
    }

    // for OnGotoFunction()
    inline wxChar GetLastNonWhitespaceChar(cbStyledTextCtrl* control, int position)
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

    // for OnGotoFunction()
    inline wxChar GetNextNonWhitespaceChar(cbStyledTextCtrl* control, int position)
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

    // Sorting in GetLocalIncludeDirs()
    inline int CompareStringLen(const wxString& first, const wxString& second)
    {
        return second.Len() - first.Len();
    }

    // for CodeCompleteIncludes()
    inline bool TestIncludeLine(wxString const &line)
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

    // for CodeCompleteIncludes()
    inline void GetStringFromSet(wxString& str, const StringSet& s, const wxString& separator)
    {
        size_t totalLen = 0;
        for (StringSet::const_iterator it = s.begin(); it != s.end(); ++it)
            totalLen += (*it).Len();
        str.Clear();
        str.Alloc(totalLen + s.size() * separator.Len() + 1);
        for (StringSet::const_iterator it = s.begin(); it != s.end(); ++it)
            str << *it << separator;
    }

    // invariant : on return true : NameUnderCursor is NOT empty
    inline bool EditorHasNameUnderCursor(wxString& NameUnderCursor, bool& IsInclude)
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

    // for CodeComplete()
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

    struct GotoDeclarationItem
    {
      wxString filename;
      unsigned line;
    };
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

// bitmap for #include file listings
/* XPM */
static const char* header_file_xpm[] = {
"16 16 9 1",
"   c None",
"+  c #D23E39",
"$  c #CF0C0A",
"@  c #CB524B",
"&  c #E2D8D8",
"#  c #C7C7C4",
"_  c #E4B9B5",
"-  c #F7F9F7",
"=  c #EBE9E7",
"  #########     ",
"  #=-----####   ",
"  #--------=##  ",
"  #--------=-#  ",
"  #--=@_-----#  ",
"  #--=+_-----#  ",
"  #--=++@_---#  ",
"  #--&$@@$=--#  ",
"  #--&$__$&=-#  ",
"  #--&$__$&=-#  ",
"  #--&$__$&=-#  ",
"  #-==#=&#==-#  ",
"  #-========-#  ",
"  #----=====-#  ",
"  ############  ",
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
int idRealtimeParsingTimer      = wxNewId();
int idToolbarTimer              = wxNewId();
int idProjectSavedTimer         = wxNewId();
int idReparsingTimer            = wxNewId();
int idTimerEditorActivated      = wxNewId();

// milliseconds
#define REALTIME_PARSING_DELAY    500
#define TOOLBAR_REFRESH_DELAY     150
#define EDITOR_ACTIVATED_DELAY    300

BEGIN_EVENT_TABLE(CodeCompletion, cbCodeCompletionPlugin)
    EVT_UPDATE_UI_RANGE(idMenuCodeComplete, idCurrentProjectReparse, CodeCompletion::OnUpdateUI)

    EVT_MENU(idMenuCodeComplete,                   CodeCompletion::OnCodeComplete             )
    EVT_MENU(idMenuShowCallTip,                    CodeCompletion::OnShowCallTip              )
    EVT_MENU(idMenuGotoFunction,                   CodeCompletion::OnGotoFunction             )
    EVT_MENU(idMenuGotoPrevFunction,               CodeCompletion::OnGotoPrevFunction         )
    EVT_MENU(idMenuGotoNextFunction,               CodeCompletion::OnGotoNextFunction         )
    EVT_MENU(idMenuGotoDeclaration,                CodeCompletion::OnGotoDeclaration          )
    EVT_MENU(idMenuGotoImplementation,             CodeCompletion::OnGotoDeclaration          )
    EVT_MENU(idMenuFindReferences,                 CodeCompletion::OnFindReferences           )
    EVT_MENU(idMenuRenameSymbols,                  CodeCompletion::OnRenameSymbols            )
    EVT_MENU(idClassMethod,                        CodeCompletion::OnClassMethod              )
    EVT_MENU(idUnimplementedClassMethods,          CodeCompletion::OnUnimplementedClassMethods)
    EVT_MENU(idGotoDeclaration,                    CodeCompletion::OnGotoDeclaration          )
    EVT_MENU(idGotoImplementation,                 CodeCompletion::OnGotoDeclaration          )
    EVT_MENU(idOpenIncludeFile,                    CodeCompletion::OnOpenIncludeFile          )
    EVT_MENU(idMenuOpenIncludeFile,                CodeCompletion::OnOpenIncludeFile          )

    EVT_MENU(idViewClassBrowser,                   CodeCompletion::OnViewClassBrowser      )
    EVT_MENU(idCurrentProjectReparse,              CodeCompletion::OnCurrentProjectReparse )
    EVT_MENU(idSelectedProjectReparse,             CodeCompletion::OnSelectedProjectReparse)
    EVT_MENU(idSelectedFileReparse,                CodeCompletion::OnSelectedFileReparse   )

    EVT_TIMER(idCodeCompleteTimer,                 CodeCompletion::OnCodeCompleteTimer    )
    EVT_TIMER(idRealtimeParsingTimer,              CodeCompletion::OnRealtimeParsingTimer )
    EVT_TIMER(idToolbarTimer,                      CodeCompletion::OnToolbarTimer         )
    EVT_TIMER(idProjectSavedTimer,                 CodeCompletion::OnProjectSavedTimer    )
    EVT_TIMER(idReparsingTimer,                    CodeCompletion::OnReparsingTimer       )
    EVT_TIMER(idTimerEditorActivated,              CodeCompletion::OnEditorActivatedTimer )

    EVT_CHOICE(XRCID("chcCodeCompletionScope"),    CodeCompletion::OnScope   )
    EVT_CHOICE(XRCID("chcCodeCompletionFunction"), CodeCompletion::OnFunction)

    EVT_MENU(SystemHeadersThreadHelper::idSystemHeadersThreadUpdate,    CodeCompletion::OnSystemHeadersThreadUpdate    )
    EVT_MENU(SystemHeadersThreadHelper::idSystemHeadersThreadCompleted, CodeCompletion::OnSystemHeadersThreadCompletion)
    EVT_MENU(SystemHeadersThreadHelper::idSystemHeadersThreadError,     CodeCompletion::OnSystemHeadersThreadError     )
END_EVENT_TABLE()

CodeCompletion::CodeCompletion() :
    m_InitDone(false),
    m_CodeRefactoring(m_NativeParser),
    m_EditorHookId(0),
    m_TimerCodeCompletion(this, idCodeCompleteTimer),
    m_TimerRealtimeParsing(this, idRealtimeParsingTimer),
    m_TimerToolbar(this, idToolbarTimer),
    m_TimerProjectSaved(this, idProjectSavedTimer),
    m_TimerReparsing(this, idReparsingTimer),
    m_TimerEditorActivated(this, idTimerEditorActivated),
    m_LastEditor(0),
    m_ActiveCalltipsNest(0),
    m_IsAutoPopup(false),
    m_CompletePPOnly(false),
    m_ToolBar(0),
    m_Function(0),
    m_Scope(0),
    m_ToolbarNeedRefresh(true),
    m_ToolbarNeedReparse(false),
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
    m_CCEnableHeaders(false),
    m_SystemHeadersThreadCS()
{
    CCLogger::Get()->Init(this, g_idCCLogger, g_idCCDebugLogger);

    if (!Manager::LoadResource(_T("codecompletion.zip")))
        NotifyMissingFile(_T("codecompletion.zip"));

    Connect(g_idCCLogger,                wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CodeCompletion::OnCCLogger));
    Connect(g_idCCDebugLogger,           wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CodeCompletion::OnCCDebugLogger));
    Connect(ParserCommon::idParserStart, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CodeCompletion::OnParserStart));
    Connect(ParserCommon::idParserEnd,   wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CodeCompletion::OnParserEnd));
}

CodeCompletion::~CodeCompletion()
{
    Disconnect(g_idCCLogger,                wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CodeCompletion::OnCCLogger));
    Disconnect(g_idCCDebugLogger,           wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CodeCompletion::OnCCDebugLogger));
    Disconnect(ParserCommon::idParserStart, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CodeCompletion::OnParserStart));
    Disconnect(ParserCommon::idParserEnd,   wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CodeCompletion::OnParserEnd));

    while (!m_SystemHeadersThreads.empty())
    {
        SystemHeadersThread* thread = m_SystemHeadersThreads.front();
        if (thread->IsAlive() && thread->IsRunning())
            thread->Wait();
        m_SystemHeadersThreads.pop_front();
    }
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
    m_ToolbarNeedRefresh = true; // by default

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

    pm->RegisterEventSink(cbEVT_APP_STARTUP_DONE,     new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnAppDoneStartup));

    pm->RegisterEventSink(cbEVT_WORKSPACE_CHANGED,    new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnWorkspaceChanged));

    pm->RegisterEventSink(cbEVT_PROJECT_ACTIVATE,     new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnProjectActivated));
    pm->RegisterEventSink(cbEVT_PROJECT_CLOSE,        new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnProjectClosed));
    pm->RegisterEventSink(cbEVT_PROJECT_SAVE,         new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnProjectSaved));
    pm->RegisterEventSink(cbEVT_PROJECT_FILE_ADDED,   new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnProjectFileAdded));
    pm->RegisterEventSink(cbEVT_PROJECT_FILE_REMOVED, new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnProjectFileRemoved));
    pm->RegisterEventSink(cbEVT_PROJECT_FILE_CHANGED, new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnProjectFileChanged));

    pm->RegisterEventSink(cbEVT_EDITOR_SAVE,          new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnEditorSaveOrModified));
    pm->RegisterEventSink(cbEVT_EDITOR_MODIFIED,      new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnEditorSaveOrModified));
    pm->RegisterEventSink(cbEVT_EDITOR_OPEN,          new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnEditorOpen));
    pm->RegisterEventSink(cbEVT_EDITOR_ACTIVATED,     new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnEditorActivated));
    pm->RegisterEventSink(cbEVT_EDITOR_TOOLTIP,       new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnEditorTooltip));
    pm->RegisterEventSink(cbEVT_EDITOR_CLOSE,         new cbEventFunctor<CodeCompletion, CodeBlocksEvent>(this, &CodeCompletion::OnEditorClosed));
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
    m_ToolbarNeedRefresh = false;

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

cbConfigurationPanel* CodeCompletion::GetConfigurationPanel(wxWindow* parent)
{
    return new CCOptionsDlg(parent, &m_NativeParser, this);
}

cbConfigurationPanel* CodeCompletion::GetProjectConfigurationPanel(wxWindow* parent, cbProject* project)
{
    return new CCOptionsProjectDlg(parent, project, &m_NativeParser);
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
        m_SearchMenu->Append(idMenuGotoFunction,       _("Goto function...\tCtrl-Alt-G"));
        m_SearchMenu->Append(idMenuGotoPrevFunction,   _("Goto previous function\tCtrl-PgUp"));
        m_SearchMenu->Append(idMenuGotoNextFunction,   _("Goto next function\tCtrl-PgDn"));
        m_SearchMenu->Append(idMenuGotoDeclaration,    _("Goto declaration\tCtrl-Shift-."));
        m_SearchMenu->Append(idMenuGotoImplementation, _("Goto implementation\tCtrl-."));
        m_SearchMenu->Append(idMenuFindReferences,     _("Find references\tAlt-."));
        m_SearchMenu->Append(idMenuOpenIncludeFile,    _("Open include file\tCtrl-Alt-."));
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

void CodeCompletion::BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data)
{
    // if not attached, exit
    if (!menu || !IsAttached() || !m_InitDone)
        return;

    if (type == mtEditorManager)
    {
        if (cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor())
        {
            if ( !IsProviderFor(ed) )
                return;
        }

        wxString NameUnderCursor;
        bool IsInclude = false;
        const bool nameUnderCursor = CodeCompletionHelper::EditorHasNameUnderCursor(NameUnderCursor, IsInclude);
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
    m_Scope    = XRCCTRL(*toolBar, "chcCodeCompletionScope",    wxChoice);

    m_ToolBar = toolBar;
    UpdateToolBar();
    EnableToolbarTools(false);

    return true;
}

bool CodeCompletion::IsProviderFor(cbEditor* ed)
{
    PluginsArray pa = Manager::Get()->GetPluginManager()->GetCodeCompletionOffers();
    for (unsigned int i=0; i<pa.Count(); ++i)
    {
        if ( (pa[i] != this) && static_cast<cbCodeCompletionPlugin*>(pa[i])->IsProviderFor(ed))
            return false;
    }
    return true;
}

int CodeCompletion::CodeComplete()
{
    const bool preprocessorOnly = m_CompletePPOnly;
    m_CompletePPOnly = false;

    if (!IsAttached() || !m_InitDone)
        return -1;

    EditorManager* edMan = Manager::Get()->GetEditorManager();
    cbEditor* ed = edMan->GetBuiltinActiveEditor();
    if (!ed)
        return -3;

    FileType ft = FileTypeOf(ed->GetShortName());
    const bool caseSens = m_NativeParser.GetParser().Options().caseSensitive;

    TRACE(_T("CodeComplete"));

    TokenIdxSet result;
    if (   m_NativeParser.MarkItemsByAI(result, m_NativeParser.GetParser().Options().useSmartSense, true, caseSens)
        || m_NativeParser.LastAISearchWasGlobal() ) // enter even if no match (code-complete C++ keywords)
    {
        if (s_DebugSmartSense)
            CCLogger::Get()->DebugLog(F(_T("%lu results"), static_cast<unsigned long>(result.size())));
        TRACE(F(_T("%lu results"), static_cast<unsigned long>(result.size())));

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

            TokenTree* tree = m_NativeParser.GetParser().GetTokenTree();

            CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

            for (TokenIdxSet::const_iterator it = result.begin(); it != result.end(); ++it)
            {
                const Token* token = tree->at(*it);
                if (!token || token->m_Name.IsEmpty())
                    continue;

                // check hashmap for unique_strings
                if (unique_strings.find(token->m_Name) != unique_strings.end())
                    continue;

                if (preprocessorOnly && token->m_TokenKind != tkPreprocessor)
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
                    m_SearchItem[token->m_Name] = token->GetFormattedArgs().size() - 2;

                if (token->m_TokenKind == tkNamespace && token->m_Aliases.size())
                {
                    for (size_t i = 0; i < token->m_Aliases.size(); ++i)
                    {
                        if (unique_strings.find(token->m_Aliases[i]) != unique_strings.end())
                            continue;

                        unique_strings.insert(token->m_Aliases[i]);
                        tmp.Empty();
                        tmp << token->m_Aliases[i] << wxString::Format(_T("?%d"), iidx);
                        items.Add(tmp);
                    }
                }
            }

            CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

            if (m_NativeParser.LastAISearchWasGlobal() && !preprocessorOnly)
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
                items.Sort(CodeCompletionHelper::SortCCList);

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

void CodeCompletion::ShowCallTip()
{
    DoShowCallTip();
}

void CodeCompletion::DoShowCallTip(int caretPos)
{
    if (!IsAttached() || !m_InitDone)
        return;

    if (!Manager::Get()->GetEditorManager())
        return;

    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed)
        return;

    if ( !IsProviderFor(ed) )
        return;

    // calculate the size of the calltips window
    int pos = caretPos;
    if (pos == wxNOT_FOUND)
        pos = ed->GetControl()->GetCurrentPos();
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
    m_NativeParser.GetCallTips(maxCalltipLineSizeInChars, items, typedCommas, caretPos);
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

void CodeCompletion::CodeCompletePreprocessor()
{
    if (!IsAttached() || !m_InitDone)
        return;

    EditorManager* edMan = Manager::Get()->GetEditorManager();
    cbEditor* ed = edMan->GetBuiltinActiveEditor();
    if (!ed)
        return;

    cbStyledTextCtrl* control = ed->GetControl();
    if (control->GetLexer() != wxSCI_LEX_CPP)
    {
        const FileType fTp = FileTypeOf(ed->GetShortName());
        if (   fTp != ftSource
            && fTp != ftHeader
            && fTp != ftResource )
        {
            return; // not C/C++
        }
    }
    const int curPos    = control->GetCurrentPos();
    const int start     = control->WordStartPosition(curPos, true);
    const wxString text = control->GetTextRange(start, curPos);

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
    control->ClearRegisteredImages();
    for (int i = 0; i < (int)tokens.GetCount(); ++i)
    {
        if (!text.IsEmpty() && tokens[i][0] != text[0])
        {
            tokens.RemoveAt(i); // remove tokens that start with a different letter
            --i;
        }
        else
            tokens[i] += wxString::Format(wxT("?%d"), PARSER_IMG_PREPROCESSOR);
    }
    control->RegisterImage(PARSER_IMG_PREPROCESSOR,
                           m_NativeParser.GetImageList()->GetBitmap(PARSER_IMG_PREPROCESSOR));
    control->AutoCompSetIgnoreCase(false);
    control->AutoCompShow(curPos - start, GetStringFromArray(tokens, _T(" ")));
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
    if (line.IsEmpty() || !CodeCompletionHelper::TestIncludeLine(line))
        return;

    int keyPos = line.Find(_T('"'));
    if (keyPos == wxNOT_FOUND || keyPos >= pos - lineStartPos)
        keyPos = line.Find(_T('<'));
    if (keyPos == wxNOT_FOUND || keyPos >= pos - lineStartPos)
        return;
    ++keyPos;

    // now, we are after the quote prompt
    wxString filename = line.SubString(keyPos, pos - lineStartPos - 1);
    filename.Replace(_T("\\"), _T("/"), true);
    if (filename.Last() == _T('"') || filename.Last() == _T('>'))
        filename.RemoveLast();

    size_t maxFiles = m_CCMaxMatches;
    if (filename.IsEmpty() && maxFiles > 3000)
        maxFiles = 3000; // do not try to collect too many files if there is no context (prevent hang)

    // fill a list of matching files
    StringSet files;

    // #include < or #include "
    {
        wxCriticalSectionLocker locker(m_SystemHeadersThreadCS);
        wxArrayString& incDirs = GetSystemIncludeDirs(project, project ? project->GetModified() : true);
        for (size_t i = 0; i < incDirs.GetCount(); ++i)
        {
            SystemHeadersMap::const_iterator shm_it = m_SystemHeadersMap.find(incDirs[i]);
            if (shm_it != m_SystemHeadersMap.end())
            {
                const StringSet& headers = shm_it->second;
                for (StringSet::const_iterator ss_it = headers.begin(); ss_it != headers.end(); ++ss_it)
                {
                    const wxString& file = *ss_it;
                    if (file.StartsWith(filename))
                    {
                        files.insert(file);
                        if (files.size() > maxFiles)
                            break;
                    }
                }
            }
            if (files.size() > maxFiles)
                break;
        }
    }

    // #include "
    if (project)
    {
        const wxArrayString localIncludeDirs = GetLocalIncludeDirs(project, buildTargets);
        for (FilesList::const_iterator it = project->GetFilesList().begin();
                                       it != project->GetFilesList().end(); ++it)
        {
            pf = *it;
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
                    if (files.size() > maxFiles)
                        break;
                }
            }
        }
    }

    // popup the auto completion window
    if (!files.empty())
    {
        control->ClearRegisteredImages();
        control->RegisterImage(0, wxBitmap(header_file_xpm));
        control->AutoCompSetIgnoreCase(false);
        control->AutoCompSetCancelAtStart(true);
        control->AutoCompSetFillUps(m_CCFillupChars);
        control->AutoCompSetChooseSingle(false);
        control->AutoCompSetAutoHide(true);
        control->AutoCompSetDropRestOfWord(m_IsAutoPopup ? false : true);
        wxString final_str;
        CodeCompletionHelper::GetStringFromSet(final_str, files, _T("?0 "));
        final_str.RemoveLast(); // remove last space
        control->AutoCompShow(pos - lineStartPos - keyPos, final_str);
        CCLogger::Get()->DebugLog(F(_T("Get include file count is %lu, use time is %ld"),
                                    static_cast<unsigned long>(files.size()), sw.Time()));
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
            wxCriticalSectionLocker locker(m_SystemHeadersThreadCS);
            if (m_SystemHeadersMap.find(dirs[i]) == m_SystemHeadersMap.end())
                sysDirs.Add(dirs[i]);
            dirs.RemoveAt(i);
        }
    }

    if (!sysDirs.IsEmpty())
    {
        SystemHeadersThread* thread = new SystemHeadersThread(this, &m_SystemHeadersThreadCS, m_SystemHeadersMap, sysDirs);
        m_SystemHeadersThreads.push_back(thread);
        if (!m_SystemHeadersThreads.front()->IsRunning() && m_NativeParser.Done())
            thread->Run();
    }

    dirs.Sort(CodeCompletionHelper::CompareStringLen);
    return dirs;
}

wxArrayString& CodeCompletion::GetSystemIncludeDirs(cbProject* project, bool force)
{
    static cbProject*    lastProject = nullptr;
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

void CodeCompletion::EditorEventHook(cbEditor* editor, wxScintillaEvent& event)
{
    if (!IsAttached() || !m_InitDone || !m_UseCodeCompletion)
    {
        event.Skip();
        return;
    }

    if ( !IsProviderFor(editor) )
    {
        event.Skip();
        return;
    }

    cbStyledTextCtrl* control = editor->GetControl();

    if      (event.GetEventType() == wxEVT_SCI_CHARADDED)
    {   TRACE(_T("wxEVT_SCI_CHARADDED")); }
    else if (event.GetEventType() == wxEVT_SCI_CHANGE)
    {   TRACE(_T("wxEVT_SCI_CHANGE")); }
    else if (event.GetEventType() == wxEVT_SCI_KEY)
    {   TRACE(_T("wxEVT_SCI_KEY")); }
    else if (event.GetEventType() == wxEVT_SCI_MODIFIED)
    {   TRACE(_T("wxEVT_SCI_MODIFIED")); }
    else if (event.GetEventType() == wxEVT_SCI_AUTOCOMP_SELECTION)
    {   TRACE(_T("wxEVT_SCI_AUTOCOMP_SELECTION")); }
    else if (event.GetEventType() == wxEVT_SCI_AUTOCOMP_CANCELLED)
    {   TRACE(_T("wxEVT_SCI_AUTOCOMP_CANCELLED")); }

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
            curPos = control->GetLineEndPosition(control->GetCurrentLine()); // delete rest of line
            bool addComment = (itemText == wxT("endif"));
            for (int i = control->GetCurrentPos(); i < curPos; ++i)
            {
                if (control->IsComment(control->GetStyleAt(i)))
                {
                    curPos = i; // preserve line comment
                    if (wxIsspace(control->GetCharAt(i - 1)))
                        --curPos; // preserve a space before the comment
                    addComment = false;
                    break;
                }
            }
            if (addComment) // search backwards for the #if*
            {
                wxRegEx ppIf(wxT("^[ \t]*#[ \t]*if"));
                wxRegEx ppEnd(wxT("^[ \t]*#[ \t]*endif"));
                int depth = -1;
                for (int ppLine = control->GetCurrentLine() - 1; ppLine >= 0; --ppLine)
                {
                    if (control->GetLine(ppLine).Find(wxT('#')) != wxNOT_FOUND) // limit testing due to performance cost
                    {
                        if (ppIf.Matches(control->GetLine(ppLine))) // ignore else's, elif's, ...
                            ++depth;
                        else if (ppEnd.Matches(control->GetLine(ppLine)))
                            --depth;
                    }
                    if (depth == 0)
                    {
                        wxRegEx pp(wxT("^[ \t]*#[ \t]*[a-z]*([ \t]+([a-zA-Z0-9_]+)|())"));
                        pp.Matches(control->GetLine(ppLine));
                        if (!pp.GetMatch(control->GetLine(ppLine), 2).IsEmpty())
                            itemText.Append(wxT(" // ") + pp.GetMatch(control->GetLine(ppLine), 2));
                        break;
                    }
                }
            }
            needReparse = true;

            int   pos = startPos;
            wxChar ch = control->GetCharAt(pos);
            while (ch != _T('<') && ch != _T('"') && ch != _T('#') && (pos>0))
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
            // Check if there are brace behind the target
            wxString addString(itemText);
            if (control->GetCharAt(curPos) != _T('('))
                addString += _T("()");

            if (control->GetTextRange(startPos, curPos) != addString)
                control->ReplaceTarget(addString);
            control->GotoPos(startPos + itemText.size() + 2);
            if ((*it).second != 0)
            {
                control->CharLeft();
                control->EnableTabSmartJump();
                TRACE(_T("wxEVT_SCI_AUTOCOMP_SELECTION -> ShowCallTip"));
                ShowCallTip();
            }
        }
        else
        {
            if (control->IsPreprocessor(control->GetStyleAt(startPos)))
            {
                const wxChar start = control->GetCharAt(startPos - 1);
                if (start == _T('"'))
                    itemText << _T('"');
                else if (start == _T('<'))
                    itemText << _T('>');
            }

            if (control->GetTextRange(startPos, curPos) != itemText)
                control->ReplaceTarget(itemText);
            control->GotoPos(startPos + itemText.Length());

            if (needReparse)
            {
                TRACE(_T("EditorEventHook: Starting m_TimerRealtimeParsing."));
                m_TimerRealtimeParsing.Start(1, wxTIMER_ONE_SHOT);
            }
        }
        control->ChooseCaretX();
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
        {
            TRACE(_T("wxEVT_SCI_CHARADDED -> ShowCallTip"));
            ShowCallTip();
        }

        // start calltip
        if (ch == _T('(') || ch == _T(','))
        {
            int style = control->GetStyleAt(control->GetCurrentPos() - 1);
            if (!(control->IsString(style) || control->IsCharacter(style) || control->IsComment(style)))
            {
                if (control->CallTipActive())
                    ++m_ActiveCalltipsNest;
                TRACE(_T("wxEVT_SCI_CHARADDED -> ShowCallTip"));
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
                    TRACE(_T("wxEVT_SCI_CHARADDED -> ShowCallTip"));
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
                     && (prevChar == _T(':')) )
                 || (   control->AutoCompActive() // refine listing:
                     && (   ch == _T('/')         // for #include (reduce directories)
                         || (pos - wordStartPos == m_CCAutoLaunchChars + 4)) ) // for more typed characters
                                                                              )
        {
            int style = control->GetStyleAt(pos);
            TRACE(_T("Style at %d is %d (char '%c')"), pos, style, ch);
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
                if (   style != wxSCI_C_DEFAULT
                    && style != wxSCI_C_PREPROCESSOR
                    && style != wxSCI_C_OPERATOR
                    && style != wxSCI_C_IDENTIFIER
                    && style != wxSCI_C_WORD2
                    && style != wxSCI_C_GLOBALCLASS )
                {
                    event.Skip();
                    return;
                }
            }

            if (autoCC || m_CCLaunchDelay == 0)
            {
                if (autoCC)
                    m_IsAutoPopup = true;

                TRACE(_T("wxEVT_SCI_CHARADDED -> DoCodeComplete"));
                DoCodeComplete();

                if (autoCC)
                    m_IsAutoPopup = false;
            }
            else
            {
                m_LastPosForCodeCompletion = pos;
                m_LastEditor = editor;
                TRACE(_T("EditorEventHook: Starting m_TimerCodeCompletion."));
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
            TRACE(_T("EditorEventHook: Starting m_TimerRealtimeParsing."));
            m_TimerRealtimeParsing.Start(REALTIME_PARSING_DELAY, wxTIMER_ONE_SHOT);
            m_CurrentLength = control->GetLength();
            m_NeedReparse = false;
        }

        if (event.GetEventType() == wxEVT_SCI_UPDATEUI)
        {
            m_ToolbarNeedRefresh = true;
            TRACE(_T("EditorEventHook: Starting m_TimerToolbar."));
            if (m_TimerEditorActivated.IsRunning())
                m_TimerToolbar.Start(EDITOR_ACTIVATED_DELAY + 1, wxTIMER_ONE_SHOT);
            else
                m_TimerToolbar.Start(TOOLBAR_REFRESH_DELAY, wxTIMER_ONE_SHOT);
        }
    }

    // allow others to handle this event
    event.Skip();
}

void CodeCompletion::RereadOptions()
{
    // Keep this in sync with CCOptionsDlg::CCOptionsDlg and CCOptionsDlg::OnApply

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
    m_UseCodeCompletion    = cfg->ReadBool(_T("/use_code_completion"),  true);
    m_CCAutoLaunchChars    = cfg->ReadInt(_T("/auto_launch_chars"),     3);
    m_CCAutoLaunch         = cfg->ReadBool(_T("/auto_launch"),          true);
    m_CCLaunchDelay        = cfg->ReadInt(_T("/cc_delay"),              300);
    m_CCMaxMatches         = cfg->ReadInt(_T("/max_matches"),           16384);
    m_CCAutoAddParentheses = cfg->ReadBool(_T("/auto_add_parentheses"), true);
    m_CCFillupChars        = cfg->Read(_T("/fillup_chars"),             wxEmptyString);
    m_CCAutoSelectOne      = cfg->ReadBool(_T("/auto_select_one"),      false);
    m_CCEnableHeaders      = cfg->ReadBool(_T("/enable_headers"),       true);

    if (m_ToolBar)
    {
        UpdateToolBar();
        CodeBlocksLayoutEvent evt(cbEVT_UPDATE_VIEW_LAYOUT);
        Manager::Get()->ProcessEvent(evt);
        m_ToolbarNeedReparse = true;
        TRACE(_T("RereadOptions: Starting m_TimerToolbar."));
        m_TimerToolbar.Start(TOOLBAR_REFRESH_DELAY, wxTIMER_ONE_SHOT);
    }
}

void CodeCompletion::UpdateToolBar()
{
    bool showScope = Manager::Get()->GetConfigManager(_T("code_completion"))->ReadBool(_T("/scope_filter"), true);

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

void CodeCompletion::LoadTokenReplacements()
{
    ConfigManagerContainer::StringToStringMap repl;
    Manager::Get()->GetConfigManager(_T("code_completion"))->Read(_T("token_replacements"), &repl);

    // Keep this in sync with CodeCompletion::LoadTokenReplacements()

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

    // for GCC 4.6.x
    repl[_T("_GLIBCXX_VISIBILITY")]             = _T("+");
    repl[_T("_GLIBCXX_BEGIN_NAMESPACE_VERSION")]= _T("");
    repl[_T("_GLIBCXX_END_NAMESPACE_VERSION")]  = _T("");

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
    const wxStringHashMap& hashRepl = Tokenizer::GetTokenReplacementsMap();
    ConfigManagerContainer::StringToStringMap repl;
    wxStringHashMap::const_iterator it = hashRepl.begin();
    for (; it != hashRepl.end(); it++)
        repl[it->first] = it->second;

    Manager::Get()->GetConfigManager(_T("code_completion"))->Write(_T("token_replacements"), repl);
}

void CodeCompletion::OnUpdateUI(wxUpdateUIEvent& event)
{
    wxString NameUnderCursor;
    bool IsInclude = false;
    const bool HasNameUnderCursor = CodeCompletionHelper::EditorHasNameUnderCursor(NameUnderCursor, IsInclude);

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

void CodeCompletion::OnViewClassBrowser(wxCommandEvent& event)
{
    if (!Manager::Get()->GetConfigManager(_T("code_completion"))->ReadBool(_T("/use_symbols_browser"), true))
    {
        cbMessageBox(_("The symbols browser is disabled in code-completion options.\n"
                        "Please enable it there first..."), _("Information"), wxICON_INFORMATION);
        return;
    }
    CodeBlocksDockEvent evt(event.IsChecked() ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
    evt.pWindow = (wxWindow*)m_NativeParser.GetClassBrowser();
    Manager::Get()->ProcessEvent(evt);
}

void CodeCompletion::OnCodeComplete(wxCommandEvent& event)
{
    TRACE(_T("OnCodeComplete"));

    // Fire-up event
    CodeBlocksEvent evt(cbEVT_COMPLETE_CODE, 0, 0, 0, this);
    Manager::Get()->ProcessEvent(evt);
    Manager::Yield();

    if (!Manager::Get()->GetConfigManager(_T("code_completion"))->ReadBool(_T("/use_code_completion"), true))
        return;

    if (IsAttached() && m_InitDone)
        DoCodeComplete();

    event.Skip();
}

void CodeCompletion::OnShowCallTip(wxCommandEvent& event)
{
    TRACE(_T("OnShowCallTip"));

    // Fire-up event
    CodeBlocksEvent evt(cbEVT_SHOW_CALL_TIP, 0, 0, 0, this);
    Manager::Get()->ProcessEvent(evt);
    Manager::Yield();

    if (IsAttached() && m_InitDone)
        ShowCallTip();

    event.Skip();
}

void CodeCompletion::OnGotoFunction(cb_unused wxCommandEvent& event)
{
    EditorManager* edMan = Manager::Get()->GetEditorManager();
    cbEditor* ed = edMan->GetBuiltinActiveEditor();
    if (!ed)
        return;

    TRACE(_T("OnGotoFunction"));

    m_NativeParser.GetParser().ParseBufferForFunctions(ed->GetControl()->GetText());

    wxArrayString tokens;
    SearchTree<Token*> tmpsearch;

    TokenTree* tree = m_NativeParser.GetParser().GetTempTokenTree();

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    if (tree->empty())
        cbMessageBox(_("No functions parsed in this file..."));
    else
    {
        for (size_t i = 0; i < tree->size(); i++)
        {
            Token* token = tree->at(i);
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
            const Token* token = tmpsearch.GetItem(sel);
            if (ed && token)
            {
                TRACE(F(_T("OnGotoFunction() : Token '%s' found at line %u."), token->m_Name.wx_str(), token->m_Line));
                ed->GotoTokenPosition(token->m_Line - 1, token->m_Name);
            }
        }

        tree->clear();
    }

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)
}

void CodeCompletion::OnGotoPrevFunction(cb_unused wxCommandEvent& event)
{
    GotoFunctionPrevNext(); // prev function
}

void CodeCompletion::OnGotoNextFunction(cb_unused wxCommandEvent& event)
{
    GotoFunctionPrevNext(true); // next function
}

void CodeCompletion::OnClassMethod(cb_unused wxCommandEvent& event)
{
    DoClassMethodDeclImpl();
}

void CodeCompletion::OnUnimplementedClassMethods(cb_unused wxCommandEvent& event)
{
    DoAllMethodsImpl();
}

void CodeCompletion::OnGotoDeclaration(wxCommandEvent& event)
{
    EditorManager* edMan  = Manager::Get()->GetEditorManager();
    cbEditor*      editor = edMan->GetBuiltinActiveEditor();
    if (!editor)
        return;

    TRACE(_T("OnGotoDeclaration"));

    const int pos      = editor->GetControl()->GetCurrentPos();
    const int startPos = editor->GetControl()->WordStartPosition(pos, true);
    const int endPos   = editor->GetControl()->WordEndPosition(pos, true);
    wxString target;
    if (CodeCompletionHelper::GetLastNonWhitespaceChar(editor->GetControl(), startPos) == _T('~'))
        target << _T('~');
    target << editor->GetControl()->GetTextRange(startPos, endPos);
    if (target.IsEmpty())
        return;

    // prepare a boolean filter for declaration/implementation
    bool isDecl = event.GetId() == idGotoDeclaration    || event.GetId() == idMenuGotoDeclaration;
    bool isImpl = event.GetId() == idGotoImplementation || event.GetId() == idMenuGotoImplementation;

    // get the matching set
    TokenIdxSet result;
    m_NativeParser.MarkItemsByAI(result, true, false, true, endPos);

    TokenTree* tree = m_NativeParser.GetParser().GetTokenTree();

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    // special handle destructor function
    if (target[0] == _T('~'))
    {
        TokenIdxSet tmp = result;
        result.clear();

        for (TokenIdxSet::const_iterator it = tmp.begin(); it != tmp.end(); ++it)
        {
            const Token* token = tree->at(*it);
            if (token && token->m_TokenKind == tkClass)
            {
                token = tree->at(tree->TokenExists(target, token->m_Index, tkDestructor));
                if (token)
                    result.insert(token->m_Index);
            }
        }
    }
    // special handle constructor function
    else
    {
        bool isClassOrConstructor = false;
        for (TokenIdxSet::const_iterator it = result.begin(); it != result.end(); ++it)
        {
            const Token* token = tree->at(*it);
            if (token && (token->m_TokenKind == tkClass || token->m_TokenKind == tkConstructor))
            {
                isClassOrConstructor = true;
                break;
            }
        }
        if (isClassOrConstructor)
        {
            const bool isConstructor = CodeCompletionHelper::GetNextNonWhitespaceChar(editor->GetControl(), endPos)   == _T('(')
                                    && CodeCompletionHelper::GetLastNonWhitespaceChar(editor->GetControl(), startPos) == _T(':');
            for (TokenIdxSet::const_iterator it = result.begin(); it != result.end();)
            {
                const Token* token = tree->at(*it);
                if (isConstructor && token && token->m_TokenKind == tkClass)
                    result.erase(it++);
                else if (!isConstructor && token && token->m_TokenKind == tkConstructor)
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
        for (TokenIdxSet::const_iterator it = result.begin(); it != result.end(); ++it)
        {
            const Token* token = tree->at(*it);
            if (token && (token->m_Line == curLine || token->m_ImplLine == curLine) )
            {
                const int theOnlyOne = *it;
                result.clear();
                result.insert(theOnlyOne);
                break;
            }
        }
    }

    // data for the choice dialog
    std::deque<CodeCompletionHelper::GotoDeclarationItem> foundItems;
    wxArrayString selections;

    wxString editorFilename;
    unsigned editorLine = -1;
    bool     tokenFound = false;

    // one match
    if (result.size() == 1)
    {
        Token* token = NULL;
        Token* sel = tree->at(*(result.begin()));
        if (   (isImpl && !sel->GetImplFilename().IsEmpty())
            || (isDecl && !sel->GetFilename().IsEmpty()) )
        {
            token = sel;
        }
        if (token)
        {
            // FIXME: implement this correctly, because now it is not showing full results
            if (   wxGetKeyState(WXK_CONTROL)
                && wxGetKeyState(WXK_SHIFT)
                && (  event.GetId() == idGotoDeclaration
                   || event.GetId() == idGotoImplementation ) )
            {
                // FIXME: this  code can lead to a deadlock (because of double locking from single thread)
                CCDebugInfo info(nullptr, &m_NativeParser.GetParser(), token);
                info.ShowModal();
            }
            else if (isImpl)
            {
                editorFilename = token->GetImplFilename();
                editorLine     = token->m_ImplLine - 1;
            }
            else if (isDecl)
            {
                editorFilename = token->GetFilename();
                editorLine     = token->m_Line - 1;
            }

            tokenFound = true;
        }
    }
    // if more than one match, display a selection dialog
    else if (result.size() > 1)
    {
        // TODO: we could parse the line containing the text so
        // if namespaces were included, we could limit the results (and be more accurate)
        for (TokenIdxSet::const_iterator it = result.begin(); it != result.end(); ++it)
        {
            const Token* token = tree->at(*it);
            if (token)
            {
                CodeCompletionHelper::GotoDeclarationItem item;

                if (isImpl)
                {
                    item.filename = token->GetImplFilename();
                    item.line     = token->m_ImplLine - 1;
                }
                else if (isDecl)
                {
                    item.filename = token->GetFilename();
                    item.line     = token->m_Line - 1;
                }

                // only match tokens that have filename info
                if (!item.filename.empty())
                {
                    selections.Add(token->DisplayName());
                    foundItems.push_back(item);
                }
            }
        }
    }

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    if (selections.GetCount() > 1)
    {
        int sel = wxGetSingleChoiceIndex(_("Please make a selection:"), _("Multiple matches"), selections);
        if (sel == -1)
            return;

        const CodeCompletionHelper::GotoDeclarationItem &item = foundItems[sel];
        editorFilename = item.filename;
        editorLine     = item.line;
        tokenFound     = true;
    }
    else if (selections.GetCount() == 1)
    {
        // number of selections can be < result.size() due to the if tests, so in case we fall
        // back on 1 entry no need to show a selection
        const CodeCompletionHelper::GotoDeclarationItem &item = foundItems.front();
        editorFilename = item.filename;
        editorLine     = item.line;
        tokenFound     = true;
    }

    // do we have a token?
    if (tokenFound)
    {
        cbEditor* targetEditor = edMan->Open(editorFilename);
        if (targetEditor)
            targetEditor->GotoTokenPosition(editorLine, target);
        else
        {
            if (isImpl)
                cbMessageBox(wxString::Format(_("Implementation not found: %s"), target.wx_str()),
                             _("Warning"), wxICON_WARNING);
            else if (isDecl)
                cbMessageBox(wxString::Format(_("Declaration not found: %s"), target.wx_str()),
                             _("Warning"), wxICON_WARNING);
        }
    }
    else
        cbMessageBox(wxString::Format(_("Not found: %s"), target.wx_str()), _("Warning"), wxICON_WARNING);
}

void CodeCompletion::OnFindReferences(cb_unused wxCommandEvent& event)
{
    m_CodeRefactoring.FindReferences();
}

void CodeCompletion::OnRenameSymbols(cb_unused wxCommandEvent& event)
{
    m_CodeRefactoring.RenameSymbols();
}

void CodeCompletion::OnOpenIncludeFile(cb_unused wxCommandEvent& event)
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
    if (CodeCompletionHelper::EditorHasNameUnderCursor(NameUnderCursor, IsInclude))
    {
        if (IsInclude)
            MoveOn = true;
    }

    if (!MoveOn)
        return; // nothing under cursor or thing under cursor is not an include

    TRACE(_T("OnOpenIncludeFile"));

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
        for (FilesList::const_iterator it = project->GetFilesList().begin();
                                       it != project->GetFilesList().end(); ++it)
        {
            ProjectFile* pf = *it;
            if (!pf)
                continue;

            if ( IsSuffixOfPath(NameUnderCursor, pf->file.GetFullPath()) )
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
        if (Dialog.ShowModal() == wxID_OK)
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

    wxTreeItemId treeItem = Manager::Get()->GetProjectManager()->GetTreeSelection();
    if (!treeItem.IsOk())
        return;

    const FileTreeData* data = static_cast<FileTreeData*>(tree->GetItemData(treeItem));
    if (!data)
        return;

    if (data->GetKind() == FileTreeData::ftdkFile)
    {
        cbProject* project = data->GetProject();
        ProjectFile* pf = data->GetProjectFile();
        if (pf && m_NativeParser.ReparseFile(project, pf->file.GetFullPath()))
        {
             CCLogger::Get()->DebugLog(_T("Reparsing the selected file ") +
                                       pf->file.GetFullPath());
        }
    }

    event.Skip();
}

void CodeCompletion::OnAppDoneStartup(CodeBlocksEvent& event)
{
    if (!m_InitDone)
        DoParseOpenedProjectAndActiveEditor();

    event.Skip();
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
        TRACE(_T("OnWorkspaceChanged: Starting m_TimerToolbar."));
        m_TimerToolbar.Start(TOOLBAR_REFRESH_DELAY, wxTIMER_ONE_SHOT);

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
    TRACE(_T("OnProjectSaved: Starting m_TimerProjectSaved."));
    m_TimerProjectSaved.Start(200, wxTIMER_ONE_SHOT);

    event.Skip();
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

        TRACE(_T("OnEditorSaveOrModified: Starting m_TimerReparsing."));
        m_TimerReparsing.Start(EDITOR_ACTIVATED_DELAY + it->second.GetCount() * 10, wxTIMER_ONE_SHOT);
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

void CodeCompletion::OnEditorActivated(CodeBlocksEvent& event)
{
    TRACE(_T("OnEditorActivated"));

    if (!ProjectManager::IsBusy() && IsAttached() && m_InitDone && event.GetEditor())
    {
        if (event.GetEditor()->GetFilename() == g_StartHereTitle)
            m_LastEditor = nullptr;
        else
            m_LastEditor = Manager::Get()->GetEditorManager()->GetBuiltinEditor(event.GetEditor());

        TRACE(_T("OnEditorActivated: Starting m_TimerEditorActivated."));
        m_TimerEditorActivated.Start(EDITOR_ACTIVATED_DELAY, wxTIMER_ONE_SHOT);

        if (m_TimerToolbar.IsRunning())
            m_TimerToolbar.Stop();
    }

    event.Skip();
}

void CodeCompletion::OnEditorClosed(CodeBlocksEvent& event)
{
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

    TRACE(_T("OnEditorClosed() : Closed editor's file is %s"), activeFile.wx_str());

    if (m_LastEditor == event.GetEditor())
    {
        m_LastEditor = nullptr;
        if (m_TimerEditorActivated.IsRunning())
            m_TimerEditorActivated.Stop();
    }

    m_NativeParser.OnEditorClosed(event.GetEditor());
    m_LastFile.Clear();

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
    cbProject*                project = static_cast<cbProject*>(event.GetClientData());
    ParserCommon::ParserState state   = static_cast<ParserCommon::ParserState>(event.GetInt());
    if (state == ParserCommon::ptCreateParser)
    {
        if (m_CCEnableHeaders)
        {
            wxArrayString&       dirs   = GetSystemIncludeDirs(project, true);
            SystemHeadersThread* thread = new SystemHeadersThread(this, &m_SystemHeadersThreadCS, m_SystemHeadersMap, dirs);
            m_SystemHeadersThreads.push_back(thread);
        }

        cbEditor* editor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
        if (m_NativeParser.GetProjectByEditor(editor) == project)
            EnableToolbarTools(false);
    }
}

void CodeCompletion::OnParserEnd(wxCommandEvent& event)
{
    ParserCommon::ParserState state = static_cast<ParserCommon::ParserState>(event.GetInt());
    if (state == ParserCommon::ptCreateParser)
    {
        if (   m_CCEnableHeaders
            && !m_SystemHeadersThreads.empty()
            && !m_SystemHeadersThreads.front()->IsRunning()
            && m_NativeParser.Done() )
        {
            m_SystemHeadersThreads.front()->Run();
        }
    }

    cbEditor* editor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (editor)
    {
        m_ToolbarNeedReparse = true;
        TRACE(_T("OnParserEnd: Starting m_TimerToolbar."));
        m_TimerToolbar.Start(TOOLBAR_REFRESH_DELAY, wxTIMER_ONE_SHOT);
    }

    event.Skip();
}

void CodeCompletion::OnEditorTooltip(CodeBlocksEvent& event)
{
    if (!IsAttached() || !m_InitDone || wxGetKeyState(WXK_CONTROL))
    {
        event.Skip();
        return;
    }

    if (!Manager::Get()->GetConfigManager(_T("code_completion"))->ReadBool(_T("eval_tooltip"), true))
    {
        event.Skip();
        return;
    }

    EditorBase* base = event.GetEditor();
    cbEditor* ed = base && base->IsBuiltinEditor() ? static_cast<cbEditor*>(base) : 0;
    if (!ed || ed->IsContextMenuOpened())
    {
        event.Skip();
        return;
    }

    if ( !IsProviderFor(ed) )
    {
        event.Skip();
        return;
    }

    if (ed->GetControl()->CallTipActive() && event.GetExtraLong() == 0)
        ed->GetControl()->CallTipCancel();
//        CCLogger::Get()->DebugLog(F(_T("CodeCompletion::OnEditorTooltip: %p"), ed));
    /* NOTE: The following 2 lines of codes can fix [Bug #11785].
    *       The solution may not the best one and it requires the editor
    *       to have the focus (even if C::B has the focus) in order to pop-up the tooltip. */
    if (wxWindow::FindFocus() != static_cast<wxWindow*>(ed->GetControl()))
    {
        event.Skip();
        return;
    }

    int pos = ed->GetControl()->PositionFromPointClose(event.GetX(), event.GetY());
    if (pos < 0 || pos >= ed->GetControl()->GetLength())
    {
        event.Skip();
        return;
    }

    // ignore comments, strings, preprocesor, etc
    int style = event.GetInt();
    if (   (style != wxSCI_C_DEFAULT)
        && (style != wxSCI_C_OPERATOR)
        && (style != wxSCI_C_IDENTIFIER)
        && (style != wxSCI_C_WORD2)
        && (style != wxSCI_C_GLOBALCLASS) )
    {
        DoShowCallTip(pos);
        event.Skip();
        return;
    }

    TRACE(_T("OnEditorTooltip"));

    TokenIdxSet result;
    int endOfWord = ed->GetControl()->WordEndPosition(pos, true);
    if (m_NativeParser.MarkItemsByAI(result, true, false, true, endOfWord))
    {
        wxString      calltip;
        wxArrayString tips;

        TokenTree* tree = m_NativeParser.GetParser().GetTokenTree();

        CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

        int count = 0;
        size_t tipWidth = 0;
        for (TokenIdxSet::const_iterator it = result.begin(); it != result.end(); ++it)
        {
            const Token* token = tree->at(*it);
            if (token)
            {
                wxString tip = token->DisplayName();
                if (tips.Index(tip) != wxNOT_FOUND) // avoid showing tips twice
                    continue;

                tips.Add(tip);
                calltip << tip << _T("\n");
                if (tip.Length() > tipWidth)
                    tipWidth = tip.Length();
                ++count;
                if (count > 32) // allow max 32 matches (else something is definitely wrong)
                {
                    calltip.Clear();
                    break;
                }
            }
        }

        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

        if (calltip.IsEmpty())
            DoShowCallTip(pos);
        else
        {
            calltip.RemoveLast(); // last \n

            int lnStart = ed->GetControl()->PositionFromLine(ed->GetControl()->LineFromPosition(pos));
                         // pos - lnStart   == distance from start of line
                         //  + tipWidth + 1 == projected virtual position of tip end (with a 1 character buffer) from start of line
                         //  - (width_of_editor_in_pixels / width_of_character) == distance tip extends past window edge
                         //       horizontal scrolling is accounted for by PointFromPosition().x
            int offset = tipWidth + pos + 1 - lnStart -
                         (ed->GetControl()->GetSize().x - ed->GetControl()->PointFromPosition(lnStart).x) /
                          ed->GetControl()->TextWidth(wxSCI_STYLE_LINENUMBER, _T("W"));
            if (offset > 0)
                pos -= offset;
            if (pos < lnStart) // do not go to previous line if tip is wider than editor
                pos = lnStart;

            ed->GetControl()->CallTipShow(pos, calltip);
            event.SetExtraLong(1);
            TRACE(calltip);
        }
    }
    else
        DoShowCallTip(pos);

    event.Skip();
}

void CodeCompletion::OnSystemHeadersThreadUpdate(wxCommandEvent& event)
{
    if (!m_SystemHeadersThreads.empty())
    {
        SystemHeadersThread* thread = static_cast<SystemHeadersThread*>(event.GetClientData());
        if (thread == m_SystemHeadersThreads.front())
            CCLogger::Get()->DebugLog(event.GetString());
    }
}

void CodeCompletion::OnSystemHeadersThreadCompletion(wxCommandEvent& event)
{
    if (m_SystemHeadersThreads.empty())
        return;

    SystemHeadersThread* thread = static_cast<SystemHeadersThread*>(event.GetClientData());
    if (thread == m_SystemHeadersThreads.front())
    {
        if (!event.GetString().IsEmpty())
            CCLogger::Get()->DebugLog(event.GetString());
        if (thread->IsAlive() && thread->IsRunning())
            thread->Wait();
        m_SystemHeadersThreads.pop_front();
    }

    if (   m_CCEnableHeaders
        && !m_SystemHeadersThreads.empty()
        && !m_SystemHeadersThreads.front()->IsRunning()
        && m_NativeParser.Done() )
    {
        m_SystemHeadersThreads.front()->Run();
    }
}

void CodeCompletion::OnSystemHeadersThreadError(wxCommandEvent& event)
{
    if (!m_SystemHeadersThreads.empty())
    {
        SystemHeadersThread* thread = static_cast<SystemHeadersThread*>(event.GetClientData());
        if (thread == m_SystemHeadersThreads.front())
            CCLogger::Get()->DebugLog(event.GetString());
    }
}

void CodeCompletion::DoCodeComplete()
{
    EditorManager* edMan = Manager::Get()->GetEditorManager();
    cbEditor* ed = edMan->GetBuiltinActiveEditor();
    if (!ed)
        return;

    if ( !IsProviderFor(ed) )
        return;

    TRACE(_T("DoCodeComplete"));

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

        if (str == _T("include") && pos > end)
            CodeCompleteIncludes();
        else if (end >= pos && pos > lineIndentPos)
            CodeCompletePreprocessor();
        else if ( (   str == _T("define")
                   || str == _T("if")
                   || str == _T("ifdef")
                   || str == _T("ifndef")
                   || str == _T("elif")
                   || str == _T("elifdef")
                   || str == _T("elifndef")
                   || str == _T("undef") )
                 && pos > end )
        {
            m_CompletePPOnly = true;
            CodeComplete();
        }
        return;
    }
    else if (curChar == _T('#'))
        return;
    else if (lineFirstChar == _T(':') && curChar == _T(':'))
        return;

    if (   style != wxSCI_C_DEFAULT
        && style != wxSCI_C_OPERATOR
        && style != wxSCI_C_IDENTIFIER
        && style != wxSCI_C_WORD2
        && style != wxSCI_C_GLOBALCLASS )
        return;

    TRACE(_T("DoCodeComplete -> CodeComplete"));
    CodeComplete();
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

    if (!m_NativeParser.GetParser().Done())
    {
        wxString msg = _("The Parser is still parsing files.");
        msg += m_NativeParser.GetParser().NotDoneReason();
        CCLogger::Get()->DebugLog(msg);
        return -5;
    }

    int success = -6;

//    TokenTree* tree = m_NativeParser.GetParser().GetTokenTree(); // The one used inside InsertClassMethodDlg

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    // open the insert class dialog
    wxString filename = ed->GetFilename();
    InsertClassMethodDlg dlg(Manager::Get()->GetAppWindow(), &m_NativeParser.GetParser(), filename);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        cbStyledTextCtrl* control = ed->GetControl();
        int pos = control->GetCurrentPos();
        int line = control->LineFromPosition(pos);
        control->GotoPos(control->PositionFromLine(line));

        wxArrayString result = dlg.GetCode();
        for (unsigned int i = 0; i < result.GetCount(); ++i)
        {
            pos = control->GetCurrentPos();
            line = control->LineFromPosition(pos);
            wxString str = ed->GetLineIndentString(line - 1) + result[i];
            MatchCodeStyle(str, control->GetEOLMode(), ed->GetLineIndentString(line - 1), control->GetUseTabs(), control->GetTabWidth());
            control->SetTargetStart(pos);
            control->SetTargetEnd(pos);
            control->ReplaceTarget(str);
            control->GotoPos(pos + str.Length());// - 3);
        }
        success = 0;
    }

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    return success;
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

    wxArrayString paths = m_NativeParser.GetAllPathsByFilename(ed->GetFilename());
    TokenTree*    tree  = m_NativeParser.GetParser().GetTokenTree();

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    // get all filenames' indices matching our mask
    TokenFileSet result;
    for (size_t i = 0; i < paths.GetCount(); ++i)
    {
        CCLogger::Get()->DebugLog(_T("Trying to find matches for: ") + paths[i]);
        TokenFileSet result_file;
        tree->GetFileMatches(paths[i], result_file, true, true);
        for (TokenFileSet::const_iterator it = result_file.begin(); it != result_file.end(); ++it)
            result.insert(*it);
    }

    if (result.empty())
    {
        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

        cbMessageBox(_("Could not find any file match in parser's database."), _("Warning"), wxICON_WARNING);
        return -5;
    }

    // loop matching files, loop tokens in file and get list of un-implemented functions
    wxArrayString arr; // for selection (keeps strings)
    wxArrayInt arrint; // for selection (keeps indices)
    typedef std::map<int, std::pair<int, wxString> > ImplMap;
    ImplMap im;
    for (TokenFileSet::const_iterator itf = result.begin(); itf != result.end(); ++itf)
    {
        const TokenIdxSet* tokens = tree->GetTokensBelongToFile(*itf);
        if (!tokens) continue;

        // loop tokens in file
        for (TokenIdxSet::const_iterator its = tokens->begin(); its != tokens->end(); ++its)
        {
            const Token* token = tree->at(*its);
            if (   token // valid token
                && (token->m_TokenKind & (tkFunction | tkConstructor | tkDestructor)) // is method
                && token->m_ImplLine == 0 ) // is un-implemented
            {
                im[token->m_Line] = std::make_pair(*its, token->DisplayName());
            }
        }
    }

    for (ImplMap::const_iterator it = im.begin(); it != im.end(); ++it)
    {
        arrint.Add(it->second.first);
        arr.Add(it->second.second);
    }

    if (arr.empty())
    {
        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

        cbMessageBox(_("No classes declared or no un-implemented class methods found."), _("Warning"), wxICON_WARNING);
        return -5;
    }

    int success = -5;

    // select tokens
    MultiSelectDlg dlg(Manager::Get()->GetAppWindow(), arr, true);
    if (dlg.ShowModal() == wxID_OK)
    {
        cbStyledTextCtrl* control = ed->GetControl();
        int pos = control->GetCurrentPos();
        int line = control->LineFromPosition(pos);
        control->GotoPos(control->PositionFromLine(line));

        bool addDoxgenComment = Manager::Get()->GetConfigManager(_T("code_completion"))->ReadBool(_T("/add_doxgen_comment"), false);

        wxArrayInt indices = dlg.GetSelectedIndices();
        for (size_t i = 0; i < indices.GetCount(); ++i)
        {
            const Token* token = tree->at(arrint[indices[i]]);
            if (!token)
                continue;

            pos  = control->GetCurrentPos();
            line = control->LineFromPosition(pos);

            // actual code generation
            wxString str;
            if (i > 0)
                str << _T("\n");
            else
                str << ed->GetLineIndentString(line - 1);
            if (addDoxgenComment)
                str << _T("/** @brief ") << token->m_Name << _T("\n  *\n  * @todo: document this function\n  */\n");
            wxString type = token->m_BaseType;
            if ((type.Last() == _T('&') || type.Last() == _T('*')) && type[type.Len() - 2] == _T(' '))
            {
                type[type.Len() - 2] = type.Last();
                type.RemoveLast();
            }
            if (!type.IsEmpty())
                str << type << _T(" ");
            if (token->m_ParentIndex != -1)
            {
                const Token* parent = tree->at(token->m_ParentIndex);
                if (parent)
                    str << parent->m_Name << _T("::");
            }
            str << token->m_Name << token->GetStrippedArgs();
            if (token->m_IsConst)
                str << _T(" const");
            str << _T("\n{\n\t\n}\n");

            MatchCodeStyle(str, control->GetEOLMode(), ed->GetLineIndentString(line - 1), control->GetUseTabs(), control->GetTabWidth());

            // add code in editor
            control->SetTargetStart(pos);
            control->SetTargetEnd(pos);
            control->ReplaceTarget(str);
            control->GotoPos(pos + str.Length());
        }
        if (!indices.IsEmpty())
        {
            pos  = control->GetCurrentPos();
            line = control->LineFromPosition(pos);
            control->GotoPos(control->GetLineEndPosition(line - 2));
        }
        success = 0;
    }

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    return success;
}

void CodeCompletion::MatchCodeStyle(wxString& str, int eolStyle, const wxString& indent, bool useTabs, int tabSize)
{
    str.Replace(wxT("\n"), (eolStyle == wxSCI_EOL_LF   ? wxT("\n")   :
                            eolStyle == wxSCI_EOL_CRLF ? wxT("\r\n") :
                          /*eolStyle == wxSCI_EOL_CR ?*/ wxT("\r")   ) + indent );
    if (!useTabs)
        str.Replace(wxT("\t"), wxString(wxT(' '), tabSize));
    if (!indent.IsEmpty())
        str.RemoveLast(indent.Length());
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

    if (!m_FunctionsScope.size())
        return;

    // search previous/next function from current line, default: previous
    int          line            = -1;
    unsigned int best_func       = 0;
    bool         found_best_func = false;
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
                    && (func_start_line > best_func_line) ) // decide which is closer
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

    if (line != -1)
    {
        ed->GotoLine(line);
        ed->SetFocus();
    }
}

// help method in finding the namespace position in the vector for the namespace containing the current line
int CodeCompletion::NameSpacePosition() const
{
    int pos = -1;
    int startLine = -1;
    for (unsigned int idxNs = 0; idxNs < m_NameSpaces.size(); ++idxNs)
    {
        const NameSpace& ns = m_NameSpaces[idxNs];
        if (m_CurrentLine >= ns.StartLine && m_CurrentLine <= ns.EndLine && ns.StartLine > startLine)
        {
            // got one, maybe there might be a better fitting namespace
            // (embedded namespaces) so keep on looking
            pos = static_cast<int>(idxNs);
            startLine = ns.StartLine;
        }
    }

    return pos;
}

void CodeCompletion::OnScope(wxCommandEvent&)
{
    int sel = m_Scope->GetSelection();
    if (sel != -1 && sel < static_cast<int>(m_ScopeMarks.size()))
        UpdateFunctions(sel);
}

void CodeCompletion::OnFunction(cb_unused wxCommandEvent& event)
{
    int selSc = (m_Scope) ? m_Scope->GetSelection() : 0;
    if (selSc != -1 && selSc < static_cast<int>(m_ScopeMarks.size()))
    {
        int idxFn = m_ScopeMarks[selSc] + m_Function->GetSelection();
        if (idxFn != -1 && idxFn < static_cast<int>(m_FunctionsScope.size()))
        {
            cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
            if (ed)
                ed->GotoTokenPosition(m_FunctionsScope[idxFn].StartLine,
                                      m_FunctionsScope[idxFn].ShortName);
        }
    }
}

void CodeCompletion::ParseFunctionsAndFillToolbar()
{
    TRACE(_T("ParseFunctionsAndFillToolbar() : m_ToolbarNeedReparse=%d, m_ToolbarNeedRefresh=%d, "),
          m_ToolbarNeedReparse?1:0, m_ToolbarNeedRefresh?1:0);
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
    if (m_ToolbarNeedReparse || !funcdata->parsed)
    {
        if (m_ToolbarNeedReparse)
            m_ToolbarNeedReparse = false;

        funcdata->m_FunctionsScope.clear();
        funcdata->m_NameSpaces.clear();

        TokenIdxSet result;
        m_NativeParser.GetParser().FindTokensInFile(filename, result,
                                                    tkAnyFunction | tkEnum | tkClass | tkNamespace);
        if (!result.empty())
            funcdata->parsed = true;
        else
            fileParseFinished = false;

        TokenTree* tree = m_NativeParser.GetParser().GetTokenTree();

        CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

        for (TokenIdxSet::const_iterator it = result.begin(); it != result.end(); ++it)
        {
            const Token* token = tree->at(*it);
            if (token && token->m_ImplLine != 0)
            {
                FunctionScope fs;
                fs.StartLine = token->m_ImplLine    - 1;
                fs.EndLine   = token->m_ImplLineEnd - 1;
                const size_t fileIdx = tree->InsertFileOrGetIndex(filename);
                if (token->m_TokenKind & tkAnyFunction && fileIdx == token->m_ImplFileIdx)
                {
                    fs.Scope = token->GetNamespace();
                    if (fs.Scope.IsEmpty())
                        fs.Scope = g_GlobalScope;
                    wxString result_str = token->m_Name;
                    fs.ShortName = result_str;
                    result_str << token->GetFormattedArgs();
                    if (!token->m_BaseType.IsEmpty())
                        result_str << _T(" : ") << token->m_BaseType;
                    fs.Name = result_str;
                    funcdata->m_FunctionsScope.push_back(fs);
                }
                else if (token->m_TokenKind & (tkEnum | tkClass | tkNamespace))
                {
                    fs.Scope = token->GetNamespace() + token->m_Name + _T("::");
                    funcdata->m_FunctionsScope.push_back(fs);
                }
            }
        }

        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

        FunctionsScopeVec& functionsScopes = funcdata->m_FunctionsScope;
        NameSpaceVec& nameSpaces = funcdata->m_NameSpaces;

        m_NativeParser.GetParser().ParseBufferForNamespaces(ed->GetControl()->GetText(), nameSpaces);
        std::sort(nameSpaces.begin(), nameSpaces.end(), CodeCompletionHelper::LessNameSpace);

        std::copy(nameSpaces.begin(), nameSpaces.end(), back_inserter(functionsScopes));
        std::sort(functionsScopes.begin(), functionsScopes.end(), CodeCompletionHelper::LessFunctionScope);

        // remove consecutive duplicates
        FunctionsScopeVec::const_iterator it;
        it = unique(functionsScopes.begin(), functionsScopes.end(), CodeCompletionHelper::EqualFunctionScope);
        functionsScopes.resize(it - functionsScopes.begin());

        TRACE(F(_T("Found %lu namespace locations"), static_cast<unsigned long>(nameSpaces.size())));
        /*
        for (unsigned int i = 0; i < nameSpaces.size(); ++i)
            CCLogger::Get()->DebugLog(F(_T("\t%s (%d:%d)"),
                nameSpaces[i].Name.wx_str(), nameSpaces[i].StartLine, nameSpaces[i].EndLine));
        */

        if (!m_ToolbarNeedRefresh)
            m_ToolbarNeedRefresh = true;
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

    TRACE(F(_T("Parsed %lu functionscope items"), static_cast<unsigned long>(m_FunctionsScope.size())));
    /*
    for (unsigned int i = 0; i < m_FunctionsScope.size(); ++i)
        CCLogger::Get()->DebugLog(F(_T("\t%s%s (%d:%d)"),
            m_FunctionsScope[i].Scope.wx_str(), m_FunctionsScope[i].Name.wx_str(),
            m_FunctionsScope[i].StartLine, m_FunctionsScope[i].EndLine));
    */

    // Does the toolbar need a refresh?
    if (m_ToolbarNeedRefresh || m_LastFile != filename)
    {
        // Update the last editor and changed flag...
        if (m_ToolbarNeedRefresh)
            m_ToolbarNeedRefresh = false;
        if (m_LastFile != filename)
        {
            TRACE(_T("ParseFunctionsAndFillToolbar() : Update last file is %s"), filename.wx_str());
            m_LastFile = filename;
        }

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
            m_Scope->SetSelection(-1);
    }

    if (selFn != -1 && selFn != m_Function->GetSelection())
        m_Function->SetSelection(selFn);
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

void CodeCompletion::EnableToolbarTools(bool enable)
{
    if (m_Scope)
        m_Scope->Enable(enable);
    if (m_Function)
        m_Function->Enable(enable);
}

void CodeCompletion::DoParseOpenedProjectAndActiveEditor()
{
    // Let the app startup before parsing
    // This is to prevent the Splash Screen from delaying so much. By adding
    // the timer, the splash screen is closed and Code::Blocks doesn't take
    // so long in starting.
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
}

void CodeCompletion::OnCodeCompleteTimer(cb_unused wxTimerEvent& event)
{
    if (Manager::Get()->GetEditorManager()->FindPageFromEditor(m_LastEditor) == -1)
        return; // editor is invalid (probably closed already)

    TRACE(_T("OnCodeCompleteTimer"));

    // ask for code-completion *only* if the editor is still after the "." or "->" operator
    if (m_LastEditor && m_LastEditor->GetControl()->GetCurrentPos() == m_LastPosForCodeCompletion)
    {
        DoCodeComplete();
        m_LastPosForCodeCompletion = -1; // reset it
    }
}

void CodeCompletion::OnToolbarTimer(cb_unused wxTimerEvent& event)
{
    TRACE(_T("OnToolbarTimer"));

    if (!ProjectManager::IsBusy())
        ParseFunctionsAndFillToolbar();
    else
    {
        TRACE(_T("OnToolbarTimer: Starting m_TimerToolbar."));
        m_TimerToolbar.Start(TOOLBAR_REFRESH_DELAY, wxTIMER_ONE_SHOT);
    }
}

void CodeCompletion::OnRealtimeParsingTimer(cb_unused wxTimerEvent& event)
{
    cbEditor* editor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!editor)
        return;

    TRACE(_T("OnRealtimeParsingTimer"));

    const int curLen = editor->GetControl()->GetLength();
    if (curLen != m_CurrentLength)
    {
        m_CurrentLength = curLen;
        TRACE(_T("OnRealtimeParsingTimer: Starting m_TimerRealtimeParsing."));
        m_TimerRealtimeParsing.Start(REALTIME_PARSING_DELAY, wxTIMER_ONE_SHOT);
        return;
    }

    cbProject* project = m_NativeParser.GetProjectByEditor(editor);
    if (project && !project->GetFileByFilename(m_LastFile, false, true))
        return;
    if (m_NativeParser.ReparseFile(project, m_LastFile))
        CCLogger::Get()->DebugLog(_T("Reparsing when typing for editor ") + m_LastFile);
}

void CodeCompletion::OnProjectSavedTimer(cb_unused wxTimerEvent& event)
{
    cbProject* project = static_cast<cbProject*>(m_TimerProjectSaved.GetClientData());
    m_TimerProjectSaved.SetClientData(NULL);

    ProjectsArray* projs = Manager::Get()->GetProjectManager()->GetProjects();
    if (projs->Index(project) == wxNOT_FOUND)
        return;

    if (IsAttached() && m_InitDone && project)
    {
        TRACE(_T("OnProjectSavedTimer"));
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

void CodeCompletion::OnReparsingTimer(cb_unused wxTimerEvent& event)
{
    if (ProjectManager::IsBusy() || !IsAttached() || !m_InitDone)
    {
        m_ReparsingMap.clear();
        CCLogger::Get()->DebugLog(_T("Reparsing files failed!"));
        return;
    }

    TRACE(_T("OnReparsingTimer"));

    ReparsingMap::iterator it = m_ReparsingMap.begin();
    if (it != m_ReparsingMap.end() && m_NativeParser.Done())
    {
        cbProject* project = it->first;
        wxArrayString& files = it->second;
        if (!project)
            project = m_NativeParser.GetProjectByFilename(files[0]);

        if (project && Manager::Get()->GetProjectManager()->IsProjectStillOpen(project))
        {
            wxString curFile;
            EditorBase* editor = Manager::Get()->GetEditorManager()->GetActiveEditor();
            if (editor)
                curFile = editor->GetFilename();

            size_t reparseCount = 0;
            while (!files.IsEmpty())
            {
                if (m_NativeParser.ReparseFile(project, files.Last()))
                {
                    ++reparseCount;
                    TRACE(_T("OnReparsingTimer: Reparsing file : ") + files.Last());
                    if (files.Last() == curFile)
                    {
                        m_ToolbarNeedReparse = true;
                        TRACE(_T("OnReparsingTimer: Starting m_TimerToolbar."));
                        m_TimerToolbar.Start(TOOLBAR_REFRESH_DELAY, wxTIMER_ONE_SHOT);
                    }
                }

                files.RemoveAt(files.GetCount() - 1);
            }

            if (reparseCount)
                CCLogger::Get()->DebugLog(F(_T("Re-parsed %lu files."), static_cast<unsigned long>(reparseCount)));
        }

        if (files.IsEmpty())
            m_ReparsingMap.erase(it);
    }

    if (!m_ReparsingMap.empty())
    {
        TRACE(_T("OnReparsingTimer: Starting m_TimerReparsing."));
        m_TimerReparsing.Start(EDITOR_ACTIVATED_DELAY, wxTIMER_ONE_SHOT);
    }
}

void CodeCompletion::OnEditorActivatedTimer(cb_unused wxTimerEvent& event)
{
    EditorBase*     editor  = Manager::Get()->GetEditorManager()->GetActiveEditor();
    const wxString& curFile = editor ? editor->GetFilename() : wxString(wxEmptyString);
    if (!editor || editor != m_LastEditor || curFile.IsEmpty())
    {
        m_LastEditor = nullptr;
        return;
    }

    if (   !m_LastFile.IsEmpty()
        && (m_LastFile != g_StartHereTitle)
        && (m_LastFile == curFile) )
    {
        TRACE(_T("OnEditorActivatedTimer() : Last activated file is %s"), curFile.wx_str());
        return;
    }

    TRACE(_T("OnEditorActivatedTimer"));

    m_NativeParser.OnEditorActivated(editor);
    TRACE(_T("OnEditorActivatedTimer: Starting m_TimerToolbar."));
    m_TimerToolbar.Start(TOOLBAR_REFRESH_DELAY, wxTIMER_ONE_SHOT);
    TRACE(_T("OnEditorActivatedTimer() : Current activated file is %s"), curFile.wx_str());
}

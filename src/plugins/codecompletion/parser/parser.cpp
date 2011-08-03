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
    #include <queue>

    #include <wx/app.h>
    #include <wx/dir.h>
    #include <wx/filename.h>
    #include <wx/intl.h>
    #include <wx/progdlg.h>
    #include <wx/tokenzr.h>

    #include <cbproject.h>
    #include <configmanager.h>
    #include <editormanager.h>
    #include <globals.h>
    #include <infowindow.h>
    #include <logmanager.h>
    #include <manager.h>
#endif

#include <cbstyledtextctrl.h>

#include "parser.h"

#include "../classbrowser.h"
#include "../classbrowserbuilderthread.h"


#ifndef CB_PRECOMP
    #include "editorbase.h"
#endif

#define CC_PARSER_DEBUG_OUTPUT 0

#if (CC_GLOBAL_DEBUG_OUTPUT)
    #undef CC_PARSER_DEBUG_OUTPUT
    #define CC_PARSER_DEBUG_OUTPUT 1
#endif

#if CC_PARSER_DEBUG_OUTPUT == 1
    #define TRACE(format, args...) \
        Manager::Get()->GetLogManager()->DebugLog(F(format, ##args))
    #define TRACE2(format, args...)
#elif CC_PARSER_DEBUG_OUTPUT == 2
    #define TRACE(format, args...)                                              \
        do                                                                      \
        {                                                                       \
            if (g_EnableDebugTrace)                                             \
                Manager::Get()->GetLogManager()->DebugLog(F(format, ##args));   \
        }                                                                       \
        while (false)
    #define TRACE2(format, args...) \
        Manager::Get()->GetLogManager()->DebugLog(F(format, ##args))
#else
    #define TRACE(format, args...)
    #define TRACE2(format, args...)
#endif

static const char CACHE_MAGIC[]      = "CCCACHE_1_4";
static const int batch_timer_delay   = 300;
static const int reparse_timer_delay = 100;

int idParserStart     = wxNewId();
int idParserEnd       = wxNewId();
int idTimerParse      = wxNewId();
int idTimerBatchParse = wxNewId();

static volatile Parser* s_CurrentParser = nullptr;
static wxCriticalSection s_ParserCritical;

class AddParseThread : public cbThreadedTask
{
public:
    AddParseThread(Parser& parser) : m_Parser(parser)
    {}

    int Execute()
    {
        wxCriticalSectionLocker locker1(s_TokensTreeCritical);
        wxCriticalSectionLocker locker2(s_ParserCritical);

        m_Parser.m_IsParsing = false;

        // Pre-defined macros
        if (!m_Parser.m_PredefinedMacros.IsEmpty())
        {
            ParserThreadOptions opts;
            opts.wantPreprocessor     = m_Parser.m_Options.wantPreprocessor;
            opts.parseComplexMacros   = m_Parser.m_Options.parseComplexMacros;
            opts.followLocalIncludes  = m_Parser.m_Options.followLocalIncludes;
            opts.followGlobalIncludes = m_Parser.m_Options.followGlobalIncludes;
            opts.useBuffer            = true;
            opts.isTemp               = false;
            opts.bufferSkipBlocks     = false;
            opts.handleFunctions      = false;
            m_Parser.Parse(m_Parser.m_PredefinedMacros, false, opts);
            m_Parser.m_PredefinedMacros.Clear();
        }

        // Add priority headers
        if (!m_Parser.m_PriorityHeaders.empty())
        {
            m_Parser.m_IsPriority = true;
            StringList::iterator it = m_Parser.m_PriorityHeaders.begin();
            for (; it != m_Parser.m_PriorityHeaders.end(); ++it)
                m_Parser.Parse(*it);
            m_Parser.m_IsPriority = false;
            m_Parser.m_PriorityHeaders.clear();
        }

        // Add all other files
        if (!m_Parser.m_BatchParseFiles.empty())
        {
            m_Parser.m_IsFirstBatch = true;
            StringList::iterator it = m_Parser.m_BatchParseFiles.begin();
            for (; it != m_Parser.m_BatchParseFiles.end(); ++it)
                m_Parser.Parse(*it);
            m_Parser.m_BatchParseFiles.clear();
        }

        m_Parser.m_IsParsing = true;

        return 0;
    }

private:
    Parser& m_Parser;
};

class MarkFileAsLocalThread : public cbThreadedTask
{
public:
    MarkFileAsLocalThread(Parser& parser, cbProject& project) :
        m_Parser(parser),
        m_Project(project)
    {}

    int Execute()
    {
        wxCriticalSectionLocker locker(s_TokensTreeCritical);

        // mark all project files as local
        for (int i = 0; i < m_Project.GetFilesCount(); ++i)
        {
            ProjectFile* pf = m_Project.GetFile(i);
            if (!pf)
                continue;
            if (CCFileTypeOf(pf->relativeFilename) != ccftOther)
                m_Parser.GetTokensTree()->MarkFileTokensAsLocal(pf->file.GetFullPath(), true, &m_Project);
        }

        return 0;
    }

private:
    Parser&    m_Parser;
    cbProject& m_Project;
};

Parser::Parser(wxEvtHandler* parent, cbProject* project) :
    m_Parent(parent),
    m_Project(project),
    m_UsingCache(false),
    m_Pool(this, wxNewId(), 1, 2 * 1024 * 1024), // in the meanwhile it'll have to be forced to 1
    m_IsPriority(false),
    m_NeedsReparse(false),
    m_IsFirstBatch(false),
    m_IsParsing(false),
    m_Timer(this, idTimerParse),
    m_BatchTimer(this, idTimerBatchParse),
    m_StopWatchRunning(false),
    m_LastStopWatchTime(0),
    m_IgnoreThreadEvents(true),
    m_IsBatchParseDone(false),
    m_ParsingType(ptCreateParser),
    m_NeedMarkFileAsLocal(true)
{
    m_TokensTree = new TokensTree;
    m_TempTokensTree = new TokensTree;
    ReadOptions();
    ConnectEvents();
}

Parser::~Parser()
{
    // 1. Stop all timers
    m_Timer.Stop();
    m_BatchTimer.Stop();

    // 2. Let's OnAllThreadsDone can not process event
    m_IgnoreThreadEvents = true;

    // 3. Disconnect events
    DisconnectEvents();

    // 4. Abort all thread
    TerminateAllThreads();

    // 5. Free memory
    delete m_TempTokensTree;
    m_TempTokensTree = nullptr;
    delete m_TokensTree;
    m_TokensTree = nullptr;

    // 6. Reset current parser
    if (s_CurrentParser == this)
        s_CurrentParser = nullptr;
}

void Parser::ConnectEvents()
{
    Connect(-1, -1, cbEVT_THREADTASK_ALLDONE,
            (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
            &Parser::OnAllThreadsDone);
    Connect(idTimerParse, wxEVT_TIMER, wxTimerEventHandler(Parser::OnTimer));
    Connect(idTimerBatchParse, wxEVT_TIMER, wxTimerEventHandler(Parser::OnBatchTimer));
}

void Parser::DisconnectEvents()
{
    Disconnect(idTimerParse, wxTimerEventHandler(Parser::OnTimer));
    Disconnect(idTimerBatchParse, wxTimerEventHandler(Parser::OnBatchTimer));
    Disconnect(-1, -1,             cbEVT_THREADTASK_ALLDONE);
}

void Parser::ReadOptions()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("code_completion"));

    // one-time default settings change: upgrade everyone
    bool force_all_on = !cfg->ReadBool(_T("/parser_defaults_changed"), false);
    if (force_all_on)
    {
        cfg->Write(_T("/parser_defaults_changed"),       true);

        cfg->Write(_T("/parser_follow_local_includes"),  true);
        cfg->Write(_T("/parser_follow_global_includes"), true);
        cfg->Write(_T("/want_preprocessor"),             true);
        cfg->Write(_T("/parse_complex_macros"),          true);
    }

    //m_Pool.SetConcurrentThreads(cfg->ReadInt(_T("/max_threads"), 1)); // Ignore it in the meanwhile

    // Page "Code Completion"
    m_Options.useSmartSense        = cfg->ReadBool(_T("/use_SmartSense"),                true);
    m_Options.whileTyping          = cfg->ReadBool(_T("/while_typing"),                  true);
    m_Options.caseSensitive        = cfg->ReadBool(_T("/case_sensitive"),                false);

    // Page "C / C++ parser"
    m_Options.followLocalIncludes  = cfg->ReadBool(_T("/parser_follow_local_includes"),  true);
    m_Options.followGlobalIncludes = cfg->ReadBool(_T("/parser_follow_global_includes"), true);
    m_Options.wantPreprocessor     = cfg->ReadBool(_T("/want_preprocessor"),             true);
    m_Options.parseComplexMacros   = cfg->ReadBool(_T("/parse_complex_macros"),          true);

    // Page "Symbol browser"
    m_BrowserOptions.showInheritance = cfg->ReadBool(_T("/browser_show_inheritance"),    false);
    m_BrowserOptions.expandNS        = cfg->ReadBool(_T("/browser_expand_ns"),           false);
    m_BrowserOptions.treeMembers     = cfg->ReadBool(_T("/browser_tree_members"),        true);

    // Token tree
    m_BrowserOptions.displayFilter   = (BrowserDisplayFilter)cfg->ReadInt(_T("/browser_display_filter"), bdfFile);
    m_BrowserOptions.sortType        = (BrowserSortType)cfg->ReadInt(_T("/browser_sort_type"),           bstKind);
}

void Parser::WriteOptions()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("code_completion"));

    // Page "Code Completion"
    cfg->Write(_T("/use_SmartSense"),                m_Options.useSmartSense);
    cfg->Write(_T("/while_typing"),                  m_Options.whileTyping);
    cfg->Write(_T("/case_sensitive"),                m_Options.caseSensitive);

    // Page "C / C++ parser"
    cfg->Write(_T("/parser_follow_local_includes"),  m_Options.followLocalIncludes);
    cfg->Write(_T("/parser_follow_global_includes"), m_Options.followGlobalIncludes);
    cfg->Write(_T("/want_preprocessor"),             m_Options.wantPreprocessor);
    cfg->Write(_T("/parse_complex_macros"),          m_Options.parseComplexMacros);
    cfg->Write(_T("/max_threads"),              (int)GetMaxThreads());

    // Page "Symbol browser"
    cfg->Write(_T("/browser_show_inheritance"),      m_BrowserOptions.showInheritance);
    cfg->Write(_T("/browser_expand_ns"),             m_BrowserOptions.expandNS);
    cfg->Write(_T("/browser_tree_members"),          m_BrowserOptions.treeMembers);

    // Token tree
    cfg->Write(_T("/browser_display_filter"),        m_BrowserOptions.displayFilter);
    cfg->Write(_T("/browser_sort_type"),             m_BrowserOptions.sortType);
}

bool Parser::CacheNeedsUpdate()
{
    if (m_UsingCache)
    {
        wxCriticalSectionLocker locker(s_TokensTreeCritical);
        return m_TokensTree->m_Modified;
    }
    return true;
}

unsigned int Parser::GetFilesCount()
{
    wxCriticalSectionLocker locker(s_TokensTreeCritical);
    return m_TokensTree->m_FilesMap.size();
}

bool Parser::Done()
{
    wxCriticalSectionLocker locker(s_ParserCritical);

    bool done =    m_PriorityHeaders.empty()
                && m_SystemPriorityHeaders.empty()
                && m_BatchParseFiles.empty()
                && m_PredefinedMacros.IsEmpty()
                && !m_NeedMarkFileAsLocal
                && m_PoolTask.empty()
                && m_Pool.Done();

    return done;
}

wxString Parser::NotDoneReason()
{
    wxCriticalSectionLocker locker(s_ParserCritical);

    wxString reason = _T(" > Reasons:");
    if (!m_PriorityHeaders.empty())
      reason += _T("\n- still priority headers to parse");
    if (!m_SystemPriorityHeaders.empty())
      reason += _T("\n- still system priority headers to parse");
    if (!m_BatchParseFiles.empty())
      reason += _T("\n- still batch parse files to parse");
    if (!m_PredefinedMacros.IsEmpty())
      reason += _T("\n- still pre-defined macros to operate");
    if (m_NeedMarkFileAsLocal)
      reason += _T("\n- still need to mark files as local");
    if (!m_PoolTask.empty())
      reason += _T("\n- still parser threads (tasks) in the pool");
    if (!m_Pool.Done())
      reason += _T("\n- thread pool is not done yet");

    return reason;
}

Token* Parser::FindTokenByName(const wxString& name, bool globalsOnly, short int kindMask)
{
    wxCriticalSectionLocker locker(s_TokensTreeCritical);
    int result = m_TokensTree->TokenExists(name, -1, kindMask);
    return m_TokensTree->at(result);
}

Token* Parser::FindChildTokenByName(Token* parent, const wxString& name, bool useInheritance, short int kindMask)
{
    if (!parent)
        return FindTokenByName(name, false, kindMask);

    s_TokensTreeCritical.Enter();
    Token* result = m_TokensTree->at(m_TokensTree->TokenExists(name, parent->GetSelf(), kindMask));
    if (!result && useInheritance)
    {
        TokenIdxSet::iterator it;
        m_TokensTree->RecalcInheritanceChain(parent);
        for (it = parent->m_DirectAncestors.begin(); it != parent->m_DirectAncestors.end(); ++it)
        {
            Token* ancestor = m_TokensTree->at(*it);
            s_TokensTreeCritical.Leave();
            result = FindChildTokenByName(ancestor, name, true, kindMask);
            s_TokensTreeCritical.Enter();
            if (result)
                break;
        }
    }
    s_TokensTreeCritical.Leave();
    return result;
}

// No critical section needed here:
// All functions that call this, already entered a critical section.
size_t Parser::FindMatches(const wxString& s, TokenIdxSet& result, bool caseSensitive, bool is_prefix)
{
    result.clear();
    TokenIdxSet tmpresult;
    if (!m_TokensTree->FindMatches(s, tmpresult, caseSensitive, is_prefix))
        return 0;

    TokenIdxSet::iterator it;
    for (it = tmpresult.begin(); it != tmpresult.end(); ++it)
    {
        Token* token = m_TokensTree->at(*it);
        if (token)
        //result.push_back(token);
        result.insert(*it);
    }
    return result.size();
}

// No critical section needed here:
// All functions that call this, already entered a critical section.
bool Parser::ParseBuffer(const wxString& buffer, bool isLocal, bool bufferSkipBlocks, bool isTemp,
                         const wxString& filename, Token* parent, int initLine)
{
    ParserThreadOptions opts;

    opts.wantPreprocessor     = m_Options.wantPreprocessor;
    opts.followLocalIncludes  = false;
    opts.followGlobalIncludes = false;
    opts.parseComplexMacros   = false;
    opts.useBuffer            = true;
    opts.isTemp               = isTemp;
    opts.bufferSkipBlocks     = bufferSkipBlocks;
    opts.handleFunctions      = false;
    opts.fileOfBuffer         = filename;
    opts.parentOfBuffer       = parent;
    opts.initLineOfBuffer     = initLine;

    return Parse(buffer, isLocal, opts);
}

void Parser::AddPriorityHeaders(const wxString& filename, bool systemHeaderFile, bool delay)
{
    wxCriticalSectionLocker locker(s_ParserCritical);

    if (!m_IsParsing && m_BatchTimer.IsRunning())
        m_BatchTimer.Stop();

    // Do priority parse in sub thread
    m_PriorityHeaders.push_back(filename);

    // Save system priority headers, when all task is over, we need reparse it!
    if (systemHeaderFile)
        m_SystemPriorityHeaders.push_back(filename);

    if (!m_IsParsing)
        m_BatchTimer.Start(delay ? batch_timer_delay : 1, wxTIMER_ONE_SHOT);
}

void Parser::AddBatchParse(const StringList& filenames, bool delay)
{
    wxCriticalSectionLocker locker(s_ParserCritical);

    if (!m_IsParsing && m_BatchTimer.IsRunning())
        m_BatchTimer.Stop();

    if (m_BatchParseFiles.empty())
        m_BatchParseFiles = filenames;
    else
        std::copy(filenames.begin(), filenames.end(), std::back_inserter(m_BatchParseFiles));

    if (!m_IsParsing)
        m_BatchTimer.Start(delay ? batch_timer_delay : 1, wxTIMER_ONE_SHOT);
}

void Parser::AddParse(const wxString& filename, bool delay)
{
    wxCriticalSectionLocker locker(s_ParserCritical);

    if (!m_IsParsing && m_BatchTimer.IsRunning())
        m_BatchTimer.Stop();

    m_BatchParseFiles.push_back(filename);

    if (!m_IsParsing)
        m_BatchTimer.Start(delay ? batch_timer_delay : 10, wxTIMER_ONE_SHOT);
}

bool Parser::Parse(const wxString& filename, bool isLocal, LoaderBase* loader)
{
    ParserThreadOptions opts;
    opts.wantPreprocessor      = m_Options.wantPreprocessor;
    opts.useBuffer             = false;
    opts.bufferSkipBlocks      = false;
    opts.bufferSkipOuterBlocks = false;
    opts.followLocalIncludes   = m_Options.followLocalIncludes;
    opts.followGlobalIncludes  = m_Options.followGlobalIncludes;
    opts.parseComplexMacros    = m_Options.parseComplexMacros;
    opts.loader                = loader; // maybe 0 at this point

    return Parse(UnixFilename(filename), isLocal, opts);
}

// No critical section needed here:
// All functions that call this, already entered a critical section.
bool Parser::Parse(const wxString& bufferOrFilename, bool isLocal, ParserThreadOptions& opts)
{
    bool result = false;
    do
    {
        if (!opts.useBuffer)
        {
            bool canparse = !m_TokensTree->IsFileParsed(bufferOrFilename);
            if (canparse)
                canparse = m_TokensTree->ReserveFileForParsing(bufferOrFilename, true) != 0;

            if (!canparse)
            {
               if (opts.loader) // if a loader is already open at this point, the caller must clean it up
                   Manager::Get()->GetLogManager()->DebugLog(_T("Parse() : CodeCompletion Plugin: FileLoader memory leak likely while loading file ")+bufferOrFilename);
               break;
            }

            if (!opts.loader) // this should always be true (memory will leak if a loader has already been initialized before this point)
                opts.loader = Manager::Get()->GetFileManager()->Load(bufferOrFilename, m_NeedsReparse);
        }

        TRACE(_T("Parse() : Creating task for: %s"), bufferOrFilename.wx_str());
        ParserThread* thread = new ParserThread(this, bufferOrFilename, isLocal, opts, m_TokensTree);
        bool doParseNow = opts.useBuffer;
#if CC_PARSER_PROFILE_TEST
        doParseNow = true;
#endif
        //if we are parsing a memory buffer or Parser is under Profile
        // CC_PARSER_PROFILE_TEST is defined as 1), then just call Parse()
        // directly and thread pool is NOT used.
        // Otherwise, we are parsing a local file, so the thread pool is used.
        // The ParserThread generated was pushed to the memory pool.

        if (doParseNow)
        {
            result = thread->Parse();
            m_TempTokensTree->RecalcData();
            delete thread;
            break;
        }

        TRACE(_T("Parse() : Parsing %s"), bufferOrFilename.wx_str());

        if (m_IsPriority)
        {
            if (isLocal) // Parsing priority files
            {
                TRACE(_T("Parse() : Parsing priority header, %s"), bufferOrFilename.wx_str());
                result = thread->Parse();
                delete thread;
                break;
            }
            else // Add task when parsing priority files
            {
                TRACE(_T("Parse() : Add task for priority header, %s"), bufferOrFilename.wx_str());
                m_PoolTask.push(PTVector());
                m_PoolTask.back().push_back(thread);
            }
        }
        else
        {
            TRACE(_T("Parse() : Parallel Parsing %s"), bufferOrFilename.wx_str());

            // Add a task for all project files
            if (m_IsFirstBatch)
            {
                m_IsFirstBatch = false;
                m_PoolTask.push(PTVector());
            }

            if (m_IsParsing)
                m_Pool.AddTask(thread, true);
            else
                m_PoolTask.back().push_back(thread);
        }

        result = true;
    }
    while (false);

    return result;
}

// No critical section needed here:
// All functions that call this, already entered a critical section.
bool Parser::ParseBufferForFunctions(const wxString& buffer)
{
    ParserThreadOptions opts;
    opts.wantPreprocessor   = m_Options.wantPreprocessor;
    opts.parseComplexMacros = m_Options.parseComplexMacros;
    opts.useBuffer          = true;
    opts.bufferSkipBlocks   = true;
    opts.handleFunctions    = true;

    ParserThread thread(this,
                        buffer,
                        false,
                        opts,
                        m_TempTokensTree);

    return thread.Parse();
}

// No critical section needed here:
// All functions that call this, already entered a critical section.
bool Parser::ParseBufferForNamespaces(const wxString& buffer, NameSpaceVec& result)
{
    ParserThreadOptions opts;
    opts.useBuffer          = true;
    opts.wantPreprocessor   = m_Options.wantPreprocessor;
    opts.parseComplexMacros = false;

    ParserThread thread(this,
                        wxEmptyString,
                        true,
                        opts,
                        m_TempTokensTree);

    return thread.ParseBufferForNamespaces(buffer, result);
}

// No critical section needed here:
// All functions that call this, already entered a critical section.
bool Parser::ParseBufferForUsingNamespace(const wxString& buffer, wxArrayString& result)
{
    ParserThreadOptions opts;
    opts.useBuffer          = true;
    opts.wantPreprocessor   = m_Options.wantPreprocessor;
    opts.parseComplexMacros = false;

    ParserThread thread(this,
                        wxEmptyString,
                        false,
                        opts,
                        m_TempTokensTree);

    return thread.ParseBufferForUsingNamespace(buffer, result);
}

bool Parser::RemoveFile(const wxString& filename)
{
    if (!Done())
        return false; // Can't alter the tokens tree if parsing has not finished

    wxCriticalSectionLocker locker(s_TokensTreeCritical);
    size_t index = m_TokensTree->GetFileIndex(UnixFilename(filename));
    const bool result = m_TokensTree->m_FilesStatus.count(index);

    m_TokensTree->RemoveFile(filename);
    m_TokensTree->m_FilesMap.erase(index);
    m_TokensTree->m_FilesStatus.erase(index);
    m_TokensTree->m_FilesToBeReparsed.erase(index);
    m_TokensTree->m_Modified = true;

    return result;
}

bool Parser::AddFile(const wxString& filename, cbProject* project, bool isLocal)
{
    if (project != m_Project)
        return false;

    wxString file = UnixFilename(filename);
    if (IsFileParsed(file))
        return false;

    if (m_ParsingType == ptUndefined)
        m_ParsingType = ptAddFileToParser;

    AddParse(file);
    if (project)
        m_NeedMarkFileAsLocal = true;

    return true;
}

bool Parser::Reparse(const wxString& filename, bool isLocal)
{
    if (!Done())
        return false; // if still parsing, exit with error

    if (m_Timer.IsRunning())
        m_Timer.Stop();

    wxString file = UnixFilename(filename);
    if (isLocal)
        m_LocalFiles.insert(filename);
    else
        m_LocalFiles.erase(filename);

    {
        wxCriticalSectionLocker locker(s_TokensTreeCritical);
        m_TokensTree->FlagFileForReparsing(file);
    }

    m_NeedsReparse = true;
    m_Timer.Start(reparse_timer_delay, wxTIMER_ONE_SHOT);

    return true;
}

bool Parser::ReadFromCache(wxInputStream* f)
{
    bool result = false;
    wxCriticalSectionLocker locker(s_TokensTreeCritical);

    char CACHE_MAGIC_READ[] = "           ";
    m_TokensTree->clear(); // Clear data

    // File format is like this:
    //
    // CACHE_MAGIC
    // Number of parsed files
    // Number of tokens
    // Parsed files
    // Tokens
    // EOF

//  Begin loading process
    do
    {

        // keep a backup of include dirs
        if (f->Read(CACHE_MAGIC_READ, sizeof(CACHE_MAGIC_READ)).LastRead() != sizeof(CACHE_MAGIC_READ) ||
            strncmp(CACHE_MAGIC, CACHE_MAGIC_READ, sizeof(CACHE_MAGIC_READ) != 0))
            break;
        int fcount = 0, actual_fcount = 0;
        int tcount = 0, actual_tcount = 0;
        int idx;
        if (!LoadIntFromFile(f, &fcount))
            break;
        if (!LoadIntFromFile(f, &tcount))
            break;
        if (fcount < 0)
            break;
        if (tcount < 0)
            break;

        wxString file;
        int nonempty_token = 0;
        Token* token = 0;
        do // do while-false block
        {
            // Filenames
            int i;
            for (i = 0; i < fcount && !f->Eof(); ++i)
            {
                if (!LoadIntFromFile(f,&idx)) // Filename index
                    break;
                if (idx != i)
                    break;
                if (!LoadStringFromFile(f,file)) // Filename data
                    break;
                if (!idx)
                    file.Clear();
                if (file.IsEmpty())
                    idx = 0;
                m_TokensTree->m_FilenamesMap.insert(file);
                actual_fcount++;
            }
            result = (actual_fcount == fcount);
            if (!result)
                break;
            if (tcount)
                m_TokensTree->m_Tokens.resize(tcount,0);
            // Tokens
            for (i = 0; i < tcount && !f->Eof(); ++i)
            {
                token = 0;
                if (!LoadIntFromFile(f, &nonempty_token))
                break;
                if (nonempty_token != 0)
                {
                    token = new Token(wxEmptyString, 0, 0, ++m_TokensTree->m_TokenTicketCount);
                    if (!token->SerializeIn(f))
                    {
                        delete token;
                        --m_TokensTree->m_TokenTicketCount;
                        token = 0;
                        break;
                    }
                    m_TokensTree->insert(i,token);
                }
                ++actual_tcount;
            }
            if (actual_tcount != tcount)
                break;
            m_TokensTree->RecalcFreeList();
            result = true;
        } while(false);

    } while(false);

//  End loading process

    if (result)
        m_UsingCache = true;
    else
        m_TokensTree->clear();

    m_TokensTree->m_Modified = false;

    return result;
}

bool Parser::WriteToCache(wxOutputStream* f)
{
    bool result = false;
    wxCriticalSectionLocker locker(s_TokensTreeCritical);
//  Begin saving process

    size_t tcount = m_TokensTree->m_Tokens.size();
    size_t fcount = m_TokensTree->m_FilenamesMap.size();
    size_t i = 0;

    // write cache magic
    f->Write(CACHE_MAGIC, sizeof(CACHE_MAGIC));

    SaveIntToFile(f, fcount); // num parsed files
    SaveIntToFile(f, tcount); // num tokens

    // Filenames
    for (i = 0; i < fcount; ++i)
    {
        SaveIntToFile(f,i);
        SaveStringToFile(f,m_TokensTree->m_FilenamesMap.GetString(i));
    }

    // Tokens

    for (i = 0; i < tcount; ++i)
    {
        TRACE(_T("WriteToCache() : Token #%d, offset %d"),i,f->TellO());
        Token* token = m_TokensTree->at(i);
        SaveIntToFile(f,(token!=0) ? 1 : 0);
        if (token)
            token->SerializeOut(f);
    }

    result = true;

    if (result)
        m_TokensTree->m_Modified = false;

//  End saving process
    return result;
}

void Parser::TerminateAllThreads()
{
    m_Pool.AbortAllTasks();
    wxMilliSleep(10);

    while (!m_Pool.Done())
        wxMilliSleep(10);

    wxMilliSleep(10);
    while (!m_PoolTask.empty())
    {
        PTVector& v = m_PoolTask.front();
        for (PTVector::const_iterator it = v.begin(); it != v.end(); ++it)
            delete *it;
        m_PoolTask.pop();
    }
}

void Parser::AddIncludeDir(const wxString& dir)
{
    if (dir.IsEmpty())
        return;

    wxString base = dir;
    if (base.Last() == wxFILE_SEP_PATH)
        base.RemoveLast();
    if (!wxDir::Exists(base))
    {
        TRACE(_T("AddIncludeDir() : Directory %s does not exist?!"), base.wx_str());
        return;
    }

    if (m_IncludeDirs.Index(base) == wxNOT_FOUND)
    {
        TRACE(_T("AddIncludeDir() : Adding %s"), base.wx_str());
        m_IncludeDirs.Add(base);
    }
}

void Parser::AddPredefinedMacros(const wxString& defs)
{
    m_PredefinedMacros << defs;
}

wxString Parser::FindFirstFileInIncludeDirs(const wxString& file)
{
    wxString FirstFound = m_GlobalIncludes.GetItem(file);
    if (FirstFound.IsEmpty())
    {
        wxArrayString FoundSet = FindFileInIncludeDirs(file,true);
        if (FoundSet.GetCount())
        {
            FirstFound = UnixFilename(FoundSet[0]);
            m_GlobalIncludes.AddItem(file, FirstFound);
        }
    }
    return FirstFound;
}

wxArrayString Parser::FindFileInIncludeDirs(const wxString& file, bool firstonly)
{
    wxArrayString FoundSet;
    for (size_t idxSearch = 0; idxSearch < m_IncludeDirs.GetCount(); ++idxSearch)
    {
        wxString base = m_IncludeDirs[idxSearch];
        wxFileName tmp = file;
        NormalizePath(tmp,base);
        wxString fullname = tmp.GetFullPath();
        if (wxFileExists(fullname))
        {
            FoundSet.Add(fullname);
            if (firstonly)
                break;
        }
    }

    TRACE(_T("FindFileInIncludeDirs() : Searching %s"), file.wx_str());
    TRACE(_T("FindFileInIncludeDirs() : Found %d"), FoundSet.GetCount());

    return FoundSet;
}

void Parser::OnAllThreadsDone(CodeBlocksEvent& event)
{
    if (m_IgnoreThreadEvents)
        return;

    if (Manager::IsAppShuttingDown())
        return;

    if (!m_IsParsing)
    {
        Manager::Get()->GetLogManager()->DebugLog(_T("m_IsParsing is false?"));
        return;
    }

    if (!m_TokensTree)
        cbThrow(_T("m_TokensTree is a nullptr?!"));

    // Do next task
    if (   !m_PoolTask.empty()
        || !m_BatchParseFiles.empty()
        || !m_PriorityHeaders.empty()
        || !m_PredefinedMacros.IsEmpty() )
    {
        m_BatchTimer.Start(1, wxTIMER_ONE_SHOT);
    }

#if !CC_PARSER_PROFILE_TEST
    // Reparse system priority headers
    else if (!m_SystemPriorityHeaders.empty())
    {
        // Part.1 Set m_IsParsing to false
        m_IsParsing = false;

        // Part.2 Remove all priority headers in token tree
        for (StringList::iterator it = m_SystemPriorityHeaders.begin(); it != m_SystemPriorityHeaders.end(); ++it)
            RemoveFile(*it);

        // Part.3 Reparse system priority headers
        AddBatchParse(m_SystemPriorityHeaders, false);

        // Part.4 Clear
        m_SystemPriorityHeaders.clear();
    }
    else if (   (m_ParsingType == ptCreateParser || m_ParsingType == ptAddFileToParser)
             && m_NeedMarkFileAsLocal
             && m_Project)
    {
        m_NeedMarkFileAsLocal = false;
        MarkFileAsLocalThread* thread = new MarkFileAsLocalThread(*this, *m_Project);
        m_Pool.AddTask(thread, true);
    }
#endif

    // Finish all task, then we need post a PARSER_END event
    else
    {
        if (!m_Project)
            m_NeedMarkFileAsLocal = false;

        m_NeedsReparse     = false;
        m_IsParsing        = false;
        m_IsBatchParseDone = true;

        EndStopWatch();

        wxString parseEndLog;
        {
            wxCriticalSectionLocker locker(s_TokensTreeCritical);
            parseEndLog.Printf(_T("Project '%s' parsing stage done (%d total parsed files, ")
                               _T("%d tokens in %ld minute(s), %ld.%03ld seconds)."),
                               m_Project    ? m_Project->GetTitle().wx_str()  : _T("*NONE*"),
                               m_TokensTree ? m_TokensTree->m_FilesMap.size() : 0,
                               m_TokensTree ? m_TokensTree->realsize()        : 0,
                               (m_LastStopWatchTime / 60000),
                               (m_LastStopWatchTime / 1000) % 60,
                               (m_LastStopWatchTime % 1000) );
        }

        PostParserEvent(m_ParsingType, idParserEnd, parseEndLog);
        m_ParsingType = ptUndefined;
        s_CurrentParser = nullptr;
    }
}

wxString Parser::GetFullFileName(const wxString& src, const wxString& tgt, bool isGlobal)
{
    wxString fullname;
    if (isGlobal)
    {
        fullname = FindFirstFileInIncludeDirs(tgt);
        if (fullname.IsEmpty())
        {
            // not found; check this case:
            //
            // we had entered the previous file like this: #include <gl/gl.h>
            // and it now does this: #include "glext.h"
            // glext.h was correctly not found above but we can now search
            // for gl/glext.h.
            // if we still not find it, it's not there. A compilation error
            // is imminent (well, almost - I guess the compiler knows a little better ;).
            wxString base = wxFileName(src).GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
            fullname = FindFirstFileInIncludeDirs(base + tgt);
        }
    }

    // NOTE: isGlobal is always true. The following code never executes...

    else // local files are more tricky, since they depend on two filenames
    {
        wxFileName fname(tgt);
        wxFileName source(src);
        if (NormalizePath(fname,source.GetPath(wxPATH_GET_VOLUME)))
        {
            fullname = fname.GetFullPath();
            if (!wxFileExists(fullname))
                fullname.Clear();
        }
    }

    return fullname;
}

void Parser::DoParseFile(const wxString& filename, bool isGlobal)
{
    if (m_IgnoreThreadEvents)
        return;

    if (   (!isGlobal && !m_Options.followLocalIncludes)
        || ( isGlobal && !m_Options.followGlobalIncludes) )
        return;

    if (filename.IsEmpty())
        return;

    bool locked = false;
    if (m_IsParsing)
    {
        s_ParserCritical.Enter();
        locked = true;
    }
    Parse(filename, !isGlobal);
    if (locked)
        s_ParserCritical.Leave();
}

void Parser::StartStopWatch()
{
    if (!m_StopWatchRunning)
    {
        m_StopWatchRunning = true;
        m_StopWatch.Start();
    }
}

void Parser::EndStopWatch()
{
    if (m_StopWatchRunning)
    {
        m_StopWatch.Pause();
        m_StopWatchRunning = false;
        if (m_IsBatchParseDone)
            m_LastStopWatchTime = m_StopWatch.Time();
        else
            m_LastStopWatchTime += m_StopWatch.Time();
    }
}

void Parser::OnTimer(wxTimerEvent& event)
{
    ReparseModifiedFiles();
    event.Skip();
}

void Parser::OnBatchTimer(wxTimerEvent& event)
{
    if (Manager::IsAppShuttingDown())
        return;

    if (!m_StopWatchRunning)
        StartStopWatch();

    if (!m_PoolTask.empty())
    {
        m_Pool.BatchBegin();

        PTVector& v = m_PoolTask.front();
        for (PTVector::const_iterator it = v.begin(); it != v.end(); ++it)
            m_Pool.AddTask(*it, true);
        m_PoolTask.pop();

        m_Pool.BatchEnd();
        return;
    }
    else if (   !m_PriorityHeaders.empty()
             || !m_BatchParseFiles.empty()
             || !m_PredefinedMacros.IsEmpty() )
    {
        do
        {
            // Current batch parser is already exists
            if (s_CurrentParser && s_CurrentParser != this)
                break;

            // Have not done any batch parsing
            if (!s_CurrentParser)
            {
                s_CurrentParser = this;
                m_StopWatch.Start(); // reset timer
                PostParserEvent(m_ParsingType, idParserStart);
            }

            m_IgnoreThreadEvents = false;
            AddParseThread* thread = new AddParseThread(*this);
            m_Pool.AddTask(thread, true);
            return;
        }
        while (false);

        m_BatchTimer.Start(1000, wxTIMER_ONE_SHOT);
        return;
    }

    PostParserEvent(ptUndefined, idParserStart, _T("No files for batch parsing"));
    CodeBlocksEvent evt;
    evt.SetEventObject(this);
    OnAllThreadsDone(evt);
}

bool Parser::ReparseModifiedFiles()
{
    if (!m_NeedsReparse || !Done())
    {
        m_NeedsReparse = false;
        return false;
    }

    std::queue<wxString> files_list;
    {
        wxCriticalSectionLocker locker(s_TokensTreeCritical);
        TokenFilesSet::iterator it;

        // loop two times so that we reparse modified *header* files first
        // because they usually hold definitions which need to exist
        // when we parse the normal source files...
        for (it = m_TokensTree->m_FilesToBeReparsed.begin(); it != m_TokensTree->m_FilesToBeReparsed.end(); ++it)
        {
            wxString filename = m_TokensTree->m_FilenamesMap.GetString(*it);
            if (FileTypeOf(filename) == ftSource) // ignore source files (*.cpp etc)
                continue;
            files_list.push(filename);
            m_TokensTree->RemoveFile(*it);
        }
        for (it = m_TokensTree->m_FilesToBeReparsed.begin(); it != m_TokensTree->m_FilesToBeReparsed.end(); ++it)
        {
            wxString filename = m_TokensTree->m_FilenamesMap.GetString(*it);
            if (FileTypeOf(filename) != ftSource) // ignore non-source files (*.h etc)
                continue;
            files_list.push(filename);
            m_TokensTree->RemoveFile(*it);
        }
    }

    while (!files_list.empty())
    {
        wxString& filename = files_list.front();
        AddParse(filename, false);
        files_list.pop();
    }

    if (m_ParsingType == ptUndefined)
        m_ParsingType = ptReparseFile;

    return true;
}

// No critical section needed here:
// All functions that call this, already entered a critical section.
size_t Parser::FindTokensInFile(const wxString& fileName, TokenIdxSet& result, short int kindMask)
{
    result.clear();
    wxString file = UnixFilename(fileName);
    TokenIdxSet tmpresult;

    TRACE(_T("Parser::FindTokensInFile() : Searching for file '%s' in tokens tree..."), file.wx_str());
    if ( !m_TokensTree->FindTokensInFile(file, tmpresult, kindMask) )
        return 0;

    for (TokenIdxSet::iterator it = tmpresult.begin(); it != tmpresult.end(); ++it)
    {
        Token* token = m_TokensTree->at(*it);
        if (token)
            result.insert(*it);
    }
    return result.size();
}

bool Parser::IsFileParsed(const wxString& filename)
{
    bool isParsed = false;
    {
        wxCriticalSectionLocker locker(s_TokensTreeCritical);
        isParsed = m_TokensTree->IsFileParsed(UnixFilename(filename));
    }

    if (!isParsed)
    {
        wxCriticalSectionLocker locker(s_ParserCritical);
        StringList::iterator it = std::find(m_BatchParseFiles.begin(), m_BatchParseFiles.end(), filename);
        isParsed = it != m_BatchParseFiles.end();
    }

    return isParsed;
}

void Parser::PostParserEvent(ParsingType type, int id, const wxString& info)
{
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, id);
    evt.SetEventObject(this);       // Parser*
    evt.SetClientData(m_Project);   // cbProject*
    evt.SetInt(type);
    evt.SetString(info);
    wxPostEvent(m_Parent, evt);
}

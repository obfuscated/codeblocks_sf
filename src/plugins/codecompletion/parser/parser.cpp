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

#if CC_GLOBAL_DEBUG_OUTPUT == 1
    #undef CC_PARSER_DEBUG_OUTPUT
    #define CC_PARSER_DEBUG_OUTPUT 1
#elif CC_GLOBAL_DEBUG_OUTPUT == 2
    #undef CC_PARSER_DEBUG_OUTPUT
    #define CC_PARSER_DEBUG_OUTPUT 2
#endif

#if CC_PARSER_DEBUG_OUTPUT == 1
    #define TRACE(format, args...) \
        CCLogger::Get()->DebugLog(F(format, ##args))
    #define TRACE2(format, args...)
#elif CC_PARSER_DEBUG_OUTPUT == 2
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

static const char CACHE_MAGIC[]      = "CCCACHE_1_4";
static const int batch_timer_delay   = 300;
static const int reparse_timer_delay = 100;

int idParserStart = wxNewId();
int idParserEnd   = wxNewId();

static volatile Parser* s_CurrentParser = nullptr;
static wxCriticalSection s_ParserCritical;

class AddParseThread : public cbThreadedTask
{
public:
    AddParseThread(Parser& parser) : m_Parser(parser)
    {}

    int Execute()
    {
        TRACK_THREAD_LOCKER(s_ParserCritical);
        s_ParserCritical.Enter();
        THREAD_LOCKER_SUCCESS(s_ParserCritical);
        wxString preDefs(m_Parser.m_PredefinedMacros);
        StringList priorityHeaders(m_Parser.m_PriorityHeaders);
        StringList batchFiles(m_Parser.m_BatchParseFiles);
        s_ParserCritical.Leave();

        if (!preDefs.IsEmpty())
            m_Parser.ParseBuffer(preDefs, false, false);

        {
            TRACK_THREAD_LOCKER(s_ParserCritical);
            wxCriticalSectionLocker locker(s_ParserCritical);
            THREAD_LOCKER_SUCCESS(s_ParserCritical);

            m_Parser.m_PredefinedMacros.Clear();
            m_Parser.m_IsPriority = true;
        }

        while (!priorityHeaders.empty())
        {
            m_Parser.Parse(priorityHeaders.front());
            priorityHeaders.pop_front();
        }

        {
            TRACK_THREAD_LOCKER(s_ParserCritical);
            wxCriticalSectionLocker locker(s_ParserCritical);
            THREAD_LOCKER_SUCCESS(s_ParserCritical);

            m_Parser.m_PriorityHeaders.clear();
            m_Parser.m_IsPriority = false;

            if (m_Parser.m_IgnoreThreadEvents)
                m_Parser.m_IsFirstBatch = true;
        }

        while (!batchFiles.empty())
        {
            m_Parser.Parse(batchFiles.front());
            batchFiles.pop_front();
        }

        {
            TRACK_THREAD_LOCKER(s_ParserCritical);
            wxCriticalSectionLocker locker(s_ParserCritical);
            THREAD_LOCKER_SUCCESS(s_ParserCritical);

            m_Parser.m_BatchParseFiles.clear();

            if (m_Parser.m_IgnoreThreadEvents)
            {
                m_Parser.m_IgnoreThreadEvents = false;
                m_Parser.m_IsParsing = true;
            }
        }

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
        // mark all project files as local
        for (int i = 0; i < m_Project.GetFilesCount(); ++i)
        {
            ProjectFile* pf = m_Project.GetFile(i);
            if (!pf)
                continue;

            if (CCFileTypeOf(pf->relativeFilename) != ccftOther)
            {
                TRACK_THREAD_LOCKER(s_TokensTreeCritical);
                wxCriticalSectionLocker locker(s_TokensTreeCritical);
                THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);

                m_Parser.GetTokensTree()->MarkFileTokensAsLocal(pf->file.GetFullPath(), true, &m_Project);
            }
        }

        return 0;
    }

private:
    Parser&    m_Parser;
    cbProject& m_Project;
};

ParserBase::ParserBase()
{
    m_TokensTree = new TokensTree;
    m_TempTokensTree = new TokensTree;
    ReadOptions();
}

ParserBase::~ParserBase()
{
    TRACK_THREAD_LOCKER(s_TokensTreeCritical);
    wxCriticalSectionLocker locker(s_TokensTreeCritical);
    THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);

    Delete(m_TokensTree);
    Delete(m_TempTokensTree);
}

bool ParserBase::ParseFile(const wxString& filename, bool isGlobal, bool locked)
{
    return false;
}

size_t ParserBase::GetFilesCount()
{
    TRACK_THREAD_LOCKER(s_TokensTreeCritical);
    wxCriticalSectionLocker locker(s_TokensTreeCritical);
    THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);

    return m_TokensTree->m_FilesMap.size();
}

void ParserBase::ReadOptions()
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

void ParserBase::WriteOptions()
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

    // Page "Symbol browser"
    cfg->Write(_T("/browser_show_inheritance"),      m_BrowserOptions.showInheritance);
    cfg->Write(_T("/browser_expand_ns"),             m_BrowserOptions.expandNS);
    cfg->Write(_T("/browser_tree_members"),          m_BrowserOptions.treeMembers);

    // Token tree
    cfg->Write(_T("/browser_display_filter"),        m_BrowserOptions.displayFilter);
    cfg->Write(_T("/browser_sort_type"),             m_BrowserOptions.sortType);
}

void ParserBase::AddIncludeDir(const wxString& dir)
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

wxString ParserBase::FindFirstFileInIncludeDirs(const wxString& file)
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

wxArrayString ParserBase::FindFileInIncludeDirs(const wxString& file, bool firstonly)
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

wxString ParserBase::GetFullFileName(const wxString& src, const wxString& tgt, bool isGlobal)
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


Token* ParserBase::FindTokenByName(const wxString& name, bool globalsOnly, short int kindMask)
{
    TRACK_THREAD_LOCKER(s_TokensTreeCritical);
    wxCriticalSectionLocker locker(s_TokensTreeCritical);
    THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);

    int result = m_TokensTree->TokenExists(name, -1, kindMask);
    return m_TokensTree->at(result);
}

Token* ParserBase::FindChildTokenByName(Token* parent, const wxString& name, bool useInheritance, short int kindMask)
{
    if (!parent)
        return FindTokenByName(name, false, kindMask);

    TRACK_THREAD_LOCKER(s_TokensTreeCritical);
    s_TokensTreeCritical.Enter();
    THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);

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

            TRACK_THREAD_LOCKER(s_TokensTreeCritical);
            s_TokensTreeCritical.Enter();
            THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);

            if (result)
                break;
        }
    }
    s_TokensTreeCritical.Leave();
    return result;
}

size_t ParserBase::FindTokensInFile(const wxString& fileName, TokenIdxSet& result, short int kindMask)
{
    result.clear();
    wxString file = UnixFilename(fileName);
    TRACE(_T("Parser::FindTokensInFile() : Searching for file '%s' in tokens tree..."), file.wx_str());

    TRACK_THREAD_LOCKER(s_TokensTreeCritical);
    wxCriticalSectionLocker locker(s_TokensTreeCritical);
    THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);

    TokenIdxSet tmpresult;
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

Parser::Parser(wxEvtHandler* parent, cbProject* project) :
    m_Parent(parent),
    m_Project(project),
    m_UsingCache(false),
    m_Pool(this, wxNewId(), 1, 2 * 1024 * 1024), // in the meanwhile it'll have to be forced to 1
    m_IsParsing(false),
    m_IsPriority(false),
    m_NeedsReparse(false),
    m_IsFirstBatch(false),
    m_ReparseTimer(this, wxNewId()),
    m_BatchTimer(this, wxNewId()),
    m_StopWatchRunning(false),
    m_LastStopWatchTime(0),
    m_IgnoreThreadEvents(true),
    m_IsBatchParseDone(false),
    m_ParsingType(ptCreateParser),
    m_NeedMarkFileAsLocal(true)
{
    ConnectEvents();
}

Parser::~Parser()
{
    TRACK_THREAD_LOCKER(s_ParserCritical);
    wxCriticalSectionLocker locker(s_ParserCritical);
    THREAD_LOCKER_SUCCESS(s_ParserCritical);

    DisconnectEvents();
    TerminateAllThreads();

    if (s_CurrentParser == this)
        s_CurrentParser = nullptr;
}

void Parser::ConnectEvents()
{
    Connect(m_Pool.GetId(), cbEVT_THREADTASK_ALLDONE,
            (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&Parser::OnAllThreadsDone);
    Connect(m_ReparseTimer.GetId(), wxEVT_TIMER, wxTimerEventHandler(Parser::OnReparseTimer));
    Connect(m_BatchTimer.GetId(), wxEVT_TIMER, wxTimerEventHandler(Parser::OnBatchTimer));
}

void Parser::DisconnectEvents()
{
    Disconnect(m_Pool.GetId(), cbEVT_THREADTASK_ALLDONE,
               (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&Parser::OnAllThreadsDone);
    Disconnect(m_ReparseTimer.GetId(), wxEVT_TIMER, wxTimerEventHandler(Parser::OnReparseTimer));
    Disconnect(m_BatchTimer.GetId(), wxEVT_TIMER, wxTimerEventHandler(Parser::OnBatchTimer));
}

bool Parser::CacheNeedsUpdate()
{
    if (m_UsingCache)
    {
        TRACK_THREAD_LOCKER(s_TokensTreeCritical);
        wxCriticalSectionLocker locker(s_TokensTreeCritical);
        THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);

        return m_TokensTree->m_Modified;
    }
    return true;
}

bool Parser::Done()
{
    TRACK_THREAD_LOCKER(s_ParserCritical);
    wxCriticalSectionLocker locker(s_ParserCritical);
    THREAD_LOCKER_SUCCESS(s_ParserCritical);

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
    TRACK_THREAD_LOCKER(s_ParserCritical);
    wxCriticalSectionLocker locker(s_ParserCritical);
    THREAD_LOCKER_SUCCESS(s_ParserCritical);

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

void Parser::AddPredefinedMacros(const wxString& defs)
{
    if (m_BatchTimer.IsRunning())
        m_BatchTimer.Stop();

    TRACK_THREAD_LOCKER(s_ParserCritical);
    wxCriticalSectionLocker locker(s_ParserCritical);
    THREAD_LOCKER_SUCCESS(s_ParserCritical);

    m_PredefinedMacros << defs;

    if (m_ParsingType == ptUndefined)
        m_ParsingType = ptCreateParser;

    if (!m_IsParsing)
        m_BatchTimer.Start(batch_timer_delay, wxTIMER_ONE_SHOT);
}

void Parser::AddPriorityHeaders(const wxString& filename, bool systemHeaderFile)
{
    if (m_BatchTimer.IsRunning())
        m_BatchTimer.Stop();

    TRACK_THREAD_LOCKER(s_ParserCritical);
    wxCriticalSectionLocker locker(s_ParserCritical);
    THREAD_LOCKER_SUCCESS(s_ParserCritical);

    // Do priority parse in sub thread
    m_PriorityHeaders.push_back(filename);

    // Save system priority headers, when all task is over, we need reparse it!
    if (systemHeaderFile)
        m_SystemPriorityHeaders.push_back(filename);

    if (m_ParsingType == ptUndefined)
        m_ParsingType = ptCreateParser;

    if (!m_IsParsing)
        m_BatchTimer.Start(batch_timer_delay, wxTIMER_ONE_SHOT);
}

void Parser::AddBatchParse(const StringList& filenames)
{
    if (m_BatchTimer.IsRunning())
        m_BatchTimer.Stop();

    TRACK_THREAD_LOCKER(s_ParserCritical);
    wxCriticalSectionLocker locker(s_ParserCritical);
    THREAD_LOCKER_SUCCESS(s_ParserCritical);

    if (m_BatchParseFiles.empty())
        m_BatchParseFiles = filenames;
    else
        std::copy(filenames.begin(), filenames.end(), std::back_inserter(m_BatchParseFiles));

    if (m_ParsingType == ptUndefined)
        m_ParsingType = ptCreateParser;

    if (!m_IsParsing)
        m_BatchTimer.Start(batch_timer_delay, wxTIMER_ONE_SHOT);
}

void Parser::AddParse(const wxString& filename)
{
    if (m_BatchTimer.IsRunning())
        m_BatchTimer.Stop();

    TRACK_THREAD_LOCKER(s_ParserCritical);
    wxCriticalSectionLocker locker(s_ParserCritical);
    THREAD_LOCKER_SUCCESS(s_ParserCritical);

    m_BatchParseFiles.push_back(filename);

    if (!m_IsParsing)
        m_BatchTimer.Start(batch_timer_delay, wxTIMER_ONE_SHOT);
}

bool Parser::Parse(const wxString& filename, bool isLocal, bool locked, LoaderBase* loader)
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

    const wxString unixFilename = UnixFilename(filename);
    bool result = false;
    do
    {
        bool canparse = false;
        {
            if (!locked)
            {
                TRACK_THREAD_LOCKER(s_TokensTreeCritical);
                s_TokensTreeCritical.Enter();
                THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);
            }

            canparse = !m_TokensTree->IsFileParsed(unixFilename);
            if (canparse)
                canparse = m_TokensTree->ReserveFileForParsing(unixFilename, true) != 0;

            if (!locked)
                s_TokensTreeCritical.Leave();
        }

        if (!canparse)
        {
           if (opts.loader) // if a loader is already open at this point, the caller must clean it up
               CCLogger::Get()->DebugLog(_T("Parse() : CodeCompletion Plugin: FileLoader memory leak ")
                                         _T("likely while loading file ") + unixFilename);
           break;
        }

        // this should always be true
        // memory will leak if a loader has already been initialized before this point
        if (!opts.loader)
            opts.loader = Manager::Get()->GetFileManager()->Load(unixFilename, m_NeedsReparse);

        ParserThread* thread = new ParserThread(this, unixFilename, isLocal, opts, m_TokensTree);
        TRACE(_T("Parse() : Parsing %s"), unixFilename.wx_str());

        if (m_IsPriority)
        {
            if (isLocal) // Parsing priority files
            {
                TRACE(_T("Parse() : Parsing priority header, %s"), unixFilename.wx_str());

                if (!locked)
                {
                    TRACK_THREAD_LOCKER(s_TokensTreeCritical);
                    s_TokensTreeCritical.Enter();
                    THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);
                }

                result = thread->Parse();
                delete thread;

                if (!locked)
                    s_TokensTreeCritical.Leave();
                return true;
            }
            else // Add task when parsing priority files
            {
                TRACK_THREAD_LOCKER(s_ParserCritical);
                wxCriticalSectionLocker locker(s_ParserCritical);
                THREAD_LOCKER_SUCCESS(s_ParserCritical);

                TRACE(_T("Parse() : Add task for priority header, %s"), unixFilename.wx_str());
                m_PoolTask.push(PTVector());
                m_PoolTask.back().push_back(thread);
            }
        }
        else
        {
            TRACK_THREAD_LOCKER(s_ParserCritical);
            wxCriticalSectionLocker locker(s_ParserCritical);
            THREAD_LOCKER_SUCCESS(s_ParserCritical);

            TRACE(_T("Parse() : Parallel Parsing %s"), unixFilename.wx_str());

            // Add a task for all project files
            if (m_IsFirstBatch)
            {
                m_IsFirstBatch = false;
                m_PoolTask.push(PTVector());
            }

            if (m_IsParsing)
                m_Pool.AddTask(thread, true);
            else
            {
                if (!m_PoolTask.empty())
                    m_PoolTask.back().push_back(thread);
                else
                {
                    Manager::Get()->GetLogManager()->DebugLog(_T("Why m_PoolTask is empty?"));
                    return false;
                }
            }
        }

        result = true;
    }
    while (false);

    return result;
}

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

    ParserThread thread(this,
                    buffer,
                    isLocal,
                    opts,
                    m_TokensTree);

    TRACK_THREAD_LOCKER(s_TokensTreeCritical);
    wxCriticalSectionLocker locker(s_TokensTreeCritical);
    THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);

    return thread.Parse();
}

bool Parser::ParseBufferForFunctions(const wxString& buffer)
{
    ParserThreadOptions opts;
    opts.wantPreprocessor     = m_Options.wantPreprocessor;
    opts.parseComplexMacros   = m_Options.parseComplexMacros;
    opts.followLocalIncludes  = false;
    opts.followGlobalIncludes = false;
    opts.useBuffer            = true;
    opts.bufferSkipBlocks     = true;
    opts.handleFunctions      = true;

    ParserThread thread(this,
                        buffer,
                        false,
                        opts,
                        m_TempTokensTree);

    TRACK_THREAD_LOCKER(s_TokensTreeCritical);
    wxCriticalSectionLocker locker(s_TokensTreeCritical);
    THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);

    return thread.Parse();
}

bool Parser::ParseBufferForNamespaces(const wxString& buffer, NameSpaceVec& result)
{
    ParserThreadOptions opts;
    opts.useBuffer            = true;
    opts.wantPreprocessor     = m_Options.wantPreprocessor;
    opts.parseComplexMacros   = false;
    opts.followLocalIncludes  = false;
    opts.followGlobalIncludes = false;

    ParserThread thread(this,
                        wxEmptyString,
                        true,
                        opts,
                        m_TempTokensTree);

    TRACK_THREAD_LOCKER(s_TokensTreeCritical);
    wxCriticalSectionLocker locker(s_TokensTreeCritical);
    THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);

    return thread.ParseBufferForNamespaces(buffer, result);
}

bool Parser::ParseBufferForUsingNamespace(const wxString& buffer, wxArrayString& result)
{
    ParserThreadOptions opts;
    opts.useBuffer            = true;
    opts.wantPreprocessor     = m_Options.wantPreprocessor;
    opts.parseComplexMacros   = false;
    opts.followLocalIncludes  = false;
    opts.followGlobalIncludes = false;

    ParserThread thread(this,
                        wxEmptyString,
                        false,
                        opts,
                        m_TempTokensTree);

    TRACK_THREAD_LOCKER(s_TokensTreeCritical);
    wxCriticalSectionLocker locker(s_TokensTreeCritical);
    THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);

    return thread.ParseBufferForUsingNamespace(buffer, result);
}

bool Parser::RemoveFile(const wxString& filename)
{
    TRACK_THREAD_LOCKER(s_TokensTreeCritical);
    wxCriticalSectionLocker locker(s_TokensTreeCritical);
    THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);

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
    {
        wxString msg(_T("Parser::Reparse : The Parser is not done."));
        msg += NotDoneReason();
        CCLogger::Get()->DebugLog(msg);
        return false;
    }

    if (m_ReparseTimer.IsRunning())
        m_ReparseTimer.Stop();

    wxString file = UnixFilename(filename);
    if (isLocal)
        m_LocalFiles.insert(filename);
    else
        m_LocalFiles.erase(filename);

    {
        TRACK_THREAD_LOCKER(s_TokensTreeCritical);
        wxCriticalSectionLocker locker(s_TokensTreeCritical);
        THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);

        m_TokensTree->FlagFileForReparsing(file);
    }

    m_NeedsReparse = true;
    m_ReparseTimer.Start(reparse_timer_delay, wxTIMER_ONE_SHOT);

    return true;
}

bool Parser::ReadFromCache(wxInputStream* f)
{
    bool result = false;

    TRACK_THREAD_LOCKER(s_TokensTreeCritical);
    wxCriticalSectionLocker locker(s_TokensTreeCritical);
    THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);

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

    TRACK_THREAD_LOCKER(s_TokensTreeCritical);
    wxCriticalSectionLocker locker(s_TokensTreeCritical);
    THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);

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
    while (!m_PoolTask.empty())
    {
        PTVector& v = m_PoolTask.front();
        for (PTVector::iterator it = v.begin(); it != v.end(); ++it)
            delete *it;
        m_PoolTask.pop();
    }

    m_Pool.AbortAllTasks();
    while (!m_Pool.Done())
        wxMilliSleep(1);
}

bool Parser::UpdateParsingProject(cbProject* project)
{
    if (m_Project == project)
        return true;
    else if (!Done())
    {
        wxString msg(_T("Parser::UpdateParsingProject : The Parser is not done."));
        msg += NotDoneReason();
        CCLogger::Get()->DebugLog(msg);
        return false;
    }
    else
    {
        m_Project = project;
        return true;
    }
}

void Parser::OnAllThreadsDone(CodeBlocksEvent& event)
{
    if (m_IgnoreThreadEvents || Manager::IsAppShuttingDown())
        return;

    if (event.GetId() != m_Pool.GetId())
    {
        CCLogger::Get()->DebugLog(_T("Why event.GetId() not equal m_Pool.GetId()?"));
        return;
    }

    if (!m_TokensTree)
        cbThrow(_T("m_TokensTree is a nullptr?!"));

    if (!m_IsParsing)
    {
        CCLogger::Get()->DebugLog(_T("Why m_IsParsing is false?"));
        return;
    }

    // Do next task
    if (   !m_PoolTask.empty()
        || !m_BatchParseFiles.empty()
        || !m_PriorityHeaders.empty()
        || !m_PredefinedMacros.IsEmpty() )
    {
        m_BatchTimer.Start(10, wxTIMER_ONE_SHOT);
    }

#if !CC_PARSER_PROFILE_TEST
    // Reparse system priority headers
    else if (!m_SystemPriorityHeaders.empty())
    {
        // 1. Remove all priority headers in token tree
        for (StringList::iterator it = m_SystemPriorityHeaders.begin(); it != m_SystemPriorityHeaders.end(); ++it)
            RemoveFile(*it);

        // 2. Reparse system priority headers
        AddBatchParse(m_SystemPriorityHeaders);

        // 3. Clear
        m_SystemPriorityHeaders.clear();

        // 4. Begin batch parsing
        m_BatchTimer.Start(10, wxTIMER_ONE_SHOT);
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

        m_IgnoreThreadEvents = true;
        m_NeedsReparse       = false;
        m_IsParsing          = false;
        m_IsBatchParseDone   = true;

        EndStopWatch();

        wxString parseEndLog;
        {
            TRACK_THREAD_LOCKER(s_TokensTreeCritical);
            wxCriticalSectionLocker locker(s_TokensTreeCritical);
            THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);

            parseEndLog.Printf(_T("Project '%s' parsing stage done (%d total parsed files, ")
                               _T("%d tokens in %ld minute(s), %ld.%03ld seconds)."),
                               m_Project    ? m_Project->GetTitle().wx_str()  : _T("*NONE*"),
                               m_TokensTree ? m_TokensTree->m_FilesMap.size() : 0,
                               m_TokensTree ? m_TokensTree->realsize()        : 0,
                               (m_LastStopWatchTime / 60000),
                               (m_LastStopWatchTime / 1000) % 60,
                               (m_LastStopWatchTime % 1000) );
        }

        ProcessParserEvent(m_ParsingType, idParserEnd, parseEndLog);
        m_ParsingType = ptUndefined;
        s_CurrentParser = nullptr;
    }
}

bool Parser::ParseFile(const wxString& filename, bool isGlobal, bool locked)
{
    if (   (!isGlobal && !m_Options.followLocalIncludes)
        || ( isGlobal && !m_Options.followGlobalIncludes) )
        return false;

    if (filename.IsEmpty())
        return false;

//    bool locked = false;
//    if (m_IsParsing)
//    {
//        TRACK_THREAD_LOCKER(s_ParserCritical);
//        s_ParserCritical.Enter();
//        THREAD_LOCKER_SUCCESS(s_ParserCritical);
//        locked = true;
//    }
    // TODO (Loaden#9#) loacker ?
    const bool ret = Parse(filename, !isGlobal, locked);
//    if (locked)
//        s_ParserCritical.Leave();
    return ret;
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

void Parser::OnReparseTimer(wxTimerEvent& event)
{
    ReparseModifiedFiles();
    event.Skip();
}

void Parser::OnBatchTimer(wxTimerEvent& event)
{
    if (Manager::IsAppShuttingDown())
        return;

    // Current batch parser is already exists
    if (s_CurrentParser && s_CurrentParser != this)
    {
        m_BatchTimer.Start(1000, wxTIMER_ONE_SHOT);
        return;
    }

    bool sendStartParseEvent = false;
    do
    {
        TRACK_THREAD_LOCKER(s_ParserCritical);
        wxCriticalSectionLocker locker(s_ParserCritical);
        THREAD_LOCKER_SUCCESS(s_ParserCritical);

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
            AddParseThread* thread = new AddParseThread(*this);
            m_Pool.AddTask(thread, true);
            // Have not done any batch parsing
            if (s_CurrentParser)
                return;
            else
            {
                s_CurrentParser = this;
                m_StopWatch.Start(); // reset timer
                sendStartParseEvent = true;
            }
        }
    } while (false);

    if (sendStartParseEvent)
        ProcessParserEvent(m_ParsingType, idParserStart);
    else
        ProcessParserEvent(ptUndefined, idParserStart, _T("Unexpected behavior!"));
}

void Parser::ReparseModifiedFiles()
{
    if (!Done())
    {
        wxString msg(_T("Parser::ReparseModifiedFiles : The Parser is not done."));
        msg += NotDoneReason();
        CCLogger::Get()->DebugLog(msg);
        m_ReparseTimer.Start(reparse_timer_delay, wxTIMER_ONE_SHOT);
        return;
    }

    if (!m_NeedsReparse)
        m_NeedsReparse = true;

    std::queue<wxString> files_list;
    {
        TRACK_THREAD_LOCKER(s_TokensTreeCritical);
        wxCriticalSectionLocker locker(s_TokensTreeCritical);
        THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);

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

    if (!files_list.empty() && m_ParsingType == ptUndefined)
        m_ParsingType = ptReparseFile;
    else
        m_NeedsReparse = false;

    while (!files_list.empty())
    {
        AddParse(files_list.front());
        files_list.pop();
    }
}

bool Parser::IsFileParsed(const wxString& filename)
{
    bool isParsed = false;
    {
        TRACK_THREAD_LOCKER(s_TokensTreeCritical);
        wxCriticalSectionLocker locker(s_TokensTreeCritical);
        THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);

        isParsed = m_TokensTree->IsFileParsed(UnixFilename(filename));
    }

    if (!isParsed)
    {
        TRACK_THREAD_LOCKER(s_ParserCritical);
        wxCriticalSectionLocker locker(s_ParserCritical);
        THREAD_LOCKER_SUCCESS(s_ParserCritical);

        StringList::iterator it = std::find(m_BatchParseFiles.begin(), m_BatchParseFiles.end(), filename);
        isParsed = it != m_BatchParseFiles.end();
    }

    return isParsed;
}

void Parser::ProcessParserEvent(ParsingType type, int id, const wxString& info)
{
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, id);
    evt.SetEventObject(this);       // Parser*
    evt.SetClientData(m_Project);   // cbProject*
    evt.SetInt(type);
    evt.SetString(info);
    m_Parent->ProcessEvent(evt);
}

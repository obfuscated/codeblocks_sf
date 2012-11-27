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

    #include <cbproject.h>
    #include <configmanager.h>
    #include <editormanager.h>
    #include <globals.h>
    #include <infowindow.h>
    #include <logmanager.h>
    #include <manager.h>
#endif

#include <wx/tokenzr.h>
#include <cbstyledtextctrl.h>

#include "parser.h"
#include "parserthreadedtask.h"

#include "../classbrowser.h"
#include "../classbrowserbuilderthread.h"


#ifndef CB_PRECOMP
    #include "editorbase.h"
#endif

#define CC_PARSER_DEBUG_OUTPUT 0

#if defined(CC_GLOBAL_DEBUG_OUTPUT)
    #if CC_GLOBAL_DEBUG_OUTPUT == 1
        #undef CC_PARSER_DEBUG_OUTPUT
        #define CC_PARSER_DEBUG_OUTPUT 1
    #elif CC_GLOBAL_DEBUG_OUTPUT == 2
        #undef CC_PARSER_DEBUG_OUTPUT
        #define CC_PARSER_DEBUG_OUTPUT 2
    #endif
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

namespace ParserCommon
{
    static const int PARSER_BATCHPARSE_TIMER_DELAY           = 300;
    static const int PARSER_BATCHPARSE_TIMER_RUN_IMMEDIATELY = 10;
    static const int PARSER_BATCHPARSE_TIMER_DELAY_LONG      = 1000;
    static const int PARSER_REPARSE_TIMER_DELAY              = 100;

    static volatile Parser* s_CurrentParser = nullptr;
    static          wxMutex s_ParserMutex;

    int idParserStart = wxNewId();
    int idParserEnd   = wxNewId();

    EFileType FileType(const wxString& filename, bool force_refresh)
    {
        static bool          cfg_read  = false;
        static bool          empty_ext = true;
        static wxArrayString header_ext;
        static wxArrayString source_ext;

        if (!cfg_read || force_refresh)
        {
            ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("code_completion"));
            empty_ext               = cfg->ReadBool(_T("/empty_ext"), true);
            wxString header_ext_str = cfg->Read(_T("/header_ext"), _T("h,hpp,tcc,xpm"));
            wxString source_ext_str = cfg->Read(_T("/source_ext"), _T("c,cpp,cxx,cc,c++"));

            header_ext.Clear();
            wxStringTokenizer header_ext_tknzr(header_ext_str, _T(","));
            while (header_ext_tknzr.HasMoreTokens())
                header_ext.Add(header_ext_tknzr.GetNextToken().Trim(false).Trim(true).Lower());

            source_ext.Clear();
            wxStringTokenizer source_ext_tknzr(source_ext_str, _T(","));
            while (source_ext_tknzr.HasMoreTokens())
                source_ext.Add(source_ext_tknzr.GetNextToken().Trim(false).Trim(true).Lower());

            cfg_read = true; // caching done
        }

        if (filename.IsEmpty())
            return ParserCommon::ftOther;

        const wxString file = filename.AfterLast(wxFILE_SEP_PATH).Lower();
        const int      pos  = file.Find(_T('.'), true);
        wxString       ext;
        if (pos != wxNOT_FOUND)
            ext = file.SubString(pos + 1, file.Len());

        if (empty_ext && ext.IsEmpty())
            return ParserCommon::ftHeader;

        for (size_t i=0; i<header_ext.GetCount(); ++i)
        {
            if (ext==header_ext[i])
                return ParserCommon::ftHeader;
        }

        for (size_t i=0; i<source_ext.GetCount(); ++i)
        {
            if (ext==source_ext[i])
                return ParserCommon::ftSource;
        }

        return ParserCommon::ftOther;
    }
}// namespace ParserCommon

ParserBase::ParserBase()
{
    m_TokenTree     = new TokenTree;
    m_TempTokenTree = new TokenTree;
    ReadOptions();
}

ParserBase::~ParserBase()
{
    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    Delete(m_TokenTree);
    Delete(m_TempTokenTree);

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)
}

TokenTree* ParserBase::GetTokenTree()
{
    return m_TokenTree;
}

bool ParserBase::ParseFile(cb_unused const wxString& filename, cb_unused bool isGlobal, cb_unused bool locked)
{
    return false;
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

    // force e-read of file types
    ParserCommon::EFileType ft_dummy = ParserCommon::FileType(wxEmptyString, true);
    wxUnusedVar(ft_dummy);
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
    TRACE(_T("FindFileInIncludeDirs() : Found %lu"), static_cast<unsigned long>(FoundSet.GetCount()));

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

size_t ParserBase::FindTokensInFile(const wxString& filename, TokenIdxSet& result, short int kindMask)
{
    result.clear();
    size_t tokens_found = 0;

    TRACE(_T("Parser::FindTokensInFile() : Searching for file '%s' in tokens tree..."), filename.wx_str());

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    TokenIdxSet tmpresult;
    if ( m_TokenTree->FindTokensInFile(filename, tmpresult, kindMask) )
    {
        for (TokenIdxSet::const_iterator it = tmpresult.begin(); it != tmpresult.end(); ++it)
        {
            const Token* token = m_TokenTree->at(*it);
            if (token)
                result.insert(*it);
        }
        tokens_found = result.size();
    }

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    return tokens_found;
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
    m_ParserState(ParserCommon::ptCreateParser),
    m_NeedMarkFileAsLocal(true)
{
    ConnectEvents();
}

Parser::~Parser()
{
    CC_LOCKER_TRACK_P_MTX_LOCK(ParserCommon::s_ParserMutex)

    DisconnectEvents();
    TerminateAllThreads();

    if (ParserCommon::s_CurrentParser == this)
        ParserCommon::s_CurrentParser = nullptr;

    CC_LOCKER_TRACK_P_MTX_UNLOCK(ParserCommon::s_ParserMutex)
}

void Parser::ConnectEvents()
{
    Connect(m_Pool.GetId(),         cbEVT_THREADTASK_ALLDONE,
            (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&Parser::OnAllThreadsDone);
    Connect(m_ReparseTimer.GetId(), wxEVT_TIMER, wxTimerEventHandler(Parser::OnReparseTimer));
    Connect(m_BatchTimer.GetId(),   wxEVT_TIMER, wxTimerEventHandler(Parser::OnBatchTimer));
}

void Parser::DisconnectEvents()
{
    Disconnect(m_Pool.GetId(),         cbEVT_THREADTASK_ALLDONE,
               (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&Parser::OnAllThreadsDone);
    Disconnect(m_ReparseTimer.GetId(), wxEVT_TIMER, wxTimerEventHandler(Parser::OnReparseTimer));
    Disconnect(m_BatchTimer.GetId(),   wxEVT_TIMER, wxTimerEventHandler(Parser::OnBatchTimer));
}

bool Parser::Done()
{
    CC_LOCKER_TRACK_P_MTX_LOCK(ParserCommon::s_ParserMutex)

    bool done =    m_PriorityHeaders.empty()
                && m_SystemPriorityHeaders.empty()
                && m_BatchParseFiles.empty()
                && m_PredefinedMacros.IsEmpty()
                && !m_NeedMarkFileAsLocal
                && m_PoolTask.empty()
                && m_Pool.Done();

    CC_LOCKER_TRACK_P_MTX_UNLOCK(ParserCommon::s_ParserMutex)

    return done;
}

wxString Parser::NotDoneReason()
{
    CC_LOCKER_TRACK_P_MTX_LOCK(ParserCommon::s_ParserMutex)

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

    CC_LOCKER_TRACK_P_MTX_UNLOCK(ParserCommon::s_ParserMutex)

    return reason;
}

void Parser::AddPredefinedMacros(const wxString& defs)
{
    if (m_BatchTimer.IsRunning())
        m_BatchTimer.Stop();

    CC_LOCKER_TRACK_P_MTX_LOCK(ParserCommon::s_ParserMutex)

    m_PredefinedMacros << defs;

    if (m_ParserState == ParserCommon::ptUndefined)
        m_ParserState = ParserCommon::ptCreateParser;

    if (!m_IsParsing)
        m_BatchTimer.Start(ParserCommon::PARSER_BATCHPARSE_TIMER_DELAY, wxTIMER_ONE_SHOT);

    CC_LOCKER_TRACK_P_MTX_UNLOCK(ParserCommon::s_ParserMutex)
}

void Parser::AddPriorityHeaders(const wxString& filename, bool systemHeaderFile)
{
    if (m_BatchTimer.IsRunning())
        m_BatchTimer.Stop();

    CC_LOCKER_TRACK_P_MTX_LOCK(ParserCommon::s_ParserMutex)

    // Do priority parse in sub thread
    m_PriorityHeaders.push_back(filename);

    // Save system priority headers, when all task is over, we need reparse it!
    if (systemHeaderFile)
        m_SystemPriorityHeaders.push_back(filename);

    if (m_ParserState == ParserCommon::ptUndefined)
        m_ParserState = ParserCommon::ptCreateParser;

    if (!m_IsParsing)
        m_BatchTimer.Start(ParserCommon::PARSER_BATCHPARSE_TIMER_DELAY, wxTIMER_ONE_SHOT);

    CC_LOCKER_TRACK_P_MTX_UNLOCK(ParserCommon::s_ParserMutex)
}

void Parser::AddBatchParse(const StringList& filenames)
{
    if (m_BatchTimer.IsRunning())
        m_BatchTimer.Stop();

    CC_LOCKER_TRACK_P_MTX_LOCK(ParserCommon::s_ParserMutex)

    if (m_BatchParseFiles.empty())
        m_BatchParseFiles = filenames;
    else
        std::copy(filenames.begin(), filenames.end(), std::back_inserter(m_BatchParseFiles));

    if (m_ParserState == ParserCommon::ptUndefined)
        m_ParserState = ParserCommon::ptCreateParser;

    if (!m_IsParsing)
        m_BatchTimer.Start(ParserCommon::PARSER_BATCHPARSE_TIMER_DELAY, wxTIMER_ONE_SHOT);

    CC_LOCKER_TRACK_P_MTX_UNLOCK(ParserCommon::s_ParserMutex)
}

void Parser::AddParse(const wxString& filename)
{
    if (m_BatchTimer.IsRunning())
        m_BatchTimer.Stop();

    CC_LOCKER_TRACK_P_MTX_LOCK(ParserCommon::s_ParserMutex)

    m_BatchParseFiles.push_back(filename);

    if (!m_IsParsing)
        m_BatchTimer.Start(ParserCommon::PARSER_BATCHPARSE_TIMER_DELAY, wxTIMER_ONE_SHOT);

    CC_LOCKER_TRACK_P_MTX_UNLOCK(ParserCommon::s_ParserMutex)
}

bool Parser::Parse(const wxString& filename, bool isLocal, bool locked, LoaderBase* loader)
{
    ParserThreadOptions opts;

    opts.useBuffer             = false;
    opts.bufferSkipBlocks      = false;
    opts.bufferSkipOuterBlocks = false;

    opts.followLocalIncludes   = m_Options.followLocalIncludes;
    opts.followGlobalIncludes  = m_Options.followGlobalIncludes;
    opts.wantPreprocessor      = m_Options.wantPreprocessor;
    opts.parseComplexMacros    = m_Options.parseComplexMacros;

    opts.loader                = loader; // maybe 0 at this point

    bool result = false;
    do
    {
        bool canparse = false;
        {
            if (!locked)
                CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

            canparse = !m_TokenTree->IsFileParsed(filename);
            if (canparse)
                canparse = m_TokenTree->ReserveFileForParsing(filename, true) != 0;

            if (!locked)
                CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)
        }

        if (!canparse)
        {
           if (opts.loader) // if a loader is already open at this point, the caller must clean it up
               CCLogger::Get()->DebugLog(_T("Parse() : CodeCompletion Plugin: FileLoader memory leak ")
                                         _T("likely while loading file ") + filename);
           break;
        }

        // this should always be true
        // memory will leak if a loader has already been initialized before this point
        if (!opts.loader)
            opts.loader = Manager::Get()->GetFileManager()->Load(filename, m_NeedsReparse);

        ParserThread* thread = new ParserThread(this, filename, isLocal, opts, m_TokenTree);
        TRACE(_T("Parse() : Parsing %s"), filename.wx_str());

        if (m_IsPriority)
        {
            if (isLocal) // Parsing priority files
            {
                TRACE(_T("Parse() : Parsing priority header, %s"), filename.wx_str());

                if (!locked)
                    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

                result = thread->Parse();
                delete thread;

                if (!locked)
                    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

                return true;
            }
            else // Add task when parsing priority files
            {
                CC_LOCKER_TRACK_P_MTX_LOCK(ParserCommon::s_ParserMutex)

                TRACE(_T("Parse() : Add task for priority header, %s"), filename.wx_str());
                m_PoolTask.push(PTVector());
                m_PoolTask.back().push_back(thread);

                CC_LOCKER_TRACK_P_MTX_UNLOCK(ParserCommon::s_ParserMutex)
            }
        }
        else
        {
            TRACE(_T("Parse() : Parallel Parsing %s"), filename.wx_str());

            CC_LOCKER_TRACK_P_MTX_LOCK(ParserCommon::s_ParserMutex)

            // Add a task for all project files
            if (m_IsFirstBatch)
            {
                m_IsFirstBatch = false;
                m_PoolTask.push(PTVector());
            }

            bool pool_task_empty = false;
            if (m_IsParsing)
                m_Pool.AddTask(thread, true);
            else
            {
                if (!m_PoolTask.empty())
                    m_PoolTask.back().push_back(thread);
                else
                    pool_task_empty = true;
            }

            CC_LOCKER_TRACK_P_MTX_UNLOCK(ParserCommon::s_ParserMutex)

            if (pool_task_empty)
            {
                CCLogger::Get()->DebugLog(_T("Parse() : m_PoolTask is empty, why?!"));
                return false;
            }
        }

        result = true;
    }
    while (false);

    return result;
}

bool Parser::ParseBuffer(const wxString& buffer,   bool isLocal,
                         bool  bufferSkipBlocks,   bool isTemp,
                         const wxString& filename, int  parentIdx, int initLine)
{
    ParserThreadOptions opts;

    opts.useBuffer            = true;
    opts.fileOfBuffer         = filename;
    opts.parentIdxOfBuffer    = parentIdx;
    opts.initLineOfBuffer     = initLine;
    opts.bufferSkipBlocks     = bufferSkipBlocks;
    opts.isTemp               = isTemp;

    opts.followLocalIncludes  = false;
    opts.followGlobalIncludes = false;
    opts.wantPreprocessor     = m_Options.wantPreprocessor;
    opts.parseComplexMacros   = false;

    opts.handleFunctions      = false;

    ParserThread thread(this, buffer, isLocal, opts, m_TokenTree);

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    bool success = thread.Parse();

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    return success;
}

bool Parser::ParseBufferForFunctions(const wxString& buffer)
{
    ParserThreadOptions opts;

    opts.useBuffer            = true;
    opts.bufferSkipBlocks     = true;

    opts.followLocalIncludes  = false;
    opts.followGlobalIncludes = false;
    opts.wantPreprocessor     = m_Options.wantPreprocessor;
    opts.parseComplexMacros   = m_Options.parseComplexMacros;

    opts.handleFunctions      = true;

    ParserThread thread(this, buffer, false, opts, m_TempTokenTree);

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    bool success = thread.Parse();

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    return success;
}

bool Parser::ParseBufferForNamespaces(const wxString& buffer, NameSpaceVec& result)
{
    ParserThreadOptions opts;

    opts.useBuffer            = true;

    opts.followLocalIncludes  = false;
    opts.followGlobalIncludes = false;
    opts.wantPreprocessor     = m_Options.wantPreprocessor;
    opts.parseComplexMacros   = false;

    ParserThread thread(this, wxEmptyString, true, opts, m_TempTokenTree);

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    bool success = thread.ParseBufferForNamespaces(buffer, result);

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    return success;
}

bool Parser::ParseBufferForUsingNamespace(const wxString& buffer, wxArrayString& result)
{
    ParserThreadOptions opts;

    opts.useBuffer            = true;

    opts.followLocalIncludes  = false;
    opts.followGlobalIncludes = false;
    opts.wantPreprocessor     = m_Options.wantPreprocessor;
    opts.parseComplexMacros   = false;

    ParserThread thread(this, wxEmptyString, false, opts, m_TempTokenTree);

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    bool success = thread.ParseBufferForUsingNamespace(buffer, result);

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    return success;
}

bool Parser::RemoveFile(const wxString& filename)
{
    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    const size_t fileIdx = m_TokenTree->InsertFileOrGetIndex(filename);
    const bool   result  = m_TokenTree->GetFileStatusCountForIndex(fileIdx);

    m_TokenTree->RemoveFile(filename);
    m_TokenTree->EraseFileMapInFileMap(fileIdx);
    m_TokenTree->EraseFileStatusByIndex(fileIdx);
    m_TokenTree->EraseFilesToBeReparsedByIndex(fileIdx);

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    return result;
}

bool Parser::AddFile(const wxString& filename, cbProject* project, cb_unused bool isLocal)
{
    if (project != m_Project)
        return false;

    if ( IsFileParsed(filename) )
        return false;

    if (m_ParserState == ParserCommon::ptUndefined)
        m_ParserState = ParserCommon::ptAddFileToParser;

    AddParse(filename);
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

    if (isLocal)
        m_LocalFiles.insert(filename);
    else
        m_LocalFiles.erase(filename);

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    m_TokenTree->FlagFileForReparsing(filename);

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    m_NeedsReparse = true;
    m_ReparseTimer.Start(ParserCommon::PARSER_REPARSE_TIMER_DELAY, wxTIMER_ONE_SHOT);

    return true;
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
        CCLogger::Get()->DebugLog(_T("Why is event.GetId() not equal m_Pool.GetId()?"));
        return;
    }

    if (!m_TokenTree)
        cbThrow(_T("m_TokenTree is a nullptr?!"));

    if (!m_IsParsing)
    {
        CCLogger::Get()->DebugLog(_T("Why is m_IsParsing false?"));
        return;
    }

    // Do next task
    if (   !m_PoolTask.empty()
        || !m_BatchParseFiles.empty()
        || !m_PriorityHeaders.empty()
        || !m_PredefinedMacros.IsEmpty() )
    {
        m_BatchTimer.Start(ParserCommon::PARSER_BATCHPARSE_TIMER_RUN_IMMEDIATELY, wxTIMER_ONE_SHOT);
    }
#if defined(CC_PARSER_PROFILE_TEST)
    // Do nothing
#else
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
        m_BatchTimer.Start(ParserCommon::PARSER_BATCHPARSE_TIMER_RUN_IMMEDIATELY, wxTIMER_ONE_SHOT);
    }
    else if (   (   m_ParserState == ParserCommon::ptCreateParser
                 || m_ParserState == ParserCommon::ptAddFileToParser )
             && m_NeedMarkFileAsLocal
             && m_Project)
    {
        m_NeedMarkFileAsLocal = false;
        MarkFileAsLocalThreadedTask* thread = new MarkFileAsLocalThreadedTask(this, m_Project);
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

        EndStopWatch(); // stop counting the time we take for parsing the files

        wxString prj = (m_Project ? m_Project->GetTitle() : _T("*NONE*"));
        wxString parseEndLog;

        CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

        parseEndLog.Printf(_T("Project '%s' parsing stage done (%lu total parsed files, ")
                           _T("%lu tokens in %ld minute(s), %ld.%03ld seconds)."), prj.wx_str(),
                           m_TokenTree ? static_cast<unsigned long>(m_TokenTree->GetFileMapSize()) : 0,
                           m_TokenTree ? static_cast<unsigned long>(m_TokenTree->realsize())       : 0,
                           (m_LastStopWatchTime / 60000),
                           (m_LastStopWatchTime / 1000) % 60,
                           (m_LastStopWatchTime % 1000) );

        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

        ProcessParserEvent(m_ParserState, ParserCommon::idParserEnd, parseEndLog);
        m_ParserState = ParserCommon::ptUndefined;
        ParserCommon::s_CurrentParser = nullptr;
    }
}

bool Parser::ParseFile(const wxString& filename, bool isGlobal, bool locked)
{
    if (   (!isGlobal && !m_Options.followLocalIncludes)
        || ( isGlobal && !m_Options.followGlobalIncludes) )
        return false;

    if (filename.IsEmpty())
        return false;

    // TODO (Loaden#9#) locker ?
    const bool ret = Parse(filename, !isGlobal, locked);

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
            m_LastStopWatchTime  = m_StopWatch.Time();
        else
            m_LastStopWatchTime += m_StopWatch.Time();
    }
}

void Parser::OnReparseTimer(wxTimerEvent& event)
{
    ReparseModifiedFiles();
    event.Skip();
}

void Parser::OnBatchTimer(cb_unused wxTimerEvent& event)
{
    if (Manager::IsAppShuttingDown())
        return;

    if (ParserCommon::s_CurrentParser && ParserCommon::s_CurrentParser != this)
    {
        // Current batch parser already exists, just return later
        m_BatchTimer.Start(ParserCommon::PARSER_BATCHPARSE_TIMER_DELAY_LONG, wxTIMER_ONE_SHOT);
        return;
    }

    StartStopWatch(); // start counting the time we take for parsing the files

    bool send_event          = true;
    bool sendStartParseEvent = false;

    if (   m_PoolTask.empty()
        && m_PriorityHeaders.empty()
        && m_BatchParseFiles.empty()
        && m_PredefinedMacros.IsEmpty() ) // easy case: is there any thing to do at all?
    {
        send_event = false; // Nothing to do.
    }
    else if (!m_PoolTask.empty()) // there are already batch jobs - so just add the new ones
    {
        m_Pool.BatchBegin();

        PTVector& v = m_PoolTask.front();
        for (PTVector::const_iterator it = v.begin(); it != v.end(); ++it)
            m_Pool.AddTask(*it, true);
        m_PoolTask.pop();

        m_Pool.BatchEnd();
        send_event = false; // nothing to do anymore, the pool si already being processed
    }
    else if (   !m_PriorityHeaders.empty()
             || !m_BatchParseFiles.empty()
             || !m_PredefinedMacros.IsEmpty() )
    {
        CC_LOCKER_TRACK_P_MTX_LOCK(ParserCommon::s_ParserMutex)

        ParserThreadedTask* thread = new ParserThreadedTask(this, ParserCommon::s_ParserMutex);
        m_Pool.AddTask(thread, true);

        if (ParserCommon::s_CurrentParser)
            send_event = false;
        else // Have not done any batch parsing yet -> assign parser
        {
            ParserCommon::s_CurrentParser = this;
            m_StopWatch.Start(); // reset timer
            sendStartParseEvent = true;
        }

        CC_LOCKER_TRACK_P_MTX_UNLOCK(ParserCommon::s_ParserMutex)
    }

    if (send_event)
    {
        if (sendStartParseEvent)
            ProcessParserEvent(m_ParserState,             ParserCommon::idParserStart);
        else
            ProcessParserEvent(ParserCommon::ptUndefined, ParserCommon::idParserStart, _T("Unexpected behaviour!"));
    }
}

void Parser::ReparseModifiedFiles()
{
    if ( !Done() )
    {
        wxString msg(_T("Parser::ReparseModifiedFiles : The Parser is not done."));
        msg += NotDoneReason();
        CCLogger::Get()->DebugLog(msg);
        m_ReparseTimer.Start(ParserCommon::PARSER_REPARSE_TIMER_DELAY, wxTIMER_ONE_SHOT);
        return;
    }

    if (!m_NeedsReparse)
        m_NeedsReparse = true;

    std::queue<size_t>   files_idx;
    std::queue<wxString> files_list;
    TokenFileSet::const_iterator it;

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    // Collect files to be re-parsed and therefore be removed before:
    // Loop two times so that we reparse modified *header* files first
    // because they usually hold definitions which need to exist
    // when we parse the normal source files...
    for (it = m_TokenTree->GetFilesToBeReparsed()->begin(); it != m_TokenTree->GetFilesToBeReparsed()->end(); ++it)
    {
        wxString filename = m_TokenTree->GetFilename(*it);
        if ( FileTypeOf(filename) == ftSource ) // ignore source files (*.cpp etc)
            continue;
        files_list.push(filename);
        files_idx.push(*it);
    }
    for (it = m_TokenTree->GetFilesToBeReparsed()->begin(); it != m_TokenTree->GetFilesToBeReparsed()->end(); ++it)
    {
        wxString filename = m_TokenTree->GetFilename(*it);
        if ( FileTypeOf(filename) != ftSource ) // ignore non-source files (*.h etc)
            continue;
        files_list.push(filename);
        files_idx.push(*it);
    }

    // Now actually remove the files from the tree
    while (!files_idx.empty())
    {
        m_TokenTree->RemoveFile(files_idx.front());
        files_idx.pop();
    }

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    if (!files_list.empty() && m_ParserState == ParserCommon::ptUndefined)
        m_ParserState = ParserCommon::ptReparseFile;
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

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    isParsed = m_TokenTree->IsFileParsed(filename);

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    if (!isParsed)
    {
        CC_LOCKER_TRACK_P_MTX_LOCK(ParserCommon::s_ParserMutex)

        StringList::iterator it = std::find(m_BatchParseFiles.begin(), m_BatchParseFiles.end(), filename);
        isParsed = it != m_BatchParseFiles.end();

        CC_LOCKER_TRACK_P_MTX_UNLOCK(ParserCommon::s_ParserMutex)
    }

    return isParsed;
}

void Parser::ProcessParserEvent(ParserCommon::ParserState state, int id, const wxString& info)
{
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, id);
    evt.SetEventObject(this);     // Parser*
    evt.SetClientData(m_Project); // cbProject*
    evt.SetInt(state);
    evt.SetString(info);
    m_Parent->ProcessEvent(evt);
}

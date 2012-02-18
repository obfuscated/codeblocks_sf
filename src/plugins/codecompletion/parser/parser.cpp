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

namespace ParserCommon
{
    static const int PARSER_BATCHPARSE_TIMER_DELAY = 300;
    static const int PARSER_REPARSE_TIMER_DELAY    = 100;

    static volatile Parser*  s_CurrentParser = nullptr;
    static wxMutex s_ParserMutex;

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
    m_TokensTree     = new TokensTree;
    m_TempTokensTree = new TokensTree;
    ReadOptions();
}

ParserBase::~ParserBase()
{
    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

    Delete(m_TokensTree);
    Delete(m_TempTokensTree);

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)
}

TokensTree* ParserBase::GetTokensTree()
{
    return m_TokensTree;
}

bool ParserBase::ParseFile(const wxString& filename, bool isGlobal, bool locked)
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

size_t ParserBase::FindTokensInFile(const wxString& fileName, TokenIdxSet& result, short int kindMask)
{
    result.clear();
    size_t tokens_found = 0;

    wxString file = UnixFilename(fileName);
    TRACE(_T("Parser::FindTokensInFile() : Searching for file '%s' in tokens tree..."), file.wx_str());

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

    TokenIdxSet tmpresult;
    if ( m_TokensTree->FindTokensInFile(file, tmpresult, kindMask) )
    {
        for (TokenIdxSet::iterator it = tmpresult.begin(); it != tmpresult.end(); ++it)
        {
            Token* token = m_TokensTree->at(*it);
            if (token)
                result.insert(*it);
        }
        tokens_found = result.size();
    }

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)

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
    m_ParsingType(ptCreateParser),
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

    if (m_ParsingType == ptUndefined)
        m_ParsingType = ptCreateParser;

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

    if (m_ParsingType == ptUndefined)
        m_ParsingType = ptCreateParser;

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

    if (m_ParsingType == ptUndefined)
        m_ParsingType = ptCreateParser;

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

    const wxString unixFilename = UnixFilename(filename);
    bool result = false;
    do
    {
        bool canparse = false;
        {
            if (!locked)
                CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

            canparse = !m_TokensTree->IsFileParsed(unixFilename);
            if (canparse)
                canparse = m_TokensTree->ReserveFileForParsing(unixFilename, true) != 0;

            if (!locked)
                CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)
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
                    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

                result = thread->Parse();
                delete thread;

                if (!locked)
                    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)

                return true;
            }
            else // Add task when parsing priority files
            {
                CC_LOCKER_TRACK_P_MTX_LOCK(ParserCommon::s_ParserMutex)

                TRACE(_T("Parse() : Add task for priority header, %s"), unixFilename.wx_str());
                m_PoolTask.push(PTVector());
                m_PoolTask.back().push_back(thread);

                CC_LOCKER_TRACK_P_MTX_UNLOCK(ParserCommon::s_ParserMutex)
            }
        }
        else
        {
            TRACE(_T("Parse() : Parallel Parsing %s"), unixFilename.wx_str());

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

    ParserThread thread(this, buffer, isLocal, opts, m_TokensTree);

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

    bool success = thread.Parse();

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)

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

    ParserThread thread(this, buffer, false, opts, m_TempTokensTree);

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

    bool success = thread.Parse();

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)

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

    ParserThread thread(this, wxEmptyString, true, opts, m_TempTokensTree);

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

    bool success = thread.ParseBufferForNamespaces(buffer, result);

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)

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

    ParserThread thread(this, wxEmptyString, false, opts, m_TempTokensTree);

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

    bool success = thread.ParseBufferForUsingNamespace(buffer, result);

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)

    return success;
}

bool Parser::RemoveFile(const wxString& filename)
{
    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

    size_t     index  = m_TokensTree->GetFileIndex(UnixFilename(filename));
    const bool result = m_TokensTree->m_FilesStatus.count(index);

    m_TokensTree->RemoveFile(filename);
    m_TokensTree->m_FilesMap.erase(index);
    m_TokensTree->m_FilesStatus.erase(index);
    m_TokensTree->m_FilesToBeReparsed.erase(index);
    m_TokensTree->m_Modified = true;

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)

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

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

    m_TokensTree->FlagFileForReparsing(file);

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)

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

        EndStopWatch();

        wxString parseEndLog;

        CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

        parseEndLog.Printf(_T("Project '%s' parsing stage done (%lu total parsed files, ")
                           _T("%lu tokens in %ld minute(s), %ld.%03ld seconds)."),
                           m_Project    ? m_Project->GetTitle().wx_str()  : wxString(_T("*NONE*")).wx_str(),
                           m_TokensTree ? static_cast<unsigned long>(m_TokensTree->m_FilesMap.size()) : 0,
                           m_TokensTree ? static_cast<unsigned long>(m_TokensTree->realsize())        : 0,
                           (m_LastStopWatchTime / 60000),
                           (m_LastStopWatchTime / 1000) % 60,
                           (m_LastStopWatchTime % 1000) );

        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)

        ProcessParserEvent(m_ParsingType, ParserCommon::idParserEnd, parseEndLog);
        m_ParsingType = ptUndefined;
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
    if (ParserCommon::s_CurrentParser && ParserCommon::s_CurrentParser != this)
    {
        m_BatchTimer.Start(1000, wxTIMER_ONE_SHOT);
        return;
    }

    bool sendStartParseEvent = false;

    CC_LOCKER_TRACK_P_MTX_LOCK(ParserCommon::s_ParserMutex)

    if (!m_StopWatchRunning)
        StartStopWatch();

    bool send_event = true;
    if (!m_PoolTask.empty())
    {
        m_Pool.BatchBegin();

        PTVector& v = m_PoolTask.front();
        for (PTVector::const_iterator it = v.begin(); it != v.end(); ++it)
            m_Pool.AddTask(*it, true);
        m_PoolTask.pop();

        m_Pool.BatchEnd();
        send_event = false; // error
    }
    else if (   !m_PriorityHeaders.empty()
             || !m_BatchParseFiles.empty()
             || !m_PredefinedMacros.IsEmpty() )
    {
        ParserThreadedTask* thread = new ParserThreadedTask(this, ParserCommon::s_ParserMutex);
        m_Pool.AddTask(thread, true);

        // Have not done any batch parsing
        if (ParserCommon::s_CurrentParser)
            send_event = false; // error
        else
        {
            ParserCommon::s_CurrentParser = this;
            m_StopWatch.Start(); // reset timer
            sendStartParseEvent = true;
        }
    }

    CC_LOCKER_TRACK_P_MTX_UNLOCK(ParserCommon::s_ParserMutex)

    if (send_event)
    {
        if (sendStartParseEvent)
            ProcessParserEvent(m_ParsingType, ParserCommon::idParserStart);
        else
            ProcessParserEvent(ptUndefined, ParserCommon::idParserStart, _T("Unexpected behaviour!"));
    }
}

void Parser::ReparseModifiedFiles()
{
    if (!Done())
    {
        wxString msg(_T("Parser::ReparseModifiedFiles : The Parser is not done."));
        msg += NotDoneReason();
        CCLogger::Get()->DebugLog(msg);
        m_ReparseTimer.Start(ParserCommon::PARSER_REPARSE_TIMER_DELAY, wxTIMER_ONE_SHOT);
        return;
    }

    if (!m_NeedsReparse)
        m_NeedsReparse = true;

    std::queue<wxString> files_list;
    TokenFilesSet::iterator it;

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

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

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)

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

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

    isParsed = m_TokensTree->IsFileParsed(UnixFilename(filename));

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)

    if (!isParsed)
    {
        CC_LOCKER_TRACK_P_MTX_LOCK(ParserCommon::s_ParserMutex)

        StringList::iterator it = std::find(m_BatchParseFiles.begin(), m_BatchParseFiles.end(), filename);
        isParsed = it != m_BatchParseFiles.end();

        CC_LOCKER_TRACK_P_MTX_UNLOCK(ParserCommon::s_ParserMutex)
    }

    return isParsed;
}

void Parser::ProcessParserEvent(ParsingType type, int id, const wxString& info)
{
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, id);
    evt.SetEventObject(this);     // Parser*
    evt.SetClientData(m_Project); // cbProject*
    evt.SetInt(type);
    evt.SetString(info);
    m_Parent->ProcessEvent(evt);
}

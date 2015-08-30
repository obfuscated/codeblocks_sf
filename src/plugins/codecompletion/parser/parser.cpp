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

    // this static variable point to the Parser instance which is currently running the taskpool
    // when the taskpool finishes, the pointer is set to nullptr.
    static volatile Parser* s_CurrentParser = nullptr;

    // NOTE (ollydbg#1#): This static variable is used to prevent changing the member variables of
    // the Parser class from different threads. Basically, It should not be a static wxMutex for all
    // the instances of the Parser class, it should be a member variable of the Parser class.
    // Maybe, the author of this locker (Loaden?) thought that accessing to different Parser instances
    // from different threads should also be avoided.
    static          wxMutex s_ParserMutex;

    int idParserStart = wxNewId();
    int idParserEnd   = wxNewId();

}// namespace ParserCommon

Parser::Parser(wxEvtHandler* parent, cbProject* project) :
    m_Parent(parent),
    m_Project(project),
    m_UsingCache(false),
    m_Pool(this, wxNewId(), 1, 2 * 1024 * 1024), // in the meanwhile it'll have to be forced to 1
    m_IsParsing(false),
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
    ReadOptions();
    ConnectEvents();
}

Parser::~Parser()
{
    // Don't wrap the s_ParserMutex lock around TerminateAllThreads(), since, it will cause a deadlock
    // in TerminateAllThreads() when calling DeleteParser() before parsing has finished.

    DisconnectEvents();
    TerminateAllThreads();

    CC_LOCKER_TRACK_P_MTX_LOCK(ParserCommon::s_ParserMutex)

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

    bool done = m_BatchParseFiles.empty()
                && m_PredefinedMacros.IsEmpty()
                && !m_NeedMarkFileAsLocal
                && m_Pool.Done();

    CC_LOCKER_TRACK_P_MTX_UNLOCK(ParserCommon::s_ParserMutex)

    return done;
}

wxString Parser::NotDoneReason()
{
    CC_LOCKER_TRACK_P_MTX_LOCK(ParserCommon::s_ParserMutex)

    wxString reason = _T(" > Reasons:");
    if (!m_BatchParseFiles.empty())
        reason += _T("\n- still batch parse files to parse");
    if (!m_PredefinedMacros.IsEmpty())
        reason += _T("\n- still pre-defined macros to operate");
    if (m_NeedMarkFileAsLocal)
        reason += _T("\n- still need to mark files as local");
    if (!m_Pool.Done())
        reason += _T("\n- thread pool is not done yet");

    CC_LOCKER_TRACK_P_MTX_UNLOCK(ParserCommon::s_ParserMutex)

    return reason;
}

void Parser::AddPredefinedMacros(const wxString& defs)
{
    if (m_BatchTimer.IsRunning())
    {
        m_BatchTimer.Stop();
        TRACE(_T("Parser::AddPredefinedMacros(): Stop the m_BatchTimer."));
    }

    CC_LOCKER_TRACK_P_MTX_LOCK(ParserCommon::s_ParserMutex)

    m_PredefinedMacros << defs;

    // ptUndefined means at least the cbproject is parsed already, this means the user try to
    // reparse the project, since the predefined macro buffer is only collected when a new Parser
    // is created
    if (m_ParserState == ParserCommon::ptUndefined)
        m_ParserState = ParserCommon::ptCreateParser;

    if (!m_IsParsing)
    {
        TRACE(_T("Parser::AddPredefinedMacros(): Starting m_BatchTimer."));
        m_BatchTimer.Start(ParserCommon::PARSER_BATCHPARSE_TIMER_DELAY, wxTIMER_ONE_SHOT);
    }

    CC_LOCKER_TRACK_P_MTX_UNLOCK(ParserCommon::s_ParserMutex)
}

void Parser::ClearPredefinedMacros()
{
    CC_LOCKER_TRACK_P_MTX_LOCK(ParserCommon::s_ParserMutex)

    m_LastPredefinedMacros = m_PredefinedMacros;
    m_PredefinedMacros.Clear();

    CC_LOCKER_TRACK_P_MTX_UNLOCK(ParserCommon::s_ParserMutex);
}

const wxString Parser::GetPredefinedMacros() const
{
    CCLogger::Get()->DebugLog(_T("Parser::GetPredefinedMacros()"));
    return m_LastPredefinedMacros;
}

void Parser::AddBatchParse(const StringList& filenames)
{
    // this function has the same logic as the previous function Parser::AddPriorityHeader
    // it just add some files to a m_BatchParseFiles, and tick the m_BatchTimer timer.
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
    {
        TRACE(_T("Parser::AddBatchParse(): Starting m_BatchTimer."));
        m_BatchTimer.Start(ParserCommon::PARSER_BATCHPARSE_TIMER_DELAY, wxTIMER_ONE_SHOT);
    }

    CC_LOCKER_TRACK_P_MTX_UNLOCK(ParserCommon::s_ParserMutex)
}

void Parser::AddParse(const wxString& filename)
{
    // similar logic as the Parser::AddBatchParse, but this function only add one file to
    // m_BatchParseFiles member, also it does not change the m_ParserState state.
    if (m_BatchTimer.IsRunning())
        m_BatchTimer.Stop();

    CC_LOCKER_TRACK_P_MTX_LOCK(ParserCommon::s_ParserMutex)

    m_BatchParseFiles.push_back(filename);

    if (!m_IsParsing)
    {
        TRACE(_T("Parser::AddParse(): Starting m_BatchTimer."));
        m_BatchTimer.Start(ParserCommon::PARSER_BATCHPARSE_TIMER_DELAY, wxTIMER_ONE_SHOT);
    }

    CC_LOCKER_TRACK_P_MTX_UNLOCK(ParserCommon::s_ParserMutex)
}

bool Parser::Parse(const wxString& filename, bool isLocal, bool locked)
{
    // most ParserThreadOptions was copied from m_Options
    ParserThreadOptions opts;

    opts.useBuffer             = false;
    opts.bufferSkipBlocks      = false;
    opts.bufferSkipOuterBlocks = false;

    opts.followLocalIncludes   = m_Options.followLocalIncludes;
    opts.followGlobalIncludes  = m_Options.followGlobalIncludes;
    opts.wantPreprocessor      = m_Options.wantPreprocessor;
    opts.parseComplexMacros    = m_Options.parseComplexMacros;
    opts.platformCheck         = m_Options.platformCheck;

    // whether to collect doxygen style documents.
    opts.storeDocumentation    = m_Options.storeDocumentation;

    opts.loader                = nullptr; // must be 0 at this point

    bool result = false;
    // a (false) do while, so we can quickly exit the loop by break statement
    do
    {
        bool canparse = false;
        {
            // check to see whether the filename is already parsed, if not, then we first add
            // it to ReserveFileForParsing
            if (!locked)
                CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

            //check to see whether it is assigned already
            canparse = !m_TokenTree->IsFileParsed(filename);
            if (canparse)
                canparse = m_TokenTree->ReserveFileForParsing(filename, true) != 0;

            if (!locked)
                CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)
        }

        if (!canparse)
        {
            TRACE(_T("Parser::Parse(): file already parsed or reserved for parsing") + filename);
            break;
        }

        // once the Load function is called, it will return a loader pointer, and start loading
        // the file content in a background thread(see: BackgroundThread class)
        // the loader will be deleted in the ParserThread::InitTokenizer() function.
        opts.loader = Manager::Get()->GetFileManager()->Load(filename, m_NeedsReparse);

        // we are going to parse this file, so create a ParserThread
        ParserThread* thread = new ParserThread(this, filename, isLocal, opts, m_TokenTree);
        TRACE(_T("Parser::Parse(): Parsing %s"), filename.wx_str());

        // We now properly parse each source file from top to bottom (i.e., expanding each
        // #include directive: If locked is true, which means this function is called when handling
        // #include directive, the tree is already locked, so recursive here.
        if (locked)
        {

            // release the tree locker, don't block the GUI to access the TokenTree for a long time
            CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)
            wxMilliSleep(1);
            CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

            TRACE(_T("Parser::Parse(): Parsing included header, %s"), filename.wx_str());
            // run the parse recursively
            result = thread->Parse();
            delete thread;
            return true;
        }
        else
        {
            // files should not be parsed immediately, so we need to put it to the pool.
            TRACE(_T("Parser::Parse(): Adding a Parsing job for %s"), filename.wx_str());
            m_Pool.AddTask(thread, true); // autodelete = true
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
    opts.parseComplexMacros   = m_Options.parseComplexMacros;
    opts.platformCheck        = true;

    opts.handleFunctions      = true;   // enabled to support function ptr in local block

    opts.storeDocumentation   = m_Options.storeDocumentation;

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
    opts.platformCheck        = m_Options.platformCheck;

    opts.handleFunctions      = true;

    opts.storeDocumentation   = m_Options.storeDocumentation;

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
    opts.platformCheck        = true;

    opts.storeDocumentation   = m_Options.storeDocumentation;

    ParserThread thread(this, wxEmptyString, true, opts, m_TempTokenTree);

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    bool success = thread.ParseBufferForNamespaces(buffer, result);

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    return success;
}

bool Parser::ParseBufferForUsingNamespace(const wxString& buffer, wxArrayString& result, bool bufferSkipBlocks)
{
    ParserThreadOptions opts;

    opts.useBuffer            = true;

    opts.followLocalIncludes  = false;
    opts.followGlobalIncludes = false;
    opts.wantPreprocessor     = m_Options.wantPreprocessor;
    opts.parseComplexMacros   = false;
    opts.platformCheck        = true;
    opts.bufferSkipBlocks     = bufferSkipBlocks;

    opts.storeDocumentation   = m_Options.storeDocumentation;

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
    // this function will lock the token tree twice
    // the first place is the function IsFileParsed() function
    // then the AddParse() call
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

bool Parser::Reparse(const wxString& filename, cb_unused bool isLocal)
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

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    m_TokenTree->FlagFileForReparsing(filename);

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    m_NeedsReparse = true;
    TRACE(_T("Parser::Reparse(): Starting m_ReparseTimer."));
    m_ReparseTimer.Start(ParserCommon::PARSER_REPARSE_TIMER_DELAY, wxTIMER_ONE_SHOT);

    return true;
}

void Parser::TerminateAllThreads()
{
    // NOTE: This should not be locked with s_ParserMutex, otherwise we'll be stuck in an
    // infinite loop below since the worker thread also enters s_ParserMutex.
    // In fact cbThreadPool maintains it's own mutex, so m_Pool is probably threadsafe.
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
        wxString msg(_T("Parser::UpdateParsingProject(): The Parser is not done."));
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
    // m_IgnoreThreadEvents is initialized to true, so we returned quickly.
    // but when the parser try to parse the batchFiles, it will set the m_IgnoreThreadEvents to false
    // so, the control will go forward. Finally, when the last stage: mark C::B project tokens as
    // local thread finished, it will set m_IgnoreThreadEvents again
    if (m_IgnoreThreadEvents || Manager::IsAppShuttingDown())
        return;

    if (event.GetId() != m_Pool.GetId())
    {
        CCLogger::Get()->DebugLog(_T("Parser::OnAllThreadsDone(): Why is event.GetId() not equal m_Pool.GetId()?"));
        return;
    }

    if (!m_TokenTree)
        cbThrow(_T("m_TokenTree is a nullptr?!"));

    if (!m_IsParsing)
    {
        CCLogger::Get()->DebugLog(_T("Parser::OnAllThreadsDone(): Why is m_IsParsing false?"));
        return;
    }

    // Do next task
    if (!m_PredefinedMacros.IsEmpty()
        || !m_BatchParseFiles.empty() )
    {
        TRACE(_T("Parser::OnAllThreadsDone(): Still some tasks left, starting m_BatchTimer."));
        m_BatchTimer.Start(ParserCommon::PARSER_BATCHPARSE_TIMER_RUN_IMMEDIATELY, wxTIMER_ONE_SHOT);
    }
#if defined(CC_PARSER_PROFILE_TEST)
    // Do nothing
#else
    else if (   (   m_ParserState == ParserCommon::ptCreateParser
                 || m_ParserState == ParserCommon::ptAddFileToParser )
             && m_NeedMarkFileAsLocal
             && m_Project)
    {
        m_NeedMarkFileAsLocal = false;
        MarkFileAsLocalThreadedTask* thread = new MarkFileAsLocalThreadedTask(this, m_Project);
        m_Pool.AddTask(thread, true);
        TRACE(_T("Parser::OnAllThreadsDone(): Add a MarkFileAsLocalThreadedTask."));
    }
#endif
    // Finish all task, then we need post a PARSER_END event
    else
    {
        if (!m_Project)
            m_NeedMarkFileAsLocal = false;

        // since the last stage: mark project files as local is done, we finish all the stages now.
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

        // tell the parent(native parser and the code completion plugin) that some tasks are done
        // and the task pool switches to idle mode.
        ProcessParserEvent(m_ParserState, ParserCommon::idParserEnd, parseEndLog);

        // reset the parser state
        m_ParserState = ParserCommon::ptUndefined;

        // the current parser is not parsing any files, so set the static pointer to NULL
        ParserCommon::s_CurrentParser = nullptr;
        TRACE(_T("Parser::OnAllThreadsDone(): Post a PARSER_END event"));
    }
}

bool Parser::ParseFile(const wxString& filename, bool isGlobal, bool locked)
{
    if (   (!isGlobal && !m_Options.followLocalIncludes)
        || ( isGlobal && !m_Options.followGlobalIncludes) )
        return false;

    if (filename.IsEmpty())
        return false;

    // TODO (Morten#9#) locker ?
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
        TRACE(_T("Parser::OnBatchTimer(): Starting m_BatchTimer."));
        m_BatchTimer.Start(ParserCommon::PARSER_BATCHPARSE_TIMER_DELAY_LONG, wxTIMER_ONE_SHOT);
        return;
    }

    StartStopWatch(); // start counting the time we take for parsing the files

    if (m_BatchParseFiles.empty()
        && m_PredefinedMacros.IsEmpty() ) // easy case: is there any thing to do at all?
    {
        return;
    }

    bool send_event          = true;
    bool sendStartParseEvent = false;
    if (   !m_BatchParseFiles.empty()
        || !m_PredefinedMacros.IsEmpty() )
    {
        CC_LOCKER_TRACK_P_MTX_LOCK(ParserCommon::s_ParserMutex)

        ParserThreadedTask* thread = new ParserThreadedTask(this, ParserCommon::s_ParserMutex);
        TRACE(_T("Parser::OnBatchTimer(): Adding a ParserThreadedTask thread to m_Pool."));
        m_Pool.AddTask(thread, true); //once this function is called, the thread will be executed from the pool.

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

        TRACE(_T("Parser::ReparseModifiedFiles(): Starting m_ReparseTimer."));
        m_ReparseTimer.Start(ParserCommon::PARSER_REPARSE_TIMER_DELAY, wxTIMER_ONE_SHOT);
        return;
    }

    if (!m_NeedsReparse)
        m_NeedsReparse = true;

    std::queue<size_t>   files_idx;
    std::queue<wxString> files_list;
    TokenFileSet::const_iterator it;

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    // Collect files to be re-parsed
    // Loop two times so that we reparse modified *header* files first, next *implementation* files
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

    // Now actually remove the files from the tree, once a file is removed from the tree, the file
    // and its tokens are totally removed
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
        // add those files again, so they will be parsed later
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
        cfg->Write(_T("/platform_check"),                true);
    }

    // Page "Code Completion"
    m_Options.useSmartSense        = cfg->ReadBool(_T("/use_SmartSense"),                true);
    m_Options.whileTyping          = cfg->ReadBool(_T("/while_typing"),                  true);

    // the m_Options.caseSensitive is following the global option in ccmanager
    // ccmcfg means ccmanager's config
    ConfigManager* ccmcfg = Manager::Get()->GetConfigManager(_T("ccmanager"));
    m_Options.caseSensitive        = ccmcfg->ReadBool(_T("/case_sensitive"),             false);

    // Page "C / C++ parser"
    m_Options.followLocalIncludes  = cfg->ReadBool(_T("/parser_follow_local_includes"),  true);
    m_Options.followGlobalIncludes = cfg->ReadBool(_T("/parser_follow_global_includes"), true);
    m_Options.wantPreprocessor     = cfg->ReadBool(_T("/want_preprocessor"),             true);
    m_Options.parseComplexMacros   = cfg->ReadBool(_T("/parse_complex_macros"),          true);
    m_Options.platformCheck        = cfg->ReadBool(_T("/platform_check"),                true);

    // Page "Symbol browser"
    m_BrowserOptions.showInheritance = cfg->ReadBool(_T("/browser_show_inheritance"),    false);
    m_BrowserOptions.expandNS        = cfg->ReadBool(_T("/browser_expand_ns"),           false);
    m_BrowserOptions.treeMembers     = cfg->ReadBool(_T("/browser_tree_members"),        true);

    // Token tree
    m_BrowserOptions.displayFilter   = (BrowserDisplayFilter)cfg->ReadInt(_T("/browser_display_filter"), bdfFile);
    m_BrowserOptions.sortType        = (BrowserSortType)cfg->ReadInt(_T("/browser_sort_type"),           bstKind);

    // Page "Documentation:
    m_Options.storeDocumentation     = cfg->ReadBool(_T("/use_documentation_helper"),         false);

    // force re-read of file types
    ParserCommon::EFileType ft_dummy = ParserCommon::FileType(wxEmptyString, true);
    wxUnusedVar(ft_dummy);
}

void Parser::WriteOptions()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("code_completion"));

    // Page "Code Completion"
    cfg->Write(_T("/use_SmartSense"),                m_Options.useSmartSense);
    cfg->Write(_T("/while_typing"),                  m_Options.whileTyping);

    // Page "C / C++ parser"
    cfg->Write(_T("/parser_follow_local_includes"),  m_Options.followLocalIncludes);
    cfg->Write(_T("/parser_follow_global_includes"), m_Options.followGlobalIncludes);
    cfg->Write(_T("/want_preprocessor"),             m_Options.wantPreprocessor);
    cfg->Write(_T("/parse_complex_macros"),          m_Options.parseComplexMacros);
    cfg->Write(_T("/platform_check"),                m_Options.platformCheck);

    // Page "Symbol browser"
    cfg->Write(_T("/browser_show_inheritance"),      m_BrowserOptions.showInheritance);
    cfg->Write(_T("/browser_expand_ns"),             m_BrowserOptions.expandNS);
    cfg->Write(_T("/browser_tree_members"),          m_BrowserOptions.treeMembers);

    // Token tree
    cfg->Write(_T("/browser_display_filter"),        m_BrowserOptions.displayFilter);
    cfg->Write(_T("/browser_sort_type"),             m_BrowserOptions.sortType);

    // Page "Documentation:
    // m_Options.storeDocumentation will be written by DocumentationPopup
}

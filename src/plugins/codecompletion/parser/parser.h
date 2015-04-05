/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef PARSER_H
#define PARSER_H

#include <wx/arrstr.h>
#include <wx/event.h>
#include <wx/file.h>
#include <wx/filefn.h> // wxPathList
#include <wx/imaglist.h>
#include <wx/string.h>
#include <wx/thread.h>
#include <wx/timer.h>
#include <wx/treectrl.h>

#include <cbthreadpool.h>
#include <sdk_events.h>

#include "parserthread.h"
#include "parser_base.h"

#define PARSER_IMG_NONE                        -2
#define PARSER_IMG_CLASS_FOLDER                 0
#define PARSER_IMG_CLASS                        1
#define PARSER_IMG_CLASS_PRIVATE                2
#define PARSER_IMG_CLASS_PROTECTED              3
#define PARSER_IMG_CLASS_PUBLIC                 4
#define PARSER_IMG_CTOR_PRIVATE                 5
#define PARSER_IMG_CTOR_PROTECTED               6
#define PARSER_IMG_CTOR_PUBLIC                  7
#define PARSER_IMG_DTOR_PRIVATE                 8
#define PARSER_IMG_DTOR_PROTECTED               9
#define PARSER_IMG_DTOR_PUBLIC                  10
#define PARSER_IMG_FUNC_PRIVATE                 11
#define PARSER_IMG_FUNC_PROTECTED               12
#define PARSER_IMG_FUNC_PUBLIC                  13
#define PARSER_IMG_VAR_PRIVATE                  14
#define PARSER_IMG_VAR_PROTECTED                15
#define PARSER_IMG_VAR_PUBLIC                   16
#define PARSER_IMG_MACRO_DEF                    17
#define PARSER_IMG_ENUM                         18
#define PARSER_IMG_ENUM_PRIVATE                 19
#define PARSER_IMG_ENUM_PROTECTED               20
#define PARSER_IMG_ENUM_PUBLIC                  21
#define PARSER_IMG_ENUMERATOR                   22
#define PARSER_IMG_NAMESPACE                    23
#define PARSER_IMG_TYPEDEF                      24
#define PARSER_IMG_TYPEDEF_PRIVATE              25
#define PARSER_IMG_TYPEDEF_PROTECTED            26
#define PARSER_IMG_TYPEDEF_PUBLIC               27
#define PARSER_IMG_SYMBOLS_FOLDER               28
#define PARSER_IMG_VARS_FOLDER                  29
#define PARSER_IMG_FUNCS_FOLDER                 30
#define PARSER_IMG_ENUMS_FOLDER                 31
#define PARSER_IMG_MACRO_DEF_FOLDER             32
#define PARSER_IMG_OTHERS_FOLDER                33
#define PARSER_IMG_TYPEDEF_FOLDER               34
#define PARSER_IMG_MACRO_USE                    35
#define PARSER_IMG_MACRO_USE_PRIVATE            36
#define PARSER_IMG_MACRO_USE_PROTECTED          37
#define PARSER_IMG_MACRO_USE_PUBLIC             38
#define PARSER_IMG_MACRO_USE_FOLDER             39

#define PARSER_IMG_MIN PARSER_IMG_CLASS_FOLDER
#define PARSER_IMG_MAX PARSER_IMG_MACRO_USE_FOLDER

/** Tree data associate with the symbol tree item */
class ClassTreeData : public wxTreeItemData
{
public:
    ClassTreeData(Token* token)   { m_Token = token; }
    Token* GetToken()             { return m_Token;  }
    void   SetToken(Token* token) { m_Token = token; }
private:
    Token* m_Token;
};

class ClassBrowser;

namespace ParserCommon
{
    extern int idParserStart;
    extern int idParserEnd;

    enum ParserState
    {
        /** the Parser object is newly created, and we are parsing the predefined macro buffer, the
         * source files, and finally mark the project's tokens as local
         */
        ptCreateParser    = 1,

        /** some files are changed by the user, so we are parsing the changed files */
        ptReparseFile     = 2,

        /** the user has add some files to the cbproject, so we are parsing the new added files */
        ptAddFileToParser = 3,

        /** non of the above three status, this means our Parser has finish all the jobs, and it is
         * in idle mode
         */
        ptUndefined       = 4
    };
}

/** @brief Parser class holds all the tokens of a C::B project
  *
  * Parser class contains the TokenTree which is a trie structure to record the token information.
  * For details about trie, see http://en.wikipedia.org/wiki/Trie
  * The parser class manages Parser threaded Tasks in a ThreadPool. A ParserThread object is
  * associated with a single source file.
  * Batch parse mode means we have a lot of files to be parsed, so a lot of ParserThreads were
  * generated and added to the ThreadPool, and finally, the ParserThread was executed by ThreadPool.
  */
class Parser : public ParserBase
{
    friend class ParserThreadedTask;

public:
    /** constructor
     * @param parent which is actually a NativeParser object
     * @param project the C::B project associated with the current Parser
     */
    Parser(wxEvtHandler* parent, cbProject* project);
    /** destructor */
    virtual ~Parser();

    /** Add files to batch parse mode, internally. The files will be parsed sequentially.
     * @param filenames input files name array
     */
    virtual void AddBatchParse(const StringList& filenames);

    /** Add one file to Batch mode Parsing, this is the bridge between the main thread and the
     * thread pool, after this function call, the file(Parserthread) will be run from the thread
     * pool.
     * @param filenames input file name
     */
    virtual void AddParse(const wxString& filename);

    /** the predefined macro definition string was collected from the GCC command line, this function
     *  add the string to an internal m_PredefinedMacros, and switch the ParserState
     */
    virtual void AddPredefinedMacros(const wxString& defs);

    /** set the associated C::B project pointer. (only used by one parser for whole workspace)
     *  @return true if it can do the switch, other wise, return false, and print some debug logs.
     */
    virtual bool UpdateParsingProject(cbProject* project);

    /** Must add a locker before call all named ParseBufferXXX functions, ParseBuffer function will
     * directly run the parsing in the same thread as the caller. So, take care if the time is limited.
     * this function usually used to parse the function body to fetch the local variable information.
     */
    virtual bool ParseBuffer(const wxString& buffer, bool isLocal, bool bufferSkipBlocks = false,
                             bool isTemp = false, const wxString& filename = wxEmptyString,
                             int parentIdx = -1, int initLine = 0);

    /** parser the current editor control, this function is used to list all the functions in the
     * current code editor
     */
    virtual bool ParseBufferForFunctions(const wxString& buffer);

    /** parse the buffer for collecting exposed namespace scopes*/
    virtual bool ParseBufferForNamespaces(const wxString& buffer, NameSpaceVec& result);

    /** parse the buffer for collecting using namespace directive*/
    virtual bool ParseBufferForUsingNamespace(const wxString& buffer, wxArrayString& result, bool bufferSkipBlocks = true);

    /** mark this file to be re-parsed in the TokenTree, tick the reparse timer, note it looks like
     * the isLocal parameter is not used in Parser::Reparse function.
     * A better function name could be: MarkFileNeedToBeReParsed()
     */
    virtual bool Reparse(const wxString& filename, bool isLocal = true);

    /** this usually happens when user adds some files to an existing project, it just use AddParse()
     * function internally to add the file. and switch the ParserState to ParserCommon::ptAddFileToParser.
     */
    virtual bool AddFile(const wxString& filename, cbProject* project, bool isLocal = true);

    /** this usually happens when the user removes a file from the existing project, it will remove
     * all the tokens belong to the file.
     */
    virtual bool RemoveFile(const wxString& filename);

    /** check to see a file is parsed already, it first check the TokenTree to see whether it has
     * the specified file, but if a file is already queued (put in m_BatchParseFiles), we regard it
     * as already parsed.
     */
    virtual bool IsFileParsed(const wxString& filename);

    /** check to see whether Parser is in Idle mode, there is no work need to be done in the Parser*/
    virtual bool     Done();

    /** if the Parser is not in Idle mode, show which need to be done */
    virtual wxString NotDoneReason();

protected:
    // used for measuring the batch parsing time
    void StartStopWatch();
    // used for measuring the batch parsing time
    void EndStopWatch();

    /** Node: Currently, the max. concurrent ParserThread number should be ONE, CC does not support
     * multiply threads parsing.
     */
    unsigned int GetMaxThreads() const { return m_Pool.GetConcurrentThreads(); }

    /** Not used, because the ThreadPool only support running ONE ParserThread concurrently */
    void SetMaxThreads(unsigned int max) { m_Pool.SetConcurrentThreads(max); }

    /** parse the file, either immediately or delayed.
     * @param isLocal true if the file belong to a C::B project
     * @param locked give the status of the Tokentree, false means the tree is not locked
     */
    bool Parse(const wxString& filename, bool isLocal = true, bool locked = false);

    /** delete those files from the TokenTree, and add them again through AddParse() function */
    void ReparseModifiedFiles();

    /** cancel all the tasks in the thread pool m_Pool*/
    void TerminateAllThreads();

    /** When a ThreadPool batch parse stage is done, it will issue a cbEVT_THREADTASK_ALLDONE message.
     * In some situations this event will be triggered, such as:
     * - batch parsing for cpp source files
     * - mark tokens belong to the cb project as local tokens
     */
    void OnAllThreadsDone(CodeBlocksEvent& event);

    /** some files in the Tokentree is marked as need to be reparsed, this can be done by a call
     * of Reparse() before. So, in this timer event handler, we need to remove all the tokens of
     * files in the Tree, and then re-parse them again. This is done by AddParse() again. the Parser
     * status now switch to ParserCommon::ptReparseFile.
     */
    void OnReparseTimer(wxTimerEvent& event);

    /** A timer is used to optimized the event handling for parsing, e.g. several files/projects
     * were added to the project, so we don't start the real parsing stage until the last
     * file/project was added,
     */
    void OnBatchTimer(wxTimerEvent& event);

    /** The parser will let its parent (NativeParser) to handle the event, as the CodeCompletion instance
     * was set as the next handler of the NativeParser. Those events can finally go to the CodeCompletion's
     * event handler.
     * @param state the state of the Parser, it could be any kind of enum ParserState
     * @param id either idParserStart or idParserEnd
     * @param info the log message
     */
    void ProcessParserEvent(ParserCommon::ParserState state, int id, const wxString& info = wxEmptyString);

    /** read Parser options from configure file */
    virtual void            ReadOptions();
    /** write Parse options to configure file */
    virtual void            WriteOptions();

private:
    /** the only usage of this function is in the Parserthread class, when handling include directives
     * the parserthread use some call like m_Parent->ParseFile() to call this function, but this function
     * just call Parser::Parse() function, which either run the syntax analysis immediately or create
     * a parsing task in the Pool.
     * @param filename the file we want to parse
     * @param isGlobal true if the file is not belong to a C::B project
     * @param locked true if the TokenTree is locked. when initially parse a translation unit file
     * the locked should be set as false, but if you want to recursive parse to an include file
     * the locked value should be set as true.
     */
    virtual bool ParseFile(const wxString& filename, bool isGlobal, bool locked = false);

    /** connect event handlers of the timers and thread pool */
    void ConnectEvents();

    /** connect event handlers of the timers and thread pool */
    void DisconnectEvents();

    /** when initialized, this variable will be an instance of a NativeParser */
    wxEvtHandler*             m_Parent;

    /** referring to the C::B cbp project currently parsing in one parser per workspace mode */
    cbProject*                m_Project;

protected:
    /** used to detect changes between in-memory data and cache, true if loaded from cache */
    bool                      m_UsingCache;

    /** Thread Pool, executing all the ParserThread, used in batch parse mode. The thread pool can
     * add/remove/execute the ParserThread tasks, it will also notify the Parser that all the thread
     * are done.
     */
    cbThreadPool              m_Pool;

    /** true, if the parser is still busy with parsing, false if the parsing stage has finished
     * this value is set in parserthreadedtask after putting all the batchFiles to pool(task)
     * it was reset after the last stage (mark tokens as local)
     */
    bool                      m_IsParsing;


    /** Indicates some files in the current project need to be re-parsed, this is commonly caused
      * that the "real-time parsing option" is enabled, and user is editing source file.
      */
    bool                      m_NeedsReparse;

    /** batch Parse mode flag. It was set after consuming m_PredefinedMacros, it was reset after the
     * final stage (mark token as local).
     */
    bool                      m_IsFirstBatch;

private:

    /** a file is need to be reparsed, maybe another file will to be reparsed very soon, so use
     * a timer to collect all the files need to be reparsed. This avoid starting running the thread
     * pool to quickly
     */
    wxTimer                   m_ReparseTimer;

    /** a timer to delay the operation of batch parsing, see OnBatchTimer() member function as a
     * reference
     */
    wxTimer                   m_BatchTimer;

    /** a stop watch to measure parsing time*/
    wxStopWatch               m_StopWatch;
    bool                      m_StopWatchRunning;
    long                      m_LastStopWatchTime;

    /** Parser::OnAllThreadsDone will be called when m_Pool finished its job, but when we run a
     * batch parsing, we may receive several such event from the m_Pool, because
     * 1, when ParserThreadedTask finished
     * 2, when batchFiles get finished
     * 3, mark C::B project files's token as local
     */
    bool                      m_IgnoreThreadEvents;

    /** All other batch parse files, like the normal headers/sources */
    StringList                m_BatchParseFiles;

    /** Pre-defined macros, its a buffer queried from the compiler command line */
    wxString                  m_PredefinedMacros;

    /** used to measure batch parse time*/
    bool                      m_IsBatchParseDone;

    /** indicated the current state the parser */
    ParserCommon::ParserState m_ParserState;

    /** if true, all the files of the current project will be labeled as "local" */
    bool                      m_NeedMarkFileAsLocal;
};

#endif // PARSER_H

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

#include "parserthread.h"

#include <cbthreadpool.h>
#include <sdk_events.h>

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

typedef std::set<wxString>  StringSet;
typedef std::list<wxString> StringList;

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

/** Setting of the Parser, some of them will be passed down to ParserThreadOptions */
struct ParserOptions
{
    bool followLocalIncludes;  /// parse XXX.h in directive #include "XXX.h"
    bool followGlobalIncludes; /// parse XXX.h in directive #include <XXX.h>
    bool caseSensitive;        /// case sensitive in MarkItemsByAI
    bool wantPreprocessor;     /// handle preprocessor directive in Tokenizer class
    bool useSmartSense;        /// use real AI(scope sequence match) or not(plain text match)
    bool whileTyping;          /// reparse the active editor while editing
    bool parseComplexMacros;   /// this will let the Tokenizer to recursive expand macros
    bool storeDocumentation;   /// should tokenizer detect and store doxygen documentation?
};

/** specify the scope of the shown symbols */
enum BrowserDisplayFilter
{
    bdfFile = 0,  /// display symbols of current file
    bdfProject,   /// display symbols of current project
    bdfWorkspace, /// display symbols of current workspace
    bdfEverything /// display every symbols
};

/** specify the sort order of the symbol tree nodes */
enum BrowserSortType
{
    bstAlphabet = 0, /// alphabetical
    bstKind,         /// class, function, macros
    bstScope,        /// public, protected, private
    bstLine,         /// code like order
    bstNone
};

struct BrowserOptions
{
    bool                 showInheritance; //!< default: false
    bool                 expandNS;        //!< default: false (auto-expand namespaces)
    bool                 treeMembers;     //
    BrowserDisplayFilter displayFilter;   //!< default: bdfFile
    BrowserSortType      sortType;        //
};

class ClassBrowser;

namespace ParserCommon
{
    extern int idParserStart;
    extern int idParserEnd;

    enum ParserState
    {
        ptCreateParser    = 1,
        ptReparseFile     = 2,
        ptAddFileToParser = 3,
        ptUndefined       = 4
    };

    enum EFileType
    {
        ftHeader,
        ftSource,
        ftOther
    };
    EFileType FileType(const wxString& filename, bool force_refresh = false);
}// namespace ParserCommon


// both the CodeCompletion plugin and the cc_test project share this class definition
// but they use different cpp files, the former use parser.cpp and the later use parserdummy.cpp
// parserdummy.cpp just implement a simplified ParserBase and Parser class used for testing only.
class ParserBase : public wxEvtHandler
{
    friend class ParserThread;

public:
    ParserBase();
    virtual ~ParserBase();

    virtual void AddBatchParse(cb_unused const StringList& filenames)                                   { ; }
    virtual void AddParse(cb_unused const wxString& filename)                                           { ; }
    virtual void AddPredefinedMacros(cb_unused const wxString& defs)                                    { ; }
    virtual bool UpdateParsingProject(cb_unused cbProject* project)                                     { return false; }

    virtual bool ParseBuffer(cb_unused const wxString& buffer, cb_unused bool isLocal, cb_unused bool bufferSkipBlocks = false,
                             cb_unused bool isTemp = false, cb_unused const wxString& filename = wxEmptyString,
                             cb_unused int parentIdx = -1, cb_unused int initLine = 0)                      { return false; }
    virtual bool ParseBufferForFunctions(cb_unused const wxString& buffer)                                  { return false; }
    virtual bool ParseBufferForNamespaces(cb_unused const wxString& buffer, cb_unused NameSpaceVec& result) { return false; }
    virtual bool ParseBufferForUsingNamespace(cb_unused const wxString& buffer, cb_unused wxArrayString& result,
                                              cb_unused bool bufferSkipBlocks = true)                       { return false; }

    virtual bool Reparse(cb_unused const wxString& filename, cb_unused bool isLocal = true);     // allow other implementations of derived (dummy) classes
    virtual bool AddFile(cb_unused const wxString& filename, cb_unused cbProject* project, cb_unused bool isLocal = true) { return false; }
    virtual bool RemoveFile(cb_unused const wxString& filename)                                                           { return false; }
    virtual bool IsFileParsed(cb_unused const wxString& filename)                                                         { return false; }

    virtual bool     Done()          { return true; }
    virtual wxString NotDoneReason() { return wxEmptyString; }

    virtual TokenTree* GetTokenTree(); // allow other implementations of derived (dummy) classes
    TokenTree* GetTempTokenTree()    { return m_TempTokenTree; }

    /** add a directory to the Parser's include path database */
    void                 AddIncludeDir(const wxString& dir);
    const wxArrayString& GetIncludeDirs() const { return m_IncludeDirs; }
    wxString             GetFullFileName(const wxString& src, const wxString& tgt, bool isGlobal);
    /** it mimic what a compiler try to find an include header files, if the firstonly option is
     * true, it will return the first found header file, otherwise, all the Parser's include path database
     * will be searched.
     */
    wxArrayString   FindFileInIncludeDirs(const wxString& file, bool firstonly = false);
    /** read Parser options from configure file */
    void            ReadOptions();
    /** write Parse options to configure file */
    void            WriteOptions();

    ParserOptions&  Options()             { return m_Options;        }
    BrowserOptions& ClassBrowserOptions() { return m_BrowserOptions; }

    size_t FindTokensInFile(const wxString& filename, TokenIdxSet& result, short int kindMask);

private:
    virtual bool ParseFile(const wxString& filename, bool isGlobal, bool locked = false);
    wxString FindFirstFileInIncludeDirs(const wxString& file);

protected:
    /** each Parser class contains a TokenTree object which used to record tokens per project
      * this tree will be created in the constructor and destroyed in destructor.
      */
    TokenTree*           m_TokenTree;

    /** a temp Token tree hold some temporary tokens, e.g. parsing a buffer containing some
      * preprocessor directives, see ParseBufferForFunctions() like functions
      * this tree will be created in the constructor and destroyed in destructor.
      */
    TokenTree*           m_TempTokenTree;

    /** options for how the parser try to parse files */
    ParserOptions        m_Options;

    /** options for how the symbol browser was shown */
    BrowserOptions       m_BrowserOptions;

private:
    /** wxString -> wxString map*/
    SearchTree<wxString> m_GlobalIncludes;

    /** the include directories can be either three kinds below:
     * 1, compiler's default search paths, e.g. E:\gcc\include
     * 2, your project's common folder, e.g. the folder where you put the cbp file in
     * 3, the compiler include search paths defined in the cbp, like: E:\wx2.8\msw\include
     */
    wxArrayString        m_IncludeDirs;
};

/** @brief Parser class holds all the tokens of a C::B project
  *
  * Parser class contains the TokenTree which is a trie structure to record the token information.
  * For details about trie, see http://en.wikipedia.org/wiki/Trie
  * The parser class manages ParserThreads in a ThreadPool. A ParserThread object is associated with a single
  * source file.
  * Batch parse mode means we have a lot of files to be parsed, so a lot of ParserThreads were generated and
  * added to the ThreadPool, and finally, the ParserThread was executed by ThreadPool.
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
     * Note that when some "#include" files were added to the batch parse,
     * their parsing sequence may be random.
     * @param filenames input files name array
     */
    virtual void AddBatchParse(const StringList& filenames);

    /** Add one file to Batch mode Parsing
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
     * @param locked give the status of the Tokentree, false means the tree is not locked
     * @param loader is the object to load the file to internally buffer (lower down to Tokenizer)
     */
    bool Parse(const wxString& filename, bool isLocal = true, bool locked = false, LoaderBase* loader = NULL);

    /** delete those files from the TokenTree, and add them again thought AddParse() function */
    void ReparseModifiedFiles();

    /** remove all the queued tasks in m_PoolTask and cancel all the tasks in m_Pool*/
    void TerminateAllThreads();

    /** When a ThreadPool batch parse stage is done, it will issue a cbEVT_THREADTASK_ALLDONE message.
     * In some situations this event will be triggered, such as:
     * - after "Priority" header parsing
     * - batch parsing for general (normal) source files
     * - system header files parsing
     */
    void OnAllThreadsDone(CodeBlocksEvent& event);

    /** some files in the Tokentree is marked as need to be reparsed, this can be done by a call
     * of Reparse() before. So, in this timer event handler, we need to remove all the tokens of
     * files in the Tree, and then re-parse them again. This is done by AddParse() again. the Parser
     * status now switch to ParserCommon::ptReparseFile.
     */
    void OnReparseTimer(wxTimerEvent& event);

    /** A timer is used to optimized the event handling for parsing, e.g. several files/projects were added
     * the the project, so we don't start the real parsing stage until the last file/project was added,
     */
    void OnBatchTimer(wxTimerEvent& event);

    /** The parser will let its parent (NativeParser) to handle the event, as the CodeCompletion instance
     * was set as the next handler of the NativeParser. Those events can finally go to the CodeCompletion's
     * event handler.
     */
    void ProcessParserEvent(ParserCommon::ParserState state, int id, const wxString& info = wxEmptyString);

private:
    /** the only usage of this function is in the Parserthread class, when handling include directives
     * the parserthread use some call like m_Parent->ParseFile() to call this function, but this function
     * just call Parser::Parse() function, which either run the syntax analysis immediately or create
     * a parsing task in the Pool.
     */
    virtual bool ParseFile(const wxString& filename, bool isGlobal, bool locked = false);
    void ConnectEvents();
    void DisconnectEvents();
    /** when initialized, this variable will be an instance of a NativeParser */
    wxEvtHandler*             m_Parent;
    /** referring to the C::B cbp project currently parsing in one parser per workspace mode*/
    cbProject*                m_Project;

protected:
    // the following three members are used to detect changes between
    // in-memory data and cache
    bool                      m_UsingCache; //!< true if loaded from cache

    /** Thread queue, these thread tasks will be executed in FIFO mode as they are added, normally
     * those ParserThread put in the the m_PoolTask will NOT run immediately, they are just tasks
     * ready to be put in the m_Pool(see below), if will finally executed in the m_Pool.
     */
    typedef std::vector<ParserThread*> PTVector;
    std::queue<PTVector>      m_PoolTask;

    /** Thread Pool, executing all the ParserThread, used in batch parse mode. The thread pool can
     * add/remove/execute the ParserThread tasks, it will also notify the Parser that all the thread
     * was done.
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
    wxTimer                   m_ReparseTimer;
    /** a timer to delay the operation of batch parsing, see OnBatchTimer() member function as a reference*/
    wxTimer                   m_BatchTimer;
    /** a stop watch to measure parsing time*/
    wxStopWatch               m_StopWatch;
    bool                      m_StopWatchRunning;
    long                      m_LastStopWatchTime;
    /** Parser::OnAllThreadsDone will be called when m_Pool finished its job, but when we run a
     * batch parsing, we may receive several such event from the m_Pool, because
     * 1, when ParserThreadedTask finished
     * 2, when batchFiles get finished
     * 3, parse system header file again
     * 4, mark C::B project files's token as local
     */
    bool                      m_IgnoreThreadEvents;

    StringList                m_BatchParseFiles;       //!< All other batch parse files, like the normal headers/sources
    wxString                  m_PredefinedMacros;      //!< Pre-defined macros, its a buffer queried from the compiler command line
    /** used to measure batch parse time*/
    bool                      m_IsBatchParseDone;
    ParserCommon::ParserState m_ParserState;           //!< indicated the current state the parser is in
    /** if true, all the files of the current project will be labeled as "local" */
    bool                      m_NeedMarkFileAsLocal;
};

#endif // PARSER_H

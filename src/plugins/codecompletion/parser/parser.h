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

#define PARSER_IMG_NONE              -2
#define PARSER_IMG_CLASS_FOLDER       0
#define PARSER_IMG_CLASS              1
#define PARSER_IMG_CLASS_PRIVATE      2
#define PARSER_IMG_CLASS_PROTECTED    3
#define PARSER_IMG_CLASS_PUBLIC       4
#define PARSER_IMG_CTOR_PRIVATE       5
#define PARSER_IMG_CTOR_PROTECTED     6
#define PARSER_IMG_CTOR_PUBLIC        7
#define PARSER_IMG_DTOR_PRIVATE       8
#define PARSER_IMG_DTOR_PROTECTED     9
#define PARSER_IMG_DTOR_PUBLIC        10
#define PARSER_IMG_FUNC_PRIVATE       11
#define PARSER_IMG_FUNC_PROTECTED     12
#define PARSER_IMG_FUNC_PUBLIC        13
#define PARSER_IMG_VAR_PRIVATE        14
#define PARSER_IMG_VAR_PROTECTED      15
#define PARSER_IMG_VAR_PUBLIC         16
#define PARSER_IMG_PREPROCESSOR       17
#define PARSER_IMG_ENUM               18
#define PARSER_IMG_ENUM_PRIVATE       19
#define PARSER_IMG_ENUM_PROTECTED     20
#define PARSER_IMG_ENUM_PUBLIC        21
#define PARSER_IMG_ENUMERATOR         22
#define PARSER_IMG_NAMESPACE          23
#define PARSER_IMG_TYPEDEF            24
#define PARSER_IMG_TYPEDEF_PRIVATE    25
#define PARSER_IMG_TYPEDEF_PROTECTED  26
#define PARSER_IMG_TYPEDEF_PUBLIC     27
#define PARSER_IMG_SYMBOLS_FOLDER     28
#define PARSER_IMG_VARS_FOLDER        29
#define PARSER_IMG_FUNCS_FOLDER       30
#define PARSER_IMG_ENUMS_FOLDER       31
#define PARSER_IMG_PREPROC_FOLDER     32
#define PARSER_IMG_OTHERS_FOLDER      33
#define PARSER_IMG_TYPEDEF_FOLDER     34
#define PARSER_IMG_MACRO              35
#define PARSER_IMG_MACRO_PRIVATE      36
#define PARSER_IMG_MACRO_PROTECTED    37
#define PARSER_IMG_MACRO_PUBLIC       38
#define PARSER_IMG_MACRO_FOLDER       39

#define PARSER_IMG_MIN PARSER_IMG_CLASS_FOLDER
#define PARSER_IMG_MAX PARSER_IMG_MACRO_FOLDER

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
        ptUndefined       = 4,
    };

    enum EFileType
    {
        ftHeader,
        ftSource,
        ftOther
    };
    EFileType FileType(const wxString& filename, bool force_refresh = false);
}// namespace ParserCommon

class ParserBase : public wxEvtHandler
{
    friend class ParserThread;

public:
    ParserBase();
    virtual ~ParserBase();

    virtual void AddPriorityHeaders(cb_unused const wxString& filename, cb_unused bool systemHeaderFile) { ; }
    virtual void AddBatchParse(cb_unused const StringList& filenames)                                    { ; }
    virtual void AddParse(cb_unused const wxString& filename)                                            { ; }
    virtual void AddPredefinedMacros(cb_unused const wxString& defs)                                     { ; }
    virtual bool UpdateParsingProject(cb_unused cbProject* project)                                      { return false; }

    virtual bool ParseBuffer(cb_unused const wxString& buffer, cb_unused bool isLocal, cb_unused bool bufferSkipBlocks = false,
                             cb_unused bool isTemp = false, cb_unused const wxString& filename = wxEmptyString,
                             cb_unused int parentIdx = -1, cb_unused int initLine = 0)                          { return false; }
    virtual bool ParseBufferForFunctions(cb_unused const wxString& buffer)                                      { return false; }
    virtual bool ParseBufferForNamespaces(cb_unused const wxString& buffer, cb_unused NameSpaceVec& result)     { return false; }
    virtual bool ParseBufferForUsingNamespace(cb_unused const wxString& buffer, cb_unused wxArrayString& result){ return false; }

    virtual bool Reparse(cb_unused const wxString& filename, cb_unused bool isLocal = true)                              { return false; }
    virtual bool AddFile(cb_unused const wxString& filename, cb_unused cbProject* project, cb_unused bool isLocal = true){ return false; }
    virtual bool RemoveFile(cb_unused const wxString& filename)                                                          { return false; }
    virtual bool IsFileParsed(cb_unused const wxString& filename)                                                        { return false; }

    virtual bool     Done()          { return true;            }
    virtual wxString NotDoneReason() { return wxEmptyString;   }

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
    wxArrayString        FindFileInIncludeDirs(const wxString& file, bool firstonly = false);

    void            ReadOptions();
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

    ParserOptions        m_Options;
    BrowserOptions       m_BrowserOptions;

private:
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
  * The parser class controls ParserThreads in a pool, which holds ParserThread objects for each source file.
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

    /** Add the priority header files, these files will be parsed with the sequence as they added.
     * @param filename input priority header file name
     * @param systemHeaderFile true if it is a system header file
     */
    virtual void AddPriorityHeaders(const wxString& filename, bool systemHeaderFile);

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

    virtual void AddPredefinedMacros(const wxString& defs);

    /** set the associated C::B project pointer. (only used by one parser for whole workspace) */
    virtual bool UpdateParsingProject(cbProject* project);

    /** Must add a locker before call all named ParseBufferXXX functions
     */
    virtual bool ParseBuffer(const wxString& buffer, bool isLocal, bool bufferSkipBlocks = false,
                             bool isTemp = false, const wxString& filename = wxEmptyString,
                             int parentIdx = -1, int initLine = 0);
    virtual bool ParseBufferForFunctions(const wxString& buffer);
    virtual bool ParseBufferForNamespaces(const wxString& buffer, NameSpaceVec& result);
    virtual bool ParseBufferForUsingNamespace(const wxString& buffer, wxArrayString& result);

    virtual bool Reparse(const wxString& filename, bool isLocal = true);
    virtual bool AddFile(const wxString& filename, cbProject* project, bool isLocal = true);
    virtual bool RemoveFile(const wxString& filename);
    virtual bool IsFileParsed(const wxString& filename);

    virtual bool     Done();
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

    bool Parse(const wxString& filename, bool isLocal = true, bool locked = false, LoaderBase* loader = NULL);
    void ReparseModifiedFiles();
    void TerminateAllThreads();

    /** When a ThreadPool batch parse stage is done, it will issue a cbEVT_THREADTASK_ALLDONE message.
     * In some situations this event will be triggered, such as:
     * - after "Priority" header parsing
     * - batch parsing for general (normal) source files
     * - system header files parsing
     */
    void OnAllThreadsDone(CodeBlocksEvent& event);

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
    virtual bool ParseFile(const wxString& filename, bool isGlobal, bool locked = false);
    void ConnectEvents();
    void DisconnectEvents();
    /** when initialized, this variable will be an instance of a NativeParser */
    wxEvtHandler*             m_Parent;
    /** referring to the cbp project currently parsing */
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

    /** true, if the parser is still busy with parsing, false if the parsing stage has finished */
    bool                      m_IsParsing;

    /** Determine whether the parser is doing Priority header parsing. If yes, the added files (usually
     * the priority files) will be parsed accordingly.
     * Otherwise, added file will be parsed by thread pool (batch parse mode), thus the sequence
     * of the parsed files is not reserved. The thread pool may run those threads randomly.
     */
    bool                      m_IsPriority;

    /** Indicates some files in the current project need to be re-parsed, this is commonly caused
      * that the "real-time parsing option" is enabled, and user is editing source file.
      */
    bool                      m_NeedsReparse;

    /** batch Parse mode flag. Normal files (not in the parse "Priority" files stage) will set this flag. */
    bool                      m_IsFirstBatch;

private:
    wxTimer                   m_ReparseTimer;
    /** a timer to delay the operation of batch parsing, see OnBatchTimer() member function as a reference*/
    wxTimer                   m_BatchTimer;
    /** a stop watch to measure parsing time*/
    wxStopWatch               m_StopWatch;
    bool                      m_StopWatchRunning;
    long                      m_LastStopWatchTime;
    bool                      m_IgnoreThreadEvents;

    /** the files(mostly the header files) need to be parsed before any other files, we use this kind
     * of files to get more correct macro definition.
     */
    StringList                m_PriorityHeaders;       //!< All priority headers

    // These priority header files are saved when first parsed, and the last stage of batch parsing,
    // they will be reparsed again.
    StringList                m_SystemPriorityHeaders; //!< Only system priority headers, for re-parse
    StringList                m_BatchParseFiles;       //!< All other batch parse files, like the normal headers/sources
    wxString                  m_PredefinedMacros;      //!< Pre-defined macros, its a buffer queried from the compiler command line
    /** used to measure batch parse time*/
    bool                      m_IsBatchParseDone;
    ParserCommon::ParserState m_ParserState;           //!< indicated the current state the parser is in
    /** if true, all the files of the current project will be labeled as "local" */
    bool                      m_NeedMarkFileAsLocal;
};

#endif // PARSER_H

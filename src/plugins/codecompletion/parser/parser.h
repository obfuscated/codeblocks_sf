/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef PARSER_H
#define PARSER_H

#include <wx/string.h>
#include <wx/treectrl.h>
#include <wx/event.h>
#include <wx/timer.h>
#include <wx/file.h>
#include <wx/filefn.h> // wxPathList
#include <wx/imaglist.h>
#include <wx/thread.h>

#include "parserthread.h"

#include <cbthreadpool.h>
#include <sdk_events.h>

#define PARSER_IMG_NONE                -2
#define PARSER_IMG_CLASS_FOLDER         0
#define PARSER_IMG_CLASS                1
#define PARSER_IMG_CLASS_PRIVATE        2
#define PARSER_IMG_CLASS_PROTECTED      3
#define PARSER_IMG_CLASS_PUBLIC         4
#define PARSER_IMG_CTOR_PRIVATE         5
#define PARSER_IMG_CTOR_PROTECTED       6
#define PARSER_IMG_CTOR_PUBLIC          7
#define PARSER_IMG_DTOR_PRIVATE         8
#define PARSER_IMG_DTOR_PROTECTED       9
#define PARSER_IMG_DTOR_PUBLIC          10
#define PARSER_IMG_FUNC_PRIVATE         11
#define PARSER_IMG_FUNC_PROTECTED       12
#define PARSER_IMG_FUNC_PUBLIC          13
#define PARSER_IMG_VAR_PRIVATE          14
#define PARSER_IMG_VAR_PROTECTED        15
#define PARSER_IMG_VAR_PUBLIC           16
#define PARSER_IMG_PREPROCESSOR         17
#define PARSER_IMG_ENUM                 18
#define PARSER_IMG_ENUM_PRIVATE         19
#define PARSER_IMG_ENUM_PROTECTED       20
#define PARSER_IMG_ENUM_PUBLIC          21
#define PARSER_IMG_ENUMERATOR           22
#define PARSER_IMG_NAMESPACE            23
#define PARSER_IMG_TYPEDEF              24
#define PARSER_IMG_TYPEDEF_PRIVATE      25
#define PARSER_IMG_TYPEDEF_PROTECTED    26
#define PARSER_IMG_TYPEDEF_PUBLIC       27
#define PARSER_IMG_SYMBOLS_FOLDER       28
#define PARSER_IMG_VARS_FOLDER          29
#define PARSER_IMG_FUNCS_FOLDER         30
#define PARSER_IMG_ENUMS_FOLDER         31
#define PARSER_IMG_PREPROC_FOLDER       32
#define PARSER_IMG_OTHERS_FOLDER        33
#define PARSER_IMG_TYPEDEF_FOLDER       34
#define PARSER_IMG_MACRO                35
#define PARSER_IMG_MACRO_PRIVATE        36
#define PARSER_IMG_MACRO_PROTECTED      37
#define PARSER_IMG_MACRO_PUBLIC         38
#define PARSER_IMG_MACRO_FOLDER         39

#define PARSER_IMG_MIN PARSER_IMG_CLASS_FOLDER
#define PARSER_IMG_MAX PARSER_IMG_MACRO_FOLDER

typedef std::set<wxString> StringSet;
typedef std::list<wxString> StringList;

class ClassTreeData : public BlockAllocated<ClassTreeData, 1000>, public wxTreeItemData
{
public:
    ClassTreeData(Token* token) { m_Token = token; }
    Token* GetToken() { return m_Token; }
    void SetToken(Token* token) { m_Token = token; }
private:
    Token* m_Token;
};

struct ParserOptions
{
    bool followLocalIncludes;
    bool followGlobalIncludes;
    bool caseSensitive;
    bool wantPreprocessor;
    bool useSmartSense;
    bool whileTyping;
    bool parseComplexMacros;
};

enum BrowserDisplayFilter
{
    bdfFile = 0,
    bdfProject,
    bdfEverything
};

enum BrowserSortType
{
    bstAlphabet = 0,
    bstKind,
    bstScope,
    bstNone
};

struct BrowserOptions
{
    bool                 showInheritance; // default: false
    bool                 expandNS;        // default: false (auto-expand namespaces)
    bool                 treeMembers;     //
    BrowserDisplayFilter displayFilter;   // default: bdfFile
    BrowserSortType      sortType;        //
};

class ClassBrowser;

extern int PARSER_START;
extern int PARSER_END;

enum ParsingType
{
    ptCreateParser    = 1,
    ptReparseFile     = 2,
    ptAddFileToParser = 3,
    ptUndefined       = 4,
};

static wxCriticalSection s_ParserCritical;

/** @brief Parser class holds all the tokens of a C::B project
  *
  * Parser class contains the TokensTree which is a Trie structure to recored Token information.
  * For compact Trie, see more details in http://en.wikipedia.org/wiki/Trie
  * Parser class controls the several ParserThread Pool, which hold ParserThread objects for each source file.
  * Batch Parse Mode means We have a lot of files to be parsed, so a lot of Parserthread was generated and
  * Add to the ThreadPool, and lately, the ParserThread was executed by ThreadPool.
  */
class Parser : public wxEvtHandler
{
    friend class ParserThread;
    friend class AddParseThread;

public:
    /** constructor
     * @param parent which is actually a NativeParser object
     * @param project the C::B project associated with the current Parser
     */
    Parser(wxEvtHandler* parent, cbProject* project);
    /** destructor */
    ~Parser();

    /** Add the UpFront header files, there files will be parsed with the sequence as they added.
     * @param filename input UpFront file name
     * @param systemHeaderFile true if it is a system header file
     * @param delay true if it use predefined delay time, otherwise it use 1 ms delay
     */
    void AddUpFrontHeaders(const wxString& filename, bool systemHeaderFile, bool delay = true);

    /** Add files to Batch Parse mode, internally, The files added were parsed sequentially.
     * Not that when some "#include" files were added to the Batch Parse, their parsing sequence
     * May be random.
     * @param filenames input files name array
     * @param delay true if it use predefined delay time, otherwise it use 1 ms delay.
     */
    void AddBatchParse(const StringList& filenames, bool delay = true);

    /** Add one file to Batch mode Parsing
     * @param filenames input file name
     * @param delay true if it use predefined delay time, otherwise it use 1 ms delay.
     */
    void AddParse(const wxString& filename, bool delay = true);

    /** verify (return) the associated C::B project pointer.
     * @return C::B project pointer
     */
    cbProject* GetParsingProject() const { return m_Project; }

    bool ParseBuffer(const wxString& buffer, bool isLocal, bool bufferSkipBlocks = false, bool isTemp = false,
                     const wxString& filename = wxEmptyString, Token* parent = nullptr, int initLine = 0);
    bool ParseBufferForFunctions(const wxString& buffer);
    bool ParseBufferForNamespaces(const wxString& buffer, NameSpaceVec& result);
    bool ParseBufferForUsingNamespace(const wxString& buffer, wxArrayString& result);

    bool Reparse(const wxString& filename, bool isLocal = true);
    bool AddFile(const wxString& filename, bool isLocal = true);
    bool RemoveFile(const wxString& filename);

    void ReadOptions();
    void WriteOptions();
    bool ReadFromCache(wxInputStream* f);
    bool WriteToCache(wxOutputStream* f);
    bool CacheNeedsUpdate();
    bool IsFileParsed(const wxString& filename);

    void StartStopWatch();
    void EndStopWatch();

    Token* FindTokenByName(const wxString& name, bool globalsOnly = true, short int kindMask = 0xFFFF);
    Token* FindChildTokenByName(Token* parent, const wxString& name, bool useInheritance = false, short int kindMask = 0xFFFF);
    size_t FindMatches(const wxString& s, TokenList&   result, bool caseSensitive = true, bool is_prefix = true);
    size_t FindMatches(const wxString& s, TokenIdxSet& result, bool caseSensitive = true, bool is_prefix = true);
    size_t FindTokensInFile(const wxString& fileName, TokenIdxSet& result, short int kindMask);
    ParserOptions& Options() { return m_Options; }
    BrowserOptions& ClassBrowserOptions() { return m_BrowserOptions; }

    void ClearIncludeDirs() { m_IncludeDirs.Clear(); }
    void AddIncludeDir(const wxString& dir);
    void AddPredefinedMacros(const wxString& defs);
    const wxArrayString& GetIncludeDirs() const { return m_IncludeDirs; }
    wxString GetFullFileName(const wxString& src, const wxString& tgt, bool isGlobal);
    wxString FindFirstFileInIncludeDirs(const wxString& file);
    wxArrayString FindFileInIncludeDirs(const wxString& file, bool firstonly = false);

    TokensTree* GetTokens() { return m_TokensTree; }
    TokensTree* GetTempTokens() { return m_TempTokensTree; }
    unsigned int GetFilesCount();

    bool Done();
    void LinkInheritance(bool tempsOnly = false);

    /** Before call this function, *MUST* add a locker
      * e.g. wxCriticalSectionLocker locker(s_TokensTreeCritical);
      */
    void MarkFileTokensAsLocal(const wxString& filename, bool local, void* userData = 0);

    /** Node: Currently, the Max concurrent Parserthread number should be ONE, CC does not support
     * Multiply threads Parsing.
     */
    unsigned int GetMaxThreads() const { return m_Pool.GetConcurrentThreads(); }

    /** Not used, because the ThreadPool only support running ONE ParserThread concurrently */
    void SetMaxThreads(unsigned int max) { m_Pool.SetConcurrentThreads(max); }

protected:
    // *MUST* called from child thread-->
    bool Parse(const wxString& filename, bool isLocal = true, LoaderBase* loader = NULL);
    bool Parse(const wxString& bufferOrFilename, bool isLocal, ParserThreadOptions& opts);
    void DoParseFile(const wxString& filename, bool isGlobal);
    // *MUST* end--<

    bool ReparseModifiedFiles();
    void TerminateAllThreads();

    /** When a ThreadPool batch Parse stage is done, it will receive cbEVT_THREADTASK_ALLDONE Message.
     * Some situations we will receive this Message, such as:
     * After UpFront header Parsing.
     * Batch Parsing for general(normal) source files.
     * System header files Parsing.
     */
    void OnAllThreadsDone(CodeBlocksEvent& event);

    void OnTimer(wxTimerEvent& event);
    void OnBatchTimer(wxTimerEvent& event);

    void ProcessParserEvent(ParsingType type, int id, const wxString& info = wxEmptyString);

private:
    void ConnectEvents();
    void DisconnectEvents();

    ParserOptions        m_Options;
    BrowserOptions       m_BrowserOptions;
    SearchTree<wxString> m_GlobalIncludes;
    wxArrayString        m_IncludeDirs;
    wxEvtHandler*        m_Parent;
    cbProject*           m_Project;

protected:
    // the following three members are used to detect changes between
    // in-memory data and cache
    bool                 m_UsingCache; // true if loaded from cache

    typedef std::vector<ParserThread*> PTVector;

    /** Thread Queue, these Thread Tasks will be executed FIFO mode as they added */
    std::queue<PTVector> m_PoolTask;

    /** Thread Pool, Manage all the ParserThread, used in Batch Parse Mode. The thread pool can
     * add/remove/execute the Parserthread Tasks.
     */
    cbThreadPool         m_Pool;

    /** determine whether we need a UpFront Header Parsing, if Yes, The added file will be parsed when they
     * added. Otherwise, added file will be parsed by thread pool(batch parse mode), thus the sequence
     * of the parsed files were not confirmed
     */
    bool                 m_IsUpFront;

    /** each Parser class contains a TokensTree object which used to record tokens per project
      * this tree will be created in the constructor and destroyed in destructor.
      */
    TokensTree*          m_TokensTree;

    /** a temp Tokenstree hold some temporary tokens, e.g. parsing a buffer containing some
      * preprocessor directives, see ParseBufferForFunctions() like functions
      * this tree will be created in the constructor and destroyed in destructor.
      */
    TokensTree*          m_TempTokensTree;

    std::set<wxString, std::less<wxString> >
                         m_LocalFiles;

    /** Indicates some files in the current project need to be re-parsed, this is commonly caused
      * that the "real-time parsing option" is enabled, and user is editing source file.
      */
    bool                 m_NeedsReparse;

    /** Batch Parse mode flag. Normal files( Non-UpFront files) stage will set this flag. */
    bool                 m_IsFirstBatch;
    /** flag variable if true, which means Parser are busy running Parsing, false if all the parse stage finishes*/
    bool                 m_IsParsing;

private:
    wxTimer              m_Timer;
    wxTimer              m_BatchTimer;
    wxStopWatch          m_StopWatch;
    bool                 m_StopWatchRunning;
    long                 m_LastStopWatchTime;
    bool                 m_IgnoreThreadEvents;

    StringList           m_UpFrontHeaders;        // All up-front headers
    StringList           m_SystemUpFrontHeaders;  // Only system up-front headers, for reparse
    StringList           m_BatchParseFiles;       // All other batch parse files
    wxString             m_PredefinedMacros;      // Pre-defined macros
    /** used to measure Batch Parse time*/
    bool                 m_IsBatchParseDone;
    ParsingType          m_ParsingType;
    /** if true, means all the files contains in the C::B project will be labeled as "local"*/
    bool                 m_NeedMarkFileAsLocal;

    DECLARE_EVENT_TABLE()
};

#endif // PARSER_H

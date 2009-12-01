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
#include "parserthread.h"
#include <cbthreadpool.h>
#include <sdk_events.h>

#ifndef STANDALONE
    #include <wx/imaglist.h>
#endif // STANDALONE

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

extern int PARSER_END;
class ClassTreeData : public BlockAllocated<ClassTreeData, 500>, public wxTreeItemData
{
    public:
        ClassTreeData(Token* token){ m_Token = token; }
        Token* GetToken(){ return m_Token; }
        void SetToken(Token* token){ m_Token = token; }
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
};

enum BrowserDisplayFilter
{
    bdfFile = 0,
    bdfProject,
    bdfWorkspace,
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
    BrowserDisplayFilter displayFilter;   // default: bdfWorkspace
    BrowserSortType      sortType;        //
};

class ClassBrowser;
class ClassBrowserBuilderThread;

class Parser : public wxEvtHandler
{
    public:
        friend class ClassBrowser;
        friend class ParserThread;
        Parser(wxEvtHandler* parent);
        ~Parser();

        void BatchParse(const wxArrayString& filenames);
        bool Parse(const wxString& filename,         bool isLocal = true, LoaderBase* loader = 0);
        bool Parse(const wxString& bufferOrFilename, bool isLocal,        ParserThreadOptions& opts);
        bool ParseBuffer(const wxString& buffer, bool isLocal = true, bool bufferSkipBlocks = false, bool isTemp = false);
        bool ParseBufferForFunctions(const wxString& buffer);
        bool ParseBufferForUsingNamespace(const wxString& buffer, wxArrayString& result);
        bool Reparse(const wxString& filename, bool isLocal = true);
        bool ReparseModifiedFiles();
        bool RemoveFile(const wxString& filename);
        void Clear();
        void ReadOptions();
        void WriteOptions();
        bool ReadFromCache(wxInputStream* f);
        bool WriteToCache(wxOutputStream* f);
        bool CacheNeedsUpdate();

        void StartStopWatch();
        void EndStopWatch();
        long EllapsedTime();
        long LastParseTime();

#ifndef STANDALONE
        int GetTokenKindImage(Token* token);
        void SetTokenKindImage(int kind, const wxBitmap& bitmap, const wxBitmap& mask = wxNullBitmap);
        void SetTokenKindImage(int kind, const wxBitmap& bitmap, const wxColour& maskColour);
        void SetTokenKindImage(int kind, const wxIcon& icon);
        wxImageList* GetImageList() { return m_pImageList; }
#endif // STANDALONE
        Token* FindTokenByName(const wxString& name, bool globalsOnly = true, short int kindMask = 0xFFFF) const;
        Token* FindChildTokenByName(Token* parent, const wxString& name, bool useInheritance = false, short int kindMask = 0xFFFF) const;
        size_t FindMatches(const wxString& s, TokenList&   result, bool caseSensitive = true, bool is_prefix = true);
        size_t FindMatches(const wxString& s, TokenIdxSet& result, bool caseSensitive = true, bool is_prefix = true);
        ParserOptions& Options() { return m_Options; }
        BrowserOptions& ClassBrowserOptions() { return m_BrowserOptions; }

        void ClearIncludeDirs() { m_IncludeDirs.Clear(); }
        void AddIncludeDir(const wxString& dir);
        const wxArrayString& GetIncludeDirs() const { return m_IncludeDirs; }
        wxString GetFullFileName(const wxString& src,const wxString& tgt, bool isGlobal);
        wxString FindFirstFileInIncludeDirs(const wxString& file);
        wxArrayString FindFileInIncludeDirs(const wxString& file,bool firstonly = false);

        TokensTree* GetTokens(){ return m_pTokens; }
        TokensTree* GetTempTokens() { return m_pTempTokens; }
        unsigned int GetFilesCount();

        bool Done();
        void LinkInheritance(bool tempsOnly = false);
        void MarkFileTokensAsLocal(const wxString& filename, bool local, void* userData = 0);

        unsigned int GetMaxThreads() const { return m_Pool.GetConcurrentThreads(); }
        void SetMaxThreads(unsigned int max) { m_Pool.SetConcurrentThreads(max); }

        void TerminateAllThreads();
    protected:
        void OnParseFile(const wxString& filename,int flags);
        void OnAllThreadsDone(CodeBlocksEvent& event);
        void OnTimer(wxTimerEvent& event);
        void OnBatchTimer(wxTimerEvent& event);
    private:
        void ConnectEvents();
        void DisconnectEvents();

        ParserOptions                  m_Options;
        BrowserOptions                 m_BrowserOptions;
        SearchTree<wxString>           m_GlobalIncludes;
        wxArrayString                  m_IncludeDirs;
        wxEvtHandler*                  m_pParent;
        wxTreeItemId                   m_RootNode;
#ifndef STANDALONE
        wxImageList*                   m_pImageList;
    protected:
        // the following three members are used to detect changes between
        // in-mem data and cache
        bool m_UsingCache; // true if loaded from cache

        cbThreadPool                   m_Pool;
        TokensTree*                    m_pTokens;
        TokensTree*                    m_pTempTokens;
        set<wxString, less<wxString> > m_LocalFiles;
        bool                           m_NeedsReparse;
        bool                           m_IsBatch;
        ClassBrowser*                  m_pClassBrowser; // Which class browser are we updating?
        int                            m_TreeBuildingStatus; // 0 = Done; 1 = Needs update; 2 = Updating.
        size_t                         m_TreeBuildingTokenIdx; // Bookmark for the tree-building process
    private:
        wxTimer                        m_Timer;
        wxTimer                        m_BatchTimer;
        wxStopWatch                    m_StopWatch;
        bool                           m_StopWatchRunning;
        long                           m_LastStopWatchTime;
        bool                           m_IgnoreThreadEvents;
        bool                           m_ShuttingDown;

        ClassBrowserBuilderThread*     m_pClassBrowserBuilderThread;

#endif // STANDALONE

        DECLARE_EVENT_TABLE()
};

#endif // PARSER_H

#ifndef PARSER_BASE_H
#define PARSER_BASE_H


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

// no browser related class!

typedef std::set<wxString>  StringSet;
typedef std::list<wxString> StringList;

namespace ParserCommon
{
    enum EFileType
    {
        ftHeader,
        ftSource,
        ftOther
    };
    EFileType FileType(const wxString& filename, bool force_refresh = false);
}// namespace ParserCommon

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
    BrowserOptions():
        showInheritance(false),
        expandNS(false),
        treeMembers(true),
        displayFilter(bdfFile),
        sortType(bstKind)
    {}

    bool                 showInheritance; //!< default: false
    bool                 expandNS;        //!< default: false (auto-expand namespaces)
    bool                 treeMembers;     //!< default: true
    BrowserDisplayFilter displayFilter;   //!< default: bdfFile
    BrowserSortType      sortType;        //!< default: bstKind
};

/** Setting of the Parser, some of them will be passed down to ParserThreadOptions */
struct ParserOptions
{
    ParserOptions():
        followLocalIncludes(true),
        followGlobalIncludes(true),
        caseSensitive(true),
        wantPreprocessor(true),
        useSmartSense(true),
        whileTyping(true),
        parseComplexMacros(true),
        platformCheck(true),
        storeDocumentation(true)
    {}

    bool followLocalIncludes;  /// parse XXX.h in directive #include "XXX.h"
    bool followGlobalIncludes; /// parse XXX.h in directive #include <XXX.h>
    bool caseSensitive;        /// case sensitive in MarkItemsByAI
    bool wantPreprocessor;     /// handle preprocessor directive in Tokenizer class
    bool useSmartSense;        /// use real AI(scope sequence match) or not(plain text match)
    bool whileTyping;          /// reparse the active editor while editing
    bool parseComplexMacros;   /// this will let the Tokenizer to recursive expand macros
    bool platformCheck;        /// this will check for the platform of the project/target when adding include folders to the parser
    bool storeDocumentation;   /// should tokenizer detect and store doxygen documentation?
};

// both the CodeCompletion plugin and the cc_test project share this class, this class hold a Token
// Tree.
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

    virtual bool ParseBuffer(const wxString& buffer, bool isLocal, bool bufferSkipBlocks = false,
                             bool isTemp = false, const wxString& filename = wxEmptyString,
                             int parentIdx = -1, int initLine = 0);
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
    /** it mimics what a compiler does to find an include header files, if the firstonly option is
     * true, it will return the first found header file, otherwise, the complete database of the
     * Parser's include paths will be searched.
     */
    wxArrayString   FindFileInIncludeDirs(const wxString& file, bool firstonly = false);

    /** read Parser options from configure file */
    virtual void            ReadOptions() {}
    /** write Parse options to configure file */
    virtual void            WriteOptions() {}

    // make them virtual, so Parser class can overwrite then!
    virtual ParserOptions&  Options()             { return m_Options;        }
    virtual BrowserOptions& ClassBrowserOptions() { return m_BrowserOptions; }

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
    /** wxString -> wxString map */
    SearchTree<wxString> m_GlobalIncludes;

    /** the include directories can be either three kinds below:
     * 1, compiler's default search paths, e.g. E:\gcc\include
     * 2, project's common folders, e.g. the folder where you put the cbp file in
     * 3, the compiler include search paths defined in the cbp, like: E:\wx2.8\msw\include
     */
    wxArrayString        m_IncludeDirs;
};

#endif

/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef NATIVEPARSER_H
#define NATIVEPARSER_H

#include "parser/parser.h"

#include <queue>
#include <map>

#include <wx/event.h>
#include <wx/hashmap.h> // TODO: replace with std::map

/** debug only variable, used to print the AI match related log*/
extern bool s_DebugSmartSense;

extern const wxString g_StartHereTitle;
extern const int g_EditorActivatedDelay;

// forward declaration
class cbEditor;
class EditorBase;
class cbProject;
class ClassBrowser;
class Token;

WX_DECLARE_HASH_MAP(cbProject*, Parser*, wxPointerHash, wxPointerEqual, ParsersMap);
WX_DECLARE_HASH_MAP(cbProject*, wxString, wxPointerHash, wxPointerEqual, ParsersFilenameMap);

typedef std::map<cbProject*, wxArrayString> ProjectSearchDirsMap;

/** divide a statement to several pieces(parser component), each component has a type character*/
enum ParserTokenType
{
    pttUndefine = 0,
    pttSearchText,
    pttClass,
    pttNamespace,
    pttFunction,
};

/** the eliminator separate two Parser Component, See ParserComponent struct for more details */
enum OperatorType
{
    otOperatorUndefine = 0,
    otOperatorSquare,
    otOperatorParentheses,
    otOperatorPointer,
    otOperatorStar
};

/** Symbol browser option */
enum BrowserViewMode
{
    bvmRaw = 0,
    bvmInheritance
};

/** @brief a long statement can divide to several ParserComponent chains
 *
 * e.g. for a statement like below:
 * Ogre::Root::getSingleton().|
 *
 *  a chains of four ParserComponent will be generated and list below:
 *  Ogre             [pttNamespace]
 *  Root             [pttClass]
 *  getSingleton     [pttFunction]
 *  (empty space)    [pttSearchText]
 */
struct ParserComponent
{
    wxString component;
    ParserTokenType tokenType;
    OperatorType tokenOperatorType;
    ParserComponent() { Clear(); }
    void Clear()
    {
        component         = wxEmptyString;
        tokenType         = pttUndefine;
        tokenOperatorType = otOperatorUndefine;
    }
};

/** Search structure combining
 *  a pointer to cbStyledTextCtrl and a filename is enough
 */
struct ccSearchData
{
    cbStyledTextCtrl* control;
    wxString file;
};

/** @brief NativeParser class is just like a manager class to control Parser objects.
  *
  * There will be a Parser object associated with a Project (cbp).
  * Nativeparser will handle all the Parser objects.
  */
class NativeParser : public wxEvtHandler
{
public:
    /** Constructor */
    NativeParser();
    /** Destructor */
    ~NativeParser();

    /** return a reference to the currently active parser object */
    Parser& GetParser() { return *m_Parser; }
    /** return a reference to the temporary parser object*/
    Parser& GetTempParser() { return m_TempParser; }

    /** return the Parser pointer corresponding to the input C::B project
     * @param project input C::B project pointer
     * @return a pointer to parser object
     */
    Parser* GetParserByProject(cbProject* project);

    /** return the Parser pointer associated with the input file
     * Internally this function will first find the project containing the input file,
     * then return the Parser pointer by the project.
     * @param filename filename with full path.
     * @return Parser pointer
     */
    Parser* GetParserByFilename(const wxString& filename);

    /** return the C::B project associated with Parser pointer
     * @param parser Parser pointer
     * @return C::B Project pointer
     */
    cbProject* GetProjectByParser(Parser* parser);

    /** return the C::B project containing the filename
     *  The function first try to match the filename in the active project,next to match other projects opened,
     *  If the file exists in several projects, the first matched project will be returned.
     * @param filename input filename
     * @return project pointer containing the file
     */
    cbProject* GetProjectByFilename(const wxString& filename);

    /** Return true if all the parser's batch-parse stage was done, otherwise return false*/
    bool Done();

    /** the function list below used to support Symbol browser and codecompletion UI
     *  Image list is used to initialize the symbol browser tree node image.
     */
    wxImageList* GetImageList() { return m_ImageList; }
    int GetTokenKindImage(Token* token);
    void SetTokenKindImage(int kind, const wxBitmap& bitmap, const wxBitmap& mask = wxNullBitmap);
    void SetTokenKindImage(int kind, const wxBitmap& bitmap, const wxColour& maskColour);
    void SetTokenKindImage(int kind, const wxIcon& icon);

    /** Get the implementation file path if the input is a header file. or Get the header file path
     * if the input is an implementation file. Both the implementation file and header file could
     * be in different directories.
     * @param filename input filename
     * @return corresponding file paths, in wxArrayString format
     */
    wxArrayString GetAllPathsByFilename(const wxString& filename);

    /** Add the paths to path array, and this will be used in GetAllPathsByFilename() function.
     *  internally, all the folder path was recored in UNIX format.
     */
    void AddPaths(wxArrayString& dirs, const wxString& path, bool hasExt);

    // the functions below are handling and managing Parser object
    /** Dynamically allocate a Parser object for the input project
     * @param project C::B project
     * @return parser pointer of the project.
     */
    Parser* CreateParser(cbProject* project);

    /** delete the Parser object for the input project
     * @param project C::B project.
     * @return true if success.
     */
    bool DeleteParser(cbProject* project);

    /** Single file re-parse.
     * this was happing when you add a single file to project, or you are just modified a file.
     * @param project C::B project
     * @param filename filename with full patch in the C::B project
     */
    bool ReparseFile(cbProject* project, const wxString& filename);

    /** New files were added to the C::B project, so this will cause a re-parse on the new added file.
     * @param project C::B project
     * @param filename filename with full patch in the C::B project
     */
    bool AddFileToParser(cbProject* project, const wxString& filename);

    /** files were removed from C::B project and Parser
     * @param project C::B project
     * @param filename filename with full patch in the C::B project
     */
    bool RemoveFileFromParser(cbProject* project, const wxString& filename);

    /** when user changes the CC option, we should re-read the option */
    void RereadParserOptions();

    /** re-parse the active Parser (the project associated with m_Parser member variable */
    void ReparseCurrentProject();

    /** re-parse the project select by context menu in projects management panel */
    void ReparseSelectedProject();

    /** collect the tokens where a code suggestion list can show
     * @param searchData input search location
     * @param result output the all matching result token index
     * @param reallyUseAI true means the context scope should be considered, false if only do a plain word match
     * @param isPrefix the other side is partially match which result all the works with the same prefix, else use full-match
     * @param caseSensitive care about case or not
     * @param caretPos Where the current caret locates, -1 means we used the current caret position.
     * @return the matching number
     */
    size_t MarkItemsByAI(ccSearchData* searchData, TokenIdxSet& result, bool reallyUseAI = true,
                         bool isPrefix = true, bool caseSensitive = false, int caretPos = -1);

    /** the same as before, but we don't specify the searchData information, so it will use the active
     *  editor and current caret information.
     */
    size_t MarkItemsByAI(TokenIdxSet& result, bool reallyUseAI = true, bool isPrefix = true,
                         bool caseSensitive = false, int caretPos = -1);

    /** Collect the suggestion list items, internally it call MarkItemsByAI() */
    const wxString& GetCodeCompletionItems();

    /** set start and end to the calltip highlight region, based on m_CallTipCommas (calculated in GetCallTips())*/
    void GetCallTipHighlight(const wxString& calltip, int* start, int* end);

    /** count commas in lineText (nesting parentheses)*/
    int CountCommas(const wxString& calltip, int start);

    /** Call tips are when you mouse pointer hover some statement, and show the information of statement below caret.
     * these tips information ware the prototype of the current function, or the type of the variable...
     * @param chars_per_line specify the char number per one line, so it can restrict the width.
     * @return all the results were stored in a wxArrayString
     */
    const wxArrayString& GetCallTips(int chars_per_line);

    /** TODO ? */
    int GetCallTipCommas() const { return m_CallTipCommas; }

    /** Word start position of the editor
     * @return position index
     */
    int GetEditorStartWord() const { return m_EditorStartWord; }
    /** Word end position of the editor
     * @return  position index
     */
    int GetEditorEndWord() const { return m_EditorEndWord; }

    /** project search path is used for auto completion for #include <> ?*/
    wxArrayString& GetProjectSearchDirs(cbProject* project);

    /** returns the editor's position where the current function starts.
     * optionally, returns the function's namespace (ends in double-colon ::) and name and token
     * @param searchData search data struct pointer
     * @param nameSpace get the namespace modifier
     * @param procName get the function name
     * @param functionToken get the token of current function
     * @param caretPos caret position in cbeditor
     * @return current function line number
     */
    int FindCurrentFunctionStart(ccSearchData* searchData, wxString* nameSpace = 0L, wxString* procName = 0L,
                                 Token** functionToken = 0L, int caretPos = -1);

    /** fills the result argument with all the tokens matching the current function (hopefully, just one)
     * @param editor editor pointer
     * @param result output result containing all the Token index
     * @param caretPos -1 if the current caret position is used.
     * @return number of result Tokens
     */
    size_t FindCurrentFunctionToken(ccSearchData* searchData, TokenIdxSet& result, int caretPos = -1);

    // The function below is used to manage symbols browser
    ClassBrowser* GetClassBrowser() const { return m_ClassBrowser; }
    /** create the class browser */
    void CreateClassBrowser();
    /** remove the class browser */
    void RemoveClassBrowser(bool appShutDown = false);
    /** update the class browser tree*/
    void UpdateClassBrowser();
    /** set the class browser view mode*/
    void SetCBViewMode(const BrowserViewMode& mode);

    /** Read project CC options  */
    void OnProjectLoadingHook(cbProject* project, TiXmlElement* elem, bool loading);

protected:
    /** When a Parser is created, we need a full parsing stage including:
     * 1, parse the up-front header files firstly.
     * 2, parse all the other project files.
     */
    bool StartCompleteParsing(cbProject* project, Parser* parser);

    /** Switch parser object according the current active editor and filename */
    bool SwitchParser(cbProject* project, Parser* parser);

    /** Set a new Parser as the active Parser
     * Set the active parser pointer (m_Parser member variable)
     * update the ClassBrowser's Parser pointer
     * re-fresh the symbol browser window.
     */
    void SetParser(Parser* parser);

    /** Clear all Parser object*/
    void ClearParsers();

    /** Remove all the obsolete Parser object
     * if the number is beyond the limited number (can be set in the CC's option), then all the
     * obsolete parser will be removed
     */
    void RemoveObsoleteParsers();

    /** Get cbProject and Parser pointer, according to the current active editor*/
    std::pair<cbProject*, Parser*> GetParserInfoByCurEditor();

private:
    friend class CodeCompletion;

    /** Start an Artificial Intelligence sequence to gather all the matching tokens..
     * The actual AI is in FindAIMatches() below...
     * @param result output parameter.
     * @param searchData cbEditor information.
     * @param lineText current statement.
     * @param isPrefix if true, then the result contains all the tokens whose name is a prefix of current lineText.
     * @param caseSensitive true is case sensitive is enabled on the match.
     * @param search_scope it is the "parent token" where we match the "search-key".
     * @param caretPos use current caret position if it is -1.
     * @return match token number
     */
    size_t AI(TokenIdxSet& result,
              ccSearchData* searchData,
              const wxString& lineText = wxEmptyString,
              bool isPrefix = false,
              bool caseSensitive = false,
              TokenIdxSet* search_scope = 0,
              int caretPos = -1);
    /**@brief Artificial Intelligence Matching
    *
    * match the ParserComponent queue from left to right, Each time a ParserComponent is consumed, output
    * matching result becomes the next search scope, and finally given the result matching the last ParserComponent
    * @param components input ParserComponent queue
    * @param parentTokenIdx, initial search scope of the left most component
    * @param fullMatch the result should be a full text match or prefix match
    * @return matching token number
    */
    size_t FindAIMatches(std::queue<ParserComponent> components,
                         TokenIdxSet& result,
                         int parentTokenIdx = -1,
                         bool isPrefix = false,
                         bool caseSensitive = false,
                         bool use_inheritance = true,
                         short int kindMask = 0xFFFF,
                         TokenIdxSet* search_scope = 0);

    /** @brief break a statement to several ParserComponent, and store them in a queue
     * @param actual a statement string to be divided.
     * @param components output variable containing the queue
     * @return number of ParserComponent
     */
    size_t BreakUpComponents(const wxString& actual, std::queue<ParserComponent>& components);

    /** Decides if the token belongs to its parent or one of its ancestors */
    bool BelongsToParentOrItsAncestors(TokensTree* tree, Token* token, int parentIdx, bool use_inheritance = true);

    /** Generate the matching result under the Parent Token index set
     * @param tree TokensTree pointer
     * @param Search Scope (defined in TokenIdxSet)
     * @param result result token index set
     * @param isPrefix whether a full match is used or only do a prefix match
     * @param kindMask define the result tokens filter, such as only class type is OK
     * @return result token set number
     */
    size_t GenerateResultSet(TokensTree* tree,
                             const wxString& search,
                             int parentIdx,
                             TokenIdxSet& result,
                             bool caseSens = true,
                             bool isPrefix = false,
                             short int kindMask = 0xFFFF);

    /** This function is just like the one above, especially that no Tokentree information is used
     * So, it use the current parser's Tokenstree.
     */
    size_t GenerateResultSet(const wxString& search,
                             const TokenIdxSet& ptrParentID,
                             TokenIdxSet& result,
                             bool caseSens = true,
                             bool isPrefix = false,
                             short int kindMask = 0xFFFF);

    /** Add all children tokens of unnamed union/struct/class */
    bool AddChildrenOfUnnamed(Token* parent, TokenIdxSet& result);

    /** used in CodeCompletion suggestion list to boost the performance*/
    bool LastAISearchWasGlobal() const { return m_LastAISearchWasGlobal; }
    /** The same as above*/
    const wxString& LastAIGlobalSearch() const { return m_LastAIGlobalSearch; }

    /** collect the using namespace xxx directive in the editor specified by searchData*/
    bool ParseUsingNamespace(ccSearchData* searchData, TokenIdxSet& search_scope, int caretPos = -1);

    /** collect function argument, add them to the tokenstree (as temporary tokens)*/
    bool ParseFunctionArguments(ccSearchData* searchData, int caretPos = -1);

    /** parses from the start of function up to the cursor*/
    bool ParseLocalBlock(ccSearchData* searchData, int caretPos = -1);

    /** helper function to split the statement*/
    unsigned int FindCCTokenStart(const wxString& line);
    /** helper function to read the next CCToken */
    wxString GetNextCCToken(const wxString& line, unsigned int& startAt, OperatorType& tokenOperatroType);
    /** helper function to split the statement*/
    wxString GetCCToken(wxString& line, ParserTokenType& tokenType, OperatorType& tokenOperatroType);
    /** helper function to split the statement*/
    void BreakUpInLines(wxString& str, const wxString& original_str, int chars_per_line = -1);

    /** collect the compiler default header file search directories */
    bool AddCompilerDirs(cbProject* project, Parser* parser);
    /** collect compiler predefined preprocessor definition */
    bool AddCompilerPredefinedMacros(cbProject* project, Parser* parser);
    /** collect project (user) defined preprocessor definition*/
    bool AddProjectDefinedMacros(cbProject* project, Parser* parser);

    /** called by AddCompilerDirs() function*/
    const wxArrayString& GetGCCCompilerDirs(const wxString &cpp_compiler);

    /** Load the cashed Tokenstree data from hard disk*/
    bool LoadCachedData(cbProject* project);
    /** save the Tokenstree data to hard disk*/
    bool SaveCachedData(const wxString& projectFilename);

    /** do nothing*/
    void OnThreadStart(wxCommandEvent& event);
    /** do nothing*/
    void OnThreadEnd(wxCommandEvent& event);

    /** Event handler when the batch parse starts, print some log information */
    void OnParserStart(wxCommandEvent& event);

    /** Event handler when the batch parse finishes, print some log information, check the active editor
     * belong to the current parser, if not, do a parser switch
     */
    void OnParserEnd(wxCommandEvent& event);

    /** Event handler when an editor activate, use a delayed timer event to avoid performance issue when
     * user click "close all" menu item
     */
    void OnEditorActivatedTimer(wxTimerEvent& event);

    /** Event handler when clear all parsers, and reparse current project */
    void OnReparseAfterClearTimer(wxTimerEvent& event);

    /** Event handler when an editor activate, *NONE* project is handled here*/
    void OnEditorActivated(EditorBase* editor);

    /** Event handler when an editor closed, if it is the last editor belong to *NONE* project, then
     *  the *NONE* parser will be removed
     */
    void OnEditorClosed(EditorBase* editor);

    /** helper function for statement parsing*/
    bool SkipWhitespaceForward(cbEditor* editor, int& pos);
    /** helper function for statement parsing*/
    bool SkipWhitespaceBackward(cbEditor* editor, int& pos);

    /** Get the Type information of the searchText
     * @param searchText input search text
     * @param searchScope search scope defined in TokenIdxSet format
     * @param result result token specify the Type of searchText
     */
    size_t ResolveActualType(wxString searchText, const TokenIdxSet& searchScope, TokenIdxSet& result);

    /** A statement(expression) now is specified by a ParserComponent queue
     *  We do a match from the left of the queue one by one.
     * @param components expression structure expressed in std::queue<ParserComponent>
     * @param searchScope search scope defined by TokenIdxSet
     * @param caseSense case sensitive match
     * @param isPrefix match type( full match or prefix match)
     * @return result token number
     */
    size_t ResolveExpression(std::queue<ParserComponent> components,
                             const TokenIdxSet& searchScope,
                             TokenIdxSet& result,
                             bool caseSense = true,
                             bool isPrefix = false);

    /** Collect search scopes, add the searchScopes's parent scope
     * @param searchScope input search scope
     * @param actualTypeScope returned search scope
     * @param tree TokensTree pointer
     */
    void CollectSS(const TokenIdxSet& searchScope,
                   TokenIdxSet& actualTypeScope,
                   TokensTree* tree);

    /** add template parameter, get the actual parameter from the formal parameter list
     * @param id template token id
     * @param actualTypeScope search scope
     * @param initialScope resolved result
     * @param tree Tokenstree pointer.
     */
    void AddTemplateAlias(const int& id,
                          const TokenIdxSet& actualTypeScope,
                          TokenIdxSet& initialScope,
                          TokensTree* tree);

    /** resolve template map [formal parameter] to [actual parameter]
     * @param searchStr input Search String
     * @param actualtypeScope Token type(actual parameter)
     * @param initialScope search scope
     */
    void ResolveTemplateMap(const wxString& searchStr,
                            const TokenIdxSet& actualTypeScope,
                            TokenIdxSet& initialScope);

    /** used to solve the overloaded operator functions return type
     * @param tokenOperatorType overloaded operator type, could be [], (), ->
     * @param tokens input tokens set
     * @param tree Tokenstree pointer
     * @param searchScope search scope
     * @param result output result
     */
    void ResolveOpeartor(const OperatorType& tokenOperatorType,
                         const TokenIdxSet& tokens,
                         TokensTree* tree,
                         const TokenIdxSet& searchScope,
                         TokenIdxSet& result);

    /** used to get the correct token in current line, e.g. class A { void test() { | } };
     * @param tokens all current file's function and class
     * @param curLine the line of the current position
     */
    Token* GetTokenFromCurrentLine(const TokenIdxSet& tokens, size_t curLine, size_t fileIdx);

    /** Init cc search member variables */
    void InitCCSearchVariables();

    /** Remove the last function's childrens */
    void RemoveLastFunctionChildren();

private:
    typedef std::pair<cbProject*, Parser*> ParserPair;
    typedef std::list<ParserPair> ParserList;

    ParserList                   m_ParserList;
    Parser                       m_TempParser;
    Parser*                      m_Parser;

    /* CC Search Member Variables => START */
    int                          m_EditorStartWord;
    int                          m_EditorEndWord;
    wxString                     m_CCItems;
    wxArrayString                m_CallTips;
    int                          m_CallTipCommas;
    int                          m_LastFuncTokenIdx; // saved the function token's index, for remove all local variable
    ParserComponent              m_LastComponent;
    cbStyledTextCtrl*            m_LastControl;
    Token*                       m_LastFunction;
    int                          m_LastLine;
    int                          m_LastResult;
    wxString                     m_LastFile;
    wxString                     m_LastNamespace;
    wxString                     m_LastPROC;
    bool                         m_LastAISearchWasGlobal; // true if the phrase for code-completion is empty or partial text (i.e. no . -> or :: operators)
    wxString                     m_LastAIGlobalSearch; // same case like above, it holds the search string
    /* CC Search Member Variables => END */

    wxTimer                      m_TimerEditorActivated;
    wxTimer                      m_TimerReparseAfterClear;
    ClassBrowser*                m_ClassBrowser;
    bool                         m_ClassBrowserIsFloating;
    ProjectSearchDirsMap         m_ProjectSearchDirsMap;
    int                          m_HookId; // project loader hook ID
    wxImageList*                 m_ImageList;

    wxString                     m_LastActivatedFile;
    wxArrayString                m_StandaloneFiles;

    std::map<wxString, wxString> m_TemplateMap;

    DECLARE_EVENT_TABLE()
};

#endif // NATIVEPARSER_H


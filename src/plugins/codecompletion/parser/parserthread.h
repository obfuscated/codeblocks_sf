/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef PARSERTHREAD_H
#define PARSERTHREAD_H

#include <wx/dynarray.h>
#include <wx/event.h>
#include <wx/string.h>
#include <wx/thread.h>

#include <queue>
#include <vector>

#include <cbthreadpool.h>
#include <filemanager.h>
#include <logmanager.h> // F()

#include "cclogger.h"
#include "tokenizer.h"
#include "token.h"
#include "tokentree.h"


struct NameSpace
{
    wxString Name;  // namespace's name
    int StartLine;  // namespace start line (the line contains openbrace)
    int EndLine;    // namespace end line (the line contains closebrace)
};

typedef std::vector<NameSpace> NameSpaceVec;

class ParserBase;

struct ParserThreadOptions
{
    ParserThreadOptions() :

        useBuffer(false),
        fileOfBuffer(),
        parentIdxOfBuffer(-1),
        initLineOfBuffer(1),
        bufferSkipBlocks(false),
        bufferSkipOuterBlocks(false),
        isTemp(false),

        followLocalIncludes(true),
        followGlobalIncludes(true),
        wantPreprocessor(true),
        parseComplexMacros(true),
        platformCheck(true),

        handleFunctions(true),
        handleVars(true),
        handleClasses(true),
        handleEnums(true),
        handleTypedefs(true),

        storeDocumentation(false),

        loader(nullptr)
        {}

    /* useBuffer specifies that we're not parsing a file,  but a temporary
     * buffer. The resulting tokens will be temporary, too,
     * and will be deleted before the next file is parsed.
     */
    bool        useBuffer;
    wxString    fileOfBuffer;
    int         parentIdxOfBuffer;
    int         initLineOfBuffer;
    bool        bufferSkipBlocks;
    bool        bufferSkipOuterBlocks; // classes, namespaces and functions
    bool        isTemp;

    bool        followLocalIncludes;
    bool        followGlobalIncludes;
    bool        wantPreprocessor;
    bool        parseComplexMacros;
    bool        platformCheck;

    bool        handleFunctions;
    bool        handleVars;
    bool        handleClasses;
    bool        handleEnums;
    bool        handleTypedefs;

    bool        storeDocumentation;

    LoaderBase* loader; // if not NULL, load through filemanager (using threads)
};

/** @brief A parser threaded task, which can be assigned to the thread task pool, and run there
  *
  * This class represents a worker threaded task for the Code Completion plug-in, the main task is doing the syntax
  * analysis and add every token to the token tree. The Token tree (sometimes, we call it TokenTree ) is a
  * Patricia tree structure, more details can be seen in token.h and token.cpp. The buffer can  either be loaded
  * from a local file or directly used of a wxString.
  */
class ParserThread : public cbThreadedTask
{
public:
    /** ParserThread constructor.
      * @param parent the parent Parser object which contain the token tree.
      * @param bufferOrFilename it's either the filename to open or a wxString buffer already in memory.
      * @param isLocal determine whether this is a file locally belong to a cbp or in other global paths.
      * @param parserThreadOptions parser thread options, see ParserThreadOptions Class for details.
      * @param tokenTree it is the tree structure holding all the tokens, ParserThread will add every token when it parsed.
      */
    ParserThread(ParserBase*          parent,
                 const wxString&      bufferOrFilename,
                 bool                 isLocal,
                 ParserThreadOptions& parserThreadOptions,
                 TokenTree*           tokenTree);

    /** ParserThread destructor.*/
    virtual ~ParserThread();

    /** Do the main job (syntax analysis) here
      */
    bool Parse();

    /** Get the context "namespace XXX { ... }" directive. It is used to find the initial search scope
      * before CC prompt a suggestion list.
      * Need a critical section locker before call this function!
      * @param buffer  wxString to be parsed.
      * @param result  vector containing all the namespace names.
      */
    bool ParseBufferForNamespaces(const wxString& buffer, NameSpaceVec& result);

    /** Get the context "using namespace XXX" directive. It is used to find the initial search scope
      * before CC prompt a suggestion list.
      * Need a critical section locker before call this function!
      * @param buffer  wxString to be parsed.
      * @param result the wxArrayString contains all the namespace names.
      */
    bool ParseBufferForUsingNamespace(const wxString& buffer, wxArrayString& result);

    wxString GetFilename() { return m_Buffer; } // used in TRACE for debug only

protected:
    /** specify which "class like type" we are handling: struct or class or union*/
    enum EClassType { ctStructure = 0, ctClass = 1, ctUnion = 3 };

    /** Execute() is a virtual function derived from cbThreadedTask class, we should override it here. In
      * the batch parsing mode, a lot of parser threads were generated and executed concurrently, this
      * often happens when user open a project. Every parserthread task will firstly be added to the thread pool, later
      * called automatically from the thread pool.
      */
    int Execute()
    {
        CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

        bool success = Parse();

        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

        return success ? 0 : 1;
    }

    /** Continuously eat the tokens until we meet one of the matching characters
      * @param chars wxString includes all the matching characters
      * @param supportNesting if true, we need to consider the "{" and "}" nesting levels when skipping,
      * @param singleCharToken if true, only single char tokens (like semicolon, brace etc.) are considered (speeds up parsing for queries like this)
      * in this case, the function returned on a match by nesting/brace level preserved.
      */
    wxChar SkipToOneOfChars(const wxString& chars, bool supportNesting = false, bool singleCharToken = true);

    /** actually run the syntax analysis*/
    void DoParse();

    /** skip blocks () [] {} <> */
    void SkipBlock();

    /** skip the template argument*/
    void SkipAngleBraces();

    /** handle include <XXXX> or include "XXXX" directive. This will internally add another
      * parserThead object associate with the included file to parserthread pool
      */
    void HandleIncludes();

    /** handle the statement: #define XXXXX */
    void HandleDefines();

    /** handle the statement: #undef XXXXX */
    void HandleUndefs();

    /** handle the statement:
      * namespace XXX {
      */
    void HandleNamespace();

    /** handle class declaration
      * @param ct specify class like type : struct or enum or class
      */
    void HandleClass(EClassType ct);

    /** handle function declaration or definition
      * @param name function name
      * @param isOperator if true, means it is an operator overload function
      * @param isPointer if true, means it is a function pointer
      */
    void HandleFunction(const wxString& name, bool isOperator = false, bool isPointer = false);

    /** parse for loop arguments:
      * for(int X; ... ; ...)
      */
    void HandleForLoopArguments();

    /** parse arguments like:
      * if(int X = getNumber())
      */
    void HandleConditionalArguments();

    /** handle enum declaration */
    void HandleEnum();

    /** calculate the value assigned to enumerator */
    bool CalcEnumExpression(Token* tokenParent, long& result, wxString& peek);

    /** handle typedef directive */
    void HandleTypedef();

    /** We guess this is a macro usage, so try to expand macro
      * @param id token id in Token tree
      * @param peek macro actual arguments
      */
    void HandleMacroExpansion(int id, const wxString& peek);

    /** eg: class ClassA{...} varA, varB, varC
      * This function will read the "varA, varB, varC"
      * \return True, if token was handled, false, if an unexpected token was read.
      */
    bool ReadVarNames();

    /** handle class names, e.g., the code below
      *typedef class AAA{
      *   int m_a;
      *   int m_b;
      *} BBB,CCC;
      * @param ancestor class name = "AAA"
      * this function reads the following "BBB" and "CCC", and regard them as derived classes of "AAA"
      * @return True, if token was handled, false, if an unexpected token was read.
      */
    bool ReadClsNames(wxString& ancestor);

    /** add one token to the token tree
      * @param kind Token type, see @TokenKind Emun for more details
      * @param name Token name, this is the key string to be searched in the token tree
      * @param line line number of the source file where the current Token locates
      * @param implLineStart if this is a function implementation, it is the start of function body
      * @param implLineEnd like the one above, it is the end line of the function implementation body
      * @param args if the token type is a function, this is the function arguments
      * @param isOperator bool variable to determine an operator override function or not
      * @param isTmpl bool variable to determine it is a function declaration or implementation
      */
    Token* DoAddToken(TokenKind       kind,
                      const wxString& name,
                      int             line,
                      int             implLineStart = 0,
                      int             implLineEnd = 0,
                      const wxString& args = wxEmptyString,
                      bool            isOperator = false,
                      bool            isImpl = false);

    /** return the actual token's base type.
      * e.g.: if the token type string is: "const wxString &"
      * then, the actual token base type is : "wxString"
      */
    wxString GetTokenBaseType();

private:
    /** initialize the m_Buffer, load from local file or use a buffer in memory */
    bool InitTokenizer();

    /** if parent is 0, then global namespace will be used, all tokens under parent scope are searched
      * @param name the search key string
      * @param parent parent token pointer, we only search under the parent token scope
      * @param kindMask filter for the result token, only the specified type of tokens were matched
      */
    Token* TokenExists(const wxString& name, const Token* parent = 0, short int kindMask = 0xFFFF);

    /** Support function overloading */
    Token* TokenExists(const wxString& name, const wxString& baseArgs, const Token* parent, TokenKind kind);

    /** TODO comment here?
      */
    Token* FindTokenFromQueue(std::queue<wxString>& q,
                              Token*                parent = 0,
                              bool                  createIfNotExist = false,
                              Token*                parentIfCreated = 0);

    /** Converts a full argument list (including variable names) to argument types only and strips spaces.
      * eg: if the argument list is like "(const TheClass* the_class, int my_int)"
      * then, the returned argument list is "(const TheClass*,int)"
      * @param args Full argument list
      * @param baseArgs argument types only
      * @return if failed, will return false, so, it must be a variable
      */
    bool GetBaseArgs(const wxString & args, wxString& baseArgs);

    /** Get the class name from a macro */
    wxString GetClassFromMacro(const wxString& macro);

    /** Get the macro's type, if the token is a macro, and saved the type in tokenName */
    bool GetRealTypeIfTokenIsMacro(wxString& tokenName);

    /** Read the <xxxx=yyy, zzz> , and store the value in m_TemplateArgs */
    void GetTemplateArgs();

    /** this function just associate the formal template argument to actual argument
     *  For example, we have such code:
     *  template <typename T> class AAA { T m_aaa;};
     *  AAA<int> bbb;
     *  When handling the "bbb", we need to construct a TemplateMap, we store the map
     *  in the "bbb"'s member variable, which is "T"->"int".
     */
    void ResolveTemplateArgs(Token* newToken);

    /** normally the template argument is all in a wxString, this function just split them
     *  to a wxArrayString, each element is an actual argument.
     */
    wxArrayString GetTemplateArgArray(const wxString& templateArgs, bool remove_gt_lt, bool add_last);

    /** Split formal template argument list*/
    void SplitTemplateFormalParameters(const wxString& templateArgs, wxArrayString& formals);

    /** Split actual template argument list*/
    void SplitTemplateActualParameters(const wxString& templateArgs, wxArrayString& actuals);

    /** associate formal argument with actual template argument*/
    bool ResolveTemplateMap(const wxString& typeStr, const wxArrayString& actuals,
                            std::map<wxString, wxString>& results);

    /** remove template arguments from an expression
      * example: 'std::list<string>' will be separated into 'std::list' and '<string>'
      * @param expr Complete expression with template arguments
      * @param expNoArgs Returned expression without template arguments
      * @param templateArgs The removed template arguments
    */
    void RemoveTemplateArgs(const wxString& expr, wxString &expNoArgs, wxString &templateArgs);

    /** Only for debug */
    bool IsStillAlive(const wxString& funcInfo);

    /** change an anonymous(unnamed) token's name to a human readable name, the m_Str is expect to
     *  store the unnamed token name, for example, for parsing the code
     *  struct
     *  {
     *      int x;
     *      float y;
     *  } abc;
     *  when we first find an anonymous token, which is named _UnnamedStruct1_2, after this function
     *  call, the anonymous token name will becomes struct1_abc, and m_Str will changed from
     *  _UnnamedStruct1_2 to struct1_abc.
     */
    void  RefineAnonymousTypeToken(short int typeMask, wxString alise);

    /** if we regard the parserThread class as a syntax analyzer, then the Tokenizer class is
      * regard as the lexer, which always feeds a wxString by calling m_Tokenizer.GetToken()
      */
    Tokenizer            m_Tokenizer;

    /** a pointer to its parent Parser object, the Parserthread class has two place to communicate
     * with Parser class. m_Parent->ParseFile() when it see an include directive.
     */
    ParserBase*          m_Parent;

    /** a pointer to the token tree, all the tokens will be added to that tree structure */
    TokenTree*           m_TokenTree;

    /** parent Token, for example, when you are parsing in the class member variables, m_LastParent
      * holds a pointer to the current context, which is a token holding class name
      */
    Token*               m_LastParent;

    /** this member define the scope type of member variables, which is: public, private
      * protected or undefined
      */
    TokenScope           m_LastScope;

    /** the file name of the parsing source */
    wxString             m_Filename;

    /** file size, actually the length of the wxString */
    unsigned int         m_FileSize;

    /** source file index on the "file map tree" */
    unsigned int         m_FileIdx;

    /** if true, means we are parsing a file which belongs to a C::B project, otherwise, we are
     *  parsing a file not belong to a C::B project(such as a system header file)
     */
    bool                 m_IsLocal;

    /** This is a very important member variables! It serves as a type stack,
      * eg: parsing the statement: "int wxString const varA;"
      * we determine 'varA' is a token variable, until we searching to the last semicolon.
      * every token before 'varA' will be pushed to m_Str, at this time
      * m_Str = "int wxString const"
      */
    wxString             m_Str;

    /** hold the previous token string */
    wxString             m_LastToken;

    /** parser options, see the ParserThreadOptions structure */
    ParserThreadOptions  m_Options;

    /** for member funcs implementation or a function declaration below
      * eg: int ClassA::FunctionB();
      * EncounteredNamespaces is 'ClassA'
      */
    std::queue<wxString> m_EncounteredNamespaces;

    /** namespaces in function return types
      * for a function declaration below:
      * e.g.: ClassC::returnValue ClassA::FunctionB();
      * m_EncounteredTypeNamespaces is 'ClassC'
      */
    std::queue<wxString> m_EncounteredTypeNamespaces;

    /** globally included namespaces
     *  by "using namespace" statement
     */
    TokenIdxSet          m_UsedNamespacesIds;

    /** TODO: describe me here*/
    wxString             m_LastUnnamedTokenName;

    /** this makes a difference in unnamed class/struct/enum handling */
    bool                 m_ParsingTypedef;

    /**  local file or buffer in memory*/
    bool                 m_IsBuffer;

    /**  a wxString holding the parsing buffer*/
    wxString             m_Buffer;

    /** a pointer indicator or a references*/
    wxString             m_PointerOrRef;

    /** holds current template argument(s) when a template occurs */
    wxString             m_TemplateArgument;

    size_t               m_StructUnionUnnamedCount;

    size_t               m_EnumUnnamedCount;
};

#endif // PARSERTHREAD_H

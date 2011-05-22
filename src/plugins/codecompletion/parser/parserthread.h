/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef PARSERTHREAD_H
#define PARSERTHREAD_H

#include <wx/thread.h>
#include <wx/event.h>
#include <wx/string.h>
#include <wx/dynarray.h>

#include <queue>
#include <vector>

#include <cbthreadpool.h>
#include <filemanager.h>

#include "tokenizer.h"
#include "token.h"

extern const wxString g_UnnamedSymbol;

struct NameSpace
{
    wxString Name;
    int StartLine;
    int EndLine;
};

typedef std::vector<NameSpace> NameSpaceVec;

class Parser;

struct ParserThreadOptions
{
    ParserThreadOptions() :
        useBuffer(false),
        bufferSkipBlocks(false),
        bufferSkipOuterBlocks(false),
        wantPreprocessor(true),
        followLocalIncludes(true),
        followGlobalIncludes(true),
        isTemp(false),
        handleFunctions(true),
        handleVars(true),
        handleClasses(true),
        handleEnums(true),
        handleTypedefs(true),
        parseComplexMacros(true),
        parentOfBuffer(nullptr),
        initLineOfBuffer(1),
        loader(nullptr)
        {}

    /* useBuffer specifies that we're not parsing a file,  but a temporary
     * buffer. The resulting tokens will be temporary, too,
     * and will be deleted before the next file is parsed.
     */
    bool        useBuffer;
    bool        bufferSkipBlocks;
    bool        bufferSkipOuterBlocks; // classes, namespaces and functions
    bool        wantPreprocessor;
    bool        followLocalIncludes;
    bool        followGlobalIncludes;
    bool        isTemp;

    bool        handleFunctions;
    bool        handleVars;
    bool        handleClasses;
    bool        handleEnums;
    bool        handleTypedefs;
    bool        parseComplexMacros;

    wxString    fileOfBuffer;
    Token*      parentOfBuffer;
    int         initLineOfBuffer;

    LoaderBase* loader; // if not NULL, load through filemanager (using threads)
};

/** @brief A parser thread
  *
  * This class represents a worker thread for the Code Completion plug-in, the main task is doing the syntax
  * analysis and add every token to the token tree. The Token tree (sometimes, we call it TokensTree ) is a
  * Patricia tree structure, more details can be seen in token.h and token.cpp. The buffer can  either be loaded
  * from a local file or directly used of a wxString.
  */
class ParserThread : public cbThreadedTask
{
public:
    /** ParserThread constructor.
      * @param parent the parent Parser object which contain the token tree.
      * @param bufferOrFilename if isLocal is true, it's the filename to open, otherwise it is a wxString already in memory.
      * @param isLocal determine whether this is a file in local disk or already in memory.
      * @param parserThreadOptions parser thread options, see ParserThreadOptions Class for details.
      * @param tokensTree it is the tree structure holding all the tokens, ParserThread will add every token when it parsed.
      */
    ParserThread(Parser* parent,
                 const wxString& bufferOrFilename,
                 bool isLocal,
                 ParserThreadOptions& parserThreadOptions,
                 TokensTree* tokensTree);

    /** ParserThread destructor.*/
    virtual ~ParserThread();

    /** Do the main job (syntax analysis) here */
    bool Parse();

    /** Get the context "namespace XXX { ... }" directive. It is used to find the initial search scope
      * before CC prompt a suggestion list.
      * @param buffer  wxString to be parsed.
      * @param result  vector containing all the namespace names.
      */
    bool ParseBufferForNamespaces(const wxString& buffer, NameSpaceVec& result);

    /** Get the context "using namespace XXX" directive. It is used to find the initial search scope
      * before CC prompt a suggestion list.
      * @param buffer  wxString to be parsed.
      * @param result the wxArrayString contains all the namespace names.
      */
    bool ParseBufferForUsingNamespace(const wxString& buffer, wxArrayString& result);

protected:
    /** specify which "class like type" we are handling: struct or class or union*/
    enum EClassType { ctStructure = 0, ctClass = 1, ctUnion = 3 };

    /** Execute() is a virtual function derived from cbThreadedTask class, we should override it here. In
      * the batch parsing mode, a lot of parser threads were generated and executed concurrently, this
      * often happens when user open a project. Every parserthread task will firstly be added to the thread pool, later
      * called automatically from the thread pool.
      */
    int Execute() { return Parse() ? 0 : 1; }

    /** skip until we meet one of the characters in the wxString
      * @param chars wxString specifies all the ending characters
      * @param supportNesting if true, we need to record the "{" and "}" nesting levels when skipping.
      */
    wxChar SkipToOneOfChars(const wxString& chars, bool supportNesting = false);

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
      */
    void HandleFunction(const wxString& name, bool isOperator = false);

    /** handle enum declaration */
    void HandleEnum();

    /** handle typedef directive */
    void HandleTypedef();

    /** handle macro definition
      * @param id token id in Tokenstree
      * @param peek macro body
      */
    void HandleMacro(int id, const wxString& peek);

    /** eg: class ClassA{...} varA, varB, varC
      * This function will read the "varA, varB, varC"
      */
    void ReadVarNames();

    /** handle class names, e.g., the code below
      *typedef class AAA{
      *   int m_a;
      *   int m_b;
      *} BBB,CCC;
      * @param ancestor class name = "AAA"
      * this function reads the following "BBB" and "CCC", and regard them as derived classes of "AAA"
      */
    void ReadClsNames(wxString& ancestor);

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
    Token* DoAddToken(TokenKind kind,
                      const wxString& name,
                      int line,
                      int implLineStart = 0,
                      int implLineEnd = 0,
                      const wxString& args = wxEmptyString,
                      bool isOperator = false,
                      bool isImpl = false);

    /** return the actual token type.
      * e.g.: if the token type string is: "const wxString &"
      * then, the actual token type is : "wxString"
      */
    wxString GetActualTokenType();

private:
    /** initialize the m_Buffer, load from local file or use a buffer in memory */
    bool InitTokenizer();

    /** if parent is 0, then global namespace will be used, all tokens under parent scope are searched
      * @param name the search key string
      * @param parent parent token pointer, we only search under the parent token scope
      * @param kindMask filter for the result token, only the specified type of tokens were matched
      */
    Token* TokenExists(const wxString& name, Token* parent = 0, short int kindMask = 0xFFFF);

    /** Support function overloading */
    Token* TokenExists(const wxString& name, const wxString& baseArgs, Token* parent, TokenKind kind);

    /** Before call this function, *MUST* add a locker
      * e.g. wxCriticalSectionLocker locker(s_TokensTreeCritical);
      */
    Token* FindTokenFromQueue(std::queue<wxString>& q,
                              Token* parent = 0,
                              bool createIfNotExist = false,
                              Token* parentIfCreated = 0);

    /** Converts a full argument list (including variable names) to argument types only and strips spaces.
      * eg: if the argument list is like "(const TheClass* the_class, int my_int)"
      * then, the returned argument list is "(const TheClass*,int)"
      * @param args Full argument list
      * @param baseArgs argument types only
      * @return if faild, will return false, so, it must be a variable
      */
    bool GetBaseArgs(const wxString & args, wxString& baseArgs);

    /** Get the class name from a macro */
    wxString GetClassFromMacro(const wxString& macro);

    /** Get the macro's type, if the token is a macro, and saved the type in tokenName */
    bool GetRealTypeIfTokenIsMacro(wxString& tokenName);

    void GetTemplateArgs();
    void ResolveTemplateArgs(Token* newToken);
    wxArrayString GetTemplateArgArray(const wxString& templateArgs, bool remove_gt_lt, bool add_last);
    void ResolveTemplateFormalArgs(const wxString& templateArgs, wxArrayString& formals);
    void ResolveTemplateActualArgs(const wxString& templateArgs, wxArrayString& actuals);
    bool ResolveTemplateMap(const wxString& typeStr, const wxArrayString& actuals,
                            std::map<wxString, wxString>& results);

    /** Only for debug */
    bool IsStillAlive(const wxString& funcInfo);

    /** if we regard the parserThread class as a syntax analyzer, then the Tokenizer class is
      * regard as the lexer, which always feeds a wxString by calling m_Tokenizer.GetToken()
      */
    Tokenizer            m_Tokenizer;

    /** a pointer to its parent Parser object */
    Parser*              m_Parent;

    /** a pointer to the token tree, all the tokens will be added to that tree structure */
    TokensTree*          m_TokensTree;

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

    /** determine whether we are parsing the local files or buffer already in memory */
    bool                 m_IsLocal;

    /** this is a very important member variables! It serves as a type stack,
      * eg: parsing the statement: "int wxString const varA;"
      * we determine 'varA' is a token variable, until we searching to the last semicolon.
      * every token before 'varA' will be pushed to m_Str, at this time
      * m_Str = "int wxString const"
      */
    wxString             m_Str;

    /**  hold the previous token string */
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
};

#endif // PARSERTHREAD_H

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
#include "tokenizer.h"
#include "token.h"
#include <cbthreadpool.h>
#include <filemanager.h>

#include <queue>

//extern int THREAD_START;
//extern int THREAD_END;
extern int NEW_TOKEN;
extern int FILE_NEEDS_PARSING;

class Parser;
static wxCriticalSection s_mutexListProtection;

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
        loader(0)
        {}

    /* useBuffer specifies that we're not parsing a file,  but a temporary
     * buffer. The resulting tokens will be temporary, too,
     * and will be deleted when the next file is parsed.
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

    LoaderBase* loader; // if not NULL, load through filemanager (using threads)
};
/** @brief A parser thread
  *
  * This class represents a thread for the Code Completion plugin, the main task is doing the syntax
  * analysis and add every token to the token tree.
  */
class ParserThread : public cbThreadedTask
{
    public:
        /** ParserThread constructor.
          * @param parent the parent Parser object which contain the token tree.
          * @param bufferOrFilename if isLocal is true, it's the filename to open, else it is a wxString already in memory buffer.
          * @param isLocal determine whether this is a file in local disk or already in memory
          * @param parserThreadOptions parser thread options, see ParserThreadOptions Class for details.
          * @param tokensTree it is the tree sturcture holding all the tokens, ParserThread will add every token when it parsed.
          */
        ParserThread(Parser* parent,
                    const wxString& bufferOrFilename,
                    bool isLocal,
                    ParserThreadOptions& parserThreadOptions,
                    TokensTree* tokensTree);

        /** ParserThread destructor.*/
        virtual ~ParserThread();

        /** Execute() is a virtual function in cbThreadedTask class, we should override it here. In
          * the batch parseing mode, the parsering taskes will be reserved in the thread pool, and
          * be called automatically from the thread pool one by one.
          */
        int Execute(){ return Parse() ? 0 : 1; }

        /** Do the main job here */
        bool Parse();

        /** Get the context "using namespace XXX" directive
          * @param buffer  wxString to be parsed.
          * @param result the wxArrayString contains all the namespace names.*/
        bool ParseBufferForUsingNamespace(const wxString& buffer, wxArrayString& result);

        /** does nothing atm */
        virtual void* DoRun();

        /** never be used atm*/
        virtual void SetTokens(TokensTree* tokensTree);

        /** return the parsing file name*/
        const wxString& GetFilename() const { return m_Filename; }
    protected:

        /** enum to specify which "class type" we are handling: struct or class or union*/
        enum EClassType { ctStructure = 0, ctClass = 1, ctUnion = 3 };

        /** skip until we meet one chracters in the wxString
          * @param chars wxString specifies all the ending characters
          * @param supportNesting if supportNesting is true, we need to record the "{" and "}" nesting levels when skipping.
          */
        wxChar SkipToOneOfChars(const wxString& chars, bool supportNesting = false);

        /** actually run the syntax analysis*/
        void DoParse();

        /** skip blocks () [] {} <> */
        void SkipBlock();

        /** skip the template argument*/
        void SkipAngleBraces();

        /** handle include <XXXX> or include "XXXX" directive. This will internally add another
          * parserThead object associate with the included file to parserthead pool
          */
        void HandleIncludes();

        /** handle the statement: #define XXXXX */
        void HandleDefines();

        /** handle the proprocessor directive:
          * #ifdef XXX or #endif or ...
          */
        void HandlePreprocessorBlocks(const wxString& preproc);

        /** handle the statement:
          * namespace XXX {
          */
        void HandleNamespace();

        /** handle class declration
          * @param ct specify class like type : struct or enum or class
          */
        void HandleClass(EClassType ct, const wxString& template_args = _T(""));

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
          * @param token macro name
          * @param peek macro body
          */
        void HandleMacro(const wxString & token, const wxString & peek);

        /** eg: class ClassA{...} varA, varB, varC
          * This function will read the "varA, varB, varC"
          */
        void ReadVarNames();

        /** handle class names, eg, the code below
          *typedef class AAA{
          *   int m_a;
          *   int m_b;
          *} BBB,CCC;
          * @param ancestor class name = 'AAA'
          * this function reads the following 'BBB' and 'CCC', and regard them as derived classes of 'AAA'
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
          * @param isTmpl bool variable to determine it is a function declearation or implememtation
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
          * eg: if the token type string is: "const wxString &"
          * then, the actual token type is : "wxString"
          */
        wxString GetActualTokenType();

    private:

        /** no usage atm */
        void Log(const wxString& log);

        /** if parent is 0(which means global namespace), all tokens under parent scope are searched
          * @param name the search key string
          * @param parent parent token pointer, we only search under the parent token scope
          * @param kindMask filter for the result token, only the specified type of tokens were matched
          */
        Token* TokenExists(const wxString& name, Token* parent = 0, short int kindMask = 0xFFFF);

        /** TODO: describe this function */
        Token* FindTokenFromQueue(std::queue<wxString>& q,
                                  Token* parent = 0,
                                  bool createIfNotExist = false,
                                  Token* parentIfCreated = 0);

        /** Converts a full argument list (including variable names) to argument types only and strips spaces.
          * eg: if the argument list is like '(const TheClass* the_class, int my_int)'
          * then, the returned argument list is '(const TheClass*,int)'
          * @param args Full argument list
          * @return Stripped argument list (argument types only)
          */
        wxString GetStrippedArgs(const wxString & args);

        /** if we regard the parserThread class as a syntax anilyzer, then the Tokenizer class is
          * regard as the lexer, which always feeds a wxString by calling m_Tokenizer.GetToken()
          */
        Tokenizer            m_Tokenizer;

        /** a pointer to its parent Parser object */
        Parser*              m_pParent;

        /** a pointer to the token tree, all the tokens will be added to that tree structure */
        TokensTree*          m_pTokensTree;

        /** parent Token, for example, when you are parsing in the class member variables, m_pLastParent
          * holds a pointer to the current context, which is a token holding class name
          */
        Token*               m_pLastParent;

        /** this member define the scope type of member variables, which is: public or private
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
          * eg: ClassC::returnValue ClassA::FunctionB();
          * m_EncounteredTypeNamespaces is 'ClassC'
          */
        std::queue<wxString> m_EncounteredTypeNamespaces;

        /** TODO: describe me here*/
        wxString             m_LastUnnamedTokenName;

        /** this makes a difference in unnamed class/struct/enum handling */
        bool                 m_ParsingTypedef;

        /** handle nesting of #if...#if...#else...#endif...#endif */
        int                  m_PreprocessorIfCount;

        /**  local file or buffer in memory*/
        bool                 m_IsBuffer;

        /**  a wxString holding the parsing buffer*/
        wxString             m_Buffer;

        /** initialze the m_Buffer, load from local file or use a buffer in memory */
        bool InitTokenizer();

        /** a pointer indicator*/
        bool m_IsPointer;
};

#endif // PARSERTHREAD_H

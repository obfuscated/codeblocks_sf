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
  * This class represents a thread for the Code Completion plugin.
  */
class ParserThread : public cbThreadedTask
{
    public:
        /** ParserThread constructor.
          * @param parent the parent Parser object which contain the Token Trie
          * @param bufferOrFilename if isLocal is true, it's the filename to open, else it is a
          * wxString already in memory buffer.
          * @param isLocal determin whether this is a file in local disk or already in memory
          * @param parserThreadOptions parser therad options, see ParserThreadOptions for details.
          * @param tokensTree it is the Trie sturcture holding all the Tokens, ParserThread will add
          * every Token when it parsed.*/
        ParserThread(Parser* parent,
                    const wxString& bufferOrFilename,
                    bool isLocal,
                    ParserThreadOptions& parserThreadOptions,
                    TokensTree* tokensTree);

        /** ParserThread destructor.*/
        virtual ~ParserThread();

        /** Execute() is a virtual function in cbThreadedTask class, we should override it here, when
          * this function will be called automatically from the thread pool.*/
        int Execute(){ return Parse() ? 0 : 1; }

        /** Do the main job */
        bool Parse();

        /**Get the context using namespace XXX directive
          * @param buffer the buffer wxString to be parsed
          * @param result the wxArrayString contains all the namespace names*/
        bool ParseBufferForUsingNamespace(const wxString& buffer, wxArrayString& result);

        /** does nothing atm */
        virtual void* DoRun();

        /** never be used*/
        virtual void SetTokens(TokensTree* tokensTree);

        /** return the parsing file name*/
        const wxString& GetFilename() const { return m_Filename; }
    protected:

        /** enum to specify which statement we are handleing: struct or class or union*/
        enum EClassType { ctStructure = 0, ctClass = 1, ctUnion = 3 };

        /** skip until we meet one chracters in the wxString
          * @param chars wxString specifies all the ending charactors
          * @param supportNesting when running this function, if supportNesting is true, we need to
          * handle the "{" and "}" nesting levels.*/
        wxChar SkipToOneOfChars(const wxString& chars, bool supportNesting = false);

        /** actually run the syntax analysis*/
        void DoParse();

        /** skip blocks () [] {} <> */
        void SkipBlock();

        /** skip the template argument*/
        void SkipAngleBraces();

        /** handle include <XXXX>' or include "XXXX" directive, this will internally add another
          * parserThead associate with the included file to parserthead pool*/
        void HandleIncludes();

        /** handle the statement: #define XXXXX  */
        void HandleDefines();

        /** handle the statement:
          * #ifdef XXX or #endif or ...  */
        void HandlePreprocessorBlocks(const wxString& preproc);

        /** handle the statement:
          * namespace XXX {  */
        void HandleNamespace();

        /** eg: class ClassA{...} varA, varB, varC
          * This function will read the "varA, varB, varC" */
        void ReadVarNames();

        /** handle class names
          * @param ancestor define ?? */
        void ReadClsNames(wxString& ancestor);

        /** handle class declration
          * @param ct specify type : struct or enum or class */
        void HandleClass(EClassType ct, const wxString& template_args = _T(""));

        void HandleMacro(const wxString & token, const wxString & peek);

        /** handle function declearation or definition
          * @param name is the function name
          * @param isOperator if true, means it is a operato override function */
        void HandleFunction(const wxString& name, bool isOperator = false);

        /** handle enum declearation */
        void HandleEnum();

        /** handle typedef directive */
        void HandleTypedef();

        /** add the Token to the token trie
          * @param kind Token type, see TokenKind Emun for more details
          * @param name Token name, this is the keyword to search in the Token trie
          * @param line line number of the source file
          * @param implLineStart if this is a function implementation, it is the start of function body
          * @param implLineEnd like the one above, it is the end line of the function body
          * @param args if the Token type is a function, then this is the function arguments string
          * @param isOperator an operator override function or not
          * @param isTmpl it is a function declearation or implememtation */
        Token* DoAddToken(TokenKind kind,
                          const wxString& name,
                          int line,
                          int implLineStart = 0,
                          int implLineEnd = 0,
                          const wxString& args = wxEmptyString,
                          bool isOperator = false,
                          bool isImpl = false);

        /** return the actual token type, eg:if the token type string is : "const wxString &"
          * then, the actual token type is : "wxString" */
        wxString GetActualTokenType();

    private:

        /** no usage ??? */
        void Log(const wxString& log);

        /** if parent is 0, all tokens are searched
          * @param name the search key word
          * @param parent parent Token pointer, only search under the parent Token
          * @param kindMask filter for the result Token, which type of Token should we search*/
        Token* TokenExists(const wxString& name, Token* parent = 0, short int kindMask = 0xFFFF);

        /** NOTICE: clears the queue too*/
        wxString GetQueueAsNamespaceString(std::queue<wxString>& q);

        /** to do describe this function */
        Token* FindTokenFromQueue(std::queue<wxString>& q,
                                  Token* parent = 0,
                                  bool createIfNotExist = false,
                                  Token* parentIfCreated = 0);

        /** if we regard the parserThread class as a syntax anilyzer, then the Tokenizer class is
          * regard as the lexer, which always return a wxString by calling m_Tokenizer.GetToken() */
        Tokenizer            m_Tokenizer;

        /** a pointer to its parent */
        Parser*              m_pParent;

        /** a pointer to the Token trie, all the Tokens will be add to that trie */
        TokensTree*          m_pTokensTree;

        /** parent Token, for example, you are parsing in the class declearation, then this member
          * keep the pointer to the current class Token */
        Token*               m_pLastParent;

        /** this member define the scope type of member variables, which is: public or private
          * protected or undefined */
        TokenScope           m_LastScope;

        /** the file name of the parsing source */
        wxString             m_Filename;

        /** file size */
        unsigned int         m_FileSize;

        /** source file index on the Token trie */
        unsigned int         m_File;

        /** determine whether we are parsing the local files or buffer already in memory */
        bool                 m_IsLocal;

        /** this is a very important member variables! it serves as a return type stack,
          * eg: int wxString const varA; in this time, we should find the last ';" to determine this
          * is a Token named 'vara', every token before 'varA' will be pushed to m_Str, at this time
          * m_Str = "int wxString const" */
        wxString             m_Str;

        /**  unknow ....?? */
        wxString             m_LastToken;

        /** parser options, see the ParserThreadOptions struc */
        ParserThreadOptions  m_Options;

        /** for member funcs implementation or a function declaration below
          * int ClassA::FunctionB();
          * EncounteredNamespaces will be 'ClassA' */
        std::queue<wxString> m_EncounteredNamespaces;

        /** namespaces in function return types
          * for a function declaration below:
          * ClassC::returnValue ClassA::FunctionB();
          * m_EncounteredTypeNamespaces is 'ClassC' */
        std::queue<wxString> m_EncounteredTypeNamespaces;

        /** unknown ....?? */
        wxString             m_LastUnnamedTokenName;

        /** this makes a difference in unnamed class/struct/enum handling */
        bool                 m_ParsingTypedef;

        /** handle nesting of #if...#if...#else...#endif...#endif */
        int                  m_PreprocessorIfCount;

        /**  unknown ...??? */
        bool                 m_IsBuffer;

        /**  unknown ...???*/
        wxString             m_Buffer;

        /** initialze the m_Buffer member? */
        bool InitTokenizer();
};

#endif // PARSERTHREAD_H

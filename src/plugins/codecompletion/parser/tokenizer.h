/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <wx/string.h>
#include <configmanager.h>
#include <filemanager.h>
#include "token.h"

#include <stack>
#include <list>


/// Enum defines the skip state of the Tokenizer
enum TokenizerState
{
    /** read parentheses as a single token */
    tsNormal        = 0x0000,
    /** read parentheses as token lists, so it return several tokens like '(' ... ')' */
    tsRawExpression = 0x0001
};

/// Enum categorizing C-preprocessor directives
enum PreprocessorType
{
    ptIf                = 0x0001,   //!< #if
    ptIfdef             = 0x0002,   //!< #ifdef
    ptIfndef            = 0x0003,   //!< #ifndef
    ptElif              = 0x0004,   //!< #elif
    ptElifdef           = 0x0005,   //!< #elifdef
    ptElifndef          = 0x0006,   //!< #elifndef
    ptElse              = 0x0007,   //!< #else
    ptEndif             = 0x0008,   //!< #endif
    ptDefine            = 0x0009,   //!< #define
    ptUndef             = 0x000A,   //!< #undef
    ptOthers            = 0x000B    //!< #include #warning and other #xxx
};

/// Whether we need to handle C-preprocessor directives
struct TokenizerOptions
{
    /** do we expand the macros in #if like conditional preprocessor directives */
    bool wantPreprocessor;
    /** do we store the doxygen like document */
    bool storeDocumentation;
};

/** @brief This is just a simple lexer class
 *
 * A Tokenizer does the lexical analysis on a buffer. The buffer is either a wxString loaded from a local source/header file
 * or a wxString already in memory(e.g. the scintilla text buffer). The most public interfaces are two member functions:
 * GetToken() and PeekToken().
 * The former one eats one token string from buffer, the later one does a "look ahead" on the buffer and return
 * the next token string(peeked string). The peeked string will be cached until the next GetToken() call,
 * thus performance can be improved.
 * Also, Tokenizer class does some kind of handling "Macro expansion" on the buffer, from this point of view, this
 * class is a kind of preprocessor
 * Further more, it handles some "conditional preprocessor directives"(like "#if xxx").
 */
class Tokenizer
{
public:
    /** Tokenizer constructor.
     * @param filename the file to be opened.
     */
    Tokenizer(TokenTree* tokenTree, const wxString& filename = wxEmptyString);

    /** Tokenizer destructor.*/
    ~Tokenizer();

    /** Initialize the buffer by opening a file through a loader, this function copy the contents
     *  from the loader's buffer to its own buffer, so after that, we can safely delete the loader
     *  after this function call
     */
    bool Init(const wxString& filename = wxEmptyString, LoaderBase* loader = 0);

    /** Initialize the buffer by directly using a wxString's content.
     *  @param initLineNumber the start line of the buffer, usually the parser try to parse a function
     *  body, so the line information of each local variable tokens are correct.
     *  @param buffer text content used for parsing
     *  @param fileOfBuffer the file name where the buffer come from.
     */
    bool InitFromBuffer(const wxString& buffer, const wxString& fileOfBuffer = wxEmptyString,
                        size_t initLineNumber = 0);

    /** Consume and return the current token string. */
    wxString GetToken();

    /** Do a "look ahead", and return the next token string. */
    wxString PeekToken();

    /** Undo the GetToken. */
    void     UngetToken();

    /** Handle condition preprocessor and store documentation or not */
    void SetTokenizerOption(bool wantPreprocessor, bool storeDocumentation)
    {
        m_TokenizerOptions.wantPreprocessor = wantPreprocessor;
        m_TokenizerOptions.storeDocumentation = storeDocumentation;
    };

    /** Set the Tokenizer skipping options. E.g. sometimes, we need to skip the statement after "=",
     * but sometimes, we should disable this options, see more details on TokenizerState.
     */
    void SetState(TokenizerState state)
    {
        m_State = state;
    };

    /** Return the skipping options value, see TokenizerState for more details. */
    TokenizerState GetState()
    {
        return m_State;
    }

    /** Return the opened files name */
    const wxString& GetFilename() const
    {
        return m_Filename;
    };

    /** Return the line number of the current token string */
    unsigned int GetLineNumber() const
    {
        return m_LineNumber;
    };

    /** Return the brace "{}" level.
     * the value will increase by one when we meet a "{", decrease by one when we meet a "}".
     */
    unsigned int GetNestingLevel() const
    {
        return m_NestLevel;
    };

    /** Save the brace "{" level, the parser might need to ignore the nesting level in some cases */
    void SaveNestingLevel()
    {
        m_SavedNestingLevel = m_NestLevel;
    };

    /** Restore the brace level */
    void RestoreNestingLevel()
    {
        m_NestLevel = m_SavedNestingLevel;
    };

    /** If the buffer is correctly loaded, this function return true. */
    bool IsOK() const
    {
        return m_IsOK;
    };

    /** return the string from the current position to the end of current line, in most case, this
     * function is used in handling #define, use with care outside this class!
     * @param stripUnneeded true if you are going to remove comments and compression spaces(two or
     * more spaces should become one space)
     */
    wxString ReadToEOL(bool stripUnneeded = true);

    /** read a string from '(' to ')', note that inner parentheses considered
     * @param str the returned string
     */
    void ReadParentheses(wxString& str);

    /** Skip from the current position to the end of line */
    bool SkipToEOL(); // use with care outside this class!

    /** Skip to then end of the C++ style comment */
    bool SkipToInlineCommentEnd();

    /** Check whether the Tokenizer reaches the end of the buffer (file) */
    bool IsEOF() const
    {
        return m_TokenIndex >= m_BufferLen;
    }

    /** return true if it is Not the end of buffer */
    bool NotEOF() const
    {
        return m_TokenIndex < m_BufferLen;
    }

    /** Backward buffer replacement for re-parsing
     *
     * @param target the new text going to replace some text on the m_Buffer
     * @param macro if it is a macro expansion, we need to remember the referenced(used) macro token
     * so that we can avoid the recursive macro expansion such as the below code:
     * @code
     * #define X Y
     * #define Y X
     * int X;
     * @endcode
     * http://forums.codeblocks.org/index.php/topic,13384.msg90391.html#msg90391
     *
     * Macro expansion is just replace some characters in the m_Buffer.
     * @code
     * xxxxxxxxxAAAA(u,v)yyyyyyyyy
     *                   ^------ m_TokenIndex (anchor point)
     * @endcode
     * For example, the above is a wxChar Array(m_Buffer), a macro usage "AAAA(u,v)" is detected and
     * need to expanded. We just do a "backward" text replace here.
     * Before replacement, m_TokenIndex points to the next char of ")" in "AAAA(u,v)"(We say it as an
     * anchor point). After replacement, the new buffer becomes:
     * @code
     * xxxNNNNNNNNNNNNNNNyyyyyyyyy
     *    ^ <----------- ^
     *    m_TokenIndex was moved backward
     * @endcode
     * Note that "NNNNNNNNNNNNNNN" is the expanded new text. The m_TokenIndex was moved backward to
     * the beginning of the new added text.
     * If the new text is small enough, then m_Buffer's length do not need to increase.
     * The situation when our m_Buffer's length need to be increased is that the new text
     * is too long, so the buffer before "anchor point" can not hold the new text, this way,
     * m_Buffer's length will adjusted. like below:
     *
     * NNNNNNNNNNNNNNNNNNNNNNyyyyyyyyy
     * ^---m_TokenIndex
     */
    bool ReplaceBufferText(const wxString& target, const Token* macro = 0);

    /** Get expanded text for the current macro usage, then replace buffer for re-parsing
     * @param tk the macro definition token
     * @return true if macro expansion successes, thus buffer is really changed and m_TokenIndex
     * moved backward a bit, and peek status get cleared
     * Both the function like macro or variable like macro usage can be handled in this function.
     */
    bool ReplaceMacroUsage(const Token* tk);

    /** Search "target" in the buffer, return first position in buffer.
     * it is used to find the formal argument in the macro
     * definition body.
     * @param buffer the content
     * @param target the search key
     */
    int GetFirstTokenPosition(const wxString& buffer, const wxString& target)
    {
        return GetFirstTokenPosition(buffer.GetData(), buffer.Len(), target.GetData(), target.Len());
    }

    /** find the sub-string key in the whole buffer, return the first position of the key
     *  @param buffer the content of the string
     *  @param bufferLen length of the string
     *  @param key the search key(sub-string)
     *  @param keyLen the search key length
     */
    int GetFirstTokenPosition(const wxChar* buffer, const size_t bufferLen,
                              const wxChar* key, const size_t keyLen);

    /** KMP find, get the first position, if find nothing, return -1
     *  https://en.wikipedia.org/wiki/Knuth%E2%80%93Morris%E2%80%93Pratt_algorithm
     */
    int KMP_Find(const wxChar* text, const wxChar* pattern, const int patternLen);

    /** a Token is added, associate doxygen style documents(comments before the variables) to the Token */
    void SetLastTokenIdx(int tokenIdx);

protected:
    /** Initialize some member variables */
    void BaseInit();

    /** Do the actual lexical analysis, both GetToken() and PeekToken() will internally call this
     * function. It just move the m_TokenIndex one step forward, and return a lexeme before the
     * m_TokenIndex.
     */
    wxString DoGetToken();

    /** check the m_Lex to see it is an identifier like token, and also if it is a macro usage,
     * replace it.
     * @return true if some text replacement happens in the m_Buffer, otherwise return false
     */
    bool CheckMacroUsageAndReplace();

    /** this function only move the m_TokenIndex and get a lexeme and store it in m_Lex, the m_Lex
     * will be further checked if it is a macro usage or not.
     * @return true if it is an identifier like token. note we need to check an identifier like
     * token is a macro usage.
     */
    bool Lex();

    /** Read a file, and fill the m_Buffer */
    bool ReadFile();

    /** Check the current character is a C-Escape character in a string. */
    bool IsEscapedChar();

    /** Skip character until we meet a ch */
    bool SkipToChar(const wxChar& ch);

    /** skips comments, spaces, preprocessor branch. */
    bool SkipUnwanted();

    /** Skip any "tab" "white-space" */
    bool SkipWhiteSpace();

    /** Skip the C/C++ comment
     * @return true if we do move m_TokenIndex
     * When C comment is handled, m_TokenIndex point to the char AFTER the '/'
     * When C++ comment is handled, m_TokenIndex point to '\n'
     */
    bool SkipComment();

    /** Skip the C preprocessor directive, such as #ifdef xxxx
     *  only the conditional preprocessor directives are handled here, the others such as
     *  #include or #warning and all kinds of ptOthers will passed to Parserthread class
     *  @return true if we do move m_TokenIndex
     */
    bool SkipPreprocessorBranch();

    /** Skip the string literal(enclosed in double quotes) or character literal(enclosed in single quotes).*/
    bool SkipString();

    /** Move to the end of string literal or character literal, the m_TokenIndex will at the
     * closing quote character.
     * @param ch is a character either double quote or single quote
     * @return true if we reach the closing quote character
     */
    bool SkipToStringEnd(const wxChar& ch);

    /** Move to the next character in the buffer */
    bool MoveToNextChar();

    /** Return the current character indexed by m_TokenIndex in the m_Buffer */
    wxChar CurrentChar() const
    {
        if(m_TokenIndex < m_BufferLen)
            return m_Buffer.GetChar(m_TokenIndex);
        return 0;
    };

    /** Do the previous two functions sequentially */
    wxChar CurrentCharMoveNext()
    {
        wxChar c = CurrentChar();
        m_TokenIndex++;
        return c;
    };

    /** Return (peek) the next character */
    wxChar NextChar() const
    {
        if ((m_TokenIndex + 1) >= m_BufferLen) // m_TokenIndex + 1) < 0  can never be true
            return 0;

        return m_Buffer.GetChar(m_TokenIndex + 1);
    };

    /** Return (peek) the previous character */
    wxChar PreviousChar() const
    {
        if (m_TokenIndex > 0 && m_BufferLen > 0) // m_TokenIndex > m_BufferLen can never be true
            return m_Buffer.GetChar(m_TokenIndex - 1);

        return 0;
    };

private:
    /** Check if a ch matches any characters in the wxChar array */
    inline bool CharInString(const wxChar ch, const wxChar* chars) const
    {
        int len = wxStrlen(chars);
        for (int i = 0; i < len; ++i)
        {
            if (ch == chars[i])
                return true;
        }
        return false;
    };

    /** Check the previous char before EOL is a backslash, call this function in the condition that
     * the CurrentChar is '\n', here we have two cases:
     * @code
     * ......\ \ \r \n......
     *               ^--current char, this is DOS style EOL
     * ......\ \ \n......
     *            ^--current char, this is Linux style EOL
     * @endcode
     */
    inline bool IsBackslashBeforeEOL()
    {
        wxChar last = PreviousChar();
        // if DOS line endings, we have hit \r and we skip to \n...
        if (last == _T('\r') && m_TokenIndex >= 2)
            return m_Buffer.GetChar(m_TokenIndex - 2) == _T('\\');
        return last == _T('\\');
    }

    /** #if xxxx, calculate the value of "xxxx" */
    bool CalcConditionExpression();

    /** If the next token string is macro usage, return true */
    bool IsMacroDefined();

    /** handle the statement: #define XXXXX */
    void HandleDefines();

    /** handle the statement: #undef XXXXX */
    void HandleUndefs();

    /** add a macro definition
     *  for example: #define AAA(x,y) x+y
     *  @param name macro name which is "AAA"
     *  @param line the line of the macro definition locates
     *  @param para the formal parameters, which is "(x,y)"
     *  @param substitues the definition which is "x+y"
     */
    void AddMacroDefinition(wxString name, int line, wxString para, wxString substitues);

    /** Skip to the next conditional preprocessor directive branch.
     *  for example:
     *  @code
     *    #if 0
     *      // skipped statements
     *    #elif 0
     *      // skipped statements
     *    #else
     *      // active statements
     *    #endif
     *  @endcode
     *  if we see a "#if 0", we need to jump to the next "#elif xxx"
     */
    void SkipToNextConditionPreprocessor();

    /** Skip to the #endif conditional preprocessor directive.
     *  for example:
     *  @code
     *    #if 1
     *      // active statements
     *    #elif x
     *      // skipped statements
     *    #else
     *      // skipped statements
     *    #endif
     *  @endcode
     *  if we see a "#if 1" branch we need to skip the next two branches, and go to "#endif"
     */
    void SkipToEndConditionPreprocessor();

    /** Get current conditional preprocessor type */
    PreprocessorType GetPreprocessorType();

    /** handle the preprocessor directive:
     * #ifdef XXX or #endif or #if or #elif or...
     * If conditional preprocessor handles correctly, return true, otherwise return false.
     */
    void HandleConditionPreprocessor(const PreprocessorType type);

    /** Split the macro arguments, and store them in results, when calling this function, we expect
     * that m_TokenIndex point to the opening '(', or some spaces before the opening '('.
     * such as below
     * @code
     *    ..... ABC  ( xxx, yyy ) zzz .....
     *             ^--------m_TokenIndex
     * @endcode
     * @param results in the above example, the result contains two items (xxx and yyy)
     * @return false if arguments (the parenthesis) are not found.
     */
    bool SplitArguments(wxArrayString& results);

    /** Get the text after macro expansion
     * @param tk the macro definition token, usually a function like macro definition
     * @param expandedText is an output string
     */
    bool GetMacroExpandedText(const Token* tk, wxString& expandedText);

    /** Just for KMP find */
    void KMP_GetNextVal(const wxChar* pattern, int next[]);

    /** Tokenizer options specify the skipping option */
    TokenizerOptions     m_TokenizerOptions;

    /** the Token tree to store the macro definition */
    TokenTree*           m_TokenTree;

    /** Filename of the buffer */
    wxString             m_Filename;
    /** File index, useful when parsing documentation; \sa SkipComment */
    int                  m_FileIdx;
    /** Buffer content, all the lexical analysis is done on this */
    wxString             m_Buffer;
    /** Buffer length */
    unsigned int         m_BufferLen;

    /** a lexeme string return by the Lex() function, this is a candidate token string, but may be
     * replaced if it is a macro usage
     */
    wxString             m_Lex;

    /** These variables define the current token string and its auxiliary information,
     * such as the token name, the line number of the token, the current brace nest level.
     */
    wxString             m_Token;                //!< token name
    /** index offset in buffer, when parsing a buffer
     * @code
     * ....... namespace std { int a; .......
     *                      ^ --- m_TokenIndex, m_Token = "std"
     * @endcode
     * m_TokenIndex always points to the next character of a valid token, in the above example,
     * it points to the space after "std".
     */
    unsigned int         m_TokenIndex;
    /** line offset in buffer, please note that it is 1 based, not 0 based */
    unsigned int         m_LineNumber;
    /** keep track of block nesting { } */
    unsigned int         m_NestLevel;

    /** Backup the previous Token information */
    unsigned int         m_UndoTokenIndex;
    unsigned int         m_UndoLineNumber;
    unsigned int         m_UndoNestLevel;

    /** Peek token information */
    bool                 m_PeekAvailable;
    wxString             m_PeekToken;
    unsigned int         m_PeekTokenIndex;
    unsigned int         m_PeekLineNumber;
    unsigned int         m_PeekNestLevel;

    /** Saved token info (for PeekToken()), m_TokenIndex will be moved forward or backward when
     *  either DoGetToken() or SkipUnwanted() is called, so we should save m_TokenIndex before it
     *  get modified.
     */
    unsigned int         m_SavedTokenIndex;
    unsigned int         m_SavedLineNumber;
    unsigned int         m_SavedNestingLevel;

    /** bool variable specifies whether the buffer is ready for parsing */
    bool                 m_IsOK;
    /** Tokeniser state specifies the skipping option */
    TokenizerState       m_State;
    /** File loader */
    LoaderBase*          m_Loader;
    /** preprocessor branch stack, if we meet a #if 1, then the value true will be pushed to
     * to the stack, if we skip the #endif, the true value should be popped.
     */
    std::stack<bool>     m_ExpressionResult;


    /** replaced buffer information
     * Here is an example of how macro are expanded
     *
     * @code
     * #define AAA BBBB
     * if we see a macro usage below
     * ..........AAA..................[EOF]
     *              ^
     * '^' is the m_TokenIndex.
     * As you can see, AAA need to be replaced to BBBB, and this is the buffer content after
     * replacement.
     *
     * .........BBBB..................[EOF]
     *          ^   ^
     * @endcode
     * The first '^' is the new m_TokenIndex, we store is as m_Begin, the second '^' is the anchor
     * point, we store it as m_End, normally the content from m_End to [EOF] are unchanged, unless
     * m_Buffer is too small to store the substituted text.
     *
     */
    struct ExpandedMacro
    {
        ExpandedMacro():m_Macro(0)
        {
        };
        /** the token index we begin to parse after replacement */
        unsigned int m_Begin;
        /** the end token index, if beyond this index, we need to pop the buffer */
        unsigned int m_End;
        /** the referenced used macro */
        const Token* m_Macro;
    };

    /** this serves as a macro replacement stack, in the above example, if AAA is replaced by BBBB,
     * we store the macro definition of AAA in the m_ExpandedMacros, and if BBBB is also defined as
     * @code
     * #define BBBB CCC + DDD
     * #define CCC 1
     *
     * When expanding BBBB, the new m_Buffer becomes
     *
     * ....CCC + DDD..................[EOF]
     *     ^
     * here, m_TokenIndex is moved back to the beginning of CCC, and you have the macro replacement
     * stack m_ExpandedMacros like below
     *
     *  The stack becomes
     *  top -> macro BBBB
     *      -> macro AAA
     *
     * next, if CCC is expand to 1, you have this
     *
     * ......1 + DDD..................[EOF]
     *       ^
     *  The stack becomes
     *  top -> macro CCC
     *      -> macro BBBB
     *      -> macro AAA
     * @endcode
     *  if 1 is parsed, and we get a next token '+', the CCC in the top is popped.
     *
     * when we try to expand a macro usage, we can look up in the stack to see whether the macro is
     * already used. C preprocessor don't allow recursively expand a same macro twice.
     * since std::stack does not allow us to loop all its elements, we use std::list.
     */
    std::list<ExpandedMacro>    m_ExpandedMacros;

    /** normally, this record the doxygen style comments for the next token definition
     *  for example, here is a comment
     *  @code
     *      /// description of aaa
     *      int aaa;
     *  @endcode
     *  Then, the "description of aaa" is stored in this variable
     *  when the token "aaa" is added to the TokenTree, it will associate the documen and token
     */
    wxString             m_NextTokenDoc;

    /** store the recent added token index
     *  for example, here is a comment
     *  @code
     *      int aaa; //!< description of aaa
     *  @endcode
     *  the token "aaa" is added to the token tree before reading the description. After that the
     *  token index is stored, and later if we read the "description of aaa", we will attach the
     *  document to the token
     */
    int                  m_LastTokenIdx;

    /** indicates whether we are reading the macro definition
     *  This variable will affect how the doxygen comments will be associated to the Token.
     *  @see Tokenizer::SkipComment for details
     */
    bool m_ReadingMacroDefinition;
};

#endif // TOKENIZER_H

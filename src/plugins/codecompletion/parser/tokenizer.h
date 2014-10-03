/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <wx/string.h>
#include <wx/hashmap.h>
#include <configmanager.h>
#include <filemanager.h>
#include "token.h"

#include <stack>

///Calculate the hash value for a wxString
class HashForWxStringMap
{
public:
    HashForWxStringMap() {}
    unsigned long operator()(const wxString& x) const
    {
        const size_t len = x.length();
        const size_t intWxChar = sizeof(unsigned int) / sizeof(wxChar);
        const size_t shortWxChar = sizeof(unsigned short) / sizeof(wxChar);
        if (len >= intWxChar)
            return size_t((128 ^ len) + *((unsigned int*)((const wxChar*)x + len - intWxChar)));
        else if (len >= shortWxChar)
            return size_t((256 ^ len) + *((unsigned short*)((const wxChar*)x + len - shortWxChar)));
        else
            return size_t((512 ^ len) + *((const wxChar*)x + len - 1));
    }
    HashForWxStringMap& operator=(const HashForWxStringMap&) { return *this; }
};

class EqualForWxStringMap
{
public:
    EqualForWxStringMap() { }
    bool operator()(const wxString& a, const wxString& b) const { return a == b; }
    EqualForWxStringMap& operator=(const EqualForWxStringMap&) { return *this; }
};

///wxString->wxString hash map used to store the macro replacement rules
WX_DECLARE_HASH_MAP(wxString, wxString, HashForWxStringMap, EqualForWxStringMap, wxStringHashMap);

/// Enum defines the skip state of the Tokenizer
enum TokenizerState
{
    tsSkipEqual         = 0x0001,         //!< Skip the assignment statement
    tsSkipQuestion      = 0x0002,         //!< Skip the conditional evaluation statement
    tsSkipSubScrip      = 0x0004,         //!< Skip the array-subscript notation statement

    tsSingleAngleBrace  = 0x0008,         //!< Reserve angle braces
    tsReadRawExpression = 0x0010,         //!< Reserve every chars

    tsSkipNone          = 0x1000,         //!< Skip None
    // convenient masks
    tsSkipUnWanted      = tsSkipEqual    | tsSkipQuestion | tsSkipSubScrip,
    tsTemplateArgument  = tsSkipUnWanted | tsSingleAngleBrace
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
    ptOthers            = 0x0009    //!< #include, #define ...
};

/// Whether we need to handle C-preprocessor directives
struct TokenizerOptions
{
    bool wantPreprocessor;
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
 * Also, Tokenizer class does some kind of handling "Macro replacement" on the buffer to imitate the macro expansion in
 * C-preprocessor, see member-function GetReplacedToken() for details.
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
     *  @param buffer text used for parsing
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

    /** Return the skipping options value, see TokenizerState for more details.*/
    TokenizerState GetState()
    {
        return m_State;
    }

    /** Checking the TokenizerState */
    bool IsSkippingUnwantedTokens() const
    {
        return (m_State == tsSkipUnWanted);
    };

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
     * @param nestBraces true if you still need to count the '{' and '}' levels
     * @param stripUnneeded true if you are going to remove comments and compression spaces(two or
     * more spaces should become one space)
     */
    wxString ReadToEOL(bool nestBraces = true, bool stripUnneeded = true);

    /** read a string from '(' to ')', note that inner parentheses considered */
    void ReadParentheses(wxString& str);

    /** Skip from the current position to the end of line.
     * @param nestBraces if true, we should still counting the brace levels in this function.
     */
    bool SkipToEOL(bool nestBraces = true); // use with care outside this class!

    /** Skip to then end of the C++ style comment */
    bool SkipToInlineCommentEnd();

    /** Add one "replacement rule", this is just a simple way of defining a macro definition.
     * The rule composites two strings: the key string and the value string.
     * When the Tokenizer gets an identifier kind string, it is lookuped in the
     * replacement rules map, if it matches one rule, the rule's value string will be returned instead..
     * Other rules are some function like macro definition,
     * E.g. to replace the "_GLIBCXX_BEGIN_NAMESPACE(std)" to  "namespace std {"
     * We can use: Tokenizer::SetReplacementString(_T("_GLIBCXX_BEGIN_NAMESPACE"), _T("+namespace"));
     * See more details in CodeCompletion::LoadTokenReplacements() function.
     * @param from the matching key string
     * @param to the matching value string
     */
    static void SetReplacementString(const wxString& from, const wxString& to)
    {
        s_Replacements[from] = to;
    };

    /** Remove a macro replacement rule */
    static void RemoveReplacementString(const wxString& from)
    {
        wxStringHashMap::iterator it = s_Replacements.find(from);
        if (it != s_Replacements.end())
            s_Replacements.erase(it);
    };

    /** return the macro replacement map */
    static wxStringHashMap& GetTokenReplacementsMap()
    {
        return s_Replacements;
    }

    static void ConvertToHashReplacementMap(const ConfigManagerContainer::StringToStringMap& map)
    {
        ConfigManagerContainer::StringToStringMap::const_iterator it = map.begin();
        for (; it != map.end(); it++)
            s_Replacements[it->first] = it->second;
    }

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
     * @param target the new text going to replace some other text on the m_Buffer
     *
     * http://forums.codeblocks.org/index.php/topic,13384.msg90391.html#msg90391
     *
     * Macro expansion is just replace some characters in the m_Buffer.
     *
     * xxxxxxxxxAAAA(u,v)yyyyyyyyy
     *                   ^------ m_TokenIndex (anchor point)
     *
     * For example, the above is a wxChar Array(m_Buffer), a macro expansion is needed to replace
     * "AAAA(u,v)" to some new text. We just do a "backward" text replace here. Before the
     * replacement, m_TokenIndex point to the next char of ")" in "AAAA(u,v)" (We say it as an
     * anchor point),
     * so the new buffer becomes:
     *
     * xxxNNNNNNNNNNNNNNNyyyyyyyyy
     *    ^ <----------- ^
     *    m_TokenIndex was moved backward
     *
     * Note that "NNNNNNNNNNNNNNN" is the new text. The m_TokenIndex was moved backward to the
     * beginning of the new added text.
     * if the new text is small enough, then m_Buffer's length do not need to increase.
     * The situation when our m_Buffer's length need to be increased is that the new text
     * is too long, so the buffer before "anchor point" can not hold the new text, this way,
     * m_Buffer's length will adjusted. like below:
     *
     * NNNNNNNNNNNNNNNNNNNNNNyyyyyyyyy
     * ^---m_TokenIndex
     */
    bool ReplaceBufferText(const wxString& target);

    /** Get expanded text for the current macro usage, then replace buffer for re-parsing
     * @param tk the macro definition token
     * @return true if macro expansion successes, thus buffer is really changed and m_TokenIndex
     * moved backward a bit, and peek status get cleared
     * Both the function like macro or variable like macro usage can be handled in this function.
     */
    bool ReplaceMacroUsage(const Token* tk);

    /** Get first token position in buffer */
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

    /** KMP find, get the first position, if find nothing, return -1 */
    int KMP_Find(const wxChar* text, const wxChar* pattern, const int patternLen);

    /** a Token is added, this function also add doxygen style document attached to the Token */
    void SetLastTokenIdx(int tokenIdx);

protected:
    /** Initialize some member variables */
    void BaseInit();

    /** Do the actual lexical analysis, both GetToken() and PeekToken() will internally call this
     * function. It just move the m_TokenIndex one step forward, and return a lexeme before the
     * m_TokenIndex.
     */
    wxString DoGetToken();

    /** Read a file, and fill the m_Buffer */
    bool ReadFile();

    /** Check the current character is a C-Escape character in a string. */
    bool IsEscapedChar();

    /** Skip character until we meet a ch */
    bool SkipToChar(const wxChar& ch);

    /** Skip characters until we meet any characters in a wxChar Array.
     * @param supportNesting handling brace level in this function.
     * @param skipPreprocessor handling preprocessor directive.
     * @param skipAngleBrace if this value is false, we will not do a match of "< > "
     */
    bool SkipToOneOfChars(const wxChar* chars, bool supportNesting = false, bool skipPreprocessor = false, bool skipAngleBrace = true);

    /** Skip the blocks like <>, {}, [], () */
    bool SkipBlock(const wxChar& ch);

    /** skips comments, assignments, preprocessor etc. Eg, sometimes, it will skip the statement after
     * the "=" statement (depend on the TokenizerState value).
     */
    bool SkipUnwanted();

    /** Skip any "tab" "white-space" */
    bool SkipWhiteSpace();

    /** Skip the C/C++ comment
     * @return true if we do move m_TokenIndex
     * When C comment is handled, m_TokenIndex point to the char AFTER the '/'
     * When C++ comment is handled, m_TokenIndex point to '\n'
     */
    bool SkipComment();

    /** Skip the string literal(enclosed in double quotes) or character literal(enclosed in single quotes).*/
    bool SkipString();

    /** Move to the end of string literal or character literal, the m_TokenIndex will at the
     * closing quote character.
     * @param ch is a character either double quote or single quote
     * @return true if we reach the closing quote character
     */
    bool SkipToStringEnd(const wxChar& ch);

    /** Move to the next character in the buffer, amount defines the steps (by default, it is one) */
    bool MoveToNextChar(const unsigned int amount = 1);

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
     * ......\\\r\n......
     *            ^--current char, this is DOS style EOL
     * ......\\\n......
     *          ^--current char, this is Linux style EOL
     */
    inline bool IsBackslashBeforeEOL()
    {
        wxChar last = PreviousChar();
        // if DOS line endings, we have hit \r and we skip to \n...
        if (last == _T('\r') && m_TokenIndex >= 2)
            return m_Buffer.GetChar(m_TokenIndex - 2) == _T('\\');
        return last == _T('\\');
    }

    /** Do the Macro replacement according to the macro replacement rules, generally we have two
     *  kinds of macro replacements. One is the user defined macro replacement rules, which is
     *  hold by static member variable s_Replacements, you can let the tokenizer directly return
     *  a "BBB" string if the origin token is"AAA" , but you have a "AAA" -> "BBB" rule. Another
     *  kind of macro replacement is that the tokenizer try to look up the token tree to see
     *  whether a token is a macro definition, if yes, it can expand the macro. The first kind of
     *  macro replacement happens every time when a tokenizer try to return a token, it is very fast
     *  as s_Replacements is a hash table, and have very limited entries, on the other side, looking
     *  up a token in the token tree takes more time, the later case only happens in some special
     *  cases, such has we are handling #if directives.
     */
    void GetReplacedToken(wxString& str);

    /** Judge what is the first block
      * It will call 'SkipToEOL(false, true)' before returned.
      */
    bool CalcConditionExpression();

    /** If the next token string is macro usage, return true
      * It will call 'SkipToEOL(false, true)' before returned.
      */
    bool IsMacroDefined();

    /** Skip to the next conditional preprocessor directive branch. */
    void SkipToNextConditionPreprocessor();

    /** Skip to the #endif conditional preprocessor directive. */
    void SkipToEndConditionPreprocessor();

    /** Get current conditional preprocessor type */
    PreprocessorType GetPreprocessorType();

    /** handle the preprocessor directive:
     * #ifdef XXX or #endif or #if or #elif or...
     * If conditional preprocessor handles correctly, return true, otherwise return false.
     */
    void HandleConditionPreprocessor(const PreprocessorType type);

    /** Split the macro arguments, and store them in results, when calling this function, we expect
     * that m_TokenIndex point to the opening parthense, or some spaces befor the opening parthense.
     * such as below
     *
     *    ..... ABC  ( xxx, yyy ) zzz .....
     *             ^--------m_TokenIndex
     *
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
    TokenTree*           m_TokenTree;

    /** Filename of the buffer */
    wxString             m_Filename;
    /** File index, useful when parsing documentation; \sa SkipComment */
    int                  m_FileIdx;
    /** Buffer content, all the lexical analysis is done on this */
    wxString             m_Buffer;
    /** Buffer length */
    unsigned int         m_BufferLen;

    /** These variables define the current token string and its auxiliary information,
     * such as the token name, the line number of the token, the current brace nest level.
     */
    wxString             m_Token;                //!< token name
    /** index offset in buffer, when parsing a buffer
     * ....... namespace std { int a; .......
     *                      ^ --- m_TokenIndex, m_Token = "std"
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

    /** Calculate Expression's result, stack for Shunting-yard algorithm */
    std::stack<bool>     m_ExpressionResult;


    /** Save the remaining length from m_TokenIndex to the end of m_Buffer before replace m_Buffer.
     *
     *  ..........AAA..................
     *               ^                 [EOF]
     *
     * It is the length between '^'(m_TokenIndex) and [EOF], sometimes there are not enough spaces
     * to put the substitute before TokenIndex, so the m_Buffer will grows after the replacement:
     *
     *  BBBBBBBBBBBBBBBBBBBBBBBBB..................
     *  ^                        !                 [EOF]
     *
     * Here, m_TokenIndex is moved backward to the beginning of the new substitute
     * string, but the length between '!' and [EOF] should not be changed.
     */
    size_t               m_FirstRemainingLength;

    /** Save the repeat replace buffer count if currently in replace parsing, if it is 0, this means
     * replace buffer does not happen.
     */
    size_t               m_RepeatReplaceCount;

    /** Static member, this is a hash map storing all user defined macro replacement rules */
    static wxStringHashMap s_Replacements;

    wxString             m_NextTokenDoc;
    int                  m_LastTokenIdx;
};

#endif // TOKENIZER_H

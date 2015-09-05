/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>

#include <cctype>
#include <wx/utils.h>
#include <wx/file.h>
#include <wx/msgdlg.h>

#include "globals.h"
#include "logmanager.h"
#include "manager.h"

#include "cclogger.h"
#include "expression.h"
#include "tokenizer.h"
#include "tokentree.h"

#define CC_TOKENIZER_DEBUG_OUTPUT 0

#if defined(CC_GLOBAL_DEBUG_OUTPUT)
    #if CC_GLOBAL_DEBUG_OUTPUT == 1
        #undef CC_TOKENIZER_DEBUG_OUTPUT
        #define CC_TOKENIZER_DEBUG_OUTPUT 1
    #elif CC_GLOBAL_DEBUG_OUTPUT == 2
        #undef CC_TOKENIZER_DEBUG_OUTPUT
        #define CC_TOKENIZER_DEBUG_OUTPUT 2
    #endif
#endif

#ifdef CC_PARSER_TEST
    #define TRACE(format, args...) \
            CCLogger::Get()->DebugLog(F(format, ##args))
    #define TRACE2(format, args...) \
            CCLogger::Get()->DebugLog(F(format, ##args))
    #define TRACE2_SET_FLAG(traceFile)
#else
    #if CC_TOKENIZER_DEBUG_OUTPUT == 1
        #define TRACE(format, args...) \
            CCLogger::Get()->DebugLog(F(format, ##args))
        #define TRACE2(format, args...)
        #define TRACE2_SET_FLAG(traceFile)
    #elif CC_TOKENIZER_DEBUG_OUTPUT == 2
        #define TRACE(format, args...)                                              \
            do                                                                      \
            {                                                                       \
                if (g_EnableDebugTrace)                                             \
                    CCLogger::Get()->DebugLog(F(format, ##args));                   \
            }                                                                       \
            while (false)
        #define TRACE2(format, args...) \
            CCLogger::Get()->DebugLog(F(format, ##args))
        #define TRACE2_SET_FLAG(traceFile) \
            g_EnableDebugTrace = !g_DebugTraceFile.IsEmpty() && traceFile.EndsWith(g_DebugTraceFile)
    #else
        #define TRACE(format, args...)
        #define TRACE2(format, args...)
        #define TRACE2_SET_FLAG(traceFile)
    #endif
#endif

namespace TokenizerConsts
{
    const wxString colon        (_T(":"));
    const wxString colon_colon  (_T("::"));
    const wxString equal        (_T("="));
    const wxString kw_if        (_T("if"));
    const wxString kw_ifdef     (_T("ifdef"));
    const wxString kw_ifndef    (_T("ifndef"));
    const wxString kw_elif      (_T("elif"));
    const wxString kw_elifdef   (_T("elifdef"));
    const wxString kw_elifndef  (_T("elifndef"));
    const wxString kw_else      (_T("else"));
    const wxString kw_endif     (_T("endif"));
    const wxString hash         (_T("#"));
    const wxString tabcrlf      (_T("\t\n\r"));
    const wxString kw_define    (_T("define"));
    const wxString kw_undef     (_T("undef"));
}// namespace TokenizerConsts

// maximun macro replacement stack size
static const size_t s_MaxMacroReplaceDepth = 5;

Tokenizer::Tokenizer(TokenTree* tokenTree, const wxString& filename) :
    m_TokenTree(tokenTree),
    m_Filename(filename),
    m_BufferLen(0),
    m_TokenIndex(0),
    m_LineNumber(1),
    m_NestLevel(0),
    m_UndoTokenIndex(0),
    m_UndoLineNumber(1),
    m_UndoNestLevel(0),
    m_PeekAvailable(false),
    m_PeekTokenIndex(0),
    m_PeekLineNumber(0),
    m_PeekNestLevel(0),
    m_SavedTokenIndex(0),
    m_SavedLineNumber(1),
    m_SavedNestingLevel(0),
    m_IsOK(false),
    m_State(tsNormal),
    m_Loader(0),
    m_NextTokenDoc(),
    m_LastTokenIdx(-1),
    m_ReadingMacroDefinition(false)
{
    m_TokenizerOptions.wantPreprocessor = true;
    m_TokenizerOptions.storeDocumentation = true;

    if (!m_Filename.IsEmpty())
        Init(m_Filename);
}

Tokenizer::~Tokenizer()
{
}

bool Tokenizer::Init(const wxString& filename, LoaderBase* loader)
{
    m_Loader = loader;
    BaseInit();
    if ( filename.IsEmpty() )
    {
        if (m_Filename.IsEmpty())
        {
            TRACE(_T("Init() : Called without filename."));
            return false;
        }
    }
    else
    {
        m_Filename = filename;
        TRACE(_T("Init() : m_Filename='%s'"), m_Filename.wx_str());
#ifdef CC_PARSER_TEST
        TRACE2_SET_FLAG(filename);
        TRACE2(filename);
#endif
    }

    if ( !wxFileExists(m_Filename) )
    {
        TRACE(_T("Init() : File '%s' does not exist."), m_Filename.wx_str());
        return false;
    }

    if ( !ReadFile() )
    {
        TRACE(_T("Init() : File '%s' could not be read."), m_Filename.wx_str());
        return false;
    }

    if (!m_BufferLen)
    {
        TRACE(_T("Init() : File '%s' is empty."), m_Filename.wx_str());
        return false;
    }

    while (m_Filename.Replace(_T("\\"),_T("/"))) { ; }

    m_FileIdx = m_TokenTree->GetFileIndex(m_Filename);

    m_IsOK = true;
    return true;
}

bool Tokenizer::InitFromBuffer(const wxString& buffer, const wxString& fileOfBuffer, size_t initLineNumber)
{
    BaseInit();

    m_BufferLen  = buffer.Length();
    m_Buffer     = buffer + _T(" "); // + 1 => sentinel
    m_IsOK       = true;
    m_Filename   = fileOfBuffer;
    m_LineNumber = initLineNumber;

    while (m_Filename.Replace(_T("\\"),_T("/"))) { ; }

    m_FileIdx = m_TokenTree->GetFileIndex(m_Filename);

    return true;
}

void Tokenizer::BaseInit()
{
    m_BufferLen            = 0;
    m_TokenIndex           = 0;
    m_LineNumber           = 1;
    m_NestLevel            = 0;
    m_UndoTokenIndex       = 0;
    m_UndoLineNumber       = 1;
    m_UndoNestLevel        = 0;
    m_PeekTokenIndex       = 0;
    m_PeekLineNumber       = 0;
    m_PeekNestLevel        = 0;
    m_SavedTokenIndex      = 0;
    m_SavedLineNumber      = 1;
    m_SavedNestingLevel    = 0;
    m_IsOK                 = false;
    m_Buffer.Clear();
    m_NextTokenDoc.clear();
    m_LastTokenIdx         = -1;
}

bool Tokenizer::ReadFile()
{
    bool success = false;
    wxString fileName = wxEmptyString;
    if (m_Loader)
    {
        fileName    = m_Loader->FileName();
        char* data  = m_Loader->GetData();
        m_BufferLen = m_Loader->GetLength();

        // the following code is faster than DetectEncodingAndConvert()
//        DetectEncodingAndConvert(data, m_Buffer);

        // same code as in cbC2U() but with the addition of the string length (3rd param in unicode version)
        // and the fallback encoding conversion
#if wxUSE_UNICODE
        m_Buffer = wxString(data, wxConvUTF8, m_BufferLen + 1); // + 1 => sentinel
        if (m_Buffer.Length() == 0)
        {
            // could not read as utf-8 encoding, try iso8859-1
            m_Buffer = wxString(data, wxConvISO8859_1, m_BufferLen + 1); // + 1 => sentinel
        }
#else
        m_Buffer = wxString(data, m_BufferLen + 1); // + 1 => sentinel
#endif

        success = (data != 0);
    }
    else
    {
        if ( !wxFileExists(m_Filename) )
            return false;

        // open file
        wxFile file(m_Filename);
        if ( !cbRead(file, m_Buffer) )
            return false;

        fileName = m_Filename;
        success  = true;
    }

    m_BufferLen = m_Buffer.Length();

    // add 'sentinel' to the end of the string (not counted to the length of the string)
    m_Buffer += _T(' ');

    return success;
}

// Behaviour consistent with SkipComment
bool Tokenizer::SkipWhiteSpace()
{
    if (CurrentChar() > _T(' ') || IsEOF())
        return false;

    // skip spaces, tabs, etc.
    // don't check EOF when MoveToNextChar already does, also replace isspace() which calls msvcrt.dll
    // with a dirty hack: CurrentChar() <= ' ' is "good enough" here
    while (CurrentChar() <= _T(' ') && MoveToNextChar())
        ;

    return true;
}

// only be called when we are in a C-string,
// To check whether the current character is the real end of C-string
// See SkipToStringEnd() for more details
bool Tokenizer::IsEscapedChar()
{
    // Easy: If previous char is not a backslash, than it's surely not a escaped char
    if (PreviousChar() != '\\')
        return false;
    else
    {
        // check for multiple backslashes, e.g. "\\"
        unsigned int numBackslash = 2; // for sure we have at least two at this point
        while (   m_TokenIndex >= numBackslash
               && ((m_TokenIndex - numBackslash) <= m_BufferLen)
               && (m_Buffer.GetChar(m_TokenIndex - numBackslash) == '\\') )
            ++numBackslash; // another one...

        if ( (numBackslash%2) == 1) // number of backslashes (including current char) is odd
            return false;           // eg: "\""
        else                        // number of backslashes (including current char) is even
            return true;            // eg: "\\""
    }
}

// expect we are not in a C-string
bool Tokenizer::SkipToChar(const wxChar& ch)
{
    // skip everything until we find ch
    while (CurrentChar() != ch && MoveToNextChar())  // don't check EOF when MoveToNextChar already does
        ;

    return NotEOF();
}

//  For example: X"ABCDEFG\"HIJKLMN"Y
//  We are now at A, and would skip to trailing char (the '"' before Y)
//  The double quote before H is a "C-escaped-character", We shouldn't quite from that
bool Tokenizer::SkipToStringEnd(const wxChar& ch)
{
    while (true)
    {
        // go to candidate of a close quote char
        while (CurrentChar() != ch && MoveToNextChar()) // don't check EOF when MoveToNextChar already does
            ;

        if (IsEOF())
            return false;

        // check to see if the close quote char is an escape char
        if (IsEscapedChar())
            MoveToNextChar(); // if true, skip the close quote char, and continue
        else
            break;            // if false, it is the closing quote
    }
    return true;
}

bool Tokenizer::MoveToNextChar()
{
    ++m_TokenIndex;
    if (IsEOF())
    {
        m_TokenIndex = m_BufferLen;
        return false;
    }

    if (PreviousChar() == _T('\n'))
        ++m_LineNumber;
    return true;
}

// return true if we really skip a string, that means m_TokenIndex has changed.
bool Tokenizer::SkipString()
{
    if (IsEOF())
        return false;

    const wxChar ch = CurrentChar();
    if (ch == _T('"') || ch == _T('\''))
    {
        // this is the case that match is inside a string!
        MoveToNextChar();
        SkipToStringEnd(ch);
        MoveToNextChar();
        return true;
    }

    return false;
}

wxString Tokenizer::ReadToEOL(bool stripUnneeded)
{
    m_ReadingMacroDefinition = true;
    if (stripUnneeded)
    {
        // there are many cases when reading the #define xxx *****
        // it can have such comments like
        //
        // a macro definition has multiply physical lines
        // #define xxx yyy BACKSLASH
        // zzz
        //
        // a macro defintion has mixed C comments
        // #define xxx /*aaa*/ yyy /*bbb*/ zzz
        //
        // a macro definition has ending C++ comments
        // #define xxx yyy zzz // aaa bbb

        TRACE(_T("%s : line=%u, CurrentChar='%c', PreviousChar='%c', NextChar='%c'"),
              wxString(__PRETTY_FUNCTION__, wxConvUTF8).wc_str(), m_LineNumber, CurrentChar(),
              PreviousChar(), NextChar());

        static const size_t maxBufferLen = 4094;
        wxChar buffer[maxBufferLen + 2];
        wxChar* p = buffer;
        wxString str;

        // loop all the physical lines when reading macro definition
        for (;;)
        {
            // this while statement end up in one physical EOL '\n'
            while (NotEOF() && CurrentChar() != _T('\n'))
            {

                // a macro definition has ending C++ comments, we should stop the parsing before
                // the "//" chars, so that the doxygen document can be added correctly to previous
                // added Macro definition token.
                if(CurrentChar() == _T('/') && NextChar() == _T('/'))
                    break;

                // Note that SkipComment() function won't skip the '\n' after comments
                while (SkipComment())
                    ;

                // if we see a '\', it could be the EOL of a physical line
                const wxChar ch = CurrentChar();
                if (ch == _T('\n'))
                    break;

                // if we see two spaces in the buffer, we should drop the second one. Note, if the
                // first char is space, we should always save it to buffer, this is to distinguish
                // a function/variable like macro definition, e.g.
                // #define MYMACRO(A)  ...   -> function like macro definition
                // #define MYMACRO (A)  ...  -> variable like macro definition, note a space before '('
                if (ch <= _T(' ') && p > buffer && *(p - 1) == ch)
                {
                    MoveToNextChar();
                    continue;
                }

                *p = ch;
                ++p;

                if (p >= buffer + maxBufferLen)
                {
                    str.Append(buffer, p - buffer);
                    p = buffer;
                }

                MoveToNextChar();
            }

            // check to see it is a logical EOL, some long macro definition contains a backslash-newline
            if (!IsBackslashBeforeEOL() || IsEOF())
                break; //break the outer for loop
            else
            {
                //remove the backslash-newline and goto next physical line
                while (p > buffer && *(--p) <= _T(' '))
                    ;
                MoveToNextChar();
            }
        }
        // remove the extra spaces in the end of buffer
        while (p > buffer && *(p - 1) <= _T(' '))
            --p;

        if (p > buffer)
            str.Append(buffer, p - buffer);

        TRACE(_T("ReadToEOL(): (END) We are now at line %u, CurrentChar='%c', PreviousChar='%c', NextChar='%c'"),
              m_LineNumber, CurrentChar(), PreviousChar(), NextChar());
        TRACE(_T("ReadToEOL(): %s"), str.wx_str());
        m_ReadingMacroDefinition = false;
        return str;
    }
    else
    {
        const unsigned int idx = m_TokenIndex;
        SkipToEOL();
        m_ReadingMacroDefinition = false;
        return m_Buffer.Mid(idx, m_TokenIndex - idx);
    }
}

// there are some rules which make the parentheses very compact
// 1, here should be no space before ',' , '*' and '&', but should have a space after that
// 2, two or more spaces becomes one space
// 3, no spaces after the '(' and before ')'
// 4, there need a space to separate to identifier. see the "unsigned" and "int" below
// 5, there is a space before and after the "=" char
// "(  unsigned  int   *  a, float  & b  )" -> "(unsigned int* a, float& b)"
// "( int a  [ 10 ], float ** b )" -> "(int a [10], float** b)"
// "( int a  =   5)" -> "(int a = 5)"

void Tokenizer::ReadParentheses(wxString& str)
{
    // brace level of '(' and ')', the str is currently "(", so the level is 1
    int level = 1;

    while (NotEOF())
    {
        wxString token = DoGetToken();

        if (token == _T("("))
        {
            ++level;
            str << token;
        }
        else if (token == _T(")"))
        {
            --level;
            str << token;
            if (level == 0)
                break;

        }
        else if (token == _T("*") || token == _T("&") )
        {
            str << token;
        }
        else if (token == _T("=")) // space before and after "="
        {
            str << _T(" ") << token << _T(" ");
        }
        else if (token == _T(",")) // space after ","
        {
            str << token << _T(" ");
        }
        else
        {
            // there is a space between two identifier like token
            wxChar nextChar = token[0];
            wxChar lastChar = str.Last();
            if (   (wxIsalpha(nextChar) || nextChar == _T('_'))
                && (wxIsalnum(lastChar) || lastChar == _T('_')) )
            {
                str << _T(" ") << token;
            }
            else // otherwise, no space is needed
                str << token;
        }

        if (level == 0)
            break;
    }//while (NotEOF())

}

bool Tokenizer::SkipToEOL()
{
    TRACE(_T("%s : line=%u, CurrentChar='%c', PreviousChar='%c', NextChar='%c'"),
          wxString(__PRETTY_FUNCTION__, wxConvUTF8).wc_str(), m_LineNumber, CurrentChar(),
          PreviousChar(), NextChar());

    // skip everything until we find EOL
    for (;;)
    {
        while (NotEOF() && CurrentChar() != '\n')
        {
            if (CurrentChar() == '/' && NextChar() == '*')
            {
                SkipComment();
                if (CurrentChar() == _T('\n'))
                    break;
            }

            MoveToNextChar();
        }

        if (!IsBackslashBeforeEOL() || IsEOF())
            break;
        else
            MoveToNextChar();
    }

    TRACE(_T("SkipToEOL(): (END) We are now at line %u, CurrentChar='%c', PreviousChar='%c', NextChar='%c'"),
          m_LineNumber, CurrentChar(), PreviousChar(), NextChar());

    return NotEOF();
}

bool Tokenizer::SkipToInlineCommentEnd()
{
    TRACE(_T("%s : line=%u, CurrentChar='%c', PreviousChar='%c', NextChar='%c'"),
          wxString(__PRETTY_FUNCTION__, wxConvUTF8).wc_str(), m_LineNumber, CurrentChar(),
          PreviousChar(), NextChar());

    // skip everything until we find EOL
    while (true)
    {
        SkipToChar(_T('\n'));
        if (!IsBackslashBeforeEOL() || IsEOF())
            break;
        else
            MoveToNextChar();
    }

    TRACE(_T("SkipToInlineCommentEnd(): (END) We are now at line %u, CurrentChar='%c', PreviousChar='%c',")
          _T(" NextChar='%c'"), m_LineNumber, CurrentChar(), PreviousChar(), NextChar());

    return NotEOF();
}

bool Tokenizer::SkipBlock(const wxChar& ch)
{
    // skip blocks () [] {} <>
    wxChar match;
    switch (ch)
    {
        case '(': match = ')'; break;
        case '[': match = ']'; break;
        case '{': match = '}'; break;
        case '<': match = '>'; break;
        default : return false;
    }

    MoveToNextChar();
    int nestLevel = 1; // counter for nested blocks (xxx())
    while (NotEOF())
    {
        while (SkipWhiteSpace() || SkipString() || SkipComment())
            ;

        if (CurrentChar() == ch)
            ++nestLevel;
        else if (CurrentChar() == match)
            --nestLevel;

        MoveToNextChar();

        if (nestLevel == 0)
            break;
    }

    return NotEOF();
}

// if we really move forward, return true, which means we have the new m_TokenIndex
// if we stay here, return false
bool Tokenizer::SkipComment()
{
    if (IsEOF())
        return false;

    bool cstyle;            // C or C++ style comments

    //check the comment prompt
    if (CurrentChar() == '/')
    {
        if      (NextChar() == '*')
            cstyle = true;
        else if (NextChar() == '/')
            cstyle = false;
        else
            return false; // Not a comment, return false;
    }
    else
        return false;     // Not a comment, return false;

    MoveToNextChar();     // Skip the comment prompt
    MoveToNextChar();

    bool isDoc = false;
    if (m_TokenizerOptions.storeDocumentation)
    {
        isDoc = (CurrentChar() == '!');	//	"/*!" or "//!"

        if (!isDoc && cstyle) //  "/*" + ?
            isDoc = (CurrentChar() == '*' && NextChar() != '/'); //	"/**" but not "/**/" and not //*

        if (!isDoc && !cstyle) // "//" + ?
            isDoc = (CurrentChar() == '/' && NextChar() != '/'); // "///" but not "////"
    }

    // m_ExpressionResult.empty() == true means we are running the Tokenizer in global level, no
    // preprocessor branch is entered.
    // m_ExpressionResult.top() == true means we are in the top true branch of the conditional
    // preprocessor directives, those scopes covers the valid doxygen comments. E.g.
    // #if 1
    //     /** valid documents */
    //     int a;
    // #else
    //     /** invalid documents */
    //     int a;
    // #endif
    if (isDoc)
        isDoc = m_ExpressionResult.empty() || m_ExpressionResult.top();

    TRACE(_T("SkipComment() : Start from line = %u"), m_LineNumber);

    if (!isDoc)
    {
        // Here, we are in the comment body
        while (true)
        {
            if (cstyle) // C style comment
            {
                SkipToChar('*');
                if (NextChar() == '/') // end of a C style comment
                {
                    MoveToNextChar();
                    MoveToNextChar();
                    break;
                }
                if (!MoveToNextChar())
                    break;
            }
            else        // C++ style comment
            {
                TRACE(_T("SkipComment() : Need to call SkipToInlineCommentEnd() here at line = %u"), m_LineNumber);
                SkipToInlineCommentEnd();
                break;
            }
        }
    }
    else
    {
        //Inside documentation body
        wxString doc;
        MoveToNextChar();    // Skip '!' or '*' or '/'
        wxChar c = CurrentChar();

        int lineToAppend = -1;

        if (c == _T('<'))
        {  // documentation for already added token - //!< or /*!< or something like this
            MoveToNextChar();
            c = CurrentChar();
            lineToAppend = m_LineNumber;
        }

        if (cstyle)
        {
            while (true)
            {
                c = CurrentChar();
                if (c == '*' && NextChar() == '/') //End of block comment
                {
                    MoveToNextChar();   // eat '/'
                    MoveToNextChar();
                    break;
                }
                else
                {
                    doc += c; // Appending char by char may be slow
                    if (!MoveToNextChar())
                        break;
                }
            }
        }
        else	// C++ style comment
        {
            while (true)
            {
                c = CurrentChar();
                if (c == '\n')
                {
                    if (IsBackslashBeforeEOL())
                    {
                        MoveToNextChar();
                        continue;
                    }
                    else
                        break;
                }
                else
                {
                    doc += c;
                    if (!MoveToNextChar())
                        break;
                }
            }
        }

        if (doc.size() > 0) // don't push empty strings
        {
            doc += _T('\n');

            if (lineToAppend >= 0) // we have document after the token place
            {
                // if we are reading the macro definition(m_ReadingMacroDefinition==true)
                // then don't run the AppendDocumentation() to the previous Token. E.g.
                //     int aaa;
                //     #define FOO /*!< comments */
                // That is: we read the "comments", but don't attach to Token aaa, instead, we
                // translate this kind of comments as "document before the token", at this time
                // the Token FOO is not constructed yet, but once it is constructed, the "comments"
                // will attach to Token FOO
                if (m_ReadingMacroDefinition)
                    m_NextTokenDoc = doc + m_NextTokenDoc;
                else
                {
                    // we need to attach the document to recent added Token
                    if (m_LastTokenIdx != -1)
                        m_TokenTree->AppendDocumentation(m_LastTokenIdx, m_NextTokenDoc + doc);

                    m_NextTokenDoc.clear();
                }
            }
            else
            {
                // Find next token's line:
                // At first skip whitespace
                while (SkipWhiteSpace())
                    ;

                // Maybe there is another comment?
                // Recursive call
                bool skipped = SkipComment();

                //!
                /*!
                */
                if (!cstyle && skipped)
                    doc = _T("@brief ") + doc + _T('\n');

                m_NextTokenDoc = doc + m_NextTokenDoc;
            }
        }
    }
    return true;
}

bool Tokenizer::SkipPreprocessorBranch()
{
    wxChar c = CurrentChar();
    const unsigned int startIndex = m_TokenIndex;

    if (c == _T('#'))
    {
        // this use the Lex() to fetch an unexpanded token
        const PreprocessorType type = GetPreprocessorType();
        if (type != ptOthers)
        {
            HandleConditionPreprocessor(type);
            c = CurrentChar();
            return true;
        }
        else
            return false;
    }

    if (startIndex != m_TokenIndex)
        return true;
    else
        return false;
}

bool Tokenizer::SkipUnwanted()
{
    while (SkipWhiteSpace() || SkipComment() || SkipPreprocessorBranch())
        ;

    return NotEOF();
}

wxString Tokenizer::GetToken()
{
    m_UndoTokenIndex = m_TokenIndex;
    m_UndoLineNumber = m_LineNumber;
    m_UndoNestLevel  = m_NestLevel;

    if (m_PeekAvailable)
    {
        m_TokenIndex = m_PeekTokenIndex;
        m_LineNumber = m_PeekLineNumber;
        m_NestLevel  = m_PeekNestLevel;
        m_Token      = m_PeekToken;
    }
    else
    {
        if (SkipUnwanted())
        {
            m_Token = DoGetToken();// this function always return a fully expanded token
            if (m_Token == _T("(") && m_State^tsRawExpression)
                ReadParentheses(m_Token);
        }
        else
            m_Token.Clear();
    }

    m_PeekAvailable = false;

    return m_Token;
}

wxString Tokenizer::PeekToken()
{
    if (!m_PeekAvailable)
    {
        // suppose we have such string buffer
        //   ... x1 x2 x3 x4 x5 x6 ....
        //            ^-----------------------m_TokenIndex point to the end of current token "x2"
        // now, ideally, when we run the PeekToken, we hopefully want to get the below status
        //   ... x1 x2 x3 x4 x5 x6 ....
        //            ^-----------------------m_TokenIndex point to the end of current token "x2"
        //               *--------------------m_PeekTokenIndex point to the end of next token "x3"
        // Note that DoGetToken() mostly manipulate on the m_TokenIndex, so after the m_TokenIndex
        // goes one step, we need to restore its position, so that m_Saved... vars are used to save
        // old m_TokenIndex values before we call the DoGetToken();
        // NOTE: The m_Saved... vars will be reset to the correct position as necessary when a
        // ReplaceBufferText() is done.
        m_SavedTokenIndex   = m_TokenIndex;
        m_SavedLineNumber   = m_LineNumber;
        m_SavedNestingLevel = m_NestLevel;

        if (SkipUnwanted())
        {
            m_PeekToken = DoGetToken();
            if (m_PeekToken == _T("(") && m_State^tsRawExpression)
                ReadParentheses(m_PeekToken);
        }
        else
            m_PeekToken.Clear();

        m_PeekAvailable     = true; // Set after DoGetToken() to avoid recursive PeekToken() calls.
        m_PeekTokenIndex    = m_TokenIndex;
        m_PeekLineNumber    = m_LineNumber;
        m_PeekNestLevel     = m_NestLevel;

        m_TokenIndex        = m_SavedTokenIndex;
        m_LineNumber        = m_SavedLineNumber;
        m_NestLevel         = m_SavedNestingLevel;
    }

    return m_PeekToken;
}
/* peek is always available when we run UngetToken() once, actually the m_TokenIndex is moved
 * backward one step. Note that the m_UndoTokenIndex value is not updated in this function, which
 * means you are not allowed to run this function twice.
 */
void Tokenizer::UngetToken()
{
    // NOTE: Test below could be true even if we haven't run UngetToken() before (eg, if we have just
    // reset the undo token)
    // if (m_TokenIndex == m_UndoTokenIndex) // this means we have already run a UngetToken() before.
    //     return;

    m_PeekTokenIndex = m_TokenIndex;
    m_PeekLineNumber = m_LineNumber;
    m_PeekNestLevel  = m_NestLevel;
    m_TokenIndex     = m_UndoTokenIndex;
    m_LineNumber     = m_UndoLineNumber;
    m_NestLevel      = m_UndoNestLevel;
    m_PeekToken      = m_Token;
    m_PeekAvailable  = true;
}

/* this function always start from the index of m_TokenIndex
 *               int X;MYMACRO;X = 1;
 *  Token:            _^
 *  Undo:            ^
 *  After that, we get a lexeme of name "MYMACRO", then Token index is move forward
 *               int X;MYMACRO;X = 1;
 *  Token:             _______^
 *  Undo:              ^
 *
 * Please note that if MYMACRO need to be replaced (macro expansion), we should replace the text
 * then run one step again, see the details in:
 * http://forums.codeblocks.org/index.php/topic,18315.msg125579.html#msg125579
 */
wxString Tokenizer::DoGetToken()
{
    while(true)
    {
        SkipUnwanted();
        bool identifier = Lex();
        // only check macro usage on identifier like tokens
        if (identifier)
        {
            bool replaced = CheckMacroUsageAndReplace();
            // replaced is true means macro expansion happens, so we need to call Lex()
            if (replaced)
                continue;
            else
                return m_Lex;
        }
        else
            return m_Lex;
    }
}

bool Tokenizer::Lex()
{
    int start = m_TokenIndex;
    // identifier like token
    bool identifier = false;

    wxChar c = CurrentChar();

    if (c == '_' || wxIsalpha(c))
    {
        // keywords, identifiers, etc.

        // operator== is cheaper than wxIsalnum, also MoveToNextChar already includes IsEOF
        while (    ( (c == '_') || (wxIsalnum(c)) )
               &&  MoveToNextChar() )
            c = CurrentChar(); // repeat

        if (IsEOF())
        {
            m_Lex = wxEmptyString;
            return false;
        }

        identifier = true;
        m_Lex = m_Buffer.Mid(start, m_TokenIndex - start);
    }
#ifdef __WXMSW__ // This is a Windows only bug!
    // fetch non-English characters, see more details in: http://forums.codeblocks.org/index.php/topic,11387.0.html
    else if (c == 178 || c == 179 || c == 185)
    {
        m_Lex = c;
        MoveToNextChar();
    }
#endif
    else if (wxIsdigit(c))
    {
        // numbers
        while (NotEOF() && CharInString(CurrentChar(), _T("0123456789.abcdefABCDEFXxLl")))
            MoveToNextChar();

        if (IsEOF())
        {
            m_Lex = wxEmptyString;
            return false;
        }

        m_Lex = m_Buffer.Mid(start, m_TokenIndex - start);
    }
    else if ( (c == '"') || (c == '\'') )
    {
        SkipString();
        //Now, we are after the end of the C-string, so return the whole string as a token.
        m_Lex = m_Buffer.Mid(start, m_TokenIndex - start);
    }
    else if (c == ':')
    {
        if (NextChar() == ':')
        {
            MoveToNextChar();
            MoveToNextChar();
            // this only copies a pointer, but operator= allocates memory and does a memcpy!
            m_Lex.assign(TokenizerConsts::colon_colon);
        }
        else
        {
            MoveToNextChar();
            m_Lex.assign(TokenizerConsts::colon);
        }
    }
    else if (c == '=')
    {
        wxChar next = NextChar();
        if (next == _T('=') || next == _T('!') || next == _T('>') || next == _T('<'))
        {
            MoveToNextChar();
            MoveToNextChar();
            m_Lex = m_Buffer.Mid(start, m_TokenIndex - start);
        }
        else
        {
            MoveToNextChar();
            // this only copies a pointer, but operator= allocates memory and does a memcpy!
            m_Lex.assign(TokenizerConsts::equal);
        }
    }
    else
    {
        if      (c == '{')
            ++m_NestLevel;
        else if (c == '}')
            --m_NestLevel;

        m_Lex = c;
        MoveToNextChar();
    }

    // when m_TokenIndex exceeds the anchor point where we start the macro replacement, we should
    // pop the remembered macro usage
    while ( !m_ExpandedMacros.empty() && m_ExpandedMacros.front().m_End < m_TokenIndex)
        m_ExpandedMacros.pop_front();

    return identifier;
}

bool Tokenizer::CheckMacroUsageAndReplace()
{
    const int id = m_TokenTree->TokenExists(m_Lex, -1, tkMacroDef);
    if (id != -1)
    {
        const Token* token = m_TokenTree->at(id);
        if (token)
            return ReplaceMacroUsage(token);// either object like macro or function like macro can be handled
    }
    return false;
}

bool Tokenizer::CalcConditionExpression()
{
    // need to force the tokenizer to read raw expression
    const TokenizerState oldState = m_State;
    m_State = tsRawExpression; // parentheses are not returned as a single token

    // we need to know what is the end of the preprocessor directive by calling SkipToEOL(), which
    // go to the end of the current logical line. After that, we rewind the m_TokenIndex and parse
    // the tokens again until we pass the EOL.
    const unsigned int undoIndex = m_TokenIndex;
    const unsigned int undoLine = m_LineNumber;
    SkipToEOL();
    // length from the current m_TokenIndex to the End
    const unsigned int untouchedBufferLen = m_BufferLen - m_TokenIndex;
    m_TokenIndex = undoIndex;
    m_LineNumber = undoLine;

    Expression exp;

    // rescan happens once macro expansion happens (m_TokenIndex rewind)
    while (m_TokenIndex < m_BufferLen - untouchedBufferLen)
    {

        // we run the while loop explicitly before calling the DoGetToken() function.
        // if m_TokenIndex pass the EOL, we should stop the calculating of preprocessor
        // condition
        while (SkipWhiteSpace() || SkipComment())
            ;

        if (m_TokenIndex >= m_BufferLen - untouchedBufferLen)
            break;


        wxString token = DoGetToken();
        // token are generally the fully macro expanded tokens, so mostly they are some numbers,
        // unknown tokens are pushed to Infix express, and later they will be seen as 0.

        if(token.Len() > 0
           && (token[0] == _T('_') || wxIsalnum(token[0]))) // identifier like token
        {

            if (token == _T("defined"))
            {
                if (IsMacroDefined())
                    exp.AddToInfixExpression(_T("1"));
                else
                    exp.AddToInfixExpression(_T("0"));
            }
            else
                exp.AddToInfixExpression(token); // not a macro usage token
        }
        else if (token.StartsWith(_T("0x"))) // hex value
        {
            long value;
            if (token.ToLong(&value, 16))
                exp.AddToInfixExpression(wxString::Format(_T("%ld"), value));
            else
                exp.AddToInfixExpression(_T("0"));
        }
        else
            exp.AddToInfixExpression(token); // other kinds of tokens, such as operators, numbers
    }

    // reset tokenizer's functionality
    m_State = oldState;

    exp.ConvertInfixToPostfix();
    if (exp.CalcPostfix())
    {
        TRACE(_T("CalcConditionExpression() : exp.GetStatus() : %d, exp.GetResult() : %d"),
              exp.GetStatus()?1:0, exp.GetResult()?1:0);
        return exp.GetStatus() && exp.GetResult();
    }
    // if exp.CalcPostfix() fails, which means it may have some undefined identifier (non-digit) in
    // the expression, we should return false
    return false;
}

bool Tokenizer::IsMacroDefined()
{
    // pattern 1: #ifdef ( xxx )
    // pattern 2: #ifdef xxx
    while (SkipWhiteSpace() || SkipComment())
        ;
    bool haveParen = false;
    Lex();
    wxString token = m_Lex;
    if (token == _T("("))
    {
        haveParen = true;
        while (SkipWhiteSpace() || SkipComment())
            ;
        // don't call DoGetToken() here, because it automatically expand macros, call Lex() instead.
        Lex();
        token = m_Lex;
    }
    int id = m_TokenTree->TokenExists(token, -1, tkMacroDef);
    if (haveParen)
    {
        while (SkipWhiteSpace() || SkipComment())
            ;
        Lex(); // eat the ")"
    }
    return (id != -1);
}

void Tokenizer::SkipToNextConditionPreprocessor()
{
    do
    {
        wxChar ch = CurrentChar();
        if (ch == _T('\'') || ch == _T('"') || ch == _T('/') || ch <= _T(' '))
        {
            while (SkipWhiteSpace() || SkipString() || SkipComment())
                ;
            ch = CurrentChar();
        }

        if (ch == _T('#'))
        {
            const unsigned int undoIndex = m_TokenIndex;
            const unsigned int undoLine = m_LineNumber;

            MoveToNextChar();
            while (SkipWhiteSpace() || SkipComment())
                ;

            const wxChar current = CurrentChar();
            const wxChar next = NextChar();

            // #if
            if (current == _T('i') && next == _T('f'))
                SkipToEndConditionPreprocessor();

            // #else #elif #elifdef #elifndef #endif
            else if (current == _T('e') && (next == _T('l') || next == _T('n')))
            {
                m_TokenIndex = undoIndex;
                m_LineNumber = undoLine;
                break;
            }
        }
    }
    while (MoveToNextChar());
}

void Tokenizer::SkipToEndConditionPreprocessor()
{
    do
    {
        wxChar ch = CurrentChar();
        if (ch == _T('\'') || ch == _T('"') || ch == _T('/') || ch <= _T(' '))
        {
            while (SkipWhiteSpace() || SkipString() || SkipComment())
                ;
            ch = CurrentChar();
        }

        if (ch == _T('#'))
        {
            MoveToNextChar();
            while (SkipWhiteSpace() || SkipComment())
                ;

            const wxChar current = CurrentChar();
            const wxChar next = NextChar();

            // #if
            if (current == _T('i') && next == _T('f'))
                SkipToEndConditionPreprocessor();

            // #endif
            else if (current == _T('e') && next == _T('n'))
            {
                SkipToEOL();
                break;
            }
        }
    }
    while (MoveToNextChar());
}

PreprocessorType Tokenizer::GetPreprocessorType()
{
    // those saved m_TokenIndex only rewind for
    // the type of ptOthers, so that Parserthread can handle # include xxxx
    const unsigned int undoIndex = m_TokenIndex;
    const unsigned int undoLine = m_LineNumber;
    const unsigned int undoNest = m_NestLevel;

    MoveToNextChar();
    while (SkipWhiteSpace() || SkipComment())
        ;
    Lex();
    const wxString token = m_Lex;

    switch (token.Len())
    {
        case 2:
            if (token == TokenizerConsts::kw_if)
                return ptIf;
            break;

        case 4:
            if (token == TokenizerConsts::kw_else)
                return ptElse;
            else if (token == TokenizerConsts::kw_elif)
                return ptElif;
            break;

        case 5:
            if (token == TokenizerConsts::kw_ifdef)
                return ptIfdef;
            else if (token == TokenizerConsts::kw_endif)
                return ptEndif;
            else if (token == TokenizerConsts::kw_undef)
                return ptUndef;
            break;

        case 6:
            if (token == TokenizerConsts::kw_ifndef)
                return ptIfndef;
            else if (token == TokenizerConsts::kw_define)
                return ptDefine;
            break;

        case 7:
            if (token == TokenizerConsts::kw_elifdef)
                return ptElifdef;
            break;

        case 8:
            if (token == TokenizerConsts::kw_elifndef)
                return ptElifndef;
            break;

        default:
            break;
    }

    // only rewind m_TokenIndex for ptOthers
    m_TokenIndex = undoIndex;
    m_LineNumber = undoLine;
    m_NestLevel = undoNest;
    return ptOthers;
}

void Tokenizer::HandleConditionPreprocessor(const PreprocessorType type)
{
    switch (type)
    {
        case ptIf:
        {
            TRACE(_T("HandleConditionPreprocessor() : #if at line = %u"), m_LineNumber);
            bool result;
            if (m_TokenizerOptions.wantPreprocessor)
                result = CalcConditionExpression();
            else
            {
                SkipToEOL();
                result = true;
            }

            m_ExpressionResult.push(result);
            if (!result)
               SkipToNextConditionPreprocessor();
        }
        break;

        case ptIfdef:
        {
            TRACE(_T("HandleConditionPreprocessor() : #ifdef at line = %u"), m_LineNumber);
            bool result;
            if (m_TokenizerOptions.wantPreprocessor)
                result = IsMacroDefined();
            else
                result = true; // default value

            SkipToEOL();
            m_ExpressionResult.push(result);
            if (!result)
               SkipToNextConditionPreprocessor();
        }
        break;

        case ptIfndef:
        {
            TRACE(_T("HandleConditionPreprocessor() : #ifndef at line = %u"), m_LineNumber);
            bool result;
            if (m_TokenizerOptions.wantPreprocessor)
                result = !IsMacroDefined();
            else
                result = true; // default value

            SkipToEOL();
            m_ExpressionResult.push(result);
            if (!result)
               SkipToNextConditionPreprocessor();
        }
        break;

        case ptElif:
        {
            TRACE(_T("HandleConditionPreprocessor() : #elif at line = %u"), m_LineNumber);
            bool result = false;
            if (!m_ExpressionResult.empty() && !m_ExpressionResult.top())
                result = CalcConditionExpression();
            if (result)
                m_ExpressionResult.top() = true;
            else
                SkipToNextConditionPreprocessor();
        }
        break;

        case ptElifdef:
        {
            TRACE(_T("HandleConditionPreprocessor() : #elifdef at line = %u"), m_LineNumber);
            bool result = false;
            if (!m_ExpressionResult.empty() && !m_ExpressionResult.top())
            {
                result = IsMacroDefined();
                SkipToEOL();
            }

            if (result)
                m_ExpressionResult.top() = true;
            else
                SkipToNextConditionPreprocessor();
        }
        break;

        case ptElifndef:
        {
            TRACE(_T("HandleConditionPreprocessor() : #elifndef at line = %u"), m_LineNumber);
            bool result = false;
            if (!m_ExpressionResult.empty() && !m_ExpressionResult.top())
            {
                result = !IsMacroDefined();
                SkipToEOL();
            }

            if (result)
                m_ExpressionResult.top() = true;
            else
                SkipToNextConditionPreprocessor();
        }
        break;

        case ptElse:
        {
            TRACE(_T("HandleConditionPreprocessor() : #else at line = %u"), m_LineNumber);
            if (!m_ExpressionResult.empty() && !m_ExpressionResult.top())
                SkipToEOL();
            else
                SkipToEndConditionPreprocessor();
        }
        break;

        case ptEndif:
        {
            TRACE(_T("HandleConditionPreprocessor() : #endif at line = %u"), m_LineNumber);
            SkipToEOL();
            if (!m_ExpressionResult.empty())
                m_ExpressionResult.pop();
        }
        break;

        case ptDefine:
        {
            TRACE(_T("HandleConditionPreprocessor() : #define at line = %u"), m_LineNumber);
            HandleDefines();
        }
        break;

        case ptUndef:
        {
            TRACE(_T("HandleConditionPreprocessor() : #undef at line = %u"), m_LineNumber);
            HandleUndefs();
        }
        break;

        case ptOthers:
        default:
            break;
    }

    // reset undo token
    m_SavedTokenIndex   = m_UndoTokenIndex = m_TokenIndex;
    m_SavedLineNumber   = m_UndoLineNumber = m_LineNumber;
    m_SavedNestingLevel = m_UndoNestLevel  = m_NestLevel;
}

bool Tokenizer::SplitArguments(wxArrayString& results)
{
    while (SkipWhiteSpace() || SkipComment())
        ;
    if (CurrentChar() != _T('('))
        return false;

    MoveToNextChar(); // Skip the '('
    while (SkipWhiteSpace() || SkipComment())
        ;

    const TokenizerState oldState = m_State;
    m_State = tsRawExpression;
    const unsigned int oldNestLevel = m_NestLevel; //

    int level = 1; // include '('

    wxString piece;
    while (NotEOF())
    {
        Lex();
        wxString token = m_Lex;
        if (token.IsEmpty())
            break;

        if (token == _T("("))
            ++level;
        else if (token == _T(")"))
            --level;

        // comma is a delimit only it is not wrapper by ()
        if (token == _T(",") && level == 1)
        {
            results.Add(piece);
            piece.Clear();
        }
        else if (level != 0)
        {
            if (!piece.IsEmpty() && piece.Last() > _T(' '))
                piece << _T(" ");
            piece << token;
        }

        if (level == 0)
        {
            if (!piece.IsEmpty())
                results.Add(piece);
            break;
        }

        while (SkipWhiteSpace() || SkipComment())
            ;
    }

    // reset tokenizer's functionality
    m_State = oldState;
    m_NestLevel = oldNestLevel;
    return true;
}

bool Tokenizer::ReplaceBufferText(const wxString& target, const Token* macro)
{
    if (target.IsEmpty())
        return true; // the token is removed from the buffer, return true, so we need to fetch another token

    if (m_ExpandedMacros.size() >= s_MaxMacroReplaceDepth)
    {
        // clear the macro expansion stack
        m_ExpandedMacros.clear();

        m_PeekAvailable = false;
        return true; // NOTE: we have to skip the problem token by returning true.
    }
    else if (macro)  // Set replace parsing state, and save first replace token index
    {
        ExpandedMacro rep;
        rep.m_End = m_TokenIndex;
        rep.m_Macro = macro;
        m_ExpandedMacros.push_front(rep);
    }
    // we don't push the stack if we don't have macro referenced(macro is 0)

    // Keep all in one line
    wxString substitute(target);
    for (size_t i = 0; i < substitute.Len(); ++i)
    {
        switch ((wxChar)substitute.GetChar(i))
        {
            case _T('\\'):
            case _T('\r'):
            case _T('\n'):
                substitute.SetChar(i, _T(' '));
                break;
            default:
                break;
        }
    }

    // Increase memory if there is not enough space before the m_TokenIndex (between beginning of the
    // the m_Buffer to the m_TokenIndex)
    const size_t len = substitute.Len();
    if (m_TokenIndex < len)
    {
        const size_t diffLen = len - m_TokenIndex;
        m_Buffer.insert(0, wxString(_T(' '), diffLen));
        m_BufferLen += diffLen;
        m_TokenIndex += diffLen;
        // loop the macro expansion stack and adjust them
        for (std::list<ExpandedMacro>::iterator i = m_ExpandedMacros.begin();
             i != m_ExpandedMacros.end();
             ++i)
        {
            (*i).m_Begin += diffLen;
            (*i).m_End += diffLen;
        }
    }

    // Replacement backward
    wxChar* p = const_cast<wxChar*>((const wxChar*)m_Buffer) + m_TokenIndex - len;
    TRACE(_T("ReplaceBufferText() : <FROM>%s<TO>%s"), wxString(p, len).wx_str(), substitute.wx_str());
    // NOTE (ollydbg#1#): This function should be changed to a native wx function if wxString (wxWidgets
    // library) is built with UTF8 encoding for wxString. Luckily, both wx2.8.12 and wx 3.0 use the fixed length
    // (wchar_t) for the wxString encoding unit, so memcpy is safe here.
    memcpy(p, (const wxChar*)target, len * sizeof(wxChar));

    // move the token index to the beginning of the substituted text
    m_TokenIndex -= len;

    if (macro)
        m_ExpandedMacros.front().m_Begin = m_TokenIndex;

    // Reset undo token
    m_SavedTokenIndex   = m_UndoTokenIndex = m_TokenIndex;
    m_SavedLineNumber   = m_UndoLineNumber = m_LineNumber;
    m_SavedNestingLevel = m_UndoNestLevel  = m_NestLevel;

    // since m_TokenIndex is changed, peek values becomes invalid
    m_PeekAvailable = false;

    return true;
}

bool Tokenizer::ReplaceMacroUsage(const Token* tk)
{
    // loop on the m_ExpandedMacros to see the macro is already used
    for (std::list<ExpandedMacro>::iterator i = m_ExpandedMacros.begin();
         i != m_ExpandedMacros.end();
         ++i)
    {
        if (tk == (*i).m_Macro)
            return false; // this macro is already used
    }

    wxString macroExpandedText;
    if ( GetMacroExpandedText(tk, macroExpandedText) )
        return ReplaceBufferText(macroExpandedText, tk);

    return false;
}

void Tokenizer::KMP_GetNextVal(const wxChar* pattern, int next[])
{
    int j = 0, k = -1;
    next[0] = -1;
    while (pattern[j] != _T('\0'))
    {
        if (k == -1 || pattern[j] == pattern[k])
        {
            ++j;
            ++k;
            if (pattern[j] != pattern[k])
                next[j] = k;
            else
                next[j] = next[k];
        }
        else
            k = next[k];
    }
}

int Tokenizer::KMP_Find(const wxChar* text, const wxChar* pattern, const int patternLen)
{
    if (!text || !pattern || pattern[0] == _T('\0') || text[0] == _T('\0'))
        return -1;

    if (patternLen > 1024)
    {
        if (patternLen < 5012)
            TRACE(_T("KMP_Find() : %s - %s"), text, pattern);
        else
        {
            TRACE(_T("KMP_Find: The plan buffer is too big, %d"), patternLen);
            return -2;
        }
    }

    int next[patternLen];
    KMP_GetNextVal(pattern, next);

    int index = 0, i = 0, j = 0;
    while (text[i] != _T('\0') && pattern[j] != _T('\0'))
    {
        if (text[i] == pattern[j])
        {
            ++i;
            ++j;
        }
        else
        {
            index += j - next[j];
            if (next[j] != -1)
                j = next[j];
            else
            {
                j = 0;
                ++i;
            }
        }
    }

    if (pattern[j] == _T('\0'))
        return index;
    else
        return -1;
}

void Tokenizer::SetLastTokenIdx(int tokenIdx)
{
    m_LastTokenIdx = tokenIdx;
    if (tokenIdx != -1 && !m_NextTokenDoc.IsEmpty())
    {
        if (m_ExpressionResult.empty() || m_ExpressionResult.top())
            m_TokenTree->AppendDocumentation(tokenIdx, m_NextTokenDoc);
    }
    m_NextTokenDoc.clear();
}

bool Tokenizer::GetMacroExpandedText(const Token* tk, wxString& expandedText)
{
    // e.g. "#define AAA AAA" and usage "AAA(x)"
    if (!tk || tk->m_Name == tk->m_FullType)
        return false;

    // sanity check if we have such macro definition that #define AAA(x,y) x+y+AAA
    // if a macro name exists in its definition, it will cause a infinite expansion loop
    // Note: disable this sanity check to work around the issue here:
    // http://forums.codeblocks.org/index.php/topic,19661.msg134291.html#msg134291
    //if (tk->m_FullType.Find(tk->m_Name) != wxNOT_FOUND)
    //    return false;

    // if it's a variable like macro definition simply return the replacement text
    if (tk->m_Args.IsEmpty())
    {
        expandedText = tk->m_FullType;
        return true;    // return true for ReplaceBufferText()
    }

    // Now, tk is a function like macro definition we are going to expand, it's m_Args contains the
    // macro formal arguments, the macro actual arguments is already in m_Buffer now.
    // Now, suppose the buffer has such contents:
    // ......ABC(abc, (def)).....
    //          ^--------m_TokenIndex
    // and we have a macro definition such as: #define ABC(x,y) x+y
    // The first thing we need to do is to breakup the formal arguments string "(x,y)", so we get a
    // argument list, we copy the formal arguments(tk->m_Args) to the buffer, so that the buffer
    // becomes, formal arguments string followed by actual arguments string
    // ....(x,y)(abc, (def)).....
    //     ^---------m_TokenIndex is moved backward after a ReplaceBufferText() call, when the
    // formal arguments is copied to the buffer.
    // now, it is ready to split macro arguments by calling the SplitArguments()
    // After the first SplitArguments() call, m_TokenIndex go forward a bit
    // ....(x,y)(abc, (def)).....
    //          ^---------m_TokenIndex after first SplitArguments() call
    // then
    // ....(x,y)(abc, (def)).....
    //                      ^---------m_TokenIndex after second SplitArguments() call
    // then we get a list of actual arguments, so we can construct a map which is:
    // x -> abc
    // y -> (def)
    // finally, the "x+y" will be replaced to "abc+(def)"

    // 1. break the formal args into substring with ","
    wxArrayString formalArgs;
    if (ReplaceBufferText(tk->m_Args))
        SplitArguments(formalArgs);

    // NOTE: some function like macros have empty args list, like #define MACRO() { ... }
    // we should handle those cases, so don't return
    //if (formalArgs.GetCount()==0)
    //    return false;

    // 2. split the actual macro arguments
    wxArrayString actualArgs;
    // NOTE: this case is handled above in "if (tk->m_Args.IsEmpty())" test
    //if (!formalArgs.IsEmpty()) // e.g. #define AAA(x) x \n #define BBB AAA \n BBB(int) variable;
    //    SplitArguments(actualArgs);

    // don't replace anything if the actual arguments are missing, such as in the case:
    // ..... AAA ;
    //          ^----m_TokenIndex, we can't find a opening '('
    if (!SplitArguments(actualArgs))
    {
        // reset the m_Lex since macro expansion failed
        m_Lex = tk->m_Name;
        return false;
    }

    // NOTE: some macros have no args (see above)
    //if (actualArgs.GetCount()==0)
    //    return false;

    //sanity check, both formalArgs.GetCount() actualArgs.GetCount() should match
    if (formalArgs.GetCount() != actualArgs.GetCount())
        return false;

    // 3. get actual context, the expanded text string
    expandedText = tk->m_FullType;
    const size_t totalCount = formalArgs.GetCount();

    // substitute all the arguments
    for (size_t i = 0; i < totalCount; ++i)
    {
        TRACE(_T("GetMacroExpandedText(): The formal args are '%s' and the actual args are '%s'."),
              formalArgs[i].wx_str(), actualArgs[i].wx_str());

        // we search replacement token list
        wxChar* data = const_cast<wxChar*>((const wxChar*)expandedText.GetData());
        const wxChar* dataEnd = data + expandedText.Len();
        // check whether a formal arg exists
        const wxChar* key = formalArgs[i].GetData();
        const int keyLen = formalArgs[i].Len();

        wxString alreadyReplaced;
        alreadyReplaced.Alloc(expandedText.Len() * 2);

        while (true)
        {
            // find the first exist of formal arg from data to dataEnd
            const int pos = GetFirstTokenPosition(data, dataEnd - data, key, keyLen);
            if (pos != -1)
            {
                alreadyReplaced << wxString(data, pos) << actualArgs[i];
                data += pos + keyLen;
                if (data == dataEnd)
                    break;
            }
            else
            {
                alreadyReplaced << data;
                break;
            }
        }

        expandedText = alreadyReplaced;
    }

    // 4. handling operator ## which concatenates two tokens leaving no blank spaces between them
    for (int pos = expandedText.Find(_T("##"));
         pos != wxNOT_FOUND;
         pos = expandedText.Find(_T("##")))
    {
        int beginPos = pos;
        int length = expandedText.size();
        while (beginPos > 0 && expandedText[beginPos-1] == _T(' '))
            beginPos--;
        int endPos = pos + 1;
        while (endPos < length - 1 && expandedText[endPos+1] == _T(' '))
            endPos++;
        // remove the ## with surrounding spaces
        expandedText.Remove(beginPos, endPos - beginPos + 1);
    }

    // 5. handling stringizing operator #
    for (int pos = expandedText.Find(_T("#"));
         pos != wxNOT_FOUND;
         pos = expandedText.Find(_T("#")))
    {
        // here, we may have spaces between the # and the next token (a macro argument)
        // we need to locate the next token's position, here the next token is xxxxxxxxxxxxxx
        // #       xxxxxxxxxxxxxx
        // ^pos   ^beginPos      ^endPos

        // let beginPos points to the space char before the next token by looping on spaces
        int beginPos = pos;
        int length = expandedText.size();
        while (beginPos < length - 1 && expandedText[beginPos+1] == _T(' '))
            beginPos++;

        // expandedText[beginPos] is not a space char, but we expect an identifier like token
        // if # is inside a string, such as "abc#+", then we should not stringizing
        // this is just a work around, the correct way is to use Lex() function to cut the
        // expandedText into tokens, and compare with tokens, not compared with raw text
        if (!wxIsalpha(expandedText[beginPos]))
            break;

        // let endPos points to the space char after the next token by looping on non spaces
        int endPos = beginPos + 1;
        while (endPos < length - 1 && expandedText[endPos+1] != _T(' '))
            endPos++;
        endPos++;

        // reach the end of string, so append an extra space
        if (endPos == length)
            expandedText << _T(" ");

        // replace the space to '"', also, remove the #
        expandedText.SetChar(pos, _T(' '));
        expandedText.SetChar(beginPos, _T('"'));
        expandedText.SetChar(endPos, _T('"'));
    }

    TRACE(_T("The actual macro expanded text is '%s'."), expandedText.wx_str());
    return true;
}

int Tokenizer::GetFirstTokenPosition(const wxChar* buffer, const size_t bufferLen,
                                     const wxChar* key, const size_t keyLen)
{
    int pos = -1;
    wxChar* p = const_cast<wxChar*>(buffer);
    const wxChar* endBuffer = buffer + bufferLen;
    for (;;)
    {
        const int ret = KMP_Find(p, key, keyLen);
        if (ret == -1)
            break;

        // check previous char
        p += ret;
        if (p > buffer)
        {
            const wxChar ch = *(p - 1);
            if (ch == _T('_') || wxIsalnum(ch))
            {
                p += keyLen;
                continue;
            }
        }

        // check next char
        p += keyLen;
        if (p < endBuffer)
        {
            const wxChar ch = *p;
            if (ch == _T('_') || wxIsalnum(ch))
                continue;
        }

        // got it
        pos = p - buffer - keyLen;
        break;
    }

    return pos;
}

void Tokenizer::HandleDefines()
{
    size_t lineNr = GetLineNumber();
    while (SkipWhiteSpace() || SkipComment())
        ;
    Lex();
    wxString token = m_Lex; // read the token after #define
    if (token.IsEmpty())
        return;

    // do *NOT* use m_Tokenizer.GetToken()
    // e.g.
    // #define AAA
    // #ifdef AAA
    // void fly() {}
    // #endif
    // The AAA is not add to token tree, so, when call GetToken(), "#ifdef AAA" parse failed
    wxString readToEOL = ReadToEOL(true);
    wxString para; // function-like macro's args
    wxString replaceList;
    if (!readToEOL.IsEmpty())
    {
        // a '(' char follow the macro name (without space between them) is regard as a
        // function like macro definition
        if (readToEOL[0] == wxT('(')) // function-like macro definition
        {
            int level = 1;
            size_t pos = 0;
            while (level && pos < readToEOL.Len())
            {
                wxChar ch = readToEOL.GetChar(++pos);
                if      (ch == wxT(')'))
                    --level;
                else if (ch == wxT('('))
                    ++level;
            }
            para = readToEOL.Left(++pos);
            replaceList << readToEOL.Right(readToEOL.Len() - (++pos));
        }
        else // variable like macro definition
            replaceList << readToEOL;
    }

    AddMacroDefinition(token, lineNr, para, replaceList);
}

void Tokenizer::HandleUndefs()
{
    while (SkipWhiteSpace() || SkipComment())
        ;
    Lex();
    wxString token = m_Lex; // read the token after #undef
    if (!token.IsEmpty())
    {
        int index = m_TokenTree->TokenExists(token, -1, tkMacroDef);
        if (index != wxNOT_FOUND)
        {
            TRACE(F(_T("macro %s undefined from %s:%d"), token.wx_str(), m_Filename.wx_str(), m_LineNumber));
            m_TokenTree->erase(index);
        }
    }
    SkipToEOL();
}

void Tokenizer::AddMacroDefinition(wxString name, int line, wxString para, wxString substitues)
{
    int index = m_TokenTree->TokenExists(name, -1, tkMacroDef);
    Token* token;

    if (index != wxNOT_FOUND) // already exists, so overwrite! or report a warning!
        token = m_TokenTree->at(index);
    else
    {
        token = new Token(name, m_FileIdx, line, ++m_TokenTree->m_TokenTicketCount);
        token->m_TokenKind = tkMacroDef;// type of the token
        token->m_ParentIndex = -1;      // global namespace
        m_TokenTree->insert(token); // by default, it was added under m_ParentIndex member
    }

    // update the definition
    token->m_Args = para;           // macro call's formal args
    token->m_FullType = substitues; // replace list
}




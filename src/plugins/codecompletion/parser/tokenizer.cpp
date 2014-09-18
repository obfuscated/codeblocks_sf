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
}// namespace TokenizerConsts

// static
wxStringHashMap     Tokenizer::s_Replacements;

// macro replacement may recursive call to an infinite loop, so we should set a limit to avoid this case
static const size_t s_MaxRepeatReplaceCount = 50;

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
    m_State(tsSkipUnWanted),
    m_Loader(0),
    m_FirstRemainingLength(0),
    m_RepeatReplaceCount(0),
    m_NextTokenDoc(),
    m_LastTokenIdx(-1)
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
    m_FirstRemainingLength = 0;
    m_RepeatReplaceCount   = 0;
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

// Behavior consistent with SkipComment
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

bool Tokenizer::MoveToNextChar(const unsigned int amount)
{
    assert(amount);
    if(amount == 1) // compiler will dead-strip this
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
    else
    {
        m_TokenIndex += amount;
        if (IsEOF())
        {
            m_TokenIndex = m_BufferLen;
            return false;
        }

        if (PreviousChar() == _T('\n'))
            ++m_LineNumber;
        return true;
    }
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

// expect we are not in a C-string.
bool Tokenizer::SkipToOneOfChars(const wxChar* chars, bool supportNesting, bool skipPreprocessor, bool skipAngleBrace)
{
    while (NotEOF() && !CharInString(CurrentChar(), chars))
    {
        MoveToNextChar();

        while (SkipString() || SkipComment())
            ;

        // use 'while' here to cater for consecutive blocks to skip (e.g. sometemplate<foo>(bar)
        // must skip <foo> and immediately after (bar))
        // because if we don't, the next block won't be skipped ((bar) in the example) leading to weird
        // parsing results
        bool done = false;
        while (supportNesting && !done)
        {
            switch (CurrentChar())
            {
                case '#':
                    if (skipPreprocessor)
                        SkipToEOL(true);
                    else
                        done = true;
                    break;
                case '{': SkipBlock('{'); break;
                case '(': SkipBlock('('); break;
                case '[': SkipBlock('['); break;
                case '<': // don't skip if << operator
                    if (skipAngleBrace)
                    {
                        if (NextChar() == '<')
                            MoveToNextChar(2); // skip it and also the next '<' or the next '<' leads to a SkipBlock('<');
                        else
                            SkipBlock('<');
                        break;
                    }

                default: done = true; break;
            }
        }

    }

    return NotEOF();
}

wxString Tokenizer::ReadToEOL(bool nestBraces, bool stripUnneeded)
{
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

        TRACE(_T("%s : line=%u, CurrentChar='%c', PreviousChar='%c', NextChar='%c', nestBrace(%d)"),
              wxString(__PRETTY_FUNCTION__, wxConvUTF8).wc_str(), m_LineNumber, CurrentChar(),
              PreviousChar(), NextChar(), nestBraces ? 1 : 0);

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

                if (nestBraces)
                {
                    if (ch == _T('{'))
                        ++m_NestLevel;
                    else if (ch == _T('}'))
                        --m_NestLevel;
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

        return str;
    }
    else
    {
        const unsigned int idx = m_TokenIndex;
        SkipToEOL(nestBraces);
        return m_Buffer.Mid(idx, m_TokenIndex - idx);
    }
}

void Tokenizer::ReadParentheses(wxString& str)
{
    // we create a local buffer here, so the data is copied from m_Buffer to buffer
    // once the local buffer is full, we append the content in local buffer to the result str
    static const size_t maxBufferLen = 4093;
    // local buffer
    wxChar buffer[maxBufferLen + 3];
    buffer[0] = _T('$'); // avoid segfault error, because we have some *(p - 1) = x in the code
    wxChar* realBuffer = buffer + 1;
    // p points to the local buffer
    wxChar* p = realBuffer;

    int level = 0; // brace level of '(' and ')'

    while (NotEOF())
    {
        while (SkipComment())
            ;
        wxChar ch = CurrentChar();

        while (ch == _T('#')) // do not use if
        {
            const PreprocessorType type = GetPreprocessorType();
            if (type == ptOthers)
                break;
            HandleConditionPreprocessor(type);
            ch = CurrentChar();
        }

        const unsigned int startIndex = m_TokenIndex;

        switch(ch)
        {
        case _T('('):
            {
                ++level;
                *p = ch;
                ++p;
            }
            break;

        case _T(')'):
            {
                if (*(p - 1) <= _T(' '))
                    --p; // if previous char is a space, we overwrite the space with a ')', so we save one char
                --level;
                *p = ch;
                ++p;
            }
            break;

        case _T('\''):
        case _T('"'):
            {
                MoveToNextChar();     // skip the leading '"' or '\''
                SkipToStringEnd(ch);  // m_TokenIndex point to the trailing '"' or '\'' now
                MoveToNextChar();     // move to the char after trailing char
                const size_t writeLen = m_TokenIndex - startIndex; // the string length
                const size_t usedLen = p - realBuffer;
                if (usedLen + writeLen > maxBufferLen) // there is not enough space left to write the string
                {
                    if (writeLen > maxBufferLen) // to big string?
                    {
                        TRACE(_T("ReadParentheses(): Catch exception 1: %lu"), static_cast<unsigned long>(writeLen));
                        return;
                    }

                    if (p != realBuffer) // add the used string to the result str
                    {
                        str.Append(realBuffer, usedLen);
                        p = realBuffer;  // adjust the p, as it was an empty buffer
                    }
                    // append the string to str (not write the string to the buffer)
                    str.Append((const wxChar*)m_Buffer + startIndex, writeLen);
                }
                else
                {   // the buffer can hold the string, so copy it to the buffer
                    memcpy(p, (const wxChar*)m_Buffer + startIndex, writeLen * sizeof(wxChar));
                    p += writeLen;
                }

                continue;
            }
            break;

        case _T(','): // if there is a space before the comma, we just remove it, but we should add a space after the comma
        case _T('*'):
        case _T('&'):
            {
                if (*(p - 1) <= _T(' '))
                    --p;

                *p = ch;
                *++p = _T(' ');
                ++p;
            }
            break;

        case _T('='):
            {
                if (*(p - 1) <= _T(' '))
                {
                    *p = _T('=');
                    // Don't add a space after '=' sign, in case another '=' follows it
                    // (see how the 'else' block below works).
                    //*++p = _T(' ');
                    ++p;
                }
                else // special handling of "==" "=!" "=>" and "=<"
                {
                    switch (*(p - 1))
                    {
                    case _T('='):
                    case _T('!'):
                    case _T('>'):
                    case _T('<'):
                        {
                            *p = _T('=');
                            *++p = _T(' ');
                            ++p;
                        }
                        break;

                    default:
                        {
                            *p = _T(' ');
                            *++p = _T('=');
                            *++p = _T(' ');
                            ++p;
                        }
                        break;
                    }
                }
            }
            break;

        case _T(' '): // we only add a space if the former char is not a space, also not a '('
        case _T('\r'): // the tab char are regard as space
        case _T('\t'):
            {
                if (*(p - 1) != _T(' ') && *(p - 1) != _T('('))
                {
                    *p = _T(' ');
                    ++p;
                }
            }
            break;

        case _T('\n'): // we need keep the \n for records paras correct position
            if (*(p - 1) == _T(' '))
                --p;
            if (*(p - 1) != _T('('))
            {
                *p = ch;
                ++p;
            }
            break;

        default: // any other case, we can directly copy the char
            {
                *p = ch;
                ++p;
            }
            break;
        }

        if (p >= realBuffer + maxBufferLen)
        {
            str.Append(realBuffer, p - realBuffer);
            p = realBuffer;
        }

        MoveToNextChar();

        if (level == 0)
            break;
    }//while (NotEOF())

    if (p > realBuffer)
        str.Append(realBuffer, p - realBuffer);
    TRACE(_T("ReadParentheses(): %s, line=%u"), str.wx_str(), m_LineNumber);
    if (str.Len() > 512)
    {   TRACE(_T("ReadParentheses(): Catch exception 2: %lu"), static_cast<unsigned long>(str.Len())); }
}

bool Tokenizer::SkipToEOL(bool nestBraces)
{
    TRACE(_T("%s : line=%u, CurrentChar='%c', PreviousChar='%c', NextChar='%c', nestBrace(%d)"),
          wxString(__PRETTY_FUNCTION__, wxConvUTF8).wc_str(), m_LineNumber, CurrentChar(),
          PreviousChar(), NextChar(), nestBraces ? 1 : 0);

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

            if (nestBraces && CurrentChar() == _T('{'))
                ++m_NestLevel;
            else if (nestBraces && CurrentChar() == _T('}'))
                --m_NestLevel;

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

    MoveToNextChar(2);    // Skip the comment prompt

    bool isDoc = false;
    if (m_TokenizerOptions.storeDocumentation)
    {
        isDoc = (CurrentChar() == '!');	//	"/*!" or "//!"

        if (!isDoc && cstyle) //  "/*" + ?
            isDoc = (CurrentChar() == '*' && NextChar() != '/'); //	"/**" but not "/**/" and not //*

        if (!isDoc && !cstyle) // "//" + ?
            isDoc = (CurrentChar() == '/' && NextChar() != '/'); // "///" but not "////"
    }

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
                    MoveToNextChar(2);
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
                    MoveToNextChar(2); // eat '/'
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

        if (doc.size()>0) // dont push empty strings
        {
            doc += _T('\n');

            if (lineToAppend >= 0) // we have document after the token place
            {
                if (m_LastTokenIdx != -1)
                    m_TokenTree->AppendDocumentation(m_LastTokenIdx, m_NextTokenDoc + doc);

                m_NextTokenDoc.clear();
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

bool Tokenizer::SkipUnwanted()
{
    while (SkipWhiteSpace() || SkipComment())
        ;

    wxChar c = CurrentChar();
    const unsigned int startIndex = m_TokenIndex;

    if (c == _T('#'))
    {
        const PreprocessorType type = GetPreprocessorType();
        if (type != ptOthers)
        {
            HandleConditionPreprocessor(type);
            c = CurrentChar();
        }
    }

    // skip [XXX][YYY]
    if (m_State & tsSkipSubScrip)
    {
        while (c == _T('[') )
        {
            SkipBlock('[');
            SkipWhiteSpace();
            if (IsEOF())
                return false;
            c = CurrentChar();
        }
    }

    // skip the following = or ?
    if (m_State & tsSkipEqual)
    {
        if (c == _T('=') && NextChar() != _T('=')) //only skip after single equal sign, not double equals sign
        {
            if (!SkipToOneOfChars(_T(",;}"), true, true, false))
                return false;
        }
    }
    else if (m_State & tsSkipQuestion)
    {
        if (c == _T('?'))
        {
            if (!SkipToOneOfChars(_T(";}"), false, true))
                return false;
        }
    }

    // skip the following white space and comments
    while (SkipWhiteSpace() || SkipComment())
        ;

    if (startIndex != m_TokenIndex && CurrentChar() == _T('#'))
        return SkipUnwanted();

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
            m_Token = DoGetToken();
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
            m_PeekToken = DoGetToken();
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
    int start = m_TokenIndex;
    bool needReplace = false;

    wxString str;
    wxChar   c = CurrentChar();

    if (c == '_' || wxIsalpha(c))
    {
        // keywords, identifiers, etc.

        // operator== is cheaper than wxIsalnum, also MoveToNextChar already includes IsEOF
        while (    ( (c == '_') || (wxIsalnum(c)) )
               &&  MoveToNextChar() )
            c = CurrentChar(); // repeat

        if (IsEOF())
            return wxEmptyString;

        needReplace = true;
        str = m_Buffer.Mid(start, m_TokenIndex - start);
    }
#ifdef __WXMSW__ // This is a Windows only bug!
    // fetch non-English characters, see more details in: http://forums.codeblocks.org/index.php/topic,11387.0.html
    else if (c == 178 || c == 179 || c == 185)
    {
        str = c;
        MoveToNextChar();
    }
#endif
    else if (wxIsdigit(c))
    {
        // numbers
        while (NotEOF() && CharInString(CurrentChar(), _T("0123456789.abcdefABCDEFXxLl")))
            MoveToNextChar();

        if (IsEOF())
            return wxEmptyString;

        str = m_Buffer.Mid(start, m_TokenIndex - start);
    }
    else if ( (c == '"') || (c == '\'') )
    {
        SkipString();
        //Now, we are after the end of the C-string, so return the whole string as a token.
        str = m_Buffer.Mid(start, m_TokenIndex - start);
    }
    else if (c == ':')
    {
        if (NextChar() == ':')
        {
            MoveToNextChar();
            MoveToNextChar();
            // this only copies a pointer, but operator= allocates memory and does a memcpy!
            str.assign(TokenizerConsts::colon_colon);
        }
        else
        {
            MoveToNextChar();
            str.assign(TokenizerConsts::colon);
        }
    }
    else if (c == '<')
    {
        if (m_State&tsSingleAngleBrace)
        {
            if ( !SkipToOneOfChars(  _T(">"), true, true)   )
                return wxEmptyString;
            MoveToNextChar();
            str= m_Buffer.Mid(start, m_TokenIndex - start);
        }
        else
        {
            str = c;
            MoveToNextChar();
        }
    }
    else if (c == '(')
    {
        if (m_State & tsReadRawExpression)
        {
            str = c;
            MoveToNextChar();
        }
        else
        {
            ReadParentheses(str);
        }
    }
    else
    {
        if      (c == '{')
            ++m_NestLevel;
        else if (c == '}')
            --m_NestLevel;

        str = c;
        MoveToNextChar();
    }
    // m_FirstRemainingLength != 0 means we are in macro replace mode, but when m_TokenIndex goes
    // forward and exceed the anchor point where we start the macro replacement, we should stop and
    // reset to non-macro replace mode
    if (m_FirstRemainingLength != 0 && m_BufferLen - m_FirstRemainingLength < m_TokenIndex)
    {
        m_FirstRemainingLength = 0;
        m_RepeatReplaceCount = 0;
    }

    if (needReplace && m_State ^ tsReadRawExpression)
        ReplaceMacro(str);

    return str;
}

void Tokenizer::ReplaceMacro(wxString& str)
{
    // this indicates we are already in macro replacement mode
    if (m_RepeatReplaceCount > 0)
    {
        const int id = m_TokenTree->TokenExists(str, -1, tkMacroDef);
        if (id != -1)
        {
            const Token* token = m_TokenTree->at(id);
            if (token)
            {
                bool replaced = false;
                if (!token->m_Args.IsEmpty())
                    replaced = ReplaceFunctionLikeMacro(token);
                else if (token->m_FullType != token->m_Name)
                    replaced = ReplaceBufferText(token->m_FullType);

                if (replaced || token->m_FullType.IsEmpty())
                {
                    SkipUnwanted();
                    // recursive call the DoGetToken function here
                    str = DoGetToken();
                }
            }
        }
        // if in macro expansion mode, we don't want to let the user replacement rule executed
        // again, so just returned
        return;
    }

    wxStringHashMap::const_iterator it = s_Replacements.find(str);
    if (it == s_Replacements.end())
        return;

    TRACE(_T("ReplaceMacro() : Replacing '%s' with '%s' (file='%s', line='%u')."), it->first.wx_str(),
          it->second.wx_str(), m_Filename.wx_str(), m_LineNumber);

    if (it->second.IsEmpty())
    {
        SkipUnwanted();
        str = DoGetToken();
    }
    else if (it->second[0] == _T('+'))
    {
        while (SkipWhiteSpace() || SkipComment())
            ;
        DoGetToken(); // eat (...)
        wxString target = (const wxChar*)it->second + 1;
        if (target.IsEmpty())
        {
            while (SkipWhiteSpace() || SkipComment())
                ;
            str = DoGetToken();
        }
        else if (target != str && ReplaceBufferText(target))
            str = DoGetToken();
    }
    else if (it->second[0] == _T('-'))
    {
        // sample rule:  "BEGIN_EVENT_TABLE" -> "-END_EVENT_TABLE"
        // we should skip to END_EVENT_TABLE, so the contents between BEGIN_EVENT_TABLE and
        // END_EVENT_TABLE is skipped, also the "()" after END_EVENT_TABLE is skipped.
        wxString end((const wxChar*)it->second + 1); // strip the '-'
        if (end.IsEmpty())
            return;

        while (NotEOF())
        {
            while (SkipComment() || SkipWhiteSpace())
                ;
            if (CurrentChar() == end[0])
            {
                if (DoGetToken() == end)
                    break;
            }
            else
                MoveToNextChar();
        }

        // eat ()
        SkipUnwanted();
        str = DoGetToken();
        if (str[0] == _T('('))
        {
            SkipUnwanted();
            str = DoGetToken();
        }
    }
    else if (it->second[0] == _T('@'))
    {
        // trigger the macro replacement mode, so that we can look up the keyword in TokenTree for a
        // macro usage, note that ReplaceBufferText() below just move the token index backword, but
        // don't change the buffer
        if(ReplaceBufferText(it->first))
            str = DoGetToken();

    }
    else // for other user defined rules, just do a buffer content replacement
    {
        if (it->second != str && ReplaceBufferText(it->second))
            str = DoGetToken();
    }
}

bool Tokenizer::CalcConditionExpression()
{
    // need to force the tokenizer skip raw expression
    const TokenizerState oldState = m_State;
    m_State = tsReadRawExpression;

    const unsigned int undoIndex = m_TokenIndex;
    const unsigned int undoLine = m_LineNumber;
    SkipToEOL(false);
    const unsigned int lastBufferLen = m_BufferLen - m_TokenIndex;
    m_TokenIndex = undoIndex;
    m_LineNumber = undoLine;

    Expression exp;
    while (m_BufferLen - m_TokenIndex > lastBufferLen)
    {
        while (SkipComment())
            ;
        wxString token = DoGetToken();
        if (token[0] <= _T(' ') || token == _T("defined") || token == _T("\\"))
            continue;

        if (token.Len() > 1 && !wxIsdigit(token[0])) // handle macro
        {
            const int id = m_TokenTree->TokenExists(token, -1, tkMacroDef);
            if (id != -1)
            {
                const Token* tk = m_TokenTree->at(id);
                if (tk)
                {
                    if (tk->m_FullType.IsEmpty() || tk->m_FullType == token)
                    {
                        if (tk->m_Args.IsEmpty()) //variable like macro definition, but no definition, set 1
                        {
                            exp.AddToInfixExpression(_T("1"));
                            continue;
                        }
                        else //variable like macro definition, need to replace texts
                        {
                            if (ReplaceBufferText(tk->m_Args))
                                continue;
                        }
                    }
                    else if (!tk->m_Args.IsEmpty()) //function like macro definition, expand them
                    {
                        if (ReplaceFunctionLikeMacro(tk))
                            continue; //after the expansion, m_TokenIndex is adjusted, so continue again
                    }
                    else if (wxIsdigit(tk->m_FullType[0]))
                        token = tk->m_FullType;
                    else if (tk->m_FullType != tk->m_Name)
                    {
                        if (ReplaceBufferText(tk->m_FullType))
                            continue;
                    }
                }
            }
            else
            {
                exp.AddToInfixExpression(_T("0"));
                continue;
            }
        }

        // only remaining number now
        if (!token.StartsWith(_T("0x")))
            exp.AddToInfixExpression(token);
        else
        {
            long value;
            if (token.ToLong(&value, 16))
                exp.AddToInfixExpression(wxString::Format(_T("%ld"), value));
            else
                exp.AddToInfixExpression(_T("0"));
        }
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
    while (SkipWhiteSpace() || SkipComment())
        ;
    int id = m_TokenTree->TokenExists(DoGetToken(), -1, tkMacroDef);
    SkipToEOL(false);
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
                SkipToEOL(false);
                break;
            }
        }
    }
    while (MoveToNextChar());
}

PreprocessorType Tokenizer::GetPreprocessorType()
{
    const unsigned int undoIndex = m_TokenIndex;
    const unsigned int undoLine = m_LineNumber;
    const unsigned int undoNest = m_NestLevel;

    MoveToNextChar();
    while (SkipWhiteSpace() || SkipComment())
        ;

    const wxString token = DoGetToken();

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
            break;

        case 6:
            if (token == TokenizerConsts::kw_ifndef)
                return ptIfndef;
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
                SkipToEOL(false);
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
            {
                SkipToEOL(false);
                result = true;
            }

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
            {
                SkipToEOL(false);
                result = true;
            }

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
                result = IsMacroDefined();
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
                result = !IsMacroDefined();
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
                SkipToEOL(false);
            else
                SkipToEndConditionPreprocessor();
        }
        break;

        case ptEndif:
        {
            TRACE(_T("HandleConditionPreprocessor() : #endif at line = %u"), m_LineNumber);
            SkipToEOL(false);
            if (!m_ExpressionResult.empty())
                m_ExpressionResult.pop();
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
    m_State = tsReadRawExpression;

    int level = 1; // include '('

    wxString piece;
    while (NotEOF())
    {
        wxString token = DoGetToken();
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
    return true;
}

bool Tokenizer::ReplaceBufferText(const wxString& target)
{
    if (target.IsEmpty())
        return false;

    if (m_RepeatReplaceCount > 0)
    {
        if (m_RepeatReplaceCount >= s_MaxRepeatReplaceCount)
        {
            m_TokenIndex = m_BufferLen - m_FirstRemainingLength;
            m_PeekAvailable = false;
            SkipToEOL(false);
            return false;
        }
        else
            ++m_RepeatReplaceCount;
    }
    else  // Set replace parsing state, and save first replace token index
    {
        m_FirstRemainingLength = m_BufferLen - m_TokenIndex;
        ++m_RepeatReplaceCount;
    }

    // Keep all in one line
    wxString buffer(target);
    for (size_t i = 0; i < buffer.Len(); ++i)
    {
        switch ((wxChar)buffer.GetChar(i))
        {
            case _T('\\'):
            case _T('\r'):
            case _T('\n'):
                buffer.SetChar(i, _T(' '));
                break;
            default:
                break;
        }
    }

    // Increase memory if there is not enough space before the m_TokenIndex (between beginning of the
    // the m_Buffer to the m_TokenIndex)
    const size_t bufferLen = buffer.Len();
    if (m_TokenIndex < bufferLen)
    {
        const size_t diffLen = bufferLen - m_TokenIndex;
        m_Buffer.insert(0, wxString(_T(' '), diffLen));
        m_BufferLen += diffLen;
        m_TokenIndex += diffLen;
    }

    // Replacement backward
    wxChar* p = const_cast<wxChar*>((const wxChar*)m_Buffer) + m_TokenIndex - bufferLen;
    TRACE(_T("ReplaceBufferText() : <FROM>%s<TO>%s"), wxString(p, bufferLen).wx_str(), buffer.wx_str());
    // NOTE (ollydbg#1#): This function should be changed to a native wx function if wxString (wxWidgets
    // library) is built with UTF8 encoding for wxString. Luckily, both wx2.8.12 and wx 3.0 use the fixed length
    // (wchar_t) for the wxString encoding unit, so memcpy is safe here.
    memcpy(p, (const wxChar*)target, bufferLen * sizeof(wxChar));

    // Fix token index
    m_TokenIndex -= bufferLen;

    // Reset undo token
    m_SavedTokenIndex   = m_UndoTokenIndex = m_TokenIndex;
    m_SavedLineNumber   = m_UndoLineNumber = m_LineNumber;
    m_SavedNestingLevel = m_UndoNestLevel  = m_NestLevel;

    // since m_TokenIndex is changed, peek values becomes invalid
    m_PeekAvailable = false;

    return true;
}

bool Tokenizer::ReplaceFunctionLikeMacro(const Token* tk)
{
    wxString macroExpandedText;
    if ( GetMacroExpandedText(tk, macroExpandedText) )
        return ReplaceBufferText(macroExpandedText);
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
    if (tk->m_FullType.Find(tk->m_Name) != wxNOT_FOUND)
        return false;

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
        return false;

    // NOTE: some macros have no args (see above)
    //if (actualArgs.GetCount()==0)
    //    return false;

    //sanity check, both formalArgs.GetCount() actualArgs.GetCount() should match
    if (formalArgs.GetCount() != actualArgs.GetCount())
        return false;

    // 3. get actual context, the expanded text string
    expandedText = tk->m_FullType;
    const size_t totalCount = formalArgs.GetCount();

    // loop on all the arguments
    for (size_t i = 0; i < totalCount; ++i)
    {
        TRACE(_T("GetMacroExpandedText(): The formal args are '%s' and the actual args are '%s'."),
              formalArgs[i].wx_str(), actualArgs[i].wx_str());

        wxChar* data = const_cast<wxChar*>((const wxChar*)expandedText.GetData());
        const wxChar* dataEnd = data + expandedText.Len();
        const wxChar* key = formalArgs[i].GetData();
        const int keyLen = formalArgs[i].Len();

        wxString alreadyReplaced;
        alreadyReplaced.Alloc(expandedText.Len() * 2);

        while (true)
        {
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

    // 4. erase string "##"
    expandedText.Replace(_T("##"), wxEmptyString);

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

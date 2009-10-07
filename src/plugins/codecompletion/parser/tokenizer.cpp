/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#include "tokenizer.h"
#include <wx/utils.h>
#include <wx/file.h>
#include <wx/msgdlg.h>
#include "manager.h"
#include <cctype>
#include <globals.h>

#define TOKENIZER_DEBUG_OUTPUT 0

#if TOKENIZER_DEBUG_OUTPUT
    #define TRACE(format, args...)\
    Manager::Get()->GetLogManager()->DebugLog(F( format , ## args))
#else
    #define TRACE(format, args...)
#endif

namespace TokenizerConsts
{
const wxString colon       (_T(":"));
const wxString colon_colon (_T("::"));
const wxString operator_str(_T("operator"));
const wxString include_str (_T("#include"));
const wxString if_str      (_T("#if"));
const wxString hash        (_T("#"));
const wxString tabcrlf     (_T("\t\n\r"));
};

// static
ConfigManagerContainer::StringToStringMap Tokenizer::s_Replacements;

Tokenizer::Tokenizer(const wxString& filename)
    : m_Filename(filename),
    m_BufferLen(0),
    m_Token(_T("")),
    m_TokenIndex(0),
    m_LineNumber(1),
    m_NestLevel(0),
    m_SavedNestingLevel(0),
    m_UndoTokenIndex(0),
    m_UndoLineNumber(1),
    m_UndoNestLevel(0),
    m_PeekAvailable(false),
    m_PeekToken(_T("")),
    m_PeekTokenIndex(0),
    m_PeekLineNumber(0),
    m_PeekNestLevel(0),
    m_IsOK(false),
    m_IsOperator(false),
    m_LastWasPreprocessor(false),
    m_SkipUnwantedTokens(true),
    m_pLoader(0)
{
    m_TokenizerOptions.wantPreprocessor = false;
    if (!m_Filename.IsEmpty())
        Init(m_Filename);
}

Tokenizer::~Tokenizer()
{
}

bool Tokenizer::Init(const wxString& filename, LoaderBase* loader)
{
    m_pLoader = loader;
    BaseInit();
    if (filename.IsEmpty())
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
        TRACE(_T("Init() : m_Filename='%s'"), m_Filename.c_str());
    }

    if (!wxFileExists(m_Filename))
    {
        TRACE(_T("Init() : File '%s' does not exist."), m_Filename.c_str());
        return false;
    }

    if (!ReadFile())
    {
        TRACE(_T("Init() : File '%s' could not be read."), m_Filename.c_str());
        return false;
    }

    if (!m_BufferLen)
    {
        TRACE(_T("Init() : File '%s' is empty."), m_Filename.c_str());
        return false;
    }

    m_IsOK = true;
    return true;
}

bool Tokenizer::InitFromBuffer(const wxString& buffer)
{
    BaseInit();
    m_BufferLen = buffer.Length();
    m_Buffer.Alloc(m_BufferLen + 1); // + 1 => sentinel
    m_Buffer = buffer;
    m_Buffer += _T(' ');
    m_IsOK = true;
    m_Filename.Clear();
    return true;
}

void Tokenizer::BaseInit()
{
    m_BufferLen           = 0;
    m_TokenIndex          = 0;
    m_LineNumber          = 1;
    m_NestLevel           = 0;
    m_SavedNestingLevel   = 0;
    m_UndoTokenIndex      = 0;
    m_UndoLineNumber      = 1;
    m_UndoNestLevel       = 0;
    m_PeekTokenIndex      = 0;
    m_PeekLineNumber      = 0;
    m_PeekNestLevel       = 0;
    m_IsOK                = false;
    m_IsOperator          = false;
    m_LastWasPreprocessor = false;
    m_LastPreprocessor.Clear();
    m_Buffer.Clear();
}

bool Tokenizer::ReadFile()
{
    bool success = false;
    wxString fileName = wxEmptyString;
    if (m_pLoader)
    {
        fileName = m_pLoader->FileName();
        char* data  = m_pLoader->GetData();
        m_BufferLen = m_pLoader->GetLength();

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
        if (!wxFileExists(m_Filename))
            return false;

        // open file
        wxFile file(m_Filename);

        if (!cbRead(file, m_Buffer))
            return false;

        fileName = m_Filename;
        success = true;
    }
/*
    size_t replacements  = m_Buffer.Replace(_T("_GLIBCXX_BEGIN_NESTED_NAMESPACE(std, _GLIBCXX_STD_D)"), _T("namespace std {"),       true);
           replacements += m_Buffer.Replace(_T("_GLIBCXX_BEGIN_NESTED_NAMESPACE(std, _GLIBCXX_STD_P)"), _T("namespace std {"),       true);
           replacements += m_Buffer.Replace(_T("_GLIBCXX_END_NESTED_NAMESPACE"),                        _T("}"),                     true);
           replacements += m_Buffer.Replace(_T("_GLIBCXX_BEGIN_NAMESPACE_TR1"),                         _T("namespace tr1 {"),       true);
           // The following must be before replacing "_GLIBCXX_END_NAMESPACE"!!!
           replacements += m_Buffer.Replace(_T("_GLIBCXX_END_NAMESPACE_TR1"),                           _T("}"),                     true);
           replacements += m_Buffer.Replace(_T("_GLIBCXX_BEGIN_NAMESPACE(__gnu_cxx)"),                  _T("namespace __gnu_cxx {"), true);
           replacements += m_Buffer.Replace(_T("_GLIBCXX_BEGIN_NAMESPACE(std)"),                        _T("namespace std {"),       true);
           replacements += m_Buffer.Replace(_T("_GLIBCXX_END_NAMESPACE"),                               _T("}"),                     true);

#if TOKENIZER_DEBUG_OUTPUT
    if (replacements)
        TRACE(F(_T("Did %d replacements in buffer of '%s'."), replacements, fileName.c_str()));
#endif
*/
    m_BufferLen = m_Buffer.Length();

    // add 'sentinel' to the end of the string (not counted to the length of the string)
    m_Buffer += _T(' ');

    return success;
}

bool Tokenizer::SkipWhiteSpace()
{
    // skip spaces, tabs, etc.
    while (CurrentChar() <= _T(' ') && MoveToNextChar()) // don't check EOF when MoveToNextChar already does, also replace isspace() which calls msvcrt.dll
        ;                                                // with a dirty hack: CurrentChar() <= ' ' is "good enough" here

    if (IsEOF())
        return false;

    return true;
}

bool Tokenizer::IsEscapedChar()
{
    // Easy: If previous char is not a backslash, too than it's surely escape'd
    if (PreviousChar() != '\\')
        return true;
    else
    {
        // check for multiple backslashes, e.g. "\\"
        unsigned int numBackslash = 2; // for sure we have at least two at this point
        while(   ((m_TokenIndex - numBackslash) >= 0)
              && ((m_TokenIndex - numBackslash) <= m_BufferLen)
              && (m_Buffer.GetChar(m_TokenIndex - numBackslash) == '\\') )
            ++numBackslash; // another one...

        if ( (numBackslash%2) == 1) // number of backslashes (including current char) is odd
            return true;            // eg: "\""
        else                        // number of backslashes (including current char) is even
            return false;           // eg: "\\""
    }
    return false;
}

bool Tokenizer::SkipToChar(const wxChar& ch)
{
    // skip everything until we find ch
    while(true)
    {
        while (CurrentChar() != ch && MoveToNextChar())  // don't check EOF when MoveToNextChar already does
            ;

        if (IsEOF())
            return false;

        if (IsEscapedChar()) break;

        MoveToNextChar();
    }
    return true;
}

bool Tokenizer::SkipToOneOfChars(const wxChar* chars, bool supportNesting)
{
    // skip everything until we find any one of chars
    while (1)
    {
        while (NotEOF() && !CharInString(CurrentChar(), chars))
        {
            if (CurrentChar() == '"' || CurrentChar() == '\'')
            {
                // this is the case that match is inside a string!
                wxChar ch = CurrentChar();
                MoveToNextChar();
                SkipToChar(ch);
            }
            MoveToNextChar();

            // make sure we skip comments
            if (CurrentChar() == '/')
                SkipComment(); // this will decide if it is a comment

            // use 'while' here to cater for consecutive blocks to skip (e.g. sometemplate<foo>(bar)
            // must skip <foo> and immediately after (bar))
            // because if we don't, the next block won't be skipped ((bar) in the example) leading to weird
            // parsing results
            bool done = false;
            while (supportNesting && !done)
            {
                switch (CurrentChar())
                {
                    case '{': SkipBlock('{'); break;
                    case '(': SkipBlock('('); break;
                    case '[': SkipBlock('['); break;
                    case '<': // don't skip if << operator
                        if (NextChar() == '<')
                            MoveToNextChar(2); // skip it and also the next '<' or the next '<' leads to a SkipBlock('<');
                        else
                            SkipBlock('<');
                        break;
                    default: done = true; break;
                }
            }
        }

        if (IsEscapedChar()) break;

        // if we are at buffer-end MoveToNextChar returns false, but does not change the token-index,
        // break to avoid endless loops
        if(!MoveToNextChar()) break;
    }
    if (IsEOF())
        return false;
    return true;
}

wxString Tokenizer::ReadToEOL(bool nestBraces)
{
    unsigned int idx = m_TokenIndex;
    SkipToEOL(nestBraces);
    return m_Buffer.Mid(idx, m_TokenIndex - idx);
}

bool Tokenizer::SkipToEOL(bool nestBraces, bool skippingComment)
{
    // skip everything until we find EOL
    while (1)
    {
        while (NotEOF() && CurrentChar() != '\n')
        {
            if(!skippingComment)
            {
                if (CurrentChar() == '/' && NextChar() == '*')
                {
                    SkipComment(false); // don't skip whitespace after the comment
                    if (skippingComment && CurrentChar() == '\n')
                    {
                        continue; // early exit from the loop
                    }
                }
                if (nestBraces && CurrentChar() == _T('{'))
                    ++m_NestLevel;
                else if (nestBraces && CurrentChar() == _T('}'))
                    --m_NestLevel;
            }
            MoveToNextChar();
        }
        wxChar last = PreviousChar();
        // if DOS line endings, we 've hit \r and we skip to \n...
        if (last == '\r')
        {
            if (m_TokenIndex - 2 >= 0)
                last = m_Buffer.GetChar(m_TokenIndex - 2);
            else
                last = _T('\0');
        }
        if (IsEOF() || last != '\\')
            break;
        else
            MoveToNextChar();
    }
    if (IsEOF())
        return false;
    return true;
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
    int count = 1; // counter for nested blocks (xxx())
    while (NotEOF())
    {
        bool noMove = false;
        if (CurrentChar() == '/')
            SkipComment(); // this will decide if it is a comment

        if (CurrentChar() == '"' || CurrentChar() == '\'')
        {
            // this is the case that match is inside a string!
            wxChar ch = CurrentChar();
            MoveToNextChar();
            SkipToChar(ch);
            MoveToNextChar();
            // don't move to next char below if concatenating strings (e.g. printf("" ""))
            if (CurrentChar() == '"' || CurrentChar() == '\'')
                noMove = true;
        }
        if (CurrentChar() == ch)
            ++count;
        else if (CurrentChar() == match)
            --count;
        if (!noMove)
            MoveToNextChar();
        if (count == 0)
            break;
    }
    if (IsEOF())
        return false;
    return true;
}

bool Tokenizer::SkipComment(bool skipWhiteAtEnd) // = true
{
    // C/C++ style comments
    bool is_comment = CurrentChar() == '/' && (NextChar() == '/' || NextChar() == '*');
    if (!is_comment)
        return true;

    bool cstyle = NextChar() == '*';
    MoveToNextChar(2);
    while (1)
    {
        if (!cstyle)
        {
            if (!SkipToEOL(false, true))
                return false;
            MoveToNextChar();
            break;
        }
        else
        {
            if (SkipToChar('/'))
            {
                if (PreviousChar() == '*')
                {
                    MoveToNextChar();
                    break;
                }
                MoveToNextChar();
            }
            else
                return false;
        }
    }
    if (IsEOF())
        return false;
    if (skipWhiteAtEnd && !SkipWhiteSpace())
        return false;
    return CurrentChar() == '/' ? SkipComment() : true; // handle chained comments
}

bool Tokenizer::SkipUnwanted()
{
    while (CurrentChar() == '#' ||
            (!m_IsOperator && CurrentChar() == '=') ||
            (!m_IsOperator && CurrentChar() == '[') ||
            CurrentChar() == '?' ||
            (CurrentChar() == '/' && (NextChar() == '/' || NextChar() == '*') ))
    {
        bool skipPreprocessor = false; // used for #include
        while (m_Buffer.Mid(m_TokenIndex, 2) == _T("//") ||
                m_Buffer.Mid(m_TokenIndex, 2) == _T("/*"))
        {
            // C/C++ style comments
            SkipComment();
            if (IsEOF())
                return false;
            if (!SkipWhiteSpace())
                return false;
        }

        while (CurrentChar() == '#')
        {
            // preprocessor directives
            // we only care for #include and #define, for now
            unsigned int backupIdx = m_TokenIndex;
            MoveToNextChar();
            SkipWhiteSpace();
            if ((CurrentChar() == 'i' && NextChar() == 'n') || // in(clude)
                (CurrentChar() == 'i' && NextChar() == 'f') || // if(|def|ndef)
                (CurrentChar() == 'e' && NextChar() == 'l') || // el(se|if)
                (CurrentChar() == 'e' && NextChar() == 'n') || // en(dif)
                (m_TokenizerOptions.wantPreprocessor && CurrentChar() == 'd' && NextChar() == 'e')) // de(fine)
            {
                // ok, we have something like #in(clude)
                m_LastWasPreprocessor = true;
                m_LastPreprocessor.Clear();
                m_TokenIndex = backupIdx; // keep #
                skipPreprocessor = true;
                break;
            }
            else
            {
                // skip the rest for now...
                SkipToEOL(false);
                if (!SkipWhiteSpace())
                    return false;
            }
            if (skipPreprocessor)
                break;
        }

        while (CurrentChar() == '[')
        {
            // array subscripts
            // skip them for now...
            SkipBlock('[');
            if (!SkipWhiteSpace())
                return false;
        }

        while (CurrentChar() == '=')
        {
            // skip assignments
            // TODO: what happens with operators?
            if (!SkipToOneOfChars(_T(",;}"), true))
                return false;
        }

        while (CurrentChar() == '?')
        {
            // skip "condition ? true : false"
            // TODO: what happens with operators?
            if (!SkipToOneOfChars(_T(";}")))
                return false;
        }
        if (skipPreprocessor)
            break;
    }
    return true;
}

wxString Tokenizer::GetToken()
{
    m_UndoTokenIndex = m_TokenIndex;
    m_UndoLineNumber = m_LineNumber;
    m_UndoNestLevel  = m_NestLevel;

    if(m_PeekAvailable)
    {
        m_TokenIndex = m_PeekTokenIndex;
        m_LineNumber = m_PeekLineNumber;
        m_NestLevel  = m_PeekNestLevel;
        m_Token      = m_PeekToken;
    }
    else
        m_Token = DoGetToken();

    m_PeekAvailable = false;

    return ThisOrReplacement(m_Token);
}

wxString Tokenizer::PeekToken()
{
    if(!m_PeekAvailable)
    {
        m_PeekAvailable = true;
        unsigned int undoTokenIndex = m_TokenIndex;
        unsigned int undoLineNumber = m_LineNumber;
        unsigned int undoNestLevel  = m_NestLevel;
        m_PeekToken                 = DoGetToken();
        m_PeekTokenIndex            = m_TokenIndex;
        m_PeekLineNumber            = m_LineNumber;
        m_PeekNestLevel             = m_NestLevel;
        m_TokenIndex                = undoTokenIndex;
        m_LineNumber                = undoLineNumber;
        m_NestLevel                 = undoNestLevel;
    }
    return m_PeekToken;
}

void Tokenizer::UngetToken()
{
    m_PeekTokenIndex = m_TokenIndex;
    m_PeekLineNumber = m_LineNumber;
    m_PeekNestLevel  = m_NestLevel;
    m_TokenIndex     = m_UndoTokenIndex;
    m_LineNumber     = m_UndoLineNumber;
    m_NestLevel      = m_UndoNestLevel;
    m_PeekToken      = m_Token;
    m_PeekAvailable  = true;
}

wxString Tokenizer::DoGetToken()
{
    if (IsEOF())
        return wxEmptyString;

    if (!SkipWhiteSpace())
        return wxEmptyString;

    if (m_SkipUnwantedTokens && !SkipUnwanted())
        return wxEmptyString;

    // if m_SkipUnwantedTokens is false, we need to handle comments here too
    if (!m_SkipUnwantedTokens)
        SkipComment();

    int start = m_TokenIndex;
    wxString str;
    wxChar c = CurrentChar();

    if (c == '_' || wxIsalpha(c))
    {
        // keywords, identifiers, etc.

        // operator== is cheaper than wxIsalnum, also MoveToNextChar already includes IsEOF
        while (    ( (c == '_') || (wxIsalnum(c)) )
               &&  MoveToNextChar() )
            c = CurrentChar(); // repeat

        if (IsEOF())
            return wxEmptyString;

        str = m_Buffer.Mid(start, m_TokenIndex - start);
        m_IsOperator = str.IsSameAs(TokenizerConsts::operator_str);
    }
#ifdef __WXMSW__ // This is a Windows only bug!
    else if (c == 178 || c == 179 || c == 185) // fetch ² and ³
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
        m_IsOperator = false;
    }
    else if ( (c == '"') || (c == '\'') )
    {
        // string, char, etc.
        wxChar match = c;

        MoveToNextChar();  // skip starting ' or "

        if (!SkipToChar(match))
            return wxEmptyString;

        MoveToNextChar(); // skip ending ' or "

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
    else if (c == '(')
    {
        m_IsOperator = false;

        // skip blocks () []
        if (!SkipBlock(CurrentChar()))
            return wxEmptyString;

        str = FixArgument(m_Buffer.Mid(start, m_TokenIndex - start));
        CompactSpaces(str);
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

    if (    m_LastWasPreprocessor
        && !str.IsSameAs(_T("#"))
        && !m_LastPreprocessor.IsSameAs(_T("#")) )
    {
        if (!m_LastPreprocessor.IsSameAs(TokenizerConsts::include_str))
        {
            // except for #include and #if[[n]def], all other preprocessor directives need only
            // one word exactly after the directive, e.g. #define THIS_WORD
            SkipToEOL();
        }
        m_LastPreprocessor.Clear();
    }

    if (m_LastWasPreprocessor)
        m_LastPreprocessor << str;

    m_LastWasPreprocessor = false;

    return str;
}

wxString Tokenizer::FixArgument(wxString src)
{
    wxString dst;

    // str.Replace is massive overkill here since it has to allocate one new block per replacement
    { // this is much faster:
        size_t i;
        while((i = src.find_first_of(TokenizerConsts::tabcrlf)) != wxString::npos)
            src[i] = _T(' ');
    }

    // fix-up arguments (remove excessive spaces/tabs/newlines)
    for (unsigned int i = 0; i < src.Length() - 1; ++i)
    {
        // skip spaces before '=' and ','
        if (   (src.GetChar(i) == ' ')
            && (   (src.GetChar(i + 1) == ',')
                || (src.GetChar(i + 1) == '=') ) )
            continue;

        if (   (src.GetChar(i)     == '/')
            && (src.GetChar(i + 1) == '*') )
        {
            // skip C comments
            i += 2;
            while (i < src.Length() - 1)
            {
                if (   (src.GetChar(i)     == '*')
                    && (src.GetChar(i + 1) == '/') )
                    break;
                ++i;
            }

            if (   (i >= src.Length() - 1)
                || (src.GetChar(i + 1) != '/') )
                continue; // we failed...

            i += 2;
        }
        else if (src.GetChar(i) == '=')
        {
            // skip default assignments
            ++i;
            int level = 0; // nesting parenthesis
            while (i < src.Length())
            {
                if      (src.GetChar(i) == '(')
                    ++level;
                else if (src.GetChar(i) == ')')
                    --level;

                if (   (src.GetChar(i) == ',' && level == 0)
                    || (src.GetChar(i) == ')' && level < 0) )
                    break;

                ++i;
            }

            if (   (i < src.Length())
                && (src.GetChar(i) == ',') )
                --i;
            continue; // we are done here
        }

        if (i < src.Length() - 1)
        {
            if (   (src.GetChar(i)     == ' ')
                && (src.GetChar(i + 1) == ' ') )
                continue; // skip excessive spaces

            // in case of c-style comments "i" might already be src.Length()
            // thus do only add the current char otherwise.
            // otherwise the following statement:
            // dst << _T(')');
            // below would add another closing bracket.
            dst << src.GetChar(i);
        }
    }

    dst << _T(')'); // add closing parenthesis (see "i < src.Length() - 1" in previous "for")
    // str.Replace is massive overkill here since it has to allocate one new block per replacement

    return dst;
}

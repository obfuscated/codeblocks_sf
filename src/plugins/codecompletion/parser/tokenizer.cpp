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
    m_State(tsSkipUnWanted),
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
        TRACE(_T("Init() : m_Filename='%s'"), m_Filename.wx_str());
    }

    if (!wxFileExists(m_Filename))
    {
        TRACE(_T("Init() : File '%s' does not exist."), m_Filename.wx_str());
        return false;
    }

    if (!ReadFile())
    {
        TRACE(_T("Init() : File '%s' could not be read."), m_Filename.wx_str());
        return false;
    }

    if (!m_BufferLen)
    {
        TRACE(_T("Init() : File '%s' is empty."), m_Filename.wx_str());
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
    m_BufferLen         = 0;
    m_TokenIndex        = 0;
    m_LineNumber        = 1;
    m_NestLevel         = 0;
    m_SavedNestingLevel = 0;
    m_UndoTokenIndex    = 0;
    m_UndoLineNumber    = 1;
    m_UndoNestLevel     = 0;
    m_PeekTokenIndex    = 0;
    m_PeekLineNumber    = 0;
    m_PeekNestLevel     = 0;
    m_IsOK              = false;
    m_IsOperator        = false;
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

//    size_t replacements  = m_Buffer.Replace(_T("_GLIBCXX_BEGIN_NESTED_NAMESPACE(std, _GLIBCXX_STD_D)"), _T("namespace std {"),       true);
//           replacements += m_Buffer.Replace(_T("_GLIBCXX_BEGIN_NESTED_NAMESPACE(std, _GLIBCXX_STD_P)"), _T("namespace std {"),       true);
//           replacements += m_Buffer.Replace(_T("_GLIBCXX_END_NESTED_NAMESPACE"),                        _T("}"),                     true);
//           replacements += m_Buffer.Replace(_T("_GLIBCXX_BEGIN_NAMESPACE_TR1"),                         _T("namespace tr1 {"),       true);
//           // The following must be before replacing "_GLIBCXX_END_NAMESPACE"!!!
//           replacements += m_Buffer.Replace(_T("_GLIBCXX_END_NAMESPACE_TR1"),                           _T("}"),                     true);
//           replacements += m_Buffer.Replace(_T("_GLIBCXX_BEGIN_NAMESPACE(__gnu_cxx)"),                  _T("namespace __gnu_cxx {"), true);
//           replacements += m_Buffer.Replace(_T("_GLIBCXX_BEGIN_NAMESPACE(std)"),                        _T("namespace std {"),       true);
//           replacements += m_Buffer.Replace(_T("_GLIBCXX_END_NAMESPACE"),                               _T("}"),                     true);
//
//    if (replacements) TRACE(_T("Did %d replacements in buffer of '%s'."), replacements, fileName.wx_str());

    m_BufferLen = m_Buffer.Length();

    // add 'sentinel' to the end of the string (not counted to the length of the string)
    m_Buffer += _T(' ');

    return success;
}

bool Tokenizer::SkipWhiteSpace()
{
    // skip spaces, tabs, etc.
    // don't check EOF when MoveToNextChar already does, also replace isspace() which calls msvcrt.dll
    // with a dirty hack: CurrentChar() <= ' ' is "good enough" here
    while (CurrentChar() <= _T(' ') && MoveToNextChar())
        ;

    if (IsEOF())
        return false;

    return true;
}

// only be called when we are in a C-string,
// To check whether the current charactor is the real end of C-string
// See SkipToStringEnd() for more details
bool Tokenizer::IsEscapedChar()
{
    // Easy: If previous char is not a backslash, too than it's surely escape'd
    if (PreviousChar() != '\\')
        return true;
    else
    {
        // check for multiple backslashes, e.g. "\\"
        unsigned int numBackslash = 2; // for sure we have at least two at this point
        while (   ((m_TokenIndex - numBackslash) >= 0)
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

// expect we are not in a C-string
bool Tokenizer::SkipToChar(const wxChar& ch)
{
    // skip everything until we find ch

    while (CurrentChar() != ch && MoveToNextChar())  // don't check EOF when MoveToNextChar already does
        ;

    if (IsEOF())
        return false;

    return true;
}

//  For example: X"ABCDEFG\"HIJKLMN"Y
//  We are now at A, and would skip to Y
//  The double quote before H is a "C-escaped-character", We shouldn't quite from that
bool Tokenizer::SkipToStringEnd(const wxChar& ch)
{
    while (true)
    {
        while (CurrentChar() != ch && MoveToNextChar()) // don't check EOF when MoveToNextChar already does
            ;

        if (IsEOF())
            return false;

        if (IsEscapedChar()) break;

        MoveToNextChar();
    }
    return true;
}

// return true if we really skip a string, that means m_TokenIndex has changed.
bool Tokenizer::SkipString()
{
    if (CurrentChar() == '"' || CurrentChar() == '\'')
    {
        // this is the case that match is inside a string!
        wxChar ch = CurrentChar();
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

        while (SkipString()||SkipComment())
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
						SkipToEOL(true, true);
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
            if (!skippingComment)
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
    int nestLevel = 1; // counter for nested blocks (xxx())
    while (NotEOF())
    {

        while (SkipString() || SkipComment())
            ;

        if (CurrentChar() == ch)
            ++nestLevel;
        else if (CurrentChar() == match)
            --nestLevel;

        MoveToNextChar();

        if (nestLevel == 0)
            break;
    }

    if (IsEOF())
        return false;
    return true;
}

// if we really move forward, return true, which means we have the new m_TokenIndex
// if we stay here, return false
bool Tokenizer::SkipComment(bool skipEndWhite)
{
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


    // Here, we are in the comment body
    while (true)
    {
        if (cstyle)      // C style comment
        {
            SkipToChar('/');
            if (PreviousChar() == '*') // end of a C style comment
            {
                MoveToNextChar();
                break;
            }
            if (!MoveToNextChar())
                break;
        }
        else             // C++ style comment
        {
            SkipToEOL(false, true); // nestBrace = false, skipComment = true
            MoveToNextChar();
            break;
        }
    }

    if (IsEOF())
        return false;

    if (skipEndWhite)
    {
        if (!SkipWhiteSpace())
            return false;
        SkipComment();
        return true;
    }

    return true;
}

bool Tokenizer::SkipUnwanted()
{
    SkipComment();
    wxChar c = CurrentChar();
    // skip [XXX][YYY]
    if (m_State&tsSkipSubScrip)
    {
        while (c == _T('[') )
        {
            SkipBlock('[');
            if (!SkipWhiteSpace())
                return false;
            c = CurrentChar();
        }
    }

    // skip the following = or ?
    if (m_State&tsSkipEqual)
    {
        if (c == _T('='))
        {
            if (!SkipToOneOfChars(_T(",;}"), true, true, false))
                return false;
        }
    }
    else if (m_State&tsSkipQuestion)
    {
        if (c == _T('?'))
        {
            if (!SkipToOneOfChars(_T(";}"), false, true))
                return false;
        }
    }

    // skip the following white space and comments
    if (!SkipWhiteSpace())
        return false;

    SkipComment();

    return true;
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
        m_Token = DoGetToken();

    m_PeekAvailable = false;

    return m_Token;
}

wxString Tokenizer::PeekToken()
{
    if (!m_PeekAvailable)
    {
        m_PeekAvailable = true;

        unsigned int savedTokenIndex = m_TokenIndex;
        unsigned int savedLineNumber = m_LineNumber;
        unsigned int savedNestLevel  = m_NestLevel;

        m_PeekToken                  = DoGetToken();

        m_PeekTokenIndex             = m_TokenIndex;
        m_PeekLineNumber             = m_LineNumber;
        m_PeekNestLevel              = m_NestLevel;

        m_TokenIndex                 = savedTokenIndex;
        m_LineNumber                 = savedLineNumber;
        m_NestLevel                  = savedNestLevel;

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

    if (!SkipUnwanted())
        return wxEmptyString;;

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


    if (needReplace)
        return MacroReplace(str);

    return str;
}

wxString Tokenizer::FixArgument(wxString src)
{
    wxString dst;

    TRACE(_T("FixArgument() : src='%s'."), src.wx_str());

    // str.Replace is massive overkill here since it has to allocate one new block per replacement
    { // this is much faster:
        size_t i;
        while ((i = src.find_first_of(TokenizerConsts::tabcrlf)) != wxString::npos)
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

    TRACE(_T("FixArgument() : dst='%s'."), dst.wx_str());

    return dst;
}

wxString Tokenizer::MacroReplace(const wxString str)
{
    ConfigManagerContainer::StringToStringMap::const_iterator it = s_Replacements.find(str);

    if (it != s_Replacements.end())
    {
        // match one!
        wxString key   = it->first;
        wxString value = it->second;
        TRACE(_T("MacroReplace() : Replacing '%s' with '%s' (file='%s')."), key.wx_str(), value.wx_str(), m_Filename.wx_str());
        if (value[0]=='+' && CurrentChar()=='(')
        {
            unsigned int start = m_TokenIndex;
            m_Buffer[start] = ' ';
            bool fillSpace = false;
            while (m_Buffer[start]!=')')
            {
                if (m_Buffer[start]==',')
                    fillSpace = true;

                if (fillSpace==true)
                    m_Buffer[start]=' ';

                start++;
            }
            m_Buffer[start] = '{';
            return value.Remove(0,1);
        }
        else if (value[0] == '-')
        {
            unsigned int lenKey = key.Len();
            value = value.Remove(0,1);
            unsigned int lenValue = value.Len();

            for (unsigned int i=1; i<=lenKey; i++)
            {
                if (i < lenValue)
                    m_Buffer[m_TokenIndex-i] = value[lenValue-i];
                else
                    m_Buffer[m_TokenIndex-i] = ' ';
            }

            int firstSpace = value.First(' ');
            // adjust m_TokenIndex
            m_TokenIndex = m_TokenIndex - lenValue + firstSpace;

            return value.Mid(0,firstSpace);
        }
        else
            return value;
    }

    return str;
}


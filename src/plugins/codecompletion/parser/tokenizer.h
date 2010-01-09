/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <wx/string.h>
#include <configmanager.h>
#include <filemanager.h>


enum TokenizerState
{
    tsSkipEqual        = 0x0001,
    tsSkipQuestion     = 0x0002,
    tsSkipSubScrip     = 0x0004,
    tsSingleAngleBrace = 0x0008,

    tsSkipNone         = 0x1000,
    // convenient masks
    tsSkipUnWanted     = tsSkipEqual | tsSkipQuestion | tsSkipSubScrip,
    tsTemplateArgument = tsSkipUnWanted|tsSingleAngleBrace

};


struct TokenizerOptions
{
    bool wantPreprocessor;
};

class Tokenizer
{
public:
    Tokenizer(const wxString& filename = wxEmptyString);
    ~Tokenizer();

    bool Init(const wxString& filename = wxEmptyString, LoaderBase* loader = 0);
    bool InitFromBuffer(const wxString& buffer);


    wxString GetToken();
    wxString PeekToken();
    void     UngetToken();

    void SetTokenizerOption(bool wantPreprocessor)
    {
      m_TokenizerOptions.wantPreprocessor = wantPreprocessor;
    };

    void SetState(TokenizerState state)
    {
        m_State = state;
    };

    TokenizerState GetState()
    {
        return m_State;
    }

    bool IsSkippingUnwantedTokens() const
    {
        return (m_State == tsSkipUnWanted);
    };

    const wxString& GetFilename() const
    {
        return m_Filename;
    };

    unsigned int GetLineNumber() const
    {
        return m_LineNumber;
    };

    unsigned int GetNestingLevel() const
    {
        return m_NestLevel;
    };

    void SaveNestingLevel() // the parser might need to ignore the nesting level in some cases
    {
        m_SavedNestingLevel = m_NestLevel;
    };

    void RestoreNestingLevel()
    {
        m_NestLevel = m_SavedNestingLevel;
    };

    bool IsOK() const
    {
        return m_IsOK;
    };

    wxString ReadToEOL(bool nestBraces = true); // use with care outside this class!

    bool SkipToEOL(bool nestBraces = true, bool skippingComment = false); // use with care outside this class!

    static void SetReplacementString(const wxString& from, const wxString& to)
    {
        s_Replacements.insert(s_Replacements.end(), std::make_pair(from, to));
    };

    static void RemoveReplacementString(const wxString& from)
    {
        ConfigManagerContainer::StringToStringMap::iterator it = s_Replacements.find(from);
        if (it != s_Replacements.end())
            s_Replacements.erase(it);
    };

    static ConfigManagerContainer::StringToStringMap& GetTokenReplacementsMap()
    {
        return s_Replacements;
    };

    bool IsEOF() const
    {
        return m_TokenIndex >= m_BufferLen;
    };

    bool NotEOF() const
    {
        return m_TokenIndex < m_BufferLen;
    };

protected:
    void BaseInit();

    wxString DoGetToken();
    wxString FixArgument(wxString src);
    bool ReadFile();
    bool SkipWhiteSpace();
    bool IsEscapedChar();
    bool SkipToChar(const wxChar& ch);
    bool SkipToOneOfChars(const wxChar* chars, bool supportNesting = false);
    bool SkipBlock(const wxChar& ch);
    bool SkipUnwanted(); // skips comments, assignments, preprocessor etc.
    bool SkipComment(bool skipWhiteAtEnd = true);
    bool SkipString();
    bool SkipToStringEnd(const wxChar& ch);

    bool MoveToNextChar(const unsigned int amount = 1)
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

            if (CurrentChar() == _T('\n'))
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

            if (CurrentChar() == _T('\n'))
                ++m_LineNumber;
            return true;
        }
    };

    wxChar CurrentChar() const
    {
        if(m_TokenIndex < m_BufferLen)
            return m_Buffer.GetChar(m_TokenIndex);
        return 0;
    };

    wxChar CurrentCharMoveNext()
    {
        wxChar c = CurrentChar();
        m_TokenIndex++;
        return c;
    };

    wxChar NextChar() const
    {
        if ((m_TokenIndex + 1) >= m_BufferLen) // m_TokenIndex + 1) < 0  can never be true
            return 0;

        return m_Buffer.GetChar(m_TokenIndex + 1);
    };

    wxChar PreviousChar() const
    {
        if ( ((m_TokenIndex - 1) < 0) || (m_BufferLen==0) ) // (m_TokenIndex - 1) >= m_BufferLen can never be true
            return 0;

        return m_Buffer.GetChar(m_TokenIndex - 1);
    };

    void CompactSpaces(wxString& str) const  // zero-alloc single-copy  --- wxString::Replace has to do an awful lot of copying
    {
        if(str.size() < 3)
            return;

        wxChar c = 0;
        wxChar last = 0;
        size_t dst = 0;

        for(size_t src = 0; src < str.size(); ++src)
        {
            c = str[src];

            if(c == _T(' ') && (last == _T(' ') || last == _T('(')) )
                continue;
            else if(c == _T(')') && last == _T(' '))
                --dst;

            str[dst++] = c;
            last = c;
        }
        str.Truncate(dst);
    };

private:
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

    inline const wxString& ThisOrReplacement(const wxString& str) const
    {
        ConfigManagerContainer::StringToStringMap::const_iterator it = s_Replacements.find(str);
        if (it != s_Replacements.end())
            return it->second;
        return str;
    };

    wxString MacroReplace(const wxString str);

    TokenizerOptions m_TokenizerOptions;
    wxString         m_Filename;
    wxString         m_Buffer;
    unsigned int     m_BufferLen;

    wxString         m_Token;
    unsigned int     m_TokenIndex;
    unsigned int     m_LineNumber;
    unsigned int     m_NestLevel; // keep track of block nesting { }
    unsigned int     m_SavedNestingLevel;

    unsigned int     m_UndoTokenIndex;
    unsigned int     m_UndoLineNumber;
    unsigned int     m_UndoNestLevel;

    bool             m_PeekAvailable;
    wxString         m_PeekToken;
    unsigned int     m_PeekTokenIndex;
    unsigned int     m_PeekLineNumber;
    unsigned int     m_PeekNestLevel;

    bool             m_IsOK;
    bool             m_IsOperator;
    bool             m_SkipUnwantedTokens;

    TokenizerState   m_State;

    LoaderBase*      m_pLoader;

    static ConfigManagerContainer::StringToStringMap s_Replacements;
};

#endif // TOKENIZER_H



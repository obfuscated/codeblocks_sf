#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <wx/string.h>
#include <configmanager.h>
#include <filemanager.h>

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
    void UngetToken();

    void SetSkipUnwantedTokens(bool skip)
    {
        m_SkipUnwantedTokens = skip;
    }

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
    }
    void RestoreNestingLevel()
    {
        m_NestLevel = m_SavedNestingLevel;
    }
    bool IsOK() const
    {
        return m_IsOK;
    };
    TokenizerOptions m_Options;

    bool SkipToEOL(bool nestBraces = true); // use with care outside this class!

    static void SetReplacementString(const wxString& from, const wxString& to)
    {
        s_Replacements.insert(s_Replacements.end(), std::make_pair(from, to));
    }

    static void RemoveReplacementString(const wxString& from)
    {
        ConfigManagerContainer::StringToStringMap::iterator it = s_Replacements.find(from);
        if (it != s_Replacements.end())
            s_Replacements.erase(it);
    }

    static ConfigManagerContainer::StringToStringMap& GetTokenReplacementsMap()
    {
        return s_Replacements;
    }

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
    bool ReadFile();
    bool SkipWhiteSpace();
    bool SkipToChar(const wxChar& ch);
    bool SkipToOneOfChars(const char* chars, bool supportNesting = false);
    bool SkipBlock(const wxChar& ch);
    bool SkipUnwanted(); // skips comments, assignments, preprocessor etc.
    bool SkipComment();

    bool MoveToNextChar(const unsigned int amount = 1)
    {
        assert(amount);
        if(amount == 1) // compiler will dead-strip this
        {
            ++m_TokenIndex;
            if (IsEOF())
                return false;

            if (CurrentChar() == _T('\n'))
                ++m_LineNumber;
            return true;
        }
        else
        {
            m_TokenIndex += amount;
            if (IsEOF())
                return false;

            if (CurrentChar() == _T('\n'))
                ++m_LineNumber;
            return true;
        }
    };

    wxChar CurrentChar() const
    {
        return m_Buffer.GetChar(m_TokenIndex);
    };

    wxChar CurrentCharMoveNext()
    {
    	size_t i = m_TokenIndex++;

    	if(m_TokenIndex < m_BufferLen)
			return m_Buffer.GetChar(i);
		else
			return 0;
    };

    wxChar NextChar() const
    {
        if ((m_TokenIndex + 1) >= m_BufferLen) //    m_TokenIndex + 1) < 0  can never be true
            return 0;
        return m_Buffer.GetChar(m_TokenIndex + 1);
    };

    wxChar PreviousChar() const
    {
        if ((m_TokenIndex - 1) < 0)       //   (m_TokenIndex - 1) >= m_BufferLen can never be true
            return 0;
        return m_Buffer.GetChar(m_TokenIndex - 1);
    };

    void CompactSpaces(wxString& str) const  // zero-alloc single-copy  --- wxString::Replace has to do an awful lot of copying
    {
    	if(str.size() < 3)
    	return;
//          str.Replace(_T("  "), _T(" "));   // replace two-spaces with single-space (introduced if it skipped comments or assignments)
//          str.Replace(_T("( "), _T("("));
//          str.Replace(_T(" )"), _T(")"));

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
    bool CharInString(const char ch, const char* chars) const
    {
        int len = strlen(chars);
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
    }

    wxString m_Filename;
    wxString m_Buffer;
    wxString m_peek;
    wxString m_curtoken;
    bool m_peekavailable;
    unsigned int m_BufferLen;
    unsigned int m_NestLevel; // keep track of block nesting { }
    unsigned int m_UndoNestLevel;
    unsigned int m_TokenIndex;
    unsigned int m_UndoTokenIndex;
    unsigned int m_LineNumber;
    unsigned int m_UndoLineNumber;
	unsigned int m_PeekTokenIndex;
	unsigned int m_PeekLineNumber;
	unsigned int m_PeekNestLevel;

    unsigned int m_SavedNestingLevel;

    bool m_IsOK;
    bool m_IsOperator;
    bool m_LastWasPreprocessor;
    wxString m_LastPreprocessor;
    bool m_SkipUnwantedTokens;
    
    LoaderBase* m_pLoader;

    static ConfigManagerContainer::StringToStringMap s_Replacements;
};

#endif // TOKENIZER_H



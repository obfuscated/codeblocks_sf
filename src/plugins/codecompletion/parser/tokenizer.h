#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <wx/string.h>
#include <gcc-attribs.h>

struct TokenizerOptions
{
    bool wantPreprocessor;
};

class Tokenizer
{
public:
    Tokenizer(const wxString& filename = wxEmptyString);
    ~Tokenizer();

    bool Init(const wxString& filename = wxEmptyString);
    bool InitFromBuffer(const wxString& buffer);
    wxString GetToken();
    wxString PeekToken();
    void UngetToken();

    ___inline___ const wxString& GetFilename() const
    {
        return m_Filename;
    };
    ___inline___ unsigned int GetLineNumber() const
    {
        return m_LineNumber;
    };
    ___inline___ unsigned int GetNestingLevel() const
    {
        return m_NestLevel;
    };
    ___inline___ bool IsOK() const
    {
        return m_IsOK;
    };
    TokenizerOptions m_Options;
protected:
    void BaseInit();
    wxString DoGetToken();
    bool ReadFile();
    bool SkipWhiteSpace();
    bool SkipToChar(const wxChar& ch);
    bool SkipToOneOfChars(const char* chars, bool supportNesting = false);
    bool SkipToEOL();
    bool SkipBlock(const wxChar& ch);
    bool SkipUnwanted(); // skips comments, assignments, preprocessor etc.

    ___inline___ bool IsEOF() const
    {
        return m_TokenIndex >= m_BufferLen;
    };

    ___inline___ bool NotEOF() const
    {
        return m_TokenIndex < m_BufferLen;
    };

    ___inline___ bool MoveToNextChar(const unsigned int amount = 1)
    {
        assert(amount);
        if(amount == 1) // compiler will dead-strip this
        {
            ++m_TokenIndex;
            if (unlikely(IsEOF()))
                return false;

            if (CurrentChar() == _T('\n'))
                ++m_LineNumber;
            return true;
        }
        else
        {
            m_TokenIndex += amount;
            if (unlikely(IsEOF()))
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

    	if(likely(m_TokenIndex < m_BufferLen))
			return m_Buffer.GetChar(i);
		else
			return 0;
    };

    wxChar Tokenizer::NextChar() const
    {
        if (unlikely((m_TokenIndex + 1) >= m_BufferLen)) //    m_TokenIndex + 1) < 0  can never be true
            return 0;
        return m_Buffer.GetChar(m_TokenIndex + 1);
    };

    wxChar Tokenizer::PreviousChar() const
    {
        if (unlikely((m_TokenIndex - 1) < 0))       //   (m_TokenIndex - 1) >= m_BufferLen can never be true
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
    ___pure___ bool CharInString(const char ch, const char* chars) const
    {
        int len = strlen(chars);
        for (int i = 0; i < len; ++i)
        {
            if (ch == chars[i])
                return true;
        }
        return false;
    };

    wxString m_Filename;
    wxString m_Buffer;
    unsigned int m_BufferLen;
    unsigned int m_NestLevel; // keep track of block nesting { }
    unsigned int m_UndoNestLevel;
    unsigned int m_TokenIndex;
    unsigned int m_UndoTokenIndex;
    unsigned int m_LineNumber;
    unsigned int m_UndoLineNumber;
    bool m_IsOK;
    bool m_IsOperator;
    bool m_LastWasPreprocessor;
    wxString m_LastPreprocessor;
};

#endif // TOKENIZER_H



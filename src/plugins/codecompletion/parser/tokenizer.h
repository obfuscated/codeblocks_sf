#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <wx/string.h>

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
		const wxString& GetFilename(){ return m_Filename; }
		unsigned int GetLineNumber(){ return m_LineNumber; }
		unsigned int GetNestingLevel(){ return m_NestLevel; }
		bool IsOK(){ return m_IsOK; }
		TokenizerOptions m_Options;
	protected:
		void BaseInit();
		wxString DoGetToken();
		bool ReadFile();
		bool SkipWhiteSpace();
		bool SkipToChar(const char& ch);
		bool SkipToOneOfChars(const char* chars, bool supportNesting = false);
		bool SkipToEOL();
		bool SkipBlock(const wxChar& ch);
		bool SkipUnwanted(); // skips comments, assignments, preprocessor etc.
		bool IsEOF(){ return m_TokenIndex >= m_BufferLen; }
		bool MoveToNextChar();
		void AdjustLineNumber();
		wxChar CurrentChar();
		wxChar NextChar();
		wxChar PreviousChar();
	private:
		bool CharInString(const char ch, const char* chars);
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


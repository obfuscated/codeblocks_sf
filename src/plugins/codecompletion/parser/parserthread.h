#ifndef PARSERTHREAD_H
#define PARSERTHREAD_H

#include <wx/thread.h>
#include <wx/event.h>
#include <wx/string.h>
#include <wx/dynarray.h>
#include "tokenizer.h"
#include "token.h"
#include <cbthreadpool.h>

#include <queue>

//extern int THREAD_START;
//extern int THREAD_END;
extern int NEW_TOKEN;
extern int FILE_NEEDS_PARSING;

class Parser;
static wxCriticalSection s_mutexListProtection;

struct ParserThreadOptions
{
    /** useBuffer specifies that we're not parsing a file,  but a temporary
      * buffer. The resulting tokens will be temporary, too,
      * and will be deleted when the next file is parsed.
      */
    bool useBuffer;
	bool bufferSkipBlocks;
	bool wantPreprocessor;
	bool followLocalIncludes;
	bool followGlobalIncludes;
};

class ParserThread : public cbThreadedTask
{
	public:
		ParserThread(Parser* parent,
                    const wxString& bufferOrFilename,
					bool isLocal,
					ParserThreadOptions& options,
					TokensTree* tree);
		virtual ~ParserThread();
		int Execute(){ return Parse() ? 0 : 1; }
		bool Parse();
		bool ParseBufferForFunctions(const wxString& buffer);
		virtual void* DoRun();
        virtual void SetTokens(TokensTree* tokens);
		const wxString& GetFilename() const { return m_Filename; }
	protected:
		wxChar SkipToOneOfChars(const wxString& chars, bool supportNesting = false);
		void DoParse();
		void SkipBlock();
		void SkipAngleBraces();
		void HandleIncludes();
		void HandleDefines();
		void HandleNamespace();
		void HandleClass(bool isClass = true);
		void HandleFunction(const wxString& name, bool isOperator = false);
		void HandleEnum();
		Token* DoAddToken(TokenKind kind, const wxString& name, int line, const wxString& args = wxEmptyString, bool isOperator = false, bool isImpl = false);
		wxString GetActualTokenType();
	private:
		void Log(const wxString& log);
		Token* TokenExists(const wxString& name, Token* parent = 0, short int kindMask = 0xFFFF); // if parent is 0, all tokens are searched
		wxString GetQueueAsNamespaceString(std::queue<wxString>& q); // NOTICE: clears the queue too
        Token* FindTokenFromQueue(std::queue<wxString>& q, Token* parent = 0, bool createIfNotExist = false);

		Tokenizer m_Tokenizer;
		Parser* m_pParent;
		TokensTree* m_pTokens;
		Token* m_pLastParent;
		TokenScope m_LastScope;
		wxString m_Filename;
		unsigned int m_File;
		bool m_IsLocal;
		wxString m_Str;
		wxString m_LastToken;
        ParserThreadOptions m_Options;
		std::queue<wxString> m_EncounteredNamespaces; // for member funcs implementation
		std::queue<wxString> m_EncounteredTypeNamespaces; // namespaces in types

        bool InitTokenizer();

		bool m_IsBuffer;
		wxString m_Buffer;
};

#endif // PARSERTHREAD_H

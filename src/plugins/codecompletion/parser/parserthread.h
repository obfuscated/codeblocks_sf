#ifndef PARSERTHREAD_H
#define PARSERTHREAD_H

#include <wx/thread.h>
#include <wx/event.h>
#include <wx/string.h>
#include <wx/dynarray.h>
#include "tokenizer.h"
#include "token.h"
#include <cbthreadpool.h>
#include <filemanager.h>

#include <queue>

//extern int THREAD_START;
//extern int THREAD_END;
extern int NEW_TOKEN;
extern int FILE_NEEDS_PARSING;

class Parser;
static wxCriticalSection s_mutexListProtection;

struct ParserThreadOptions
{
    ParserThreadOptions()
        : useBuffer(false),
        bufferSkipBlocks(false),
        bufferSkipOuterBlocks(false),
        wantPreprocessor(true),
        followLocalIncludes(true),
        followGlobalIncludes(true),
        isTemp(false),
        handleFunctions(true),
        handleVars(true),
        handleClasses(true),
        handleEnums(true),
        handleTypedefs(true),
        loader(0)
        {}
    /* useBuffer specifies that we're not parsing a file,  but a temporary
     * buffer. The resulting tokens will be temporary, too,
     * and will be deleted when the next file is parsed.
     */
    bool useBuffer;
	bool bufferSkipBlocks;
	bool bufferSkipOuterBlocks; // classes, namespaces and functions
	bool wantPreprocessor;
	bool followLocalIncludes;
	bool followGlobalIncludes;
	bool isTemp;

	bool handleFunctions;
	bool handleVars;
	bool handleClasses;
	bool handleEnums;
	bool handleTypedefs;
	
	LoaderBase* loader; // if not NULL, load through filemanager (using threads)
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
		bool ParseBufferForUsingNamespace(const wxString& buffer, wxArrayString& result);
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
		void HandlePreprocessorBlocks(const wxString& preproc);
		void HandleNamespace();
		void HandleClass(bool isClass = true);
		void HandleFunction(const wxString& name, bool isOperator = false);
		void HandleEnum();
		void HandleTypedef();
		Token* DoAddToken(TokenKind kind, const wxString& name, int line, int implLineStart = 0, int implLineEnd = 0, const wxString& args = wxEmptyString, bool isOperator = false, bool isImpl = false);
		wxString GetActualTokenType();
	private:
		void Log(const wxString& log);
		Token* TokenExists(const wxString& name, Token* parent = 0, short int kindMask = 0xFFFF); // if parent is 0, all tokens are searched
		wxString GetQueueAsNamespaceString(std::queue<wxString>& q); // NOTICE: clears the queue too
        Token* FindTokenFromQueue(std::queue<wxString>& q, Token* parent = 0, bool createIfNotExist = false, Token* parentIfCreated = 0);

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

		int m_PreprocessorIfCount; // handle nesting of #if...#if...#else...#endif...#endif

        bool InitTokenizer();

		bool m_IsBuffer;
		wxString m_Buffer;
};

#endif // PARSERTHREAD_H

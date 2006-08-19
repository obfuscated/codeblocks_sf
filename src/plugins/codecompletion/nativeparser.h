#ifndef NATIVEPARSER_H
#define NATIVEPARSER_H

#include <queue>

#include <wx/event.h>
#include <wx/hashmap.h>
#include "parser/parser.h"

//#define DEBUG_CC_AI

// forward decls
class cbEditor;
class EditorBase;
class cbProject;
class ClassBrowser;
//class Parser;
class Token;

WX_DECLARE_HASH_MAP(cbProject*, Parser*, wxPointerHash, wxPointerEqual, ParsersMap);
WX_DECLARE_HASH_MAP(cbProject*, wxString, wxPointerHash, wxPointerEqual, ParsersFilenameMap);

enum ParserTokenType
{
	pttSearchText = 0,
	pttClass,
	pttNamespace,
	pttFunction
};

enum BrowserViewMode
{
	bvmRaw = 0,
	bvmInheritance
};

struct ParserComponent
{
    wxString component;
    ParserTokenType token_type;
};

class NativeParser : public wxEvtHandler
{
	public:
		NativeParser();
		~NativeParser();

		void AddParser(cbProject* project, bool useCache = true);
		void RemoveParser(cbProject* project, bool useCache = true);
		void ClearParsers();
		void RereadParserOptions();
		void AddFileToParser(cbProject* project, const wxString& filename);
		void RemoveFileFromParser(cbProject* project, const wxString& filename);
		void ForceReparseActiveProject();

		size_t MarkItemsByAI(TokenIdxSet& result, bool reallyUseAI = true);

		const wxString& GetCodeCompletionItems();
		const wxArrayString& GetCallTips();
		int GetCallTipCommas(){ return m_CallTipCommas; }
        int CountCommas(const wxString& calltip, int start);
		void GetCallTipHighlight(const wxString& calltip, int* start, int* end);

		int GetEditorStartWord() const { return m_EditorStartWord; }
		int GetEditorEndWord() const { return m_EditorEndWord; }

		Parser* FindParserFromActiveEditor();
		Parser* FindParserFromEditor(cbEditor* editor);
		Parser* FindParserFromActiveProject();
		Parser* FindParserFromProject(cbProject* project);

        ClassBrowser* GetClassBrowser() const { return m_pClassBrowser; }
		void CreateClassBrowser();
		void RemoveClassBrowser(bool appShutDown = false);
		void UpdateClassBrowser();
		void SetClassBrowserProject(cbProject* project);
		void SetCBViewMode(const BrowserViewMode& mode);
	protected:
	private:
        friend class CodeCompletion;
		size_t AI(TokenIdxSet& result, cbEditor* editor, Parser* parser, const wxString& lineText = wxEmptyString, bool noPartialMatch = false, bool caseSensitive = false);

		size_t FindAIMatches(Parser* parser, std::queue<ParserComponent> components, TokenIdxSet& result, int parentTokenIdx = -1, bool noPartialMatch = false, bool caseSensitive = false, bool use_inheritance = true, short int kindMask = 0xFFFF);
        size_t BreakUpComponents(Parser* parser, const wxString& actual, std::queue<ParserComponent>& components);
        bool BelongsToParentOrItsAncestors(TokensTree* tree, Token* token, int parentIdx, bool use_inheritance = true);
        size_t GenerateResultSet(TokensTree* tree, const wxString& search, int parentIdx, TokenIdxSet& result, bool caseSens = true, bool isPrefix = false, short int kindMask = 0xFFFF);

		bool ParseFunctionArguments(cbEditor* ed);
		bool ParseLocalBlock(cbEditor* ed); // parses from the start of function up to the cursor

		unsigned int FindCCTokenStart(const wxString& line);
		wxString GetNextCCToken(const wxString& line, unsigned int& startAt, bool& is_function);
		wxString GetCCToken(wxString& line, ParserTokenType& tokenType);
		bool FindFunctionNamespace(cbEditor* editor, wxString* nameSpace = 0L, wxString* procName = 0L);
		int FindCurrentFunctionStart(cbEditor* editor);
		bool IsFunctionSignature(cbEditor* editor, int& pos, bool* is_member_initialiser = 0);
		void AddCompilerDirs(Parser* parser, cbProject* project);
		bool LoadCachedData(Parser* parser, cbProject* project);
		bool SaveCachedData(Parser* parser, const wxString& projectFilename);
		void DisplayStatus(Parser* parser);
		void OnThreadStart(wxCommandEvent& event);
		void OnThreadEnd(wxCommandEvent& event);
		void OnParserEnd(wxCommandEvent& event);
		void OnEditorActivated(EditorBase* editor);

		bool SkipWhitespaceForward(cbEditor* editor, int& pos);
		bool SkipWhitespaceBackward(cbEditor* editor, int& pos);

        Parser m_Parser;
		int m_EditorStartWord;
		int m_EditorEndWord;
		wxString m_CCItems;
		wxArrayString m_CallTips;
		int m_CallTipCommas;
    	ClassBrowser* m_pClassBrowser;
    	bool m_ClassBrowserIsFloating; // docked to project manager notebook, or free floating?
    	bool m_GettingCalltips; // flag while getting calltips

        DECLARE_EVENT_TABLE()
};

#endif // NATIVEPARSER_H


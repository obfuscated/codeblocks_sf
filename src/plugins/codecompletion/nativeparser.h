#ifndef NATIVEPARSER_H
#define NATIVEPARSER_H

#include <wx/event.h>
#include <wx/hashmap.h>
#include "parser/parser.h"

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
	pttNamespace
};

enum BrowserViewMode
{
	bvmRaw = 0,
	bvmInheritance
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

		int MarkItemsByAI(bool reallyUseAI = true);

		const wxString& GetCodeCompletionItems();
		const wxArrayString& GetCallTips();

		int GetEditorStartWord(){ return m_EditorStartWord; }
		int GetEditorEndWord(){ return m_EditorEndWord; }

		Parser* FindParserFromActiveEditor();
		Parser* FindParserFromEditor(cbEditor* editor);
		Parser* FindParserFromActiveProject();
		Parser* FindParserFromProject(cbProject* project);

		void CreateClassBrowser();
		void RemoveClassBrowser(bool appShutDown = false);
		void SetClassBrowserProject(cbProject* project);
		void SetCBViewMode(const BrowserViewMode& mode);
	protected:
	private:
        friend class CodeCompletion;
		int AI(cbEditor* editor, Parser* parser, const wxString& lineText = wxEmptyString, bool noPartialMatch = false, bool caseSensitive = false);
		unsigned int FindCCTokenStart(const wxString& line);
		wxString GetNextCCToken(const wxString& line, unsigned int& startAt);
		wxString GetCCToken(wxString& line, ParserTokenType& tokenType);
		bool FindFunctionNamespace(cbEditor* editor, wxString* nameSpace = 0L, wxString* procName = 0L);
		int FindCurrentBlockStart(cbEditor* editor);
		int DoInheritanceAI(Token* parentToken, Token* scopeToken, const wxString& searchText = wxEmptyString, bool caseSensitive = true);
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
    	ClassBrowser* m_pClassBrowser;
    	bool m_ClassBrowserIsFloating; // docked to project manager notebook, or free floating?

        DECLARE_EVENT_TABLE()
};

#endif // NATIVEPARSER_H


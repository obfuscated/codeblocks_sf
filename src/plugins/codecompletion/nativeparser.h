#ifndef NATIVEPARSER_H
#define NATIVEPARSER_H

#include <wx/event.h>
#include <wx/hashmap.h>

// forward decls
class cbEditor;
class cbProject;
class ClassBrowser;
class Parser;
class Token;

WX_DECLARE_HASH_MAP(cbProject*, Parser*, wxPointerHash, wxPointerEqual, ParsersMap);

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

		void AddParser(cbProject* project);
		void RemoveParser(cbProject* project);
		void ClearParsers();
		void RereadParserOptions();
		void AddFileToParser(cbProject* project, const wxString& filename);
		void RemoveFileFromParser(cbProject* project, const wxString& filename);

		int MarkItemsByAI();
		
		const wxString& GetCodeCompletionItems();
		const wxArrayString& GetCallTips();

		int GetEditorStartWord(){ return m_EditorStartWord; }
		int GetEditorEndWord(){ return m_EditorEndWord; }

		cbProject* FindProjectFromParser(Parser* parser);
		cbProject* FindProjectFromEditor(cbEditor* editor);
		cbProject* FindProjectFromActiveEditor();
		Parser* FindParserFromActiveEditor();
		Parser* FindParserFromEditor(cbEditor* editor);

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
		void OnThreadStart(wxCommandEvent& event);
		void OnThreadEnd(wxCommandEvent& event);
		void OnParserEnd(wxCommandEvent& event);
		
		ParsersMap m_Parsers;
		int m_EditorStartWord;
		int m_EditorEndWord;
		wxString m_CCItems;
		wxArrayString m_CallTips;
    	ClassBrowser* m_pClassBrowser;

        DECLARE_EVENT_TABLE()
};

#endif // NATIVEPARSER_H


#ifndef PARSER_H
#define PARSER_H

#include <wx/string.h>
#include <wx/treectrl.h>
#include <wx/event.h>
#include <wx/timer.h>
#include <wx/dynarray.h>
#include "parserthread.h"

#ifndef STANDALONE
	#include <wx/imaglist.h>
#endif // STANDALONE

#define PARSER_IMG_NONE				-2
#define PARSER_IMG_FOLDER			0
#define PARSER_IMG_CLASS			1
#define PARSER_IMG_CTOR_PRIVATE		2
#define PARSER_IMG_CTOR_PROTECTED	3
#define PARSER_IMG_CTOR_PUBLIC		4
#define PARSER_IMG_DTOR_PRIVATE		5
#define PARSER_IMG_DTOR_PROTECTED	6
#define PARSER_IMG_DTOR_PUBLIC		7
#define PARSER_IMG_FUNC_PRIVATE		8
#define PARSER_IMG_FUNC_PROTECTED	9
#define PARSER_IMG_FUNC_PUBLIC		10
#define PARSER_IMG_VAR_PRIVATE		11
#define PARSER_IMG_VAR_PROTECTED	12
#define PARSER_IMG_VAR_PUBLIC		13
#define PARSER_IMG_PREPROCESSOR		14
#define PARSER_IMG_ENUM				15
#define PARSER_IMG_ENUMERATOR		16
#define PARSER_IMG_NAMESPACE 		17

#define PARSER_IMG_MIN PARSER_IMG_FOLDER
#define PARSER_IMG_MAX PARSER_IMG_ENUMERATOR

WX_DEFINE_ARRAY(ParserThread*, ThreadsArray);
extern int PARSER_END;

class ClassTreeData : public wxTreeItemData
{
    public:
        ClassTreeData(Token* token){ m_Token = token; }
        Token* GetToken(){ return m_Token; }
        void SetToken(Token* token){ m_Token = token; }
    private:
        Token* m_Token;
};

struct ParserOptions
{
	bool followLocalIncludes;
	bool followGlobalIncludes;
	bool caseSensitive;
	bool wantPreprocessor;
};

struct BrowserOptions
{
	bool showInheritance;
	bool viewFlat;
};

class Parser : public wxEvtHandler
{
	public:
		Parser(wxEvtHandler* parent);
		~Parser();

		bool Parse(const wxString& filename, bool isLocal = true);
		bool Parse(const wxString& bufferOrFilename, bool isLocal, ParserThreadOptions& opts);
		bool ParseBuffer(const wxString& buffer, bool isLocal = true, bool bufferSkipBlocks = false);
		bool ParseBufferForFunctions(const wxString& buffer);
		bool Reparse(const wxString& filename, bool isLocal = true);
		bool RemoveFile(const wxString& filename);
		void Clear();
		void ReadOptions();
		void WriteOptions();

		void StartTimer(){ wxStartTimer(); }
		unsigned long int GetElapsedTime(){ return wxGetElapsedTime(); }
#ifndef STANDALONE
		int GetTokenKindImage(Token* token);
		void SetTokenKindImage(int kind, const wxBitmap& bitmap, const wxBitmap& mask = wxNullBitmap);
		void SetTokenKindImage(int kind, const wxBitmap& bitmap, const wxColour& maskColor);
		void SetTokenKindImage(int kind, const wxIcon& icon);
		wxImageList* GetImageList(){ return m_pImageList; }
#endif // STANDALONE
		Token* FindTokenByName(const wxString& name, bool globalsOnly = true);
		Token* FindChildTokenByName(Token* parent, const wxString& name, bool useInheritance = false);
		Token* FindTokenByDisplayName(const wxString& name);

		ParserOptions& Options(){ return m_Options; }
		BrowserOptions& ClassBrowserOptions(){ return m_BrowserOptions; }
		wxArrayString& IncludeDirs(){ return m_IncludeDirs; }

		const TokensArray& GetTokens(){ return m_Tokens; }
		unsigned int GetThreadsCount();
		unsigned int GetFilesCount();
		unsigned int GetLeftThreadsCount();
		
		bool Done();
		
		unsigned int GetMaxThreads(){ return m_MaxThreadsCount; }
		void SetMaxThreads(unsigned int max){ m_MaxThreadsCount = max; }

		void BuildTree(wxTreeCtrl& tree);
		wxTreeItemId GetRootNode(){ return m_RootNode; }

		void TerminateAllThreads();
		void PauseAllThreads();
		void ResumeAllThreads();
		wxString ThreadFilename(unsigned int idx);
		void ClearTemporaries();
		void SortAllTokens();
	protected:
		void OnStartThread(wxCommandEvent& event);
		void OnEndThread(wxCommandEvent& event);
		void OnNewToken(wxCommandEvent& event);
		void OnParseFile(wxCommandEvent& event);
	private:
		void AddTreeNode(wxTreeCtrl& tree, const wxTreeItemId& parentNode, Token* token, bool childrenOnly = false);
		void ScheduleThreads();
		void LinkInheritance(bool tempsOnly = false);
		ParserOptions m_Options;
		BrowserOptions m_BrowserOptions;
		unsigned int m_MaxThreadsCount;
		TokensArray m_Tokens;
		ThreadsArray m_Threads;
		ThreadsArray m_ThreadsStore;
		wxArrayString m_ParsedFiles;
		wxArrayString m_ReparsedFiles;
		wxArrayString m_IncludeDirs;
		wxEvtHandler* m_pParent;
		wxTreeItemId m_RootNode;
#ifndef STANDALONE
		wxImageList* m_pImageList;
#endif // STANDALONE
		
		DECLARE_EVENT_TABLE()
};

#endif // PARSER_H


#ifndef PARSER_H
#define PARSER_H

#include <wx/string.h>
#include <wx/treectrl.h>
#include <wx/event.h>
#include <wx/timer.h>
#include <wx/file.h>
#include <wx/filefn.h> // wxPathList
#include "parserthread.h"
#include <cbthreadpool.h>
#include <sdk_events.h>

#ifndef STANDALONE
	#include <wx/imaglist.h>
#endif // STANDALONE

#define PARSER_IMG_NONE				-2
#define PARSER_IMG_CLASS_FOLDER		0
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
#define PARSER_IMG_SYMBOLS_FOLDER	18
#define PARSER_IMG_ENUMS_FOLDER	    19
#define PARSER_IMG_PREPROC_FOLDER	20
#define PARSER_IMG_OTHERS_FOLDER	21

#define PARSER_IMG_MIN PARSER_IMG_CLASS_FOLDER
#define PARSER_IMG_MAX PARSER_IMG_OTHERS_FOLDER

extern int PARSER_END;

class ClassTreeData : public BlockAllocated<ClassTreeData, 500>, public wxTreeItemData
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
	bool useSmartSense;
};

struct BrowserOptions
{
	bool showInheritance; // default: false
	bool viewFlat; // default: false
	bool showAllSymbols; // default: false
};

class Parser : public wxEvtHandler
{
	public:
		Parser(wxEvtHandler* parent);
		~Parser();

		void BatchParse(const wxArrayString& filenames);
		bool Parse(const wxString& filename, bool isLocal = true);
		bool Parse(const wxString& bufferOrFilename, bool isLocal, ParserThreadOptions& opts);
		bool ParseBuffer(const wxString& buffer, bool isLocal = true, bool bufferSkipBlocks = false);
		bool ParseBufferForFunctions(const wxString& buffer);
		bool Reparse(const wxString& filename, bool isLocal = true);
		bool RemoveFile(const wxString& filename);
		void Clear();
		void ReadOptions();
		void WriteOptions();
		bool ReadFromCache(wxInputStream* f);
		bool WriteToCache(wxOutputStream* f);
		bool CacheNeedsUpdate();

		void StartTimer(){ wxStartTimer(); }
		unsigned long int GetElapsedTime(){ return wxGetElapsedTime(); }
#ifndef STANDALONE
		int GetTokenKindImage(Token* token);
		void SetTokenKindImage(int kind, const wxBitmap& bitmap, const wxBitmap& mask = wxNullBitmap);
		void SetTokenKindImage(int kind, const wxBitmap& bitmap, const wxColour& maskColor);
		void SetTokenKindImage(int kind, const wxIcon& icon);
		wxImageList* GetImageList(){ return m_pImageList; }
#endif // STANDALONE
		Token* FindTokenByName(const wxString& name, bool globalsOnly = true, short int kindMask = 0xFFFF) const;
		Token* FindChildTokenByName(Token* parent, const wxString& name, bool useInheritance = false, short int kindMask = 0xFFFF) const;
		Token* FindTokenByDisplayName(const wxString& name) const;

		ParserOptions& Options(){ return m_Options; }
		BrowserOptions& ClassBrowserOptions(){ return m_BrowserOptions; }

		void ClearIncludeDirs(){ m_IncludeDirs.Clear(); }
		void AddIncludeDir(const wxString& dir);
		wxString FindFileInIncludeDirs(const wxString& file);

		const TokensArray& GetTokens(){ return m_Tokens; }
		unsigned int GetFilesCount();

		bool Done();

		unsigned int GetMaxThreads(){ return m_MaxThreadsCount; }
		void SetMaxThreads(unsigned int max){ m_MaxThreadsCount = max; }

		void BuildTree(wxTreeCtrl& tree);
		wxTreeItemId GetRootNode(){ return m_RootNode; }

		void TerminateAllThreads();
		void PauseAllThreads();
		void ResumeAllThreads();
		void ClearTemporaries();
		void SortAllTokens();
	protected:
		void OnStartThread(CodeBlocksEvent& event);
		void OnEndThread(CodeBlocksEvent& event);
        void OnAllThreadsDone(CodeBlocksEvent& event);
		void OnNewToken(wxCommandEvent& event);
		void OnParseFile(wxCommandEvent& event);
	private:
        void ConnectEvents();
        void DisconnectEvents();
        void BuildTreeNamespace(wxTreeCtrl& tree, const wxTreeItemId& parentNode, Token* parent);
        void AddTreeNamespace(wxTreeCtrl& tree, const wxTreeItemId& parentNode, Token* parent);
		void AddTreeNode(wxTreeCtrl& tree, const wxTreeItemId& parentNode, Token* token, bool childrenOnly = false);
		void LinkInheritance(bool tempsOnly = false);
		ParserOptions m_Options;
		BrowserOptions m_BrowserOptions;
		unsigned int m_MaxThreadsCount;
		TokensArray m_Tokens;
		wxArrayString m_ParsedFiles;
		wxArrayString m_ReparsedFiles;
		wxPathList m_IncludeDirs;
		wxEvtHandler* m_pParent;
		wxTreeItemId m_RootNode;
#ifndef STANDALONE
		wxImageList* m_pImageList;
    protected:
        bool m_abort_flag;
        // the following three members are used to detect changes between
        // in-mem data and cache
        bool m_UsingCache; // true if loaded from cache
        int m_CacheFilesCount; // m_ParsedFiles.GetCount() when (if) loaded from cache
        int m_CacheTokensCount; // m_Tokens.GetCount() when (if) loaded from cache

        cbThreadPool m_Pool;

#endif // STANDALONE

		DECLARE_EVENT_TABLE()
};

#endif // PARSER_H


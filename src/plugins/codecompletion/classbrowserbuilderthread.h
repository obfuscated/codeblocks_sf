/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef CLASSBROWSERBUILDERTHREAD_H
#define CLASSBROWSERBUILDERTHREAD_H

#include <wx/thread.h>
#include <wx/treectrl.h>

#include "parser/token.h"
#include "parser/parser.h"

enum SpecialFolder
{
    sfToken         = 0x0001, // token node
    sfRoot          = 0x0002, // root node
    sfGFuncs        = 0x0004, // global funcs node
    sfGVars         = 0x0008, // global vars node
    sfPreproc       = 0x0010, // preprocessor symbols node
    sfTypedef       = 0x0020, // typedefs node
    sfBase          = 0x0040, // base classes node
    sfDerived       = 0x0080, // derived classes node
};

class CBTreeData : public wxTreeItemData
{
    public:
        CBTreeData(SpecialFolder sf = sfToken, Token* token = 0, int kindMask = 0xffffffff, int parentIdx = -1)
            : m_pToken(token),
            m_KindMask(kindMask),
            m_SpecialFolder(sf),
            m_TokenIndex(token ? token->GetSelf() : -1),
            m_TokenKind(token ? token->m_TokenKind : tkUndefined),
            m_TokenName(token ? token->m_Name : _T("")),
            m_ParentIndex(parentIdx),
            m_Ticket(token ? token->GetTicket() : 0)
        {
        }
        Token* m_pToken;
        int m_KindMask;
        SpecialFolder m_SpecialFolder;
        int m_TokenIndex;
        TokenKind m_TokenKind;
        wxString m_TokenName;
        int m_ParentIndex;
        unsigned long m_Ticket;
};

class CBExpandedItemData
{
	public:
		CBExpandedItemData(const CBTreeData* data, const int level):
			m_Data(*data),
			m_Level(level)
		{
		}
		int GetLevel() const { return m_Level; }
		const CBTreeData& GetData() { return m_Data; }
	private:
		CBTreeData m_Data;	// copy of tree item data
		int m_Level; 		// nesting level in the tree
};

class CBTreeDataCompare
{
	public:
		virtual ~CBTreeDataCompare() {}
		virtual int operator() (CBTreeData* lhs, CBTreeData* rhs) = 0;
};

class CBAlphabetCompare: public CBTreeDataCompare
{
	public:
		virtual int operator() (CBTreeData* lhs, CBTreeData* rhs);
};

class CBKindCompare: public CBTreeDataCompare
{
	public:
		CBKindCompare();
		virtual int operator() (CBTreeData* lhs, CBTreeData* rhs);
	private:
		std::vector<TokenKind> m_KindPrior;
		CBAlphabetCompare m_CompareEquals;
};

class CBScopeCompare: public CBTreeDataCompare
{
	public:
		virtual int operator() (CBTreeData* lhs, CBTreeData* rhs);
	private:
		CBKindCompare m_CompareEquals;
};

typedef std::deque<CBExpandedItemData> ExpandedItemVect;
typedef std::deque<CBTreeData> SelectedItemPath;

class ClassBrowserBuilderThread : public wxThread
{
    public:
        ClassBrowserBuilderThread(wxSemaphore& sem, ClassBrowserBuilderThread** threadVar);
        virtual ~ClassBrowserBuilderThread();

        void Init(Parser* parser,
                    wxTreeCtrl* treeTop,
                    wxTreeCtrl* treeBottom,
                    const wxString& active_filename,
                    void* user_data, // active project
                    const BrowserOptions& options,
                    TokensTree* pTokens,
                    bool build_tree);
        void AbortBuilding();
        void ExpandItem(wxTreeItemId item);
        void CollapseItem(wxTreeItemId item, bool useLock=true);
        void SelectItem(wxTreeItemId item);
    protected:
        virtual void* Entry();

        void BuildTree(bool useLock=true);
        void RemoveInvalidNodes(wxTreeCtrl* tree, wxTreeItemId parent);
        wxTreeItemId AddNodeIfNotThere(wxTreeCtrl* tree, wxTreeItemId parent, const wxString& name, int imgIndex = -1, CBTreeData* data = 0);
        bool AddChildrenOf(wxTreeCtrl* tree, wxTreeItemId parent, int parentTokenIdx, int tokenKindMask = 0xffff, bool sorted = true, int tokenScopeMask = 0);
        bool AddAncestorsOf(wxTreeCtrl* tree, wxTreeItemId parent, int tokenIdx);
        bool AddDescendantsOf(wxTreeCtrl* tree, wxTreeItemId parent, int tokenIdx, bool allowInheritance = true);
        bool AddNodes(wxTreeCtrl* tree, wxTreeItemId parent, const TokenIdxSet& tokens, int tokenKindMask = 0xffff, int tokenScopeMask = 0, bool allowGlobals = false, bool sorted = true);
        void AddMembersOf(wxTreeCtrl* tree, wxTreeItemId node);
        bool TokenMatchesFilter(Token* token);
        bool TokenContainsChildrenOfKind(Token* token, int kind);
        bool CreateSpecialFolders(wxTreeCtrl* tree, wxTreeItemId parent);
        void ExpandNamespaces(wxTreeItemId node);

        wxSemaphore& m_Semaphore;
        Parser* m_pParser;
        wxTreeCtrl* m_pTreeTop;
        wxTreeCtrl* m_pTreeBottom;
        wxString m_ActiveFilename;
        void* m_pUserData; // active project
        BrowserOptions m_Options;
        TokensTree* m_pTokens;
        ClassBrowserBuilderThread** m_ppThreadVar;

        // pair of current-file-filter
        TokenFilesSet m_CurrentFileSet;
        TokenIdxSet m_CurrentTokenSet;
        TokenIdxSet m_CurrentGlobalTokensSet;


        wxMutex m_BuildMutex;
        CBTreeDataCompare* m_pDataCompare;
    private:
        void SaveExpandedItems(wxTreeCtrl* tree, wxTreeItemId parent, int level);
        void ExpandSavedItems(wxTreeCtrl* tree, wxTreeItemId parent, int level);
        void SaveSelectedItem();
        void SelectSavedItem();

        ExpandedItemVect m_ExpandedVect;
        SelectedItemPath m_SelectedPath;
};

#endif // CLASSBROWSERBUILDERTHREAD_H

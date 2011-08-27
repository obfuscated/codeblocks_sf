/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef CLASSBROWSERBUILDERTHREAD_H
#define CLASSBROWSERBUILDERTHREAD_H

#include <wx/thread.h>
#include <wx/treectrl.h>

#include "nativeparser.h"
#include "parser/token.h"
#include "parser/parser.h"

enum SpecialFolder
{
    sfToken   = 0x0001, // token node
    sfRoot    = 0x0002, // root node
    sfGFuncs  = 0x0004, // global funcs node
    sfGVars   = 0x0008, // global vars node
    sfPreproc = 0x0010, // preprocessor symbols node
    sfTypedef = 0x0020, // typedefs node
    sfBase    = 0x0040, // base classes node
    sfDerived = 0x0080, // derived classes node
    sfMacro   = 0x0100  // global macro node
};

class CBTreeData : public wxTreeItemData
{
public:
    CBTreeData(SpecialFolder sf = sfToken, Token* token = 0, short int kindMask = 0xffff, int parentIdx = -1) :
        m_Token(token),
        m_KindMask(kindMask),
        m_SpecialFolder(sf),
        m_TokenIndex(token ? token->m_Index : -1),
        m_TokenKind(token ? token->m_TokenKind : tkUndefined),
        m_TokenName(token ? token->m_Name : _T("")),
        m_ParentIndex(parentIdx),
        m_Ticket(token ? token->GetTicket() : 0)
    {
    }
    Token*        m_Token;
    short int     m_KindMask;
    SpecialFolder m_SpecialFolder;
    int           m_TokenIndex;
    TokenKind     m_TokenKind;
    wxString      m_TokenName;
    int           m_ParentIndex;
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
    CBTreeData m_Data;  // copy of tree item data
    int        m_Level; // nesting level in the tree
};


typedef std::deque<CBExpandedItemData> ExpandedItemVect;
typedef std::deque<CBTreeData> SelectedItemPath;

class CBTreeCtrl : public wxTreeCtrl
{
public:
    CBTreeCtrl();
    CBTreeCtrl(wxWindow *parent, const wxWindowID id,const wxPoint& pos, const wxSize& size, long style);
    void SetCompareFunction(const BrowserSortType type);
    void RemoveDoubles(const wxTreeItemId& parent);

protected:
    static int CBAlphabetCompare (CBTreeData* lhs, CBTreeData* rhs);
    static int CBKindCompare (CBTreeData* lhs, CBTreeData* rhs);
    static int CBScopeCompare (CBTreeData* lhs, CBTreeData* rhs);
    static int CBNoCompare (CBTreeData* lhs, CBTreeData* rhs);
    int OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2);
    int (*Compare)(CBTreeData* lhs, CBTreeData* rhs);

    DECLARE_DYNAMIC_CLASS(CBTreeCtrl)
};

class ClassBrowserBuilderThread : public wxThread
{
public:
    ClassBrowserBuilderThread(wxSemaphore& sem, ClassBrowserBuilderThread** threadVar);
    virtual ~ClassBrowserBuilderThread();

    void Init(NativeParser* nativeParser, CBTreeCtrl* treeTop, CBTreeCtrl* treeBottom,
              const wxString& active_filename, void* user_data/*active project*/,
              const BrowserOptions& options, TokensTree* pTokensTree, bool build_tree);
    void ExpandItem(wxTreeItemId item);
#ifndef CC_NO_COLLAPSE_ITEM
    void CollapseItem(wxTreeItemId item);
#endif // CC_NO_COLLAPSE_ITEM
    void SelectItem(wxTreeItemId item);

protected:
    virtual void* Entry();

    void BuildTree();
    void RemoveInvalidNodes(CBTreeCtrl* tree, wxTreeItemId parent);
    wxTreeItemId AddNodeIfNotThere(CBTreeCtrl* tree, wxTreeItemId parent, const wxString& name, int imgIndex = -1, CBTreeData* data = 0);
    bool AddChildrenOf(CBTreeCtrl* tree, wxTreeItemId parent, int parentTokenIdx, short int tokenKindMask = 0xffff, int tokenScopeMask = 0);
    bool AddAncestorsOf(CBTreeCtrl* tree, wxTreeItemId parent, int tokenIdx);
    bool AddDescendantsOf(CBTreeCtrl* tree, wxTreeItemId parent, int tokenIdx, bool allowInheritance = true);
    bool AddNodes(CBTreeCtrl* tree, wxTreeItemId parent, const TokenIdxSet& tokens, short int tokenKindMask = 0xffff, int tokenScopeMask = 0, bool allowGlobals = false);
    void AddMembersOf(CBTreeCtrl* tree, wxTreeItemId node);
    bool TokenMatchesFilter(Token* token, bool locked = false);
    bool TokenContainsChildrenOfKind(Token* token, int kind);
    bool CreateSpecialFolders(CBTreeCtrl* tree, wxTreeItemId parent);
    void ExpandNamespaces(wxTreeItemId node);

private:
    void SaveExpandedItems(CBTreeCtrl* tree, wxTreeItemId parent, int level);
    void ExpandSavedItems(CBTreeCtrl* tree, wxTreeItemId parent, int level);
    void SaveSelectedItem();
    void SelectSavedItem();

protected:
    wxSemaphore&                m_Semaphore;
    NativeParser*               m_NativeParser;
    CBTreeCtrl*                 m_TreeTop;
    CBTreeCtrl*                 m_TreeBottom;
    wxString                    m_ActiveFilename;
    void*                       m_UserData; // active project
    BrowserOptions              m_Options;
    TokensTree*                 m_TokensTree;
    ClassBrowserBuilderThread** m_ThreadVar;

    // pair of current-file-filter
    TokenFilesSet               m_CurrentFileSet;
    TokenIdxSet                 m_CurrentTokenSet;
    TokenIdxSet                 m_CurrentGlobalTokensSet;
    wxMutex                     m_BuildMutex;

private:
    ExpandedItemVect m_ExpandedVect;
    SelectedItemPath m_SelectedPath;
    bool             m_initDone;
};

#endif // CLASSBROWSERBUILDERTHREAD_H

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
        CBTreeData(SpecialFolder sf = sfToken, Token* token = 0, int kindMask = 0xffff, int parentIdx = -1)
            : m_pToken(token),
            m_KindMask(kindMask),
            m_SpecialFolder(sf),
            m_TokenIndex(token ? token->GetSelf() : -1),
            m_TokenKind(token ? token->m_TokenKind : tkUndefined),
            m_TokenName(token ? token->m_Name : _T("")),
            m_ParentIndex(parentIdx)
        {
        }
        Token* m_pToken;
        int m_KindMask;
        SpecialFolder m_SpecialFolder;
        int m_TokenIndex;
        TokenKind m_TokenKind;
        wxString m_TokenName;
        int m_ParentIndex;
};

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
                    TokensTree* pTokens);
        void AbortBuilding();
        void ExpandItem(wxTreeItemId item);
        void CollapseItem(wxTreeItemId item);
        void SelectItem(wxTreeItemId item);
    protected:
        virtual void* Entry();

        void BuildTree();
        void RemoveInvalidNodes(wxTreeCtrl* tree, wxTreeItemId parent);
        wxTreeItemId AddNodeIfNotThere(wxTreeCtrl* tree, wxTreeItemId parent, const wxString& name, int imgIndex = -1, CBTreeData* data = 0, bool sorted = true);
        bool AddChildrenOf(wxTreeCtrl* tree, wxTreeItemId parent, int parentTokenIdx, int tokenKindMask = 0xffff);
        bool AddAncestorsOf(wxTreeCtrl* tree, wxTreeItemId parent, int tokenIdx);
        bool AddDescendantsOf(wxTreeCtrl* tree, wxTreeItemId parent, int tokenIdx, bool allowInheritance = true);
        bool AddNodes(wxTreeCtrl* tree, wxTreeItemId parent, TokenIdxSet::iterator start, TokenIdxSet::iterator end, int tokenKindMask = 0xffff, bool allowGlobals = false);
        void SelectNode(wxTreeItemId node);
        bool TokenMatchesFilter(Token* token);
        bool TokenContainsChildrenOfKind(Token* token, int kind);
        bool CreateSpecialFolders(wxTreeCtrl* tree, wxTreeItemId parent);

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
        TokenIdxSet m_CurrentFileSet;

        wxMutex m_BuildMutex;
    private:
};

#endif // CLASSBROWSERBUILDERTHREAD_H

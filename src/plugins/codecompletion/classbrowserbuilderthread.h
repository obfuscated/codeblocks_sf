#ifndef CLASSBROWSERBUILDERTHREAD_H
#define CLASSBROWSERBUILDERTHREAD_H

#include <wx/thread.h>
#include <wx/treectrl.h>

#include "parser/token.h"
#include "parser/parser.h"

enum SpecialFolder
{
    sfToken,
    sfRoot,
    sfGFuncs,
    sfGVars,
    sfPreproc,
    sfBase, // base classes
    sfDerived, // derived classes
};

class CBTreeData : public wxTreeItemData
{
    public:
        CBTreeData(SpecialFolder sf = sfToken, Token* token = 0, int kindMask = 0xffff, int parentIdx = -1)
            : m_pToken(token),
            m_KindMask(kindMask),
            m_SpecialFolder(sf),
            m_TokenIndex(token ? token->GetSelf() : -1),
            m_ParentIndex(parentIdx)
        {
        }
        Token* m_pToken;
        int m_KindMask;
        SpecialFolder m_SpecialFolder;
        int m_TokenIndex;
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
        void AddChildrenOf(wxTreeCtrl* tree, wxTreeItemId parent, int parentTokenIdx, int tokenKindMask = 0xffff);
        void AddAncestorsOf(wxTreeCtrl* tree, wxTreeItemId parent, int tokenIdx);
        void AddDescendantsOf(wxTreeCtrl* tree, wxTreeItemId parent, int tokenIdx);
        void AddNodes(wxTreeCtrl* tree, wxTreeItemId parent, TokenIdxSet::iterator start, TokenIdxSet::iterator end, int tokenKindMask = 0xffff, bool allowGlobals = false);
        void SelectNode(wxTreeItemId node);
        bool TokenMatchesFilter(Token* token);
        bool TokenContainsChildrenOfKind(Token* token, int kind);

        wxSemaphore& m_Semaphore;
        Parser* m_pParser;
        wxTreeCtrl* m_pTreeTop;
        wxTreeCtrl* m_pTreeBottom;
        wxString m_ActiveFilename;
        void* m_pUserData; // active project
        BrowserOptions m_Options;
        TokensTree* m_pTokens;
        ClassBrowserBuilderThread** m_ppThreadVar;

        wxMutex m_BuildMutex;
    private:
};

#endif // CLASSBROWSERBUILDERTHREAD_H

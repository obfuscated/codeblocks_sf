/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef CLASSBROWSERBUILDERTHREAD_H
#define CLASSBROWSERBUILDERTHREAD_H

#include <wx/thread.h>
#include <wx/treectrl.h>

#include "cctreectrl.h"
#include "nativeparser.h"
#include "parser/token.h"
#include "parser/parser.h"

class ClassBrowserBuilderThread : public wxThread
{
public:
    enum EThreadEvent { selectItemRequired, buildTreeStart, buildTreeEnd };

    ClassBrowserBuilderThread(wxEvtHandler* evtHandler, wxSemaphore& sem);
    virtual ~ClassBrowserBuilderThread();

    // Called from external:
    void Init(NativeParser* np, CCTreeCtrl* treeTop, CCTreeCtrl* treeBottom,
              const wxString& active_filename, void* user_data/*active project*/,
              const BrowserOptions& bo, TokenTree* tt,
              int idThreadEvent);

    // Called from external, BuildTree():
    void ExpandItem(wxTreeItemId item);
#ifndef CC_NO_COLLAPSE_ITEM
    // Called from external, BuildTree(), RemoveInvalidNodes():
    void CollapseItem(wxTreeItemId item);
#endif // CC_NO_COLLAPSE_ITEM
    // Called from external and SelectItemRequired():
    void SelectItem(wxTreeItemId item);
    // Called from external:
    void SelectItemRequired();

    void RequestTermination(bool terminate = true) { m_TerminationRequested = terminate; }

protected:
    virtual void* Entry();

    // Called from Entry():
    void BuildTree();

    // Called from BuildTree():
    void RemoveInvalidNodes(CCTreeCtrl* tree, wxTreeItemId parent); // recursive
    void ExpandNamespaces(wxTreeItemId node, TokenKind tokenKind, int level);            // recursive

    // Called from ExpandItem():
    bool CreateSpecialFolders(CCTreeCtrl* tree, wxTreeItemId parent);
    // Called from CreateSpecialFolders():
    wxTreeItemId AddNodeIfNotThere(CCTreeCtrl* tree, wxTreeItemId parent,
                                   const wxString& name, int imgIndex = -1, CCTreeCtrlData* data = 0);

    // Called from ExpandItem():
    bool AddChildrenOf(CCTreeCtrl* tree, wxTreeItemId parent, int parentTokenIdx,
                       short int tokenKindMask = 0xffff, int tokenScopeMask = 0);
    bool AddAncestorsOf(CCTreeCtrl* tree, wxTreeItemId parent, int tokenIdx);
    bool AddDescendantsOf(CCTreeCtrl* tree, wxTreeItemId parent, int tokenIdx, bool allowInheritance = true);
    // Called from ExpandItem(), SelectItem():
    void AddMembersOf(CCTreeCtrl* tree, wxTreeItemId node);

private:
    // Called from AddChildrenOf(), AddAncestorsOf(), AddDescendantsOf():
    bool AddNodes(CCTreeCtrl* tree, wxTreeItemId parent, const TokenIdxSet* tokens,
                  short int tokenKindMask = 0xffff, int tokenScopeMask = 0, bool allowGlobals = false);

    // Called from RemoveInvalidNodes(), AddNodes(), CreateSpecialFolder():
    bool TokenMatchesFilter(const Token* token, bool locked = false);
    // Called from AddNodes():
    bool TokenContainsChildrenOfKind(const Token* token, int kind);

    // Called from BuildTree():
    void SaveExpandedItems(CCTreeCtrl* tree, wxTreeItemId parent, int level);
    void ExpandSavedItems(CCTreeCtrl* tree, wxTreeItemId parent, int level);
    void SaveSelectedItem();
    void SelectSavedItem();

protected:
    wxEvtHandler*    m_Parent;
    wxSemaphore&     m_ClassBrowserSemaphore;
    wxMutex          m_ClassBrowserBuilderThreadMutex;
    NativeParser*    m_NativeParser;
    CCTreeCtrl*      m_CCTreeCtrlTop;
    CCTreeCtrl*      m_CCTreeCtrlBottom;
    wxString         m_ActiveFilename;
    void*            m_UserData; // active project
    BrowserOptions   m_BrowserOptions;
    TokenTree*       m_TokenTree;

    // pair of current-file-filter
    TokenFileSet     m_CurrentFileSet;
    TokenIdxSet      m_CurrentTokenSet;
    TokenIdxSet      m_CurrentGlobalTokensSet;

private:
    ExpandedItemVect m_ExpandedVect;
    SelectedItemPath m_SelectedPath;
    bool             m_InitDone;
    bool             m_TerminationRequested;
    int              m_idThreadEvent;
    wxTreeItemId     m_SelectItemRequired;
};

#endif // CLASSBROWSERBUILDERTHREAD_H

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
    ClassBrowserBuilderThread(wxSemaphore& sem);
    virtual ~ClassBrowserBuilderThread();

    void Init(NativeParser* nativeParser, CCTreeCtrl* treeTop, CCTreeCtrl* treeBottom,
              const wxString& active_filename, void* user_data/*active project*/,
              const BrowserOptions& options, TokensTree* pTokensTree, bool build_tree, int idCBMakeSelectItem);

    void ExpandItem(wxTreeItemId item);
#ifndef CC_NO_COLLAPSE_ITEM
    void CollapseItem(wxTreeItemId item);
#endif // CC_NO_COLLAPSE_ITEM
    void SelectItem(wxTreeItemId item);
    void SelectItemRequired();

protected:
    virtual void* Entry();

    void BuildTree();
    void RemoveInvalidNodes(CCTreeCtrl* tree, wxTreeItemId parent);
    wxTreeItemId AddNodeIfNotThere(CCTreeCtrl* tree, wxTreeItemId parent, const wxString& name, int imgIndex = -1, CCTreeCtrlData* data = 0);
    bool AddChildrenOf(CCTreeCtrl* tree, wxTreeItemId parent, int parentTokenIdx, short int tokenKindMask = 0xffff, int tokenScopeMask = 0);
    bool AddAncestorsOf(CCTreeCtrl* tree, wxTreeItemId parent, int tokenIdx);
    bool AddDescendantsOf(CCTreeCtrl* tree, wxTreeItemId parent, int tokenIdx, bool allowInheritance = true);
    bool AddNodes(CCTreeCtrl* tree, wxTreeItemId parent, const TokenIdxSet& tokens, short int tokenKindMask = 0xffff, int tokenScopeMask = 0, bool allowGlobals = false);
    void AddMembersOf(CCTreeCtrl* tree, wxTreeItemId node);
    bool TokenMatchesFilter(Token* token, bool locked = false);
    bool TokenContainsChildrenOfKind(Token* token, int kind);
    bool CreateSpecialFolders(CCTreeCtrl* tree, wxTreeItemId parent);
    void ExpandNamespaces(wxTreeItemId node);

private:
    void SaveExpandedItems(CCTreeCtrl* tree, wxTreeItemId parent, int level);
    void ExpandSavedItems(CCTreeCtrl* tree, wxTreeItemId parent, int level);
    void SaveSelectedItem();
    void SelectSavedItem();

protected:
    wxMutex          m_ClassBrowserBuilderThreadMutex;
    wxSemaphore&     m_ClassBrowserSemaphore;
    NativeParser*    m_NativeParser;
    CCTreeCtrl*      m_CCTreeCtrlTop;
    CCTreeCtrl*      m_CCTreeCtrlBottom;
    wxString         m_ActiveFilename;
    void*            m_UserData; // active project
    BrowserOptions   m_Options;
    TokensTree*      m_TokensTree;

    // pair of current-file-filter
    TokenFilesSet    m_CurrentFileSet;
    TokenIdxSet      m_CurrentTokenSet;
    TokenIdxSet      m_CurrentGlobalTokensSet;

private:
    ExpandedItemVect m_ExpandedVect;
    SelectedItemPath m_SelectedPath;
    bool             m_InitDone;

    wxTreeItemId     m_SelectItemRequired;
    int              m_idCBMakeSelectItem;
};

#endif // CLASSBROWSERBUILDERTHREAD_H

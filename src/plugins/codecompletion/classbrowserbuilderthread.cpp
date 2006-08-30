#include <sdk.h>
#include "classbrowserbuilderthread.h"
#include <globals.h>
#include <manager.h>
#include <projectmanager.h>
#include <cbproject.h>

#include <wx/utils.h>

ClassBrowserBuilderThread::ClassBrowserBuilderThread(wxSemaphore& sem, ClassBrowserBuilderThread** threadVar)
    : wxThread(wxTHREAD_DETACHED),
    m_Semaphore(sem),
    m_pParser(0),
    m_pTreeTop(0),
    m_pTreeBottom(0),
    m_pUserData(0),
    m_pTokens(0),
    m_ppThreadVar(threadVar)
{
    //ctor
}

ClassBrowserBuilderThread::~ClassBrowserBuilderThread()
{
    //dtor
}

void ClassBrowserBuilderThread::Init(Parser* parser,
                                    wxTreeCtrl* treeTop,
                                    wxTreeCtrl* treeBottom,
                                    const wxString& active_filename,
                                    void* user_data, // active project
                                    const BrowserOptions& options,
                                    TokensTree* pTokens)
{
    wxMutexLocker lock(m_BuildMutex);
    m_pParser = parser;
    m_pTreeTop = treeTop;
    m_pTreeBottom = treeBottom;
    m_ActiveFilename = active_filename;
    m_pUserData = user_data;
    m_Options = options;
    m_pTokens = pTokens;
}

void* ClassBrowserBuilderThread::Entry()
{
    while (!TestDestroy())
    {
        // wait until the classbrowser signals
        m_Semaphore.Wait();
//        DBGLOG(_T(" - - - - - -"));

        if (TestDestroy())
            break;

        BuildTree();
    }

    if (m_ppThreadVar)
        *m_ppThreadVar = 0;
    return 0;
}

void ClassBrowserBuilderThread::BuildTree()
{
//    wxMutexLocker lock(m_BuildMutex);

    m_pTreeTop->SetImageList(m_pParser->GetImageList());
    m_pTreeBottom->SetImageList(m_pParser->GetImageList());

    wxTreeItemId root = m_pTreeTop->GetRootItem();
    if (!root.IsOk())
    {
        root = m_pTreeTop->AddRoot(_("Symbols"), PARSER_IMG_SYMBOLS_FOLDER, PARSER_IMG_SYMBOLS_FOLDER, new CBTreeData(sfRoot));
        m_pTreeTop->SetItemHasChildren(root);
    }

    RemoveInvalidNodes(m_pTreeTop, root);
    RemoveInvalidNodes(m_pTreeBottom, m_pTreeBottom->GetRootItem());

    if (TestDestroy())
        return;

    // the tree is completely dynamic: it is populated when a node expands/collapses.
    // so, by expanding the root node, we already instruct it to fill the top level :)
    //
    // this technique makes it really fast to draw (we only draw what's expanded) and
    // has very minimum memory overhead since it contains as few items as possible.
    // plus, it doesn't flicker because we 're not emptying it and re-creating it each time ;)
    m_pTreeTop->Expand(root);

    SelectNode(m_pTreeTop->GetSelection()); // refresh selection
}

void ClassBrowserBuilderThread::RemoveInvalidNodes(wxTreeCtrl* tree, wxTreeItemId parent)
{
    if (TestDestroy())
        return;

    // recursively enters all existing nodes and deletes the node if the token it references
    // is invalid (i.e. m_pTokens->at() != token_in_data)

    // we 'll loop backwards so we can delete nodes without problems
    wxTreeItemId existing = tree->GetLastChild(parent);
    while (existing)
    {
        CBTreeData* data = (CBTreeData*)tree->GetItemData(existing);
        if (data && data->m_pToken)
        {
            if (m_pTokens->at(data->m_TokenIndex) != data->m_pToken || !TokenMatchesFilter(data->m_pToken))
            {
//                DBGLOG(_T("Item %s is invalid"), tree->GetItemText(existing).c_str());
                wxTreeItemId next = tree->GetPrevSibling(existing);
                tree->Delete(existing);
                existing = next;
                continue;
            }
        }
        // recurse
        if (tree->ItemHasChildren(existing))
            RemoveInvalidNodes(tree, existing);

        existing = tree->GetPrevSibling(existing);
    }

//    if (parent != tree->GetRootItem() && tree->GetChildrenCount(parent) == 0)
//        tree->Delete(parent);
}

wxTreeItemId ClassBrowserBuilderThread::AddNodeIfNotThere(wxTreeCtrl* tree, wxTreeItemId parent, const wxString& name, int imgIndex, wxTreeItemData* data)
{
#if (wxMAJOR_VERSION == 2) && (wxMINOR_VERSION < 5)
    long int cookie = 0;
#else
    wxTreeItemIdValue cookie; //2.6.0
#endif
    wxTreeItemId existing = tree->GetFirstChild(parent, cookie);
    while (existing)
    {
        if (tree->GetItemText(existing) == name)
        {
            // update the existing node's image indices and user-data.
            // it's not possible to have the same token name more than once
            // under the same namespace anyway. if we do, there's a bug in the parser :(
            tree->SetItemImage(existing, imgIndex, wxTreeItemIcon_Normal);
            tree->SetItemImage(existing, imgIndex, wxTreeItemIcon_Selected);
            tree->SetItemData(existing, data);

            return existing;
        }
        existing = tree->GetNextChild(parent, cookie);
    }
    existing = tree->AppendItem(parent, name, imgIndex, imgIndex, data);
    return existing;
}

void ClassBrowserBuilderThread::AddChildrenOf(wxTreeCtrl* tree, wxTreeItemId parent, int parentTokenIdx, int tokenKindMask)
{
    if (TestDestroy())
        return;

    Token* parentToken = 0;
	TokenIdxSet::iterator it;
	TokenIdxSet::iterator it_end;

	if (parentTokenIdx == -1)
	{
        it = m_pTokens->m_GlobalNameSpace.begin();
        it_end = m_pTokens->m_GlobalNameSpace.end();
	}
    else
    {
        parentToken = m_pTokens->at(parentTokenIdx);
        if (!parentToken)
        {
            DBGLOG(_T("Token not found?!?"));
            return;
        }
        it = parentToken->m_Children.begin();
        it_end = parentToken->m_Children.end();
    }

    AddNodes(tree, parent, it, it_end, tokenKindMask);
}

void ClassBrowserBuilderThread::AddAncestorsOf(wxTreeCtrl* tree, wxTreeItemId parent, int tokenIdx)
{
    if (TestDestroy())
        return;

    Token* token = m_pTokens->at(tokenIdx);
    if (!token)
        return;

    AddNodes(tree, parent, token->m_DirectAncestors.begin(), token->m_DirectAncestors.end(), tkClass, true);
}

void ClassBrowserBuilderThread::AddDescendantsOf(wxTreeCtrl* tree, wxTreeItemId parent, int tokenIdx)
{
    if (TestDestroy())
        return;

    Token* token = m_pTokens->at(tokenIdx);
    if (!token)
        return;

    AddNodes(tree, parent, token->m_Descendants.begin(), token->m_Descendants.end(), tkClass, true);
}

void ClassBrowserBuilderThread::AddNodes(wxTreeCtrl* tree, wxTreeItemId parent, TokenIdxSet::iterator start, TokenIdxSet::iterator end, int tokenKindMask, bool allowGlobals)
{
    int count = 0;
	for ( ; start != end; ++start)
    {
        Token* token = m_pTokens->at(*start);
        if (token &&
            !token->m_IsTemp &&
            (token->m_TokenKind & tokenKindMask) &&
            (allowGlobals || token->m_IsLocal) &&
            TokenMatchesFilter(token))
        {
            ++count;
            int img = m_pParser->GetTokenKindImage(token);

            wxString str = token->m_Name;
            if (token->m_TokenKind == tkFunction || token->m_TokenKind == tkConstructor || token->m_TokenKind == tkDestructor)
                str << token->m_Args;
            if (!token->m_ActualType.IsEmpty())
                 str = str + _T(" : ") + token->m_ActualType;

            if (tree == m_pTreeTop)
            {
                wxTreeItemId child = AddNodeIfNotThere(tree, parent, str, img, new CBTreeData(sfToken, token));
                // mark as expanding if it is a container
                if (token->m_TokenKind & (tkClass | tkNamespace | tkEnumerator))
                    tree->SetItemHasChildren(child);
            }
            else // the bottom tree needs no checks
                tree->AppendItem(parent, str, img, img, new CBTreeData(sfToken, token));
        }
    }
//    DBGLOG(_T("Sorting..."));
    tree->SortChildren(parent);
//    DBGLOG(_T("Added %d nodes"), count);
}

bool ClassBrowserBuilderThread::TokenMatchesFilter(Token* token)
{
    if (m_Options.displayFilter == bdfWorkspace)
        return true;

    if (m_Options.displayFilter == bdfFile && !m_ActiveFilename.IsEmpty())
    {
        // TODO: fixme, it's too much because this function is called from inside a loop...
        if (token->m_File > 0)
        {
            const wxString& str = m_pTokens->m_FilenamesMap.GetString(token->m_File);
            if (!str.IsEmpty() && str.StartsWith(m_ActiveFilename))
                return true;
        }
        if (token->m_ImplFile > 0)
        {
            const wxString& str = m_pTokens->m_FilenamesMap.GetString(token->m_ImplFile);
            if (!str.IsEmpty() && str.StartsWith(m_ActiveFilename))
                return true;
        }

        // reached here; tough case:
        // we got to check all children of this token (recursively)
        // to see if any of them match the filter...
        for (TokenIdxSet::iterator it = token->m_Children.begin(); it != token->m_Children.end(); ++it)
        {
            if (TokenMatchesFilter(m_pTokens->at(*it)))
                return true;
        }
    }
    else if (m_Options.displayFilter == bdfProject && m_pUserData)
    {
        return token->m_pUserData == m_pUserData;
    }

    return false;
}

bool ClassBrowserBuilderThread::TokenHasChildren(Token* token)
{
}

void ClassBrowserBuilderThread::SelectNode(wxTreeItemId node)
{
    if (TestDestroy())
        return;

    wxMutexLocker lock(m_BuildMutex);
    wxBusyCursor busy;

    m_pTreeBottom->Freeze();
    wxTreeItemId root = m_pTreeBottom->GetRootItem();
    if (!root)
        root = m_pTreeBottom->AddRoot(_T("Members")); // not visible, so don't translate
    else
        m_pTreeBottom->DeleteChildren(root);
    CBTreeData* data = (CBTreeData*)m_pTreeTop->GetItemData(node);
    if (data)
    {
        switch (data->m_SpecialFolder)
        {
            case sfGFuncs: AddChildrenOf(m_pTreeBottom, root, -1, tkFunction); break;
            case sfGVars: AddChildrenOf(m_pTreeBottom, root, -1, tkVariable); break;
            case sfPreproc: AddChildrenOf(m_pTreeBottom, root, -1, tkPreprocessor); break;
            case sfToken:
            {
                AddChildrenOf(m_pTreeBottom, root, data->m_pToken->GetSelf());
                break;
            }
            default: break;
        }
    }
    m_pTreeBottom->Thaw();
}

void ClassBrowserBuilderThread::ExpandItem(wxTreeItemId item)
{
    if (TestDestroy())
        return;

    wxMutexLocker lock(m_BuildMutex);
    CBTreeData* data = (CBTreeData*)m_pTreeTop->GetItemData(item);
    if (data)
    {
        switch (data->m_SpecialFolder)
        {
            case sfRoot:
            {
                wxTreeItemId gfuncs = AddNodeIfNotThere(m_pTreeTop, item, _("Global functions"), PARSER_IMG_OTHERS_FOLDER, new CBTreeData(sfGFuncs, 0, tkFunction, -1));
                wxTreeItemId gvars = AddNodeIfNotThere(m_pTreeTop, item, _("Global variables"), PARSER_IMG_OTHERS_FOLDER, new CBTreeData(sfGVars, 0, tkVariable, -1));
                wxTreeItemId gpreproc = AddNodeIfNotThere(m_pTreeTop, item, _("Preprocessor symbols"), PARSER_IMG_PREPROC_FOLDER, new CBTreeData(sfPreproc, 0, tkPreprocessor, -1));

                AddChildrenOf(m_pTreeTop, item, -1, ~(tkFunction | tkVariable | tkPreprocessor));
                break;
            }
            case sfBase: AddAncestorsOf(m_pTreeTop, item, data->m_pToken->GetSelf()); break;
            case sfDerived: AddDescendantsOf(m_pTreeTop, item, data->m_pToken->GetSelf()); break;
            case sfToken:
            {
                int kind = 0;
                switch (data->m_pToken->m_TokenKind)
                {
                    case tkClass:
                    {
                        // add base and derived classes folders
                        if (m_Options.showInheritance)
                        {
                            wxTreeItemId base = AddNodeIfNotThere(m_pTreeTop, item, _("Base classes"), PARSER_IMG_CLASS_FOLDER, new CBTreeData(sfBase, data->m_pToken, tkClass, data->m_pToken->GetSelf()));
                            if (!data->m_pToken->m_DirectAncestors.empty())
                                m_pTreeTop->SetItemHasChildren(base);
                            wxTreeItemId derived = AddNodeIfNotThere(m_pTreeTop, item, _("Derived classes"), PARSER_IMG_CLASS_FOLDER, new CBTreeData(sfDerived, data->m_pToken, tkClass, data->m_pToken->GetSelf()));
                            if (!data->m_pToken->m_Descendants.empty())
                                m_pTreeTop->SetItemHasChildren(derived);
                        }
                        kind = tkClass | tkEnum;
                        break;
                    }
                    case tkNamespace:
                        kind = tkNamespace | tkClass | tkEnum;
                        break;
                    default:
                        break;
                }
                if (kind != 0)
                    AddChildrenOf(m_pTreeTop, item, data->m_pToken->GetSelf(), kind);
                m_pTreeTop->SetItemHasChildren(item, m_pTreeTop->GetChildrenCount(item));
                break;
            }
            default: break;
        }
    }
//    DBGLOG(_("E: %d items"), m_pTreeTop->GetCount());
}

void ClassBrowserBuilderThread::CollapseItem(wxTreeItemId item)
{
    if (TestDestroy())
        return;

    wxMutexLocker lock(m_BuildMutex);
    m_pTreeTop->CollapseAndReset(item);
    m_pTreeTop->SetItemHasChildren(item);
//    DBGLOG(_("C: %d items"), m_pTreeTop->GetCount());
}

void ClassBrowserBuilderThread::SelectItem(wxTreeItemId item)
{
    if (TestDestroy())
        return;

    wxMutexLocker lock(m_BuildMutex);
    SelectNode(item);
//    DBGLOG(_T("Select ") + m_pTreeTop->GetItemText(item));
}

/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#include "classbrowserbuilderthread.h"
#include <globals.h>
#include <manager.h>
#include <projectmanager.h>
#include <cbproject.h>

#include <wx/settings.h>
#include <wx/utils.h>

#include <algorithm>

namespace compatibility { typedef TernaryCondTypedef<wxMinimumVersion<2,5>::eval, wxTreeItemIdValue, long int>::eval tree_cookie_t; };

// CBAlphabetCompare
int CBAlphabetCompare::operator() (CBTreeData* lhs, CBTreeData* rhs)
{
    if (!lhs->m_pToken || !rhs->m_pToken) return 1;
    //if () return -1;
    return wxStricmp(lhs->m_pToken->DisplayName(), rhs->m_pToken->DisplayName());
}

// CBKindCompare
CBKindCompare::CBKindCompare()
{
    TokenKind prior[] =
    {
        tkNamespace,
        tkClass,
        tkEnum,
        tkTypedef,
        tkConstructor,
        tkDestructor,
        tkFunction,
        tkVariable
    };
    m_KindPrior.assign(prior, prior + sizeof(prior)/sizeof(prior[0]));
}

int CBKindCompare::operator() (CBTreeData* lhs, CBTreeData* rhs)
{
    if (lhs->m_TokenKind == rhs->m_TokenKind)
        return m_CompareEquals(lhs, rhs);

    size_t l_prior, r_prior;
    l_prior = r_prior = m_KindPrior.size();
    for (size_t i = 0; i < m_KindPrior.size(); ++i)
    {
        if (m_KindPrior[i] == lhs->m_TokenKind) l_prior = i;
        if (m_KindPrior[i] == rhs->m_TokenKind) r_prior = i;
    }

    return (l_prior == r_prior) ? m_CompareEquals(lhs, rhs) : l_prior - r_prior;
}

// CBScopeCompare
int CBScopeCompare::operator() (CBTreeData* lhs, CBTreeData* rhs)
{
    if (lhs->m_SpecialFolder != sfToken || rhs->m_SpecialFolder != sfToken)
        return 1;
    if (lhs->m_pToken->m_Scope == rhs->m_pToken->m_Scope)
        return m_CompareEquals(lhs, rhs);

    return rhs->m_pToken->m_Scope - lhs->m_pToken->m_Scope;
}

// ClassBrowserBuilderThread
ClassBrowserBuilderThread::ClassBrowserBuilderThread(wxSemaphore& sem, ClassBrowserBuilderThread** threadVar)
    : wxThread(wxTHREAD_JOINABLE),
    m_Semaphore(sem),
    m_pParser(0),
    m_pTreeTop(0),
    m_pTreeBottom(0),
    m_pUserData(0),
    m_Options(),
    m_pTokens(0),
    m_ppThreadVar(threadVar),
    m_pDataCompare(0)
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
                                    TokensTree* pTokens,
                                    bool build_tree)
{
    wxMutexLocker lock(m_BuildMutex);
    m_pParser = parser;
    m_pTreeTop = treeTop;
    m_pTreeBottom = treeBottom;
    m_ActiveFilename = active_filename;
    m_pUserData = user_data;
    m_Options = options;
    m_pTokens = pTokens;

    m_CurrentFileSet.clear();
    m_CurrentTokenSet.clear();

    TokensTree* tree = m_pParser->GetTokens();
    // fill filter set for current-file-filter
    if (m_Options.displayFilter == bdfFile && !m_ActiveFilename.IsEmpty())
    {
        // m_ActiveFilename is the full filename up to the extension dot. No extension though.
        // get all filenames' indices matching our mask
        tree->m_FilenamesMap.FindMatches(m_ActiveFilename, m_CurrentFileSet, true, true);
    }

    if(m_Options.displayFilter == bdfProject && (user_data != 0))
    {
        cbProject* prj = (cbProject*)user_data;
        for(int i = 0; i < prj->GetFilesCount(); i++)
        {
            ProjectFile* curfile = prj->GetFile(i);
            if(!curfile)
                continue;
            wxString filename = curfile->file.GetFullPath();
            size_t fileIdx = tree->m_FilenamesMap.GetItemNo(filename);
            if(fileIdx)
            {
                m_CurrentFileSet.insert(fileIdx);
            }
        }
    }

    if (!m_CurrentFileSet.empty())
    {
        m_CurrentTokenSet.clear();
        m_CurrentGlobalTokensSet.clear();
        for(TokenFilesSet::iterator it = m_CurrentFileSet.begin();it != m_CurrentFileSet.end(); it++)
        {
            TokenIdxSet* curset = &(tree->m_FilesMap[*it]);
            for(TokenIdxSet::iterator it2 = curset->begin(); it2 != curset->end(); it2++)
            {
                Token* curtoken = tree->at(*it2);
                if(curtoken)
                {
                    m_CurrentTokenSet.insert(*it2);
                    if(curtoken->m_ParentIndex == -1)
                        m_CurrentGlobalTokensSet.insert(*it2);
                }
            }
        }
    }

    if (build_tree)
    {
        BuildTree(false);
    }
}

void* ClassBrowserBuilderThread::Entry()
{
    while (!TestDestroy() && !Manager::IsAppShuttingDown())
    {
        // wait until the classbrowser signals
        m_Semaphore.Wait();
//        Manager::Get()->GetLogManager()->DebugLog(F(_T(" - - - - - -")));

        if (TestDestroy() || Manager::IsAppShuttingDown())
            break;

        if(platform::gtk)
        {
            // this code (PART 1/2) seems to be good on linux
            // because of it the libcairo crash on dualcore processors
            // is gone, but on windows it has very bad influence,
            // henceforth the ifdef guard
            // the questions remains if it is the correct solution
            if(!::wxIsMainThread())
                ::wxMutexGuiEnter();
        }

        BuildTree();

        if(platform::gtk)
        {
          // this code (PART 2/2) seems to be good on linux
          // because of it the libcairo crash on dualcore processors
          // is gone, but on windows it has very bad influence,
          // henceforth the ifdef guard
          // the questions remains if it is the correct solution
            if(!::wxIsMainThread())
                ::wxMutexGuiLeave();
        }
    }

    m_pParser = 0;
    m_pTreeTop = 0;
    m_pTreeBottom = 0;

    if (m_pDataCompare)
    {
        delete m_pDataCompare;
        m_pDataCompare = 0;
    }

    /*if (m_ppThreadVar)
        *m_ppThreadVar = 0;*/
    return 0;
}

void ClassBrowserBuilderThread::ExpandNamespaces(wxTreeItemId node)
{
    if (TestDestroy() || Manager::IsAppShuttingDown())
        return;

    if (!m_Options.expandNS || !node.IsOk())
        return;

    wxTreeItemIdValue enumerationCookie;
    wxTreeItemId existing = m_pTreeTop->GetFirstChild(node, enumerationCookie);
    while (existing.IsOk())
    {
        CBTreeData* data = (CBTreeData*)m_pTreeTop->GetItemData(existing);
        if (data && data->m_pToken && data->m_pToken->m_TokenKind == tkNamespace)
        {
//            Manager::Get()->GetLogManager()->DebugLog(F(_T("Auto-expanding: ") + data->m_pToken->m_Name));
            m_pTreeTop->Expand(existing);
            ExpandNamespaces(existing); // recurse
        }

        existing = m_pTreeTop->GetNextSibling(existing);
    }
}

void ClassBrowserBuilderThread::BuildTree(bool useLock)
{
    if (TestDestroy() || Manager::IsAppShuttingDown())
        return;
//    wxMutexLocker lock(m_BuildMutex);

#ifdef buildtree_measuring
wxStopWatch sw;
long delay = 0;
#endif
    m_pTreeTop->SetImageList(m_pParser->GetImageList());
    m_pTreeBottom->SetImageList(m_pParser->GetImageList());

    wxTreeItemId root = m_pTreeTop->GetRootItem();
    if (!root.IsOk())
    {
        root = m_pTreeTop->AddRoot(_("Symbols"), PARSER_IMG_SYMBOLS_FOLDER, PARSER_IMG_SYMBOLS_FOLDER, new CBTreeData(sfRoot));
        m_pTreeTop->SetItemHasChildren(root);
    }

    if (m_pDataCompare)
    {
        delete m_pDataCompare;
        m_pDataCompare = 0;
    }

    switch (m_Options.sortType)
    {
        case bstAlphabet:
            m_pDataCompare = new CBAlphabetCompare;
            break;
        case bstKind:
            m_pDataCompare = new CBKindCompare;
            break;
        case bstScope:
        default:
            m_pDataCompare = new CBScopeCompare;
            break;
    }

    m_ExpandedVect.clear();
    SaveExpandedItems(m_pTreeTop, root, 0);
#ifdef buildtree_measuring
delay = sw.Time()-delay;
Manager::Get()->GetLogManager()->DebugLog(F(_T("Saving expanded items took : %ld ms"),delay));
#endif

    SaveSelectedItem();
#ifdef buildtree_measuring
delay = sw.Time()-delay;
Manager::Get()->GetLogManager()->DebugLog(F(_T("Saving selected items took : %ld ms"),delay));
#endif

    if (m_Options.treeMembers)
    {
        m_pTreeBottom->Hide();
        m_pTreeBottom->Freeze();
    }
    m_pTreeTop->Hide();
    m_pTreeTop->Freeze();

    RemoveInvalidNodes(m_pTreeTop, root);
#ifdef buildtree_measuring
delay = sw.Time()-delay;
Manager::Get()->GetLogManager()->DebugLog(F(_T("Removing invalid nodes (top tree) took : %ld ms"),delay));
#endif
    if (m_Options.treeMembers)
    {
        RemoveInvalidNodes(m_pTreeBottom, m_pTreeBottom->GetRootItem());
#ifdef buildtree_measuring
delay = sw.Time()-delay;
Manager::Get()->GetLogManager()->DebugLog(F(_T("Removing invalid nodes (bottom tree) took : %ld ms"),delay));
#endif
    }

    if (TestDestroy() || Manager::IsAppShuttingDown())
        return;

    // the tree is completely dynamic: it is populated when a node expands/collapses.
    // so, by expanding the root node, we already instruct it to fill the top level :)
    //
    // this technique makes it really fast to draw (we only draw what's expanded) and
    // has very minimum memory overhead since it contains as few items as possible.
    // plus, it doesn't flicker because we 're not emptying it and re-creating it each time ;)

    CollapseItem(root, useLock);
#ifdef buildtree_measuring
delay = sw.Time()-delay;
Manager::Get()->GetLogManager()->DebugLog(F(_T("Collapsing root item took : %ld ms"),delay));
#endif
    // Bottleneck: Takes ~4 secs on C::B workspace:
    m_pTreeTop->Expand(root);
#ifdef buildtree_measuring
delay = sw.Time()-delay;
Manager::Get()->GetLogManager()->DebugLog(F(_T("Expanding root item took : %ld ms"),delay));
#endif

    // seems like the "expand" event comes too late in wxGTK, so make it happen now
    if(platform::gtk)
        ExpandItem(root);

    ExpandSavedItems(m_pTreeTop, root, 0);
#ifdef buildtree_measuring
delay = sw.Time()-delay;
Manager::Get()->GetLogManager()->DebugLog(F(_T("Expanding saved items took : %ld ms"),delay));
#endif
    // Bottleneck: Takes ~4 secs on C::B workspace:
    SelectSavedItem();
#ifdef buildtree_measuring
delay = sw.Time()-delay;
Manager::Get()->GetLogManager()->DebugLog(F(_T("Selecting saved item took : %ld ms"),delay));
#endif

    if (m_Options.treeMembers)
    {
        m_pTreeBottom->Thaw();
        m_pTreeBottom->Show();
#ifdef buildtree_measuring
delay = sw.Time()-delay;
Manager::Get()->GetLogManager()->DebugLog(F(_T("Showing bottom tree took : %ld ms"),delay));
#endif
    }

    ExpandNamespaces(m_pTreeTop->GetRootItem());
#ifdef buildtree_measuring
delay = sw.Time()-delay;
Manager::Get()->GetLogManager()->DebugLog(F(_T("Expanding namespaces took : %ld ms"),delay));
#endif

    m_pTreeTop->Thaw();
    // Bottleneck: Takes ~4 secs on C::B workspace:
    m_pTreeTop->Show();
#ifdef buildtree_measuring
delay = sw.Time()-delay;
Manager::Get()->GetLogManager()->DebugLog(F(_T("Thawing/Showing top tree took : %ld ms"),delay));
#endif
}

#if 1
void ClassBrowserBuilderThread::RemoveInvalidNodes(wxTreeCtrl* tree, wxTreeItemId parent)
{
    if (TestDestroy() || Manager::IsAppShuttingDown() || (!(parent.IsOk())))
        return;

    // recursively enters all existing nodes and deletes the node if the token it references
    // is invalid (i.e. m_pTokens->at() != token_in_data)

    // we 'll loop backwards so we can delete nodes without problems
    wxTreeItemId existing = tree->GetLastChild(parent);
    while (parent.IsOk() && existing.IsOk())
    {
        bool removeCurrent = false;
        bool hasChildren = tree->ItemHasChildren(existing);
        CBTreeData* data = (CBTreeData*)(tree->GetItemData(existing));

        if (tree == m_pTreeBottom)
        {
            removeCurrent = true;
        }
        else if (data && data->m_pToken)
        {

            if (m_pTokens->at(data->m_TokenIndex) != data->m_pToken ||
                (data->m_Ticket && data->m_Ticket != data->m_pToken->GetTicket()) ||
                !TokenMatchesFilter(data->m_pToken))
            {
                removeCurrent = true;
            }
        }

        if(removeCurrent)
        {
            if(hasChildren)
                tree->DeleteChildren(existing);

            wxTreeItemId next = tree->GetPrevSibling(existing);
            if(!next.IsOk() && parent.IsOk() && tree == m_pTreeTop && tree->GetChildrenCount(parent, false) == 1 )
            {
                CollapseItem(parent);
                // tree->SetItemHasChildren(parent, false);
                // existing is the last item an gets deleted in CollapseItem and at least on 64-bit linux it can
                // lead to a crash, because we use it again some lines later, but m_pItem is not 0 in some rare cases,
                // and therefore IsOk returns true !!
                // so we leave the function here
                return;
            }
            else
            {
                tree->Delete(existing);
                existing = next;
                continue;
            }
        }
        else
        {
            RemoveInvalidNodes(tree, existing); // recurse
        }
        if(existing.IsOk())
            existing = tree->GetPrevSibling(existing);
    }
}
#else
void ClassBrowserBuilderThread::RemoveInvalidNodes(wxTreeCtrl* tree, wxTreeItemId parent)
{
    if (TestDestroy() || Manager::IsAppShuttingDown() || (!(parent.IsOk())))
        return;

    // recursively enters all existing nodes and deletes the node if the token it references
    // is invalid (i.e. m_pTokens->at() != token_in_data)

    // we 'll loop backwards so we can delete nodes without problems
    wxTreeItemId existing = tree->GetLastChild(parent);
    while (existing.IsOk())
    {
        // recurse
        if (tree->ItemHasChildren(existing))
            RemoveInvalidNodes(tree, existing);

        CBTreeData* data = (CBTreeData*)tree->GetItemData(existing);
        if (data && data->m_pToken)
        {
            if (m_pTokens->at(data->m_TokenIndex) != data->m_pToken ||
                data->m_TokenKind != data->m_pToken->m_TokenKind || // need to compare kinds: the token index might have been reused...
                data->m_TokenName != data->m_pToken->m_Name || // same for the token name
                !TokenMatchesFilter(data->m_pToken))
            {
                // keep parent and set flag if this is the last child of parent
                wxTreeItemId parent = tree->GetItemParent(existing);
                bool isLastChild = tree->GetChildrenCount(parent) == 1;
                // we have to do this in two steps: first collapse and then set haschildren to false
                if (isLastChild && parent.IsOk() && tree == m_pTreeTop)
                {
                    CollapseItem(parent);
                    return;
                }
                else
                {
//                    Manager::Get()->GetLogManager()->DebugLog(F(_T("Item %s is invalid"), tree->GetItemText(existing).c_str()));
                    wxTreeItemId next = tree->GetPrevSibling(existing);
                    tree->Delete(existing);
                    existing = next;
                }

                // if this was the last child of its parent, collapse the parent
//                if (isLastChild)
//                    tree->SetItemHasChildren(parent, false);

                continue;
            }
        }

        existing = tree->GetPrevSibling(existing);
    }

//    if (parent != tree->GetRootItem() && tree->GetChildrenCount(parent) == 0)
//        tree->Delete(parent);
}
#endif

wxTreeItemId ClassBrowserBuilderThread::AddNodeIfNotThere(wxTreeCtrl* tree, wxTreeItemId parent, const wxString& name, int imgIndex, CBTreeData* data)
{
    SpecialFolder new_type = data->m_SpecialFolder;
    bool newIsNamespace = data->m_TokenKind == tkNamespace;

    compatibility::tree_cookie_t cookie = 0;

    wxTreeItemId insert_after; // node to insert after; we 'll be looping all children so we might as well sort at the same time
    wxTreeItemId existing = tree->GetFirstChild(parent, cookie);
    while (existing)
    {
        wxString itemText = tree->GetItemText(existing);
        if (itemText == name)
        {
            // update the existing node's image indices and user-data.
            // it's not possible to have the same token name more than once
            // under the same namespace anyway. if we do, there's a bug in the parser :(
            tree->SetItemImage(existing, imgIndex, wxTreeItemIcon_Normal);
            tree->SetItemImage(existing, imgIndex, wxTreeItemIcon_Selected);
            delete tree->GetItemData(existing); // make Valgrind happy
            tree->SetItemData(existing, data);

            return existing;
        }

        if (data)
        {
            CBTreeData* existing_data = (CBTreeData*)tree->GetItemData(existing);
            if (existing_data)
            {
                SpecialFolder existing_type = existing_data->m_SpecialFolder;

                // first go special folders
                if ((existing_type & (sfGFuncs | sfGVars | sfPreproc | sfTypedef)) &&
                    !(new_type & (sfGFuncs | sfGVars | sfPreproc | sfTypedef)))
                {
                    insert_after = existing;
                }
                // then go namespaces, alphabetically
                else if (newIsNamespace &&
                        existing_data->m_TokenKind == tkNamespace &&
                        (*m_pDataCompare)(data, existing_data) >= 0)
                        //name.CompareTo(itemText, wxString::ignoreCase) >= 0)
                {
                    insert_after = existing;
                }
                // then everything else, alphabetically
                else if (!newIsNamespace &&
                        (existing_data->m_TokenKind == tkNamespace ||
                        (*m_pDataCompare)(data, existing_data) >= 0))
                        //name.CompareTo(itemText, wxString::ignoreCase) >= 0))
                {
                    insert_after = existing;
                }
            }
        }
        existing = tree->GetNextChild(parent, cookie);
    }

    if (data)
        existing = tree->InsertItem(parent, insert_after, name, imgIndex, imgIndex, data);
    else
        existing = tree->AppendItem(parent, name, imgIndex, imgIndex, data);

    return existing;
}

bool ClassBrowserBuilderThread::AddChildrenOf(wxTreeCtrl* tree, wxTreeItemId parent, int parentTokenIdx, int tokenKindMask, bool sorted, int tokenScopeMask)
{
    if (TestDestroy() || Manager::IsAppShuttingDown())
        return false;

    Token* parentToken = 0;
    TokenIdxSet* tokens = 0;

    if (parentTokenIdx == -1)
    {
        if(m_Options.displayFilter >= bdfWorkspace)
            tokens = &m_pTokens->m_GlobalNameSpace;
        else
            tokens = &m_CurrentGlobalTokensSet;
    }
    else
    {
        parentToken = m_pTokens->at(parentTokenIdx);
        if (!parentToken)
        {
//            Manager::Get()->GetLogManager()->DebugLog(F(_T("Token not found?!?")));
            return false;
        }
        tokens = &parentToken->m_Children;
    }

    return AddNodes(tree, parent, *tokens, tokenKindMask, tokenScopeMask, m_Options.displayFilter == bdfEverything, sorted);
}

bool ClassBrowserBuilderThread::AddAncestorsOf(wxTreeCtrl* tree, wxTreeItemId parent, int tokenIdx)
{
    if (TestDestroy() || Manager::IsAppShuttingDown())
        return false;

    Token* token = m_pTokens->at(tokenIdx);
    if (!token)
        return false;

    return AddNodes(tree, parent, token->m_DirectAncestors, tkClass | tkTypedef, 0, true, true);
}

bool ClassBrowserBuilderThread::AddDescendantsOf(wxTreeCtrl* tree, wxTreeItemId parent, int tokenIdx, bool allowInheritance)
{
    if (TestDestroy() || Manager::IsAppShuttingDown())
        return false;

    Token* token = m_pTokens->at(tokenIdx);
    if (!token)
        return false;

    bool inh = m_Options.showInheritance;
    m_Options.showInheritance = allowInheritance;

    bool ret = AddNodes(tree, parent, token->m_Descendants, tkClass | tkTypedef, 0, true, true);

    m_Options.showInheritance = inh;
    return ret;
}

bool ClassBrowserBuilderThread::AddNodes(wxTreeCtrl* tree, wxTreeItemId parent, const TokenIdxSet& tokens, int tokenKindMask, int tokenScopeMask, bool allowGlobals, bool sorted)
{
    int count = 0;
    set<unsigned long, less<unsigned long> > tickets;

    // Build temporary list of Token tickets - if the token's ticket is present
    // among the parent node's children, it's a dupe, and we'll skip it.
    if(parent.IsOk() && tree == m_pTreeTop)
    {
        wxTreeItemIdValue cookie;
        wxTreeItemId curchild = tree->GetFirstChild(parent,cookie);
        while(curchild.IsOk())
        {
            CBTreeData* data = (CBTreeData*)(tree->GetItemData(curchild));
            curchild = tree->GetNextSibling(curchild);
            if(data && data->m_Ticket)
                tickets.insert(data->m_Ticket);
        }
    }

    TokenIdxSet::iterator start = tokens.begin();
    TokenIdxSet::iterator end = tokens.end();

    for ( ; start != end; ++start)
    {
        Token* token = m_pTokens->at(*start);
        if (token &&
            (token->m_TokenKind & tokenKindMask) &&
            (tokenScopeMask == 0 || token->m_Scope == tokenScopeMask) &&
            (allowGlobals || token->m_IsLocal ||
            TokenMatchesFilter(token)))
        {
            if(tree == m_pTreeTop && sorted && tickets.find(token->GetTicket()) != tickets.end())
                continue; // dupe
            ++count;
            int img = m_pParser->GetTokenKindImage(token);

            wxString str = token->m_Name;
            if (token->m_TokenKind == tkFunction || token->m_TokenKind == tkConstructor || token->m_TokenKind == tkDestructor)
                str << token->m_Args;
            // modification suggested by ollydbg in http://forums.codeblocks.org/index.php/topic,10242.msg70865.html#msg70865:
//            if (!token->m_ActualType.IsEmpty())
//                 str = str + _T(" : ") + token->m_ActualType;
            if (!token->m_Type.IsEmpty())
                 str = str + _T(" : ") + token->m_Type;

            if (sorted)
            {
                wxTreeItemId child = AddNodeIfNotThere(tree, parent, str, img, new CBTreeData(sfToken, token, tokenKindMask));

                // mark as expanding if it is a container
                int kind = tkClass | tkNamespace | tkEnum;
                if (token->m_TokenKind == tkClass)
                {
                    if (!m_Options.treeMembers)
                        kind |= tkTypedef | tkFunction | tkVariable | tkEnum;
                    tree->SetItemHasChildren(child, m_Options.showInheritance || TokenContainsChildrenOfKind(token, kind));
                }
                else if (token->m_TokenKind & (tkNamespace | tkEnum))
                {
                    if (!m_Options.treeMembers)
                        kind |= tkTypedef | tkFunction | tkVariable | tkEnumerator;
                    tree->SetItemHasChildren(child, TokenContainsChildrenOfKind(token, kind));
                }
            }
            else // the bottom tree needs no checks
                tree->AppendItem(parent, str, img, img, new CBTreeData(sfToken, token));
        }
    }

    //Manager::Get()->GetLogManager()->DebugLog(F(_T("Sorting...")));
    if (!sorted) // fast alphabetical sort
        tree->SortChildren(parent);
    //Manager::Get()->GetLogManager()->DebugLog(F(_T("Added %d nodes"), count));
    return count != 0;
}

bool ClassBrowserBuilderThread::TokenMatchesFilter(Token* token)
{
    if (token->m_IsTemp)
        return false;

    if (m_Options.displayFilter == bdfEverything ||
        (m_Options.displayFilter == bdfWorkspace && token->m_IsLocal))
        return true;

    if (m_Options.displayFilter == bdfFile && !m_CurrentTokenSet.empty())
    {
        if (m_CurrentTokenSet.find(token->GetSelf()) != m_CurrentTokenSet.end())
            return true;

        // we got to check all children of this token (recursively)
        // to see if any of them matches the filter...
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

bool ClassBrowserBuilderThread::TokenContainsChildrenOfKind(Token* token, int kind)
{
    if (!token)
        return false;
    TokensTree* tt = token->GetTree();
    for (TokenIdxSet::iterator it = token->m_Children.begin(); it != token->m_Children.end(); ++it)
    {
        Token* child = tt->at(*it);
        if (child->m_TokenKind & kind)
            return true;
    }
    return false;
}

void ClassBrowserBuilderThread::AddMembersOf(wxTreeCtrl* tree, wxTreeItemId node)
{
   if (TestDestroy() || Manager::IsAppShuttingDown() || !node.IsOk())
        return;

    CBTreeData* data = (CBTreeData*)m_pTreeTop->GetItemData(node);

    bool bottom = (tree == m_pTreeBottom);
    if (bottom)
    {
        tree->Freeze();
        tree->DeleteAllItems();
        node = tree->AddRoot(_T("Members")); // not visible, so don't translate
    }

    if (data)
    {
        switch (data->m_SpecialFolder)
        {
            case sfGFuncs: AddChildrenOf(tree, node, -1, tkFunction, false); break;
            case sfGVars: AddChildrenOf(tree, node, -1, tkVariable, false); break;
            case sfPreproc: AddChildrenOf(tree, node, -1, tkPreprocessor, false); break;
            case sfTypedef: AddChildrenOf(tree, node, -1, tkTypedef, false); break;
            case sfToken:
            {
                if (bottom)
                {
                    if (m_Options.sortType == bstKind && !(data->m_pToken->m_TokenKind & tkEnum))
                    {
                        wxTreeItemId rootCtorDtor = tree->AppendItem(node, _("Ctors & Dtors"), PARSER_IMG_CLASS_FOLDER);
                        wxTreeItemId rootFuncs = tree->AppendItem(node, _("Functions"), PARSER_IMG_FUNCS_FOLDER);
                        wxTreeItemId rootVars = tree->AppendItem(node, _("Variables"), PARSER_IMG_VARS_FOLDER);
                        wxTreeItemId rootOthers = tree->AppendItem(node, _("Others"), PARSER_IMG_OTHERS_FOLDER);

                        AddChildrenOf(tree, rootCtorDtor, data->m_pToken->GetSelf(), tkConstructor | tkDestructor);
                        AddChildrenOf(tree, rootFuncs, data->m_pToken->GetSelf(), tkFunction);
                        AddChildrenOf(tree, rootVars, data->m_pToken->GetSelf(), tkVariable);
                        AddChildrenOf(tree, rootOthers, data->m_pToken->GetSelf(), ~(tkNamespace | tkClass | tkEnum | tkAnyFunction | tkVariable));
                    }
                    else if (m_Options.sortType == bstScope && data->m_pToken->m_TokenKind & tkClass)
                    {
                        wxTreeItemId rootPublic = tree->AppendItem(node, _("Public"), PARSER_IMG_CLASS_FOLDER);
                        wxTreeItemId rootProtected = tree->AppendItem(node, _("Protected"), PARSER_IMG_FUNCS_FOLDER);
                        wxTreeItemId rootPrivate = tree->AppendItem(node, _("Private"), PARSER_IMG_VARS_FOLDER);

                        AddChildrenOf(tree, rootPublic, data->m_pToken->GetSelf(), ~(tkNamespace | tkClass | tkEnum), true, tsPublic);
                        AddChildrenOf(tree, rootProtected, data->m_pToken->GetSelf(), ~(tkNamespace | tkClass | tkEnum), true, tsProtected);
                        AddChildrenOf(tree, rootPrivate, data->m_pToken->GetSelf(), ~(tkNamespace | tkClass | tkEnum), true, tsPrivate);
                    }
                    else
                    {
                        AddChildrenOf(tree, node, data->m_pToken->GetSelf(), ~(tkNamespace | tkClass | tkEnum));
                        break;
                    }

                    wxTreeItemId existing = tree->GetLastChild(tree->GetRootItem());
                    while (existing.IsOk())
                    {
                        wxTreeItemId next = tree->GetPrevSibling(existing);

                        if (tree->GetChildrenCount(existing) > 0)
                            tree->SetItemBold(existing, true);
                        else
                        {
                            tree->Delete(existing);
                            existing = next;
                            continue;
                        }
                        existing = tree->GetPrevSibling(existing);
                    }
                }
                else
                    AddChildrenOf(tree, node, data->m_pToken->GetSelf(), ~(tkNamespace | tkClass | tkEnum));

                // add all children, except containers
                // AddChildrenOf(tree, node, data->m_pToken->GetSelf(), ~(tkNamespace | tkClass | tkEnum));
                break;
            }
            default: break;
        }
    }

    if (bottom)
    {
        tree->ExpandAll();
        tree->Thaw();
    }
}

// checks if there are respective children and colors the nodes
bool ClassBrowserBuilderThread::CreateSpecialFolders(wxTreeCtrl* tree, wxTreeItemId parent)
{
    bool hasGF = false;
    bool hasGV = false;
    bool hasGP = false;
    bool hasTD = false;

    // loop all tokens in global namespace and see if we have matches
    TokensTree* tt = m_pParser->GetTokens();
    for (TokenIdxSet::iterator it = tt->m_GlobalNameSpace.begin(); it != tt->m_GlobalNameSpace.end(); ++it)
    {
        Token* token = tt->at(*it);
        if (token && token->m_IsLocal && TokenMatchesFilter(token))
        {
            if (!hasGF && token->m_TokenKind == tkFunction)
                hasGF = true;
            else if (!hasGV && token->m_TokenKind == tkVariable)
                hasGV = true;
            else if (!hasGP && token->m_TokenKind == tkPreprocessor)
                hasGP = true;
            else if (!hasTD && token->m_TokenKind == tkTypedef)
                hasTD = true;
        }

        if (hasGF && hasGV && hasGP && hasTD)
            break; // we have everything, stop iterating...
    }

    wxTreeItemId gfuncs = AddNodeIfNotThere(m_pTreeTop, parent, _("Global functions"), PARSER_IMG_FUNCS_FOLDER, new CBTreeData(sfGFuncs, 0, tkFunction, -1));
    wxTreeItemId tdef = AddNodeIfNotThere(m_pTreeTop, parent, _("Global typedefs"), PARSER_IMG_TYPEDEF_FOLDER, new CBTreeData(sfTypedef, 0, tkTypedef, -1));
    wxTreeItemId gvars = AddNodeIfNotThere(m_pTreeTop, parent, _("Global variables"), PARSER_IMG_VARS_FOLDER, new CBTreeData(sfGVars, 0, tkVariable, -1));
    wxTreeItemId preproc = AddNodeIfNotThere(m_pTreeTop, parent, _("Preprocessor symbols"), PARSER_IMG_PREPROC_FOLDER, new CBTreeData(sfPreproc, 0, tkPreprocessor, -1));

    bool bottom = m_Options.treeMembers;
    m_pTreeTop->SetItemHasChildren(gfuncs, !bottom && hasGF);
    m_pTreeTop->SetItemHasChildren(tdef, !bottom && hasTD);
    m_pTreeTop->SetItemHasChildren(gvars, !bottom && hasGV);
    m_pTreeTop->SetItemHasChildren(preproc, !bottom && hasGP);

    wxColour black = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    wxColour grey = wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT);

    tree->SetItemTextColour(gfuncs, hasGF ? black : grey);
    tree->SetItemTextColour(gvars, hasGV ? black : grey);
    tree->SetItemTextColour(preproc, hasGP ? black : grey);
    tree->SetItemTextColour(tdef, hasTD ? black : grey);

    return hasGF || hasGV || hasGP || hasTD;
}

void ClassBrowserBuilderThread::ExpandItem(wxTreeItemId item)
{
    if (TestDestroy() || Manager::IsAppShuttingDown())
        return;

//    wxMutexLocker lock(m_BuildMutex);
    CBTreeData* data = (CBTreeData*)m_pTreeTop->GetItemData(item);
    if (data)
    {
        switch (data->m_SpecialFolder)
        {
            case sfRoot:
            {
                CreateSpecialFolders(m_pTreeTop, item);
                AddChildrenOf(m_pTreeTop, item, -1, ~(tkFunction | tkVariable | tkPreprocessor | tkTypedef));
                break;
            }
            case sfBase: AddAncestorsOf(m_pTreeTop, item, data->m_pToken->GetSelf()); break;
            case sfDerived: AddDescendantsOf(m_pTreeTop, item, data->m_pToken->GetSelf(), false); break;
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
                break;
            }
            default: break;
        }
    }
    if (m_pParser && !m_Options.treeMembers)
    {
        AddMembersOf(m_pTreeTop, item);
    }
//    Manager::Get()->GetLogManager()->DebugLog(F(_("E: %d items"), m_pTreeTop->GetCount()));
}

void ClassBrowserBuilderThread::CollapseItem(wxTreeItemId item, bool useLock)
{
    if (TestDestroy() || Manager::IsAppShuttingDown())
        return;

    if (useLock)
    wxMutexLocker lock(m_BuildMutex);
#ifndef __WXGTK__
    m_pTreeTop->CollapseAndReset(item); // this freezes gtk
#else
    m_pTreeTop->DeleteChildren(item);
#endif
    m_pTreeTop->SetItemHasChildren(item);
//    Manager::Get()->GetLogManager()->DebugLog(F(_("C: %d items"), m_pTreeTop->GetCount()));
}

void ClassBrowserBuilderThread::SelectItem(wxTreeItemId item)
{
    if (TestDestroy() || Manager::IsAppShuttingDown())
        return;

    wxMutexLocker lock(m_BuildMutex);

    wxTreeCtrl* tree = (m_Options.treeMembers) ? m_pTreeBottom : m_pTreeTop;
    AddMembersOf(tree, item);
//    Manager::Get()->GetLogManager()->DebugLog(F(_T("Select ") + m_pTreeTop->GetItemText(item)));
}

void ClassBrowserBuilderThread::SaveExpandedItems(wxTreeCtrl* tree, wxTreeItemId parent, int level)
{
    if (TestDestroy() || Manager::IsAppShuttingDown())
        return;

    wxTreeItemIdValue cookie;
    wxTreeItemId existing = tree->GetFirstChild(parent, cookie);
    while (existing.IsOk())
    {
        CBTreeData* data = (CBTreeData*)(tree->GetItemData(existing));
        if (tree->GetChildrenCount(existing,false) > 0)
        {
            m_ExpandedVect.push_back(CBExpandedItemData(data, level));

            SaveExpandedItems(tree, existing, level + 1);
        }

        existing = tree->GetNextSibling(existing);
    }
}

void ClassBrowserBuilderThread::ExpandSavedItems(wxTreeCtrl* tree, wxTreeItemId parent, int level)
{
    if (TestDestroy() || Manager::IsAppShuttingDown())
        return;

    wxTreeItemIdValue cookie;
    wxTreeItemId existing = tree->GetFirstChild(parent, cookie);
    while (existing.IsOk() && !m_ExpandedVect.empty())
    {
        CBTreeData* data = (CBTreeData*)(tree->GetItemData(existing));
        CBExpandedItemData saved = m_ExpandedVect.front();

        if (level == saved.GetLevel() &&
            wxStrcmp(data->m_TokenName, saved.GetData().m_TokenName) == 0 &&
            data->m_TokenKind == saved.GetData().m_TokenKind &&
            data->m_SpecialFolder == saved.GetData().m_SpecialFolder)
        {
            tree->Expand(existing);
            //ExpandItem(existing);

            m_ExpandedVect.pop_front();

            if (m_ExpandedVect.empty())
                return;

            saved = m_ExpandedVect.front(); //next saved
            if (saved.GetLevel() < level)
                return;

            if (saved.GetLevel() > level)
                ExpandSavedItems(tree, existing, saved.GetLevel());
        }

        existing = tree->GetNextSibling(existing);
    }

    //remove non-existing by now saved items
    while(!m_ExpandedVect.empty() && m_ExpandedVect.front().GetLevel() > level)
        m_ExpandedVect.pop_front();

    //m_ExpandedVect.clear();
}

void ClassBrowserBuilderThread::SaveSelectedItem()
{
    if (TestDestroy() || Manager::IsAppShuttingDown())
        return;

    m_SelectedPath.clear();

    wxTreeItemId item = m_pTreeTop->GetSelection();
    while(item.IsOk() && item != m_pTreeTop->GetRootItem())
    {
        CBTreeData* data = (CBTreeData*)m_pTreeTop->GetItemData(item);
        m_SelectedPath.push_front(*data);

        item = m_pTreeTop->GetItemParent(item);
    }
}

void ClassBrowserBuilderThread::SelectSavedItem()
{
    if (TestDestroy() || Manager::IsAppShuttingDown())
        return;

    wxTreeItemId parent = m_pTreeTop->GetRootItem();

    wxTreeItemIdValue cookie;
    wxTreeItemId item = m_pTreeTop->GetFirstChild(parent, cookie);

    while (!m_SelectedPath.empty() && item.IsOk())
    {
        CBTreeData* data = (CBTreeData*)m_pTreeTop->GetItemData(item);
        CBTreeData* saved = &m_SelectedPath.front();

        if (data->m_SpecialFolder == saved->m_SpecialFolder &&
            wxStrcmp(data->m_TokenName, saved->m_TokenName) == 0 &&
            data->m_TokenKind == saved->m_TokenKind)
        {
            wxTreeItemIdValue cookie;
            parent = item;
            item = m_pTreeTop->GetFirstChild(item, cookie);
            m_SelectedPath.pop_front();
        }
        else
            item = m_pTreeTop->GetNextSibling(item);
    }

    m_pTreeTop->SelectItem(parent, true);
    m_SelectedPath.clear();
}

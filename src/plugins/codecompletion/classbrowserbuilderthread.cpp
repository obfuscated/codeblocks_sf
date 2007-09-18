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

ClassBrowserBuilderThread::ClassBrowserBuilderThread(wxSemaphore& sem, ClassBrowserBuilderThread** threadVar)
    : wxThread(wxTHREAD_DETACHED),
    m_Semaphore(sem),
    m_pParser(0),
    m_pTreeTop(0),
    m_pTreeBottom(0),
    m_pUserData(0),
    m_Options(),
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
        BuildTree();
    }
}

void* ClassBrowserBuilderThread::Entry()
{
    while (!TestDestroy() && !Manager::IsAppShuttingDown())
    {
        // wait until the classbrowser signals
        m_Semaphore.Wait();
//        DBGLOG(_T(" - - - - - -"));

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
            {
                ::wxMutexGuiEnter();
            }
        }

        BuildTree();

        if (TestDestroy() || Manager::IsAppShuttingDown())
        {
        if(platform::gtk)
        {
			if(!::wxIsMainThread())
			{
				::wxMutexGuiLeave();
			}
        }
            break;
        }

		m_pTreeTop->Freeze();
        ExpandNamespaces(m_pTreeTop->GetRootItem());
	    m_pTreeTop->Thaw();

        if(platform::gtk)
        {
            // this code (PART 2/2) seems to be good on linux
            // because of it the libcairo crash on dualcore processors
            // is gone, but on windows it has very bad influence,
            // henceforth the ifdef guard
            // the questions remains if it is the correct solution
            if(!::wxIsMainThread())
            {
                ::wxMutexGuiLeave();
            }
        }
    }

    m_pParser = 0;
    m_pTreeTop = 0;
    m_pTreeBottom = 0;

    if (m_ppThreadVar)
        *m_ppThreadVar = 0;
    return 0;
}

void ClassBrowserBuilderThread::ExpandNamespaces(wxTreeItemId node)
{
	if (!m_Options.expandNS || !node.IsOk())
		return;

	wxTreeItemIdValue enumerationCookie;
    wxTreeItemId existing = m_pTreeTop->GetFirstChild(node, enumerationCookie);
    while (existing.IsOk())
    {
		CBTreeData* data = (CBTreeData*)m_pTreeTop->GetItemData(existing);
		if (data && data->m_pToken && data->m_pToken->m_TokenKind == tkNamespace)
		{
//			DBGLOG(_T("Auto-expanding: ") + data->m_pToken->m_Name);
			m_pTreeTop->Expand(existing);
			ExpandNamespaces(existing); // recurse
		}

    	existing = m_pTreeTop->GetNextSibling(existing);
	}
}

void ClassBrowserBuilderThread::BuildTree()
{
    if (Manager::IsAppShuttingDown())
        return;
//    wxMutexLocker lock(m_BuildMutex);

    m_pTreeTop->SetImageList(m_pParser->GetImageList());
    m_pTreeBottom->SetImageList(m_pParser->GetImageList());

    wxTreeItemId root = m_pTreeTop->GetRootItem();
    if (!root.IsOk())
    {
        root = m_pTreeTop->AddRoot(_("Symbols"), PARSER_IMG_SYMBOLS_FOLDER, PARSER_IMG_SYMBOLS_FOLDER, new CBTreeData(sfRoot));
        m_pTreeTop->SetItemHasChildren(root);
    }

    m_pTreeTop->Hide();
    m_pTreeBottom->Hide();
    m_pTreeTop->Freeze();
    m_pTreeBottom->Freeze();

	RemoveInvalidNodes(m_pTreeTop, root);
	RemoveInvalidNodes(m_pTreeBottom, m_pTreeBottom->GetRootItem());

    if (!TestDestroy() && !Manager::IsAppShuttingDown())
    {
        // the tree is completely dynamic: it is populated when a node expands/collapses.
        // so, by expanding the root node, we already instruct it to fill the top level :)
        //
        // this technique makes it really fast to draw (we only draw what's expanded) and
        // has very minimum memory overhead since it contains as few items as possible.
        // plus, it doesn't flicker because we 're not emptying it and re-creating it each time ;)
        m_pTreeTop->Expand(root);

        if(platform::gtk)
        {
            // seems like the "expand" event comes too late in wxGTK,
            // so make it happen now
            ExpandItem(root);
        }
    }

    m_pTreeBottom->Thaw();
    m_pTreeTop->Thaw();
    m_pTreeBottom->Show();
    m_pTreeTop->Show();

    SelectNode(m_pTreeTop->GetSelection()); // refresh selection
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
    wxTreeItemId root = tree->GetRootItem();
    while (parent.IsOk() && existing.IsOk())
    {
        bool removeCurrent = false;
        bool hasChildren = (tree->GetChildrenCount(existing) > 0);
        CBTreeData* data = (CBTreeData*)(tree->GetItemData(existing));

        if (tree == m_pTreeBottom)
            removeCurrent = true;
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
//                	DBGLOG(_T("Item %s is invalid"), tree->GetItemText(existing).c_str());
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
wxTreeItemId ClassBrowserBuilderThread::AddNodeIfNotThere(wxTreeCtrl* tree, wxTreeItemId parent, const wxString& name, int imgIndex, CBTreeData* data, bool sorted)
{
    sorted = sorted & tree == m_pTreeTop && data; // sorting only for the top tree
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

        if (sorted)
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
                        name.CompareTo(itemText, wxString::ignoreCase) >= 0)
                {
                    insert_after = existing;
                }
                // then everything else, alphabetically
                else if (!newIsNamespace &&
                        (existing_data->m_TokenKind == tkNamespace ||
                        name.CompareTo(itemText, wxString::ignoreCase) >= 0))
                {
                    insert_after = existing;
                }
            }
        }
        existing = tree->GetNextChild(parent, cookie);
    }

    if (sorted)
        existing = tree->InsertItem(parent, insert_after, name, imgIndex, imgIndex, data);
    else
        existing = tree->AppendItem(parent, name, imgIndex, imgIndex, data);
    return existing;
}

bool ClassBrowserBuilderThread::AddChildrenOf(wxTreeCtrl* tree, wxTreeItemId parent, int parentTokenIdx, int tokenKindMask)
{
    if (TestDestroy() || Manager::IsAppShuttingDown())
        return false;

    Token* parentToken = 0;
    TokenIdxSet::iterator it;
    TokenIdxSet::iterator it_end;

    if (parentTokenIdx == -1)
    {
        if(m_Options.displayFilter == bdfWorkspace)
        {
            it = m_pTokens->m_GlobalNameSpace.begin();
            it_end = m_pTokens->m_GlobalNameSpace.end();
        }
        else
        {
            it = m_CurrentGlobalTokensSet.begin();
            it_end = m_CurrentGlobalTokensSet.end();
        }
    }
    else
    {
        parentToken = m_pTokens->at(parentTokenIdx);
        if (!parentToken)
        {
//            DBGLOG(_T("Token not found?!?"));
            return false;
        }
        it = parentToken->m_Children.begin();
        it_end = parentToken->m_Children.end();
    }

    return AddNodes(tree, parent, it, it_end, tokenKindMask);
}

bool ClassBrowserBuilderThread::AddAncestorsOf(wxTreeCtrl* tree, wxTreeItemId parent, int tokenIdx)
{
    if (TestDestroy() || Manager::IsAppShuttingDown())
        return false;

    Token* token = m_pTokens->at(tokenIdx);
    if (!token)
        return false;

    return AddNodes(tree, parent, token->m_DirectAncestors.begin(), token->m_DirectAncestors.end(), tkClass | tkTypedef, true);
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

    bool ret = AddNodes(tree, parent, token->m_Descendants.begin(), token->m_Descendants.end(), tkClass | tkTypedef, true);

    m_Options.showInheritance = inh;
    return ret;
}

bool ClassBrowserBuilderThread::AddNodes(wxTreeCtrl* tree, wxTreeItemId parent, TokenIdxSet::iterator start, TokenIdxSet::iterator end, int tokenKindMask, bool allowGlobals)
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

    for ( ; start != end; ++start)
    {
        Token* token = m_pTokens->at(*start);
        if (token &&
            (token->m_TokenKind & tokenKindMask) &&
            (allowGlobals || token->m_IsLocal) &&
            TokenMatchesFilter(token))
        {
            if(tree == m_pTreeTop && tickets.find(token->GetTicket()) != tickets.end())
                continue; // dupe
            ++count;
            int img = m_pParser->GetTokenKindImage(token);

            wxString str = token->m_Name;
            if (token->m_TokenKind == tkFunction || token->m_TokenKind == tkConstructor || token->m_TokenKind == tkDestructor)
                str << token->m_Args;
            if (!token->m_ActualType.IsEmpty())
                 str = str + _T(" : ") + token->m_ActualType;

            if (tree == m_pTreeTop)
            {
                wxTreeItemId child = AddNodeIfNotThere(tree, parent, str, img, new CBTreeData(sfToken, token, tokenKindMask));
                // mark as expanding if it is a container
                if (token->m_TokenKind == tkClass)
                    tree->SetItemHasChildren(child, m_Options.showInheritance || TokenContainsChildrenOfKind(token, tkClass | tkNamespace | tkEnum));
                else if (token->m_TokenKind & (tkNamespace | tkEnum))
                    tree->SetItemHasChildren(child, TokenContainsChildrenOfKind(token, tkClass | tkNamespace | tkEnum));
            }
            else // the bottom tree needs no checks
                tree->AppendItem(parent, str, img, img, new CBTreeData(sfToken, token));
        }
    }
//    DBGLOG(_T("Sorting..."));
    if (tree == m_pTreeBottom) // only sort alphabetically the bottom tree
        tree->SortChildren(parent);
//    DBGLOG(_T("Added %d nodes"), count);
    return count != 0;
}

bool ClassBrowserBuilderThread::TokenMatchesFilter(Token* token)
{
    if (token->m_IsTemp)
        return false;

    if (m_Options.displayFilter == bdfWorkspace)
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

void ClassBrowserBuilderThread::SelectNode(wxTreeItemId node)
{
    if (TestDestroy() || Manager::IsAppShuttingDown() || (!(node.IsOk())))
        return;

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
            case sfTypedef: AddChildrenOf(m_pTreeBottom, root, -1, tkTypedef); break;
            case sfToken:
            {
                // add all children, except containers
                AddChildrenOf(m_pTreeBottom, root, data->m_pToken->GetSelf(), ~(tkNamespace | tkClass | tkEnum));
                break;
            }
            default: break;
        }
    }
    m_pTreeBottom->Thaw();
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

    wxTreeItemId gfuncs = AddNodeIfNotThere(m_pTreeTop, parent, _("Global functions"), PARSER_IMG_OTHERS_FOLDER, new CBTreeData(sfGFuncs, 0, tkFunction, -1));
    wxTreeItemId tdef = AddNodeIfNotThere(m_pTreeTop, parent, _("Global typedefs"), PARSER_IMG_TYPEDEF_FOLDER, new CBTreeData(sfTypedef, 0, tkTypedef, -1));
    wxTreeItemId gvars = AddNodeIfNotThere(m_pTreeTop, parent, _("Global variables"), PARSER_IMG_OTHERS_FOLDER, new CBTreeData(sfGVars, 0, tkVariable, -1));
    wxTreeItemId preproc = AddNodeIfNotThere(m_pTreeTop, parent, _("Preprocessor symbols"), PARSER_IMG_PREPROC_FOLDER, new CBTreeData(sfPreproc, 0, tkPreprocessor, -1));

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

    wxMutexLocker lock(m_BuildMutex);
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
//    DBGLOG(_("E: %d items"), m_pTreeTop->GetCount());
}

void ClassBrowserBuilderThread::CollapseItem(wxTreeItemId item)
{
    if (TestDestroy() || Manager::IsAppShuttingDown())
        return;

    wxMutexLocker lock(m_BuildMutex);
#ifndef __WXGTK__
    m_pTreeTop->CollapseAndReset(item); // this freezes gtk
#else
    m_pTreeTop->DeleteChildren(item);
#endif
    m_pTreeTop->SetItemHasChildren(item);
//    DBGLOG(_("C: %d items"), m_pTreeTop->GetCount());
}

void ClassBrowserBuilderThread::SelectItem(wxTreeItemId item)
{
    if (TestDestroy() || Manager::IsAppShuttingDown())
        return;

    wxMutexLocker lock(m_BuildMutex);
    SelectNode(item);
//    DBGLOG(_T("Select ") + m_pTreeTop->GetItemText(item));
}

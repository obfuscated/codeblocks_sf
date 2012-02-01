/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>

#ifndef CB_PRECOMP
    #include <algorithm>

    #include <wx/settings.h>
#ifdef CC_BUILDTREE_MEASURING
    #include <wx/stopwatch.h>
#endif
    #include <wx/utils.h>

    #include <cbproject.h>
    #include <cbstyledtextctrl.h>
    #include <globals.h>
    #include <logmanager.h>
    #include <manager.h>
    #include <projectmanager.h>
#endif

#include "classbrowserbuilderthread.h"

#define CC_BUILDERTHREAD_DEBUG_OUTPUT 0

#if CC_GLOBAL_DEBUG_OUTPUT == 1
    #undef CC_BUILDERTHREAD_DEBUG_OUTPUT
    #define CC_BUILDERTHREAD_DEBUG_OUTPUT 1
#elif CC_GLOBAL_DEBUG_OUTPUT == 2
    #undef CC_BUILDERTHREAD_DEBUG_OUTPUT
    #define CC_BUILDERTHREAD_DEBUG_OUTPUT 2
#endif

#if CC_BUILDERTHREAD_DEBUG_OUTPUT == 1
    #define TRACE(format, args...) \
        CCLogger::Get()->DebugLog(F(format, ##args))
    #define TRACE2(format, args...)
#elif CC_BUILDERTHREAD_DEBUG_OUTPUT == 2
    #define TRACE(format, args...)                                              \
        do                                                                      \
        {                                                                       \
            if (g_EnableDebugTrace)                                             \
                CCLogger::Get()->DebugLog(F(format, ##args));                   \
        }                                                                       \
        while (false)
    #define TRACE2(format, args...) \
        CCLogger::Get()->DebugLog(F(format, ##args))
#else
    #define TRACE(format, args...)
    #define TRACE2(format, args...)
#endif

IMPLEMENT_DYNAMIC_CLASS(CBTreeCtrl, wxTreeCtrl)

CBTreeCtrl::CBTreeCtrl()
{
   Compare = &CBNoCompare;
}

CBTreeCtrl::CBTreeCtrl(wxWindow *parent, const wxWindowID id,const wxPoint& pos, const wxSize& size,long style)
                       :wxTreeCtrl(parent, id, pos, size, style)
{
   Compare = &CBNoCompare;
}

void CBTreeCtrl::SetCompareFunction(const BrowserSortType type)
{
    switch (type)
    {
        case bstAlphabet:
            Compare = &CBAlphabetCompare;
            break;
        case bstKind:
            Compare = &CBKindCompare;
            break;
        case bstScope:
            Compare = &CBScopeCompare;
            break;
        case bstNone:
        default:
            Compare = &CBNoCompare;
            break;
    }

}

int CBTreeCtrl::OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2)
{
    return Compare((CBTreeData*)GetItemData(item1), (CBTreeData*)GetItemData(item2));
}

int CBTreeCtrl::CBAlphabetCompare (CBTreeData* lhs, CBTreeData* rhs)
{
    if (!lhs || !rhs)
        return 1;
    if (lhs->m_SpecialFolder != sfToken || rhs->m_SpecialFolder != sfToken)
        return -1;
    if (!lhs->m_Token || !rhs->m_Token)
        return 1;
    return wxStricmp(lhs->m_Token->m_Name, rhs->m_Token->m_Name);
}

int CBTreeCtrl::CBKindCompare(CBTreeData* lhs, CBTreeData* rhs)
{
    if (!lhs || !rhs)
        return 1;
    if (lhs->m_SpecialFolder != sfToken || rhs->m_SpecialFolder != sfToken)
        return -1;
    if (lhs->m_TokenKind == rhs->m_TokenKind)
        return CBAlphabetCompare(lhs, rhs);

    return lhs->m_TokenKind - rhs->m_TokenKind;
}

int CBTreeCtrl::CBScopeCompare(CBTreeData* lhs, CBTreeData* rhs)
{
    if (!lhs || !rhs)
        return 1;
    if (lhs->m_SpecialFolder != sfToken || rhs->m_SpecialFolder != sfToken)
        return -1;

    if (lhs->m_Token->m_Scope == rhs->m_Token->m_Scope)
        return CBKindCompare(lhs, rhs);

    return rhs->m_Token->m_Scope - lhs->m_Token->m_Scope;
}

int CBTreeCtrl::CBNoCompare(CBTreeData* lhs, CBTreeData* rhs)
{
    return 0;
}

// This does not really do what it says !
// It only removes doubles, if they are neighbours, so the tree should be sorted !!
// The last one (after sorting) remains.
void CBTreeCtrl::RemoveDoubles(const wxTreeItemId& parent)
{
    if (Manager::IsAppShuttingDown() || (!(parent.IsOk())))
        return;

#ifdef CC_BUILDTREE_MEASURING
    wxStopWatch sw;
#endif
    // we 'll loop backwards so we can delete nodes without problems
    wxTreeItemId existing = GetLastChild(parent);
    while (parent.IsOk() && existing.IsOk())
    {
        wxTreeItemId prevItem = GetPrevSibling(existing);
        if (!prevItem.IsOk())
            break;
        CBTreeData* dataExisting = (CBTreeData*)(GetItemData(existing));
        CBTreeData* dataPrev = (CBTreeData*)(GetItemData(prevItem));
        if (dataExisting &&
           dataPrev &&
           dataExisting->m_SpecialFolder == sfToken &&
           dataPrev->m_SpecialFolder == sfToken &&
           dataExisting->m_Token &&
           dataPrev->m_Token &&
           (dataExisting->m_Token->DisplayName() == dataPrev->m_Token->DisplayName()))
        {
            Delete(prevItem);
        }
        else if (existing.IsOk())
            existing = GetPrevSibling(existing);
    }
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F(_T("RemoveDoubles took : %ld"), sw.Time()));
#endif
}

// ClassBrowserBuilderThread
ClassBrowserBuilderThread::ClassBrowserBuilderThread(wxSemaphore& sem) :
    wxThread(wxTHREAD_JOINABLE),
    m_ClassBrowserBuilderThreadMutex(),
    m_ClassBrowserSemaphore(sem),
    m_NativeParser(0),
    m_TreeTop(0),
    m_TreeBottom(0),
    m_UserData(0),
    m_Options(),
    m_TokensTree(0),
    m_InitDone(false)
{
    //ctor
}

ClassBrowserBuilderThread::~ClassBrowserBuilderThread()
{
    //dtor
}

void ClassBrowserBuilderThread::Init(NativeParser*         nativeParser,
                                     CBTreeCtrl*           treeTop,
                                     CBTreeCtrl*           treeBottom,
                                     const wxString&       active_filename,
                                     void*                 user_data, // active project
                                     const BrowserOptions& options,
                                     TokensTree*           pTokensTree,
                                     bool                  build_tree,
                                     int                   idCBMakeSelectItem)
{
    CC_LOCKER_TRACK_MTX_LOCK(m_ClassBrowserBuilderThreadMutex);

    m_NativeParser       = nativeParser;
    m_TreeTop            = treeTop;
    m_TreeBottom         = treeBottom;
    m_ActiveFilename     = active_filename;
    m_UserData           = user_data;
    m_Options            = options;
    m_TokensTree         = pTokensTree;
    m_idCBMakeSelectItem = idCBMakeSelectItem;

    m_CurrentFileSet.clear();
    m_CurrentTokenSet.clear();

    TokensTree* tree = m_NativeParser->GetParser().GetTokensTree();

    // fill filter set for current-file-filter
    if (m_Options.displayFilter == bdfFile && !m_ActiveFilename.IsEmpty())
    {
        // m_ActiveFilename is the full filename up to the extension dot. No extension though.
        // get all filenames' indices matching our mask
        wxArrayString paths = m_NativeParser->GetAllPathsByFilename(m_ActiveFilename);

        // Should add locker after called m_NativeParser->GetAllPathsByFilename
        CC_LOCKER_TRACK_CS_ENTER(s_TokensTreeCritical)

        TokenFilesSet tmp;
        for (size_t i = 0; i < paths.GetCount(); ++i)
        {
            tree->m_FilenamesMap.FindMatches(paths[i], tmp, true, true);
            for (TokenFilesSet::iterator it = tmp.begin(); it != tmp.end(); ++it)
                m_CurrentFileSet.insert(*it);
        }

        CC_LOCKER_TRACK_CS_LEAVE(s_TokensTreeCritical);
    }
    else if (m_Options.displayFilter == bdfProject && (user_data != 0))
    {
        CC_LOCKER_TRACK_CS_ENTER(s_TokensTreeCritical)

        cbProject* prj = (cbProject*)user_data;
        for (FilesList::iterator it = prj->GetFilesList().begin(); it != prj->GetFilesList().end(); ++it)
        {
            ProjectFile* curfile = *it;
            if (!curfile)
                continue;

            wxString filename = curfile->file.GetFullPath();
            size_t fileIdx = tree->m_FilenamesMap.GetItemNo(filename);
            if (fileIdx)
                m_CurrentFileSet.insert(fileIdx);
        }

        CC_LOCKER_TRACK_CS_LEAVE(s_TokensTreeCritical);
    }

    if (!m_CurrentFileSet.empty())
    {
        CC_LOCKER_TRACK_CS_ENTER(s_TokensTreeCritical)

        m_CurrentTokenSet.clear();
        m_CurrentGlobalTokensSet.clear();
        for (TokenFilesSet::iterator it = m_CurrentFileSet.begin();it != m_CurrentFileSet.end(); ++it)
        {
            TokenIdxSet* curset = &(tree->m_FilesMap[*it]);
            for (TokenIdxSet::iterator it2 = curset->begin(); it2 != curset->end(); ++it2)
            {
                Token* curtoken = tree->at(*it2);
                if (curtoken)
                {
                    m_CurrentTokenSet.insert(*it2);
                    if (curtoken->m_ParentIndex == -1)
                        m_CurrentGlobalTokensSet.insert(*it2);
                }
            }
        }

        CC_LOCKER_TRACK_CS_LEAVE(s_TokensTreeCritical);
    }

    if (build_tree)
        BuildTree();

    m_InitDone = true;

    CC_LOCKER_TRACK_MTX_UNLOCK(m_ClassBrowserBuilderThreadMutex)
}

void* ClassBrowserBuilderThread::Entry()
{
    while (!TestDestroy() && !Manager::IsAppShuttingDown())
    {
        // waits here, until the ClassBrowser unlocks
        m_ClassBrowserSemaphore.Wait();

        if (TestDestroy() || Manager::IsAppShuttingDown())
            break;

        if (platform::gtk)
        {
            // this code (PART 1/2) seems to be good on linux
            // because of it the libcairo crash on dualcore processors
            // is gone, but on windows it has very bad influence,
            // henceforth the ifdef guard
            // the questions remains if it is the correct solution
            if (!::wxIsMainThread())
                ::wxMutexGuiEnter();
        }

        BuildTree();

        if (platform::gtk)
        {
            // this code (PART 2/2) seems to be good on linux
            // because of it the libcairo crash on dualcore processors
            // is gone, but on windows it has very bad influence,
            // henceforth the ifdef guard
            // the questions remains if it is the correct solution
            if (!::wxIsMainThread())
                ::wxMutexGuiLeave();
        }
    }

    m_NativeParser = 0;
    m_TreeTop = 0;
    m_TreeBottom = 0;

    return 0;
}

void ClassBrowserBuilderThread::ExpandNamespaces(wxTreeItemId node)
{
    if ((!::wxIsMainThread() && TestDestroy()) || Manager::IsAppShuttingDown())
        return;

    if (!m_Options.expandNS || !node.IsOk())
        return;

    wxTreeItemIdValue enumerationCookie;
    wxTreeItemId existing = m_TreeTop->GetFirstChild(node, enumerationCookie);
    while (existing.IsOk())
    {
        CBTreeData* data = (CBTreeData*)m_TreeTop->GetItemData(existing);
        if (data && data->m_Token && data->m_Token->m_TokenKind == tkNamespace)
        {
//            CCLogger::Get()->DebugLog(F(_T("Auto-expanding: ") + data->m_Token->m_Name));
            m_TreeTop->Expand(existing);
            ExpandNamespaces(existing); // recurse
        }

        existing = m_TreeTop->GetNextSibling(existing);
    }
}

void ClassBrowserBuilderThread::BuildTree()
{
    if ((!::wxIsMainThread() && TestDestroy()) || Manager::IsAppShuttingDown())
        return;

#ifdef CC_BUILDTREE_MEASURING
    wxStopWatch sw;
    wxStopWatch sw_total;
#endif
    m_TreeTop->SetImageList(m_NativeParser->GetImageList());
    m_TreeBottom->SetImageList(m_NativeParser->GetImageList());

    // Create initial root node, if not already there
    wxTreeItemId root = m_TreeTop->GetRootItem();
    if (!root.IsOk())
    {
        root = m_TreeTop->AddRoot(_("Symbols"), PARSER_IMG_SYMBOLS_FOLDER, PARSER_IMG_SYMBOLS_FOLDER, new CBTreeData(sfRoot));
        m_TreeTop->SetItemHasChildren(root);
    }

    m_TreeTop->SetCompareFunction(m_Options.sortType);
    m_TreeBottom->SetCompareFunction(m_Options.sortType);

    m_ExpandedVect.clear();
    SaveExpandedItems(m_TreeTop, root, 0);
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F(_T("Saving expanded items took : %ld ms"),sw.Time()));
    sw.Start();
#endif

    SaveSelectedItem();
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F(_T("Saving selected items took : %ld ms"),sw.Time()));
    sw.Start();
#endif

    if (m_Options.treeMembers)
    {
        m_TreeBottom->Hide();
        m_TreeBottom->Freeze();
    }
    m_TreeTop->Hide();
    m_TreeTop->Freeze();
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F(_T("Hiding and freezing trees took : %ld ms"),sw.Time()));
    sw.Start();
#endif

    RemoveInvalidNodes(m_TreeTop, root);
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F(_T("Removing invalid nodes (top tree) took : %ld ms"),sw.Time()));
    sw.Start();
#endif

    if (m_Options.treeMembers)
    {
        RemoveInvalidNodes(m_TreeBottom, m_TreeBottom->GetRootItem());
#ifdef CC_BUILDTREE_MEASURING
        CCLogger::Get()->DebugLog(F(_T("Removing invalid nodes (bottom tree) took : %ld ms"),sw.Time()));
        sw.Start();
#endif
    }

    if ((!::wxIsMainThread() && TestDestroy()) || Manager::IsAppShuttingDown())
        return;
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F(_T("TestDestroy() took : %ld ms"),sw.Time()));
    sw.Start();
#endif

#ifndef CC_NO_COLLAPSE_ITEM
    // the tree is completely dynamic: it is populated when a node expands/collapses.
    // so, by expanding the root node, we already instruct it to fill the top level :)
    //
    // this technique makes it really fast to draw (we only draw what's expanded) and
    // has very minimum memory overhead since it contains as few items as possible.
    // plus, it doesn't flicker because we 're not emptying it and re-creating it each time ;)

    CollapseItem(root);
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F(_T("Collapsing root item took : %ld ms"),sw.Time()));
    sw.Start();
#endif

    // Bottleneck: Takes ~4 secs on C::B workspace:
    m_TreeTop->Expand(root);
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F(_T("Expanding root item took : %ld ms"),sw.Time()));
    sw.Start();
#endif
#endif // CC_NO_COLLAPSE_ITEM

    // seems like the "expand" event comes too late in wxGTK, so make it happen now
    if (platform::gtk)
        ExpandItem(root);
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F(_T("Expanding root item (gtk only) took : %ld ms"),sw.Time()));
    sw.Start();
#endif

    ExpandSavedItems(m_TreeTop, root, 0);
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F(_T("Expanding saved items took : %ld ms"),sw.Time()));
    sw.Start();
#endif

    // Bottleneck: Takes ~4 secs on C::B workspace:
    SelectSavedItem();
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F(_T("Selecting saved item took : %ld ms"),sw.Time()));
    sw.Start();
#endif

    if (m_Options.treeMembers)
    {
        m_TreeBottom->Thaw();
#ifdef CC_BUILDTREE_MEASURING
        CCLogger::Get()->DebugLog(F(_T("Thaw bottom tree took : %ld ms"),sw.Time()));
        sw.Start();
#endif

        m_TreeBottom->Show();
#ifdef CC_BUILDTREE_MEASURING
        CCLogger::Get()->DebugLog(F(_T("Showing bottom tree took : %ld ms"),sw.Time()));
        sw.Start();
#endif
    }

    ExpandNamespaces(m_TreeTop->GetRootItem());
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F(_T("Expanding namespaces took : %ld ms"),sw.Time()));
    sw.Start();
#endif

    m_TreeTop->Thaw();
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F(_T("Thaw top tree took : %ld ms"),sw.Time()));
    sw.Start();
#endif

    // Bottleneck: Takes ~4 secs on C::B workspace:
    m_TreeTop->Show();
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F(_T("Show top tree took : %ld ms"),sw.Time()));
    CCLogger::Get()->DebugLog(F(_T("BuildTree took : %ld ms in total"),sw_total.Time()));
#endif
}

void ClassBrowserBuilderThread::RemoveInvalidNodes(CBTreeCtrl* tree, wxTreeItemId parent)
{
    if ((!::wxIsMainThread() && TestDestroy()) || Manager::IsAppShuttingDown() || (!(parent.IsOk())))
        return;

    // recursively enters all existing nodes and deletes the node if the token it references
    // is invalid (i.e. m_TokensTree->at() != token_in_data)

    // we 'll loop backwards so we can delete nodes without problems
    wxTreeItemId existing = tree->GetLastChild(parent);
    while (parent.IsOk() && existing.IsOk())
    {
        bool removeCurrent = false;
        bool hasChildren = tree->ItemHasChildren(existing);
        CBTreeData* data = (CBTreeData*)(tree->GetItemData(existing));

        if (tree == m_TreeBottom)
            removeCurrent = true;
        else if (data && data->m_Token)
        {
            Token* token = nullptr;
            {
                CC_LOCKER_TRACK_CS_ENTER(s_TokensTreeCritical)

                token = m_TokensTree->at(data->m_TokenIndex);

                CC_LOCKER_TRACK_CS_LEAVE(s_TokensTreeCritical);
            }
            if ( token != data->m_Token ||
                (data->m_Ticket && data->m_Ticket != data->m_Token->GetTicket()) ||
                !TokenMatchesFilter(data->m_Token))
            {
                removeCurrent = true;
            }
        }

        if (removeCurrent)
        {
            if (hasChildren)
                tree->DeleteChildren(existing);

            wxTreeItemId next = tree->GetPrevSibling(existing);
            if (!next.IsOk() && parent.IsOk() && tree == m_TreeTop && tree->GetChildrenCount(parent, false) == 1 )
            {
#ifndef CC_NO_COLLAPSE_ITEM
                CollapseItem(parent);
                // tree->SetItemHasChildren(parent, false);
                // existing is the last item an gets deleted in CollapseItem and at least on 64-bit linux it can
                // lead to a crash, because we use it again some lines later, but m_Item is not 0 in some rare cases,
                // and therefore IsOk returns true !!
                // so we leave the function here
#endif // CC_NO_COLLAPSE_ITEM
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
        if (existing.IsOk())
            existing = tree->GetPrevSibling(existing);
    }
}

wxTreeItemId ClassBrowserBuilderThread::AddNodeIfNotThere(CBTreeCtrl* tree, wxTreeItemId parent, const wxString& name, int imgIndex, CBTreeData* data)
{
    wxTreeItemIdValue cookie = 0;

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
        existing = tree->GetNextChild(parent, cookie);
    }
        return tree->AppendItem(parent, name, imgIndex, imgIndex, data);
}

bool ClassBrowserBuilderThread::AddChildrenOf(CBTreeCtrl* tree, wxTreeItemId parent, int parentTokenIdx, short int tokenKindMask, int tokenScopeMask)
{
    if ((!::wxIsMainThread() && TestDestroy()) || Manager::IsAppShuttingDown())
        return false;

    Token* parentToken = 0;
    bool parentTokenError = false;
    TokenIdxSet* tokens = 0;

    CC_LOCKER_TRACK_CS_ENTER(s_TokensTreeCritical)

    if (parentTokenIdx == -1)
    {
        if (m_Options.displayFilter == bdfWorkspace || m_Options.displayFilter == bdfEverything)
            tokens = &m_TokensTree->m_GlobalNameSpace;
        else
            tokens = &m_CurrentGlobalTokensSet;
    }
    else
    {
        parentToken = m_TokensTree->at(parentTokenIdx);
        if (!parentToken)
        {
            TRACE(_T("Token not found?!?"));
            parentTokenError = true;
        }
        if (!parentTokenError) tokens = &parentToken->m_Children;
    }

    CC_LOCKER_TRACK_CS_LEAVE(s_TokensTreeCritical);

    if (parentTokenError) return false;

    return AddNodes(tree, parent, *tokens, tokenKindMask, tokenScopeMask, m_Options.displayFilter == bdfEverything);
}

bool ClassBrowserBuilderThread::AddAncestorsOf(CBTreeCtrl* tree, wxTreeItemId parent, int tokenIdx)
{
    if ((!::wxIsMainThread() && TestDestroy()) || Manager::IsAppShuttingDown())
        return false;

    CC_LOCKER_TRACK_CS_ENTER(s_TokensTreeCritical)

    Token* token = m_TokensTree->at(tokenIdx);
    if (token)
        m_TokensTree->RecalcInheritanceChain(token);

    CC_LOCKER_TRACK_CS_LEAVE(s_TokensTreeCritical);

    if (!token)
        return false;

    return AddNodes(tree, parent, token->m_DirectAncestors, tkClass | tkTypedef, 0, true);
}

bool ClassBrowserBuilderThread::AddDescendantsOf(CBTreeCtrl* tree, wxTreeItemId parent, int tokenIdx, bool allowInheritance)
{
    if ((!::wxIsMainThread() && TestDestroy()) || Manager::IsAppShuttingDown())
        return false;

    CC_LOCKER_TRACK_CS_ENTER(s_TokensTreeCritical)

    Token* token = m_TokensTree->at(tokenIdx);
    if (token)
        m_TokensTree->RecalcInheritanceChain(token);

    CC_LOCKER_TRACK_CS_LEAVE(s_TokensTreeCritical);

    if (!token)
        return false;

    bool oldShowInheritance = m_Options.showInheritance;
    m_Options.showInheritance = allowInheritance;

    bool ret = AddNodes(tree, parent, token->m_Descendants, tkClass | tkTypedef, 0, true);

    m_Options.showInheritance = oldShowInheritance;
    return ret;
}

bool ClassBrowserBuilderThread::AddNodes(CBTreeCtrl* tree, wxTreeItemId parent, const TokenIdxSet& tokens,
                                         short int tokenKindMask, int tokenScopeMask, bool allowGlobals)
{
    int count = 0;
    std::set<unsigned long, std::less<unsigned long> > tickets;

    // Build temporary list of Token tickets - if the token's ticket is present
    // among the parent node's children, it's a dupe, and we'll skip it.
    if (parent.IsOk() && tree == m_TreeTop)
    {
        wxTreeItemIdValue cookie;
        wxTreeItemId curchild = tree->GetFirstChild(parent,cookie);
        while (curchild.IsOk())
        {
            CBTreeData* data = (CBTreeData*)(tree->GetItemData(curchild));
            curchild = tree->GetNextSibling(curchild);
            if (data && data->m_Ticket)
                tickets.insert(data->m_Ticket);
        }
    }

    TokenIdxSet::iterator end = tokens.end();
    for (TokenIdxSet::iterator start = tokens.begin(); start != end; ++start)
    {
        CC_LOCKER_TRACK_CS_ENTER(s_TokensTreeCritical)

        Token* token = m_TokensTree->at(*start);

        CC_LOCKER_TRACK_CS_LEAVE(s_TokensTreeCritical);

        if (token &&
            (token->m_TokenKind & tokenKindMask) &&
            (tokenScopeMask == 0 || token->m_Scope == tokenScopeMask) &&
            (allowGlobals || token->m_IsLocal ||
            TokenMatchesFilter(token)))
        {
            if (tree == m_TreeTop && tickets.find(token->GetTicket()) != tickets.end())
                continue; // dupe
            ++count;
            int img = m_NativeParser->GetTokenKindImage(token);

            wxString str = token->m_Name;
            if (   (token->m_TokenKind == tkFunction)
                || (token->m_TokenKind == tkConstructor)
                || (token->m_TokenKind == tkDestructor)
                || (token->m_TokenKind == tkMacro)
                || (token->m_TokenKind == tkClass) )
                str << token->GetFormattedArgs();
            // modification suggested by ollydbg in http://forums.codeblocks.org/index.php/topic,10242.msg70865.html#msg70865:
//            if (!token->m_ActualType.IsEmpty())
//                 str = str + _T(" : ") + token->m_ActualType;
            if (!token->m_BaseType.IsEmpty())
                 str = str + _T(" : ") + token->m_BaseType + token->m_TemplateArgument;

            wxTreeItemId child = tree->AppendItem(parent, str, img, img, new CBTreeData(sfToken, token, tokenKindMask));

            // mark as expanding if it is a container
            int kind = tkClass | tkNamespace | tkEnum;
            if (token->m_TokenKind == tkClass)
            {
                if (!m_Options.treeMembers)
                    kind |= tkTypedef | tkFunction | tkVariable | tkEnum | tkMacro;
                tree->SetItemHasChildren(child, m_Options.showInheritance || TokenContainsChildrenOfKind(token, kind));
            }
            else if (token->m_TokenKind & (tkNamespace | tkEnum))
            {
                if (!m_Options.treeMembers)
                    kind |= tkTypedef | tkFunction | tkVariable | tkEnumerator | tkMacro;
                tree->SetItemHasChildren(child, TokenContainsChildrenOfKind(token, kind));
            }
        }
    }

    tree->SortChildren(parent);
//    tree->RemoveDoubles(parent);
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F(_T("Added %d nodes"), count));
#endif
    return count != 0;
}

bool ClassBrowserBuilderThread::TokenMatchesFilter(Token* token, bool locked)
{
    if (!token || token->m_IsTemp)
        return false;

    if (    m_Options.displayFilter == bdfEverything
        || (m_Options.displayFilter == bdfWorkspace && token->m_IsLocal) )
        return true;

    if (m_Options.displayFilter == bdfFile && !m_CurrentTokenSet.empty())
    {
        if (m_CurrentTokenSet.find(token->m_Index) != m_CurrentTokenSet.end())
            return true;

        // we got to check all children of this token (recursively)
        // to see if any of them matches the filter...
        for (TokenIdxSet::iterator it = token->m_Children.begin(); it != token->m_Children.end(); ++it)
        {
            if (!locked)
                CC_LOCKER_TRACK_CS_ENTER(s_TokensTreeCritical)

            Token* token = m_TokensTree->at(*it);

            if (!locked)
                CC_LOCKER_TRACK_CS_LEAVE(s_TokensTreeCritical);

            if (!token)
                break;

            if ( TokenMatchesFilter(token, locked) )
                return true;
        }
    }
    else if (m_Options.displayFilter == bdfProject && m_UserData)
        return token->m_UserData == m_UserData;

    return false;
}

bool ClassBrowserBuilderThread::TokenContainsChildrenOfKind(Token* token, int kind)
{
    if (!token)
        return false;

    bool isOfKind = false;

    CC_LOCKER_TRACK_CS_ENTER(s_TokensTreeCritical)

    TokensTree* tt = token->GetTree();
    for (TokenIdxSet::iterator it = token->m_Children.begin(); it != token->m_Children.end(); ++it)
    {
        Token* child = tt->at(*it);
        if (child->m_TokenKind & kind)
        {
            isOfKind = true;
            break;
        }
    }

    CC_LOCKER_TRACK_CS_LEAVE(s_TokensTreeCritical);

    return isOfKind;
}

void ClassBrowserBuilderThread::AddMembersOf(CBTreeCtrl* tree, wxTreeItemId node)
{
   if ((!::wxIsMainThread() && TestDestroy()) || Manager::IsAppShuttingDown() || !node.IsOk())
        return;

    CBTreeData* data = (CBTreeData*)m_TreeTop->GetItemData(node);

    bool bottom = (tree == m_TreeBottom);
    if (bottom)
    {
#ifdef CC_BUILDTREE_MEASURING
        wxStopWatch sw;
#endif
        tree->Freeze();
#ifdef CC_BUILDTREE_MEASURING
        CCLogger::Get()->DebugLog(F(_T("tree->Freeze() took : %ld ms"),sw.Time()));
        sw.Start();
#endif
        tree->DeleteAllItems();
#ifdef CC_BUILDTREE_MEASURING
        CCLogger::Get()->DebugLog(F(_T("tree->DeleteAllItems() took : %ld ms"),sw.Time()));
        sw.Start();
#endif
        node = tree->AddRoot(_T("Members")); // not visible, so don't translate
#ifdef CC_BUILDTREE_MEASURING
        CCLogger::Get()->DebugLog(F(_T("tree->AddRoot() took : %ld ms"),sw.Time()));
#endif
    }

    wxTreeItemId firstItem;
    bool haveFirstItem = false;
    if (data)
    {
        switch (data->m_SpecialFolder)
        {
            case sfGFuncs  : AddChildrenOf(tree, node, -1, tkFunction,     false); break;
            case sfGVars   : AddChildrenOf(tree, node, -1, tkVariable,     false); break;
            case sfPreproc : AddChildrenOf(tree, node, -1, tkPreprocessor, false); break;
            case sfTypedef : AddChildrenOf(tree, node, -1, tkTypedef,      false); break;
            case sfMacro   : AddChildrenOf(tree, node, -1, tkMacro,        false); break;
            case sfToken:
            {
                if (bottom)
                {
                    if (m_Options.sortType == bstKind && !(data->m_Token->m_TokenKind & tkEnum))
                    {
                        wxTreeItemId rootCtorDtor = tree->AppendItem(node, _("Ctors & Dtors"), PARSER_IMG_CLASS_FOLDER);
                        wxTreeItemId rootFuncs    = tree->AppendItem(node, _("Functions"), PARSER_IMG_FUNCS_FOLDER);
                        wxTreeItemId rootVars     = tree->AppendItem(node, _("Variables"), PARSER_IMG_VARS_FOLDER);
                        wxTreeItemId rootMacro    = tree->AppendItem(node, _("Macros"), PARSER_IMG_MACRO_FOLDER);
                        wxTreeItemId rootOthers   = tree->AppendItem(node, _("Others"), PARSER_IMG_OTHERS_FOLDER);

                        AddChildrenOf(tree, rootCtorDtor, data->m_Token->m_Index, tkConstructor | tkDestructor);
                        AddChildrenOf(tree, rootFuncs,    data->m_Token->m_Index, tkFunction);
                        AddChildrenOf(tree, rootVars,     data->m_Token->m_Index, tkVariable);
                        AddChildrenOf(tree, rootMacro,    data->m_Token->m_Index, tkMacro);
                        AddChildrenOf(tree, rootOthers,   data->m_Token->m_Index, ~(tkNamespace | tkClass | tkEnum | tkAnyFunction | tkVariable | tkMacro));

                        firstItem = rootCtorDtor;
                    }
                    else if (m_Options.sortType == bstScope && data->m_Token->m_TokenKind & tkClass)
                    {
                        wxTreeItemId rootPublic    = tree->AppendItem(node, _("Public"), PARSER_IMG_CLASS_FOLDER);
                        wxTreeItemId rootProtected = tree->AppendItem(node, _("Protected"), PARSER_IMG_FUNCS_FOLDER);
                        wxTreeItemId rootPrivate   = tree->AppendItem(node, _("Private"), PARSER_IMG_VARS_FOLDER);

                        AddChildrenOf(tree, rootPublic,    data->m_Token->m_Index, ~(tkNamespace | tkClass | tkEnum), tsPublic);
                        AddChildrenOf(tree, rootProtected, data->m_Token->m_Index, ~(tkNamespace | tkClass | tkEnum), tsProtected);
                        AddChildrenOf(tree, rootPrivate,   data->m_Token->m_Index, ~(tkNamespace | tkClass | tkEnum), tsPrivate);

                        firstItem = rootPublic;
                    }
                    else
                    {
                        AddChildrenOf(tree, node, data->m_Token->m_Index, ~(tkNamespace | tkClass | tkEnum));
                        break;
                    }

                    wxTreeItemId existing = tree->GetLastChild(tree->GetRootItem());
                    while (existing.IsOk())
                    {
                        wxTreeItemId next = tree->GetPrevSibling(existing);

                        if (tree->GetChildrenCount(existing) > 0)
                        {
                            tree->SetItemBold(existing, true);
                            // make existing the firstItem, because the former firstItem might get deleted
                            // in the else-clause, if it has no children, what can lead to a crash
                            firstItem = existing;
                            // needed, if no child remains, because firstItem IsOk() returns true anyway
                            // in some cases.
                            haveFirstItem = true;
                        }
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
                    AddChildrenOf(tree, node, data->m_Token->m_Index, ~(tkNamespace | tkClass | tkEnum));

                // add all children, except containers
                // AddChildrenOf(tree, node, data->m_Token->GetSelf(), ~(tkNamespace | tkClass | tkEnum));
                break;
            }
            default: break;
        }
    }

    if (bottom)
    {
        tree->ExpandAll();
        if (haveFirstItem && firstItem.IsOk())
        {
            tree->ScrollTo(firstItem);
            tree->EnsureVisible(firstItem);
        }
        tree->Thaw();
    }
}

// checks if there are respective children and colors the nodes
bool ClassBrowserBuilderThread::CreateSpecialFolders(CBTreeCtrl* tree, wxTreeItemId parent)
{
    bool hasGF = false;
    bool hasGV = false;
    bool hasGP = false;
    bool hasTD = false;
    bool hasGM = false;

    // loop all tokens in global namespace and see if we have matches
    TokensTree* tt = m_NativeParser->GetParser().GetTokensTree();
    {
        CC_LOCKER_TRACK_CS_ENTER(s_TokensTreeCritical)

        for (TokenIdxSet::iterator it = tt->m_GlobalNameSpace.begin(); it != tt->m_GlobalNameSpace.end(); ++it)
        {
            Token* token = tt->at(*it);
            if (token && token->m_IsLocal && TokenMatchesFilter(token, true))
            {
                if      (!hasGF && token->m_TokenKind == tkFunction)
                    hasGF = true;
                else if (!hasGM && token->m_TokenKind == tkMacro)
                    hasGM = true;
                else if (!hasGV && token->m_TokenKind == tkVariable)
                    hasGV = true;
                else if (!hasGP && token->m_TokenKind == tkPreprocessor)
                    hasGP = true;
                else if (!hasTD && token->m_TokenKind == tkTypedef)
                    hasTD = true;
            }

            if (hasGF && hasGV && hasGP && hasTD && hasGM)
                break; // we have everything, stop iterating...
        }

        CC_LOCKER_TRACK_CS_LEAVE(s_TokensTreeCritical);
    }

    wxTreeItemId gfuncs  = AddNodeIfNotThere(m_TreeTop, parent, _("Global functions"),
                           PARSER_IMG_FUNCS_FOLDER,   new CBTreeData(sfGFuncs,  0, tkFunction,     -1));
    wxTreeItemId tdef    = AddNodeIfNotThere(m_TreeTop, parent, _("Global typedefs"),
                           PARSER_IMG_TYPEDEF_FOLDER, new CBTreeData(sfTypedef, 0, tkTypedef,      -1));
    wxTreeItemId gvars   = AddNodeIfNotThere(m_TreeTop, parent, _("Global variables"),
                           PARSER_IMG_VARS_FOLDER,    new CBTreeData(sfGVars,   0, tkVariable,     -1));
    wxTreeItemId preproc = AddNodeIfNotThere(m_TreeTop, parent, _("Preprocessor symbols"),
                           PARSER_IMG_PREPROC_FOLDER, new CBTreeData(sfPreproc, 0, tkPreprocessor, -1));
    wxTreeItemId gmacro  = AddNodeIfNotThere(m_TreeTop, parent, _("Global macros"),
                           PARSER_IMG_MACRO_FOLDER,   new CBTreeData(sfMacro,   0, tkMacro,        -1));

    bool bottom = m_Options.treeMembers;
    m_TreeTop->SetItemHasChildren(gfuncs,  !bottom && hasGF);
    m_TreeTop->SetItemHasChildren(tdef,    !bottom && hasTD);
    m_TreeTop->SetItemHasChildren(gvars,   !bottom && hasGV);
    m_TreeTop->SetItemHasChildren(preproc, !bottom && hasGP);
    m_TreeTop->SetItemHasChildren(gmacro,  !bottom && hasGM);

    wxColour black = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    wxColour grey  = wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT);

    tree->SetItemTextColour(gfuncs,  hasGF ? black : grey);
    tree->SetItemTextColour(gvars,   hasGV ? black : grey);
    tree->SetItemTextColour(preproc, hasGP ? black : grey);
    tree->SetItemTextColour(tdef,    hasTD ? black : grey);
    tree->SetItemTextColour(gmacro,  hasGM ? black : grey);

    return hasGF || hasGV || hasGP || hasTD || hasGM;
}

void ClassBrowserBuilderThread::ExpandItem(wxTreeItemId item)
{
    if ((!::wxIsMainThread() && TestDestroy()) || Manager::IsAppShuttingDown())
        return;

    bool locked = false;
    if (m_InitDone)
    {
        CC_LOCKER_TRACK_MTX_LOCK(m_ClassBrowserBuilderThreadMutex)
        locked = true;
    }

#ifdef CC_BUILDTREE_MEASURING
    wxStopWatch sw;
#endif

    CC_LOCKER_TRACK_CS_ENTER(s_TokensTreeCritical)

    CBTreeData* data = (CBTreeData*)m_TreeTop->GetItemData(item);
    m_TokensTree->RecalcInheritanceChain(data->m_Token);

    CC_LOCKER_TRACK_CS_LEAVE(s_TokensTreeCritical);

    if (data)
    {
        switch (data->m_SpecialFolder)
        {
            case sfRoot:
            {
                CreateSpecialFolders(m_TreeTop, item);
                if( ! (m_Options.displayFilter == bdfFile  && m_ActiveFilename.IsEmpty()))
                    AddChildrenOf(m_TreeTop, item, -1, ~(tkFunction | tkVariable | tkPreprocessor | tkTypedef | tkMacro));
                break;
            }
            case sfBase: AddAncestorsOf(m_TreeTop, item, data->m_Token->m_Index); break;
            case sfDerived: AddDescendantsOf(m_TreeTop, item, data->m_Token->m_Index, false); break;
            case sfToken:
            {
                short int kind = 0;
                switch (data->m_Token->m_TokenKind)
                {
                    case tkClass:
                    {
                        // add base and derived classes folders
                        if (m_Options.showInheritance)
                        {
                            wxTreeItemId base = m_TreeTop->AppendItem(item, _("Base classes"),
                                                PARSER_IMG_CLASS_FOLDER, PARSER_IMG_CLASS_FOLDER,
                                                new CBTreeData(sfBase, data->m_Token, tkClass, data->m_Token->m_Index));
                            if (!data->m_Token->m_DirectAncestors.empty())
                                m_TreeTop->SetItemHasChildren(base);
                            wxTreeItemId derived = m_TreeTop->AppendItem(item, _("Derived classes"),
                                                   PARSER_IMG_CLASS_FOLDER, PARSER_IMG_CLASS_FOLDER,
                                                   new CBTreeData(sfDerived, data->m_Token, tkClass, data->m_Token->m_Index));
                            if (!data->m_Token->m_Descendants.empty())
                                m_TreeTop->SetItemHasChildren(derived);
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
                    AddChildrenOf(m_TreeTop, item, data->m_Token->m_Index, kind);
                break;
            }
            default: break;
        }
    }

    if (m_NativeParser && !m_Options.treeMembers)
        AddMembersOf(m_TreeTop, item);
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F(_T("ExpandItems (internally) took : %ld ms for %d items."),sw.Time(),m_TreeTop->GetCount()));
#endif

    if (locked)
        CC_LOCKER_TRACK_MTX_UNLOCK(m_ClassBrowserBuilderThreadMutex)
}

#ifndef CC_NO_COLLAPSE_ITEM
void ClassBrowserBuilderThread::CollapseItem(wxTreeItemId item)
{
    if ((!::wxIsMainThread() && TestDestroy()) || Manager::IsAppShuttingDown())
        return;

    bool locked = false;
    if (m_InitDone)
    {
        CC_LOCKER_TRACK_MTX_LOCK(m_ClassBrowserBuilderThreadMutex)
        locked = true;
    }

#ifndef __WXGTK__
    m_TreeTop->CollapseAndReset(item); // this freezes gtk
#else
    m_TreeTop->DeleteChildren(item);
#endif
    m_TreeTop->SetItemHasChildren(item);

    if (locked)
        CC_LOCKER_TRACK_MTX_UNLOCK(m_ClassBrowserBuilderThreadMutex)
}
#endif // CC_NO_COLLAPSE_ITEM

void ClassBrowserBuilderThread::SelectItem(wxTreeItemId item)
{
    if ((!::wxIsMainThread() && TestDestroy()) || Manager::IsAppShuttingDown())
        return;

    CC_LOCKER_TRACK_MTX_LOCK(m_ClassBrowserBuilderThreadMutex)

#ifdef CC_BUILDTREE_MEASURING
    wxStopWatch sw;
#endif

    CBTreeCtrl* tree = (m_Options.treeMembers) ? m_TreeBottom : m_TreeTop;
    if ( !(m_Options.displayFilter == bdfFile && m_ActiveFilename.IsEmpty()))
        AddMembersOf(tree, item);

#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F(_T("SelectItem (internally) took : %ld ms"),sw.Time()));
#endif

    CC_LOCKER_TRACK_MTX_UNLOCK(m_ClassBrowserBuilderThreadMutex)
}

void ClassBrowserBuilderThread::SaveExpandedItems(CBTreeCtrl* tree, wxTreeItemId parent, int level)
{
    if ((!::wxIsMainThread() && TestDestroy()) || Manager::IsAppShuttingDown())
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

void ClassBrowserBuilderThread::ExpandSavedItems(CBTreeCtrl* tree, wxTreeItemId parent, int level)
{
    if ((!::wxIsMainThread() && TestDestroy()) || Manager::IsAppShuttingDown())
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

    // remove non-existing by now saved items
    while (!m_ExpandedVect.empty() && m_ExpandedVect.front().GetLevel() > level)
        m_ExpandedVect.pop_front();
}

void ClassBrowserBuilderThread::SaveSelectedItem()
{
    if ((!::wxIsMainThread() && TestDestroy()) || Manager::IsAppShuttingDown())
        return;

    m_SelectedPath.clear();

    wxTreeItemId item = m_TreeTop->GetSelection();
    while (item.IsOk() && item != m_TreeTop->GetRootItem())
    {
        CBTreeData* data = (CBTreeData*)m_TreeTop->GetItemData(item);
        m_SelectedPath.push_front(*data);

        item = m_TreeTop->GetItemParent(item);
    }
}

void ClassBrowserBuilderThread::SelectSavedItem()
{
    if ((!::wxIsMainThread() && TestDestroy()) || Manager::IsAppShuttingDown())
        return;

    wxTreeItemId parent = m_TreeTop->GetRootItem();

    wxTreeItemIdValue cookie;
    wxTreeItemId item = m_TreeTop->GetFirstChild(parent, cookie);

    while (!m_SelectedPath.empty() && item.IsOk())
    {
        CBTreeData* data = (CBTreeData*)m_TreeTop->GetItemData(item);
        CBTreeData* saved = &m_SelectedPath.front();

        if (data->m_SpecialFolder == saved->m_SpecialFolder &&
            wxStrcmp(data->m_TokenName, saved->m_TokenName) == 0 &&
            data->m_TokenKind == saved->m_TokenKind)
        {
            wxTreeItemIdValue cookie;
            parent = item;
            item = m_TreeTop->GetFirstChild(item, cookie);
            m_SelectedPath.pop_front();
        }
        else
            item = m_TreeTop->GetNextSibling(item);
    }

    m_SelectItemRequired = parent;

    wxCommandEvent event(wxEVT_COMMAND_ENTER, m_idCBMakeSelectItem);
    m_TreeTop->GetEventHandler()->AddPendingEvent(event);
    m_SelectedPath.clear();
}

void ClassBrowserBuilderThread::SelectItemRequired()
{
    if (Manager::IsAppShuttingDown())
        return;

    if (m_SelectItemRequired.IsOk())
    {
        m_TreeTop->SelectItem(m_SelectItemRequired);
        m_TreeTop->EnsureVisible(m_SelectItemRequired);
    }
}

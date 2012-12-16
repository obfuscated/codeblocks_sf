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

#define CBBT_SANITY_CHECK ((!::wxIsMainThread() && m_TerminationRequested) || Manager::IsAppShuttingDown())

#define CC_BUILDERTHREAD_DEBUG_OUTPUT 0

#if defined(CC_GLOBAL_DEBUG_OUTPUT)
    #if CC_GLOBAL_DEBUG_OUTPUT == 1
        #undef CC_BUILDERTHREAD_DEBUG_OUTPUT
        #define CC_BUILDERTHREAD_DEBUG_OUTPUT 1
    #elif CC_GLOBAL_DEBUG_OUTPUT == 2
        #undef CC_BUILDERTHREAD_DEBUG_OUTPUT
        #define CC_BUILDERTHREAD_DEBUG_OUTPUT 2
    #endif
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

ClassBrowserBuilderThread::ClassBrowserBuilderThread(wxEvtHandler* evtHandler, wxSemaphore& sem) :
    wxThread(wxTHREAD_JOINABLE),
    m_Parent(evtHandler),
    m_ClassBrowserSemaphore(sem),
    m_ClassBrowserBuilderThreadMutex(),
    m_NativeParser(0),
    m_CCTreeCtrlTop(0),
    m_CCTreeCtrlBottom(0),
    m_UserData(0),
    m_BrowserOptions(),
    m_TokenTree(0),
    m_InitDone(false),
    m_TerminationRequested(false)

{
}

ClassBrowserBuilderThread::~ClassBrowserBuilderThread()
{
}

void ClassBrowserBuilderThread::Init(NativeParser*         np,
                                     CCTreeCtrl*           treeTop,
                                     CCTreeCtrl*           treeBottom,
                                     const wxString&       active_filename,
                                     void*                 user_data, // active project
                                     const BrowserOptions& bo,
                                     TokenTree*            tt,
                                     int                   idThreadEvent)
{
    TRACE(_T("ClassBrowserBuilderThread::Init"));

    CC_LOCKER_TRACK_CBBT_MTX_LOCK(m_ClassBrowserBuilderThreadMutex);

    m_NativeParser     = np;
    m_CCTreeCtrlTop    = treeTop;
    m_CCTreeCtrlBottom = treeBottom;
    m_ActiveFilename   = active_filename;
    m_UserData         = user_data;
    m_BrowserOptions   = bo;
    m_TokenTree        = tt;
    m_idThreadEvent    = idThreadEvent;

    m_CurrentFileSet.clear();
    m_CurrentTokenSet.clear();

    TokenTree* tree = m_NativeParser->GetParser().GetTokenTree();

    // fill filter set for current-file-filter
    if (   m_BrowserOptions.displayFilter == bdfFile
        && !m_ActiveFilename.IsEmpty() )
    {
        // m_ActiveFilename is the full filename up to the extension dot. No extension though.
        // get all filenames' indices matching our mask
        wxArrayString paths = m_NativeParser->GetAllPathsByFilename(m_ActiveFilename);

        // Should add locker after called m_NativeParser->GetAllPathsByFilename
        CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

        TokenFileSet result;
        for (size_t i = 0; i < paths.GetCount(); ++i)
        {
            tree->GetFileMatches(paths[i], result, true, true);
            for (TokenFileSet::const_iterator tfs_it = result.begin(); tfs_it != result.end(); ++tfs_it)
                m_CurrentFileSet.insert(*tfs_it);
        }

        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)
    }
    else if (   m_BrowserOptions.displayFilter == bdfProject
             && m_UserData )
    {
        CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

        cbProject* prj = static_cast<cbProject*>(m_UserData);
        for (FilesList::const_iterator fl_it = prj->GetFilesList().begin();
                                       fl_it != prj->GetFilesList().end(); ++fl_it)
        {
            ProjectFile* curFile = *fl_it;
            if (!curFile)
                continue;

            const size_t fileIdx = tree->GetFileIndex(curFile->file.GetFullPath());
            if (fileIdx)
                m_CurrentFileSet.insert(fileIdx);
        }

        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)
    }

    if (!m_CurrentFileSet.empty())
    {
        CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

        m_CurrentTokenSet.clear();
        m_CurrentGlobalTokensSet.clear();
        for (TokenFileSet::const_iterator itf = m_CurrentFileSet.begin(); itf != m_CurrentFileSet.end(); ++itf)
        {
            const TokenIdxSet* tokens = tree->GetTokensBelongToFile(*itf);
            if (!tokens) continue;

            // loop tokens in file
            for (TokenIdxSet::const_iterator its = tokens->begin(); its != tokens->end(); ++its)
            {
                Token* curToken = tree->at(*its);
                if (curToken)
                {
                    m_CurrentTokenSet.insert(*its);
                    if (curToken->m_ParentIndex == -1)
                        m_CurrentGlobalTokensSet.insert(*its);
                }
            }
        }

        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)
    }

    CC_LOCKER_TRACK_CBBT_MTX_UNLOCK(m_ClassBrowserBuilderThreadMutex)
}

// Thread function

void* ClassBrowserBuilderThread::Entry()
{
    while (!m_TerminationRequested && !Manager::IsAppShuttingDown() )
    {
        // waits here, until the ClassBrowser unlocks
        m_ClassBrowserSemaphore.Wait();

        if (m_TerminationRequested || Manager::IsAppShuttingDown() )
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
    m_CCTreeCtrlTop = 0;
    m_CCTreeCtrlBottom = 0;

    return 0;
}

// Functions accessible from outside

void ClassBrowserBuilderThread::ExpandItem(wxTreeItemId item)
{
    TRACE(_T("ClassBrowserBuilderThread::ExpandItem"));

    if (CBBT_SANITY_CHECK || !item.IsOk())
        return;

    bool locked = false;
    if (m_InitDone)
    {
        CC_LOCKER_TRACK_CBBT_MTX_LOCK(m_ClassBrowserBuilderThreadMutex)
        locked = true;
    }

#ifdef CC_BUILDTREE_MEASURING
    wxStopWatch sw;
#endif

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    CCTreeCtrlData* data = static_cast<CCTreeCtrlData*>(m_CCTreeCtrlTop->GetItemData(item));
    m_TokenTree->RecalcInheritanceChain(data->m_Token);

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    if (data)
    {
        switch (data->m_SpecialFolder)
        {
            case sfRoot:
            {
                CreateSpecialFolders(m_CCTreeCtrlTop, item);
                if( !(   m_BrowserOptions.displayFilter == bdfFile
                      && m_ActiveFilename.IsEmpty() ) )
                    AddChildrenOf(m_CCTreeCtrlTop, item, -1, ~(tkFunction | tkVariable | tkPreprocessor | tkTypedef | tkMacro));
                break;
            }
            case sfBase:    AddAncestorsOf(m_CCTreeCtrlTop, item, data->m_Token->m_Index); break;
            case sfDerived: AddDescendantsOf(m_CCTreeCtrlTop, item, data->m_Token->m_Index, false); break;
            case sfToken:
            {
                short int kind = 0;
                switch (data->m_Token->m_TokenKind)
                {
                    case tkClass:
                    {
                        // add base and derived classes folders
                        if (m_BrowserOptions.showInheritance)
                        {
                            wxTreeItemId base = m_CCTreeCtrlTop->AppendItem(item, _("Base classes"),
                                                PARSER_IMG_CLASS_FOLDER, PARSER_IMG_CLASS_FOLDER,
                                                new CCTreeCtrlData(sfBase, data->m_Token, tkClass, data->m_Token->m_Index));
                            if (!data->m_Token->m_DirectAncestors.empty())
                                m_CCTreeCtrlTop->SetItemHasChildren(base);
                            wxTreeItemId derived = m_CCTreeCtrlTop->AppendItem(item, _("Derived classes"),
                                                   PARSER_IMG_CLASS_FOLDER, PARSER_IMG_CLASS_FOLDER,
                                                   new CCTreeCtrlData(sfDerived, data->m_Token, tkClass, data->m_Token->m_Index));
                            if (!data->m_Token->m_Descendants.empty())
                                m_CCTreeCtrlTop->SetItemHasChildren(derived);
                        }
                        kind = tkClass | tkEnum;
                        break;
                    }
                    case tkNamespace:
                        kind = tkNamespace | tkClass | tkEnum;
                        break;
                    case tkEnum:
                    case tkTypedef:
                    case tkConstructor:
                    case tkDestructor:
                    case tkFunction:
                    case tkVariable:
                    case tkEnumerator:
                    case tkPreprocessor:
                    case tkMacro:
                    case tkAnyContainer:
                    case tkAnyFunction:
                    case tkUndefined:
                    default:
                        break;
                }
                if (kind != 0)
                    AddChildrenOf(m_CCTreeCtrlTop, item, data->m_Token->m_Index, kind);
                break;
            }
            case sfGFuncs:
            case sfGVars:
            case sfPreproc:
            case sfTypedef:
            case sfMacro:
            default:
                break;
        }
    }

    if (m_NativeParser && !m_BrowserOptions.treeMembers)
        AddMembersOf(m_CCTreeCtrlTop, item);
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F(_T("ExpandItems (internally) took : %ld ms for %u items."),sw.Time(),m_CCTreeCtrlTop->GetCount()));
#endif

    if (locked)
        CC_LOCKER_TRACK_CBBT_MTX_UNLOCK(m_ClassBrowserBuilderThreadMutex)
}

#ifndef CC_NO_COLLAPSE_ITEM
void ClassBrowserBuilderThread::CollapseItem(wxTreeItemId item)
{
    TRACE(_T("ClassBrowserBuilderThread::CollapseItem"));

    if (CBBT_SANITY_CHECK || !item.IsOk())
        return;

    bool locked = false;
    if (m_InitDone)
    {
        CC_LOCKER_TRACK_CBBT_MTX_LOCK(m_ClassBrowserBuilderThreadMutex)
        locked = true;
    }

#ifndef __WXGTK__
    m_CCTreeCtrlTop->CollapseAndReset(item); // this freezes gtk
#else
    m_CCTreeCtrlTop->DeleteChildren(item);
#endif
    m_CCTreeCtrlTop->SetItemHasChildren(item);

    if (locked)
        CC_LOCKER_TRACK_CBBT_MTX_UNLOCK(m_ClassBrowserBuilderThreadMutex)
}
#endif // CC_NO_COLLAPSE_ITEM

void ClassBrowserBuilderThread::SelectItem(wxTreeItemId item)
{
    TRACE(_T("ClassBrowserBuilderThread::SelectItem"));

    if (CBBT_SANITY_CHECK || !item.IsOk())
        return;

    CC_LOCKER_TRACK_CBBT_MTX_LOCK(m_ClassBrowserBuilderThreadMutex)

#ifdef CC_BUILDTREE_MEASURING
    wxStopWatch sw;
#endif

    CCTreeCtrl* tree = (m_BrowserOptions.treeMembers) ? m_CCTreeCtrlBottom : m_CCTreeCtrlTop;
    if ( !(   m_BrowserOptions.displayFilter == bdfFile
           && m_ActiveFilename.IsEmpty() ) )
        AddMembersOf(tree, item);

#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F(_T("SelectItem (internally) took : %ld ms"),sw.Time()));
#endif

    CC_LOCKER_TRACK_CBBT_MTX_UNLOCK(m_ClassBrowserBuilderThreadMutex)
}

void ClassBrowserBuilderThread::SelectItemRequired()
{
    if (Manager::IsAppShuttingDown())
        return;

    if (m_SelectItemRequired.IsOk())
    {
        m_CCTreeCtrlTop->SelectItem(m_SelectItemRequired);
        m_CCTreeCtrlTop->EnsureVisible(m_SelectItemRequired);
    }
}

// Main worker functions

void ClassBrowserBuilderThread::BuildTree()
{
    if (CBBT_SANITY_CHECK || !m_CCTreeCtrlTop || !m_CCTreeCtrlBottom || !m_NativeParser)
        return; // Called before UI tree construction completed?!

    wxCommandEvent e1(wxEVT_COMMAND_ENTER, m_idThreadEvent);
    e1.SetInt(buildTreeStart);
    m_Parent->AddPendingEvent(e1);

#ifdef CC_BUILDTREE_MEASURING
    wxStopWatch sw;
    wxStopWatch sw_total;
#endif
    // 1.) Registration of images
    m_CCTreeCtrlTop->SetImageList(m_NativeParser->GetImageList());
    m_CCTreeCtrlBottom->SetImageList(m_NativeParser->GetImageList());

    // 2.) Create initial root node, if not already there
    wxTreeItemId root = m_CCTreeCtrlTop->GetRootItem();
    if (!root.IsOk())
    {
        root = m_CCTreeCtrlTop->AddRoot(_("Symbols"), PARSER_IMG_SYMBOLS_FOLDER, PARSER_IMG_SYMBOLS_FOLDER, new CCTreeCtrlData(sfRoot));
        m_CCTreeCtrlTop->SetItemHasChildren(root);
    }

    // 3.) Update compare functions
    m_CCTreeCtrlTop->SetCompareFunction(m_BrowserOptions.sortType);
    m_CCTreeCtrlBottom->SetCompareFunction(m_BrowserOptions.sortType);

    // 4.) Save expanded items to restore later
    m_ExpandedVect.clear();
    SaveExpandedItems(m_CCTreeCtrlTop, root, 0);
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F(_T("Saving expanded items took : %ld ms"),sw.Time()));
    sw.Start();
#endif

    // 5.) Save selected item to restore later
    SaveSelectedItem();
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F(_T("Saving selected items took : %ld ms"),sw.Time()));
    sw.Start();
#endif

    // 6.) Hide&Freeze trees shown
    if (m_BrowserOptions.treeMembers)
    {
        m_CCTreeCtrlBottom->Hide();
        m_CCTreeCtrlBottom->Freeze();
    }
    m_CCTreeCtrlTop->Hide();
    m_CCTreeCtrlTop->Freeze();
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F(_T("Hiding and freezing trees took : %ld ms"),sw.Time()));
    sw.Start();
#endif

    // 7.) Remove any nodes no longer valid (due to update)
    RemoveInvalidNodes(m_CCTreeCtrlTop, root);
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F(_T("Removing invalid nodes (top tree) took : %ld ms"),sw.Time()));
    sw.Start();
#endif

    if (m_BrowserOptions.treeMembers)
    {
        RemoveInvalidNodes(m_CCTreeCtrlBottom, m_CCTreeCtrlBottom->GetRootItem());
#ifdef CC_BUILDTREE_MEASURING
        CCLogger::Get()->DebugLog(F(_T("Removing invalid nodes (bottom tree) took : %ld ms"),sw.Time()));
        sw.Start();
#endif
    }

    // Meanwhile, C::B might want to shutdown?!
    if (CBBT_SANITY_CHECK)
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

    // 8.) Collapse item
    CollapseItem(root);
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F(_T("Collapsing root item took : %ld ms"),sw.Time()));
    sw.Start();
#endif

    // 9.) Expand item --> Bottleneck: Takes ~4 secs on C::B workspace
    m_CCTreeCtrlTop->Expand(root);
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

    // 10.) Expand the items saved before
    ExpandSavedItems(m_CCTreeCtrlTop, root, 0);
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F(_T("Expanding saved items took : %ld ms"),sw.Time()));
    sw.Start();
#endif

    // 11.) Select the item saved before --> Bottleneck: Takes ~4 secs on C::B workspace
    SelectSavedItem();
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F(_T("Selecting saved item took : %ld ms"),sw.Time()));
    sw.Start();
#endif

    // 12.) Show the bottom tree again (it's finished)
    if (m_BrowserOptions.treeMembers)
    {
        m_CCTreeCtrlBottom->Thaw();
#ifdef CC_BUILDTREE_MEASURING
        CCLogger::Get()->DebugLog(F(_T("Thaw bottom tree took : %ld ms"),sw.Time()));
        sw.Start();
#endif

        m_CCTreeCtrlBottom->Show();
#ifdef CC_BUILDTREE_MEASURING
        CCLogger::Get()->DebugLog(F(_T("Showing bottom tree took : %ld ms"),sw.Time()));
        sw.Start();
#endif
    }

    // 13.) Expand namespaces
    ExpandNamespaces(m_CCTreeCtrlTop->GetRootItem());
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F(_T("Expanding namespaces took : %ld ms"),sw.Time()));
    sw.Start();
#endif

    m_CCTreeCtrlTop->Thaw();
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F(_T("Thaw top tree took : %ld ms"),sw.Time()));
    sw.Start();
#endif

    // 14.) Show the top tree again (it's finished) --> Bottleneck: Takes ~4 secs on C::B workspace:
    m_CCTreeCtrlTop->Show();
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F(_T("Show top tree took : %ld ms"),sw.Time()));
    CCLogger::Get()->DebugLog(F(_T("BuildTree took : %ld ms in total"),sw_total.Time()));
#endif

    // Initialisation is done after Init() and at least *one* call to BuildTree().
    // Also, in Init() m_InitDone is set to false, directly followed by a
    // re-launch of the thread resulting in a call to BuildTree() due to
    // posting the semaphore from ClassBrowser.
    m_InitDone = true;

    wxCommandEvent e2(wxEVT_COMMAND_ENTER, m_idThreadEvent);
    e2.SetInt(buildTreeEnd);
    m_Parent->AddPendingEvent(e2);
}

void ClassBrowserBuilderThread::RemoveInvalidNodes(CCTreeCtrl* tree, wxTreeItemId parent)
{
    TRACE(_T("ClassBrowserBuilderThread::RemoveInvalidNodes"));

    if (CBBT_SANITY_CHECK || !parent.IsOk())
        return;

    // recursively enters all existing nodes and deletes the node if the token it references
    // is invalid (i.e. m_TokenTree->at() != token_in_data)

    // we 'll loop backwards so we can delete nodes without problems
    wxTreeItemId existing = tree->GetLastChild(parent);
    while (parent.IsOk() && existing.IsOk())
    {
        bool removeCurrent = false;
        bool hasChildren = tree->ItemHasChildren(existing);
        CCTreeCtrlData* data = static_cast<CCTreeCtrlData*>(tree->GetItemData(existing));

        if (tree == m_CCTreeCtrlBottom)
            removeCurrent = true;
        else if (data && data->m_Token)
        {
            const Token* token = nullptr;
            {
                CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

                token = m_TokenTree->at(data->m_TokenIndex);

                CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)
            }
            if (    token != data->m_Token
                || (data->m_Ticket && data->m_Ticket != data->m_Token->GetTicket())
                || !TokenMatchesFilter(data->m_Token) )
            {
                removeCurrent = true;
            }
        }

        if (removeCurrent)
        {
            if (hasChildren)
                tree->DeleteChildren(existing);

            wxTreeItemId next = tree->GetPrevSibling(existing);
            if (!next.IsOk() && parent.IsOk() && tree == m_CCTreeCtrlTop && tree->GetChildrenCount(parent, false) == 1 )
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
            RemoveInvalidNodes(tree, existing); // re-curse

        if (existing.IsOk())
            existing = tree->GetPrevSibling(existing);
    }
}

void ClassBrowserBuilderThread::ExpandNamespaces(wxTreeItemId node)
{
    TRACE(_T("ClassBrowserBuilderThread::ExpandNamespaces"));

    if (CBBT_SANITY_CHECK || !m_BrowserOptions.expandNS || !node.IsOk())
        return;

    wxTreeItemIdValue enumerationCookie;
    wxTreeItemId existing = m_CCTreeCtrlTop->GetFirstChild(node, enumerationCookie);
    while (existing.IsOk())
    {
        CCTreeCtrlData* data = static_cast<CCTreeCtrlData*>(m_CCTreeCtrlTop->GetItemData(existing));
        if (   data
            && data->m_Token
            && data->m_Token->m_TokenKind == tkNamespace )
        {
            TRACE(F(_T("Auto-expanding: ") + data->m_Token->m_Name));
            m_CCTreeCtrlTop->Expand(existing);
            ExpandNamespaces(existing); // re-curse
        }

        existing = m_CCTreeCtrlTop->GetNextSibling(existing);
    }
}

// checks if there are respective children and colours the nodes
bool ClassBrowserBuilderThread::CreateSpecialFolders(CCTreeCtrl* tree, wxTreeItemId parent)
{
    TRACE(_T("ClassBrowserBuilderThread::CreateSpecialFolders"));

    bool hasGF = false;
    bool hasGV = false;
    bool hasGP = false;
    bool hasTD = false;
    bool hasGM = false;

    // loop all tokens in global namespace and see if we have matches
    TokenTree* tt = m_NativeParser->GetParser().GetTokenTree();

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    const TokenIdxSet* tis = tt->GetGlobalNameSpaces();
    for (TokenIdxSet::const_iterator tis_it = tis->begin(); tis_it != tis->end(); ++tis_it)
    {
        const Token* token = tt->at(*tis_it);
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

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    wxTreeItemId gfuncs  = AddNodeIfNotThere(m_CCTreeCtrlTop, parent, _("Global functions"),
                           PARSER_IMG_FUNCS_FOLDER,   new CCTreeCtrlData(sfGFuncs,  0, tkFunction,     -1));
    wxTreeItemId tdef    = AddNodeIfNotThere(m_CCTreeCtrlTop, parent, _("Global typedefs"),
                           PARSER_IMG_TYPEDEF_FOLDER, new CCTreeCtrlData(sfTypedef, 0, tkTypedef,      -1));
    wxTreeItemId gvars   = AddNodeIfNotThere(m_CCTreeCtrlTop, parent, _("Global variables"),
                           PARSER_IMG_VARS_FOLDER,    new CCTreeCtrlData(sfGVars,   0, tkVariable,     -1));
    wxTreeItemId preproc = AddNodeIfNotThere(m_CCTreeCtrlTop, parent, _("Preprocessor symbols"),
                           PARSER_IMG_PREPROC_FOLDER, new CCTreeCtrlData(sfPreproc, 0, tkPreprocessor, -1));
    wxTreeItemId gmacro  = AddNodeIfNotThere(m_CCTreeCtrlTop, parent, _("Global macros"),
                           PARSER_IMG_MACRO_FOLDER,   new CCTreeCtrlData(sfMacro,   0, tkMacro,        -1));

    bool bottom = m_BrowserOptions.treeMembers;
    m_CCTreeCtrlTop->SetItemHasChildren(gfuncs,  !bottom && hasGF);
    m_CCTreeCtrlTop->SetItemHasChildren(tdef,    !bottom && hasTD);
    m_CCTreeCtrlTop->SetItemHasChildren(gvars,   !bottom && hasGV);
    m_CCTreeCtrlTop->SetItemHasChildren(preproc, !bottom && hasGP);
    m_CCTreeCtrlTop->SetItemHasChildren(gmacro,  !bottom && hasGM);

    wxColour black = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    wxColour grey  = wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT);

    tree->SetItemTextColour(gfuncs,  hasGF ? black : grey);
    tree->SetItemTextColour(gvars,   hasGV ? black : grey);
    tree->SetItemTextColour(preproc, hasGP ? black : grey);
    tree->SetItemTextColour(tdef,    hasTD ? black : grey);
    tree->SetItemTextColour(gmacro,  hasGM ? black : grey);

    return hasGF || hasGV || hasGP || hasTD || hasGM;
}

wxTreeItemId ClassBrowserBuilderThread::AddNodeIfNotThere(CCTreeCtrl* tree, wxTreeItemId parent, const wxString& name, int imgIndex, CCTreeCtrlData* data)
{
    TRACE(_T("ClassBrowserBuilderThread::AddNodeIfNotThere"));

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

bool ClassBrowserBuilderThread::AddChildrenOf(CCTreeCtrl* tree, wxTreeItemId parent, int parentTokenIdx, short int tokenKindMask, int tokenScopeMask)
{
    TRACE(_T("ClassBrowserBuilderThread::AddChildrenOf"));

    if (CBBT_SANITY_CHECK)
        return false;

    const Token* parentToken = 0;
    bool parentTokenError = false;
    const TokenIdxSet* tokens = 0;

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    if (parentTokenIdx == -1)
    {
        if (   m_BrowserOptions.displayFilter == bdfWorkspace
            || m_BrowserOptions.displayFilter == bdfEverything )
            tokens =  m_TokenTree->GetGlobalNameSpaces();
        else
            tokens = &m_CurrentGlobalTokensSet;
    }
    else
    {
        parentToken = m_TokenTree->at(parentTokenIdx);
        if (!parentToken)
        {
            TRACE(_T("Token not found?!?"));
            parentTokenError = true;
        }
        if (!parentTokenError)
            tokens = &parentToken->m_Children;
    }

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    if (parentTokenError) return false;

    return AddNodes(tree, parent, tokens, tokenKindMask, tokenScopeMask,
                    m_BrowserOptions.displayFilter == bdfEverything);
}

bool ClassBrowserBuilderThread::AddAncestorsOf(CCTreeCtrl* tree, wxTreeItemId parent, int tokenIdx)
{
    TRACE(_T("ClassBrowserBuilderThread::AddAncestorsOf"));

    if (CBBT_SANITY_CHECK)
        return false;

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    Token* token = m_TokenTree->at(tokenIdx);
    if (token)
        m_TokenTree->RecalcInheritanceChain(token);

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    if (!token)
        return false;

    return AddNodes(tree, parent, &token->m_DirectAncestors, tkClass | tkTypedef, 0, true);
}

bool ClassBrowserBuilderThread::AddDescendantsOf(CCTreeCtrl* tree, wxTreeItemId parent, int tokenIdx, bool allowInheritance)
{
    TRACE(_T("ClassBrowserBuilderThread::AddDescendantsOf"));

    if (CBBT_SANITY_CHECK)
        return false;

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    Token* token = m_TokenTree->at(tokenIdx);
    if (token)
        m_TokenTree->RecalcInheritanceChain(token);

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    if (!token)
        return false;

    bool oldShowInheritance = m_BrowserOptions.showInheritance;
    m_BrowserOptions.showInheritance = allowInheritance;

    bool ret = AddNodes(tree, parent, &token->m_Descendants, tkClass | tkTypedef, 0, true);

    m_BrowserOptions.showInheritance = oldShowInheritance;
    return ret;
}

void ClassBrowserBuilderThread::AddMembersOf(CCTreeCtrl* tree, wxTreeItemId node)
{
    TRACE(_T("ClassBrowserBuilderThread::AddMembersOf"));

    if (CBBT_SANITY_CHECK || !node.IsOk())
        return;

    CCTreeCtrlData* data = static_cast<CCTreeCtrlData*>(m_CCTreeCtrlTop->GetItemData(node));

    bool bottom = (tree == m_CCTreeCtrlBottom);
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
                    if (   m_BrowserOptions.sortType == bstKind
                        && !(data->m_Token->m_TokenKind & tkEnum))
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
                    else if (   m_BrowserOptions.sortType == bstScope
                             && data->m_Token->m_TokenKind & tkClass )
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
            case sfRoot:
            case sfBase:
            case sfDerived:
            default:
                break;
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

bool ClassBrowserBuilderThread::AddNodes(CCTreeCtrl* tree, wxTreeItemId parent, const TokenIdxSet* tokens,
                                         short int tokenKindMask, int tokenScopeMask, bool allowGlobals)
{
    TRACE(_T("ClassBrowserBuilderThread::AddNodes"));

    int count = 0;
    std::set<unsigned long, std::less<unsigned long> > tickets;

    // Build temporary list of Token tickets - if the token's ticket is present
    // among the parent node's children, it's a dupe, and we'll skip it.
    if (parent.IsOk() && tree == m_CCTreeCtrlTop)
    {
        wxTreeItemIdValue cookie;
        wxTreeItemId curchild = tree->GetFirstChild(parent,cookie);
        while (curchild.IsOk())
        {
            CCTreeCtrlData* data = static_cast<CCTreeCtrlData*>(tree->GetItemData(curchild));
            curchild = tree->GetNextSibling(curchild);
            if (data && data->m_Ticket)
                tickets.insert(data->m_Ticket);
        }
    }

    TokenIdxSet::const_iterator end = tokens->end();
    for (TokenIdxSet::const_iterator start = tokens->begin(); start != end; ++start)
    {
        CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

        Token* token = m_TokenTree->at(*start);

        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

        if (    token
            && (token->m_TokenKind & tokenKindMask)
            && (tokenScopeMask == 0 || token->m_Scope == tokenScopeMask)
            && (allowGlobals || token->m_IsLocal || TokenMatchesFilter(token)) )
        {
            if (   tree == m_CCTreeCtrlTop
                && tickets.find(token->GetTicket()) != tickets.end() )
                continue; // dupe
            ++count;
            int img = m_NativeParser->GetTokenKindImage(token);

            wxString str = token->m_Name;
            if (   (token->m_TokenKind == tkFunction)
                || (token->m_TokenKind == tkConstructor)
                || (token->m_TokenKind == tkDestructor)
                || (token->m_TokenKind == tkMacro)
                || (token->m_TokenKind == tkClass) )
            {
                str << token->GetFormattedArgs();
            }
            if (!token->m_FullType.IsEmpty())
                str = str + _T(" : ") + token->m_FullType + token->m_TemplateArgument;

            wxTreeItemId child = tree->AppendItem(parent, str, img, img, new CCTreeCtrlData(sfToken, token, tokenKindMask));

            // mark as expanding if it is a container
            int kind = tkClass | tkNamespace | tkEnum;
            if (token->m_TokenKind == tkClass)
            {
                if (!m_BrowserOptions.treeMembers)
                    kind |= tkTypedef | tkFunction | tkVariable | tkEnum | tkMacro;
                tree->SetItemHasChildren(child,    m_BrowserOptions.showInheritance
                                                || TokenContainsChildrenOfKind(token, kind));
            }
            else if (token->m_TokenKind & (tkNamespace | tkEnum))
            {
                if (!m_BrowserOptions.treeMembers)
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

bool ClassBrowserBuilderThread::TokenMatchesFilter(const Token* token, bool locked)
{
    TRACE(_T("ClassBrowserBuilderThread::TokenMatchesFilter"));

    if (!token || token->m_IsTemp)
        return false;

    if (    m_BrowserOptions.displayFilter == bdfEverything
        || (m_BrowserOptions.displayFilter == bdfWorkspace && token->m_IsLocal) )
        return true;

    if (m_BrowserOptions.displayFilter == bdfFile && !m_CurrentTokenSet.empty())
    {
        if (m_CurrentTokenSet.find(token->m_Index) != m_CurrentTokenSet.end())
            return true;

        // we got to check all children of this token (recursively)
        // to see if any of them matches the filter...
        for (TokenIdxSet::const_iterator tis_it = token->m_Children.begin(); tis_it != token->m_Children.end(); ++tis_it)
        {
            if (!locked)
                CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

            const Token* curr_token = m_TokenTree->at(*tis_it);

            if (!locked)
                CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

            if (!curr_token)
                break;

            if ( TokenMatchesFilter(curr_token, locked) )
                return true;
        }
    }
    else if (m_BrowserOptions.displayFilter == bdfProject && m_UserData)
        return token->m_UserData == m_UserData;

    return false;
}

bool ClassBrowserBuilderThread::TokenContainsChildrenOfKind(const Token* token, int kind)
{
    TRACE(_T("ClassBrowserBuilderThread::TokenContainsChildrenOfKind"));

    if (!token)
        return false;

    bool isOfKind = false;
    const TokenTree* tree = token->GetTree();

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    for (TokenIdxSet::const_iterator tis_it = token->m_Children.begin(); tis_it != token->m_Children.end(); ++tis_it)
    {
        const Token* child = tree->at(*tis_it);
        if (child->m_TokenKind & kind)
        {
            isOfKind = true;
            break;
        }
    }

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    return isOfKind;
}

void ClassBrowserBuilderThread::SaveExpandedItems(CCTreeCtrl* tree, wxTreeItemId parent, int level)
{
    TRACE(_T("ClassBrowserBuilderThread::SaveExpandedItems"));

    if (CBBT_SANITY_CHECK)
        return;

    wxTreeItemIdValue cookie;
    wxTreeItemId existing = tree->GetFirstChild(parent, cookie);
    while (existing.IsOk())
    {
        CCTreeCtrlData* data = static_cast<CCTreeCtrlData*>(tree->GetItemData(existing));
        if (tree->GetChildrenCount(existing,false) > 0)
        {
            m_ExpandedVect.push_back(CCTreeCtrlExpandedItemData(data, level));

            SaveExpandedItems(tree, existing, level + 1);
        }

        existing = tree->GetNextSibling(existing);
    }
}

void ClassBrowserBuilderThread::ExpandSavedItems(CCTreeCtrl* tree, wxTreeItemId parent, int level)
{
    TRACE(_T("ClassBrowserBuilderThread::ExpandSavedItems"));

    if (CBBT_SANITY_CHECK)
        return;

    wxTreeItemIdValue cookie;
    wxTreeItemId existing = tree->GetFirstChild(parent, cookie);
    while (existing.IsOk() && !m_ExpandedVect.empty())
    {
        CCTreeCtrlData* data = static_cast<CCTreeCtrlData*>(tree->GetItemData(existing));
        CCTreeCtrlExpandedItemData saved = m_ExpandedVect.front();

        if (   level == saved.GetLevel()
            && wxStrcmp(data->m_TokenName, saved.GetData().m_TokenName) == 0
            && data->m_TokenKind == saved.GetData().m_TokenKind
            && data->m_SpecialFolder == saved.GetData().m_SpecialFolder )
        {
            tree->Expand(existing);

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
    TRACE(_T("ClassBrowserBuilderThread::SaveSelectedItem"));

    if (CBBT_SANITY_CHECK)
        return;

    m_SelectedPath.clear();

    wxTreeItemId item = m_CCTreeCtrlTop->GetSelection();
    while (item.IsOk() && item != m_CCTreeCtrlTop->GetRootItem())
    {
        CCTreeCtrlData* data = static_cast<CCTreeCtrlData*>(m_CCTreeCtrlTop->GetItemData(item));
        m_SelectedPath.push_front(*data);

        item = m_CCTreeCtrlTop->GetItemParent(item);
    }
}

void ClassBrowserBuilderThread::SelectSavedItem()
{
    TRACE(_T("ClassBrowserBuilderThread::SelectSavedItem"));

    if (CBBT_SANITY_CHECK)
        return;

    wxTreeItemId parent = m_CCTreeCtrlTop->GetRootItem();

	// TODO: (Martin) wxTreeCtrl documentation states that cookie is for re-entrancy an must be unique for all calls that belong together.
	//        So, this needs to be initialized to some value?
	//        (Which value, though... I'm inclined to just use 1 and 2 for here and below... but no clue if you've used those elsewhere)
    wxTreeItemIdValue cookie;
    wxTreeItemId item = m_CCTreeCtrlTop->GetFirstChild(parent, cookie);

    while (!m_SelectedPath.empty() && item.IsOk())
    {
        CCTreeCtrlData* data  = static_cast<CCTreeCtrlData*>(m_CCTreeCtrlTop->GetItemData(item));
        CCTreeCtrlData* saved = &m_SelectedPath.front();

        if (   data->m_SpecialFolder == saved->m_SpecialFolder
            && wxStrcmp(data->m_TokenName, saved->m_TokenName) == 0
            && data->m_TokenKind == saved->m_TokenKind )
        {
			// TODO: (Martin) see above. Different value here, I'd assume?
            wxTreeItemIdValue cookie2;
            parent = item;
            item   = m_CCTreeCtrlTop->GetFirstChild(item, cookie2);
            m_SelectedPath.pop_front();
        }
        else
            item = m_CCTreeCtrlTop->GetNextSibling(item);
    }

    if (parent.IsOk())
    {
        m_SelectItemRequired = parent; // remember what item to select

        wxCommandEvent e(wxEVT_COMMAND_ENTER, m_idThreadEvent);
        e.SetInt(selectItemRequired);
        m_Parent->AddPendingEvent(e);
    }

    m_SelectedPath.clear();
}

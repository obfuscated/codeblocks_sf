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
#include "classbrowser.h"

// sanity check for the build tree functions, this function should only be called in a worker thread
// also, there should be no termination requested, otherwise, it will return false
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
    m_NativeParser(nullptr),
    m_CCTreeTop(nullptr),
    m_CCTreeBottom(nullptr),
    m_UserData(nullptr),
    m_BrowserOptions(),
    m_TokenTree(nullptr),
    m_InitDone(false),
    m_Busy(false),
    m_TerminationRequested(false),
    m_idThreadEvent(wxID_NONE),
    m_topCrc32(CRC32_CCITT),
    m_bottomCrc32(CRC32_CCITT)
{
}

ClassBrowserBuilderThread::~ClassBrowserBuilderThread()
{
    delete m_CCTreeTop;
    delete m_CCTreeBottom;
}

void ClassBrowserBuilderThread::Init(NativeParser*         np,
                                     const wxString&       active_filename,
                                     void*                 user_data, // active project
                                     const BrowserOptions& bo,
                                     TokenTree*            tt,
                                     int                   idThreadEvent)
{
    TRACE("ClassBrowserBuilderThread::Init");

    CC_LOCKER_TRACK_CBBT_MTX_LOCK(m_ClassBrowserBuilderThreadMutex);

    m_NativeParser     = np;
    m_CCTreeTop        = new CCTree();
    m_CCTreeBottom     = new CCTree();
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

        cbProject* prj = static_cast <cbProject*> (m_UserData);
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

    // our token tree has an internal map file -> tokens, so, we can collect all the tokens if we
    // are given the file set.
    // the tokens are stored in m_CurrentTokenSet, and the special global scope tokens are stored
    // in the m_CurrentGlobalTokensSet.
    if (!m_CurrentFileSet.empty())
    {
        CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

        m_CurrentTokenSet.clear();
        m_CurrentGlobalTokensSet.clear();
        for (TokenFileSet::const_iterator itf = m_CurrentFileSet.begin(); itf != m_CurrentFileSet.end(); ++itf)
        {
            const TokenIdxSet* tokens = tree->GetTokensBelongToFile(*itf);
            if (!tokens)
                continue;

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
    while (!m_TerminationRequested && !Manager::IsAppShuttingDown())
    {
        // waits here, until the ClassBrowser unlocks
        // we put a semaphore wait function in the while loop, so the first time if
        // the semaphore is 1, we can call BuildTree() in the loop, in the meanwhile
        // the semaphore becomes 0. We will be blocked by semaphore's wait function
        // in the next while loop. The semaphore post function will be called in the
        // GUI thread once a new BuildTree() call is needed.
        m_ClassBrowserSemaphore.Wait();

        if (m_TerminationRequested || Manager::IsAppShuttingDown() )
            break;

        m_Busy = true;

        // The thread can do many jobs:
        switch (m_nextJob)
          {
          case JobBuildTree:  // build internal trees and transfer to GUI ones
              BuildTree();
              break;
          case JobSelectTree: // fill the bottom tree with data relative to the selected item
              SelectGUIItem();
              FillGUITree(false);
              break;
          case JobExpandItem: // add child items on the fly
              ExpandGUIItem();
              break;
          default:
              ;
          }

        m_Busy = false;
    }

    m_NativeParser = nullptr;
    m_CCTreeTop = nullptr;
    m_CCTreeBottom = nullptr;

    return nullptr;
}

void ClassBrowserBuilderThread::ExpandGUIItem()
{
    if (m_targetItem)
    {
        ExpandItem(m_targetItem);
        AddItemChildrenToGuiTree(m_CCTreeTop, m_targetItem, true);
        m_Parent->CallAfter(&ClassBrowser::TreeOperation, ClassBrowser::OpExpandCurrent, nullptr);
    }
}

void ClassBrowserBuilderThread::ExpandItem(CCTreeItem* item)
{
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

    // we want to show the children of the current node, inheritance information such as
    // base class or derived class need to be shown
    CCTreeCtrlData* data = m_CCTreeTop->GetItemData(item);
    if (data)
        m_TokenTree->RecalcInheritanceChain(data->m_Token);

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    if (data)
    {
        switch (data->m_SpecialFolder)
        {
            case sfRoot:
            {
                CreateSpecialFolders(m_CCTreeTop, item);
                if( !(   m_BrowserOptions.displayFilter == bdfFile
                      && m_ActiveFilename.IsEmpty() ) )
                    AddChildrenOf(m_CCTreeTop, item, -1, ~(tkFunction | tkVariable | tkMacroDef | tkTypedef | tkMacroUse));
                break;
            }
            case sfBase:    AddAncestorsOf(m_CCTreeTop, item, data->m_Token->m_Index); break;
            case sfDerived: AddDescendantsOf(m_CCTreeTop, item, data->m_Token->m_Index, false); break;
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
                            CCTreeItem* base = m_CCTreeTop->AppendItem(item, _("Base classes"),
                                               PARSER_IMG_CLASS_FOLDER, PARSER_IMG_CLASS_FOLDER,
                                               new CCTreeCtrlData(sfBase, data->m_Token, tkClass, data->m_Token->m_Index));
                            if (!data->m_Token->m_DirectAncestors.empty())
                                m_CCTreeTop->SetItemHasChildren(base);
                            CCTreeItem* derived = m_CCTreeTop->AppendItem(item, _("Derived classes"),
                                                  PARSER_IMG_CLASS_FOLDER, PARSER_IMG_CLASS_FOLDER,
                                                  new CCTreeCtrlData(sfDerived, data->m_Token, tkClass, data->m_Token->m_Index));
                            if (!data->m_Token->m_Descendants.empty())
                                m_CCTreeTop->SetItemHasChildren(derived);
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
                    case tkMacroDef:
                    case tkMacroUse:
                    case tkAnyContainer:
                    case tkAnyFunction:
                    case tkUndefined:
                    default:
                        break;
                }
                if (kind != 0)
                    AddChildrenOf(m_CCTreeTop, item, data->m_Token->m_Index, kind);
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
        AddMembersOf(m_CCTreeTop, item);

#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F("ExpandItem (internally) took : %ld ms for %u items.", sw.Time(), m_CCTreeTop->GetCount()));
#endif

    if (locked)
        CC_LOCKER_TRACK_CBBT_MTX_UNLOCK(m_ClassBrowserBuilderThreadMutex)
}

void ClassBrowserBuilderThread::SelectGUIItem()
{
    TRACE("ClassBrowserBuilderThread::SelectItem");

    if (!m_targetItem)
        return;

    CC_LOCKER_TRACK_CBBT_MTX_LOCK(m_ClassBrowserBuilderThreadMutex)

#ifdef CC_BUILDTREE_MEASURING
    wxStopWatch sw;
#endif

    CCTree* tree = (m_BrowserOptions.treeMembers) ? m_CCTreeBottom : m_CCTreeTop;
    if ( !(   m_BrowserOptions.displayFilter == bdfFile
           && m_ActiveFilename.IsEmpty() ) )
        AddMembersOf(tree, m_targetItem);

#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F("SelectGUIItem (internally) took : %ld ms", sw.Time()));
#endif

    CC_LOCKER_TRACK_CBBT_MTX_UNLOCK(m_ClassBrowserBuilderThreadMutex)
}

// Main worker functions

void ClassBrowserBuilderThread::BuildTree()
{
    if (CBBT_SANITY_CHECK || !m_NativeParser)
        return; // Called before UI tree construction completed?!

    m_Parent->CallAfter(&ClassBrowser::BuildTreeStartOrStop, true);

#ifdef CC_BUILDTREE_MEASURING
    wxStopWatch sw;
    wxStopWatch sw_total;
#endif

    // 1.) Create initial root node, if not already there
    CCTreeItem* root = m_CCTreeTop->GetRootItem();
    if (!root)
        root = m_CCTreeTop->AddRoot(_("Symbols"), PARSER_IMG_SYMBOLS_FOLDER, PARSER_IMG_SYMBOLS_FOLDER, new CCTreeCtrlData(sfRoot));

    m_CCTreeTop->SetItemHasChildren(root);

    // 2.) Update compare functions
    m_CCTreeTop->SetCompareFunction(m_BrowserOptions.sortType);
    m_CCTreeBottom->SetCompareFunction(m_BrowserOptions.sortType);

    // 3.) Save expanded items to restore later
    m_ExpandedVect.clear();
    SaveExpandedItems(m_CCTreeTop, root, 0);
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F("Saving expanded items took : %ld ms", sw.Time()));
    sw.Start();
#endif

    // 4.) Remove any nodes no longer valid (due to update)
    RemoveInvalidNodes(m_CCTreeTop, root);
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F("Removing invalid nodes (top tree) took : %ld ms", sw.Time()));
    sw.Start();
#endif

    if (m_BrowserOptions.treeMembers)
    {
        RemoveInvalidNodes(m_CCTreeBottom, m_CCTreeBottom->GetRootItem());
#ifdef CC_BUILDTREE_MEASURING
        CCLogger::Get()->DebugLog(F("Removing invalid nodes (bottom tree) took : %ld ms", sw.Time()));
        sw.Start();
#endif
    }

    // Meanwhile, C::B might want to shutdown?!
    if (CBBT_SANITY_CHECK)
    {
        m_Parent->CallAfter(&ClassBrowser::BuildTreeStartOrStop, false);
        return;
    }
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F("TestDestroy() took : %ld ms", sw.Time()));
    sw.Start();
#endif

    // 6.) Expand item
    ExpandItem(root);
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F("Expanding root item took : %ld ms", sw.Time()));
    sw.Start();
#endif

    // 7.) Expand the items saved before
    ExpandSavedItems(m_CCTreeTop, root, 0);
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F("Expanding saved items took : %ld ms", sw.Time()));
    sw.Start();
#endif

    // 8.) Expand namespaces and classes
    ExpandNamespaces(root, tkNamespace, 1);
    ExpandNamespaces(root, tkClass,     1);

#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F("Expanding namespaces took : %ld ms", sw.Time()));
    sw.Start();
#endif

    m_Parent->CallAfter(&ClassBrowser::BuildTreeStartOrStop, false);

    if (CBBT_SANITY_CHECK)
        return;

    // 9.) Fill top GUI tree, the bottom GUI tree will be filled later when making a selection
    FillGUITree(true);  // top tree

    // Initialisation is done after Init() and at least *one* call to BuildTree().
    // Also, in Init() m_InitDone is set to false, directly followed by a
    // re-launch of the thread resulting in a call to BuildTree() due to
    // posting the semaphore from ClassBrowser.

    m_InitDone = true;
}

void ClassBrowserBuilderThread::RemoveInvalidNodes(CCTree* tree, CCTreeItem* parent)
{
    TRACE("ClassBrowserBuilderThread::RemoveInvalidNodes");

    if (CBBT_SANITY_CHECK || !parent)
        return;

    // recursively enters all existing nodes and deletes the node if the token it references
    // is invalid (i.e. m_TokenTree->at() != token_in_data)

    // we'll loop backwards so we can delete nodes without problems
    CCTreeItem* existing = tree->GetLastChild(parent);
    while (existing)
    {
        bool removeCurrent = false;
        bool hasChildren = tree->ItemHasChildren(existing);
        CCTreeCtrlData* data = tree->GetItemData(existing);

        if (tree == m_CCTreeBottom)
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

            CCTreeItem* next = tree->GetPrevSibling(existing);
            if (!next && (tree == m_CCTreeTop) && (tree->GetChildrenCount(parent, false) == 1))
            {
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

        if (existing)
            existing = tree->GetPrevSibling(existing);
    }
}

void ClassBrowserBuilderThread::ExpandNamespaces(CCTreeItem* node, TokenKind tokenKind, int level)
{
    TRACE("ClassBrowserBuilderThread::ExpandNamespaces");

    if (CBBT_SANITY_CHECK || !m_BrowserOptions.expandNS || !node || level <= 0 )
        return;

    CCCookie cookie;
    for (CCTreeItem* existing = m_CCTreeTop->GetFirstChild(node, cookie); existing; existing = m_CCTreeTop->GetNextSibling(existing))
    {
        CCTreeCtrlData* data = m_CCTreeTop->GetItemData(existing);
        if (   data
            && data->m_Token
            && (data->m_Token->m_TokenKind == tokenKind) )
        {
            TRACE(F("Auto-expanding: " + data->m_Token->m_Name));
            ExpandItem(existing);
            ExpandNamespaces(existing, tokenKind, level-1); // re-curse
        }
    }
}

// checks if there are respective children and colours the nodes
bool ClassBrowserBuilderThread::CreateSpecialFolders(CCTree* tree, CCTreeItem* parent)
{
    TRACE("ClassBrowserBuilderThread::CreateSpecialFolders");

    bool hasGF = false;     // has global functions
    bool hasGV = false;     // has global variables
    bool hasGP = false;     // has global  macro definition
    bool hasTD = false;     // has type defines
    bool hasGM = false;     // has macro usage, note that this kind of tokens does not exits in the
                            //   token tree, so we don't show such special folder

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
            else if (!hasGM && token->m_TokenKind == tkMacroUse)
                hasGM = true;
            else if (!hasGV && token->m_TokenKind == tkVariable)
                hasGV = true;
            else if (!hasGP && token->m_TokenKind == tkMacroDef)
                hasGP = true;
            else if (!hasTD && token->m_TokenKind == tkTypedef)
                hasTD = true;
        }

        if (hasGF && hasGV && hasGP && hasTD && hasGM)
            break; // we have everything, stop iterating...
    }

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    CCTreeItem* gfuncs  = AddNodeIfNotThere(m_CCTreeTop, parent, _("Global functions"),
                          PARSER_IMG_FUNCS_FOLDER,   new CCTreeCtrlData(sfGFuncs,    0, tkFunction, -1));
    CCTreeItem* tdef    = AddNodeIfNotThere(m_CCTreeTop, parent, _("Global typedefs"),
                          PARSER_IMG_TYPEDEF_FOLDER, new CCTreeCtrlData(sfTypedef,   0, tkTypedef,  -1));
    CCTreeItem* gvars   = AddNodeIfNotThere(m_CCTreeTop, parent, _("Global variables"),
                          PARSER_IMG_VARS_FOLDER,    new CCTreeCtrlData(sfGVars,     0, tkVariable, -1));
    CCTreeItem* preproc = AddNodeIfNotThere(m_CCTreeTop, parent, _("Macro definitions"),
                          PARSER_IMG_MACRO_DEF_FOLDER, new CCTreeCtrlData(sfPreproc, 0, tkMacroDef, -1));
    CCTreeItem* gmacro  = AddNodeIfNotThere(m_CCTreeTop, parent, _("Macro usages"),
                          PARSER_IMG_MACRO_USE_FOLDER,   new CCTreeCtrlData(sfMacro, 0, tkMacroUse, -1));

    // the logic here is: if the treeMembers option is on, then all the child members will be shownn
    // in the bottom, for example, if we have some global functions for the current file, then the
    // function tokens will be shown in the bottom tree, so we don't have a '+' in the
    // Symbols(root node)->Global functions(1 level node), when the user click on the "Global functions"
    // node, all the global functions will be shown in the bottom tree.
    // if the treeMembers is false, then all the global function tokens will be children of the
    // Global functions node
    const bool bottom = m_BrowserOptions.treeMembers;
    m_CCTreeTop->SetItemHasChildren(gfuncs,  !bottom && hasGF);
    m_CCTreeTop->SetItemHasChildren(tdef,    !bottom && hasTD);
    m_CCTreeTop->SetItemHasChildren(gvars,   !bottom && hasGV);
    m_CCTreeTop->SetItemHasChildren(preproc, !bottom && hasGP);
    m_CCTreeTop->SetItemHasChildren(gmacro,  !bottom && hasGM);

    wxColour black = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    wxColour grey  = wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT);

    // if we don't have any global function tokens, then we set the label of "Global functions" node
    // as grey color.
    tree->SetItemTextColour(gfuncs,  hasGF ? black : grey);
    tree->SetItemTextColour(gvars,   hasGV ? black : grey);
    tree->SetItemTextColour(preproc, hasGP ? black : grey);
    tree->SetItemTextColour(tdef,    hasTD ? black : grey);
    tree->SetItemTextColour(gmacro,  hasGM ? black : grey);

    return hasGF || hasGV || hasGP || hasTD || hasGM;
}

CCTreeItem* ClassBrowserBuilderThread::AddNodeIfNotThere(CCTree* tree, CCTreeItem* parent, const wxString& name, int imgIndex, CCTreeCtrlData* data)
{
    TRACE("ClassBrowserBuilderThread::AddNodeIfNotThere");

    CCCookie cookie;
    CCTreeItem* existing = tree->GetFirstChild(parent, cookie);
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

bool ClassBrowserBuilderThread::AddChildrenOf(CCTree* tree,
                                              CCTreeItem* parent,
                                              int parentTokenIdx,
                                              short int tokenKindMask,
                                              int tokenScopeMask)
{
    TRACE("ClassBrowserBuilderThread::AddChildrenOf");

    if (CBBT_SANITY_CHECK)
        return false;

    const Token* parentToken = nullptr;
    bool parentTokenError = false;
    const TokenIdxSet* tokens = nullptr;

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
            TRACE("Token not found?!?");
            parentTokenError = true;
        }
        if (!parentTokenError)
            tokens = &parentToken->m_Children;
    }

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    if (parentTokenError)
        return false;

    return AddNodes(tree, parent, tokens, tokenKindMask, tokenScopeMask,
                    m_BrowserOptions.displayFilter == bdfEverything);
}

bool ClassBrowserBuilderThread::AddAncestorsOf(CCTree* tree, CCTreeItem* parent, int tokenIdx)
{
    TRACE("ClassBrowserBuilderThread::AddAncestorsOf");

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

bool ClassBrowserBuilderThread::AddDescendantsOf(CCTree* tree, CCTreeItem* parent, int tokenIdx, bool allowInheritance)
{
    TRACE("ClassBrowserBuilderThread::AddDescendantsOf");

    if (CBBT_SANITY_CHECK)
        return false;

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    Token* token = m_TokenTree->at(tokenIdx);
    if (token)
        m_TokenTree->RecalcInheritanceChain(token);

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    if (!token)
        return false;

    const bool oldShowInheritance = m_BrowserOptions.showInheritance;
    m_BrowserOptions.showInheritance = allowInheritance;
    const bool ret = AddNodes(tree, parent, &token->m_Descendants, tkClass | tkTypedef, 0, true);
    m_BrowserOptions.showInheritance = oldShowInheritance;
    return ret;
}

void ClassBrowserBuilderThread::AddMembersOf(CCTree* tree, CCTreeItem* node)
{
    TRACE("ClassBrowserBuilderThread::AddMembersOf");

    if (CBBT_SANITY_CHECK || !node)
        return;

    CCTreeCtrlData* data = m_CCTreeTop->GetItemData(node);

    const bool bottom = (tree == m_CCTreeBottom);
    if (bottom)
    {
#ifdef CC_BUILDTREE_MEASURING
        wxStopWatch sw;
#endif
        tree->DeleteAllItems();
#ifdef CC_BUILDTREE_MEASURING
        CCLogger::Get()->DebugLog(F("tree->DeleteAllItems() took : %ld ms", sw.Time()));
        sw.Start();
#endif
        node = tree->AddRoot("Members"); // not visible, so don't translate
#ifdef CC_BUILDTREE_MEASURING
        CCLogger::Get()->DebugLog(F("tree->AddRoot() took : %ld ms", sw.Time()));
#endif
    }

    if (data)
    {
        switch (data->m_SpecialFolder)
        {
            case sfGFuncs  : AddChildrenOf(tree, node, -1, tkFunction, false); break;
            case sfGVars   : AddChildrenOf(tree, node, -1, tkVariable, false); break;
            case sfPreproc : AddChildrenOf(tree, node, -1, tkMacroDef, false); break;
            case sfTypedef : AddChildrenOf(tree, node, -1, tkTypedef,  false); break;
            case sfMacro   : AddChildrenOf(tree, node, -1, tkMacroUse, false); break;
            case sfToken:
            {
                if (bottom)
                {
                    if (   m_BrowserOptions.sortType == bstKind
                        && !(data->m_Token->m_TokenKind & tkEnum))
                    {
                        CCTreeItem* rootCtorDtor = tree->AppendItem(node, _("Ctors & Dtors"), PARSER_IMG_CLASS_FOLDER);
                        CCTreeItem* rootFuncs    = tree->AppendItem(node, _("Functions"), PARSER_IMG_FUNCS_FOLDER);
                        CCTreeItem* rootVars     = tree->AppendItem(node, _("Variables"), PARSER_IMG_VARS_FOLDER);
                        CCTreeItem* rootMacro    = tree->AppendItem(node, _("Macros"), PARSER_IMG_MACRO_USE_FOLDER);
                        CCTreeItem* rootOthers   = tree->AppendItem(node, _("Others"), PARSER_IMG_OTHERS_FOLDER);

                        AddChildrenOf(tree, rootCtorDtor, data->m_Token->m_Index, tkConstructor | tkDestructor);
                        AddChildrenOf(tree, rootFuncs,    data->m_Token->m_Index, tkFunction);
                        AddChildrenOf(tree, rootVars,     data->m_Token->m_Index, tkVariable);
                        AddChildrenOf(tree, rootMacro,    data->m_Token->m_Index, tkMacroUse);
                        AddChildrenOf(tree, rootOthers,   data->m_Token->m_Index, ~(tkNamespace | tkClass | tkEnum | tkAnyFunction | tkVariable | tkMacroUse));
                    }
                    else if (   m_BrowserOptions.sortType == bstScope
                             && data->m_Token->m_TokenKind & tkClass )
                    {
                        CCTreeItem* rootPublic    = tree->AppendItem(node, _("Public"), PARSER_IMG_CLASS_FOLDER);
                        CCTreeItem* rootProtected = tree->AppendItem(node, _("Protected"), PARSER_IMG_FUNCS_FOLDER);
                        CCTreeItem* rootPrivate   = tree->AppendItem(node, _("Private"), PARSER_IMG_VARS_FOLDER);

                        AddChildrenOf(tree, rootPublic,    data->m_Token->m_Index, ~(tkNamespace | tkClass | tkEnum), tsPublic);
                        AddChildrenOf(tree, rootProtected, data->m_Token->m_Index, ~(tkNamespace | tkClass | tkEnum), tsProtected);
                        AddChildrenOf(tree, rootPrivate,   data->m_Token->m_Index, ~(tkNamespace | tkClass | tkEnum), tsPrivate);
                    }
                    else
                    {
                        AddChildrenOf(tree, node, data->m_Token->m_Index, ~(tkNamespace | tkClass | tkEnum));
                        break;
                    }

                    CCTreeItem* existing = tree->GetLastChild(tree->GetRootItem());
                    while (existing)
                    {
                        CCTreeItem* next = tree->GetPrevSibling(existing);

                        if (tree->GetChildrenCount(existing) > 0)
                        {
                            tree->SetItemBold(existing, true);
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
}

bool ClassBrowserBuilderThread::AddNodes(CCTree* tree, CCTreeItem* parent, const TokenIdxSet* tokens,
                                         short int tokenKindMask, int tokenScopeMask, bool allowGlobals)
{
    TRACE("ClassBrowserBuilderThread::AddNodes");

    int count = 0;
    std::set<unsigned long, std::less<unsigned long> > tickets;

    // Build temporary list of Token tickets - if the token's ticket is present
    // among the parent node's children, it's a duplicate node, and we'll skip it.
    if (parent && tree == m_CCTreeTop)
    {
        CCCookie cookie;
        CCTreeItem* curchild = tree->GetFirstChild(parent, cookie);
        while (curchild)
        {
            CCTreeCtrlData* data = tree->GetItemData(curchild);
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
            if (   tree == m_CCTreeTop
                && tickets.find(token->GetTicket()) != tickets.end() )
                continue; // duplicate node
            ++count;
            int img = m_NativeParser->GetTokenKindImage(token);

            wxString str = token->m_Name;
            if (   (token->m_TokenKind == tkFunction)
                || (token->m_TokenKind == tkConstructor)
                || (token->m_TokenKind == tkDestructor)
                || (token->m_TokenKind == tkMacroUse)
                || (token->m_TokenKind == tkClass) )
            {
                str << token->GetFormattedArgs();
            }
            if (!token->m_FullType.IsEmpty())
                str = str + " : " + token->m_FullType + token->m_TemplateArgument;

            CCTreeItem* child = tree->AppendItem(parent, str, img, img, new CCTreeCtrlData(sfToken, token, tokenKindMask));

            // mark as expanding if it is a container
            int kind = tkClass | tkNamespace | tkEnum;
            if (token->m_TokenKind == tkClass)
            {
                if (!m_BrowserOptions.treeMembers)
                    kind |= tkTypedef | tkFunction | tkVariable | tkEnum | tkMacroUse;
                tree->SetItemHasChildren(child, m_BrowserOptions.showInheritance || TokenContainsChildrenOfKind(token, kind));
            }
            else if (token->m_TokenKind & (tkNamespace | tkEnum))
            {
                if (!m_BrowserOptions.treeMembers)
                    kind |= tkTypedef | tkFunction | tkVariable | tkEnumerator | tkMacroUse;
                tree->SetItemHasChildren(child, TokenContainsChildrenOfKind(token, kind));
            }
        }
    }

    tree->SortChildren(parent);
//    tree->RemoveDoubles(parent);
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F("Added %d nodes", count));
#endif
    return count != 0;
}

bool ClassBrowserBuilderThread::TokenMatchesFilter(const Token* token, bool locked) const
{
    TRACE("ClassBrowserBuilderThread::TokenMatchesFilter");

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
        // tis_it -> token index set iterator
        for (TokenIdxSet::const_iterator tis_it = token->m_Children.begin();
             tis_it != token->m_Children.end();
             ++tis_it)
        {
            if (!locked)
                CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

            const Token* curr_token = m_TokenTree->at(*tis_it);

            if (!locked)
                CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

            if (!curr_token)
                break;

            if (TokenMatchesFilter(curr_token, locked))
                return true;
        }
    }
    else if (m_BrowserOptions.displayFilter == bdfProject && m_UserData)
        return token->m_UserData == m_UserData;

    return false;
}

bool ClassBrowserBuilderThread::TokenContainsChildrenOfKind(const Token* token, int kind) const
{
    TRACE("ClassBrowserBuilderThread::TokenContainsChildrenOfKind");

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

void ClassBrowserBuilderThread::SaveExpandedItems(CCTree* tree, CCTreeItem* parent, int level)
{
    TRACE("ClassBrowserBuilderThread::SaveExpandedItems");

    if (CBBT_SANITY_CHECK)
        return;

    CCCookie cookie;
    for (CCTreeItem* existing = tree->GetFirstChild(parent, cookie); existing; existing = tree->GetNextSibling(existing))
    {
        if (tree->GetChildrenCount(existing, false))
        {
            CCTreeCtrlData* data = tree->GetItemData(existing);
            m_ExpandedVect.push_back(CCTreeCtrlExpandedItemData(data, level));
            SaveExpandedItems(tree, existing, level + 1);
        }
    }
}

void ClassBrowserBuilderThread::ExpandSavedItems(CCTree* tree, CCTreeItem* parent, int level)
{
    TRACE("ClassBrowserBuilderThread::ExpandSavedItems");

    if (CBBT_SANITY_CHECK)
        return;

    CCCookie cookie;
    for (CCTreeItem* existing = tree->GetFirstChild(parent, cookie); existing && !m_ExpandedVect.empty(); existing = tree->GetNextSibling(existing))
    {
        CCTreeCtrlData* data = tree->GetItemData(existing);
        CCTreeCtrlExpandedItemData saved = m_ExpandedVect.front();

        if (   level == saved.GetLevel()
            && wxStrcmp(data->m_TokenName, saved.GetData().m_TokenName) == 0
            && data->m_TokenKind == saved.GetData().m_TokenKind
            && data->m_SpecialFolder == saved.GetData().m_SpecialFolder )
        {
            ExpandItem(existing);
            m_ExpandedVect.pop_front();
            if (m_ExpandedVect.empty())
                return;

            saved = m_ExpandedVect.front(); //next saved
            if (saved.GetLevel() < level)
                return;

            if (saved.GetLevel() > level)
                ExpandSavedItems(tree, existing, saved.GetLevel());
        }
    }

    // remove non-existing by now saved items
    while (!m_ExpandedVect.empty() && m_ExpandedVect.front().GetLevel() > level)
        m_ExpandedVect.pop_front();
}

void ClassBrowserBuilderThread::FillGUITree(bool top)
{
    CCTree* localTree = top ? m_CCTreeTop : m_CCTreeBottom;

    // When Code Completion information changes refreshing is made in two steps:
    //   1.- Top is refreshed and bottom is cleared. Top refresh calls ReselectItem()
    //         if there were no changes or SelectSavedItem() otherwise; this forces
    //         bottom tree regeneration.
    //   2.- Bottom is refreshed.

#ifdef CC_BUILDTREE_MEASURING
    wxStopWatch sw;
#endif

    // Compare the new tree with the old one using CRC32, if they are equal
    // the GUI tree does not need refreshing (but bottom may need it)
    const uint32_t NewCrc32 = localTree->GetCrc32();

#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F("GetCrc32() took : %ld ms", sw.Time()));
#endif

    if (NewCrc32 == (top ? m_topCrc32 : m_bottomCrc32))
    {
        // The bottom tree can change even if the top didn't, force recalculation
        if (top)
            m_Parent->CallAfter(&ClassBrowser::ReselectItem);

        return;
    }

    if (top)
        m_topCrc32 = NewCrc32;
    else
        m_bottomCrc32 = NewCrc32;

    // Save selected item to restore later. The restoration will fire bottom tree regeneration
    if (top)
        m_Parent->CallAfter(&ClassBrowser::SaveSelectedItem);

    m_Parent->CallAfter(&ClassBrowser::SelectTargetTree, top);
    m_Parent->CallAfter(&ClassBrowser::TreeOperation, ClassBrowser::OpClear, nullptr);

    CCTreeItem* sourceRoot = localTree->GetRootItem();
    if (sourceRoot)
    {
        m_Parent->CallAfter(&ClassBrowser::TreeOperation, ClassBrowser::OpAddRoot, sourceRoot);
        AddItemChildrenToGuiTree(localTree, sourceRoot, true);
        m_Parent->CallAfter(&ClassBrowser::TreeOperation, top ? ClassBrowser::OpExpandRoot : ClassBrowser::OpExpandAll, nullptr);
    }

    if (top)
        m_Parent->CallAfter(&ClassBrowser::SelectSavedItem);
    else
        m_Parent->CallAfter(&ClassBrowser::TreeOperation, ClassBrowser::OpShowFirst, nullptr);

    m_Parent->CallAfter(&ClassBrowser::TreeOperation, ClassBrowser::OpEnd, nullptr);
}

// Copies all children of parent under destination's current node in the GUI tree

void ClassBrowserBuilderThread::AddItemChildrenToGuiTree(CCTree* localTree, CCTreeItem* parent, bool recursive) const
{
    CCCookie cookie;
    for (CCTreeItem* child = localTree->GetFirstChild(parent, cookie); child; child = localTree->GetNextChild(parent, cookie))
    {
        if (CBBT_SANITY_CHECK)
            break;

        m_Parent->CallAfter(&ClassBrowser::TreeOperation, ClassBrowser::OpAddChild, child);
        // The semaphore prevents flooding message queue. The timeout is needed when C::B shuts down so the thread can exit
        child->m_semaphore.WaitTimeout(250);
        if (recursive)
            AddItemChildrenToGuiTree(localTree, child, recursive);

        m_Parent->CallAfter(&ClassBrowser::TreeOperation, ClassBrowser::OpGoUp, nullptr);
    }
}

/*
 * CCTreeItem
 */

// Tree items are linked (like a + sign) with the parent, the first child, the previous sibling and the next sibling

CCTreeItem::CCTreeItem(CCTreeItem* parent, const wxString& text, int image, int selImage, CCTreeCtrlData* data) :
    m_parent(parent),
    m_prevSibling(nullptr),
    m_nextSibling(nullptr),
    m_firstChild(nullptr),
    m_text(text),
    m_data(data),
    m_bold(false),
    m_hasChildren(false),
    m_semaphore(0, 1)
{
    m_image[wxTreeItemIcon_Normal]           = image;
    m_image[wxTreeItemIcon_Selected]         = selImage;
    m_image[wxTreeItemIcon_Expanded]         = image;
    m_image[wxTreeItemIcon_SelectedExpanded] = selImage;
}

CCTreeItem::~CCTreeItem()
{
    // Kill my children
    DeleteChildren();

    // Free memory, if any
    if (m_data)
        delete m_data;

    // Report my death to the previous sibling, if any
    if (m_prevSibling)
        m_prevSibling->m_nextSibling = m_nextSibling;

    // Report my death to the next sibling, if any
    if (m_nextSibling)
        m_nextSibling->m_prevSibling = m_prevSibling;

    // If I am not root and I am the first child link my next sibling with the parent
    // If there were no next sibling this marks the parent as no-children
    if (m_parent && !m_prevSibling)
    {
        m_parent->m_firstChild = m_nextSibling;
        if (!m_nextSibling)
            m_parent->m_hasChildren = false;
    }
}

void CCTreeItem::Swap(CCTreeItem* a, CCTreeItem* b)
{
    // Swap the payload part, leaving the pointers untouched
    std::swap(a->m_text,        b->m_text);
    std::swap(a->m_data,        b->m_data);
    std::swap(a->m_bold,        b->m_bold);
    std::swap(a->m_hasChildren, b->m_hasChildren);
    std::swap(a->m_colour,      b->m_colour);
    std::swap(a->m_image,       b->m_image);
}

/*
 * CCTree
 */

CCTreeItem* CCTree::AddRoot(const wxString& text, int image, int selImage, CCTreeCtrlData* data)
{
    wxASSERT_MSG(!m_root, "CCTree can have only a single root");

    m_root = new CCTreeItem(nullptr, text, image, selImage, data);
    return m_root;
}

CCTreeItem* CCTree::AppendItem(CCTreeItem* parent, const wxString& text, int image, int selImage, CCTreeCtrlData* data)
{
    return DoInsertItem(parent, (size_t)-1, text, image, selImage, data);
}

CCTreeItem* CCTree::PrependItem(CCTreeItem* parent, const wxString& text, int image, int selImage, CCTreeCtrlData* data)
{
    return DoInsertItem(parent, 0U, text, image, selImage, data);
}

CCTreeItem* CCTree::InsertItem(CCTreeItem* parent, CCTreeItem* idPrevious, const wxString& text, int image, int selImage, CCTreeCtrlData* data)
{
    return DoInsertAfter(parent, idPrevious, text, image, selImage, data);
}

CCTreeItem* CCTree::InsertItem(CCTreeItem* parent, size_t pos, const wxString& text, int image, int selImage, CCTreeCtrlData* data)
{
    return DoInsertItem(parent, pos, text, image, selImage, data);
}

CCTreeItem* CCTree::GetFirstChild(CCTreeItem* item, CCCookie& cookie) const
{
    cookie.SetCurrent(item ? item->m_firstChild : nullptr);
    return cookie.GetCurrent();
}

CCTreeItem* CCTree::GetNextChild(CCTreeItem* item, CCCookie& cookie) const
{
    cookie.SetCurrent((item && cookie.GetCurrent()) ? cookie.GetCurrent()->m_nextSibling : nullptr);
    return cookie.GetCurrent();
}

CCTreeItem* CCTree::GetLastChild(CCTreeItem* item) const
{
    CCTreeItem* last = nullptr;
    if (item)
    {
        for (CCTreeItem* child = item->m_firstChild; child; child = child->m_nextSibling)
            last = child;
    }

    return last;
}

size_t CCTree::GetChildrenCount(CCTreeItem* item, bool recursively) const
{
    size_t count = 0;
    if (item)
    {
        for (CCTreeItem* child = item->m_firstChild; child; child = child->m_nextSibling)
        {
            ++count;  // count child itself
            if (recursively)
                count += GetChildrenCount(child, recursively);
        }
    }

    return count;
}

void CCTree::QuickSort(CCTreeItem* first, CCTreeItem* last)
{
    if (first && last && (first != last))
    {
        // Partition. Use last as pivot
        CCTreeItem* bound = first;

        // Put all items < pivot before bound
        for (CCTreeItem* j = first; j != last; j = j->m_nextSibling)
        {
            if (LessThan(j, last))
            {
                CCTreeItem::Swap(bound, j);
                bound = bound->m_nextSibling;
            }
        }

        // Move pivot to the bound
        CCTreeItem::Swap(bound, last);

        // Now all items before bound are less than bound, and all items after bound are equal or greater

        // Divide & conquer
        if (bound != first)
          QuickSort(first, bound->m_prevSibling);

        if (bound != last)
          QuickSort(bound->m_nextSibling, last);
    }
}

// Returns a negative value if lhs < rhs, 0 if they are equal and positive if lhs > rhs

int CCTree::CompareFunction(const CCTreeCtrlData* lhs, const CCTreeCtrlData* rhs) const
{
    if (lhs && rhs)
    {
        switch (m_compare)
        {
            case bstAlphabet:
                if (lhs->m_SpecialFolder != sfToken || rhs->m_SpecialFolder != sfToken)
                    return -1;
                if (!lhs->m_Token || !rhs->m_Token)
                    return 1;
                return wxStricmp(lhs->m_Token->m_Name, rhs->m_Token->m_Name);
                break;
            case bstKind:
                if (lhs->m_SpecialFolder != sfToken || rhs->m_SpecialFolder != sfToken)
                    return -1;
                if (lhs->m_TokenKind == rhs->m_TokenKind)
                    return AlphabetCompare(lhs, rhs);
                return lhs->m_TokenKind - rhs->m_TokenKind;
                break;
            case bstScope:
                if (lhs->m_SpecialFolder != sfToken || rhs->m_SpecialFolder != sfToken)
                    return -1;
                if (lhs->m_Token->m_Scope == rhs->m_Token->m_Scope)
                    return KindCompare(lhs, rhs);
                return rhs->m_Token->m_Scope - lhs->m_Token->m_Scope;
                break;
            case bstLine:
                if (lhs->m_SpecialFolder != sfToken || rhs->m_SpecialFolder != sfToken)
                    return -1;
                if (!lhs->m_Token || !rhs->m_Token)
                    return 1;
                if (lhs->m_Token->m_FileIdx == rhs->m_Token->m_FileIdx)
                    return (lhs->m_Token->m_Line > rhs->m_Token->m_Line) * 2 - 1; // from 0,1 to -1,1
                return (lhs->m_Token->m_FileIdx > rhs->m_Token->m_FileIdx) * 2 - 1;
                break;
            default:
                return 0;
        }
    }

    return 1;
}

int CCTree::AlphabetCompare(const CCTreeCtrlData* lhs, const CCTreeCtrlData* rhs) const
{
    if (!lhs || !rhs)
        return 1;
    if (lhs->m_SpecialFolder != sfToken || rhs->m_SpecialFolder != sfToken)
        return -1;
    if (!lhs->m_Token || !rhs->m_Token)
        return 1;
    return wxStricmp(lhs->m_Token->m_Name, rhs->m_Token->m_Name);
}

int CCTree::KindCompare(const CCTreeCtrlData* lhs, const CCTreeCtrlData* rhs) const
{
    if (!lhs || !rhs)
        return 1;
    if (lhs->m_SpecialFolder != sfToken || rhs->m_SpecialFolder != sfToken)
        return -1;
    if (lhs->m_TokenKind == rhs->m_TokenKind)
        return AlphabetCompare(lhs, rhs);
    return lhs->m_TokenKind - rhs->m_TokenKind;
}

CCTreeItem* CCTree::DoInsertAfter(CCTreeItem* parent, CCTreeItem* hInsertAfter, const wxString& text, int image, int selectedImage, CCTreeCtrlData* data)
{
    CCTreeItem* newItem = nullptr;
    if (parent)
    {
        newItem = new CCTreeItem(parent, text, image, selectedImage, data);
        if (!hInsertAfter)
        {
            // Insert as first child
            newItem->m_nextSibling = parent->m_firstChild;
            parent->m_firstChild = newItem;
        }
        else
        {
            // Set my siblings
            newItem->m_prevSibling = hInsertAfter;
            newItem->m_nextSibling = hInsertAfter->m_nextSibling;
            // Tell my previous sibling about me
            newItem->m_prevSibling->m_nextSibling = newItem;
        }

        // Tell my next sibling (if any) about me
        if (newItem->m_nextSibling)
            newItem->m_nextSibling->m_prevSibling = newItem;
    }

    return newItem;
}

CCTreeItem* CCTree::DoInsertItem(CCTreeItem* parent, size_t index, const wxString& text, int image, int selectedImage, CCTreeCtrlData* data)
{
    CCTreeItem* idPrev = nullptr;
    if (parent)
    {
        if (index == (size_t)-1)  // append?
        {
            idPrev = GetLastChild(parent);
        }
        else
        {
            CCCookie cookie;
            for (idPrev = GetFirstChild(parent, cookie); idPrev && index; idPrev = GetNextChild(parent, cookie), --index);
        }
    }

    return DoInsertAfter(parent, idPrev, text, image, selectedImage, data);
}

uint32_t CCTree::GetCrc32() const
{
    Crc32 crc;

    CCTreeItem* root = GetRootItem();
    if (root)
        CalculateCrc32(root, crc);

    return crc.GetCrc();
}

void CCTree::CalculateCrc32(CCTreeItem* parent, Crc32 &crc) const
{
    CCCookie cookie;
    for (CCTreeItem* child = GetFirstChild(parent, cookie); child; child = GetNextChild(parent, cookie))
    {
        crc.Update(child->m_text.data(), child->m_text.size());
        crc.Update(child->m_bold ? 1 : 0);
        crc.Update(child->m_hasChildren ? 1 : 0);
        crc.Update(child->m_colour.Red());
        crc.Update(child->m_colour.Green());
        crc.Update(child->m_colour.Blue());
        crc.Update(child->m_image, sizeof(child->m_image));
        // Compare only token name
        if (child->m_data)
            crc.Update(child->m_data->m_TokenName.data(), child->m_data->m_TokenName.size());

        CalculateCrc32(child, crc);
    }
}

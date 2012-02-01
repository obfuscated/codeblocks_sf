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
    #include <wx/button.h>
    #include <wx/choice.h>
    #include <wx/choicdlg.h>
    #include <wx/intl.h>
    #include <wx/listctrl.h>
    #include <wx/menu.h>
    #include <wx/sizer.h>
    #include <wx/stattext.h>
    #include <wx/treectrl.h>
    #include <wx/settings.h>
    #include <wx/splitter.h>
    #include <wx/utils.h> // wxBusyCursor
    #include <wx/tipwin.h>
    #include <wx/xrc/xmlres.h>

    #include <cbeditor.h>
    #include <cbproject.h>
    #include <configmanager.h>
    #include <editormanager.h>
    #include <globals.h>
    #include <logmanager.h>
    #include <manager.h>
    #include <pluginmanager.h>
    #include <projectmanager.h>
#endif

#include <wx/tokenzr.h>

#include <cbstyledtextctrl.h>

#include "classbrowser.h" // class's header file
#include "nativeparser.h"

#include "parser/ccdebuginfo.h"

#define CC_CLASS_BROWSER_DEBUG_OUTPUT 0

#if CC_GLOBAL_DEBUG_OUTPUT == 1
    #undef CC_CLASS_BROWSER_DEBUG_OUTPUT
    #define CC_CLASS_BROWSER_DEBUG_OUTPUT 1
#elif CC_GLOBAL_DEBUG_OUTPUT == 2
    #undef CC_CLASS_BROWSER_DEBUG_OUTPUT
    #define CC_CLASS_BROWSER_DEBUG_OUTPUT 2
#endif

#if CC_CLASS_BROWSER_DEBUG_OUTPUT == 1
    #define TRACE(format, args...) \
        CCLogger::Get()->DebugLog(F(format, ##args))
    #define TRACE2(format, args...)
#elif CC_CLASS_BROWSER_DEBUG_OUTPUT == 2
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

int idMenuJumpToDeclaration    = wxNewId();
int idMenuJumpToImplementation = wxNewId();
int idMenuRefreshTree          = wxNewId();
int idCBViewInheritance        = wxNewId();
int idCBExpandNS               = wxNewId();
int idCBViewModeFlat           = wxNewId();
int idCBViewModeStructured     = wxNewId();
int idMenuForceReparse         = wxNewId();
int idMenuDebugSmartSense      = wxNewId();
int idCBNoSort                 = wxNewId();
int idCBSortByAlpabet          = wxNewId();
int idCBSortByKind             = wxNewId();
int idCBSortByScope            = wxNewId();
int idCBBottomTree             = wxNewId();
int idCBMakeSelectItem         = wxNewId();

BEGIN_EVENT_TABLE(ClassBrowser, wxPanel)
    EVT_TREE_ITEM_ACTIVATED  (XRCID("treeMembers"), ClassBrowser::OnTreeItemDoubleClick)
    EVT_TREE_ITEM_RIGHT_CLICK(XRCID("treeMembers"), ClassBrowser::OnTreeItemRightClick)

    EVT_TREE_ITEM_ACTIVATED  (XRCID("treeAll"), ClassBrowser::OnTreeItemDoubleClick)
    EVT_TREE_ITEM_RIGHT_CLICK(XRCID("treeAll"), ClassBrowser::OnTreeItemRightClick)
    EVT_TREE_ITEM_EXPANDING  (XRCID("treeAll"), ClassBrowser::OnTreeItemExpanding)
#ifndef CC_NO_COLLAPSE_ITEM
    EVT_TREE_ITEM_COLLAPSING (XRCID("treeAll"), ClassBrowser::OnTreeItemCollapsing)
#endif // CC_NO_COLLAPSE_ITEM
    EVT_TREE_SEL_CHANGED     (XRCID("treeAll"), ClassBrowser::OnTreeItemSelected)

    EVT_TEXT_ENTER(XRCID("cmbSearch"), ClassBrowser::OnSearch)
    EVT_COMBOBOX  (XRCID("cmbSearch"), ClassBrowser::OnSearch)

    EVT_CHOICE(XRCID("cmbView"), ClassBrowser::OnViewScope)
    EVT_BUTTON(XRCID("btnSearch"), ClassBrowser::OnSearch)

    EVT_MENU(idMenuJumpToDeclaration, ClassBrowser::OnJumpTo)
    EVT_MENU(idMenuJumpToImplementation, ClassBrowser::OnJumpTo)
    EVT_MENU(idMenuRefreshTree, ClassBrowser::OnRefreshTree)
    EVT_MENU(idMenuForceReparse, ClassBrowser::OnForceReparse)
    EVT_MENU(idCBViewInheritance, ClassBrowser::OnCBViewMode)
    EVT_MENU(idCBExpandNS, ClassBrowser::OnCBExpandNS)
    EVT_MENU(idCBViewModeFlat, ClassBrowser::OnCBViewMode)
    EVT_MENU(idMenuDebugSmartSense, ClassBrowser::OnDebugSmartSense)
    EVT_MENU(idCBNoSort, ClassBrowser::OnSetSortType)
    EVT_MENU(idCBSortByAlpabet, ClassBrowser::OnSetSortType)
    EVT_MENU(idCBSortByKind, ClassBrowser::OnSetSortType)
    EVT_MENU(idCBSortByScope, ClassBrowser::OnSetSortType)
    EVT_MENU(idCBBottomTree, ClassBrowser::OnCBViewMode)
    EVT_COMMAND(idCBMakeSelectItem, wxEVT_COMMAND_ENTER, ClassBrowser::OnMakeSelectItem)
END_EVENT_TABLE()

// class constructor
ClassBrowser::ClassBrowser(wxWindow* parent, NativeParser* np) :
    m_NativeParser(np),
    m_TreeForPopupMenu(0),
    m_Parser(0L),
    m_ActiveProject(0),
    m_ClassBrowserSemaphore(0, 1),
    m_ClassBrowserBuilderThread(0)
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("code_completion"));

    wxXmlResource::Get()->LoadPanel(this, parent, _T("pnlCB"));
    m_Search = XRCCTRL(*this, "cmbSearch", wxComboBox);

    if (platform::windows)
        m_Search->SetWindowStyle(wxTE_PROCESS_ENTER); // it's a must on windows to catch EVT_TEXT_ENTER

    // Subclassed in XRC file, for reference see here: http://wiki.wxwidgets.org/Resource_Files
    m_Tree       = XRCCTRL(*this, "treeAll", CBTreeCtrl);
    m_TreeBottom = XRCCTRL(*this, "treeMembers", CBTreeCtrl);

    int filter = cfg->ReadInt(_T("/browser_display_filter"), bdfFile);
    XRCCTRL(*this, "cmbView", wxChoice)->SetSelection(filter);

    XRCCTRL(*this, "splitterWin", wxSplitterWindow)->SetMinSize(wxSize(-1, 200));
    // if the classbrowser is put under the control of a wxFlatNotebook,
    // somehow the main panel is like "invisible" :/
    // so we force the correct color for the panel here...
    XRCCTRL(*this, "MainPanel", wxPanel)->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
}

// class destructor
ClassBrowser::~ClassBrowser()
{
    int pos = XRCCTRL(*this, "splitterWin", wxSplitterWindow)->GetSashPosition();
    Manager::Get()->GetConfigManager(_T("code_completion"))->Write(_T("/splitter_pos"), pos);

    SetParser(NULL);

    if (m_ClassBrowserBuilderThread)
    {
        m_ClassBrowserSemaphore.Post();
        // m_ClassBrowserBuilderThread->Delete(); --> would delete it twice and leads to a warning
        m_ClassBrowserBuilderThread->Wait();
    }
}

void ClassBrowser::SetParser(ParserBase* parser)
{
    if (m_Parser == parser)
        return;

    m_Parser = parser;
    if (m_Parser)
    {
        BrowserDisplayFilter filter = (BrowserDisplayFilter)XRCCTRL(*this, "cmbView", wxChoice)->GetSelection();
        if (!m_NativeParser->IsParserPerWorkspace() && filter == bdfWorkspace)
            filter = bdfProject;
        m_Parser->ClassBrowserOptions().displayFilter = filter;
        m_Parser->WriteOptions();
    }

    UpdateView();
}

void ClassBrowser::UpdateSash()
{
    int pos = Manager::Get()->GetConfigManager(_T("code_completion"))->ReadInt(_T("/splitter_pos"), 250);
    XRCCTRL(*this, "splitterWin", wxSplitterWindow)->SetSashPosition(pos, false);
}

void ClassBrowser::UpdateView(bool checkHeaderSwap)
{
    m_ActiveProject = 0;
    TRACE(_T("ClassBrowser::UpdateView(), the m_ActiveFilename = %s"), m_ActiveFilename.wx_str());
    wxString oldActiveFilename = m_ActiveFilename;
    m_ActiveFilename.Clear();

    if (m_Parser && !Manager::IsAppShuttingDown())
    {
        cbEditor* editor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
        if (editor)
            m_ActiveFilename = editor->GetFilename();

        if (!m_NativeParser->IsParserPerWorkspace())
            m_ActiveProject = m_NativeParser->GetProjectByParser(m_Parser);
        else
            m_ActiveProject = m_NativeParser->GetCurrentProject();

        TRACE(_T("ClassBrowser::UpdateView(), new m_ActiveFilename = %s"), m_ActiveFilename.wx_str());

        if (checkHeaderSwap)
        {
            wxString oldShortName = oldActiveFilename.AfterLast(wxFILE_SEP_PATH);
            if (oldShortName.Find(_T('.')) != wxNOT_FOUND)
                oldShortName = oldShortName.BeforeLast(_T('.'));

            wxString newShortName = m_ActiveFilename.AfterLast(wxFILE_SEP_PATH);
            if (newShortName.Find(_T('.')) != wxNOT_FOUND)
                newShortName = newShortName.BeforeLast(_T('.'));

            if (oldShortName.IsSameAs(newShortName))
            {
                TRACE(_T("ClassBrowser::UpdateView() match the old filename, return!"));
                return;
            }
        }

        BuildTree();

        wxSplitterWindow* splitter = XRCCTRL(*this, "splitterWin", wxSplitterWindow);
        if (m_Parser->ClassBrowserOptions().treeMembers)
        {
            splitter->SplitHorizontally(m_Tree, m_TreeBottom);
            m_TreeBottom->Show(true);
        }
        else
        {
            splitter->Unsplit();
            m_TreeBottom->Show(false);
        }
    }
}

void ClassBrowser::ShowMenu(wxTreeCtrl* tree, wxTreeItemId id, const wxPoint& pt)
{
// NOTE: local variables are tricky! If you build two local menus
// and attach menu B to menu A, on function exit both menu A and menu B
// will be destroyed. But when destroying menu A, menu B will be destroyed
// again. Its already-freed memory will be accessed, generating a segfault.

// A safer approach is to make all menus heap-based, and delete the topmost
// on exit.

    m_TreeForPopupMenu = tree;
    if ( !id.IsOk() || !m_Parser)
        return;

#if wxUSE_MENUS
    wxString caption;
    wxMenu *menu=new wxMenu(wxEmptyString);

    CBTreeData* ctd = (CBTreeData*)tree->GetItemData(id);
    if (ctd && ctd->m_Token)
    {
        switch (ctd->m_Token->m_TokenKind)
        {
            case tkConstructor:
            case tkDestructor:
            case tkFunction:
                if (ctd->m_Token->m_ImplLine != 0 && !ctd->m_Token->GetImplFilename().IsEmpty())
                    menu->Append(idMenuJumpToImplementation, _("Jump to &implementation"));
                // intentionally fall through
            default:
                menu->Append(idMenuJumpToDeclaration, _("Jump to &declaration"));
        }
    }

    const BrowserOptions& options = m_Parser->ClassBrowserOptions();
    if (tree == m_Tree)
    {
        // only in top tree
        if (menu->GetMenuItemCount() != 0)
            menu->AppendSeparator();

        menu->AppendCheckItem(idCBViewInheritance, _("Show inherited members"));
        menu->AppendCheckItem(idCBExpandNS, _("Auto-expand namespaces"));
        menu->Append(idMenuRefreshTree, _("&Refresh tree"));

        if (id == m_Tree->GetRootItem())
        {
            menu->AppendSeparator();
            menu->Append(idMenuForceReparse, _("Re-parse now"));
        }

        if (wxGetKeyState(WXK_CONTROL) && wxGetKeyState(WXK_SHIFT))
        {
            menu->AppendSeparator();
            menu->AppendCheckItem(idMenuDebugSmartSense, _("Debug SmartSense"));
            menu->Check(idMenuDebugSmartSense, s_DebugSmartSense);
        }

        menu->Check(idCBViewInheritance, m_Parser ? options.showInheritance : false);
        menu->Check(idCBExpandNS, m_Parser ? options.expandNS : false);
    }

    menu->AppendSeparator();
    menu->AppendCheckItem(idCBNoSort, _("Do not sort"));
    menu->AppendCheckItem(idCBSortByAlpabet, _("Sort alphabetically"));
    menu->AppendCheckItem(idCBSortByKind, _("Sort by kind"));
    menu->AppendCheckItem(idCBSortByScope, _("Sort by access"));

    const BrowserSortType& bst = options.sortType;
    switch (bst)
    {
        case bstAlphabet:
            menu->Check(idCBSortByAlpabet, true);
            break;
        case bstKind:
            menu->Check(idCBSortByKind, true);
            break;
        case bstScope:
            menu->Check(idCBSortByScope, true);
            break;
        case bstNone:
        default:
            menu->Check(idCBNoSort, true);
            break;
    }

    menu->AppendSeparator();
    menu->AppendCheckItem(idCBBottomTree, _("Display bottom tree"));
    menu->Check(idCBBottomTree, options.treeMembers);

    if (menu->GetMenuItemCount() != 0)
        PopupMenu(menu);

    delete menu; // Prevents memory leak
#endif // wxUSE_MENUS
}

bool ClassBrowser::FoundMatch(const wxString& search, wxTreeCtrl* tree, const wxTreeItemId& item)
{
    ClassTreeData* ctd = static_cast<ClassTreeData*>(tree->GetItemData(item));
    if (ctd && ctd->GetToken())
    {
        Token* token = ctd->GetToken();
        if (token->m_Name.Lower().StartsWith(search) ||
            token->m_Name.Lower().StartsWith(_T('~') + search))
        {
            return true;
        }
    }
    return false;
}

wxTreeItemId ClassBrowser::FindNext(const wxString& search, wxTreeCtrl* tree, const wxTreeItemId& start)
{
    wxTreeItemId ret;
    if (!start.IsOk())
        return ret;

    // look at siblings
    ret = tree->GetNextSibling(start);
    if (ret.IsOk())
        return ret;

    // ascend one level now and recurse
    return FindNext(search, tree, tree->GetItemParent(start));
}

wxTreeItemId ClassBrowser::FindChild(const wxString& search, wxTreeCtrl* tree, const wxTreeItemId& start, bool recurse, bool partialMatch)
{
    wxTreeItemIdValue cookie;
    wxTreeItemId res = tree->GetFirstChild(start, cookie);
    while (res.IsOk())
    {
        wxString text = tree->GetItemText(res);
        if ((!partialMatch && text == search) ||
            (partialMatch && text.StartsWith(search)))
        {
            return res;
        }
        if (recurse && tree->ItemHasChildren(res))
        {
            res = FindChild(search, tree, res, true, partialMatch);
            if (res.IsOk())
                return res;
        }
        res = m_Tree->GetNextChild(start, cookie);
    }
    res.Unset();
    return res;
}

bool ClassBrowser::RecursiveSearch(const wxString& search, wxTreeCtrl* tree, const wxTreeItemId& parent, wxTreeItemId& result)
{
    if (!parent.IsOk() || !tree)
        return false;

    // first check the parent item
    if (FoundMatch(search, tree, parent))
    {
        result = parent;
        return true;
    }

    wxTreeItemIdValue cookie;
    wxTreeItemId child = tree->GetFirstChild(parent, cookie);

    if (!child.IsOk())
        return RecursiveSearch(search, tree, FindNext(search, tree, parent), result);

    while (child.IsOk())
    {
        if (FoundMatch(search, tree, child))
        {
            result = child;
            return true;
        }
        if (tree->ItemHasChildren(child))
        {
            if (RecursiveSearch(search, tree, child, result))
                return true;
        }
        child = tree->GetNextChild(parent, cookie);
    }

    return RecursiveSearch(search, tree, FindNext(search, tree, parent), result);
}

// events

void ClassBrowser::OnTreeItemRightClick(wxTreeEvent& event)
{
    wxTreeCtrl* tree = (wxTreeCtrl*)event.GetEventObject();
    if (!tree)
        return;

    tree->SelectItem(event.GetItem());
    ShowMenu(tree, event.GetItem(), event.GetPoint());
}

void ClassBrowser::OnJumpTo(wxCommandEvent& event)
{
    wxTreeCtrl* tree = m_TreeForPopupMenu;
    if (!tree || !m_Parser)
        return;

    wxTreeItemId id = tree->GetSelection();
    CBTreeData* ctd = (CBTreeData*)tree->GetItemData(id);
    if (ctd)
    {
        wxFileName fname;
        if (event.GetId() == idMenuJumpToImplementation)
            fname.Assign(ctd->m_Token->GetImplFilename());
        else
            fname.Assign(ctd->m_Token->GetFilename());

        cbProject* project = nullptr;
        if (!m_NativeParser->IsParserPerWorkspace())
            project = m_NativeParser->GetProjectByParser(m_Parser);
        else
            project = m_NativeParser->GetCurrentProject();

        wxString base;
        if (project)
        {
            base = project->GetBasePath();
            NormalizePath(fname, base);
        }
        else
        {
            const wxArrayString& incDirs = m_Parser->GetIncludeDirs();
            for (size_t i = 0; i < incDirs.GetCount(); ++i)
            {
                if (NormalizePath(fname, incDirs.Item(i)))
                    break;
            }
        }

        cbEditor* ed = Manager::Get()->GetEditorManager()->Open(fname.GetFullPath());
        if (ed)
        {
            int line;
            if (event.GetId() == idMenuJumpToImplementation)
                line = ctd->m_Token->m_ImplLine - 1;
            else
                line = ctd->m_Token->m_Line - 1;

            ed->GotoTokenPosition(line, ctd->m_Token->m_Name);
        }
    }
}

void ClassBrowser::OnTreeItemDoubleClick(wxTreeEvent& event)
{
    wxTreeCtrl* tree = (wxTreeCtrl*)event.GetEventObject();
    if (!tree || !m_Parser)
        return;

    wxTreeItemId id = event.GetItem();
    CBTreeData* ctd = (CBTreeData*)tree->GetItemData(id);
    if (ctd && ctd->m_Token)
    {
        if (wxGetKeyState(WXK_CONTROL) && wxGetKeyState(WXK_SHIFT))
        {
            THREAD_LOCKER_ENTER(s_TokensTreeCritical);
            s_TokensTreeCritical.Enter();
            THREAD_LOCKER_ENTERED(s_TokensTreeCritical);

            CCDebugInfo info(tree, m_Parser, ctd->m_Token);
            info.ShowModal();

            THREAD_LOCKER_LEAVE(s_TokensTreeCritical);
            s_TokensTreeCritical.Leave();

            return;
        }

        bool toImp = false;
        switch (ctd->m_Token->m_TokenKind)
        {
        case tkConstructor:
        case tkDestructor:
        case tkFunction:
            if (ctd->m_Token->m_ImplLine != 0 && !ctd->m_Token->GetImplFilename().IsEmpty())
                toImp = true;
            break;
        default:
            break;
        }

        wxFileName fname;
        if (toImp)
            fname.Assign(ctd->m_Token->GetImplFilename());
        else
            fname.Assign(ctd->m_Token->GetFilename());

        cbProject* project = nullptr;
        if (!m_NativeParser->IsParserPerWorkspace())
            project = m_NativeParser->GetProjectByParser(m_Parser);
        else
            project = m_NativeParser->GetCurrentProject();

        wxString base;
        if (project)
        {
            base = project->GetBasePath();
            NormalizePath(fname, base);
        }
        else
        {
            const wxArrayString& incDirs = m_Parser->GetIncludeDirs();
            for (size_t i = 0; i < incDirs.GetCount(); ++i)
            {
                if (NormalizePath(fname, incDirs.Item(i)))
                    break;
            }
        }

        cbEditor* ed = Manager::Get()->GetEditorManager()->Open(fname.GetFullPath());
        if (ed)
        {
            int line;
            if (toImp)
                line = ctd->m_Token->m_ImplLine - 1;
            else
                line = ctd->m_Token->m_Line - 1;

            ed->GotoTokenPosition(line, ctd->m_Token->m_Name);
        }
    }
}

void ClassBrowser::OnRefreshTree(wxCommandEvent& /*event*/)
{
    UpdateView();
}

void ClassBrowser::OnForceReparse(wxCommandEvent& /*event*/)
{
    if (m_NativeParser)
        m_NativeParser->ReparseCurrentProject();
}

void ClassBrowser::OnCBViewMode(wxCommandEvent& event)
{
    if (!m_Parser)
        return;

    BrowserOptions& options = m_Parser->ClassBrowserOptions();

    if (event.GetId() == idCBViewInheritance)
        options.showInheritance = event.IsChecked();
    if (event.GetId() == idCBExpandNS)
        options.expandNS = event.IsChecked();
    if (event.GetId() == idCBBottomTree)
        options.treeMembers = event.IsChecked();

    m_Parser->WriteOptions();
    UpdateView();
}

void ClassBrowser::OnCBExpandNS(wxCommandEvent& event)
{
    if (!m_Parser)
        return;

    if (event.GetId() == idCBExpandNS)
        m_Parser->ClassBrowserOptions().expandNS = event.IsChecked();

    m_Parser->WriteOptions();
    UpdateView();
}

void ClassBrowser::OnViewScope(wxCommandEvent& event)
{
    if (m_Parser)
    {
        BrowserDisplayFilter filter = (BrowserDisplayFilter)event.GetSelection();
        if (!m_NativeParser->IsParserPerWorkspace() && filter == bdfWorkspace)
        {
            cbMessageBox(_("This feature is not supported in combination with\n"
                           "the option \"one parser per whole worspace\"."),
                         _("Information"), wxICON_INFORMATION);
            filter = bdfProject;
            XRCCTRL(*this, "cmbView", wxChoice)->SetSelection(filter);
        }

        m_Parser->ClassBrowserOptions().displayFilter = filter;
        m_Parser->WriteOptions();
        UpdateView();
    }
    else
    {
        // we have no parser; just write the setting in the configuration
        Manager::Get()->GetConfigManager(_T("code_completion"))->Write(_T("/browser_display_filter"), (int)event.GetSelection());
    }
}

void ClassBrowser::OnDebugSmartSense(wxCommandEvent& event)
{
    s_DebugSmartSense = !s_DebugSmartSense;
}

void ClassBrowser::OnSetSortType(wxCommandEvent& event)
{
    BrowserSortType bst;
    if (event.GetId() == idCBSortByAlpabet) bst = bstAlphabet;
    else if (event.GetId() == idCBSortByKind) bst = bstKind;
    else if (event.GetId() == idCBSortByScope) bst = bstScope;
    else bst = bstNone;

    if (m_Parser)
    {
        m_Parser->ClassBrowserOptions().sortType = bst;
        m_Parser->WriteOptions();
        UpdateView();
    }
    else
        Manager::Get()->GetConfigManager(_T("code_completion"))->Write(_T("/browser_sort_type"), (int)bst);
}

void ClassBrowser::OnSearch(wxCommandEvent& event)
{
    wxString search = m_Search->GetValue();
    if (search.IsEmpty() || !m_Parser)
        return;

    TokenIdxSet result;
    Token* token = 0;
    {
        TokensTree* tokensTree = m_Parser->GetTokensTree();
        size_t count = 0;
        {
            THREAD_LOCKER_ENTER(s_TokensTreeCritical);
            s_TokensTreeCritical.Enter();
            THREAD_LOCKER_ENTERED(s_TokensTreeCritical);

            count = tokensTree->FindMatches(search, result, false, true);

            THREAD_LOCKER_LEAVE(s_TokensTreeCritical);
            s_TokensTreeCritical.Leave();
        }

        if (count == 0)
        {
            cbMessageBox(_("No matches were found: ") + search,
                         _("Search failed"), wxICON_INFORMATION);
            return;
        }
        else if (count == 1)
        {
            THREAD_LOCKER_ENTER(s_TokensTreeCritical);
            s_TokensTreeCritical.Enter();
            THREAD_LOCKER_ENTERED(s_TokensTreeCritical);

            token = tokensTree->at(*result.begin());

            THREAD_LOCKER_LEAVE(s_TokensTreeCritical);
            s_TokensTreeCritical.Leave();
        }
        else if (count > 1)
        {
            wxArrayString selections;
            wxArrayInt int_selections;
            for (TokenIdxSet::iterator it = result.begin(); it != result.end(); ++it)
            {
                THREAD_LOCKER_ENTER(s_TokensTreeCritical);
                s_TokensTreeCritical.Enter();
                THREAD_LOCKER_ENTERED(s_TokensTreeCritical);

                Token* sel = tokensTree->at(*it);
                if (sel)
                {
                    selections.Add(sel->DisplayName());
                    int_selections.Add(*it);
                }

                THREAD_LOCKER_LEAVE(s_TokensTreeCritical);
                s_TokensTreeCritical.Leave();
            }
            if (selections.GetCount() > 1)
            {
                int sel = wxGetSingleChoiceIndex(_("Please make a selection:"), _("Multiple matches"), selections);
                if (sel == -1)
                    return;

                THREAD_LOCKER_ENTER(s_TokensTreeCritical);
                s_TokensTreeCritical.Enter();
                THREAD_LOCKER_ENTERED(s_TokensTreeCritical);

                token = tokensTree->at(int_selections[sel]);

                THREAD_LOCKER_LEAVE(s_TokensTreeCritical);
                s_TokensTreeCritical.Leave();
            }
            else if (selections.GetCount() == 1)
            {
                THREAD_LOCKER_ENTER(s_TokensTreeCritical);
                s_TokensTreeCritical.Enter();
                THREAD_LOCKER_ENTERED(s_TokensTreeCritical);

                // number of selections can be < result.size() due to the if tests, so in case we fall
                // back on 1 entry no need to show a selection
                token = tokensTree->at(int_selections[0]);

                THREAD_LOCKER_LEAVE(s_TokensTreeCritical);
                s_TokensTreeCritical.Leave();
            }
        }
    }

    // time to "walk" the tree
    if (token)
    {
        // store the search in the combobox
        if (m_Search->FindString(token->m_Name) == wxNOT_FOUND)
            m_Search->Append(token->m_Name);

        if (token->m_ParentIndex == -1 && !(token->m_TokenKind & tkAnyContainer))
        {
            // a global non-container: search in special folders only
            wxTreeItemIdValue cookie;
            wxTreeItemId res = m_Tree->GetFirstChild(m_Tree->GetRootItem(), cookie);
            while (res.IsOk())
            {
                CBTreeData* data = (CBTreeData*)m_Tree->GetItemData(res);
                if (data && (data->m_SpecialFolder & (sfGFuncs | sfGVars | sfPreproc | sfTypedef)))
                {
                    m_Tree->SelectItem(res);
                    wxTreeItemId srch = FindChild(token->m_Name, m_TreeBottom, m_TreeBottom->GetRootItem(), false, true);
                    if (srch.IsOk())
                    {
                        m_TreeBottom->SelectItem(srch);
                        return;
                    }
                }
                res = m_Tree->GetNextChild(m_Tree->GetRootItem(), cookie);
            }
            return;
        }

        // example:
        //   search="cou"
        //   token->GetNamespace()="std::"
        //   token->m_Name="cout"
        wxTreeItemId start = m_Tree->GetRootItem();
        wxStringTokenizer tkz(token->GetNamespace(), _T(":"));
        while (tkz.HasMoreTokens())
        {
            wxString part = tkz.GetNextToken();
            if (!part.IsEmpty())
            {
                m_Tree->Expand(start);
                wxTreeItemId res = FindChild(part, m_Tree, start);
                if (!res.IsOk())
                    break;
                start = res;
            }
        }
        // now the actual token
        m_Tree->Expand(start);
        m_Tree->SelectItem(start);
        wxTreeItemId res = FindChild(token->m_Name, m_Tree, start);
        if (res.IsOk())
            m_Tree->SelectItem(res);
        else
        {
            // search in bottom tree too
            wxTreeItemId res = FindChild(token->m_Name, m_TreeBottom, m_TreeBottom->GetRootItem(), true, true);
            if (res.IsOk())
                m_TreeBottom->SelectItem(res);
        }
    }
}

void ClassBrowser::BuildTree()
{
    if (Manager::IsAppShuttingDown() || !m_Parser)
        return;

    // tree shall only be  created in case of a new builder thread
    bool create_tree = false;

    // create the thread if needed
    if (!m_ClassBrowserBuilderThread)
    {
        m_ClassBrowserBuilderThread = new ClassBrowserBuilderThread(m_ClassBrowserSemaphore);
        m_ClassBrowserBuilderThread->Create();
        m_ClassBrowserBuilderThread->Run();
        create_tree = true; // new builder thread - need to create new tree
    }

    // initialise it
    m_ClassBrowserBuilderThread->Init(m_NativeParser,
                                      m_Tree,
                                      m_TreeBottom,
                                      m_ActiveFilename,
                                      m_ActiveProject,
                                      m_Parser->ClassBrowserOptions(),
                                      m_Parser->GetTokensTree(),
                                      create_tree,
                                      idCBMakeSelectItem);

    // and launch it
    if (!create_tree)
        m_ClassBrowserSemaphore.Post();
}

void ClassBrowser::OnTreeItemExpanding(wxTreeEvent& event)
{
    if (m_ClassBrowserBuilderThread)
        m_ClassBrowserBuilderThread->ExpandItem(event.GetItem());
#ifndef CC_NO_COLLAPSE_ITEM
    event.Allow();
#endif // CC_NO_COLLAPSE_ITEM
}

#ifndef CC_NO_COLLAPSE_ITEM
void ClassBrowser::OnTreeItemCollapsing(wxTreeEvent& event)
{
    if (m_ClassBrowserBuilderThread)
        m_ClassBrowserBuilderThread->CollapseItem(event.GetItem());
    event.Allow();
}
#endif // CC_NO_COLLAPSE_ITEM

void ClassBrowser::OnTreeItemSelected(wxTreeEvent& event)
{
    if (!::wxIsMainThread())
        return; // just to be sure it called from main thread

    if (m_ClassBrowserBuilderThread && m_Parser && m_Parser->ClassBrowserOptions().treeMembers)
        m_ClassBrowserBuilderThread->SelectItem(event.GetItem());
#ifndef CC_NO_COLLAPSE_ITEM
    event.Allow();
#endif // CC_NO_COLLAPSE_ITEM
}

void ClassBrowser::OnMakeSelectItem(wxCommandEvent& event)
{
    if (m_ClassBrowserBuilderThread && m_Parser && m_Parser->ClassBrowserOptions().treeMembers)
        m_ClassBrowserBuilderThread->SelectItemRequired();
}

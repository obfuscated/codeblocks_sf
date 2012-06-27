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
int idThreadEvent              = wxNewId();

BEGIN_EVENT_TABLE(ClassBrowser, wxPanel)
    EVT_TREE_ITEM_ACTIVATED  (XRCID("treeMembers"),      ClassBrowser::OnTreeItemDoubleClick)
    EVT_TREE_ITEM_RIGHT_CLICK(XRCID("treeMembers"),      ClassBrowser::OnTreeItemRightClick)

    EVT_TREE_ITEM_ACTIVATED  (XRCID("treeAll"),          ClassBrowser::OnTreeItemDoubleClick)
    EVT_TREE_ITEM_RIGHT_CLICK(XRCID("treeAll"),          ClassBrowser::OnTreeItemRightClick)
    EVT_TREE_ITEM_EXPANDING  (XRCID("treeAll"),          ClassBrowser::OnTreeItemExpanding)
#ifndef CC_NO_COLLAPSE_ITEM
    EVT_TREE_ITEM_COLLAPSING (XRCID("treeAll"),          ClassBrowser::OnTreeItemCollapsing)
#endif // CC_NO_COLLAPSE_ITEM
    EVT_TREE_SEL_CHANGED     (XRCID("treeAll"),          ClassBrowser::OnTreeSelChanged)

    EVT_TEXT_ENTER(XRCID("cmbSearch"),                   ClassBrowser::OnSearch)
    EVT_COMBOBOX  (XRCID("cmbSearch"),                   ClassBrowser::OnSearch)

    EVT_CHOICE(XRCID("cmbView"),                         ClassBrowser::OnViewScope)
    EVT_BUTTON(XRCID("btnSearch"),                       ClassBrowser::OnSearch)

    EVT_MENU(idMenuJumpToDeclaration,                    ClassBrowser::OnJumpTo)
    EVT_MENU(idMenuJumpToImplementation,                 ClassBrowser::OnJumpTo)
    EVT_MENU(idMenuRefreshTree,                          ClassBrowser::OnRefreshTree)
    EVT_MENU(idMenuForceReparse,                         ClassBrowser::OnForceReparse)
    EVT_MENU(idCBViewInheritance,                        ClassBrowser::OnCBViewMode)
    EVT_MENU(idCBExpandNS,                               ClassBrowser::OnCBExpandNS)
    EVT_MENU(idCBViewModeFlat,                           ClassBrowser::OnCBViewMode)
    EVT_MENU(idMenuDebugSmartSense,                      ClassBrowser::OnDebugSmartSense)
    EVT_MENU(idCBNoSort,                                 ClassBrowser::OnSetSortType)
    EVT_MENU(idCBSortByAlpabet,                          ClassBrowser::OnSetSortType)
    EVT_MENU(idCBSortByKind,                             ClassBrowser::OnSetSortType)
    EVT_MENU(idCBSortByScope,                            ClassBrowser::OnSetSortType)
    EVT_MENU(idCBBottomTree,                             ClassBrowser::OnCBViewMode)

    EVT_COMMAND(idThreadEvent, wxEVT_COMMAND_ENTER,      ClassBrowser::OnThreadEvent)
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
    wxXmlResource::Get()->LoadPanel(this, parent, _T("pnlCB"));
    m_Search = XRCCTRL(*this, "cmbSearch", wxComboBox);

    if (platform::windows)
        m_Search->SetWindowStyle(wxTE_PROCESS_ENTER); // it's a must on windows to catch EVT_TEXT_ENTER

    // Subclassed in XRC file, for reference see here: http://wiki.wxwidgets.org/Resource_Files
    m_CCTreeCtrl       = XRCCTRL(*this, "treeAll",     CCTreeCtrl);
    m_CCTreeCtrlBottom = XRCCTRL(*this, "treeMembers", CCTreeCtrl);

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("code_completion"));
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
        // tell the thread, that we want to terminate it, TestDestroy only works after Delete(), which should not
        // be used on joinable threads
        // if we disable the cc-plugin, we otherwise come to an infinite wait in the threads Entry()-function
        m_ClassBrowserBuilderThread->RequestTermination();
        // awake the thread
        m_ClassBrowserSemaphore.Post();
        // free the system-resources
        m_ClassBrowserBuilderThread->Wait();
        // according to the wxWidgets-documentation the wxThread object itself has to be deleted explicitely,
        // to free the memory, if it is created on the heap, this is not done by Wait()
        delete m_ClassBrowserBuilderThread;
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

    UpdateClassBrowserView();
}

void ClassBrowser::UpdateSash()
{
    int pos = Manager::Get()->GetConfigManager(_T("code_completion"))->ReadInt(_T("/splitter_pos"), 250);
    XRCCTRL(*this, "splitterWin", wxSplitterWindow)->SetSashPosition(pos, false);
}

void ClassBrowser::UpdateClassBrowserView(bool checkHeaderSwap)
{
    m_ActiveProject = 0;
    TRACE(_T("ClassBrowser::UpdateClassBrowserView(), m_ActiveFilename = %s"), m_ActiveFilename.wx_str());
    wxString oldActiveFilename = m_ActiveFilename;
    m_ActiveFilename.Clear();

    if (!m_Parser || Manager::IsAppShuttingDown())
        return;

    cbEditor* editor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (editor)
        m_ActiveFilename = editor->GetFilename();

    if (!m_NativeParser->IsParserPerWorkspace())
        m_ActiveProject = m_NativeParser->GetProjectByParser(m_Parser);
    else
        m_ActiveProject = m_NativeParser->GetCurrentProject();

    TRACE(_T("ClassBrowser::UpdateClassBrowserView(), new m_ActiveFilename = %s"), m_ActiveFilename.wx_str());

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
            TRACE(_T("ClassBrowser::UpdateClassBrowserView() match the old filename, return!"));
            return;
        }
    }

    ThreadedBuildTree(); // (Re-) create tree UI

    wxSplitterWindow* splitter = XRCCTRL(*this, "splitterWin", wxSplitterWindow);
    if (m_Parser->ClassBrowserOptions().treeMembers)
    {
        splitter->SplitHorizontally(m_CCTreeCtrl, m_CCTreeCtrlBottom);
        m_CCTreeCtrlBottom->Show(true);
    }
    else
    {
        splitter->Unsplit();
        m_CCTreeCtrlBottom->Show(false);
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
    if (!id.IsOk() || !m_Parser)
        return;

    wxString caption;
    wxMenu* menu = new wxMenu(wxEmptyString);

    CCTreeCtrlData* ctd = (CCTreeCtrlData*)tree->GetItemData(id);
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
    if (tree == m_CCTreeCtrl)
    {
        // only in top tree
        if (menu->GetMenuItemCount() != 0)
            menu->AppendSeparator();

        menu->AppendCheckItem(idCBViewInheritance, _("Show inherited members"));
        menu->AppendCheckItem(idCBExpandNS,        _("Auto-expand namespaces"));
        menu->Append         (idMenuRefreshTree,   _("&Refresh tree"));

        if (id == m_CCTreeCtrl->GetRootItem())
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
        menu->Check(idCBExpandNS,        m_Parser ? options.expandNS        : false);
    }

    menu->AppendSeparator();
    menu->AppendCheckItem(idCBNoSort,        _("Do not sort"));
    menu->AppendCheckItem(idCBSortByAlpabet, _("Sort alphabetically"));
    menu->AppendCheckItem(idCBSortByKind,    _("Sort by kind"));
    menu->AppendCheckItem(idCBSortByScope,   _("Sort by access"));

    const BrowserSortType& bst = options.sortType;
    switch (bst)
    {
        case bstAlphabet:
            menu->Check(idCBSortByAlpabet, true);
            break;
        case bstKind:
            menu->Check(idCBSortByKind,    true);
            break;
        case bstScope:
            menu->Check(idCBSortByScope,   true);
            break;
        case bstNone:
        default:
            menu->Check(idCBNoSort,        true);
            break;
    }

    menu->AppendSeparator();
    menu->AppendCheckItem(idCBBottomTree, _("Display bottom tree"));
    menu->Check(idCBBottomTree, options.treeMembers);

    if (menu->GetMenuItemCount() != 0)
        PopupMenu(menu);

    delete menu; // Prevents memory leak
}

bool ClassBrowser::FoundMatch(const wxString& search, wxTreeCtrl* tree, const wxTreeItemId& item)
{
    ClassTreeData* ctd = static_cast<ClassTreeData*>(tree->GetItemData(item));
    if (ctd && ctd->GetToken())
    {
        Token* token = ctd->GetToken();
        if (   token->m_Name.Lower().StartsWith(search)
            || token->m_Name.Lower().StartsWith(_T('~') + search) ) // C++ destructor
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
    if (!tree)
        return wxTreeItemId();

    wxTreeItemIdValue cookie;
    wxTreeItemId res = tree->GetFirstChild(start, cookie);
    while (res.IsOk())
    {
        wxString text = tree->GetItemText(res);
        if (   (!partialMatch && text == search)
            || ( partialMatch && text.StartsWith(search)) )
        {
            return res;
        }

        if (recurse && tree->ItemHasChildren(res))
        {
            res = FindChild(search, tree, res, true, partialMatch);
            if (res.IsOk())
                return res;
        }
        res = m_CCTreeCtrl->GetNextChild(start, cookie);
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
    CCTreeCtrlData* ctd = (CCTreeCtrlData*)tree->GetItemData(id);
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
    wxTreeCtrl* wx_tree = (wxTreeCtrl*)event.GetEventObject();
    if (!wx_tree || !m_Parser)
        return;

    wxTreeItemId id = event.GetItem();
    CCTreeCtrlData* ctd = (CCTreeCtrlData*)wx_tree->GetItemData(id);
    if (ctd && ctd->m_Token)
    {
        if (wxGetKeyState(WXK_CONTROL) && wxGetKeyState(WXK_SHIFT))
        {
//            TokensTree* tree = m_Parser->GetTokensTree(); // the one used inside CCDebugInfo

            CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

            CCDebugInfo info(wx_tree, m_Parser, ctd->m_Token);
            info.ShowModal();

            CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)

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
    UpdateClassBrowserView();
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
    UpdateClassBrowserView();
}

void ClassBrowser::OnCBExpandNS(wxCommandEvent& event)
{
    if (!m_Parser)
        return;

    if (event.GetId() == idCBExpandNS)
        m_Parser->ClassBrowserOptions().expandNS = event.IsChecked();

    m_Parser->WriteOptions();
    UpdateClassBrowserView();
}

void ClassBrowser::OnViewScope(wxCommandEvent& event)
{
    if (m_Parser)
    {
        BrowserDisplayFilter filter = (BrowserDisplayFilter)event.GetSelection();
        if (!m_NativeParser->IsParserPerWorkspace() && filter == bdfWorkspace)
        {
            cbMessageBox(_("This feature is not supported in combination with\n"
                           "the option \"one parser per whole workspace\"."),
                         _("Information"), wxICON_INFORMATION);
            filter = bdfProject;
            XRCCTRL(*this, "cmbView", wxChoice)->SetSelection(filter);
        }

        m_Parser->ClassBrowserOptions().displayFilter = filter;
        m_Parser->WriteOptions();
        UpdateClassBrowserView();
    }
    else
        // we have no parser; just write the setting in the configuration
        Manager::Get()->GetConfigManager(_T("code_completion"))->Write(_T("/browser_display_filter"), (int)event.GetSelection());
}

void ClassBrowser::OnDebugSmartSense(wxCommandEvent& event)
{
    s_DebugSmartSense = !s_DebugSmartSense;
}

void ClassBrowser::OnSetSortType(wxCommandEvent& event)
{
    BrowserSortType bst;
    if      (event.GetId() == idCBSortByAlpabet) bst = bstAlphabet;
    else if (event.GetId() == idCBSortByKind)    bst = bstKind;
    else if (event.GetId() == idCBSortByScope)   bst = bstScope;
    else                                         bst = bstNone;

    if (m_Parser)
    {
        m_Parser->ClassBrowserOptions().sortType = bst;
        m_Parser->WriteOptions();
        UpdateClassBrowserView();
    }
    else
        Manager::Get()->GetConfigManager(_T("code_completion"))->Write(_T("/browser_sort_type"), (int)bst);
}

void ClassBrowser::OnSearch(wxCommandEvent& event)
{
    wxString search = m_Search->GetValue();
    if (search.IsEmpty() || !m_Parser)
        return;

    TokensTree* tree = m_Parser->GetTokensTree();

    TokenIdxSet result;
    Token* token = 0;
    size_t count = 0;
    {
        CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

        count = tree->FindMatches(search, result, false, true);

        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)
    }

    if (count == 0)
    {
        cbMessageBox(_("No matches were found: ") + search,
                     _("Search failed"), wxICON_INFORMATION);
        return;
    }
    else if (count == 1)
    {
        CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

        token = tree->at(*result.begin());

        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)
    }
    else if (count > 1)
    {
        wxArrayString selections;
        wxArrayInt int_selections;
        for (TokenIdxSet::iterator it = result.begin(); it != result.end(); ++it)
        {
            CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

            Token* sel = tree->at(*it);
            if (sel)
            {
                selections.Add(sel->DisplayName());
                int_selections.Add(*it);
            }

            CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)
        }
        if (selections.GetCount() > 1)
        {
            int sel = wxGetSingleChoiceIndex(_("Please make a selection:"), _("Multiple matches"), selections);
            if (sel == -1)
                return;

            CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

            token = tree->at(int_selections[sel]);

            CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)
        }
        else if (selections.GetCount() == 1)
        {
            CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokensTreeMutex)

            // number of selections can be < result.size() due to the if tests, so in case we fall
            // back on 1 entry no need to show a selection
            token = tree->at(int_selections[0]);

            CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokensTreeMutex)
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
            wxTreeItemId res = m_CCTreeCtrl->GetFirstChild(m_CCTreeCtrl->GetRootItem(), cookie);
            while (res.IsOk())
            {
                CCTreeCtrlData* data = (CCTreeCtrlData*)m_CCTreeCtrl->GetItemData(res);
                if (data && (data->m_SpecialFolder & (sfGFuncs | sfGVars | sfPreproc | sfTypedef)))
                {
                    m_CCTreeCtrl->SelectItem(res);
                    wxTreeItemId srch = FindChild(token->m_Name, m_CCTreeCtrlBottom, m_CCTreeCtrlBottom->GetRootItem(), false, true);
                    if (srch.IsOk())
                    {
                        m_CCTreeCtrlBottom->SelectItem(srch);
                        return;
                    }
                }
                res = m_CCTreeCtrl->GetNextChild(m_CCTreeCtrl->GetRootItem(), cookie);
            }
            return;
        }

        // example:
        //   search="cou"
        //   token->GetNamespace()="std::"
        //   token->m_Name="cout"
        wxTreeItemId start = m_CCTreeCtrl->GetRootItem();
        wxStringTokenizer tkz(token->GetNamespace(), _T(":"));
        while (tkz.HasMoreTokens())
        {
            wxString part = tkz.GetNextToken();
            if (!part.IsEmpty())
            {
                m_CCTreeCtrl->Expand(start);
                wxTreeItemId res = FindChild(part, m_CCTreeCtrl, start);
                if (!res.IsOk())
                    break;
                start = res;
            }
        }
        // now the actual token
        m_CCTreeCtrl->Expand(start);
        m_CCTreeCtrl->SelectItem(start);
        wxTreeItemId res = FindChild(token->m_Name, m_CCTreeCtrl, start);
        if (res.IsOk())
            m_CCTreeCtrl->SelectItem(res);
        else
        {
            // search in bottom tree too
            wxTreeItemId res = FindChild(token->m_Name, m_CCTreeCtrlBottom, m_CCTreeCtrlBottom->GetRootItem(), true, true);
            if (res.IsOk())
                m_CCTreeCtrlBottom->SelectItem(res);
        }
    }
}

void ClassBrowser::ThreadedBuildTree()
{
    if (Manager::IsAppShuttingDown() || !m_Parser)
        return;

    TRACE(wxT("ClassBrowser: ThreadedBuildTree started."));

    // create the thread if needed
    bool thread_needs_run = false;
    if (!m_ClassBrowserBuilderThread)
    {
        m_ClassBrowserBuilderThread = new ClassBrowserBuilderThread(this, m_ClassBrowserSemaphore);
        m_ClassBrowserBuilderThread->Create();
        thread_needs_run = true; // just created, so surely need to run it
    }

    if (!thread_needs_run)
        TRACE(wxT("ClassBrowser: Pausing ClassBrowserBuilderThread..."));

    bool thread_needs_resume = false;
    while (   !thread_needs_run
           &&  m_ClassBrowserBuilderThread->IsAlive()
           &&  m_ClassBrowserBuilderThread->IsRunning()
           && !m_ClassBrowserBuilderThread->IsPaused() )
    {
        thread_needs_resume = true;
        m_ClassBrowserBuilderThread->Pause();
        wxMilliSleep(20); // allow processing
    }
    if (thread_needs_resume)
        TRACE(wxT("ClassBrowser: ClassBrowserBuilderThread: Paused."));

    // initialise it
    m_ClassBrowserBuilderThread->Init(m_NativeParser,
                                      m_CCTreeCtrl,
                                      m_CCTreeCtrlBottom,
                                      m_ActiveFilename,
                                      m_ActiveProject,
                                      m_Parser->ClassBrowserOptions(),
                                      m_Parser->GetTokensTree(),
                                      idThreadEvent);

    if      (thread_needs_run)
    {
        TRACE(wxT("ClassBrowser: Run ClassBrowserBuilderThread."));
        m_ClassBrowserBuilderThread->Run();        // run newly created thread
        m_ClassBrowserSemaphore.Post();            // ...and allow BuildTree
    }
    else if (thread_needs_resume)                  // no resume without run ;-)
    {
        if (   m_ClassBrowserBuilderThread->IsAlive()
            && m_ClassBrowserBuilderThread->IsPaused() )
        {
            TRACE(wxT("ClassBrowser: Resume ClassBrowserBuilderThread."));
            m_ClassBrowserBuilderThread->Resume(); // resume existing thread
            m_ClassBrowserSemaphore.Post();        // ...and allow BuildTree
        }
    }
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

void ClassBrowser::OnTreeSelChanged(wxTreeEvent& event)
{
    if (!::wxIsMainThread())
        return; // just to be sure it called from main thread

    if (m_ClassBrowserBuilderThread && m_Parser && m_Parser->ClassBrowserOptions().treeMembers)
        m_ClassBrowserBuilderThread->SelectItem(event.GetItem());
#ifndef CC_NO_COLLAPSE_ITEM
    event.Allow();
#endif // CC_NO_COLLAPSE_ITEM
}

void ClassBrowser::OnThreadEvent(wxCommandEvent& event)
{
    ClassBrowserBuilderThread::EThreadEvent query =
        static_cast<ClassBrowserBuilderThread::EThreadEvent>(event.GetInt());

    switch (query)
    {
      case ClassBrowserBuilderThread::selectItemRequired:
      {
          if (m_ClassBrowserBuilderThread && m_Parser && m_Parser->ClassBrowserOptions().treeMembers)
              m_ClassBrowserBuilderThread->SelectItemRequired();
          break;
      }
      case ClassBrowserBuilderThread::buildTreeStart:
      {
          CCLogger::Get()->DebugLog(wxT("Updating class browser..."));
          break;
      }
      case ClassBrowserBuilderThread::buildTreeEnd:
      {
          CCLogger::Get()->DebugLog(wxT("Class browser updated."));
          break;
      }
    }
}

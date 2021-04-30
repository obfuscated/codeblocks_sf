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

#include <stack>

#define CC_CLASS_BROWSER_DEBUG_OUTPUT 0

#if defined(CC_GLOBAL_DEBUG_OUTPUT)
    #if CC_GLOBAL_DEBUG_OUTPUT == 1
        #undef CC_CLASS_BROWSER_DEBUG_OUTPUT
        #define CC_CLASS_BROWSER_DEBUG_OUTPUT 1
    #elif CC_GLOBAL_DEBUG_OUTPUT == 2
        #undef CC_CLASS_BROWSER_DEBUG_OUTPUT
        #define CC_CLASS_BROWSER_DEBUG_OUTPUT 2
    #endif
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
int idMenuForceReparse         = wxNewId();
int idMenuDebugSmartSense      = wxNewId();
int idCBNoSort                 = wxNewId();
int idCBSortByAlpabet          = wxNewId();
int idCBSortByKind             = wxNewId();
int idCBSortByScope            = wxNewId();
int idCBSortByLine             = wxNewId();
int idCBBottomTree             = wxNewId();

/** the event ID which will be sent from worker thread to ClassBrowser */
int idThreadEvent              = wxNewId();

BEGIN_EVENT_TABLE(ClassBrowser, wxPanel)
    EVT_TREE_ITEM_ACTIVATED  (XRCID("treeMembers"),      ClassBrowser::OnTreeItemDoubleClick)
    EVT_TREE_ITEM_RIGHT_CLICK(XRCID("treeMembers"),      ClassBrowser::OnTreeItemRightClick)

    EVT_TREE_ITEM_ACTIVATED  (XRCID("treeAll"),          ClassBrowser::OnTreeItemDoubleClick)
    EVT_TREE_ITEM_RIGHT_CLICK(XRCID("treeAll"),          ClassBrowser::OnTreeItemRightClick)
    EVT_TREE_ITEM_EXPANDING  (XRCID("treeAll"),          ClassBrowser::OnTreeItemExpanding)
    EVT_TREE_SEL_CHANGED     (XRCID("treeAll"),          ClassBrowser::OnTreeSelChanged)

    EVT_TEXT_ENTER(XRCID("cmbSearch"),                   ClassBrowser::OnSearch)
    EVT_COMBOBOX  (XRCID("cmbSearch"),                   ClassBrowser::OnSearch)
    EVT_BUTTON(XRCID("btnSearch"),                       ClassBrowser::OnSearch)

    EVT_CHOICE(XRCID("cmbView"),                         ClassBrowser::OnViewScope)

    EVT_MENU(idMenuJumpToDeclaration,                    ClassBrowser::OnJumpTo)
    EVT_MENU(idMenuJumpToImplementation,                 ClassBrowser::OnJumpTo)
    EVT_MENU(idMenuRefreshTree,                          ClassBrowser::OnRefreshTree)
    EVT_MENU(idMenuForceReparse,                         ClassBrowser::OnForceReparse)
    EVT_MENU(idCBViewInheritance,                        ClassBrowser::OnCBViewMode)
    EVT_MENU(idCBExpandNS,                               ClassBrowser::OnCBExpandNS)
    EVT_MENU(idMenuDebugSmartSense,                      ClassBrowser::OnDebugSmartSense)
    EVT_MENU(idCBNoSort,                                 ClassBrowser::OnSetSortType)
    EVT_MENU(idCBSortByAlpabet,                          ClassBrowser::OnSetSortType)
    EVT_MENU(idCBSortByKind,                             ClassBrowser::OnSetSortType)
    EVT_MENU(idCBSortByScope,                            ClassBrowser::OnSetSortType)
    EVT_MENU(idCBSortByLine,                             ClassBrowser::OnSetSortType)
    EVT_MENU(idCBBottomTree,                             ClassBrowser::OnCBViewMode)

    // EVT_COMMAND(idThreadEvent, wxEVT_COMMAND_ENTER,      ClassBrowser::OnThreadEvent)
END_EVENT_TABLE()

// class constructor
ClassBrowser::ClassBrowser(wxWindow* parent, NativeParser* np) :
    m_NativeParser(np),
    m_targetTreeCtrl(nullptr),
    m_TreeForPopupMenu(nullptr),
    m_Parser(nullptr),
    m_ClassBrowserSemaphore(0, 1),  // initial count, max count
    m_ClassBrowserBuilderThread(nullptr)
{
    wxXmlResource::Get()->LoadPanel(this, parent, "pnlCB"); // panel class browser -> pnlCB
    m_Search = XRCCTRL(*this, "cmbSearch", wxComboBox);

    if (platform::windows)
        m_Search->SetWindowStyle(wxTE_PROCESS_ENTER); // it's a must on windows to catch EVT_TEXT_ENTER

    // Subclassed in XRC file, for reference see here: http://wiki.wxwidgets.org/Resource_Files
    m_CCTreeCtrl       = XRCCTRL(*this, "treeAll",     CCTreeCtrl);
    m_CCTreeCtrlBottom = XRCCTRL(*this, "treeMembers", CCTreeCtrl);

    // Registration of images
    m_CCTreeCtrl->SetImageList(m_NativeParser->GetImageList(16));
    m_CCTreeCtrlBottom->SetImageList(m_NativeParser->GetImageList(16));

    ConfigManager* cfg = Manager::Get()->GetConfigManager("code_completion");
    const int filter = cfg->ReadInt("/browser_display_filter", bdfFile);
    XRCCTRL(*this, "cmbView", wxChoice)->SetSelection(filter);

    XRCCTRL(*this, "splitterWin", wxSplitterWindow)->SetMinSize(wxSize(-1, 200));
    // if the classbrowser is put under the control of a wxFlatNotebook,
    // somehow the main panel is like "invisible" :/
    // so we force the correct colour for the panel here...
    XRCCTRL(*this, "MainPanel", wxPanel)->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
}

// class destructor
ClassBrowser::~ClassBrowser()
{
    const int pos = XRCCTRL(*this, "splitterWin", wxSplitterWindow)->GetSashPosition();
    Manager::Get()->GetConfigManager("code_completion")->Write("/splitter_pos", pos);

    SetParser(nullptr);

    if (m_ClassBrowserBuilderThread)
    {
        // tell the thread, that we want to terminate it, TestDestroy only works after Delete(), which should not
        // be used on joinable threads
        // if we disable the cc-plugin, we otherwise come to an infinite wait in the threads Entry()-function
        m_ClassBrowserBuilderThread->RequestTermination();
        // awake the thread so it can terminate
        m_ClassBrowserSemaphore.Post();
        // free the system-resources
        m_ClassBrowserBuilderThread->Wait();
        // according to the wxWidgets-documentation the wxThread object itself has to be deleted explicitly,
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
        const int sel = XRCCTRL(*this, "cmbView", wxChoice)->GetSelection();
        BrowserDisplayFilter filter = static_cast<BrowserDisplayFilter>(sel);
        if (!m_NativeParser->IsParserPerWorkspace() && filter == bdfWorkspace)
            filter = bdfProject;

        m_Parser->ClassBrowserOptions().displayFilter = filter;
        m_Parser->WriteOptions();
        UpdateClassBrowserView();
    }
    else
        CCLogger::Get()->DebugLog("SetParser: No parser available.");
}

void ClassBrowser::UpdateSash()
{
    const int pos = Manager::Get()->GetConfigManager("code_completion")->ReadInt("/splitter_pos", 250);
    XRCCTRL(*this, "splitterWin", wxSplitterWindow)->SetSashPosition(pos, false);
    XRCCTRL(*this, "splitterWin", wxSplitterWindow)->Refresh();
}

void ClassBrowser::UpdateClassBrowserView(bool checkHeaderSwap)
{
    TRACE("ClassBrowser::UpdateClassBrowserView(), m_ActiveFilename = %s", m_ActiveFilename);

    const wxString oldActiveFilename(m_ActiveFilename);
    m_ActiveFilename.Clear();

    if (!m_Parser || Manager::IsAppShuttingDown())
        return;

    cbEditor* editor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (editor)
        m_ActiveFilename = editor->GetFilename();

    TRACE("ClassBrowser::UpdateClassBrowserView(), new m_ActiveFilename = %s", m_ActiveFilename);

    if (checkHeaderSwap)
    {
        wxString oldShortName = oldActiveFilename.AfterLast(wxFILE_SEP_PATH);
        if (oldShortName.Find('.') != wxNOT_FOUND)
            oldShortName = oldShortName.BeforeLast('.');

        wxString newShortName = m_ActiveFilename.AfterLast(wxFILE_SEP_PATH);
        if (newShortName.Find('.') != wxNOT_FOUND)
            newShortName = newShortName.BeforeLast('.');

        if (oldShortName.IsSameAs(newShortName))
        {
            TRACE("ClassBrowser::UpdateClassBrowserView() match the old filename, return!");
            return;
        }
    }

    cbProject* activeProject = nullptr;
    if (!m_NativeParser->IsParserPerWorkspace())
        activeProject = m_NativeParser->GetProjectByParser(m_Parser);
    else
        activeProject = m_NativeParser->GetCurrentProject();

    if (!activeProject)
        CCLogger::Get()->DebugLog("ClassBrowser::UpdateClassBrowserView(): No active project available.");

    ThreadedBuildTree(activeProject); // (Re-) create tree UI

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

void ClassBrowser::ShowMenu(wxTreeCtrl* tree, wxTreeItemId id, cb_unused const wxPoint& pt)
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
            case tkNamespace:
            case tkClass:
            case tkEnum:
            case tkTypedef:
            case tkVariable:
            case tkEnumerator:
            case tkMacroDef:
            case tkMacroUse:
            case tkAnyContainer:
            case tkAnyFunction:
            case tkUndefined:
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
    menu->AppendCheckItem(idCBSortByLine,    _("Sort by line"));

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
        case bstLine:
            menu->Check(idCBSortByLine,    true);
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
        const Token* token = ctd->GetToken();
        if (   token->m_Name.Lower().StartsWith(search)
            || token->m_Name.Lower().StartsWith('~' + search) ) // C++ destructor
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

        cbProject* project;
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
            const int line = (event.GetId() == idMenuJumpToImplementation) ? (ctd->m_Token->m_ImplLine - 1) : (ctd->m_Token->m_Line - 1);
            ed->GotoTokenPosition(line, ctd->m_Token->m_Name);
        }
    }
}
/* NOTE (ollydbg#1#05/17/15): This function can directly access to the TokenTree, but I don't see
  any protector here, do we need one? In the meanwhile, the parserthread may be running, and the
  TokenTree could be updated. */
void ClassBrowser::OnTreeItemDoubleClick(wxTreeEvent& event)
{
    wxTreeCtrl* wx_tree = (wxTreeCtrl*)event.GetEventObject();
    if (!wx_tree || !m_Parser)
        return;

    wxTreeItemId id = event.GetItem();
    CCTreeCtrlData* ctd = (CCTreeCtrlData*)wx_tree->GetItemData(id);
    if (ctd && ctd->m_Token)
    {
        // when user double click on an item, also with CONTROL and SHIFT key pressed, then we
        // pop up a cc debugging dialog.
        if (wxGetKeyState(WXK_CONTROL) && wxGetKeyState(WXK_SHIFT))
        {
//            TokenTree* tree = m_Parser->GetTokenTree(); // the one used inside CCDebugInfo

            CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

            CCDebugInfo info(wx_tree, m_Parser, ctd->m_Token);
            PlaceWindow(&info);
            info.ShowModal();

            CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

            return;
        }

        // jump to the implementation line only if the token is a function, and has a valid
        // implementation field
        bool toImp = false;
        switch (ctd->m_Token->m_TokenKind)
        {
            case tkConstructor:
            case tkDestructor:
            case tkFunction:
                if (ctd->m_Token->m_ImplLine != 0 && !ctd->m_Token->GetImplFilename().IsEmpty())
                    toImp = true;
                break;
            case tkNamespace:
            case tkClass:
            case tkEnum:
            case tkTypedef:
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
            // our Token's line is zero based, but Scintilla's one based, so we need to adjust the
            // line number
            const int line = toImp ? (ctd->m_Token->m_ImplLine - 1) : (ctd->m_Token->m_Line - 1);
            ed->GotoTokenPosition(line, ctd->m_Token->m_Name);
        }
    }
}

void ClassBrowser::OnRefreshTree(cb_unused wxCommandEvent& event)
{
    UpdateClassBrowserView();
}

void ClassBrowser::OnForceReparse(cb_unused wxCommandEvent& event)
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
    const int sel = event.GetSelection();
    if (m_Parser)
    {
        BrowserDisplayFilter filter = static_cast <BrowserDisplayFilter> (sel);
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
    {
        // we have no parser; just write the setting in the configuration
        Manager::Get()->GetConfigManager("code_completion")->Write("/browser_display_filter", sel);
        CCLogger::Get()->DebugLog("OnViewScope: No parser available.");
    }
}

void ClassBrowser::OnDebugSmartSense(cb_unused wxCommandEvent& event)
{
    s_DebugSmartSense = !s_DebugSmartSense;
}

void ClassBrowser::OnSetSortType(wxCommandEvent& event)
{
    BrowserSortType bst;
    if      (event.GetId() == idCBSortByAlpabet) bst = bstAlphabet;
    else if (event.GetId() == idCBSortByKind)    bst = bstKind;
    else if (event.GetId() == idCBSortByScope)   bst = bstScope;
    else if (event.GetId() == idCBSortByLine)    bst = bstLine;
    else                                         bst = bstNone;

    if (m_Parser)
    {
        m_Parser->ClassBrowserOptions().sortType = bst;
        m_Parser->WriteOptions();
        UpdateClassBrowserView();
    }
    else
        Manager::Get()->GetConfigManager("code_completion")->Write("/browser_sort_type", (int)bst);
}

void ClassBrowser::OnSearch(cb_unused wxCommandEvent& event)
{
    wxString search = m_Search->GetValue();
    if (search.IsEmpty() || !m_Parser)
        return;

    TokenTree* tree = m_Parser->GetTokenTree();

    TokenIdxSet result;
    size_t count = 0;
    {
        CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

        count = tree->FindMatches(search, result, false, true);

        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)
    }

    const Token* token = nullptr;
    if (count == 0)
    {
        cbMessageBox(_("No matches were found: ") + search,
                     _("Search failed"), wxICON_INFORMATION);
        return;
    }
    else if (count == 1)
    {
        CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

        token = tree->at(*result.begin());

        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)
    }
    else if (count > 1)
    {
        wxArrayString selections;
        wxArrayInt int_selections;
        for (TokenIdxSet::iterator it = result.begin(); it != result.end(); ++it)
        {
            CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

            const Token* sel = tree->at(*it);
            if (sel)
            {
                selections.Add(sel->DisplayName());
                int_selections.Add(*it);
            }

            CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)
        }

        if (selections.GetCount() > 1)
        {
            const int sel = cbGetSingleChoiceIndex(_("Please make a selection:"), _("Multiple matches"), selections,
                                                   Manager::Get()->GetAppWindow(), wxSize(400, 400));
            if (sel == -1)
                return;

            CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

            token = tree->at(int_selections[sel]);

            CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)
        }
        else if (selections.GetCount() == 1)
        {
            CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

            // number of selections can be < result.size() due to the if tests, so in case we fall
            // back on 1 entry no need to show a selection
            token = tree->at(int_selections[0]);

            CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)
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
        wxStringTokenizer tkz(token->GetNamespace(), ":");
        while (tkz.HasMoreTokens())
        {
            const wxString part(tkz.GetNextToken());
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
        {
            m_CCTreeCtrl->SelectItem(res);
        }
        else
        {
            // search in bottom tree too
            res = FindChild(token->m_Name, m_CCTreeCtrlBottom, m_CCTreeCtrlBottom->GetRootItem(), true, true);
            if (res.IsOk())
                m_CCTreeCtrlBottom->SelectItem(res);
        }
    }
}

/* There are several cases:
 A: If the worker thread is not created yet, just create one, and build the tree.
 B: If the worker thread is already created
    B1: the thread is running, then we need to pause it, and  re-initialize it and rebuild the tree.
    B2: if the thread is already paused, then we only need to resume it again.
*/
void ClassBrowser::ThreadedBuildTree(cbProject* activeProject)
{
    if (Manager::IsAppShuttingDown() || !m_Parser)
        return;

    TRACE("ClassBrowser: ThreadedBuildTree started.");

    // create the thread if needed
    bool thread_needs_run = false;
    if (!m_ClassBrowserBuilderThread)
    {
        m_ClassBrowserBuilderThread = new ClassBrowserBuilderThread(this, m_ClassBrowserSemaphore);
        m_ClassBrowserBuilderThread->Create();
        thread_needs_run = true; // just created, so surely need to run it
    }

    if (!thread_needs_run) // this means a worker thread is already created
    {
        TRACE("ClassBrowser: Pausing ClassBrowserBuilderThread...");
    }

    // whether the thread is running or paused, we try to pause the tree
    // this is an infinite loop, the loop only exists when the thread is actually paused
    bool thread_needs_resume = false;
    while (   !thread_needs_run  // the thread already created
           &&  m_ClassBrowserBuilderThread->IsAlive()     // thread is alive: i.e. running or suspended
           &&  m_ClassBrowserBuilderThread->IsRunning()   // running
           && !m_ClassBrowserBuilderThread->IsPaused() )  // not paused
    {
        thread_needs_resume = true;
        m_ClassBrowserBuilderThread->Pause();
        wxMilliSleep(20); // allow processing
    }

    // there are two conditions here:
    // 1, the thread is newly created, but hasn't run yet
    // 2, the thread is already created, and we have paused it
    if (thread_needs_resume) // satisfy the above condition 2
    {
        TRACE("ClassBrowser: ClassBrowserBuilderThread: Paused.");
    }

    // initialise it, this function is called from the GUI main thread.
    m_ClassBrowserBuilderThread->Init(m_NativeParser,
                                      m_ActiveFilename,
                                      activeProject,
                                      m_Parser->ClassBrowserOptions(),
                                      m_Parser->GetTokenTree(),
                                      idThreadEvent);

    // when m_ClassBrowserSemaphore.Post(), the worker thread has chance to build the tree
    if      (thread_needs_run)
    {
        TRACE("ClassBrowser: Run ClassBrowserBuilderThread.");
        m_ClassBrowserBuilderThread->Run();                    // run newly created thread
        m_ClassBrowserBuilderThread->SetNextJob(JobBuildTree); // ask to build the tree
        m_ClassBrowserSemaphore.Post();                        // ...and start job
    }
    else if (thread_needs_resume)                          // no resume without run ;-)
    {
        if (   m_ClassBrowserBuilderThread->IsAlive()
            && m_ClassBrowserBuilderThread->IsPaused() )
        {
            TRACE("ClassBrowser: Resume ClassBrowserBuilderThread.");
            m_ClassBrowserBuilderThread->Resume();                 // resume existing thread
            m_ClassBrowserBuilderThread->SetNextJob(JobBuildTree); // ask to build the tree
            m_ClassBrowserSemaphore.Post();                        // ...and start job
        }
    }
}

void ClassBrowser::OnTreeItemExpanding(wxTreeEvent& event)
{
    if (m_ClassBrowserBuilderThread && !m_ClassBrowserBuilderThread->IsBusy())  // targets can't be changed while busy
    {
        if (event.GetItem().IsOk() && !m_CCTreeCtrl->GetChildrenCount(event.GetItem(), false))
        {
            m_targetNode = event.GetItem();
            m_targetTreeCtrl = m_CCTreeCtrl;
            m_ClassBrowserBuilderThread->SetNextJob(JobExpandItem, GetItemPtr(m_targetNode));
            m_ClassBrowserSemaphore.Post();
        }
    }
}

void ClassBrowser::OnTreeSelChanged(wxTreeEvent& event)
{
    if (m_ClassBrowserBuilderThread && m_Parser && m_Parser->ClassBrowserOptions().treeMembers)
    {
        m_ClassBrowserBuilderThread->SetNextJob(JobSelectTree, GetItemPtr(event.GetItem()));
        m_ClassBrowserSemaphore.Post();
    }
}

void ClassBrowser::SetNodeProperties(CCTreeItem* Item)
{
    m_targetTreeCtrl->SetItemHasChildren(m_targetNode, Item->m_hasChildren);
    m_targetTreeCtrl->SetItemBold(m_targetNode, Item->m_bold);
    m_targetTreeCtrl->SetItemTextColour(m_targetNode, Item->m_colour);
    m_targetTreeCtrl->SetItemImage(m_targetNode, Item->m_image[wxTreeItemIcon_Normal],           wxTreeItemIcon_Normal);
    m_targetTreeCtrl->SetItemImage(m_targetNode, Item->m_image[wxTreeItemIcon_Selected],         wxTreeItemIcon_Selected);
    m_targetTreeCtrl->SetItemImage(m_targetNode, Item->m_image[wxTreeItemIcon_Expanded],         wxTreeItemIcon_Expanded);
    m_targetTreeCtrl->SetItemImage(m_targetNode, Item->m_image[wxTreeItemIcon_SelectedExpanded], wxTreeItemIcon_SelectedExpanded);
    if (Item->m_data)
    {
        // Link wxTreeCtrl item with the mirror CCTree item
        Item->m_data->m_MirrorNode = Item;
        m_targetTreeCtrl->SetItemData(m_targetNode, new CCTreeCtrlData(*(Item->m_data)));
    }
}

CCTreeItem* ClassBrowser::GetItemPtr(wxTreeItemId ItemId)
{
    if (!ItemId.IsOk())
        return nullptr;

    CCTreeCtrlData* tcd = static_cast <CCTreeCtrlData*> (m_CCTreeCtrl->GetItemData(ItemId));
    if (!tcd)
        return nullptr;

    return static_cast <CCTreeItem*> (tcd->m_MirrorNode);
}

////////////////////////////////////////////////////////////////////////
// The methods below are called from the worker thread using CallAfter()
////////////////////////////////////////////////////////////////////////

void ClassBrowser::BuildTreeStartOrStop(bool start)
{
    if (start)
        CCLogger::Get()->DebugLog("Updating class browser...");
    else
        CCLogger::Get()->DebugLog("Class browser updated");
}

void ClassBrowser::SelectTargetTree(bool top)
{
    m_targetTreeCtrl = top ? m_CCTreeCtrl : m_CCTreeCtrlBottom;
    m_targetNode.Unset();
}

void ClassBrowser::TreeOperation(ETreeOperator op, CCTreeItem* item)
{
    wxTreeItemId root;

    if (!m_targetTreeCtrl)
      return;

    switch (op)
      {
      case OpClear:
          m_targetTreeCtrl->Freeze();
          m_targetTreeCtrl->DeleteAllItems();
          m_targetNode.Unset();
          break;
      case OpAddRoot:
          // Only add it if there is no root. At the end m_targetNode always points to the root node
          m_targetNode = m_targetTreeCtrl->GetRootItem();
          if (!m_targetNode.IsOk() && item)
          {
              m_targetNode = m_targetTreeCtrl->AddRoot(item->m_text,
                                                       item->m_image[wxTreeItemIcon_Normal],
                                                       item->m_image[wxTreeItemIcon_Selected],
                                                       item->m_data);
              SetNodeProperties(item);
          }
          break;
      case OpAddChild:
          if (m_targetNode.IsOk() && item)
          {
              m_targetTreeCtrl->SetItemHasChildren(m_targetNode);
              m_targetNode = m_targetTreeCtrl->AppendItem(m_targetNode,
                                                          item->m_text,
                                                          item->m_image[wxTreeItemIcon_Normal],
                                                          item->m_image[wxTreeItemIcon_Selected],
                                                          item->m_data);
              SetNodeProperties(item);
              item->m_semaphore.Post();
          }
          break;
      case OpGoUp:
          if (m_targetNode.IsOk())
              m_targetNode = m_targetTreeCtrl->GetItemParent(m_targetNode);
          break;
      case OpExpandCurrent:
          if (m_targetNode.IsOk())
              m_targetTreeCtrl->Expand(m_targetNode);
          break;
      case OpExpandRoot:
          root = m_targetTreeCtrl->GetRootItem();
          if (root.IsOk())
              m_targetTreeCtrl->Expand(m_targetTreeCtrl->GetRootItem());
          break;
      case OpExpandAll:
          m_targetTreeCtrl->ExpandAll();
          break;
      case OpShowFirst:
          root = m_targetTreeCtrl->GetRootItem();
          if (root.IsOk())
          {
              wxTreeItemIdValue cookie;
              wxTreeItemId first = m_targetTreeCtrl->GetFirstChild(root, cookie);
              if (first.IsOk())
                  m_targetTreeCtrl->ScrollTo(first);
          }
          break;
      case OpEnd:
          m_targetTreeCtrl->Thaw();
      }
}

void ClassBrowser::SaveSelectedItem()
{
#ifdef CC_BUILDTREE_MEASURING
    wxStopWatch sw;
#endif

    m_SelectedPath.clear();
    wxTreeItemId item = m_CCTreeCtrl->GetSelection();
    while (item.IsOk() && item != m_CCTreeCtrl->GetRootItem())
    {
        CCTreeCtrlData* data = static_cast <CCTreeCtrlData*> (m_CCTreeCtrl->GetItemData(item));
        m_SelectedPath.push_front(*data);
        item = m_CCTreeCtrl->GetItemParent(item);
    }

#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F("SaveSelectedItem() took : %ld ms for %u items", sw.Time(), m_CCTreeCtrl->GetCount()));
#endif
}

void ClassBrowser::SelectSavedItem()
{
#ifdef CC_BUILDTREE_MEASURING
    wxStopWatch sw;
#endif

    wxTreeItemId parent = m_CCTreeCtrl->GetRootItem();
    if (!parent.IsOk())
        return;

    wxTreeItemIdValue cookie;
    wxTreeItemId item = m_CCTreeCtrl->GetFirstChild(parent, cookie);
    while (!m_SelectedPath.empty() && item.IsOk())
    {
        CCTreeCtrlData* data  = static_cast<CCTreeCtrlData*>(m_CCTreeCtrl->GetItemData(item));
        CCTreeCtrlData* saved = &m_SelectedPath.front();

        if (   data->m_SpecialFolder == saved->m_SpecialFolder
            && wxStrcmp(data->m_TokenName, saved->m_TokenName) == 0
            && data->m_TokenKind == saved->m_TokenKind )
        {
            wxTreeItemIdValue cookie2;
            parent = item;
            item   = m_CCTreeCtrl->GetFirstChild(item, cookie2);
            m_SelectedPath.pop_front();
        }
        else
            item = m_CCTreeCtrl->GetNextSibling(item);
    }

    if (parent.IsOk() && m_ClassBrowserBuilderThread && m_Parser && m_Parser->ClassBrowserOptions().treeMembers)
    {
        m_CCTreeCtrl->SelectItem(parent);
        m_CCTreeCtrl->EnsureVisible(parent);
    }

#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F("SelectSavedItem() took : %ld ms for %u items", sw.Time(), m_CCTreeCtrl->GetCount()));
#endif
}

void ClassBrowser::ReselectItem()
{
    if (m_ClassBrowserBuilderThread && m_Parser && m_Parser->ClassBrowserOptions().treeMembers)
    {
        wxTreeItemId item = m_CCTreeCtrl->GetFocusedItem();
        if (item.IsOk())
        {
            m_ClassBrowserBuilderThread->SetNextJob(JobSelectTree, GetItemPtr(item));
            m_ClassBrowserSemaphore.Post();
        }
        else
            m_CCTreeCtrlBottom->DeleteAllItems();
    }
}

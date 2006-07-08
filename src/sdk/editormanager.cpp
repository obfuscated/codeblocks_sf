/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/notebook.h>
    #include <wx/menu.h>
    #include <wx/splitter.h>
    #include <wx/imaglist.h>

    #include "editormanager.h" // class's header file
    #include "configmanager.h"
    #include <wx/xrc/xmlres.h>
    #include "messagemanager.h"
    #include "projectmanager.h"
    #include "pluginmanager.h"
    #include "manager.h"
    #include "sdk_events.h"
    #include "projectbuildtarget.h"
    #include "cbproject.h"
    #include "cbeditor.h"
    #include "globals.h"
    #include "xtra_classes.h"
    #include "sdk_events.h"
    #include <wx/file.h>
    #include <wx/dir.h>
#endif

#include <wx/bmpbuttn.h>
#include <wx/progdlg.h>
#include <wx/fontutil.h>

#include "editorcolourset.h"
#include "editorconfigurationdlg.h"
#include "finddlg.h"
#include "replacedlg.h"
#include "confirmreplacedlg.h"
#include "filefilters.h"
#include "searchresultslog.h"

#include "wxFlatNotebook/wxFlatNotebook.h"


//#define DONT_USE_OPENFILES_TREE

int ID_NBEditorManager = wxNewId();
int ID_EditorManager = wxNewId();
int idEditorManagerCheckFiles = wxNewId();

// static
bool EditorManager::s_CanShutdown = true;

struct cbFindReplaceData
{
    int start;
    int end;
    wxString findText;
    wxString replaceText;
    bool initialreplacing;
    bool findInFiles;
    bool delOldSearches;
    bool matchWord;
    bool startWord;
    bool matchCase;
    bool regEx;
    bool directionDown;
    bool originEntireScope;
    int scope;
    wxString searchPath;
    wxString searchMask;
    bool recursiveSearch;
    bool hiddenSearch;
    bool NewSearch;     //!< only true when a new search has been started
    int SearchInSelectionStart; //!< keep track of the start of a 'search' selection
    int SearchInSelectionEnd;  //!< keep track of the end of a 'search' selection
};

static const int idNBTabSplitHorz = wxNewId();
static const int idNBTabSplitVert = wxNewId();
static const int idNBTabUnsplit = wxNewId();
static const int idNBTabClose = wxNewId();
static const int idNBTabCloseAll = wxNewId();
static const int idNBTabCloseAllOthers = wxNewId();
static const int idNBTabSave = wxNewId();
static const int idNBTabSaveAll = wxNewId();
static const int idNBSwapHeaderSource = wxNewId();
static const int idNBTabTop = wxNewId();
static const int idNBTabBottom = wxNewId();
static const int idNB = wxNewId();

/** *******************************************************
  * struct EditorManagerInternalData                      *
  * This is the private data holder for the EditorManager *
  * All data not relevant to other classes should go here *
  ********************************************************* */

struct EditorManagerInternalData
{
    /* Methods */

    EditorManagerInternalData(EditorManager* owner)
            : m_pOwner(owner),
            m_NeedsRefresh(false),
            m_TreeNeedsRefresh(false),
            m_pImages(0),
            m_SetFocusFlag(false)
    {}

    void BuildTree(wxTreeCtrl* pTree)
    {
        wxBitmap bmp;
        m_pImages = new wxImageList(16, 16);
        wxString prefix = ConfigManager::GetDataFolder() + _T("/images/");
        bmp.LoadFile(prefix + _T("folder_open.png"), wxBITMAP_TYPE_PNG); // folder
        m_pImages->Add(bmp);
        bmp.LoadFile(prefix + _T("ascii.png"), wxBITMAP_TYPE_PNG); // file
        m_pImages->Add(bmp);
        bmp.LoadFile(prefix + _T("modified_file.png"), wxBITMAP_TYPE_PNG); // modified file
        m_pImages->Add(bmp);
        pTree->SetImageList(m_pImages);
        m_TreeOpenedFiles=pTree->AddRoot(_T("Opened Files"), 0, 0);
        pTree->SetItemBold(m_TreeOpenedFiles);
    }

    void InvalidateTree()
    {
        m_TreeNeedsRefresh = true;
    }

    /* Static data */

    EditorManager* m_pOwner;

    /* Used for refreshing the notebook if necessary */
    bool m_NeedsRefresh;
    bool m_TreeNeedsRefresh;
    wxImageList* m_pImages;
    wxTreeItemId m_TreeOpenedFiles;

    bool m_SetFocusFlag;
};

// *********** End of EditorManagerInternalData **********


BEGIN_EVENT_TABLE(EditorManager, wxEvtHandler)
    EVT_APP_STARTUP_DONE(EditorManager::OnAppDoneStartup)
    EVT_APP_START_SHUTDOWN(EditorManager::OnAppStartShutdown)
    EVT_FLATNOTEBOOK_PAGE_CHANGED(ID_NBEditorManager, EditorManager::OnPageChanged)
    EVT_FLATNOTEBOOK_PAGE_CHANGING(ID_NBEditorManager, EditorManager::OnPageChanging)
    EVT_FLATNOTEBOOK_PAGE_CLOSING(ID_NBEditorManager, EditorManager::OnPageClosing)
    EVT_FLATNOTEBOOK_CONTEXT_MENU(ID_NBEditorManager, EditorManager::OnPageContextMenu)
    EVT_MENU(idNBTabSplitHorz, EditorManager::OnGenericContextMenuHandler)
    EVT_MENU(idNBTabSplitVert, EditorManager::OnGenericContextMenuHandler)
    EVT_MENU(idNBTabUnsplit, EditorManager::OnGenericContextMenuHandler)
    EVT_MENU(idNBTabTop, EditorManager::OnTabPosition)
    EVT_MENU(idNBTabBottom, EditorManager::OnTabPosition)
    EVT_MENU(idNBTabClose, EditorManager::OnClose)
    EVT_MENU(idNBTabCloseAll, EditorManager::OnCloseAll)
    EVT_MENU(idNBTabCloseAllOthers, EditorManager::OnCloseAllOthers)
    EVT_MENU(idNBTabSave, EditorManager::OnSave)
    EVT_MENU(idNBTabSaveAll, EditorManager::OnSaveAll)
    EVT_MENU(idNBSwapHeaderSource, EditorManager::OnSwapHeaderSource)
    EVT_MENU(idEditorManagerCheckFiles, EditorManager::OnCheckForModifiedFiles)
#ifdef USE_OPENFILES_TREE
    EVT_UPDATE_UI(ID_EditorManager, EditorManager::OnUpdateUI)
    EVT_TREE_ITEM_ACTIVATED(ID_EditorManager, EditorManager::OnTreeItemActivated)
    EVT_TREE_ITEM_RIGHT_CLICK(ID_EditorManager, EditorManager::OnTreeItemRightClick)
#endif
END_EVENT_TABLE()

// class constructor
EditorManager::EditorManager()
        : m_pNotebook(0L),
        m_LastFindReplaceData(0L),
        m_pTree(0L),
        m_LastActiveFile(_T("")),
        m_LastModifiedflag(false),
        m_pSearchLog(0),
        m_SearchLogIndex(-1),
        m_SashPosition(150), // no longer used
        m_isCheckingForExternallyModifiedFiles(false)
{
    m_pData = new EditorManagerInternalData(this);

    m_pNotebook = new wxFlatNotebook(Manager::Get()->GetAppWindow(), ID_NBEditorManager, wxDefaultPosition, wxDefaultSize, wxNO_FULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN);
    m_pNotebook->SetWindowStyleFlag(Manager::Get()->GetConfigManager(_T("app"))->ReadInt(_T("/environment/editor_tabs_style"), wxFNB_DEFAULT_STYLE | wxFNB_MOUSE_MIDDLE_CLOSES_TABS));

#ifdef USE_OPENFILES_TREE
    m_pData->m_TreeNeedsRefresh = false;
    ShowOpenFilesTree(Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/show_opened_files_tree"), true));
#endif

Manager::Get()->GetMessageManager()->DebugLog(_T("Initialize EditColourSet ....."));
    m_Theme = new EditorColourSet(Manager::Get()->GetConfigManager(_T("editor"))->Read(_T("/colour_sets/active_colour_set"), COLORSET_DEFAULT));
Manager::Get()->GetMessageManager()->DebugLog(_T("Initialize EditColourSet: done."));

    Manager::Get()->GetAppWindow()->PushEventHandler(this);

    CreateSearchLog();
    LoadAutoComplete();
    m_zoom = Manager::Get()->GetConfigManager(_T("editor"))->ReadInt(_T("/zoom"));
}

// class destructor
EditorManager::~EditorManager()
{
    SaveAutoComplete();

    if (m_pTree)
    {
        CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
        evt.pWindow = m_pTree;
        Manager::Get()->GetAppWindow()->ProcessEvent(evt);
        m_pTree->Destroy();
    }

    delete m_Theme;
    delete m_LastFindReplaceData;
    delete m_pData->m_pImages;
    delete m_pData;
    Manager::Get()->GetConfigManager(_T("editor"))->Write(_T("/zoom"), m_zoom);
} // end of destructor

void EditorManager::CreateMenu(wxMenuBar* menuBar)
{
}

void EditorManager::ReleaseMenu(wxMenuBar* menuBar)
{
}

void EditorManager::Configure()
{
    // editor lexers loading takes some time; better reflect this with a hourglass
    wxBeginBusyCursor();

    EditorConfigurationDlg dlg(Manager::Get()->GetAppWindow());
    PlaceWindow(&dlg);

    // done, restore pointer
    wxEndBusyCursor();

    if (dlg.ShowModal() == wxID_OK)
    {
        // tell all open editors to re-create their styles
        for (int i = 0; i < m_pNotebook->GetPageCount(); ++i)
        {
            cbEditor* ed = InternalGetBuiltinEditor(i);
            if (ed)
                ed->SetEditorStyle();
        }
        RebuildOpenedFilesTree(0); // maybe the tab text naming changed
    }
}

void EditorManager::CreateSearchLog()
{
    wxArrayString titles;
    int widths[3] =
        {
            128, 48, 640
        };
    titles.Add(_("File"));
    titles.Add(_("Line"));
    titles.Add(_("Text"));

    m_pSearchLog = new SearchResultsLog(3, widths, titles);
    m_SearchLogIndex = LOGGER->AddLog(m_pSearchLog, _("Search results"));

    // Set search log font & size like all other logs         //pecan 2006/5/22
    m_pSearchLog->ResetLogFont();

    // set log image
    wxBitmap bmp;
    wxString prefix = ConfigManager::GetDataFolder() + _T("/images/16x16/");
    bmp.LoadFile(prefix + _T("filefind.png"), wxBITMAP_TYPE_PNG);
    Manager::Get()->GetMessageManager()->SetLogImage(m_pSearchLog, bmp);
}

void EditorManager::LogSearch(const wxString& file, int line, const wxString& lineText)
{
    wxArrayString values;
    wxString lineTextL;
    wxString lineStr;

    // line number -1 is used for empty string
    if( line != -1)
    {
        lineStr.Printf(_T("%d"), line);
    }
    else
    {
        lineStr.Printf(_T(" "));
    }
    lineTextL = lineText;
    lineTextL.Replace(_T("\t"), _T(" "));
    lineTextL.Replace(_T("\r"), _T(" "));
    lineTextL.Replace(_T("\n"), _T(" "));
    lineTextL.Trim(false);
    lineTextL.Trim(true);

    values.Add(file);
    values.Add(lineStr);
    values.Add(lineTextL);

    m_pSearchLog->AddLog(values);
    m_pSearchLog->GetListControl()->SetColumnWidth(0, wxLIST_AUTOSIZE);
    m_pSearchLog->GetListControl()->SetColumnWidth(2, wxLIST_AUTOSIZE);
}

void EditorManager::LoadAutoComplete()
{
    m_AutoCompleteMap.clear();
    wxArrayString list = Manager::Get()->GetConfigManager(_T("editor"))->EnumerateSubPaths(_T("/auto_complete"));
    for (unsigned int i = 0; i < list.GetCount(); ++i)
    {
        wxString name = Manager::Get()->GetConfigManager(_T("editor"))->Read(_T("/auto_complete/") + list[i] + _T("/name"), wxEmptyString);
        wxString code = Manager::Get()->GetConfigManager(_T("editor"))->Read(_T("/auto_complete/") + list[i] + _T("/code"), wxEmptyString);
        if (name.IsEmpty() || code.IsEmpty())
            continue;
        // convert non-printable chars to printable
        code.Replace(_T("\\n"), _T("\n"));
        code.Replace(_T("\\r"), _T("\r"));
        code.Replace(_T("\\t"), _T("\t"));
        m_AutoCompleteMap[name] = code;
    }

    if (m_AutoCompleteMap.size() == 0)
    {
        // default auto-complete items
        m_AutoCompleteMap[_T("if")] = _T("if (|)\n\t;");
        m_AutoCompleteMap[_T("ifb")] = _T("if (|)\n{\n\t\n}");
        m_AutoCompleteMap[_T("ife")] = _T("if (|)\n{\n\t\n}\nelse\n{\n\t\n}");
        m_AutoCompleteMap[_T("ifei")] = _T("if (|)\n{\n\t\n}\nelse if ()\n{\n\t\n}\nelse\n{\n\t\n}");
        m_AutoCompleteMap[_T("guard")] = _T("#ifndef $(Guard token)\n#define $(Guard token)\n\n|\n\n#endif // $(Guard token)\n");
        m_AutoCompleteMap[_T("while")] = _T("while (|)\n\t;");
        m_AutoCompleteMap[_T("whileb")] = _T("while (|)\n{\n\t\n}");
        m_AutoCompleteMap[_T("switch")] = _T("switch (|)\n{\n\tcase :\n\t\tbreak;\n\n\tdefault:\n\t\tbreak;\n}\n");
        m_AutoCompleteMap[_T("for")] = _T("for (|; ; )\n\t;");
        m_AutoCompleteMap[_T("forb")] = _T("for (|; ; )\n{\n\t\n}");
        m_AutoCompleteMap[_T("class")] = _T("class $(Class name)|\n{\n\tpublic:\n\t\t$(Class name)();\n\t\t~$(Class name)();\n\tprotected:\n\t\t\n\tprivate:\n\t\t\n};\n");
        m_AutoCompleteMap[_T("struct")] = _T("struct |\n{\n\t\n};\n");
    }
}

void EditorManager::SaveAutoComplete()
{
    Manager::Get()->GetConfigManager(_T("editor"))->DeleteSubPath(_T("/auto_complete"));
    AutoCompleteMap::iterator it;
    int count = 0;
    for (it = m_AutoCompleteMap.begin(); it != m_AutoCompleteMap.end(); ++it)
    {
        wxString code = it->second;
        // convert non-printable chars to printable
        code.Replace(_T("\n"), _T("\\n"));
        code.Replace(_T("\r"), _T("\\r"));
        code.Replace(_T("\t"), _T("\\t"));

        ++count;
        wxString key;
        key.Printf(_T("/auto_complete/entry%d/name"), count);
        Manager::Get()->GetConfigManager(_T("editor"))->Write(key, it->first);
        key.Printf(_T("/auto_complete/entry%d/code"), count);
        Manager::Get()->GetConfigManager(_T("editor"))->Write(key, code);
    }
}

int EditorManager::GetEditorsCount()
{
    return m_pNotebook->GetPageCount();
}

EditorBase* EditorManager::InternalGetEditorBase(int page)
{
    EditorBase* eb = static_cast<EditorBase*>(m_pNotebook->GetPage(page));
    return eb;
}

cbEditor* EditorManager::InternalGetBuiltinEditor(int page)
{
    EditorBase* eb = InternalGetEditorBase(page);
    if (eb && eb->IsBuiltinEditor())
        return (cbEditor*)eb;
    return 0;
}

cbEditor* EditorManager::GetBuiltinEditor(EditorBase* eb)
{
    return eb && eb->IsBuiltinEditor() ? (cbEditor*)eb : 0;
}

EditorBase* EditorManager::IsOpen(const wxString& filename)
{
    wxString uFilename = UnixFilename(filename);
    for (int i = 0; i < m_pNotebook->GetPageCount(); ++i)
    {
        EditorBase* eb = InternalGetEditorBase(i);
        if (!eb)
            continue;
        wxString fname = eb->GetFilename();
#ifdef __WXMSW__
        // MSW must use case-insensitive comparison
        if (fname.IsSameAs(uFilename,false) || fname.IsSameAs(g_EditorModified + uFilename,false))
            return eb;
#else

        if (fname.IsSameAs(uFilename) || fname.IsSameAs(g_EditorModified + uFilename))
            return eb;
#endif
    }

    return NULL;
}

EditorBase* EditorManager::GetEditor(int index)
{
    return InternalGetEditorBase(index);
}

void EditorManager::SetColourSet(EditorColourSet* theme)
{
    if (m_Theme)
        delete m_Theme;

    // copy locally
    m_Theme = new EditorColourSet(*theme);

    for (int i = 0; i < m_pNotebook->GetPageCount(); ++i)
    {
        cbEditor* ed = InternalGetBuiltinEditor(i);
        if (ed)
            ed->SetColourSet(m_Theme);
    }
}

cbEditor* EditorManager::Open(const wxString& filename, int pos,ProjectFile* data)
{
    bool can_updateui = !GetActiveEditor() || !Manager::Get()->GetProjectManager()->IsLoading();
    wxFileName fn(filename);
    NormalizePath(fn, wxEmptyString);
    wxString fname = UnixFilename(fn.GetFullPath());
    //  Manager::Get()->GetMessageManager()->DebugLog("Trying to open '%s'", fname.c_str());
    if (!wxFileExists(fname))
        return NULL;
    //  Manager::Get()->GetMessageManager()->DebugLog("File exists '%s'", fname.c_str());

    // disallow application shutdown while opening files
    // WARNING: remember to set it to true, when exiting this function!!!
    s_CanShutdown = false;

    EditorBase* eb = IsOpen(fname);
    cbEditor* ed = 0;
    if (eb)
    {
        if (eb->IsBuiltinEditor())
            ed = (cbEditor*)eb;
        else
            return 0; // is open but not a builtin editor
    }

    if (!ed)
    {
        ed = new cbEditor(m_pNotebook, fname, m_Theme);
        if (ed->IsOK())
            AddEditorBase(ed);
        else
        {
            ed->Destroy();
            ed = NULL;
        }
    }

    if(can_updateui)
    {
        if (ed)
        {
            SetActiveEditor(ed);
            ed->GetControl()->SetFocus();
        }
    }

    // check for ProjectFile
    if (ed && !ed->GetProjectFile())
    {
        // First checks if we're already being passed a ProjectFile
        // as a parameter
        if(data)
        {
            Manager::Get()->GetMessageManager()->DebugLog(_T("project data set for %s"), data->file.GetFullPath().c_str());
        }
        else
        {
            ProjectsArray* projects = Manager::Get()->GetProjectManager()->GetProjects();
            for (unsigned int i = 0; i < projects->GetCount(); ++i)
            {
                cbProject* prj = projects->Item(i);
                ProjectFile* pf = prj->GetFileByFilename(ed->GetFilename(), false);
                if (pf)
                {
                    Manager::Get()->GetMessageManager()->DebugLog(_T("found %s"), pf->file.GetFullPath().c_str());
                    data = pf;
                    break;
                }
            }
        }
        if(data)
            ed->SetProjectFile(data,true);
    }
#ifdef USE_OPENFILES_TREE
    if(can_updateui)
        AddFiletoTree(ed);
#endif

    // we 're done
    s_CanShutdown = true;

    return ed;
}

EditorBase* EditorManager::GetActiveEditor()
{
    return InternalGetEditorBase(m_pNotebook->GetSelection());
}

void EditorManager::ActivateNext()
{
    m_pNotebook->AdvanceSelection(true);
}

void EditorManager::ActivatePrevious()
{
    m_pNotebook->AdvanceSelection(false);
}

void EditorManager::SetActiveEditor(EditorBase* ed)
{
    if (!ed)
        return;
    if (ed->IsBuiltinEditor())
        static_cast<cbEditor*>(ed)->GetControl()->SetFocus();
    int page = FindPageFromEditor(ed);
    if (page != -1)
    {
        m_pNotebook->SetSelection(page);
        // because we use SetSelection() manually, the event handler isn't called,
        // so we must send this event manually
        CodeBlocksEvent evt(cbEVT_EDITOR_ACTIVATED, -1, 0, GetActiveEditor());
        Manager::Get()->GetPluginManager()->NotifyPlugins(evt);
    }
}

cbEditor* EditorManager::New(const wxString& newFileName)
{
    wxString old_title = Manager::Get()->GetAppWindow()->GetTitle(); // Fix for Bug #1389450
    // create a dummy file
    if (!newFileName.IsEmpty() && !wxFileExists(newFileName) && wxDirExists(wxPathOnly(newFileName)))
    {
        wxLogNull ln; // we don't care if this fails...
        wxFile f(newFileName, wxFile::write);
        if (!f.IsOpened())
            return 0;
    }
    cbEditor* ed = new cbEditor(m_pNotebook, newFileName);
    if ((newFileName.IsEmpty() && !ed->SaveAs()) || !ed->Save())
    {
        //DeletePage(ed->GetPageIndex());
        ed->Destroy();
        Manager::Get()->GetAppWindow()->SetTitle(old_title); // Though I can't reproduce the bug, this does no harm
        return 0L;
    }

    // add default text
    wxString key;
    key.Printf(_T("/default_code/set%d"), (int)FileTypeOf(ed->GetFilename()));
    wxString code = Manager::Get()->GetConfigManager(_T("editor"))->Read(key, wxEmptyString);
    ed->GetControl()->SetText(code);

    ed->SetColourSet(m_Theme);
    AddEditorBase(ed);

#ifdef USE_OPENFILES_TREE
    AddFiletoTree(ed);
#endif

    ed->Show(true);
    SetActiveEditor(ed);
    CodeBlocksEvent evt(cbEVT_EDITOR_OPEN, -1, 0, ed);
    Manager::Get()->GetPluginManager()->NotifyPlugins(evt);
    return ed;
}

void EditorManager::AddCustomEditor(EditorBase* eb)
{
    AddEditorBase(eb);
}

void EditorManager::RemoveCustomEditor(EditorBase* eb)
{
    RemoveEditorBase(eb, false);
}

void EditorManager::AddEditorBase(EditorBase* eb)
{
    int page = FindPageFromEditor(eb);
    if (page == -1)
    {
        //        LOGSTREAM << wxString::Format(_T("AddEditorBase(): ed=%p, title=%s\n"), eb, eb ? eb->GetTitle().c_str() : _T(""));
        m_pNotebook->AddPage(eb, eb->GetTitle(), true);
    }
}

void EditorManager::RemoveEditorBase(EditorBase* eb, bool deleteObject)
{
    //    LOGSTREAM << wxString::Format(_T("RemoveEditorBase(): ed=%p, title=%s\n"), eb, eb ? eb->GetFilename().c_str() : _T(""));
    int page = FindPageFromEditor(eb);
   if (page != -1 && !Manager::isappShuttingDown())
        m_pNotebook->RemovePage(page, false);

#ifdef USE_OPENFILES_TREE
    //        if (eb->IsBuiltinEditor())
    //        {
    //            cbEditor* ed = static_cast<cbEditor*>(eb);
    //            DeleteFilefromTree(ed->GetProjectFile()->file.GetFullPath());
    //        }
    //        else
    DeleteFilefromTree(eb->GetFilename());
#endif

    //    if (deleteObject)
    //        eb->Destroy();
}

bool EditorManager::UpdateProjectFiles(cbProject* project)
{
    for (int i = 0; i < m_pNotebook->GetPageCount(); ++i)
    {
        cbEditor* ed = InternalGetBuiltinEditor(i);
        if (!ed)
            continue;
        ProjectFile* pf = ed->GetProjectFile();
        if (!pf)
            continue;
        if (pf->GetParentProject() != project)
            continue;
        pf->editorTopLine = ed->GetControl()->GetFirstVisibleLine();
        pf->editorPos = ed->GetControl()->GetCurrentPos();
        pf->editorTabPos = i + 1;
        pf->editorOpen = true;
    }
    return true;
}

bool EditorManager::CloseAll(bool dontsave)
{
    return CloseAllExcept(0L,dontsave);
}

bool EditorManager::QueryCloseAll()
{
    for (int i = m_pNotebook->GetPageCount() - 1; i >= 0; --i)
    {
        EditorBase* eb = InternalGetEditorBase(i);
        if(eb && !QueryClose(eb))
            return false; // aborted
    }
    return true;
}

bool EditorManager::CloseAllExcept(EditorBase* editor,bool dontsave)
{
    if(!dontsave)
    {
        for (int i = 0; i < m_pNotebook->GetPageCount(); ++i)
        {
            EditorBase* eb = InternalGetEditorBase(i);
            if(eb && eb != editor && !QueryClose(eb))
                return false; // aborted
        }
    }

    m_pNotebook->Freeze();
    int count = m_pNotebook->GetPageCount();
    for (int i = m_pNotebook->GetPageCount() - 1; i >= 0; --i)
    {
        EditorBase* eb = InternalGetEditorBase(i);
        if (eb && eb != editor && Close(eb, true))
            --count;
    }
    m_pNotebook->Thaw();
    return count == (editor ? 1 : 0);
}

bool EditorManager::CloseActive(bool dontsave)
{
    return Close(GetActiveEditor(),dontsave);
}

bool EditorManager::QueryClose(EditorBase *ed)
{
    if(!ed)
        return true;
    if (ed->GetModified())
    {
        // TODO (mandrav#1#): Move this in EditorBase
        wxString msg;
        msg.Printf(_("File %s is modified...\nDo you want to save the changes?"), ed->GetFilename().c_str());
        switch (cbMessageBox(msg, _("Save file"), wxICON_QUESTION | wxYES_NO | wxCANCEL))
        {
        case wxID_YES:
            if (!ed->Save())
                return false;
            break;
        case wxID_NO:
            break;
        case wxID_CANCEL:
            return false;
        }
        ed->SetModified(false);
    }
    else
    {
        return ed->QueryClose();
    }
    return true;
}

int EditorManager::FindPageFromEditor(EditorBase* eb)
{
    for (int i = 0; i < m_pNotebook->GetPageCount(); ++i)
    {
        if (m_pNotebook->GetPage(i) == eb)
            return i;
    }
    return -1;
}

bool EditorManager::Close(const wxString& filename,bool dontsave)
{
    return Close(IsOpen(filename),dontsave);
}

bool EditorManager::Close(EditorBase* editor,bool dontsave)
{
    if (editor)
    {
        int idx = FindPageFromEditor(editor);
        if (idx != -1)
        {
            if(!dontsave)
                if(!QueryClose(editor))
                    return false;
            wxString filename = editor->GetFilename();
            //            LOGSTREAM << wxString::Format(_T("Close(): ed=%p, title=%s\n"), editor, editor ? editor->GetTitle().c_str() : _T(""));
            m_pNotebook->DeletePage(idx, false);
        }
    }
    m_pData->m_NeedsRefresh = true;
    return true;
}

bool EditorManager::Close(int index,bool dontsave)
{
    EditorBase* ed = InternalGetEditorBase(index);
    if (ed)
        return Close(ed,dontsave);
    return false;
}

bool EditorManager::Save(const wxString& filename)
{
    //    cbEditor* ed = GetBuiltinEditor(IsOpen(filename));
    EditorBase* ed = IsOpen(filename);
    if (ed)
        return ed->Save();
    return true;
}

bool EditorManager::Save(int index)
{
    EditorBase* ed = InternalGetEditorBase(index);
    if (ed)
        return ed->Save();
    return false;
}

bool EditorManager::SaveActive()
{
    EditorBase* ed = GetActiveEditor();
    if (ed)
        return ed->Save();
    return true;
}

bool EditorManager::SaveAs(int index)
{
    cbEditor* ed = GetBuiltinEditor(GetEditor(index));
    if(!ed)
        return false;
    wxString oldname=ed->GetFilename();
    if(!ed->SaveAs())
        return false;
    RenameTreeFile(oldname,ed->GetFilename());
    return true;
}

bool EditorManager::SaveActiveAs()
{
    cbEditor* ed = GetBuiltinEditor(GetActiveEditor());
    if (ed)
    {
        wxString oldname=ed->GetFilename();
        if(ed->SaveAs())
            RenameTreeFile(oldname,ed->GetFilename());
    }
    return true;
}

bool EditorManager::SaveAll()
{
    for (int i = 0; i < m_pNotebook->GetPageCount(); ++i)
    {
        EditorBase* ed = InternalGetEditorBase(i);
        if (ed && !ed->Save())
        {
            wxString msg;
            msg.Printf(_("File %s could not be saved..."), ed->GetFilename().c_str());
            cbMessageBox(msg, _("Error saving file"));
        }
    }
#ifdef USE_OPENFILES_TREE
    RefreshOpenedFilesTree(true);
#endif

    return true;
}

void EditorManager::Print(PrintScope ps, PrintColourMode pcm, bool line_numbers)
{
    switch (ps)
    {
    case psAllOpenEditors:
        {
            for (int i = 0; i < m_pNotebook->GetPageCount(); ++i)
            {
                cbEditor* ed = InternalGetBuiltinEditor(i);
                if (ed)
                    ed->Print(false, pcm, line_numbers);
            }
            break;
        }
    default:
        {
            cbEditor* ed = GetBuiltinEditor(GetActiveEditor());
            if (ed)
                ed->Print(ps == psSelection, pcm, line_numbers);
            break;
        }
    }
}

void EditorManager::CheckForExternallyModifiedFiles()
{
    if(m_isCheckingForExternallyModifiedFiles) // for some reason, a mutex locker does not work???
        return;
    m_isCheckingForExternallyModifiedFiles = true;

    wxLogNull ln;
    bool reloadAll = false; // flag to stop bugging the user
    wxArrayString failedFiles; // list of files failed to reload
    for (int i = 0; i < m_pNotebook->GetPageCount(); ++i)
    {
        cbEditor* ed = InternalGetBuiltinEditor(i);
        bool b_modified = false;

        // no builtin editor or new file not yet saved
        if (!ed || !ed->IsOK())
            continue;
        //File was deleted?
        if (!wxFileExists(ed->GetFilename()))
        {
            if(ed->GetModified()) // Already set the flag
                continue;
            wxString msg;
            msg.Printf(_("%s has been deleted, or is no longer available.\n"
                         "Do you wish to keep the file open?\n"
                         "Yes to keep the file, No to close it."), ed->GetFilename().c_str());
            if (cbMessageBox(msg, _("File changed!"), wxYES_NO) == wxID_YES)
                ed->SetModified(true);
            else
            {
                ed->Close();
                ProjectFile* pf = ed->GetProjectFile();
                if (pf)
                    pf->SetFileState(fvsMissing);
            }
            continue;
        }

        ProjectFile* pf = ed->GetProjectFile();
        wxFileName fname(ed->GetFilename());
        wxDateTime last = fname.GetModificationTime();

        //File changed from RO -> RW?
        if (ed->GetControl()->GetReadOnly() &&
                wxFile::Access(ed->GetFilename().c_str(), wxFile::write))
        {
            b_modified = false;
            ed->GetControl()->SetReadOnly(false);
            if (pf)
                pf->SetFileState(fvsNormal);
        }
        //File changed from RW -> RO?
        if (!ed->GetControl()->GetReadOnly() &&
                !wxFile::Access(ed->GetFilename().c_str(), wxFile::write))
        {
            b_modified = false;
            ed->GetControl()->SetReadOnly(true);
            if (pf)
                pf->SetFileState(fvsReadOnly);
        }
        //File content changed?
        if (last.IsLaterThan(ed->GetLastModificationTime()))
            b_modified = true;

        if (b_modified)
        {
            // modified; ask to reload
            int ret = -1;
            if (!reloadAll)
            {
                wxString msg;
                msg.Printf(_("File %s is modified outside the IDE...\nDo you want to reload it (you will lose any unsaved work)?"),
                           ed->GetFilename().c_str());
                ConfirmReplaceDlg dlg(Manager::Get()->GetAppWindow(), false, msg);
                dlg.SetTitle(_("Reload file?"));
                PlaceWindow(&dlg);
                ret = dlg.ShowModal();
                reloadAll = ret == crAll;
            }
            if (reloadAll || ret == crYes)
            {
                if (!ed->Reload())
                    failedFiles.Add(ed->GetFilename());
            }
            else if (ret == crCancel)
                break;
            else if (ret == crNo)
                ed->Touch();
        }
    }

    // this will emmit a EVT_EDITOR_ACTIVATED event, which in turn will notify
    // the app to update the currently active file's info
    // (we 're interested in updating read-write state)
    SetActiveEditor(GetActiveEditor());

    if (failedFiles.GetCount())
    {
        wxString msg;
        msg.Printf(_("Could not reload all files:\n\n%s"), GetStringFromArray(failedFiles, _T("\n")).c_str());
        cbMessageBox(msg, _("Error"), wxICON_ERROR);
    }
    m_isCheckingForExternallyModifiedFiles = false;
}

bool EditorManager::SwapActiveHeaderSource()
{
    cbEditor* ed = GetBuiltinEditor(GetActiveEditor());
    if (!ed)
        return false;

    FileType ft = FileTypeOf(ed->GetFilename());
    if (ft != ftHeader && ft != ftSource)
        return 0L;

    // create a list of search dirs
    wxArrayString dirs;

    // get project's include dirs
    cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
    if (project)
    {
        dirs = project->GetIncludeDirs();

        // first add all paths that contain project files
        for (int i = 0; i < project->GetFilesCount(); ++i)
        {
            ProjectFile* pf = project->GetFile(i);
            if (pf)
            {
                wxString dir = pf->file.GetPath(wxPATH_GET_VOLUME);
                if (dirs.Index(dir) == wxNOT_FOUND)
                    dirs.Add(dir);
            }
        }

        // get targets include dirs
        for (int i = 0; i < project->GetBuildTargetsCount(); ++i)
        {
            ProjectBuildTarget* target = project->GetBuildTarget(i);
            if (target)
            {
                for (unsigned int ti = 0; ti < target->GetIncludeDirs().GetCount(); ++ti)
                {
                    wxString dir = target->GetIncludeDirs()[ti];
                    if (dirs.Index(dir) == wxNOT_FOUND)
                        dirs.Add(dir);
                }
            }
        }
    }

    wxFileName fname;
    wxFileName fn(ed->GetFilename());
    dirs.Insert(fn.GetPath(wxPATH_GET_VOLUME), 0); // add file's dir

    for (unsigned int i = 0; i < dirs.GetCount(); ++i)
    {
        fname.Assign(dirs[i] + wxFileName::GetPathSeparator() + fn.GetFullName());
        if (!fname.IsAbsolute() && project)
        {
            fname.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_CASE, project->GetBasePath());
        }
        //Manager::Get()->GetMessageManager()->DebugLog("Looking for '%s'", fname.GetFullPath().c_str());
        if (ft == ftHeader)
        {
            fname.SetExt(FileFilters::CPP_EXT);
            if (fname.FileExists())
                break;
            fname.SetExt(FileFilters::C_EXT);
            if (fname.FileExists())
                break;
            fname.SetExt(FileFilters::CC_EXT);
            if (fname.FileExists())
                break;
            fname.SetExt(FileFilters::CXX_EXT);
            if (fname.FileExists())
                break;
        }
        else if (ft == ftSource)
        {
            fname.SetExt(FileFilters::HPP_EXT);
            if (fname.FileExists())
                break;
            fname.SetExt(FileFilters::H_EXT);
            if (fname.FileExists())
                break;
            fname.SetExt(FileFilters::HH_EXT);
            if (fname.FileExists())
                break;
            fname.SetExt(FileFilters::HXX_EXT);
            if (fname.FileExists())
                break;
        }
    }

    if (fname.FileExists())
    {
        //Manager::Get()->GetMessageManager()->DebugLog("ed=%s, pair=%s", ed->GetFilename().c_str(), pair.c_str());
        cbEditor* newEd = Open(fname.GetFullPath());
        //if (newEd)
        //    newEd->SetProjectFile(ed->GetProjectFile());
        return newEd;
    }

    // We couldn't find the file, maybe it does not exist. Ask the user if we
    // should create it:
    if (cbMessageBox(_("The file does not exist. Do you want to create it?"),
				_("Error"), wxICON_QUESTION | wxYES_NO) == wxID_YES)
    {
		cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
		if (project)
			wxSetWorkingDirectory(project->GetBasePath());

		// Create a suggestion for the new file name:
        if (ft == ftHeader)
			fn.SetExt(FileFilters::CPP_EXT);
		else if (ft == ftSource)
			fn.SetExt(FileFilters::H_EXT);
		// else? Well, if the filename is not changed we could possibly
		// overwrite an existing file with our suggestion.

    	cbEditor* newEd = New(fn.GetFullPath());
		if (cbMessageBox(_("Do you want to add this new file in the active project?"),
					_("Add file to project"),
					wxYES_NO | wxICON_QUESTION) == wxID_YES)
		{
			wxArrayInt targets;
			if (Manager::Get()->GetProjectManager()->AddFileToProject(newEd->GetFilename(), project, targets) != 0)
			{
				ProjectFile* pf = project->GetFileByFilename(newEd->GetFilename(), false);
				newEd->SetProjectFile(pf);
				Manager::Get()->GetProjectManager()->RebuildTree();
			}
		}
		// verify that the open files are still in sync
		// the new file might have overwritten an existing one)
		Manager::Get()->GetEditorManager()->CheckForExternallyModifiedFiles();
    }
    return 0L;
}

int EditorManager::ShowFindDialog(bool replace, bool explicitly_find_in_files)
{
    wxString phraseAtCursor;
    bool hasSelection = false;
    cbStyledTextCtrl* control = 0;

    cbEditor* ed = GetBuiltinEditor(GetActiveEditor());
    if (ed)
    {
        control = ed->GetControl();

        hasSelection = control->GetSelectionStart() != control->GetSelectionEnd();
        int wordStart = control->WordStartPosition(control->GetCurrentPos(), true);
        int wordEnd = control->WordEndPosition(control->GetCurrentPos(), true);
        wxString wordAtCursor = control->GetTextRange(wordStart, wordEnd);
        phraseAtCursor = control->GetSelectedText();
        // if selected text is the last searched text, don't suggest "search in selection"
        if ((m_LastFindReplaceData &&
                !phraseAtCursor.IsEmpty() &&
                phraseAtCursor == m_LastFindReplaceData->findText)
                || phraseAtCursor == wordAtCursor)
        {
            hasSelection = false;
        }

        if ( phraseAtCursor.IsEmpty())
            phraseAtCursor = wordAtCursor;

        int selstartline = control->LineFromPosition(control->GetSelectionStart());
        int selendline   = control->LineFromPosition(control->GetSelectionEnd());
        // the selection of several lines is not proposed as search pattern
        if ( selstartline != selendline )
            phraseAtCursor = wxEmptyString;

    }

    FindReplaceBase* dlg;
    if (!replace)
        dlg = new FindDlg(Manager::Get()->GetAppWindow(), phraseAtCursor, hasSelection, !ed, explicitly_find_in_files);
    else
        dlg = new ReplaceDlg(Manager::Get()->GetAppWindow(), phraseAtCursor, hasSelection, !ed, explicitly_find_in_files);

    PlaceWindow(dlg);
    if (dlg->ShowModal() == wxID_CANCEL)
    {
        dlg->Destroy();
        return -2;
    }

    // Don't look for empty strings:
    if (dlg->GetFindString().empty())
    {
        dlg->Destroy();
        cbMessageBox(_("Can't look for an empty search criterion!"), _("Error"), wxOK | wxICON_EXCLAMATION, Manager::Get()->GetAppWindow());
        return -2;
    }

    if (!m_LastFindReplaceData)
        m_LastFindReplaceData = new cbFindReplaceData;

    m_LastFindReplaceData->start = 0;
    m_LastFindReplaceData->end = 0;
    m_LastFindReplaceData->findText = dlg->GetFindString();
    m_LastFindReplaceData->replaceText = dlg->GetReplaceString();
    m_LastFindReplaceData->findInFiles = dlg->IsFindInFiles();
    m_LastFindReplaceData->delOldSearches = dlg->GetDeleteOldSearches();
    m_LastFindReplaceData->matchWord = dlg->GetMatchWord();
    m_LastFindReplaceData->startWord = dlg->GetStartWord();
    m_LastFindReplaceData->matchCase = dlg->GetMatchCase();
    m_LastFindReplaceData->regEx = dlg->GetRegEx();
    m_LastFindReplaceData->directionDown = dlg->GetDirection() == 1;
    m_LastFindReplaceData->originEntireScope = dlg->GetOrigin() == 1;
    m_LastFindReplaceData->scope = dlg->GetScope();
    m_LastFindReplaceData->searchPath = dlg->GetSearchPath();
    m_LastFindReplaceData->searchMask = dlg->GetSearchMask();
    m_LastFindReplaceData->recursiveSearch = dlg->GetRecursive();
    m_LastFindReplaceData->hiddenSearch = dlg->GetHidden();
    m_LastFindReplaceData->initialreplacing = false;
    m_LastFindReplaceData->NewSearch = true;
    if(control)
    {   // if editor : store the selection start/end
        // only use this in case of !findInFiles and scope==1 (search in selection)
        m_LastFindReplaceData->SearchInSelectionStart = control->GetSelectionStart();
        m_LastFindReplaceData->SearchInSelectionEnd = control->GetSelectionEnd();
    }
    dlg->Destroy();

    int ReturnValue = 0;
    if (!replace)
    {
        if (m_LastFindReplaceData->findInFiles)
            ReturnValue = FindInFiles(m_LastFindReplaceData);
        else
            ReturnValue = Find(control, m_LastFindReplaceData);
    }
    else
    {
        m_LastFindReplaceData->initialreplacing = true;

        if (m_LastFindReplaceData->findInFiles)
            ReturnValue = ReplaceInFiles(m_LastFindReplaceData);
        else
            ReturnValue = Replace(control, m_LastFindReplaceData);
    }
    m_LastFindReplaceData->NewSearch = false; // we have searched, so no longer new search

    //Default back to find or replace in Editor
    if(m_LastFindReplaceData->findInFiles)
    {
        m_LastFindReplaceData->findInFiles = false;
    }
    return ReturnValue;
} // end of ShowFindDialog

void EditorManager::CalculateFindReplaceStartEnd(cbStyledTextCtrl* control, cbFindReplaceData* data, bool replace)
{
    if (!control || !data)
        return;

    if (!data->findInFiles)   // Find in current Editor
    {
        int ssta = control->GetSelectionStart();
        int send = control->GetSelectionEnd();
        int cpos = control->GetCurrentPos();
        int clen = control->GetLength();

        data->start = 0;
        data->end   = clen;

        if (!data->originEntireScope || !data->NewSearch)   // from pos or next/prev search
        {
            if (!data->directionDown)   // up
                // initial replacing mode - include selection end : normal mode - skip until selection start
                data->start = (data->initialreplacing)? std::max(send, cpos) : std::min(ssta, cpos);
            else                      // down
                // initial replacing mode - include selection start : normal mode - skip until selection end
                data->start = (data->initialreplacing)? std::min(ssta, cpos) : std::max(send, cpos);
        }
        else                            // entire scope
        {
            if (!data->directionDown)   // up
                data->start = clen;
        }

        if (!data->directionDown)       // up
            data->end = 0;

        if (data->scope == 1) // selected text
        {
            if(data->NewSearch)
            {
                if (!data->directionDown)   // up
                {
                    data->start = std::max(ssta, send);
                    data->end   = std::min(ssta, send);
                }
                else // down
                {
                    data->start = std::min(ssta, send);
                    data->end   = std::max(ssta, send);
                }
            }
            else
            {   // this is the result of a next/previous search
                // rebase depending on the cursor position
                ssta = data->SearchInSelectionStart;
                send = data->SearchInSelectionEnd;
                if(cpos < ssta || cpos > send)
                {   // regular reset (this also provide some sort of wrap around) (other editors also did it like that)
                    data->start = ssta;
                    data->end = send;
                }
                else
                {
                    data->start = cpos;
                    data->end = (data->directionDown)?send:ssta;
                }
            }
        }
    }
    else        // FindInFiles
    {           // searching direction down, entire scope
        //Replace needs the entire scope, while find can wrap around.
        data->start = ( replace ? 0 : control->GetCurrentPos() );
        data->end   = control->GetLength();
    }
} // end of CalculateFindReplaceStartEnd

int EditorManager::Replace(cbStyledTextCtrl* control, cbFindReplaceData* data)
{
    if (!control || !data)
        return -1;

    int flags = 0;
    CalculateFindReplaceStartEnd(control, data);

    if (data->matchWord)
        flags |= wxSCI_FIND_WHOLEWORD;
    if (data->startWord)
        flags |= wxSCI_FIND_WORDSTART;
    if (data->matchCase)
        flags |= wxSCI_FIND_MATCHCASE;
    if (data->regEx)
        flags |= wxSCI_FIND_REGEXP;

    control->BeginUndoAction();
    int pos = -1;
    bool replace = false;
    bool confirm = true;
    bool stop = false;
    wxPoint LastDlgPosition;
    bool HaveLastDlgPosition = false;
    bool wrapAround = false;
    int  data_start_initial = data->start;

    while (!stop)
    {
        int lengthFound = 0;
        pos = control->FindText(data->start, data->end, data->findText, flags, &lengthFound);
        if (pos != -1)
        {
            control->GotoPos(pos);
            control->EnsureVisible(control->LineFromPosition(pos));
            control->SetSelection(pos, pos + lengthFound);
            data->start = pos;
            data->initialreplacing = false;  // special treatment only necessary the first time
        }
        else if (!wrapAround)
        {
            if ( (data->scope == 0) &&      // scope = global text
                    ((data->directionDown && data_start_initial != 0) ||
                     (!data->directionDown && data_start_initial != control->GetLength())))
            {
                wxString msg;
                if (data->directionDown)
                    msg = _("Text not found.\nSearch from the start of the document?");
                else
                    msg = _("Text not found.\nSearch from the end of the document?");

                bool auto_wrap_around = Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/auto_wrap_search"), true);
                if (auto_wrap_around)
                    wxBell();
                if (auto_wrap_around || cbMessageBox(msg, _("Result"), wxOK | wxCANCEL | wxICON_QUESTION) == wxID_OK)
                {
                    data->end = data_start_initial;
                    data->start = (data->directionDown)? 0 : control->GetLength();
                    wrapAround = true; // signal the wrap-around
                    continue;
                }
                else
                    break;  // done - user doesn't want to wrap around
            }
            else
                break; // done - we're replacing in a selection of text
        }
        else
            break; // done - already wrapped around once


        if (confirm)
        {
            ConfirmReplaceDlg dlg(Manager::Get()->GetAppWindow());
            // dlg.CalcPosition(control);
            // TODO (thomas#1#): Check whether the existing code actually works with twin view
            // else, we need something like:
            // PlaceWindow(&dlg, pdlRelative);

            // NOTE (Tiwag#1#): dlg.CalcPosition doesn't work for me with dual monitor setup,
            //     workaround : remember last dialog position, user can position
            //                  it outside of text where he wants
            // Move dialog to last position if already available,
            // else place it according to environments settings
            if ( HaveLastDlgPosition )
                dlg.Move(LastDlgPosition);
            else
                dlg.CalcPosition(control);

            int ans = dlg.ShowModal();
            LastDlgPosition = dlg.GetPosition();
            HaveLastDlgPosition = true;
            switch (ans)
            {
            case crYes:
                replace = true;
                break;
            case crNo:
                replace = false;
                break;
            case crAll:
                replace = true;
                confirm = false;
                break;
            case crCancel:
                stop = true;
                break;
            }
        }

        if (!stop)
        {
            if (replace)
            {
                int lengthReplace = data->replaceText.Length();
                if (data->regEx)
                {
                    // set target same as selection
                    control->SetTargetStart(control->GetSelectionStart());
                    control->SetTargetEnd(control->GetSelectionEnd());
                    // replace with regEx support
                    lengthReplace = control->ReplaceTargetRE(data->replaceText);
                    // reset target
                    control->SetTargetStart(0);
                    control->SetTargetEnd(0);
                }
                else
                    control->ReplaceSelection(data->replaceText);
                data->start += lengthReplace;
                // adjust end pos by adding the length difference between find and replace strings
                int diff = lengthReplace - lengthFound;
                if (data->directionDown)
                    data->end += diff;
                else
                    data->end -= diff;
            }
            else
                if (data->directionDown)
                    data->start += lengthFound;
                else
                    data->start -= lengthFound;

        }
    }
    control->EndUndoAction();

    return pos;
}

int EditorManager::ReplaceInFiles(cbFindReplaceData* data)
{
    if (!data) return 0;
    if (data->findText.IsEmpty()) return 0;

    // let's make a list of all the files to search in
    wxArrayString filesList;

    if (data->scope == 0) // find in project files
    {
        // fill the search list with all the project files
        cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
        if (!prj)
            return 0;

        wxString fullpath = _T("");
        for (int i = 0; i < prj->GetFilesCount(); ++i)
        {
            ProjectFile* pf = prj->GetFile(i);
            if (pf)
            {
                fullpath = pf->file.GetFullPath();
                if (filesList.Index(fullpath) == -1) // avoid adding duplicates
                {
                    if(wxFileExists(fullpath))  // Does the file exist?
                        filesList.Add(fullpath);
                }
            }
        }
    }
    else if (data->scope == 1) // find in open files
    {
        // fill the search list with the open files
        for (int i = 0; i < m_pNotebook->GetPageCount(); ++i)
        {
            cbEditor* ed = InternalGetBuiltinEditor(i);
            if (ed)
                filesList.Add(ed->GetFilename());
        }
    }
    else if (data->scope == 2) // find in workspace
    {
		// loop over all the projects in the workspace (they are contained in the ProjectManager)
		const ProjectsArray* pProjects = Manager::Get()->GetProjectManager()->GetProjects();
		if(pProjects)
		{
			int count = pProjects->GetCount();
			for (int idxProject = 0; idxProject < count; ++idxProject)
			{
				cbProject* pProject = pProjects->Item(idxProject);
				if(pProject)
				{
					wxString fullpath = _T("");
					for (int idxFile = 0; idxFile < pProject->GetFilesCount(); ++idxFile)
					{
						ProjectFile* pf = pProject->GetFile(idxFile);
						if (pf)
						{
							fullpath = pf->file.GetFullPath();
							if (filesList.Index(fullpath) == -1) // avoid adding duplicates
							{
								if(wxFileExists(fullpath))  // Does the file exist?
									filesList.Add(fullpath);
							}
						}
					} // end for : idx : idxFile
				}
			} // end for : idx : idxProject
		}
    }

    // if the list is empty, leave
    int filesCount = filesList.GetCount();
    if (filesCount == 0)
    {
        cbMessageBox(_("No files to search in!"), _("Error"), wxICON_WARNING);
        return 0;
    }

    int flags = 0;
    if (data->matchWord)
        flags |= wxSCI_FIND_WHOLEWORD;
    if (data->startWord)
        flags |= wxSCI_FIND_WORDSTART;
    if (data->matchCase)
        flags |= wxSCI_FIND_MATCHCASE;
    if (data->regEx)
        flags |= wxSCI_FIND_REGEXP;

    bool replace = false;
    bool confirm = true;
    bool stop = false;
    bool wholeFile = false;
    bool all = false;
    int pos = -1;

    wxPoint LastDlgPosition;
    bool HaveLastDlgPosition = false;

    wxProgressDialog* progress = 0;
    wxString fileContents;
    wxString enc_name = Manager::Get()->GetConfigManager(_T("editor"))->Read(_T("/default_encoding"), wxLocale::GetSystemEncodingName());
    wxFontEncoding def_encoding = wxFontMapper::GetEncodingFromName(enc_name);

    // keep a copy of the find struct
    cbFindReplaceData dataCopy = *data;

    for (int i = 0; i<filesCount && !stop; ++i)
    {
        cbEditor *ed = NULL;
        cbStyledTextCtrl *control = NULL;
        bool fileWasNotOpen = false;

        if (progress)
        {
            if (!progress->Update(i))
            {
                if (cbMessageBox(_("Are you sure you want to stop replacing in files?"), _("Confirmation"), wxICON_QUESTION | wxYES_NO) == wxID_YES)
                    break;
                else
                    progress->Resume();
            }
        }

        //Check if this file is already open
        EditorBase *eb = IsOpen(filesList[i]);
        if (eb)
        {
            //File was already open
            fileWasNotOpen = false;

            ed = GetBuiltinEditor(eb);
            if (ed) control = ed->GetControl();
        }

        //If it's still NULL, open a new editor
        if (!control)
        {
            wxFile file(filesList[i]);
            if (!file.IsOpened())
                continue;
            fileContents = cbReadFileContents(file, def_encoding);
            if (fileContents.Find(data->findText) == -1)
                continue;

            //File was not open, i opened it.
            fileWasNotOpen = true;

            ed = Open(filesList[i]);
            if (ed) control = ed->GetControl();
        }
        //Still NULL?
        if (!control || !ed)
            continue;

        SetActiveEditor(ed);
        control->BeginUndoAction(); //undo

        *data = dataCopy;
        CalculateFindReplaceStartEnd(control, data, true);

        //reset bools
        wholeFile = false;
        if (!all) confirm = true;

        //Replace in this file
        while(!stop || wholeFile)
        {
            int lengthFound = 0;
            pos = control->FindText(data->start, data->end, data->findText,
                flags, &lengthFound);

            if (pos == -1)
                break;

            if (confirm)
            {
                control->GotoPos(pos);
                control->EnsureVisible(control->LineFromPosition(pos));
            }
            control->SetSelection(pos, pos + lengthFound);
            data->start = pos;
            data->initialreplacing = false;  // special treatment only necessary the first time

            if (confirm)
            {
                ConfirmReplaceDlg dlg(Manager::Get()->GetAppWindow(), true);
                // dlg.CalcPosition(control);
                // TODO (thomas#1#): Check whether the existing code actually works with twin view
                // else, we need something like:
                // PlaceWindow(&dlg, pdlRelative);

                // NOTE (Tiwag#1#): dlg.CalcPosition doesn't work for me with dual monitor setup,
                //     workaround : remember last dialog position, user can position
                //                  it outside of text where he wants
                // Move dialog to last position if already available,
                // else place it according to environments settings
                if ( HaveLastDlgPosition )
                    dlg.Move(LastDlgPosition);
                else
                    dlg.CalcPosition(control);

                int ans = dlg.ShowModal();
                LastDlgPosition = dlg.GetPosition();
                HaveLastDlgPosition = true;
                switch (ans)
                {
                    case crYes:
                        replace = true;
                        break;
                    case crNo:
                        replace = false;
                        break;
                    case crAllInFile:
                        confirm = false;
                        replace = true;
                        wholeFile = true;
                        break;
                    case crSkipFile:
                        confirm = false;
                        replace = false;
                        wholeFile = true;
                        break;
                    case crAll:
                        replace = true;
                        confirm = false;
                        all = true;
                        // let's create a progress dialog because it might take some time depending on the files count
                        progress = new wxProgressDialog(_("Replace in files"),
                                     _("Please wait while replacing in files..."),
                                     filesCount,
                                     Manager::Get()->GetAppWindow(),
                                     wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_CAN_ABORT);
                        PlaceWindow(progress);
                        // now that we need no confirmation, freeze the app window
                        Manager::Get()->GetAppWindow()->Freeze();
                        break;
                    case crCancel:
                        stop = true;
                        break;
                }
            }

            if (!stop)
            {
                if (replace)
                {
                    int lengthReplace = data->replaceText.Length();
                    if (data->regEx)
                    {
                        // set target same as selection
                        control->SetTargetStart(control->GetSelectionStart());
                        control->SetTargetEnd(control->GetSelectionEnd());
                        // replace with regEx support
                        lengthReplace = control->ReplaceTargetRE(data->replaceText);
                        // reset target
                        control->SetTargetStart(0);
                        control->SetTargetEnd(0);
                    }
                    else
                        control->ReplaceSelection(data->replaceText);

                    data->start += lengthReplace;

                    // adjust end pos by adding the length difference
                    //between find and replace strings
                    int diff = lengthReplace - lengthFound;
                    if (data->directionDown)
                        data->end += diff;
                    else
                        data->end -= diff;
                }
                else
                {
                    if (data->directionDown)
                        data->start += lengthFound;
                    else
                        data->start -= lengthFound;
                }
            }
        }

        control->EndUndoAction(); // undo

        //If i opened the file and no replacement was made,
        //close the editor
        if (!ed->GetModified() && fileWasNotOpen)
            Close(ed, true);
    }

    // if we showed the progress, the app window is frozen; unfreeze it
    if (progress)
        Manager::Get()->GetAppWindow()->Thaw();

    delete progress;
    return pos;
}

int EditorManager::Find(cbStyledTextCtrl* control, cbFindReplaceData* data)
{
    if (!control || !data)
        return -1;

    int flags = 0;
    CalculateFindReplaceStartEnd(control, data);

    if (data->matchWord)
        flags |= wxSCI_FIND_WHOLEWORD;
    if (data->startWord)
        flags |= wxSCI_FIND_WORDSTART;
    if (data->matchCase)
        flags |= wxSCI_FIND_MATCHCASE;
    if (data->regEx)
        flags |= wxSCI_FIND_REGEXP;

    int pos = -1;
    // avoid infinite loop when wrapping search around, eventually crashing WinLogon O.O
    bool wrapAround = false;
    int StartPos = 0;
    int EndPos = control->GetLength();
    if(data->scope == 1)
    {
        StartPos = data->SearchInSelectionStart;
        EndPos = data->SearchInSelectionEnd;
    }
    while (true) // loop while not found and user selects to start again from the top
    {
        int lengthFound = 0;
        pos = control->FindText(data->start, data->end, data->findText, flags, &lengthFound);
        if (pos != -1)
        {
            control->GotoPos(pos);
            control->EnsureVisible(control->LineFromPosition(pos));
            control->SetSelection(pos, pos + lengthFound);
            //            Manager::Get()->GetMessageManager()->DebugLog("pos=%d, selLen=%d, length=%d", pos, data->end - data->start, lengthFound);
            data->start = pos;
            break; // done
        }
        else if (!wrapAround && !data->findInFiles) // for "find in files" we don't want to show messages
        {
            if (     (data->directionDown && data->start != StartPos) ||
                     (!data->directionDown && data->start != EndPos)     )
            {
                wxString msg;
                if(!data->scope == 1)
                {
                    if (data->directionDown)
                        msg = _("Text not found.\nSearch from the start of the document?");
                    else
                        msg = _("Text not found.\nSearch from the end of the document?");
                }
                else
                {
                    if (data->directionDown)
                        msg = _("Text not found.\nSearch from the start of the selection?");
                    else
                        msg = _("Text not found.\nSearch from the end of the selection?");
                }

                bool auto_wrap_around = Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/auto_wrap_search"), true);
                if (auto_wrap_around)
                    wxBell();
                if (auto_wrap_around || cbMessageBox(msg, _("Result"), wxOK | wxCANCEL | wxICON_QUESTION) == wxID_OK)
                {
                    wrapAround = true; // signal the wrap-around
                    if(!data->scope == 1)
                    {
                        if (data->directionDown)
                        {
                            data->start = 0;
                            data->end = control->GetLength();
                        }
                        else
                        {
                            data->start = control->GetLength();
                            data->end = 0;
                        }
                    }
                    else
                    {
                        if (data->directionDown)
                        {
                            data->start = data->SearchInSelectionStart;
                            data->end = data->SearchInSelectionEnd;
                        }
                        else
                        {
                            data->start = data->SearchInSelectionEnd;
                            data->end = data->SearchInSelectionStart;
                        }
                    }
                }
                else
                    break; // done
            }
            else
            {
                wxString msg;
                msg.Printf(_("Not found: %s"), data->findText.c_str());
                cbMessageBox(msg, _("Result"), wxICON_INFORMATION);
                break; // done
            }
        }
        else if (wrapAround)
        {
            wxString msg;
            msg.Printf(_("Not found: %s"), data->findText.c_str());
            cbMessageBox(msg, _("Result"), wxICON_INFORMATION);
            break; // done
        }
        else
            break; // done
    }
    return pos;
}

int EditorManager::FindInFiles(cbFindReplaceData* data)
{
    // clear old search results
    if ( data->delOldSearches )
    {
        m_pSearchLog->GetListControl()->DeleteAllItems();
    }
    int oldcount = m_pSearchLog->GetListControl()->GetItemCount();

    if (!data || data->findText.IsEmpty())
        return 0;

    // let's make a list of all the files to search in
    wxArrayString filesList;

    if (data->scope == 0) // find in project files
    {
        // fill the search list with all the project files
        cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
        if (!prj)
            return 0;

        wxString fullpath = _T("");
        for (int i = 0; i < prj->GetFilesCount(); ++i)
        {
            ProjectFile* pf = prj->GetFile(i);
            if (pf)
            {
                fullpath = pf->file.GetFullPath();
                if (filesList.Index(fullpath) == -1) // avoid adding duplicates
                {
                    if(wxFileExists(fullpath))  // Does the file exist?
                        filesList.Add(fullpath);
                }
            }
        }
    }
    else if (data->scope == 1) // find in open files
    {
        // fill the search list with the open files
        for (int i = 0; i < m_pNotebook->GetPageCount(); ++i)
        {
            cbEditor* ed = InternalGetBuiltinEditor(i);
            if (ed)
                filesList.Add(ed->GetFilename());
        }
    }
    else if (data->scope == 2) // find in custom search path and mask
    {
        // fill the search list with the files found under the search path
        int flags = wxDIR_FILES |
                    (data->recursiveSearch ? wxDIR_DIRS : 0) |
                    (data->hiddenSearch ? wxDIR_HIDDEN : 0);
        wxArrayString masks = GetArrayFromString(data->searchMask);
        if (!masks.GetCount())
            masks.Add(_T("*"));
        unsigned int count = masks.GetCount();
        wxLogNull ln; // no logging
        for (unsigned int i = 0; i < count; ++i)
        {
            // wxDir::GetAllFiles() does *not* clear the array, so it suits us just fine ;)
            wxDir::GetAllFiles(data->searchPath, &filesList, masks[i], flags);
        }
    }
    else if (data->scope == 3) // find in workspace
    {
		// loop over all the projects in the workspace (they are contained in the ProjectManager)
		const ProjectsArray* pProjects = Manager::Get()->GetProjectManager()->GetProjects();
		if(pProjects)
		{
			int count = pProjects->GetCount();
			for (int idxProject = 0; idxProject < count; ++idxProject)
			{
				cbProject* pProject = pProjects->Item(idxProject);
				if(pProject)
				{
					wxString fullpath = _T("");
					for (int idxFile = 0; idxFile < pProject->GetFilesCount(); ++idxFile)
					{
						ProjectFile* pf = pProject->GetFile(idxFile);
						if (pf)
						{
							fullpath = pf->file.GetFullPath();
							if (filesList.Index(fullpath) == -1) // avoid adding duplicates
							{
								if(wxFileExists(fullpath))  // Does the file exist?
									filesList.Add(fullpath);
							}
						}
					} // end for : idx : idxFile
				}
			} // end for : idx : idxProject
		}
    }

    // if the list is empty, leave
    if (filesList.GetCount() == 0)
    {
        cbMessageBox(_("No files to search in!"), _("Error"), wxICON_WARNING);
        return 0;
    }

    // now that are list is filled, we 'll search
    // but first we 'll create a hidden cbStyledTextCtrl to do the search for us ;)
    cbStyledTextCtrl* control = new cbStyledTextCtrl(m_pNotebook, -1, wxDefaultPosition, wxSize(0, 0));
    control->Show(false); //hidden

    // let's create a progress dialog because it might take some time depending on the files count
    wxProgressDialog* progress = new wxProgressDialog(_("Find in files"),
                                 _("Please wait while searching inside the files..."),
                                 filesList.GetCount(),
                                 Manager::Get()->GetAppWindow(),
                                 wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_CAN_ABORT);

    PlaceWindow(progress);

    // keep a copy of the find struct
    cbFindReplaceData localData = *data;

    if ( !data->delOldSearches )
    {
        LogSearch(_T("=========="), -1, _T("=== \"") + data->findText + _T("\" ==="));
        oldcount++;
    }

    int lastline = -1;
    int count = 0;
    for (size_t i = 0; i < filesList.GetCount(); ++i)
    {
        // update the progress bar
        if (!progress->Update(i))
            break; // user pressed "Cancel"

        // re-initialize the find struct for every file searched
        *data = localData;

        // first load the file in the control
        if (!control->LoadFile(filesList[i]))
        {
            //            LOGSTREAM << _("Failed opening ") << filesList[i] << wxT('\n');
            continue; // failed
        }

        // now search for first occurence
        if (Find(control, data) == -1)
        {
            lastline = -1;
            continue;
        }

        int line = control->LineFromPosition(control->GetSelectionStart());
        lastline = line;

        // make the filename relative
        wxString filename = filesList[i];
        if (filename.StartsWith(data->searchPath))
            filename.Remove(0, data->searchPath.Length());

        // log it
        LogSearch(filename, line + 1, control->GetLine(line));
        ++count;

        // now loop finding the next occurence
        while (FindNext(true, control, data) != -1)
        {
            // log it
            line = control->LineFromPosition(control->GetSelectionStart());
            if(line == lastline)  // avoid multiple hits on the same line (try search for "manager")
                continue;

            lastline = line;
            LogSearch(filename, line + 1, control->GetLine(line));
            ++count;
        }
    }
    delete control; // done with it
    delete progress; // done here too

    if (count > 0)
    {
        static_cast<SearchResultsLog*>(m_pSearchLog)->SetBasePath(data->searchPath);
        Manager::Get()->GetMessageManager()->ShowLog(m_SearchLogIndex);
        Manager::Get()->GetMessageManager()->SwitchTo(m_SearchLogIndex);
        if (Manager::Get()->GetConfigManager(_T("message_manager"))->ReadBool(_T("/auto_show_search"), true))
            Manager::Get()->GetMessageManager()->Open();
        static_cast<SearchResultsLog*>(m_pSearchLog)->FocusEntry(oldcount);
    }
    else
    {
        wxString msg;
        if ( data->delOldSearches )
        {
            msg.Printf(_("Not found: %s"), data->findText.c_str());
            cbMessageBox(msg, _("Result"), wxICON_INFORMATION);
        }
        else
        {
            msg.Printf(_("not found in %d files"), filesList.GetCount());
            LogSearch(_T(""), -1, msg );
            static_cast<SearchResultsLog*>(m_pSearchLog)->FocusEntry(oldcount);
        }
    }

    return count;
}

int EditorManager::FindNext(bool goingDown, cbStyledTextCtrl* control, cbFindReplaceData* data)
{
    if (!control)
    {
        cbEditor* ed = GetBuiltinEditor(GetActiveEditor());
        if (ed)
            control = ed->GetControl();
    }
    if (!control)
        return -1;

    if (!data)
        data = m_LastFindReplaceData;

    if (!data)
        return ShowFindDialog(false, false);

    if(!data->findInFiles)
    {
        wxString phraseAtCursor = control->GetSelectedText();
        // NOTE (mandrav): it is not intuitive to change the search text
        //                  just because there's a selection present...
        // change findText to selected text (if any text is selected and no search text was set before)
        if (!phraseAtCursor.IsEmpty() && data->findText.IsEmpty())
            data->findText = phraseAtCursor;
    }

    data->directionDown = goingDown;
    return Find(control, data);
}

void EditorManager::OnGenericContextMenuHandler(wxCommandEvent& event)
{
    EditorBase* eb = GetActiveEditor();
    cbEditor* ed = GetBuiltinEditor(eb);
    int id = event.GetId();

    if (id == idNBTabSplitHorz && ed)
        ed->Split(cbEditor::stHorizontal);
    else if (id == idNBTabSplitVert && ed)
        ed->Split(cbEditor::stVertical);
    else if (id == idNBTabUnsplit && ed)
        ed->Unsplit();
}

void EditorManager::OnPageChanged(wxFlatNotebookEvent& event)
{
    EditorBase* eb = static_cast<EditorBase*>(m_pNotebook->GetPage(event.GetSelection()));
    //    LOGSTREAM << wxString::Format(_T("OnPageChanged(): ed=%p, title=%s\n"), eb, eb ? eb->GetTitle().c_str() : _T(""));
    CodeBlocksEvent evt(cbEVT_EDITOR_ACTIVATED, -1, 0, eb);
    Manager::Get()->GetPluginManager()->NotifyPlugins(evt);

    RefreshOpenedFilesTree();

    // focus editor on next update event
    m_pData->m_SetFocusFlag = true;

    event.Skip(); // allow others to process it too
}

void EditorManager::OnPageChanging(wxFlatNotebookEvent& event)
{
    EditorBase* eb = static_cast<EditorBase*>(m_pNotebook->GetPage(event.GetSelection()));
    //    LOGSTREAM << wxString::Format(_T("OnPageChanging(): ed=%p, title=%s\n"), eb, eb ? eb->GetTitle().c_str() : _T(""));
    CodeBlocksEvent evt(cbEVT_EDITOR_DEACTIVATED, -1, 0, eb);
    Manager::Get()->GetPluginManager()->NotifyPlugins(evt);

    event.Skip(); // allow others to process it too
}

void EditorManager::OnPageClosing(wxFlatNotebookEvent& event)
{
    EditorBase* eb = static_cast<EditorBase*>(m_pNotebook->GetPage(event.GetSelection()));
    //    LOGSTREAM << wxString::Format(_T("OnPageClosing(): ed=%p, title=%s\n"), eb, eb ? eb->GetTitle().c_str() : _T(""));
    if (!QueryClose(eb))
        event.Veto();
    event.Skip(); // allow others to process it too
}

void EditorManager::OnPageContextMenu(wxFlatNotebookEvent& event)
{
    if (event.GetSelection() == -1)
        return;
    wxMenu* pop = new wxMenu;
    pop->Append(idNBTabClose, _("Close"));
    if (GetEditorsCount() > 1)
    {
        pop->Append(idNBTabCloseAll, _("Close all"));
        pop->Append(idNBTabCloseAllOthers, _("Close all others"));
    }
    pop->AppendSeparator();
    pop->Append(idNBTabSave, _("Save"));
    pop->Append(idNBTabSaveAll, _("Save all"));
    pop->AppendSeparator();
    pop->Append(idNBSwapHeaderSource, _("Swap header/source"));
    pop->AppendSeparator();
    pop->Append(idNBTabTop, _("Tabs at top"));
    pop->Append(idNBTabBottom, _("Tabs at bottom"));

    cbEditor* ed = GetBuiltinEditor(event.GetSelection());
    if (ed)
    {
        wxMenu* splitMenu = new wxMenu;
        splitMenu->Append(idNBTabSplitHorz, _("Horizontally"));
        splitMenu->Append(idNBTabSplitVert, _("Vertically"));
        splitMenu->AppendSeparator();
        splitMenu->Append(idNBTabUnsplit, _("Unsplit"));
        splitMenu->Enable(idNBTabSplitHorz, ed->GetSplitType() != cbEditor::stHorizontal);
        splitMenu->Enable(idNBTabSplitVert, ed->GetSplitType() != cbEditor::stVertical);
        splitMenu->Enable(idNBTabUnsplit, ed->GetSplitType() != cbEditor::stNoSplit);

        pop->AppendSeparator();
        pop->Append(-1, _("Split view..."), splitMenu);
    }

    bool any_modified = false;

    for(int i = 0; i < GetEditorsCount(); ++i)
    {
        EditorBase* ed = GetEditor(i);
        if (ed && ed->GetModified())
        {
            any_modified = true;
            break;
        }
    }

    pop->Enable(idNBTabSave, GetEditor(event.GetSelection())->GetModified());
    pop->Enable(idNBTabSaveAll, any_modified );

    m_pNotebook->PopupMenu(pop);
    delete pop;
}

void EditorManager::OnClose(wxCommandEvent& event)
{
    Manager::Get()->GetEditorManager()->Close(GetActiveEditor());
}

void EditorManager::OnCloseAll(wxCommandEvent& event)
{
    Manager::Get()->GetEditorManager()->CloseAll();
}

void EditorManager::OnCloseAllOthers(wxCommandEvent& event)
{
    Manager::Get()->GetEditorManager()->CloseAllExcept(GetActiveEditor());
}

void EditorManager::OnSave(wxCommandEvent& event)
{
    Manager::Get()->GetEditorManager()->Save(m_pNotebook->GetSelection());
}

void EditorManager::OnSaveAll(wxCommandEvent& event)
{
    Manager::Get()->GetEditorManager()->SaveAll();
}

void EditorManager::OnSwapHeaderSource(wxCommandEvent& event)
{
    Manager::Get()->GetEditorManager()->SwapActiveHeaderSource();
}

void EditorManager::OnTabPosition(wxCommandEvent& event)
{
    long style = m_pNotebook->GetWindowStyleFlag();
    style &= ~wxFNB_BOTTOM;

    if (event.GetId() == idNBTabBottom)
        style |= wxFNB_BOTTOM;
    m_pNotebook->SetWindowStyleFlag(style);
    // (style & wxFNB_BOTTOM) saves info only about the the tabs position
    Manager::Get()->GetConfigManager(_T("app"))->Write(_T("/environment/editor_tabs_bottom"), (bool)(style & wxFNB_BOTTOM));
}

void EditorManager::OnAppDoneStartup(wxCommandEvent& event)
{
    event.Skip(); // allow others to process it too
}

void EditorManager::OnAppStartShutdown(wxCommandEvent& event)
{
    event.Skip(); // allow others to process it too
}

void EditorManager::OnCheckForModifiedFiles(wxCommandEvent& event)
{
    CheckForExternallyModifiedFiles();
    cbEditor* ed = GetBuiltinActiveEditor();
    if (ed)
        ed->GetControl()->SetFocus();
}


bool EditorManager::OpenFilesTreeSupported()
{
#ifdef DONT_USE_OPENFILES_TREE
    return false;
#else

    return true;
#endif
}

// This function is not used anymore, so we turned it into a synonym
// for RefreshOpenedFilesTree().
void EditorManager::RefreshOpenFilesTree()
{
    RefreshOpenedFilesTree();
}

void EditorManager::ShowOpenFilesTree(bool show)
{
    if (!OpenFilesTreeSupported())
        return;
    if (!m_pTree)
        InitPane();
    if (!m_pTree)
        return;
    if(Manager::isappShuttingDown())
        return;
    //    if (show && !IsOpenFilesTreeVisible())
    //        m_pTree->Show(true);
    //    else if (!show && IsOpenFilesTreeVisible())
    //        m_pTree->Show(false);
    RefreshOpenFilesTree();

    CodeBlocksDockEvent evt(show ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
    evt.pWindow = m_pTree;
    Manager::Get()->GetAppWindow()->ProcessEvent(evt);

    // update user prefs
    Manager::Get()->GetConfigManager(_T("editor"))->Write(_T("/show_opened_files_tree"), show);
}

bool EditorManager::IsOpenFilesTreeVisible() const
{
    return m_pTree && m_pTree->IsShown();
}

wxTreeCtrl* EditorManager::GetTree()
{
    return m_pTree;
    // Manager::Get()->GetProjectManager()->GetTree();
}

wxTreeItemId EditorManager::FindTreeFile(const wxString& filename)
{
    wxTreeItemId item = wxTreeItemId();
    do
    {
        if(Manager::isappShuttingDown())
            break;
        if(filename==_T(""))
            break;
        wxTreeCtrl *tree=GetTree();
        if(!tree || !m_pData->m_TreeOpenedFiles)
            break;
#if !wxCHECK_VERSION(2,5,0)

        long int cookie = 0;
#else

        wxTreeItemIdValue cookie; //2.6.0
#endif

        for(item = tree->GetFirstChild(m_pData->m_TreeOpenedFiles,cookie);
                item;
                item = tree->GetNextChild(m_pData->m_TreeOpenedFiles, cookie))
        {
            if(GetTreeItemFilename(item)==filename)
                break;
        }
    }
    while(false);
    return item;
}

wxString EditorManager::GetTreeItemFilename(wxTreeItemId item)
{
    if(Manager::isappShuttingDown())
        return _T("");
    wxTreeCtrl *tree=GetTree();
    if(!tree || !m_pData->m_TreeOpenedFiles || !item)
        return _T("");
    MiscTreeItemData *data=(MiscTreeItemData*)tree->GetItemData(item);
    if(!data)
        return _T("");
    if(data->GetOwner()!=this)
        return _T("");
    return ((EditorTreeData*)data)->GetFullName();
}

void EditorManager::DeleteItemfromTree(wxTreeItemId item)
{
    if(Manager::isappShuttingDown())
        return;
    wxTreeCtrl *tree=GetTree();
    if(!tree || !m_pData->m_TreeOpenedFiles || !item)
        return;
    wxTreeItemId itemparent=tree->GetItemParent(item);
    if(itemparent!=m_pData->m_TreeOpenedFiles)
        return;
    tree->Delete(item);
}

void EditorManager::DeleteFilefromTree(const wxString& filename)
{
    if(Manager::isappShuttingDown())
        return;
    DeleteItemfromTree(FindTreeFile(filename));
    //    RefreshOpenedFilesTree();
}

void EditorManager::AddFiletoTree(EditorBase* ed)
{
    if(Manager::isappShuttingDown())
        return;
    if(!ed)
        return;
    if(!ed->VisibleToTree())
        return;
    wxString shortname=ed->GetShortName();
    wxString filename=ed->GetFilename();
    wxTreeItemId item=FindTreeFile(filename);
    if(item.IsOk())
        return;
    wxTreeCtrl *tree=GetTree();
    if(!tree)
        return;
    if(!m_pData->m_TreeOpenedFiles)
        return;
    int mod = ed->GetModified() ? 2 : 1;
    tree->AppendItem(m_pData->m_TreeOpenedFiles,shortname,mod,mod,
                     new EditorTreeData(this,filename));
    tree->SortChildren(m_pData->m_TreeOpenedFiles);
    RefreshOpenedFilesTree(true);
}

void EditorManager::HideNotebook()
{
    //    if(!this)
    //        return;
    //    if(m_pNotebook)
    //        m_pNotebook->Hide();
    //    m_pData->m_NeedsRefresh = false;
    //    return;
}

void EditorManager::ShowNotebook()
{
    //    if(!this)
    //        return;
    //    if(m_pNotebook)
    //        m_pNotebook->Show();
    //    m_pData->m_NeedsRefresh = true;
    //    m_pData->InvalidateTree();
    //    return;
}

bool EditorManager::RenameTreeFile(const wxString& oldname, const wxString& newname)
{
    if(Manager::isappShuttingDown())
        return false;
    wxTreeCtrl *tree = GetTree();
    if(!tree)
        return false;
#if !wxCHECK_VERSION(2,5,0)

    long int cookie = 0;
#else

    wxTreeItemIdValue cookie; //2.6.0
#endif

    wxTreeItemId item;
    wxString filename,shortname;
    for(item=tree->GetFirstChild(m_pData->m_TreeOpenedFiles,cookie);
            item;
            item = tree->GetNextChild(m_pData->m_TreeOpenedFiles, cookie))
    {
        EditorTreeData *data=(EditorTreeData*)tree->GetItemData(item);
        if(!data)
            continue;
        filename=data->GetFullName();
        if(filename!=oldname)
            continue;
        data->SetFullName(newname);
        EditorBase *ed=GetEditor(filename);
        if(ed)
        {
            shortname=ed->GetShortName();
            int mod = ed->GetModified() ? 2 : 1;
            if(tree->GetItemText(item)!=shortname)
                tree->SetItemText(item,shortname);
            if (tree->GetItemImage(item) != mod)
            {
                tree->SetItemImage(item, mod, wxTreeItemIcon_Normal);
                tree->SetItemImage(item, mod, wxTreeItemIcon_Selected);
            }
            if(ed==GetActiveEditor())
                tree->SelectItem(item);
        }
        return true;
    }
    return false;
}

void EditorManager::InitPane()
{
#if defined(DONT_USE_OPENFILES_TREE)
    return;
#endif

    BuildOpenedFilesTree(Manager::Get()->GetAppWindow());
    if (!m_pTree)
        return;
    CodeBlocksDockEvent evt(cbEVT_ADD_DOCK_WINDOW);
    evt.name = _T("OpenFilesPane");
    evt.title = _("Open files list");
    evt.pWindow = m_pTree;
    evt.minimumSize.Set(50, 50);
    evt.desiredSize.Set(150, 100);
    evt.floatingSize.Set(100, 150);
    evt.dockSide = CodeBlocksDockEvent::dsLeft;
    evt.stretch = true;
    Manager::Get()->GetAppWindow()->ProcessEvent(evt);
}

void EditorManager::BuildOpenedFilesTree(wxWindow* parent)
{
#if defined(DONT_USE_OPENFILES_TREE)
    return;
#endif

    if(m_pTree)
        return;
    m_pTree = new wxTreeCtrl(parent, ID_EditorManager,wxDefaultPosition,wxSize(150, 100),wxTR_HAS_BUTTONS | wxNO_BORDER);
    m_pData->BuildTree(m_pTree);
    RebuildOpenedFilesTree(m_pTree);
}

void EditorManager::RebuildOpenedFilesTree(wxTreeCtrl *tree)
{
#if defined(DONT_USE_OPENFILES_TREE)
    return;
#endif

    if(Manager::isappShuttingDown())
        return;
    if(!tree)
        tree=GetTree();
    if(!tree)
        return;
    tree->Freeze();
    tree->DeleteChildren(m_pData->m_TreeOpenedFiles);
    if(!GetEditorsCount())
    {
        tree->Thaw();
        return;
    }
    for (int i = 0; i < m_pNotebook->GetPageCount(); ++i)
    {
        EditorBase* ed = InternalGetEditorBase(i);
        if(!ed)
            continue;
        if(!ed->VisibleToTree())
            continue;
        wxString shortname=ed->GetShortName();
        int mod = ed->GetModified() ? 2 : 1;
        wxTreeItemId item=tree->AppendItem(m_pData->m_TreeOpenedFiles,shortname,mod,mod,
                                           new EditorTreeData(this,ed->GetFilename()));
        if(GetActiveEditor()==ed)
            tree->SelectItem(item);
    }
    tree->Expand(m_pData->m_TreeOpenedFiles);
    tree->Thaw();
    m_pData->InvalidateTree();
}

void EditorManager::RefreshOpenedFilesTree(bool force)
{
#if defined(DONT_USE_OPENFILES_TREE)
    return;
#endif

    if(Manager::isappShuttingDown())
        return;
    if(!m_pTree)
        return;
    wxString fname;
    EditorBase *aed=GetActiveEditor();
    if(!aed)
        return;
    if(!aed->VisibleToTree())
        return;
    bool ismodif=aed->GetModified();
    fname=aed->GetFilename();

    if(!force && m_LastActiveFile==fname && m_LastModifiedflag==ismodif)
        return; // Nothing to do

    m_LastActiveFile=fname;
    m_LastModifiedflag=ismodif;
    m_pTree->Freeze();
#if !wxCHECK_VERSION(2,5,0)

    long int cookie = 0;
#else

    wxTreeItemIdValue cookie; //2.6.0
#endif

    wxTreeItemId item = m_pTree->GetFirstChild(m_pData->m_TreeOpenedFiles,cookie);
    wxString filename,shortname;
    while (item)
    {
        EditorTreeData *data=(EditorTreeData*)m_pTree->GetItemData(item);
        if(data)
        {
            filename=data->GetFullName();
            EditorBase *ed=GetEditor(filename);
            if(ed)
            {
                shortname=ed->GetShortName();
                int mod = ed->GetModified() ? 2 : 1;
                if(m_pTree->GetItemText(item)!=shortname)
                    m_pTree->SetItemText(item,shortname);
                if (m_pTree->GetItemImage(item) != mod)
                {
                    m_pTree->SetItemImage(item, mod, wxTreeItemIcon_Normal);
                    m_pTree->SetItemImage(item, mod, wxTreeItemIcon_Selected);
                }
                if(ed==aed)
                    m_pTree->SelectItem(item);
                // m_pTree->SetItemBold(item,(ed==aed));
            }
        }
        item = m_pTree->GetNextChild(m_pData->m_TreeOpenedFiles, cookie);
    }
    m_pTree->Thaw();
}

void EditorManager::OnTreeItemActivated(wxTreeEvent &event)
{
    if(Manager::isappShuttingDown())
        return;
    if(!MiscTreeItemData::OwnerCheck(event,GetTree(),this,true))
        return;
    wxString filename=GetTreeItemFilename(event.GetItem());
    if(filename==_T(""))
        return;
    Open(filename);
}

void EditorManager::OnTreeItemRightClick(wxTreeEvent &event)
{
    if(Manager::isappShuttingDown())
        return;
    if(!MiscTreeItemData::OwnerCheck(event,GetTree(),this,true))
        return;
    wxString filename=GetTreeItemFilename(event.GetItem());
    if(filename.IsEmpty())
        return;
    EditorBase* ed = GetEditor(filename);
    if(ed)
    {
        wxPoint pt = m_pTree->ClientToScreen(event.GetPoint());
        ed->DisplayContextMenu(pt,mtOpenFilesList); //pecan 2006/03/22
    }
}

void EditorManager::OnUpdateUI(wxUpdateUIEvent& event)
{
    if (m_pData->m_SetFocusFlag)
    {
        cbEditor* ed = GetBuiltinActiveEditor();
        if (ed)
            ed->GetControl()->SetFocus();
        m_pData->m_SetFocusFlag = false;
    }

    // allow other UpdateUI handlers to process this event
    // *very* important! don't forget it...
    event.Skip();
}

void EditorManager::SetZoom(int zoom)
{
    m_zoom = zoom;
}

int EditorManager::GetZoom() const
{
    return m_zoom;
}


/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/dir.h>
    #include <wx/menu.h>
    #include <wx/notebook.h>
    #include <wx/settings.h>
    #include <wx/splitter.h>
    #include <wx/xrc/xmlres.h>

    #include "cbeditor.h"
    #include "cbproject.h"
    #include "compiler.h"
    #include "compilerfactory.h"
    #include "configmanager.h"
    #include "editormanager.h" // class's header file
    #include "filemanager.h"
    #include "logmanager.h"
    #include "macrosmanager.h"
    #include "manager.h"
    #include "pluginmanager.h"
    #include "projectbuildtarget.h"
    #include "projectmanager.h"
    #include "sdk_events.h"
#endif

#include "annoyingdialog.h"
#include "cbstyledtextctrl.h"
#include "cbcolourmanager.h"

#include <wx/bmpbuttn.h>
#include <wx/clipbrd.h>  // for wxTheClipboard
#include <wx/fontutil.h>
#include <wx/progdlg.h>

#include "cbauibook.h"
#include "editorcolourset.h"
#include "confirmreplacedlg.h"
#include "filefilters.h"
#include "projectfileoptionsdlg.h"
#include "filegroupsandmasks.h"

template<> EditorManager* Mgr<EditorManager>::instance = nullptr;
template<> bool  Mgr<EditorManager>::isShutdown = false;

int ID_NBEditorManager        = wxNewId();
int ID_EditorManager          = wxNewId();
int idEditorManagerCheckFiles = wxNewId();

// static
bool EditorManager::s_CanShutdown = true;

// needed for initialization of variables
inline int cbRegisterId(int id)
{
    wxRegisterId(id);
    return id;
}

static const int idNBTabSplitHorz            = wxNewId();
static const int idNBTabSplitVert            = wxNewId();
static const int idNBTabUnsplit              = wxNewId();
static const int idNBTabClose                = wxNewId();
static const int idNBTabCloseAll             = wxNewId();
static const int idNBTabCloseAllOthers       = wxNewId();
static const int idNBTabSave                 = wxNewId();
static const int idNBTabSaveAll              = wxNewId();
static const int idNBSwapHeaderSource        = wxNewId();
static const int idNBTabOpenContainingFolder = wxNewId();
static const int idNBTabTop                  = wxNewId();
static const int idNBTabBottom               = wxNewId();
static const int idNBProperties              = wxNewId();
static const int idNBAddFileToProject        = wxNewId();
static const int idNBRemoveFileFromProject   = wxNewId();
static const int idNBShowFileInTree          = wxNewId();

// The following lines reserve 255 consecutive id's
static const int EditorMaxSwitchTo           = 255;
static const int idNBSwitchFile1             = wxNewId();
static const int idNBSwitchFileMax           = cbRegisterId(idNBSwitchFile1 + EditorMaxSwitchTo - 1);


/** *******************************************************
  * struct EditorManagerInternalData                      *
  * This is the private data holder for the EditorManager *
  * All data not relevant to other classes should go here *
  ********************************************************* */

struct EditorManagerInternalData
{
    /* Methods */

    EditorManagerInternalData(EditorManager* owner)
            : m_pOwner(owner)
    {}

    /* Static data */

    EditorManager* m_pOwner;
    bool m_SetFocusFlag;
    wxString m_SelectionClipboard;
};

// *********** End of EditorManagerInternalData **********


BEGIN_EVENT_TABLE(EditorManager, wxEvtHandler)
    EVT_APP_STARTUP_DONE(EditorManager::OnAppDoneStartup)
    EVT_APP_START_SHUTDOWN(EditorManager::OnAppStartShutdown)
    EVT_AUINOTEBOOK_PAGE_CHANGED(ID_NBEditorManager, EditorManager::OnPageChanged)
    EVT_AUINOTEBOOK_PAGE_CHANGING(ID_NBEditorManager, EditorManager::OnPageChanging)
    EVT_AUINOTEBOOK_PAGE_CLOSE(ID_NBEditorManager, EditorManager::OnPageClose)
    EVT_AUINOTEBOOK_TAB_RIGHT_UP(ID_NBEditorManager, EditorManager::OnPageContextMenu)
    EVT_MENU_RANGE(idNBSwitchFile1, idNBSwitchFileMax, EditorManager::OnGenericContextMenuHandler)
    EVT_MENU(idNBTabSplitHorz, EditorManager::OnGenericContextMenuHandler)
    EVT_MENU(idNBTabSplitVert, EditorManager::OnGenericContextMenuHandler)
    EVT_MENU(idNBTabUnsplit, EditorManager::OnGenericContextMenuHandler)
    EVT_MENU(idNBTabTop, EditorManager::OnTabPosition)
    EVT_MENU(idNBTabBottom, EditorManager::OnTabPosition)
    EVT_MENU(idNBTabClose, EditorManager::OnClose)
    EVT_MENU(idNBTabCloseAll, EditorManager::OnCloseAll)
    EVT_MENU(idNBTabCloseAllOthers, EditorManager::OnCloseAllOthers)
    EVT_MENU(idNBTabOpenContainingFolder, EditorManager::OnOpenContainingFolder)
    EVT_MENU(idNBTabSave, EditorManager::OnSave)
    EVT_MENU(idNBTabSaveAll, EditorManager::OnSaveAll)
    EVT_MENU(idNBSwapHeaderSource, EditorManager::OnSwapHeaderSource)
    EVT_MENU(idNBProperties, EditorManager::OnProperties)
    EVT_MENU(idNBAddFileToProject, EditorManager::OnAddFileToProject)
    EVT_MENU(idNBRemoveFileFromProject, EditorManager::OnRemoveFileFromProject)
    EVT_MENU(idNBShowFileInTree, EditorManager::OnShowFileInTree)
    EVT_MENU(idEditorManagerCheckFiles, EditorManager::OnCheckForModifiedFiles)
END_EVENT_TABLE()

EditorManager::EditorManager()
        : m_pNotebook(nullptr),
        m_pNotebookStackHead(new cbNotebookStack),
        m_pNotebookStackTail(m_pNotebookStackHead),
        m_nNotebookStackSize(0),
        m_isCheckingForExternallyModifiedFiles(false)
{
    m_pData = new EditorManagerInternalData(this);

    m_pNotebook = new cbAuiNotebook(Manager::Get()->GetAppWindow(), ID_NBEditorManager, wxDefaultPosition, wxDefaultSize,
                                    wxAUI_NB_DEFAULT_STYLE | wxAUI_NB_WINDOWLIST_BUTTON | wxNO_FULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN);

    if (Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/editor_tabs_bottom"), false))
        m_pNotebook->SetWindowStyleFlag(m_pNotebook->GetWindowStyleFlag() | wxAUI_NB_BOTTOM);

    Manager::Get()->GetLogManager()->DebugLog(_T("Initialize EditColourSet ....."));
    m_Theme = new EditorColourSet(Manager::Get()->GetConfigManager(_T("editor"))->Read(_T("/colour_sets/active_colour_set"), COLORSET_DEFAULT));
    Manager::Get()->GetLogManager()->DebugLog(_T("Initialize EditColourSet: done."));

    Manager::Get()->GetAppWindow()->PushEventHandler(this);

    m_Zoom = Manager::Get()->GetConfigManager(_T("editor"))->ReadInt(_T("/zoom"));
    Manager::Get()->RegisterEventSink(cbEVT_BUILDTARGET_SELECTED,       new cbEventFunctor<EditorManager, CodeBlocksEvent>(this, &EditorManager::CollectDefines));
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_ACTIVATE,           new cbEventFunctor<EditorManager, CodeBlocksEvent>(this, &EditorManager::CollectDefines));
    Manager::Get()->RegisterEventSink(cbEVT_WORKSPACE_LOADING_COMPLETE, new cbEventFunctor<EditorManager, CodeBlocksEvent>(this, &EditorManager::CollectDefines));
    Manager::Get()->RegisterEventSink(cbEVT_APP_ACTIVATED,              new cbEventFunctor<EditorManager, CodeBlocksEvent>(this, &EditorManager::OnAppActivated));

    ColourManager *colours = Manager::Get()->GetColourManager();
    colours->RegisterColour(_("Editor"), _("Caret"), wxT("editor_caret"), *wxBLACK);
    colours->RegisterColour(_("Editor"), _("Right margin"), wxT("editor_gutter"), *wxLIGHT_GREY);
    colours->RegisterColour(_("Editor"), _("Line numbers foreground colour"), wxT("editor_linenumbers_fg"),
                            wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
    colours->RegisterColour(_("Editor"), _("Line numbers background colour"), wxT("editor_linenumbers_bg"),
                            wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

    // These two are taken from Platform::Chrome() and Platform::ChromeHightlight()
    colours->RegisterColour(_("Editor"), _("Margin chrome colour"), wxT("editor_margin_chrome"),
                            wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
    colours->RegisterColour(_("Editor"), _("Margin chrome highlight colour"), wxT("editor_margin_chrome_highlight"),
                            wxSystemSettings::GetColour(wxSYS_COLOUR_3DHIGHLIGHT));
}

EditorManager::~EditorManager()
{
    DeleteNotebookStack();
    delete m_pNotebookStackHead;
    delete m_Theme;
    delete m_pData;
    Manager::Get()->GetConfigManager(_T("editor"))->Write(_T("/zoom"), m_Zoom);
}

cbNotebookStack* EditorManager::GetNotebookStack()
{
    bool found = false;
    wxWindow* wnd;
    cbNotebookStack* body;
    cbNotebookStack* prev_body;

    while (m_nNotebookStackSize != m_pNotebook->GetPageCount()) // Sync stack with Notebook
    {
        if (m_nNotebookStackSize < m_pNotebook->GetPageCount())
        {
            for (size_t i = 0; i<m_pNotebook->GetPageCount(); ++i)
            {
                wnd = m_pNotebook->GetPage(i);
                found = false;
                for (body = m_pNotebookStackHead->next; body != NULL; body = body->next)
                {
                    if (wnd == body->window)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    m_pNotebookStackTail->next = new cbNotebookStack(wnd);
                    m_pNotebookStackTail = m_pNotebookStackTail->next;
                    ++m_nNotebookStackSize;
                }
            }
        }
        if (m_nNotebookStackSize > m_pNotebook->GetPageCount())
        {
            for (prev_body = m_pNotebookStackHead, body = prev_body->next; body != NULL; prev_body = body, body = body->next)
            {
                if (m_pNotebook->GetPageIndex(body->window) == wxNOT_FOUND)
                {
                    prev_body->next = body->next;
                    delete body;
                    --m_nNotebookStackSize;
                    body = prev_body;
                }
            }
        }
    }

    return m_pNotebookStackHead->next;
}

void EditorManager::DeleteNotebookStack()
{
    cbNotebookStack* tmp;
    while(m_pNotebookStackHead->next)
    {
        tmp = m_pNotebookStackHead->next;
        m_pNotebookStackHead->next = tmp->next;
        delete tmp;
    }
    m_pNotebookStackTail = m_pNotebookStackHead;
    m_nNotebookStackSize = 0;
}

void EditorManager::RebuildNotebookStack()
{
    DeleteNotebookStack();
    for (size_t i = 0; i < m_pNotebook->GetPageCount(); ++i)
    {
        m_pNotebookStackTail->next = new cbNotebookStack(m_pNotebook->GetPage(i));
        m_pNotebookStackTail = m_pNotebookStackTail->next;
        ++m_nNotebookStackSize;
    }
}

void EditorManager::RecreateOpenEditorStyles()
{
    // tell all open editors to re-create their styles
    for (size_t i = 0; i < m_pNotebook->GetPageCount(); ++i)
    {
        cbEditor* ed = InternalGetBuiltinEditor(i);
        if (ed)
        {
            bool saveSuccess = ed->SaveFoldState(); //First Save the old fold levels
            ed->SetEditorStyle();
            if (saveSuccess)
            {
                ed->FixFoldState(); //Compare old fold levels with new and change the bugs
            }
        }
    }
}

int EditorManager::GetEditorsCount()
{
    return m_pNotebook->GetPageCount();
}

EditorBase* EditorManager::InternalGetEditorBase(int page)
{
    if (page >= 0 && page < (int)m_pNotebook->GetPageCount())
    {
        return static_cast<EditorBase*>(m_pNotebook->GetPage(page));
    }
    return nullptr;
}

cbEditor* EditorManager::InternalGetBuiltinEditor(int page)
{
    EditorBase* eb = InternalGetEditorBase(page);
    if (eb && eb->IsBuiltinEditor())
        return (cbEditor*)eb;
    return nullptr;
}

cbEditor* EditorManager::GetBuiltinEditor(EditorBase* eb)
{
    return eb && eb->IsBuiltinEditor() ? (cbEditor*)eb : nullptr;
}

EditorBase* EditorManager::IsOpen(const wxString& filename)
{
    wxString uFilename = UnixFilename(realpath(filename));
    for (size_t i = 0; i < m_pNotebook->GetPageCount(); ++i)
    {
        EditorBase* eb = InternalGetEditorBase(i);
        if (!eb)
            continue;
        wxString fname = eb->GetFilename();

        // MSW must use case-insensitive comparison
        if (fname.IsSameAs(uFilename, platform::windows == false) || fname.IsSameAs(g_EditorModified + uFilename, platform::windows == false))
            return eb;
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

    for (size_t i = 0; i < m_pNotebook->GetPageCount(); ++i)
    {
        cbEditor* ed = InternalGetBuiltinEditor(i);
        if (ed)
            ed->SetColourSet(m_Theme);
    }
}

cbEditor* EditorManager::Open(const wxString& filename, int pos, ProjectFile* data)
{
    return Open(nullptr, filename, pos, data);
}

cbEditor* EditorManager::Open(LoaderBase* fileLdr, const wxString& filename, int /*pos*/, ProjectFile* data)
{
    bool can_updateui = !GetActiveEditor() || !Manager::Get()->GetProjectManager()->IsLoading();
    wxFileName fn(realpath(filename));
    NormalizePath(fn, wxEmptyString);
    wxString fname = UnixFilename(fn.GetFullPath());
    //  Manager::Get()->GetLogManager()->DebugLog("Trying to open '%s'", fname.c_str());
    if (!wxFileExists(fname))
        return nullptr;
    //  Manager::Get()->GetLogManager()->DebugLog("File exists '%s'", fname.c_str());

    // disallow application shutdown while opening files
    // WARNING: remember to set it to true, when exiting this function!!!
    s_CanShutdown = false;

    EditorBase* eb = IsOpen(fname);
    cbEditor* ed = nullptr;
    if (eb)
    {
        if (eb->IsBuiltinEditor())
        {
            ed = (cbEditor*)eb;
        }
        else
        {
            s_CanShutdown = true;
            return nullptr; // is open but not a builtin editor
        }
    }

    if (!ed)
    {
        if (!fileLdr)
            fileLdr = Manager::Get()->GetFileManager()->Load(filename);
        if (fileLdr)
        {
            ed = new cbEditor(m_pNotebook, fileLdr, fname, m_Theme);
            if (ed->IsOK())
                AddEditorBase(ed);
            else
            {
                ed->Destroy();
                ed = nullptr;
            }
        }
    }

    // check for ProjectFile
    if (ed && !ed->GetProjectFile())
    {
        // First checks if we're already being passed a ProjectFile as a parameter
        if (data)
            Manager::Get()->GetLogManager()->DebugLog(_T("Project data set for ") + data->file.GetFullPath());
        else
            Manager::Get()->GetProjectManager()->FindProjectForFile(ed->GetFilename(), &data, false, false);
        if (data)
            ed->SetProjectFile(data,true);
    }

    if (can_updateui)
    {
        if (ed)
        {
            SetActiveEditor(ed); // fires the cbEVT_EDITOR_ACTIVATED event
            ed->GetControl()->SetFocus();
        }
    }

    // we 're done
    s_CanShutdown = true;

    return ed;
}

EditorBase* EditorManager::GetActiveEditor()
{
    if (m_pNotebook->GetPageCount() > 0)
    {
        return InternalGetEditorBase(m_pNotebook->GetSelection());
    }
    return nullptr;
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
    int page = FindPageFromEditor(ed);
    if (page != -1)
    {
        // Previously the Activated event was only sent for built-in editors, which makes no sense
        int sel = m_pNotebook->GetSelection();
        m_pNotebook->SetSelection(page);
        EditorBase* eb_old = nullptr;
        if (sel>=0)
            eb_old = static_cast<EditorBase*>(m_pNotebook->GetPage(sel));

        CodeBlocksEvent evt(cbEVT_EDITOR_SWITCHED, -1, nullptr, ed, nullptr, eb_old);
        Manager::Get()->GetPluginManager()->NotifyPlugins(evt);

        CodeBlocksEvent evt2(cbEVT_EDITOR_ACTIVATED, -1, nullptr, ed);
        Manager::Get()->GetPluginManager()->NotifyPlugins(evt2);
    }

    if (ed->IsBuiltinEditor())
        static_cast<cbEditor*>(ed)->GetControl()->SetFocus();
}

cbEditor* EditorManager::New(const wxString& newFileName)
{
//    wxString old_title = Manager::Get()->GetAppWindow()->GetTitle(); // Fix for Bug #1389450
    // create a dummy file
    if (!newFileName.IsEmpty() && !wxFileExists(newFileName) && wxDirExists(wxPathOnly(newFileName)))
    {
        wxFile f(newFileName, wxFile::write);
        if (!f.IsOpened())
            return nullptr;
    }
    cbEditor* ed = new cbEditor(m_pNotebook, newFileName);
//    if ((newFileName.IsEmpty() && !ed->SaveAs()) || !ed->Save())
//    {
//        //DeletePage(ed->GetPageIndex());
//        ed->Destroy();
//        Manager::Get()->GetAppWindow()->SetTitle(old_title); // Though I can't reproduce the bug, this does no harm
//        return 0;
//    }

    // add default text
    wxString key;
    key.Printf(_T("/default_code/set%d"), (int)FileTypeOf(ed->GetFilename()));
    wxString code = Manager::Get()->GetConfigManager(_T("editor"))->Read(key, wxEmptyString);
    // Allow usage of macros
    // TODO (Morten#5#): Is it worth making this configurable?!
    Manager::Get()->GetMacrosManager()->ReplaceMacros(code);
    ed->GetControl()->SetText(code);

    ed->SetColourSet(m_Theme);
    AddEditorBase(ed);

    ed->Show(true);
    SetActiveEditor(ed);

    CodeBlocksEvent evt(cbEVT_EDITOR_OPEN, -1, nullptr, ed);
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
        // use fullname as default, so tabs stay as small as possible
        wxFileName fn(eb->GetTitle());
        m_pNotebook->AddPage(eb, fn.GetFullName(), true);
    }
}

void EditorManager::RemoveEditorBase(EditorBase* eb, cb_unused bool deleteObject)
{
    int page = FindPageFromEditor(eb);
    if (page != -1 && !Manager::IsAppShuttingDown())
        m_pNotebook->RemovePage(page);

    //    if (deleteObject)
    //        eb->Destroy();
}

bool EditorManager::UpdateProjectFiles(cbProject* project)
{
    for (size_t i = 0; i < m_pNotebook->GetPageCount(); ++i)
    {
        cbEditor* ed = InternalGetBuiltinEditor(i);
        if (!ed)
            continue;
        ProjectFile* pf = ed->GetProjectFile();
        if (!pf)
            continue;
        if (pf->GetParentProject() != project)
            continue;

        pf->editorTabPos = m_pNotebook->GetTabPositionFromIndex(i) + 1;

        ed->UpdateProjectFile();
    }
    return true;
}

bool EditorManager::CloseAll(bool dontsave)
{
    return CloseAllExcept(GetEditor(_("Start here")), dontsave);
}

bool EditorManager::QueryCloseAll()
{
    for (int i = m_pNotebook->GetPageCount() - 1; i >= 0; --i)
    {
        EditorBase* eb = InternalGetEditorBase(i);
        if (eb && !QueryClose(eb))
            return false; // aborted
    }
    return true;
}

bool EditorManager::CloseAllExcept(EditorBase* editor, bool dontsave)
{
    if (!dontsave)
    {
        for (size_t i = 0; i < m_pNotebook->GetPageCount(); ++i)
        {
            EditorBase* eb = InternalGetEditorBase(i);
            if (eb && eb != editor && !QueryClose(eb))
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
    return Close(GetActiveEditor(), dontsave);
}

bool EditorManager::QueryClose(EditorBase *ed)
{
    if (!ed)
        return true;

    return ed->QueryClose();
}

int EditorManager::FindPageFromEditor(EditorBase* eb)
{
    if (!m_pNotebook || !eb)
        return -1;

    for (size_t i = 0; i < m_pNotebook->GetPageCount(); ++i)
    {
        if (m_pNotebook->GetPage(i) == eb)
            return i;
    }
    return -1;
}

bool EditorManager::Close(const wxString& filename, bool dontsave)
{
    return Close(IsOpen(filename), dontsave);
}

bool EditorManager::Close(EditorBase* editor, bool dontsave)
{
    if (editor)
    {
        int idx = FindPageFromEditor(editor);
        if (idx != -1)
        {
            if (!dontsave)
                if (!QueryClose(editor))
                    return false;
            m_pNotebook->DeletePage(idx);
        }
    }
    return true;
}

bool EditorManager::Close(int index, bool dontsave)
{
    EditorBase* ed = InternalGetEditorBase(index);
    if (ed)
        return Close(ed, dontsave);
    return false;
}

bool EditorManager::Save(const wxString& filename)
{
    EditorBase* ed = IsOpen(filename);
    if (ed)
    {
        if (!ed->Save())
            return false;
        return true;
    }
    return true;
}

bool EditorManager::Save(int index)
{
    EditorBase* ed = InternalGetEditorBase(index);
    if (ed)
    {
        if (!ed->Save())
            return false;
        return true;
    }
    return false;
}

bool EditorManager::SaveActive()
{
    EditorBase* ed = GetActiveEditor();
    if (ed)
    {
        if (!ed->Save())
            return false;
        return true;
    }
    return true;
}

bool EditorManager::SaveAs(int index)
{
    EditorBase* eb = InternalGetEditorBase(index);
    if (!eb)
        return false;
    return eb->SaveAs();
}

bool EditorManager::SaveActiveAs()
{
    EditorBase* eb = GetActiveEditor();
    if (eb)
    {
        return eb->SaveAs();
    }
    return true;
}

bool EditorManager::SaveAll()
{
    for (size_t i = 0; i < m_pNotebook->GetPageCount(); ++i)
    {
        EditorBase* ed = InternalGetEditorBase(i);
        if (ed && ed->GetModified() && !ed->Save())
        {
            wxString msg;
            msg.Printf(_("File %s could not be saved..."), ed->GetFilename().c_str());
            cbMessageBox(msg, _("Error saving file"), wxICON_ERROR);
        }
    }

    return true;
}

void EditorManager::Print(PrintScope ps, PrintColourMode pcm, bool line_numbers)
{
    switch (ps)
    {
    case psAllOpenEditors:
        {
            for (size_t i = 0; i < m_pNotebook->GetPageCount(); ++i)
            {
                cbEditor* ed = InternalGetBuiltinEditor(i);
                if (ed)
                    ed->Print(false, pcm, line_numbers);
            }
            break;
        }
    case psActiveEditor: // fall through
    case psSelection:    // fall through
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
    if (m_isCheckingForExternallyModifiedFiles) // for some reason, a mutex locker does not work???
        return;
    m_isCheckingForExternallyModifiedFiles = true;

    bool reloadAll = false; // flag to stop bugging the user
    wxArrayString failedFiles; // list of files failed to reload
    for (size_t i = 0; i < m_pNotebook->GetPageCount(); ++i)
    {
        cbEditor* ed = InternalGetBuiltinEditor(i);
        bool b_modified = false;

        // no builtin editor or new file not yet saved
        if (!ed)
        {
            EditorBase* eb = InternalGetEditorBase(i);
            if (eb)
            {
                ProjectFile* pf = nullptr;

                wxFileName fn(eb->GetFilename());
                fn.MakeAbsolute();
                Manager::Get()->GetProjectManager()->FindProjectForFile(fn.GetFullPath(), &pf, false, false);

                bool readOnly = eb->IsReadOnly() || (fn.FileExists() && !wxFile::Access(fn.GetFullPath(), wxFile::write));
                MarkReadOnly(i, readOnly);

                if (pf)
                {
                    if (!fn.FileExists()) // file was deleted ?
                    {
                        if (pf->GetFileState() != fvsMissing) // already asked
                        {
                            wxString msg;
                            msg.Printf(_("%s has been deleted, or is no longer available.\n"
                                         "Do you wish to try to save the file to disk?\n"
                                         "If you close it, it will most likely be lost !\n"
                                         "If you cancel this dialog, you have to take care yourself !\n"
                                         "Yes: save the file, No: close it, Cancel: keep your fingers crossed."), eb->GetFilename().c_str());
                            int ret = cbMessageBox(msg, _("File changed!"), wxICON_QUESTION | wxYES_NO | wxCANCEL );
                            switch (ret)
                            {
                                case wxID_YES:
                                {
                                    eb->Save();
                                    break;
                                }
                                case wxID_NO:
                                {
                                    pf->SetFileState(fvsMissing);
                                    eb->Close();
                                    break;
                                }
                                case wxID_CANCEL: // fall through
                                default:
                                    eb->SetModified(true); // some editors might implement it
                                    pf->SetFileState(fvsMissing);
                                    break;
                            }
                        }
                    }
                    else
                        pf->SetFileState(readOnly?fvsReadOnly:fvsNormal);
                }
                continue;
            }
        }
        if (!ed->IsOK())
            continue;

        ProjectFile* pf = ed->GetProjectFile();
        if (pf)
        {
            cbProject* prj = pf->GetParentProject();
            if (prj && !prj->GetCheckForExternallyModifiedFiles())
                continue;
        }

        // File was deleted?
        if (!wxFileExists(ed->GetFilename()))
        {
            if (ed->GetModified()) // Already set the flag
                continue;
            wxString msg;
            msg.Printf(_("%s has been deleted, or is no longer available.\n"
                         "Do you wish to keep the file open?\n"
                         "Yes to keep the file, No to close it."), ed->GetFilename().c_str());
            if (cbMessageBox(msg, _("File changed!"), wxICON_QUESTION | wxYES_NO) == wxID_YES)
                ed->SetModified(true);
            else
            {
                if (pf)
                    pf->SetFileState(fvsMissing);
                ed->Close();
            }
            continue;
        }

        wxFileName fname(ed->GetFilename());
        wxDateTime last = fname.GetModificationTime();

        //File changed from RO -> RW?
        if (ed->GetControl()->GetReadOnly() &&
                wxFile::Access(ed->GetFilename().c_str(), wxFile::write))
        {
            b_modified = false;
            ed->SetReadOnly(false);
            MarkReadOnly(i, false);
            if (pf)
                pf->SetFileState(fvsNormal);
        }
        //File changed from RW -> RO?
        if (!ed->GetControl()->GetReadOnly() &&
                !wxFile::Access(ed->GetFilename().c_str(), wxFile::write))
        {
            b_modified = false;
            ed->SetReadOnly(true);
            MarkReadOnly(i);
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
                dlg.GetSizer()->SetSizeHints(&dlg);
                PlaceWindow(&dlg);

                // Find the window, that actually has the mouse-focus and force a release
                // prevents crash on windows or hang on wxGTK
                wxWindow* win = wxWindow::GetCapture();
                if (win)
                    win->ReleaseMouse();

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

void EditorManager::MarkReadOnly(int page, bool readOnly)
{
    if (page > -1)
    {
        wxBitmap bmp = readOnly?cbLoadBitmap(ConfigManager::GetDataFolder() + _T("/images/") + _T("readonly.png")):wxNullBitmap;
        if (m_pNotebook)
            m_pNotebook->SetPageBitmap(page, bmp);
    }
}



bool EditorManager::IsHeaderSource(const wxFileName& candidateFile, const wxFileName& activeFile, FileType ftActive, bool& isCandidate)
{
    // Verify the base name matches
    if (candidateFile.GetName().CmpNoCase(activeFile.GetName()) == 0)
    {
        // matching case has priority over case-insensitive
        isCandidate = (candidateFile.GetName() != activeFile.GetName());

        // Verify:
        // If looking for a header we have a source OR
        // If looking for a source we have a header
        FileType ftTested = FileTypeOf(candidateFile.GetFullName());
        if (    ((ftActive == ftHeader) && (ftTested == ftSource))
             || ((ftActive == ftSource) && (ftTested == ftHeader)) )
        {
            // Handle the case where two files (in different directories) have the same name:
            // Example: A project file with three files dir1/file.h dir1/file.cpp dir2/file.h
            // If you are in dir2/file.h and you want to swap Code::Blocks will first look if there
            // isn't a file.h in that directory, which is in this case and would then ask the user
            // to create a new file.cpp in dir2
            if (candidateFile.GetPath() != activeFile.GetPath()) // Check if we are not in the same Directory
            {
                wxArrayString fileArray;
                wxDir::GetAllFiles(candidateFile.GetPath(wxPATH_GET_VOLUME), &fileArray, candidateFile.GetName() + _T(".*"), wxDIR_FILES | wxDIR_HIDDEN);
                for (unsigned i=0; i<fileArray.GetCount(); i++)                             // if in this directory there is already
                    if (wxFileName(fileArray[i]).GetFullName() == activeFile.GetFullName()) // a header file (or source file) for our candidate
                        return false;                                                       // file it can't be our candidate file
            }
            if (candidateFile.FileExists())
                return true;
        }
    }
    return false;
}

wxFileName EditorManager::FindHeaderSource(const wxArrayString& candidateFilesArray, const wxFileName& activeFile, bool& isCandidate)
{
    FileType ftActive = FileTypeOf(activeFile.GetFullName());

    // Because ftActive == ftHeader || ftSource, the extension has at least 1 character
    bool extStartsWithCapital = wxIsupper(activeFile.GetExt()[0]);

    wxFileName candidateFile;
    for (unsigned i = 0; i < candidateFilesArray.GetCount(); ++i)
    {
        wxFileName currentCandidateFile(candidateFilesArray[i]);

        if (IsHeaderSource(currentCandidateFile, activeFile, ftActive, isCandidate))
        {
            bool isUpper = wxIsupper(currentCandidateFile.GetExt()[0]);
            if (isUpper == extStartsWithCapital && !isCandidate)
            {
                // we definitely found the header/source we were searching for
                return currentCandidateFile;
            }
            else
            {
                // the header/source has a different capitalization of its extension
                // use this if nothing better is found
                candidateFile = currentCandidateFile;
            }
        }
    }

    isCandidate = true;

    // may be invalid (empty) file name
    return candidateFile;
}

bool EditorManager::OpenContainingFolder()
{
    cbEditor* ed = GetBuiltinEditor(GetActiveEditor());
    if (!ed)
        return false;

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));
#if defined __WXMSW__
    const wxString defCmds = _T("explorer.exe /select,");
#elif defined __WXMAC__
    const wxString defCmds = _T("open -R");
#else
    const wxString defCmds = _T("xdg-open");

    if (!cfg->ReadBool(_T("scanned_open_folder_mime"), false))
    {
        cfg->Write(_T("scanned_open_folder_mime"), true);
        wxString cmd =   Manager::Get()->GetConfigManager(_T("app"))->Read(_T("/console_shell"), DEFAULT_CONSOLE_SHELL)
                       + _T(" 'cat /usr/share/applications/`xdg-mime query default inode/directory` | grep Exec'");
        wxArrayString output, errors;
        wxExecute(cmd, output, errors, wxEXEC_SYNC);
        if (!output.IsEmpty())
        {
            cmd = output[0].AfterFirst('=').BeforeFirst('%').Trim(true).Trim(false);
            if (!cmd.IsEmpty() && cmd != defCmds)
                cfg->Write(_T("open_containing_folder"), cmd);
        }
    }
#endif

    wxString cmds = cfg->Read(_T("open_containing_folder"), defCmds) + _T(" ");
    const wxString& fullPath = ed->GetFilename();
#if defined __WXMSW__ || defined __WXMAC__
    cmds << fullPath;   // Open folder with the file selected
#else
    if (cmds.StartsWith(defCmds))
    {
        // Cannot select the file with "xdg-open", so just extract the folder name
        wxString splitPath;
        wxFileName::SplitPath(fullPath, &splitPath, nullptr, nullptr);
        cmds << splitPath;
    }
    else
        cmds << fullPath; // Open folder with the file selected
#endif

    wxExecute(cmds);
    return true;
}

bool EditorManager::SwapActiveHeaderSource()
{
    cbEditor* ed = GetBuiltinEditor(GetActiveEditor());
    if (!ed)
        return false;

    ProjectManager *pm = Manager::Get()->GetProjectManager();
    if (!pm)
        return false;

    FileType ft = FileTypeOf(ed->GetFilename());
    if (ft != ftHeader && ft != ftSource)
        return false;

    cbProject* project = nullptr;

    // if the file in question belongs to a different open project,
    // then use that project instead.
    // this fixes locating the file's pair in a workspace when the active file
    // does not belong to the active project.
    ProjectFile* opf = ed->GetProjectFile();
    if (opf)
        project = opf->GetParentProject();

    // if we didn't get a valid project, try the active one
    if (!project)
        project = pm->GetActiveProject();

    wxFileName theFile(ed->GetFilename());
    wxFileName candidateFile;
    bool isCandidate;
    wxArrayString fileArray;

    // find all files with the same name as the active file, but with possibly different extension
    // search in the directory of the active file:
    wxDir::GetAllFiles(theFile.GetPath(wxPATH_GET_VOLUME), &fileArray, theFile.GetName() + _T(".*"), wxDIR_FILES | wxDIR_HIDDEN);

    // try to find the header/source in the list
    wxFileName currentCandidateFile = FindHeaderSource(fileArray, theFile, isCandidate);

    if (isCandidate)
    {
        candidateFile = currentCandidateFile;
    }
    else if (currentCandidateFile.IsOk())
    {
        cbEditor* newEd = Open(currentCandidateFile.GetFullPath());
        if (newEd!=nullptr) // we found and were able to open it
            return true; // --> RETURN;
    }

    // try to find the file among the opened files

    // build a list of opened files
    fileArray.Clear();
    for (int i = 0; i < GetEditorsCount(); ++i)
    {
        cbEditor* edit = GetBuiltinEditor(GetEditor(i));
        if (!edit)
            continue;

        ProjectFile* pf = edit->GetProjectFile();
        if (!pf)
            continue;

        fileArray.Add(pf->file.GetFullPath());
    }

    // try to find the header/source in the list
    currentCandidateFile = FindHeaderSource(fileArray, theFile, isCandidate);

    if (!isCandidate && currentCandidateFile.IsOk())
    {
        cbEditor* newEd = Open(currentCandidateFile.GetFullPath());
        if (newEd!=nullptr) // we found and were able to open it
            return true; // --> RETURN;
    }

    if (project)
    {
        // try to find in the project files

        // build a list of project files
        fileArray.Clear();
        for (FilesList::iterator it = project->GetFilesList().begin(); it != project->GetFilesList().end(); ++it)
        {
            ProjectFile* pf = *it;
            if (!pf)
                continue;

            fileArray.Add(pf->file.GetFullPath());
        }

        // try to find the header/source in the list
        currentCandidateFile = FindHeaderSource(fileArray, theFile, isCandidate);

        if (isCandidate && !candidateFile.IsOk())
        {
            candidateFile = currentCandidateFile;
        }
        else if (currentCandidateFile.IsOk())
        {
            cbEditor* newEd = Open(currentCandidateFile.GetFullPath());
            if (newEd!=nullptr) // we found and were able to open it
                return true; // --> RETURN;
        }

        // if not found, build the list of directories for further searching

        // get project's include dirs
        wxArrayString dirs = project->GetIncludeDirs();

        // get targets include dirs
        for (int i = 0; i < project->GetBuildTargetsCount(); ++i)
        {
            ProjectBuildTarget* target = project->GetBuildTarget(i);
            if (target)
            {
                for (unsigned int ti = 0; ti < target->GetIncludeDirs().GetCount(); ++ti)
                {
                    // TODO (Morten#5#): target include dirs might override project include dirs, take append/prepend option into account
                    wxString dir = target->GetIncludeDirs()[ti];
                    if (dirs.Index(dir) == wxNOT_FOUND)
                        dirs.Add(dir);
                }
            }
        }

        // go through the directories and try to find the header/source there
        for (unsigned int i = 0; i < dirs.GetCount(); ++i)
        {
            wxString dir = dirs[i]; // might contain macros -> replace them
            Manager::Get()->GetMacrosManager()->ReplaceMacros(dir);

            wxFileName dname(dir);
            if (!dname.IsAbsolute())
            {
                dname.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_CASE, project->GetBasePath());
    //            Manager::Get()->GetLogManager()->DebugLog(F(_T("Normalizing dir to '%s'."), dname.GetFullPath().c_str()));
            }

            fileArray.Clear();
            // find all files inside the directory with the same name as the active file, but with possibly different extension
            wxDir::GetAllFiles(dname.GetPath(), &fileArray, theFile.GetName() + _T(".*"), wxDIR_FILES | wxDIR_HIDDEN);
            // try to find the header/source in the list
            currentCandidateFile = FindHeaderSource(fileArray, theFile, isCandidate);

            if (isCandidate)
            {
                candidateFile = currentCandidateFile;
            }
            else if (currentCandidateFile.IsOk())
            {
                cbEditor* newEd = Open(currentCandidateFile.GetFullPath());
                if (newEd!=nullptr) // we found and were able to open it
                    return true; // --> RETURN;
            }
        }
    }

    // candidateFile is header/source whose extension does not match the capitalization of the active file
    // - open it if nothing better is found
    if (candidateFile.IsOk())
    {
        cbEditor* newEd = Open(candidateFile.GetFullPath());
        if (newEd!=nullptr) // we found and were able to open it
            return true; // --> RETURN;
    }

    // nothing else worked, try to find source-source pairs of auto-generated files
    if (opf)
    {
        ProjectFile* candidatePF = opf->AutoGeneratedBy();
        if (!candidatePF && !opf->generatedFiles.empty())
            candidatePF = opf->generatedFiles.front();
        if (candidatePF)
        {
            cbEditor* newEd = Open(candidatePF->file.GetFullPath());
            if (newEd!=nullptr) // we found and were able to open it
                return true; // --> RETURN;
        }
    }

    // We couldn't find the file, maybe it does not exist. Ask the user if we
    // should create it:
    if (cbMessageBox(_("The file seems not to exist. Do you want to create it?"),
                _("Error"), wxICON_QUESTION | wxYES_NO) == wxID_YES)
    {
        project = Manager::Get()->GetProjectManager()->GetActiveProject();
        if (project)
            wxSetWorkingDirectory(project->GetBasePath());

        // Create a suggestion for the new file name:
        if      (ft == ftHeader)
            theFile.SetExt(FileFilters::CPP_EXT);
        else if (ft == ftSource)
            theFile.SetExt(FileFilters::H_EXT);
        // else? Well, if the filename is not changed we could possibly
        // overwrite an existing file with our suggestion.

        cbEditor* newEd = New(theFile.GetFullPath());
        if (project)
        {
            if (cbMessageBox(_("Do you want to add this new file in the active project?"),
                        _("Add file to project"),
                        wxYES_NO | wxICON_QUESTION) == wxID_YES)
            {
                wxArrayInt targets;
                if (Manager::Get()->GetProjectManager()->AddFileToProject(newEd->GetFilename(), project, targets) != 0)
                {
                    ProjectFile* pf = project->GetFileByFilename(newEd->GetFilename(), false);
                    newEd->SetProjectFile(pf);
                    Manager::Get()->GetProjectManager()->GetUI().RebuildTree();
                }
            }
        }

        // verify that the open files are still in sync
        // the new file might have overwritten an existing one)
        Manager::Get()->GetEditorManager()->CheckForExternallyModifiedFiles();
    }

    return false;
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
    else if (id >= idNBSwitchFile1 && id <= idNBSwitchFileMax)
    {
        eb = GetEditor(id - idNBSwitchFile1);
        if (eb)
            SetActiveEditor(eb);
    }
}

void EditorManager::OnPageChanged(wxAuiNotebookEvent& event)
{
    EditorBase* eb = static_cast<EditorBase*>(m_pNotebook->GetPage(event.GetSelection()));
    EditorBase* eb_old = nullptr;
    if (event.GetOldSelection()!=-1)
        eb_old = static_cast<EditorBase*>(m_pNotebook->GetPage(event.GetOldSelection()));

    CodeBlocksEvent evt(cbEVT_EDITOR_SWITCHED, -1, nullptr, eb, nullptr, eb_old);
    Manager::Get()->GetPluginManager()->NotifyPlugins(evt);

    CodeBlocksEvent evt2(cbEVT_EDITOR_ACTIVATED, -1, nullptr, eb);
    Manager::Get()->GetPluginManager()->NotifyPlugins(evt2);

    if (Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/tabs_stacked_based_switching")))
    {
        wxWindow*        wnd;
        cbNotebookStack* body;
        cbNotebookStack* tmp;
        wnd = m_pNotebook->GetPage(event.GetSelection());
        for (body = m_pNotebookStackHead; body->next != nullptr; body = body->next)
        {
            if (wnd == body->next->window)
            {
                if (m_pNotebookStackTail == body->next)
                    m_pNotebookStackTail = body;
                tmp = body->next;
                body->next = tmp->next;
                tmp->next = m_pNotebookStackHead->next;
                m_pNotebookStackHead->next = tmp;
                break;
            }
        }
        if (   (m_pNotebookStackHead->next == nullptr)
            || (wnd != m_pNotebookStackHead->next->window) )
        {
            body = new cbNotebookStack(wnd);
            body->next = m_pNotebookStackHead->next;
            m_pNotebookStackHead->next = body;
            ++m_nNotebookStackSize;
        }
    }

    // focus editor on next update event
    m_pData->m_SetFocusFlag = true;

    event.Skip(); // allow others to process it too
}

void EditorManager::OnPageChanging(wxAuiNotebookEvent& event)
{
    int old_sel = event.GetOldSelection();
    if (old_sel != -1 && static_cast<size_t>(old_sel) < m_pNotebook->GetPageCount())
    {
        EditorBase* eb = static_cast<EditorBase*>(m_pNotebook->GetPage(old_sel));
        CodeBlocksEvent evt(cbEVT_EDITOR_DEACTIVATED, -1, nullptr, eb);
        Manager::Get()->GetPluginManager()->NotifyPlugins(evt);
    }
    event.Skip(); // allow others to process it too
}

void EditorManager::OnPageClose(wxAuiNotebookEvent& event)
{
    int sel = event.GetSelection();
    bool doClose = false;
    EditorBase* eb = nullptr;
    if (sel != -1)
    {
        // veto it in any case, so we can handle the page delete or remove ourselves
        event.Veto();
        eb = static_cast<EditorBase*>(m_pNotebook->GetPage(sel));
        if (QueryClose(eb))
        {
            doClose = true;
            if (m_pNotebook->GetPageCount()<=1)
            {
                CodeBlocksEvent evt(cbEVT_EDITOR_SWITCHED, -1, nullptr, nullptr, nullptr, eb);
                Manager::Get()->GetPluginManager()->NotifyPlugins(evt);
            }
        }
    }

    if (Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/tabs_stacked_based_switching")))
    {
        wxWindow* wnd;
        cbNotebookStack* body;
        cbNotebookStack* tmp;
        wnd = m_pNotebook->GetPage(event.GetSelection());
        for (body = m_pNotebookStackHead; body->next != nullptr; body = body->next)
        {
            if (wnd == body->next->window)
            {
                tmp = body->next;
                body->next = tmp->next;
                delete tmp;
                --m_nNotebookStackSize;
                break;
            }
        }
    }

    if (doClose && eb != nullptr)
        Close(eb);
    else
        event.Skip(); // allow others to process it too
}

void EditorManager::OnPageContextMenu(wxAuiNotebookEvent& event)
{
    if (event.GetSelection() == -1)
        return;

    if (wxGetKeyState(WXK_CONTROL) && GetEditorsCount() > 1)
    {
        wxMenu* pop = new wxMenu;
        EditorBase* current = GetActiveEditor();
        for (int i = 0; i < EditorMaxSwitchTo && i < GetEditorsCount(); ++i)
        {
            EditorBase* other = GetEditor(i);
            if (!other)
                continue;
            const wxString name = (other->GetModified() ? wxT("*") : wxEmptyString) + other->GetShortName();
            if (other == current)
            {
                pop->AppendCheckItem(wxID_ANY, name); // do nothing if the current tab is selected
                pop->FindItemByPosition(pop->GetMenuItemCount() - 1)->Check(); // and mark it as active
            }
            else
                pop->Append(idNBSwitchFile1 + i, name);
        }
        m_pNotebook->PopupMenu(pop);
        delete pop;
        return;
    }

    // select the notebook that sends the event, because the context menu-entries act on the actual selected tab
    m_pNotebook->SetSelection(event.GetSelection());
    wxMenu* pop = new wxMenu;
    pop->Append(idNBTabClose, _("Close"));
    if (GetEditorsCount() > 1)
    {
        pop->Append(idNBTabCloseAll, _("Close all"));
        pop->Append(idNBTabCloseAllOthers, _("Close all others"));
    }

    int any_modified = 0;
    for (int i = 0; i<GetEditorsCount(); ++i)
    {
        EditorBase* ed = GetEditor(i);
        if (ed && ed->GetModified())
        {
            if (++any_modified > 1)
                break; // more than one editor is modified -> enable "Save all"
        }
    }
    if (any_modified > 0)
    {
        pop->AppendSeparator();
        if (GetEditor(event.GetSelection())->GetModified())
            pop->Append(idNBTabSave, _("Save"));
        if (any_modified > 1 || !GetEditor(event.GetSelection())->GetModified())
            pop->Append(idNBTabSaveAll, _("Save all"));
    }

    pop->AppendSeparator();
    cbEditor* ed = GetBuiltinEditor(event.GetSelection());
    if (ed)
    {
        pop->Append(idNBSwapHeaderSource, _("Swap header/source"));
        pop->Append(idNBTabOpenContainingFolder, _("Open containing folder"));
        pop->Append(idNBProperties, _("Properties..."));
        pop->AppendSeparator();
    }

    if (Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/editor_tabs_bottom"), false))
        pop->Append(idNBTabTop, _("Tabs at top"));
    else
        pop->Append(idNBTabBottom, _("Tabs at bottom"));

    if (ed)
    {
        wxMenu* splitMenu = new wxMenu;
        if (ed->GetSplitType() != cbEditor::stHorizontal)
            splitMenu->Append(idNBTabSplitHorz, _("Horizontally"));
        if (ed->GetSplitType() != cbEditor::stVertical)
            splitMenu->Append(idNBTabSplitVert, _("Vertically"));
        if (ed->GetSplitType() != cbEditor::stNoSplit)
        {
            splitMenu->AppendSeparator();
            splitMenu->Append(idNBTabUnsplit, _("Unsplit"));
        }
        pop->Append(-1, _("Split view"), splitMenu);

        if (Manager::Get()->GetProjectManager()->GetActiveProject()) // project must be open
        {
            pop->AppendSeparator();

            ProjectFile *projectFile = ed->GetProjectFile();
            if (projectFile)
            {
                pop->Append(idNBRemoveFileFromProject, _("Remove file from project"));
                pop->Append(idNBShowFileInTree, _("Show file in the project tree"));
            }
            else
                pop->Append(idNBAddFileToProject, _("Add file to active project"));
        }
    }

    // allow plugins to use this menu
    Manager::Get()->GetPluginManager()->AskPluginsForModuleMenu(mtEditorTab, pop);

    m_pNotebook->PopupMenu(pop);
    delete pop;
}

void EditorManager::OnClose(cb_unused wxCommandEvent& event)
{
    Manager::Get()->GetEditorManager()->Close(GetActiveEditor());
}

void EditorManager::OnCloseAll(cb_unused wxCommandEvent& event)
{
    Manager::Get()->GetEditorManager()->CloseAll();
}

void EditorManager::OnCloseAllOthers(cb_unused wxCommandEvent& event)
{
    Manager::Get()->GetEditorManager()->CloseAllExcept(GetActiveEditor());
}

void EditorManager::OnSave(cb_unused wxCommandEvent& event)
{
    Manager::Get()->GetEditorManager()->Save(m_pNotebook->GetSelection());
}

void EditorManager::OnSaveAll(cb_unused wxCommandEvent& event)
{
    Manager::Get()->GetEditorManager()->SaveAll();
}

void EditorManager::OnSwapHeaderSource(cb_unused wxCommandEvent& event)
{
    Manager::Get()->GetEditorManager()->SwapActiveHeaderSource();
}

void EditorManager::OnOpenContainingFolder(cb_unused wxCommandEvent& event)
{
    Manager::Get()->GetEditorManager()->OpenContainingFolder();
}

void EditorManager::OnTabPosition(wxCommandEvent& event)
{
    long style = m_pNotebook->GetWindowStyleFlag();
    style &= ~wxAUI_NB_BOTTOM;

    if (event.GetId() == idNBTabBottom)
        style |= wxAUI_NB_BOTTOM;
    m_pNotebook->SetWindowStyleFlag(style);
    m_pNotebook->Refresh();

    // (style & wxAUI_NB_BOTTOM) saves info only about the the tabs position
    Manager::Get()->GetConfigManager(_T("app"))->Write(_T("/environment/editor_tabs_bottom"),       (bool)(style & wxAUI_NB_BOTTOM));
}

void EditorManager::OnProperties(cb_unused wxCommandEvent& event)
{
    cbEditor* ed = GetBuiltinActiveEditor();
    ProjectFile* pf = nullptr;
    if (ed)
        pf = ed->GetProjectFile();
    if (pf)
        pf->ShowOptions(Manager::Get()->GetAppWindow());
    else
    {
        ProjectFileOptionsDlg dlg(Manager::Get()->GetAppWindow(), GetActiveEditor()->GetFilename());
        PlaceWindow(&dlg);
        dlg.ShowModal();
    }
}

void EditorManager::OnAddFileToProject(cb_unused wxCommandEvent& event)
{
    cbProject *project = Manager::Get()->GetProjectManager()->GetActiveProject();
    wxString fname = GetBuiltinActiveEditor()->GetFilename();

    wxArrayInt targets;
    if (Manager::Get()->GetProjectManager()->AddFileToProject(fname, project, targets) != 0)
    {
        ProjectFile* pf = project->GetFileByFilename(fname, false);
        GetBuiltinActiveEditor()->SetProjectFile(pf);
        Manager::Get()->GetProjectManager()->GetUI().RebuildTree();
    }
}

void EditorManager::OnRemoveFileFromProject(cb_unused wxCommandEvent& event)
{
    ProjectFile* pf = GetBuiltinActiveEditor()->GetProjectFile();
    if (pf) // should be in any case, otherwise something went wrong between popup menu creation and here
    {
        cbProject *project = pf->GetParentProject();
        Manager::Get()->GetProjectManager()->RemoveFileFromProject(pf, project);
        Manager::Get()->GetProjectManager()->GetUI().RebuildTree();
    }
}

void EditorManager::OnShowFileInTree(cb_unused wxCommandEvent& event)
{
    ProjectFile* pf = GetBuiltinActiveEditor()->GetProjectFile();
    if (pf)
    {
        cbProjectManagerUI &ui = Manager::Get()->GetProjectManager()->GetUI();
        ui.SwitchToProjectsPage();
        ui.ShowFileInTree(*pf);
    }
}

void EditorManager::OnAppDoneStartup(wxCommandEvent& event)
{
    event.Skip(); // allow others to process it too
}

void EditorManager::OnAppStartShutdown(wxCommandEvent& event)
{
    event.Skip(); // allow others to process it too
}

void EditorManager::OnCheckForModifiedFiles(cb_unused wxCommandEvent& event)
{
    CheckForExternallyModifiedFiles();
}

void EditorManager::HideNotebook()
{
    if (m_pNotebook)
        m_pNotebook->Hide();
}

void EditorManager::ShowNotebook()
{
    if (m_pNotebook)
        m_pNotebook->Show();
}

void EditorManager::OnUpdateUI(wxUpdateUIEvent& event)
{
    /* Don't process UpdateUI event when the app is closing.
     * It may crash C::B */
    if (!Manager::Get()->IsAppShuttingDown() && m_pData->m_SetFocusFlag)
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

void EditorManager::CollectDefines(CodeBlocksEvent& event)
{
    cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
    if (   !prj
        || !Manager::Get()->GetConfigManager(wxT("editor"))->ReadBool(wxT("/track_preprocessor"),  true)
        || !Manager::Get()->GetConfigManager(wxT("editor"))->ReadBool(wxT("/collect_prj_defines"), true) )
    {
        event.Skip();
        return;
    }

    wxArrayString compilerFlags = prj->GetCompilerOptions();
    ProjectBuildTarget* tgt = prj->GetBuildTarget(prj->GetActiveBuildTarget());
    FilesList* lst;
    wxString id;
    if (tgt)
    {
        AppendArray(tgt->GetCompilerOptions(), compilerFlags);
        lst = &tgt->GetFilesList();
        id  = tgt->GetCompilerID();
    }
    else
    {
        lst = &prj->GetFilesList();
        id  = prj->GetCompilerID();
    }

    Compiler* comp = CompilerFactory::GetCompiler(id); // get global flags
    if (comp)
        AppendArray(comp->GetCompilerOptions(), compilerFlags);

    wxArrayString defines;
    for (size_t i = 0; i < compilerFlags.GetCount(); ++i)
    {
        if (   compilerFlags[i].StartsWith(wxT("-D"))
            || compilerFlags[i].StartsWith(wxT("/D")) )
        {
            defines.Add(compilerFlags[i].Mid(2));
        }
        else if (compilerFlags[i].Find(wxT("`")) != wxNOT_FOUND)
        {
            wxString str = compilerFlags[i];
            ExpandBackticks(str);
            str.Replace(wxT("`"), wxT(" ")); // remove any leftover backticks to prevent an infinite loop
            AppendArray(GetArrayFromString(str, wxT(" ")), compilerFlags);
        }
        else if (   compilerFlags[i] == wxT("-ansi")
                 || compilerFlags[i] == wxT("-std=c90")
                 || compilerFlags[i] == wxT("-std=c++98"))
        {
            defines.Add(wxT("__STRICT_ANSI__"));
        }
    }

    defines.Add(wxT("__cplusplus"));
    for (FilesList::iterator it = lst->begin(); it != lst->end(); ++it)
    {
        if ((*it)->relativeFilename.EndsWith(wxT(".c")))
        {
            defines.RemoveAt(defines.GetCount() - 1); // do not define '__cplusplus' if even a single C file is found
            break;
        }
    }

    if (id.Find(wxT("gcc")) != wxNOT_FOUND)
    {
        defines.Add(wxT("__GNUC__"));
        defines.Add(wxT("__GNUG__"));
    }
    else if (id.Find(wxT("msvc")) != wxNOT_FOUND)
    {
        defines.Add(wxT("_MSC_VER"));
        defines.Add(wxT("__VISUALC__"));
    }

    if (Manager::Get()->GetConfigManager(wxT("editor"))->ReadBool(wxT("/platform_defines"), false))
    {
        if (platform::windows)
        {
            defines.Add(wxT("_WIN32"));
            defines.Add(wxT("__WIN32"));
            defines.Add(wxT("__WIN32__"));
            defines.Add(wxT("WIN32"));
            defines.Add(wxT("__WINNT"));
            defines.Add(wxT("__WINNT__"));
            defines.Add(wxT("WINNT"));
            defines.Add(wxT("__WXMSW__"));
            defines.Add(wxT("__WINDOWS__"));
            if (platform::bits == 64)
            {
                defines.Add(wxT("_WIN64"));
                defines.Add(wxT("__WIN64__"));
            }
        }
        else if (platform::macosx)
        {
            defines.Add(wxT("__WXMAC__"));
            defines.Add(wxT("__WXOSX__"));
            defines.Add(wxT("__WXCOCOA__"));
            defines.Add(wxT("__WXOSX_MAC__"));
            defines.Add(wxT("__APPLE__"));
        }
        else if (platform::linux)
        {
            defines.Add(wxT("LINUX"));
            defines.Add(wxT("linux"));
            defines.Add(wxT("__linux"));
            defines.Add(wxT("__linux__"));
        }
        else if (platform::freebsd)
        {
            defines.Add(wxT("FREEBSD"));
            defines.Add(wxT("__FREEBSD__"));
        }
        else if (platform::netbsd)
        {
            defines.Add(wxT("NETBSD"));
            defines.Add(wxT("__NETBSD__"));
        }
        else if (platform::openbsd)
        {
            defines.Add(wxT("OPENBSD"));
            defines.Add(wxT("__OPENBSD__"));
        }
        else if (platform::darwin)
        {
            defines.Add(wxT("DARWIN"));
            defines.Add(wxT("__APPLE__"));
        }
        else if (platform::solaris)
        {
            defines.Add(wxT("sun"));
            defines.Add(wxT("__sun"));
            defines.Add(wxT("__SUN__"));
            defines.Add(wxT("__SUNOS__"));
            defines.Add(wxT("__SOLARIS__"));
        }
        if (platform::unix)
        {
            defines.Add(wxT("unix"));
            defines.Add(wxT("__unix"));
            defines.Add(wxT("__unix__"));
            defines.Add(wxT("__UNIX__"));
        }
        if (platform::gtk)
            defines.Add(wxT("__WXGTK__"));
        if (platform::bits == 32)
        {
            defines.Add(wxT("i386"));
            defines.Add(wxT("__i386"));
            defines.Add(wxT("__i386__"));
            defines.Add(wxT("__i386__"));
            defines.Add(wxT("_X86_"));
            defines.Add(wxT("__INTEL__"));
        }
        else if (platform::bits == 64)
        {
            defines.Add(wxT("__amd64"));
            defines.Add(wxT("__amd64__"));
            defines.Add(wxT("__x86_64"));
            defines.Add(wxT("__x86_64__"));
            defines.Add(wxT("__IA64__"));
        }
    }

    const wxString keywords = GetStringFromArray(MakeUniqueArray(defines, true), wxT(" "), false);
    const HighlightLanguage hlCpp = m_Theme->GetHighlightLanguage(wxT("C/C++"));
    if (m_Theme->GetKeywords(hlCpp, 4) == keywords)
        return; // no change

    m_Theme->SetKeywords(hlCpp, 4, keywords);
    const wxString key = wxT("/colour_sets/") + m_Theme->GetName() + wxT("/cc/");
    Manager::Get()->GetConfigManager(wxT("editor"))->Write(key + wxT("editor/keywords/set4"), keywords);
    Manager::Get()->GetConfigManager(wxT("editor"))->Write(key + wxT("name"), wxT("C/C++"));

    // update open editors
    for (int index = 0; index < GetEditorsCount(); ++index)
    {
        cbEditor* ed = GetBuiltinEditor(index);
        if ( ed && (ed->GetLanguage() == hlCpp) )
        {
            cbStyledTextCtrl* stc = ed->GetControl();
            stc->SetKeyWords(4, keywords);
        }
    }
    event.Skip();
}

void EditorManager::SetZoom(int zoom)
{
    m_Zoom = zoom;
}

int EditorManager::GetZoom() const
{
    return m_Zoom;
}

void EditorManager::OnAppActivated(CodeBlocksEvent& event)
{
    event.Skip();
    if (!platform::gtk)
        return;
    wxTextDataObject data;
    bool gotData = false;
    wxTheClipboard->UsePrimarySelection(true);
    if (wxTheClipboard->Open())
    {
        gotData = wxTheClipboard->GetData(data);
        wxTheClipboard->Close();
    }
    wxTheClipboard->UsePrimarySelection(false);
    if (gotData && !data.GetText().IsEmpty())
        m_pData->m_SelectionClipboard = data.GetText();
}

wxString EditorManager::GetSelectionClipboard()
{
    return m_pData->m_SelectionClipboard;
}

void EditorManager::SetSelectionClipboard(const wxString& data)
{
    m_pData->m_SelectionClipboard = data;
}

/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

//warning: ...trunk/src/include/sdk_precomp.h.gch: not used because `EXPORT_LIB' not defined|
//#ifdef CB_PRECOMP
//    #include "sdk_precomp.h"
//#endif
#undef CB_PRECOMP
#ifndef CB_PRECOMP
    #include <wx/dir.h>
    #include <wx/file.h>
    #include <wx/imaglist.h>
    #include <wx/listctrl.h>
    #include <wx/menu.h>
    #include <wx/notebook.h>
    #include <wx/regex.h>
    #include <wx/splitter.h>

    #include "seditormanager.h" // class's header file
    #include "configmanager.h"
    #include "infowindow.h"
    #include "logmanager.h"
    #include "projectmanager.h"
    #include "projectfile.h"
    #include "pluginmanager.h"
    #include "manager.h"
    #include "filemanager.h"
    #include "sdk_events.h"
    #include "projectbuildtarget.h"
    #include "cbproject.h"
    #include "scbeditor.h"
    #include "globals.h"
    #include "sdk_events.h"
#endif
#include "cbstyledtextctrl.h"

#include <wx/bmpbuttn.h>
#include <wx/progdlg.h>
#include <wx/fontutil.h>
#include <wx/tokenzr.h>
#include "seditorcolourset.h"
//-#include "editorconfigurationdlg.h" //(pecan 2008/5/06)
#include "encodingdetector.h"
#include "findreplacedlg.h"
#include "confirmreplacedlg.h"
#include "filefilters.h"
#include "searchresultslog.h"
#include "projectfileoptionsdlg.h"
#include "filegroupsandmasks.h"

#include "cbauibook.h"
#include "snippetsconfig.h"
#include "seditormanager.h" // class's header file
#include "scbeditor.h"
#include "sprintdlg.h"
#include "editproperties.h"
#if defined(LOGGING)
    #include "version.h"
#endif

//template<> EditorManager* Mgr<EditorManager>::instance = 0;
//template<> bool  Mgr<EditorManager>::isShutdown = false;

int ID_NBSEditorManager = wxNewId();
int ID_SEditorManager = wxNewId();
int idSEditorManagerCheckFiles = wxNewId();

// static
bool SEditorManager::s_CanShutdown = true;

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
    bool autoWrapSearch;
    bool findUsesSelectedText;
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
static const int idNBProperties = wxNewId();
static const int idNB = wxNewId();

/** *******************************************************
  * struct EditorManagerInternalData                      *
  * This is the private data holder for the EditorManager *
  * All data not relevant to other classes should go here *
  ********************************************************* */

struct EditorManagerInternalData
{
    /* Methods */

    EditorManagerInternalData(SEditorManager* owner)
            : m_pOwner(owner)
    {}

    /* Static data */

    SEditorManager* m_pOwner;
    bool m_SetFocusFlag;
};

// *********** End of EditorManagerInternalData **********


BEGIN_EVENT_TABLE(SEditorManager, wxEvtHandler)
    EVT_APP_STARTUP_DONE(SEditorManager::OnAppDoneStartup)
    EVT_APP_START_SHUTDOWN(SEditorManager::OnAppStartShutdown)
    EVT_AUINOTEBOOK_PAGE_CHANGED(ID_NBSEditorManager, SEditorManager::OnPageChanged)
    EVT_AUINOTEBOOK_PAGE_CHANGING(ID_NBSEditorManager, SEditorManager::OnPageChanging)
    EVT_AUINOTEBOOK_PAGE_CLOSE(ID_NBSEditorManager, SEditorManager::OnPageClose)
    EVT_AUINOTEBOOK_TAB_RIGHT_UP(ID_NBSEditorManager, SEditorManager::OnTabRightUp)
    EVT_MENU(idNBTabSplitHorz, SEditorManager::OnGenericContextMenuHandler)
    EVT_MENU(idNBTabSplitVert, SEditorManager::OnGenericContextMenuHandler)
    EVT_MENU(idNBTabUnsplit, SEditorManager::OnGenericContextMenuHandler)
    EVT_MENU(idNBTabTop, SEditorManager::OnTabPosition)
    EVT_MENU(idNBTabBottom, SEditorManager::OnTabPosition)
    EVT_MENU(idNBTabClose, SEditorManager::OnClose)
    EVT_MENU(idNBTabCloseAll, SEditorManager::OnCloseAll)
    EVT_MENU(idNBTabCloseAllOthers, SEditorManager::OnCloseAllOthers)
    EVT_MENU(idNBTabSave, SEditorManager::OnSave)
    EVT_MENU(idNBTabSaveAll, SEditorManager::OnSaveAll)
    EVT_MENU(idNBSwapHeaderSource, SEditorManager::OnSwapHeaderSource)
    EVT_MENU(idNBProperties, SEditorManager::OnProperties)
    EVT_MENU(idSEditorManagerCheckFiles, SEditorManager::OnCheckForModifiedFiles)
END_EVENT_TABLE()

// class constructor
// ----------------------------------------------------------------------------
SEditorManager::SEditorManager(wxWindow* parent)
// ----------------------------------------------------------------------------
        : m_pNotebook(0L),
        m_LastFindReplaceData(0L),
        m_pSearchLog(0),
        m_SearchLogIndex(-1),
        m_SashPosition(150), // no longer used
        m_isCheckingForExternallyModifiedFiles(false)
{
    InitSEditorManager(parent);
}
// ----------------------------------------------------------------------------
void SEditorManager::InitSEditorManager(wxWindow* parent)
// ----------------------------------------------------------------------------
{
    m_pParent = parent;
    GetConfig()->RegisterEditorManager( (wxFrame*)parent, this );
    m_pData = new EditorManagerInternalData(this);

    m_pNotebook = new cbAuiNotebook( parent, ID_NBSEditorManager, wxDefaultPosition, wxDefaultSize, wxNO_FULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN);
    m_pNotebook->SetWindowStyleFlag(Manager::Get()->GetConfigManager(_T("app"))->ReadInt(_T("/environment/editor_tabs_style"), wxAUI_NB_DEFAULT_STYLE | wxAUI_NB_MIDDLE_CLICK_CLOSE));

    Manager::Get()->GetLogManager()->DebugLog(_T("Initialize EditColourSet ....."));

    m_Theme = 0;
    //-m_Theme = new EditorColourSet(Manager::Get()->GetConfigManager(_T("editor"))->Read(_T("/colour_sets/active_colour_set"), COLORSET_DEFAULT));
    m_Theme = new SEditorColourSet(Manager::Get()->GetConfigManager(_T("editor"))->Read(_T("/colour_sets/active_colour_set"), COLORSET_DEFAULT));
    Manager::Get()->GetLogManager()->DebugLog(_T("Initialize EditColourSet: done."));

    //-Manager::Get()->GetAppWindow()->PushEventHandler(this);
    parent->PushEventHandler(this); //-causes crash- on Destroy()

    //--CreateSearchLog(); Crashes eventually with Assert failure in sdk_events.cpp
    // line 37 "LogManager::invalid_log" after starting the 28th editor.

    LoadAutoComplete();
    m_zoom = Manager::Get()->GetConfigManager(_T("editor"))->ReadInt(_T("/zoom"));
}

// class destructor
// ----------------------------------------------------------------------------
SEditorManager::~SEditorManager()
// ----------------------------------------------------------------------------
{
    SaveAutoComplete();


    if (m_pSearchLog) //(pecan 2008/5/04)
    {
        CodeBlocksLogEvent evt(cbEVT_REMOVE_LOG_WINDOW, m_pSearchLog);
        Manager::Get()->ProcessEvent(evt);
        m_pSearchLog = 0;
    }

    delete m_Theme;
    delete m_LastFindReplaceData;
    delete m_pData;
    Manager::Get()->GetConfigManager(_T("editor"))->Write(_T("/zoom"), m_zoom);

    //(pecan 2008/5/04)
    m_pParent->RemoveEventHandler(this); //Do this or crash
    // De-register this EditorManager
    GetConfig()->RemoveEditorManager((wxFrame*)m_pParent );

} // end of destructor

void SEditorManager::CreateMenu(wxMenuBar* menuBar)
{
}

void SEditorManager::ReleaseMenu(wxMenuBar* menuBar)
{
}

void SEditorManager::Configure()
{
    //(pecan 2008/5/06) This belongs to the sdk, not this separate editor
////    // editor lexers loading takes some time; better reflect this with a hourglass
////    wxBeginBusyCursor();
////
////    EditorConfigurationDlg dlg(Manager::Get()->GetAppWindow());
////    PlaceWindow(&dlg);
////
////    // done, restore pointer
////    wxEndBusyCursor();
////
////    if (dlg.ShowModal() == wxID_OK)
////    {
////        // tell all open editors to re-create their styles
////        for (int i = 0; i < m_pNotebook->GetPageCount(); ++i)
////        {
////            ScbEditor* ed = (ScbEditor*)InternalGetBuiltinEditor(i);
////            if (ed)
////            {
////                bool saveSuccess = ed->SaveFoldState(); //First Save the old fold levels
////                ed->SetEditorStyle();
////                if(saveSuccess)
////                {
////                    ed->FixFoldState(); //Compare old fold levels with new and change the bugs
////                }
////            }
////        }
////    }
} // end of Configure
// ----------------------------------------------------------------------------
void SEditorManager::CreateSearchLog()
// ----------------------------------------------------------------------------
{
    if (Manager::IsBatchBuild())
        return;

    wxArrayInt widths;
    wxArrayString titles;
    titles.Add(_("File"));
    titles.Add(_("Line"));
    titles.Add(_("Text"));
    widths.Add(128);
    widths.Add(48);
    widths.Add(640);

    wxString prefix = ConfigManager::GetDataFolder() + _T("/images/16x16/");
    wxBitmap * bmp = new wxBitmap(cbLoadBitmap(prefix + _T("filefind.png"), wxBITMAP_TYPE_PNG));

    m_pSearchLog = new SearchResultsLog(titles, widths);
    CodeBlocksLogEvent evt(cbEVT_ADD_LOG_WINDOW, m_pSearchLog, _("Search results"), bmp);
    Manager::Get()->ProcessEvent(evt);
}

void SEditorManager::LogSearch(const wxString& file, int line, const wxString& lineText)
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

    m_pSearchLog->Append(values, line == -1 ? Logger::caption : Logger::info);
}

void SEditorManager::LoadAutoComplete()
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

    // date and time macros
    // these are auto-added if they 're found to be missing
    const wxString timeAndDate[9][2] =
    {
        { _T("tday"), _T("$TDAY") },
        { _T("tdayu"), _T("$TDAY_UTC") },
        { _T("today"), _T("$TODAY") },
        { _T("todayu"), _T("$TODAY_UTC") },
        { _T("now"), _T("$NOW") },
        { _T("nowl"), _T("$NOW_L") },
        { _T("nowu"), _T("$NOW_UTC") },
        { _T("nowlu"), _T("$NOW_L_UTC") },
        { _T("wdu"), _T("$WEEKDAY_UTC") },
    };
    for (int i = 0; i < 9; ++i)
    {
        if (m_AutoCompleteMap.find(timeAndDate[i][0]) == m_AutoCompleteMap.end())
            m_AutoCompleteMap[timeAndDate[i][0]] = timeAndDate[i][1];
    }
}

void SEditorManager::SaveAutoComplete()
{
    Manager::Get()->GetConfigManager(_T("editor"))->DeleteSubPath(_T("/auto_complete"));
    SAutoCompleteMap::iterator it;
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

int SEditorManager::GetEditorsCount()
{
    return m_pNotebook->GetPageCount();
}

SEditorBase* SEditorManager::InternalGetEditorBase(int page)
{
    //-5995 SEditorBase* eb = static_cast<SEditorBase*>(m_pNotebook->GetPage(page));
    //-5995 return eb;
    if (page < (int)m_pNotebook->GetPageCount())
    {
        return static_cast<SEditorBase*>(m_pNotebook->GetPage(page));
    }
    return 0;
}

ScbEditor* SEditorManager::InternalGetBuiltinEditor(int page)
{
    SEditorBase* eb = InternalGetEditorBase(page);
    if (eb && eb->IsBuiltinEditor())
        return (ScbEditor*)eb;
    return 0;
}

ScbEditor* SEditorManager::GetBuiltinEditor(SEditorBase* eb)
{
    return eb && eb->IsBuiltinEditor() ? (ScbEditor*)eb : 0;
}

SEditorBase* SEditorManager::IsOpen(const wxString& filename)
{
    wxString uFilename = UnixFilename(realpath(filename));
    for (size_t i = 0; i < m_pNotebook->GetPageCount(); ++i)
    {
        SEditorBase* eb = InternalGetEditorBase(i);
        if (!eb)
            continue;
        wxString fname = eb->GetFilename();

        // MSW must use case-insensitive comparison
        if (fname.IsSameAs(uFilename, platform::windows == false) || fname.IsSameAs(g_EditorModified + uFilename, platform::windows == false))
            return eb;
    }

    return NULL;
}

SEditorBase* SEditorManager::GetEditor(int index)
{
    return InternalGetEditorBase(index);
}

void SEditorManager::SetColourSet(SEditorColourSet* theme)
{
    if (m_Theme)
        delete m_Theme;

    // copy locally
    //-m_Theme = new EditorColourSet(*theme);
    m_Theme = new SEditorColourSet(*theme);

    for (size_t i = 0; i < m_pNotebook->GetPageCount(); ++i)
    {
        ScbEditor* ed = InternalGetBuiltinEditor(i);
        if (ed)
            ed->SetColourSet(m_Theme);
    }
}

ScbEditor* SEditorManager::Open(const wxString& filename, int pos, ProjectFile* data)
{
    LoaderBase* fileLdr = Manager::Get()->GetFileManager()->Load(filename);
    if (!fileLdr)
        return 0;

    return Open(fileLdr, filename, pos, data);
}

ScbEditor* SEditorManager::Open(LoaderBase* fileLdr, const wxString& filename, int pos,ProjectFile* data)
{
    bool can_updateui = !GetActiveEditor() || !Manager::Get()->GetProjectManager()->IsLoading();
    wxFileName fn(realpath(filename));
    NormalizePath(fn, wxEmptyString);
    wxString fname = UnixFilename(fn.GetFullPath());
    //  Manager::Get()->GetLogManager()->DebugLog("Trying to open '%s'", fname.c_str());
    if (!wxFileExists(fname))
        return NULL;
    //  Manager::Get()->GetLogManager()->DebugLog("File exists '%s'", fname.c_str());

    // disallow application shutdown while opening files
    // WARNING: remember to set it to true, when exiting this function!!!
    s_CanShutdown = false;

    SEditorBase* eb = IsOpen(fname);
    ScbEditor* ed = 0;
    if (eb)
    {
        if (eb->IsBuiltinEditor())
            ed = (ScbEditor*)eb;
        else
            return 0; // is open but not a builtin editor
    }

    if (!ed)
    {
        ed = new ScbEditor(m_pNotebook, fileLdr, fname, m_Theme);
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
            Manager::Get()->GetLogManager()->DebugLog(_T("project data set for ") + data->file.GetFullPath());
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
                    Manager::Get()->GetLogManager()->DebugLog(_T("found ") + pf->file.GetFullPath());
                    data = pf;
                    break;
                }
            }
        }
        if(data)
            ed->SetProjectFile(data,true);
    }

    // we 're done
    s_CanShutdown = true;

    return ed;
}

SEditorBase* SEditorManager::GetActiveEditor()
{
    //-5995 return InternalGetEditorBase(m_pNotebook->GetSelection());
    if (m_pNotebook->GetPageCount() > 0)
    {
        return InternalGetEditorBase(m_pNotebook->GetSelection());
    }
    return 0;
}

void SEditorManager::ActivateNext()
{
    m_pNotebook->AdvanceSelection(true);
}

void SEditorManager::ActivatePrevious()
{
    m_pNotebook->AdvanceSelection(false);
}

void SEditorManager::SetActiveEditor(SEditorBase* ed)
{
    if (!ed)
        return;
    if (ed->IsBuiltinEditor())
        static_cast<ScbEditor*>(ed)->GetControl()->SetFocus();
    int page = FindPageFromEditor(ed);
    if (page != -1)
    {
        m_pNotebook->SetSelection(page);
    }
}

ScbEditor* SEditorManager::New(const wxString& newFileName)
{
//    wxString old_title = Manager::Get()->GetAppWindow()->GetTitle(); // Fix for Bug #1389450
    // create a dummy file
    if (!newFileName.IsEmpty() && !wxFileExists(newFileName) && wxDirExists(wxPathOnly(newFileName)))
    {
        wxFile f(newFileName, wxFile::write);
        if (!f.IsOpened())
            return 0;
    }
    ScbEditor* ed = new ScbEditor(m_pNotebook, newFileName);
//    if ((newFileName.IsEmpty() && !ed->SaveAs()) || !ed->Save())
//    {
//        //DeletePage(ed->GetPageIndex());
//        ed->Destroy();
//        Manager::Get()->GetAppWindow()->SetTitle(old_title); // Though I can't reproduce the bug, this does no harm
//        return 0L;
//    }

    // add default text
    wxString key;
    key.Printf(_T("/default_code/set%d"), (int)FileTypeOf(ed->GetFilename()));
    wxString code = Manager::Get()->GetConfigManager(_T("editor"))->Read(key, wxEmptyString);
    ed->GetControl()->SetText(code);

    ed->SetColourSet(m_Theme);
    AddEditorBase(ed);

    ed->Show(true);
    SetActiveEditor(ed);
    CodeBlocksEvent evt(cbEVT_EDITOR_OPEN, -1, 0, (EditorBase*)ed);
    ////(pecan 2008/4/22) Dont notify plugings from this non-sdk editor
    //-Manager::Get()->GetPluginManager()->NotifyPlugins(evt);
    return ed;
}

void SEditorManager::AddCustomEditor(SEditorBase* eb)
{
    AddEditorBase(eb);
}

void SEditorManager::RemoveCustomEditor(SEditorBase* eb)
{
    RemoveEditorBase(eb, false);
}

void SEditorManager::AddEditorBase(SEditorBase* eb)
{
    int page = FindPageFromEditor(eb);
    if (page == -1)
    {
        //        LOGSTREAM << wxString::Format(_T("AddEditorBase(): ed=%p, title=%s\n"), eb, eb ? eb->GetTitle().c_str() : _T(""));
        m_pNotebook->AddPage(eb, eb->GetTitle(), true);
    }
}

void SEditorManager::RemoveEditorBase(SEditorBase* eb, bool deleteObject)
{
    //    LOGSTREAM << wxString::Format(_T("RemoveEditorBase(): ed=%p, title=%s\n"), eb, eb ? eb->GetFilename().c_str() : _T(""));
    int page = FindPageFromEditor(eb);
   if (page != -1 && !Manager::IsAppShuttingDown())
         m_pNotebook->RemovePage(page);

    //    if (deleteObject)
    //        eb->Destroy();
}

bool SEditorManager::UpdateSnippetFiles(cbProject* project)
{
    for (size_t i = 0; i < m_pNotebook->GetPageCount(); ++i)
    {
        ScbEditor* ed = InternalGetBuiltinEditor(i);
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

bool SEditorManager::CloseAll(bool dontsave)
{
    //return CloseAllExcept(0L,dontsave);
    return CloseAllExcept(GetEditor(_("Start here")), dontsave);
}

bool SEditorManager::QueryCloseAll()
{
    for (int i = m_pNotebook->GetPageCount() - 1; i >= 0; --i)
    {
        SEditorBase* eb = InternalGetEditorBase(i);
        if(eb && !QueryClose(eb))
            return false; // aborted
    }
    return true;
}

bool SEditorManager::CloseAllExcept(SEditorBase* editor,bool dontsave)
{
    if(!dontsave)
    {
        for (size_t i = 0; i < m_pNotebook->GetPageCount(); ++i)
        {
            SEditorBase* eb = InternalGetEditorBase(i);
            if(eb && eb != editor && !QueryClose(eb))
                return false; // aborted
        }
    }

    m_pNotebook->Freeze();
    int count = m_pNotebook->GetPageCount();
    for (int i = m_pNotebook->GetPageCount() - 1; i >= 0; --i)
    {
        SEditorBase* eb = InternalGetEditorBase(i);
        if (eb && eb != editor && Close(eb, true))
            --count;
    }
    m_pNotebook->Thaw();
    return count == (editor ? 1 : 0);
}

bool SEditorManager::CloseActive(bool dontsave)
{
    return Close(GetActiveEditor(),dontsave);
}

bool SEditorManager::QueryClose(SEditorBase *ed)
{
    if(!ed)
        return true;
    if (ed->GetModified())
    {
        // TODO (mandrav#1#): Move this in SEditorBase
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

int SEditorManager::FindPageFromEditor(SEditorBase* eb)
{
    for (size_t i = 0; i < m_pNotebook->GetPageCount(); ++i)
    {
        if (m_pNotebook->GetPage(i) == eb)
            return i;
    }
    return -1;
}

bool SEditorManager::Close(const wxString& filename,bool dontsave)
{
    return Close(IsOpen(filename),dontsave);
}

bool SEditorManager::Close(SEditorBase* editor,bool dontsave)
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
            m_pNotebook->DeletePage(idx);
        }
    }
    return true;
}

bool SEditorManager::Close(int index,bool dontsave)
{
    SEditorBase* ed = InternalGetEditorBase(index);
    if (ed)
        return Close(ed,dontsave);
    return false;
}

bool SEditorManager::Save(const wxString& filename)
{
    //    ScbEditor* ed = GetBuiltinEditor(IsOpen(filename));
    SEditorBase* ed = IsOpen(filename);
    if (ed)
    {
        wxString oldname = ed->GetFilename();
        if (!ed->Save())
            return false;
        return true;
    }
    return true;
}

bool SEditorManager::Save(int index)
{
    SEditorBase* ed = InternalGetEditorBase(index);
    if (ed)
    {
        wxString oldname = ed->GetFilename();
        if (!ed->Save())
            return false;
        return true;
    }
    return false;
}

bool SEditorManager::SaveActive()
{
    SEditorBase* ed = GetActiveEditor();
    if (ed)
    {
        wxString oldname = ed->GetFilename();
        if (!ed->Save())
            return false;
        return true;
    }
    return true;
}

bool SEditorManager::SaveAs(int index)
{
    ScbEditor* ed = GetBuiltinEditor(GetEditor(index));
    if(!ed)
        return false;
    return ed->SaveAs();
}

bool SEditorManager::SaveActiveAs()
{
    ScbEditor* ed = GetBuiltinEditor(GetActiveEditor());
    if (ed)
    {
        return ed->SaveAs();
    }
    return true;
}

bool SEditorManager::SaveAll()
{
    for (size_t i = 0; i < m_pNotebook->GetPageCount(); ++i)
    {
        SEditorBase* ed = InternalGetEditorBase(i);
        if (ed && ed->GetModified() && !ed->Save())
        {
            wxString msg;
            msg.Printf(_("File %s could not be saved..."), ed->GetFilename().c_str());
            cbMessageBox(msg, _("Error saving file"), wxICON_ERROR);
        }
    }

    return true;
}
// ----------------------------------------------------------------------------
void SEditorManager::FilePrint(wxWindow* parent) //ported from main.cpp
// ----------------------------------------------------------------------------
{
    SPrintDialog dlg( parent, this);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
        Print(dlg.GetPrintScope(), dlg.GetPrintColourMode(), dlg.GetPrintLineNumbers());
}
// ----------------------------------------------------------------------------
void SEditorManager::Print(PrintScope ps, PrintColourMode pcm, bool line_numbers)
{
    switch (ps)
    {
    case psAllOpenEditors:
        {
            for (size_t i = 0; i < m_pNotebook->GetPageCount(); ++i)
            {
                ScbEditor* ed = InternalGetBuiltinEditor(i);
                if (ed)
                    ed->Print(false, pcm, line_numbers);
            }
            break;
        }
    default:
        {
            ScbEditor* ed = GetBuiltinEditor(GetActiveEditor());
            if (ed)
                ed->Print(ps == psSelection, pcm, line_numbers);
            break;
        }
    }
}

void SEditorManager::CheckForExternallyModifiedFiles()
{
    if(m_isCheckingForExternallyModifiedFiles) // for some reason, a mutex locker does not work???
        return;
    m_isCheckingForExternallyModifiedFiles = true;

    bool reloadAll = false; // flag to stop bugging the user
    wxArrayString failedFiles; // list of files failed to reload
    for (size_t i = 0; i < m_pNotebook->GetPageCount(); ++i)
    {
        ScbEditor* ed = InternalGetBuiltinEditor(i);
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
            if (cbMessageBox(msg, _("File changed!"), wxICON_QUESTION | wxYES_NO) == wxID_YES)
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
} // end of CheckForExternallyModifiedFiles

//bool SEditorManager::SwapActiveHeaderSource()
//{
//    ScbEditor* ed = GetBuiltinEditor(GetActiveEditor());
//    if (!ed)
//        return false;
//
//    FileType ft = FileTypeOf(ed->GetFilename());
//    if (ft != ftHeader && ft != ftSource)
//        return 0L;
//
//    // create a list of search dirs
//    wxArrayString dirs;
//
//    cbProject* project = 0;
//
//    // if the file in question belongs to a different open project,
//    // then use that project instead.
//    // this fixes locating the file's pair in a workspace when the active file
//    // does not belong to the active project.
//    ProjectFile* opf = ed->GetProjectFile();
//    if (opf)
//        project = opf->GetParentProject();
//
//    // if we didn't get a valid project, try the active one
//    if (!project)
//        project = Manager::Get()->GetProjectManager()->GetActiveProject();
//
//    // get project's include dirs
//    if (project)
//    {
//        dirs = project->GetIncludeDirs();
//
//        // first add all paths that contain project files
//        for (int i = 0; i < project->GetFilesCount(); ++i)
//        {
//            ProjectFile* pf = project->GetFile(i);
//            if (pf)
//            {
//                wxString dir = pf->file.GetPath(wxPATH_GET_VOLUME);
//                if (dirs.Index(dir) == wxNOT_FOUND)
//                    dirs.Add(dir);
//            }
//        }
//
//        // get targets include dirs
//        for (int i = 0; i < project->GetBuildTargetsCount(); ++i)
//        {
//            ProjectBuildTarget* target = project->GetBuildTarget(i);
//            if (target)
//            {
//                for (unsigned int ti = 0; ti < target->GetIncludeDirs().GetCount(); ++ti)
//                {
//                    wxString dir = target->GetIncludeDirs()[ti];
//                    if (dirs.Index(dir) == wxNOT_FOUND)
//                        dirs.Add(dir);
//                }
//            }
//        }
//    }
//
//    wxFileName fname;
//    wxFileName fn(ed->GetFilename());
//    dirs.Insert(fn.GetPath(wxPATH_GET_VOLUME), 0); // add file's dir
//
//    for (unsigned int i = 0; i < dirs.GetCount(); ++i)
//    {
//        fname.Assign(dirs[i] + wxFileName::GetPathSeparator() + fn.GetFullName());
//        if (!fname.IsAbsolute() && project)
//        {
//            fname.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_CASE, project->GetBasePath());
//        }
//        //Manager::Get()->GetLogManager()->DebugLog("Looking for '%s'", fname.GetFullPath().c_str());
//        if (ft == ftHeader)
//        {
//            fname.SetExt(FileFilters::C_EXT);
//            if (fname.FileExists())
//                break;
//            fname.SetExt(FileFilters::CC_EXT);
//            if (fname.FileExists())
//                break;
//            fname.SetExt(FileFilters::CPP_EXT);
//            if (fname.FileExists())
//                break;
//            fname.SetExt(FileFilters::CXX_EXT);
//            if (fname.FileExists())
//                break;
//            fname.SetExt(FileFilters::INL_EXT);
//            if (fname.FileExists())
//                break;
//
//// DrewBoo: See if the project manager knows of other extensions
////   (This could also outright replace the hard-coded extensions above)
//// TODO (Morten#5#): Do what DrewBoo said: Try removing the above code
//// TODO (Morten#3#): This code should actually be a method of filegrous and masks or alike. So we collect all extension specific things in one place. As of now this would break ABI compatibilty with 08.02 so this should happen later.
//            ProjectManager *pm = Manager::Get()->GetProjectManager();
//            if ( !pm )
//                break;
//
//            const FilesGroupsAndMasks* fg = pm->GetFilesGroupsAndMasks();
//            if ( !fg )
//                break;
//
//            for ( unsigned int i = 0; i != fg->GetGroupsCount(); ++i )
//            {
//                if ( fg->GetGroupName(i) == _("Sources") )
//                {
//                    wxStringTokenizer tkz( fg->GetFileMasks(i), _T(";") );
//                    while ( tkz.HasMoreTokens() )
//                    {
//                        wxString token = tkz.GetNextToken();
//                        wxString ext;
//                        if ( token.StartsWith( _("*."), &ext ) )
//                        {
//                            fname.SetExt(ext);
//                            if (fname.FileExists())
//                                break;
//                        }
//                    }
//                    break;
//                }
//            }
//        }
//        else if (ft == ftSource)
//        {
//            fname.SetExt(FileFilters::H_EXT);
//            if (fname.FileExists())
//                break;
//            fname.SetExt(FileFilters::HH_EXT);
//            if (fname.FileExists())
//                break;
//            fname.SetExt(FileFilters::HPP_EXT);
//            if (fname.FileExists())
//                break;
//            fname.SetExt(FileFilters::HXX_EXT);
//            if (fname.FileExists())
//                break;
//
//// DrewBoo: See if the project manager knows of other extensions
////   (This could also outright replace the hard-coded extensions above)
//// TODO (Morten#5#): Do what DrewBoo said: Try removing the above code
//// TODO (Morten#3#): This code should actually be a method of filegrous and masks or alike. So we collect all extension specific things in one place. As of now this would break ABI compatibilty with 08.02 so this should happen later.
//            ProjectManager *pm = Manager::Get()->GetProjectManager();
//            if ( !pm )
//                break;
//
//            const FilesGroupsAndMasks* fg = pm->GetFilesGroupsAndMasks();
//            if ( !fg )
//                break;
//
//            for ( unsigned int i = 0; i != fg->GetGroupsCount(); ++i )
//            {
//                if ( fg->GetGroupName(i) == _("Headers") )
//                {
//                    wxStringTokenizer tkz( fg->GetFileMasks(i), _T(";") );
//                    while ( tkz.HasMoreTokens() )
//                    {
//                        wxString token = tkz.GetNextToken();
//                        wxString ext;
//                        if ( token.StartsWith( _("*."), &ext ) )
//                        {
//                            fname.SetExt(ext);
//                            if (fname.FileExists())
//                                break;
//                        }
//                    }
//                    break;
//                }
//            }
//        }
//    }
//
//    if (fname.FileExists())
//    {
//        //Manager::Get()->GetLogManager()->DebugLog("ed=%s, pair=%s", ed->GetFilename().c_str(), pair.c_str());
//        ScbEditor* newEd = Open(fname.GetFullPath());
//        //if (newEd)
//        //    newEd->SetProjectFile(ed->GetProjectFile());
//        return newEd;
//    }
//
//    // We couldn't find the file, maybe it does not exist. Ask the user if we
//    // should create it:
//    if (cbMessageBox(_("The file does not exist. Do you want to create it?"),
//                _("Error"), wxICON_QUESTION | wxYES_NO) == wxID_YES)
//    {
//        cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
//        if (project)
//            wxSetWorkingDirectory(project->GetBasePath());
//
//        // Create a suggestion for the new file name:
//        if (ft == ftHeader)
//            fn.SetExt(FileFilters::CPP_EXT);
//        else if (ft == ftSource)
//            fn.SetExt(FileFilters::H_EXT);
//        // else? Well, if the filename is not changed we could possibly
//        // overwrite an existing file with our suggestion.
//
//        ScbEditor* newEd = New(fn.GetFullPath());
//        if (cbMessageBox(_("Do you want to add this new file in the active project?"),
//                    _("Add file to project"),
//                    wxYES_NO | wxICON_QUESTION) == wxID_YES)
//        {
//            wxArrayInt targets;
//            if (Manager::Get()->GetProjectManager()->AddFileToProject(newEd->GetFilename(), project, targets) != 0)
//            {
//                ProjectFile* pf = project->GetFileByFilename(newEd->GetFilename(), false);
//                newEd->SetProjectFile(pf);
//                Manager::Get()->GetProjectManager()->RebuildTree();
//            }
//        }
//        // verify that the open files are still in sync
//        // the new file might have overwritten an existing one)
//        //-GetConfig()->GetEditorManager()->CheckForExternallyModifiedFiles();
//        CheckForExternallyModifiedFiles();
//    }
//    return 0L;
//}
bool SEditorManager::IsHeaderSource(const wxFileName& testedFileName, const wxFileName& activeFileName, FileType ftActive)
{
    if (testedFileName.GetName() == activeFileName.GetName())
    {
        FileType ftTested = FileTypeOf(testedFileName.GetFullName());
        if (    ((ftActive == ftHeader) && (ftTested == ftSource))
             || ((ftActive == ftSource) && (ftTested == ftHeader)) )
        {
            if (testedFileName.FileExists())
            {
                return true;
            }
        }
    }
    return false;
}

wxFileName SEditorManager::FindHeaderSource(const wxArrayString& candidateFilesArray, const wxFileName& activeFile, bool& isCandidate)
{
    FileType ftActive = FileTypeOf(activeFile.GetFullName());

    // Because ftActive == ftHeader || ftSource, the extension has at least 1 character
    bool extStartsWithCapital = wxIsupper(activeFile.GetExt()[0]);

    wxFileName candidateFile;
    for (unsigned i = 0; i < candidateFilesArray.GetCount(); ++i)
    {
        wxFileName currentCandidateFile(candidateFilesArray[i]);

        if (IsHeaderSource(currentCandidateFile, activeFile, ftActive))
        {
            bool isUpper = wxIsupper(currentCandidateFile.GetExt()[0]);
            if (isUpper == extStartsWithCapital)
            {
                // we definitely found the header/source we were searching for
                isCandidate = false;
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

bool SEditorManager::SwapActiveHeaderSource()
{
    ScbEditor* ed = GetBuiltinEditor(GetActiveEditor());
    if (!ed)
        return false;

    ProjectManager *pm = Manager::Get()->GetProjectManager();
    if (!pm)
        return false;

    FileType ft = FileTypeOf(ed->GetFilename());
    if (ft != ftHeader && ft != ftSource)
        return false;

    cbProject* project = 0;

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
        ScbEditor* newEd = Open(currentCandidateFile.GetFullPath());
        if (newEd!=0L) // we found and were able to open it
            return true; // --> RETURN;
    }

    // try to find the file among the opened files

    // build a list of opened files
    fileArray.Clear();
    for (int i = 0; i < GetEditorsCount(); ++i)
    {
        ScbEditor* edit = GetBuiltinEditor(GetEditor(i));
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
        ScbEditor* newEd = Open(currentCandidateFile.GetFullPath());
        if (newEd!=0L) // we found and were able to open it
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
            ScbEditor* newEd = Open(currentCandidateFile.GetFullPath());
            if (newEd!=0L) // we found and were able to open it
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
            //FIXME: MacrosManager is undefined
            // Manager::Get()->GetMacrosManager()->ReplaceMacros(dir);

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
                ScbEditor* newEd = Open(currentCandidateFile.GetFullPath());
                if (newEd!=0L) // we found and were able to open it
                    return true; // --> RETURN;
            }
        }
    }

    // candidateFile is header/source whose extension does not match the capitalization of the active file
    // - open it if nothing better is found
    if (candidateFile.IsOk())
    {
        ScbEditor* newEd = Open(candidateFile.GetFullPath());
        if (newEd!=0L) // we found and were able to open it
            return true; // --> RETURN;
    }

    // We couldn't find the file, maybe it does not exist. Ask the user if we
    // should create it:
    if (cbMessageBox(_("The file seems not to exist. Do you want to create it?"),
                _("Error"), wxICON_QUESTION | wxYES_NO) == wxID_YES)
    {
        cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
        if (project)
            wxSetWorkingDirectory(project->GetBasePath());

        // Create a suggestion for the new file name:
        if      (ft == ftHeader)
            theFile.SetExt(FileFilters::CPP_EXT);
        else if (ft == ftSource)
            theFile.SetExt(FileFilters::H_EXT);
        // else? Well, if the filename is not changed we could possibly
        // overwrite an existing file with our suggestion.

        ScbEditor* newEd = New(theFile.GetFullPath());
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
        //-Manager::Get()->GetEditorManager()->CheckForExternallyModifiedFiles();
        CheckForExternallyModifiedFiles();
    }

    return false;
}

int SEditorManager::ShowFindDialog(bool replace, bool explicitly_find_in_files)
{
    wxString phraseAtCursor;
    bool hasSelection = false;
    cbStyledTextCtrl* control = 0;

    ScbEditor* ed = GetBuiltinEditor(GetActiveEditor());
    if (ed)
    {
        control = ed->GetControl();

        hasSelection = control->GetSelectionStart() != control->GetSelectionEnd();
        int wordStart = control->WordStartPosition(control->GetCurrentPos(), true);
        int wordEnd = control->WordEndPosition(control->GetCurrentPos(), true);
        wxString wordAtCursor = control->GetTextRange(wordStart, wordEnd);
        phraseAtCursor = control->GetSelectedText();
        // if selected text is part of a single line, don't suggest "search in selection"
        if (control->LineFromPosition(control->GetSelectionStart())
            == control->LineFromPosition(control->GetSelectionEnd()))
            hasSelection = false;

        if ( phraseAtCursor.IsEmpty())
            phraseAtCursor = wordAtCursor;

        int selstartline = control->LineFromPosition(control->GetSelectionStart());
        int selendline   = control->LineFromPosition(control->GetSelectionEnd());
        // the selection of several lines is not proposed as search pattern
        if ( selstartline != selendline )
            phraseAtCursor = wxEmptyString;

    }

    FindReplaceBase* dlg = new FindReplaceDlg(Manager::Get()->GetAppWindow(), phraseAtCursor, hasSelection,
                                              !replace, !ed, explicitly_find_in_files);

    PlaceWindow(dlg);
    // Move dlg into the parents frame space //(pecan 2008/7/21)
    wxPoint mousePosn = ::wxGetMousePosition();
    wxWindow* parentWin = wxFindWindowAtPoint(mousePosn);
    if (parentWin)
        GetConfig()->CenterChildOnParent( dlg, parentWin);
    else
        dlg->Move(mousePosn.x, mousePosn.y);

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
    if(!m_LastFindReplaceData->findInFiles)
    {
        //AutoWrapSearch does not exist in FindInFiles dialog
        m_LastFindReplaceData->autoWrapSearch = dlg->GetAutoWrapSearch();

        //FindUsesSelectedText does not exist in Replace dialogs
        if (!replace)
            m_LastFindReplaceData->findUsesSelectedText = dlg->GetFindUsesSelectedText();
    }
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

void SEditorManager::CalculateFindReplaceStartEnd(cbStyledTextCtrl* control, cbFindReplaceData* data, bool replace)
{
    if (!control || !data)
        return;

    if (!data->findInFiles)   // Find in current Editor
    {
        int ssta = control->GetSelectionStart();
        int send = control->GetSelectionEnd();
        int cpos = control->GetCurrentPos();
        int clen = control->GetLength();

        // when the user initially had a selection, but then changed the scope
        // to entire scope, the values of ssta and send will have a bad influence in
        // the following calculations, therefor check for the scenario
        // and set the ssta en send to cpos (in the case there would be no selection
        // that's the value they have [no selection : ssta=send=cpos])
        // only do this when it's a new search (when the search is continued (F3/Shift-F3)
        // there can be a selection, the last found match)
        if(data->scope== 0 && data->NewSearch && (ssta != cpos || send != cpos))
        {
            ssta = cpos;
            send = cpos;
        }



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

        // selected text, if user has deslected since last, then change scope
        if (data->scope == 1 &&
            control->GetSelectionStart()==control->GetSelectionEnd())
                data->scope = 0;

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

int SEditorManager::Replace(cbStyledTextCtrl* control, cbFindReplaceData* data)
{
    if (!control || !data)
        return -1;

    if (control->GetReadOnly())
    {
        cbMessageBox(_("This file is read-only.\nReplacing in a read-only file is not possible."),
                     _("Warning"), wxICON_EXCLAMATION);
        return -1;
    }

    bool AdvRegex=false;
    int replacecount=0;
    int foundcount=0;
    int flags = 0;
    CalculateFindReplaceStartEnd(control, data);

    if (data->matchWord)
        flags |= wxSCI_FIND_WHOLEWORD;
    if (data->startWord)
        flags |= wxSCI_FIND_WORDSTART;
    if (data->matchCase)
        flags |= wxSCI_FIND_MATCHCASE;
    if (data->regEx)
    {
        flags |= wxSCI_FIND_REGEXP;
        if (Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/use_posix_style_regexes"), false))
            flags |= wxSCI_FIND_POSIX;
        #ifdef wxHAS_REGEX_ADVANCED
        AdvRegex=Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/use_advanced_regexes"), false);
        #endif
    }

    wxRegEx re;
    #ifdef wxHAS_REGEX_ADVANCED
    if(AdvRegex)
    {
        if(data->matchCase)
            re.Compile(data->findText,wxRE_ADVANCED|wxRE_NEWLINE);
        else
            re.Compile(data->findText,wxRE_ADVANCED|wxRE_NEWLINE|wxRE_ICASE);
    }
    #endif

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
        if(!AdvRegex)
            pos = control->FindText(data->start, data->end, data->findText, flags, &lengthFound);
        else
        {
            wxString text=control->GetTextRange(data->start, data->end);
            if(re.Matches(text))
            {
                size_t start,len;
                re.GetMatch(&start,&len,0);
                pos=start+data->start;
                lengthFound=len;
                if(start==0&&len==0) //For searches for "^" or "$" (and null returning variants on this) need to make sure we have forward progress and not simply matching on a previous BOL/EOL find
                {
                    text=text.Mid(1);
                    if(re.Matches(text))
                    {
                        size_t start,len;
                        re.GetMatch(&start,&len,0);
                        pos=start+data->start+1;
                        lengthFound=len;
                    } else
                        pos=-1;
                }
            } else
                pos=-1;
        }
        if (pos != -1 && data->start!=data->end)
        {
            control->GotoPos(pos);
            control->EnsureVisible(control->LineFromPosition(pos));
            control->SetSelectionVoid(pos, pos + lengthFound);
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

                bool auto_wrap_around = data->autoWrapSearch;
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

        foundcount++;

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
                replacecount++;
                if (data->regEx)
                {
                    // set target same as selection
                    control->SetTargetStart(control->GetSelectionStart());
                    control->SetTargetEnd(control->GetSelectionEnd());
                    if(AdvRegex)
                    {
                        wxString text=control->GetSelectedText();
                        re.Replace(&text,data->replaceText,1);
                        lengthReplace=text.Len();
                        control->ReplaceSelection(text);
                    } else
                    {
                        // replace with regEx support
                        lengthReplace = control->ReplaceTargetRE(data->replaceText);
                    }
                    // reset target
                    control->SetTargetStart(0);
                    control->SetTargetEnd(0);
                }
                else
                    control->ReplaceSelection(data->replaceText);
                if (data->directionDown)
                {
                    data->start += lengthReplace;
                }
                // adjust end pos by adding the length difference between find and replace strings
                int diff = lengthReplace - lengthFound;
                if (data->directionDown)
                {
                    data->end += diff;
                }
                else
                {
                    if(data->end < diff)
                    {
                       data->end = 0;
                    }
                    else
                    {
                       data->end -= diff;
                    }
                }
            }
            else
                if (data->directionDown)
                    data->start += lengthFound;
                else
                    data->start -= lengthFound;

        }
    }
    control->EndUndoAction();
    wxString msg;
    if (foundcount == 0)
        msg = _T("No matches found for \"") + data->findText + _T("\"");
    else if (replacecount == 0 && foundcount == 1)
        msg = _T("One match found but not replaced");
    else
        msg.Printf(_("Replaced %i of %i matches"), replacecount, foundcount);
    cbMessageBox(msg, _("Result"), wxICON_INFORMATION);
    control->SetSCIFocus(true);

    return pos;
}

int SEditorManager::ReplaceInFiles(cbFindReplaceData* data)
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
        for (FilesList::iterator it = prj->GetFilesList().begin(); it != prj->GetFilesList().end(); ++it)
        {
            ProjectFile* pf = *it;
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
        for (size_t i = 0; i < m_pNotebook->GetPageCount(); ++i)
        {
            ScbEditor* ed = InternalGetBuiltinEditor(i);
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
                    for (FilesList::iterator it = pProject->GetFilesList().begin(); it != pProject->GetFilesList().end(); ++it)
                    {
                        ProjectFile* pf = *it;
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

    bool AdvRegex=false;
    int flags = 0;
    if (data->matchWord)
        flags |= wxSCI_FIND_WHOLEWORD;
    if (data->startWord)
        flags |= wxSCI_FIND_WORDSTART;
    if (data->matchCase)
        flags |= wxSCI_FIND_MATCHCASE;
    if (data->regEx)
    {
        flags |= wxSCI_FIND_REGEXP;
        if (Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/use_posix_style_regexes"), false))
            flags |= wxSCI_FIND_POSIX;
        #ifdef wxHAS_REGEX_ADVANCED
        AdvRegex=Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/use_advanced_regexes"), false);
        #endif
    }

    wxRegEx re;
    #ifdef wxHAS_REGEX_ADVANCED
    if(AdvRegex)
    {
        if(data->matchCase)
            re.Compile(data->findText,wxRE_ADVANCED|wxRE_NEWLINE);
        else
            re.Compile(data->findText,wxRE_ADVANCED|wxRE_NEWLINE|wxRE_ICASE);
    }
    #endif


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

    int read_only_files_skipped = 0;
    for (int i = 0; i<filesCount && !stop; ++i)
    {
        ScbEditor *ed = NULL;
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
        SEditorBase *eb = IsOpen(filesList[i]);
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

        if (control->GetReadOnly())     //	Rev 5076 - mortenmacfly
        {
            read_only_files_skipped++;
            continue;
        }

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
            if(!AdvRegex)
                pos = control->FindText(data->start, data->end, data->findText, flags, &lengthFound);
            else
            {
                wxString text=control->GetTextRange(data->start, data->end);
                if(re.Matches(text))
                {
                    size_t start,len;
                    re.GetMatch(&start,&len,0);
                    pos=start+data->start;
                    lengthFound=len;
                    if(start==0&&len==0) //For searches for "^" or "$" (and null returning variants on this) need to make sure we have forward progress and not simply matching on a previous BOL/EOL find
                    {
                        text=text.Mid(1);
                        if(re.Matches(text))
                        {
                            size_t start2,len2;
                            re.GetMatch(&start2,&len2,0);
                            pos=start2+data->start+1;
                            lengthFound=len2;
                        } else
                            pos=-1;
                    }
                } else
                    pos=-1;
            }

            if (pos == -1 || data->start==data->end)
                break;

            if (confirm)
            {
                control->GotoPos(pos);
                control->EnsureVisible(control->LineFromPosition(pos));
            }
            control->SetSelectionVoid(pos, pos + lengthFound);
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
            }// if

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
                        if(AdvRegex)
                        {
                            wxString text=control->GetSelectedText();
                            re.Replace(&text,data->replaceText,1);
                            lengthReplace=text.Len();
                            control->ReplaceSelection(text);
                        } else
                        {
                            // replace with regEx support
                            lengthReplace = control->ReplaceTargetRE(data->replaceText);
                        }
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
            }// if
        }// while

        control->EndUndoAction(); // undo

        //If i opened the file and no replacement was made,
        //close the editor
        if (!ed->GetModified() && fileWasNotOpen)
            Close(ed, true);
    }// for

    if (read_only_files_skipped)
    {
        wxString msg;
        msg.Printf(_("Skipped %d read-only file(s)."), read_only_files_skipped);
        InfoWindow::Display(_("Warning"), msg);
    }

    // if we showed the progress, the app window is frozen; unfreeze it
    if (progress)
        Manager::Get()->GetAppWindow()->Thaw();

    delete progress;
    return pos;
}

int SEditorManager::Find(cbStyledTextCtrl* control, cbFindReplaceData* data)
{
    if (!control || !data)
        return -1;

    bool AdvRegex=false;
    int flags = 0;
    CalculateFindReplaceStartEnd(control, data);

    if (data->matchWord)
        flags |= wxSCI_FIND_WHOLEWORD;
    if (data->startWord)
        flags |= wxSCI_FIND_WORDSTART;
    if (data->matchCase)
        flags |= wxSCI_FIND_MATCHCASE;
    if (data->regEx)
    {
        flags |= wxSCI_FIND_REGEXP;
        if (Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/use_posix_style_regexes"), false))
            flags |= wxSCI_FIND_POSIX;
        #ifdef wxHAS_REGEX_ADVANCED
        AdvRegex=Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/use_advanced_regexes"), false);
        #endif
    }

    wxRegEx re;
    #ifdef wxHAS_REGEX_ADVANCED
    if(AdvRegex)
    {
        if(data->matchCase)
            re.Compile(data->findText,wxRE_ADVANCED|wxRE_NEWLINE);
        else
            re.Compile(data->findText,wxRE_ADVANCED|wxRE_NEWLINE|wxRE_ICASE);
    }
    #endif

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
        if(!AdvRegex)
            pos = control->FindText(data->start, data->end, data->findText, flags, &lengthFound);
        else
        {
            wxString text=control->GetTextRange(data->start, data->end);
            if(re.Matches(text))
            {
                size_t start,len;
                re.GetMatch(&start,&len,0);
                pos=start+data->start;
                lengthFound=len;
                if(start==0&&len==0) //For searches for "^" or "$" (and null returning variants on this) need to make sure we have forward progress and not simply matching on a previous BOL/EOL find
                {
                    text=text.Mid(1);
                    if(re.Matches(text))
                    {
                        size_t start2,len2;
                        re.GetMatch(&start2,&len2,0);
                        pos=start2+data->start+1;
                        lengthFound=len2;
                    } else
                        pos=-1;
                }
            } else
                pos=-1;
        }
        if (pos != -1 && data->start!=data->end)
        {
            int line = control->LineFromPosition(pos);
            int onScreen = control->LinesOnScreen() >> 1;
            int l1 = line - onScreen;
            int l2 = line + onScreen;
            for(int l=l1; l<=l2;l+=2)       // unfold visible lines on screen
                control->EnsureVisible(l);
            control->GotoLine(l1);          // center selection on screen
            control->GotoLine(l2);
            control->GotoLine(line);
            control->SetSelectionVoid(pos, pos + lengthFound);
            //            Manager::Get()->GetLogManager()->DebugLog("pos=%d, selLen=%d, length=%d", pos, data->end - data->start, lengthFound);
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

                bool auto_wrap_around = data->autoWrapSearch;
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
                control->SetSCIFocus(true);
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

int SEditorManager::FindInFiles(cbFindReplaceData* data)
{
    // No find-in-files for CodeSnippets yet (causes crash)
    return 0; //(pecan 2008/7/20)


    if (!data || data->findText.IsEmpty())
        return 0;

    // clear old search results
    if ( data->delOldSearches )
    {
        m_pSearchLog->Clear();
    }
    int oldcount = m_pSearchLog->GetItemsCount();

    // let's make a list of all the files to search in
    wxArrayString filesList;

    if (data->scope == 0) // find in project files
    {
        // fill the search list with all the project files
        cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
        if (!prj)
        {
            cbMessageBox(_("No project to search in!"), _("Error"), wxICON_WARNING);
            return 0;
        }

        wxString fullpath = _T("");
        for (FilesList::iterator it = prj->GetFilesList().begin(); it != prj->GetFilesList().end(); ++it)
        {
            ProjectFile* pf = *it;
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
        for (size_t i = 0; i < m_pNotebook->GetPageCount(); ++i)
        {
            ScbEditor* ed = InternalGetBuiltinEditor(i);
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
        int count = 0;
        if(pProjects)
            count = pProjects->GetCount();
        if(!count)
        {
            cbMessageBox(_("No workspace to search in!"), _("Error"), wxICON_WARNING);
            return 0;
        }
        for (int idxProject = 0; idxProject < count; ++idxProject)
        {
            cbProject* pProject = pProjects->Item(idxProject);
            if(pProject)
            {
                wxString fullpath = _T("");
                for (FilesList::iterator it = pProject->GetFilesList().begin(); it != pProject->GetFilesList().end(); ++it)
                {
                    ProjectFile* pf = *it;
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

    // if the list is empty, leave
    if (filesList.GetCount() == 0)
    {
        cbMessageBox(_("No files to search in!"), _("Error"), wxICON_WARNING);
        return 0;
    }

    // now that list is filled, we'll search
    // but first we'll create a hidden cbStyledTextCtrl to do the search for us ;)
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

        // check if the file is already opened in built-in editor and do search in it
        ScbEditor* ed = IsBuiltinOpen(filesList[i]);
        if (ed)
            control->SetText(ed->GetControl()->GetText());
        else // else load the file in the control
        {
            EncodingDetector detector(filesList[i]);
            if (!detector.IsOK())
            {
                continue; // failed
            }
            control->SetText(detector.GetWxStr());
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
        {
            wxFileName fname(filename);
            fname.MakeRelativeTo(data->searchPath);
            filename = fname.GetFullPath();
        }

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
        if (Manager::Get()->GetConfigManager(_T("message_manager"))->ReadBool(_T("/auto_show_search"), true))
        {
            CodeBlocksLogEvent evtSwitch(cbEVT_SWITCH_TO_LOG_WINDOW, m_pSearchLog);
            CodeBlocksLogEvent evtShow(cbEVT_SHOW_LOG_MANAGER);

            Manager::Get()->ProcessEvent(evtSwitch);
            Manager::Get()->ProcessEvent(evtShow);
        }
        static_cast<SearchResultsLog*>(m_pSearchLog)->FocusEntry(oldcount);
    }
    else
    {
        wxString msg;
        if ( data->delOldSearches )
        {
            msg.Printf(_("Not found: %s"), data->findText.c_str());
            cbMessageBox(msg, _("Result"), wxICON_INFORMATION);
            //-ScbEditor* ed = GetConfig()->GetEditorManager()->GetBuiltinActiveEditor();
            ScbEditor* ed = GetBuiltinActiveEditor();
            if (ed)
                ed->GetControl()->SetSCIFocus(true);
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

int SEditorManager::FindNext(bool goingDown, cbStyledTextCtrl* control, cbFindReplaceData* data)
{
    if (!control)
    {
        ScbEditor* ed = GetBuiltinEditor(GetActiveEditor());
        if (ed)
            control = ed->GetControl();
    }
    if (!control)
        return -1;

    if (!data)
    {
        data = m_LastFindReplaceData;
        //FindNext/Previous called from Search menu (F3/Shift-F3)
        if(data) data->findInFiles = false;
    }

    if (!data)
        return ShowFindDialog(false, false);

    if(!data->findInFiles)
    {
        wxString phraseAtCursor = control->GetSelectedText();

        if ( not data->findUsesSelectedText )
        {   // The mandrav find behavior
            // change findText to selected text (if any text is selected and no search text was set before)
            if (!phraseAtCursor.IsEmpty() && data->findText.IsEmpty())
                data->findText = phraseAtCursor;
        }
        else
        {   // The tiwag find behavior
            // change findText to selected text (if any text is selected)
            if (!phraseAtCursor.IsEmpty())
            {
                data->findText = phraseAtCursor;
                data->originEntireScope = false;  //search from cursor
                data->scope = 0; // global ("selected text" is useful only from Find Dialog)
            }
        }
    }

    data->directionDown = goingDown;
    return Find(control, data);
}

void SEditorManager::OnGenericContextMenuHandler(wxCommandEvent& event)
{
    SEditorBase* eb = GetActiveEditor();
    ScbEditor* ed = GetBuiltinEditor(eb);
    int id = event.GetId();

    if (id == idNBTabSplitHorz && ed)
        ed->Split(ScbEditor::stHorizontal);
    else if (id == idNBTabSplitVert && ed)
        ed->Split(ScbEditor::stVertical);
    else if (id == idNBTabUnsplit && ed)
        ed->Unsplit();
}

void SEditorManager::OnPageChanged(wxAuiNotebookEvent& event)
{
    SEditorBase* eb = static_cast<SEditorBase*>(m_pNotebook->GetPage(event.GetSelection()));
    //    LOGSTREAM << wxString::Format(_T("OnPageChanged(): ed=%p, title=%s\n"), eb, eb ? eb->GetTitle().c_str() : _T(""));
    CodeBlocksEvent evt(cbEVT_EDITOR_ACTIVATED, -1, 0, (EditorBase*)eb);
    //(pecan 2008/4/22) Dont notify plugins from non-sdk
    //-Manager::Get()->GetPluginManager()->NotifyPlugins(evt);

    // focus editor on next update event
    m_pData->m_SetFocusFlag = true;

    event.Skip(); // allow others to process it too
}

void SEditorManager::OnPageChanging(wxAuiNotebookEvent& event)
{
    SEditorBase* eb = static_cast<SEditorBase*>(m_pNotebook->GetPage(event.GetOldSelection()));
    //    LOGSTREAM << wxString::Format(_T("OnPageChanging(): ed=%p, title=%s\n"), eb, eb ? eb->GetTitle().c_str() : _T(""));
    CodeBlocksEvent evt(cbEVT_EDITOR_DEACTIVATED, -1, 0, (EditorBase*)eb);
    ////(pecan 2008/4/22) Dont notify plugins from this non-sdk
    //-Manager::Get()->GetPluginManager()->NotifyPlugins(evt);

    event.Skip(); // allow others to process it too
}

void SEditorManager::OnPageClose(wxAuiNotebookEvent& event)
{
    SEditorBase* eb = static_cast<SEditorBase*>(m_pNotebook->GetPage(event.GetSelection()));
    //    LOGSTREAM << wxString::Format(_T("OnPageClose(): ed=%p, title=%s\n"), eb, eb ? eb->GetTitle().c_str() : _T(""));
    if (!QueryClose(eb))
        event.Veto();
    event.Skip(); // allow others to process it too
}

void SEditorManager::OnTabRightUp(wxAuiNotebookEvent& event)
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

    ScbEditor* ed = GetBuiltinEditor(event.GetSelection());
    if (ed)
    {
        pop->AppendSeparator();
        pop->Append(idNBProperties, _("Properties..."));

        wxMenu* splitMenu = new wxMenu;
        splitMenu->Append(idNBTabSplitHorz, _("Horizontally"));
        splitMenu->Append(idNBTabSplitVert, _("Vertically"));
        splitMenu->AppendSeparator();
        splitMenu->Append(idNBTabUnsplit, _("Unsplit"));
        splitMenu->Enable(idNBTabSplitHorz, ed->GetSplitType() != ScbEditor::stHorizontal);
        splitMenu->Enable(idNBTabSplitVert, ed->GetSplitType() != ScbEditor::stVertical);
        splitMenu->Enable(idNBTabUnsplit, ed->GetSplitType() != ScbEditor::stNoSplit);

        pop->AppendSeparator();
        pop->Append(-1, _("Split view"), splitMenu);
    }

    bool any_modified = false;

    for(int i = 0; i < GetEditorsCount(); ++i)
    {
        SEditorBase* ed2 = GetEditor(i);
        if (ed2 && ed2->GetModified())
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

void SEditorManager::OnClose(cb_unused wxCommandEvent& event)
{
    //-GetConfig()->GetEditorManager()->Close(GetActiveEditor());
    Close(GetActiveEditor());
}

void SEditorManager::OnCloseAll(cb_unused wxCommandEvent& event)
{
    //-GetConfig()->GetEditorManager()->CloseAll();
    CloseAll();
}

void SEditorManager::OnCloseAllOthers(cb_unused wxCommandEvent& event)
{
    //-GetConfig()->GetEditorManager()->CloseAllExcept(GetActiveEditor());
    CloseAllExcept(GetActiveEditor());
}

void SEditorManager::OnSave(cb_unused wxCommandEvent& event)
{
    //-GetConfig()->GetEditorManager()->Save(m_pNotebook->GetSelection());
    Save(m_pNotebook->GetSelection());
}

void SEditorManager::OnSaveAll(cb_unused wxCommandEvent& event)
{
    //-GetConfig()->GetEditorManager()->SaveAll();
    SaveAll();
}

void SEditorManager::OnSwapHeaderSource(cb_unused wxCommandEvent& event)
{
    //-GetConfig()->GetEditorManager()->SwapActiveHeaderSource();
    SwapActiveHeaderSource();
}

void SEditorManager::OnTabPosition(wxCommandEvent& event)
{
    long style = m_pNotebook->GetWindowStyleFlag();
    style &= ~wxAUI_NB_BOTTOM;

    if (event.GetId() == idNBTabBottom)
        style |= wxAUI_NB_BOTTOM;
    m_pNotebook->SetWindowStyleFlag(style);
    // (style & wxFNB_BOTTOM) saves info only about the the tabs position
    Manager::Get()->GetConfigManager(_T("app"))->Write(_T("/environment/editor_tabs_bottom"), (bool)(style & wxAUI_NB_BOTTOM));
}

void SEditorManager::OnProperties(cb_unused wxCommandEvent& event)
{
    ScbEditor* ed = GetBuiltinActiveEditor();
    ProjectFile* pf = 0;
    if (ed)
        pf = ed->GetProjectFile();
    if (pf)
        pf->ShowOptions(Manager::Get()->GetAppWindow());
    else
    {
        //-ProjectFileOptionsDlg dlg(Manager::Get()->GetAppWindow(), GetActiveEditor()->GetFilename());
        //-PlaceWindow(&dlg);
        EditProperties dlg(m_pParent, GetBuiltinActiveEditor());
        //dlg.ShowModal();
    }
}

void SEditorManager::OnAppDoneStartup(wxCommandEvent& event)
{
    event.Skip(); // allow others to process it too
}

void SEditorManager::OnAppStartShutdown(wxCommandEvent& event)
{
    event.Skip(); // allow others to process it too
}

void SEditorManager::OnCheckForModifiedFiles(cb_unused wxCommandEvent& event)
{
    CheckForExternallyModifiedFiles();
}

void SEditorManager::HideNotebook()
{
    //    if(!this)
    //        return;
    //    if(m_pNotebook)
    //        m_pNotebook->Hide();
    //    m_pData->m_NeedsRefresh = false;
    //    return;
}

void SEditorManager::ShowNotebook()
{
    //    if(!this)
    //        return;
    //    if(m_pNotebook)
    //        m_pNotebook->Show();
    //    m_pData->m_NeedsRefresh = true;
    //    m_pData->InvalidateTree();
    //    return;
}

void SEditorManager::OnUpdateUI(wxUpdateUIEvent& event)
{
    /* Don't process UpdateUI event when the app is closing.
     * It may crash C::B */
    if (!Manager::Get()->IsAppShuttingDown() && m_pData->m_SetFocusFlag)
    {
        ScbEditor* ed = GetBuiltinActiveEditor();
        if (ed)
            ed->GetControl()->SetFocus();
        m_pData->m_SetFocusFlag = false;
    }

    // allow other UpdateUI handlers to process this event
    // *very* important! don't forget it...
    event.Skip();
}

void SEditorManager::SetZoom(int zoom)
{
    m_zoom = zoom;
}

int SEditorManager::GetZoom() const
{
    return m_zoom;
}
// ----------------------------------------------------------------------------
int SEditorManager::GetManagerID()
// ----------------------------------------------------------------------------
{
    return ID_NBSEditorManager;
}
// ----------------------------------------------------------------------------
int SEditorManager::GetLongestLinePixelWidth(int top_line, int bottom_line)  //(pecan 2007/3/28)
// ----------------------------------------------------------------------------
{
    int ste_ctrlCharLengths[32] = { 3, 3, 3, 3, 3, 3, 3, 3,
                                       2, 2, 2, 2, 2, 2, 2, 2,
                                       3, 3, 3, 3, 3, 3, 3, 3,
                                       3, 2, 3, 3, 2, 2, 2, 2 };

    cbStyledTextCtrl* ed = GetBuiltinActiveEditor()->GetControl();
    if (not ed) return 0;

    int longest_len   = 0;
    int first_line    = (top_line < 0) ? ed->GetFirstVisibleLine() : top_line;
    int line_count    = ed->GetLineCount();
    int lines_visible = ed->LinesOnScreen();
    int last_line     = bottom_line < 0 ? wxMin(line_count, first_line + lines_visible) : bottom_line;
    int tab_width     = ed->GetTabWidth();
    int ctrl_char_symbol = ed->GetControlCharSymbol();

    if (last_line < first_line)
    {
        int tmp = first_line; first_line = last_line; last_line = tmp;
    }

    // FIXME this is not the best solution, but with some luck it'll work
    //       Scintilla should provide this info from its LayoutCache
    for (int n = first_line; n <= last_line; n++)
    {
        int len = ed->LineLength(n);

        int tabs = 0;
        if ((tab_width > 1) && (len*tab_width > longest_len))
        {
            // need to sum up only how much of the tab is used
            wxCharBuffer buf = ed->GetLineRaw(n);
            const char* c = buf.data();
            for (int i = 0; i < len; i++, c++)
            {
                if (*c == '\t')
                    tabs += tab_width - ((i + tabs) % tab_width);
                else if ((ctrl_char_symbol >= 32) && (size_t(*c) < 32))
                {
                    // scintilla writes name of char
                    tabs += ste_ctrlCharLengths[size_t(*c)] - 1;
                }
            }

            //wxPrintf(wxT("line %d len %d pos %d\n"), n, len, len+tabs); fflush(stdout);
        }
        len += tabs + 3; // add a little extra if showing line endings
        if (longest_len < len) longest_len = len;
    }
    // FIXME:
    //return pEdit->TextWidth(wxSTC_STYLE_DEFAULT, wxString(longest_len, wxT('D')));
    return ed->TextWidth(0, wxString(longest_len, wxT('D')));
}


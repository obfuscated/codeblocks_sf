/***************************************************************
 * Name:      SnipSearchAppMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Pecan ()
 * Created:   2008-02-27
 * Copyright: Pecan ()
 * License:
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include <wx/filesys.h>
#include <wx/fs_zip.h>
#include <wx/fs_mem.h>
#include <wx/xrc/xmlres.h>
#include <wx/dnd.h>
#include <wx/utils.h>
#include "cbauibook.h"


#include <filefilters.h>
#include <cbworkspace.h>

#include "ThreadSearchFrame.h"
#include "ThreadSearch.h"
#include "ThreadSearchView.h"
#include "editormanager.h"
#include "scbeditor.h"
#include "personalitymanager.h"
#include "configmanager.h"
#include "version.h"
#include "snippetsconfig.h"
#include "codesnippetswindow.h"
#include "seditormanager.h"
#include "scbeditor.h"

// ----------------------------------------------------------------------------
class wxMyFileDropTarget : public wxFileDropTarget
// ----------------------------------------------------------------------------
{
public:
    wxMyFileDropTarget(ThreadSearchFrame *frame):m_frame(frame){}
    virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
    {
        if(!m_frame) return false;
        return m_frame->OnDropFiles(x,y,filenames);
    }
private:
    ThreadSearchFrame* m_frame;
};

// ----------------------------------------------------------------------------
namespace
// ----------------------------------------------------------------------------
{
    //helper functions
    enum wxbuildinfoformat {
        short_f, long_f };

    wxString wxbuildinfo(wxbuildinfoformat format)
    {
        wxString wxbuild(wxVERSION_STRING);

        if (format == long_f )
        {
            #if defined(__WXMSW__)
                    wxbuild << _T("-Windows");
            #elif defined(__WXMAC__)
                    wxbuild << _T("-Mac");
            #elif defined(__UNIX__)
                    wxbuild << _T("-Linux");
            #endif

            #if wxUSE_UNICODE
                    wxbuild << _T("-Unicode build");
            #else
                    wxbuild << _T("-ANSI build");
            #endif // wxUSE_UNICODE
        }

        return wxbuild;
    }

    int wxID_FILE10 = wxNewId();
    int wxID_FILE11 = wxNewId();
    int wxID_FILE12 = wxNewId();
    int wxID_FILE13 = wxNewId();
    int wxID_FILE14 = wxNewId();
    int wxID_FILE15 = wxNewId();
    int wxID_FILE16 = wxNewId();
    int wxID_FILE17 = wxNewId();
    int wxID_FILE18 = wxNewId();
    int wxID_FILE19 = wxNewId();

////    int idToolNew = XRCID("idToolNew");
////    int idFileNew = XRCID("idFileNew");
////    int idFileNewEmpty = XRCID("idFileNewEmpty");
////    int idFileNewProject = XRCID("idFileNewProject");
////    int idFileNewTarget = XRCID("idFileNewTarget");
////    int idFileNewFile = XRCID("idFileNewFile");
////    int idFileNewCustom = XRCID("idFileNewCustom");
////    int idFileNewUser = XRCID("idFileNewUser");
    int idFileOpen = XRCID("idFileOpen");
////    int idFileReopen = XRCID("idFileReopen");
    int idFileOpenRecentFileClearHistory = XRCID("idFileOpenRecentFileClearHistory");
    int idFileOpenRecentProjectClearHistory = XRCID("idFileOpenRecentProjectClearHistory");
////    int idFileImportProjectDevCpp = XRCID("idFileImportProjectDevCpp");
////    int idFileImportProjectMSVC = XRCID("idFileImportProjectMSVC");
////    int idFileImportProjectMSVCWksp = XRCID("idFileImportProjectMSVCWksp");
////    int idFileImportProjectMSVS = XRCID("idFileImportProjectMSVS");
////    int idFileImportProjectMSVSWksp = XRCID("idFileImportProjectMSVSWksp");
////    int idFileSave = XRCID("idFileSave");
////    int idFileSaveAs = XRCID("idFileSaveAs");
////    int idFileSaveAllFiles = XRCID("idFileSaveAllFiles");
////    int idFileSaveProject = XRCID("idFileSaveProject");
////    int idFileSaveProjectAs = XRCID("idFileSaveProjectAs");
////    int idFileSaveProjectAllProjects = XRCID("idFileSaveProjectAllProjects");
////    int idFileSaveProjectTemplate = XRCID("idFileSaveProjectTemplate");
////    int idFileOpenDefWorkspace = XRCID("idFileOpenDefWorkspace");
////    int idFileSaveWorkspace = XRCID("idFileSaveWorkspace");
////    int idFileSaveWorkspaceAs = XRCID("idFileSaveWorkspaceAs");
////    int idFileSaveAll = XRCID("idFileSaveAll");
////    int idFileCloseWorkspace = XRCID("idFileCloseWorkspace");
////    int idFileClose = XRCID("idFileClose");
////    int idFileCloseAll = XRCID("idFileCloseAll");
////    int idFileCloseProject = XRCID("idFileCloseProject");
////    int idFileCloseAllProjects = XRCID("idFileCloseAllProjects");
////    int idFilePrintSetup = XRCID("idFilePrintSetup");
////    int idFilePrint = XRCID("idFilePrint");
////    int idFileExit = XRCID("idFileExit");
////    int idFileNext = wxNewId();
////    int idFilePrev = wxNewId();
////
////    int idEditUndo = XRCID("idEditUndo");
////    int idEditRedo = XRCID("idEditRedo");
////    int idEditCopy = XRCID("idEditCopy");
////    int idEditCut = XRCID("idEditCut");
////    int idEditPaste = XRCID("idEditPaste");
////    int idEditSwapHeaderSource = XRCID("idEditSwapHeaderSource");
////    int idEditGotoMatchingBrace = XRCID("idEditGotoMatchingBrace");
////    int idEditHighlightMode = XRCID("idEditHighlightMode");
////    int idEditHighlightModeText = XRCID("idEditHighlightModeText");
////    int idEditBookmarks = XRCID("idEditBookmarks");
////    int idEditBookmarksToggle = XRCID("idEditBookmarksToggle");
////    int idEditBookmarksPrevious = XRCID("idEditBookmarksPrevious");
////    int idEditBookmarksNext = XRCID("idEditBookmarksNext");
////    int idEditFolding = XRCID("idEditFolding");
////    int idEditFoldAll = XRCID("idEditFoldAll");
////    int idEditUnfoldAll = XRCID("idEditUnfoldAll");
////    int idEditToggleAllFolds = XRCID("idEditToggleAllFolds");
////    int idEditFoldBlock = XRCID("idEditFoldBlock");
////    int idEditUnfoldBlock = XRCID("idEditUnfoldBlock");
////    int idEditToggleFoldBlock = XRCID("idEditToggleFoldBlock");
////    int idEditEOLMode = XRCID("idEditEOLMode");
////    int idEditEOLCRLF = XRCID("idEditEOLCRLF");
////    int idEditEOLCR = XRCID("idEditEOLCR");
////    int idEditEOLLF = XRCID("idEditEOLLF");
////    int idEditEncoding = XRCID("idEditEncoding");
////    int idEditEncodingDefault = XRCID("idEditEncodingDefault");
////    int idEditEncodingUseBom = XRCID("idEditEncodingUseBom");
////    int idEditEncodingAscii = XRCID("idEditEncodingAscii");
////    int idEditEncodingUtf7 = XRCID("idEditEncodingUtf7");
////    int idEditEncodingUtf8 = XRCID("idEditEncodingUtf8");
////    int idEditEncodingUnicode = XRCID("idEditEncodingUnicode");
////    int idEditEncodingUtf16 = XRCID("idEditEncodingUtf16");
////    int idEditEncodingUtf32 = XRCID("idEditEncodingUtf32");
////    int idEditEncodingUnicode16BE = XRCID("idEditEncodingUnicode16BE");
////    int idEditEncodingUnicode16LE = XRCID("idEditEncodingUnicode16LE");
////    int idEditEncodingUnicode32BE = XRCID("idEditEncodingUnicode32BE");
////    int idEditEncodingUnicode32LE = XRCID("idEditEncodingUnicode32LE");
////    int idEditSpecialCommands = XRCID("idEditSpecialCommands");
////    int idEditSpecialCommandsMovement = XRCID("idEditSpecialCommandsMovement");
////    int idEditParaUp = XRCID("idEditParaUp");
////    int idEditParaUpExtend = XRCID("idEditParaUpExtend");
////    int idEditParaDown = XRCID("idEditParaDown");
////    int idEditParaDownExtend = XRCID("idEditParaDownExtend");
////    int idEditWordPartLeft = XRCID("idEditWordPartLeft");
////    int idEditWordPartLeftExtend = XRCID("idEditWordPartLeftExtend");
////    int idEditWordPartRight = XRCID("idEditWordPartRight");
////    int idEditWordPartRightExtend = XRCID("idEditWordPartRightExtend");
////    int idEditSpecialCommandsZoom = XRCID("idEditSpecialCommandsZoom");
////    int idEditZoomIn = XRCID("idEditZoomIn");
////    int idEditZoomOut = XRCID("idEditZoomOut");
////    int idEditZoomReset = XRCID("idEditZoomReset");
////    int idEditSpecialCommandsLine = XRCID("idEditSpecialCommandsLine");
////    int idEditLineCut = XRCID("idEditLineCut");
////    int idEditLineDelete = XRCID("idEditLineDelete");
////    int idEditLineDuplicate = XRCID("idEditLineDuplicate");
////    int idEditLineTranspose = XRCID("idEditLineTranspose");
////    int idEditLineCopy = XRCID("idEditLineCopy");
////    int idEditLinePaste = XRCID("idEditLinePaste");
////    int idEditSpecialCommandsCase = XRCID("idEditSpecialCommandsCase");
////    int idEditUpperCase = XRCID("idEditUpperCase");
////    int idEditLowerCase = XRCID("idEditLowerCase");
////    int idEditSelectAll = XRCID("idEditSelectAll");
////    int idEditCommentSelected = XRCID("idEditCommentSelected");
////    int idEditUncommentSelected = XRCID("idEditUncommentSelected");
////    int idEditToggleCommentSelected = XRCID("idEditToggleCommentSelected");
////    int idEditStreamCommentSelected = XRCID("idEditStreamCommentSelected");
////    int idEditBoxCommentSelected = XRCID("idEditBoxCommentSelected");
////    int idEditAutoComplete = XRCID("idEditAutoComplete");
////
    int idSearchFind = XRCID("idSearchFind");
    int idSearchFindInFiles = XRCID("idSearchFindInFiles");
    int idSearchFindNext = XRCID("idSearchFindNext");
    int idSearchFindPrevious = XRCID("idSearchFindPrevious");
////    int idSearchReplace = XRCID("idSearchReplace");
////    int idSearchReplaceInFiles = XRCID("idSearchReplaceInFiles");
////    int idSearchGotoLine = XRCID("idSearchGotoLine");
}
// ----------------------------------------------------------------------------
//      Events Table
// ----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(ThreadSearchFrame, wxFrame)

    EVT_ACTIVATE(         ThreadSearchFrame::OnFrameActivated)
    EVT_SIZE(             ThreadSearchFrame::OnSizeWindow)
    EVT_CLOSE(            ThreadSearchFrame::OnClose)
    EVT_MENU(idMenuQuit,  ThreadSearchFrame::OnQuit)
    EVT_MENU(idMenuAbout, ThreadSearchFrame::OnAbout)
    // File
    EVT_MENU(idFileOpen,  ThreadSearchFrame::OnFileOpen)

    // Edit

    // Search/Find
    EVT_MENU(idSearchFind,          ThreadSearchFrame::OnSearchFind)
    EVT_MENU(idSearchFindInFiles,   ThreadSearchFrame::OnSearchFind)
    EVT_MENU(idSearchFindNext,      ThreadSearchFrame::OnSearchFindNext)
    EVT_MENU(idSearchFindPrevious,  ThreadSearchFrame::OnSearchFindNext)

    EVT_CODESNIPPETS_NEW_INDEX (wxID_ANY,    ThreadSearchFrame::OnCodeSnippetsNewIndex)

END_EVENT_TABLE()

// ----------------------------------------------------------------------------
ThreadSearchFrame::ThreadSearchFrame(wxFrame* appFrame, const wxString& title)
// ----------------------------------------------------------------------------
    : wxFrame(appFrame, -1, title)
      ,m_pFilesHistory(0)
      ,m_pProjectsHistory(0)
      ,m_bOnActivateBusy(0)
      ,m_pThreadSearch(0)
{
    bool ok = InitThreadSearchFrame( appFrame, title);
    wxUnusedVar(ok);
}
// ----------------------------------------------------------------------------
bool ThreadSearchFrame::InitThreadSearchFrame(wxFrame* appFrame, const wxString& title)
// ----------------------------------------------------------------------------
{
    GetConfig()->SetThreadSearchFrame( this );

    // create a menu bar
    CreateMenuBar();

    // create a status bar with some information about the used wxWidgets version
    CreateStatusBar(2);
    SetStatusText(_("CodeSnippets Search"),0);
    SetStatusText(wxbuildinfo(short_f), 1);

    InitializeRecentFilesHistory(); // -not used yet-

    // allocate a separate EditorManager/Notebook
    if (not GetConfig()->GetEditorManager(this))
    {
        SEditorManager* m_pEdMan = new SEditorManager(this);
        GetConfig()->RegisterEditorManager(this, m_pEdMan);
    }//if GetEditorManager

    // create ThreadSearch and alter its menu items
    m_pThreadSearch = new ThreadSearch( this );
    if (  m_pThreadSearch ) do
    {
        m_pThreadSearch->ThreadSearch::m_IsAttached = true;
        m_pThreadSearch->OnAttach();
        PushEventHandler(m_pThreadSearch);
        m_pThreadSearch->SetEvtHandlerEnabled( true );

        // add View and Search menu items
        wxMenuBar* pMenuBar = this->GetMenuBar();
        wxMenu* pMenuView = new wxMenu();
        //-wxMenu* pMenuSearch = pMenuBar->GetMenu( pMenuBar->FindMenu(_T("Search")));
        pMenuBar->Insert( 1, pMenuView, _T("View"));
        //-pMenuBar->Insert( 2, pMenuSearch, _T("Search"));
        m_pThreadSearch->BuildMenu( pMenuBar );
        // Change 'View/ThreadSearch' to 'View/Options'
        int idOptionsThreadSearch = pMenuBar->FindMenuItem(_T("View"),_T("Snippets search"));
        if (idOptionsThreadSearch not_eq wxNOT_FOUND)
        {   pMenuBar->SetLabel( idOptionsThreadSearch, _T("Options...") );
            m_pThreadSearch->Connect(idOptionsThreadSearch, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ThreadSearchFrame::OnMenuOptions), NULL, this);
        }

        // create tool bar and hide it (avoids bar reference crashes)
        wxToolBar* pToolBar = new wxToolBar(this, -1);
        if (  m_pThreadSearch )
            m_pThreadSearch->BuildToolBar( pToolBar );
        pToolBar->Hide();

        // move frame to last know frame position
        ConfigManager* pCfg = Manager::Get()->GetConfigManager(_T("SnippetsSearch"));
        int xPos = pCfg->ReadInt( wxT("/FramePosX"), 120);
        int yPos = pCfg->ReadInt( wxT("/FramePosY"), 60);
        int width = pCfg->ReadInt( wxT("/FrameWidth"), 120);
        int height = pCfg->ReadInt( wxT("/FrameHeight"), 60);
        SetSize( xPos, yPos, width, height);

        // Catch Destroyed windows
        Connect( wxEVT_DESTROY,
            (wxObjectEventFunction) (wxEventFunction)
            (wxCommandEventFunction) &ThreadSearchFrame::OnWindowDestroy);

        // Allow filenames to be dropped/opened on ThreadSearchFrame
        SetDropTarget(new wxMyFileDropTarget(this));
        GetConfig()->GetEditorManager(this)->GetNotebook()->SetDropTarget(new wxMyFileDropTarget(this));

    }while(false);//if m_pThreadSearch do

    return m_pThreadSearch;

}//initThreadSearchFrame ctor
// ----------------------------------------------------------------------------
ThreadSearchFrame::~ThreadSearchFrame()
// ----------------------------------------------------------------------------
{
    //dtor
    Disconnect( wxEVT_DESTROY,
        (wxObjectEventFunction) (wxEventFunction)
        (wxCommandEventFunction) &ThreadSearchFrame::OnWindowDestroy);

    GetConfig()->SetThreadSearchFrame(0);
}
// ----------------------------------------------------------------------------
void ThreadSearchFrame::OnMenuOptions(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    //m_pThreadSearch->m_pThreadSearchView->OnBtnOptionsClick(event);
    m_pThreadSearch->Configure();
}
// ----------------------------------------------------------------------------
void ThreadSearchFrame::OnClose(wxCloseEvent &event)
// ----------------------------------------------------------------------------
{
    this->Show(false);  //avoid fragmented disappearing windows

    // memorize current ThreadSearchFrame position
    ConfigManager* pCfg = Manager::Get()->GetConfigManager(_T("SnippetsSearch"));
    int xPos, yPos, width, height;
    GetPosition(&xPos,&yPos);
    GetSize(&width,&height);
    pCfg->Write(wxT("/FramePosX"), xPos );
    pCfg->Write(wxT("/FramePosY"), yPos);
    pCfg->Write(wxT("/FrameWidth"), width);
    pCfg->Write(wxT("/FrameHeight"), height);

    //-if (GetConfig()->IsApplication())
    {   // we have to close all open editors or we'll crash
        // in wxAuiNoteBook::GetPageCount(). EditoManager call wxAuiNotebook
        // to close 'em, but wxAuiNotebook has alread been deleted by
        // wxWidgets Destory().
        SEditorManager* pEdMan = GetConfig()->GetEditorManager(this);
        if (pEdMan)
        {   ScbEditor* ed;
            int knt = pEdMan->GetEditorsCount();
            for ( int i=knt; i>0; --i )
            {
                ed = (ScbEditor*)pEdMan->GetEditor(i-1);
                if ( ed ) ed->Close();
            }
        }
    }//if

    // reverse splitterwindow switches and reparenting
    if (m_pThreadSearch){
        RemoveEventHandler(m_pThreadSearch);
        m_pThreadSearch->OnRelease(true);
    }

    // free separate editor manager
    SEditorManager* m_pEdMan = GetConfig()->GetEditorManager(this);
    if ( m_pEdMan )
    {
        RemoveEventHandler( m_pEdMan ); // *do this or crash @ Destroy()*
        delete m_pEdMan;
        GetConfig()->RemoveEditorManager((wxFrame*)this);
    }
    // free ThreadSearch plugin/evtHandler
    if ( m_pThreadSearch ){
        delete m_pThreadSearch; // deletes ThreadSearch wxEvtHandler object
        m_pThreadSearch = 0;
    }

    // release memory in FileLinks array used by ThreadSearch
    GetConfig()->ClearFileLinksMapArray();

    //- write the configuation file
    //- Don't free Manager unless we initialized it. which we didn't.
    //-Manager::Free();
    //-GetConfig()->GetSnippetsWindow()->CloseThreadSearchFrame();

    Destroy();
}
// ----------------------------------------------------------------------------
void ThreadSearchFrame::OnQuit(wxCommandEvent &event)
// ----------------------------------------------------------------------------
{
    wxWindow::Close();
}
// ----------------------------------------------------------------------------
void ThreadSearchFrame::OnAbout(wxCommandEvent &event)
// ----------------------------------------------------------------------------
{
    wxString msg = wxbuildinfo(long_f);
    msg << _T("\n\n");
    msg << _T("Original ThreadSearch code by Jerome Antoine \n");
	msg << _T("Ported to CodeSnippets by Pecan Heber \n");
    msg << _T("\n");
	msg << _T("Click Log item once to display snippet in preview window. \n");
	msg << _T("Double click Log item to display in editor window. \n");
    msg << _T("\n");
	msg << _T("Double clicking a CodeSnippets \"Category\" log item \n");
	msg << _T("simple highlights the item in the index (tree) window. \n");

    wxMessageBox(msg, _("Welcome to..."));
}
// ----------------------------------------------------------------------------
void ThreadSearchFrame::OnSizeWindow(wxSizeEvent &event)
// ----------------------------------------------------------------------------
{
    // On the first resizing of the frame, the editor notebook splitter sash
    // is getting put back in the middle of the window, not where the user
    // placed it.
    #if defined(LOGGING)
    LOGIT( _T("ThreadSearchFrame::OnSizeWindow"));
    #endif

    event.Skip();
    if (not GetConfig()->GetThreadSearchPlugin() ) return;
    GetConfig()->GetThreadSearchPlugin()->UserResizingWindow(event);
    event.Skip();
    return;
}
// ----------------------------------------------------------------------------
void ThreadSearchFrame::OnSearchFind(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    //(pecan 2008/7/20)
    //Don't search in preview pane, and don't search if no editor is open
    ScbEditor* sEd = GetConfig()->GetEditorManager(this)->GetBuiltinActiveEditor();
    if (not sEd)
        return;
    if (not ((wxWindow*)(sEd->GetControl()) == wxWindow::FindFocus()))
        return;

    bool bDoMultipleFiles = (event.GetId() == idSearchFindInFiles);
    if(!bDoMultipleFiles)
    {
        //-bDoMultipleFiles = !Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
        bDoMultipleFiles = not GetConfig()->GetEditorManager(this)->GetBuiltinActiveEditor();
    }
    //-Manager::Get()->GetEditorManager()->ShowFindDialog(false, bDoMultipleFiles);
    GetConfig()->GetEditorManager(this)->ShowFindDialog(false, false);
}// end of OnSearchFind
// ----------------------------------------------------------------------------
void ThreadSearchFrame::OnSearchFindNext(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    //(pecan 2008/7/20) FullSearch
    //Don't search in preview pane, and don't search if no editor is open
    ScbEditor* sEd = GetConfig()->GetEditorManager(this)->GetBuiltinActiveEditor();
    if (not sEd)
        return;
    if (not ((wxWindow*)(sEd->GetControl()) == wxWindow::FindFocus()))
        return;

    bool bNext = !(event.GetId() == idSearchFindPrevious);
    //-Manager::Get()->GetEditorManager()->FindNext(bNext);
    GetConfig()->GetEditorManager(this)->FindNext(bNext);
} // end of OnSearchFindNext

// ----------------------------------------------------------------------------
bool ThreadSearchFrame::InitXRCStuff()
// ----------------------------------------------------------------------------
{
    if (!Manager::LoadResource(_T("resources.zip")))
	{
		ComplainBadInstall();
		return false;
	}
    if (!Manager::LoadResource(_T("resources.zip")))
	{
		ComplainBadInstall();
		return false;
	}
    return true;
}
// ----------------------------------------------------------------------------
void ThreadSearchFrame::ComplainBadInstall()
// ----------------------------------------------------------------------------
{
    wxString msg;
    msg.Printf(_T("Cannot find resources...\n"
        "%s was configured to be installed in '%s'.\n"
        "Please use the command-line switch '--prefix' or "
        "set the CODEBLOCKS_DATA_DIR environment variable "
        "to point where %s is installed,\n"
        "or try re-installing the application..."),
        _T("CodeSnippetsApp"),
        ConfigManager::ReadDataPath().c_str(),
        wxTheApp->GetAppName().c_str());
    cbMessageBox(msg);
}
// ----------------------------------------------------------------------------
void ThreadSearchFrame::CreateMenuBar()
// ----------------------------------------------------------------------------
{
    // create a menu bar
    wxMenuBar* mbar = new wxMenuBar();
    wxMenu* fileMenu = new wxMenu(_T(""));
    fileMenu->Append(idFileOpen, _("&Open...\tCtrl-O"), _("Open file"));
    fileMenu->Append(idMenuQuit, _("&Quit\tAlt-F4"), _("Quit the application"));
    mbar->Append(fileMenu, _("&File"));

    wxMenu* searchMenu = new wxMenu(_T(""));
    searchMenu->Append(idSearchFind, _("&Find...\tCtrl-F"), _("Find"));
    searchMenu->Append(idSearchFindNext, _("Find &Next\tCtrl-N"), _("Find Next"));
    searchMenu->Append(idSearchFindPrevious, _("Find &Previous\tShift-Ctrl-N"), _("Find Previous"));
    mbar->Append(searchMenu, _("&Search"));

    wxMenu* helpMenu = new wxMenu(_T(""));
    helpMenu->Append(idMenuAbout, _("&About\tF1"), _("Show info about this application"));
    mbar->Append(helpMenu, _("&Help"));

    SetMenuBar(mbar);
}
// ----------------------------------------------------------------------------
void ThreadSearchFrame::OnFileOpen(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    if (not GetConfig()->GetThreadSearchPlugin() ) return;
    GetConfig()->GetThreadSearchPlugin()->SplitThreadSearchWindow();
    DoOnFileOpen(false); // through file menu (not sure if we are opening a project)
} // end of OnFileOpen

// ----------------------------------------------------------------------------
// DoOnFileOpen:
// in case we are opening a project (bProject == true) we do not want to interfere
// with 'last type of files' (probably the call was open (existing) project on the
// start here page --> so we know it's a project --> set the filter accordingly
// but as said don't force the 'last used type of files' to change, that should
// only change when an open file is carried out (so (source) file <---> project (file) )
// TODO : when regular file open and user manually sets filter to project files --> will change
//      the last type : is that expected behaviour ???
// ----------------------------------------------------------------------------
void ThreadSearchFrame::DoOnFileOpen(bool bProject)
// ----------------------------------------------------------------------------
{
    wxString Filters = FileFilters::GetFilterString();
    // the value returned by GetIndexForFilterAll() is updated by GetFilterString()
    int StoredIndex = FileFilters::GetIndexForFilterAll();
    wxString Path;
    ConfigManager* mgr = Manager::Get()->GetConfigManager(_T("app"));
    if(mgr)
    {
        if(!bProject)
        {
            wxString Filter = mgr->Read(_T("/file_dialogs/file_new_open/filter"));
            if(!Filter.IsEmpty())
            {
                FileFilters::GetFilterIndexFromName(Filters, Filter, StoredIndex);
            }
            Path = mgr->Read(_T("/file_dialogs/file_new_open/directory"), Path);
        }
        else
        {
            FileFilters::GetFilterIndexFromName(Filters, _("Code::Blocks project files"), StoredIndex);
        }
    }
    wxFileDialog* dlg = new wxFileDialog(this,
                            _("Open file"),
                            Path,
                            wxEmptyString,
                            Filters,
                            wxFD_OPEN | wxFD_MULTIPLE | compatibility::wxHideReadonly);
    dlg->SetFilterIndex(StoredIndex);

    PlaceWindow(dlg);
    if (dlg->ShowModal() == wxID_OK)
    {
        // store the last used filter and directory
        // as said : don't do this in case of an 'open project'
        if(mgr && !bProject)
        {
            int Index = dlg->GetFilterIndex();
            wxString Filter;
            if(FileFilters::GetFilterNameFromIndex(Filters, Index, Filter))
            {
                mgr->Write(_T("/file_dialogs/file_new_open/filter"), Filter);
            }
            wxString Test = dlg->GetDirectory();
            mgr->Write(_T("/file_dialogs/file_new_open/directory"), dlg->GetDirectory());
        }
        wxArrayString files;
        dlg->GetPaths(files);
        OnDropFiles(0,0,files);
    }

    dlg->Destroy();
} // end of DoOnFileOpen
// ----------------------------------------------------------------------------
bool ThreadSearchFrame::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& files)
// ----------------------------------------------------------------------------
{
    bool success = true; // Safe case initialisation

    // first check to see if a workspace is passed. If so, only this will be loaded
    wxString foundWorkspace;
    for (unsigned int i = 0; i < files.GetCount(); ++i)
    {
        FileType ft = FileTypeOf(files[i]);
        if (ft == ftCodeBlocksWorkspace || ft == ftMSVC6Workspace || ft == ftMSVC7Workspace)
        {
            foundWorkspace = files[i];
            break;
        }
    }

    if (!foundWorkspace.IsEmpty())
      success &= OpenGeneric(foundWorkspace);
    else
    {
        wxBusyCursor useless;
        wxPaintEvent e;
        ProcessEvent(e);

        Freeze();
        for (unsigned int i = 0; i < files.GetCount(); ++i)
          success &= OpenGeneric(files[i]);
        Thaw();
    }
    return success;
}
// ----------------------------------------------------------------------------
bool ThreadSearchFrame::OpenGeneric(const wxString& filename, bool addToHistory)
// ----------------------------------------------------------------------------
{
    if (filename.IsEmpty())
        return false;

    // Split the window to show notebook and file panel //(pecan 2008/5/19)
    if (not GetConfig()->GetThreadSearchPlugin() ) return false;
    GetConfig()->GetThreadSearchPlugin()->SplitThreadSearchWindow();

    wxFileName fname(filename);
    fname.ClearExt();
    fname.SetExt(_T("cbp"));
    switch(FileTypeOf(filename))
    {
        //
        // Workspace
        //
        ////case ftCodeBlocksWorkspace:
        ////    // verify that it's not the same as the one already open
        ////    if (filename != Manager::Get()->GetProjectManager()->GetWorkspace()->GetFilename() &&
        ////        DoCloseCurrentWorkspace())
        ////    {
        ////        wxBusyCursor wait; // loading a worspace can take some time -> showhourglass
        ////        bool ret = Manager::Get()->GetProjectManager()->LoadWorkspace(filename);
        ////        if (ret && addToHistory)
        ////            AddToRecentProjectsHistory(Manager::Get()->GetProjectManager()->GetWorkspace()->GetFilename());
        ////        return ret;
        ////    }
        ////    else
        ////        return false;
        ////    break;

        //
        // Project
        //
        ////case ftCodeBlocksProject:
        ////{
        ////    // Make a check whether the project exists in current workspace
        ////    cbProject* prj = Manager::Get()->GetProjectManager()->IsOpen(fname.GetFullPath());
        ////    if (!prj)
        ////    {
        ////        wxBusyCursor wait; // loading a worspace can take some time -> showhourglass
        ////        return DoOpenProject(filename, addToHistory);
        ////    }
        ////    else
        ////    {
        ////        // NOTE (Morten#1#): A message here will prevent batch-builds from working and is shown sometimes even if correct
        ////        Manager::Get()->GetProjectManager()->SetProject(prj, false);
        ////        return true;
        ////    }
        ////}

        //
        // Source files
        //
        case ftHeader:
            // fallthrough

        case ftSource:
            // fallthrough
        case ftResource:
            return DoOpenFile(filename, addToHistory);
        //
        // For all other files, ask MIME plugin for a suitable handler
        //
        default:
        {
            ////cbMimePlugin* plugin = Manager::Get()->GetPluginManager()->GetMIMEHandlerForFile(filename);
            ////// warn user that "Files extension handler" is disabled
            ////if (!plugin)
            ////{
            ////    cbMessageBox(_("Could not open file ") + filename + _(",\nbecause no extension handler could be found."), _("Error"), wxICON_ERROR);
            ////    return false;
            ////}
            ////if (plugin->OpenFile(filename) == 0)
            ////{
            ////    AddToRecentFilesHistory(filename);
            ////    return true;
            ////}
            ////return false;
            return DoOpenFile(filename, addToHistory); //(pecan 2008/3/15)
        }//default
    }
    return true;
}
// ----------------------------------------------------------------------------
bool ThreadSearchFrame::DoOpenFile(const wxString& filename, bool addToHistory)
// ----------------------------------------------------------------------------
{
    //-if (Manager::Get()->GetEditorManager()->Open(filename))
    if ( GetConfig()->GetEditorManager(this)->Open(filename))
    {
        if (addToHistory)
            AddToRecentFilesHistory(filename);
        return true;
    }
    return false;
}
// ----------------------------------------------------------------------------
void ThreadSearchFrame::AddToRecentFilesHistory(const wxString& FileName)
// ----------------------------------------------------------------------------
{
    wxString filename = FileName;
#ifdef __WXMSW__
    // for windows, look for case-insensitive matches
    // if found, don't add it
    wxString low = filename.Lower();
    for (size_t i = 0; i < m_pFilesHistory->GetCount(); ++i)
    {
        if (low == m_pFilesHistory->GetHistoryFile(i).Lower())
        {    // it exists, set filename to the existing name, so it can become
            // the most recent one
            filename = m_pFilesHistory->GetHistoryFile(i);
            break;
        }
    }
#endif

    m_pFilesHistory->AddFileToHistory(filename);

    // because we append "clear history" menu to the end of the list,
    // each time we must add a history item we have to:
    // a) remove "Clear history"
    // b) clear the menu
    // c) fill it with the history items
    // and d) append "Clear history"...
    wxMenuBar* mbar = GetMenuBar();
    if (!mbar)
        return;
    int pos = mbar->FindMenu(_("&File"));
    if (pos == wxNOT_FOUND)
        return;
    wxMenu* menu = mbar->GetMenu(pos);
    if (!menu)
        return;
    wxMenu* recentFiles = 0;
    wxMenuItem* clear = menu->FindItem(idFileOpenRecentFileClearHistory, &recentFiles);
    if (clear && recentFiles)
    {
        // a)
        recentFiles->Remove(clear);
        // b)
        m_pFilesHistory->RemoveMenu(recentFiles);
        while (recentFiles->GetMenuItemCount())
            recentFiles->Delete(recentFiles->GetMenuItems()[0]);
        // c)
        m_pFilesHistory->UseMenu(recentFiles);
        m_pFilesHistory->AddFilesToMenu(recentFiles);
        // d)
        if (recentFiles->GetMenuItemCount())
            recentFiles->AppendSeparator();
        recentFiles->Append(clear);
    }

    // update start here page
    ////EditorBase* sh = Manager::Get()->GetEditorManager()->GetEditor(g_StartHereTitle);
    ////if (sh)
    ////    ((StartHerePage*)sh)->Reload();
}
// ----------------------------------------------------------------------------
void ThreadSearchFrame::InitializeRecentFilesHistory()
// ----------------------------------------------------------------------------
{
    TerminateRecentFilesHistory();

    wxMenuBar* mbar = GetMenuBar();
    if (!mbar)
        return;
    int pos = mbar->FindMenu(_("&File"));
    if (pos != wxNOT_FOUND)
    {
        m_pFilesHistory = new wxFileHistory(9, wxID_FILE1);

        wxMenu* menu = mbar->GetMenu(pos);
        if (!menu)
            return;
        wxMenu* recentFiles = 0;
        wxMenuItem* clear = menu->FindItem(idFileOpenRecentFileClearHistory, &recentFiles);
        if (recentFiles)
        {
            recentFiles->Remove(clear);

            wxArrayString files = Manager::Get()->GetConfigManager(_T("app"))->ReadArrayString(_T("/recent_files"));
            for (int i = (int)files.GetCount() - 1; i >= 0; --i)
            {
                if(wxFileExists(files[i]))
                    m_pFilesHistory->AddFileToHistory(files[i]);
            }
            m_pFilesHistory->UseMenu(recentFiles);
            m_pFilesHistory->AddFilesToMenu(recentFiles);
            if (recentFiles->GetMenuItemCount())
                recentFiles->AppendSeparator();
            recentFiles->Append(clear);
        }
        wxMenu* recentProjects = 0;
        clear = menu->FindItem(idFileOpenRecentProjectClearHistory, &recentProjects);
        if (recentProjects)
        {
            m_pProjectsHistory = new wxFileHistory(9, wxID_FILE10);
            recentProjects->Remove(clear);

            wxArrayString files = Manager::Get()->GetConfigManager(_T("app"))->ReadArrayString(_T("/recent_projects"));
            for (int i = (int)files.GetCount() - 1; i >= 0; --i)
            {
                if(wxFileExists(files[i]))
                    m_pProjectsHistory->AddFileToHistory(files[i]);
            }
            m_pProjectsHistory->UseMenu(recentProjects);
            m_pProjectsHistory->AddFilesToMenu(recentProjects);
            if (recentProjects->GetMenuItemCount())
                recentProjects->AppendSeparator();
            recentProjects->Append(clear);
        }
    }
}
// ----------------------------------------------------------------------------
void ThreadSearchFrame::TerminateRecentFilesHistory()
// ----------------------------------------------------------------------------
{
    if (m_pFilesHistory)
    {
        wxArrayString files;
        for (unsigned int i = 0; i < m_pFilesHistory->GetCount(); ++i)
            files.Add(m_pFilesHistory->GetHistoryFile(i));
        Manager::Get()->GetConfigManager(_T("app"))->Write(_T("/recent_files"), files);

        wxMenuBar* mbar = GetMenuBar();
        if (mbar)
        {
            int pos = mbar->FindMenu(_("&File"));
            if (pos != wxNOT_FOUND)
            {
                wxMenu* menu = mbar->GetMenu(pos);
                if (menu)
                {
                    wxMenu* recentFiles = 0;
                    menu->FindItem(idFileOpenRecentFileClearHistory, &recentFiles);
                    if (recentFiles)
                        m_pFilesHistory->RemoveMenu(recentFiles);
                }
            }
        }
        delete m_pFilesHistory;
        m_pFilesHistory = 0;
    }

    if (m_pProjectsHistory)
    {
        wxArrayString files;
        for (unsigned int i = 0; i < m_pProjectsHistory->GetCount(); ++i)
            files.Add(m_pProjectsHistory->GetHistoryFile(i));
        Manager::Get()->GetConfigManager(_T("app"))->Write(_T("/recent_projects"), files);

        wxMenuBar* mbar = GetMenuBar();
        if (mbar)
        {
            int pos = mbar->FindMenu(_("&File"));
            if (pos != wxNOT_FOUND)
            {
                wxMenu* menu = mbar->GetMenu(pos);
                if (menu)
                {
                    wxMenu* recentProjects = 0;
                    menu->FindItem(idFileOpenRecentProjectClearHistory, &recentProjects);
                    if (recentProjects)
                        m_pProjectsHistory->RemoveMenu(recentProjects);
                }
            }
        }
        delete m_pProjectsHistory;
        m_pProjectsHistory = 0;
    }
}
// ----------------------------------------------------------------------------
void ThreadSearchFrame::OnCodeSnippetsNewIndex(CodeSnippetsEvent& event)
// ----------------------------------------------------------------------------
{
    #if defined(LOGGING)
    LOGIT( _T("ThreadSearchFrame::OnCodeSnippetsNewIndex id[%d]str[%s]"), event.GetSnippetID(), event.GetSnippetString().c_str());
    #endif

    event.Skip();
    return;
}
// ----------------------------------------------------------------------------
void ThreadSearchFrame::OnWindowDestroy(wxEvent& event)
// ----------------------------------------------------------------------------
{
    // wxEVT_DESTROY entry

    wxWindow* pWindow = (wxWindow*)(event.GetEventObject());

    if ( (pWindow) && (pWindow->GetName() == _T("SCIwindow")))
    {
        #ifdef LOGGING
         LOGIT( _T("ThreadSearchFrame::OnWindowDestroy [%p]"), pWindow);
        #endif //LOGGING
        int count = GetConfig()->GetEditorManager(this)->GetEditorsCount();
        if (count == 1) //closing last window
            GetConfig()->GetThreadSearchPlugin()->UnsplitThreadSearchWindow();
    }
    event.Skip();
}//OnWindowClose
// ----------------------------------------------------------------------------
void ThreadSearchFrame::OnFrameActivated(wxActivateEvent& event)
// ----------------------------------------------------------------------------
{
    // This frame has been activated

    if ( m_bOnActivateBusy ) {event.Skip();return;}
    ++m_bOnActivateBusy;

    // Check that it's us that got activated
    if ( event.GetActive() )
      do { //only once

        // Check that CodeSnippets actually has a file open
        if (not GetConfig()->GetSnippetsWindow() )  break;
        if (not GetConfig()->GetSnippetsTreeCtrl() ) break;

        #if defined(LOGGING)
        LOGIT( _T("ThreadSearchFrame::OnAppActivate"));
        #endif
        //-if  ( (GetConfig()->GetEditorManagerCount() ) <--causes loop betwn ThreadSearchFrame and EditSnippetFrame
        if  ( (GetConfig()->GetEditorManager(this) )
              && (Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/check_modified_files"), true))
            )
            //-for (int i = 0; i < GetConfig()->GetEditorManagerCount(); ++i) <--causes loop betwn ThreadSearchFrame and EditSnippetFrame
            {
                // for some reason a mouse up event doesnt make it into scintilla (scintilla bug)
                // therefor the workaournd is not to directly call the editorManager, but
                // take a detour through an event
                // the bug is when the file has been offered to reload, no matter what answer you
                // give the mouse is in a selecting mode, adding/removing things to it's selection as you
                // move it around
                // so : idEditorManagerCheckFiles, EditorManager::OnCheckForModifiedFiles just exist for this workaround

                // If SEditorManager belongs to this frame, check for modified files
                wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, idSEditorManagerCheckFiles);
                //-wxPostEvent(GetConfig()->GetEditorManager(i), evt);
                wxPostEvent(GetConfig()->GetEditorManager(this), evt);
                //-GetConfig()->GetEditorManager(i)->ProcessEvent( evt);
            }
    }while(0); //do only once

    m_bOnActivateBusy = 0;
    event.Skip();
    return;
}
// ----------------------------------------------------------------------------

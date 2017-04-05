/*
	This file is part of Browse Tracker, a plugin for Code::Blocks
	Copyright (C) 2007 Pecan Heber

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
// RCS-ID: $Id$

// Notes:
//
//      There is no way to know when a project is loading. So we have to accept
//      all editors which are activated during the loading process, then remove them
//      after the loading finishes. This gives us editors that are only activated
//      by the user, not by the loading process. Which is what we want.
//      Trying to use OnProjectLoadingHook does not always work because it is
//      not called if the project xml file has no "extensions" entry.
//
//      cbProject::IsLoading/IsLoadingProject is actually turned OFF while editor
//      loading takes place. Very wierd.
//
//  OnProjectOpened
//      Remove loaded editors from our array that the user did not activate.
//      Open the layout file and build an array of old BrowseMarks.
//      These will be used on the first activation of an editor.
//  OnProjectActivated
//      Compress the arrays of pointers so we have more usable slots.
//  OnEditorActivated
//      If we've never seen this editor before, build containers to hold
//      the mouse click (BrowseMarks) locations.
//      Copy the archived layout BrowseMarks to an active BrowseMarks container
//      and have scintilla mark the lines with a "..." icon.
//  OnEditorOpened
//      Add the editor pointer to an array of active editors.
//      Set pointers that track "active" editors & projects.
//  OnEditorClosed
//      Copy the editor BrowseMarks back to the BrowseMark archive so we can
//      use them if the user reopens this editor.
//      Clear this editor out of our containers and arrays. (Except the archive)
//  OnStartShutdown
//      This happens before the editors are actually closed.
//      We simulate closing the editors to force the current Markers into
//      the ProjectData archive markers. Then write a layout
//      file containing the BrowseMarks for each open file.
//      Free the ProjectData container for the current project
//  Containers
//      EbBrowse_MarksHash:      EditorBase*, BrowseMarks*
//      EdBook_MarksHash:        EditorBase*, BrowseMarks*
//      ProjectDataHash:         cbProject*,  ProjectData*
//      ArrayOfEditorBasePtrs:   EditorBase*'s of user activated editors
//      FileBrowse_MarksArchive: EditorBase*, Book_Marks* of archived bookmarks
//                               (layout BrowseMarks, closed editor BrowseMarks etc)
//      FileBook_MarksArchive:   filePath, BrowseMarks*
//      BrowseMarks              wxArray containing editor cursor locations


#if defined(CB_PRECOMP)
#include "sdk.h"
#else
	#include "sdk_events.h"
	#include "manager.h"
	#include "editormanager.h"
	#include "editorbase.h"
	#include "cbeditor.h"
	#include "projectmanager.h"
	#include "cbproject.h"
	#include "configmanager.h"
	#include "logmanager.h"
	#include "cbauibook.h"
	#include "infowindow.h"
#endif
#include "projectloader_hooks.h"
#include "configurationpanel.h"

    #include <wx/dynarray.h> //for wxArray and wxSortedArray
    #include <cbstyledtextctrl.h>
    #include <editor_hooks.h>
    #include "personalitymanager.h"
	#include <wx/stdpaths.h>
	#include <wx/app.h>
	#include <wx/menu.h>
	#include <wx/xrc/xmlres.h>
	#include <wx/fileconf.h>
    #include <wx/aui/auibook.h>

#include "Version.h"
#include "BrowseTracker.h"
#include "BrowseSelector.h"
#include "BrowseMarks.h"
#include "BrowseTrackerDefs.h"
#include "ProjectData.h"
#include "BrowseTrackerConfPanel.h"
#include "JumpTracker.h"

//#define BROWSETRACKER_MARKER        9
//#define BROWSETRACKER_MARKER_STYLE  wxSCI_MARK_DOTDOTDOT
// ----------------------------------------------------------------------------
//  Globals
// ----------------------------------------------------------------------------
        int     gBrowse_MarkerId;       //scintilla marker id
        int     gBrowse_MarkerStyle;    //scintialla marker style
        int     GetBrowseMarkerId(){return gBrowse_MarkerId;}
        int     GetBrowseMarkerStyle(){return gBrowse_MarkerStyle;}

// ----------------------------------------------------------------------------
namespace
// ----------------------------------------------------------------------------
{
    // Register the plugin
    PluginRegistrant<BrowseTracker> reg(_T("BrowseTracker"));

    int idMenuViewTracker           = wxNewId();
    int idMenuTrackerforward        = wxNewId();
    int idMenuTrackerBackward       = wxNewId();
    int idMenuTrackerClear          = wxNewId();
    int idMenuBrowseMarkPrevious    = wxNewId();
    int idMenuBrowseMarkNext        = wxNewId();
    int idMenuRecordBrowseMark      = wxNewId();
    int idMenuClearBrowseMark       = wxNewId();
    int idMenuClearAllBrowse_Marks  = wxNewId();
    int idMenuSortBrowse_Marks      = wxNewId();
    int idMenuConfigBrowse_Marks    = wxNewId();
    int idMenuToggleBrowseMark      = wxNewId();
    #ifdef LOGGING
    int idMenuTrackerDump           = wxNewId();
    #endif
    int idEditBookmarksToggle = XRCID("idEditBookmarksToggle");

    int idToolMarkToggle = XRCID("idMarkToggle");
    int idToolMarkPrev = XRCID("idMarkPrev");
    int idToolMarkNext = XRCID("idMarkNext");
    int idToolMarksClear = XRCID("idMarksClear");
};

// ----------------------------------------------------------------------------
// Event hooks
// ----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(BrowseTracker, cbPlugin)

	EVT_UPDATE_UI(idToolMarkToggle, BrowseTracker::OnUpdateUI)
	EVT_UPDATE_UI(idToolMarkPrev,   BrowseTracker::OnUpdateUI)
	EVT_UPDATE_UI(idToolMarkNext,   BrowseTracker::OnUpdateUI)
	EVT_UPDATE_UI(idToolMarksClear, BrowseTracker::OnUpdateUI)

    EVT_IDLE(                BrowseTracker::OnIdle)
	// --
	// The following replaced by OnMenuTrackerSelect() dialog popup
	//EVT_MENU(     idMenuTrackerBackward,  BrowseTracker::OnMenuTrackBackward)
	//EVT_MENU(     idMenuTrackerforward,  BrowseTracker::OnMenuTrackforward)

	EVT_MENU(     idMenuTrackerBackward,    BrowseTracker::OnMenuTrackerSelect)
	EVT_MENU(     idMenuTrackerforward,     BrowseTracker::OnMenuTrackerSelect)
	EVT_MENU(     idMenuTrackerClear,       BrowseTracker::OnMenuTrackerClear)
	EVT_MENU(     idMenuBrowseMarkPrevious, BrowseTracker::OnMenuBrowseMarkPrevious)
	EVT_MENU(     idMenuBrowseMarkNext,     BrowseTracker::OnMenuBrowseMarkNext)
	EVT_MENU(     idMenuRecordBrowseMark,   BrowseTracker::OnMenuRecordBrowseMark)
	EVT_MENU(     idMenuClearBrowseMark,    BrowseTracker::OnMenuClearBrowseMark)
	EVT_MENU(     idMenuClearAllBrowse_Marks,BrowseTracker::OnMenuClearAllBrowse_Marks)
	EVT_MENU(     idMenuSortBrowse_Marks,    BrowseTracker::OnMenuSortBrowse_Marks)
	EVT_MENU(     idMenuConfigBrowse_Marks,  BrowseTracker::OnMenuSettings)
	EVT_MENU(     idMenuToggleBrowseMark,  BrowseTracker::OnMenuToggleBrowseMark)
   #ifdef LOGGING
	EVT_MENU(     idMenuTrackerDump,        BrowseTracker::OnMenuTrackerDump)
   #endif
   // -- BOOK Marks --
    EVT_MENU(idEditBookmarksToggle, BrowseTracker::OnBook_MarksToggle)

    EVT_TOOL(idToolMarkToggle,  BrowseTracker::OnMenuToggleBrowseMark)
    EVT_TOOL(idToolMarkPrev,    BrowseTracker::OnMenuBrowseMarkPrevious)
    EVT_TOOL(idToolMarkNext,    BrowseTracker::OnMenuBrowseMarkNext)
    EVT_TOOL(idToolMarksClear,  BrowseTracker::OnMenuClearAllBrowse_Marks)

END_EVENT_TABLE()

// ----------------------------------------------------------------------------
BrowseTracker::BrowseTracker()
// ----------------------------------------------------------------------------
{
    //ctor
    //-m_nCurrentEditorIndex = 0;
    m_CurrEditorIndex = 0;
    m_LastEditorIndex = 0;
    m_apEditors.Clear();

    m_bProjectIsLoading = false;
	m_UpdateUIFocusEditor = 0;
    m_nRemoveEditorSentry = 0;
    m_nBrowseMarkPreviousSentry = 0;
    m_nBrowseMarkNextSentry = 0;
    m_nBrowsedEditorCount = 0;

    m_pCfgFile = 0;

    m_MouseDownTime = 0;
    m_ToggleKey = Left_Mouse;
    m_LeftMouseDelay = 200;
    m_ClearAllKey = ClearAllOnSingleClick;
    m_IsMouseDoubleClick = false;
    m_UpdateUIEditorIndex = 0;
    m_pJumpTracker = 0;
    m_bProjectClosing = false;
    m_bAppShutdown = false;
    m_nProjectClosingFileCount = 0;
    m_LastEbDeactivated = 0;

    if (!Manager::LoadResource(_T("BrowseTracker.zip")))
        NotifyMissingFile(_T("BrowseTracker.zip"));
}
// ----------------------------------------------------------------------------
BrowseTracker::~BrowseTracker()
// ----------------------------------------------------------------------------
{
    //dtor
    m_bProjectClosing = false;
    m_pMenuBar = 0;
    m_pToolBar = 0;
}

// ----------------------------------------------------------------------------
void BrowseTracker::OnAttach()
// ----------------------------------------------------------------------------
{
    m_pJumpTracker = new JumpTracker();
    m_pJumpTracker->OnAttach();
    m_pJumpTracker->m_IsAttached = true;

	m_InitDone = false;
	m_CurrEditorIndex = 0;
	m_LastEditorIndex = MaxEntries-1;
    m_apEditors.SetCount(MaxEntries, 0);    //patch 2886
	m_nBrowsedEditorCount = 0;
	m_UpdateUIFocusEditor = 0;
	m_nRemoveEditorSentry = 0;
    m_nBrowseMarkPreviousSentry = 0;
    m_nBrowseMarkNextSentry = 0;
    m_OnEditorEventHookIgnoreMarkerChanges = true; //used to avoid editor hook overhead

    m_LoadingProjectFilename = wxT("");
    m_pEdMgr = Manager::Get()->GetEditorManager();
    m_pPrjMgr = Manager::Get()->GetProjectManager();

    // initialize version and logging
    m_pAppWin  = Manager::Get()->GetAppWindow();
    m_pMenuBar = Manager::Get()->GetAppFrame()->GetMenuBar();

    AppVersion pgmVersion;
    m_AppName = wxT("BrowseTracker");

    #if LOGGING
     wxLog::EnableLogging(true);
     m_pLog = new wxLogWindow( m_pAppWin, _T(" BrowseTracker Plugin"),true,false);
     wxLog::SetActiveTarget( m_pLog);
     m_pLog->GetFrame()->SetSize(20,30,600,300);
     LOGIT( _T("BrowseTracker Plugin Logging Started[%s]"),pgmVersion.GetVersion().c_str());
     m_pLog->Flush();
    #endif

    // Set current plugin version
	PluginInfo* pInfo = (PluginInfo*)(Manager::Get()->GetPluginManager()->GetPluginInfo(this));
	pInfo->version = pgmVersion.GetVersion();

    // ---------------------------------------
    // determine location of settings
    // ---------------------------------------
    //-wxStandardPaths stdPaths;
    // memorize the key file name as {%HOME%}\codesnippets.ini
    //-m_ConfigFolder = stdPaths.GetUserDataDir();
    //-m_ConfigFolder = GetCBConfigDir();
    m_ConfigFolder = Manager::Get()->GetConfigManager(_T("app"))->GetConfigFolder();
    #if defined(LOGGING)
     LOGIT( _T("BT Argv[0][%s] Cwd[%s]"), wxTheApp->argv[0], ::wxGetCwd().GetData() );
    #endif
    m_ExecuteFolder = FindAppPath(wxTheApp->argv[0], ::wxGetCwd(), wxEmptyString);

    // remove the double //s from filename
    m_ConfigFolder.Replace(_T("//"),_T("/"));
    m_ExecuteFolder.Replace(_T("//"),_T("/"));
    #if defined(LOGGING)
        LOGIT(wxT("CfgFolder[%s]"),m_ConfigFolder.c_str());
        LOGIT(wxT("ExecFolder[%s]"),m_ExecuteFolder.c_str());
    #endif

    // get the CodeBlocks "personality" argument
    wxString m_Personality = Manager::Get()->GetPersonalityManager()->GetPersonality();
	if (m_Personality == wxT("default")) m_Personality = wxEmptyString;

    // if codesnippets.ini is in the executable folder, use it
    // else use the default config folder
    m_CfgFilenameStr = m_ExecuteFolder + wxFILE_SEP_PATH;
    if (not m_Personality.IsEmpty()) m_CfgFilenameStr << m_Personality + wxT(".") ;
    m_CfgFilenameStr << m_AppName + _T(".ini");

    if (::wxFileExists(m_CfgFilenameStr)) {;/*OK Use exe path*/}
    else // use the default.conf folder
    {   m_CfgFilenameStr = m_ConfigFolder + wxFILE_SEP_PATH;
        if (not m_Personality.IsEmpty()) m_CfgFilenameStr <<  m_Personality + wxT(".") ;
        m_CfgFilenameStr << m_AppName + _T(".ini");
        // if default doesn't exist, create it
        if (not ::wxDirExists(m_ConfigFolder))
            ::wxMkdir(m_ConfigFolder);
    }
    // ---------------------------------------
    // Initialize Globals
    // ---------------------------------------
    TrackerCfgFullPath = m_CfgFilenameStr;
    #if defined(LOGGING)
     LOGIT( _T("BT TrackerCfgFullPath[%s]"),TrackerCfgFullPath.c_str() );
    #endif

    ReadUserOptions( m_CfgFilenameStr );

    if (m_pJumpTracker)
        m_pJumpTracker->SetWrapJumpEntries(m_WrapJumpEntries);

    switch(m_UserMarksStyle)
    {
        case BrowseMarksStyle:
        {
            gBrowse_MarkerId = BROWSETRACKER_MARKER ;
            gBrowse_MarkerStyle  = BROWSETRACKER_MARKER_STYLE;
            break;
        }
        case BookMarksStyle:
        {
            gBrowse_MarkerId = BOOKMARK_MARKER;
            gBrowse_MarkerStyle  = BOOKMARK_STYLE;
            break;
        }
        case HiddenMarksStyle:
        {
            gBrowse_MarkerId = BROWSETRACKER_MARKER;
            gBrowse_MarkerStyle  = BROWSETRACKER_HIDDEN_STYLE;
            break;
        }
        default:
            break;
    }//switch

	// Hook to plugin events

    // EVT_APP_START_SHUTDOWN
    Manager::Get()->RegisterEventSink(cbEVT_APP_START_SHUTDOWN, new cbEventFunctor<BrowseTracker, CodeBlocksEvent>(this, &BrowseTracker::OnStartShutdown));

    // -- Editor Events
    // EVT_EDITOR_ACTIVATED
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_ACTIVATED, new cbEventFunctor<BrowseTracker, CodeBlocksEvent>(this, &BrowseTracker::OnEditorActivated));
    // EVT_EDITOR_DEACTIVATED
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_DEACTIVATED, new cbEventFunctor<BrowseTracker, CodeBlocksEvent>(this, &BrowseTracker::OnEditorDeactivated));
    // EVT_EDITOR_CLOSE
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_CLOSE, new cbEventFunctor<BrowseTracker, CodeBlocksEvent>(this, &BrowseTracker::OnEditorClosed));
    // EVT_EDITOR_OPEN
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_OPEN, new cbEventFunctor<BrowseTracker, CodeBlocksEvent>(this, &BrowseTracker::OnEditorOpened));

    // -- Project events
    // EVT_PROJECT_OPEN
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_OPEN, new cbEventFunctor<BrowseTracker, CodeBlocksEvent>(this, &BrowseTracker::OnProjectOpened));
    // EVT_PROJECT_CLOSE
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_CLOSE, new cbEventFunctor<BrowseTracker, CodeBlocksEvent>(this, &BrowseTracker::OnProjectClosing));

    // EVT_PROJECT_ACTIVATE
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_ACTIVATE, new cbEventFunctor<BrowseTracker, CodeBlocksEvent>(this, &BrowseTracker::OnProjectActivatedEvent));

    // hook to project loading procedure
    // This hook only occurs if the project has an "extension" xml entry
    ProjectLoaderHooks::HookFunctorBase* myProjhook = new ProjectLoaderHooks::HookFunctor<BrowseTracker>(this, &BrowseTracker::OnProjectLoadingHook);
    m_ProjectHookId = ProjectLoaderHooks::RegisterHook(myProjhook);

    // hook to editors
    EditorHooks::HookFunctorBase* myEdhook = new EditorHooks::HookFunctor<BrowseTracker>(this, &BrowseTracker::OnEditorEventHook);
    m_EditorHookId = EditorHooks::RegisterHook(myEdhook);

}//OnAttach

// ----------------------------------------------------------------------------
void BrowseTracker::OnRelease(bool appShutDown)
// ----------------------------------------------------------------------------
{
    // ------------------------------------------------------------
    // watch out, CodeBlocks can enter this routine multiple times
    // ------------------------------------------------------------

    if (m_pJumpTracker)
    {
        m_pJumpTracker->OnRelease(appShutDown);
        m_pJumpTracker->m_IsAttached = false;
        //-delete m_pJumpTracker; causes crash on CB exit (heap area already freed)
        m_pJumpTracker = 0;
    }

}
// ----------------------------------------------------------------------------
void BrowseTracker::BuildMenu(wxMenuBar* menuBar)
// ----------------------------------------------------------------------------
{
    if (m_pJumpTracker)
        m_pJumpTracker->BuildMenu(menuBar);

    m_pMenuBar = menuBar;

	int idx = menuBar->FindMenu(_("&View"));
	if (idx != wxNOT_FOUND)
	{
		wxMenu* viewMenu = menuBar->GetMenu(idx);

	    wxMenu* pforwardBackwardSubMenu = new wxMenu(wxT(""));
        pforwardBackwardSubMenu->Append(idMenuTrackerBackward, _("Backward Ed\tAlt-Left"), _("Browse Backward"));
        pforwardBackwardSubMenu->Append(idMenuTrackerforward, _("Forward Ed\tAlt-Right"), _("Browse forward"));

        //pforwardBackwardSubMenu->Append(idMenuBrowseMarkPrevious, _("Prev Mark\tAlt-Up"), _("Browse Up"));
        pforwardBackwardSubMenu->Append(idMenuBrowseMarkPrevious, _("Prev Mark"), _("Browse Up"));

        //pforwardBackwardSubMenu->Append(idMenuBrowseMarkNext, _("Next Mark\tAlt-Down"), _("Browse Down"));
        pforwardBackwardSubMenu->Append(idMenuBrowseMarkNext, _("Next Mark"), _("Browse Down"));

        pforwardBackwardSubMenu->AppendSeparator();
        pforwardBackwardSubMenu->Append(idMenuToggleBrowseMark, _("Toggle BrowseMark"), _("Toggle Browse Mark"));
        pforwardBackwardSubMenu->Append(idMenuRecordBrowseMark, _("Set BrowseMark"), _("Record Browse Mark"));
        pforwardBackwardSubMenu->Append(idMenuClearBrowseMark,  _("Clear BrowseMark"), _("Unset Browse Mark"));
        pforwardBackwardSubMenu->Append(idMenuSortBrowse_Marks,  _("Sort BrowseMarks"), _("Sort Browse Marks"));
        pforwardBackwardSubMenu->Append(idMenuClearAllBrowse_Marks,  _("Clear All BrowseMarks"), _("Unset All Browse Marks"));
        pforwardBackwardSubMenu->AppendSeparator();
        pforwardBackwardSubMenu->Append(idMenuTrackerClear,     _("Clear All"), _("Clear History"));
        pforwardBackwardSubMenu->Append(idMenuConfigBrowse_Marks,     _("Settings"), _("Configure"));
       #ifdef LOGGING
        pforwardBackwardSubMenu->Append(idMenuTrackerDump, _("Dump Arrays"), _("Dump Arrays"));
       #endif
        viewMenu->Append(idMenuViewTracker, _("Browse Tracker"), pforwardBackwardSubMenu , _("Browse Tracker"));

	}
    #if defined(LOGGING)
     LOGIT(wxT("Menubar[%p]idMenuViewTracker[%d]"),menuBar, idMenuViewTracker);
    #endif

    idx = menuBar->FindMenu(_("&Edit"));
    if (idx != wxNOT_FOUND)
    {
        wxMenu* editMenu = menuBar->GetMenu(idx);
        idx = editMenu->FindItem(_("&Bookmarks"));
        // main menu will be automatically recreated after plugin disable
        //-editMenu->Destroy(idx);
    }

	m_InitDone = true;

    EditorBase* eb = m_pEdMgr->GetActiveEditor();
    if (eb) {
        CodeBlocksEvent evtea(cbEVT_EDITOR_ACTIVATED, -1, 0, eb);
        //OnEditorActivated(evtea);
    }
}
// ----------------------------------------------------------------------------
void BrowseTracker::BuildModuleMenu(const ModuleType type, wxMenu* popup, const FileTreeData* /*data*/)
// ----------------------------------------------------------------------------
{
	//Some library module is ready to display a pop-up menu.
	//Check the parameter \"type\" and see which module it is
	//and append any items you need in the menu...
	//TIP: for consistency, add a separator as the first item...

    if (not IsAttached() ) return;
    if (type != mtEditorManager) return;

    // obtain ptr to menus menu
    wxMenuBar* pMenuBar = Manager::Get()->GetAppFrame()->GetMenuBar();
    wxMenu* pbtMenu = 0;
    // Ask for the submenu containing the first BrowseTracker menu item
    wxMenuItem* pbtMenuItem = pMenuBar->FindItem(idMenuTrackerforward, &pbtMenu);
    if (not pbtMenuItem) return;

    // Reproduce the BrowseTracker menu onto a Context Menu sub_menu
    int knt = pbtMenu->GetMenuItemCount();
    if (not knt) return;

    wxMenu* sub_menu = new wxMenu;
    // search the BrowseTracker main menu, duplicating each macro item
    // on to a context menu sub-menu without the command keys.
    for (int i=0; i<knt; ++i)
    {
        wxMenuItem* item = pbtMenu->FindItemByPosition(i);
        int menuId = item->GetId();
        #if wxCHECK_VERSION(3, 0, 0)
        wxString menuLabel = item->GetItemLabelText();
        #else
        wxString menuLabel = item->GetLabel();
        #endif
        //LOGIT( _T("BT OnContextMenu insert[%s]"),menuLabel.c_str() );
        wxMenuItem* pContextItem= new wxMenuItem(sub_menu, menuId, menuLabel); //patch 2886
        sub_menu->Append( pContextItem );
    }
    popup->AppendSeparator();
    pbtMenuItem = new wxMenuItem(sub_menu, wxID_ANY, _("Browse Tracker"), _T(""), wxITEM_NORMAL);   //patch 2886
    pbtMenuItem->SetSubMenu(sub_menu);
    popup->Append(pbtMenuItem);

}//BuildModuleMenu
// ----------------------------------------------------------------------------
bool BrowseTracker::BuildToolBar(wxToolBar* toolBar)
// ----------------------------------------------------------------------------
{
    if (m_pJumpTracker) {
        m_pJumpTracker->BuildToolBar(toolBar);
    }

    m_pToolBar = toolBar;

    if (!m_IsAttached || !toolBar)
    {
        return false;
    }
    wxString is16x16 = Manager::isToolBar16x16(toolBar) ? _T("_16x16") : _T("");
    Manager::Get()->AddonToolBar(toolBar, _T("browse_tracker_toolbar") + is16x16);
    m_pToolBar->Realize();

    return true;
}
// ----------------------------------------------------------------------------
int BrowseTracker::Configure()
// ----------------------------------------------------------------------------
{
	if ( !IsAttached() )
		return -1;

	// Creates and displays the configuration dialog
	cbConfigurationDialog dlg(Manager::Get()->GetAppWindow(), wxID_ANY, wxT("BrowseTracker"));
	cbConfigurationPanel* panel = GetConfigurationPanel(&dlg);
	if (panel)
	{
		dlg.AttachConfigurationPanel(panel);
		PlaceWindow(&dlg);
		return dlg.ShowModal() == wxID_OK ? 0 : -1;
	}
	return -1;
}
// ----------------------------------------------------------------------------
cbConfigurationPanel* BrowseTracker::GetConfigurationPanel(wxWindow* parent)
// ----------------------------------------------------------------------------
{
    // Called by plugin manager to show config panel in global Setting Dialog
	if ( !IsAttached() )
		return NULL;

	return new BrowseTrackerConfPanel(*this, parent);
}
// ----------------------------------------------------------------------------
void BrowseTracker::ReadUserOptions(wxString configFullPath)
// ----------------------------------------------------------------------------
{
    if (not m_pCfgFile) m_pCfgFile = new wxFileConfig(
                    wxEmptyString,              // appname
                    wxEmptyString,              // vendor
                    configFullPath,             // local filename
                    wxEmptyString,              // global file
                    wxCONFIG_USE_LOCAL_FILE);
                    //0);
    wxFileConfig& cfgFile = *m_pCfgFile;

	cfgFile.Read( wxT("BrowseMarksEnabled"),        &m_BrowseMarksEnabled, 0 ) ;
	cfgFile.Read( wxT("BrowseMarksStyle"),          &m_UserMarksStyle, 0 ) ;
	cfgFile.Read( wxT("BrowseMarksToggleKey"),      &m_ToggleKey, Left_Mouse ) ;
	cfgFile.Read( wxT("LeftMouseDelay"),            &m_LeftMouseDelay, 200 ) ;
	cfgFile.Read( wxT("BrowseMarksClearAllMethod"), &m_ClearAllKey, ClearAllOnSingleClick ) ;
	cfgFile.Read( wxT("WrapJumpEntries"),           &m_WrapJumpEntries, 0 ) ;

}
// ----------------------------------------------------------------------------
void BrowseTracker::SaveUserOptions(wxString configFullPath)
// ----------------------------------------------------------------------------
{
    if (not m_pCfgFile) m_pCfgFile = new wxFileConfig(
                    wxEmptyString,              // appname
                    wxEmptyString,              // vendor
                    configFullPath,             // local filename
                    wxEmptyString,              // global file
                    wxCONFIG_USE_LOCAL_FILE);
                    //0);
    wxFileConfig& cfgFile = *m_pCfgFile;

	cfgFile.Write( wxT("BrowseMarksEnabled"),       m_BrowseMarksEnabled ) ;
	cfgFile.Write( wxT("BrowseMarksStyle"),         m_UserMarksStyle ) ;
    cfgFile.Write( wxT("BrowseMarksToggleKey"),     m_ToggleKey ) ;
    cfgFile.Write( wxT("LeftMouseDelay"),           m_LeftMouseDelay ) ;
    cfgFile.Write( wxT("BrowseMarksClearAllMethod"),m_ClearAllKey ) ;
	cfgFile.Write( wxT("WrapJumpEntries"),          m_WrapJumpEntries ) ;

    cfgFile.Flush();

}
// ----------------------------------------------------------------------------
wxString BrowseTracker::GetPageFilename(int index)
// ----------------------------------------------------------------------------
{
    // Load BrowseTracker XML history file for this project
    wxString filename = wxEmptyString;
    EditorBase* eb = GetEditor(index);
    if (not eb) return filename;

    EditorManager* EdMgr = Manager::Get()->GetEditorManager();

    if (-1 == EdMgr->FindPageFromEditor(eb) )
    {   // this entry has been closed behind our backs

        ///#if defined(LOGGING)
        /// LOGIT( _T("BT GetEditorFilename Removing ed[%p]"), GetEditor(index) );
        ///#endif
        RemoveEditor( GetEditor(index) );
        return filename;
    }
    filename = eb->GetShortName();
    return filename;
}
// ----------------------------------------------------------------------------
wxString BrowseTracker::GetPageFilename(EditorBase* eb)
// ----------------------------------------------------------------------------
{
    // Ask Notebook for short file name of EditorBase.
    // If no page returned, file is not open

    wxString filename = wxEmptyString;
    if (not eb) return filename;

    if (-1 == Manager::Get()->GetEditorManager()->FindPageFromEditor(eb) )
    {   // this entry has been closed behind our backs
        return filename;
    }

    filename = eb->GetShortName();
    return filename;
}
// ----------------------------------------------------------------------------
int BrowseTracker::GetEditor(EditorBase* eb)
// ----------------------------------------------------------------------------
{
    // return the editor index from our array of user activated editos
    for (int i=0; i<MaxEntries; ++i )
    	if ( m_apEditors[i] == eb ) return i;
    return -1;
}
// ----------------------------------------------------------------------------
EditorBase* BrowseTracker::GetEditor(int index)
// ----------------------------------------------------------------------------
{
    // return the EditorBase* from our array of user activated editors
    return m_apEditors[index];
}
// ----------------------------------------------------------------------------
EditorBase* BrowseTracker::GetCurrentEditor()
// ----------------------------------------------------------------------------
{
    // return the EditorBase* of the currently activated editor
    return GetEditor(m_CurrEditorIndex);
}
// ----------------------------------------------------------------------------
int BrowseTracker::GetCurrentEditorIndex()
// ----------------------------------------------------------------------------
{
    // return the index of the currently activated editor
    if ( GetEditorBrowsedCount() )
        return m_CurrEditorIndex;
    return -1;
}
// ----------------------------------------------------------------------------
EditorBase* BrowseTracker::GetPreviousEditor()
// ----------------------------------------------------------------------------
{
    // return the EditorBase* of the previoiusly user activated editor
    EditorBase* p = 0;
    int index = m_CurrEditorIndex;
    for (int i = 0; i<MaxEntries; ++i)
    {
        --index;
        if (index < 0) index = MaxEntries-1;
    	p = GetEditor(index);
    	if ( p != 0 ) break;
    }
    return p;
}
// ----------------------------------------------------------------------------
int BrowseTracker::GetEditorBrowsedCount()
// ----------------------------------------------------------------------------
{
    #if defined(LOGGING)
    ///LOGIT( _T("BT GetEditorBrowsedCount()[%d]"), m_nBrowsedEditorCount );
    #endif
    return m_nBrowsedEditorCount;
}
// ----------------------------------------------------------------------------
int BrowseTracker::GetPreviousEditorIndex()
// ----------------------------------------------------------------------------
{
    // return the index of the previously user activated editor

    EditorBase* eb = 0;
    int index = m_CurrEditorIndex;
    // scan for previous editor, skipping nulls (null is a closed editors)
    for (int i=0; i<MaxEntries; ++i)
    {
        --index;
        if ( index < 0 ) index = MaxEntries-1;
        eb = GetEditor(index);
        if ( eb ) break;
    }//for

    #if defined(LOGGING)
    /// LOGIT( _T("BT GetPreviousEditorIndex[%d][%p][%s]"), index, eb, eb?eb->GetShortName().c_str():wxEmptyString );
    #endif
    if ( not eb) index = -1;
    return index;
}
// ----------------------------------------------------------------------------
void BrowseTracker::SetSelection(int index)
// ----------------------------------------------------------------------------
{
    // user has selected an editor, make it active

    if ((index < 0) || (index >= MaxEntries )) return;

    EditorBase* eb = GetEditor(index);
    if (eb)
    {
        Manager::Get()->GetEditorManager()->SetActiveEditor(eb);
        #if defined(LOGGING)
        LOGIT( _T("BT SetSelection[%d] editor[%p][%s]"), index, eb, eb->GetShortName().wx_str() );
        #endif

        // Tell OnIdle to focus the new editor. CB sdk editorManager::OnUpdateUI used to
        // do this for us, but something broke it.
        m_UpdateUIFocusEditor = eb;
    }
}
// ----------------------------------------------------------------------------
void BrowseTracker::OnMenuTrackerSelect(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // create a selection popup, allow user to choose an editor to activate

    if ( GetEditorBrowsedCount() == 0) return;

    EditorBase* eb = Manager::Get()->GetEditorManager()->GetActiveEditor();
    cbEditor* cbed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if ((not eb) || (not cbed)) return;

    m_popupWin = new BrowseSelector( wxTheApp->GetTopWindow(), this, event.GetId() == idMenuTrackerforward );
    m_popupWin->ShowModal();
    m_popupWin->Destroy();
    m_popupWin = 0;
    // BrowseSelector returns the index of the selected editor in m_UpdateUIEditorIndex
    // Activate the new editor
    SetSelection( m_UpdateUIEditorIndex );
}
// ----------------------------------------------------------------------------
void BrowseTracker::OnMenuBrowseMarkPrevious(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // For cbEditors, position to previous memorized cursor position

    EditorBase* eb = Manager::Get()->GetEditorManager()->GetActiveEditor();
    cbEditor* cbed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (cbed) do
    {
        if (m_EbBrowse_MarksHash.find(eb) != m_EbBrowse_MarksHash.end() )
        {
            GetCurrentScreenPositions();
            BrowseMarks& EdBrowse_Marks = *m_EbBrowse_MarksHash[eb];
            int newPos = EdBrowse_Marks.GetMarkCurrent();
                //#if defined(LOGGING)
                //LOGIT( _T("BT curPos[%d]m_CurrScrTopPosn[%d]m_CurrScrLastPosn[%d]"),
                //    newPos, m_CurrScrTopPosn, m_CurrScrLastPosn);
                //#endif

            // if current browse mark is off screen, go to "current", not "pevious"
            if ( ((newPos < m_CurrScrTopPosn) || (newPos > m_CurrScrLastPosn))
                && (newPos != -1) )
                /*use current mark*/;
            else newPos = EdBrowse_Marks.GetMarkPrevious();
            if (newPos == -1) break;

            cbStyledTextCtrl* control = cbed->GetControl();
            int line = control->LineFromPosition(newPos);
            if ( LineHasBrowseMarker(control, line) )
            {   // center the BrowseMark line if off screen
                if ( (line < m_CurrScrTopLine) || (line > m_CurrScrLastLine ))
                    cbed->GotoLine(line, true);    // gotoline centers the line on the screen
                control->GotoPos(newPos);          // gotopos puts cursor at correct offset
                GetCurrentScreenPositions();
                #if defined(LOGGING)
                ///LOGIT( _T("BT OnMenuBrowseMarkPrev: pos[%d]line[%d]eb[%p][%s]"),
                ///    newPos, control->LineFromPosition(newPos), eb, eb->GetShortName().c_str() );
                #endif
            }//if
            else
            {
                // rebuild and retry, but guard against any possible loop
                if ( m_nBrowseMarkPreviousSentry++ ) break;
                EdBrowse_Marks.ImportBrowse_Marks(); // Browse marks out of sync
                OnMenuBrowseMarkPrevious( event ); //retry
            }
        }//if
    }while(0);//if do

    m_nBrowseMarkPreviousSentry = 0;

}//OnMenuBrowseMarkPrevious
// ----------------------------------------------------------------------------
void BrowseTracker::OnMenuBrowseMarkNext(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // For cbEditors, position to next memorized cursor position

    EditorBase* eb = Manager::Get()->GetEditorManager()->GetActiveEditor();
    cbEditor* cbed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (cbed) do
    {
        if (m_EbBrowse_MarksHash.find(eb) != m_EbBrowse_MarksHash.end() )
        {
            GetCurrentScreenPositions();
            BrowseMarks& EdBrowse_Marks = *m_EbBrowse_MarksHash[eb];
            int newPos = EdBrowse_Marks.GetMarkCurrent();
            // if current browse mark is off screen, go to "current", not "next"
            if ( ((newPos < m_CurrScrTopPosn) || (newPos > m_CurrScrLastPosn))
                && (newPos != -1) )
                /*use current mark*/;
            else newPos = EdBrowse_Marks.GetMarkNext();
            if (newPos == -1) break;

            cbStyledTextCtrl* control = cbed->GetControl();
            int line = control->LineFromPosition(newPos);
            if ( LineHasBrowseMarker(control, line) )
            {   // center the BrowseMark line if off screen
                if ( (line < m_CurrScrTopLine)  || (line > m_CurrScrLastLine) )
                    cbed->GotoLine(line, true);    // gotoline centers the line on the screen
                control->GotoPos(newPos);          // gotopos puts cursor at correct offset
                GetCurrentScreenPositions();
                #if defined(LOGGING)
                ///LOGIT( _T("BT OnMenuBrowseMarkNext: pos[%d]line[%d]eb[%p][%s]"),
                ///    newPos, control->LineFromPosition(newPos), eb, eb->GetShortName().c_str() );
                #endif
            }
            else
            {
                // rebuild, but guard against any possible loop
                if ( m_nBrowseMarkNextSentry++ ) break;
                EdBrowse_Marks.ImportBrowse_Marks(); // Browse marks out of sync
                OnMenuBrowseMarkNext( event ); // retry
            }
        }//if
    }while(0);//if do

    m_nBrowseMarkNextSentry = 0;

}//OnMenuBrowseMarkNext
// ----------------------------------------------------------------------------
void BrowseTracker::OnMenuRecordBrowseMark(wxCommandEvent& WXUNUSED(event))
// ----------------------------------------------------------------------------
{
    EditorBase* eb = Manager::Get()->GetEditorManager()->GetActiveEditor();
    if (eb) RecordBrowseMark(eb);
}
// ----------------------------------------------------------------------------
void BrowseTracker::OnMenuClearBrowseMark(wxCommandEvent& WXUNUSED(event))
// ----------------------------------------------------------------------------
{
    //LOGIT( _T("BT ClearBrowseMark") );
    bool removeScreenMark = true;
    ClearLineBrowseMark(removeScreenMark);
    if ( GetBrowseMarkerId() == BOOKMARK_MARKER ) ClearLineBookMark();
}
// ----------------------------------------------------------------------------
void BrowseTracker::OnMenuToggleBrowseMark(wxCommandEvent& WXUNUSED(event))
// ----------------------------------------------------------------------------
{
    EditorBase* eb = Manager::Get()->GetEditorManager()->GetActiveEditor();
    cbEditor* cbed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();

    if (cbed) {
        cbStyledTextCtrl* control = cbed->GetControl();
        int line = control->GetCurrentLine();

        if (LineHasBrowseMarker(control, line))
            ClearLineBrowseMark(true);
        else
            RecordBrowseMark(eb);
    }
}
// ----------------------------------------------------------------------------
void BrowseTracker::ClearLineBrowseMark(bool removeScreenMark)
// ----------------------------------------------------------------------------
{
    // clear BrowseMarks for a current line. If the line has no marker
    // clear All markers.

    ///LOGIT( _T("BT ClearBrowseMark") );
    EditorBase* eb = Manager::Get()->GetEditorManager()->GetActiveEditor();
    cbEditor* cbed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(eb);
    if (cbed) do
    {
        if (m_EbBrowse_MarksHash.find(eb) != m_EbBrowse_MarksHash.end() )
        {
            BrowseMarks& EdBrowse_Marks = *m_EbBrowse_MarksHash[eb];
            GetCurrentScreenPositions();
            EdBrowse_Marks.ClearMark( m_CurrScrLineStartPosn, m_CurrScrLineEndPosn);
            cbStyledTextCtrl* control = cbed->GetControl();
            int line = control->LineFromPosition(m_CurrScrLineStartPosn);
            if ( removeScreenMark )
                if ( LineHasBrowseMarker(control,line) )
                    MarkRemove(cbed->GetControl(), line);
            #if defined(LOGGING)
            LOGIT( _T("BT ClearLineBROWSEMark Current Line[%d]"),m_CurrScrLine );
            #endif
        }
    }while(0);
}
////// ----------------------------------------------------------------------------
////void BrowseTracker::ClearLineBrowseMark(int posn)
////// ----------------------------------------------------------------------------
////{
////    // clear BrowseMarks for a single line
////
////    ///LOGIT( _T("BT ClearBrowseMark") );
////    EditorBase* eb = Manager::Get()->GetEditorManager()->GetActiveEditor();
////    cbEditor* cbed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(eb);
////    if (cbed) do
////    {
////        if (m_EbBrowse_MarksHash.find(eb) != m_EbBrowse_MarksHash.end() )
////        {
////            BrowseMarks& EdBrowse_Marks = *m_EbBrowse_MarksHash[eb];
////            cbStyledTextCtrl* control = cbed->GetControl();
////            int line = control->LineFromPosition(posn);
////            int start = control->PositionFromLine( line );
////            int end   = start + control->LineLength( line );
////            EdBrowse_Marks.ClearMark( start, end );
////            if ( LineHasBrowseMarker(control,line) )
////                MarkRemove( control, line);
////            #if defined(LOGGING)
////            LOGIT( _T("BT ClearLineBrowse_MarksByPosn Line[%d]Posn[%d]"), line, posn );
////            #endif
////        }
////    }while(0);
////}
// ----------------------------------------------------------------------------
void BrowseTracker::SetBrowseMarksStyle( int userStyle)
// ----------------------------------------------------------------------------
{
    // BrowseMarks, BookMarks, or Hidden style

    BrowseMarks* pBrowse_Marks = 0;
    for (int i=0; i<MaxEntries ; ++i )
    {
        EditorBase* eb = GetEditor(i);
        if (eb) pBrowse_Marks = GetBrowse_MarksFromHash(  eb);
        if (eb && pBrowse_Marks) pBrowse_Marks->SetBrowseMarksStyle( userStyle);
    }//for
}
// ----------------------------------------------------------------------------
void BrowseTracker::OnMenuSortBrowse_Marks( wxCommandEvent& WXUNUSED(event))
// ----------------------------------------------------------------------------
{
    // sort the BrowseMarks by simply importing them from scintilla

    EditorBase* eb = GetCurrentEditor();
    BrowseMarks* pBrowse_Marks = GetBrowse_MarksFromHash(  eb);
    if (eb && pBrowse_Marks) pBrowse_Marks->ImportBrowse_Marks();
}
// ----------------------------------------------------------------------------
void BrowseTracker::OnMenuSettings( wxCommandEvent& WXUNUSED(event))
// ----------------------------------------------------------------------------
{
    Configure();
}
// ----------------------------------------------------------------------------
void BrowseTracker::OnConfigApply( )
// ----------------------------------------------------------------------------
{
    // Called from OnApply() of BrowseTrackerConfPanel
    // reset options according to user responses

    // Don't allow set and clear_all key to be the same
    if ( (m_ToggleKey == Ctrl_Left_Mouse) && (m_ClearAllKey == ClearAllOnSingleClick) )
    {   wxString msg;
        msg.Printf(_("Program cannot use CTRL-LEFT_MOUSE as both a \nToggle key *AND* a Clear-All-Key"));
        cbMessageBox(msg, _("Error"), wxICON_ERROR);

        m_ClearAllKey = ClearAllOnDoubleClick;
    }

	#if defined(LOGGING)
	LOGIT( _T("BT New Config values: BrowseMarksStyle[%d]ToggleKey[%d]MouseDelay[%d]ClearKey[%d]"),
            m_UserMarksStyle, m_ToggleKey, m_LeftMouseDelay, m_ClearAllKey);
	#endif
	// FIXME (ph#): This may not be working when View/BrowseTracker/Settings used
	// as opposed to Settings/Editor/Browstracker.
    if (m_OldUserMarksStyle not_eq m_UserMarksStyle)
        SetBrowseMarksStyle( m_UserMarksStyle );

    if (m_OldBrowseMarksEnabled not_eq m_BrowseMarksEnabled )
    {
        // Simulate activation of the current editor so mouse
        // events get connected.
        EditorBase* eb = m_pEdMgr->GetBuiltinActiveEditor();
        if ( eb )
        {
            CodeBlocksEvent evt;
            evt.SetEditor(eb);
            OnEditorActivated(evt);
        }
    }
    if (m_pJumpTracker)
    {
        m_pJumpTracker->SetWrapJumpEntries(m_WrapJumpEntries);
    }
}
// ----------------------------------------------------------------------------
void BrowseTracker::OnMenuClearAllBrowse_Marks(wxCommandEvent& WXUNUSED(event))
// ----------------------------------------------------------------------------
{
    ClearAllBrowse_Marks(/*clearScreenMarks*/true);
}
// ----------------------------------------------------------------------------
void BrowseTracker::ClearAllBrowse_Marks(bool clearScreenMarks)
// ----------------------------------------------------------------------------
{
    // Clear every BrowseMark for the currently active editor

    ///LOGIT( _T("BT ClearAllBrowseMark") );
    EditorBase* eb = Manager::Get()->GetEditorManager()->GetActiveEditor();
    cbEditor* cbed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(eb);
    if (cbed) do
    {
        if (m_EbBrowse_MarksHash.find(eb) != m_EbBrowse_MarksHash.end() )
        {
            cbStyledTextCtrl* control = cbed->GetControl();
            BrowseMarks& EdBrowse_Marks = *m_EbBrowse_MarksHash[eb];
            EdBrowse_Marks.ClearAllBrowse_Marks();
            if (clearScreenMarks)
                control->MarkerDeleteAll( GetBrowseMarkerId());
            #if defined(LOGGING)
            LOGIT( _T("BT ClearAllBrowseMarks()") );
            #endif
        }
        // When using Book marks as Browse marks, clear book marks also
        if ( GetBrowseMarkerId() == BOOKMARK_MARKER )
        if (m_EdBook_MarksHash.find(eb) != m_EdBook_MarksHash.end() )
        {
            cbStyledTextCtrl* control = cbed->GetControl();
            BrowseMarks& EdBook_Marks = *m_EdBook_MarksHash[eb];
            EdBook_Marks.ClearAllBrowse_Marks();
            if (clearScreenMarks) control->MarkerDeleteAll( GetBrowseMarkerId());
            #if defined(LOGGING)
            LOGIT( _T("BT ClearAllBookMarks()") );
            #endif
        }
    }while(0);
}
// ----------------------------------------------------------------------------
void BrowseTracker::GetCurrentScreenPositions()
// ----------------------------------------------------------------------------
{
    // Update copy of current editor screen data

    EditorBase* eb = Manager::Get()->GetEditorManager()->GetActiveEditor();
    cbEditor* cbed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(eb);
    if (cbed) do
    {
        if (m_EbBrowse_MarksHash.find(eb) != m_EbBrowse_MarksHash.end() )
        {
            //-BrowseMarks& EdBrowse_Marks = *m_EbBrowse_MarksHash[eb];
            cbStyledTextCtrl* control = cbed->GetControl();
            m_CurrScrPosn = control->GetCurrentPos();
            //-if (curPos == -1) break;

            m_CurrScrLine       = control->LineFromPosition(m_CurrScrPosn);
            m_CurrScrTopLine    = control->GetFirstVisibleLine();
            m_CurrLinesOnScreen = control->LinesOnScreen();
            m_CurrScrLastLine   = m_CurrScrTopLine + m_CurrLinesOnScreen;

            m_CurrScrTopPosn    = control->PositionFromLine(m_CurrScrTopLine);
            m_CurrScrLastPosn   = control->PositionFromLine(m_CurrScrLastLine);
            // Lines might not fill the screen.
            if (m_CurrScrLastPosn == -1)
                m_CurrScrLastPosn = control->PositionFromLine(control->GetLineCount());

            m_CurrScrLineStartPosn  = control->PositionFromLine(m_CurrScrLine);
            m_CurrScrLineLength     = control->LineLength(m_CurrScrLine);
            m_CurrScrLineEndPosn    = m_CurrScrLineStartPosn + m_CurrScrLineLength;

            #if defined(LOGGING)
                //LOGIT( _T("BT UpdateCurrent: CurentLine[%d]TopLine[%d]ScrnLines[%d]LastLine[%d]"),
                //    m_CurrScrLine, m_CurrScrTopLine, m_CurrLinesOnScreen, m_CurrScrLastLine );
                //LOGIT( _T("BT UpdateCurrent: CurrPosn[%d]TopPosn[%d]LastPosn[%d]"),
                //    m_CurrScrPosn, m_CurrScrTopPosn, m_CurrScrLastPosn );
            #endif
        }//if
    }while(0);//if do
}
// ----------------------------------------------------------------------------
void BrowseTracker::OnMouseKeyEvent(wxMouseEvent& event)
// ----------------------------------------------------------------------------
{
    // Record the cursor position when user holds down the left mouse key;
    // If user is also holding down the control key, clear the browse marks
    // on this line.

    if ( (not IsAttached()) || (not m_InitDone) )
        { event.Skip(); return; }
    if (not IsBrowseMarksEnabled() )
        { event.Skip(); return; }

    if (   ( event.GetEventType() ==  wxEVT_LEFT_UP)
        || ( event.GetEventType() ==  wxEVT_LEFT_DOWN)
        || ( event.GetEventType() ==  wxEVT_LEFT_DCLICK)
        || ( event.GetEventType() ==  wxEVT_MOTION)
    ) do
    {
        // -- MOUSE_MOTION -------------------------------------------
        if (event.GetEventType() == wxEVT_MOTION)
        {
            // ignore dragging events even if key is held down
            if (event.LeftIsDown() && event.Dragging())
            {   //allow nervous movement if not really dragging
                if ( (abs(event.GetX() - m_MouseXPosn) >3)
                  or (abs(event.GetY() - m_MouseYPosn) >3) )
                m_IsMouseDoubleClick = true;
            }
            break;
        }
        // Skip unmonitored editors
        EditorBase* eb = m_pEdMgr->GetActiveEditor();
        if (m_EbBrowse_MarksHash.find(eb) == m_EbBrowse_MarksHash.end() )
            break;
        cbEditor* cbed = m_pEdMgr->GetBuiltinEditor(eb);
        if (not cbed) break;
        cbStyledTextCtrl* pControl = cbed->GetControl();

        // -- MOUSE_KEY_DOWN --------------------------------------------
        if (event.GetEventType() == wxEVT_LEFT_DOWN)
        {   //Record the mouse down time
            #if defined(LOGGING)
            ////LOGIT( _T("BT Mouse DOWN") );
            #endif
            m_MouseDownTime = ::wxGetLocalTimeMillis();
            m_MouseXPosn = event.GetX(); m_MouseYPosn = event.GetY();
            // A double click substitutes for the Mouse_Down event
            // so we can clear it here.
            m_IsMouseDoubleClick = false;
            break;
        }

        // -- MOUSE_KEY_DCLICK ------------------------------------
        if (event.GetEventType() == wxEVT_LEFT_DCLICK)
        {   // Tell wxEVT_LEFT_UP about double clicks
            #if defined(LOGGING)
            ////LOGIT( _T("BT Double Click") );
            #endif
            m_IsMouseDoubleClick = true;
            break;
        }

        // -- MOUSE_KEY_UP ----------------------------------------
        if (event.GetEventType() == wxEVT_LEFT_UP)
        {   // we're monitoring this editor, record/clear this cursor position
            #if defined(LOGGING)
            ////LOGIT( _T("BT Mouse UP") );
            #endif

            bool ctrlKeyIsDown          = ::wxGetMouseState().ControlDown();
            bool useOnlyLeftMouse       = (m_ToggleKey == Left_Mouse);
            bool useCtrlLeftMouse       = (m_ToggleKey == Ctrl_Left_Mouse);
            bool clearUsesDoubleClick   = (m_ClearAllKey == ClearAllOnDoubleClick);
            bool clearUsesSingleClick   = (m_ClearAllKey == ClearAllOnSingleClick);
           //bool bEdMultiSelOn = Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/selection/multi_select"), false);
           bool bEdMultiSelOn = pControl->GetMultipleSelection();

            // -- ONLY_LEFT_MOUSE --
            if ( useOnlyLeftMouse )
            {
                if( ctrlKeyIsDown && bEdMultiSelOn)
                    break;
                if( ctrlKeyIsDown && clearUsesDoubleClick && m_IsMouseDoubleClick)
                {   //Clear all on Ctrl Double Click
                    ClearAllBrowse_Marks(/*clearScreenMarks*/true);
                    m_IsMouseDoubleClick = false;
                    pControl->SetSelectionVoid (-1, pControl->GetCurrentPos()); //clear selection
                    break;
                }
                if (ctrlKeyIsDown && clearUsesSingleClick)
                {   //Clear all on Ctrl Single Click
                    ClearAllBrowse_Marks(/*clearScreenMarks*/true);
                    break;
                }
                if ( ctrlKeyIsDown) break; //This is useOnlyLeftMouse w/o ctrl
                if ( m_IsMouseDoubleClick) break;
                wxLongLong mouseDwellMillisecs = ::wxGetLocalTimeMillis() - m_MouseDownTime;
                if (mouseDwellMillisecs >= m_LeftMouseDelay)
                        RecordBrowseMark(eb);
                break;
            }//if useOnlyLeftMouse

            // -- CTRL-LEFT_MOUSE --
            // Don't intercept Ctrl key if it belongs to editor multi-selection mechanism
            if (bEdMultiSelOn) break;
            if ( useCtrlLeftMouse && ctrlKeyIsDown)
            {   if( clearUsesDoubleClick && m_IsMouseDoubleClick)
                {   //Clear all on Ctrl Double Click
                    ClearAllBrowse_Marks(/*clearScreenMarks*/true);
                    m_IsMouseDoubleClick = false;
                    pControl->SetSelectionVoid (-1, pControl->GetCurrentPos()); //clear selection
                    break;
                }
                RecordBrowseMark(eb);
                break;
            }//if useCtrlLeftMouse
        }//if MOUSE_KEY_UP
    }while(0);

    event.Skip();

}//OnMouseKeyEvent
// ----------------------------------------------------------------------------
//  BROWSETRACKER_MARKER scintilla test/set/unset marker routines
// ----------------------------------------------------------------------------
bool BrowseTracker::LineHasBookMarker(cbStyledTextCtrl* pControl, int line) const
{
    if (line == -1)
        line = pControl->GetCurrentLine();
    //-return pControl->MarkerGet(line) & (1 << BOOKMARK_MARKER);
    return pControl->MarkerGet(line) & (1 << GetBrowseMarkerId());
}
// ----------------------------------------------------------------------------
bool BrowseTracker::LineHasBrowseMarker(cbStyledTextCtrl* pControl, int line) const
{
    if (line == -1)
        line = pControl->GetCurrentLine();
    //-return pControl->MarkerGet(line) & (1 << BROWSETRACKER_MARKER);
    return pControl->MarkerGet(line) & (1 << GetBrowseMarkerId());
}
// ----------------------------------------------------------------------------
void BrowseTracker::MarkerToggle(cbStyledTextCtrl* pControl, int line)
{
    if (line == -1)
        line = pControl->GetCurrentLine();
    if (LineHasBrowseMarker(pControl, line))
        //-pControl->MarkerDelete(line, BROWSETRACKER_MARKER);
        pControl->MarkerDelete(line, GetBrowseMarkerId());
    else
    //-pControl->MarkerAdd(line, BROWSETRACKER_MARKER);
    pControl->MarkerAdd(line, GetBrowseMarkerId());
}
// ----------------------------------------------------------------------------
void BrowseTracker::MarkerNext(cbStyledTextCtrl* pControl)
{
    int line = pControl->GetCurrentLine() + 1;
    //-int newLine = pControl->MarkerNext(line, 1 << BROWSETRACKER_MARKER);
    int newLine = pControl->MarkerNext(line, 1 << GetBrowseMarkerId());
    if (newLine != -1)
        pControl->GotoLine(newLine);
}
// ----------------------------------------------------------------------------
void BrowseTracker::MarkerPrevious(cbStyledTextCtrl* pControl)
{
    int line = pControl->GetCurrentLine() - 1;
    //-int newLine = pControl->MarkerPrevious(line, 1 << BROWSETRACKER_MARKER);
    int newLine = pControl->MarkerPrevious(line, 1 << GetBrowseMarkerId());
    if (newLine != -1)
        pControl->GotoLine(newLine);
}
// ----------------------------------------------------------------------------
void BrowseTracker::MarkLine(cbStyledTextCtrl* pControl, int line)
{
    if (line == -1)
        //-pControl->MarkerDeleteAll(BROWSETRACKER_MARKER);
        pControl->MarkerDeleteAll(GetBrowseMarkerId());
    else
        //-pControl->MarkerAdd(line, BROWSETRACKER_MARKER);
        pControl->MarkerAdd(line, GetBrowseMarkerId());
}
// ----------------------------------------------------------------------------
void BrowseTracker::MarkRemove(cbStyledTextCtrl* pControl, int line)
{
    if (line == -1)
        line = pControl->GetCurrentLine();
    if (LineHasBrowseMarker(pControl, line))
        //-pControl->MarkerDelete(line, BROWSETRACKER_MARKER);
        pControl->MarkerDelete(line, GetBrowseMarkerId());
}
// ----------------------------------------------------------------------------
void BrowseTracker::RecordBrowseMark(EditorBase* eb)
// ----------------------------------------------------------------------------
{
    // stow a browse mark by EditorBase current line

    cbEditor* cbed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(eb);
    if (cbed) do
    {
        if (m_EbBrowse_MarksHash.find(eb) != m_EbBrowse_MarksHash.end() )
        {
            cbStyledTextCtrl* control = cbed->GetControl();
            BrowseMarks& EdBrowse_Marks = *m_EbBrowse_MarksHash[eb];
            GetCurrentScreenPositions();
            // Toggle BrowseMark
            if ( LineHasBrowseMarker(control, m_CurrScrLine) )
            {
                ClearLineBrowseMark(/*removeScreenmark*/true); //clear previous marks
                if ( GetBrowseMarkerId() == BOOKMARK_MARKER ) ClearLineBookMark();
                return;
            }
            int pos = control->GetCurrentPos();
            EdBrowse_Marks.RecordMark(pos);
            MarkLine( control, m_CurrScrLine);
            #if defined(LOGGING)
            LOGIT( _T("BT RecordBrowseMarkByEb: pos[%d]line[%d]eb[%p][%s]"),
                pos, m_CurrScrLine, eb, eb->GetShortName().c_str() );
            if (not LineHasBrowseMarker(control, m_CurrScrLine)  ) {asm("int3"); /*trap*/}
            #endif
        }//if
    }while(0);//if do
}//RecordBrowseMark
////// ----------------------------------------------------------------------------
////void BrowseTracker::RecordBrowseMarkPosition(EditorBase* eb, int posn)
////// ----------------------------------------------------------------------------
////{
////    // stow a browse mark by editor cursor position
////
////    cbEditor* cbed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(eb);
////    if (cbed) do
////    {
////        if (m_EbBrowse_MarksHash.find(eb) != m_EbBrowse_MarksHash.end() )
////        {
////            cbStyledTextCtrl* control = cbed->GetControl();
////            BrowseMarks& EdBrowse_Marks = *m_EbBrowse_MarksHash[eb];
////            int line = control->LineFromPosition(posn);
////            ClearLineBrowseMark( posn ); //clear previous marks
////            EdBrowse_Marks.RecordMark( posn );
////            //-if (not LineHasMarker( control,line))
////                MarkLine( control, line);
////            #if defined(LOGGING)
////            LOGIT( _T("BT RecordBrowseMarkByPosn: pos[%d]line[%d]eb[%p][%s]"),
////                posn, line,eb, eb->GetShortName().c_str() );
////            if (not LineHasBrowseMarker(control, line)) {asm("int3"); /*trap*/}
////            #endif
////        }//if
////    }while(0);//if do
////}//RecordBrowseMarkPosition
// ----------------------------------------------------------------------------
void BrowseTracker::OnMenuTrackerDump(wxCommandEvent& WXUNUSED(event))
// ----------------------------------------------------------------------------
{
    // For debugging. Dump arrays and pointers

   #ifdef LOGGING
        LOGIT( _T("BT --Browsed--Editors-------------") );
        LOGIT( _T("BT CurrIndex[%d]LastIndex[%d]count[%d]"), m_CurrEditorIndex, m_LastEditorIndex, GetEditorBrowsedCount() );
        for (int i=0;i<MaxEntries ;++i )
        {
            wxString edName = GetPageFilename(i);
            LOGIT( _T("BT Index[%d]Editor[%p]Name[%s]"), i, GetEditor(i), edName.wx_str()  );;
        }
        return; //FIXME: remove this line to get rest of diagnostics
        for (EbBrowse_MarksHash::iterator it = m_EbBrowse_MarksHash.begin(); it != m_EbBrowse_MarksHash.end(); ++it)
        {
            LOGIT( _T("BT Hash Ed[%p] AryPtr[%p]"), it->first, it->second );
        }

        // dump the array containing the current editors BrowseMarks
        EditorBase* eb = Manager::Get()->GetEditorManager()->GetActiveEditor();
        cbEditor* cbed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
        if (cbed) do
        {
            if (m_EbBrowse_MarksHash.find(eb) != m_EbBrowse_MarksHash.end() )
            {
                //cbStyledTextCtrl* control = cbed->GetControl();
                BrowseMarks& EdBrowse_Marks = *m_EbBrowse_MarksHash[eb];
                LOGIT( _T("BT --Browse--Marks--for--[%s]----"), eb->GetShortName().c_str() );
                EdBrowse_Marks.Dump();
            }//if
        }while(0);//if do
        // dump the current Project ProjectData
        ProjectData* pProjectData = GetProjectDataByEditorName( eb->GetFilename() );
        if (not pProjectData)
        {
            LOGIT( _T("BT *CRASH* BrowseTracker::OnMenuTrackerDump No project pointer") );
            return;
        }
        LOGIT( _T("BT ProjectData for[%s]"),pProjectData->GetProjectFilename().c_str() );
        pProjectData->DumpHash(wxT("BrowseMarks"));
        pProjectData->DumpHash(wxT("BookMarks"));
   #endif
}

// ----------------------------------------------------------------------------
void BrowseTracker::OnMenuTrackerClear(wxCommandEvent& WXUNUSED(event))
// ----------------------------------------------------------------------------
{
    // Clear the editor array of pointers (History)

    if (IsAttached() && m_InitDone)
    {
        TrackerClearAll();
    }
}
// ----------------------------------------------------------------------------
void BrowseTracker::TrackerClearAll()
// ----------------------------------------------------------------------------
{
    // Clear the editor array of pointers (History)

    if (IsAttached() && m_InitDone)
    {
        for (int i=0; i<MaxEntries ; ++i )
            RemoveEditor(GetEditor(i));
        m_CurrEditorIndex = 0;
        m_LastEditorIndex = MaxEntries-1;
    }

    // Simulate activation of the current editor. If the list is empty
    // it'll hang the selector dialog
    EditorBase* eb = m_pEdMgr->GetBuiltinActiveEditor();
    if ( eb )
    {
        CodeBlocksEvent evt;
        evt.SetEditor(eb);
        OnEditorActivated(evt);
    }

}
// ----------------------------------------------------------------------------
cbProject* BrowseTracker::GetProject(EditorBase* eb)
// ----------------------------------------------------------------------------
{
    cbEditor* cbed = m_pEdMgr->GetBuiltinEditor(eb);
    if ( not cbed ) {return 0;}
    ProjectFile* pPrjFile = cbed->GetProjectFile();
    if ( not pPrjFile ) {return 0;}
    cbProject* pProject = pPrjFile->GetParentProject();
    return pProject;
}
// ----------------------------------------------------------------------------
void BrowseTracker::OnEditorActivated(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    // Record this activation event and place activation in history
    // Create structures to hold new editor info if we never saw this editor before.
    // Structures are: a hash to point to a class holding editor cursor postiions used
    // as a history to place markers.

    event.Skip();

    if (IsAttached() && m_InitDone) do
    {

        EditorBase* eb = event.GetEditor();
        wxString editorFullPath = eb->GetFilename();
        cbEditor* cbed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(eb);

        if ( m_bProjectIsLoading )
        {
            #if defined(LOGGING)
            LOGIT( _T("BT OnEditorActivated ignored: Project Loading[%s]"), editorFullPath.c_str());
            #endif
             return;
        }
        if ( m_bProjectClosing )
        {
            #if defined(LOGGING)
            LOGIT( _T("BT OnEditorActivated ignored: Project Closing[%s]"), editorFullPath.c_str());
            #endif
             return;
        }

        if (not cbed)
        {
            // Since wxAuiNotebook added, there's no cbEditor associated during
            // an initial cbEVT_EDITOR_ACTIVATED event. So we ignore the inital
            // call and get OnEditorOpened() to re-issue OnEditorActivated() when
            // it does have a cbEditor, but no cbProject associated;
            #if defined(LOGGING)
            LOGIT( _T("BT [OnEditorActivated ignored:no cbEditor[%s]"), editorFullPath.c_str());
            #endif
            return;
        }

        #if defined(LOGGING)
        cbProject* pcbProject = GetProject( eb );
        LOGIT( _T("BT Editor Activated[%p]proj[%p][%s]"), eb, pcbProject, eb->GetShortName().c_str() );
        #endif


        // New editor, append to circular queue
        // remove previous entries for this editor first
        for (int i=0; i < MaxEntries; ++i)
            if (eb == GetEditor(i)) ClearEditor(i);
        // compress the array
        if ( GetEditorBrowsedCount() )
            for (int i=0; i < MaxEntries-1; ++i)
            {
                if (m_apEditors[i] == 0)
                {   m_apEditors[i] = m_apEditors[i+1];
                    m_apEditors[i+1] = 0;
                    if ( m_CurrEditorIndex == (i+1) ) --m_CurrEditorIndex;
                    if ( m_LastEditorIndex == (i+1) ) --m_LastEditorIndex;
                }
            }
        AddEditor(eb);
        #if defined(LOGGING)
        LOGIT( _T("BT OnEditorActivated AddedEditor[%p]proj[%p][%s]"), eb, GetProject(eb),eb->GetShortName().c_str() );
        #endif
        m_CurrEditorIndex = m_LastEditorIndex;

        // ---------------------------------------------------------------------
        // For new cbEditors, add an entry to the editor cursor positions hash
        // and allocate a cursor positions array to hold the cursor positions.
        // Set hooks to catch mouse activity
        // ---------------------------------------------------------------------

        if ( IsBrowseMarksEnabled()
            && (not GetBrowse_MarksFromHash( eb )) ) //guard against duplicates
        {   // new editor
            if (cbed)
            {
                HashAddBrowse_Marks( eb->GetFilename() ); //create hashs and book/browse marks arrays

                // Debugging statements
                ////DumpHash(wxT("BrowseMarks"));
                ////DumpHash(wxT("BookMarks"));
                ////m_pActiveProjectData->DumpHash(wxT("BrowseMarks"));
                ////m_pActiveProjectData->DumpHash(wxT("BookMarks"));

                cbStyledTextCtrl* control = cbed->GetControl();
                // Setting the initial browsemark
                //-int pos = control->GetCurrentPos();
                //Connect to mouse to see user setting/clearing browse marks
                control->Connect(wxEVT_LEFT_UP,
                                (wxObjectEventFunction)(wxEventFunction)
                                (wxMouseEventFunction)&BrowseTracker::OnMouseKeyEvent,
                                 NULL, this);
                control->Connect(wxEVT_LEFT_DOWN,
                                (wxObjectEventFunction)(wxEventFunction)
                                (wxMouseEventFunction)&BrowseTracker::OnMouseKeyEvent,
                                 NULL, this);
                control->Connect(wxEVT_LEFT_DCLICK,
                                (wxObjectEventFunction)(wxEventFunction)
                                (wxMouseEventFunction)&BrowseTracker::OnMouseKeyEvent,
                                 NULL, this);
                control->Connect(wxEVT_MOTION,
                                (wxObjectEventFunction)(wxEventFunction)
                                (wxMouseEventFunction)&BrowseTracker::OnMouseKeyEvent,
                                 NULL, this);
                control->Connect(wxEVT_CONTEXT_MENU,
                                (wxObjectEventFunction)(wxEventFunction)
                                (wxContextMenuEventFunction)&BrowseTracker::OnMarginContextMenu,
                                 NULL, this);
                #if defined(LOGGING)
                LOGIT( _T("BT Added hash entry for [%p][%s]"), eb, eb->GetShortName().c_str() );
                #endif
                // Define scintilla BrowseTracker margin marker
                //ConfigManager* mgr = Manager::Get()->GetConfigManager(_T("app"));
                //control->SetMarginWidth(1, 16);
                //control->SetMarginType(1, wxSCI_MARGIN_SYMBOL);
                //control->SetMarginSensitive(1, mgr->ReadBool(_T("/margin_1_sensitive"), true));
                //control->SetMarginMask(1, (1 << BOOKMARK_MARKER) |
                //                             (1 << BREAKPOINT_MARKER) |
                //                             (1 << DEBUG_MARKER) |
                //                             (1 << ERROR_MARKER));
                //control->MarkerDefine(BOOKMARK_MARKER, BOOKMARK_STYLE);
                //control->MarkerSetBackground(BOOKMARK_MARKER, wxColour(0xA0, 0xA0, 0xFF));
                int marginMask = control->GetMarginMask(1);
                control->SetMarginMask( 1, marginMask | (1<<GetBrowseMarkerId()) );
                control->MarkerDefine( GetBrowseMarkerId(), GetBrowseMarkerStyle() );
                // the following stmt seems to do nothing for wxSCI_MARK_DOTDOTDOT
                control->MarkerSetBackground( GetBrowseMarkerId(), wxColour(0xA0, 0xA0, 0xFF));
                #if defined(LOGGING)
                 //LOGIT( _T("BT UserStyle[%d]MarkerId[%d]MarkerStyle[%d]"),m_UserMarksStyle,GetBrowseMarkerId(), GetBrowseMarkerStyle());
                #endif
                // Set archived Layout browse marks in the editor
                ProjectData* pProjectData = GetProjectDataByEditorName(eb->GetFilename() );
                    #if defined(LOGGING)
                    if (not pProjectData)
                        // Since wxAuiNotebook added, there's no proj associated with cbeditor
                        // during EVT_EDITOR_OPEN or EVT_EDITOR_ACTIVATED
                        LOGIT( _T("BT OnEditorActivated FAILED TO FIND PROJECT for [%s]"), eb->GetShortName().c_str() );
                    #endif
                if ( pProjectData )
                {   // Set the Book/Browse marks from the Layout/History arrays
                    BrowseMarks* pBrowse_MarksArc = pProjectData->GetBrowse_MarksFromHash( eb->GetFilename());
                        //#if defined(LOGGING)
                        //LOGIT( _T("BT \nDumping ARCHIVE data for[%s]"), eb->GetFilename().c_str());
                        //LOGIT( _T("BT Project Data[%s]"),pProjectData->GetProjectFilename().c_str() );
                        //pBrowse_MarksArc->Dump();
                        //#endif
                    if (pBrowse_MarksArc)
                        m_EbBrowse_MarksHash[eb]->RecordMarksFrom( *pBrowse_MarksArc);
                            //LOGIT( _T("BT Dumping CURRENT data for[%s]"), eb->GetFilename().c_str());
                            //m_EbBrowse_MarksHash[eb]->Dump();

                    // record the current cursor position
                    //-if (not LineHasBrowseMarker(control, control->LineFromPosition(pos)))
                    //-if ( not m_EbBrowse_MarksHash[eb]->GetMarkCount() )
                    //-    RecordBrowseMark(eb);

                        ////DumpHash(wxT("BrowseMarks"));
                        ////DumpHash(wxT("BookMarks"));
                        ////m_pActiveProjectData->DumpHash(wxT("BrowseMarks"));
                        ////m_pActiveProjectData->DumpBrowse_Marks(wxT("BrowseMarks"));
                        ////pBrowse_MarksArc->Dump();
                        ////m_pActiveProjectData->DumpHash(wxT("BookMarks"));

                    // copy/set the old book marks, if any
                    BrowseMarks* pCurrBook_Marks = GetBook_MarksFromHash( eb->GetFilename());
                    BrowseMarks* pArchBook_Marks = pProjectData->GetBook_MarksFromHash(eb->GetFilename());
                    if (pArchBook_Marks && pCurrBook_Marks)
                        pCurrBook_Marks->ToggleBook_MarksFrom(*pArchBook_Marks);
                }//if project

            }//if cbed
        }//if new editor

        // Cause editor to be focused and browse marks sorted in OnIdle()
        m_UpdateUIFocusEditor = eb;

    }while(0);

}//OnEditorActivated
// ----------------------------------------------------------------------------
void BrowseTracker::OnUpdateUI(wxUpdateUIEvent& event)
// ----------------------------------------------------------------------------
{
    if (Manager::Get()->IsAppShuttingDown())
        return;

    EditorBase* eb = Manager::Get()->GetEditorManager()->GetActiveEditor();
    if (m_EbBrowse_MarksHash.find(eb) != m_EbBrowse_MarksHash.end())
    {
        int count = m_EbBrowse_MarksHash[eb]->GetMarkCount();

        m_pToolBar->EnableTool(idToolMarkToggle, true);
        m_pToolBar->EnableTool(idToolMarkNext, count > 0);
        m_pToolBar->EnableTool(idToolMarkPrev, count > 0);
        m_pToolBar->EnableTool(idToolMarksClear, count > 0);

    }
    else // This editor not recorded
    {
        m_pToolBar->EnableTool(idToolMarkToggle, true);
        m_pToolBar->EnableTool(idToolMarkNext, false);
        m_pToolBar->EnableTool(idToolMarkPrev, false);
        m_pToolBar->EnableTool(idToolMarksClear, false);
    }

    event.Skip();
}
// ----------------------------------------------------------------------------
void BrowseTracker::OnIdle(wxIdleEvent& event)
// ----------------------------------------------------------------------------
{
     event.Skip();

    // Focus the new selected editor. This doesn't work if a long compile
    // is active since there's no idle time. User will have to click into
    // the editor window to activate it.
    // This used to be done by the CB editor manager, but someone removed the UI hook.
    if (m_bAppShutdown)
        return;
    if ((not Manager::Get()->IsAppShuttingDown()) && m_UpdateUIFocusEditor)
    {
        if (m_UpdateUIFocusEditor)
        {
            EditorBase* eb = m_UpdateUIFocusEditor;
            m_UpdateUIFocusEditor = 0;
            if (not eb) return;
            if (not IsEditorBaseOpen(eb)) return;
            if( Manager::Get()->GetEditorManager()->GetActiveEditor() not_eq eb )
            {   Manager::Get()->GetEditorManager()->SetActiveEditor(eb);
                eb->SetFocus();
                #if defined(LOGGING)
                LOGIT( _T("BT OnIdle Focused Editor[%p] Title[%s]"), eb, eb->GetTitle().c_str() );
                #endif
            }
            // re-sort the browse marks
            wxCommandEvent ev;
            OnMenuSortBrowse_Marks(ev);
        }
    }
}//OnIdle
// ----------------------------------------------------------------------------
void BrowseTracker::OnStartShutdown(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    #if defined(LOGGING)
    //wxMessageBox(_T("BrowseTracker: CB initiated OnStartShutdown"));
    LOGIT( _T("BT BrowseTracker: CB initiated OnStartShutdown"));
    #endif
    Manager::Get()->GetLogManager()->Log(_T("BrowseTracker OnStartShutdown() initiated."));
    event.Skip();

    m_bAppShutdown = true;
    #if defined(LOGGING)
    InfoWindow::Display(_T("Browstracker"),_T("Browstracker OnStartShutdown"), 7000);
    #endif

    if ( m_InitDone )
    {
        Manager::Get()->GetLogManager()->Log(_T("BrowseTracker Released"));

        //*SDK Gotcha* A cbEVT_PROJECT_CLOSE is issued, but only
        // after the plugin OnRelease() is called. So we
        // simulate closing all open projects in order to save
        // current Browse/Book marks in layout
        ProjectsArray* pPrjs = m_pPrjMgr->GetProjects();
        for (size_t i=0; i<pPrjs->GetCount(); ++i )
        {
            CodeBlocksEvent evtpa(cbEVT_PROJECT_ACTIVATE);
            evtpa.SetProject(pPrjs->Item(i));
            OnProjectActivatedEvent(evtpa);

            CodeBlocksEvent evtpc(cbEVT_PROJECT_CLOSE);
            evtpc.SetProject(pPrjs->Item(i));
            OnProjectClosing(evtpc);
        }

        // remove project load/save hook
        ProjectLoaderHooks::UnregisterHook(m_ProjectHookId, true);

        // remove editor/scintilla hook
        EditorHooks::UnregisterHook(m_EditorHookId, true);

        //  Remove menu item
        int idx = m_pMenuBar->FindMenu(_("&View"));
        if (idx != wxNOT_FOUND)
        {
            wxMenu* viewMenu = m_pMenuBar->GetMenu(idx);
            viewMenu->Destroy(idMenuViewTracker);
        }

        // *Book_Marks* release the editor hash table ptrs to Book_Marks
        for (EbBrowse_MarksHash::iterator it = m_EdBook_MarksHash.begin(); it != m_EdBook_MarksHash.end(); ++it)
        {
            delete it->second;
        }
        m_EdBook_MarksHash.clear();

        // *BrowseMarks* release the editor hash table ptrs to BrowseMarks
        for (EbBrowse_MarksHash::iterator it = m_EbBrowse_MarksHash.begin(); it != m_EbBrowse_MarksHash.end(); ++it)
        {
            delete it->second;
        }
        m_EbBrowse_MarksHash.clear();

        // *ProjectData* release any ProjectData remaining in the ProjectDataHash
        for (ProjectDataHash::iterator it = m_ProjectDataHash.begin(); it != m_ProjectDataHash.end(); ++it)
        {
            delete it->second;
        }
        m_ProjectDataHash.clear();

    }
	m_InitDone = false;

}
// ----------------------------------------------------------------------------
void BrowseTracker::OnEditorDeactivated(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    // This event is practically useless. When an editor is opened you get the
    // following:
    //12:14:33: Editor DE-activated[03894E88][]
    //12:14:33: OnEditorOpen ebase[03894E88]cbed[03894E88]stc[03894C88][Version.cpp]
    //12:14:33: Editor DE-activated[03894E88][Version.cpp]
    //12:14:33: Editor Activated[03894E88][Version.cpp]

    if (m_bAppShutdown) return;
    EditorBase* eb = event.GetEditor();
    if (not eb) return;
    if (IsAttached() && m_InitDone)
    {
        m_LastEbDeactivated = eb;
        #if defined(LOGGING)
        LOGIT( _T("BT Editor DE-ACTIVATED[%p][%s]"), eb, eb->GetShortName().c_str() );
        wxUnusedVar(eb);
        #endif
    }
    #if not defined(LOGGING)
        wxUnusedVar(eb);
    #endif

    event.Skip();
}
// ----------------------------------------------------------------------------
void BrowseTracker::OnEditorOpened(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    // When editor belongs to a project, tell ProjectData about it.

    event.Skip();

    if (IsAttached() && m_InitDone)
    {

        EditorBase* eb = event.GetEditor();
        cbEditor* cbed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(eb);
        if (not cbed) return;

        // validate cbProject has been set
        cbProject* pcbProject = GetProject( eb );
        #if defined(LOGGING)
        cbStyledTextCtrl* control = 0;
        if (cbed) control = cbed->GetControl();
        LOGIT( _T("BT OnEditorOpen ebase[%p]cbed[%p]stc[%p]proj[%p][%s]"), eb, cbed, control, pcbProject, eb->GetShortName().c_str() );
        #endif

        // stow opened editor info in the ProjectData class
        if (pcbProject)
        {    ProjectData* pProjectData = GetProjectDataFromHash(pcbProject);
            if (pProjectData) pProjectData->AddEditor( eb->GetFilename() );
        }

            //This code shows that the cbProject*'s are empty at this event
            // That seems very odd ?
            //#if defined(LOGGING)
            ////NB: There is no cbEditor::ProjectFile* or ProjectFile::GetBaseName()
            ////    and there's no initialized Project* that I can find.
            //ProjectFile* pProjectFile = cbed->GetProjectFile();
            //wxString projectBaseName = pProjectFile?pProjectFile->GetBaseName() :*wxEmptyString;
            // LOGIT( _T("BT OnEditorOpen project[%p]projectBaseName[%s]"), pProjectFile, projectBaseName.c_str() );
            //// -- there is no intialized cbProject* yet for the opening project.
            ////Note here, that the returned project is the project about to be deactivated.
            //ProjectManager* prjMgr = Manager::Get()->GetProjectManager();
            //cbProject* pcbProject = prjMgr->GetActiveProject();
            //wxString filename  = pcbProject?pcbProject->GetFilename():*wxEmptyString;
            // LOGIT( _T("BT OnEditorOpen cbProject[%p]filename[%s]"), pcbProject, filename.c_str() );
            // #endif

        // Editors opened by Alt-G and Swap header/source do not have
        // cbEditors attached. So we have to re-call OnEditorActivated here.
        CodeBlocksEvent evt;
        evt.SetEditor(eb);
        OnEditorActivated(evt);

    }//if isAttached
}
// ----------------------------------------------------------------------------
void BrowseTracker::OnEditorClosed(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    // clear this editor out of our arrays and pointers

    //_NOTE: using Manager::Get->GetEditorManager()->GetEditor... etc will
    //      fail in this codeblocks event.
    //      The cbEditors are nolonger available
    event.Skip();

    if (IsAttached() && m_InitDone)
    {
        EditorBase* eb = event.GetEditor();
        wxString filePath = event.GetString();
        ProjectData* pProjectData = GetProjectDataByEditorName( filePath);

        #if defined(LOGGING)
            LOGIT( _T("BT OnEditorClosed Eb[%p][%s]"), eb, eb->GetShortName().c_str() );
            ///LOGIT( _T("BT Closing Eb[%p][%s]"), eb, eb->GetFilename().c_str() );
        #endif

        //-cbEditor* cbed = m_pEdMgr->GetBuiltinEditor(eb);
        //-if (not cbed) return;

        // If not our editor, or editor already closed, return
        if ( GetEditor(eb) == -1) return;

        // If editor belong to a project,
        // Copy current Browse/Book marks to archive so we have
        // them if the user reopens this editor
        if ( pProjectData )
        {
            BrowseMarks* pArchBook_Marks = pProjectData->GetBook_MarksFromHash( eb->GetFilename());
            BrowseMarks* pCurrBook_Marks = GetBook_MarksFromHash( eb->GetFilename());
            //*note* cannot get access to scintilla book marks here because the cbStyledTextCtrl
            // is not accessible
            if (pCurrBook_Marks && pArchBook_Marks)
                pArchBook_Marks->CopyMarksFrom(*pCurrBook_Marks);

            // Copy current BrowseMarks to archive BrowseMarks so we have
            // them if the user reopens this editor
            BrowseMarks* pArchBrowse_Marks = pProjectData->GetBrowse_MarksFromHash( eb->GetFilename());
            BrowseMarks* pCurrBrowse_Marks = GetBrowse_MarksFromHash( eb);
            if (pCurrBrowse_Marks && pArchBrowse_Marks)
                pArchBrowse_Marks->CopyMarksFrom(*pCurrBrowse_Marks);
        }
        // Clean up the closed editor and its associated Book/BrowseMarks
        for (int i=0; i<MaxEntries; ++i )
            if ( eb == GetEditor(i)  )
            {
                #if defined(LOGGING)
                LOGIT( _T("BT OnEditorClosed cleared[%p]"), GetEditor(i));
                #endif
                RemoveEditor(GetEditor(i));
            }//if

        // Activate the previously active editor. EditorManager::OnUpdateUI used to do this
        // but wzAuiNotebook broke it. wxAuiNotebook always activates the last page(tab).
        if ( m_LastEbDeactivated and IsEditorBaseOpen(m_LastEbDeactivated) )
            m_UpdateUIFocusEditor = m_LastEbDeactivated;
        else
            m_UpdateUIFocusEditor = GetPreviousEditor();
        #if defined(LOGGING)
        if (m_UpdateUIFocusEditor)
        LOGIT( _T("BT OnEditorClosed activating eb[%s]"), m_UpdateUIFocusEditor->GetShortName().c_str());
        #endif

    }//if(IsAttached() && m_InitDone)


}//OnEditorClosed
// ----------------------------------------------------------------------------
#if defined(LOGGING)
void BrowseTracker::OnWindowSetFocus(wxFocusEvent& event)
#else
void BrowseTracker::OnWindowSetFocus(wxFocusEvent& WXUNUSED(event))
#endif
// ----------------------------------------------------------------------------
{
    #if defined(LOGGING)
    wxWindow* p = (wxWindow*)event.GetEventObject();
    LOGIT( _T("BT SetFocusEvent for[%p]"), p);
    #endif
}
// ----------------------------------------------------------------------------
void BrowseTracker::AddEditor(EditorBase* eb)
// ----------------------------------------------------------------------------
{
    // Add this editor the array of activated editors

    if (not eb) return;
    if ( ++m_LastEditorIndex >= MaxEntries ) m_LastEditorIndex = 0;
    m_apEditors[m_LastEditorIndex] = eb;
    ++m_nBrowsedEditorCount;
    #if defined(LOGGING)
    //LOGIT( _T("BT AddEditor[%p][%s]"), eb, eb->GetShortName().c_str() );
    #endif
}
// ----------------------------------------------------------------------------
BrowseMarks* BrowseTracker::HashAddBrowse_Marks( const wxString fullPath)
// ----------------------------------------------------------------------------
{
    // EditorManager calls fail during the OnEditorClose event
    // eg,EditorBase* eb = Manager::Get()->GetEditorManager()->GetEditor(filename);

    EditorBase* eb = m_pEdMgr->GetEditor(fullPath);
    if (not eb) return 0;
    // don't add duplicates
    EbBrowse_MarksHash& hash = m_EbBrowse_MarksHash;
    BrowseMarks* pBrowse_Marks = GetBrowse_MarksFromHash( eb);
    if (not pBrowse_Marks)
    {   pBrowse_Marks = new BrowseMarks(eb->GetFilename() );
        hash[eb] = pBrowse_Marks;
    }

    // Allocate book marks array also
    HashAddBook_Marks( fullPath);

    // If this editor belongs to a project,
    // Ask ProjectData to alloc the archive Browse/Book marks arrays
    ProjectData* pProjectData = GetProjectDataByEditorName( fullPath );
    if(pProjectData)
        pProjectData->HashAddBrowse_Marks( fullPath );

    #if defined(LOGGING)
    ///LOGIT( _T("BT HashAddBrowse_Marks[%s]"), eb->GetFilename().c_str() );
    #endif
    return pBrowse_Marks;
}
// ----------------------------------------------------------------------------
BrowseMarks* BrowseTracker::HashAddBook_Marks( const wxString fullPath)
// ----------------------------------------------------------------------------
{
    // EditorManager calls fail during the OnEditorClose event
    // eg,EditorBase* eb = Manager::Get()->GetEditorManager()->GetEditor(filename);

    EditorBase* eb = m_pEdMgr->GetEditor(fullPath);
    if (not eb) return 0;
    EbBrowse_MarksHash& hash = m_EdBook_MarksHash;
    BrowseMarks* pBook_Marks = GetBook_MarksFromHash( eb);
    if (not pBook_Marks)
    {   pBook_Marks = new BrowseMarks(fullPath);
        hash[eb] = pBook_Marks;
    }

    // If this editor belongs to a project
    // Ask ProjectData to alloc the archive Browse/Book marks arrays
    ProjectData* pProjectData = GetProjectDataByEditorName( fullPath );
    if(pProjectData)
        pProjectData->HashAddBook_Marks( eb->GetFilename());

    #if defined(LOGGING)
    ///LOGIT( _T("BT HashAddBOOKMakrs[%s]"), eb->GetFilename().c_str() );
    #endif
    return pBook_Marks;
}
// ----------------------------------------------------------------------------
void BrowseTracker::ClearEditor(int index)
// ----------------------------------------------------------------------------
{
    // Used to remove duplicate editors without deleting array data
    // Duplicates especially occur when a previous editor is re-activated after
    // a secondary project is closed.

    if (index < 0) return;
    m_apEditors[index] = 0;
    --m_nBrowsedEditorCount;
}
// ----------------------------------------------------------------------------
void BrowseTracker::RemoveEditor(EditorBase* eb)
// ----------------------------------------------------------------------------
{
    // clear this editor out of our arrays and pointers
    // ie, reverse of the processing in OnEditorActivated()

    // don't allow recursion from our called routines.
    if (m_nRemoveEditorSentry) return;
    if (not eb) return;

    ++m_nRemoveEditorSentry;

    if (eb == m_UpdateUIFocusEditor)
        m_UpdateUIFocusEditor = 0;

    if (IsAttached() && m_InitDone) do
    {
        #if defined(LOGGING)
        //Dont use eb to reference data. It may have already been destroyed.
         //LOGIT( _T("BT Removing[%p][%s]"), eb, eb->GetShortName().c_str() );
         ///LOGIT( _T("BT RemoveEditor[%p]"), eb );
        #endif

        for (int i=0; i<MaxEntries; ++i )
            if ( eb == GetEditor(i)  )
                ClearEditor(i);

        // remove the hash entry for this editor
        if ( m_EbBrowse_MarksHash.find(eb) != m_EbBrowse_MarksHash.end())
        {
            delete m_EbBrowse_MarksHash[eb]; //Browse Marks
            m_EbBrowse_MarksHash.erase(eb);
            delete m_EdBook_MarksHash[eb];   //Book Marks
            m_EdBook_MarksHash.erase(eb);

            // using a stale eb will cause a crash
            if (-1 != m_pEdMgr->FindPageFromEditor(eb) )
            {
                wxWindow* win = wxWindow::FindWindowByName(wxT("SCIwindow"),eb);
                if ( win )
                {
                    win->Disconnect(wxEVT_LEFT_UP,
                                    (wxObjectEventFunction)(wxEventFunction)
                                    (wxMouseEventFunction)&BrowseTracker::OnMouseKeyEvent,
                                     NULL, this);
                    win->Disconnect(wxEVT_LEFT_DOWN,
                                    (wxObjectEventFunction)(wxEventFunction)
                                    (wxMouseEventFunction)&BrowseTracker::OnMouseKeyEvent,
                                     NULL, this);
                    win->Disconnect(wxEVT_LEFT_DCLICK,
                                    (wxObjectEventFunction)(wxEventFunction)
                                    (wxMouseEventFunction)&BrowseTracker::OnMouseKeyEvent,
                                     NULL, this);
                    win->Disconnect(wxEVT_MOTION,
                                    (wxObjectEventFunction)(wxEventFunction)
                                    (wxMouseEventFunction)&BrowseTracker::OnMouseKeyEvent,
                                     NULL, this);
                    win->Disconnect(wxEVT_CONTEXT_MENU,
                                    (wxObjectEventFunction)(wxEventFunction)
                                    (wxCommandEventFunction)&BrowseTracker::OnMarginContextMenu,
                                     NULL, this);
                }//if win
            }//if find page from editor
            #if defined(LOGGING)
            ///LOGIT( _T("BT RemoveEditor Erased hash entry[%p]"), eb );
            #endif
        }
    }while(0);

    m_nRemoveEditorSentry = 0;
}
// ----------------------------------------------------------------------------
void BrowseTracker::OnProjectOpened(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    // NB: EVT_PROJECT_ACTIVATE is occuring before EVT_PROJECT_OPEN
    // NB: EVT_EDITOR_ACTIVATE events occur before EVT_PROJECT_ACTIVATE or EVT_PROJECT_OPEN
    // Currently, this event is a hack to us since it occurs AFTER editors are activated
    //  and AFTER the project is activated

    // But since the editors are now already open, we can read the layout file
    // that saved previous BrowseMark and book mark history, and use that data
    // to build/set old saved Browse/Book marks.

    if ( not IsBrowseMarksEnabled() )
        return;
    m_bProjectClosing = false;

    cbProject* pProject = event.GetProject();

    if ( not pProject )
    {   //caused when project imported
        m_bProjectIsLoading = false;
        return;
    }
    #if defined(LOGGING)
     LOGIT( _T("BT -----------------------------------"));
     LOGIT( _T("BT Project OPENED[%s]"), event.GetProject()->GetFilename().c_str() );
    #endif

    wxString projectFilename = event.GetProject()->GetFilename();


    // allocate a ProjectData to hold activated editors
    cbProject* pCBProject = event.GetProject();
    ProjectData* pProjectData = GetProjectDataFromHash( pCBProject);
    if (not pProjectData)
    {   pProjectData = new ProjectData(pCBProject);
        m_ProjectDataHash[pCBProject] = pProjectData;
    }

    // Read the layout file for this project, build BrowseMarks for each editor
    pProjectData = GetProjectDataFromHash( event.GetProject() );
    if ( pProjectData)
        if (not pProjectData->IsLayoutLoaded() )
            pProjectData->LoadLayout();

    // There is a bug such that the project loading hook is *not* called
    // for some projects with a stray </unit> in its xml file.
    // Remove all the activated editors for this project when
    // the project was loaded. We don't want to see them if the user
    // didn't manually activate them.
    if (not m_bProjectIsLoading)
    {
        for (FilesList::iterator it = pCBProject->GetFilesList().begin(); it != pCBProject->GetFilesList().end(); ++it)
        {
            for (int j=0; j<MaxEntries; ++j)
            {
                if ( GetEditor(j) == 0 ) continue;
                //#if defined(LOGGING)
                //LOGIT( _T("BT eb[%s]projectFile[%s]"),
                //    GetEditor(j)->GetFilename().c_str(), pProject->GetFile(i)->file.GetFullPath().c_str() );
                //#endif
                if ( (*it)->file.GetFullPath() ==  GetEditor(j)->GetFilename())
                {
                    //#if defined(LOGGING)
                    //LOGIT( _T("BT OnProjectOpened:Removing[%s]"),GetEditor(j)->GetFilename().c_str() );
                    //#endif
                    RemoveEditor(GetEditor(j));
                    break;
                }
            }
        }//for
    }//if

    // Turn off "project loading" in order to record the last activated editor
    m_bProjectIsLoading = false;

    // Record the last CB activated editor as if the user activate it.
    EditorBase* eb = m_pEdMgr->GetBuiltinActiveEditor();
    if ( eb && (eb != GetCurrentEditor()) )
    {
        CodeBlocksEvent evt;
        evt.SetEditor(eb);
        OnEditorActivated(evt);
        #if defined(LOGGING)
        LOGIT( _T("BT OnProjectOpened Activated Editor[%p][%s]"), eb, eb->GetShortName().c_str() );
        #endif
    }
    //*Testing*
    //for (EbBrowse_MarksHash::iterator it = m_EdBrowse_MarksArchive.begin(); it !=m_EdBrowse_MarksArchive.end(); ++it )
    //	it->second->Dump();

    event.Skip();

}//OnProjectOpened
// ----------------------------------------------------------------------------
void BrowseTracker::OnProjectClosing(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    // This hook occurs before the editors are closed. That allows
    // us to reference CB project and editor related data before CB
    // deletes it all.

    if (not IsBrowseMarksEnabled())
        return;

    m_bProjectClosing = true;
    m_nProjectClosingFileCount = 0;

    cbProject* pProject = event.GetProject();
    if (not pProject) return; //It happens!

    #if defined(LOGGING)
     LOGIT( _T("BT Project CLOSING[%p][%s]"), pProject, pProject->GetFilename().c_str() );
    #endif

    // Simulate closing the remaining editors here so that we can write
    // a layout file containing the BrowseMarks and Book_Marks
    // This is the last chance we get to write a layout file.

    ProjectData* pProjectData = GetProjectDataFromHash( pProject);
    #if defined(LOGGING)
        if ( not pProjectData )
            LOGIT( _T("BT *CRASH* OnProjectClosing entered w/o project pointer") );
    #endif
    if ( not pProjectData ) return;

    // Close editors that belong to the current project
    for (int i=0; i < m_pEdMgr->GetEditorsCount(); ++i)
    {
    	EditorBase* eb = m_pEdMgr->GetEditor(i);
        // Simulate closing editors that belong to the current project
        if ( pProjectData->FindFilename(eb->GetFilename()) )
        {
            CodeBlocksEvent evt(cbEVT_EDITOR_CLOSE);
            evt.SetEditor(eb);
            evt.SetString(eb->GetFilename());
            OnEditorClosed( evt );
            m_nProjectClosingFileCount += 1;
        }
        else{
            #if defined(LOGGING)
            ////LOGIT( _T("BT ProjectClosing failed to find[%s]"), eb->GetFilename().c_str() );
            #endif
        }
    }

    // Write the layout for this project
    if (pProjectData) pProjectData->SaveLayout();

    // Free the ProjectData
    if ( pProjectData )
    {
        #if defined(LOGGING)
        LOGIT( _T("BT deleting ProjectData[%p][%s]"), pProjectData, pProjectData->GetProjectFilename().c_str()  );
        #endif
        delete pProjectData;
        m_ProjectDataHash.erase(pProject);
    }

        //-wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED);
        //-OnMenuTrackerDump(evt);

    event.Skip();

}//OnProjectClosing
// ----------------------------------------------------------------------------
void BrowseTracker::OnProjectActivatedEvent(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    // NB: EVT_PROJECT_ACTIVATE is occuring before EVT_PROJECT_OPEN
    // NB: EVT_EDITOR_ACTIVATE event occur before EVT_PROJECT_ACTIVATE or EVT_PROJECT_OPEN

    // _*NOTE* when a secondary project closes and re-activates a previous one,
    // *NO* cbEVT_PROJECT_ACTIVATE occurs. We end up with the wrong active project
    // pointer. Only a cbEVT_EDITOR_ACTIVATE occurs.

    event.Skip();

    if (not IsBrowseMarksEnabled() )
        return;

    // allocate a ProjectData to hold activated editors
    cbProject* pCBProject = event.GetProject();

    if (not pCBProject) return; //caused by importing a project

    #if defined(LOGGING)
    LOGIT( _T("BT -----------------------------------"));
    LOGIT( _T("BT Project ACTIVATED[%p][%s]"), pCBProject,  pCBProject->GetFilename().c_str() );
    #endif

    ProjectData* pProjectData = GetProjectDataFromHash( pCBProject);
    if (not pProjectData)
    {   pProjectData = new ProjectData(pCBProject);
        m_ProjectDataHash[pCBProject] = pProjectData;
    }
    pProjectData->IncrementActivationCount();

    // store the current editor in BrowseTracker array
    EditorBase* eb = Manager::Get()->GetEditorManager()->GetActiveEditor();
        //LOGIT( _T("BT ProjectActivated eb[%p][%s]Current[%p][%s]"),
        //    eb, GetEditorFilename(eb).c_str(),
        //    m_pCurrentEditor, GetEditorFilename(m_pCurrentEditor).c_str());

    // Record the last CB activated editor as if the user activate it.
    // See OnEditorActivated()
    if ( eb && (eb != GetCurrentEditor()) )
    {
        CodeBlocksEvent evt;
        evt.SetEditor(eb);
        OnEditorActivated(evt);
        //LOGIT( _T("BT OnProjectActivated Editor[%p][%s]"), eb, GetEditorFilename(eb).c_str()  );
    }

    // Compress the editor pointer array to allow all "forward" available slots
    int index = GetCurrentEditorIndex();
    if ( GetEditorBrowsedCount() )
    {
        ArrayOfEditorBasePtrs tmpArray;
        tmpArray.Alloc(MaxEntries);
        // copy current editors & clear for compression
        for (int i = 0; i<MaxEntries; ++i)
        {
            tmpArray.Add(m_apEditors[i]);   //patch 2886
            m_apEditors[i] = 0;
        }//for
        m_CurrEditorIndex = 0;
        m_LastEditorIndex = MaxEntries-1;
        for (int i = 0; i<MaxEntries; ++i)
        {
            if ( tmpArray[index] )
            {   if (++m_LastEditorIndex >= MaxEntries) m_LastEditorIndex = 0;
                 m_apEditors[m_LastEditorIndex] = tmpArray[index];
            }
            if (++index >= MaxEntries) index = 0;
        }//for
    }//if
    else
    {   m_CurrEditorIndex = 0;
        m_LastEditorIndex = MaxEntries-1;
    }

    // Previous project was closing
    if (m_bProjectClosing) do
    {
        m_bProjectClosing = false;
        // wxAUI activates the last displayed tab of a previous project
        // We want to activate the previously *active* tab
        if (m_nProjectClosingFileCount)
            m_UpdateUIFocusEditor =  GetPreviousEditor();
        else
            m_UpdateUIFocusEditor =  GetCurrentEditor();
        #if defined(LOGGING)
        if (m_UpdateUIFocusEditor)
        {   LOGIT( _T("BT OnProjectActivated m_nProjectClosingFileCount[%d]"), m_nProjectClosingFileCount);
            LOGIT( _T("BT OnProjectActivated setting Next Ed[%s]"), m_UpdateUIFocusEditor->GetShortName().wx_str());
        }
        #endif
        m_nProjectClosingFileCount = 0;
    }while(0);

}//OnProjectActivatedEvent
// ----------------------------------------------------------------------------
void BrowseTracker::OnProjectLoadingHook(cbProject* project, TiXmlElement* /*elem*/, bool loading)
// ----------------------------------------------------------------------------
{
    // This turned out to be a problem. It isn't always called for old type projects
    // or a project with an extra </unit> in it.

    // CB does not issue Project_Opened or Project_activated events until
    // after the editors are loaded and activated. That causes us to record
    // the loading editors as if the user activated them manually.
    // So we use this hook to tell the recorder NOT to record the loading editors.

    // *Logic Gotcha* When a work space loads, OnProjectActivated() is called for
    // the active project, then loading proceeds for other projects. This can cause
    // the m_bProjectIsLoading flag to be set forever.

    #if defined(LOGGING)
    //-LOGIT( _T("BT OnProjectLoadingHook [%s]"), loading? wxT("Loading"):wxT("Saving") );
    #endif

    if (not IsBrowseMarksEnabled() )
        return;

    if (loading)
    {
        // dont record CB activated editors while project is loading
        //-------------------------------
        m_bProjectIsLoading = true;
        //-------------------------------

        m_LoadingProjectFilename = project->GetFilename();

        #if defined(LOGGING)
        LOGIT( _T("BT ----------------------------------------------") );
        LOGIT( _T("BT ProjectLoadingHook::LOADING[%p][%s]"), project, m_LoadingProjectFilename.c_str() );
        #endif
    }
    else //saving project
    {
            //#if defined(LOGGING)
            //LOGIT( _T("BT ProjectLoadingHook::SAVING[]TiXmlElement[%p]"), elem  );
            //#endif
    }
}//OnProjectLoadingHook
// ----------------------------------------------------------------------------
void BrowseTracker::OnEditorEventHook(cbEditor* pcbEditor, wxScintillaEvent& event)
// ----------------------------------------------------------------------------
{
    // Rebuild the BrowseMarks array if user deletes/adds editor lines

//    wxString txt = _T("OnEditorModified(): ");
//    int flags = event.GetModificationType();
//    if (flags & wxSCI_MOD_CHANGEMARKER) txt << _T("wxSCI_MOD_CHANGEMARKER, ");
//    if (flags & wxSCI_MOD_INSERTTEXT) txt << _T("wxSCI_MOD_INSERTTEXT, ");
//    if (flags & wxSCI_MOD_DELETETEXT) txt << _T("wxSCI_MOD_DELETETEXT, ");
//    if (flags & wxSCI_MOD_CHANGEFOLD) txt << _T("wxSCI_MOD_CHANGEFOLD, ");
//    if (flags & wxSCI_PERFORMED_USER) txt << _T("wxSCI_PERFORMED_USER, ");
//    if (flags & wxSCI_MOD_BEFOREINSERT) txt << _T("wxSCI_MOD_BEFOREINSERT, ");
//    if (flags & wxSCI_MOD_BEFOREDELETE) txt << _T("wxSCI_MOD_BEFOREDELETE, ");
//    txt << _T("pos=")
//        << wxString::Format(_T("%d"), event.GetPosition())
//        << _T(", line=")
//        << wxString::Format(_T("%d"), event.GetLine())
//        << _T(", linesAdded=")
//        << wxString::Format(_T("%d"), event.GetLinesAdded());
//    Manager::Get()->GetLogManager()->DebugLog(txt);

    event.Skip();

    if (not IsBrowseMarksEnabled())
        return;

    //if ( event.GetEventType() != wxEVT_SCI_MODIFIED )
    if ( event.GetEventType() == wxEVT_SCI_MODIFIED )
    {
        // Whenever event.GetLinesAdded() != 0, we must re-set BrowseMarks for lines greater
        // than LineFromPosition(event.GetPosition())

        int linesAdded = event.GetLinesAdded();
        bool isAdd = event.GetModificationType() & wxSCI_MOD_INSERTTEXT;
        bool isDel = event.GetModificationType() & wxSCI_MOD_DELETETEXT;
        if ((isAdd || isDel) && linesAdded != 0)
        {
            #if defined(LOGGING)
            //LOGIT( _T("BT EditorEventHook isAdd[%d]isDel[%d]lines[%d]"), isAdd, isDel, linesAdded );
            #endif
            // rebuild BrowseMarks from scintilla marks
            RebuildBrowse_Marks( pcbEditor, isAdd );
        }//if
    }//wxEVT_SCI_MODIFIED

    // wxSCI_MOD_CHANGEMARKER is an extremely expensive call. It's called
    // for each line during a file load, and for every change to every
    // margin marker in the known cosmos. So here we allow a "one shot only"
    // to catch the marker changed by a margin context menu.
    // cf: CloneBookMarkFromEditor() and OnMarginContextMenu()
    if ( event.GetEventType() == wxEVT_SCI_MODIFIED )
    do{
        if ( m_OnEditorEventHookIgnoreMarkerChanges )
            break;
        int flags = event.GetModificationType();
        if (flags & wxSCI_MOD_CHANGEMARKER )
        {
            m_OnEditorEventHookIgnoreMarkerChanges = true;
            int line = event.GetLine();
            #if defined(LOGGING)
            //LOGIT( _T("BT wxSCI_MOD_CHANGEMARKER line[%d]"), line );
            #endif
            CloneBookMarkFromEditor( line );
        }
    }while(false);

}//OnEditorEventHook
// ----------------------------------------------------------------------------
void BrowseTracker::CloneBookMarkFromEditor( int line )
// ----------------------------------------------------------------------------
{
    // Record the editor bookmark as is.

    EditorBase* eb = Manager::Get()->GetEditorManager()->GetActiveEditor();
    cbEditor* cbed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(eb);
    if (cbed)
    {
        cbStyledTextCtrl* control = cbed->GetControl();
        if ( LineHasBookMarker(control, line) )
            AddBook_Mark( eb );
        else
            ClearLineBookMark();
        #if defined(LOGGING)
        //LOGIT( _T("BT CloneBookMarkFromEditor[%d][%s]"), line,
        //    LineHasBookMarker( control, line)?wxT("Added"):wxT("Cleared") );
        #endif
    }
}
// ----------------------------------------------------------------------------
void BrowseTracker::OnMarginContextMenu(wxContextMenuEvent& event)
// ----------------------------------------------------------------------------
{
    // User has invoked the context menu. Set a flag to allow
    // OnEditorEventHook wxSCI_MOD_CHANGEMARKER overhead
    // If the user clicked withing the number margins, the hook
    // will see it, do its thing, then turn the overhead off.
    // If the user does not change the a marker, the hook will not turn
    // the overhead off, but the next invocatin of OnEditorEventHook will.
    // turn it off anyway.

    m_OnEditorEventHookIgnoreMarkerChanges = false;

    event.Skip();
    return;
}
// ----------------------------------------------------------------------------
void BrowseTracker::OnBook_MarksToggle(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // a Book mark has been toggled
    event.Skip();
    #if defined(LOGGING)
    LOGIT( _T("BT OnBook_MarksToggle") );
    #endif
    ToggleBook_Mark(GetCurrentEditor() );
}
// ----------------------------------------------------------------------------
void BrowseTracker::AddBook_Mark(EditorBase* eb, int /*line*/ /*=-1*/)
// ----------------------------------------------------------------------------
{
    // Stow a Book mark by EditorBase current line
    // Note: the book mark has not been cleared/set by scintilla yet

    cbEditor* cbed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(eb);
    if (cbed)
    {
        if (m_EdBook_MarksHash.find(eb) != m_EdBook_MarksHash.end() )
        do {
            cbStyledTextCtrl* control = cbed->GetControl();
            BrowseMarks& EdBook_Marks = *m_EdBook_MarksHash[eb];
            GetCurrentScreenPositions();
            int pos = control->GetCurrentPos();
            EdBook_Marks.RecordMark(pos);
            #if defined(LOGGING)
            LOGIT( _T("BT AddBook_Mark: pos[%d]line[%d]eb[%p][%s]"),
                pos, m_CurrScrLine, eb, eb->GetShortName().wx_str() );
            ///EdBook_Marks.Dump();
            #endif
        }while(false);//if do

        // When using BookMarks as BrowseMarks toggle the BrowseMark also.
        if ( GetBrowseMarkerId() == BOOKMARK_MARKER )
        if (m_EbBrowse_MarksHash.find(eb) != m_EbBrowse_MarksHash.end() )
        do {
            cbStyledTextCtrl* control = cbed->GetControl();
            BrowseMarks& EdBrowse_Marks = *m_EbBrowse_MarksHash[eb];
            GetCurrentScreenPositions();
            int pos = control->GetCurrentPos();
            EdBrowse_Marks.RecordMark(pos);
            #if defined(LOGGING)
            LOGIT( _T("BT AddBrowseMarkByEb: pos[%d]line[%d]eb[%p][%s]"),
                pos, m_CurrScrLine, eb, eb->GetShortName().wx_str() );
            #endif
        }while(false);//if do

    }//if
}//ToggleBook_Mark
// ----------------------------------------------------------------------------
void BrowseTracker::ToggleBook_Mark(EditorBase* eb)
// ----------------------------------------------------------------------------
{
    // Stow a Book mark by EditorBase current line
    // Note: the book mark has not been cleared/set by scintilla yet

    cbEditor* cbed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(eb);
    if (cbed)
    {
        if (m_EdBook_MarksHash.find(eb) != m_EdBook_MarksHash.end() )
        do {
            cbStyledTextCtrl* control = cbed->GetControl();
            BrowseMarks& EdBook_Marks = *m_EdBook_MarksHash[eb];
            GetCurrentScreenPositions();
            // Toggle BookMark
            if ( LineHasBookMarker(control, m_CurrScrLine) )
            {
                ClearLineBookMark(); //clear previous marks
                break;
            }
            int pos = control->GetCurrentPos();
            EdBook_Marks.RecordMark(pos);
            #if defined(LOGGING)
            LOGIT( _T("BT RecordBook_Mark: pos[%d]line[%d]eb[%p][%s]"),
                pos, m_CurrScrLine, eb, eb->GetShortName().wx_str() );
            ///EdBook_Marks.Dump();
            #endif
        }while(false);//if do

        // When using BookMarks as BrowseMarks toggle the BrowseMark also.
        if ( GetBrowseMarkerId() == BOOKMARK_MARKER )
        if (m_EbBrowse_MarksHash.find(eb) != m_EbBrowse_MarksHash.end() )
        do {
            cbStyledTextCtrl* control = cbed->GetControl();
            BrowseMarks& EdBrowse_Marks = *m_EbBrowse_MarksHash[eb];
            GetCurrentScreenPositions();
            // Toggle BrowseMark
            if ( LineHasBrowseMarker(control, m_CurrScrLine) )
            {
                ClearLineBrowseMark(/*markRemove*/false);
                break;
            }
            int pos = control->GetCurrentPos();
            EdBrowse_Marks.RecordMark(pos);
            #if defined(LOGGING)
            LOGIT( _T("BT RecordBrowseMarkByEb: pos[%d]line[%d]eb[%p][%s]"),
                pos, m_CurrScrLine, eb, eb->GetShortName().wx_str() );
            #endif
        }while(false);//if do

    }//if
}//ToggleBook_Mark
// ----------------------------------------------------------------------------
void BrowseTracker::ClearLineBookMark()
// ----------------------------------------------------------------------------
{
    // clear BookMark entry for a single line out of our history array

    ///LOGIT( _T("BT ClearBookMark") );

    EditorBase* eb = Manager::Get()->GetEditorManager()->GetActiveEditor();
    cbEditor* cbed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(eb);
    if (cbed)
    {
        if (m_EdBook_MarksHash.find(eb) != m_EdBook_MarksHash.end() )
        {
            BrowseMarks& EdBook_Marks = *m_EdBook_MarksHash[eb];
            GetCurrentScreenPositions();
            EdBook_Marks.ClearMark( m_CurrScrLineStartPosn, m_CurrScrLineEndPosn);
            #if defined(LOGGING)
            LOGIT( _T("BT ClearLineBookMark") );
            #endif
        }

    }
}//ClearLineBookMark
// ----------------------------------------------------------------------------
void BrowseTracker::RebuildBrowse_Marks(cbEditor* /*pcbEditor*/, bool addedLines)
// ----------------------------------------------------------------------------
{
    // Adjust BrowseMarks from scintilla moved markers

    #if defined(LOGGING)
    ///LOGIT( _T("BT RebuildBrowse_Marks") );
    #endif
    EditorBase* eb = GetCurrentEditor();
    if (not eb) return;
    cbEditor* cbed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(eb);
    if (not cbed) return;

    if (m_EbBrowse_MarksHash.find(eb) != m_EbBrowse_MarksHash.end() )
    {
        BrowseMarks& EdBrowse_Marks = *m_EbBrowse_MarksHash[eb];
        EdBrowse_Marks.RebuildBrowse_Marks( cbed, addedLines);
    }

}//RebuildBrowse_Marks
// ----------------------------------------------------------------------------
ProjectData* BrowseTracker::GetProjectDataByEditorName( wxString filePath)
// ----------------------------------------------------------------------------
{
    wxString reason = wxT("");

    do {
        EditorBase* eb = m_pEdMgr->GetEditor( filePath );
        reason = wxT("eb");
        if ( not eb ) break;
        cbEditor* cbed = m_pEdMgr->GetBuiltinEditor(eb);
        reason = wxT("cbed");
        if (not cbed) break;
        ProjectFile* pf = cbed->GetProjectFile();
        reason = wxT("ProjectFile");
        if (not pf) break;
        cbProject* pcbProject = pf->GetParentProject();
        reason = wxT("cbProject");
        if (not pcbProject) break;
        return GetProjectDataFromHash( pcbProject);
    }while(0);

    #if defined(LOGGING)
     //LOGIT( _T("BT GetProjectDataByEditorName FAILED to find [%s] for [%s]"), reason.c_str(), filePath.c_str() );
    #endif

    // At this point CB has failed to find the project by its editor filename
    // So search our own ProjectData objects the hard way.
    for (ProjectDataHash::iterator it = m_ProjectDataHash.begin(); it != m_ProjectDataHash.end(); ++it)
    {
        ProjectData* pProjectData = it->second;
        if ( pProjectData->GetBrowse_MarksFromHash( filePath) )
            return pProjectData;
    }

    // Since wxAuiNotebook added, an initial cbEVT_EDITOR_ACTIVATED has no cbEditor
    // or project associated with it. So we'll try to use the current active project.
    ProjectData* pProjectData = 0;
    cbProject* pcbProject = Manager::Get()->GetProjectManager()->GetActiveProject();
    if (pcbProject)
    {
        pProjectData = GetProjectDataFromHash( pcbProject );
        if (pProjectData)
        {
            #if defined(LOGGING)
            LOGIT( _T("BT GetProjectDataByEditorName FAILED, using Active Project for[%s]"),filePath.c_str());
            #endif
            return pProjectData;
        }
    }
    #if defined(LOGGING)
     LOGIT( _T("BT GetProjectDataByEditorName FAILED to find [%s] for [%s]"), wxT("Hash entry"), filePath.c_str() );
    #endif

    return 0;
}//GetProjectDataByEditorName
// ----------------------------------------------------------------------------
ProjectData* BrowseTracker::GetProjectDataByProjectName( wxString projectFilePath)
// ----------------------------------------------------------------------------
{
    cbProject* pProject = m_pPrjMgr->IsOpen(projectFilePath);
    if (not pProject) return 0;
    return GetProjectDataFromHash( pProject);
}
// ----------------------------------------------------------------------------
ProjectData* BrowseTracker::GetProjectDataFromHash( cbProject* pProject)
// ----------------------------------------------------------------------------
{
    // Get the ProjectData* that's associated with this cbProject&
    // eg. cbProject* pProjectData = GetProjectFromHash(m_ProjectDataHash, pProject);
    ProjectDataHash& hash = m_ProjectDataHash;
    ProjectDataHash::iterator it = hash.find( pProject );
    if ( it == hash.end() ) return 0;
    return it->second;
}
// ----------------------------------------------------------------------------
BrowseMarks* BrowseTracker::GetBrowse_MarksFromHash( EditorBase* eb)
// ----------------------------------------------------------------------------
{
    // Return the BrowseMarks array associated with the Editor
    //EbBrowse_MarksHash m_EdBrowse_MarksArchive;
    EbBrowse_MarksHash::iterator it = m_EbBrowse_MarksHash.find(eb);
    if ( it == m_EbBrowse_MarksHash.end() ) return 0;
    return it->second;
}
// ----------------------------------------------------------------------------
BrowseMarks* BrowseTracker::GetBrowse_MarksFromHash( wxString filePath)
// ----------------------------------------------------------------------------
{
    // Return the BrowseMarks array associated with this file path

    for (EbBrowse_MarksHash::iterator it = m_EbBrowse_MarksHash.begin(); it != m_EbBrowse_MarksHash.end(); it++)
    {
        BrowseMarks* p = it->second;
        if ( p->GetFilePath() == filePath ) {return p;}
    }

    return 0;
}
// ----------------------------------------------------------------------------
BrowseMarks* BrowseTracker::GetBook_MarksFromHash( EditorBase* eb)
// ----------------------------------------------------------------------------
{
    // return the Book marks history array associated with this editor
    EbBrowse_MarksHash::iterator it = m_EdBook_MarksHash.find(eb);
    if ( it == m_EdBook_MarksHash.end() ) return 0;
    return it->second;
}
// ----------------------------------------------------------------------------
BrowseMarks* BrowseTracker::GetBook_MarksFromHash( wxString filePath)
// ----------------------------------------------------------------------------
{
    // return the Book marks history array associated with this file path
    #if defined(LOGGING)
    ///LOGIT( _T("BT GetBook_MarksFromHash looking for[%s]"),filePath.c_str() );
    #endif

    for (EbBrowse_MarksHash::iterator it = m_EdBook_MarksHash.begin(); it != m_EdBook_MarksHash.end(); it++)
    {
        BrowseMarks* p = it->second;
        #if defined(LOGGING)
        ///LOGIT( _T("BT GetBook_MarksFromHash finding[%s]"),p->GetFilePath().c_str() );
        #endif
        if ( p->GetFilePath() == filePath ) {return p;}
    }

    return 0;
}
// ----------------------------------------------------------------------------
#if defined(LOGGING)
void BrowseTracker::DumpHash( const wxString hashType)
#else
void BrowseTracker::DumpHash( const wxString /*hashType*/)
#endif
// ----------------------------------------------------------------------------
{

    #if defined(LOGGING)
    LOGIT( _T("BT --- DumpHash ---[%s]"), hashType.c_str()  );

    EbBrowse_MarksHash* phash = &m_EbBrowse_MarksHash;
    if ( hashType == wxT("BookMarks") )
        phash = &m_EdBook_MarksHash;
    EbBrowse_MarksHash& hash = *phash;

    for (EbBrowse_MarksHash::iterator it = hash.begin(); it != hash.end(); it++)
    {
        EditorBase* eb = it->first;
        BrowseMarks* p = it->second;
        LOGIT( _T("BT eb*[%p]%s*[%p]name[%s]"), eb, hashType.c_str(), p, p->GetFilePath().c_str() );
    }

    #endif
}
// ----------------------------------------------------------------------------
wxString BrowseTracker::FindAppPath(const wxString& argv0, const wxString& cwd, const wxString& appVariableName)
// ----------------------------------------------------------------------------
{
    // Find the absolute path from where this application has been run.
    // argv0 is wxTheApp->argv[0]
    // cwd is the current working directory (at startup)
    // appVariableName is the name of a variable containing the directory for this app, e.g.
    // MYAPPDIR. This is checked first.

    wxString str;

    // Try appVariableName
    if (!appVariableName.IsEmpty())
    {
        str = wxGetenv(appVariableName);
        if (!str.IsEmpty())
            return str;
    }

#if defined(__WXMAC__) && !defined(__DARWIN__)
    // On Mac, the current directory is the relevant one when
    // the application starts.
    return cwd;
#endif

    wxString argv0Str = argv0;
    #if defined(__WXMSW__)
        do{
            if (argv0Str.Contains(wxT(".exe")) ) break;
            if (argv0Str.Contains(wxT(".bat")) ) break;
            if (argv0Str.Contains(wxT(".cmd")) ) break;
            argv0Str.Append(wxT(".exe"));
        }while(0);
    #endif

    if (wxIsAbsolutePath(argv0Str))
    {
        #if defined(LOGGING)
        LOGIT( _T("BT FindAppPath: AbsolutePath[%s]"), wxPathOnly(argv0Str).GetData() );
        #endif
        return wxPathOnly(argv0Str);
    }
    else
    {
        // Is it a relative path?
        wxString currentDir(cwd);
        if (currentDir.Last() != wxFILE_SEP_PATH)
            currentDir += wxFILE_SEP_PATH;

        str = currentDir + argv0Str;
        if (wxFileExists(str))
        {
            #if defined(LOGGING)
            LOGIT( _T("BT FindAppPath: RelativePath[%s]"), wxPathOnly(str).GetData() );
            #endif
            return wxPathOnly(str);
        }
    }

    // OK, it's neither an absolute path nor a relative path.
    // Search PATH.

    wxPathList pathList;
    pathList.AddEnvList(wxT("PATH"));
    str = pathList.FindAbsoluteValidPath(argv0Str);
    if (!str.IsEmpty())
    {
        #if defined(LOGGING)
        LOGIT( _T("BT FindAppPath: SearchPath[%s]"), wxPathOnly(str).GetData() );
        #endif
        return wxPathOnly(str);
    }

    // Failed
    #if defined(LOGGING)
     LOGIT(  _T("FindAppPath: Failed, returning cwd") );
    #endif
    return wxEmptyString;
    //return cwd;
}//FindAppPath
// ----------------------------------------------------------------------------
bool BrowseTracker::IsEditorBaseOpen(EditorBase* eb)
// ----------------------------------------------------------------------------
{
    cbAuiNotebook* m_pNotebook = Manager::Get()->GetEditorManager()->GetNotebook();
    for (size_t i = 0; i < m_pNotebook->GetPageCount(); ++i)
    {
        //wxWindow* winPage = m_pNotebook->GetPage(i);
        //#if defined(LOGGING)
        //if ( winPage )
        //    LOGIT( _T("IsEditorBaseOpen[%s]"), ((EditorBase*)winPage)->GetShortName().c_str());
        //#endif
        if (m_pNotebook->GetPage(i) == eb)
            return true;
    }
    return false;
}
//// ----------------------------------------------------------------------------
//void BrowseTracker::OnMenuTrackBackward(wxCommandEvent& event)
//// ----------------------------------------------------------------------------
//{
//    // *** Deprecated *** unused *** routine
//    // Browse Tracker menu Backward
//
//    EditorManager* EdMgr = Manager::Get()->GetEditorManager();
//    EditorBase* eb = 0;
//    EditorBase* ebCurrent = EdMgr->GetActiveEditor();
//
//    int index = m_nCurrentEditorIndex-1;
//    // scan for previous editor, skipping nulls (null is a closed editors)
//    for (int i=0; i<MaxEntries; ++i)
//    {
//        if ( index < 0 ) index = MaxEntries-1;
//        eb = m_apEditors[index];
//        LOGIT( _T("BT Backward:m_nCurrentEditorIndex[%d]index[%d]eb[%p][%s]"),m_nCurrentEditorIndex, index, eb, GetEditorFilename(eb).c_str() );
//        if ( eb )
//        {
//            if ( eb == ebCurrent ) { --index; continue;}
//            if (-1 == EdMgr->FindPageFromEditor(eb) )
//            {   // this entry has been closed behind our backs
//                m_apEditors[index] = 0; { --index; continue;}
//            }
//            m_bProjectIsLoading = true;
//            eb->Activate();
//            m_bProjectIsLoading = false;
//            m_nCurrentEditorIndex = index;
//            m_pCurrentEditor = eb;
//            break;
//        }
//        else --index;
//    }//for
//}
//// ----------------------------------------------------------------------------
//void BrowseTracker::OnMenuTrackforward(wxCommandEvent& event)
//// ----------------------------------------------------------------------------
//{
//    // *** Deprecated *** unused *** routine
//    // Browse Tracker menu forward
//
//    EditorBase* eb = 0;
//    EditorManager* EdMgr = Manager::Get()->GetEditorManager();
//    EditorBase* ebCurrent = EdMgr->GetActiveEditor();
//
//    int index = m_nCurrentEditorIndex+1;
//    // scan for previous editor, skipping nulls (null is a closed editors)
//    for (int i=0; i<MaxEntries; ++i)
//    {
//        if ( index >= MaxEntries ) index = 0;
//        eb = m_apEditors[index];
//         //LOGIT( _T("BT Forward:m_nCurrentEditorIndex[%d]index[%d]eb[%p]"),m_nCurrentEditorIndex, index, eb );
//        if ( eb )
//        {
//            if ( eb == ebCurrent ) { ++index; continue;}
//            if (-1 == EdMgr->FindPageFromEditor(eb) )
//            {   // this entry has been closed behind our backs
//                m_apEditors[index] = 0; {++index; continue;}
//            }
//
//            m_bProjectIsLoading = true;
//            eb->Activate();
//            m_bProjectIsLoading = false;
//            m_nCurrentEditorIndex = index;
//            m_pCurrentEditor = eb;
//            break;
//        }
//        else ++index;
//    }//for
//}
// ----------------------------------------------------------------------------

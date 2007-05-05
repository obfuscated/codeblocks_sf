/*
	This file is part of Code Snippets, a plugin for Code::Blocks
	Copyright (C) 2006 Arto Jonsson
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
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
// RCS-ID: $Id: codesnippets.cpp 75 2007-05-05 03:28:42Z Pecan $

#include "sdk.h"
#ifndef CB_PRECOMB
	#include <wx/event.h>
	#include <wx/frame.h> // Manager::Get()->GetAppWindow()
	#include <wx/intl.h>
	#include <wx/menu.h>
	#include <wx/menuitem.h>
	#include <wx/string.h>
	#include "manager.h"
	#include "sdk_events.h"
	#include <wx/stdpaths.h>
#endif

#include <wx/dnd.h>

#include "version.h"
#include "codesnippets.h"
#include "codesnippetswindow.h"
#include "snippetsconfig.h"
#include "messagebox.h"
#include <wx/wxFlatNotebook/wxFlatNotebook.h>
#include "memorymappedfile.h"

#if defined(__WXGTK__)
    #include "wx/gtk/win_gtk.h"
    #include <gdk/gdkx.h>
#endif

// Register the plugin
namespace
{
    PluginRegistrant<CodeSnippets> reg(_T("CodeSnippets"));
};

int idViewSnippets = wxNewId();

// Events handling
BEGIN_EVENT_TABLE(CodeSnippets, cbPlugin)
	EVT_UPDATE_UI(idViewSnippets, CodeSnippets::OnUpdateUI)
	EVT_MENU(idViewSnippets, CodeSnippets::OnViewSnippets)
    EVT_ACTIVATE(            CodeSnippets::OnActivate)
    EVT_IDLE(                CodeSnippets::OnIdle)
    EVT_APP_START_SHUTDOWN(  CodeSnippets::OnRelease)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
CodeSnippets::CodeSnippets()
// ----------------------------------------------------------------------------
{
}

// ----------------------------------------------------------------------------
CodeSnippets::~CodeSnippets()
// ----------------------------------------------------------------------------
{
}

// ----------------------------------------------------------------------------
void CodeSnippets::OnAttach()
// ----------------------------------------------------------------------------
{
    // Initialize one and only Global class
    // Must be done first to allocate config file
    g_pConfig = new CodeSnippetsConfig;

    // initialize version and logging
    m_pAppWin = Manager::Get()->GetAppWindow();
    GetConfig()->AppName = wxT("codesnippets");
    GetConfig()->pMainFrame  = Manager::Get()->GetAppWindow() ;

    AppVersion pgmVersion;

    #if LOGGING
     m_pLog = new wxLogWindow( m_pAppWin, _T(" CodeSnippets Plugin"),true,false);
     wxLog::SetActiveTarget( m_pLog);
     m_pLog->Flush();
     m_pLog->GetFrame()->SetSize(20,30,600,300);
     LOGIT( _T("CodeSnippets Plugin Logging Started[%s]"),pgmVersion.GetVersion().c_str());
    #endif

    // Set current plugin version
	PluginInfo* pInfo = (PluginInfo*)(Manager::Get()->GetPluginManager()->GetPluginInfo(this));
	pInfo->version = pgmVersion.GetVersion();

    // ---------------------------------------
    // determine location of settings
    // ---------------------------------------
    wxStandardPaths stdPaths;
    //memorize the key file name as {%HOME%}\codesnippets.ini
    GetConfig()->m_ConfigFolder = stdPaths.GetUserDataDir();
    //-wxString m_ExecuteFolder = stdPaths.GetDataDir();
     LOGIT( _T("Argv[0][%s] Cwd[%s]"), wxTheApp->argv[0], ::wxGetCwd().GetData() );
    GetConfig()->m_ExecuteFolder = FindAppPath(wxTheApp->argv[0], ::wxGetCwd(), wxEmptyString);

    //GTK GetConfigFolder is returning double "//?, eg, "/home/pecan//.codeblocks"
    // remove the double //s from filename //+v0.4.11
    GetConfig()->m_ConfigFolder.Replace(_T("//"),_T("/"));
    GetConfig()->m_ExecuteFolder.Replace(_T("//"),_T("/"));
    LOGIT(wxT("CfgFolder[%s]"),GetConfig()->m_ConfigFolder.c_str());
    LOGIT(wxT("ExecFolder[%s]"),GetConfig()->m_ExecuteFolder.c_str());

    // if codesnippets.ini is in the executable folder, use it
    // else use the default config folder
    wxString
      m_CfgFilenameStr = GetConfig()->m_ExecuteFolder + wxFILE_SEP_PATH + GetConfig()->AppName + _T(".ini");
    if (::wxFileExists(m_CfgFilenameStr)) {;/*OK Use exe path*/}
    else //use the default.conf folder
    {   m_CfgFilenameStr = GetConfig()->m_ConfigFolder + wxFILE_SEP_PATH + GetConfig()->AppName + _T(".ini");
        // if default doesn't exist, create it
        if (not ::wxDirExists(GetConfig()->m_ConfigFolder))
            ::wxMkdir(GetConfig()->m_ConfigFolder);
    }
    // ---------------------------------------
    // Initialize Globals
    // ---------------------------------------
    GetConfig()->SettingsSnippetsCfgFullPath = m_CfgFilenameStr;
     LOGIT( _T("SettingsSnippetsCfgFullPath[%s]"),GetConfig()->SettingsSnippetsCfgFullPath.c_str() );
    GetConfig()->SettingsLoad();

    // Set Drop targets so we can drag items in/out of the Project/Files Tree ctrls
    // memorize manager of Open files tree
    m_pPrjMan = Manager::Get()->GetProjectManager();
	m_pEdMan  = Manager::Get()->GetEditorManager();
    // set a drop target for the project managers wxFlatNotebook
    m_pPrjMan->GetNotebook()->SetDropTarget(new DropTargets(this));
    // set drop targets on the Project/File tree controls
    m_pEdMan->GetTree()->SetDropTarget(new DropTargets(this));
    SetTreeCtrlHandler( m_pPrjMan->GetTree(), wxEVT_COMMAND_TREE_BEGIN_DRAG );
    SetTreeCtrlHandler( m_pEdMan->GetTree(),  wxEVT_COMMAND_TREE_BEGIN_DRAG );

    m_nOnActivateBusy = 0;
    m_ExternalPid = 0;
    m_pMappedFile = 0;

    // ---------------------------------------
    // load tree icons/images
    // ---------------------------------------
    GetConfig()->pSnipImages = new SnipImages();

    // wait on user to open an external window with the view/snippets menu
    if ( GetConfig()->IsExternalWindow() ) {return;}
    // ---------------------------------------
    // setup snippet tree docking window
    // ---------------------------------------
    CreateSnippetWindow();

}//OnAttach

// ----------------------------------------------------------------------------
void CodeSnippets::OnRelease(bool appShutDown)
// ----------------------------------------------------------------------------
{
    // watch out, CodeBlocks can enter this routine multiple times

    // Unmap and delete the temporary memory mapped communications file
    ReleaseMemoryMappedFile();

    wxString myPid(wxString::Format(wxT("%lu"),::wxGetProcessId()));
    #if defined(__WXMSW__)
        wxString mappedFileName = wxT("/temp/cbsnippetspid") +myPid+ wxT(".plg");
    #else
        wxString mappedFileName = wxT("/tmp/cbsnippetspid") +myPid+  wxT(".plg");
    #endif
    bool result = ::wxRemoveFile( mappedFileName );
    wxUnusedVar(result);
    // ----------------------------------
    if (not GetSnippetsWindow()) return;
    // ----------------------------------

    // Don't close down if file checking is active
    while ( m_nOnActivateBusy )
    {   wxMilliSleep(10) ; wxYield();
    }

    // If floating window, we have to close it or CB crashes
    if ( GetConfig()->IsFloatingWindow() )
    {
        CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
        evt.pWindow = GetSnippetsWindow();
        Manager::Get()->GetAppWindow()->ProcessEvent(evt);

        GetSnippetsWindow()->Destroy();
        SetSnippetsWindow(0);
    }


}

// ----------------------------------------------------------------------------
void CodeSnippets::BuildMenu(wxMenuBar* menuBar)
// ----------------------------------------------------------------------------
{
	int idx = menuBar->FindMenu(_("View"));
	if (idx != wxNOT_FOUND)
	{
		wxMenu* viewMenu = menuBar->GetMenu(idx);
		wxMenuItemList& items = viewMenu->GetMenuItems();

		// Find the first separator and insert before it
		for (size_t i = 0; i < items.GetCount(); ++i)
		{
			if (items[i]->IsSeparator())
			{
				viewMenu->InsertCheckItem(i, idViewSnippets, _("Code snippets"), _("Toggle displaying the code snippets."));
				return;
			}
		}

		// Not found, just append
		viewMenu->AppendCheckItem(idViewSnippets, _("Code snippets"), _("Toggle displaying the code snippets."));
	}
}

// ----------------------------------------------------------------------------
void CodeSnippets::CreateSnippetWindow()
// ----------------------------------------------------------------------------
{
    // Launch the executable if user specified "External" WindowState

    // GetConfig()->m_ExternalPid will contain the resulting launched pgm Pid if successful
   	if ( GetConfig()->GetSettingsWindowState().Contains(wxT("External")) )
    {
        /*bool result =*/ LaunchExternalSnippets();
        return;
    }

    // ---------------------------------------
    // setup snippet tree docking window
    // ---------------------------------------
	SetSnippetsWindow( new CodeSnippetsWindow(GetConfig()->pMainFrame));

    // Floating windows must be set by their parent
   #if !defined(BUILDING_PLUGIN)
    // We can position an application window
    GetSnippetsWindow()->SetSize(GetConfig()->windowXpos, GetConfig()->windowYpos,
            GetConfig()->windowWidth, GetConfig()->windowHeight);
   #endif

	CodeBlocksDockEvent evt(cbEVT_ADD_DOCK_WINDOW);
	evt.name = _T("CodeSnippetsPane");
	evt.title = _(" CodeSnippets");
	evt.pWindow = GetSnippetsWindow();
	evt.desiredSize.Set(300, 400);
	evt.floatingSize.Set(300, 400);
	evt.minimumSize.Set( 30, 40 );
	// assume floating window
    evt.dockSide = CodeBlocksDockEvent::dsFloating;
////    #if defined(__WXMSW__)
////        evt.stretch = false; //must be false for Floating window (MSW)
////    #else
        evt.stretch = true; //must be true for Floating window (GTK)
////    #endif

	if ( GetConfig()->GetSettingsWindowState().Contains(wxT("Docked")) )
	{
        evt.dockSide = CodeBlocksDockEvent::dsLeft;
        evt.stretch = true;
	}

     //LOGIT( _T("CreateSnippetWindow[%s]"), GetConfig()->GetSettingsWindowState().c_str() );
	Manager::Get()->GetAppWindow()->ProcessEvent(evt);


    #if defined(__WXMSW__)
    // Linux never creates an initial floating window. So skip this.
        // Set floating window to last position and size (except linux with buggy wxAUI)
        //if ( evt.dockSide==CodeBlocksDockEvent::dsFloating)
        if ( GetConfig()->IsFloatingWindow())
        {
            GetSnippetsWindow()->GetParent()->SetSize(GetConfig()->windowXpos, GetConfig()->windowYpos,
                GetConfig()->windowWidth, GetConfig()->windowHeight);
            // connect to the wxAUI EVT_CLOSE event
            if ( not GetConfig()->m_pEvtCloseConnect )
            {   GetSnippetsWindow()->GetParent()->Connect( wxEVT_CLOSE_WINDOW,
                        (wxObjectEventFunction)(wxEventFunction)
                        (wxCloseEventFunction) &CodeSnippetsWindow::OnClose,NULL,this);
                GetConfig()->m_pEvtCloseConnect = GetSnippetsWindow()->GetParent();
            }
        }
    #endif

    //    LOGIT( _T("[%s]X[%d]Y[%d]Width[%d]Height[%d]"),
    //            GetSnippetsWindow()->GetParent()->GetName().c_str(),
    //            GetConfig()->windowXpos, GetConfig()->windowYpos,
    //            GetConfig()->windowWidth, GetConfig()->windowHeight);
    //GetSnippetsWindow()->GetParent()->SetSize(20,20,300,400);
    //wxWindow* p = GetSnippetsWindow()->GetParent();
    //int x,y,w,h;
    //p->GetPosition(&x,&y); p->GetSize(&w,&h);
    //LOGIT(  _T("WinPostCreate[%s]X[%d]Y[%d]Width[%d]Height[%d]"), p->GetName().c_str(),x,y,w,h);
    //
    // Set pgm icon again, but from converted images (NB: doesn't work on wxAUI windows)
    //if (GetConfig()->IsFloatingWindow())
    //{   asm("int3");
    //    while (p->GetParent()) p = p->GetParent();
    //    ((wxTopLevelWindow*)p)->SetIcon( GetConfig()->GetSnippetsTreeCtrl()->GetImageList()->GetIcon(TREE_IMAGE_ALL_SNIPPETS) );
    //}


}
// ----------------------------------------------------------------------------
void CodeSnippets::TellExternalSnippetsToTerminate()
// ----------------------------------------------------------------------------
{
    // Get ptr to mapped area and zero my pid as a terminate signal
    // Unmap the file so child can remove it.
    char* pMappedData = (char*)m_pMappedFile->GetStream();
    char onebyte[1] = {0};
    std::strncpy(pMappedData, onebyte, 1);
    wxYield();
}
// ----------------------------------------------------------------------------
bool CodeSnippets::ReleaseMemoryMappedFile()
// ----------------------------------------------------------------------------
{
    // Unmap & delete the memory mapped file used to communicate with the
    // external snippets process
    if ( not m_pMappedFile ) return true;
    if ( m_pMappedFile->IsOk() )
        m_pMappedFile->UnmapFile();
    delete m_pMappedFile;
    m_pMappedFile = 0;

    wxString myPid(wxString::Format(wxT("%lu"),::wxGetProcessId()));
    #if defined(__WXMSW__)
        wxString mappedFileName = wxT("/temp/cbsnippetspid") +myPid+ wxT(".plg");
    #else
        wxString mappedFileName = wxT("/tmp/cbsnippetspid") +myPid+  wxT(".plg");
    #endif
    bool result = ::wxRemoveFile( mappedFileName );
    return result;
}
// ----------------------------------------------------------------------------
void CodeSnippets::SetSnippetsWindow(CodeSnippetsWindow* p)
// ----------------------------------------------------------------------------
{
    GetConfig()->pSnippetsWindow = p;
}

// ----------------------------------------------------------------------------
void CodeSnippets::OnViewSnippets(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
        // ---------------------------------------
        // Check for snippet independent window
        // ---------------------------------------
    if ( GetConfig()->GetSettingsWindowState().Contains(wxT("External")) )
    {
         LOGIT( _T("OnView External m_ExternalPid[%lu] Checked[%d]"), m_ExternalPid, event.IsChecked() );
        if ( (not m_ExternalPid) && event.IsChecked() )
        {    CreateSnippetWindow();
            LOGIT( _T("m_ExternalPid[%lu]"), m_ExternalPid );
            return;
        }
        if ( m_ExternalPid && (not event.IsChecked()) )
        {   // user closing external snippets with View/Snippets menu item
            // Signal, via mapped file, external snippets to terminate
            // LOGIT( _T("m_ExternalPid[%lu]"), m_ExternalPid );
            TellExternalSnippetsToTerminate();
            ReleaseMemoryMappedFile();
            m_ExternalPid = 0;
            return;
        }
    }
    else if ( m_ExternalPid )
    {   // user changed from Independent window to someting else ;
        TellExternalSnippetsToTerminate();
        ReleaseMemoryMappedFile();
        m_ExternalPid = 0;
    }
        // ---------------------------------------
        // setup snippet tree docking window
        // ---------------------------------------

     if (not GetSnippetsWindow())
    {   // Snippets Window is closed, initialize and open it.
        CreateSnippetWindow();
    }

    LOGIT( _T("OnView Floating[%d] Checked[%d] IsShown[%d]"),
            GetConfig()->IsFloatingWindow(),
            event.IsChecked(),
            IsWindowReallyShown(GetSnippetsWindow())
            );

    // hiding window remember last window position
    if ( IsWindowReallyShown(GetSnippetsWindow()) ) do
    {   if (not event.IsChecked()) //hiding window
        {   //wxAUI is so lousy, we have to check that it's *not* giving us
            // the position and size of CodeBlocks.
            if ( GetConfig()->IsFloatingWindow() )
            {
                GetConfig()->SettingsSaveWinPosition();
                if ( GetConfig()->m_pEvtCloseConnect )
                    GetSnippetsWindow()->GetParent()->Disconnect( wxEVT_CLOSE_WINDOW,
                        (wxObjectEventFunction)(wxEventFunction)
                        (wxCloseEventFunction) &CodeSnippetsWindow::OnClose,NULL,this);
                GetConfig()->m_pEvtCloseConnect = 0;
            }

            LOGIT( _T("OnViewSnippets Saving Settings on HideWindow"));
            GetConfig()->SettingsSave();
        }
    }while(0);

	CodeBlocksDockEvent evt(event.IsChecked() ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
	evt.pWindow = GetSnippetsWindow();
	Manager::Get()->GetAppWindow()->ProcessEvent(evt);

    // connect to the wxAUI EVT_CLOSE event
    if ( event.IsChecked() && GetConfig()->IsFloatingWindow()  )
    {   if( not GetConfig()->m_pEvtCloseConnect )
        {   GetSnippetsWindow()->GetParent()->Connect( wxEVT_CLOSE_WINDOW,
                (wxObjectEventFunction)(wxEventFunction)
                (wxCloseEventFunction) &CodeSnippetsWindow::OnClose,NULL,this);
            GetConfig()->m_pEvtCloseConnect = GetSnippetsWindow()->GetParent();
        }
    }//if
}

// ----------------------------------------------------------------------------
void CodeSnippets::OnUpdateUI(wxUpdateUIEvent& /*event*/)
// ----------------------------------------------------------------------------
{

    wxMenuBar* pbar = Manager::Get()->GetAppWindow()->GetMenuBar();

    // check for externally started CodeSnippets
    if (not GetSnippetsWindow() && (not m_ExternalPid) )
    {   pbar->Check(idViewSnippets, false);
         return;
    }
    // check if external CodeSnippets terminated without us
    if ( (not GetSnippetsWindow()) && m_ExternalPid )
    {
        if (not ::wxProcess::Exists(m_ExternalPid))
        {
            ReleaseMemoryMappedFile();
            m_ExternalPid = 0;
        }
        pbar->Check(idViewSnippets, m_ExternalPid);
        return;
    }

    // -----------------------------------
    // Floating or Docked snippets window
    // -----------------------------------
	// Check if the Code Snippets window is visible, if it's not, uncheck the menu item
	if (GetSnippetsWindow() )
    {    pbar->Check(idViewSnippets, IsWindowReallyShown(GetSnippetsWindow()));
        return;
    }

    // check for independent window running
    if (m_ExternalPid)
    {
        pbar->Check(idViewSnippets, m_ExternalPid!=0);

    }

}
// ----------------------------------------------------------------------------
void CodeSnippets::OnIdle(wxIdleEvent& event)
// ----------------------------------------------------------------------------
{
    // Because the current wxAUI is such a lousy interface, we have to poll
    // our docked window to see if it has closed. There are no events
    // that tell us if the user closed the docked or floating window.

    // Unfortunately wxAUI crashes when we close a docked window
    // When user closes docked window with system [x] button, wxAUI crashes
    // This seems to have been fixed in wx2.8.3
    ////    if ( GetConfig()->IsDockedWindow() )
    ////         { event.Skip(); return; }

    // Don't close down if file checking is active
    if (m_nOnActivateBusy) { event.Skip();return; }

    if (not GetSnippetsWindow()) { event.Skip();return; }

    if ( GetSnippetsWindow()->IsTreeBusy() ) {return;}

    bool bOpen = IsWindowReallyShown(GetSnippetsWindow());
    if ( (not bOpen) && GetSnippetsWindow())
    {   //Docked/Floating window is closed. Release our resources
        if ( GetConfig()->m_pEvtCloseConnect
                && GetConfig()->IsFloatingWindow())
        {    GetConfig()->m_pEvtCloseConnect->Disconnect( wxEVT_CLOSE_WINDOW,
                (wxObjectEventFunction)(wxEventFunction)
                (wxCloseEventFunction) &CodeSnippetsWindow::OnClose,NULL,this);
        }
        // dragging a floating to a docked orphans the Connect(EVT_CLOSE)
        // because wxAUI fails to tell us what the user is doing
        // but the Connect()ed window has been Destroyed() by wxAUI anyway.
        GetConfig()->m_pEvtCloseConnect = 0;

        CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
        evt.pWindow = GetSnippetsWindow();
        Manager::Get()->GetAppWindow()->ProcessEvent(evt);

        GetSnippetsWindow()->Destroy();
        SetSnippetsWindow(0);
        LOGIT( _T("OnIdle:SnippetsWindow [%s]"), bOpen?wxT("Open"):wxT("Closed") );
    }

     event.Skip();
}
// ----------------------------------------------------------------------------
void CodeSnippets::OnActivate(wxActivateEvent& event)
// ----------------------------------------------------------------------------
{

    // An application has been activated by the OS

    if ( m_nOnActivateBusy ) {event.Skip();return;}

    ++m_nOnActivateBusy;
    do{
        // check that it's us that was activated
        if (not event.GetActive()) break;

        // Check that CodeSnippets actually has a file open
        if (not GetConfig()->GetSnippetsWindow() )   break;
        if (not GetConfig()->GetSnippetsTreeCtrl() ) break;

        CodeSnippetsWindow* p = GetConfig()->GetSnippetsWindow();
        p->CheckForExternallyModifiedFiles();

    }while(0);
    m_nOnActivateBusy = 0;

    event.Skip();
    return;
}

// ----------------------------------------------------------------------------
// The following are attempts to avoid the OnIdle polling. But they didn't work.
// ----------------------------------------------------------------------------
//void CodeSnippets::OnActivate(wxActivateEvent& event)
//// ----------------------------------------------------------------------------
//{
//    // An application has been activated by the OS
//    // Notes: At routine entry,
//    //      wxFindWindowAtPointer = window to be activated
//    //      wxWindow::FindFocus() = window being deactivated
//    //      each can be null when not a window for this app
//    //      When moving mouse from non-app window to docked window to app
//    //          no EVT_ACTIVATE occurs.
//    //      ::wxGetActiveWindow always returns a ptr to CodeBlocks
//    //      wxTheApp->GetTopWindow always return a ptr to Codeblocks
//
//     LOGIT( _T("-----OnActivate----------[%s]"),event.GetActive()?wxT("Active"):wxT("Deactive") );
//
//    // Wait until codeblocks is fully initialized
//    if (not GetConfig()->pSnippetsWindow) return;
//
//     //if (not event.GetActive()) { event.Skip();return; }
//
//     wxPoint pt;
//     wxWindow* pwMouse = ::wxFindWindowAtPointer( pt );
//     wxWindow* pwSnippet = GetConfig()->pSnippetsWindow;
//     wxWindow* pwFocused = wxWindow::FindFocus();
//     wxWindow* pwTreeCtrl = GetConfig()->pSnippetsWindow->GetSnippetsTreeCtrl();
//      LOGIT( _T("MouseWin   [%p]Name[%s]"),pwMouse, pwMouse?pwMouse->GetName().c_str():wxT(""));
//      LOGIT( _T("FocusedWin [%p]Name[%s]"),pwFocused, pwFocused?pwFocused->GetName().c_str():wxT(""));
//      LOGIT( _T("SnippetWin [%p]Name[%s]"),pwSnippet, pwSnippet->GetName().c_str());
//      LOGIT( _T("TreeCtrWin [%p]Name[%s]"),pwTreeCtrl, pwFocused?pwTreeCtrl->GetName().c_str():wxT(""));
//      wxWindow* pwTreeParent = pwTreeCtrl->GetParent();
//      wxWindow* pwSnipParent = pwSnippet->GetParent();
//      LOGIT( _T("SnippetParent [%p]Name[%s]"),pwSnipParent, pwSnipParent->GetName().c_str());
//      LOGIT( _T("TreeParent [%p]Name[%s]"),pwTreeParent, pwFocused?pwTreeParent->GetName().c_str():wxT(""));
//      if (pwSnipParent)
//      {     wxWindow* pwSnipGrndParent = pwSnipParent->GetParent();
//            if (pwSnipGrndParent)
//             LOGIT( _T("SnippetGrndParent [%p]Name[%s]"),pwSnipGrndParent, pwSnipGrndParent->GetName().c_str());
//      }
//      m_pLog->Flush();
//
//     if (wxWindow::FindFocus() != pwTreeCtrl )
//        {event.Skip(); return;}
//    // Deactivated window was our CodeSnippets TreeCtrl
//    LOGIT( _T(" Activated Plugin") );
//    event.Skip();
//    return;
//    //========================================================
//    // An application has been activated by the OS
//
//    // Wait until codeblocks is fully initialized
//    if (not GetConfig()->pSnippetsWindow) return;
//
//    //LOGIT( _T(" OnActivate Plugin") );
//
//     if (not event.GetActive()) { event.Skip();return; }
//     //Err: wxGetActiveWindow and wxTheApp->GetTopWindow is always the same.
//     //  namely code::blocks
//     if (::wxGetActiveWindow() != wxTheApp->GetTopWindow() )
//        {event.Skip(); return;}
//
//    do
//    {
//        CodeSnippetsWindow* p = GetConfig()->pSnippetsWindow;
//        p->CheckForExternallyModifiedFiles();
//
//    }while(0);
//
//    event.Skip();
//    return;
//}
//// ----------------------------------------------------------------------------
//void CodeSnippets::OnWindowDestroy(wxEvent& event)
//// ----------------------------------------------------------------------------
//{
//    wxWindow* pWindow = (wxWindow*)(event.GetEventObject());
//     LOGIT( _T("OnWindowClose[%p]"), pWindow );
//     event.Skip();
//}
// ----------------------------------------------------------------------------
bool CodeSnippets::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& files)
// ----------------------------------------------------------------------------
{
    // This is the drop routine for the Management Project/Files tree controls
    // It's used to pass files off to MainFrame's drop method
    #ifdef LOGGING
     LOGIT( _T("CodeSnippets::OnDropFiles Entered") );
    #endif //LOGGING
    wxDropTarget*  pMainDrpTgt = GetConfig()->GetMainFrame()->GetDropTarget();
    if (not pMainDrpTgt) {
        #ifdef LOGGING
         LOGIT( _T("CodeSnippets::OnDropFiles Error pMainDrpTgt is %p"),pMainDrpTgt );
        #endif //LOGGING
        return false;
    }//fi
    // invoke MainFrame wxFilesDropTarget
    bool bRC = ((wxMyFileDropTarget*)pMainDrpTgt)->OnDropFiles(x, y, files);
    #ifdef LOGGING
     LOGIT( _T("CodeSnippets::OnDropFiles MainFrame returned %s"), bRC?wxT("True"):wxT("False") );
    #endif //LOGGING
    return bRC;
}
// ----------------------------------------------------------------------------
bool CodeSnippets::GetTreeSelectionData(wxTreeCtrl* pTree, wxString& selString)
// ----------------------------------------------------------------------------
{
    selString = wxEmptyString;

    if (not pTree) { return false; }

    if ( (pTree == m_pPrjMan->GetTree())
        or (pTree == m_pEdMan->GetTree()) )
        {/*ok*/;}
    else{ return false; }
    #ifdef LOGGING
     LOGIT( _T("Focused Tree:%p"),pTree );
    #endif //LOGGING

    // check for a file selection in the treeCtrl
    wxTreeItemId sel = pTree->GetSelection();
    if (not sel) {return false;}
    #ifdef LOGGING
     LOGIT( _T("Selection:%d"), (unsigned int)sel);
    #endif //LOGGING

    // Opened Files Tree
    if ( pTree == m_pEdMan->GetTree() ) {
        selString = m_pEdMan->GetTreeItemFilename( sel );
    }//fi Opened Files Tree

    // Project Tree
    if ( pTree == m_pPrjMan->GetTree() ) {
        // create a string from highlighted Project treeCtrl item

        // Workspace/root
        if (sel && sel == pTree->GetRootItem())
        {   // selected workspace
            cbWorkspace* pWorkspace = m_pPrjMan->GetWorkspace();
            if (not pWorkspace) {return false;}
            selString = pWorkspace->GetFilename();
            return (not selString.IsEmpty());
        }//workspace

        FileTreeData* ftd = (FileTreeData*)pTree->GetItemData(sel);
        if (not ftd){return false;}

        // if it is a project...
        if (ftd->GetKind() == FileTreeData::ftdkProject)
        {
            cbProject* pPrj = ftd->GetProject();
            if (pPrj) selString = pPrj->GetFilename();
        }//Project

        // if it is a file...
        if (ftd->GetKind() == FileTreeData::ftdkFile)
        {
            // selected project file
           ProjectFile* pPrjFile = ftd->GetProjectFile();
            if (not pPrjFile) { return false;}
            selString = pPrjFile->file.GetFullPath();
        }//file
    }//fi Project Tree

    return (not selString.IsEmpty() ) ;

}//GetTreeSelectionData
// ----------------------------------------------------------------------------
wxArrayString* CodeSnippets::TextToFilenames(const wxString& str)
// ----------------------------------------------------------------------------
{
    // convert text to filenames acceptable to MainFrame Projects panel drop target

    wxArrayString* pFilenames = new wxArrayString;

    if (0 == str.Freq('\r'))
        pFilenames->Add(str);
    else{ // parse big string into individual filenames
        wxString ostr;
        for (size_t i=0; i<str.Length(); i++ ) {
            if (str[i] != '\r') ostr.Append(str[i]);
            else{
                pFilenames->Add(ostr);
                ostr.Empty();
                if ( ((i+1)<str.Length()) && (str[i+1] == '\n') )
                    i++;    //skip over newline
            }//esle
        }//rof
        if (not ostr.IsEmpty()) pFilenames->Add(ostr);
    }//esle

    // verify filenames exists
    for (size_t i=0;i<pFilenames->GetCount(); i++ ) {
        #ifdef LOGGING
         LOGIT( wxT("CodeSnippets::OnPaste:Text converted to filename %d:[%s]"), i,pFilenames->Item(i).GetData());
        #endif //LOGGING
        if (not wxFileExists(pFilenames->Item(i))) {
             pFilenames->RemoveAt(i);
             i--;
        }
    }

    return pFilenames;
}
// ----------------------------------------------------------------------------
// DropTargets
// ----------------------------------------------------------------------------
DropTargets::DropTargets (CodeSnippets* pcbDndExtn)
           : wxDropTarget ()
// ----------------------------------------------------------------------------
{
    //constructor

    m_pcbDndExtn = pcbDndExtn;

    // create targets
    m_file = new wxFileDataObject();
    m_text = new wxTextDataObject();

    // set composite target
    DropTargetsComposite *data = new DropTargetsComposite();
    data->Add ((wxDataObjectSimple*)m_file);
    data->Add ((wxDataObjectSimple*)m_text, true); // set as preferred
    SetDataObject (data);
    #ifdef LOGGING
     LOGIT( wxT("DropTargets::ctor set") );
    #endif //LOGGING

}

// ----------------------------------------------------------------------------
// drop handlers
// ----------------------------------------------------------------------------
wxDragResult DropTargets::OnData (wxCoord x, wxCoord y, wxDragResult def)
// ----------------------------------------------------------------------------
{
    //wxDropTarget::OnData
    //virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def)
    //Called after OnDrop returns true. By default this will usually GetData
    // and will return the suggested default value def.

    // This is a composite drop target; handles both text and filenames
    #ifdef LOGGING
     LOGIT( wxT("DropTargets::OnData") );
    #endif //LOGGING
    if (!GetData()) return wxDragNone;

    // get object
    wxDataObjectSimple *obj = ((DropTargetsComposite *)GetDataObject())->GetLastDataObject();

    if (obj == ((wxDataObjectSimple*)m_file)) {
        if (not OnDataFiles (x, y, m_file->GetFilenames())) def = wxDragNone;
    }else if (obj == ((wxDataObjectSimple*)m_text)) {
        if (not OnDataText (x, y, m_text->GetText())) def = wxDragNone;
    }else{
        def = wxDragNone;
    }

    return def;
}

// ----------------------------------------------------------------------------
bool DropTargets::OnDataFiles (wxCoord WXUNUSED(x), wxCoord WXUNUSED(y),
                               const wxArrayString& filenames)
// ----------------------------------------------------------------------------
{
    #ifdef LOGGING
     LOGIT( wxT("DropTargets::OnDataFiles") );
    #endif //LOGGING
    m_pcbDndExtn->OnDropFiles(1, 1, filenames);
    return TRUE;
}

// ----------------------------------------------------------------------------
bool DropTargets::OnDataText (wxCoord x, wxCoord y, const wxString& data)
// ----------------------------------------------------------------------------
{
    // convert text string to filename array and drop on target
    #ifdef LOGGING
     LOGIT( wxT("DropTargets::OnDataText") );
    #endif //LOGGING
    bool ok;
    wxArrayString* pFilenames = m_pcbDndExtn->TextToFilenames(data);
    if (pFilenames->GetCount()) ok = m_pcbDndExtn->OnDropFiles(1, 1, *pFilenames);
    delete pFilenames;
    //return ok;
    // ---------------------------------------------------
    // return false so source data doesn't get cut||deleted
    // ---------------------------------------------------
    return false;
}

// ----------------------------------------------------------------------------
wxDragResult DropTargets::OnDragOver (wxCoord x, wxCoord y, wxDragResult def)
// ----------------------------------------------------------------------------
{
    //wxDropTarget::OnDragOver
    //virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
    //Called when the mouse is being dragged over the drop target. By default,
    // this calls functions return the suggested return value def.

    #ifdef LOGGING
     LOGIT( wxT("DropTargets::OnDragOver") );
    #endif //LOGGING
    return wxDragCopy;
}

// ----------------------------------------------------------------------------
wxDragResult DropTargets::OnEnter (wxCoord x, wxCoord y, wxDragResult def)
// ----------------------------------------------------------------------------
{
    //wxDropTarget::OnEnter
    //virtual wxDragResult OnEnter(wxCoord x, wxCoord y, wxDragResult def)
    //Called when the mouse enters the drop target. By default, this calls OnDragOver.
    //Better stated: Called when the mouse is down and dragging inside the drop target.

    #ifdef LOGGING
     LOGIT( wxT("DropTargets::OnEnter") );
    #endif //LOGGING
    return wxDragCopy;
}

// ----------------------------------------------------------------------------
void DropTargets::OnLeave()
// ----------------------------------------------------------------------------
{
    //wxDropTarget::OnLeave
    //virtual void OnLeave()
    //Called when the mouse leaves the drop target.
    //Actually: when the mouse leaves the window with the drop target.

    #ifdef LOGGING
     LOGIT( wxT("DropTargets::OnLeave") );
    #endif //LOGGING
}
// ----------------------------------------------------------------------------
//   Events and Event support routines
// ----------------------------------------------------------------------------
void CodeSnippets::OnTreeCtrlEvent(wxTreeEvent& event)
// ----------------------------------------------------------------------------
{
    // Drag event.
    // When user drags left mouse key out of the Projects/Files tree,
    // create a dropSource from the focused treeCtrl item

    if (not m_IsAttached) {event.Skip(); return;}

    // Events enabled from OnInit() by:
    // SetTreeCtrlHandler(m_pPrjMan->GetTree(), wxEVT_TREE_DRAG_BEGIN );

    // allow all others to process first
    event.Skip();

    wxTreeCtrl* pTree = (wxTreeCtrl*)event.GetEventObject();

    #ifdef LOGGING
    	 //LOGIT( wxT("CodeSnippets::OnTreeCtrlEvent %p"), pTree );
    #endif //LOGGING

    // -----------------------
    // TREE_BEGIN_DRAG
    // -----------------------

    if (event.GetEventType() == wxEVT_COMMAND_TREE_BEGIN_DRAG)
    {
        #ifdef LOGGING
         LOGIT( _T("TREE_BEGIN_DRAG %p"), pTree );
        #endif //LOGGING

        if (pTree == (wxTreeCtrl*)m_pPrjMan->GetTree())
        {

            m_pMgtTreeBeginDrag = pTree;
            m_TreeMousePosn = ::wxGetMousePosition();
            m_TreeItemId    =   event.GetItem();
            pTree->SelectItem(m_TreeItemId);
        }
        m_TreeText = wxEmptyString;
        if (not GetTreeSelectionData(pTree, m_TreeText))
        {
            m_TreeText = wxEmptyString;
            m_pMgtTreeBeginDrag = 0;
        }
        return;
    }//fi
    // -----------------------
    // TREE_END_DRAG
    // -----------------------
    if (event.GetEventType() == wxEVT_COMMAND_TREE_END_DRAG)
    {
        #ifdef LOGGING
         LOGIT( _T("TREE_END_DRAG %p"), pTree );
        #endif //LOGGING

        m_TreeText = wxEmptyString;
        if (pTree == (wxTreeCtrl*)m_pPrjMan->GetTree())
           m_pMgtTreeBeginDrag = 0;
        return;
    }
    // -----------------------
    // LEAVE_WINDOW
    // -----------------------
    if (event.GetEventType() == wxEVT_LEAVE_WINDOW)
    {
        //user has dragged mouse out of Management/File window
        #ifdef LOGGING
         //LOGIT( _T("MOUSE EVT_LEAVE_WINDOW") );
        #endif //LOGGING

        // Left mouse key must be down (dragging)
        if (not ((wxMouseEvent&)event).LeftIsDown() ) return;
        // check if data is available
        if ( m_TreeText.IsEmpty()) return;

        #ifdef LOGGING
         LOGIT( _T("TREE_LEAVE_WINDOW %p"), pTree );
        #endif //LOGGING

        // we now have data, create both a simple text and filename drop source
        wxTextDataObject* textData = new wxTextDataObject();
        wxFileDataObject* fileData = new wxFileDataObject();
            // fill text and file sources with snippet
        wxDropSource textSource( *textData, (wxWindow*)event.GetEventObject() );
        textData->SetText( m_TreeText );
        wxDropSource fileSource( *fileData, (wxWindow*)event.GetEventObject() );
        fileData->AddFile( m_TreeText );
            // set composite data object to contain both text and file data
        wxDataObjectComposite *data = new wxDataObjectComposite();
        data->Add( (wxDataObjectSimple*)textData );
        data->Add( (wxDataObjectSimple*)fileData, true ); // set file data as preferred
            // create the drop source containing both data types
        wxDropSource source( *data, (wxWindow*)event.GetEventObject()  );
        #ifdef LOGGING
         LOGIT( _T("DropSource Text[%s],File[%s]"),
                    textData->GetText().GetData(),
                    fileData->GetFilenames().Item(0).GetData() );
        #endif //LOGGING

        // allow both copy and move
        int flags = 0;
        flags |= wxDrag_AllowMove;

        wxDragResult result = source.DoDragDrop(flags);

        #if LOGGING
            wxString pc;
            switch ( result )
            {
                case wxDragError:   pc = _T("Error!");    break;
                case wxDragNone:    pc = _T("Nothing");   break;
                case wxDragCopy:    pc = _T("Copied");    break;
                case wxDragMove:    pc = _T("Moved");     break;
                case wxDragCancel:  pc = _T("Cancelled"); break;
                default:            pc = _T("Huh?");      break;
            }
            LOGIT( wxT("CodeSnippets::OnLeftDown DoDragDrop returned[%s]"),pc.GetData() );
        #else
            wxUnusedVar(result);
        #endif

        // ---MSW WORKAROUNG --------------------------------------------------
        // Since we dragged outside the tree control with an EVT_TREE_DRAG_BEGIN
        // pending, Wx will *not* send the EVT_TREE_DRAG_END from a
        // mouse key up event until the user re-clicks inside the tree control.
        // The mouse is still captured and the tree exibits very bad behavior.

        // Send an mouse_key_up to the tree control so it releases the
        // mouse and and receives a EVT_TREE_DRAG_END event.

        if ( m_pMgtTreeBeginDrag )
        {
            //send Mouse LeftKeyUp to Tree Control window
            #ifdef LOGGING
             LOGIT( _T("Sending Mouse LeftKeyUp") );
            #endif //LOGGING

            // move mouse into the Tree control
            wxPoint CurrentMousePosn = ::wxGetMousePosition();
            #if defined(__WXMSW__)
                MSW_MouseMove( m_TreeMousePosn.x, m_TreeMousePosn.y );

                // send mouse LeftKeyUp
                INPUT Input         = {0};
                Input.type          = INPUT_MOUSE;
                Input.mi.dwFlags    = MOUSEEVENTF_LEFTUP;
                ::SendInput(1,&Input,sizeof(INPUT));

                // put mouse back in drag-end position
                MSW_MouseMove( CurrentMousePosn.x, CurrentMousePosn.y );
            #endif //(__WXMSW__)
            #if defined(__WXGTK__)
                // move cursor to source window and send a left button up event
                XWarpPointer (GDK_WINDOW_XDISPLAY(GDK_ROOT_PARENT()),
                        None,              /* not source window -> move from anywhere */
                        GDK_WINDOW_XID(GDK_ROOT_PARENT()),  /* dest window */
                        0, 0, 0, 0,        /* not source window -> move from anywhere */
                        m_TreeMousePosn.x, m_TreeMousePosn.y );
                // send LeftMouseRelease key
                m_pMgtTreeBeginDrag->SetFocus();
                GdkDisplay* display = gdk_display_get_default ();
                int xx=0,yy=0;
                GdkWindow* pGdkWindow = gdk_display_get_window_at_pointer( display, &xx, &yy);
                // LOGIT(wxT("Tree[%p][%d %d]"), m_pEvtTreeCtrlBeginDrag,m_TreeMousePosn.x, m_TreeMousePosn.y);
                // LOGIT(wxT("gdk [%p][%d %d]"), pWindow, xx, yy);
                GdkEventButton evb;
                memset(&evb, 0, sizeof(evb));
                evb.type = GDK_BUTTON_RELEASE;
                evb.window = pGdkWindow;
                evb.x = xx;
                evb.y = yy;
                evb.state = GDK_BUTTON1_MASK;
                evb.button = 1;
                // gdk display put event, namely mouse release
                gdk_display_put_event( display, (GdkEvent*)&evb);
                // put mouse back in pre-moved "dropped" position
                XWarpPointer (GDK_WINDOW_XDISPLAY(GDK_ROOT_PARENT()),
                        None,              /* not source window -> move from anywhere */
                        GDK_WINDOW_XID(GDK_ROOT_PARENT()),  /* dest window */
                        0, 0, 0, 0,        /* not source window -> move from anywhere */
                        CurrentMousePosn.x, CurrentMousePosn.y );
            #endif//(__WXGTK__)

        }

        delete textData ;
        delete fileData ;

        m_pMgtTreeBeginDrag = 0;
        m_TreeText = wxEmptyString;
    }//fi event.GetEventType()

    return;

}//OnTreeCtrlEvent
// ----------------------------------------------------------------------------
#if defined(__WXMSW__)
void CodeSnippets::MSW_MouseMove(int x, int y )
// ----------------------------------------------------------------------------
{
    // Move the MSW mouse to absolute screen coords x,y
      double fScreenWidth   = ::GetSystemMetrics( SM_CXSCREEN )-1;
      double fScreenHeight  = ::GetSystemMetrics( SM_CYSCREEN )-1;
      double fx = x*(65535.0f/fScreenWidth);
      double fy = y*(65535.0f/fScreenHeight);
      INPUT  Input={0};
      Input.type      = INPUT_MOUSE;
      Input.mi.dwFlags  = MOUSEEVENTF_MOVE|MOUSEEVENTF_ABSOLUTE;
      Input.mi.dx = (LONG)fx;
      Input.mi.dy = (LONG)fy;
      ::SendInput(1,&Input,sizeof(INPUT));
}
#endif
// ----------------------------------------------------------------------------
void CodeSnippets::SetTreeCtrlHandler(wxWindow *p, WXTYPE eventType)
// ----------------------------------------------------------------------------
{
	if (!p ) return;		// already attached !!!

    #ifdef LOGGING
	 LOGIT(wxT("CodeSnippets::Attach - attaching to [%s] %p"), p->GetName().c_str(),p);
    #endif //LOGGING

    p->Connect(wxEVT_COMMAND_TREE_BEGIN_DRAG,       //eg.,wxEVT_LEAVE_WINDOW,
                     wxTreeEventHandler(CodeSnippets::OnTreeCtrlEvent),
                     NULL, this);
    p->Connect(wxEVT_COMMAND_TREE_END_DRAG,       //eg.,wxEVT_LEAVE_WINDOW,
                     wxTreeEventHandler(CodeSnippets::OnTreeCtrlEvent),
                     NULL, this);
    p->Connect(wxEVT_LEAVE_WINDOW,       //eg.,wxEVT_LEAVE_WINDOW,
                     wxTreeEventHandler(CodeSnippets::OnTreeCtrlEvent),
                     NULL, this);
}
// ----------------------------------------------------------------------------
void CodeSnippets::RemoveTreeCtrlHandler(wxWindow *p, WXTYPE eventType)
// ----------------------------------------------------------------------------
{
	if (!p ) return;		// already attached !!!

    #ifdef LOGGING
	 LOGIT(wxT("CodeSnippets::Detach - detaching to [%s] %p"), p->GetName().c_str(),p);
    #endif //LOGGING

    p->Disconnect(wxEVT_COMMAND_TREE_BEGIN_DRAG,       //eg.,wxEVT_LEAVE_WINDOW,
                     wxTreeEventHandler(CodeSnippets::OnTreeCtrlEvent),
                     NULL, this);
    p->Disconnect(wxEVT_COMMAND_TREE_END_DRAG,       //eg.,wxEVT_LEAVE_WINDOW,
                     wxTreeEventHandler(CodeSnippets::OnTreeCtrlEvent),
                     NULL, this);
    p->Disconnect(wxEVT_LEAVE_WINDOW,       //eg.,wxEVT_LEAVE_WINDOW,
                     wxTreeEventHandler(CodeSnippets::OnTreeCtrlEvent),
                     NULL, this);
}
// ----------------------------------------------------------------------------
wxString CodeSnippets::FindAppPath(const wxString& argv0, const wxString& cwd, const wxString& appVariableName)
// ----------------------------------------------------------------------------
{
    // Find the absolute path where this application has been run from.
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
        LOGIT( _T("FindAppPath: AbsolutePath[%s]"), wxPathOnly(argv0Str).GetData() );
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
            LOGIT( _T("FindAppPath: RelativePath[%s]"), wxPathOnly(str).GetData() );
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
        LOGIT( _T("FindAppPath: SearchPath[%s]"), wxPathOnly(str).GetData() );
        return wxPathOnly(str);
    }

    // Failed
     LOGIT(  _T("FindAppPath: Failed, returning cwd") );
    return wxEmptyString;
    //return cwd;
}
// ----------------------------------------------------------------------------
int CodeSnippets::LaunchProcess(const wxString& cmd, const wxString& cwd)
// ----------------------------------------------------------------------------
{

    #if defined(__WXMSW__)
        // Append DLL folder to MSW path
        wxString mswPath;
        wxGetEnv(wxT("PATH"),&mswPath);
        mswPath = mswPath + wxT(";") + GetConfig()->m_ExecuteFolder;
        wxSetEnv(_T("PATH"), mswPath);
        wxGetEnv(wxT("PATH"),&mswPath);
         LOGIT( _T("Path set to[%s]"), mswPath.c_str() );
    #endif

    #ifndef __WXMSW__
        // setup dynamic linker path
    #if defined(__APPLE__) && defined(__MACH__)
        wxSetEnv(_T("DYLD_LIBRARY_PATH"), _T(".:$DYLD_LIBRARY_PATH"));
    #else
        wxSetEnv(_T("LD_LIBRARY_PATH"), _T(".:$LD_LIBRARY_PATH"));
    #endif // __APPLE__ && __MACH__
    #endif

    // start codesnippets
    Manager::Get()->GetMessageManager()->DebugLog( _("Starting program:")+cmd);
    m_ExternalPid = wxExecute(cmd, wxEXEC_ASYNC);
     LOGIT( _T("Launch [%s\n] from [%s]\n] Pid[%lu]"), cmd.c_str(), cwd.c_str(), m_ExternalPid );

    #if defined(__WXMAC__)
        if (m_ExternalPid == -1)
        {
            // Great! We got a fake PID. Time to Go Fish with our "ps" rod:

            m_ExternalPid = 0;
            pid_t mypid = getpid();
            wxString mypidStr;
            mypidStr << mypid;

            long pspid = 0;
            wxString psCmd;
            wxArrayString psOutput;
            wxArrayString psErrors;

            psCmd << wxT("/bin/ps -o ppid,pid,command");
            Manager::Get()->GetMessageManager()->DebugLog(wxString::Format( _("Executing: %s"), psCmd.c_str()) );
            int result = wxExecute(psCmd, psOutput, psErrors, wxEXEC_SYNC);

            mypidStr << wxT(" ");

            for (int i = 0; i < psOutput.GetCount(); ++i)
            { //  PPID   PID COMMAND
               wxString psLine = psOutput.Item(i);
               if (psLine.StartsWith(mypidStr) && psLine.Contains(wxT("codesnippets")))
               {
                   wxString pidStr = psLine.Mid(mypidStr.Length());
                   pidStr = pidStr.BeforeFirst(' ');
                   if (pidStr.ToLong(&pspid))
                   {
                       m_ExternalPid = pspid;
                       break;
                   }
               }
             }

            for (int i = 0; i < psErrors.GetCount(); ++i)
                Manager::Get()->GetMessageManager()->DebugLog(wxString::Format( _("PS Error:%s"), psErrors.Item(i).c_str()) );

        }//if(m_ExternalPid == -1)
    #endif

    if (!m_ExternalPid)
    {
        //Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("failed"));
        Manager::Get()->GetMessageManager()->DebugLog( _("failed"));
        return -1;
    }
    //Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("done"));
    Manager::Get()->GetMessageManager()->DebugLog( _("done"));
    return 0;
}
// ----------------------------------------------------------------------------
long CodeSnippets::LaunchExternalSnippets()
// ----------------------------------------------------------------------------
{
   // Launch the executable if user specified "External" WindowState

    // First, create a memory mapped file with our Pid in it.
    // The launched process will check for the Pid to disappear
    // and terminate, saving its data and conf settings

    // deallocate any previously mapped file
    ReleaseMemoryMappedFile();

    // make a unique mapped file name with my pid
    wxString myPid(wxString::Format(wxT("%lu"),::wxGetProcessId()));
    // To memory map a file there must exists a non-zero length file
    #if defined(__WXMSW__)
        wxString mappedFileName = wxT("/temp/cbsnippetspid") +myPid+ wxT(".plg");
    #else
        wxString mappedFileName = wxT("/tmp/cbsnippetspid") +myPid+  wxT(".plg");
    #endif
    wxFile mappedFile;
    mappedFile.Create( mappedFileName, true);
    char buf[1024] = {0};
    mappedFile.Write(buf,1024);
    mappedFile.Close();
    m_pMappedFile = 0;

    // Map the file
    m_pMappedFile = new  wxMemoryMappedFile( mappedFileName, false);
    if ( not m_pMappedFile ) { return -1;}
    if ( not m_pMappedFile->IsOk() )
    {
        messageBox(wxString::Format(wxT("Error %d allocating\n%s\n\n"), m_pMappedFile->GetLastError(), mappedFileName.GetData() ));
        return -1;
    }
    // Get ptr to mapped area and write my pid as a semaphore flag
    char* pMappedData = (char*)m_pMappedFile->GetStream();
    std::strncpy(pMappedData, cbU2C(myPid), myPid.Length());
    //pMappedFile->UnmapFile(); will deallocate the file (so will the dtor)

    // Launch the external process
    wxString PgmFullPath =
                    GetConfig()->m_ExecuteFolder
                    + wxT("/share/codeblocks/plugins/codesnippets");
    wxString pgmArgs( wxString::Format( wxT("KeepAlivePid=%lu"), ::wxGetProcessId() ) );
    wxString command = PgmFullPath + wxT(" ") + pgmArgs;

     LOGIT( _T("Launching[%s]"), command.GetData());
    bool result = LaunchProcess(command, wxGetCwd());
     LOGIT( _T("Launch Result[%d] m_ExternalPid[%lu]"),result, m_ExternalPid );
    if ( 0 != result )
    {  wxString msg(wxString::Format(wxT("Error [%d] Launching\n %s\n"),result, PgmFullPath.c_str()));
       messageBox( msg );
    }

    return result;
}
// ----------------------------------------------------------------------------

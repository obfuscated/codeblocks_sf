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
// $Id$

#include <cstring>
#include "sdk.h"
#ifndef CB_PRECOMP
	#include <wx/event.h>
	#include <wx/frame.h> // Manager::Get()->GetAppWindow()
	#include <wx/intl.h>
	#include <wx/menu.h>
	#include <wx/menuitem.h>
	#include <wx/string.h>
	#include "sdk_events.h"
	#include "manager.h"
	#include "configmanager.h"
	#include "projectmanager.h"
	#include "personalitymanager.h"
	#include "cbworkspace.h"
	#include "cbproject.h"
	#include "logmanager.h"
	#include "editorbase.h"
#endif
	#include <wx/stdpaths.h>
	#include <wx/process.h>

    #include <wx/dnd.h>
    #include <wx/utils.h>

#include "version.h"
#include "codesnippets.h"
#include "codesnippetswindow.h"
#include "snippetsconfig.h"
#include "GenericMessageBox.h"
#include "cbauibook.h"
#include "dragscrollevent.h"

// ----------------------------------------------------------------------------
namespace
// ----------------------------------------------------------------------------
{
    // Register the plugin
    PluginRegistrant<CodeSnippets> reg(_T("CodeSnippets"));

    // Duplicated from OpenFilesList plugin in order to drag out
    // full filenames from CB tree items to CodeSnippets tree
    class OpenFilesListData : public wxTreeItemData
    {
    public:
        OpenFilesListData(EditorBase* ed) : ed(ed) {}
        EditorBase* GetEditor() const
        {
            return ed;
        }
    private:
        EditorBase* ed;
    };

};
int idViewSnippets = wxNewId();

// Events handling
BEGIN_EVENT_TABLE(CodeSnippets, cbPlugin)
	EVT_UPDATE_UI(idViewSnippets, CodeSnippets::OnUpdateUI)
	EVT_MENU(idViewSnippets, CodeSnippets::OnViewSnippets)
    EVT_ACTIVATE(            CodeSnippets::OnActivate)
    //-EVT_IDLE(                CodeSnippets::OnIdle) replaced by Connect()
    //EVT_APP_START_SHUTDOWN(  CodeSnippets::OnRelease)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
CodeSnippets::CodeSnippets()
// ----------------------------------------------------------------------------
{
    m_bMouseCtrlKeyDown = false;
    m_bMouseLeftKeyDown = false;
    m_bMouseIsDragging = false;
    m_bDragCursorOn = false;
    m_MouseDownX = m_MouseDownY = 0;
    m_MouseUpX = m_MouseUpY = 0;
    #if !wxCHECK_VERSION(2, 8, 12)
    m_prjTreeItemAtKeyUp = m_prjTreeItemAtKeyDown= 0;
    #endif
    m_bMouseExitedWindow = false;
    m_bBeginInternalDrag = false;
    m_pDragCursor = new wxCursor(wxCURSOR_HAND);

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

    // Do not allow a secondary plugin enable
    //if (g_pConfig){
    if (GetConfig()){
        wxMessageBox(wxT("CodeSnippets will enable on CodeBlocks restart."));
        return;
    }

    // Initialize one and only Global class
    // Must be done first to allocate config file
    //-g_pConfig = new CodeSnippetsConfig;
    SetConfig( new CodeSnippetsConfig);

    GetConfig()->m_bIsPlugin = true;

    GetConfig()->SetOpenFilesList( 0);
    m_nDragScrollEventId = 0;

    // initialize version and logging
    m_pAppWin = Manager::Get()->GetAppWindow();
    GetConfig()->AppName = wxT("codesnippets");
    GetConfig()->pMainFrame  = Manager::Get()->GetAppWindow() ;
    GetConfig()->m_pMenuBar = Manager::Get()->GetAppFrame()->GetMenuBar();

    AppVersion pgmVersion;

    #if LOGGING
     wxLog::EnableLogging(true);
     m_pLog = new wxLogWindow( m_pAppWin, _T(" CodeSnippets Plugin"),true,false);
     wxLog::SetActiveTarget( m_pLog);
     m_pLog->Flush();
     m_pLog->GetFrame()->SetSize(20,30,600,300);
     #if defined(LOGGING)
     LOGIT( _T("CodeSnippets Plugin Logging Started[%s]"),pgmVersion.GetVersion().c_str());
     #endif
    #endif

    // Set current plugin version
	PluginInfo* pInfo = (PluginInfo*)(Manager::Get()->GetPluginManager()->GetPluginInfo(this));
	pInfo->version = pgmVersion.GetVersion();

    // ---------------------------------------
    // determine location of settings
    // ---------------------------------------
    //memorize the key file name as {%HOME%}\codesnippets.ini
    GetConfig()->m_ConfigFolder = GetCBConfigDir();
    #if defined(LOGGING)
     LOGIT( _T("Argv[0][%s] Cwd[%s]"), wxTheApp->argv[0], ::wxGetCwd().GetData() );
    #endif
    GetConfig()->m_ExecuteFolder = FindAppPath(wxTheApp->argv[0], ::wxGetCwd(), wxEmptyString);

    // remove the double //s from filenames
    GetConfig()->m_ConfigFolder.Replace(_T("//"),_T("/"));
    GetConfig()->m_ExecuteFolder.Replace(_T("//"),_T("/"));
    #if defined(LOGGING)
    LOGIT(wxT("CfgFolder[%s]"),GetConfig()->m_ConfigFolder.c_str());
    LOGIT(wxT("ExecFolder[%s]"),GetConfig()->m_ExecuteFolder.c_str());
    #endif

    // get the CodeBlocks "personality" argument
    wxString m_Personality = Manager::Get()->GetPersonalityManager()->GetPersonality();
    if (m_Personality == wxT("default")) m_Personality = wxEmptyString;
    #if defined(LOGGING)
     LOGIT( _T("Personality is[%s]"), m_Personality.GetData() );
    #endif

    // if codesnippets.ini is in the executable folder, use it
    // else use the config folder
    wxString m_CfgFilenameStr = GetConfig()->m_ExecuteFolder + wxFILE_SEP_PATH;
    if (not m_Personality.IsEmpty()) m_CfgFilenameStr << m_Personality + wxT(".") ;
    m_CfgFilenameStr << GetConfig()->AppName + _T(".ini");

    if (::wxFileExists(m_CfgFilenameStr)) {;/*OK Use exe path*/}
    else // use the .conf folder
    {   m_CfgFilenameStr = GetConfig()->m_ConfigFolder + wxFILE_SEP_PATH;
        if (not m_Personality.IsEmpty()) m_CfgFilenameStr <<  m_Personality + wxT(".") ;
        m_CfgFilenameStr << GetConfig()->AppName + _T(".ini");
        // if default doesn't exist, create it
        if (not ::wxDirExists(GetConfig()->m_ConfigFolder))
            ::wxMkdir(GetConfig()->m_ConfigFolder);
    }
    // ---------------------------------------
    // Initialize Globals
    // ---------------------------------------
    GetConfig()->SettingsSnippetsCfgPath = m_CfgFilenameStr;
    #if defined(LOGGING)
     LOGIT( _T("SettingsSnippetsCfgPath[%s]"),GetConfig()->SettingsSnippetsCfgPath.c_str() );
    #endif
    GetConfig()->SettingsCBConfigPath = GetConfig()->m_ConfigFolder; //default
    wxString cbConfigPath = GetConfig()->m_ExecuteFolder + wxFILE_SEP_PATH + _T("default.conf");
    if (::wxFileExists(cbConfigPath))
        GetConfig()->SettingsCBConfigPath = GetConfig()->m_ExecuteFolder;

    GetConfig()->SettingsLoad();

    // Set Drop targets so we can drag items in/out of the Project/Files Tree ctrls
    // memorize manager of Open files tree
    m_pProjectMgr = Manager::Get()->GetProjectManager();
    // set a drop target for the project managers wxAuiNotebook/cbAuiNotebook
    m_pProjectMgr->GetUI().GetNotebook()->SetDropTarget(new DropTargets(this));

    //NB: On Linux, we don't enable dragging out of the file windows because of the drag/drop freeze bug
    #if defined(__WXMSW__)
        wxTreeCtrl* pPrjTree = m_pProjectMgr->GetUI().GetTree();
        m_oldCursor = pPrjTree->GetCursor();
        SetTreeCtrlHandler( pPrjTree, wxEVT_COMMAND_TREE_BEGIN_DRAG );
    #endif
    GetConfig()->SetOpenFilesList( FindOpenFilesListWindow() );
    if (GetConfig()->GetOpenFilesList() )
    {
        // set drop targets on the OpenFilesList tree control
        GetConfig()->GetOpenFilesList()->SetDropTarget(new DropTargets(this));
        //NB: On Linux, we don't enable dragging out of the file windows because of the drag/drop freeze bug
        #if defined(__WXMSW__)
            // set event hooks
            SetTreeCtrlHandler( GetConfig()->GetOpenFilesList(),  wxEVT_COMMAND_TREE_BEGIN_DRAG );
        #endif
    }//if

    m_nOnActivateBusy = 0;
    m_ExternalPid = 0;
    m_KeepAliveFileName = wxEmptyString;
    GetConfig()->m_appIsShutdown = false;
    GetConfig()->m_appIsDisabled = false;


    // ---------------------------------------
    // load tree icons/images
    // ---------------------------------------
    GetConfig()->pSnipImages = new SnipImages();

    //- If Codesnippets is running as an external application
    //- wait on user to open an external window with the view/snippets menu
    //-if ( GetConfig()->IsExternalWindow() ) {return;}

    // ---------------------------------------
    // setup snippets tree docking window
    // ---------------------------------------
    if ( not GetConfig()->IsExternalWindow() )
        CreateSnippetWindow();

    #if defined(LOGGING)
    //LOGIT(wxT("idViewSnippets[%d]"), idViewSnippets);
    #endif
    // ---------------------
	// register event sink
    // ---------------------
	// request app to switch view layout - BUG: this is not being issued on View/Layout change
    Manager::Get()->RegisterEventSink(cbEVT_SWITCH_VIEW_LAYOUT, new cbEventFunctor<CodeSnippets, CodeBlocksLayoutEvent>(this, &CodeSnippets::OnSwitchViewLayout));
    // app notifies that a new layout has been applied
    Manager::Get()->RegisterEventSink(cbEVT_SWITCHED_VIEW_LAYOUT, new cbEventFunctor<CodeSnippets, CodeBlocksLayoutEvent>(this, &CodeSnippets::OnSwitchedViewLayout));
    // app notifies that a docked window has been hidden/shown
    Manager::Get()->RegisterEventSink(cbEVT_DOCK_WINDOW_VISIBILITY, new cbEventFunctor<CodeSnippets, CodeBlocksDockEvent>(this, &CodeSnippets::OnDockWindowVisability));
    // hook App Startup Done event
	Manager::Get()->RegisterEventSink(cbEVT_APP_STARTUP_DONE, new cbEventFunctor<CodeSnippets, CodeBlocksEvent>(this, &CodeSnippets::OnAppStartupDone));
	// hook App Shutdown Begin
    Manager::Get()->RegisterEventSink(cbEVT_APP_START_SHUTDOWN, new cbEventFunctor<CodeSnippets, CodeBlocksEvent>(this, &CodeSnippets::OnAppStartShutdown));

    Connect(wxEVT_IDLE,
            wxIdleEventHandler(CodeSnippets::OnIdle), NULL, this);

}//OnAttach

// ----------------------------------------------------------------------------
void CodeSnippets::OnRelease(bool appShutDown)
// ----------------------------------------------------------------------------
{
    // ------------------------------------------------------------
    // watch out, CodeBlocks can enter this routine multiple times
    // ------------------------------------------------------------
    #if defined(LOGGING)
    LOGIT( _T("CodeSnippets OnRelease[%s]"), appShutDown?wxT("true"):wxT("false") );
    #endif

    if (GetConfig()->m_appIsShutdown)
        return;

    if (not appShutDown)
    {
        OnDisable(appShutDown);
        return;
    }

    // delete the temporary keepAlive communications file
    RemoveKeepAliveFile();

    // ----------------------------------
    if (not GetSnippetsWindow()) return;
    // ----------------------------------

    // Don't close down if file checking is active
    while ( m_nOnActivateBusy )
    {   wxMilliSleep(10) ; wxYield();
    }

    //-CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
    //--CodeBlocksDockEvent evt(cbEVT_HIDE_DOCK_WINDOW);
    //-evt.pWindow = GetSnippetsWindow();
    //-Manager::Get()->ProcessEvent(evt);

    Disconnect(wxEVT_IDLE,
            wxIdleEventHandler(CodeSnippets::OnIdle), NULL, this);
    #if defined(__WXMSW__)
    wxTreeCtrl* pPrjTree = Manager::Get()->GetProjectManager()->GetTree();
    RemoveTreeCtrlHandler( pPrjTree, wxEVT_COMMAND_TREE_BEGIN_DRAG );
    RemoveTreeCtrlHandler( GetConfig()->GetOpenFilesList(),  wxEVT_COMMAND_TREE_BEGIN_DRAG );
    #endif

    //  On Linux, the following causes CB to crash when the snippet window is floating
    ////    if (GetSnippetsWindow())
    ////    {   //GetSnippetsWindow()->Close(); <- causes crash when user disables plugin
    ////        GetSnippetsWindow()->Destroy(); <- causes crash when CB closes
    ////        SetSnippetsWindow(0);
    ////    }

    if (GetSnippetsWindow())
            if ( GetSnippetsWindow()->GetFileChanged() )
                GetSnippetsWindow()->GetSnippetsTreeCtrl()->SaveItemsToFile(GetConfig()->SettingsSnippetsXmlPath);

    wxCloseEvent closeevt;
    closeevt.SetEventObject(GetConfig()->GetSnippetsWindow());
    GetConfig()->GetSnippetsWindow()->OnClose(closeevt);

    // Make sure user cannot re-enable CodeSnippets until a CB restart
    GetConfig()->m_appIsShutdown = true;

}//OnRelease
// ----------------------------------------------------------------------------
void CodeSnippets::BuildMenu(wxMenuBar* menuBar)
// ----------------------------------------------------------------------------
{
    if (GetConfig()->m_appIsShutdown) return;
    if (GetConfig()->m_appIsDisabled) return;

    GetConfig()->m_pMenuBar = menuBar;
    bool isSet = false;

	int idx = menuBar->FindMenu(_("View"));
	if (idx != wxNOT_FOUND) do
	{
		wxMenu* viewMenu = menuBar->GetMenu(idx);
		wxMenuItemList& items = viewMenu->GetMenuItems();

		// Find the first separator and insert before it
		for (size_t i = 0; i < items.GetCount(); ++i)
		{
			if (items[i]->IsSeparator())
			{
				viewMenu->InsertCheckItem(i, idViewSnippets, _("Code snippets"), _("Toggle displaying the code snippets."));
				isSet = true;
				break;
			}
		}//for

		// Not found, just append
		if (not isSet)
            viewMenu->AppendCheckItem(idViewSnippets, _("Code snippets"), _("Toggle displaying the code snippets."));
	}while(0);
	#if defined(LOGGING)
	LOGIT(wxT("Menubar[%p]idViewSnippets[%d]"),menuBar, idViewSnippets);
	#endif
}
// ----------------------------------------------------------------------------
void CodeSnippets::OnDisable(bool appShutDown)
// ----------------------------------------------------------------------------
{
    // It's impossible to remove CodeSnippets from memory when a user disables it
    // with the Settings/ManagePlugsin dlg. Doing so causes one crash after another.
    // So, here, we simple disable the user's ability to get to it. On the next
    // CodeBlocks reload, CodeSnippets won't be loaded. That's a good compromise.

    if (GetConfig()->m_appIsShutdown) return;
    if (GetConfig()->m_appIsDisabled) return;

    if (appShutDown) return;

    GetConfig()->m_appIsDisabled = true;

    // disable all idle processing
    Disconnect(wxEVT_IDLE,
        wxIdleEventHandler(CodeSnippets::OnIdle), NULL, this);

    // disable our menu item
    wxMenuBar* pbar = GetConfig()->m_pMenuBar;
    pbar->Check(idViewSnippets, false);

    // hide the window if showing
     //-CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
    CodeBlocksDockEvent evt(cbEVT_HIDE_DOCK_WINDOW);
    evt.pWindow = GetSnippetsWindow();
    Manager::Get()->ProcessEvent(evt);

     return;
}
// ----------------------------------------------------------------------------
void CodeSnippets::OnAppStartupDone(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    // if OpenFilesList plugin initializes *after* us, we didn't
    // find it in OnAttach(). So do it now.

    FindDragScroll();

    if (not GetConfig()->GetOpenFilesList())
    {
        GetConfig()->SetOpenFilesList( FindOpenFilesListWindow() );
        // set drop targets on the OpenFilesList tree control
        if (GetConfig()->GetOpenFilesList())
        {
            GetConfig()->GetOpenFilesList()->SetDropTarget(new DropTargets(this));
            //NB: On Linux, we don't enable dragging out of the file windows because of the drag/drop freeze bug
            // set event hooks
            #if defined(__WXMSW__)
                SetTreeCtrlHandler( GetConfig()->GetOpenFilesList(),  wxEVT_COMMAND_TREE_BEGIN_DRAG );
            #endif
            #if defined(LOGGING)
            LOGIT( _T("OpenFilesList found @[%p]"), GetConfig()->GetOpenFilesList());
            #endif
        }
        else{
            #if defined(LOGGING)
            LOGIT( _T("OpenFilesList *NOT* found."));
            #endif
        }
    }

    // ---------------------------------------
    // If external CodeSnippets, start it up
    // ---------------------------------------
    // If last session had CS open, start it up again
    if ( GetConfig()->IsExternalWindow() )
        if ( GetConfig()->IsExternalPersistentOpen() )
            CreateSnippetWindow();

    event.Skip();

}//OnAppStartupDone
// ----------------------------------------------------------------------------
void CodeSnippets::OnAppStartShutdown(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    //NOTE: This Event is begin called twice from main.cpp
    if (GetConfig()->m_appIsShutdown)
        return;
    GetConfig()->SettingsSave();
    // Call OnRelease() before CodeBlocks actually closes down or we'll crash
    OnRelease(true);
}
// ----------------------------------------------------------------------------
void CodeSnippets::CreateSnippetWindow()
// ----------------------------------------------------------------------------
{
    // Launch the executable if user specified "External" WindowState
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
   if ( GetConfig()->IsApplication() )
   {     // We can position an application window
        GetSnippetsWindow()->SetSize(GetConfig()->windowXpos, GetConfig()->windowYpos,
            GetConfig()->windowWidth, GetConfig()->windowHeight);
   }

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
	Manager::Get()->ProcessEvent(evt);

    // Add new code snippets scrollable windows to DragScroll plugin
    sDragScrollEvent dsevt(wxEVT_S_DRAGSCROLL_EVENT,idDragScrollRescan);
    dsevt.SetEventObject( GetConfig()->GetSnippetsTreeCtrl());
    dsevt.SetString( GetConfig()->GetSnippetsTreeCtrl()->GetName());
    GetConfig()->GetDragScrollEvtHandler()->AddPendingEvent(dsevt);

}//CreateSnippetWindow
// ----------------------------------------------------------------------------
bool CodeSnippets::TellExternalSnippetsToTerminate()
// ----------------------------------------------------------------------------
{
    bool result = ::wxRemoveFile( m_KeepAliveFileName );
    #if defined(LOGGING)
    LOGIT( _T("TellExternalSnippetsToTerminate result[%s]on[%s]"),
            result?_T("success"):_T("failed"), m_KeepAliveFileName.c_str());
    #endif
    return result;
}
// ----------------------------------------------------------------------------
bool CodeSnippets::RemoveKeepAliveFile()
// ----------------------------------------------------------------------------
{
    // delete the keepAlive file used to keep alive the
    // external snippets process
    bool result = ::wxRemoveFile( m_KeepAliveFileName );
    #if defined(LOGGING)
    LOGIT( _T("RemoveKeepAliveFile result[%s]on[%s]"),
            result?_T("success"):_T("failed"), m_KeepAliveFileName.c_str());
    #endif

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
    // For some unknow reason, event.IsCheck() gives an incorrect state
    // So we'll find the menu check item ourselves
    wxMenuBar* pbar = Manager::Get()->GetAppFrame()->GetMenuBar();
    wxMenu* pViewMenu = 0;
    wxMenuItem* pViewItem = pbar->FindItem(idViewSnippets, &pViewMenu);
    #if defined(LOGGING)
    LOGIT( _T("OnViewSnippets [%s] Checked[%d] IsShown[%d]"),
            GetConfig()->IsFloatingWindow()?wxT("float"):wxT("dock"),
            pViewMenu->IsChecked(idViewSnippets),
            IsWindowReallyShown(GetSnippetsWindow())
            );
    #endif

        // ---------------------------------------
        // Check for snippet independent window
        // ---------------------------------------
    if ( GetConfig()->GetSettingsWindowState().Contains(wxT("External")) )
    {
         #if defined(LOGGING)
         LOGIT( _T("OnView External m_ExternalPid[%lu] Checked[%d]"), m_ExternalPid, pViewItem->IsChecked() );
         #endif
        if ( (not m_ExternalPid) && pViewItem->IsChecked() )
        {
            // if previously open dock window, close it
            if ( GetSnippetsWindow()) CloseDockWindow();
            CreateSnippetWindow();
            #if defined(LOGGING)
            LOGIT( _T("m_ExternalPid[%lu]"), m_ExternalPid );
            #endif
            if ( m_ExternalPid )
                GetConfig()->SetExternalPersistentOpen(true);
            return;
        }
        if ( m_ExternalPid && (not pViewItem->IsChecked()) )
        {   // user closing external snippets with View/Snippets menu item
            // Signal, via keepAlive file, external snippets to terminate
            // LOGIT( _T("m_ExternalPid[%lu]"), m_ExternalPid );
            TellExternalSnippetsToTerminate();
            RemoveKeepAliveFile();
            m_ExternalPid = 0;

            GetConfig()->SetExternalPersistentOpen(false);
            return;
        }
    }
    else if ( m_ExternalPid )
    {   // user changed from Independent window to some other type(docked or floating) ;

        TellExternalSnippetsToTerminate();
        RemoveKeepAliveFile();
        m_ExternalPid = 0;
        GetConfig()->SetExternalPersistentOpen(false);
    }
        // ---------------------------------------
        // setup snippet tree docking window
        // ---------------------------------------

     if (not GetSnippetsWindow())
    {   // Snippets Window is closed, initialize and open it.
        CreateSnippetWindow();
    }


    #if defined(LOGGING)
    LOGIT( _T("OnView [%s] Checked[%d] IsShown[%d]"),
            GetConfig()->IsFloatingWindow()?wxT("float"):wxT("dock"),
            pViewItem->IsChecked(),
            IsWindowReallyShown(GetSnippetsWindow())
            );
    #endif

    // hiding window. remember last window position
    if ( IsWindowReallyShown(GetSnippetsWindow()) ) do
    {   if (not pViewItem->IsChecked()) //hiding window
        {   //wxAUI is so annoying, we have to check that it's *not* giving us
            // the position and size of CodeBlocks.
            if ( GetConfig()->IsFloatingWindow() )
                GetConfig()->SettingsSaveWinPosition();
            #if defined(LOGGING)
            LOGIT( _T("OnViewSnippets Saving Settings on HideWindow"));
            #endif
            GetConfig()->SettingsSave();
        }
    }while(0);

	CodeBlocksDockEvent evt(pViewItem->IsChecked() ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
	evt.pWindow = GetSnippetsWindow();
	Manager::Get()->ProcessEvent(evt);
	// if we close the window, it won't remember its layout position when reopened.
    //if (evt.GetEventType() == cbEVT_HIDE_DOCK_WINDOW) CloseDockWindow();

    // connect to the wxAUI wxEVT_CLOSE event
    if ( pViewItem->IsChecked() && GetConfig()->IsFloatingWindow()  )
    {
        ;//code removed
    }//if

}//OnViewSnippets
// ----------------------------------------------------------------------------
void CodeSnippets::OnUpdateUI(wxUpdateUIEvent& /*event*/)
// ----------------------------------------------------------------------------
{
    wxMenuBar* pbar = Manager::Get()->GetAppFrame()->GetMenuBar();

    #if defined(LOGGING)
    LOGIT( _T("OnUpdateUI Window[%p],Pid[%d]"), GetSnippetsWindow(), m_ExternalPid );
    #endif

    // check for externally started CodeSnippets
    if (not GetSnippetsWindow() && (not m_ExternalPid) )
    {   pbar->Check(idViewSnippets, false);
        #if defined(LOGGING)
        LOGIT( _T("OnUpdateUI Check[%s]"), wxT("to OFF") );
        #endif
         return;
    }
    // check if external CodeSnippets terminated without us
    if ( (not GetSnippetsWindow()) && m_ExternalPid )
    {
        if (not ::wxProcess::Exists(m_ExternalPid))
        {
            RemoveKeepAliveFile();
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
    if ( GetConfig()->m_appIsShutdown )
        { event.Skip(); return; }

    // Don't close windows if file checking is active
    if (m_nOnActivateBusy) { event.Skip();return; }

    // if user manipulating snippet tree, punt for now
    if ( GetSnippetsWindow() && GetSnippetsWindow()->IsTreeBusy() ) {event.Skip();return;}

    // ---------------------------------------------------------------------------
    // If user changed the docked window type, close the current and open the new
    // ---------------------------------------------------------------------------
    // if user requested different window type close docked/floating window
    // m_bWindowStateChanged is set true by the Settings dialog
    // This does not apply when the external Snippets pgm changes window type
    // because there's no way for the external app to signal m_bWindowStatChanged, yet
    // In that case, the external app closes itself and the user must open Snippets
    // from the View menu item.

    // if external snippets pgm terminated, clean up our environment
    if ( m_ExternalPid && (not wxProcess::Exists( m_ExternalPid)) ) do
    {
        GetConfig()->SetExternalPersistentOpen(false);
        // Get the new window state docked or floating
        wxString windowState = GetConfig()->GetSettingsWindowState();
        #if defined(LOGGING)
        //LOGIT( _T("OnIdle windowState[%s]"), GetConfig()->GetSettingsWindowState().c_str());
        #endif

        // If window stat is still "External" User must have closed the window
        if ( windowState.Matches(_T("External")) ) break;

        wxMenuBar* pbar = Manager::Get()->GetAppFrame()->GetMenuBar();
        wxMenu* pViewMenu = 0;
        wxMenuItem* pViewItem = pbar->FindItem(idViewSnippets, &pViewMenu);
        if ( pViewItem )
            pViewItem->Check(true);
        #if defined(LOGGING)
        LOGIT( _T("OnIdle [%s] Checked[%d] IsShown[%d]"),
                GetConfig()->IsFloatingWindow()?wxT("float"):wxT("dock"),
                pViewMenu->IsChecked(idViewSnippets),
                IsWindowReallyShown(GetSnippetsWindow())
                );
        #endif
        wxCommandEvent menuViewEvent(wxEVT_COMMAND_MENU_SELECTED, idViewSnippets);
        AddPendingEvent( menuViewEvent);
    }while(0);

    if ( GetConfig()->m_bWindowStateChanged )
    {
        // close docked/floating window
        if ( GetSnippetsWindow() && GetConfig()->m_bWindowStateChanged )
            CloseDockWindow();

        // close external snippets pgm if it's running
        if ( m_ExternalPid && GetConfig()->m_bWindowStateChanged )
        {
            TellExternalSnippetsToTerminate();
            RemoveKeepAliveFile();
            m_ExternalPid = 0;
            GetConfig()->SetExternalPersistentOpen(false);
        }

        // if external snippets pgm terminated, clean up our environment
        if ( m_ExternalPid && (not wxProcess::Exists( m_ExternalPid)) )
        {
            TellExternalSnippetsToTerminate();
            RemoveKeepAliveFile();
            m_ExternalPid = 0;
            GetConfig()->SetExternalPersistentOpen(false);
        }

        // if no snippet window is open, open one
        if ( (not GetSnippetsWindow()) && (not m_ExternalPid) )
        {
            if (GetConfig()->m_bWindowStateChanged)
            {
                GetConfig()->m_bWindowStateChanged = false;
                //-wxMenuBar* pbar = Manager::Get()->GetAppWindow()->GetMenuBar();
                //-if ( pbar->IsChecked(idViewSnippets) )
                {   CreateSnippetWindow();
                    bool bExternalRequest = GetConfig()->GetSettingsWindowState().Contains(wxT("External"));
                    if (not bExternalRequest)
                    {
                        CodeBlocksDockEvent evt( cbEVT_SHOW_DOCK_WINDOW );
                        evt.pWindow = GetSnippetsWindow();
                        Manager::Get()->ProcessEvent(evt);

                    }
                }//if pbar
            }
        }
        GetConfig()->m_bWindowStateChanged = false;

    }//if ( GetConfig()->m_bWindowStateChanged )



    CodeSnippetsTreeCtrl* pTree = GetConfig()->GetSnippetsTreeCtrl();
    if ( pTree ) pTree->OnIdle();

    event.Skip();
}
// ----------------------------------------------------------------------------
void CodeSnippets::CloseDockWindow()
// ----------------------------------------------------------------------------
{
    if (not GetSnippetsWindow()) return;

    bool bOpen = IsWindowReallyShown(GetSnippetsWindow());
    if ( bOpen && GetConfig()->IsFloatingWindow() )
    {
        CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
        evt.pWindow = GetSnippetsWindow();
        Manager::Get()->ProcessEvent(evt);
    }

    if ( GetSnippetsWindow())
    {   //Close Docked/Floating window. Release our resources

        CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
        evt.pWindow = GetSnippetsWindow();
        Manager::Get()->ProcessEvent(evt);

        GetSnippetsWindow()->Destroy();
        SetSnippetsWindow(0);
        #if defined(LOGGING)
        LOGIT( _T("CloseDockWindow:SnippetsWindow [%s]"), bOpen?wxT("Open"):wxT("Closed") );
        #endif
    }

}
// ----------------------------------------------------------------------------
void CodeSnippets::OnSwitchViewLayout(CodeBlocksLayoutEvent& event)
// ----------------------------------------------------------------------------
{
    //event.layout
    #if defined(LOGGING)
    LOGIT( _T("cbEVT_SWITCH_LAYOUT[%s]"),event.layout.c_str() );
    #endif
    event.Skip();
}
// ----------------------------------------------------------------------------
void CodeSnippets::OnSwitchedViewLayout(CodeBlocksLayoutEvent& event)
// ----------------------------------------------------------------------------
{
    //event.layout
    #if defined(LOGGING)
    LOGIT( _T("cbEVT_SWITCHED_LAYOUT[%s]"),event.layout.c_str() );
    #endif
    event.Skip();
}
// ----------------------------------------------------------------------------
void CodeSnippets::OnDockWindowVisability(CodeBlocksDockEvent& event)
// ----------------------------------------------------------------------------
{
    // Called when user issues the cbEVT_SHOW_DOCK_WINDOW/cbEVT_HIDE_DOCK_WINDOW)
    // But not called when user uses system [x] close on docked/float window

    //event.layout
    //BUG: the event.GetId() is alway null. It should be the window pointer.
    #if defined(LOGGING)
    LOGIT( _T("cbEVT_DOCK_WINDOW_VISIBILITY[%p]"),event.GetId() );
    #endif
    wxMenuBar* pbar = Manager::Get()->GetAppFrame()->GetMenuBar();
    if (not IsWindowReallyShown(GetSnippetsWindow()))
        pbar->Check(idViewSnippets, false);

    event.Skip();
}
// ----------------------------------------------------------------------------
void CodeSnippets::OnActivate(wxActivateEvent& event)
// ----------------------------------------------------------------------------
{

    // An application has been activated by the OS

    if ( m_nOnActivateBusy ) {event.Skip();return;}
    ++m_nOnActivateBusy;

    do{ //only once
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
bool CodeSnippets::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& files)
// ----------------------------------------------------------------------------
{
    // This is the drop routine for the Management Project/Files tree controls
    // It's used to pass files off to CB MainFrame's drop method
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
bool CodeSnippets::GetTreeSelectionData(const wxTreeCtrl* pTree, const wxTreeItemId itemID, wxString& selString)
// ----------------------------------------------------------------------------
{
    selString = wxEmptyString;

    if (not pTree) { return false; }

    if ( (pTree == m_pProjectMgr->GetUI().GetTree())
        or (pTree == GetConfig()->GetOpenFilesList()) )
        {/*ok*/;}
    else{ return false; }

    #ifdef LOGGING
     //LOGIT( _T("Focused Tree:%p item[%p]"),pTree, itemID.IsOk()?itemID.m_pItem:0 );
    #endif //LOGGING

    // check for a file selection in the treeCtrl
    // note: the following gets the wrong item when we're called from a tree event
    //-wxTreeItemId sel = pTree->GetSelection(); This is wrong for Project Tree(use previous hit data)
    wxTreeItemId sel = itemID;
    if ( itemID.IsOk()) sel = itemID;
    else return false;
    if (not sel)
        return false;
    #ifdef LOGGING
     LOGIT( _T("Selection:%p"), sel.m_pItem);
    #endif //LOGGING
    // -------------------------
    // Opened Files Tree
    // -------------------------
    if ( pTree == GetConfig()->GetOpenFilesList() )
    {   //-selString = pTree->GetTreeItemFilename( sel );
        //-EditorBase* ed = static_cast<EditorBase*>(static_cast<OpenFilesListData*>(pTree->GetItemData(event.GetItem()))->GetEditor());
        EditorBase* ed = static_cast<EditorBase*>(static_cast<OpenFilesListData*>(pTree->GetItemData(sel))->GetEditor());
        selString = (ed ? ed->GetFilename() : wxT(""));
    }

    // -------------------------
    // Project Tree
    // -------------------------
    if ( pTree == m_pProjectMgr->GetUI().GetTree() ) {
        // create a string from highlighted Project treeCtrl item

        // Workspace/root
        if (sel && sel == pTree->GetRootItem())
        {   // selected workspace
            cbWorkspace* pWorkspace = m_pProjectMgr->GetWorkspace();
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

    if ( (0 == str.Freq('\r')) && (0 == str.Freq('\n')) )
        pFilenames->Add(str);
    else{ // parse big string into individual filenames
        wxString ostr;
        for (size_t i=0; i<str.Length(); i++ ) {
            if ((str[i] != '\r') && (str[i] != '\n'))
                ostr.Append(str[i]);
            else
            {
                pFilenames->Add(ostr);
                ostr.Empty();
                if ( ((i+1)<str.Length()) && (str[i+1] == '\r') )
                    i++;    //skip over carrige return
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
// ///////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////
// DropTargets
// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
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
// ///////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////
// drop handlers
// ///////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////
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
    //bool ok;
    wxArrayString* pFilenames = m_pcbDndExtn->TextToFilenames(data);
    if (pFilenames->GetCount()) /*ok =*/ m_pcbDndExtn->OnDropFiles(1, 1, *pFilenames);
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
// ----------------------------------------------------------------------------
void CodeSnippets::OnPrjTreeMouseMotionEvent(wxMouseEvent& event)
// ----------------------------------------------------------------------------
{
    // Drag event from the Project Tree Window
    // When user drags left mouse key out of the Projects/Files tree,
    // create a dropSource from the focused treeCtrl item

    // EVT_MOTION
    // allow all other user to see event
    event.Skip();

    if (not m_IsAttached) return;

    // memorize position of the mouse ctrl key as copy/delete flag
    m_bMouseCtrlKeyDown = event.ControlDown();
    m_bMouseLeftKeyDown = event.LeftIsDown();
    m_bMouseIsDragging  = event.Dragging();

    // If dragging an item, show drag cursor
    wxTreeCtrl* pTree = (wxTreeCtrl*)event.GetEventObject();
    if (m_bMouseIsDragging and m_bMouseLeftKeyDown
        and (not m_bDragCursorOn) and m_prjTreeItemAtKeyDown)
    {
        m_oldCursor = pTree->GetCursor();
        pTree->SetCursor(*m_pDragCursor);
        m_bDragCursorOn = true;
    }
    else    //restore regular cursor
    if (m_bDragCursorOn)
    {
        pTree->SetCursor(m_oldCursor);
        m_bDragCursorOn = false;
    }
}

// ----------------------------------------------------------------------------
void CodeSnippets::OnPrjTreeMouseLeftDownEvent(wxMouseEvent& event)
// ----------------------------------------------------------------------------
{
   // wxTreeCtrl* pTree = (wxTreeCtrl*)event.GetEventObject();

    #ifdef LOGGING
     //LOGIT(wxT("OnMouseLeftDown") );
    #endif

    event.Skip();
    if (not m_IsAttached) return;

    // memorize position of the mouse
    m_bMouseLeftKeyDown = true;
    m_MouseDownX = event.GetX();
    m_MouseDownY = event.GetY();

    #if !wxCHECK_VERSION(2, 8, 12)
    m_prjTreeItemAtKeyDown = 0;
    m_prjTreeItemAtKeyUp = 0;
    #endif
    int hitFlags = 0;

    wxTreeCtrl* pTree = (wxTreeCtrl*)event.GetEventObject();
    wxTreeItemId id = pTree->HitTest(wxPoint(m_MouseDownX, m_MouseDownY), hitFlags);
    if (id.IsOk() and (hitFlags & (wxTREE_HITTEST_ONITEMICON | wxTREE_HITTEST_ONITEMLABEL )))
        m_prjTreeItemAtKeyDown = id;

    #ifdef LOGGING
     //LOGIT(wxT("MouseCtrlKeyDown[%s]"), m_bMouseCtrlKeyDown?wxT("Down"):wxT("UP") );
    #endif
}//OnPrjTreeMouseUpEvent
// ----------------------------------------------------------------------------
void CodeSnippets::OnPrjTreeMouseLeftUpEvent(wxMouseEvent& event)
// ----------------------------------------------------------------------------
{
    // EVT_LEFT_UP

    event.Skip();
    if (not m_IsAttached) return;

    #ifdef LOGGING
     //LOGIT(wxT("OnMouseLeftUp") );
    #endif
    // memorize position of the mouse
    m_bMouseLeftKeyDown = false;
    m_MouseUpX = event.GetX();
    m_MouseUpY = event.GetY();

    #if !wxCHECK_VERSION(2, 8, 12)
    m_prjTreeItemAtKeyUp = 0;
    #endif
    int hitFlags = 0;

    wxTreeCtrl* pTree = (wxTreeCtrl*)event.GetEventObject();
    wxTreeItemId id = pTree->HitTest(wxPoint(m_MouseUpX, m_MouseUpY), hitFlags);
    if (id.IsOk() and (hitFlags & (wxTREE_HITTEST_ONITEMICON | wxTREE_HITTEST_ONITEMLABEL )))
        m_prjTreeItemAtKeyUp = id;

    if (m_bMouseExitedWindow and m_bMouseIsDragging and m_prjTreeItemAtKeyDown)
    {   //Doesnt work for leaf items; never receive Evt_Left_Up; moved to OnLeaveWindow
        //DoPrjTreeExternalDrag(pTree);
    }
    // We don't do internalitem-to-item dragging for the project Tree
    //else
    //if ( (not m_bMouseExitedWindow) and m_bMouseIsDragging
    //     and m_prjTreeItemAtKeyDown and m_prjTreeItemAtKeyUp
    //     and (m_itemAtKeyDown not_eq m_itemAtKeyUp ))
    //{
    //    DoPrjTreeItemDrag(pTree);
    //}

    // Do not release the mouse until after the drag has finished
    // else the drag will not complete.
    if (m_bMouseExitedWindow)
    {
        if (pTree->HasCapture())
        {
            pTree->ReleaseMouse();
            #if defined(LOGGING)
            LOGIT( _T("Mouse Released"));
            #endif
        }
        else
        {
            #if defined(LOGGING)
            LOGIT( _T("Lost Mouse capture"));
            #endif
        }
    }

    m_bMouseExitedWindow = false;
    m_bMouseIsDragging = false;

    if (m_bDragCursorOn)
    {
        pTree->SetCursor(m_oldCursor);
        m_bDragCursorOn = false;
    }

}//OnPrjTreeMouseLeftUpEvent
// ----------------------------------------------------------------------------
void CodeSnippets::OnPrjTreeMouseLeaveWindowEvent(wxMouseEvent& event)
// ----------------------------------------------------------------------------
{
    // -----------------------
    // EVT_LEAVE_WINDOW
    // -----------------------
    event.Skip();

    // User has dragged mouse out of project window.
    // if us is dragging mouse, save the source item for later use
    // in DoTreeExternalDrag()

    m_bBeginInternalDrag = false; //This is not an internal drag

    // Left mouse key must be down (dragging)
    if (not m_bMouseLeftKeyDown ) return;
    if (not m_bMouseIsDragging ) return;
    // check if data is available
    if (not m_prjTreeItemAtKeyDown) return;

    wxTreeCtrl* pTree = (wxTreeCtrl*)event.GetEventObject();

    #ifdef LOGGING
     //LOGIT( _T("EVT_LEAVE_WINDOW %p"), pTree );
    #endif //LOGGING

    // when user drags an item out of the window, this routine is called
    // OnMouseKeyUpEvent will clear this flag
    m_bMouseExitedWindow = true;
    //pTree->CaptureMouse(); //this does no good. DoDragDrop will cancel it.
    #if defined(LOGGING)
      //LOGIT( _T("Mouse Captured"));
    #endif

    if (m_bMouseExitedWindow and m_bMouseIsDragging and m_prjTreeItemAtKeyDown)
    {
        DoPrjTreeExternalDrag(pTree);
    }

    return;
}//OnLeaveWindow
// ----------------------------------------------------------------------------
void CodeSnippets::DoPrjTreeExternalDrag(wxTreeCtrl* pTree)
// ----------------------------------------------------------------------------
{
    if ( not m_prjTreeItemAtKeyDown)
        return;

    // we now have data; create both a simple text and filename drop source
    wxTextDataObject* textData = new wxTextDataObject();
    wxFileDataObject* fileData = new wxFileDataObject();
    // fill text and file sources with data
    wxString m_prjTreeText;
    if (not GetTreeSelectionData(pTree, m_prjTreeItemAtKeyDown, m_prjTreeText))
    {
        m_prjTreeText = wxEmptyString;
        return;
    }

    static const wxString delim(_T("$%["));
    if( m_prjTreeText.find_first_of(delim) != wxString::npos )
        Manager::Get()->GetMacrosManager()->ReplaceMacros(m_prjTreeText);

    wxDropSource textSource( *textData, pTree );
    textData->SetText( m_prjTreeText );

    wxDropSource fileSource( *fileData, pTree );
    wxString fileName = m_prjTreeText;

    if (not ::wxFileExists(fileName) ) fileName = wxEmptyString;
    // If no filename, but text is URL/URI, pass it as a file (esp. for browsers)
    if ( fileName.IsEmpty())
    {   if (m_prjTreeText.StartsWith(_T("http://")))
            fileName = m_prjTreeText;
        if (m_prjTreeText.StartsWith(_T("file://")))
            fileName = m_prjTreeText;
        // Remove anything pass the first \n or \r {v1.3.92}
        fileName = fileName.BeforeFirst('\n');
        fileName = fileName.BeforeFirst('\r');
        if (not fileName.IsEmpty())
            textData->SetText( fileName );
    }
    fileData->AddFile( (fileName.Len() > 128) ? wxString(wxEmptyString) : fileName );

    // set composite data object to contain both text and file data
    wxDataObjectComposite* data = new wxDataObjectComposite();
    data->Add( (wxDataObjectSimple*)textData );
    data->Add( (wxDataObjectSimple*)fileData, true ); // set file data as preferred

    // create the drop source containing both data types
    wxDropSource source( *data, pTree  );

    #ifdef LOGGING
     LOGIT( _T("PrjTree DropSource Text[%s], File[%s]"),
                textData->GetText().c_str(),
                fileData->GetFilenames().Item(0).c_str() );
    #endif //LOGGING

    // allow both copy and move
    int flags = 0;
    flags |= wxDrag_AllowMove;
    // do the dragNdrop
    wxDragResult result = source.DoDragDrop(flags);

    // report the results
    #if LOGGING
        wxString pc;
        switch ( result )
        {
            case wxDragError:   pc = _T("Error!");    break;
            case wxDragNone:    pc = _T("Nothing");   break;
            case wxDragCopy:    pc = _T("Copied");    break;
            case wxDragMove:    pc = _T("Moved");     break;
            case wxDragLink:    pc = _T("Linked");    break;
            case wxDragCancel:  pc = _T("Cancelled"); break;
            default:            pc = _T("Huh?");      break;
        }
        LOGIT( wxT("DoDragDrop returned[%s]"),pc.GetData() );
    #else
        wxUnusedVar(result);
    #endif

    delete textData; //wxTextDataObject
    delete fileData; //wxFileDataObject
    m_TreeText = wxEmptyString;
    #if !wxCHECK_VERSION(2, 8, 12)
    m_prjTreeItemAtKeyDown = 0;
    m_prjTreeItemAtKeyUp = 0;
    #endif

    // correct for treeCtrl bug
    SendMouseLeftUp(pTree, m_MouseDownX, m_MouseDownY);

}//DoPrjTreeExternalDrag
// ----------------------------------------------------------------------------
void CodeSnippets::SendMouseLeftUp(const wxWindow* pWin, const int mouseX, const int mouseY)
// ----------------------------------------------------------------------------
{
    // ---MSW WORKAROUNG --------------------------------------------------
    // Since we dragged outside the tree control with a mouse_left_down,
    // Wx will *not* send us a mouse_key_up event until the user explcitly
    // re-clicks inside the tree control. The tree exibits very bad behavior.

    // Send an mouse_key_up to the tree control so it releases the
    // mouse and behaves correctly.
  #if defined(__WXMSW__)
    if ( pWin )
    {
        //send Mouse LeftKeyUp to Tree Control window
        #ifdef LOGGING
         LOGIT( _T("Sending Mouse LeftKeyUp"));
        #endif //LOGGING
        // Remember current mouse position
        wxPoint CurrentMousePosn = ::wxGetMousePosition();
        // Get absolute location of mouse x and y
        wxPoint fullScreen = pWin->ClientToScreen(wxPoint(mouseX,mouseY));
        // move mouse into the window
        MSW_MouseMove( fullScreen.x, fullScreen.y );
        // send mouse LeftKeyUp
        INPUT Input         = {0};
        Input.type          = INPUT_MOUSE;
        Input.mi.dwFlags    = MOUSEEVENTF_LEFTUP;
        ::SendInput(1,&Input,sizeof(INPUT));
        // put mouse back in drag-end position
        MSW_MouseMove( CurrentMousePosn.x, CurrentMousePosn.y );
    }
  #endif //(__WXMSW__)
}
// ----------------------------------------------------------------------------
void CodeSnippets::MSW_MouseMove(int x, int y )
// ----------------------------------------------------------------------------
{
   #if defined(__WXMSW__)
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
   #endif
}
// ----------------------------------------------------------------------------
void CodeSnippets::SetTreeCtrlHandler(wxWindow *p, WXTYPE eventType)
// ----------------------------------------------------------------------------
{
	if (!p ) return;		// sanity check

    #ifdef LOGGING
	 LOGIT(wxT("CodeSnippets::SetTreeCtrlHandler[%s] %p"), p->GetName().c_str(),p);
    #endif //LOGGING

////    p->Connect(wxEVT_COMMAND_TREE_BEGIN_DRAG,
////                     wxTreeEventHandler(CodeSnippets::OnTreeDragEvent),
////                     NULL, this);
////    p->Connect(wxEVT_COMMAND_TREE_END_DRAG,
////                     wxTreeEventHandler(CodeSnippets::OnTreeDragEvent),
////                     NULL, this);
////    p->Connect(wxEVT_LEAVE_WINDOW,
////                     wxTreeEventHandler(CodeSnippets::OnTreeDragEvent),
////                     NULL, this);

    p->Connect(wxEVT_LEFT_UP,
                     wxMouseEventHandler(CodeSnippets::OnPrjTreeMouseLeftUpEvent),
                     NULL, this);
    p->Connect(wxEVT_LEFT_DOWN,
                     wxMouseEventHandler(CodeSnippets::OnPrjTreeMouseLeftDownEvent),
                     NULL, this);
    p->Connect(wxEVT_MOTION,
                     wxMouseEventHandler(CodeSnippets::OnPrjTreeMouseMotionEvent),
                     NULL, this);
    p->Connect(wxEVT_LEAVE_WINDOW,
                     wxMouseEventHandler(CodeSnippets::OnPrjTreeMouseLeaveWindowEvent),
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

////    p->Disconnect(wxEVT_COMMAND_TREE_BEGIN_DRAG,       //eg.,wxEVT_LEAVE_WINDOW,
////                     wxTreeEventHandler(CodeSnippets::OnTreeDragEvent),
////                     NULL, this);
////    p->Disconnect(wxEVT_COMMAND_TREE_END_DRAG,       //eg.,wxEVT_LEAVE_WINDOW,
////                     wxTreeEventHandler(CodeSnippets::OnTreeDragEvent),
////                     NULL, this);
////    p->Disconnect(wxEVT_LEAVE_WINDOW,       //eg.,wxEVT_LEAVE_WINDOW,
////                     wxTreeEventHandler(CodeSnippets::OnTreeDragEvent),
////                     NULL, this);
    p->Disconnect(wxEVT_LEFT_UP,
                     wxMouseEventHandler(CodeSnippets::OnPrjTreeMouseLeftUpEvent),
                     NULL, this);
    p->Disconnect(wxEVT_LEFT_DOWN,
                     wxMouseEventHandler(CodeSnippets::OnPrjTreeMouseLeftDownEvent),
                     NULL, this);
    p->Disconnect(wxEVT_MOTION,
                     wxMouseEventHandler(CodeSnippets::OnPrjTreeMouseMotionEvent),
                     NULL, this);
    p->Disconnect(wxEVT_LEAVE_WINDOW,
                     wxMouseEventHandler(CodeSnippets::OnPrjTreeMouseLeaveWindowEvent),
                     NULL, this);
}
// ----------------------------------------------------------------------------
wxString CodeSnippets::FindAppPath(const wxString& argv0, const wxString& cwd, const wxString& appVariableName)
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
        LOGIT( _T("FindAppPath: AbsolutePath[%s]"), wxPathOnly(argv0Str).GetData() );
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
            LOGIT( _T("FindAppPath: RelativePath[%s]"), wxPathOnly(str).GetData() );
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
        LOGIT( _T("FindAppPath: SearchPath[%s]"), wxPathOnly(str).GetData() );
        #endif
        return wxPathOnly(str);
    }

    // Failed
    #if defined(LOGGING)
     LOGIT(  _T("FindAppPath: Failed, returning cwd") );
    #endif
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
         #if defined(LOGGING)
         LOGIT( _T("Launch Path set to[%s]"), mswPath.c_str() );
         #endif
    #endif

    #ifndef __WXMSW__
        // setup dynamic linker path
        #if defined(__APPLE__) && defined(__MACH__)
            wxSetEnv(_T("DYLD_LIBRARY_PATH"), _T(".:$DYLD_LIBRARY_PATH"));
        #else   // it's linux
            //wxString ldLibraryPath = wxT(".:");
            wxString ldLibraryPath = ::wxPathOnly( cmd ) + wxT("/");
            if ( wxDirExists( ldLibraryPath + wxT("./lib")) ) ldLibraryPath << wxT("./lib");
            if ( wxDirExists( ldLibraryPath + wxT("../lib")) ) ldLibraryPath << wxT("../lib");
            ldLibraryPath << wxT(":$LD_LIBRARY_PATH");
            wxSetEnv( _T("LD_LIBRARY_PATH"), ldLibraryPath );
            wxGetEnv( _T("LD_LIBRARY_PATH"), &ldLibraryPath );
            Manager::Get()->GetLogManager()->DebugLog(wxString::Format( _("CodeSnippets CWD: %s"), cwd.c_str()) );
            Manager::Get()->GetLogManager()->DebugLog(wxString::Format( _("CodeSnippets LD_LIBRARY_PATH is: %s"), ldLibraryPath.c_str()) );
        #endif // __APPLE__ && __MACH__
    #endif  //ndef __WXMSW__

    // start codesnippets
    wxString exeCmd = cmd;
    ////wxString exeCmd = _T("cmd /c gdb --args ") + cmd;
    Manager::Get()->GetLogManager()->DebugLog( _("Starting program:")+ exeCmd);
    m_ExternalPid = wxExecute( exeCmd, wxEXEC_ASYNC);
    #if defined(LOGGING)
     LOGIT( _T("Launch [%s] from [%s] Pid[%lu]"), exeCmd.c_str(), cwd.c_str(), m_ExternalPid );
    #endif

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
            //-Manager::Get()->GetMessageManager()->DebugLog(wxString::Format( _("Executing: %s"), psCmd.c_str()) );
            Manager::Get()->GetLogManager()->DebugLog(wxString::Format( _("Executing: %s"), psCmd.c_str()) );
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
                //-Manager::Get()->GetMessageManager()->DebugLog(wxString::Format( _("PS Error:%s"), psErrors.Item(i).c_str()) );
                Manager::Get()->GetLogManager()->DebugLog(wxString::Format( _("PS Error:%s"), psErrors.Item(i).c_str()) );

        }//if(m_ExternalPid == -1)
    #endif

    if (!m_ExternalPid)
    {
        //Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("failed"));
        Manager::Get()->GetLogManager()->DebugLog( _("failed"));
        return -1;
    }
    //Manager::Get()->GetMessageManager()->Log(m_PageIndex, _("done"));
    Manager::Get()->GetLogManager()->DebugLog( _("done"));
    return 0;
}
// ----------------------------------------------------------------------------
long CodeSnippets::LaunchExternalSnippets()
// ----------------------------------------------------------------------------
{
   // Launch the executable if user specified "External" WindowState

    // First, create a temporary keepAlive file with our Pid in the name.
    // The launched process will check for the file to disappear
    // and terminate, saving its data and conf settings

    // deallocate any previously mapped file
    RemoveKeepAliveFile();

    // make a unique keepAlive file name with this process's pid
    wxString myPid(wxString::Format(wxT("%lu"),::wxGetProcessId()));
    wxString tempDir = GetConfig()->GetTempDir();
    m_KeepAliveFileName = tempDir + wxT("/cbsnippetspid") +myPid+ wxT(".plg");

    // Create a temporary keepalive file that indicates that the external
    // CodeSnippets pgm should keep running until the file disappears
    m_PidTmpFile.Create( m_KeepAliveFileName, true);
    m_PidTmpFile.Close();

    // Launch the external process
    wxString execFolder = GetConfig()->m_ExecuteFolder;
    wxString PgmFullPath ;
    do {
        PgmFullPath = execFolder + wxT("/codesnippets");
        #if defined(__WXMSW__)
            PgmFullPath << wxT(".exe");
        #endif
        if ( ::wxFileExists(PgmFullPath) ) break;
        #if defined(LOGGING)
        LOGIT(wxT("codesnippets not found at[%s]"),PgmFullPath.GetData());
        #endif

        PgmFullPath = execFolder
                    + wxT("/share/codeblocks/plugins/codesnippets");
        #if defined(__WXMSW__)
            PgmFullPath << wxT(".exe") ;
        #endif
    }while(0);

    wxString appName = wxTheApp->GetAppName();
    wxString pgmArgs( wxString::Format( wxT("--KeepAlivePid=%lu --AppParent=%s"), ::wxGetProcessId(), appName.c_str() ) );
    wxString command = PgmFullPath + wxT(" ") + pgmArgs;

    #if defined(LOGGING)
     LOGIT( _T("Launching[%s]"), command.GetData());
    #endif

    bool result = LaunchProcess(command, wxGetCwd());
    #if defined(LOGGING)
     LOGIT( _T("Launch Result[%d] m_ExternalPid[%lu]"),result, m_ExternalPid );
    #endif
    if ( 0 != result )
    {  wxString msg(wxString::Format(wxT("Error [%d] Launching\n %s\n"),result, PgmFullPath.c_str()));
       GenericMessageBox( msg );
    }

    return result;
}
// ----------------------------------------------------------------------------
wxWindow* CodeSnippets::FindOpenFilesListWindow()
// ----------------------------------------------------------------------------
{
    //Find "Open files list" menu item.
    wxFrame* pFrame = Manager::Get()->GetAppFrame();
    int idMenuOpenFilesList = ::wxFindMenuItemId( pFrame, wxT("View"), wxT("Open files list"));
    #if defined(__WXGTK__)
      idMenuOpenFilesList = ::wxFindMenuItemId( pFrame, wxT("View"), wxT("_Open files list"));
    #endif
    int idWindowOpenFilesList = 0;
    if (idMenuOpenFilesList != wxNOT_FOUND)
    {
        #if defined(LOGGING)
        LOGIT( _T("idMenuOpenFilesList:[%d]"), idMenuOpenFilesList );
        #endif
        // Hack: we know that the id is assigned in a namespace with wxNewId() just
        // before the menu id is.
        idWindowOpenFilesList = idMenuOpenFilesList - 1;
        wxWindow* pOpenFilesListWin = wxWindow::FindWindowById( idWindowOpenFilesList, pFrame);
        if (pOpenFilesListWin)
        {
            #if defined(LOGGING)
            LOGIT( _T("pOpenFilesListWin[%p] name[%s] label[%s]"), pOpenFilesListWin,
                pOpenFilesListWin->GetName().c_str(),
                pOpenFilesListWin->GetLabel().c_str()
                );
            #endif
            return pOpenFilesListWin;
        }
    }
    return 0;
}//FindOpenFilesListWindow
// ----------------------------------------------------------------------------
cbDragScroll* CodeSnippets::FindDragScroll()
// ----------------------------------------------------------------------------
{
    // Initialize local DragScrolling pointer. We'll use it to
    // post event requests.
    //-if ( GetConfig()->GetDragScrollPlugin() )
    if ( GetConfig()->m_pDragScrollPlugin )
        return GetConfig()->GetDragScrollPlugin();

    // If DragScroll isn't loaded, absorb the sDragScrollEvents
    // (fake out the plugin address) so
    // ProcessEvent() and AddPendingEvent() don't crash
    GetConfig()->SetDragScrollPlugin( (cbDragScroll*)this );

    // Check to see if Dragscroll is loaded
    cbPlugin* pPlgn = Manager::Get()->GetPluginManager()->FindPluginByName(_T("cbDragScroll"));
    if ( pPlgn )
    {
        GetConfig()->SetDragScrollPlugin( (cbDragScroll*)pPlgn );
        // Hack to get actual sDragScrollEvent value
        PluginInfo* pInfo = (PluginInfo*)(Manager::Get()->GetPluginManager()->GetPluginInfo(pPlgn));
        pInfo->authorWebsite.ToLong(&m_nDragScrollEventId);
        if ( m_nDragScrollEventId )
        {
            // Reset our copy of the DragScroll event to the actual from DragScroll plugin
            wxEventType* p = (wxEventType*)&wxEVT_S_DRAGSCROLL_EVENT;
                        *p = m_nDragScrollEventId ;
        }
        #if defined(LOGGING)
        LOGIT( _T("CodeSnippets found DragScroll @[%p]EvemtId[%ld]"), GetConfig()->GetDragScrollPlugin(), m_nDragScrollEventId);
        LOGIT( _T("DragScroll events are[%d]"), wxEVT_S_DRAGSCROLL_EVENT );
        #endif
    }

    return GetConfig()->GetDragScrollPlugin();
}
// ----------------------------------------------------------------------------
wxString CodeSnippets::GetCBConfigFile()
// ----------------------------------------------------------------------------
{
    PersonalityManager* PersMan = Manager::Get()->GetPersonalityManager();
    wxString personality = PersMan->GetPersonality();
    ConfigManager* CfgMan = Manager::Get()->GetConfigManager(_T("app"));
    wxString current_conf_file = CfgMan->LocateDataFile(personality+_T(".conf"), sdAllKnown);

    // Config manager will return an empty string on the first run of CodeBlocks
    if (current_conf_file.IsEmpty())
    {
        wxString appdata;
        if ( personality == _T("default") )
            personality = _T("");
        // Get APPDATA env var and append ".codeblocks" to it
        wxGetEnv(_T("APPDATA"), &appdata);
        current_conf_file = appdata +
                    //-wxFILE_SEP_PATH + _T("codeblocks") + wxFILE_SEP_PATH
                    wxFILE_SEP_PATH + wxTheApp->GetAppName() + wxFILE_SEP_PATH
                    + personality + _T(".codesnippets.ini");
    }
    return current_conf_file;
}
// ----------------------------------------------------------------------------
wxString CodeSnippets::GetCBConfigDir()
// ----------------------------------------------------------------------------
{
    return GetCBConfigFile().BeforeLast(wxFILE_SEP_PATH);
}
//// ----------------------------------------------------------------------------
//void CodeSnippets::OnWindowDestroy(wxEvent& event)
//// ----------------------------------------------------------------------------
//{
//    wxWindow* pWindow = (wxWindow*)(event.GetEventObject());
//     LOGIT( _T("OnWindowClose[%p]"), pWindow );
//     event.Skip();
//}

// ----------------------------------------------------------------------------
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

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
// RCS-ID: $Id: codesnippets.cpp 33 2007-04-03 02:45:43Z Pecan $

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


#include "version.h"
#include "codesnippets.h"
#include "codesnippetswindow.h"
#include "snippetsconfig.h"
//#include "wxAUI/manager.h"

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
    g_pConfig = new CodeSnippetsConfig;

    // initialize version and logging
    m_pAppWin = Manager::Get()->GetAppWindow();
    GetConfig()->AppName = wxT("codesnippets");

    AppVersion pgmVersion;

    #if LOGGING
     m_pLog = new wxLogWindow( m_pAppWin, _T(" CodeSnippets Log"),true,false);
     wxLog::SetActiveTarget( m_pLog);
     m_pLog->Flush();
     m_pLog->GetFrame()->SetSize(20,20,600,300);
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
    wxString m_ConfigFolder = stdPaths.GetUserDataDir();
    wxString m_ExecuteFolder = stdPaths.GetDataDir();

    //GTK GetConfigFolder is returning double "//?, eg, "/home/pecan//.codeblocks"
    // remove the double //s from filename //+v0.4.11
    m_ConfigFolder.Replace(_T("//"),_T("/"));
    m_ExecuteFolder.Replace(_T("//"),_T("/"));
    LOGIT(wxT("CfgFolder[%s]"),m_ConfigFolder.c_str());
    LOGIT(wxT("ExecFolder[%s]"),m_ExecuteFolder.c_str());

    // if codesnippets.ini is in the executable folder, use it
    // else use the default config folder
    wxString
      m_CfgFilenameStr = m_ExecuteFolder + wxFILE_SEP_PATH + GetConfig()->AppName + _T(".ini");
    if (::wxFileExists(m_CfgFilenameStr)) {;/*OK Use exe path*/}
    else //use the default.conf folder
        m_CfgFilenameStr = m_ConfigFolder + wxFILE_SEP_PATH + GetConfig()->AppName + _T(".ini");
    GetConfig()->SettingsSnippetsCfgFullPath = m_CfgFilenameStr;
     LOGIT( _T("SettingsSnippetsCfgFullPath[%s]"),GetConfig()->SettingsSnippetsCfgFullPath.c_str() );

    // ---------------------------------------
    // Initialize Globals
    // ---------------------------------------
    GetConfig()->SettingsLoad();

    // ---------------------------------------
    // load tree icons/images
    // ---------------------------------------
    GetConfig()->pSnipImages = new SnipImages();

    //-// Check Destroyed windows for our docked window
    //-Connect( wxEVT_DESTROY,
    //-    (wxObjectEventFunction) (wxEventFunction)
    //-    (wxCommandEventFunction) &CodeSnippets::OnWindowDestroy);

    // ---------------------------------------
    // setup snippet tree docking window
    // ---------------------------------------
    CreateSnippetWindow();
}

// ----------------------------------------------------------------------------
void CodeSnippets::OnRelease(bool appShutDown)
// ----------------------------------------------------------------------------
{
    if (not GetSnippetsWindow()) return;

	CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
	evt.pWindow = GetSnippetsWindow();
	Manager::Get()->GetAppWindow()->ProcessEvent(evt);

	GetSnippetsWindow()->Destroy();
	SetSnippetsWindow(0);
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
    // ---------------------------------------
    // setup snippet tree docking window
    // ---------------------------------------
    GetConfig()->pMainFrame  = Manager::Get()->GetAppWindow() ;
	SetSnippetsWindow( new CodeSnippetsWindow(GetConfig()->pMainFrame));

    // Floating windows must be set by their parent
   #if !defined(BUILDING_PLUGIN)
    // We can position an application window
    GetSnippetsWindow()->SetSize(GetConfig()->windowXpos, GetConfig()->windowYpos,
            GetConfig()->windowWidth, GetConfig()->windowHeight);
   #endif

	CodeBlocksDockEvent evt(cbEVT_ADD_DOCK_WINDOW);
	evt.name = _T("CodeSnippetsPane");
	evt.title = _("CodeSnippets");
	evt.pWindow = GetSnippetsWindow();
	evt.dockSide = CodeBlocksDockEvent::dsFloating;
	// A bug in wxAUI interferes with the main menu when floating windows
	#if defined(__WXGTK__)
	  evt.dockSide = CodeBlocksDockEvent::dsBottom;
    #endif
	evt.desiredSize.Set(300, 400);
	evt.floatingSize.Set(300, 400);
	evt.minimumSize.Set(1, 1);
	Manager::Get()->GetAppWindow()->ProcessEvent(evt);

   #if defined(BUILDING_PLUGIN)
        #if !defined(__WXGTK__)
            // Set floating window to last position and size (except linux with buggy wxAUI)
            GetSnippetsWindow()->GetParent()->SetSize(GetConfig()->windowXpos, GetConfig()->windowYpos,
                    GetConfig()->windowWidth, GetConfig()->windowHeight);
        #endif
        //    LOGIT( _T("[%s]X[%d]Y[%d]Width[%d]Height[%d]"),
        //            GetSnippetsWindow()->GetParent()->GetName().c_str(),
        //            GetConfig()->windowXpos, GetConfig()->windowYpos,
        //            GetConfig()->windowWidth, GetConfig()->windowHeight);
        //GetSnippetsWindow()->GetParent()->SetSize(20,20,300,400);
        wxWindow* p = GetSnippetsWindow()->GetParent();
        int x,y,w,h;
        p->GetPosition(&x,&y); p->GetSize(&w,&h);
         LOGIT(  _T("WinPostCreate[%s]X[%d]Y[%d]Width[%d]Height[%d]"), p->GetName().c_str(),x,y,w,h);
   #endif

}
// ----------------------------------------------------------------------------
void CodeSnippets::SetSnippetsWindow(CodeSnippetsWindow* p)
// ----------------------------------------------------------------------------
{
    m_pSnippetsWindow = p;
    GetConfig()->pSnippetsWindow = p;
}
// ----------------------------------------------------------------------------
void CodeSnippets::OnViewSnippets(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    if (not GetSnippetsWindow())
    {   // Snippets Window is closed, initialize and open it.
        // ---------------------------------------
        // setup snippet tree docking window
        // ---------------------------------------
        CreateSnippetWindow();
    }
    // remember last window position
    if ( IsWindowReallyShown(GetSnippetsWindow()) ) do
    {   if (not event.IsChecked())
        {   //wxAUI is so lousy, we have to check that it's *not* giving us
            // the position and size of CodeBlocks.
            wxWindow* pw = GetConfig()->GetSnippetsWindow()->GetParent();
            if (pw && (pw == GetConfig()->GetMainFrame()) ) break;
            GetConfig()->SettingsSaveWinPosition();
        }
    }while(0);

	CodeBlocksDockEvent evt(event.IsChecked() ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
	evt.pWindow = GetSnippetsWindow();
	Manager::Get()->GetAppWindow()->ProcessEvent(evt);
}

// ----------------------------------------------------------------------------
void CodeSnippets::OnUpdateUI(wxUpdateUIEvent& /*event*/)
// ----------------------------------------------------------------------------
{
    if (not GetSnippetsWindow())
    {   Manager::Get()->GetAppWindow()->GetMenuBar()->Check(idViewSnippets, false);
         return;
    }
	// Check if the Code Snippets window is visible, if it's not, uncheck the menu item
	Manager::Get()->GetAppWindow()->GetMenuBar()->Check(idViewSnippets, IsWindowReallyShown(GetSnippetsWindow()));
}
// ----------------------------------------------------------------------------
void CodeSnippets::OnIdle(wxIdleEvent& event)
// ----------------------------------------------------------------------------
{
    // Because the current wxAUI is such a lousy interface, we have to poll
    // our docked window to see if it has closed. There are no events
    // that tell us if the user closed the docked or floating window.

    if (not GetSnippetsWindow()) return;
    bool bOpen = IsWindowReallyShown(GetSnippetsWindow());
    if ( (not bOpen) && GetSnippetsWindow())
    {   //Docked window is closed. Release our resources
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

    // check that it's us that was activated
    if (not event.GetActive()) { event.Skip();return; }

    // Wait until CodeSnippets actually has a file open
    if (not GetConfig()->pSnippetsWindow) return;

    //LOGIT( _T(" OnActivate Plugin") );

        CodeSnippetsWindow* p = GetConfig()->pSnippetsWindow;
        p->CheckForExternallyModifiedFiles();

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

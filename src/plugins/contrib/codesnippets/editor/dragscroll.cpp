/***************************************************************
 * Name:      dragscroll.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    Pecan<>
 * Copyright: (c) Pecan
 * License:   GPL
 **************************************************************/

#if defined(__GNUG__) && !defined(__APPLE__)
	#pragma implementation "dragscroll.h"
#endif


#include <sdk.h>
#ifndef CB_PRECOMP
	#include <wx/intl.h>
	#include <wx/listctrl.h>
	//-#include "cbeditor.h"
	#include "configmanager.h"
	//-#include "editormanager.h"
	#include "manager.h"
	#include "personalitymanager.h"
	#include "sdk_events.h" // EVT_APP_STARTUP_DONE
#endif
#include <wx/fileconf.h> // wxFileConfig
#include "cbstyledtextctrl.h"
#include "snippetsconfig.h"
#include "scbeditor.h"
#include "seditormanager.h"
#include "dragscroll.h"
#include "dragscrollcfg.h"
#include "version.h" //logit debugging
#include "dragscrollevent.h" //logit debugging

// Register the plugin
namespace
{
    //-PluginRegistrant<cbDragScroll> reg(_T("cbDragScroll"));

};

int ID_DLG_DONE = wxNewId();

// ----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(cbDragScroll, cbPlugin)
	// add events here...
    EVT_UPDATE_UI(ID_DLG_DONE, cbDragScroll::OnDoConfigRequests)
    //EVT_DRAGSCROLL_ADD_WINDOW(wxID_ANY, cbDragScroll::OnDragScrollEventAddWindow)
    //EVT_DRAGSCROLL_REMOVE_WINDOW(wxID_ANY, cbDragScroll::OnDragScrollEventRemoveWindow)
    //EVT_DRAGSCROLL_RESCAN(wxID_ANY, cbDragScroll::OnDragScrollEventRescan)
    //EVT_DRAGSCROLL_LOAD_CONFIG(wxID_ANY, cbDragScroll::OnDragScrollEvent_RereadConfig)
    EVT_DRAGSCROLL_EVENT(wxID_ANY, cbDragScroll::OnDragScrollEvent_Dispatcher)
    //-EVT_DRAGSCROLL_RESCAN(wxID_ANY, cbDragScroll::OnDragScrollTestRescan)
END_EVENT_TABLE()
// ----------------------------------------------------------------------------
//  Statics
// ----------------------------------------------------------------------------
// global used by mouse events to get user configuration settings
cbDragScroll* cbDragScroll::pDragScroll;

// ----------------------------------------------------------------------------
cbDragScroll::cbDragScroll()
// ----------------------------------------------------------------------------
{
	//ctor
	// anchor to this one and only object
    pDragScroll = this;
    m_pMouseEventsHandler = new MouseEventsHandler();
}
// ----------------------------------------------------------------------------
cbDragScroll::~cbDragScroll()
// ----------------------------------------------------------------------------
{
	//dtor
	delete m_pMouseEventsHandler;
	m_pMouseEventsHandler = 0;
}

// ----------------------------------------------------------------------------
void cbDragScroll::OnAttach()
// ----------------------------------------------------------------------------
{
	// do whatever initialization you need for your plugin
	// NOTE: after this function, the inherited member variable
	// IsAttached() will be TRUE...
	// You should check for it in other functions, because if it
	// is FALSE, it means that the application did *not* "load"
	// (see: does not need) this plugin...

    pMyLog = NULL;
    m_bNotebooksAttached = false;
    plugin = 0;

    m_pCB_AppWindow = Manager::Get()->GetAppWindow();

////    #if defined(LOGGING)
////        wxWindow* pcbWindow = m_pCB_AppWindow;
////        wxLog::EnableLogging(true);
////        /*wxLogWindow**/ pMyLog = new wxLogWindow(pcbWindow, wxT("DragScroll"), true, false);
////        wxLog::SetActiveTarget(pMyLog);
////        pMyLog->Flush();
////        pMyLog->GetFrame()->Move(20,20);
////        wxLogMessage(_T("Logging cbDragScroll version %s"),wxString(wxT(VERSION)).c_str());
////        LOGIT( _T("DragScroll::cbDragScroll Address is:[%p]"), pDragScroll);
////	#endif

    if (not m_pMouseEventsHandler )
        m_pMouseEventsHandler = new MouseEventsHandler();

    // names of windows we're allowed to attach
    //-m_UsableWindows.Add(_T("text")); // CB compiler logs
    m_UsableWindows.Add(_T("listctrl"));
    m_UsableWindows.Add(_T("textctrl"));
    m_UsableWindows.Add(_T("treectrl"));
    m_UsableWindows.Add(_T("treeAll"));
    m_UsableWindows.Add(_T("treeMembers"));
    //-m_UsableWindows.Add(_T("csTreeCtrl"));
    m_UsableWindows.Add(_T("sciwindow"));

    MouseDragScrollEnabled  = true;
    MouseEditorFocusEnabled = false;
    MouseFocusEnabled       = false;
    MouseDragDirection      = 0;
    MouseDragKey            = 0;
    MouseDragSensitivity    = 5;
    MouseToLineRatio        = 30;
    //-MouseRightKeyCtrl       = 0 ;
    MouseContextDelay       = 10;

    // Create filename like cbDragScroll.ini
    //memorize the key file name as {%HOME%}\cbDragScroll.ini
    m_ConfigFolder = ConfigManager::GetConfigFolder();
    m_DataFolder = ConfigManager::GetDataFolder();
    m_ExecuteFolder = FindAppPath(wxTheApp->argv[0], ::wxGetCwd(), wxEmptyString);
    wxString appName = wxConfigBase::Get()->GetAppName();

    //GTK GetConfigFolder is returning double "//?, eg, "/home/pecan//.codeblocks"
    // remove the double //s from filename //+v0.4.11
    m_ConfigFolder.Replace(_T("//"),_T("/"));
    m_ExecuteFolder.Replace(_T("//"),_T("/"));

    // get the CodeBlocks "personality" argument
    wxString m_Personality = Manager::Get()->GetPersonalityManager()->GetPersonality();
	if (m_Personality == wxT("default")) m_Personality = wxEmptyString;
     LOGIT( _T("Personality is[%s]"), m_Personality.GetData() );

    wxString cfgFilenameStr = wxEmptyString;
    do{
        // if pgm.ini is in the executable folder, use it
        cfgFilenameStr = m_ExecuteFolder + wxFILE_SEP_PATH + GetConfig()->AppName + _T(".ini");
         LOGIT( _T("ExecCfgName[%s]"),cfgFilenameStr.c_str() );
        if (::wxFileExists(cfgFilenameStr)) break;

        //if codeblocks has codesnippets.ini, use it
        cfgFilenameStr = m_ConfigFolder+wxFILE_SEP_PATH + GetConfig()->AppName + _T(".ini");
        cfgFilenameStr = cfgFilenameStr.Lower();
        cfgFilenameStr.Replace(wxT("codesnippets"), wxT("codeblocks"),false);
         LOGIT( _T("MSWCfgName 3[%s]"),cfgFilenameStr.c_str() );
        if (::wxFileExists(cfgFilenameStr) ) break;

        // if Linux has codesnippets.ini, use it
        cfgFilenameStr.Replace(wxT("codeblocks"),wxT(".codeblocks"));
        LOGIT( _T("UNXCfgName[%s]"),cfgFilenameStr.c_str() );
        if (::wxFileExists(cfgFilenameStr)) break;

        //use the default.conf folder
        cfgFilenameStr = m_ConfigFolder + wxFILE_SEP_PATH + GetConfig()->AppName + _T(".ini");
        LOGIT( _T("DefaultCfgName[%s]"),cfgFilenameStr.c_str() );
    }while(0);

    // ----------------------------------------------------------------------
    // For CodeSnippets version of DragScroll, use the .conf file set by CodeSnippets
    // ----------------------------------------------------------------------
    cfgFilenameStr = GetConfig()->SettingsSnippetsCfgPath;

    m_CfgFilenameStr = cfgFilenameStr;
    LOGIT(_T("DragScroll Config Filename:[%s]"), cfgFilenameStr.GetData());
    // read configuaton file
    wxFileConfig cfgFile(wxEmptyString,     // appname
                        wxEmptyString,      // vendor
                        cfgFilenameStr,     // local filename
                        wxEmptyString,      // global file
                        wxCONFIG_USE_LOCAL_FILE);

	cfgFile.Read(_T("MouseDragScrollEnabled"),  &MouseDragScrollEnabled ) ;
	cfgFile.Read(_T("MouseEditorFocusEnabled"), &MouseEditorFocusEnabled ) ;
	cfgFile.Read(_T("MouseFocusEnabled"),       &MouseFocusEnabled ) ;
	cfgFile.Read(_T("MouseDragDirection"),      &MouseDragDirection ) ;
	cfgFile.Read(_T("MouseDragKey"),            &MouseDragKey ) ;
	cfgFile.Read(_T("MouseDragSensitivity"),    &MouseDragSensitivity ) ;
	cfgFile.Read(_T("MouseToLineRatio"),        &MouseToLineRatio ) ;
	//-cfgFile.Read(_T("MouseRightKeyCtrl"),       &MouseRightKeyCtrl) ;
	cfgFile.Read(_T("MouseContextDelay"),       &MouseContextDelay) ;

	// Don't allow less than 10 mils on context/scroll delay.
	if ( MouseContextDelay < 10) { MouseContextDelay = 10;}

    #ifdef LOGGING
        LOGIT(_T("MouseDragScrollEnabled:%d"),  MouseDragScrollEnabled ) ;
        LOGIT(_T("MouseEditorFocusEnabled:%d"), MouseEditorFocusEnabled ) ;
        LOGIT(_T("MouseFocusEnabled:%d"),       MouseFocusEnabled ) ;
        LOGIT(_T("MouseDragDirection:%d"),      MouseDragDirection ) ;
        LOGIT(_T("MouseDragKey:%d"),            MouseDragKey ) ;
        LOGIT(_T("MouseDragSensitivity:%d"),    MouseDragSensitivity ) ;
        LOGIT(_T("MouseToLineRatio:%d"),        MouseToLineRatio ) ;
        //-LOGIT(_T("MouseRightKeyCtrl:%d"),       MouseRightKeyCtrl ) ;
        LOGIT(_T("MouseContextDelay:%d"),       MouseContextDelay ) ;
    #endif //LOGGING

    // Pointer to "Search Results" Window (first listCtrl window)
    m_pSearchResultsWindow = 0;

    // Catch creation of windows
    Connect( wxEVT_CREATE,
	(wxObjectEventFunction) (wxEventFunction)
	(wxCommandEventFunction) &cbDragScroll::OnWindowOpen);

    // Catch Destroyed windows
    Connect( wxEVT_DESTROY,
	(wxObjectEventFunction) (wxEventFunction)
	(wxCommandEventFunction) &cbDragScroll::OnWindowClose);


    // Set current plugin version
	plugin = (PluginInfo*)(Manager::Get()->GetPluginManager()->GetPluginInfo(this));
	if (plugin)
	{
        //-m_DragScrollFirstId = wxString::Format( _T("%ld"), idEvtDragScrollFirstId);
        plugin->version = VERSION;
        //-plugin->authorWebsite = m_DragScrollFirstId;
	}
	else
	{

	}

	// register event sink
    //-Manager::Get()->RegisterEventSink(cbEVT_APP_STARTUP_DONE, new cbEventFunctor<cbDragScroll, CodeBlocksEvent>(this, &cbDragScroll::OnAppStartupDone));

	return ;
}
// ----------------------------------------------------------------------------
void cbDragScroll::OnRelease(bool appShutDown)
// ----------------------------------------------------------------------------
{
	// do de-initialization for your plugin
	// if appShutDown is false, the plugin is unloaded because Code::Blocks is being shut down,
	// which means you must not use any of the SDK Managers
	// NOTE: after this function, the inherited member variable
	// IsAttached() will be FALSE...

	// Remove all Mouse event handlers
	DetachAll();
}
// ----------------------------------------------------------------------------
cbConfigurationPanel* cbDragScroll::GetConfigurationPanel(wxWindow* parent)
// ----------------------------------------------------------------------------
{
	//create and display the configuration dialog for your plugin
    if(!IsAttached()) {	return 0;}
    // Create a configuration dialogue and hand it off to codeblocks

    //cbConfigurationPanel* pDlg = new cbDragScrollCfg(parent, this);
    cbDragScrollCfg* pDlg = new cbDragScrollCfg(parent, this);
    // set current mouse scrolling options
    pDlg->SetMouseDragScrollEnabled ( MouseDragScrollEnabled );
    pDlg->SetMouseEditorFocusEnabled ( MouseEditorFocusEnabled );
    pDlg->SetMouseFocusEnabled ( MouseFocusEnabled );
    pDlg->SetMouseDragDirection ( MouseDragDirection );
    pDlg->SetMouseDragKey ( MouseDragKey );
    pDlg->SetMouseDragSensitivity ( MouseDragSensitivity );
    pDlg->SetMouseToLineRatio ( MouseToLineRatio );
    //-pDlg->SetMouseRightKeyCtrl ( MouseRightKeyCtrl );
    pDlg->SetMouseContextDelay ( MouseContextDelay );

    // when the configuration panel is closed with OK, OnDialogDone() will be called
    return pDlg;
}
// ----------------------------------------------------------------------------
int cbDragScroll::Configure(wxWindow* parent)
// ----------------------------------------------------------------------------
{
	if ( !IsAttached() )
		return -1;

	// Creates and displays the configuration dialog for the plugin
	cbConfigurationDialog dlg(Manager::Get()->GetAppWindow(), wxID_ANY, wxT("DragScroll"));
	cbConfigurationPanel* panel = GetConfigurationPanel(&dlg);
	if (panel)
	{
		dlg.AttachConfigurationPanel(panel);
		if (parent)
            CenterChildOnParent( parent, &dlg);
        else
            PlaceWindow(&dlg,pdlConstrain);

		return dlg.ShowModal() == wxID_OK ? 0 : -1;
	}
	return -1;
}
// ----------------------------------------------------------------------------
void cbDragScroll::CenterChildOnParent(wxWindow* parent, wxWindow* child)
// ----------------------------------------------------------------------------
{

    int displayX; int displayY;
    ::wxDisplaySize(&displayX, &displayY);

    int childx = 1, childy = 1;
    // place bottomLeft child at bottomLeft of parent window
        int childsizex,childsizey;
        parent->GetScreenPosition(&childx,&childy);
        child->GetSize(&childsizex,&childsizey);
        // Make sure child is not off right/bottom of screen
        if ( (childx+childsizex) > displayX)
            childx = displayX-childsizex;
        if ( (childy+childsizey) > displayY)
            childy = displayY-childsizey;
        // Make sure child is not off left/top of screen
        if ( childx < 1) childx = 1;
        if ( childy < 1) childy = 1;

    child->Move( childx, childy);
    return;
}
// ----------------------------------------------------------------------------
void cbDragScroll::OnDialogDone(cbDragScrollCfg* pDlg)
// ----------------------------------------------------------------------------
{
    // The configuration panel has run its OnApply() function.
    // So here it's like we were using ShowModal() and it just returned wxID_OK.

    MouseDragScrollEnabled  = pDlg->GetMouseDragScrollEnabled();
    MouseEditorFocusEnabled = pDlg->GetMouseEditorFocusEnabled();
    MouseFocusEnabled       = pDlg->GetMouseFocusEnabled();
    MouseDragDirection      = pDlg->GetMouseDragDirection();
    MouseDragKey            = pDlg->GetMouseDragKey();
    MouseDragSensitivity    = pDlg->GetMouseDragSensitivity();
    MouseToLineRatio        = pDlg->GetMouseToLineRatio();
    //- MouseRightKeyCtrl       = pDlg->GetMouseRightKeyCtrl(); removed
    MouseContextDelay       = pDlg->GetMouseContextDelay();
    #ifdef LOGGING
     LOGIT(_T("MouseDragScrollEnabled:%d"),  MouseDragScrollEnabled);
     LOGIT(_T("MouseEditorFocusEnabled:%d"), MouseEditorFocusEnabled);
     LOGIT(_T("MouseFocusEnabled:%d"),       MouseFocusEnabled);
     LOGIT(_T("MouseDragDirection:%d"),      MouseDragDirection);
     LOGIT(_T("MouseDragKey:%d"),            MouseDragKey);
     LOGIT(_T("MouseDragSensitivity:%d"),    MouseDragSensitivity);
     LOGIT(_T("MouseToLineRatio:%d"),        MouseToLineRatio);
     //-LOGIT(_T("MouseRightKeyCtrl:%d"),       MouseRightKeyCtrl);
     LOGIT(_T("MouseContextDelay:%d"),       MouseContextDelay);
     LOGIT(_T("-----------------------------"));
    #endif //LOGGING

    // Post a pending request to later update the configuration requests
    // Doing work here will stall the dlg window on top of the editor
    wxUpdateUIEvent eventdone(ID_DLG_DONE);
    eventdone.SetEventObject( m_pCB_AppWindow );
    m_pCB_AppWindow->GetEventHandler()->AddPendingEvent(eventdone);

    // don't delete dlg; Codeblocks should destroy the dialog

}//OnDialogDone
// ----------------------------------------------------------------------------
void cbDragScroll::OnDoConfigRequests(wxUpdateUIEvent& event)
// ----------------------------------------------------------------------------
{
    // This is an event triggered by OnDialogDone() to update config settings

    LOGIT(_T("OnDoConfigRequest event"));

    // Attach or Detach windows to match  Mouse Enabled config setting
    if (GetMouseDragScrollEnabled() )  //v04.14
    {   if (not m_bNotebooksAttached)
        {
            AttachRecursively(m_pCB_AppWindow);
            m_bNotebooksAttached = true;
        }
    }//fi
    else {
        DetachAll();
        m_bNotebooksAttached = false;
    }//esle

    // update/write configuaton file
    wxFileConfig cfgFile(wxEmptyString,     // appname
                        wxEmptyString,      // vendor
                        m_CfgFilenameStr,   // local filename
                        wxEmptyString,      // global file
                        wxCONFIG_USE_LOCAL_FILE);

	cfgFile.Write(_T("MouseDragScrollEnabled"),  MouseDragScrollEnabled ) ;
	cfgFile.Write(_T("MouseEditorFocusEnabled"), MouseEditorFocusEnabled ) ;
	cfgFile.Write(_T("MouseFocusEnabled"),       MouseFocusEnabled ) ;
	cfgFile.Write(_T("MouseDragDirection"),      MouseDragDirection ) ;
	cfgFile.Write(_T("MouseDragKey"),            MouseDragKey ) ;
	cfgFile.Write(_T("MouseDragSensitivity"),    MouseDragSensitivity ) ;
	cfgFile.Write(_T("MouseToLineRatio"),        MouseToLineRatio ) ;
	//-cfgFile.Write(_T("MouseRightKeyCtrl"),       MouseRightKeyCtrl ) ;
	cfgFile.Write(_T("MouseContextDelay"),       MouseContextDelay ) ;
	cfgFile.Flush();

    #ifdef LOGGING
        LOGIT( _T("DragScroll:Writing config file[%s]"), m_CfgFilenameStr.c_str());
        LOGIT(_T("MouseDragScrollEnabled:%d"),  MouseDragScrollEnabled ) ;
        LOGIT(_T("MouseEditorFocusEnabled:%d"), MouseEditorFocusEnabled ) ;
        LOGIT(_T("MouseFocusEnabled:%d"),       MouseFocusEnabled ) ;
        LOGIT(_T("MouseDragDirection:%d"),      MouseDragDirection ) ;
        LOGIT(_T("MouseDragKey:%d"),            MouseDragKey ) ;
        LOGIT(_T("MouseDragSensitivity:%d"),    MouseDragSensitivity ) ;
        LOGIT(_T("MouseToLineRatio:%d"),        MouseToLineRatio ) ;
        //-LOGIT(_T("MouseRightKeyCtrl:%d"),       MouseRightKeyCtrl ) ;
        LOGIT(_T("MouseContextDelay:%d"),       MouseContextDelay ) ;
    #endif //LOGGING

}
// ----------------------------------------------------------------------------
void cbDragScroll::OnDragScrollEvent_Dispatcher(wxCommandEvent& event )
// ----------------------------------------------------------------------------
{
    // Received a request to process an event

    if ( not IsAttached() )
        return;

    switch ( event.GetId() )
    {
	    case idDragScrollAddWindow:
	    {
	        if (not GetMouseDragScrollEnabled() )
                return;
            OnDragScrollEventAddWindow( event );
	        break;
	    }
	    case idDragScrollRemoveWindow:
	    {
            OnDragScrollEventRemoveWindow( event );
	        break;
	    }
	    case idDragScrollRescan:
	    {
 	        if (not GetMouseDragScrollEnabled() )
                return;
           OnDragScrollEventRescan( event );
	        break;
	    }
	    case idDragScrollReadConfig:
        {
            OnDragScrollEvent_RereadConfig( event );
	        break;
        }
	    case idDragScrollInvokeConfig:
        {
            OnDragScrollEvent_InvokeConfig( event );
	        break;
        }
        default: break;
    }//switch
}
// ----------------------------------------------------------------------------
void cbDragScroll::OnDragScrollEventAddWindow(wxCommandEvent& event )
// ----------------------------------------------------------------------------
{
    // Received a request to add a scrollable window

    wxWindow* pWin = (wxWindow*)event.GetEventObject();
    wxString winName = event.GetString();
    if ( (not winName.IsEmpty()) && (wxNOT_FOUND == m_UsableWindows.Index(winName)) )
        m_UsableWindows.Add(winName);

    Attach( pWin );
    event.Skip();
    #if defined(LOGGING)
    int windowID = event.GetId();
    LOGIT( _T("cbDragScroll::OnDragScrollEvent AddWindow[%d][%p][%s]"), windowID, pWin, pWin->GetName().c_str());
    #endif
}
// ----------------------------------------------------------------------------
void cbDragScroll::OnDragScrollEventRemoveWindow(wxCommandEvent& event )
// ----------------------------------------------------------------------------
{
    // Received a request to remove a scrollable window
    wxWindow* pWin = (wxWindow*)event.GetEventObject();
    Detach( pWin );
    event.Skip();
    #if defined(LOGGING)
    int windowID = event.GetId();
    LOGIT( _T("cbDragScroll::OnDragScrollEvent RemoveWindow[%d][%p][%s]"), windowID, pWin, pWin->GetName().c_str());
    #endif
}
// ----------------------------------------------------------------------------
void cbDragScroll::OnDragScrollEventRescan(wxCommandEvent& event )
// ----------------------------------------------------------------------------
{
    // Received a request to rescan for child windows starting
    // at the given event.GetWindow() pointer. This allows us
    // to scroll windows not on the main frame tree.

    // But first, clean out any dead window pointers. This occurs
    // when a window is deleted w/o being closed first, eg.
    // ThreadSearch cbStyledTextCtrl preView control.
    CleanUpWindowPointerArray();

    // Rescan for scrollable children starting from the window provided
    wxWindow* pWin = (wxWindow*)event.GetEventObject();
    wxString winName = event.GetString();
    if ( (not winName.IsEmpty()) && (wxNOT_FOUND == m_UsableWindows.Index(winName)) )
        m_UsableWindows.Add(winName);
    AttachRecursively( pWin );
    event.Skip();
    #if defined(LOGGING)
    LOGIT( _T("cbDragScroll::OnDragScrollEvent Rescan[%p][%s]"), pWin, pWin->GetName().c_str());
    #endif
}
// ----------------------------------------------------------------------------
void cbDragScroll::OnDragScrollEvent_RereadConfig(wxCommandEvent& event )
// ----------------------------------------------------------------------------
{

    #if defined(LOGGING)
    LOGIT( _T("CodeSnippets:DragScroll RereadConfig"));
    #endif
    // ----------------------------------------------------------------------
    // For CodeSnippets version of DragScroll, use the .conf file set by CodeSnippets
    // ----------------------------------------------------------------------
    wxString cfgFilenameStr = GetConfig()->SettingsSnippetsCfgPath;

    m_CfgFilenameStr = cfgFilenameStr;
    LOGIT(_T("DragScroll Config Filename:[%s]"), cfgFilenameStr.GetData());
    // read configuaton file
    wxFileConfig cfgFile(wxEmptyString,     // appname
                        wxEmptyString,      // vendor
                        cfgFilenameStr,     // local filename
                        wxEmptyString,      // global file
                        wxCONFIG_USE_LOCAL_FILE);

	cfgFile.Read(_T("MouseDragScrollEnabled"),  &MouseDragScrollEnabled ) ;
	cfgFile.Read(_T("MouseEditorFocusEnabled"), &MouseEditorFocusEnabled ) ;
	cfgFile.Read(_T("MouseFocusEnabled"),       &MouseFocusEnabled ) ;
	cfgFile.Read(_T("MouseDragDirection"),      &MouseDragDirection ) ;
	cfgFile.Read(_T("MouseDragKey"),            &MouseDragKey ) ;
	cfgFile.Read(_T("MouseDragSensitivity"),    &MouseDragSensitivity ) ;
	cfgFile.Read(_T("MouseToLineRatio"),        &MouseToLineRatio ) ;
	//-cfgFile.Read(_T("MouseRightKeyCtrl"),       &MouseRightKeyCtrl) ;
	cfgFile.Read(_T("MouseContextDelay"),       &MouseContextDelay) ;

	// Don't allow less than 10 mils on context/scroll delay.
	if ( MouseContextDelay < 10) { MouseContextDelay = 10;}

    #ifdef LOGGING
        LOGIT(_T("MouseDragScrollEnabled:%d"),  MouseDragScrollEnabled ) ;
        LOGIT(_T("MouseEditorFocusEnabled:%d"), MouseEditorFocusEnabled ) ;
        LOGIT(_T("MouseFocusEnabled:%d"),       MouseFocusEnabled ) ;
        LOGIT(_T("MouseDragDirection:%d"),      MouseDragDirection ) ;
        LOGIT(_T("MouseDragKey:%d"),            MouseDragKey ) ;
        LOGIT(_T("MouseDragSensitivity:%d"),    MouseDragSensitivity ) ;
        LOGIT(_T("MouseToLineRatio:%d"),        MouseToLineRatio ) ;
        //-LOGIT(_T("MouseRightKeyCtrl:%d"),       MouseRightKeyCtrl ) ;
        LOGIT(_T("MouseContextDelay:%d"),       MouseContextDelay ) ;
    #endif //LOGGING

}
// ----------------------------------------------------------------------------
void cbDragScroll::OnDragScrollEvent_InvokeConfig(wxCommandEvent& event )
// ----------------------------------------------------------------------------
{
    wxWindow* parent = (wxWindow*)event.GetEventObject() ;
    Configure( parent );
}
// ----------------------------------------------------------------------------
void cbDragScroll::OnDragScrollTestRescan(wxCommandEvent& event )
// ----------------------------------------------------------------------------
{
    asm("int3"); /*trap*/
    #if defined(LOGGING)
    LOGIT( _T("TEST RESCAN DragScrollevent"));
    #endif
}
// ----------------------------------------------------------------------------
void cbDragScroll::CleanUpWindowPointerArray()
// ----------------------------------------------------------------------------
{
    wxArrayPtrVoid tmpAry = m_EditorPtrs;
    for (size_t i=0; i < tmpAry.GetCount(); ++i )
    	if ( not winExists((wxWindow*)tmpAry.Item(i)) )
        {    m_EditorPtrs.RemoveAt(i);
            #if defined(LOGGING)
            LOGIT( _T("csDragScroll CleanedUp[%p]"), tmpAry.Item(i));
            #endif
        }
}
// ----------------------------------------------------------------------------
MouseEventsHandler* cbDragScroll::GetMouseEventsHandler()
// ----------------------------------------------------------------------------
{
    if (not m_pMouseEventsHandler)
        m_pMouseEventsHandler = new MouseEventsHandler();
    return m_pMouseEventsHandler;
}
// ----------------------------------------------------------------------------
bool cbDragScroll::IsAttachedTo(wxWindow* p)
// ----------------------------------------------------------------------------

{
    if ( wxNOT_FOUND == m_EditorPtrs.Index(p))
        return false;
    return true;

}//IsAttachedTo
// ----------------------------------------------------------------------------
void cbDragScroll::Attach(wxWindow *p)
// ----------------------------------------------------------------------------{
{
	if (!p || IsAttachedTo(p))
		return;		// already attached !!!

    // allow only static windows to be attached by codeblocks
    // Disappearing frames/windows cause crashes
    // eg., wxArrayString m_UsableWindows = "sciwindow notebook";

    wxString windowName = p->GetName().MakeLower();

    // memorize "Search Results" Window address
    // We're assuming it's the first listcrl window found
    if ( (not m_pSearchResultsWindow) && (windowName ==  wxT("listctrl")) )
    {   m_pSearchResultsWindow = p;
        #ifdef LOGGING
         LOGIT(wxT("SearchResultsWindow: %p"),p );
        #endif
    }

    if (wxNOT_FOUND == m_UsableWindows.Index(windowName,false/*casecmp*/))
     {
        LOGIT(wxT("csDS::Attach skipping [%s]"), p->GetName().c_str());
        return;
     }

    LOGIT(wxT("csDS::Attach - attaching to [%s] %p"), p->GetName().c_str(),p);

    //add window to our array, create a mouse event handler
    // and memorize event handler instance
    m_EditorPtrs.Add(p);

    //-MouseEventsHandler* thisEvtHndlr = new MouseEventsHandler();
    //-m_EventHandlerArray.Add(thisEvtHndlr);
    MouseEventsHandler* thisEvtHndlr = GetMouseEventsHandler();

    p->Connect(wxEVT_MIDDLE_DOWN,
                    (wxObjectEventFunction)(wxEventFunction)
                    (wxMouseEventFunction)&MouseEventsHandler::OnMouseEvent,
                     NULL, thisEvtHndlr);
    p->Connect(wxEVT_MIDDLE_UP,
                    (wxObjectEventFunction)(wxEventFunction)
                    (wxMouseEventFunction)&MouseEventsHandler::OnMouseEvent,
                     NULL, thisEvtHndlr);
    p->Connect(wxEVT_RIGHT_DOWN,
                    (wxObjectEventFunction)(wxEventFunction)
                    (wxMouseEventFunction)&MouseEventsHandler::OnMouseEvent,
                     NULL, thisEvtHndlr);
    p->Connect(wxEVT_RIGHT_UP,
                    (wxObjectEventFunction)(wxEventFunction)
                    (wxMouseEventFunction)&MouseEventsHandler::OnMouseEvent,
                     NULL, thisEvtHndlr);
    p->Connect(wxEVT_MOTION,
                    (wxObjectEventFunction)(wxEventFunction)
                    (wxMouseEventFunction)&MouseEventsHandler::OnMouseEvent,
                     NULL, thisEvtHndlr);
    p->Connect(wxEVT_ENTER_WINDOW,
                    (wxObjectEventFunction)(wxEventFunction)
                    (wxMouseEventFunction)&MouseEventsHandler::OnMouseEvent,
                     NULL, thisEvtHndlr);

    #if defined(LOGGING)
     LOGIT(_T("csDS:Attach Window:%p Handler:%p"), p,thisEvtHndlr);
    #endif
}

// ----------------------------------------------------------------------------
void cbDragScroll::AttachRecursively(wxWindow *p)
// ----------------------------------------------------------------------------{
{
 	if (!p)
		return;

	Attach(p);

 	// this is the standard way wxWidgets uses to iterate through children...
	for (wxWindowList::compatibility_iterator node = p->GetChildren().GetFirst();
		node;
		node = node->GetNext())
	{
		// recursively attach each child
		wxWindow *win = (wxWindow *)node->GetData();

		if (win)
			AttachRecursively(win);
	}
}
// ----------------------------------------------------------------------------
wxWindow* cbDragScroll::FindWindowRecursively(const wxWindow* parent, const wxWindow* handle)
// ----------------------------------------------------------------------------{
{//+v0.4.4
    if ( parent )
    {
        // see if this is the one we're looking for
        if ( parent == handle )
            return (wxWindow *)parent;

        // It wasn't, so check all its children
        for ( wxWindowList::compatibility_iterator node = parent->GetChildren().GetFirst();
              node;
              node = node->GetNext() )
        {
            // recursively check each child
            wxWindow *win = (wxWindow *)node->GetData();
            wxWindow *retwin = FindWindowRecursively(win, handle);
            if (retwin)
                return retwin;
        }
    }

    // Not found
    return NULL;
}
// ----------------------------------------------------------------------------
wxWindow* cbDragScroll::winExists(wxWindow *parent)
// ----------------------------------------------------------------------------{
{ //+v0.4.4

    if ( !parent )
    {
        return NULL;
    }

    // start at very top of wx's windows
    for ( wxWindowList::compatibility_iterator node = wxTopLevelWindows.GetFirst();
          node;
          node = node->GetNext() )
    {
        // recursively check each window & its children
        wxWindow* win = node->GetData();
        wxWindow* retwin = FindWindowRecursively(win, parent);
        if (retwin)
            return retwin;
    }

    return NULL;
}//winExists
// ----------------------------------------------------------------------------
void cbDragScroll::Detach(wxWindow* thisEditor)
// ----------------------------------------------------------------------------
{
    if ( (thisEditor) && (m_EditorPtrs.Index(thisEditor) != wxNOT_FOUND))
    {
         #if defined(LOGGING)
          LOGIT(_T("csDS:Detaching %p"), thisEditor);
         #endif

        //-int edIndex = m_EditorPtrs.Index(thisEditor);
        //-MouseEventsHandler* thisEvtHandler = (MouseEventsHandler*)m_EventHandlerArray.Item(edIndex);
        m_EditorPtrs.Remove(thisEditor);
        //-m_EventHandlerArray.Remove(thisEvtHandler);

        MouseEventsHandler* thisEvtHandler = GetMouseEventsHandler();
        // If win already deleted, dont worry about receiving events
	    if ( not winExists(thisEditor) )
	    {
	        LOGIT(_T("csDS:DetachAll window NOT found %p Handlr: %p"),
                    thisEditor, thisEvtHandler);
	    } else {
            thisEditor->Disconnect(wxEVT_MIDDLE_DOWN,
                            (wxObjectEventFunction)(wxEventFunction)
                            (wxMouseEventFunction)&MouseEventsHandler::OnMouseEvent,
                             NULL, thisEvtHandler);
            thisEditor->Disconnect(wxEVT_MIDDLE_UP,
                            (wxObjectEventFunction)(wxEventFunction)
                            (wxMouseEventFunction)&MouseEventsHandler::OnMouseEvent,
                             NULL, thisEvtHandler);
            thisEditor->Disconnect(wxEVT_RIGHT_DOWN,
                            (wxObjectEventFunction)(wxEventFunction)
                            (wxMouseEventFunction)&MouseEventsHandler::OnMouseEvent,
                             NULL, thisEvtHandler);
            thisEditor->Disconnect(wxEVT_RIGHT_UP,
                            (wxObjectEventFunction)(wxEventFunction)
                            (wxMouseEventFunction)&MouseEventsHandler::OnMouseEvent,
                             NULL, thisEvtHandler);
            thisEditor->Disconnect(wxEVT_MOTION,
                            (wxObjectEventFunction)(wxEventFunction)
                            (wxMouseEventFunction)&MouseEventsHandler::OnMouseEvent,
                             NULL, thisEvtHandler);
            thisEditor->Disconnect(wxEVT_ENTER_WINDOW,
                            (wxObjectEventFunction)(wxEventFunction)
                            (wxMouseEventFunction)&MouseEventsHandler::OnMouseEvent,
                             NULL, thisEvtHandler);
        }//fi (not winExists

        //-delete(thisEvtHandler);

        #if defined(LOGGING)
         LOGIT(_T("Detach: Editor:%p EvtHndlr: %p"),thisEditor,thisEvtHandler);
        #endif
    }//if (thisEditor..
}//Detach
// ----------------------------------------------------------------------------
void cbDragScroll::DetachAll()
// ----------------------------------------------------------------------------
{
	// delete all handlers
	LOGIT(wxT("csDS:DetachAll - detaching all [%d] targets"),m_EditorPtrs.GetCount() );

    // Detach from memorized windows and remove event handlers
    while( m_EditorPtrs.GetCount() )
    {
	    wxWindow* pw = (wxWindow*)m_EditorPtrs.Item(0);
        Detach(pw);
    }//elihw

    m_EditorPtrs.Empty();

        // -deprecated-
        ////// Free any leaked event handlers caused by disappearing windows
        ////// *Danger* There are dynamic events connected to these Event Handlers
        ////// Could cause segfaults. This shouldnt happen. Fix the bug if it does.
        ////for (int i=0; i < (int)m_EventHandlerArray.GetCount(); i++)
        ////{
        ////    MouseEventsHandler* pHdlr = (MouseEventsHandler*)m_EventHandlerArray.Item(i);
        ////    delete pHdlr;
        ////    LOGIT(_T("csDS:DetachAll - Removing Leaked EventHdl:[%p]"), pHdlr );
        ////}//rof

        ////m_EventHandlerArray.Empty();
    // say no windows attached
    m_bNotebooksAttached = false;
    m_pSearchResultsWindow = 0;
    return;

}//DetachAll
// ----------------------------------------------------------------------------
wxString cbDragScroll::FindAppPath(const wxString& argv0, const wxString& cwd, const wxString& appVariableName)
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

    if (wxIsAbsolutePath(argv0))
        return wxPathOnly(argv0);
    else
    {
        // Is it a relative path?
        wxString currentDir(cwd);
        if (currentDir.Last() != wxFILE_SEP_PATH)
            currentDir += wxFILE_SEP_PATH;

        str = currentDir + argv0;
        if (wxFileExists(str))
            return wxPathOnly(str);
    }

    // OK, it's neither an absolute path nor a relative path.
    // Search PATH.

    wxPathList pathList;
    pathList.AddEnvList(wxT("PATH"));
    str = pathList.FindAbsoluteValidPath(argv0);
    if (!str.IsEmpty())
        return wxPathOnly(str);

    // Failed
    return wxEmptyString;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//    cbDragScroll Routines to push/remove mouse event handlers
// ----------------------------------------------------------------------------
void cbDragScroll::OnAppStartupDone(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    // EVT_APP_STARTUP_DONE
    //attach windows
    LOGIT(_T("AppStartupDone"));

    OnAppStartupDoneInit();

    event.Skip();
    return;
}//OnAppStartupDone
// ----------------------------------------------------------------------------
void cbDragScroll::OnAppStartupDoneInit()
// ----------------------------------------------------------------------------
{
    // This routine may be entered multiple times during initializatin,
    // but the Attach() routine guards against duplicate window attaches.
    // This catches windows that open after we initialize.

    #if defined(LOGGING)
    LOGIT( _T("OnAppStartUpDoneInit()"));
    #endif
    if (not GetMouseDragScrollEnabled() )    //v04.14
        return;

    //-if (! m_bNotebooksAttached) //(pecan 2008/3/04)
    {
        AttachRecursively( m_pCB_AppWindow );
        m_bNotebooksAttached = true;
    }
}
// ----------------------------------------------------------------------------
void cbDragScroll::OnWindowOpen(wxEvent& event)
// ----------------------------------------------------------------------------
{
    // wxEVT_CREATE entry
    // Have to do this especially for split windows since CodeBlocks does not have
    // events when opening/closing split windows

    wxWindow* pWindow = (wxWindow*)(event.GetEventObject());

    // Some code (at times) is not issueing EVT_APP_STARTUP_DONE;
    // so here we do it ourselves. If not initialized and this is the first
    // scintilla window, initialize now.

    if ( (not m_bNotebooksAttached)
        && ( pWindow->GetName().Lower() == wxT("sciwindow")) )
    {
        #if defined(LOGGING)
        LOGIT( _T("OnWindowOpen[%s]"), pWindow->GetName().c_str());
        #endif
        OnAppStartupDoneInit();
    }

    // Attach a split window (or any other window)
    if ( m_bNotebooksAttached )
    {
        wxWindow* pWindow = (wxWindow*)(event.GetEventObject());
        //?ScbEditor* ed = 0;
        //?if ( GetConfig()->GetEditorManager() )
        //?    ed  = GetConfig()->GetEditorManager()->GetBuiltinActiveEditor();
        //if (ed)
        if ( pWindow)
        {
            //?if (pWindow->GetParent() ==  ed)
            //if (wxNOT_FOUND not_eq m_UsableWindows.Index(windowName,false/*casecmp*/))
            if (pWindow->GetName() ==  _T("SCIwindow"))
            {   Attach(pWindow);
                #ifdef LOGGING
                    LOGIT( _T("OnWindowOpen Attached:%p name: %s"),
                            pWindow, pWindow->GetName().GetData() );
                #endif //LOGGING
            }
        }//fi (ed)
    }//fi m_bNote...

    event.Skip();
}//OnWindowOpen
// ----------------------------------------------------------------------------
void cbDragScroll::OnWindowClose(wxEvent& event)
// ----------------------------------------------------------------------------
{
    // wxEVT_DESTROY entry

    wxWindow* pWindow = (wxWindow*)(event.GetEventObject());

    if ( (pWindow) && (m_EditorPtrs.Index(pWindow) != wxNOT_FOUND))
    {   // window is one of ours
        Detach(pWindow);
        #ifdef LOGGING
         LOGIT( _T("OnWindowClose Detached %p"), pWindow);
        #endif //LOGGING
    }
    event.Skip();
}//OnWindowClose
// ----------------------------------------------------------------------------
//      MOUSE DRAG and SCROLL Routines
// ----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(MouseEventsHandler, wxEvtHandler)
    //-Deprecated- EVT_MOUSE_EVENTS( MouseEventsHandler::OnMouseEvent)
    // Using Connect/Disconnect events  and EVT_CREATE/EVT_DESTROY
    // wxWidgets events since split-windows were
    // introduced without providing codeblocks events to plugins.
    // Without CB events, event handlers were being leaked for each split
    // window.
END_EVENT_TABLE()
// ----------------------------------------------------------------------------
MouseEventsHandler::~MouseEventsHandler()
{
    #if defined(LOGGING)
     LOGIT(_T("MouseEventsHandler dtor"));
    #endif
    return;
}//dtor

///////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------------
//      MOUSE SCROLLING for __WXMSW__
// ----------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
#ifdef __WXMSW__
// ----------------------------------------------------------------------------
void MouseEventsHandler::OnMouseEvent(wxMouseEvent& event)    //MSW
// ----------------------------------------------------------------------------
{

    // For efficiency, skip wheel events now
    if ( event.GetEventType() ==  wxEVT_MOUSEWHEEL)
        { event.Skip(); return; }

    // Why is an event getting in here when this window doesnt have the OS focus
    wxWindow* pTopWin = ::wxGetActiveWindow();
    if (pTopWin) pTopWin = ::wxGetTopLevelParent(pTopWin);
        else {event.Skip(); return;}
    if ( (not pTopWin) || (not pTopWin->IsEnabled()) )
        {event.Skip(); return;}

    //remember event window pointer
    wxObject* pEvtObject = event.GetEventObject();
    cbDragScroll* pDS = cbDragScroll::pDragScroll;

    // if "focus follows mouse" enabled, set focus to window
    if (pDS->GetMouseFocusEnabled() )
    {   // use EVT_ENTER_WINDOW instead of EVT_MOTION so that double
        // clicking a search window item allows activating the editor cursor
        // while mouse is still in the search window
        if (event.GetEventType() ==  wxEVT_ENTER_WINDOW)
            if (pEvtObject) ((wxWindow*)pEvtObject)->SetFocus();
    }

    // if StyledTextCtrl, remember for later scrolling
    wxScintilla* pStyledTextCtrl = 0;
    if ( ((wxWindow*)pEvtObject)->GetName() == _T("SCIwindow"))
        pStyledTextCtrl = (wxScintilla*)pEvtObject;

    // set focus to editor window if mouse is in it
    if (event.GetEventType() ==  wxEVT_MOTION)
    {   // use EVT_MOTION here to avoid missing EVT_ENTER_WINDOW.
        // also allows auto activating the editor during long compiles
        if (pDS->GetMouseEditorFocusEnabled() && pStyledTextCtrl )
            ((wxWindow*)pEvtObject)->SetFocus();
    }

    int scrollx;
    int scrolly;

    #if defined(LOGGING)
    //LOGIT(_T("OnMouseEvent"));
    #endif

    if (KeyDown(event))
    {
            m_Direction = pDS->GetMouseDragDirection() ? 1 : -1 ; //v0.14
            m_MouseMoveToLineMoveRatio = pDS->GetMouseToLineRatio()/100.0;
            #ifdef LOGGING
             //LOGIT( _T("m_MouseMoveToLineMoveRatio %f"),m_MouseMoveToLineMoveRatio );
            #endif //LOGGING
            // We tentatively start dragging, but wait for
            // mouse movement before dragging properly.

            m_MouseHasMoved = false;
            //start position will change for each move
            m_StartY = event.GetPosition().y; m_StartX = event.GetPosition().x;
            //remember initial position for entire drag activity
            m_InitY = m_StartY; m_InitX = m_StartX;

            m_DragMode = DRAG_START;
            m_DragStartPos = event.GetPosition();
            #if defined(LOGGING)
             //LOGIT(_T("Down X:%d Y:%d"), m_InitY, m_InitX);
            #endif
            if ( (GetUserDragKey() ==  wxMOUSE_BTN_MIDDLE ) && event.MiddleIsDown() )
                return; //dont allow paste from middle-mouse used as scroll key
            event.Skip(); //v0.21
            return;
    }// if KeyDown

    else if (KeyUp(event) && (m_DragMode != DRAG_NONE) )
    {
        // Finish dragging
        int lastmode = m_DragMode;
        m_DragMode = DRAG_NONE;
        // if our trapped drag, hide event from others, ie. don't event.skip()
        #if defined(LOGGING)
         //LOGIT(_T("Up"));
        #endif
        if (lastmode ==  DRAG_DRAGGING) return;
        // allow context menu processing
        event.Skip();
        return;
    }// if KeyUp

    else if ( event.Dragging() && (m_DragMode != DRAG_NONE ) )
    {
        //make sure user didnt leave client area and lift mouse key
        if ( not KeyIsDown(event))
         {  m_DragMode = DRAG_NONE;
            return;
         }

       //allow user some slop moves in case this is a "moving" context menu request
       if ( ! m_MouseHasMoved
            && abs(event.GetPosition().x - m_InitX) < 3
            && abs(event.GetPosition().y - m_InitY) < 3)
        {  return;}

        //+v0.6 code moved here to allow sloppier context menu requests
        if (m_DragMode == DRAG_START)
         {
            // Start the drag. This will stop the context popup
            #if defined(LOGGING)
              //LOGIT(_T("Drag_Start"));
            #endif
            m_DragMode = DRAG_DRAGGING;
         }

        m_MouseHasMoved = true;
        int dX = event.GetPosition().x - m_StartX;
        int dY = event.GetPosition().y - m_StartY;

        //show some sensitivity to speed of user mouse movements
        m_RatioX = m_RatioY = m_MouseMoveToLineMoveRatio;
        // build up some mouse movements to guarantee ratios won't cancel scrolling
        if ( (abs(dX)*m_RatioX >= 1) || (abs(dY)*m_RatioY >= 1) )
        { m_StartY = event.GetPosition().y; m_StartX = event.GetPosition().x;}

        //add one full mousemove for every x mouse positions beyond start position
        //so scrolling is faster as user is faster
        // slider values 1...2...3...4...5...6...7...8...9...10   //v0.14
        // divisn values 90  80  70  60 50   40  30  20  10   1
        int nThreshold = 1+( 100-(pDS->GetMouseDragSensitivity()*10) );
        m_RatioX += (abs(dX)/nThreshold);
        m_RatioY += (abs(dY)/nThreshold);

        // scroll the client area
        if (abs(dX) > abs(dY))
        {
            scrolly = 0; scrollx = int(dX * m_RatioX);
        }
        else
        {
            scrollx = 0; scrolly = int(dY * m_RatioY);
        }
        #if defined(LOGGING)
        //  LOGIT(_T("RatioX:%f RatioY:%f"), m_RatioX, m_RatioY);
        //  LOGIT(_T("Drag: dX:%d dY:%d scrollx:%d scrolly:%d"), dX, dY, scrollx, scrolly);
        #endif

        // Scroll horizontally and vertically.
        // void LineScroll (int columns, int lines);
        if ((scrollx==0) && (scrolly==0)) return;
        scrollx *= m_Direction; scrolly *= m_Direction;

        // if editor window, use scintilla scroll
        if (pStyledTextCtrl )
        {
                pStyledTextCtrl->LineScroll (scrollx,scrolly);
        }
        else //use wxControl scrolling
        {
            //use wxTextCtrl scroll for y scrolling
            if ( scrolly)
                ((wxWindow*)pEvtObject)->ScrollLines(scrolly);
            else  // use listCtrl for x scrolling
                ((wxListCtrl*)pEvtObject)->ScrollList(scrollx<<2,scrolly);
        }//else
    }//else if ( event.Dragging() && (m_DragMode != DRAG_NONE )

    // pass the event onward
    event.Skip();

}//OnMouseEvent
#endif //__WXMSW__ scroling

///////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------------
//      __WXGTK__ MOUSE SCROLLING __WXGTK__
// ----------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
#if defined(__WXGTK__) || defined(__WXMAC__)
// ----------------------------------------------------------------------------
void MouseEventsHandler::OnMouseEvent(wxMouseEvent& event)    //GTK
// ----------------------------------------------------------------------------
{

    // For efficiency, skip wheel events now
    if ( event.GetEventType() ==  wxEVT_MOUSEWHEEL)
        { event.Skip(); return; }

    // Why is an event getting in here when this window doesnt have the OS focus
    wxWindow* pTopWin = ::wxGetActiveWindow();
    if (pTopWin) pTopWin = ::wxGetTopLevelParent(pTopWin);
    else {event.Skip(); return;}
    if ( (not pTopWin) || (not pTopWin->IsEnabled()) )
        {event.Skip(); return;}

    //remember event window pointer
    wxObject* pEvtObject = event.GetEventObject();
    cbDragScroll* pDS = cbDragScroll::pDragScroll;


    // if "focus follows mouse" enabled, set focus to window
    if (pDS->GetMouseFocusEnabled() )
    {   // use EVT_ENTER_WINDOW instead of EVT_MOTION so that double
        // clicking a search window item allows activating the editor cursor
        // while mouse is still in the search window
        if (event.GetEventType() ==  wxEVT_ENTER_WINDOW)
            if (pEvtObject) ((wxWindow*)pEvtObject)->SetFocus();
    }

    // if StyledTextCtrl, remember for later scrolling
    wxScintilla* pStyledTextCtrl = 0;
    if ( ((wxWindow*)pEvtObject)->GetName() == _T("SCIwindow"))
        pStyledTextCtrl = (wxScintilla*)pEvtObject;

    // set focus to editor window if mouse is in it
    if (event.GetEventType() ==  wxEVT_MOTION)
    {   // use EVT_MOTION here to avoid missing EVT_ENTER_WINDOW.
        // also allows auto activating the editor during long compiles
        if (pDS->GetMouseEditorFocusEnabled() && pStyledTextCtrl )
            ((wxWindow*)pEvtObject)->SetFocus();
    }

    int scrollx;
    int scrolly;

    #if defined(LOGGING)
     //LOGIT(_T("OnMouseEvent"));
    #endif

    //--------- Key Down ------------------------------------------------------
    if (KeyDown(event))
     {
        m_Direction = pDS->GetMouseDragDirection() ? 1 : -1 ; //v0.14
        m_MouseMoveToLineMoveRatio = pDS->GetMouseToLineRatio()/100.0;

        // We tentatively start dragging, but wait for
        // mouse movement before dragging properly.

        m_MouseHasMoved = false;
        //start position will change for each move
        m_StartY = event.GetPosition().y; m_StartX = event.GetPosition().x;
        //remember initial position for entire drag activity
        m_InitY = m_StartY; m_InitX = m_StartX;

        m_DragMode = DRAG_NONE;
        m_DragStartPos = event.GetPosition();
        #if defined(LOGGING)
         LOGIT(_T("Down at  X:%d Y:%d"), m_InitX, m_InitY);
        #endif

        wxPoint mouseXY = ((wxWindow*)pEvtObject)->ScreenToClient(wxGetMousePosition());
        LOGIT(_T("Down MoveTo X:%d Y:%d"), mouseXY.x, mouseXY.y);

        // wait for possible mouse moves before poping context menu
        for (int i = 0; i < pDS->GetMouseContextDelay();)
        {
            ::wxMilliSleep(10);    // wait for move (if any)
            mouseXY = ((wxWindow*)pEvtObject)->ScreenToClient(wxGetMousePosition());
            scrollx = abs(mouseXY.x - m_InitX) ;
            scrolly = abs(mouseXY.y - m_InitY) ;
            //-if ( ( scrolly > 1) || (scrollx > 1) ) break; 2008/02/2
            if ( ( scrolly > 2) || (scrollx > 2) ) break;
            i += 10;
        }

        // capture middle mouse key for immediate dragging
        if ( (GetUserDragKey() ==  wxMOUSE_BTN_MIDDLE ) && event.MiddleIsDown() )
        {   m_DragMode = DRAG_START;
            return;
        }
        else // wait for movement if right mouse key; might be context menu request
        {
            #if defined(LOGGING)
             LOGIT(_T("Down delta x:%d y:%d"), scrollx, scrolly );
            #endif
            if (pStyledTextCtrl && (pEvtObject == pStyledTextCtrl) //v0.21
                && ( ( scrolly > 2) || (scrollx > 2) ))
            {   m_DragMode = DRAG_START;
                return;
            }
            // Since scrolling other types of windows doesnt work on GTK
            // just event.Skip()
            //else {  // listctrl windows ALWAYS report 24 pixel y move
            //        // when just hitting the mouse button.
            //    if ( (scrolly > 24) || (scrollx > 1))
            //    {   m_DragMode = DRAG_START;
            //        return;
            //    }
            //}//endelse
            else {  // listctrl windows ALWAYS report 24 pixel y move
                    // when just hitting the mouse button.
                //-if ( (scrolly > 1) || (scrollx > 1)) 2008/02/2
                if ( (scrolly > 2) || (scrollx > 2))
                {   m_DragMode = DRAG_START;
                    return;
                }
            }//endelse
        }//else wait for movement

        ////        // --------------------------------
        ////        // Dont do the following on Linux, it kills all context menus
        ////        // --------------------------------
        ////        //// If hiding Right mouse keydown from ListCtrls, return v0.22
        ////        //// RightMouseDown is causing an immediate selection in the control
        ////        //// This stops it.
        ////        //-if (pDS->GetMouseRightKeyCtrl()) return; removed
        ////        //-event.Skip(); //v0.21
        ////        //-return;

        //no mouse movements, so pass off to context menu processing
        event.Skip();
        return;
    }//fi (event.RightDown()
    //--------- Key UP -------------------------------------------------------
    else if (KeyUp(event) )
     {
        // Finish dragging
        int lastmode = m_DragMode;
        m_DragMode = DRAG_NONE;
        #if defined(LOGGING)
         LOGIT( _T("Up") ) ;
        #endif
        if (lastmode ==  DRAG_DRAGGING) return;
        // allow non-drag processing
        event.Skip();
        return;
     }//fi (event.RighUp)
    //--------- DRAGGING  -----------------------------------------------------
    else if ( (m_DragMode!=DRAG_NONE) && event.Dragging() ) //v0.12
    {

        //-LOGIT( _T("Dragging") ) ;
        //make sure user didnt leave client area and lift mouse key
        if ( not KeyIsDown(event))
         {  m_DragMode = DRAG_NONE;
            return;
         }

        ////allow user some slop moves in case this is a "moving" context menu request
        //if ( ! m_MouseHasMoved
        //    && abs(event.GetPosition().x - m_InitX) < 3
        //    && abs(event.GetPosition().y - m_InitY) < 3)
        //{  return;}
        //else m_DragMode = DRAG_START;//v0.12

        if (m_DragMode == DRAG_START)
         {
            // Start the drag. This will stop the context popup
            #if defined(LOGGING)
            LOGIT(_T("Drag_Start"));
            #endif
            m_DragMode = DRAG_DRAGGING;
         }

       m_MouseHasMoved = true;
       int dX = event.GetPosition().x - m_StartX;
       int dY = event.GetPosition().y - m_StartY;

      //set ration of mouse moves to lines scrolled (currently 30 percent)
      m_RatioX = m_RatioY = m_MouseMoveToLineMoveRatio;
      // build up some mouse movements to guarantee ratios won't cancel scrolling
      if ( (abs(dX)*m_RatioX >= 1) || (abs(dY)*m_RatioY >= 1) )
       { m_StartY = event.GetPosition().y; m_StartX = event.GetPosition().x;}

      //add one full mousemove for every x mouse positions beyond start position
      //so scrolling is faster as user is faster
      // slider values 1...2...3...4...5...6...7...8...9...10   //v0.14
      // divisn values 90  80  70  60  50  40  30  20  10   1
      int nThreshold = 1+( 100-(pDS->GetMouseDragSensitivity()*10) );
      m_RatioX += (abs(dX)/nThreshold);
      m_RatioY += (abs(dY)/nThreshold);

      // scroll the client area
      if (abs(dX) > abs(dY))
       {
            scrolly = 0; scrollx = int(dX * m_RatioX);
       }
      else
       {
            scrollx = 0; scrolly = int(dY * m_RatioY);
       }
        #if defined(LOGGING)
       //  LOGIT(_T("RatioX:%f RatioY:%f"), m_RatioX, m_RatioY);
       //  LOGIT(_T("Drag: dX:%d dY:%d scrollx:%d scrolly:%d"), dX, dY, scrollx, scrolly);
        #endif

        // Scroll horizontally and vertically.
        // void LineScroll (int columns, int lines);
        if ((scrollx==0) && (scrolly==0)) return;
        scrollx *= m_Direction; scrolly *= m_Direction;


        // if editor window, use scintilla scroll
        if (pStyledTextCtrl )
        {
                pStyledTextCtrl->LineScroll (scrollx,scrolly);
        }
        else //use control scrolling
        {
            // ---------------------------------
            //The following works in the BuildLog, but now the SearchResults
            // ---------------------------------
            //use wxTextCtrl scroll for y scrolling
            if ( scrolly )//&& (classname == wxT("wxTextCtrl")) )
            {   //LOGIT(wxT("ScrollText x:%d y:%d"),scrollx, scrolly );
                ((wxWindow*)pEvtObject)->ScrollLines(scrolly);
            }
            // Following does not work. GTK does not scroll wxListCtrl
            //else  // use listCtrl for x scrolling
            //{    LOGIT(wxT("ScrollList x:%d y:%d"),scrollx, scrolly );
            //    ((wxListCtrl*)m_pEvtObject)->ScrollList(scrollx,scrolly);
            //}
        }//esle
    }//esle fi (event.Dragging() && m_dragMode != DRAG_NONE)

    // pass on the event
    event.Skip();

}//OnMouseEvent
#endif //__WXGTK__ scrolling
// ----------------------------------------------------------------------------
//   end __WXGTK__ scrolling
// ----------------------------------------------------------------------------

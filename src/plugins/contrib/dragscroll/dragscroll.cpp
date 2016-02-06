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
    #include <wx/app.h>
	#include <wx/intl.h>
	#include <wx/listctrl.h>
	#include "configmanager.h"
	#include "manager.h"
	#include "personalitymanager.h"
	#include "sdk_events.h" // EVT_APP_STARTUP_DONE
#endif

#include <wx/fileconf.h> // wxFileConfig
#include <wx/html/htmlwin.h>
#include <wx/tokenzr.h>
#include "cbstyledtextctrl.h"
#include "dragscroll.h"
#include "dragscrollcfg.h"
#include "dragscrollevent.h"
#include "logmanager.h"
#include "loggers.h"
#include "projectmanager.h"
#include "editormanager.h"

#include "startherepage.h"
// remove this line - comment line to test commits at SourceForge
// ----------------------------------------------------------------------------
//  TextCtrlLogger class to allow IsLoggerControl() access to "control" pointer
// ----------------------------------------------------------------------------
class dsTextCtrlLogger : public TextCtrlLogger
// ----------------------------------------------------------------------------
{
    //Helper class to verify logger control
   friend class cbDragScroll;
   public:
    dsTextCtrlLogger(){};
    ~dsTextCtrlLogger(){};
};
// ----------------------------------------------------------------------------
class dsStartHerePage : public StartHerePage
// ----------------------------------------------------------------------------
{
    //Helper class to verify htmlWindow
    friend class cbDragScroll;
    dsStartHerePage(wxEvtHandler* owner, wxWindow* parent);
    ~dsStartHerePage();
};
// ----------------------------------------------------------------------------
// Register the plugin
// ----------------------------------------------------------------------------
namespace
{
    PluginRegistrant<cbDragScroll> reg(_T("cbDragScroll"));
    int ID_DLG_DONE = wxNewId();
};
// ----------------------------------------------------------------------------
//  Events table
// ----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(cbDragScroll, cbPlugin)
	// End Configuration event
    EVT_UPDATE_UI(ID_DLG_DONE, cbDragScroll::OnDoConfigRequests)
    // DragScroll Event types
    EVT_DRAGSCROLL_EVENT( wxID_ANY, cbDragScroll::OnDragScrollEvent_Dispatcher )

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
    m_ZoomWindowIds      = wxEmptyString;
    m_ZoomFontSizes      = wxEmptyString;
    m_ZoomWindowIdsAry.Clear();
    m_ZoomFontSizesAry.Clear();

    m_pCB_AppWindow = Manager::Get()->GetAppWindow();

    #if defined(LOGGING)
        wxWindow* pcbWindow = m_pCB_AppWindow;
        wxLog::EnableLogging(true);
        //wxLogWindow*
            pMyLog = new wxLogWindow(pcbWindow, wxT("DragScroll"), true, false);
        wxLog::SetActiveTarget(pMyLog);
        pMyLog->Flush();
        //pMyLog->GetFrame()->Move(20,20);
        pMyLog->GetFrame()->SetSize(20,20, 600, 300);
        wxLogMessage(_T("Logging cbDragScroll version %s"),wxString(wxT(VERSION)).c_str());
        LOGIT( _T("DragScroll::cbDragScroll Address is:[%p]"), pDragScroll);
	#endif

    if (not m_pMouseEventsHandler )
        m_pMouseEventsHandler = new MouseEventsHandler();

    // names of windows we're allowed to attach
    m_UsableWindows.Add(_T("text"));        // compiler logs
    m_UsableWindows.Add(_T("listctrl"));    // compiler errors
    m_UsableWindows.Add(_T("textctrl"));    // logs
    m_UsableWindows.Add(_T("treectrl"));    // management trees
    m_UsableWindows.Add(_T("treeAll"));
    m_UsableWindows.Add(_T("treeMembers"));
    m_UsableWindows.Add(_T("csTreeCtrl"));  // codesnippets
    m_UsableWindows.Add(_T("sciwindow"));   // editor controls
    m_UsableWindows.Add(_T("htmlwindow"));  // start here page

    MouseDragScrollEnabled  = true;
    MouseEditorFocusEnabled = false;
    MouseFocusEnabled       = false;
    MouseDragDirection      = 0;
    MouseDragKey            = 0;
    MouseDragSensitivity    = 5;
    MouseToLineRatio        = 30;
    MouseContextDelay       = 10;
    MouseWheelZoom          = false;
    PropagateLogZoomSize    = false;
    m_MouseHtmlFontSize     = 0;

    // Create filename like cbDragScroll.ini
    //memorize the key file name as {%HOME%}\cbDragScroll.ini
    m_ConfigFolder = ConfigManager::GetConfigFolder();
    m_DataFolder = ConfigManager::GetDataFolder();
    m_ExecuteFolder = FindAppPath(wxTheApp->argv[0], ::wxGetCwd(), wxEmptyString);

    //GTK GetConfigFolder is returning double "//?, eg, "/home/pecan//.codeblocks"
    // remove the double //s from filename //+v0.4.11
    m_ConfigFolder.Replace(_T("//"),_T("/"));
    m_ExecuteFolder.Replace(_T("//"),_T("/"));

    // get the CodeBlocks "personality" argument
    wxString m_Personality = Manager::Get()->GetPersonalityManager()->GetPersonality();
	if (m_Personality == wxT("default")) m_Personality = wxEmptyString;
	#if defined(LOGGING)
     LOGIT( _T("Personality is[%s]"), m_Personality.GetData() );
	#endif

    // if DragScroll.ini is in the executable folder, use it
    // else use the default config folder
    m_CfgFilenameStr = m_ExecuteFolder + wxFILE_SEP_PATH;
    if (not m_Personality.IsEmpty()) m_CfgFilenameStr << m_Personality + wxT(".") ;
    m_CfgFilenameStr << _T("DragScroll.ini");

    if (::wxFileExists(m_CfgFilenameStr)) {;/*OK Use exe path*/}
    else //use the default.conf folder
    {   m_CfgFilenameStr = m_ConfigFolder + wxFILE_SEP_PATH;
        if (not m_Personality.IsEmpty()) m_CfgFilenameStr << m_Personality + wxT(".") ;
        m_CfgFilenameStr << _T("DragScroll.ini");
    }
    #if defined(LOGGING)
    LOGIT(_T("DragScroll Config Filename:[%s]"), m_CfgFilenameStr.GetData());
    #endif
    // read configuaton file
    wxFileConfig cfgFile(wxEmptyString,     // appname
                        wxEmptyString,      // vendor
                        m_CfgFilenameStr,   // local filename
                        wxEmptyString,      // global file
                        wxCONFIG_USE_LOCAL_FILE);

	cfgFile.Read(_T("MouseDragScrollEnabled"),  &MouseDragScrollEnabled ) ;
	cfgFile.Read(_T("MouseEditorFocusEnabled"), &MouseEditorFocusEnabled ) ;
	cfgFile.Read(_T("MouseFocusEnabled"),       &MouseFocusEnabled ) ;
	cfgFile.Read(_T("MouseDragDirection"),      &MouseDragDirection ) ;
	cfgFile.Read(_T("MouseDragKey"),            &MouseDragKey ) ;
	cfgFile.Read(_T("MouseDragSensitivity"),    &MouseDragSensitivity ) ;
	cfgFile.Read(_T("MouseToLineRatio"),        &MouseToLineRatio ) ;
	cfgFile.Read(_T("MouseContextDelay"),       &MouseContextDelay) ;
	cfgFile.Read(_T("MouseWheelZoom"),          &MouseWheelZoom) ;
	cfgFile.Read(_T("PropagateLogZoomSize"),    &PropagateLogZoomSize) ;
	cfgFile.Read(_T("MouseHtmlFontSize"),       &m_MouseHtmlFontSize, 0) ;
	cfgFile.Read(_T("ZoomWindowIds"),           &m_ZoomWindowIds, wxEmptyString) ;
	cfgFile.Read(_T("ZoomFontSizes"),           &m_ZoomFontSizes, wxEmptyString) ;

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
        LOGIT(_T("MouseContextDelay:%d"),       MouseContextDelay ) ;
        LOGIT(_T("MouseWheelZoom:%d"),          MouseWheelZoom ) ;
        LOGIT(_T("PropagateLogZoomSize:%d"),    PropagateLogZoomSize ) ;
        LOGIT(_T("MouseHtmlFontSize:%d"),       m_MouseHtmlFontSize ) ;
        LOGIT(_T("ZoomWindowIds:[%s]"),         m_ZoomWindowIds.c_str() ) ;
        LOGIT(_T("ZoomFontSizes:[%s]"),         m_ZoomFontSizes.c_str() ) ;
    #endif //LOGGING

    // Fill ZoomWindowIds and ZoomFontSizes arrays from config strings
    // The strings contain last sessions window ids and font sizes
    GetZoomWindowsArraysFrom( m_ZoomWindowIds, m_ZoomFontSizes );

    // Catch creation of windows
    Connect( wxEVT_CREATE,
        (wxObjectEventFunction) (wxEventFunction)
        (wxCommandEventFunction) &cbDragScroll::OnWindowOpen);

    // Catch Destroyed windows
    Connect( wxEVT_DESTROY,
        (wxObjectEventFunction) (wxEventFunction)
        (wxCommandEventFunction) &cbDragScroll::OnWindowClose);

    // Set current plugin version
	PluginInfo* pInfo = (PluginInfo*)(Manager::Get()->GetPluginManager()->GetPluginInfo(this));
	pInfo->version = wxT(VERSION);
	// Allow other plugins to find our Event ID
	m_DragScrollFirstId = wxString::Format( _T("%d"), wxEVT_DRAGSCROLL_EVENT);
	pInfo->authorWebsite = m_DragScrollFirstId;

	#if defined(LOGGING)
	LOGIT( _T("DragScroll EventTypes[%d]"), wxEVT_DRAGSCROLL_EVENT);
	#endif

	// register event sink
    Manager::Get()->RegisterEventSink(cbEVT_APP_STARTUP_DONE, new cbEventFunctor<cbDragScroll, CodeBlocksEvent>(this, &cbDragScroll::OnAppStartupDone));
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_CLOSE, new cbEventFunctor<cbDragScroll, CodeBlocksEvent>(this, &cbDragScroll::OnProjectClose));
    Manager::Get()->RegisterEventSink(cbEVT_APP_START_SHUTDOWN, new cbEventFunctor<cbDragScroll, CodeBlocksEvent>(this, &cbDragScroll::OnStartShutdown));

	return ;
}
// ----------------------------------------------------------------------------
void cbDragScroll::OnRelease(bool /*appShutDown*/)
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
    pDlg->SetMouseContextDelay ( MouseContextDelay );
    pDlg->SetMouseWheelZoom ( MouseWheelZoom );
    pDlg->SetPropagateLogZoomSize ( PropagateLogZoomSize );

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
    MouseContextDelay       = pDlg->GetMouseContextDelay();
    MouseWheelZoom          = pDlg->GetMouseWheelZoom();
    PropagateLogZoomSize    = pDlg->IsLogZoomSizePropagated() and MouseWheelZoom;
    #ifdef LOGGING
     LOGIT(_T("MouseDragScrollEnabled:%d"),  MouseDragScrollEnabled);
     LOGIT(_T("MouseEditorFocusEnabled:%d"), MouseEditorFocusEnabled);
     LOGIT(_T("MouseFocusEnabled:%d"),       MouseFocusEnabled);
     LOGIT(_T("MouseDragDirection:%d"),      MouseDragDirection);
     LOGIT(_T("MouseDragKey:%d"),            MouseDragKey);
     LOGIT(_T("MouseDragSensitivity:%d"),    MouseDragSensitivity);
     LOGIT(_T("MouseToLineRatio:%d"),        MouseToLineRatio);
     LOGIT(_T("MouseContextDelay:%d"),       MouseContextDelay);
     LOGIT(_T("MouseMouseWheelZoom:%d"),     MouseWheelZoom);
     LOGIT(_T("PropagateLogZoomSize:%d"),    PropagateLogZoomSize);
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
void cbDragScroll::OnDoConfigRequests(wxUpdateUIEvent& /*event*/)
// ----------------------------------------------------------------------------
{
    // This is an event triggered by OnDialogDone() to update config settings
    #if defined(LOGGING)
    LOGIT(_T("OnDoConfigRequest event"));
    #endif

    // Attach or Detach windows to match  Mouse Enabled config setting
    if (GetMouseDragScrollEnabled() )  //v04.14
    {   if (not m_bNotebooksAttached)
        {
            AttachRecursively(m_pCB_AppWindow);
            m_bNotebooksAttached = true;
        }
    }//if
    else {
        DetachAll();
        m_bNotebooksAttached = false;
    }//else

    // update/write configuaton file
    UpdateConfigFile();

}//OnDoConfigRequests
// ----------------------------------------------------------------------------
void cbDragScroll::UpdateConfigFile()
// ----------------------------------------------------------------------------
{

    // update/write configuaton file

    #if defined(LOGGING)
    LOGIT(_T("UpdateConfigFile"));
    #endif

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
	cfgFile.Write(_T("MouseContextDelay"),       MouseContextDelay ) ;
	cfgFile.Write(_T("MouseWheelZoom"),          MouseWheelZoom ) ;
	cfgFile.Write(_T("PropagateLogZoomSize"),    PropagateLogZoomSize ) ;
	cfgFile.Write(_T("MouseHtmlFontSize"),       m_MouseHtmlFontSize ) ;

	if ( not m_ZoomWindowIds.IsEmpty() )
	{
        cfgFile.Write(_T("ZoomWindowIds"),       m_ZoomWindowIds ) ;
        cfgFile.Write(_T("ZoomFontSizes"),       m_ZoomFontSizes ) ;
	}

}//UpdateConfigFile
// ----------------------------------------------------------------------------
int cbDragScroll::GetZoomWindowsArraysFrom( wxString zoomWindowIds, wxString zoomFontSizes )
// ----------------------------------------------------------------------------
{
    wxStringTokenizer ids(  zoomWindowIds, wxT(","));
    wxStringTokenizer sizes(zoomFontSizes, wxT(","));
    while ( ids.HasMoreTokens()  && sizes.HasMoreTokens() )
    {
        long winId ; long size;
        ids.GetNextToken().ToLong(&winId);
        sizes.GetNextToken().ToLong(&size);
        m_ZoomWindowIdsAry.Add( winId );
        m_ZoomFontSizesAry.Add( size );
    }

    return m_ZoomWindowIdsAry.GetCount();
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

    #if defined(LOGGING)
    int windowID = event.GetId();
    LOGIT( _T("cbDragScroll::OnDragScrollEvent AddWindow[%d][%p][%s]"), windowID, pWin, pWin->GetName().c_str());
    #endif
}
// ----------------------------------------------------------------------------
void cbDragScroll::OnDragScrollEventRemoveWindow(wxCommandEvent& event )
// ----------------------------------------------------------------------------
{
    // Received a request to remove a window pointer
    // from our array of monitored windows
    wxWindow* pWin = (wxWindow*)event.GetEventObject();
    Detach( pWin );

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
    // ThreadSearch cbStyledTextCtrl preView control
    CleanUpWindowPointerArray();

    // Rescan for scrollable children starting from the window provided
    wxWindow* pWin = (wxWindow*)event.GetEventObject();
    wxString winName = event.GetString();
    if ( (not winName.IsEmpty()) && (wxNOT_FOUND == m_UsableWindows.Index(winName)) )
        m_UsableWindows.Add(winName);
    if (pWin)
        AttachRecursively( pWin );

    #if defined(LOGGING)
    if (pWin)
    LOGIT( _T("cbDragScroll::OnDragScrollEvent Rescan[%p][%s]"), pWin, pWin->GetName().c_str());
    #endif
}
// ----------------------------------------------------------------------------
void cbDragScroll::OnDragScrollEvent_RereadConfig(wxCommandEvent& /*event*/ )
// ----------------------------------------------------------------------------
{
    #if defined(LOGGING)
    LOGIT( _T("CodeSnippets:DragScroll RereadConfig"));
    #endif

    wxString cfgFilenameStr = m_CfgFilenameStr;
    #if defined(LOGGING)
    LOGIT(_T("DragScroll Config Filename:[%s]"), cfgFilenameStr.GetData());
    #endif
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
	cfgFile.Read(_T("MouseContextDelay"),       &MouseContextDelay) ;
	cfgFile.Read(_T("MouseWheelZoom"),          &MouseWheelZoom) ;
	cfgFile.Read(_T("PropagateLogZoomSize"),    &PropagateLogZoomSize) ;
	cfgFile.Read(_T("MouseHtmlFontSize"),       &m_MouseHtmlFontSize, 0 ) ;

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
        LOGIT(_T("MouseContextDelay:%d"),       MouseContextDelay ) ;
        LOGIT(_T("MouseWheelZoom:%d"),          MouseWheelZoom ) ;
        LOGIT(_T("PropagateLogZoomSize:%d"),    PropagateLogZoomSize ) ;
        LOGIT(_T("MouseHtmlFontSize:%d"),       m_MouseHtmlFontSize ) ;
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
void cbDragScroll::OnDragScrollTestRescan(DragScrollEvent& /*event*/ )
// ----------------------------------------------------------------------------
{
    #if defined(LOGGING)
    LOGIT( _T("TESING DragScrollevent"));
    #endif
}
// ----------------------------------------------------------------------------
void cbDragScroll::CleanUpWindowPointerArray()
// ----------------------------------------------------------------------------
{
    unsigned int i = 0;
    while (i < m_WindowPtrs.GetCount() )
    {
    	if ( not winExists((wxWindow*)m_WindowPtrs.Item(i)) )
        {    m_WindowPtrs.RemoveAt(i);
            #if defined(LOGGING)
            //LOGIT( _T("csDragScroll CleanedUp[%d][%p]"), i, m_WindowPtrs.Item(i));
            #endif
        }
        else
            ++i;
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
    if ( wxNOT_FOUND == m_WindowPtrs.Index(p))
        return false;
    #if defined(LOGGING)
    LOGIT( _T("IsAttachedTo previously[%p][%s]"), p, p->GetName().c_str());
    #endif
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

    if (wxNOT_FOUND == m_UsableWindows.Index(windowName,false))
     {
        #if defined(LOGGING)
        LOGIT(wxT("cbDS::Attach skipping [%s]"), p->GetName().c_str());
        #endif
        return;
     }
    #if defined(LOGGING)
    LOGIT(wxT("cbDS::Attach - attaching to [%s][%d][%p]"), p->GetName().c_str(),p->GetId(),p);
    #endif

    // add window to our array, create a mouse event handler
    // and memorize event handler instance
    m_WindowPtrs.Add(p);

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
    p->Connect(wxEVT_MOUSEWHEEL,
                    (wxObjectEventFunction)(wxEventFunction)
                    (wxMouseEventFunction)&cbDragScroll::OnMouseWheelEvent,
                     NULL, this);

    #if defined(LOGGING)
     LOGIT(_T("cbDS:Attach Window:%p Handler:%p"), p,thisEvtHndlr);
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
{
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
{
    if ( !parent )
        return NULL;

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
void cbDragScroll::Detach(wxWindow* pWindow)
// ----------------------------------------------------------------------------
{
    if ( (pWindow) && (m_WindowPtrs.Index(pWindow) != wxNOT_FOUND))
    {
         #if defined(LOGGING)
          LOGIT(_T("cbDS:Detaching %p"), pWindow);
         #endif

        m_WindowPtrs.Remove(pWindow);

        MouseEventsHandler* thisEvtHandler = GetMouseEventsHandler();
        // If win already deleted, dont worry about disconnectng events
	    if ( not winExists(pWindow) )
	    {
            #if defined(LOGGING)
	        LOGIT(_T("cbDS:Detach window NOT found %p Handlr: %p"),
                    pWindow, thisEvtHandler);
            #endif
            return;
	    } else {
            pWindow->Disconnect(wxEVT_MIDDLE_DOWN,
                            (wxObjectEventFunction)(wxEventFunction)
                            (wxMouseEventFunction)&MouseEventsHandler::OnMouseEvent,
                             NULL, thisEvtHandler);
            pWindow->Disconnect(wxEVT_MIDDLE_UP,
                            (wxObjectEventFunction)(wxEventFunction)
                            (wxMouseEventFunction)&MouseEventsHandler::OnMouseEvent,
                             NULL, thisEvtHandler);
            pWindow->Disconnect(wxEVT_RIGHT_DOWN,
                            (wxObjectEventFunction)(wxEventFunction)
                            (wxMouseEventFunction)&MouseEventsHandler::OnMouseEvent,
                             NULL, thisEvtHandler);
            pWindow->Disconnect(wxEVT_RIGHT_UP,
                            (wxObjectEventFunction)(wxEventFunction)
                            (wxMouseEventFunction)&MouseEventsHandler::OnMouseEvent,
                             NULL, thisEvtHandler);
            pWindow->Disconnect(wxEVT_MOTION,
                            (wxObjectEventFunction)(wxEventFunction)
                            (wxMouseEventFunction)&MouseEventsHandler::OnMouseEvent,
                             NULL, thisEvtHandler);
            pWindow->Disconnect(wxEVT_ENTER_WINDOW,
                            (wxObjectEventFunction)(wxEventFunction)
                            (wxMouseEventFunction)&MouseEventsHandler::OnMouseEvent,
                             NULL, thisEvtHandler);
            pWindow->Disconnect(wxEVT_MOUSEWHEEL,
                            (wxObjectEventFunction)(wxEventFunction)
                            (wxMouseEventFunction)&cbDragScroll::OnMouseWheelEvent,
                             NULL, this);
        }//else

        #if defined(LOGGING)
         LOGIT(_T("Detach: Editor:%p EvtHndlr: %p"),pWindow,thisEvtHandler);
        #endif
    }//if (pWindow..
}//Detach
// ----------------------------------------------------------------------------
void cbDragScroll::DetachAll()
// ----------------------------------------------------------------------------
{
	// delete all handlers
	#if defined(LOGGING)
	LOGIT(wxT("cbDS:DetachAll - detaching all [%lu] targets"), static_cast<unsigned long>(m_WindowPtrs.GetCount()) );
	#endif

    // Detach from memorized windows and remove event handlers
    while( m_WindowPtrs.GetCount() )
    {
	    wxWindow* pw = (wxWindow*)m_WindowPtrs.Item(0);
        Detach(pw);
    }//elihw

    m_WindowPtrs.Empty();

    // say no windows attached
    m_bNotebooksAttached = false;
    //-m_pSearchResultsWindow = 0;
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
//    cbDragScroll Routines to push/remove mouse event handlers
// ----------------------------------------------------------------------------
void cbDragScroll::OnAppStartupDone(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    // EVT_APP_STARTUP_DONE
    //attach windows
    #if defined(LOGGING)
    LOGIT(_T("cbDragScroll::AppStartupDone"));
    #endif
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
    if (not GetMouseDragScrollEnabled() )
        return;

    AttachRecursively( m_pCB_AppWindow );
    m_bNotebooksAttached = true;

    // For Linux:
    // OnWindowOpen() misses the first main.cpp open of the StartHere page.
    // So find & issue the users font zoom change here.
    if ( GetMouseWheelZoom() ) do
    {   // Tell mouse handler to initalize the mouseWheel data
        // after the htmlWindow is fully initialized
        const EditorBase* sh = Manager::Get()->GetEditorManager()->GetEditor(_T("Start here"));
        if (not sh) break;
        wxWindow* pWindow = ((dsStartHerePage*)sh)->m_pWin; //htmlWindow
        if (not pWindow) break;
        wxMouseEvent wheelEvt(wxEVT_MOUSEWHEEL);
        wheelEvt.SetEventObject(pWindow);
        wheelEvt.m_controlDown = true;
        wheelEvt.m_wheelRotation = 0;
        wheelEvt.m_wheelDelta = 1; //Avoid FPE wx3.0 //(pecan 2015/08/19)
        #if wxCHECK_VERSION(3, 0, 0)
        pWindow->GetEventHandler()->AddPendingEvent(wheelEvt);
        #else
        pWindow->AddPendingEvent(wheelEvt);
        #endif
    }while(0);

    // Issue SetFont() for saved font sizes on our monitored windows
    // Arrays contain the previous sessions window id and the font size for that window
    if ( GetMouseWheelZoom() )
    for (int i=0; i<(int)m_WindowPtrs.GetCount(); ++i)
    {
        wxWindow* pWindow = (wxWindow*)m_WindowPtrs.Item(i);
        // verify the window still exists (htmWindows disappear without notice)
        if (not winExists(pWindow))
        {
            m_WindowPtrs.RemoveAt(i);
            --i;
            if (i<0) break;
            continue;
        }
        // check for font size change
        #if defined(LOGGING)
        LOGIT( _T("pWindow GetName[%s]"), pWindow->GetName().c_str());
        #endif
        if ( (pWindow->GetName() not_eq  _T("SCIwindow"))
                and (pWindow->GetName() not_eq  _T("htmlWindow")) )
        {
            int windowId = pWindow->GetId();
            int posn;
            int fontSize = 0;
            wxFont font;
            if ( wxNOT_FOUND not_eq (posn = m_ZoomWindowIdsAry.Index( windowId)) )
            {
                fontSize = m_ZoomFontSizesAry.Item(posn);
                font = pWindow->GetFont();
                font.SetPointSize( fontSize );
                pWindow->SetFont( font);
                // Tell mouse handler to refresh new font size
                // after the window is fully initialied
                wxMouseEvent wheelEvt(wxEVT_MOUSEWHEEL);
                wheelEvt.SetEventObject(pWindow);
                wheelEvt.m_controlDown = true;
                wheelEvt.m_wheelRotation = 0;
                wheelEvt.m_wheelDelta = 1; //Avoid FPE wx3.0 //(pecan 2015/08/19)
                #if wxCHECK_VERSION(3, 0, 0)
                pWindow->GetEventHandler()->AddPendingEvent(wheelEvt);
                #else
                pWindow->AddPendingEvent(wheelEvt);
                #endif
                #if defined(LOGGING)
                //LOGIT( _T("OnAppStartupDoneInit Issued Wheel Zoom event 0[%p]size[%d]"),pWindow, fontSize);
                #endif
            }//if
        }//if
    }//for

}
// ----------------------------------------------------------------------------
void cbDragScroll::OnProjectClose(CodeBlocksEvent& /*event*/)
// ----------------------------------------------------------------------------
{
    // Ask DragScoll to clean up any orphaned windows and rescan for
    // (validate) our monitored windows

    // We only want to know if there are no more projects open before
    // cleaning up

    if (Manager::IsAppShuttingDown())
        return;

    ProjectsArray* prjary = Manager::Get()->GetProjectManager()->GetProjects();
    if ( prjary->GetCount() )
        return;

    // Issue a pending event so we rescan after other events have settled down.
    DragScrollEvent dsEvt(wxEVT_DRAGSCROLL_EVENT, idDragScrollRescan);
    dsEvt.SetEventObject( m_pCB_AppWindow);
    dsEvt.SetString( _T("") );
    this->AddPendingEvent(dsEvt);
    return;
}
// ----------------------------------------------------------------------------
void cbDragScroll::OnStartShutdown(CodeBlocksEvent& /*event*/)
// ----------------------------------------------------------------------------
{
    //NOTE: CB is invoking this event TWICE

    // Save DragScroll configuration .ini file

    // Create ini entry with this session Zoom window id's & Zoom font sizes
    // They'll be used next session to re-instate the zoom font sizes

    // Make sure the array is clear of destroyed window pointer
    CleanUpWindowPointerArray();
    wxString zoomWindowIds = _T("");
    wxString zoomFontSizes = _T("");
    if ( GetMouseWheelZoom() )
    {
        for (size_t i=0; i<m_WindowPtrs.GetCount(); ++i )
        {
            #if defined(LOGGING)
            //LOGIT( _T("OnStartShutdown[%d][%p][%d]"), i, m_WindowPtrs.Item(i),((wxWindow*)m_WindowPtrs.Item(i))->GetId());
            #endif
            zoomWindowIds << wxString::Format(_T("%d,"),((wxWindow*)m_WindowPtrs.Item(i))->GetId() );
            wxFont font = ((wxWindow*)m_WindowPtrs.Item(i))->GetFont();
            zoomFontSizes << wxString::Format(_T("%d,"),font.GetPointSize() );
            //#if defined(LOGGING)
            //LOGIT( _T("WindowPtr[%p]Id[%d]fontSize[%d]"),
            //    m_WindowPtrs.Item(i),
            //    ((wxWindow*)m_WindowPtrs.Item(i))->GetId(),
            //    font.GetPointSize()
            //);
            //#endif
        }
        // Remove trailing comma
        zoomWindowIds.Truncate(zoomWindowIds.Length()-1);
        zoomFontSizes.Truncate(zoomFontSizes.Length()-1);
        #if defined(LOGGING)
        //LOGIT( _T("ZoomWindowIds[%s]"), zoomWindowIds.c_str());
        //LOGIT( _T("ZoomFontSizes[%s]"), zoomFontSizes.c_str());
        #endif
    }//if GetMouseWheelZoom

    SetZoomWindowsStrings(zoomWindowIds, zoomFontSizes);

    // Write out any outstanding config data changes
    UpdateConfigFile();
}
// ----------------------------------------------------------------------------
void cbDragScroll::OnWindowOpen(wxEvent& event)
// ----------------------------------------------------------------------------
{
    // wxEVT_CREATE entry

    wxWindow* window = (wxWindow*)(event.GetEventObject());

    // Some code (at times) is not issueing EVT_APP_STARTUP_DONE;
    // so here we do it ourselves. If not initialized and this is the first
    // scintilla window, initialize now.

    if ( (not m_bNotebooksAttached)
        && ( window->GetName().Lower() == wxT("sciwindow")) )
    {
        #if defined(LOGGING)
        LOGIT( _T("OnWindowOpen[%s]"), window->GetName().c_str());
        #endif
        OnAppStartupDoneInit();
    }

    // Attach a window
    if ( m_bNotebooksAttached ) do
    {
        wxWindow* pWindow = (wxWindow*)(event.GetEventObject());
        if ( pWindow )
        {
            //#if defined(LOGGING)
            //LOGIT( _T("OnWindowOpen by[%s]"), pWindow->GetName().GetData());
            //#endif
            if ( (pWindow->GetName() ==  _T("SCIwindow"))
                or (pWindow->GetName() ==  _T("htmlWindow")) )
            {
                // Clean this address from our array of window pointers.
                // Some child windows are deleted by wxWidgets and never get
                // a wxEVT_DESTROY (eg., htmlWindow in StartHerePage).
                Detach(pWindow);

                #ifdef LOGGING
                    LOGIT( _T("OnWindowOpen Attaching:%p name: %s"),
                            pWindow, pWindow->GetName().GetData() );
                #endif //LOGGING

                Attach(pWindow);
            }
        }//fi (ed)
        // For Windows: issueing the StartHerePage font change here
        // avoids the "font pop" redraw seen on Linux.
        if ( pWindow->GetName() ==  _T("htmlWindow"))
        {   if ( GetMouseWheelZoom() )
                {
                    // Tell mouse handler to initalize the font
                    // after the htmlWindow is fully initialied
                    wxMouseEvent wheelEvt(wxEVT_MOUSEWHEEL);
                    wheelEvt.SetEventObject(pWindow);
                    wheelEvt.m_controlDown = true;
                    wheelEvt.m_wheelRotation = 0; //set user font
                    wheelEvt.m_wheelDelta = 1; //Avoid FPE wx3.0 //(pecan 2015/08/19)
                    #if wxCHECK_VERSION(3, 0, 0)
                    pWindow->GetEventHandler()->AddPendingEvent(wheelEvt);
                    #else
                    pWindow->AddPendingEvent(wheelEvt);
                    #endif
                    #if defined(LOGGING)
                    //LOGIT( _T("OnWindowOpen Issued htmlWindow Zoom event"));
                    #endif
                }
            break;
        }

    }while(0);//if

    event.Skip();
}//OnWindowOpen
// ----------------------------------------------------------------------------
void cbDragScroll::OnWindowClose(wxEvent& event)
// ----------------------------------------------------------------------------
{
    // wxEVT_DESTROY entry

    wxWindow* pWindow = (wxWindow*)(event.GetEventObject());
    #if defined(LOGGING)
    //LOGIT( _T("OnWindowClose[%p]"), pWindow);
    #endif
    if ( (pWindow) && (m_WindowPtrs.Index(pWindow) != wxNOT_FOUND))
    {   // window is one of ours
        Detach(pWindow);
        #ifdef LOGGING
         LOGIT( _T("OnWindowClose Detached %p"), pWindow);
        #endif //LOGGING
    }
    event.Skip();
}//OnWindowClose
// ----------------------------------------------------------------------------
void cbDragScroll::OnMouseWheelEvent(wxMouseEvent& event)
// ----------------------------------------------------------------------------
{
    // This routines does a font zoom when the user uses ctrl-wheelmouse
    // on one of our monitored tree, list, or text
    // (non-scintilla) controls.

    cbDragScroll* pDS = cbDragScroll::pDragScroll;
    if (not pDS->GetMouseWheelZoom() )
    {   event.Skip(); return; }

    //remember event window pointer
    wxObject* pEvtObject = event.GetEventObject();
    wxWindow* pEvtWindow = (wxWindow*)pEvtObject;

    // Ctrl-MouseWheel Zoom for non-scintilla windows
    if ( event.GetEventType() ==  wxEVT_MOUSEWHEEL)
    {
        bool mouseCtrlKeyDown = event.ControlDown();
        if (not mouseCtrlKeyDown) {event.Skip(); return;}
        if ( pEvtWindow->GetName() == _T("SCIwindow"))
        {event.Skip(); return; }

        if ( pEvtWindow->GetName() == _T("htmlWindow"))
        {
            if ( not OnMouseWheelInHtmlWindowEvent(event))
                event.Skip();
            return;
        }

        #ifdef LOGGING
        //LOGIT(wxT("OnMouseWheel[%p][%d][%s]"), pEvtWindow, pEvtWindow->GetId(), pEvtWindow->GetName().c_str() );
        #endif

        int nRotation = event.GetWheelRotation();
        wxFont ctrlFont = pEvtWindow->GetFont();

        if ( nRotation > 0)
            ctrlFont.SetPointSize( ctrlFont.GetPointSize()-1);
        if ( nRotation < 0)
            ctrlFont.SetPointSize( ctrlFont.GetPointSize()+1);
        // a rotation of 0 means to refresh (set) the current window font size
        pEvtWindow->SetFont(ctrlFont);

        // if wxListCtrl, issue SetItemFont() because wxWindow->SetFont() won't do it.
        if ( pEvtWindow->IsKindOf(CLASSINFO(wxListCtrl)) )
        {
            wxListCtrl* pListCtrl = (wxListCtrl*)pEvtWindow;
            for (int i=0; i<pListCtrl->GetItemCount(); ++i)
            {
                wxFont font = pListCtrl->GetItemFont(i);
                font.SetPointSize(ctrlFont.GetPointSize());
                pListCtrl->SetItemFont( i, font );
            }//for
            pEvtWindow->Refresh(); //update colume header fonts
            pEvtWindow->Update();
        }//if

        // If Logger, and option "propagate font size to all loggers" is true,
        // update font for all list & text loggers
        if ( IsLogZoomSizePropagated() )
        {   if ( pEvtWindow->IsKindOf(CLASSINFO(wxListCtrl))
                    or pEvtWindow->IsKindOf(CLASSINFO(wxTextCtrl)) )
               if ( IsLoggerControl((wxTextCtrl*)pEvtWindow) )
               {
                    Manager::Get()->GetConfigManager(_T("message_manager"))->Write(_T("/log_font_size"),ctrlFont.GetPointSize() );
                    Manager::Get()->GetLogManager()->NotifyUpdate();
                    // remove this when SetFont/SetItemFont patch accepted in loggers.cpp
                    // Accepted 2008/08/17
                    //-UpdateAllLoggerWindowFonts(ctrlFont.GetPointSize());
               }
        }
        else //update only this particular logger font
        if ( pEvtWindow->IsKindOf(CLASSINFO(wxListCtrl))
                or pEvtWindow->IsKindOf(CLASSINFO(wxTextCtrl)) )
        {
            dsTextCtrlLogger* pLogger = 0;
            if ( (pLogger = IsLoggerControl((wxTextCtrl*)pEvtWindow)) )
            {
                int newSize = ctrlFont.GetPointSize();
                int oldSize = Manager::Get()->GetConfigManager(_T("message_manager"))->ReadInt(_T("/log_font_size"), platform::macosx ? 10 : 8);
                Manager::Get()->GetConfigManager(_T("message_manager"))->Write(_T("/log_font_size"), newSize );
                pLogger->UpdateSettings();
                Manager::Get()->GetConfigManager(_T("message_manager"))->Write(_T("/log_font_size"),oldSize );
            }
        }

    }//if

}//OnMouseWheelEvent
// ----------------------------------------------------------------------------
bool cbDragScroll::OnMouseWheelInHtmlWindowEvent(wxMouseEvent& event)
// ----------------------------------------------------------------------------
{
    // This routines does a font zoom when the user uses ctrl-wheelmouse
    // on one of our monitored html windows

    //-Debugging: to verify we're getting the right window
    //-const EditorBase* sh = Manager::Get()->GetEditorManager()->GetEditor(_T("Start here"));
    //-if (not sh) return false;
    //-dsStartHerePage* psh = (dsStartHerePage*)sh;

    #if defined(LOGGING)
    //LOGIT( _T("OnMouseWheelInHtmlWindowEvent Begin"));
    #endif

    wxHtmlWindow* pEvtWindow = (wxHtmlWindow*)event.GetEventObject();
    if ( pEvtWindow->GetName() not_eq  _T("htmlWindow"))
        return false;

    //-Debugging: to verify we get the right window
    //-if ( psh->m_pWin not_eq (wxHtmlWindow*)pEvtWindow )
    //-    return false;
    //-#if defined(LOGGING)
    //-//LOGIT( _T("Have the StartHerePage[%p]"), psh);
    //-#endif

    int nRotation = event.GetWheelRotation();
    wxFont ctrlFont = pEvtWindow->GetFont();
    if (not m_MouseHtmlFontSize)
        m_MouseHtmlFontSize = ctrlFont.GetPointSize();

    // a WHEEL Rotation of 0 means just set the last users font.
    // It's issued by cbDragScroll::OnWindowOpen() to reset users font
    // when the htmlWindow is re-created.
    if ( nRotation > 0)
        ctrlFont.SetPointSize( --m_MouseHtmlFontSize);
    if ( nRotation < 0)
        ctrlFont.SetPointSize( ++m_MouseHtmlFontSize);
    #if defined(LOGGING)
    //LOGIT( _T("wheel rotation[%d]font[%d]"), nRotation, m_MouseHtmlFontSize);
    #endif

	int sizes[7] = {};
	for (int i = 0; i < 7; ++i)
        sizes[i] = m_MouseHtmlFontSize;
    //-psh->m_pWin->SetFonts(wxEmptyString, wxEmptyString, &sizes[0]); //debug
    pEvtWindow->SetFonts(wxEmptyString, wxEmptyString, &sizes[0]);

    #if defined(LOGGING)
    //LOGIT( _T("OnMouseWheelInHtmlWindowEvent End"));
    #endif
    return true;
}//OnMouseWheelInHtmlWindowEvent
// ----------------------------------------------------------------------------
dsTextCtrlLogger* cbDragScroll::IsLoggerControl(const wxTextCtrl* pControl)
// ----------------------------------------------------------------------------
{
    // Verify that pControl is actually a text or list logger
    dsTextCtrlLogger* pTextLogger;

    LogManager* pLogMgr = Manager::Get()->GetLogManager();
    int nNumLogs = 10; //just a guess
    for (int i=0; i<nNumLogs; ++i)
    {
        LogSlot& logSlot = pLogMgr->Slot(i);
        if (pLogMgr->FindIndex(logSlot.log)== pLogMgr->invalid_log)
            continue;
        pTextLogger = (dsTextCtrlLogger*)logSlot.GetLogger();
        if ( pTextLogger )
            if ( pTextLogger->control == pControl)
                return pTextLogger;
    }//for

    return 0;
}
////// ----------------------------------------------------------------------------
////void cbDragScroll::UpdateAllLoggerWindowFonts(const int pointSize)
////// ----------------------------------------------------------------------------
////{
////    // remove this when SetFont/SetItemFont patch accepted to loggers.h
////    // removed, patch applied 2008/08/17
////
////    dsTextCtrlLogger* pTextLogger;
////
////    LogManager* pLogMgr = Manager::Get()->GetLogManager();
////    int nNumLogs = 10; //just a guess
////    for (int i=0; i<nNumLogs; ++i)
////    {
////        LogSlot& logSlot = pLogMgr->Slot(i);
////        if (pLogMgr->FindIndex(logSlot.log)== pLogMgr->invalid_log)
////            continue;
////        pTextLogger = (dsTextCtrlLogger*)logSlot.GetLogger();
////        if ( (not pTextLogger) or (not pTextLogger->control) )
////            continue;
////
////        #if defined(LOGGING)
////        //LOGIT( _T("Logger Name[%s]"), logSlot.title.c_str());
////        #endif
////        // Only update logs with controls
////        if (wxNOT_FOUND == m_UsableLogs.Index(logSlot.title,false))
////            continue;
////
////        wxFont font;
////        if ( pTextLogger->control->IsKindOf(CLASSINFO(wxTextCtrl)) )
////        {
////            wxTextCtrl* pTextCtrl = pTextLogger->control;
////            font = pTextCtrl->GetFont();
////            font.SetPointSize( pointSize);
////            pTextCtrl->SetFont(font);
////            continue;
////        }
////
////        if ( pTextLogger->control->IsKindOf(CLASSINFO(wxListCtrl)) )
////        {
////            wxListCtrl* pListCtrl = (wxListCtrl*)pTextLogger->control;
////            font = pListCtrl->GetFont();
////            font.SetPointSize(pointSize);
////            pListCtrl->SetFont(font);
////            for (int i=0; i<pListCtrl->GetItemCount(); ++i)
////            {   // update font for each list item
////                font = pListCtrl->GetItemFont(i);
////                font.SetPointSize(pointSize);
////                pListCtrl->SetItemFont( i, font );
////            }//for
////            pListCtrl->Refresh();
////            pListCtrl->Update();
////        }//if
////    }//for
////
////}
// ----------------------------------------------------------------------------
//      MOUSE DRAG and SCROLL Routines
// ----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(MouseEventsHandler, wxEvtHandler)
    //-Deprecated- EVT_MOUSE_EVENTS( MouseEventsHandler::OnMouseEvent)
    // Using Connect/Disconnect events  and EVT_CREATE/EVT_DESTROY
END_EVENT_TABLE()
// ----------------------------------------------------------------------------
MouseEventsHandler::~MouseEventsHandler()
// ----------------------------------------------------------------------------
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
    // Why is an event getting in here when this window doesnt have the OS focus
    wxWindow* pTopWin = ::wxGetActiveWindow();
    if (pTopWin) pTopWin = ::wxGetTopLevelParent(pTopWin);
    else {event.Skip(); return;}
    if ( (not pTopWin) || (not pTopWin->IsEnabled()) )
        {event.Skip(); return;}

    //remember event window pointer
    wxObject* pEvtObject = event.GetEventObject();
    //-wxWindow* pEvtWindow = (wxWindow*)pEvtObject;
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
        if (pStyledTextCtrl )//&& (m_pEvtObject == p_cbStyledTextCtrl))
        {
            if (scrollx >= 0)
                pStyledTextCtrl->LineScroll (scrollx,scrolly);
            else
            {   //work around scintilla colume scroll left bug
                // any LineScroll(-1,y) will jump to colume 0
                int xpixels = 0; int ypixels = 0; int xoffset = 0;
                xoffset = pStyledTextCtrl->GetXOffset();
                pStyledTextCtrl->GetTextExtent(_T("M"),&xpixels, &ypixels);
                xoffset = pStyledTextCtrl->GetXOffset() + (scrollx * xpixels);
                if (xoffset < 0) xoffset = 0;
                pStyledTextCtrl->SetXOffset(xoffset);
            }
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

    int scrollx = 0;
    int scrolly = 0;

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
         //LOGIT(_T("Down at  X:%d Y:%d"), m_InitX, m_InitY);
        #endif

        wxPoint mouseXY = ((wxWindow*)pEvtObject)->ScreenToClient(wxGetMousePosition());
        //LOGIT(_T("Down MoveTo X:%d Y:%d"), mouseXY.x, mouseXY.y);

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
             //LOGIT(_T("Down delta x:%d y:%d"), scrollx, scrolly );
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
         //LOGIT( _T("Up") ) ;
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
            //LOGIT(_T("Drag_Start"));
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
        if (pStyledTextCtrl )//&& (m_pEvtObject == p_cbStyledTextCtrl))
        {
            if (scrollx >= 0)
                pStyledTextCtrl->LineScroll (scrollx,scrolly);
            else
            {   //work around scintilla colume scroll left bug
                // any LineScroll(-1,y) will jump to colume 0
                int xpixels = 0; int ypixels = 0; int xoffset = 0;
                xoffset = pStyledTextCtrl->GetXOffset();
                pStyledTextCtrl->GetTextExtent(_T("M"),&xpixels, &ypixels);
                xoffset = pStyledTextCtrl->GetXOffset() + (scrollx * xpixels);
                if (xoffset < 0) xoffset = 0;
                pStyledTextCtrl->SetXOffset(xoffset);
            }
        }
        else //use wxControl scrolling
        {
            //use wxTextCtrl scroll for y scrolling
            if ( scrolly && pEvtObject->IsKindOf(CLASSINFO(wxTextCtrl)))
            {
                #if defined(LOGGING)
                //LOGIT(wxT("wxTextCtrl scroll[%d]"), scrolly);
                #endif
                ((wxWindow*)pEvtObject)->ScrollLines(scrolly);
            }
            else  // use listCtrl for x scrolling
            if ( scrolly && pEvtObject->IsKindOf(CLASSINFO(wxListCtrl)))
            {
                //NOTE: wxListCtrl->ScrollList(x,y) is not supported under Linux
                // This is here just in case it gets supported by wxWidgets
                #if defined(LOGGING)
                //LOGIT(wxT("wxListCtrl scroll s[%d]y[%d]"), scrollx, scrolly);
                #endif
                ((wxListCtrl*)pEvtObject)->ScrollList(scrollx<<2,scrolly);
            }
        }//else

    }//else if (event.Dragging() && m_dragMode != DRAG_NONE)

    // pass on the event
    event.Skip();

}//OnMouseEvent
#endif //__WXGTK__ scrolling
// ----------------------------------------------------------------------------
//   end __WXGTK__ scrolling
// ----------------------------------------------------------------------------

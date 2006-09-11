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

#include "dragscroll.h"
#include "dragscrollcfg.h"
#include <licenses.h> // defines some common licenses (like the GPL)
#include <wx/utils.h>
#include <wx/event.h>
#include <wx/generic/splitter.h>

// Implement the plugin's hooks
CB_IMPLEMENT_PLUGIN(cbDragScroll, "DragScroll" );

int ID_DLG_DONE = wxNewId();

// ----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(cbDragScroll, cbPlugin)
	// add events here...
    EVT_APP_STARTUP_DONE(cbDragScroll::OnAppStartupDone)
    EVT_UPDATE_UI(ID_DLG_DONE, cbDragScroll::OnDoConfigRequests)
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
	m_PluginInfo.name = _T("DragScroll");
	m_PluginInfo.title = _("DragScroll");
	m_PluginInfo.version = _T("0.28 2006/09/11");
	m_PluginInfo.description = _("Mouse Drag and Scroll\nUsing Right or Middle Mouse Key");
	m_PluginInfo.author = _T("Pecan");
	m_PluginInfo.authorEmail = _T("");
	m_PluginInfo.authorWebsite = _T("");
	m_PluginInfo.thanksTo = _("wxWidgets & Code::Blocks Team");
	m_PluginInfo.license = LICENSE_GPL;
	//m_PluginInfo.hasConfigure = true;

	// anchor to this one and only object
    pDragScroll = this;

}

// ----------------------------------------------------------------------------
cbDragScroll::~cbDragScroll()
// ----------------------------------------------------------------------------
{
	//dtor
}

// ----------------------------------------------------------------------------
void cbDragScroll::OnAttach()
// ----------------------------------------------------------------------------
{
	// do whatever initialization you need for your plugin
	// NOTE: after this function, the inherited member variable
	// m_IsAttached will be TRUE...
	// You should check for it in other functions, because if it
	// is FALSE, it means that the application did *not* "load"
	// (see: does not need) this plugin...

    pMyLog = NULL;
    m_bNotebooksAttached = false;

    wxWindow* pcbWindow = Manager::Get()->GetAppWindow();
    m_pMS_Window = pcbWindow;
    #if LOGGING
        /*wxLogWindow**/ pMyLog = new wxLogWindow(pcbWindow, m_PluginInfo.name, true, false);
        wxLog::SetActiveTarget(pMyLog);
        pMyLog->Flush();
        pMyLog->GetFrame()->Move(20,20);
        wxLogMessage(_T("Logging cbDragScroll version %s"),m_PluginInfo.version.c_str());
	#endif

    // names of windows we're allowed to attach
    m_UsableWindows.Add(_T("text"));
    m_UsableWindows.Add(_T("listctrl"));
    m_UsableWindows.Add(_T("textctrl"));
    m_UsableWindows.Add(_T("treectrl"));
    m_UsableWindows.Add(_T("sciwindow"));

    MouseDragScrollEnabled = true;
    MouseEditorFocusEnabled = true;
    MouseDragDirection      = 0;
    MouseDragKey            = 0;
    MouseDragSensitivity    = 5;
    MouseToLineRatio        = 30;
    MouseRightKeyCtrl       = 0 ;

    // Create filename like cbDragScroll.ini
    //memorize the key file name as {%HOME%}\cbDragScroll.ini
    m_CfgFilenameStr = ConfigManager::GetConfigFolder();

    //GTK GetConfigFolder is returning double "//?, eg, "/home/pecan//.codeblocks"

    // remove the double //s from filename //+v0.4.11
    m_CfgFilenameStr.Replace(_T("//"),_T("/"));
    m_CfgFilenameStr = m_CfgFilenameStr + wxFILE_SEP_PATH + _T("DragScroll.ini");
    LOGIT(_T("DragScroll Config Filename:[%s]"), m_CfgFilenameStr.GetData());
    // read configuaton file
    wxFileConfig cfgFile(wxEmptyString,     // appname
                        wxEmptyString,      // vendor
                        m_CfgFilenameStr,   // local filename
                        wxEmptyString,      // global file
                        wxCONFIG_USE_LOCAL_FILE);

	cfgFile.Read(_T("MouseDragScrollEnabled"),  &MouseDragScrollEnabled ) ;
	cfgFile.Read(_T("MouseEditorFocusEnabled"), &MouseEditorFocusEnabled ) ;
	cfgFile.Read(_T("MouseDragDirection"),      &MouseDragDirection ) ;
	cfgFile.Read(_T("MouseDragKey"),            &MouseDragKey ) ;
	cfgFile.Read(_T("MouseDragSensitivity"),    &MouseDragSensitivity ) ;
	cfgFile.Read(_T("MouseToLineRatio"),        &MouseToLineRatio ) ;
	cfgFile.Read(_T("MouseRightKeyCtrl"),       &MouseRightKeyCtrl) ;

    #ifdef LOGGING
        LOGIT(_T("MouseDragScrollEnabled:%d"),  MouseDragScrollEnabled ) ;
        LOGIT(_T("MouseEditorFocusEnabled:%d"), MouseEditorFocusEnabled ) ;
        LOGIT(_T("MouseDragDirection:%d"),      MouseDragDirection ) ;
        LOGIT(_T("MouseDragKey:%d"),            MouseDragKey ) ;
        LOGIT(_T("MouseDragSensitivity:%d"),    MouseDragSensitivity ) ;
        LOGIT(_T("MouseToLineRatio:%d"),        MouseToLineRatio ) ;
        LOGIT(_T("MouseRightKeyCtrl:%d"),       MouseRightKeyCtrl ) ;
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
	// m_IsAttached will be FALSE...

	// Remove all Mouse event handlers
	DetachAll();
}
// ----------------------------------------------------------------------------
cbConfigurationPanel* cbDragScroll::GetConfigurationPanel(wxWindow* parent)
// ----------------------------------------------------------------------------
{
	//create and display the configuration dialog for your plugin
    if(!m_IsAttached) {	return 0;}
    // Create a configuration dialogue and hand it off to codeblocks

    //cbConfigurationPanel* pDlg = new cbDragScrollCfg(parent, this);
    cbDragScrollCfg* pDlg = new cbDragScrollCfg(parent, this);
    // set current mouse scrolling options
    pDlg->SetMouseDragScrollEnabled ( MouseDragScrollEnabled );
    pDlg->SetMouseEditorFocusEnabled ( MouseEditorFocusEnabled );
    pDlg->SetMouseDragDirection ( MouseDragDirection );
    pDlg->SetMouseDragKey ( MouseDragKey );
    pDlg->SetMouseDragSensitivity ( MouseDragSensitivity );
    pDlg->SetMouseToLineRatio ( MouseToLineRatio );
    pDlg->SetMouseRightKeyCtrl ( MouseRightKeyCtrl );


    // when the configuration panel is closed with OK, OnDialogDone() will be called
    return pDlg;
}
// ----------------------------------------------------------------------------
void cbDragScroll::OnDialogDone(cbDragScrollCfg* pDlg)
// ----------------------------------------------------------------------------
{
    // The configuration panel has run its OnApply() function.
    // So here it's like we were using ShowModal() and it just returned wxID_OK.

    MouseDragScrollEnabled  = pDlg->GetMouseDragScrollEnabled();
    MouseEditorFocusEnabled = pDlg->GetMouseEditorFocusEnabled();
    MouseDragDirection      = pDlg->GetMouseDragDirection();
    MouseDragKey            = pDlg->GetMouseDragKey();
    MouseDragSensitivity    = pDlg->GetMouseDragSensitivity();
    MouseToLineRatio        = pDlg->GetMouseToLineRatio();
    MouseRightKeyCtrl       = pDlg->GetMouseRightKeyCtrl();
    #ifdef LOGGING
     LOGIT(_T("MouseDragScrollEnabled:%d"),  MouseDragScrollEnabled);
     LOGIT(_T("MouseEditorFocusEnabled:%d"), MouseEditorFocusEnabled);
     LOGIT(_T("MouseDragDirection:%d"),      MouseDragDirection);
     LOGIT(_T("MouseDragKey:%d"),            MouseDragKey);
     LOGIT(_T("MouseDragSensitivity:%d"),    MouseDragSensitivity);
     LOGIT(_T("MouseToLineRatio:%d"),        MouseToLineRatio);
     LOGIT(_T("MouseRightKeyCtrl:%d"),       MouseRightKeyCtrl);
     LOGIT(_T("-----------------------------"));
    #endif //LOGGING

    // Post a pending request to later update the configuration requests
    // Doing work here will stall the dlg window on top of the editor
    wxUpdateUIEvent eventdone(ID_DLG_DONE);
    eventdone.SetEventObject(Manager::Get()->GetAppWindow() );
    Manager::Get()->GetAppWindow()->GetEventHandler()->AddPendingEvent(eventdone);

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
            AttachRecursively(Manager::Get()->GetAppWindow());
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
	cfgFile.Write(_T("MouseDragDirection"),      MouseDragDirection ) ;
	cfgFile.Write(_T("MouseDragKey"),            MouseDragKey ) ;
	cfgFile.Write(_T("MouseDragSensitivity"),    MouseDragSensitivity ) ;
	cfgFile.Write(_T("MouseToLineRatio"),        MouseToLineRatio ) ;
	cfgFile.Write(_T("MouseRightKeyCtrl"),       MouseRightKeyCtrl ) ;

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
    if ( (not m_pSearchResultsWindow) && (windowName eq wxT("listctrl")) )
    {   m_pSearchResultsWindow = p;
        #ifdef LOGGING
         LOGIT(wxT("SearchResultsWindow: %p"),p );
        #endif
    }

    if (wxNOT_FOUND == m_UsableWindows.Index(windowName,false))
     {
        LOGIT(wxT("cbDS::Attach skipping [%s]"), p->GetName().c_str());
        return;
     }

    LOGIT(wxT("cbDS::Attach - attaching to [%s] %p"), p->GetName().c_str(),p);

    //add window to our array, create a mouse event handler
    // and memorize event handler instance
    m_EditorPtrs.Add(p);
    MyMouseEvents* thisEvtHndlr = new MyMouseEvents(p);
    m_EventHandlerArray.Add(thisEvtHndlr);

    p->Connect(wxEVT_MIDDLE_DOWN,
                    (wxObjectEventFunction)(wxEventFunction)
                    (wxMouseEventFunction)&MyMouseEvents::OnMouseEvent,
                     NULL, thisEvtHndlr);
    p->Connect(wxEVT_MIDDLE_UP,
                    (wxObjectEventFunction)(wxEventFunction)
                    (wxMouseEventFunction)&MyMouseEvents::OnMouseEvent,
                     NULL, thisEvtHndlr);
    p->Connect(wxEVT_RIGHT_DOWN,
                    (wxObjectEventFunction)(wxEventFunction)
                    (wxMouseEventFunction)&MyMouseEvents::OnMouseEvent,
                     NULL, thisEvtHndlr);
    p->Connect(wxEVT_RIGHT_UP,
                    (wxObjectEventFunction)(wxEventFunction)
                    (wxMouseEventFunction)&MyMouseEvents::OnMouseEvent,
                     NULL, thisEvtHndlr);
    p->Connect(wxEVT_MOTION,
                    (wxObjectEventFunction)(wxEventFunction)
                    (wxMouseEventFunction)&MyMouseEvents::OnMouseEvent,
                     NULL, thisEvtHndlr);
    p->Connect(wxEVT_ENTER_WINDOW,
                    (wxObjectEventFunction)(wxEventFunction)
                    (wxMouseEventFunction)&MyMouseEvents::OnMouseEvent,
                     NULL, thisEvtHndlr);

    #if LOGGING
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
         #if LOGGING
          LOGIT(_T("cbDS:Detaching %p"), thisEditor);
         #endif

        int edIndex = m_EditorPtrs.Index(thisEditor);
        MyMouseEvents* thisEvtHandler = (MyMouseEvents*)m_EventHandlerArray.Item(edIndex);
        m_EditorPtrs.Remove(thisEditor);
        m_EventHandlerArray.Remove(thisEvtHandler);

        // If win already deleted, dont worry about receiving events
	    if ( not winExists(thisEditor) )
	    {
	        LOGIT(_T("cbDS:DetachAll window NOT found %p Handlr: %p"),
                    thisEditor, thisEvtHandler);
	    } else {
            thisEditor->Disconnect(wxEVT_MIDDLE_DOWN,
                            (wxObjectEventFunction)(wxEventFunction)
                            (wxMouseEventFunction)&MyMouseEvents::OnMouseEvent,
                             NULL, thisEvtHandler);
            thisEditor->Disconnect(wxEVT_MIDDLE_UP,
                            (wxObjectEventFunction)(wxEventFunction)
                            (wxMouseEventFunction)&MyMouseEvents::OnMouseEvent,
                             NULL, thisEvtHandler);
            thisEditor->Disconnect(wxEVT_RIGHT_DOWN,
                            (wxObjectEventFunction)(wxEventFunction)
                            (wxMouseEventFunction)&MyMouseEvents::OnMouseEvent,
                             NULL, thisEvtHandler);
            thisEditor->Disconnect(wxEVT_RIGHT_UP,
                            (wxObjectEventFunction)(wxEventFunction)
                            (wxMouseEventFunction)&MyMouseEvents::OnMouseEvent,
                             NULL, thisEvtHandler);
            thisEditor->Disconnect(wxEVT_MOTION,
                            (wxObjectEventFunction)(wxEventFunction)
                            (wxMouseEventFunction)&MyMouseEvents::OnMouseEvent,
                             NULL, thisEvtHandler);
            thisEditor->Disconnect(wxEVT_ENTER_WINDOW,
                            (wxObjectEventFunction)(wxEventFunction)
                            (wxMouseEventFunction)&MyMouseEvents::OnMouseEvent,
                             NULL, thisEvtHandler);
        }//fi (not winExists

        delete(thisEvtHandler);

        #if LOGGING
         LOGIT(_T("Detach: Editor:%p EvtHndlr: %p"),thisEditor,thisEvtHandler);
        #endif
    }//if (thisEditor..
}//Detach
// ----------------------------------------------------------------------------
void cbDragScroll::DetachAll()
// ----------------------------------------------------------------------------
{
	// delete all handlers
	LOGIT(wxT("cbDS:DetachAll - detaching all [%d] targets"),m_EditorPtrs.GetCount() );

    // Detach from memorized windows and remove event handlers
    while( m_EditorPtrs.GetCount() )
    {
	    wxWindow* pw = (wxWindow*)m_EditorPtrs.Item(0);
        Detach(pw);
    }//elihw

    m_EditorPtrs.Empty();

    // Free any leaked event handlers caused by disappearing windows
    // *Danger* There are dynamic events connected to these Event Handlers
    // Could cause segfaults. This shouldnt happen. Fix the bug if it does.
	for (int i=0; i < (int)m_EventHandlerArray.GetCount(); i++)
	{
	    MyMouseEvents* pHdlr = (MyMouseEvents*)m_EventHandlerArray.Item(i);
	    delete pHdlr;
        LOGIT(_T("cbDS:DetachAll - Removing Leaked EventHdl:[%p]"), pHdlr );
	}//rof

    m_EventHandlerArray.Empty();
    // say no windows attached
    m_bNotebooksAttached = false;
    m_pSearchResultsWindow = 0;
    return;

}//DetachAll
// ----------------------------------------------------------------------------
//    cbDragScroll Routines to push/remove mouse event handlers
// ----------------------------------------------------------------------------
void cbDragScroll::OnAppStartupDone(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    //attach windows //+v0.6
    LOGIT(_T("AppStartupDone"));

    if (not GetMouseDragScrollEnabled() )    //v04.14
        return;

    if (! m_bNotebooksAttached)
    {
        AttachRecursively(Manager::Get()->GetAppWindow());
        m_bNotebooksAttached = true;
    }
    event.Skip();
    return;
}//OnAppStartupDone
// ----------------------------------------------------------------------------
void cbDragScroll::OnWindowOpen(wxEvent& event)
// ----------------------------------------------------------------------------
{
    // wxEVT_CREATE entry
    // Have to do this especially for split windows since CodeBlocks does not have
    // events when opening/closing split windows

    // Attach a split window (or any other window)
    if ( m_bNotebooksAttached )
    {
        wxWindow* pWindow = (wxWindow*)(event.GetEventObject());
        cbEditor* ed = 0;
        ed  = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
        if (ed)
        {
            if (pWindow->GetParent() eq ed)
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
BEGIN_EVENT_TABLE(MyMouseEvents, wxEvtHandler)
    //-Deprecated- EVT_MOUSE_EVENTS( MyMouseEvents::OnMouseEvent)
    // Using Connect/Disconnect events  and EVT_CREATE/EVT_DESTROY
    // widgets events since split-windows were
    // introduced without providing codeblocks events to plugins.
    // Witout events, event handlers were being leaked for each split
    // window.
END_EVENT_TABLE()
// ----------------------------------------------------------------------------
MyMouseEvents::~MyMouseEvents()
{
    #if LOGGING
     LOGIT(_T("MyMouseEvents dtor"));
    #endif
    return;
}//dtor

// ----------------------------------------------------------------------------
//      MOUSE SCROLLING for __WXMSW__
// ----------------------------------------------------------------------------
#ifdef __WXMSW__
void MyMouseEvents::OnMouseEvent(wxMouseEvent& event)    //MSW
{

    //remember event window pointer
    m_pEvtObject = event.GetEventObject();
    cbDragScroll* pDS = cbDragScroll::pDragScroll;

    // Why is an event getting in here when this OS window doesnt have the focus
    if (::wxGetActiveWindow() != pDS->m_pMS_Window)
        {event.Skip(); return;}
    // For efficiency, skip wheel events right now
    if ( event.GetEventType() eq wxEVT_MOUSEWHEEL)
        { event.Skip(); return; }

    cbEditor* ed = 0;
    cbStyledTextCtrl* p_cbStyledTextCtrl = 0;
    cbStyledTextCtrl* pLeftSplitWin = 0;
    cbStyledTextCtrl* pRightSplitWin = 0;
    ed  = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
    {   p_cbStyledTextCtrl = ed->GetControl();
        pLeftSplitWin = ed->GetLeftSplitViewControl();
        pRightSplitWin = ed->GetRightSplitViewControl();
    }
    // set focus to any window with mouse
    // if (m_pEvtObject) ((wxWindow*)m_pEvtObject)->SetFocus();

    // set focus to editor window if mouse is in it
    if (event.GetEventType() eq wxEVT_ENTER_WINDOW)
    {
         //LOGIT( _T("EVT_ENTER_WINDOW:%p Styled:%p LeftSplit:%p RightSplit:%p"), m_pEvtObject, p_cbStyledTextCtrl, pLeftSplitWin, pRightSplitWin );
        if (pDS->GetMouseEditorFocusEnabled() )
        {  if (p_cbStyledTextCtrl && (m_pEvtObject eq pLeftSplitWin))
           {    pLeftSplitWin->SetFocus();
                #ifdef LOGGING
                 //LOGIT( _T("OnMouseEvent:SetFocus %p"), pLeftSplitWin );
                #endif //LOGGING
           }
           if (pRightSplitWin && (m_pEvtObject eq pRightSplitWin))
           {    pRightSplitWin->SetFocus();
                #ifdef LOGGING
                 //LOGIT( _T("OnMouseEvent:SetFocus %p"), pRightSplitWin );
                #endif //LOGGING
           }
        }
    }

    int scrollx;
    int scrolly;

    #if LOGGING
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
            #if LOGGING
             //LOGIT(_T("Down X:%d Y:%d"), m_InitY, m_InitX);
            #endif
            // If hiding Right mouse keydown from ListCtrls, return v0.22
            if (pDS->GetMouseRightKeyCtrl()) return;
            //- If Search Results window, hide right mouse click
            //-if (m_pEvtObject eq pDS->m_pSearchResultsWindow)
            //-    return;
            event.Skip(); //v0.21
            return;
     }

    else if (KeyUp(event) && (m_DragMode != DRAG_NONE) )
     {
        // Finish dragging
        int lastmode = m_DragMode;
        m_DragMode = DRAG_NONE;
        // if our trapped drag, hide event from others, ie. don't event.skip()
        #if LOGGING
         //LOGIT(_T("Up"));
        #endif
        if (lastmode ==  DRAG_DRAGGING) return;
        // allow context menu processing
        event.Skip();
        return;
     }

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
            #if LOGGING
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
          scrollx = int(dX * m_RatioX); scrolly = 0;
       }
      else
       {
            scrollx = 0; scrolly = int(dY * m_RatioY);
       }
        #if LOGGING
       //  LOGIT(_T("RatioX:%f RatioY:%f"), m_RatioX, m_RatioY);
       //  LOGIT(_T("Drag: dX:%d dY:%d scrollx:%d scrolly:%d"), dX, dY, scrollx, scrolly);
        #endif

        // Scroll horizontally and vertically.
        // void LineScroll (int columns, int lines);
        if ((scrollx==0) && (scrolly==0)) return;
        scrollx *= m_Direction; scrolly *= m_Direction;

        // if editor window, use scintilla scroll
        if (p_cbStyledTextCtrl && (m_pEvtObject == p_cbStyledTextCtrl))
        {
                p_cbStyledTextCtrl->LineScroll (scrollx,scrolly);
        }
        else //use control scrolling
        {
            //use wxTextCtrl scroll for y scrolling
            if ( scrolly)
                ((wxWindow*)m_pEvtObject)->ScrollLines(scrolly);
            else  // use listCtrl for x scrolling
                ((wxListCtrl*)m_pEvtObject)->ScrollList(scrollx,scrolly);
        }//esle
    }//else if (event.Dragging() && m_dragMode != DRAG_NONE)

    // pass on the event
    event.Skip();

}//OnMouseEvent
#endif //__WXMSW__ scroling

// ----------------------------------------------------------------------------
//      MOUSE SCROLLING __WXGTK__
// ----------------------------------------------------------------------------
#ifdef __WXGTK__
void MyMouseEvents::OnMouseEvent(wxMouseEvent& event)    //GTK
{

    // For efficiency, skip wheel events now
    if ( event.GetEventType() eq wxEVT_MOUSEWHEEL)
        { event.Skip(); return; }

    //remember window pointer
    m_pEvtObject = event.GetEventObject();
    cbDragScroll* pDS = cbDragScroll::pDragScroll;

    #ifdef LOGGING
     //LOGIT( _T("m_MouseMoveToLineMoveRatio %f"),m_MouseMoveToLineMoveRatio );
    #endif //LOGGING

     cbEditor* ed = 0;
     cbStyledTextCtrl* p_cbStyledTextCtrl = 0;
     ed  = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
     if (ed) p_cbStyledTextCtrl = ed->GetControl();

     // set focus to editor window if mouse is in it
    if (event.GetEventType() eq wxEVT_ENTER_WINDOW)
        if (pDS->GetMouseEditorFocusEnabled() )
           if (p_cbStyledTextCtrl && (m_pEvtObject == p_cbStyledTextCtrl))
                p_cbStyledTextCtrl->SetFocus();


    int scrollx;
    int scrolly;

    #if LOGGING
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
        #if LOGGING
         LOGIT(_T("Down at  X:%d Y:%d"), m_InitX, m_InitY);
        #endif

        // 1/8 second wait for any mouse moves
        ::wxMilliSleep(128);

        //allow user some slop moves in case this is a context menu request
        wxPoint mouseXY = ((wxWindow*)m_pEvtObject)->ScreenToClient(wxGetMousePosition());
        LOGIT(_T("Down MoveTo X:%d Y:%d"), mouseXY.x, mouseXY.y);

        scrollx = abs(mouseXY.x - m_InitX) ;
        scrolly = abs(mouseXY.y - m_InitY) ;

        // capture middle mouse key for immediate dragging
        if ( (GetUserDragKey() eq wxMOUSE_BTN_MIDDLE ) && event.MiddleIsDown() )
        {   m_DragMode = DRAG_START;
            return;
        }
        else // wait for movement if right mouse key; might be context menu request
        {
            #if LOGGING
             LOGIT(_T("Down delta x:%d y:%d"), scrollx, scrolly );
            #endif
            if (p_cbStyledTextCtrl && (m_pEvtObject == p_cbStyledTextCtrl) //v0.21
                && ( ( scrolly > 1) || (scrollx > 1) ))
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
        #if LOGGING
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

        LOGIT( _T("Dragging") ) ;
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
            #if LOGGING
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
          scrollx = int(dX * m_RatioX); scrolly = 0;
       }
      else
       {
            scrollx = 0; scrolly = int(dY * m_RatioY);
       }
        #if LOGGING
       //  LOGIT(_T("RatioX:%f RatioY:%f"), m_RatioX, m_RatioY);
       //  LOGIT(_T("Drag: dX:%d dY:%d scrollx:%d scrolly:%d"), dX, dY, scrollx, scrolly);
        #endif

        // Scroll horizontally and vertically.
        // void LineScroll (int columns, int lines);
        if ((scrollx==0) && (scrolly==0)) return;
        scrollx *= m_Direction; scrolly *= m_Direction;


        // if editor window, use scintilla scroll
        if (p_cbStyledTextCtrl && (m_pEvtObject == p_cbStyledTextCtrl))
        {
                p_cbStyledTextCtrl->LineScroll (scrollx,scrolly);
        }
        else //use control scrolling
        {   //NONE of the following works on GTK
            // ---------------------------------
            //use wxTextCtrl scroll for y scrolling
            if ( scrolly)
                ((wxWindow*)m_pEvtObject)->ScrollLines(scrolly);
            else  // use listCtrl for x scrolling
                //LOGIT(wxT("ScrollList x:%d y:%d"),scrollx, scrolly );
                ((wxListCtrl*)m_pEvtObject)->ScrollList(scrollx,scrolly);
        }//esle
    }//esle fi (event.Dragging() && m_dragMode != DRAG_NONE)

    // pass on the event
    event.Skip();

}//OnMouseEvent
#endif //__WXGTK__ scrolling
// ----------------------------------------------------------------------------
//   end __WXGTK__ scrolling
// ----------------------------------------------------------------------------

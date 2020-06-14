/***************************************************************
 * Name:      MouseSap.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    Pecan<>
 * Copyright: (c) Pecan
 * License:   GPL
 **************************************************************/
// This plugin emulates Linux GPM functions within the editors in linux and msWindows.

// This plugin adds fuctions within the editors using the shiftKey, ctrlKey and  middleMouse button.
// If selected text & shift-middleMouse, paste selected text at current cursor position.
// If selected text & shift-middleMouse inside selection, copy to clipboard.
// If selected text & shift-switch editor, copy selection to clipboard.
// If selected text & shift-ctrl-middleMouse, overwrite selection with clipboard data.
// If no selected text & Shift-middleMouse, paste clipboard data at cursor position(like ctrl-v).

#if defined(__GNUG__) && !defined(__APPLE__)
	#pragma implementation "MouseSap.h"
#endif


#include <sdk.h>
#ifndef CB_PRECOMP
    #include <wx/app.h>
	#include <wx/intl.h>
	#include <wx/listctrl.h>
	#include "cbeditor.h"
	#include "configmanager.h"
	#include "editormanager.h"
	#include "manager.h"
	#include "personalitymanager.h"
	#include "sdk_events.h" // EVT_APP_STARTUP_DONE
#endif

#include <wx/fileconf.h> // wxFileConfig
#include <wx/clipbrd.h>
#include <wx/strconv.h>
#include "cbstyledtextctrl.h"
#include "MouseSap.h"
#include "MouseSapCfg.h"

// Register the plugin
// ----------------------------------------------------------------------------
namespace
// ----------------------------------------------------------------------------
{
    PluginRegistrant<MouseSap> reg(_T("MouseSap"));
    int ID_DLG_DONE = wxNewId();
};

int ID_DLG_DONE = wxNewId();

// ----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(MouseSap, cbPlugin)
//
END_EVENT_TABLE()
// ----------------------------------------------------------------------------
//  Statics
// ----------------------------------------------------------------------------
// global used by mouse events to get user configuration settings
// and plugin routine support
MouseSap* MouseSap::pMouseSap;

// ----------------------------------------------------------------------------
MouseSap::MouseSap()
// ----------------------------------------------------------------------------
{
	//ctor
	// anchor to this one and only object
    m_pMyLog            = nullptr;
    m_bEditorsAttached  = false;
    m_pMMSapEvents      = nullptr;
    pMouseSap           = this;
    m_bMouseSapEnabled  = false;
}
// ----------------------------------------------------------------------------
MouseSap::~MouseSap()
// ----------------------------------------------------------------------------
{
	//dtor
}

// ----------------------------------------------------------------------------
void MouseSap::OnAttach()
// ----------------------------------------------------------------------------
{
	// do whatever initialization you need for your plugin
	// NOTE: after this function, the inherited member variable
	// IsAttached() will be TRUE...
	// You should check for it in other functions, because if it
	// is FALSE, it means that the application did *not* "load"
	// (see: does not need) this plugin...

    m_pMyLog = NULL;
    m_pMMSapEvents = 0;

    wxWindow* pcbWindow = Manager::Get()->GetAppWindow();
    m_pMS_Window = pcbWindow;

    #if defined(LOGGING)
        wxLog::EnableLogging(true);
        /*wxLogWindow**/ m_pMyLog = new wxLogWindow(pcbWindow, wxT("MouseSap"), true, false);
        wxLog::SetActiveTarget(m_pMyLog);
        m_pMyLog->Flush();
        m_pMyLog->GetFrame()->Move(20,20);
        wxLogMessage(_T("Logging MouseSap version %s"),wxString(wxT(VERSION)).c_str());
	#endif

    // Set current plugin version
	PluginInfo* pInfo = (PluginInfo*)(Manager::Get()->GetPluginManager()->GetPluginInfo(this));
	pInfo->version = wxT(VERSION);

	// register event sink AppStartupDone
    Manager::Get()->RegisterEventSink(cbEVT_APP_STARTUP_DONE, new cbEventFunctor<MouseSap, CodeBlocksEvent>(this, &MouseSap::OnAppStartupDone));

	return ;
}
// ----------------------------------------------------------------------------
void MouseSap::OnRelease(bool /*appShutDown*/)
// ----------------------------------------------------------------------------
{
	// do de-initialization for your plugin
	// if appShutDown is false, the plugin is unloaded because Code::Blocks is being shut down,
	// which means you must not use any of the SDK Managers
	// NOTE: after this function, the inherited member variable
	// IsAttached() will be FALSE...

	// Remove all Mouse event handlers
	DetachAllWindows();
	#if defined(LOGGING)
	// Reminder: deleting the log crashes CB on exit
	//-delete pMyLog;
	//-m_pMyLog = 0;
    #endif
    // Catch creation of windows
    Disconnect( wxEVT_CREATE,
        (wxObjectEventFunction) (wxEventFunction)
        (wxCommandEventFunction) &MouseSap::OnWindowOpen);

    // Catch Destroyed windows
    Disconnect( wxEVT_DESTROY,
        (wxObjectEventFunction) (wxEventFunction)
        (wxCommandEventFunction) &MouseSap::OnWindowClose);

    if (m_pMMSapEvents)
        delete m_pMMSapEvents;
    m_pMMSapEvents = 0;
    m_bMouseSapEnabled = false;
}
// ----------------------------------------------------------------------------
cbConfigurationPanel* MouseSap::GetConfigurationPanel(wxWindow* parent)
// ----------------------------------------------------------------------------
{
	//create and display the configuration dialog for your plugin
    if(not IsAttached()) {	return 0;}
    // Create a configuration dialogue and hand it off to codeblocks

    m_bPreviousMouseSapEnabled = m_bMouseSapEnabled;

    //cbConfigurationPanel* pDlg = new cbDragScrollCfg(parent, this);
    cbMouseSapCfg* pDlg = new cbMouseSapCfg(parent, this);
    // set current mouse scrolling options
    pDlg->SetMouseSapEnabled( m_bMouseSapEnabled );

    return pDlg;
}
// ----------------------------------------------------------------------------
void MouseSap::OnDialogDone(cbMouseSapCfg* pdlg)
// ----------------------------------------------------------------------------
{
    // The configuration panel has run its OnApply() function.

    m_bMouseSapEnabled  = pdlg->GetMouseSapEnabled();

    #ifdef LOGGING
     LOGIT(_T("MouseSapEnabled:%d"),    m_bMouseDragScrollEnabled);
    #endif //LOGGING

    // Preserve setting across runs
    Manager::Get()->GetConfigManager(_T("mousesap"))->Write(_T("/enabled"), m_bMouseSapEnabled);

    if ( m_bPreviousMouseSapEnabled != m_bMouseSapEnabled)
    {
        if (m_bMouseSapEnabled)
            OnAppStartupDoneInit();
        else
            OnRelease(false);
    }

    // don't delete dlg; Codeblocks should destroy the dialog

}//OnDialogDone
// ----------------------------------------------------------------------------
bool MouseSap::IsAttachedTo(wxWindow* p)
// ----------------------------------------------------------------------------
{
    if ( wxNOT_FOUND == m_EditorPtrs.Index(p))
        return false;
    return true;

}//IsAttachedTo
// ----------------------------------------------------------------------------
void MouseSap::AttachWindow(wxWindow *p)
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
        LOGIT(wxT("MMSap::Attach skipping [%s]"), p->GetName().c_str());
        #endif
        return;
     }

    #if defined(LOGGING)
    LOGIT(wxT("MMSap::Attach - attaching to [%s] %p"), p->GetName().c_str(),p);
    #endif

    //add window to our array, attach a mouse event handler
    m_EditorPtrs.Add(p);
    if ( not m_pMMSapEvents ) m_pMMSapEvents = new MMSapEvents(p);
    MMSapEvents* thisEvtHndlr = m_pMMSapEvents;

    p->Connect(wxEVT_MIDDLE_DOWN,
                    (wxObjectEventFunction)(wxEventFunction)
                    (wxMouseEventFunction)&MMSapEvents::OnMouseEvent,
                     NULL, thisEvtHndlr);
    p->Connect(wxEVT_MIDDLE_UP,
                    (wxObjectEventFunction)(wxEventFunction)
                    (wxMouseEventFunction)&MMSapEvents::OnMouseEvent,
                     NULL, thisEvtHndlr);
    p->Connect(wxEVT_KILL_FOCUS ,
                    (wxObjectEventFunction)(wxEventFunction)
                    (wxFocusEventFunction)&MMSapEvents::OnKillFocusEvent,
                     NULL, thisEvtHndlr);
    #if defined(LOGGING)
     LOGIT(_T("MMSap:Attach Window:%p Handler:%p"), p,thisEvtHndlr);
    #endif
}
// ----------------------------------------------------------------------------
void MouseSap::AttachWindowsRecursively(wxWindow *p)
// ----------------------------------------------------------------------------{
{
 	if (!p)
		return;

	AttachWindow(p);

 	// this is the standard way wxWidgets uses to iterate through children...
	for (wxWindowList::compatibility_iterator node = p->GetChildren().GetFirst();
		node;
		node = node->GetNext())
	{
		// recursively attach each child
		wxWindow *win = (wxWindow *)node->GetData();

		if (win)
			AttachWindowsRecursively(win);
	}
}
// ----------------------------------------------------------------------------
wxWindow* MouseSap::FindWindowRecursively(const wxWindow* parent, const wxWindow* handle)
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
wxWindow* MouseSap::WindowExists(wxWindow *parent)
// ----------------------------------------------------------------------------{
{

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
void MouseSap::DetachWindow(wxWindow* thisEditor)
// ----------------------------------------------------------------------------
{
    if ( (thisEditor) && (m_EditorPtrs.Index(thisEditor) != wxNOT_FOUND))
    {
         #if defined(LOGGING)
          LOGIT(_T("MMSap:Detaching %p"), thisEditor);
         #endif

        MMSapEvents* thisEvtHandler = m_pMMSapEvents;
        m_EditorPtrs.Remove(thisEditor);

        // If win already deleted, dont worry about receiving events
	    if ( not WindowExists(thisEditor) )
	    {
            #if defined(LOGGING)
	        LOGIT(_T("MMSap:DetachAll window NOT found %p"), thisEditor);
            #endif
	    } else {
            thisEditor->Disconnect(wxEVT_MIDDLE_DOWN,
                            (wxObjectEventFunction)(wxEventFunction)
                            (wxMouseEventFunction)&MMSapEvents::OnMouseEvent,
                             NULL, thisEvtHandler);
            thisEditor->Disconnect(wxEVT_MIDDLE_UP,
                            (wxObjectEventFunction)(wxEventFunction)
                            (wxMouseEventFunction)&MMSapEvents::OnMouseEvent,
                             NULL, thisEvtHandler);
            thisEditor->Disconnect(wxEVT_KILL_FOCUS ,
                            (wxObjectEventFunction)(wxEventFunction)
                            (wxFocusEventFunction)&MMSapEvents::OnKillFocusEvent,
                            NULL, thisEvtHandler);

        }//fi (not winExists

        #if defined(LOGGING)
         LOGIT(_T("Detach: Editor:%p EvtHndlr: %p"),thisEditor,thisEvtHandler);
        #endif
    }//if (thisEditor..
}//Detach
// ----------------------------------------------------------------------------
void MouseSap::DetachAllWindows()
// ----------------------------------------------------------------------------
{
	// delete all handlers
    #if defined(LOGGING)
	LOGIT(wxT("MMSap:DetachAll - detaching all [%lu] targets"), static_cast<unsigned long>(m_EditorPtrs.GetCount()) );
    #endif

    // Detach from memorized windows and remove event handlers
    while( m_EditorPtrs.GetCount() )
    {
	    wxWindow* pw = (wxWindow*)m_EditorPtrs.Item(0);
        DetachWindow(pw);
    }//elihw

    m_EditorPtrs.Empty();

    // say no windows attached
    m_bEditorsAttached = false;
    return;

}//DetachAll
// ----------------------------------------------------------------------------
wxString MouseSap::FindAppPath(const wxString& argv0, const wxString& cwd, const wxString& appVariableName)
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
//    MouseSap Routines to push/remove mouse event handlers
// ----------------------------------------------------------------------------
void MouseSap::OnAppStartupDone(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    // EVT_APP_STARTUP_DONE
    //attach windows
    #if defined(LOGGING)
    LOGIT(_T("MouseSap::AppStartupDone"));
    #endif

    OnAppStartupDoneInit();

    event.Skip();
    return;
}//OnAppStartupDone
// ----------------------------------------------------------------------------
void MouseSap::OnAppStartupDoneInit()
// ----------------------------------------------------------------------------
{
    m_bMouseSapEnabled = false;
    m_bMouseSapEnabled = Manager::Get()->GetConfigManager(_T("mousesap"))->ReadBool(_T("enabled"), false);
    if (not m_bMouseSapEnabled)
        return;

    // names of windows we're allowed to attach
    m_UsableWindows.Add(_T("sciwindow"));


    // Catch creation of windows
    Connect( wxEVT_CREATE,
        (wxObjectEventFunction) (wxEventFunction)
        (wxCommandEventFunction) &MouseSap::OnWindowOpen);

    // Catch Destroyed windows
    Connect( wxEVT_DESTROY,
        (wxObjectEventFunction) (wxEventFunction)
        (wxCommandEventFunction) &MouseSap::OnWindowClose);

    if (not m_bEditorsAttached)
    {
        AttachWindowsRecursively(Manager::Get()->GetAppWindow());
        m_bEditorsAttached = true;
    }
}
// ----------------------------------------------------------------------------
void MouseSap::OnWindowOpen(wxEvent& event)
// ----------------------------------------------------------------------------
{
    // wxEVT_CREATE entry
    // Have to do this especially for split windows since CodeBlocks does not have
    // events when opening/closing split windows

    wxWindow* Window = (wxWindow*)(event.GetEventObject());

    // Some code (at times) is not issuing event EVT_APP_STARTUP_DONE
    // so here we do it ourselves. If not initialized and this is the first
    // scintilla window, initialize now.
    if ( (not m_bEditorsAttached)
        && ( Window->GetName().Lower() == wxT("sciwindow")) )
        OnAppStartupDoneInit();

    // Attach a split window (or any other window)
    if ( m_bEditorsAttached )
    {
        wxWindow* pWindow = (wxWindow*)(event.GetEventObject());
        cbEditor* ed = 0;
        ed  = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
        if (ed)
        {
            if (pWindow->GetParent() ==  ed)
            {   AttachWindow(pWindow);
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
void MouseSap::OnWindowClose(wxEvent& event)
// ----------------------------------------------------------------------------
{
    // wxEVT_DESTROY entry

    wxWindow* pWindow = (wxWindow*)(event.GetEventObject());

    if ( (pWindow) && (m_EditorPtrs.Index(pWindow) != wxNOT_FOUND))
    {   // window is one of ours
        DetachWindow(pWindow);
        #ifdef LOGGING
         LOGIT( _T("OnWindowClose Detached %p"), pWindow);
        #endif //LOGGING
    }
    event.Skip();
}//OnWindowClose
////////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------------
//      MOUSE Select and Paste Events
// ----------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(MMSapEvents, wxEvtHandler)
    //-Deprecated- EVT_MOUSE_EVENTS( MMSapEvents::OnMouseEvent)
    // Using Connect/Disconnect events  and EVT_CREATE/EVT_DESTROY
    // wxWidgets events since split-windows were
    // introduced without providing codeblocks events to plugins.
    // Without CB events, event handlers were being leaked for each split
    // window.
END_EVENT_TABLE()
// ----------------------------------------------------------------------------
MMSapEvents::~MMSapEvents()
// ----------------------------------------------------------------------------
{
    #if defined(LOGGING)
     LOGIT(_T("MMSapEvents dtor"));
    #endif
    return;
}//dtor

// ----------------------------------------------------------------------------
void MMSapEvents::OnMouseEvent(wxMouseEvent& event)    //MSW
// ----------------------------------------------------------------------------
{
    // For windows, user must enable MiddleMousePaste thru editor configuration
    #if defined(__WXMSW__)
    if (not Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/enable_middle_mouse_paste"), false))
        {event.Skip(); return;}
    #endif

    if (not MouseSap::pMouseSap->m_bMouseSapEnabled)
        {event.Skip(); return; }

    if (not ::wxGetKeyState(WXK_SHIFT)) //2020/06/7
        {event.Skip(); return;}

    //remember event window pointer
    //-wxObject* pEvtObject = event.GetEventObject();
    int eventType = event.GetEventType();

    #if defined(__WXMSW__)
    // Why is an event getting in here when this window doesnt have the OS focus
    MouseSap* pMMSap = MouseSap::pMouseSap;
    if (::wxGetActiveWindow() != pMMSap->m_pMS_Window)
        {event.Skip(); return;}
    #endif

    // differentiate window, left, right split window
    cbEditor* ed = 0;
    cbStyledTextCtrl* pControl = 0;
    /*cbStyledTextCtrl* pLeftSplitWin = 0;
    cbStyledTextCtrl* pRightSplitWin = 0;*/

    ed  = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if ( not ed ) { event.Skip(); return; }

    pControl = ed->GetControl();
    // editor must have the current focus
    if ( pControl not_eq wxWindow::FindFocus()  )
        { event.Skip(); return; }

    /*pLeftSplitWin = ed->GetLeftSplitViewControl();
    pRightSplitWin = ed->GetRightSplitViewControl();*/

    #if defined(LOGGING)
    //LOGIT(_T("OnMouseSap[%d]"), eventType);
    #endif

    if ( eventType == wxEVT_MIDDLE_DOWN)
    {
        // shift key must be down
        if (not ::wxGetKeyState(WXK_SHIFT)) //2020/06/7
            {event.Skip(); return;}

        OnMiddleMouseDown( event, pControl );
        return;
    }// if KeyDown

    else if ( eventType == wxEVT_MIDDLE_UP )
    {   // We have to return so we can preserve the selection on gtk
        return;
    }// if KeyUp

    // pass the event onward
    event.Skip();

}//OnMouseEvent
// ----------------------------------------------------------------------------
void MMSapEvents::OnMiddleMouseDown(wxMouseEvent& event, cbStyledTextCtrl* ed)
// ----------------------------------------------------------------------------
{
    // If selected text, C
    // If no selected text, & shift-middleMouse, paste clipboard data at cursor position
    // If selected text, & shift-middleMouse inside selection, copy to clipboard
    // If sselected-text & shift-ctrl-middleMouse, paste text over selection

    // MiddleMouse only, paste selected data at cursor position
    // Shift-MiddleMouse, if no selection, paste clipboard data at cursor.
    // Shift-MiddleMouse, if inside selection, copy to clipboard
    // ctrl-shift-middleMouse replace selected text with clipboard.


    int pos = ed->PositionFromPoint(wxPoint(event.GetX(), event.GetY()));

    if(pos == wxSCI_INVALID_POSITION)
        return;

    int start = ed->GetSelectionStart();
    int end = ed->GetSelectionEnd();

    const wxString selectedText = ed->GetSelectedText();

    bool shiftKeyState = ::wxGetKeyState(WXK_SHIFT);
    bool ctrlKeyState = ::wxGetKeyState(WXK_CONTROL);

    // If no current selection, and shift key is down, use paste from the clipboard
    if ( (0 == selectedText.Length()) and shiftKeyState and (not ctrlKeyState) )
    {
        PasteFromClipboard( event, ed, shiftKeyState, ctrlKeyState );
        return;
    }

    // if selected text and ctrl-shift-middleMouse, overwrite selection from clipboard
    if (selectedText.Length() and shiftKeyState and ctrlKeyState)
    {
        PasteFromClipboard( event, ed, shiftKeyState, ctrlKeyState );
        return;
    }
    //if selected text and shift-middleMouse inside the selection, copy to clipboard
    if ( (shiftKeyState and (not ctrlKeyState)) && (pos >= start) && (pos <= end) && (start != end))
    {
        if (wxTheClipboard->Open())
        {
            wxTheClipboard->AddData(new wxTextDataObject(selectedText));
            wxTheClipboard->Close();
        }
        return;
    }//if

    if(pos < ed->GetCurrentPos())
    {
        start += selectedText.length();
        end += selectedText.length();
    }

    #if defined(LOGGING)
    //LOGIT( _T("pos,start,end[%d][%d][%d]"), pos, start, end);
    #endif

    // paste selected text at current cursor position
    ed->InsertText(pos, selectedText);
    //-SetSelection(start, end);
    ed->GotoPos(pos);
    ed->SetSelectionVoid(pos, pos+selectedText.length());
    #if defined(LOGGING)
    LOGIT( _T("OnMiddleMouseDown[%s]"), selectedText.c_str());
    #endif

} // end of OnGPM
// ----------------------------------------------------------------------------
void MMSapEvents::PasteFromClipboard( wxMouseEvent& event, cbStyledTextCtrl* ed, bool shiftKeyState, bool ctrlKeyState )
// ----------------------------------------------------------------------------
{
    // Set the current position to the mouse click point
    // if ctrl-shift-middleMouse then replace selection with the clipboard, if any.
    // if shift-middleMouse, paste from the clipboard

    // Info: Selection/Copy/Paste
    // PRIMARY selection is typically used by e.g. terminals when selecting text
    //         and pasting it by pressing middle mouse button.
    //         As in selected text is in Primary Clipboard without any explicit
    //         copy action taking place. Quick-Copy is a good name for it.
    //         (Not limited to terminal emulators, but as an example.)
    // CLIPBOARD
    //        is primarily used in connection with MS Windows-style clipboard operations.
    //        Select+Copy. The data resides in the buffer.

    #if defined(LOGGING)
    LOGIT( _T("MMSapEvents pasting from Clipboard"));
    #endif

    int pos = ed->PositionFromPoint(wxPoint(event.GetX(), event.GetY()));
    if(pos == wxSCI_INVALID_POSITION)
        return;

    int start = ed->GetSelectionStart();
    int end = ed->GetSelectionEnd();

    wxTextDataObject data;
    bool gotData = false;
    if (wxTheClipboard->Open())
    {
        wxTheClipboard->UsePrimarySelection(true);
        gotData = wxTheClipboard->GetData(data); //try Primary user text selection
        wxTheClipboard->UsePrimarySelection(false);
    if ( (not gotData) or (shiftKeyState and ctrlKeyState)) //try for clipboard
            gotData = wxTheClipboard->GetData(data); //try non-primary
        wxTheClipboard->Close();
    }
    if (gotData)
    {
        wxString text = data.GetText() ;
        //if shiftstate & ctrlState
        if (  shiftKeyState and ctrlKeyState
                &&  ((pos >= start) && (pos <= end) ) )
        {
            //-ed->Paste(); paste over selection
            ed->SetTargetStart(start);
            ed->SetTargetEnd(end);
            ed->ReplaceTarget(text);

        }
        else if(shiftKeyState and (not ctrlKeyState))
        {
            ed->InsertText(pos, text);
            ed->SetSelectionVoid(pos, pos + text.Length());
        }
    }

}
// ----------------------------------------------------------------------------
void MMSapEvents::OnKillFocusEvent( wxFocusEvent& event )
// ----------------------------------------------------------------------------
{
    //For GTK, we copy the selected text to the PRIMARY clipboard
    // when we lose the focus

//    #if not defined(__WXGTK__)
//        event.Skip(); return;
//    #endif

    if (not platform::windows)
        { event.Skip(); return; }
    if (not MouseSap::pMouseSap->m_bMouseSapEnabled)
        { event.Skip(); return; }

    if (not ::wxGetKeyState(WXK_SHIFT))
        {event.Skip(); return;}

    // If selected text, copy to clipboard

    //remember event window pointer
    wxObject* pEvtObject = event.GetEventObject();

    cbStyledTextCtrl* pControl = 0;
    cbEditor* ed = 0;
    wxString selectedText = wxEmptyString;
    if ( ((wxWindow*)pEvtObject)->GetName().Lower() == _T("sciwindow") )
    {   ed = (cbEditor*)((wxWindow*)pEvtObject)->GetParent();
        pControl = (cbStyledTextCtrl*)pEvtObject;
    }

    if ( ed ) do
    {
        if ( not MouseSap::pMouseSap->IsAttachedTo( pControl ) )
            break;

        selectedText = pControl->GetSelectedText();
        if ( selectedText.IsEmpty() )
            break;

        #if defined(__WXGTK__)
            //gtk_clipboard_set_text(
            //    gtk_clipboard_get(GDK_SELECTION_PRIMARY),
            //    selectedText.mb_str(wxConvUTF8),
            //    selectedText.Length() );
            wxTheClipboard->UsePrimarySelection(true);
            if (wxTheClipboard->Open())
            {
                wxTheClipboard->AddData(new wxTextDataObject(selectedText));
                wxTheClipboard->Close();
            }
        #else //__WXMSW__ //testing
                if (wxTheClipboard->Open())
                {
                    wxTheClipboard->AddData(new wxTextDataObject(selectedText));
                    wxTheClipboard->Close();
                }
        #endif

        #if defined(LOGGING)
            DumpClipboard();
            if (ed && pControl && (not selectedText.IsEmpty()))
            LOGIT( _T("OnKillFocus[%s][%s]"), ed->GetTitle().c_str(),selectedText.c_str());
        #endif
    }while(false);

    event.Skip();

} // end of OnGPM
// ----------------------------------------------------------------------------
void MMSapEvents::DumpClipboard()
// ----------------------------------------------------------------------------
{
 #if defined(LOGGING)
    wxTextDataObject primaryData;
    wxTextDataObject normalData;
    bool gotPrimaryData = false;
    bool gotNormalData = false;
    wxString primaryText = wxEmptyString;
    wxString normalText = wxEmptyString;

    if (wxTheClipboard->Open())
    {
        wxTheClipboard->UsePrimarySelection(true);
        gotPrimaryData = wxTheClipboard->GetData(primaryData);
        wxTheClipboard->UsePrimarySelection(false);
        gotNormalData = wxTheClipboard->GetData(normalData);
        wxTheClipboard->Close();
    }
    else {
        LOGIT(wxT("DumpClipboard: failed to open"));
        return;
    }

    if (gotPrimaryData)
        primaryText = primaryData.GetText() ;
    if (gotNormalData)
        normalText = normalData.GetText() ;
    LOGIT(wxT("Clipboard Primary[%s]"), primaryText.c_str() );
    LOGIT(wxT("Clipboard Normal[%s]"),  normalText.c_str() );
  #endif //defined(LOGGING)
}//DumpClipboard
///////////////////////////////////////////////////////////////////////////////////
//  The way Scintilla does GPM
///////////////////////////////////////////////////////////////////////////////////
////#ifdef __WXGTK__
////void ScintillaWX::DoMiddleButtonUp(Point pt) {
////    // Set the current position to the mouse click point and
////    // then paste in the PRIMARY selection, if any.  wxGTK only.
////    int newPos = PositionFromLocation(pt);
////    MovePositionTo(newPos, noSel, true);
////
////    pdoc->BeginUndoAction();
////    wxTextDataObject data;
////    bool gotData = false;
////    if (wxTheClipboard->Open()) {
////        wxTheClipboard->UsePrimarySelection(true);
////        gotData = wxTheClipboard->GetData(data);
////        wxTheClipboard->UsePrimarySelection(false);
////        wxTheClipboard->Close();
////    }
////    if (gotData) {
////        wxString   text = wxTextBuffer::Translate(data.GetText(),
////                                                  wxConvertEOLMode(pdoc->eolMode));
////        wxWX2MBbuf buf = (wxWX2MBbuf)wx2stc(text);
////        int        len = strlen(buf);
////        pdoc->InsertString(currentPos, buf, len);
////        SetEmptySelection(currentPos + len);
////    }
////    pdoc->EndUndoAction();
////    NotifyChange();
////    Redraw();
////
////    ShowCaretAtCurrentPosition();
////    EnsureCaretVisible();
////}
////#else
////void ScintillaWX::DoMiddleButtonUp(Point WXUNUSED(pt)) {
////}

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
// RCS-ID: $Id$

#include "editsnippetframe.h"

#include <wx/filename.h>
#include <wx/fileconf.h>
#include <wx/xrc/xmlres.h>


#include "codesnippetswindow.h"
#include "snippetsconfig.h"
#include "GenericMessageBox.h"
#include "version.h"

#include "sdk.h"
#include "sdk_events.h"
#include "configmanager.h"
#include "cbstyledtextctrl.h"
//-#include "wxscintilla/include/wx/wxscintilla.h"  //svn5785
#include <wx/wxscintilla.h>                         //svn5785
#include "cbauibook.h"

#include "defsext.h"
#include "prefs.h"
#include "seditorcolourset.h"
#include "scbeditor.h"
#include "seditormanager.h"
#include "editproperties.h"
// ----------------------------------------------------------------------------
//  resources
// ----------------------------------------------------------------------------
//int idEditHighlightMode = XRCID("idEditHighlightMode");
extern int idEditHighlightMode;
//int idEditHighlightModeText = XRCID("idEditHighlightModeText");
extern int idEditHighlightModeText;
// ----------------------------------------------------------------------------
BEGIN_EVENT_TABLE (EditSnippetFrame, wxFrame)

    EVT_ACTIVATE(                    EditSnippetFrame::OnFrameActivated)
    // common
    EVT_CLOSE (                      EditSnippetFrame::OnCloseFrameOrWindow)
    //-- EVT_CLOSE works, but the next three EVTs never get called
    EVT_SET_FOCUS  (                 EditSnippetFrame::OnFocusWindow)
    EVT_KILL_FOCUS (                 EditSnippetFrame::OnKillFocusWindow)
    EVT_LEAVE_WINDOW (               EditSnippetFrame::OnLeaveWindow)

    // file
    EVT_MENU (wxID_OPEN,             EditSnippetFrame::OnFileOpen)
    EVT_MENU (wxID_SAVE,             EditSnippetFrame::OnFileSave)
    EVT_MENU (wxID_SAVEAS,           EditSnippetFrame::OnFileSaveAs)
    EVT_MENU (wxID_CLOSE,            EditSnippetFrame::OnMenuFileClose)

    // properties
    EVT_MENU (myID_PROPERTIES,       EditSnippetFrame::OnProperties)

    // print and exit
    EVT_MENU (wxID_PRINT,            EditSnippetFrame::OnFilePrint)
    EVT_MENU (wxID_EXIT,             EditSnippetFrame::OnExit)

    /// edit
    EVT_MENU (wxID_CLEAR,            EditSnippetFrame::OnEditEvent)
    EVT_MENU (wxID_CUT,              EditSnippetFrame::OnEditEvent)
    EVT_MENU (wxID_COPY,             EditSnippetFrame::OnEditEvent)
    EVT_MENU (wxID_PASTE,            EditSnippetFrame::OnEditEvent)
    EVT_MENU (myID_INDENTINC,        EditSnippetFrame::OnEditEvent)
    EVT_MENU (myID_INDENTRED,        EditSnippetFrame::OnEditEvent)
    EVT_MENU (wxID_SELECTALL,        EditSnippetFrame::OnEditEvent)
    EVT_MENU (wxID_REDO,             EditSnippetFrame::OnEditEvent)
    EVT_MENU (wxID_UNDO,             EditSnippetFrame::OnEditEvent)

    /// find
    EVT_MENU        (wxID_FIND,      EditSnippetFrame::OnEditEvent)
    EVT_UPDATE_UI   (wxID_FIND,      EditSnippetFrame::OnEditEventUI)
    EVT_MENU        (myID_FINDNEXT,  EditSnippetFrame::OnEditEvent)
    EVT_UPDATE_UI   (myID_FINDNEXT,  EditSnippetFrame::OnEditEventUI)
    EVT_MENU        (myID_FINDPREV,  EditSnippetFrame::OnEditEvent)
    EVT_UPDATE_UI   (myID_FINDPREV,  EditSnippetFrame::OnEditEventUI)
    // replace
    EVT_MENU        (myID_REPLACE,   EditSnippetFrame::OnEditEvent)
    EVT_UPDATE_UI   (myID_REPLACE,   EditSnippetFrame::OnEditEventUI)
    EVT_MENU        (myID_REPLACENEXT,EditSnippetFrame::OnEditEvent)
    EVT_UPDATE_UI   (myID_REPLACENEXT,EditSnippetFrame::OnEditEventUI)

    EVT_MENU        (myID_BRACEMATCH, EditSnippetFrame::OnEditEvent)
    EVT_UPDATE_UI   (myID_BRACEMATCH, EditSnippetFrame::OnEditEventUI)

    EVT_MENU (myID_GOTO,             EditSnippetFrame::OnEditEvent)
    EVT_UPDATE_UI (myID_GOTO,        EditSnippetFrame::OnEditEventUI)

    /// view
    //-EVT_MENU_RANGE (myID_HILIGHTFIRST, myID_HILIGHTLAST,
    EVT_MENU (idEditHighlightMode,     EditSnippetFrame::OnEditHighlightMode)
    //note: the next two statement don't work. OnEditEventUI never gets called.
    // for these events. Had to add UI call to myID_INDENTGUIDE
    EVT_UPDATE_UI(idEditHighlightMode, EditSnippetFrame::OnEditEventUI)
    EVT_UPDATE_UI(idEditHighlightModeText, EditSnippetFrame::OnEditEventUI)

    EVT_MENU (myID_DISPLAYEOL,       EditSnippetFrame::OnEditEvent)
    EVT_UPDATE_UI(myID_DISPLAYEOL,   EditSnippetFrame::OnEditEventUI)
    EVT_MENU (myID_INDENTGUIDE,      EditSnippetFrame::OnEditEvent)
    EVT_MENU (myID_LONGLINEON,       EditSnippetFrame::OnEditEvent)
    EVT_MENU (myID_WHITESPACE,       EditSnippetFrame::OnEditEvent)
    EVT_MENU (myID_FOLDTOGGLE,       EditSnippetFrame::OnEditEvent)
    EVT_MENU (myID_OVERTYPE,         EditSnippetFrame::OnEditEvent)
    EVT_MENU (myID_READONLY,         EditSnippetFrame::OnEditEvent)
    EVT_MENU (myID_WRAPMODEON,       EditSnippetFrame::OnEditEvent)
    // extra
    EVT_MENU (myID_CHANGELOWER,      EditSnippetFrame::OnEditEvent)
    EVT_MENU (myID_CHANGEUPPER,      EditSnippetFrame::OnEditEvent)
    EVT_MENU (myID_CONVERTCR,        EditSnippetFrame::OnEditEvent)
    EVT_MENU (myID_CONVERTCRLF,      EditSnippetFrame::OnEditEvent)
    EVT_MENU (myID_CONVERTLF,        EditSnippetFrame::OnEditEvent)
    EVT_MENU (myID_CHARSETANSI,      EditSnippetFrame::OnEditEvent)
    EVT_MENU (myID_CHARSETMAC,       EditSnippetFrame::OnEditEvent)
    // help
    EVT_MENU (wxID_ABOUT,            EditSnippetFrame::OnAbout)

    EVT_EDITOR_SAVE(               EditSnippetFrame::On_cbEditorSaveEvent)

END_EVENT_TABLE ()
// ----------------------------------------------------------------------------
class EditFrameDropTextTarget : public wxTextDropTarget
// ----------------------------------------------------------------------------
{
    // Drop target used to place dragged data into Properties dialog

	public:
		EditFrameDropTextTarget(EditSnippetFrame* window) : m_Window(window) {}
		~EditFrameDropTextTarget() {}
		bool OnDropText(wxCoord x, wxCoord y, const wxString& data);
	private:
		EditSnippetFrame* m_Window;
};
// ----------------------------------------------------------------------------
bool EditFrameDropTextTarget::OnDropText(wxCoord /*x*/, wxCoord /*y*/, const wxString& data)
// ----------------------------------------------------------------------------
{
    // Put dragged text into TextCtrl
    #ifdef LOGGING
     LOGIT( _T("Dragged Data[%s]"), data.GetData() );
    #endif //LOGGING
    //m_Window->m_pEdit->WriteText(data);
    //    m_Window->m_pEdit->AddText(data);
    m_Window->m_pScbEditor->GetControl()->AddText(data);
    return true;

} // end of OnDropText
// ----------------------------------------------------------------------------
class EditFrameDropFileTarget : public wxFileDropTarget
// ----------------------------------------------------------------------------
{
    // Drop target used to place dragged data into Properties dialog

	public:
		EditFrameDropFileTarget(EditSnippetFrame* window) : m_Window(window) {}
		~EditFrameDropFileTarget() {}
    bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);
	private:
		EditSnippetFrame* m_Window;
};
// ----------------------------------------------------------------------------
bool EditFrameDropFileTarget::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& arrayData)
// ----------------------------------------------------------------------------
{
    //NOTE: putting breakpoints in this routine will hang CB
    // From a console do "gdb cbEdit.exe" instead

    if ( arrayData.GetCount())
        m_Window->OpenDroppedFiles(arrayData);
    return true;

} // end of OnDropText

// ----------------------------------------------------------------------------
//  EditSnippetFrame
// ----------------------------------------------------------------------------
EditSnippetFrame::EditSnippetFrame(const wxTreeItemId  TreeItemId, int* pRetcode )
// ----------------------------------------------------------------------------
{
    // If EditorsStayOnTop options, force stay-on-top editors for full-screen programmers
    long windowStyle = wxDEFAULT_FRAME_STYLE;
    if ( GetConfig()->GetEditorsStayOnTop() )
        windowStyle = windowStyle | wxFRAME_FLOAT_ON_PARENT;

    Create( GetConfig()->GetSnippetsWindow(), wxID_ANY, _T("Edit snippet"),
		wxDefaultPosition, wxDefaultSize, windowStyle );

    InitEditSnippetFrame(TreeItemId, pRetcode );
    m_bEditorSaveEvent = 0;

}
// ----------------------------------------------------------------------------
void EditSnippetFrame::InitEditSnippetFrame(const wxTreeItemId  TreeItemId, int* pRetcode )
// ----------------------------------------------------------------------------
{
    //ctor

    m_bOnActivateBusy = 0;
    m_ActiveEventId = 0;
    m_OncloseWindowEntries = 0;
    m_TmpFileName = wxEmptyString;

    // Create EditorManager for this frame and following calls
    m_pEditorManager = new SEditorManager(this);

    // Get the snippet text associated with this tree id
    m_SnippetItemId = TreeItemId;
    m_EditSnippetText = GetConfig()->GetSnippetsTreeCtrl()->GetSnippet(TreeItemId);

    // Determine wheither this is just text or a filename
    m_EditFileName = m_EditSnippetText.BeforeFirst('\r');
    m_EditFileName = m_EditFileName.BeforeFirst('\n');
    //-#if defined(BUILDING_PLUGIN)
    static const wxString delim(_T("$%["));
    if( m_EditFileName.find_first_of(delim) != wxString::npos )
        Manager::Get()->GetMacrosManager()->ReplaceMacros(m_EditFileName);
    //-#endif

    if ( (m_EditFileName.Length() < 129) && (::wxFileExists(m_EditFileName)) )
        /*OK we're editing a physical file, not just text*/;
    else m_EditFileName = wxEmptyString;

    // Snippet label becomes frame title
    m_EditSnippetLabel = GetConfig()->GetSnippetsTreeCtrl()->GetSnippetLabel(TreeItemId);
    *pRetcode = 0;
    m_nReturnCode = wxID_CANCEL;

    // intitialize important variables
    m_pScbEditor = NULL;

    // set icon and background
    SetTitle(m_EditSnippetLabel);
    SetIcon(GetConfig()->GetSnipImages()->GetSnipListIcon(TREE_IMAGE_ALL_SNIPPETS));
    m_SysWinBkgdColour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW); //(pecan 2007/3/27)
    SetBackgroundColour( m_SysWinBkgdColour); //(pecan 2007/3/27)

    // create menu
    m_menuBar = new wxMenuBar;
    CreateMenu ();

    // open first page. this == wxFrame
    if (not m_EditFileName.IsEmpty() )
    {    m_pScbEditor = GetEditorManager()->Open(m_EditFileName);
    }
    else
    {
        // Need temp file for snippet text
        m_TmpFileName = wxFileName::GetTempDir();
        m_TmpFileName << _T("/")<< m_EditSnippetLabel << _T(".txt");
        m_pScbEditor = GetEditorManager()->New( m_TmpFileName );
        m_pScbEditor->GetControl()->SetText(m_EditSnippetText);
        // SetText() marked the file as modified
        m_pScbEditor->SetModified(false);
        // reset the undo history to avoid undoing to a blank page
        m_pScbEditor->GetControl()->EmptyUndoBuffer();
        //-m_pScbEditor->Activate();
    }

	// Load the window's size
    wxFileConfig cfgFile(wxEmptyString,     // appname
                        wxEmptyString,      // vendor
                        GetConfig()->SettingsSnippetsCfgPath,      // local filename
                        wxEmptyString,      // global file
                        wxCONFIG_USE_LOCAL_FILE);

    cfgFile.Read( wxT("EditDlgXpos"),       &GetConfig()->nEditDlgXpos,20);
    cfgFile.Read( wxT("EditDlgYpos"),       &GetConfig()->nEditDlgYpos,20);
	cfgFile.Read( wxT("EditDlgWidth"),      &GetConfig()->nEditDlgWidth, 500 ) ;
	cfgFile.Read( wxT("EditDlgHeight"),     &GetConfig()->nEditDlgHeight, 400 ) ;
	cfgFile.Read( wxT("EditDlgMaximized"),  &GetConfig()->bEditDlgMaximized, false );
	//SetSize(GetConfig()->nEditDlgWidth, GetConfig()->nEditDlgHeight);
	#if defined(LOGGING)
    LOGIT( _T("EditDlgPosition IN X[%d]Y[%d]Width[%d]Height[%d]"),
        GetConfig()->nEditDlgXpos,GetConfig()->nEditDlgYpos,
        GetConfig()->nEditDlgWidth, GetConfig()->nEditDlgHeight );
	#endif
    SetSize(GetConfig()->nEditDlgXpos, GetConfig()->nEditDlgYpos, GetConfig()->nEditDlgWidth, GetConfig()->nEditDlgHeight);

    m_pScbEditor->Connect(wxEVT_LEAVE_WINDOW,
                    (wxObjectEventFunction)(wxEventFunction)
                    (wxMouseEventFunction)&EditSnippetFrame::OnLeaveWindow,
                     NULL, this);
    // This event will only occur if the parent (SnippetsTreeCtrl) has
    // prior focus and an item is active(ie, an item is selected).
    // The OS focus's the window, but the event does not happen.
    // A side effect of this is a missing caret/cursor.
    m_pScbEditor->Connect(wxEVT_SET_FOCUS,
                    (wxObjectEventFunction)(wxEventFunction)
                    (wxFocusEventFunction)&EditSnippetFrame::OnFocusWindow,
                     NULL, this);
    m_pScbEditor->Connect(wxEVT_KILL_FOCUS,
                    (wxObjectEventFunction)(wxEventFunction)
                    (wxFocusEventFunction)&EditSnippetFrame::OnKillFocusWindow,
                     NULL, this);

    // wxEVT_DESTROY won't work for us because the window is already deleted and
    // therefore doesn't get to save any file changes.
////    m_pScbEditor->Connect( wxEVT_DESTROY,
////                    (wxObjectEventFunction) (wxEventFunction)
////                    (wxCommandEventFunction) &EditSnippetFrame::OnWindowDestroy,
////                    NULL, this);

    //EVT_AUINOTEBOOK_PAGE_CLOSE(ID_NBEditorManager, SEditorManager::OnPageClose)
    Connect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE,
                (wxObjectEventFunction)(wxEventFunction)
                (wxAuiNotebookEventFunction)&EditSnippetFrame::OnPageClose,
                NULL, this);

    Connect(cbEVT_EDITOR_SAVE,
                (wxObjectEventFunction)(wxEventFunction)
                (CodeBlocksEventFunction)&EditSnippetFrame::OncbEditorSave,
                NULL, this);

    SetDropTarget(new EditFrameDropFileTarget(this));
	m_pScbEditor->SetFocus();

}
// ----------------------------------------------------------------------------
EditSnippetFrame::~EditSnippetFrame()
// ----------------------------------------------------------------------------
{
    //dtor

    //-if (m_pScbEditor) m_pScbEditor->Close();
    if (m_pEditorManager)
    {
        // Better close all open editors before deleting EditorManager
        // or wxAuiNotebook will crash since it's deleted before
        // the EditorManager close calls.
        if ( m_pEditorManager )
        {   ScbEditor* ed;
            int knt = m_pEditorManager->GetEditorsCount();
            for ( int i=knt; i>0; --i )
            {
                ed = (ScbEditor*)m_pEditorManager->GetEditor(i-1);
                if ( ed ) ed->Close();
            }
        }
        RemoveEventHandler(m_pEditorManager);
        delete m_pEditorManager;
    }
    // Remove any editor temporary file
    if (not m_TmpFileName.IsEmpty())
    {
        ::wxRemoveFile( m_TmpFileName );
        m_TmpFileName = wxEmptyString;
    }
}
// ----------------------------------------------------------------------------
void EditSnippetFrame::SaveSnippetFramePosn()
// ----------------------------------------------------------------------------
{
    // Called from close routines or OnClose

    wxString CfgFilenameStr = GetConfig()->SettingsSnippetsCfgPath;
    wxFileConfig cfgFile(wxEmptyString,     // appname
                        wxEmptyString,      // vendor
                        CfgFilenameStr,     // local filename
                        wxEmptyString,      // global file
                        wxCONFIG_USE_LOCAL_FILE);
    // save last location of the closing editor
    int x,y,w,h;
    GetPosition(&x,&y); GetSize(&w,&h);
    cfgFile.Write( wxT("EditDlgXpos"),  x );
    cfgFile.Write( wxT("EditDlgYpos"),  y );
    cfgFile.Write( wxT("EditDlgWidth"),  w );
    cfgFile.Write( wxT("EditDlgHeight"), h );
    #if defined(LOGGING)
     LOGIT( _T("EditDlgPositin OUT X[%d]Y[%d]Width[%d]Height[%d]"),x,y,w,h );
    #endif
    cfgFile.Write( wxT("EditDlgMaximized"),  false );
    cfgFile.Flush();
}
// ----------------------------------------------------------------------------
wxString EditSnippetFrame::GetName()
// ----------------------------------------------------------------------------
{
	return m_EditSnippetLabel;
}

// ----------------------------------------------------------------------------
wxString EditSnippetFrame::GetText()
// ----------------------------------------------------------------------------
{
    // data was saved by file close
	return m_EditSnippetText;
}

////// ----------------------------------------------------------------------------
////void EditSnippetFrame::OnOK(wxCommandEvent& event)
////// ----------------------------------------------------------------------------
////{
////    // This routine not called, because EditSnippetFrame is no longer a dialog
////    SaveSnippetFramePosn(wxID_OK);
////	//-EndModal(wxID_OK);
////}
////
////// ----------------------------------------------------------------------------
////void EditSnippetFrame::OnCancel(wxCommandEvent& event)
////// ----------------------------------------------------------------------------
////{
////    // This routine not called, because EditSnippetFrame is no longer a dialog
////    SaveSnippetFramePosn(wxID_CANCEL);
////	//-EndModal(wxID_CANCEL);
////}
// ----------------------------------------------------------------------------
void EditSnippetFrame::OnHelp(wxCommandEvent& /*event*/)
// ----------------------------------------------------------------------------
{
	// Link to the Wiki which contains information about the available macros
	//FIXME: wxLaunchDefaultBrowser(_T("http://wiki.codeblocks.org/index.php?title=Builtin_variables"));
}

// ----------------------------------------------------------------------------
// common event handlers
// ----------------------------------------------------------------------------
void EditSnippetFrame::OnFocusWindow (wxFocusEvent &event)
// ----------------------------------------------------------------------------
{
    // This event will only occur if the parent (SnippetsTreeCtrl) has
    // prior focus and is active(ie, an item is selected).
    // The OS focus's the window, but the event does not happen.
    // A side effect of this is a missing caret/cursor.
    #if defined(LOGGING)
    LOGIT( _T("EditSnippetFrame::OnFocusWindow"));
    #endif
    if ( GetConfig()->IsExternalWindow() )
    { event.Skip(); return; }

    // must be floating or docked snippets
    //MakeModal(true); <= using wxFRAME_FLOAT_ON_PARENT works much better
    //Raise(); //<=doesn't seem to do anything here
    event.Skip();
}
// ----------------------------------------------------------------------------
void EditSnippetFrame::OnKillFocusWindow (wxFocusEvent &event)
// ----------------------------------------------------------------------------
{
    #if defined(LOGGING)
    LOGIT( _T("EditSnippetFrame::OnKillFocusWindow"));
    #endif
    //MakeModal(false);<= using wxFRAME_FLOAT_ON_PARENT works much better
    event.Skip();
}
// ----------------------------------------------------------------------------
void EditSnippetFrame::OnLeaveWindow (wxMouseEvent &event)
// ----------------------------------------------------------------------------
{
    #if defined(LOGGING)
    //LOGIT( _T("EditSnippetFrame::OnLeaveWindow"));
    #endif
    //MakeModal(false);<= using wxFRAME_FLOAT_ON_PARENT works much better
    event.Skip();
}

// ----------------------------------------------------------------------------
void EditSnippetFrame::OnAbout (wxCommandEvent &WXUNUSED(event))
// ----------------------------------------------------------------------------
{
    //-AppAbout(this);
}

// ----------------------------------------------------------------------------
void EditSnippetFrame::OnExit (wxCommandEvent &WXUNUSED(event))
// ----------------------------------------------------------------------------
{
    GetEditorManager()->CloseAll();
    Close(true);
}

// ----------------------------------------------------------------------------
// file event handlers
// ----------------------------------------------------------------------------
void EditSnippetFrame::OnFileOpen (wxCommandEvent &WXUNUSED(event))
// ----------------------------------------------------------------------------
{
    // Open from the frame Menu
    if (! GetEditorManager() ) return;
    wxString fname;
    wxFileDialog dlg (this, _T("Open file"), _T(""), _T(""), _T("Any file (*)|*"),
                      wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR);
    if (dlg.ShowModal() != wxID_OK) return;
    fname = dlg.GetPath ();
    ////FileOpen (fname);
    GetEditorManager()->Open( fname );
}
// ----------------------------------------------------------------------------
void EditSnippetFrame::On_cbEditorSaveEvent (CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    // This event can be entered multiple times
    // Set a guard to avoid a loop
    ++m_bEditorSaveEvent;
    if (m_bEditorSaveEvent > 1) return;
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, wxID_SAVE);
    OnFileSave(evt);
    m_bEditorSaveEvent = 0;
    event.Skip();
}
// ----------------------------------------------------------------------------
void EditSnippetFrame::OnFileSave (wxCommandEvent& WXUNUSED(event))
// ----------------------------------------------------------------------------
{

    if ( not GetEditorManager()) return;

    if ( not m_EditFileName.IsEmpty())
    {   //physical file
        GetEditorManager()->SaveActive();
    }
    else if (GetEditorManager()->GetActiveEditor() == m_pScbEditor)
    {   // XML data to save
        m_EditSnippetText = m_pScbEditor->GetControl()->GetText();
        // we just transfered the data, set wxID_OK.
        m_nReturnCode = wxID_OK;
        m_pScbEditor->SetModified(false);
        GetConfig()->GetSnippetsTreeCtrl()->SaveEditorsXmlData(this);
    }
}
// ----------------------------------------------------------------------------
void EditSnippetFrame::OnFileSaveAs (wxCommandEvent &WXUNUSED(event))
// ----------------------------------------------------------------------------
{
    if ( not GetEditorManager() ) return;

    m_ActiveEventId = wxID_SAVEAS;
    GetEditorManager()->SaveActiveAs();
    m_ActiveEventId = 0;
}
// ----------------------------------------------------------------------------
void EditSnippetFrame::OncbEditorSave( CodeBlocksEvent& event )
// ----------------------------------------------------------------------------
{
    // called before actually saving the data. Used here to save
    // snippet XML data that is never written to a physical file.

    event.Skip();
    // Don't save XML data on SaveAs() calls
    if (m_ActiveEventId == wxID_SAVEAS) return;

    SEditorBase* eb = GetEditorManager()->GetActiveEditor();
    if (m_pScbEditor && (eb == m_pScbEditor) )
    {   // Save local XML data (snippet text)
        //wxCommandEvent cmdevt;
        //cmdevt.SetEventObject(m_pScbEditor);
        //OnFileClose(cmdevt);
        OnFileCheckModified();
    }
}
// ----------------------------------------------------------------------------
void EditSnippetFrame::OnPageClose( wxAuiNotebookEvent event )
// ----------------------------------------------------------------------------
{
    #if defined(LOGGING)
    LOGIT( _T("EditSnippetFrame::OnPageClose"));
    #endif

    //Connect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE,
    //            (wxObjectEventFunction)(wxEventFunction)
    //            wxAuiNotebookEventHandler(EditSnippetFrame::OnPageClose),
    //            NULL, this);

    // This event on a notebook tab close context menu
    // or window closed from title banner

    event.Skip();
    cbAuiNotebook* pnb = (cbAuiNotebook*)event.GetEventObject();
    SEditorBase* eb = static_cast<SEditorBase*>(pnb->GetPage(event.GetSelection()));
    if (m_pScbEditor && (eb == m_pScbEditor) )
    {   // Save local XML data (snippet text)
        //wxCommandEvent cmdevt;
        //cmdevt.SetEventObject(m_pScbEditor);
        //OnFileClose(cmdevt);
        OnFileCheckModified();
        m_pScbEditor = 0;
    }

    // if this is the last editor tab, send close window event
    if ( 1 >= GetEditorManager()->GetEditorsCount() )
    {   // Close() causes a loop with OnWindowClose()
        // This allows the last window to close first.
        wxCloseEvent closeEvent( wxEVT_CLOSE_WINDOW, this->GetId() );
        closeEvent.SetEventObject( this );
        AddPendingEvent(closeEvent);
    }
}
// ----------------------------------------------------------------------------
void EditSnippetFrame::OnCloseFrameOrWindow(wxCloseEvent &event)
// ----------------------------------------------------------------------------
{
    // window will be destroyed by CodeSnippetTreeCtrl::OnIdle()
    // after it copies the snippet text and gets return code
    #if defined(LOGGING)
    LOGIT( _T("EditSnippetFrame::OnCloseFrameOrWindow"));
    #endif

    // Set guard! Loop can occur here from OnPageClose call!
    if (m_OncloseWindowEntries++)
        return;

////    int knt = m_pEditorManager->GetEditorsCount();
////    //if (not knt)
////    if (knt)
////    {
////        Show(false);
////        // Ask SnippetsTreeCtrl to Save and Destroy the editor frame
////    }

    SaveSnippetFramePosn();
    GetConfig()->GetSnippetsTreeCtrl()->SaveDataAndCloseEditorFrame(this);

////    else //destroy any frame with no editors
////        if ( event.GetEventObject() == this)
////        {   // frame is closing
////            #if defined(LOGGING)
////            LOGIT( _T("Frame is closing"));
////            #endif
////            SEditorBase* eb = GetEditorManager()->GetActiveEditor();
////            if (eb)
////                GetEditorManager()->CloseAll();
////            SaveSnippetFramePosn();
////            // Allow all events to clear before frame destroy
////            Manager::Yield();
////            Destroy();
////            return;
////        }

    m_OncloseWindowEntries = ((m_OncloseWindowEntries<1)? 0 : m_OncloseWindowEntries - 1);

}
//// ----------------------------------------------------------------------------
//void EditSnippetFrame::OnCloseWindow (wxCloseEvent &event)
//// ----------------------------------------------------------------------------
//{
//    // wxEVT_DESTROY
//    // This won't work for us because we dont get to save changed text
//    // before the window is deleted.
//    #if defined(LOGGING)
//    LOGIT( _T("EditSnippetFrame::OnCloseWindow"));
//    #endif
//}
// ----------------------------------------------------------------------------
void EditSnippetFrame::OnMenuFileClose (wxCommandEvent &/*event*/)
// ----------------------------------------------------------------------------
{
    if ( GetEditorManager()->GetEditorsCount()) //sanity or crash
    {
        SEditorBase* eb = GetEditorManager()->GetActiveEditor();
        GetEditorManager()->Close(eb);
        if ( eb == m_pScbEditor)
            m_pScbEditor = 0;

    }//if

    // if this is the last editor tab, send close window event
    if ( 0 >= GetEditorManager()->GetEditorsCount() )
    {   // Close() cause a loop with OnWindowClose()
        // This allows the last window to close first.
        wxCloseEvent closeEvent( wxEVT_CLOSE_WINDOW, this->GetId() );
        closeEvent.SetEventObject( this );
        AddPendingEvent(closeEvent);
    }//if

}
// ----------------------------------------------------------------------------
void EditSnippetFrame::OnFileCheckModified()
// ----------------------------------------------------------------------------
{
    if (not m_pScbEditor) return;
    SEditorBase* eb = GetEditorManager()->GetActiveEditor();
    if (not eb) return;
    if ( eb not_eq m_pScbEditor ) return;

    // if text in external file, save if needed
    if (m_pScbEditor->GetModified() )
    {
        // We don't need to ask anymore since cbEditor already has.
        //if (wxMessageBox (_("Text is not saved, save before closing?"), _("Close"),
        //if (messageBox (_("Text is not saved, save before closing?"), _("Close"),
        //                  wxYES_NO | wxICON_QUESTION) == wxYES) {
            // save a real file
            if ( not m_EditFileName.IsEmpty() )
            {   // Physical file
                // cbEditor will save the file on return
                //m_pScbEditor->Save();
                //if (m_pScbEditor->GetModified())
                //{
                //    messageBox (_("Text could not be saved!"), _("Close abort"),
                //                  wxOK | wxICON_EXCLAMATION);
                //    m_nReturnCode = wxID_CANCEL;
                //    return;
                //}
                //m_nReturnCode = wxID_OK;
            }
            else  // else save XML data
            {   // Snippet XML data
                m_EditSnippetText = m_pScbEditor->GetControl()->GetText();
                m_nReturnCode = wxID_OK;
                //-m_pScbEditor->GetControl()->ClearAll();
                m_pScbEditor->SetModified(false);
                m_pScbEditor->GetControl()->SetSavePoint();
            }//snippet
        //-}//if save
    }//if modified

}//OnFileCheckModified

// ----------------------------------------------------------------------------
// properties event handlers
// ----------------------------------------------------------------------------
void EditSnippetFrame::OnProperties (wxCommandEvent& /*event*/)
// ----------------------------------------------------------------------------
{
    //?if (!m_pEdit) return;
    //if (!m_pScbEditor) return;
    //??EditProperties (m_pEdit, 0); Edit.h

    //-GetEditorManager()->OnProperties( event );
    ScbEditor* ed = GetEditorManager()->GetBuiltinActiveEditor();
    EditProperties* pDlg = new EditProperties(this, ed);
    delete pDlg;
}
// ----------------------------------------------------------------------------
// print event handlers
// ----------------------------------------------------------------------------
////void EditSnippetFrame::OnPrintSetup (wxCommandEvent &WXUNUSED(event))
////// ----------------------------------------------------------------------------
////{
////    (*g_pageSetupData) = * g_printData;
////    wxPageSetupDialog pageSetupDialog(this, g_pageSetupData);
////    pageSetupDialog.ShowModal();
////    (*g_printData) = pageSetupDialog.GetPageSetupData().GetPrintData();
////    (*g_pageSetupData) = pageSetupDialog.GetPageSetupData();
////
////    // force printing to colors on a white background
////    //?m_pEdit->SetPrintColourMode(wxSCI_PRINT_COLOURONWHITE);
////    m_pScbEditor->GetControl()->SetPrintColourMode(wxSCI_PRINT_COLOURONWHITE);
////
////    g_bPrinterIsSetup = true;
////}

/***
////// ----------------------------------------------------------------------------
////void EditSnippetFrame::OnPrintPreview (wxCommandEvent& event)
////// ----------------------------------------------------------------------------
////{
////    // ----------------------------------------------------------------
////    // very bad loop in wx284 if setup isnt called first
////    // ----------------------------------------------------------------
////    //(pecan 2007/8/24)
////    // The printer goes into a loop if setup isnt called before preview or print
////    if (not g_bPrinterIsSetup)
////    {
////        OnPrintSetup(event);
////        g_bPrinterIsSetup = true;
////    }
////
////    wxPrintDialogData printDialogData( *g_printData);
////    //FIXME:
//////??    wxPrintPreview *preview =
//////??        new wxPrintPreview (new EditPrint (m_pEdit),
//////??                            new EditPrint (m_pEdit),
//////??                            &printDialogData);
//////??    if (!preview->Ok()) {
//////??        delete preview;
//////??        //wxMessageBox (_("There was a problem with previewing.\n\
//////??/(        messageBox (_("There was a problem with previewing.\n\
//////??                         Perhaps your current printer is not setup correctly?"),
//////??                      _("Previewing"), wxOK);
//////??        return;
//////??    }
////
//////??    wxRect rect = DeterminePrintSize();
//////??    wxPreviewFrame *frame = new wxPreviewFrame (preview, this, _("Print Preview"));
//////??    frame->SetSize (rect);
//////??    frame->Centre(wxBOTH);
//////??    frame->Initialize();
//////??    frame->Show(true);
////}

////// ----------------------------------------------------------------------------
////void EditSnippetFrame::OnPrint (wxCommandEvent& event)
////// ----------------------------------------------------------------------------
////{
////    //FIXME:
////    //(pecan 2007/8/24)
////    // The printer goes into a loop if setup isnt called before preview or print
////    if (not g_bPrinterIsSetup)
////    {
////        OnPrintSetup(event);
////        g_bPrinterIsSetup = true;
////    }
////
////    wxPrintDialogData printDialogData( *g_printData);
////    wxPrinter printer (&printDialogData);
////    EditPrint printout (m_pEdit);
////    if (!printer.Print (this, &printout, true)) {
////        if (wxPrinter::GetLastError() == wxPRINTER_ERROR) {
////        //- wxMessageBox (_("There was a problem with printing.\n\
////        messageBox (_("There was a problem with printing.\n\
////                         Perhaps your current printer is not correctly configured?"),
////                      _("Previewing"), wxOK);
////            return;
////        }
////    }
////    (*g_printData) = printer.GetPrintDialogData().GetPrintData();
////}
***/
// ----------------------------------------------------------------------------
// edit events
// ----------------------------------------------------------------------------
void EditSnippetFrame::OnEditEvent (wxCommandEvent &event)
// ----------------------------------------------------------------------------
{

    SEditorBase* eb = GetEditorManager()->GetActiveEditor();
    ScbEditor* ed = GetEditorManager()->GetBuiltinActiveEditor();

    if (( not ed) || ( not eb )) return;
    cbStyledTextCtrl* stcCtrl = ed->GetControl();
    if (not stcCtrl) return;

    int id = event.GetId();
    switch (id)
    {
        //case wxID_CLEAR:        ed->GetControl()->LineDelete(); break;
        case wxID_CLEAR:        ed->GetControl()->LineCut(); break;
        case wxID_CUT:          ed->Cut(); break;
        case wxID_COPY:         ed->Copy(); break;
        case wxID_PASTE:        ed->Paste(); break;
        case myID_INDENTINC:    ed->DoIndent(); break;
        case myID_INDENTRED:    ed->DoUnIndent(); break;
        case wxID_SELECTALL:    ed->GetControl()->SelectAll(); break;
        case myID_OVERTYPE:     ed->GetControl()->SetOvertype (!ed->GetControl()->GetOvertype()); break;
        case wxID_REDO:         ed->Redo(); break;
        case wxID_UNDO:         ed->Undo(); break;
        case myID_DISPLAYEOL:   stcCtrl->SetViewEOL (!stcCtrl->GetViewEOL()); break;
        case myID_INDENTGUIDE:  stcCtrl->SetIndentationGuides( ! stcCtrl->GetIndentationGuides()); break;
        case myID_LONGLINEON:   stcCtrl->SetEdgeMode (stcCtrl->GetEdgeMode() == 0? wxSCI_EDGE_LINE: wxSCI_EDGE_NONE); break;
        case myID_WHITESPACE:   stcCtrl->SetViewWhiteSpace (stcCtrl->GetViewWhiteSpace() == 0?
                                            wxSCI_WS_VISIBLEALWAYS: wxSCI_WS_INVISIBLE); break;

        case wxID_FIND:         eb->SearchFind(); break;
        case myID_FINDNEXT:
        case myID_FINDPREV:     eb->SearchFindNext( id == myID_FINDNEXT ); break;

        case myID_REPLACE:
        case myID_REPLACENEXT:  eb->OnSearchReplace(); break;

        case myID_BRACEMATCH:   ed->GotoMatchingBrace(); break;
        case myID_GOTO:         eb->SearchGotoLine(); break;


        case myID_READONLY:     stcCtrl->SetReadOnly ( ! stcCtrl->GetReadOnly()); break;

        case myID_CHARSETANSI:
        case myID_CHARSETMAC:
        {       int Nr;
                int charset = stcCtrl->GetCodePage();
                switch (event.GetId()) {
                    case myID_CHARSETANSI: {charset = wxSCI_CHARSET_ANSI; break;}
                    case myID_CHARSETMAC:  {charset = wxSCI_CHARSET_ANSI; break;}
                }
                for (Nr = 0; Nr < wxSCI_STYLE_LASTPREDEFINED; Nr++)
                {
                    stcCtrl->StyleSetCharacterSet (Nr, charset);
                }
                stcCtrl->SetCodePage (charset);
                break;
        }//case

        case myID_CHANGEUPPER:
        case myID_CHANGELOWER:
        {
            switch (event.GetId()) {
                case myID_CHANGELOWER: {
                    stcCtrl->CmdKeyExecute (wxSCI_CMD_LOWERCASE);
                    break;
                }//case
                case myID_CHANGEUPPER: {
                    stcCtrl->CmdKeyExecute (wxSCI_CMD_UPPERCASE);
                    break;
                }//case
            }//switch
            break;
        }//case

        case myID_CONVERTCR:
        case myID_CONVERTCRLF:
        case myID_CONVERTLF: OnConvertEOL(event); break;

        case myID_WRAPMODEON:
            stcCtrl->SetWrapMode (stcCtrl->GetWrapMode() == 0? wxSCI_WRAP_WORD: wxSCI_WRAP_NONE); break;

        default: break;
    }//switch id
}//OnEditEvent
// ----------------------------------------------------------------------------
void EditSnippetFrame::OnConvertEOL (wxCommandEvent &event)
// ----------------------------------------------------------------------------
{
    SEditorBase* eb = GetEditorManager()->GetActiveEditor();
    ScbEditor* ed = GetEditorManager()->GetBuiltinActiveEditor();

    if (( not ed) || ( not eb )) return;
    cbStyledTextCtrl* stcCtrl = ed->GetControl();
    if (not stcCtrl) return;

    int id = event.GetId();
    wxUnusedVar(id);
    int eolMode = stcCtrl->GetEOLMode();
    switch (event.GetId()) {
        case myID_CONVERTCR: { eolMode = wxSCI_EOL_CR; break;}
        case myID_CONVERTCRLF: { eolMode = wxSCI_EOL_CRLF; break;}
        case myID_CONVERTLF: { eolMode = wxSCI_EOL_LF; break;}
    }
    stcCtrl->ConvertEOLs (eolMode);
    stcCtrl->SetEOLMode (eolMode);
}
// ----------------------------------------------------------------------------
void EditSnippetFrame::OnEditEventUI (wxUpdateUIEvent &event)
// ----------------------------------------------------------------------------
{
     //LOGIT( _T("EditSnippetFrame::OnEditEventUI") );

    int id = event.GetId();
    wxUnusedVar(id);
    ScbEditor* ed = GetEditorManager()->GetBuiltinActiveEditor();
    if ( not ed) { event.Skip(); return; }

    //if ( (id == idEditHighlightModeText) || (id == idEditHighlightMode) )
    {
        wxMenu* hl = 0;
        m_menuBar->FindItem(idEditHighlightModeText, &hl);
        //#if defined(LOGGING)
        //LOGIT( _T("updateUI Language[%s]"), ed->GetColourSet()->GetLanguageName(ed->GetLanguage()).c_str());
        //#endif
        if (hl)
            m_menuBar->Check(hl->FindItem(ed->GetColourSet()->GetLanguageName(ed->GetLanguage())), true);
    }

    if (GetEditorManager()->GetEditorsCount() > 0 )
    {
       if (GetEditorManager()->GetActiveEditor())
            event.Enable(true);
    }
    else{
        event.Enable (false);
    }

}

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------
void EditSnippetFrame::CreateMenu ()
// ----------------------------------------------------------------------------
{

    // File menu
    wxMenu *menuFile = new wxMenu;
    menuFile->Append (wxID_OPEN, _("&Open ..\tCtrl+O"));
    menuFile->Append (wxID_SAVE, _("&Save\tCtrl+S"));
    menuFile->Append (wxID_SAVEAS, _("Save &as ..\tCtrl+Shift+S"));
    menuFile->Append (wxID_CLOSE, _("&Close\tCtrl+W"));
    menuFile->AppendSeparator();
    menuFile->Append (myID_PROPERTIES, _("Proper&ties ..\tCtrl+I"));
    //-menuFile->AppendSeparator();
    //-menuFile->Append (wxID_PRINT_SETUP, _("Print Set&up .."));
    //-menuFile->Append (wxID_PREVIEW, _("Print Pre&view\tCtrl+Shift+P"));
    menuFile->Append (wxID_PRINT, _("&Print ..\tCtrl+P"));
    menuFile->AppendSeparator();
    menuFile->Append (wxID_EXIT, _("&Quit\tCtrl+Q"));

    // Edit menu
    wxMenu *menuEdit = new wxMenu;
    menuEdit->Append (wxID_UNDO, _("&Undo\tCtrl+Z"));
    menuEdit->Append (wxID_REDO, _("&Redo\tCtrl+Shift+Z"));
    menuEdit->AppendSeparator();
    menuEdit->Append (wxID_CUT, _("Cu&t\tCtrl+X"));
    menuEdit->Append (wxID_COPY, _("&Copy\tCtrl+C"));
    menuEdit->Append (wxID_PASTE, _("&Paste\tCtrl+V"));
    menuEdit->Append (wxID_CLEAR, _("&LineDelete\tCtrl+L"));
    menuEdit->AppendSeparator();
    menuEdit->Append (wxID_FIND, _("&Find\tCtrl+F"));
    menuEdit->Enable (wxID_FIND, false);
    menuEdit->Append (myID_FINDNEXT, _("Find &next\tF3"));
    menuEdit->Enable (myID_FINDNEXT, false);
    menuEdit->Append (myID_FINDPREV,       _("Find pre&vious\tShift+F3"));
    menuEdit->Enable (myID_FINDPREV, false);

    menuEdit->Append (myID_REPLACE, _("&Replace\tCtrl+R"));
    menuEdit->Enable (myID_REPLACE, false);
    menuEdit->Append (myID_REPLACENEXT, _("Replace &again\tShift+F4"));
    menuEdit->Enable (myID_REPLACENEXT, false);

    menuEdit->AppendSeparator();
    menuEdit->Append (myID_BRACEMATCH, _("&Match brace\tCtrl+M"));
    menuEdit->Append (myID_GOTO, _("&Goto\tCtrl+G"));
    menuEdit->AppendSeparator();
    menuEdit->Append (myID_INDENTINC, _("&Indent increase\tTab"));
    menuEdit->Append (myID_INDENTRED, _("I&ndent reduce\tBksp"));
    menuEdit->AppendSeparator();
    menuEdit->Append (wxID_SELECTALL, _("&Select all\tCtrl+A"));

////    // hilight submenu
////    wxMenu *menuHilight = new wxMenu;
////    int Nr;
////    for (Nr = 0; Nr < g_LanguagePrefsSize; Nr++) {
////        menuHilight->Append (myID_HILIGHTFIRST + Nr,
////                             g_LanguagePrefs [Nr].name);
////    }

    // charset submenu
    wxMenu *menuCharset = new wxMenu;
    menuCharset->Append (myID_CHARSETANSI, _("&ANSI (Windows)"));
    menuCharset->Append (myID_CHARSETMAC, _("&MAC (Macintosh)"));

    // View menu
    wxMenu *menuView = new wxMenu;

    //-menuView->Append (myID_HILIGHTLANG, _("&Hilight language .."), menuHilight);
    wxMenu* menuHilight = new wxMenu;
    CreateMenuViewLanguage( menuHilight );
    menuView->Append (idEditHighlightMode, _("&Hilight language .."), menuHilight);

    menuView->AppendSeparator();
    menuView->AppendCheckItem (myID_OVERTYPE, _("&Overwrite mode\tIns"));
    menuView->Check (myID_OVERTYPE, g_CommonPrefs.overTypeInitial);
    menuView->AppendCheckItem (myID_WRAPMODEON, _("&Wrap mode\tCtrl+U"));
    menuView->Check (myID_WRAPMODEON, g_CommonPrefs.wrapModeInitial);
    menuView->AppendSeparator();
    menuView->AppendCheckItem (myID_DISPLAYEOL, _("Show line &endings"));
    menuView->Check (myID_DISPLAYEOL, g_CommonPrefs.displayEOLEnable);
    menuView->AppendCheckItem (myID_INDENTGUIDE, _("Show &indent guides"));
    menuView->Check (myID_INDENTGUIDE, g_CommonPrefs.indentGuideEnable);
    menuView->AppendCheckItem (myID_LONGLINEON, _("Show &long line marker"));
    menuView->Check (myID_LONGLINEON, g_CommonPrefs.longLineOnEnable);
    menuView->AppendCheckItem (myID_WHITESPACE, _("Show white&space"));
    menuView->Check (myID_WHITESPACE, g_CommonPrefs.whiteSpaceEnable);
    menuView->AppendSeparator();
    menuView->Append (myID_USECHARSET, _("Use &code page of .."), menuCharset);

    // change case submenu
    wxMenu *menuChangeCase = new wxMenu;
    menuChangeCase->Append (myID_CHANGEUPPER, _("&Upper case"));
    menuChangeCase->Append (myID_CHANGELOWER, _("&Lower case"));

    // convert EOL submenu
    wxMenu *menuConvertEOL = new wxMenu;
    menuConvertEOL->Append (myID_CONVERTCR, _("CR (&Linux)"));
    menuConvertEOL->Append (myID_CONVERTCRLF, _("CR+LF (&Windows)"));
    menuConvertEOL->Append (myID_CONVERTLF, _("LF (&Macintosh)"));

    // Extra menu
    wxMenu *menuExtra = new wxMenu;
    menuExtra->AppendCheckItem (myID_READONLY, _("&Readonly mode"));
    menuExtra->Check (myID_READONLY, g_CommonPrefs.readOnlyInitial);
    menuExtra->AppendSeparator();
    menuExtra->Append (myID_CHANGECASE, _("Change &case to .."), menuChangeCase);
    menuExtra->AppendSeparator();
    menuExtra->Append (myID_CONVERTEOL, _("Convert line &endings to .."), menuConvertEOL);

    // Help menu
    //-wxMenu *menuHelp = new wxMenu;
    //-menuHelp->Append (wxID_ABOUT, _("&About ..\tShift+F1"));

    // construct menu
    m_menuBar->Append (menuFile, _("&File"));
    m_menuBar->Append (menuEdit, _("&Edit"));
    m_menuBar->Append (menuView, _("&View"));
    m_menuBar->Append (menuExtra, _("E&xtra"));
    //-m_menuBar->Append (menuHelp, _("&Help"));


    SetMenuBar (m_menuBar);

}//CreateMenu
// ----------------------------------------------------------------------------
void EditSnippetFrame::CreateMenuViewLanguage(wxMenu* menuHilight)
// ----------------------------------------------------------------------------
{
    // -------------------------------------
    // Find Menus that we'll change later
    // -------------------------------------
    //-int tmpidx = m_menuBar->FindMenu(_("&View"));
    wxMenu* hl = menuHilight;

    // Add "Plain text" at top of list
    int id = idEditHighlightModeText;
    hl->AppendRadioItem(id, _T("Plain text"),wxString::Format(_("Switch highlighting mode for current document to \"%s\""), _T("Plain text")));
    Connect(id, -1, wxEVT_COMMAND_MENU_SELECTED,
                    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                    &EditSnippetFrame::OnEditHighlightMode);

    //-if(tmpidx!=wxNOT_FOUND)
    {
        //-m_menuBar->FindItem(idEditHighlightModeText, &hl);
        if (hl)
        {
            SEditorColourSet* theme = GetEditorManager()->GetColourSet();
            if (theme)
            {
                wxArrayString langs = theme->GetAllHighlightLanguages();
                for (size_t i = 0; i < langs.GetCount(); ++i)
                {
                    if (i > 0 && !(i % 20))
                        hl->Break(); // break into columns every 20 items
                    int id = wxNewId();
                    hl->AppendRadioItem(id, langs[i],
                                wxString::Format(_("Switch highlighting mode for current document to \"%s\""), langs[i].c_str()));
                    Connect(id, -1, wxEVT_COMMAND_MENU_SELECTED,
                            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                            &EditSnippetFrame::OnEditHighlightMode);
                }//for
            }//if theme
        }//if hl
    }//if tmpidx

}
// ----------------------------------------------------------------------------
void EditSnippetFrame::FileOpen (wxString fname)
// ----------------------------------------------------------------------------
{
    wxFileName w(fname); w.Normalize(); fname = w.GetFullPath();


    //m_pScbEditor->GetControl()->LoadFile(fname);
    GetEditorManager()->Open(fname);
}

// ----------------------------------------------------------------------------
wxRect EditSnippetFrame::DeterminePrintSize ()
// ----------------------------------------------------------------------------
{

    wxSize scr = wxGetDisplaySize();

    // determine position and size (shifting 16 left and down)
    wxRect rect = GetRect();
    rect.x += 16;
    rect.y += 16;
    rect.width = wxMin (rect.width, (scr.x - rect.x));
    rect.height = wxMin (rect.height, (scr.x - rect.y));

    return rect;
}
// ----------------------------------------------------------------------------
void EditSnippetFrame::OnFilePrint(wxCommandEvent& /*event*/)
// ----------------------------------------------------------------------------
{
    GetEditorManager()->FilePrint(this);
}
// ----------------------------------------------------------------------------
void EditSnippetFrame::OnEditHighlightMode(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    ScbEditor* ed = GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
    {
        SEditorColourSet* theme = GetEditorManager()->GetColourSet();
        if (theme)
        {
            HighlightLanguage lang = theme->GetHighlightLanguage(_T(""));
            if (event.GetId() != idEditHighlightModeText)
            {
                wxMenu* hl = 0;
                GetMenuBar()->FindItem(idEditHighlightModeText, &hl);
                if (hl)
                {
                    wxMenuItem* item = hl->FindItem(event.GetId());
                    if (item)
                    #if wxCHECK_VERSION(2, 9, 0)
                        lang = theme->GetHighlightLanguage(item->GetItemLabelText());
                    #else
                        lang = theme->GetHighlightLanguage(item->GetLabel());
                    #endif
                }
            }
            ed->SetLanguage(lang);
        }
    }
}
// ----------------------------------------------------------------------------
void EditSnippetFrame::OnFrameActivated(wxActivateEvent& event)
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

        //-wxWindow* mainFrame = GetConfig()->GetMainFrame();
        //-wxActivateEvent evt(wxEVT_ACTIVATE);
        //-mainFrame->ProcessEvent(evt);
        #if defined(LOGGING)
        LOGIT( _T("EditSnippetFrame::OnAppActivate"));
        #endif
        //-if  ( (GetConfig()->GetEditorManagerCount() ) <--causes loop betwn ThreadSearchFrame and EditSnippetFrame
        if  ( (GetConfig()->GetEditorManager(this) )
              && (Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/check_modified_files"), true))
            )
            //-for (int i = 0; i < GetConfig()->GetEditorManagerCount(); ++i) <--causes loop betwn ThreadSearchFrame and EditSnippetFrame
            {
                // for some reason a mouse up event doesnt make it into scintilla (scintilla bug)
                // therefore the workaournd is not to directly call the editorManager, but
                // take a detour through an event
                // the bug is when the file has been offered to reload, no matter what answer you
                // give the mouse is in a selecting mode, adding/removing things to it's selection as you
                // move it around
                // so : idEditorManagerCheckFiles, EditorManager::OnCheckForModifiedFiles just exist for this workaround
                wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, idSEditorManagerCheckFiles);
                wxPostEvent(GetConfig()->GetEditorManager(this), evt);
                //-GetConfig()->GetEditorManager(i)->ProcessEvent( evt);
            }
    }while(0); //do only once

    m_bOnActivateBusy = 0;
    //-event.Skip();
    return;
}
// ----------------------------------------------------------------------------
void EditSnippetFrame::OpenDroppedFiles( const wxArrayString& arrayData)
// ----------------------------------------------------------------------------
{
    //NOTE: putting breakpoints in this routine will hang CB
    // From a console do "gdb cbEdit.exe" instead


    for( int i=0; i<(int)arrayData.GetCount(); ++i)
    {
        #ifdef LOGGING
         LOGIT( _T("Dragged File[%s]"), arrayData.Item(i).c_str() );
        #endif //LOGGING
        wxString filename = arrayData.Item(i);
        if ( wxFileExists(filename) )
        {
            ScbEditor* sed = GetEditorManager()->Open( filename );
            if ( sed )
            {
                //FIXME:AddToRecentFilesHistory( filename );
                //FIXME:DoUpdateStatusBar();
            }
        }//if exists
    }//for
}
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

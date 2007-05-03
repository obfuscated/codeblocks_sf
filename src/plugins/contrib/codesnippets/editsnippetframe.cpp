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
// RCS-ID: $Id: editsnippetframe.cpp 74 2007-05-03 20:57:17Z Pecan $

#include "editsnippetframe.h"

#include <wx/filename.h>
#include <wx/fileconf.h>


#include "codesnippetswindow.h"
#include "snippetsconfig.h"
#include "edit.h"
#include "messagebox.h"
#include "version.h"

// ----------------------------------------------------------------------------
BEGIN_EVENT_TABLE (EditSnippetFrame, wxFrame)
    // common
    EVT_CLOSE (                      EditSnippetFrame::OnCloseWindow)
    //-- EVT_CLOSE works, but the next three EVTs never get called
    EVT_SET_FOCUS  (                 EditSnippetFrame::OnFocusWindow)
    EVT_KILL_FOCUS (                 EditSnippetFrame::OnKillFocusWindow)
    EVT_LEAVE_WINDOW (               EditSnippetFrame::OnLeaveWindow)
    // file
    EVT_MENU (wxID_OPEN,             EditSnippetFrame::OnFileOpen)
    EVT_MENU (wxID_SAVE,             EditSnippetFrame::OnFileSave)
    EVT_MENU (wxID_SAVEAS,           EditSnippetFrame::OnFileSaveAs)
    EVT_MENU (wxID_CLOSE,            EditSnippetFrame::OnFileClose)
    // properties
    EVT_MENU (myID_PROPERTIES,       EditSnippetFrame::OnProperties)
    // print and exit
    //EVT_MENU (wxID_PRINT_SETUP,      EditSnippetFrame::OnPrintSetup)
    //EVT_MENU (wxID_PREVIEW,          EditSnippetFrame::OnPrintPreview)
    //EVT_MENU (wxID_PRINT,            EditSnippetFrame::OnPrint)
    EVT_MENU (wxID_EXIT,             EditSnippetFrame::OnExit)
    // edit
    EVT_MENU (wxID_CLEAR,            EditSnippetFrame::OnEditEvent)
    EVT_MENU (wxID_CUT,              EditSnippetFrame::OnEditEvent)
    EVT_MENU (wxID_COPY,             EditSnippetFrame::OnEditEvent)
    EVT_MENU (wxID_PASTE,            EditSnippetFrame::OnEditEvent)
    EVT_MENU (myID_INDENTINC,        EditSnippetFrame::OnEditEvent)
    EVT_MENU (myID_INDENTRED,        EditSnippetFrame::OnEditEvent)
    EVT_MENU (wxID_SELECTALL,        EditSnippetFrame::OnEditEvent)
    EVT_MENU (myID_SELECTLINE,       EditSnippetFrame::OnEditEvent)
    EVT_MENU (wxID_REDO,             EditSnippetFrame::OnEditEvent)
    EVT_MENU (wxID_UNDO,             EditSnippetFrame::OnEditEvent)
    // find
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
    // view
    EVT_MENU_RANGE (myID_HILIGHTFIRST, myID_HILIGHTLAST,
                                     EditSnippetFrame::OnEditEvent)
    EVT_MENU (myID_DISPLAYEOL,       EditSnippetFrame::OnEditEvent)
    EVT_MENU (myID_INDENTGUIDE,      EditSnippetFrame::OnEditEvent)
    EVT_MENU (myID_LINENUMBER,       EditSnippetFrame::OnEditEvent)
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

END_EVENT_TABLE ()
// ----------------------------------------------------------------------------
class EditFrameDropTarget : public wxTextDropTarget
// ----------------------------------------------------------------------------
{
    // Drop target used to place dragged data into Properties dialog

	public:
		EditFrameDropTarget(EditSnippetFrame* window) : m_Window(window) {}
		~EditFrameDropTarget() {}
		bool OnDropText(wxCoord x, wxCoord y, const wxString& data);
	private:
		EditSnippetFrame* m_Window;
};
// ----------------------------------------------------------------------------
bool EditFrameDropTarget::OnDropText(wxCoord x, wxCoord y, const wxString& data)
// ----------------------------------------------------------------------------
{
    // Put dragged text into SnippetTextCtrl
    #ifdef LOGGING
     LOGIT( _T("Dragged Data[%s]"), data.GetData() );
    #endif //LOGGING
    //m_Window->m_pEdit->WriteText(data);
    m_Window->m_pEdit->AddText(data);
    return true;

} // end of OnDropText

// ----------------------------------------------------------------------------
EditSnippetFrame::EditSnippetFrame(const wxTreeItemId  TreeItemId, int* pRetcode )
// ----------------------------------------------------------------------------
	: wxFrame( GetConfig()->GetSnippetsWindow(), wxID_ANY, _T("Edit snippet"),
		wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE|wxFRAME_FLOAT_ON_PARENT)
{
    //ctor

    //pWaitingSemaphore = pWaitSem;
    m_SnippetItemId = TreeItemId;
    m_EditSnippetText = GetConfig()->GetSnippetsTreeCtrl()->GetSnippet(TreeItemId);

    m_EditFileName = wxEmptyString;
    if ( (m_EditSnippetText.Length() < 129) && (::wxFileExists(m_EditSnippetText)) )
        m_EditFileName = m_EditSnippetText;

    m_EditSnippetLabel = GetConfig()->GetSnippetsTreeCtrl()->GetSnippetLabel(TreeItemId);
    m_pReturnCode = pRetcode;
    *pRetcode = 0;
    m_nReturnCode = wxID_CANCEL;

    // intitialize important variables
    m_pEdit = NULL;

    // set icon and background
    //SetTitle (m_EditFileName);
    //if (m_EditFileName.IsEmpty())
        SetTitle(m_EditSnippetLabel);
    //SetIcon (wxICON (mondrian));
    SetIcon(GetConfig()->GetSnipImages()->GetSnipListIcon(TREE_IMAGE_ALL_SNIPPETS));
    //-SetBackgroundColour (_T("WHITE"));
    m_SysWinBkgdColour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW); //(pecan 2007/3/27)
    SetBackgroundColour( m_SysWinBkgdColour); //(pecan 2007/3/27)

    // create menu
    m_menuBar = new wxMenuBar;
    CreateMenu ();

    // open first page
    m_pEdit = new Edit (this, -1);
	if (not m_EditFileName.IsEmpty())
                m_pEdit->LoadFile(m_EditFileName);
    else
    {    m_pEdit->SetText(m_EditSnippetText);
        // SetText() marked the file as modified
        // Unmarked it by saving to a dummy file
        #if defined(__WXMSW__)
            m_pEdit->SaveFile(wxT("nul"));
        #else
            m_pEdit->SaveFile(wxT("/dev/null"));
        #endif
        // reset the undo history to avoid undoing to a blank page
        m_pEdit->EmptyUndoBuffer();
    }

	// Load the window's size
//    wxFileConfig cfgFile(wxEmptyString,     // appname
//                        wxEmptyString,      // vendor
//                        GetConfig()->SettingsSnippetsCfgFullPath,      // local filename
//                        wxEmptyString,      // global file
//                        wxCONFIG_USE_LOCAL_FILE);

    wxFileConfig& cfgFile = *(GetConfig()->GetCfgFile());

    cfgFile.Read( wxT("EditDlgXpos"),       &GetConfig()->nEditDlgXpos,20);
    cfgFile.Read( wxT("EditDlgYpos"),       &GetConfig()->nEditDlgYpos,20);
	cfgFile.Read( wxT("EditDlgWidth"),      &GetConfig()->nEditDlgWidth, 500 ) ;
	cfgFile.Read( wxT("EditDlgHeight"),     &GetConfig()->nEditDlgHeight, 400 ) ;
	cfgFile.Read( wxT("EditDlgMaximized"),  &GetConfig()->bEditDlgMaximized, false );
	//SetSize(GetConfig()->nEditDlgWidth, GetConfig()->nEditDlgHeight);
    LOGIT( _T("EditDlgPositin IN X[%d]Y[%d]Width[%d]Height[%d]"),
        GetConfig()->nEditDlgXpos,GetConfig()->nEditDlgYpos,
        GetConfig()->nEditDlgWidth, GetConfig()->nEditDlgHeight );
    SetSize(GetConfig()->nEditDlgXpos, GetConfig()->nEditDlgYpos, GetConfig()->nEditDlgWidth, GetConfig()->nEditDlgHeight);

//	if (( GetConfig()->IsPlugin()) && (GetConfig()->bEditDlgMaximized) )
//		Maximize(true);


	//EVT_SET_FOCUS, EVT_KILL_FOCUS, EVT_ENTER_WINDER, EVT_LEAVE_WINDOW
	// never get invoked by wsWidgets for this frame. So we'll connect instead.
////    m_pEdit->Connect(wxEVT_ENTER_WINDOW,
////                    (wxObjectEventFunction)(wxEventFunction)
////                    (wxMouseEventFunction)&EditSnippetFrame::OnEnterWindow,
////                     NULL, this);
    m_pEdit->Connect(wxEVT_LEAVE_WINDOW,
                    (wxObjectEventFunction)(wxEventFunction)
                    (wxMouseEventFunction)&EditSnippetFrame::OnLeaveWindow,
                     NULL, this);
    m_pEdit->Connect(wxEVT_SET_FOCUS,
                    (wxObjectEventFunction)(wxEventFunction)
                    (wxFocusEventFunction)&EditSnippetFrame::OnFocusWindow,
                     NULL, this);
    m_pEdit->Connect(wxEVT_KILL_FOCUS,
                    (wxObjectEventFunction)(wxEventFunction)
                    (wxFocusEventFunction)&EditSnippetFrame::OnKillFocusWindow,
                     NULL, this);

    //SetDropTarget(new EditFrameDropTarget(this));
	m_pEdit->SetFocus();

}

// ----------------------------------------------------------------------------
EditSnippetFrame::~EditSnippetFrame()
// ----------------------------------------------------------------------------
{
    //dtor
    if (m_pEdit) delete m_pEdit;
}
// ----------------------------------------------------------------------------
void EditSnippetFrame::End_SnippetFrame(int wxID_OKorCANCEL)
// ----------------------------------------------------------------------------
{
    // Called from OnOk/OnCancel routines or OnClose

	// Save the window's size
	//ConfigManager* cfgMan = Manager::Get()->GetConfigManager(_T("codesnippets"));
//    wxFileConfig cfgFile(wxEmptyString,     // appname
//                        wxEmptyString,      // vendor
//                        GetConfig()->SettingsSnippetsCfgFullPath,      // local filename
//                        wxEmptyString,      // global file
//                        wxCONFIG_USE_LOCAL_FILE);

    wxFileConfig& cfgFile = *(GetConfig()->GetCfgFile());

    int x,y,w,h;
    GetPosition(&x,&y); GetSize(&w,&h);
    cfgFile.Write( wxT("EditDlgXpos"),  x );
    cfgFile.Write( wxT("EditDlgYpos"),  y );
    cfgFile.Write( wxT("EditDlgWidth"),  w );
    cfgFile.Write( wxT("EditDlgHeight"), h );
     LOGIT( _T("EditDlgPositin OUT X[%d]Y[%d]Width[%d]Height[%d]"),x,y,w,h );

    cfgFile.Write( wxT("EditDlgMaximized"),  false );
    cfgFile.Flush();

    // If this was an external file, it's already been saved by FileClose()
    // XML data is in m_pEditSnippetText and will be obtained by the
    // parent via the GetText() call below.


    // If parent is waiting on us, post we're finished
	//if (pWaitingSemaphore)
    //    pWaitingSemaphore->Post();
	*m_pReturnCode = (wxID_OKorCANCEL);
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

// ----------------------------------------------------------------------------
void EditSnippetFrame::OnOK(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // This routine not clled, because Edit is not a dialog
    End_SnippetFrame(wxID_OK);
	//-EndModal(wxID_OK);
}

// ----------------------------------------------------------------------------
void EditSnippetFrame::OnCancel(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // This routine not called, because Edit is not a dialog
    End_SnippetFrame(wxID_CANCEL);
	//-EndModal(wxID_CANCEL);
}
// ----------------------------------------------------------------------------
void EditSnippetFrame::OnHelp(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
	// Link to the Wiki which contains information about the available macros
	wxLaunchDefaultBrowser(_T("http://wiki.codeblocks.org/index.php?title=Builtin_variables"));
}

// ----------------------------------------------------------------------------
// common event handlers
// ----------------------------------------------------------------------------
void EditSnippetFrame::OnCloseWindow (wxCloseEvent &event)
// ----------------------------------------------------------------------------
{
    wxCommandEvent evt;
    OnFileClose (evt);
    if (m_pEdit && m_pEdit->Modified()) {
        if (event.CanVeto()) event.Veto (true);
        return;
    }
    End_SnippetFrame(m_nReturnCode);

}
// ----------------------------------------------------------------------------
void EditSnippetFrame::OnFocusWindow (wxFocusEvent &event)
// ----------------------------------------------------------------------------
{
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
    //MakeModal(false);<= using wxFRAME_FLOAT_ON_PARENT works much better
    event.Skip();
}
// ----------------------------------------------------------------------------
void EditSnippetFrame::OnLeaveWindow (wxMouseEvent &event)
// ----------------------------------------------------------------------------
{

    //MakeModal(false);<= using wxFRAME_FLOAT_ON_PARENT works much better
    event.Skip();
}

// ----------------------------------------------------------------------------
void EditSnippetFrame::OnAbout (wxCommandEvent &WXUNUSED(event))
// ----------------------------------------------------------------------------
{
    //-AppAbout (this);
}

// ----------------------------------------------------------------------------
void EditSnippetFrame::OnExit (wxCommandEvent &WXUNUSED(event))
// ----------------------------------------------------------------------------
{
    Close (true);
}

// ----------------------------------------------------------------------------
// file event handlers
// ----------------------------------------------------------------------------
void EditSnippetFrame::OnFileOpen (wxCommandEvent &WXUNUSED(event))
// ----------------------------------------------------------------------------
{
    if (!m_pEdit) return;
    wxString fname;
    wxFileDialog dlg (this, _T("Open file"), _T(""), _T(""), _T("Any file (*)|*"),
                      wxOPEN | wxFILE_MUST_EXIST | wxCHANGE_DIR);
    if (dlg.ShowModal() != wxID_OK) return;
    fname = dlg.GetPath ();
    FileOpen (fname);
}

// ----------------------------------------------------------------------------
void EditSnippetFrame::OnFileSave (wxCommandEvent &WXUNUSED(event))
// ----------------------------------------------------------------------------
{
    if (!m_pEdit) return;
    if (!m_pEdit->Modified()) {
//        wxMessageBox (_("There are no changes to save!"), _("Save file"),
//                      wxOK | wxICON_EXCLAMATION);
        messageBox (_("There are no changes to save!"), _("Save file"),
                      wxOK | wxICON_EXCLAMATION);
        return;
    }
    if ( not m_EditFileName.IsEmpty())
        m_pEdit->SaveFile ();
    else
    {   // XML data to save
        m_EditSnippetText = m_pEdit->GetText();
        // we just transfered the data, set wxID_OK.
        m_nReturnCode = wxID_OK;
        // Unmarked "modified"  by saving to a dummy file
        #if defined(__WXMSW__)
            m_pEdit->SaveFile(wxT("nul"));
        #else
            m_pEdit->SaveFile(wxT("/dev/null"));
        #endif

    }
}

// ----------------------------------------------------------------------------
void EditSnippetFrame::OnFileSaveAs (wxCommandEvent &WXUNUSED(event))
// ----------------------------------------------------------------------------
{
    if (!m_pEdit) return;
    wxString filename = wxEmptyString;
    wxFileDialog dlg (this, _T("Save file"), _T(""), _T(""), _T("Any file (*)|*"), wxSAVE|wxOVERWRITE_PROMPT);
    if (dlg.ShowModal() != wxID_OK) return;
    filename = dlg.GetPath();
    m_pEdit->SaveFile (filename);
}

// ----------------------------------------------------------------------------
void EditSnippetFrame::OnFileClose (wxCommandEvent &WXUNUSED(event))
// ----------------------------------------------------------------------------
{

    if (!m_pEdit) return;
    // if text in external file, save if needed
    if (m_pEdit->Modified() )
    {
        //if (wxMessageBox (_("Text is not saved, save before closing?"), _("Close"),
        if (messageBox (_("Text is not saved, save before closing?"), _("Close"),
                          wxYES_NO | wxICON_QUESTION) == wxYES) {
            // save a real file
            if ( not m_EditFileName.IsEmpty() )
            {
                m_pEdit->SaveFile();
                if (m_pEdit->Modified())
                {
                    //wxMessageBox (_("Text could not be saved!"), _("Close abort"),
                    messageBox (_("Text could not be saved!"), _("Close abort"),
                                  wxOK | wxICON_EXCLAMATION);
                    m_nReturnCode = wxID_CANCEL;
                    return;
                }
                m_nReturnCode = wxID_OK;
            }
            else  // else save XML data
            {
                m_EditSnippetText = m_pEdit->GetText();
                m_nReturnCode = wxID_OK;
            }
        }
    }

    m_pEdit->SetFilename (wxEmptyString);
    m_pEdit->ClearAll();
    m_pEdit->SetSavePoint();

}

// ----------------------------------------------------------------------------
// properties event handlers
// ----------------------------------------------------------------------------
void EditSnippetFrame::OnProperties (wxCommandEvent &WXUNUSED(event))
// ----------------------------------------------------------------------------
{
    if (!m_pEdit) return;
    EditProperties (m_pEdit, 0);
}

// ----------------------------------------------------------------------------
// print event handlers
// ----------------------------------------------------------------------------
void EditSnippetFrame::OnPrintSetup (wxCommandEvent &WXUNUSED(event))
// ----------------------------------------------------------------------------
{
    (*g_pageSetupData) = * g_printData;
    wxPageSetupDialog pageSetupDialog(this, g_pageSetupData);
    pageSetupDialog.ShowModal();
    (*g_printData) = pageSetupDialog.GetPageSetupData().GetPrintData();
    (*g_pageSetupData) = pageSetupDialog.GetPageSetupData();
}

// ----------------------------------------------------------------------------
void EditSnippetFrame::OnPrintPreview (wxCommandEvent &WXUNUSED(event))
// ----------------------------------------------------------------------------
{
    wxPrintDialogData printDialogData( *g_printData);
    wxPrintPreview *preview =
        new wxPrintPreview (new EditPrint (m_pEdit),
                            new EditPrint (m_pEdit),
                            &printDialogData);
    if (!preview->Ok()) {
        delete preview;
        /*wxMessageBox (_("There was a problem with previewing.\n\ */
        messageBox (_("There was a problem with previewing.\n\
                         Perhaps your current printer is not correctly?"),
                      _("Previewing"), wxOK);
        return;
    }
    wxRect rect = DeterminePrintSize();
    wxPreviewFrame *frame = new wxPreviewFrame (preview, this, _("Print Preview"));
    frame->SetSize (rect);
    frame->Centre(wxBOTH);
    frame->Initialize();
    frame->Show(true);
}

// ----------------------------------------------------------------------------
void EditSnippetFrame::OnPrint (wxCommandEvent &WXUNUSED(event))
// ----------------------------------------------------------------------------
{
    wxPrintDialogData printDialogData( *g_printData);
    wxPrinter printer (&printDialogData);
    EditPrint printout (m_pEdit);
    if (!printer.Print (this, &printout, true)) {
        if (wxPrinter::GetLastError() == wxPRINTER_ERROR) {
        /* wxMessageBox (_("There was a problem with printing.\n\ */
        messageBox (_("There was a problem with printing.\n\
                         Perhaps your current printer is not correctly configured?"),
                      _("Previewing"), wxOK);
            return;
        }
    }
    (*g_printData) = printer.GetPrintDialogData().GetPrintData();
}

// ----------------------------------------------------------------------------
// edit events
// ----------------------------------------------------------------------------
void EditSnippetFrame::OnEditEvent (wxCommandEvent &event)
// ----------------------------------------------------------------------------
{
    if (m_pEdit) m_pEdit->ProcessEvent (event);
}
// ----------------------------------------------------------------------------
void EditSnippetFrame::OnEditEventUI (wxUpdateUIEvent &event)
// ----------------------------------------------------------------------------
{
     //LOGIT( _T("EditSnippetFrame::OnEditEventUI") );
    if (m_pEdit )
    {
        m_pEdit->ProcessEvent (event);
    }else{
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
    menuFile->AppendSeparator();
    //menuFile->Append (wxID_PRINT_SETUP, _("Print Set&up .."));
    //menuFile->Append (wxID_PREVIEW, _("Print Pre&view\tCtrl+Shift+P"));
    //menuFile->Append (wxID_PRINT, _("&Print ..\tCtrl+P"));
    //menuFile->AppendSeparator();
    menuFile->Append (wxID_EXIT, _("&Quit\tCtrl+Q"));

    // Edit menu
    wxMenu *menuEdit = new wxMenu;
    menuEdit->Append (wxID_UNDO, _("&Undo\tCtrl+Z"));
    menuEdit->Append (wxID_REDO, _("&Redo\tCtrl+Shift+Z"));
    menuEdit->AppendSeparator();
    menuEdit->Append (wxID_CUT, _("Cu&t\tCtrl+X"));
    menuEdit->Append (wxID_COPY, _("&Copy\tCtrl+C"));
    menuEdit->Append (wxID_PASTE, _("&Paste\tCtrl+V"));
    menuEdit->Append (wxID_CLEAR, _("&Delete\tDel"));
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
    menuEdit->Enable (myID_GOTO, false);
    menuEdit->AppendSeparator();
    menuEdit->Append (myID_INDENTINC, _("&Indent increase\tTab"));
    menuEdit->Append (myID_INDENTRED, _("I&ndent reduce\tBksp"));
    menuEdit->AppendSeparator();
    menuEdit->Append (wxID_SELECTALL, _("&Select all\tCtrl+A"));
    menuEdit->Append (myID_SELECTLINE, _("Select &line\tCtrl+L"));

    // hilight submenu
    wxMenu *menuHilight = new wxMenu;
    int Nr;
    for (Nr = 0; Nr < g_LanguagePrefsSize; Nr++) {
        menuHilight->Append (myID_HILIGHTFIRST + Nr,
                             g_LanguagePrefs [Nr].name);
    }

    // charset submenu
    wxMenu *menuCharset = new wxMenu;
    menuCharset->Append (myID_CHARSETANSI, _("&ANSI (Windows)"));
    menuCharset->Append (myID_CHARSETMAC, _("&MAC (Macintosh)"));

    // View menu
    wxMenu *menuView = new wxMenu;
    menuView->Append (myID_HILIGHTLANG, _("&Hilight language .."), menuHilight);
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
    menuView->AppendCheckItem (myID_LINENUMBER, _("Show line &numbers"));
    menuView->Check (myID_LINENUMBER, g_CommonPrefs.lineNumberEnable);
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

}

// ----------------------------------------------------------------------------
void EditSnippetFrame::FileOpen (wxString fname)
// ----------------------------------------------------------------------------
{
    wxFileName w(fname); w.Normalize(); fname = w.GetFullPath();
    m_pEdit->LoadFile (fname);
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

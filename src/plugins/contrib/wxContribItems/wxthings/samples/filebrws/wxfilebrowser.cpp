/////////////////////////////////////////////////////////////////////////////
// Name:        wxfilebrowser.cpp
// Purpose:     wxFileBrowser control test program
// Author:      John Labenski
// Modified by:
// Created:     04/01/98
// RCS-ID:
// Copyright:   (c) John Labenski
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/buffer.h>
#include <wx/image.h>
#include <wx/splitter.h>
#include <wx/notebook.h>
#include "wx/things/filebrws.h"

/* XPM */
static const char *mondrian_xpm[] = {
/* columns rows colors chars-per-pixel */
"32 32 6 1",
"  c Black",
". c Blue",
"X c #00bf00",
"o c Red",
"O c Yellow",
"+ c Gray100",
/* pixels */
"                                ",
" oooooo +++++++++++++++++++++++ ",
" oooooo +++++++++++++++++++++++ ",
" oooooo +++++++++++++++++++++++ ",
" oooooo +++++++++++++++++++++++ ",
" oooooo +++++++++++++++++++++++ ",
" oooooo +++++++++++++++++++++++ ",
" oooooo +++++++++++++++++++++++ ",
"                                ",
" ++++++ ++++++++++++++++++ .... ",
" ++++++ ++++++++++++++++++ .... ",
" ++++++ ++++++++++++++++++ .... ",
" ++++++ ++++++++++++++++++ .... ",
" ++++++ ++++++++++++++++++ .... ",
" ++++++ ++++++++++++++++++      ",
" ++++++ ++++++++++++++++++ ++++ ",
" ++++++ ++++++++++++++++++ ++++ ",
" ++++++ ++++++++++++++++++ ++++ ",
" ++++++ ++++++++++++++++++ ++++ ",
" ++++++ ++++++++++++++++++ ++++ ",
" ++++++ ++++++++++++++++++ ++++ ",
" ++++++ ++++++++++++++++++ ++++ ",
" ++++++ ++++++++++++++++++ ++++ ",
" ++++++ ++++++++++++++++++ ++++ ",
" ++++++                    ++++ ",
" ++++++ OOOOOOOOOOOO XXXXX ++++ ",
" ++++++ OOOOOOOOOOOO XXXXX ++++ ",
" ++++++ OOOOOOOOOOOO XXXXX ++++ ",
" ++++++ OOOOOOOOOOOO XXXXX ++++ ",
" ++++++ OOOOOOOOOOOO XXXXX ++++ ",
" ++++++ OOOOOOOOOOOO XXXXX ++++ ",
"                                "};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

enum
{
    Minimal_Quit = 1,
    Minimal_GenericFileDlg,
    Minimal_About = wxID_ABOUT,
};

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size,
            long style = wxDEFAULT_FRAME_STYLE);

    virtual ~MyFrame() {}

    void OnQuit(wxCommandEvent& WXUNUSED(event)) { Close(true); }
    void OnAbout(wxCommandEvent& event);
    void OnGenericFileDlg( wxCommandEvent &event );

    void OnMenu(wxCommandEvent &event);
    void OnFileBrowser(wxFileBrowserEvent &event);

    wxTextCtrl *m_textCtrl;
    wxSplitterWindow *m_mainSplitter;
    wxSplitterWindow *m_fileBrowserSplitter;
    wxFileBrowser *m_fileBrowser1;
    wxFileBrowser *m_fileBrowser2;

private:
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

class MyApp : public wxApp
{
public:
    virtual bool OnInit()
    {
        MyFrame *frame = new MyFrame(_T("Minimal wxWidgets App"),
                                 wxPoint(50, 50), wxSize(450, 400));
        frame->Show(true);

        return true;
    }
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)
    EVT_MENU(Minimal_GenericFileDlg,  MyFrame::OnGenericFileDlg)

    EVT_MENU(-1, MyFrame::OnMenu)

    EVT_FILEBROWSER_FILE_SELECTED (-1, MyFrame::OnFileBrowser)
    EVT_FILEBROWSER_FILE_ACTIVATED(-1, MyFrame::OnFileBrowser)
    EVT_FILEBROWSER_DIR_SELECTED  (-1, MyFrame::OnFileBrowser)
    EVT_FILEBROWSER_DIR_ACTIVATED (-1, MyFrame::OnFileBrowser)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style)
       : wxFrame(NULL, -1, title, pos, size, style)
{
    SetIcon(wxICON(mondrian));

    wxMenu *menuFile = new wxMenu;
    menuFile->Append(Minimal_GenericFileDlg, wxT("Generic File Dialog..."));
    menuFile->AppendSeparator();
    menuFile->Append(Minimal_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, _T("&About...\tF1"), _T("Show about dialog"));

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    SetMenuBar(menuBar);

    // create a status bar just for fun
    CreateStatusBar(2);
    SetStatusText(_T("Welcome to wxWidgets!"));

    wxInitAllImageHandlers();

    m_mainSplitter = new wxSplitterWindow(this, -1, wxDefaultPosition, wxSize(500, 400), wxSP_3D);
    m_mainSplitter->SetMinimumPaneSize(20);

    m_textCtrl = new wxTextCtrl(m_mainSplitter, -1, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxVSCROLL|wxHSCROLL );

    m_fileBrowserSplitter = new wxSplitterWindow(m_mainSplitter, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D);
    m_fileBrowser1 = new wxFileBrowser(m_fileBrowserSplitter, -1, wxGetCwd(),
                                       wxDefaultPosition, wxDefaultSize,
                                       wxFILEBROWSER_DETAILS|wxNO_BORDER);
    m_fileBrowser2 = new wxFileBrowser(m_fileBrowserSplitter, -1, wxGetCwd());
    m_fileBrowserSplitter->SplitVertically(m_fileBrowser1, m_fileBrowser2, 200);

    m_mainSplitter->SplitHorizontally(m_fileBrowserSplitter, m_textCtrl, 300);
}

void MyFrame::OnFileBrowser(wxFileBrowserEvent &event)
{
    wxString text = wxString::Format(wxT("%s FileBrowser id %d, name %s"), wxNow().c_str(), event.GetId(), event.GetFilePath().c_str());

    wxEventType type = event.GetEventType();

    if      (type == wxEVT_FILEBROWSER_FILE_SELECTED)  text += wxT(" File Selected\n");
    else if (type == wxEVT_FILEBROWSER_FILE_ACTIVATED) text += wxT(" File Activated\n");
    else if (type == wxEVT_FILEBROWSER_DIR_SELECTED)   text += wxT(" Dir Selected\n");
    else if (type == wxEVT_FILEBROWSER_DIR_ACTIVATED)  text += wxT(" Dir Activated\n");

    m_textCtrl->AppendText(text);
    event.Skip();
}

void MyFrame::OnMenu(wxCommandEvent& WXUNUSED(event))
{

}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("This is the About dialog of the wxfilebrowser sample.\n")
                _T("Written by John Labenski.\n")
                _T("Compiled with %s"), wxVERSION_STRING);

    wxMessageBox(msg, _T("About wxFileBrowser"), wxOK | wxICON_INFORMATION, this);
}

//#include <wx/log.h>

void MyFrame::OnGenericFileDlg( wxCommandEvent &WXUNUSED(event) )
{
    //wxLogStderr log(stdout);
    //log.AddTraceMask(_T("mimetest"));
    //wxLog *oldLog;
    //oldLog = wxLog::SetActiveTarget(&log);

    wxGenericFileDialog fileDialog(this, wxT("Select a file"));

    //log.Flush();
    //wxLog::SetActiveTarget(oldLog);

    wxString path;
    if ( fileDialog.ShowModal() == wxID_OK )
    {
        path = fileDialog.GetPath();
        m_textCtrl->AppendText(wxString::Format(wxT("%s Path selected [%s]\n"), wxNow().c_str(), path.c_str()));
    }
}

/////////////////////////////////////////////////////////////////////////////
// Name:        wizard.cpp
// Purpose:     wxWindows sample demonstrating wxWizard control
// Author:      Vadim Zeitlin
// Modified by:
// Created:     15.08.99
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma implementation "wizard.cpp"
    #pragma interface "wizard.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "AspellDictionaryDownloader.h"
#include "DictionaryWizard.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// ids for menu items
enum
{
    Wizard_Quit = 100,
    Wizard_Run,
};

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    virtual bool OnInit();
};

class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnRunWizard(wxCommandEvent& event);
    void OnWizardCancel(wxWizardEvent& event);

private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};


// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// event tables and such
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Wizard_Quit,  MyFrame::OnQuit)
    EVT_MENU(Wizard_Run,   MyFrame::OnRunWizard)

    EVT_WIZARD_CANCEL(-1, MyFrame::OnWizardCancel)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// `Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame(_T("wxWizard Sample"));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(TRUE);

    // we're done
    return TRUE;
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

MyFrame::MyFrame(const wxString& title)
       : wxFrame((wxFrame *)NULL, -1, title,
                  wxDefaultPosition, wxSize(250, 150))  // small frame
{
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(Wizard_Run, _T("&Run wizard...\tCtrl-R"));
    menuFile->AppendSeparator();
    menuFile->Append(Wizard_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    // also create status bar which we use in OnWizardCancel
    CreateStatusBar();
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void MyFrame::OnRunWizard(wxCommandEvent& WXUNUSED(event))
{
  EngineDictionaryDownloader* pDownloader = new AspellDictionaryDownloader();
  if (pDownloader)
  {
    DictionaryWizard* pWizard = new DictionaryWizard(this);
    pWizard->SetEngineDownloader(pDownloader);
    pWizard->Run();
    pWizard->SetEngineDownloader(NULL);
    pWizard->Destroy();
  
    delete pDownloader;
  }
}

void MyFrame::OnWizardCancel(wxWizardEvent& WXUNUSED(event))
{
    wxLogStatus(this, wxT("The wizard was cancelled."));
}

/***************************************************************
 * Name:      CodeSnippetsApp.cpp
 * Purpose:   Code for Application Class
 * Author:    pecan ()
 * Created:   2007-03-18
 * Copyright: pecan ()
 * License:
 **************************************************************/

#ifdef WX_PRECOMP //
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include <wx/stdpaths.h>

#include "version.h"
#include "codesnippetsapp.h"
#include "codesnippetswindow.h"
#include "snippetsconfig.h"
#include "snippetsimages.h"
#include "messagebox.h"

IMPLEMENT_APP(CodeSnippetsApp);

BEGIN_EVENT_TABLE(CodeSnippetsApp, wxApp)
    // ---
    //-EVT_ACTIVATE_APP(        CodeSnippetsApp::OnActivateApp)
    //-EVT_ACTIVATE(               CodeSnippetsApp::OnActivate)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
bool CodeSnippetsApp::OnInit()
// ----------------------------------------------------------------------------
{
    // Initialize the one and only global
    g_pConfig = new CodeSnippetsConfig;

	CodeSnippetsAppFrame* frame = new CodeSnippetsAppFrame(0L, _("CodeSnippets"));
	frame->Show();

	return true;
}
//// ----------------------------------------------------------------------------
//void CodeSnippetsApp::OnActivate(wxActivateEvent& event)
//// ----------------------------------------------------------------------------
//{
//    // unused
//     LOGIT( _T("App OnActivate") );
//
//    event.Skip();
//    return;
//}
// ----------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////


/***************************************************************
 * Name:      CodeSnippetsAppMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    pecan ()
 * Created:   2007-03-18
 * Copyright: pecan ()
 * License:
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__


//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

int idMenuFileOpen              = wxNewId();
int idMenuFileSave              = wxNewId();
int idMenuFileSaveAs            = wxNewId();
int idMenuQuit                  = wxNewId();
int idMenuSettingsOptions       = wxNewId();
int idMenuProperties            = wxNewId();
int idMenuAbout                 = wxNewId();
// ----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(CodeSnippetsAppFrame, wxFrame)
    EVT_MENU(idMenuFileOpen,        CodeSnippetsAppFrame::OnFileLoad)
    EVT_MENU(idMenuFileSave,        CodeSnippetsAppFrame::OnFileSave)
    EVT_MENU(idMenuFileSaveAs,      CodeSnippetsAppFrame::OnFileSaveAs)
    EVT_MENU(idMenuAbout,           CodeSnippetsAppFrame::OnAbout)
    EVT_MENU(idMenuQuit,            CodeSnippetsAppFrame::OnQuit)
    EVT_MENU(idMenuSettingsOptions, CodeSnippetsAppFrame::OnSettings)
    // ---
    EVT_ACTIVATE(                   CodeSnippetsAppFrame::OnActivate)
    // ---
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
CodeSnippetsAppFrame::CodeSnippetsAppFrame(wxFrame *frame, const wxString& title)
// ----------------------------------------------------------------------------
    : wxFrame(frame, -1, title)
{
    wxStandardPaths stdPaths;

    // initialize version and logging
    AppVersion* pVersion = new AppVersion;
    GetConfig()->pMainFrame = this;
    //-GetConfig()->AppName = wxTheApp->GetAppName();
    GetConfig()->AppName = wxT("codesnippets");
    #if LOGGING
     wxWindow* m_pAppWin = this;
     m_pLog = new wxLogWindow( m_pAppWin, _T(" CodeSnippets Log"),true,false);
     wxLog::SetActiveTarget( m_pLog);
     m_pLog->Flush();
     m_pLog->GetFrame()->SetSize(20,20,600,300);
     LOGIT( _T("CodeSnippets App Logging Started[%s]"),pVersion->GetVersion().c_str());
    #endif
    LOGIT(wxT("AppName is[%s]"),GetConfig()->AppName.c_str());

#if wxUSE_MENUS
        // create a menu bar
    wxMenuBar* mbar = new wxMenuBar();
    wxMenu* fileMenu = new wxMenu(_T(""));
    fileMenu->Append(idMenuFileOpen, _("&Load File...\tCtrl-O"), _("Load Snippets"));
    fileMenu->Append(idMenuFileSave, _("&Save\tCtrl-S"), _("Save Snippets"));
    fileMenu->Append(idMenuFileSaveAs, _("Save &As..."), _("Save Snippets As..."));
        fileMenu->Append(idMenuQuit, _("&Quit\tCtrl-Q"), _("Quit the application"));
    mbar->Append(fileMenu, _("&File"));

        // Settings menu
    wxMenu* settingsMenu = new wxMenu(_T(""));
    settingsMenu->Append(idMenuSettingsOptions, _("Options..."), _("Configuration Options"));
    //settingsMenu->Append(idMenuSettingsSave, _("Save"), _("Save Settings"));
    mbar->Append(settingsMenu, _("Settings"));
        // About menu item
    wxMenu* helpMenu = new wxMenu(_T(""));
    helpMenu->Append(idMenuAbout, _("&About\tF1"), _("Show info about this application"));
    mbar->Append(helpMenu, _("&Help"));

    SetMenuBar(mbar);
#endif // wxUSE_MENUS

    // Create filename like codesnippets.ini
    //memorize the key file name as {%HOME%}\codesnippets.ini
    wxString m_ConfigFolder = stdPaths.GetUserDataDir();
    wxString m_ExecuteFolder = stdPaths.GetDataDir();

    //GTK GetConfigFolder is returning double "//", eg, "/home/pecan//.codeblocks"
    // remove the double //s from filename //+v0.4.11
    m_ConfigFolder.Replace(_T("//"),_T("/"));
    m_ExecuteFolder.Replace(_T("//"),_T("/"));
    LOGIT(wxT("CfgFolder[%s]"),m_ConfigFolder.c_str());
    LOGIT(wxT("ExecFolder[%s]"),m_ExecuteFolder.c_str());

    // Find the config file
    wxString cfgFilenameStr;
    do{
        // if codesnippets.ini is in the executable folder, use it
        cfgFilenameStr = m_ExecuteFolder + wxFILE_SEP_PATH + GetConfig()->AppName + _T(".ini");
         LOGIT( _T("ExecName[%s]"),cfgFilenameStr.c_str() );
        if (::wxFileExists(cfgFilenameStr)) break;

        //if codeblocks has codesnippets.ini, use it
        cfgFilenameStr = m_ConfigFolder+wxFILE_SEP_PATH + GetConfig()->AppName + _T(".ini");
        cfgFilenameStr.Replace(wxT("codesnippets"), wxT("codeblocks"),false);
         LOGIT( _T("CodeblocksMSW[%s]"),cfgFilenameStr.c_str() );
        if (::wxFileExists(cfgFilenameStr) ) break;

        // if Linux has codesnippets.ini, use it
        cfgFilenameStr.Replace(wxT("codeblocks"),wxT(".codeblocks"));
        LOGIT( _T("CodeblocksUNX[%s]"),cfgFilenameStr.c_str() );
        if (::wxFileExists(cfgFilenameStr)) break;

        //use the default.conf folder
        cfgFilenameStr = m_ConfigFolder + wxFILE_SEP_PATH + GetConfig()->AppName + _T(".ini");
        LOGIT( _T("Default[%s]"),cfgFilenameStr.c_str() );
    }while(0);
    GetConfig()->SettingsSnippetsCfgFullPath = cfgFilenameStr;
     LOGIT( _T("SettingsSnippetsCfgFullPath[%s]"),GetConfig()->SettingsSnippetsCfgFullPath.c_str() );

    // Initialize Globals
    GetConfig()->SettingsLoad();

#if wxUSE_STATUSBAR
    // create a status bar with some information about the used wxWidgets version
    CreateStatusBar(1);
    wxString versionStr;
    versionStr = versionStr + wxT("CodeSnippets") + wxT(" ") + pVersion->GetVersion();
    SetStatusText( versionStr, 0);
    SetStatusText(wxbuildinfo(short_f), 1);
#endif // wxUSE_STATUSBAR

        // set the frame icon
    GetConfig()->pSnipImages = new SnipImages();
    SetIcon(GetConfig()->GetSnipImages()->GetSnipListIcon(TREE_IMAGE_ALL_SNIPPETS));

    // Create CodeSnippetsWindow with snippet tree
    GetConfig()->pMainFrame    = this;
    GetConfig()->pSnippetsWindow = new CodeSnippetsWindow(this);
    SetSize(GetConfig()->windowXpos, GetConfig()->windowYpos,
            GetConfig()->windowWidth, GetConfig()->windowHeight);

    buildInfo = wxbuildinfo(long_f);
    wxString
        pgmVersionString = wxT("CodeSnippets v") + pVersion->GetVersion();
    buildInfo = wxT("\t")+pgmVersionString + wxT("\n")+ wxT("\t")+buildInfo;
    buildInfo = buildInfo + wxT("\n\n\t")+wxT("Original Code by Arto Jonsson");
    buildInfo = buildInfo + wxT("\n\t")+wxT("Modified/Enhanced by Pecan Heber");

}

// ----------------------------------------------------------------------------
CodeSnippetsAppFrame::~CodeSnippetsAppFrame()
// ----------------------------------------------------------------------------
{
}

// ----------------------------------------------------------------------------
void CodeSnippetsAppFrame::OnClose(wxCloseEvent &event)
// ----------------------------------------------------------------------------
{
    Destroy();
}

// ----------------------------------------------------------------------------
void CodeSnippetsAppFrame::OnQuit(wxCommandEvent &event)
// ----------------------------------------------------------------------------
{
    Destroy();
}

// ----------------------------------------------------------------------------
void CodeSnippetsAppFrame::OnAbout(wxCommandEvent &event)
// ----------------------------------------------------------------------------
{
    GetSnippetsWindow()->ShowSnippetsAbout( buildInfo );

    //    SetActiveMenuId( event.GetId() );
    //
    //    //wxString msg = wxbuildinfo(long_f);
    //    wxString helpText;
    //    helpText << wxT(" Each Snippet item may specify either text or a File Link.\n")
    //             << wxT("\n")
    //             << wxT(" Snippets may be edited from within the context menu \n")
    //             << wxT("\n")
    //
    //             << wxT(" File Link snippets are created by dragging text to a new snippet, \n")
    //             << wxT(" then using the context menu to \"Convert to File Link\". \n")
    //             << wxT(" The data will be written to the specified file and the filename \n")
    //             << wxT(" will be placed in the snippets text area as a Link. \n")
    //             << wxT("\n")
    //
    //             << wxT(" Snippets are accessed by using the context menu \"Edit\" \n")
    //             << wxT(" or via the Properties context menu entry. \n")
    //             << wxT("\n")
    //
    //             << wxT(" Use the \"Settings\" menu to specify an external editor and \n")
    //             << wxT(" to specify a non-default Snippets index file. \n")
    //             << wxT("\n")
    //
    //             << wxT(" Both the text and file snippets may be dragged outward (Windows only)\n")
    //             << wxT(" or copied to the clipboard.\n")
    //             << wxT("\n")
    //
    //             << wxT(" For MS Windows:\n")
    //             << wxT(" Dragging a file snippet onto an external program window \n")
    //             << wxT(" will open the file. Dragging it into the edit area will \n")
    //             << wxT(" insert the text.\n");
    //
    //    messageBox(wxT("\n\n")+buildInfo+wxT("\n\n")+helpText, _("About"),wxOK, wxSIMPLE_BORDER);
    //
}
// ----------------------------------------------------------------------------
void CodeSnippetsAppFrame::OnSettings(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    SetActiveMenuId( event.GetId() );
    GetSnippetsWindow()->OnMnuSettings(event);
}
// ----------------------------------------------------------------------------
void CodeSnippetsAppFrame::OnFileLoad(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // Load main xml file from user specified filename
    SetActiveMenuId( event.GetId() );

    // Save any previously modified file
    if ( GetFileChanged() )
    {    // Ask users if they want to save the snippet xml file
        int answer = messageBox( wxT("Save SnipList file?\n")+GetConfig()->SettingsSnippetsXmlFullPath,
                                                wxT("Open"),wxYES_NO );
        if ( answer == wxYES)
        {
            // Save the snippets
            //SaveSnippetsToFile( GetConfig()->SettingsSnippetsXmlFullPath );
            OnFileSave( event );
        }
    }//fi

    GetConfig()->pSnippetsWindow->OnMnuLoadSnippetsFromFile( event);
    return;
}
// ----------------------------------------------------------------------------
void CodeSnippetsAppFrame::OnFileSave(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // save main xml file to fileName configured in Settings
    SetActiveMenuId( event.GetId() );

    #ifdef LOGGING
     LOGIT( _T("Saving XML file[%s]"), GetConfig()->SettingsSnippetsXmlFullPath.GetData() );
    #endif //LOGGING

    GetSnippetsWindow()->OnMnuSaveSnippets( event );

}

// ----------------------------------------------------------------------------
void CodeSnippetsAppFrame::OnFileSaveAs(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // save the main xml file to another file name

    SetActiveMenuId( event.GetId() );

//    wxString newFileName = wxEmptyString;
//    // Ask user for filename
//    wxFileDialog dlg(this,                      //parent  window
//                 _("Save as"),                  //message
//                 wxEmptyString,                 //default directory
//                 newFileName,                   //default file
//                 wxT("*.*"),                    //wildcards
//                 wxSAVE | wxOVERWRITE_PROMPT);  //style
//    // move dialog into the parents frame space
//    wxPoint mousePosn = ::wxGetMousePosition();
//    (&dlg)->Move(mousePosn.x, mousePosn.y);
//    if (dlg.ShowModal() != wxID_OK) return;
//
//    newFileName = dlg.GetPath();
//
//    #ifdef LOGGING
//     LOGIT( _T("Save As filename[%s]"), newFileName.GetData() );
//    #endif //LOGGING;
//
//    if ( newFileName.IsEmpty() ) return;
//
//    pSnippetsWindow->SaveSnippetsToFile(newFileName);

    GetConfig()->pSnippetsWindow->OnMnuSaveSnippetsAs( event );
    event.Skip();
    return;
}
// ----------------------------------------------------------------------------
void CodeSnippetsAppFrame::OnActivate(wxActivateEvent& event)
// ----------------------------------------------------------------------------
{
    // Application/Codeblocks has been activated

    // Check that it's us that got activated
     if (not event.GetActive()) { event.Skip();return; }

    //- LOGIT( _T(" OnActivate is CodeSnippet") );
        CodeSnippetsWindow* p = GetConfig()->pSnippetsWindow;
        p->CheckForExternallyModifiedFiles();

    event.Skip();
    return;
}
// ----------------------------------------------------------------------------

/***************************************************************
 * Name:      CodeSnippetsApp.cpp
 * Purpose:   Code for Application Class
 * Author:    pecan ()
 * Created:   2007-03-18
 * Copyright: pecan ()
 * License:
 **************************************************************/
/*
	This file is part of Code Snippets, a plugin for Code::Blocks
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
// RCS-ID: $Id: codesnippetsapp.cpp 60 2007-04-23 00:02:55Z Pecan $

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

//  missing mingw header definitions
    #define MAPVK_VK_TO_VSC     0

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
	if (GetConfig()->m_sWindowHandle.IsEmpty() ) return false;
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
int idMenuFileBackup            = wxNewId();
int idMenuQuit                  = wxNewId();
int idMenuSettingsOptions       = wxNewId();
int idMenuProperties            = wxNewId();
int idMenuAbout                 = wxNewId();
// ----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(CodeSnippetsAppFrame, wxFrame)
    EVT_MENU(idMenuFileOpen,        CodeSnippetsAppFrame::OnFileLoad)
    EVT_MENU(idMenuFileSave,        CodeSnippetsAppFrame::OnFileSave)
    EVT_MENU(idMenuFileSaveAs,      CodeSnippetsAppFrame::OnFileSaveAs)
    EVT_MENU(idMenuFileBackup,      CodeSnippetsAppFrame::OnFileBackup)
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
    GetConfig()->pMainFrame    = 0;
    GetConfig()->pSnippetsWindow = 0;
    m_bOnActivateBusy = 0;

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


    // Create filename like codesnippets.ini
    //memorize the key file name as {%HOME%}\codesnippets.ini
    wxString m_ConfigFolder = stdPaths.GetUserDataDir();
    //-wxString m_ExecuteFolder = stdPaths.GetDataDir(); Incorrect report on Linux
    wxString m_ExecuteFolder = FindAppPath(wxTheApp->argv[0], ::wxGetCwd(), wxEmptyString);

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
         LOGIT( _T("ExecCfgName[%s]"),cfgFilenameStr.c_str() );
        if (::wxFileExists(cfgFilenameStr)) break;

        //if codeblocks has codesnippets.ini, use it
        cfgFilenameStr = m_ConfigFolder+wxFILE_SEP_PATH + GetConfig()->AppName + _T(".ini");
         LOGIT( _T("MSWCfgName 1[%s]"),cfgFilenameStr.c_str() );
        cfgFilenameStr = cfgFilenameStr.Lower();
         LOGIT( _T("MSWCfgName 2[%s]"),cfgFilenameStr.c_str() );
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

    GetConfig()->SettingsSnippetsCfgFullPath = cfgFilenameStr;
     LOGIT( _T("SettingsSnippetsCfgFullPath[%s]"),GetConfig()->SettingsSnippetsCfgFullPath.c_str() );

    // ---------------------
    // Initialize Globals
    // ---------------------
    GetConfig()->SettingsLoad();

    // -----------------------------------------
    // Check for pgm instance already running
    // -----------------------------------------
    const wxString name = wxString::Format(wxT("CodeSnippets-%s"), wxGetUserId().c_str());
    m_checker = new wxSingleInstanceChecker(name);

   #if defined(__WXMSW__)
    if ( m_checker->IsAnotherRunning() )
    {   // Previous instance is running.
        // Minimize then restore the first instance so pgm appears on active screen
        // Get the first instance handle of the window from the config file
        HWND pFirstInstance;
        //-cfgFile.Read( wxT("WindowHandle"),  &windowHandle ) ;
        unsigned long val;
        if ( GetConfig()->m_sWindowHandle.ToULong( &val, 16) )
        pFirstInstance = (HWND)val;
        if (pFirstInstance && ::IsWindow(pFirstInstance) )
        {
            //wxMessageBox(wxT("CodeSnippets is already running."), name);
            SwitchToThisWindow( pFirstInstance, true );
            //-::ShowWindow(pFirstInstance,SW_FORCEMINIMIZE);  //minimize the window
            //-::ShowWindow(pFirstInstance,SW_RESTORE);        //restore the window
            //-::BringWindowToTop(pFirstInstance);
        }
        // Tell app class we're terminating
        GetConfig()->m_sWindowHandle = wxEmptyString;
        return ;
    }//fi m_checker
   #endif
    // This is first instance of program
    // write the window handle to the config file for other instance loads
    GetConfig()->m_sWindowHandle = wxString::Format( wxT("%p"),this->GetHandle());
    GetConfig()->SettingsSaveString(wxT("WindowHandle"), GetConfig()->m_sWindowHandle);

#if wxUSE_MENUS
        // create a menu bar
    wxMenuBar* mbar = new wxMenuBar();
    wxMenu* fileMenu = new wxMenu(_T(""));
    fileMenu->Append(idMenuFileOpen, _("&Load Index...\tCtrl-O"), _("Load Snippets"));
    fileMenu->Append(idMenuFileSave, _("&Save Index\tCtrl-S"), _("Save Snippets"));
    fileMenu->Append(idMenuFileSaveAs, _("Save Index &As..."), _("Save Snippets As..."));
    fileMenu->Append(idMenuFileBackup, _("Backup Index "), _("Backup Snippets Index"));
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

    // ----------------------------
    // create window
    // -----------------------------
    // Create CodeSnippetsWindow with snippet tree
    GetConfig()->pMainFrame    = this;
    GetConfig()->pSnippetsWindow = new CodeSnippetsWindow(this);
    // dont allow window to disappear
    if ( GetConfig()->windowWidth<20 ) {GetConfig()->windowWidth = 100;}
    if ( GetConfig()->windowHeight<40 ) {GetConfig()->windowHeight = 200;}
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
     // Don't close down if file checking is active
    if (m_bOnActivateBusy)
        return;
    Destroy();
}

// ----------------------------------------------------------------------------
void CodeSnippetsAppFrame::OnQuit(wxCommandEvent &event)
// ----------------------------------------------------------------------------
{
    // Don't close down if file checking is active
   if (m_bOnActivateBusy)
        return;
    Destroy();
}

// ----------------------------------------------------------------------------
void CodeSnippetsAppFrame::OnAbout(wxCommandEvent &event)
// ----------------------------------------------------------------------------
{
    GetSnippetsWindow()->ShowSnippetsAbout( buildInfo );

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
        int answer = messageBox( wxT("Save Snippets file?\n")+GetConfig()->SettingsSnippetsXmlFullPath,
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

    GetConfig()->GetSnippetsWindow()->OnMnuSaveSnippetsAs( event );
    event.Skip();
    return;
}
// ----------------------------------------------------------------------------
void CodeSnippetsAppFrame::OnFileBackup(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // backup the main xml file to file name with a sequential extension number

    SetActiveMenuId( event.GetId() );

    GetConfig()->GetSnippetsWindow()->OnMnuFileBackup( event );
    event.Skip();
    return;
}
// ----------------------------------------------------------------------------
void CodeSnippetsAppFrame::OnActivate(wxActivateEvent& event)
// ----------------------------------------------------------------------------
{
    // Application/Codeblocks has been activated

    if ( m_bOnActivateBusy ) {event.Skip();return;}
    ++m_bOnActivateBusy;
    do{
        // Check that it's us that got activated
         if (not event.GetActive()) break;

         // Check that CodeSnippets actually has a file open
        if (not GetConfig()->GetSnippetsWindow() )  break;
        if (not GetConfig()->GetSnippetsTreeCtrl() ) break;

            CodeSnippetsWindow* p = GetConfig()->pSnippetsWindow;
            if (not p) break;
            p->CheckForExternallyModifiedFiles();
    }while(0);

    m_bOnActivateBusy = 0;
    event.Skip();
    return;
}
// ----------------------------------------------------------------------------
wxString CodeSnippetsAppFrame::FindAppPath(const wxString& argv0, const wxString& cwd, const wxString& appVariableName)
// ----------------------------------------------------------------------------
{
    // Find the absolute path from where this application has been run.
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

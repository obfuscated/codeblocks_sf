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
// RCS-ID: $Id$

#ifdef WX_PRECOMP //
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include <wx/stdpaths.h>
#include <wx/process.h>
#include <wx/filename.h>
#include <wx/filesys.h>
#include <wx/fs_zip.h>
#include <wx/fs_mem.h>
#include <wx/xrc/xmlres.h>
#include <wx/dynlib.h>
#include <wx/dir.h>

#if defined(__APPLE__) && defined(__MACH__)
#include <sys/param.h>
#include <mach-o/dyld.h>
#endif

#include "prep.h"
#include "xtra_res.h"
#include "configmanager.h"
#include "seditormanager.h"

#include "version.h"
#include "codesnippetsapp.h"
#include "codesnippetswindow.h"
#include "snippetsconfig.h"
#include "snippetsimages.h"
#include "GenericMessageBox.h"
#include "codesnippetsevent.h"
#include "FileImport.h"
#include "dragscroll.h"
#include "dragscrollevent.h"

//#include "../Utils/ToolBox/ToolBox.h" //debugging

#ifndef APP_PREFIX
#define APP_PREFIX ""
#endif

#ifndef __WXMAC__
wxString GetResourcesDir(){ return wxEmptyString; };
#endif

// The app needs a flag to disable some plugin calls
////#if defined(BUILDING_PLUGIN)
////    #error preprocessor BUILDING_PLUGIN flag must *not* be defined for this target
////#endif


//  missing mingw header definitions
    #define MAPVK_VK_TO_VSC     0

IMPLEMENT_APP(CodeSnippetsApp);

int fileOpenRecentFilesSubMenu =        wxNewId();
int idFileOpenRecentFileClearHistory =  wxNewId();
int idFileOpenRecentFile =              wxNewId();

BEGIN_EVENT_TABLE(CodeSnippetsApp, wxApp)
    // --- See below for CodeSnippetsAppFrame events ---
END_EVENT_TABLE()

#ifdef __WXMAC__
    #if wxCHECK_VERSION(2,9,0)
        #include "wx/osx/core/cfstring.h"
    #else
        #include "wx/mac/corefoundation/cfstring.h"
    #endif
    #include "wx/intl.h"

    #include <CoreFoundation/CFBundle.h>
    #include <CoreFoundation/CFURL.h>

    // returns e.g. "/Applications/appname.app/Contents/Resources" if application is bundled,
    // or the directory of the binary, e.g. "/usr/local/bin/appname", if it is *not* bundled.
    // ----------------------------------------------------------------------------
    static wxString GetResourcesDir()
    // ----------------------------------------------------------------------------
    {
        CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(CFBundleGetMainBundle());
        CFURLRef absoluteURL = CFURLCopyAbsoluteURL(resourcesURL); // relative -> absolute
        CFRelease(resourcesURL);
        CFStringRef cfStrPath = CFURLCopyFileSystemPath(absoluteURL,kCFURLPOSIXPathStyle);
        CFRelease(absoluteURL);
        #if wxCHECK_VERSION(2,9,0)
          return wxCFStringRef(cfStrPath).AsString(wxLocale::GetSystemEncoding());
        #else
          return wxMacCFStringHolder(cfStrPath).AsString(wxLocale::GetSystemEncoding());
        #endif
    }
#endif

// ----------------------------------------------------------------------------
bool CodeSnippetsApp::OnInit()
// ----------------------------------------------------------------------------
{

    //stop wxMessageBox's when running release version
    //wxLog::EnableLogging(false);
    wxLog::EnableLogging(true);

    // Initialize the one and only global
    // Must be done first to allocate config file
    //-g_pConfig = new CodeSnippetsConfig;
    SetConfig( new CodeSnippetsConfig );
    GetConfig()->m_bIsPlugin = false;

	CodeSnippetsAppFrame* frame = new CodeSnippetsAppFrame(0L, _("CodeSnippets"));

	// Return without show if this is *not* first instance of pgm
	// The instance address was cleared by CodeSnippetsApp to show
	// that this is not the first instance.
	if (GetConfig()->m_sWindowHandle.IsEmpty() ) return false;
	if (frame->GetInitXRCResult() == false) return false;

	frame->Show();
	return true;
}
// ----------------------------------------------------------------------------
int CodeSnippetsApp::OnExit()
// ----------------------------------------------------------------------------
{
    return 0;
}

/***************************************************************
 * Name:      CodeSnippetsAppFrame.cpp
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


// ----------------------------------------------------------------------------
//helper functions
// ----------------------------------------------------------------------------
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
// dummy definition of idViewSnippets because it exists in the plugin
// but not in the app. But it's referenced in the app anyway because the CB
// linux makefile does not allow preprocessor definitions to get rid of it
int idViewSnippets = wxNewId();

int idMenuFileOpen              = wxNewId();
int idMenuFileSave              = wxNewId();
int idMenuFileSaveAs            = wxNewId();
int idMenuFileBackup            = wxNewId();
int idMenuQuit                  = wxNewId();
int idMenuSettingsOptions       = wxNewId();
//-int idMenuProperties            = wxNewId();
//-extern int idMenuProperties;
int idMenuAbout                 = wxNewId();
int idMenuTest                  = wxNewId();
// ----------------------------------------------------------------------------
//  Events table
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
    EVT_TIMER(-1,                   CodeSnippetsAppFrame::OnTimerAlarm)
    // ---
    EVT_ACTIVATE(                   CodeSnippetsAppFrame::OnActivate)
    EVT_CLOSE(                      CodeSnippetsAppFrame::OnClose)
    EVT_IDLE(                       CodeSnippetsAppFrame::OnIdle)
    // -- recently used --
    EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9,      CodeSnippetsAppFrame::OnRecentFileReopen)
    EVT_MENU(idFileOpenRecentFileClearHistory,  CodeSnippetsAppFrame::OnRecentFileClearHistory)

    // -- Testing --
    #if defined(LOGGING)
    EVT_MENU(idMenuTest,        CodeSnippetsAppFrame::OnEventTest)
    #endif
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
CodeSnippetsAppFrame::CodeSnippetsAppFrame(wxFrame* frame, const wxString& title)
// ----------------------------------------------------------------------------
    : wxFrame(frame, -1, title)
      ,m_Timer(this,0)
{
      frame = this;
      InitCodeSnippetsAppFrame( frame, title); //allow us to gdb break
}//ctor
// ----------------------------------------------------------------------------
CodeSnippetsAppFrame::~CodeSnippetsAppFrame()
// ----------------------------------------------------------------------------
{
}//dtor
// ----------------------------------------------------------------------------
void CodeSnippetsAppFrame::InitCodeSnippetsAppFrame(wxFrame *frame, const wxString& title)
// ----------------------------------------------------------------------------
{
    // Constructor code:
    // This routine separates this code from the constructor so that
    // the gdb debugging breakpoints are honored

    GetConfig()->pMainFrame      = 0;
    GetConfig()->pSnippetsWindow = 0;
    GetConfig()->m_appIsShutdown = 0;
    GetConfig()->m_appIsDisabled = 0;
    m_bOnActivateBusy = 0;
    GetConfig()->SetKeepAlivePid(0);
    GetConfig()->SetAppParent(wxEmptyString);
    m_KeepAliveFileName = wxEmptyString;
    m_pFilesHistory = 0;


    // -------------------------------
    // initialize version and logging
    // -------------------------------
    AppVersion* pVersion = new AppVersion;
    GetConfig()->pMainFrame = this;
    GetConfig()->AppName = wxT("codesnippets");

    #if LOGGING
     wxWindow* m_pAppWin = this;
     wxLog::EnableLogging(true);
     m_pLog = new wxLogWindow( m_pAppWin, _T(" CodeSnippets Log"),true,false);
     wxLog::SetActiveTarget( m_pLog);
     m_pLog->Flush();
     m_pLog->GetFrame()->SetSize(20,20,600,300);
     LOGIT( _T("CodeSnippets App Logging Started[%s]"),pVersion->GetVersion().c_str());
    #endif

    #if defined(LOGGING)
    LOGIT(wxT("AppName is[%s]"),GetConfig()->AppName.c_str());
    #endif

    // If invoked from CodeBlocks? (as opposed to being standalone)
    // there'll be a "--KeepAlivePid=<pid>" argument. This is the CB pid which
    // has been written to /temp/cbsnippetspid<pid>.plg keepAlive file.
    // When CB wants us to terminate, the file will disappear.
    if ( wxTheApp->argc >1 ) do
    {
        wxString keepAliveArg = wxTheApp->argv[1];
        if ( not keepAliveArg.Contains(wxT("KeepAlivePid")) ) break;
        wxString keepAlivePid = keepAliveArg.AfterLast('=');
        long lKeepAlivePid = 0;
        keepAlivePid.ToLong(&lKeepAlivePid);
        GetConfig()->SetKeepAlivePid( lKeepAlivePid );
        #if defined(LOGGING)
        LOGIT( _T("App: KeepAlivePid is [%lu]"), GetConfig()->GetKeepAlivePid() );
        #endif
    }while(false); //if KeepAlivePid argument

    // If invoked from CodeBlocks? (as opposed to being standalone)
    // there'll be a "--AppParent=<string>" argument.
    if ( wxTheApp->argc >2 ) do
    {
        wxString appParentArg = wxTheApp->argv[2];
        if ( not appParentArg.Contains(wxT("AppParent")) ) break;
        wxString appParent = appParentArg.AfterLast('=');
        GetConfig()->SetAppParent( appParent );
        #if defined(LOGGING)
        LOGIT( _T("App: AppParent is [%s]"), GetConfig()->GetAppParent().c_str() );
        #endif
    }while(false); //if AppParent argument

    // -----------------------------------------
    // Find Config File
    // -----------------------------------------
    // Create filename like {%HOME%}\codesnippets.ini
    m_ConfigFolder = Normalize(wxStandardPaths::Get().GetUserDataDir());
    wxString m_ExecuteFolder = Normalize(FindAppPath(wxTheApp->argv[0], ::wxGetCwd(), wxEmptyString));

    #if defined(LOGGING)
    LOGIT(wxT("CfgFolder[%s]"),m_ConfigFolder.c_str());
    LOGIT(wxT("ExecFolder[%s]"),m_ExecuteFolder.c_str());
    #endif

    // ----------------------------------------
    // Find the codesnippets .ini config file
    // ----------------------------------------
    wxString iniFilenameStr;
    do{
        // if codesnippets.ini is in the executable folder, use it
        iniFilenameStr = m_ExecuteFolder + wxFILE_SEP_PATH + GetConfig()->AppName + _T(".ini");
        if (::wxFileExists(iniFilenameStr))
        {
            m_ConfigFolder = m_ExecuteFolder;
            GetConfig()->SettingsSnippetsCfgPath = m_ExecuteFolder;
            break;
        }
        // if default.conf exists in exec folder, use it as config folder
        wxString defaultconfStr =  m_ExecuteFolder + wxFILE_SEP_PATH + _T("default.conf");
        if (::wxFileExists(defaultconfStr))
        {
            m_ConfigFolder = m_ExecuteFolder;
            GetConfig()->SettingsCBConfigPath = m_ExecuteFolder;
            break;
        }

        //if launched from CB & CodeBlocks has codesnippets.ini, use it
        iniFilenameStr = m_ConfigFolder + wxFILE_SEP_PATH + GetConfig()->AppName + _T(".ini");
        iniFilenameStr = iniFilenameStr.Lower();
        //-iniFilenameStr.Replace(wxT("codesnippets"), wxT("codeblocks"),false);
        iniFilenameStr.Replace(wxT("codesnippets"), GetConfig()->GetAppParent(),false);
        if ( GetConfig()->GetKeepAlivePid() )
            if (::wxFileExists(iniFilenameStr) )
                break;

        // if launched from CB & Linux has codesnippets.ini, use it
        //-iniFilenameStr.Replace(wxT("codeblocks"),wxT(".codeblocks"));
        iniFilenameStr.Replace(GetConfig()->GetAppParent(), _T(".")+GetConfig()->GetAppParent());
        if ( GetConfig()->GetKeepAlivePid() )
            if (::wxFileExists(iniFilenameStr))
                break;

        //use the default app folder <user>/codesnippets/codesnippets.ini
        iniFilenameStr = m_ConfigFolder + wxFILE_SEP_PATH + GetConfig()->AppName + _T(".ini");
    }while(0);
    #if defined(LOGGING)
    LOGIT( _T("using .ini at[%s]"),iniFilenameStr.c_str() );
    #endif

    // ---------------------
    // Initialize Globals, read the .ini file
    // ---------------------
    GetConfig()->SettingsSnippetsCfgPath = iniFilenameStr;

    wxString defaultconfStr =  m_ExecuteFolder + wxFILE_SEP_PATH + _T("default.conf");
    if (::wxFileExists(defaultconfStr))
        GetConfig()->SettingsCBConfigPath = m_ExecuteFolder;

     #if defined(LOGGING)
     LOGIT( _T("SettingsSnippetsCfgPath[%s]"),GetConfig()->SettingsSnippetsCfgPath.c_str() );
     #endif

    // Read in the .ini file
    GetConfig()->SettingsLoad();

    // This is an application, not a plugin, force External mode
    if ( GetConfig()->GetSettingsWindowState() not_eq _T("External") )
    {
        GetConfig()->SetSettingsWindowState( _T("External") );
        GetConfig()->SettingsSaveString(wxT("WindowState"), _T("External") );
    }

    // Before initializing CB SDK make sure we have a usable local default.conf
    // in the users data directory along with the necessary resouce files in the .exe folder.
    ImportCBResources();

    #if defined(__WXMSW__)
        // -----------------------------------------
        // Check for pgm instance already running
        // -----------------------------------------
        const wxString name = wxString::Format(wxT("CodeSnippets-%s"), wxGetUserId().c_str());
        m_checker = new wxSingleInstanceChecker(name);

        if ( m_checker->IsAnotherRunning() ) do
        {   // Previous instance is running.
            // Minimize then restore the first instance so pgm appears on active screen
            // Got the first instance handle of the window from the config file
            HWND pFirstInstance;
            // gotten from cfgFile.Read( wxT("WindowHandle"),  &windowHandle ) ;
            #if defined(_WIN64) | defined(WIN64)
            size_t val;
            if ( GetConfig()->m_sWindowHandle.ToULongLong( &val, 16) )
            #else
            long unsigned int val;
            if ( GetConfig()->m_sWindowHandle.ToULong( &val, 16) )
            #endif
                pFirstInstance = (HWND)val;
            if (pFirstInstance && ::IsWindow(pFirstInstance) )
            {
                wxString msg = wxT("Another CodeSnippets is already running from this folder.\n");
                msg << _T("Starting multiple CodeSnippets could scramble this configuration file.\n");
                msg << _T("Run multiple CodeSnippets anyway?\n");
                int answer = GenericMessageBox( msg, _T("Multiple CodeSnippets"),wxYES_NO );
                if ( answer == wxYES)
                    break;

                // The following does not work well; esp. when the first session of
                // CodeSnippets is in another virtual screen. It doesnt show to the user
                // the first session.
                ////wxMessageBox(wxT("CodeSnippets is already running."), name);
                //// The following is necessary if the window is in another virtual screen
                //// but it unfortunately returns to the original virtual screen when the
                //// screen is next switched.
                //::ShowWindow(pFirstInstance,SW_FORCEMINIMIZE);  //minimize the window
                //::ShowWindow(pFirstInstance,SW_RESTORE);        //restore the window
                //::BringWindowToTop(pFirstInstance);
                //SwitchToThisWindow( pFirstInstance, true );

                // Tell app class we're terminating
                GetConfig()->m_sWindowHandle = wxEmptyString;
                return ;
            }
        }while(0);//fi m_checker
    #endif //WXMSW

    // This is first instance of program
    // write the window handle to the config file for other instance loads
    // The secondary pgm instances will switch to m_sWindowHandle, then terminate
    GetConfig()->m_sWindowHandle = wxString::Format( wxT("%p"),this->GetHandle());
    GetConfig()->SettingsSaveString(wxT("WindowHandle"), GetConfig()->m_sWindowHandle);

    // -------------------------------
    //  SDK initialization
    // -------------------------------
    if (not InitializeSDK() )
    {   // Got an error.Tell app class we're terminating
        GetConfig()->m_sWindowHandle = wxEmptyString;
        return;
    }

    // Correct the SDK LIE that .conf is always in the user app directory
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("app"));
    GetConfig()->SettingsCBConfigPath = cfg->GetConfigFolder();
    defaultconfStr =  m_ExecuteFolder + wxFILE_SEP_PATH + _T("default.conf");
    if (::wxFileExists(defaultconfStr))
        GetConfig()->SettingsCBConfigPath = m_ExecuteFolder;

    // -----------------
    // create a menu bar
    // -----------------
    wxMenuBar* mbar = new wxMenuBar();
    wxMenu* fileMenu = new wxMenu(_T(""));
    fileMenu->Append(idMenuFileOpen, _("&Load Index...\tCtrl-O"), _("Load Snippets"));
    fileMenu->Append(idMenuFileSave, _("&Save Index\tCtrl-S"), _("Save Snippets"));
    fileMenu->Append(idMenuFileSaveAs, _("Save Index &As..."), _("Save Snippets As..."));
    fileMenu->Append(idMenuFileBackup, _("Backup Index "), _("Backup Snippets Index"));

    wxMenu* fileOpenRecentFilesSubMenu = new wxMenu(wxT(""));
    fileOpenRecentFilesSubMenu->Append(idFileOpenRecentFileClearHistory, _("Clear History"), _("Clear Recent History"));
    fileMenu->Append(idFileOpenRecentFile, _("Recent History"), fileOpenRecentFilesSubMenu , _("Recent Files History"));
    fileMenu->Append(idMenuQuit, _("&Quit\tAlt-F4"), _("Quit the application"));
    mbar->Append(fileMenu, _("&File"));

        // Settings menu
    wxMenu* settingsMenu = new wxMenu(_T(""));
    settingsMenu->Append(idMenuSettingsOptions, _("Options..."), _("Configuration Options"));
    //settingsMenu->Append(idMenuSettingsSave, _("Save"), _("Save Settings"));
    #if defined(LOGGING)
    settingsMenu->Append(idMenuTest, _("Test"), _("Testing"));
    #endif
    mbar->Append(settingsMenu, _("Settings"));

        // About menu item
    wxMenu* helpMenu = new wxMenu(_T(""));
    helpMenu->Append(idMenuAbout, _("&About\tF1"), _("Show info about this application"));
    mbar->Append(helpMenu, _("&Help"));

    SetMenuBar(mbar);
    GetConfig()->m_pMenuBar = mbar;

    // Initialize recent files history
    InitializeRecentFilesHistory();

    // -------------------
    // Create Status Bar
    // -------------------
    // create a status bar with wxWidgets info
    CreateStatusBar(1);
    versionStr = versionStr + wxT("CodeSnippets") + wxT(" ") + pVersion->GetVersion();
    SetStatusText( versionStr, 0);
    SetStatusText(wxbuildinfo(short_f), 1);

        // set the frame icon
    GetConfig()->pSnipImages = new SnipImages();
    SetIcon(GetConfig()->GetSnipImages()->GetSnipListIcon(TREE_IMAGE_ALL_SNIPPETS));

    // ----------------------------
    // create window
    // -----------------------------
    // Create CodeSnippetsWindow with snippets tree
    GetConfig()->pMainFrame = this;
    GetConfig()->pSnippetsWindow = new CodeSnippetsWindow(this);
    // Reset the SDK window sizer to our window sizer
    wxBoxSizer* m_pMainSizer = new wxBoxSizer( wxVERTICAL );
    m_pMainSizer->Add(GetSnippetsWindow(), 1, wxEXPAND);
    SetSizer( m_pMainSizer );
    Layout();
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

    // -------------------------------------
    // Setup KeepAlive check
    // -------------------------------------
    // we may have been invoked with a parameter of KeepAlivePid=####
    // We terminate when the Pid no longer exists.
    if ( wxTheApp->argc >1 ) do
    {
        wxString keepAliveArg = wxTheApp->argv[1];
        if ( not keepAliveArg.Contains(wxT("KeepAlivePid")) ) break;
        wxString keepAlivePid = keepAliveArg.AfterLast('=');
        long lKeepAlivePid = 0;
        keepAlivePid.ToLong(&lKeepAlivePid);
        GetConfig()->SetKeepAlivePid( lKeepAlivePid );
         #if defined(LOGGING)
         LOGIT( _T("App: KeepAlivePid is [%lu]"), GetConfig()->GetKeepAlivePid() );
         #endif

        wxString tempDir = GetConfig()->GetTempDir();
        m_KeepAliveFileName = tempDir + wxT("/cbsnippetspid") +keepAlivePid+ wxT(".plg");
         #if defined(LOGGING)
         LOGIT( _T("KeepAlive FileName[%s]"),m_KeepAliveFileName.GetData() );
         #endif
        if ( not ::wxFileExists(m_KeepAliveFileName) )
        {
            GenericMessageBox(wxString::Format(wxT("Error: Did not find KeepAlive File[%s]"), m_KeepAliveFileName.GetData() ));
            m_KeepAliveFileName = wxEmptyString;
            break;
        }


        if ( GetConfig()->GetKeepAlivePid() )
        {    StartKeepAliveTimer( 2 );
             #if defined(LOGGING)
             LOGIT( _T("StartKeepAliveTimer for[%lu]"),GetConfig()->GetKeepAlivePid() );
             #endif
        }

    }while(0);

    // This App version of CodeSnippets has a local copy of DragScroll
    // for it's external editors
	InitializeDragScroll();

	// Add TreeCtrl to DragScroll managed windows
    sDragScrollEvent dsevt(wxEVT_S_DRAGSCROLL_EVENT , idDragScrollAddWindow);
    dsevt.SetEventObject(GetConfig()->GetSnippetsTreeCtrl());
    dsevt.SetString( GetConfig()->GetSnippetsTreeCtrl()->GetName() );
    GetConfig()->GetDragScrollEvtHandler()->AddPendingEvent( dsevt );

}//InitCodeSnippetsAppFrame

// ----------------------------------------------------------------------------
void CodeSnippetsAppFrame::InitializeDragScroll()
// ----------------------------------------------------------------------------
{
    // Initialize local DragScrolling module
    GetConfig()->SetDragScrollPlugin( 0 );
    GetConfig()->SetDragScrollPlugin( new cbDragScroll() );
    cbDragScroll* pds = GetConfig()->GetDragScrollPlugin();
    pds->cbDragScroll::m_IsAttached = true;
    pds->OnAttach();
    PushEventHandler( pds );
    pds->SetEvtHandlerEnabled( true );
    CodeBlocksEvent evt;
    pds->OnAppStartupDone(evt);

}//InitializeDragScroll
// ----------------------------------------------------------------------------
wxString CodeSnippetsAppFrame::FindAppPath(const wxString& argv0, const wxString& cwd, const wxString& appVariableName)
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

    wxString argv0Str = argv0;
    #if defined(__WXMSW__)
        do{
            if (argv0Str.Contains(wxT(".exe")) ) break;
            if (argv0Str.Contains(wxT(".bat")) ) break;
            if (argv0Str.Contains(wxT(".cmd")) ) break;
            argv0Str.Append(wxT(".exe"));
        }while(0);
    #endif

    if (wxIsAbsolutePath(argv0Str))
    {
        #if defined(LOGGING)
        LOGIT( _T("FindAppPath: AbsolutePath[%s]"), wxPathOnly(argv0Str).GetData() );
        #endif
        return wxPathOnly(argv0Str);
    }
    else
    {
        // Is it a relative path?
        wxString currentDir(cwd);
        if (currentDir.Last() != wxFILE_SEP_PATH)
            currentDir += wxFILE_SEP_PATH;

        str = currentDir + argv0Str;
        if (wxFileExists(str))
        {
            #if defined(LOGGING)
            LOGIT( _T("FindAppPath: RelativePath[%s]"), wxPathOnly(str).GetData() );
            #endif
            return wxPathOnly(str);
        }
    }

    // OK, it's neither an absolute path nor a relative path.
    // Search PATH.

    wxPathList pathList;
    pathList.AddEnvList(wxT("PATH"));
    str = pathList.FindAbsoluteValidPath(argv0Str);
    if (!str.IsEmpty())
    {
        #if defined(LOGGING)
        LOGIT( _T("FindAppPath: SearchPath[%s]"), wxPathOnly(str).GetData() );
        #endif
        return wxPathOnly(str);
    }

    // Failed
     #if defined(LOGGING)
     LOGIT(  _T("FindAppPath: Failed, returning cwd") );
     #endif
    return wxEmptyString;
    //return cwd;
}

// ----------------------------------------------------------------------------
void CodeSnippetsAppFrame::OnClose(wxCloseEvent &event)
// ----------------------------------------------------------------------------
{
     // Don't close down if file checking is active
    if (m_bOnActivateBusy)
        return;

    if ( GetConfig()->GetDragScrollPlugin() )
    {
        CodeBlocksEvent cbEvent;
        GetConfig()->GetDragScrollPlugin()->OnStartShutdown( cbEvent);
        GetConfig()->GetDragScrollPlugin()->OnRelease(true);
    }

    // EVT_CLOSE is never called for codesnippetswindow.
    // so we'll invoke it here. It saves the xml indexes.
    if (GetSnippetsWindow())
            if ( GetSnippetsWindow()->GetFileChanged() )
                GetSnippetsWindow()->GetSnippetsTreeCtrl()->SaveItemsToFile(GetConfig()->SettingsSnippetsXmlPath);

    // Save CodeSnippets.ini file
    //-GetConfig()->SetExternalPersistentOpen(false);
    GetConfig()->GetSnippetsWindow()->OnClose(event);

    // Make sure user cannot re-enable CodeSnippets until a restart
    GetConfig()->m_appIsShutdown = true;

    RemoveKeepAliveFile();
    // save recently opened indexes
    TerminateRecentFilesHistory();

    // If not started by CB, ask SDK to write config file
    if ( 0 == GetConfig()->GetKeepAlivePid() )
    if ( Manager::Get() )
    {
        //-Manager::Free(); dont clobber codeblocks' .conf file
        if (wxFileExists(m_ConfigFolder+_T("/default.conf.backup")) )
            wxRemoveFile(m_ConfigFolder+_T("/default.conf.backup")) ;
        if (wxFileExists(m_ConfigFolder+_T("/default.conf.cbTemp")) )
            wxRemoveFile(m_ConfigFolder+_T("/default.conf.cbTemp")) ;
    }

    Destroy();
}

// ----------------------------------------------------------------------------
void CodeSnippetsAppFrame::OnQuit(wxCommandEvent &event)
// ----------------------------------------------------------------------------
{
    // Don't close down if file checking is active
    if (m_bOnActivateBusy)
        return;
    wxCloseEvent evtClose;
    OnClose(evtClose);
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
        int answer = GenericMessageBox( wxT("Save Snippets file?\n\n")+GetConfig()->SettingsSnippetsXmlPath,
                                                wxT("Open"),wxYES_NO );
        if ( answer == wxYES)
        {
            // Save the snippets
            //SaveSnippetsToFile( GetConfig()->SettingsSnippetsXmlPath );
            OnFileSave( event );
        }
    }//fi

    GetConfig()->pSnippetsWindow->OnMnuLoadSnippetsFromFile( event);
    AddToRecentFilesHistory( GetConfig()->SettingsSnippetsXmlPath );
    return;
}
// ----------------------------------------------------------------------------
void CodeSnippetsAppFrame::OnFileSave(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // save main xml file to fileName configured in Settings
    SetActiveMenuId( event.GetId() );

    #ifdef LOGGING
     LOGIT( _T("Saving XML file[%s]"), GetConfig()->SettingsSnippetsXmlPath.GetData() );
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
    //-event.Skip(); Don't do this or we get double entry here
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
    // Application/CodeSnippets has been activated

    if ( m_bOnActivateBusy ) {event.Skip();return;}
    ++m_bOnActivateBusy;
    #if defined(LOGGING)
    //LOGIT( _T("CodeSnippetsAppFrame::OnActivate[%d]"), m_bOnActivateBusy);
    #endif
    do{ //only once
        // Check that it's us that got activated
         if (not event.GetActive()) break;

         // Check that CodeSnippets actually has a file open
        if (not GetConfig()->GetSnippetsWindow() )  break;
        if (not GetConfig()->GetSnippetsTreeCtrl() ) break;

        CodeSnippetsWindow* p = GetConfig()->GetSnippetsWindow();
        if (not p) break;
        p->CheckForExternallyModifiedFiles();

    }while(0);

    m_bOnActivateBusy = 0;
    event.Skip();
    return;
}
// ----------------------------------------------------------------------------
void CodeSnippetsAppFrame::OnTimerAlarm(wxTimerEvent& event)
// ----------------------------------------------------------------------------
{
    // Check the keepAlive file to see if CodeSnippets plugin
    // deleted it. If so, terminate

    // Check to see if our KeepAlive file still exixts
    if ( not ::wxFileExists(m_KeepAliveFileName) )
    {
        RemoveKeepAliveFile();
        wxCloseEvent evtClose(wxEVT_CLOSE_WINDOW,GetId());
        evtClose.SetEventObject(this);
        this->AddPendingEvent(evtClose);
        event.Skip();
        return;
    }
    // When this pgm is invoked by another pgm, we got a pid argument
    // if our creator pid is gone, terminate this pgm
    if ( GetConfig()->GetKeepAlivePid()  && (not wxProcess::Exists( GetConfig()->GetKeepAlivePid() )) )
    {
        RemoveKeepAliveFile();
        wxCloseEvent evtClose(wxEVT_CLOSE_WINDOW,GetId());
        evtClose.SetEventObject(this);
        this->AddPendingEvent(evtClose);
        event.Skip();
        return;
    }
    // our creator is still alive
    StartKeepAliveTimer( 1 );
}
// ----------------------------------------------------------------------------
void CodeSnippetsAppFrame::OnIdle(wxIdleEvent& event)
// ----------------------------------------------------------------------------
{
    if ( GetConfig()->m_appIsShutdown) { event.Skip(); return;}

    // when menu help clears the statusbar, put back the version string
    wxStatusBar* sb = GetStatusBar();
    if (not sb)
        {event.Skip(); return;}

    if (GetConfig()->m_appIsShutdown)
        { event.Skip(); return; }

    if ( sb->GetStatusText() == wxEmptyString )
    { sb->SetStatusText( versionStr);
    }

    // See if user changed from "external" to "docked" or "floating"
    // This happens when user changes the state in the settings dialog
    if ( GetConfig()->m_bWindowStateChanged )
    {
        // Don't close down if file checking is active
        if (m_bOnActivateBusy)
            {event.Skip(); return;}
        wxCloseEvent evtClose(wxEVT_CLOSE_WINDOW, GetId());
        //-OnClose(evtClose);
        evtClose.SetEventObject(this);
        this->AddPendingEvent( evtClose );
        GetConfig()->m_bWindowStateChanged = false;
    }

    CodeSnippetsTreeCtrl* pTree = GetConfig()->GetSnippetsTreeCtrl();
    if ( pTree ) pTree->OnIdle();

    event.Skip();return;
}
// ----------------------------------------------------------------------------
void CodeSnippetsAppFrame::OnEventTest(wxCommandEvent &event)
// ----------------------------------------------------------------------------
{
    #if defined(LOGGING)
    LOGIT( _T("CodeSnippetsAppFrame::OnEventTest()"));
    #endif
    ////    CodeSnippetsEvent evt(wxEVT_CODESNIPPETS_SELECT, 15);
    ////    evt.PostCodeSnippetsEvent(evt);
    ////
    ////    // Issue event for new snippets index file
    ////    CodeSnippetsEvent evt2( wxEVT_CODESNIPPETS_NEW_INDEX, 0);
    ////    evt2.SetSnippetString( GetConfig()->SettingsSnippetsXmlPath );
    ////    evt.PostCodeSnippetsEvent(evt2);
    ////
    ////    ToolBox toolbox;
    ////    toolbox.ShowWindowsAndEvtHandlers();

    sDragScrollEvent dsEvt(wxEVT_S_DRAGSCROLL_EVENT, idDragScrollRescan);
    dsEvt.SetEventObject( GetConfig()->GetSnippetsTreeCtrl());
    dsEvt.SetString( GetConfig()->GetSnippetsTreeCtrl()->GetName() );
    GetConfig()->GetDragScrollEvtHandler()->AddPendingEvent(dsEvt);

}
// ----------------------------------------------------------------------------
bool CodeSnippetsAppFrame::RemoveKeepAliveFile()
// ----------------------------------------------------------------------------
{
    bool result = ::wxRemoveFile( m_KeepAliveFileName );
    return result;
}
// ----------------------------------------------------------------------------
void CodeSnippetsAppFrame::InitializeRecentFilesHistory()
// ----------------------------------------------------------------------------
{
    TerminateRecentFilesHistory();

    wxMenuBar* mbar = GetMenuBar();
    if (!mbar)
        return;
    int pos = mbar->FindMenu(_("&File"));
    if (pos != wxNOT_FOUND)
    {
        m_pFilesHistory = new wxFileHistory(9, wxID_FILE1);

        wxMenu* menu = mbar->GetMenu(pos);
        if (!menu)
            return;
        wxMenu* recentFiles = 0;
        wxMenuItem* clear = menu->FindItem(idFileOpenRecentFileClearHistory, &recentFiles);
        if (recentFiles)
        {
            recentFiles->Remove(clear);

            wxFileConfig cfgFile(wxEmptyString,     // appname
                        wxEmptyString,      // vendor
                        GetConfig()->SettingsSnippetsCfgPath,     // local filename
                        wxEmptyString,      // global file
                        wxCONFIG_USE_LOCAL_FILE);

            m_pFilesHistory->Load( cfgFile );
            wxArrayString files;
            //int fknt = (int)m_pFilesHistory->GetCount();
            for (int i = 0; i < (int)m_pFilesHistory->GetCount(); ++i)
                files.Add(m_pFilesHistory->GetHistoryFile(i) ) ;

            for (int i = (int)files.GetCount() - 1; i >= 0; --i)
            {
                if(wxFileExists(files[i]))
                    m_pFilesHistory->AddFileToHistory(files[i]);
            }
            m_pFilesHistory->UseMenu(recentFiles);
            m_pFilesHistory->AddFilesToMenu(recentFiles);
            if (recentFiles->GetMenuItemCount())
                recentFiles->AppendSeparator();
            recentFiles->Append(clear);
        }
    }
}//InitializeRecentFilesHistory
// ----------------------------------------------------------------------------
void CodeSnippetsAppFrame::AddToRecentFilesHistory(const wxString& FileName)
// ----------------------------------------------------------------------------
{
    wxString filename = FileName;
#ifdef __WXMSW__
    // for windows, look for case-insensitive matches
    // if found, don't add it
    wxString low = filename.Lower();
    for (size_t i = 0; i < m_pFilesHistory->GetCount(); ++i)
    {
        if (low == m_pFilesHistory->GetHistoryFile(i).Lower())
        {    // it exists, set filename to the existing name, so it can become
            // the most recent one
            filename = m_pFilesHistory->GetHistoryFile(i);
            break;
        }
    }
#endif

    m_pFilesHistory->AddFileToHistory(filename);

    // because we append "clear history" menu to the end of the list,
    // each time we must add a history item we have to:
    // a) remove "Clear history"
    // b) clear the menu
    // c) fill it with the history items
    // and d) append "Clear history"...
    wxMenuBar* mbar = GetMenuBar();
    if (!mbar)
        return;
    int pos = mbar->FindMenu(_("&File"));
    if (pos == wxNOT_FOUND)
        return;
    wxMenu* menu = mbar->GetMenu(pos);
    if (!menu)
        return;
    wxMenu* recentFiles = 0;
    wxMenuItem* clear = menu->FindItem(idFileOpenRecentFileClearHistory, &recentFiles);
    if (clear && recentFiles)
    {
        // a)
        recentFiles->Remove(clear);
        // b)
        m_pFilesHistory->RemoveMenu(recentFiles);
        while (recentFiles->GetMenuItemCount())
            recentFiles->Delete(recentFiles->GetMenuItems()[0]);
        // c)
        m_pFilesHistory->UseMenu(recentFiles);
        m_pFilesHistory->AddFilesToMenu(recentFiles);
        // d)
        if (recentFiles->GetMenuItemCount())
            recentFiles->AppendSeparator();
        recentFiles->Append(clear);
    }
    //-b_RecentFilesModified = true;

}//AddToRecentFilesHistory
// ----------------------------------------------------------------------------
void CodeSnippetsAppFrame::TerminateRecentFilesHistory()
// ----------------------------------------------------------------------------
{
    if (m_pFilesHistory)
    {
        wxArrayString files;
        for (unsigned int i = 0; i < m_pFilesHistory->GetCount(); ++i)
            files.Add(m_pFilesHistory->GetHistoryFile(i));

     wxFileConfig cfgFile(wxEmptyString,     // appname
                        wxEmptyString,      // vendor
                        GetConfig()->SettingsSnippetsCfgPath,     // local filename
                        wxEmptyString,      // global file
                        wxCONFIG_USE_LOCAL_FILE);

        m_pFilesHistory->Save( cfgFile );
        cfgFile.Flush();

        wxMenuBar* mbar = GetMenuBar();
        if (mbar)
        {
            int pos = mbar->FindMenu(_("&File"));
            if (pos != wxNOT_FOUND)
            {
                wxMenu* menu = mbar->GetMenu(pos);
                if (menu)
                {
                    wxMenu* recentFiles = 0;
                    menu->FindItem(idFileOpenRecentFileClearHistory, &recentFiles);
                    if (recentFiles)
                        m_pFilesHistory->RemoveMenu(recentFiles);
                }
            }
        }
        delete m_pFilesHistory;
        m_pFilesHistory = 0;
    }
    //-b_RecentFilesModified = false;
}//TerminateRecentFilesHistory
// ----------------------------------------------------------------------------
void CodeSnippetsAppFrame::OnRecentFileReopen(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    size_t id = event.GetId() - wxID_FILE1;
    wxString fname = m_pFilesHistory->GetHistoryFile(id);
////    if (!OpenGeneric(fname, true))
////    {
////        AskToRemoveFileFromHistory(m_pFilesHistory, id);
////    }

    // save any changes
    if (GetFileChanged() ) OnFileSave(event);

    // load specified recent xml index
    if (::wxFileExists(fname))
    {
        GetConfig()->SettingsSnippetsXmlPath = fname;
        GetSnippetsWindow()->GetSnippetsTreeCtrl()->LoadItemsFromFile( fname, /*appending=*/false);
        //-GetSnippetsWindow()->GetSnippetsTreeCtrl()->SetFileChanged(false);
        //-GetSnippetsWindow()->GetSnippetsTreeCtrl()->FetchFileModificationTime();
    }
    else
    {   // file not found
        //-wxString msg(wxString::Format(wxT("File not found:\n%s\n\n"),fname.GetData()));
        //-messageBox( msg );
        AskToRemoveFileFromHistory(m_pFilesHistory, id);
    }
}//OnFileReopen
// ----------------------------------------------------------------------------
void CodeSnippetsAppFrame::OnRecentFileClearHistory(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    while (m_pFilesHistory->GetCount())
    {
        m_pFilesHistory->RemoveFileFromHistory(0);
    }

}//OnFileOpenRecentClearHistory
// ----------------------------------------------------------------------------
void CodeSnippetsAppFrame::AskToRemoveFileFromHistory(wxFileHistory* hist, int id)
// ----------------------------------------------------------------------------
{
    if (cbMessageBox(_("Can't open file.\nDo you want to remove it from the recent files list?"),
                    _("Question"),
                    wxYES_NO | wxICON_QUESTION) == wxID_YES)
    {
        hist->RemoveFileFromHistory(id);
    }
}//AskToRemoveFileFromHistory
// ----------------------------------------------------------------------------
bool CodeSnippetsAppFrame::InitializeSDK()
// ----------------------------------------------------------------------------
{
    // we'll do this once and for all at startup
    wxFileSystem::AddHandler(new wxZipFSHandler);
    wxFileSystem::AddHandler(new wxMemoryFSHandler);
    //-wxXmlResource::Get()->InsertHandler(new wxToolBarAddOnXmlHandler);
    wxInitAllImageHandlers();
    wxXmlResource::Get()->InitAllHandlers();
    wxXmlResource::Get()->InsertHandler(new wxScrollingDialogXmlHandler);

    // ---------------------
    // sdk initialization
    // ---------------------
    LoadConfig(); //get data_path, ie, codeblocks resource path

    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("app"));
    // Must execute the GetFolder() statements, else sdk strings will be uninitialized
    wxString userPgmData      = cfg->GetFolder(sdDataUser);
    wxString homeFolder       = cfg->GetHomeFolder();
    //LIE:LIE:LIE: wxString userConfigFolder = cfg->GetConfigFolder();
    wxString pluginsFolder    = cfg->GetPluginsFolder( /*global =*/ true);
    wxString scriptsFolder    = cfg->GetScriptsFolder(/*global =*/ true);
    wxString dataFolderGlobal = cfg->GetDataFolder(/*global =*/ true);
    wxString dataFolderUser   = cfg->GetDataFolder(/*global =*/ false);
    wxString executableFolder = cfg->GetExecutableFolder();
    wxString tempFolder       = cfg->GetTempFolder();

    // "Manager::Get( this )" sets the AppWindow/Frame pointer and
    // must be called *after* the folders are initialized in order for
    // the resources to be found.
    Manager::Get( this );  // this sets the AppWindow/Frame pointer

    // Kill message that "<appdata>/<exeName>/share/codeblocks/lexers
    // can not be enumerated."
    if ( not wxDirExists( dataFolderUser+_T("/lexers")) )
        wxMkdir(dataFolderUser+_T("/lexers"));

    // Initializing EditorManager is screwing up top half of tree events
    //  when wxFlatNotebook.cpp::115 creates its wxPanel
    //  So leave initialization to someone else using the editors on first EditorManager call.
    //- m_pEdMan  = Manager::Get()->GetEditorManager();
    //- m_pProjectMgr = Manager::Get()->GetProjectManager();
    //- m_pMsgMan = Manager::Get()->GetLogManager();

    // Load the resources
    m_bInitXRC_Result = InitXRCStuff();

    return m_bInitXRC_Result;
}
// ----------------------------------------------------------------------------
bool CodeSnippetsAppFrame::LoadConfig()
// ----------------------------------------------------------------------------
{
    if (ParseCmdLine(0L) == -1) // only abort if '--help' was passed in the command line
        return false;

    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("app"));

    wxString data(_T(APP_PREFIX));

    if(platform::windows)
    {
        data.assign(GetAppPath());
    }
    else if(platform::macosx)
    {
        data.assign(GetResourcesDir());                 // CodeBlocks.app/Contents/Resources
        if (!data.Contains(wxString(_T("/Resources")))) // not a bundle, use relative path
            data = GetAppPath() + _T("/..");
    }

    if(data.IsEmpty())
    {
        data.assign(GetAppPath());  // fallback
        data.Replace(_T("/bin"),_T(""));
    }

    if (!m_Prefix.IsEmpty())        // --prefix command line switch overrides builtin value
    {
        data = m_Prefix;
    }
    else                            // also, check for environment
    {

        wxString env;
        wxGetEnv(_T("CODEBLOCKS_DATA_DIR"), &env);
        if (!env.IsEmpty())
            data = env;
    }

    data.append(_T("/share/codeblocks"));

    cfg->Write(_T("data_path"), data);

    //m_HasDebugLog = Manager::Get()->GetConfigManager(_T("message_manager"))->ReadBool(_T("/has_debug_log"), false) || m_HasDebugLog;
    //Manager::Get()->GetConfigManager(_T("message_manager"))->Write(_T("/has_debug_log"), m_HasDebugLog);

    return true;
}
// ----------------------------------------------------------------------------
wxString CodeSnippetsAppFrame::GetAppPath()
// ----------------------------------------------------------------------------
{
    wxString base;
#ifdef __WXMSW__
    wxChar name[MAX_PATH] = {0};
    GetModuleFileName(0L, name, MAX_PATH);
    wxFileName fname(name);
    base = fname.GetPath(wxPATH_GET_VOLUME);
#else
    if (!m_Prefix.IsEmpty())
        return m_Prefix;

#ifdef SELFPATH
    // SELFPATH is a macro from prefix.h (binreloc)
    // it returns the absolute filename of us
    // similar to win32 GetModuleFileName()...
    base = wxString(SELFPATH,wxConvUTF8);
    base = wxFileName(base).GetPath();
#endif
#if defined(sun) || defined(__sun)
    base = wxString(getexecname(),wxConvCurrent);
    base = wxFileName(base).GetPath();
#endif
#if defined(__APPLE__) && defined(__MACH__)
    char path[MAXPATHLEN+1];
    uint32_t path_len = MAXPATHLEN;
    // SPI first appeared in Mac OS X 10.2
    _NSGetExecutablePath(path, &path_len);
    base = wxString(path, wxConvUTF8);
    base = wxFileName(base).GetPath();
#endif
    if (base.IsEmpty())
        //-base = _T("."); <== Causes errors in LoadResources calls
        base = FindAppPath(wxTheApp->argv[0], ::wxGetCwd(), wxEmptyString);;
#endif
    return base;
}
// ----------------------------------------------------------------------------
bool CodeSnippetsAppFrame::InitXRCStuff()
// ----------------------------------------------------------------------------
{
    // This seems to be loaded by Manager::Get(this)-> above
    //-Manager::LoadResource(_T("manager_resources.zip"));
    if (!Manager::LoadResource(_T("resources.zip")))
	{
		ComplainBadInstall();
		return false;
	}
    return true;
}
// ----------------------------------------------------------------------------
void CodeSnippetsAppFrame::ComplainBadInstall()
// ----------------------------------------------------------------------------
{
    wxString msg;
    msg.Printf(_T("Cannot find resources...\n"
        "%s was configured to be installed in '%s'.\n"
        "Please use the command-line switch '--prefix' or "
        "set the CODEBLOCKS_DATA_DIR environment variable "
        "to point where %s is installed,\n"
        "or try re-installing the application..."),
        _T("CodeSnippetsApp"),
        ConfigManager::ReadDataPath().c_str(),
        wxTheApp->GetAppName().c_str());
    cbMessageBox(msg);
}
// ----------------------------------------------------------------------------
// CodeSnippetsAppFrame:: command line parsing
// ----------------------------------------------------------------------------
#if wxUSE_CMDLINE_PARSER

#if wxCHECK_VERSION(2, 9, 0)
#define CMD_ENTRY(X) X
#else
#define CMD_ENTRY(X) _T(X)
#endif

const wxCmdLineEntryDesc cmdLineDesc[] =
{
    { wxCMD_LINE_SWITCH, CMD_ENTRY("h"), CMD_ENTRY("help"), CMD_ENTRY("show this help message"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
    { wxCMD_LINE_OPTION, CMD_ENTRY(""), CMD_ENTRY("prefix"),  CMD_ENTRY("the shared data dir prefix"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_NEEDS_SEPARATOR },
    { wxCMD_LINE_OPTION, CMD_ENTRY("p"), CMD_ENTRY("personality"),  CMD_ENTRY("the personality to use: \"ask\" or <personality-name>"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_NEEDS_SEPARATOR },
    { wxCMD_LINE_OPTION, CMD_ENTRY(""), CMD_ENTRY("profile"),  CMD_ENTRY("synonym to personality"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_NEEDS_SEPARATOR },
    { wxCMD_LINE_OPTION, CMD_ENTRY(""), CMD_ENTRY("KeepAlivePid"),  CMD_ENTRY("launchers pid"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_NEEDS_SEPARATOR },
    { wxCMD_LINE_OPTION, CMD_ENTRY(""), CMD_ENTRY("AppParent"),  CMD_ENTRY("parent app name"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_NEEDS_SEPARATOR },
    { wxCMD_LINE_NONE }
};
#endif // wxUSE_CMDLINE_PARSER
// ----------------------------------------------------------------------------
int CodeSnippetsAppFrame::ParseCmdLine(wxFrame* handlerFrame)
// ----------------------------------------------------------------------------
{
    // code shamelessely taken from the console wxWindows sample :)
    bool filesInCmdLine = false;

#if wxUSE_CMDLINE_PARSER
    wxCmdLineParser& parser = *Manager::GetCmdLineParser();
    parser.SetDesc(cmdLineDesc);
    parser.SetCmdLine(wxTheApp->argc, wxTheApp->argv);
    // wxApp::argc is a wxChar**

    // don't display errors as plugins will have the chance to parse the command-line
    // too, so we don't know here what exactly are the supported options
    switch ( parser.Parse(false) )
    {
        case -1:
            parser.Usage();
            return -1;

        case 0:
            {
                {
                    wxString val;
                    parser.Found(_T("prefix"), &m_Prefix);

                    if (parser.Found(_T("personality"), &val) ||
                        parser.Found(_T("profile"), &val))
                    {
                        //SetupPersonality(val);
                    }

                }
            }
            break;

        default:
            return 1; // syntax error / unknown option
    }
#endif // wxUSE_CMDLINE_PARSER
    return filesInCmdLine ? 1 : 0;
}
// ----------------------------------------------------------------------------
void CodeSnippetsAppFrame::ImportCBResources()
// ----------------------------------------------------------------------------
{

    // Location of app config folder
    wxString appConfigFolder =  Normalize(m_ConfigFolder) ;

    // Get location of app exe folder
    wxString appExeFolder = Normalize(FindAppPath(wxTheApp->argv[0], ::wxGetCwd(), wxEmptyString));

    // Get location of CB dll and exe
    wxString cbExeFolder = Normalize(GetCBExeFolder());

    // location of CodeBlocks config folder
    wxString cbConfigFolder = Normalize(wxStandardPaths::Get().GetUserDataDir());
    wxString appParent = GetConfig()->GetAppParent();
    if ( appParent.empty()) appParent =_T("codeblocks");
    wxString prefixPath;
    #if defined(__WXMSW__)
        if (cbConfigFolder.EndsWith(_T("codesnippets"), &prefixPath))
            cbConfigFolder = prefixPath.Append( appParent );
    #else
        if (cbConfigFolder.EndsWith(_T(".codesnippets"), &prefixPath))
            cbConfigFolder = prefixPath.Append(wxT(".")+appParent);
    #endif

    // If the <appdata>/<pgmName>/ dir has no "default.conf", make a copy
    // of the one found in CB configFolder so that user will not get
    // popups about missing cb global variables.
    wxString fileToCopy = cbConfigFolder+_T("/default.conf");
    if ( wxFileExists(fileToCopy) )
    {
        //.ini must be in .exe folder to receive .conf
        if (appConfigFolder == appExeFolder)
        if (not wxFileExists(appExeFolder + _T("/default.conf")) )
        {
            #if defined(LOGGING)
            bool copied =
            #endif
            wxCopyFile( fileToCopy, appExeFolder+_T("/default.conf") );
            #if defined(LOGGING)
            LOGIT( _T("Copy [%s][%s][%s]"), fileToCopy.c_str(), cbConfigFolder.c_str(), copied?_T("OK"):_T("FAILED"));
            #endif
        }
    }
    // Copy missing resources from CB exe folder to app config folder
    if (not wxDirExists(appExeFolder + _T("/share/codeblocks/")))
    {
        FileImport( cbExeFolder + _T("/share/codeblocks/images/"), appExeFolder + _T("/share/codeblocks/images/"));
        FileImport( cbExeFolder + _T("/share/codeblocks/lexers/"), appExeFolder + _T("/share/codeblocks/lexers/"));
        FileImport( cbExeFolder + _T("/share/codeblocks/manager_resources.zip"), appExeFolder + _T("/share/codeblocks/"));
        FileImport( cbExeFolder + _T("/share/codeblocks/resources.zip"), appExeFolder + _T("/share/codeblocks/"));
        FileImport( cbExeFolder + _T("/share/codeblocks/xpmanifest.zip"), appExeFolder + _T("/share/codeblocks/"));
    }

}//ImportCBResources
// ----------------------------------------------------------------------------
wxString CodeSnippetsAppFrame::GetCBExeFolder() //Get CodeBlocks executable folder
// ----------------------------------------------------------------------------
{
    wxString cbExeFolder = GetAppPath(); //for linux

    // Assume that codeblocks.dll is in the resource path base.
    // Then look for <dllPath>/share/codeblocks/resources.zip
    // to verify.
    #if defined(__WXMSW__)
    if( m_Prefix.IsEmpty() )
    do{
        wxChar dllPath[1024] = {0};
        HMODULE dllHandle = LoadLibrary(_T("codeblocks.dll"));
        if (not dllHandle) break;
        DWORD pathLen = GetModuleFileName( dllHandle, dllPath, sizeof(dllPath));
        if (not pathLen) break;
        cbExeFolder.assign(::wxPathOnly(dllPath));
        #if defined(LOGGING)
          LOGIT( _T("CodeBlocks.dll Path[%s]"), cbExeFolder.c_str());
        #endif
        //cbExeFolder.append(_T("/share/codeblocks"));
        //if (not ::wxFileExists(cbExeFolder + _T("/resources.zip")) ) break;
    }while(false);
    #endif
    return cbExeFolder;
}
// ----------------------------------------------------------------------------
bool CodeSnippetsAppFrame::FileImport(const wxString& source, const wxString& dest)
// ----------------------------------------------------------------------------
{
    //::wxBusyCursor ();

    wxString msg = wxEmptyString;

    // If the src file is just a file, then use ::wxCopyFile
    //  otherwise is wxDirTraverser to copy the whole directory
    #if defined(LOGGING)
    //LOGIT(_("Copying file[%s] into working directory[%s]\n"),
    //            source.c_str(), dest.c_str());
    #endif
    if (::wxDirExists (source))
    {
        #if defined(LOGGING)
        //LOGIT(_("Adding directory[%s] to repository[%s]\n"),
        //            source.c_str(), dest.c_str()) ;
        #endif
        wxDir dir (source);
        if (dir.IsOpened ())
        {
            FileImportTraverser dirTraverser(source, dest);
            dir.Traverse (dirTraverser);
        }
    }
    else //file
    {
        wxString srcNative(source);
        #if defined(LOGGING)
        //LOGIT(_("Adding file[%s] to repository[%s]\n"),
        //            srcNative.c_str(), dest.c_str());
        #endif
        wxString destfile = dest;
        // convert destdir to destdir + filename
        if (::wxDirExists(destfile))
            destfile += wxFileName(srcNative).GetFullName();
        ::wxCopyFile (srcNative, destfile);
    }

    return true;
}
// ----------------------------------------------------------------------------
wxString CodeSnippetsAppFrame::Normalize(const wxString& filenamein )
// ----------------------------------------------------------------------------
{
    wxFileName filename(filenamein);
    filename.Normalize();
    return filename.GetFullPath();
}
